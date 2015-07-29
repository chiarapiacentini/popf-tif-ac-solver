#include <iostream>
#include <complex>
#include <cassert>
#include <time.h>
#include <iomanip>
#include "Network.h"
#include "Solver.h"
#include "NewtonRaphson.h"
#include "armadillo" 
#include <iomanip>
#include <cmath>

using namespace std;
using namespace arma;
      typedef unsigned long long timestamp_t;

    static timestamp_t
    get_timestamp ()
    {
      struct timeval now;
      gettimeofday (&now, NULL);
      return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
    } 
NewtonRaphson::NewtonRaphson(Network *net, int nIt, double p):Solver(net){
  nIteration = nIt;
  precision = p;
}

bool NewtonRaphson::solvePF(){
  return solution();
}

void NewtonRaphson::calculateJacobianAndDeltaX(){
    timestamp_t begin, step1, step2, step3, step4, end;
    double time_spent;
    //begin = clock();
    JJ.reset();
    PP.reset();
    JJ.reshape(2*nNode,2*nNode);
    PP.reshape(2*nNode,1);
    
    for(int i=0;i<nNode;++i){
        begin = get_timestamp();
        double voltageI = voltage[i];
        double deltaI = delta[i];
        
        for(int j=0;j<nNode;j++){
            double voltageJ = voltage[j];
            double deltaJ = delta[j];
            calculateJacobianElement(i,j,voltageI, voltageJ, deltaI, deltaJ);
        }
        //Px[i]=activePowerGen[i]+activePowerLoad[i]-Px[i]*voltage[i];
        //Qx[i]=reactivePowerGen[i]+reactivePowerLoad[i]-Qx[i]*voltage[i];
        PP.at(i)=activePowerGen[i]+activePowerLoad[i]-PP.at(i)*voltage[i]; 
        PP.at(i+nNode)=reactivePowerGen[i]+reactivePowerLoad[i]-PP.at(i+nNode)*voltage[i]; 
        end = get_timestamp();
    }
}

void NewtonRaphson::calculateJacobianElement(int i, int j, double voltageI, double voltageJ, double deltaI, double deltaJ){
 

    pair<int,int> index(i,j);
    complex<double> Y(reY.at(i,j),imY.at(i,j));
    double absY = sqrt(reY.at(i,j)*reY.at(i,j) + imY.at(i,j)*imY.at(i,j));
    long double theta = atan2(imY.at(i,j),reY.at(i,j));
    //  cout << PP << endl;
    PP.at(i)+=absY*voltageJ*cos(deltaI-deltaJ-theta); 
    PP.at(i+nNode)+=absY*voltageJ*sin(deltaI-deltaJ-theta);
    //Px[i]+=absY*voltageJ*cos(deltaI-deltaJ-theta);
    //Qx[i]+=absY*voltageJ*sin(deltaI-deltaJ-theta);
    
    if(i!=j){
        JJ.at(i,j)=voltageI*absY*voltageJ*sin(deltaI-deltaJ-theta);
        JJ.at(i,j+nNode)=voltageI*absY*cos(deltaI-deltaJ-theta);
        JJ.at(i+nNode,j)=-voltageI*absY*voltageJ*cos(deltaI-deltaJ-theta);
        JJ.at(i+nNode,j+nNode)=voltageI*absY*sin(deltaI-deltaJ-theta);
    }else{
        JJ.at(i,j)=0;
        JJ.at(i,j+nNode)=voltageI*absY*cos(theta);
        JJ.at(i+nNode,j)=0;
        JJ.at(i+nNode,j+nNode)=-voltageI*absY*sin(theta);
        for(int k=0;k<nNode;k++){
            
            double voltageK = voltage[k];
            double deltaK = delta[k];
            pair<int,int> newIndex(i,k);
            complex<double> newY(reY.at(i,k), imY.at(i,k));
            double newAbsY = abs(newY);
            double newTheta = arg(newY);
            if(i!=k){
                
                JJ.at(i,j)-=newAbsY*voltageK*sin(deltaI-deltaK-newTheta);
                JJ.at(i+nNode,j)+=newAbsY*voltageK*cos(deltaI-deltaK-newTheta);
            }
            JJ.at(i,j+nNode)+=newAbsY*voltageK*cos(deltaI-deltaK-newTheta);
            JJ.at(i+nNode,j+nNode)+=newAbsY*voltageK*sin(deltaI-deltaK-newTheta);
        }
        JJ.at(i,j)=JJ.at(i,j)*voltageI;
        JJ.at(i+nNode,j)=JJ.at(i+nNode,j)*voltageI;
        
    }

}
bool NewtonRaphson::newtonRaphson(){

    
    clock_t begin, step1, step2, step3, step4, end;
    double time_spent;
    begin = clock();
    calculateJacobianAndDeltaX();
    step1 = clock();
    //    cout << JJ << endl;
    //  JJ = mat(2*nNode,2*nNode);
    //  PP = mat(2*nNode,1);
    /*int i=0;
     * 
     f or(int k=0;k<n*Node;k++,i++){//;vIt!=vEnd;++vIt,++i){
         int iIndex = k;
         //    unordered_map<int, double>::iterator wIt=voltage.begin();
         //    unordered_map<int, double>::iterator wEnd=voltage.end();
         int j=0;
         for(int jIndex=0;jIndex<nNode;jIndex++,j++){//++wIt,++j){
             cout << i << " " << iIndex << " " << j << " " << jIndex << endl;
             pair<int, int> index (iIndex,jIndex);
             JJ.at(i,j)= J1[index];
             JJ.at(i,nNode+j)= J2[index] ;
             JJ.at(i+nNode,j)= J3[index] ;
             JJ.at(i+nNode,nNode+j)= J4[index] ;
         }
         //  PP.at(i) = Px[iIndex];
         //PP.at(i+nNode) = Qx[iIndex];
         
     }
     */
    set<int>::reverse_iterator shIt = toShed.rbegin();
    set<int>::reverse_iterator shEnd = toShed.rend();
    for(;shIt!=shEnd;++shIt){
        int index = *shIt;
        JJ.shed_rows(index, index);
        JJ.shed_cols(index, index);
        PP.shed_rows(index, index);
    }
    step2 = clock();
    mat result = inv(JJ)*PP;
    //cout << fixed << setprecision(4) << PP << endl;
    int count = 0;
    mat copyRes(2*nNode,1);
    //  vIt=voltage.begin();
    int n=0;
    for(int i=0;i<nNode;i++,++n){//;vIt!=vEnd;++vIt,++n){
        if(toShed.find(n)!=toShed.end()){
            copyRes.at(n)=0;
            count++;
        }else{
            copyRes.at(n)=result(n-count);
        }
    }
    //  vIt=voltage.begin();
    for(int i=0;i<nNode;i++,n++){//;vIt!=vEnd;++vIt,++n){
        if(toShed.find(n)!=toShed.end()){
            copyRes.at(n)=0;
            count++;
        }else{
            copyRes.at(n)=result(n-count);
        }
    }
    step3 = clock();
    bool keepGoing = false;
    //  vIt=voltage.begin();
    n=0;
    for(int i=0;i<nNode;++i,++n){//;vIt!=vEnd;++vIt,++n){
        //    int i = vIt->first;
        voltage[i]+=copyRes.at(n+nNode);
        delta[i]+=copyRes.at(n);
        if(abs(copyRes.at(i))>precision){
            keepGoing = true;
        }
    }
    step4 = clock();
    //  cout << "\t\tJacobian : " << (double)(step1 - begin) / CLOCKS_PER_SEC <<  endl;
    //  cout << "\t\tMat : " << (double)(step2 - step1) / CLOCKS_PER_SEC <<  endl;
    //  cout << "\t\tsolve : " << (double)(step3 - step2) / CLOCKS_PER_SEC <<  endl;
    //  cout << "\t\tObjective Function : " << (double)(step4 - step3) / CLOCKS_PER_SEC <<  endl;
    return keepGoing;
}


bool NewtonRaphson::solution(){
  
  clock_t begin, step1, step2, step3, step4, end;
  double time_spent;
  begin = clock();
  bool toReturn = false;
  for(int i=0;i<nIteration;i++){
    //cout << "iteration " << i << endl;
    step1 = clock();
    toReturn = newtonRaphson();
    step2 = clock();
    //cout <<fixed << setprecision(3)<< "\tIteration " << i << " : "  << (double)(step2 - step1) / CLOCKS_PER_SEC  << endl;

    if (!toReturn){
      end = clock();
      //cout << "\tIteration " << i << " : "  << (double)(end - begin) / CLOCKS_PER_SEC  << endl;
      return !toReturn;
    }
  }
  return !toReturn;
}