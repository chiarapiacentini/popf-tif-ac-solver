/*
 *   <one line to give the program's name and a brief idea of what it does.>
 *   Copyright (C) 2014  <copyright holder> <email>
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "NetworkSolver.h"
#include "ExternalSolver.h"
#include "Network.h"
#include "NewtonRaphson.h"
#include <map>
#include <string>
#include "armadillo"

using namespace std;
using namespace arma;



extern "C" ExternalSolver* create_object(){
    return new NetworkSolver();
}

extern "C" void destroy_object(ExternalSolver* externalSolver){
    delete externalSolver;
}

NetworkSolver::NetworkSolver(){
    
}


NetworkSolver::~NetworkSolver(){
}

void NetworkSolver::loadSolver(string* parameters, int n){

    string nameNetwork = parameters[0];
    string capacitor = "";
    if(n>1){
        capacitor = parameters[1];
    }
    Network net(nameNetwork);
    net.addCapacitors(capacitor);
    theNetwork = net;
    theSolver = new NewtonRaphson(&theNetwork);
    theNetwork.setSolver(theSolver);
    char const *x[]={"ipsa-slack-p","ipsa-voltage", "ipsa-total-voltage"};
    char const *y[]={"p-level","q-level","p-shedding","q-shedding","tap-level","gen-p-level","gen-q-level","thermal-limit","q-minimum-gen","p-minimum-gen","p-maximum-gen","q-maximum-gen", "capacitor-level", "switch-level"};
    affected = list<string>(x,x+3);
    dependencies = list<string>(y,y+13);
    }
    
    map<string,double> NetworkSolver::callExternalSolver(map<string,double> initialState, bool isHeuristic){
        map<string,double> toReturn;
        map<string,pair<double, double> > loadParameters;
        map<string,pair<double, double> > generatorParameters;
        map<string,pair<double, double> > shedParameters;
        map<string,pair<double,double> > tapParameters;
        map<string, int> switchParameters;
        map<string,bool> capacitorParameters;
        map<string, string> voltageResults;
        map<string, string> pSlackResults;
        map<string, string> qSlackResults;
        map<string, string> lineResults;
        
        
        map<string, double>::iterator iSIt = initialState.begin();
        map<string, double>::iterator isEnd = initialState.end();
        for(;iSIt!=isEnd;++iSIt){
            string parameter = iSIt->first;
            string function = iSIt->first;
            double value = iSIt->second;
            function.erase(0,1);
            function.erase(function.length()-1,function.length());
            int n=function.find(" ");
            if(n!=-1){
                string arg=function;
                function.erase(n,function.length()-1);
                arg.erase(0,n+1);
                if(function=="p-level"){
                    loadParameters[arg].first=value;
                }else if(function=="q-level"){
                    loadParameters[arg].second=value;
                }else if(function=="gen-p-level"){
                    generatorParameters[arg].first=value;
                }else if(function=="gen-q-level"){
                    generatorParameters[arg].second=value;
                }else if(function=="p-shedding"){
                    shedParameters[arg].first=value;
                }else if(function=="q-shedding"){
                    shedParameters[arg].second=value;
                }else if(function=="tap-level"){
                    tapParameters[arg].first=value;
                    tapParameters[arg].second=0;
                }else if(function=="capacitor-level"){
                    capacitorParameters[arg] = value==0?false:true;
                }else if(function=="ipsa-voltage"){
                    voltageResults[arg] = parameter;
                }else if(function=="ipsa-slack-p"){
                    pSlackResults[arg] = parameter;
                }else if(function=="ipsa-slack-q"){
                    qSlackResults[arg] = parameter;
                }else if(function=="ipsa-line-power"){
                    lineResults[arg] = parameter;
                }else if (function=="switch-level"){
                    switchParameters[arg] = value;
                }
            }
        }  
        map<string, pair<double, double> >::iterator it=shedParameters.begin();
        const map<string, pair<double, double> >::iterator itEnd=shedParameters.end();
        for(;it!=itEnd;++it){
            loadParameters[it->first].first-=it->second.first;
            loadParameters[it->first].second-=it->second.second;
        }
        
        
        theNetwork.changeLoadParameters(loadParameters);
        theNetwork.changeGeneratorParameters(generatorParameters);
        theNetwork.changeTransformerParameters(tapParameters);
        theNetwork.changeCapacitorParameters(capacitorParameters);
        theNetwork.changeSwitchParameters(switchParameters);
        theNetwork.solvePowerFlow(false);
        
        map<string, string>::iterator vIt = voltageResults.begin();
        map<string, string>::iterator vEnd = voltageResults.end();
        for(;vIt!=vEnd;++vIt){
            string arg = vIt->first;
            string parameter = vIt->second;
            toReturn[parameter]=theNetwork.takeBusbarValue(arg).first;
        }
        map<string, string>::iterator pIt = pSlackResults.begin();
        map<string, string>::iterator pEnd = pSlackResults.end();
        for(;pIt!=pEnd;++pIt){
            string arg = pIt->first;
            string parameter = pIt->second;
            pair<double, double> results = theNetwork.takeGeneratorValue(arg);
            string qParameter = qSlackResults[arg];
            toReturn[parameter] = results.first;
            toReturn[qParameter] = results.second;
        }
        map<string, string>::iterator lineIt = lineResults.begin();
        map<string, string>::iterator lineEnd = lineResults.end();
        for(;lineIt!=lineEnd;++lineIt){
            string arg = lineIt->first;
            string parameter = lineIt->second;
            toReturn[parameter]= theNetwork.takeBranchValue(arg).first;
        }
        return toReturn;
    }
    
    list<string> NetworkSolver::getParameters(){    
        return affected;
    }
    
    list<string> NetworkSolver::getDependencies(){
        return dependencies; 
    }
