#include <iostream>
#include <cmath>
#include <cassert>
#include <iomanip>
#include <time.h>
#include "Solver.h";
#include "Network.h";
#include "Busbar.h";
#include "Generator.h"
#include "Branch.h"

using namespace std;


Solver::Solver(){

}

Solver::Solver(int i){
  Solver();
}
Solver::Solver(Network* net){
  theNetwork = net;
  //  initialValue();
  isSolved = false;
  isOptimal = false;
  busbars = theNetwork->getBusbars();
  nNode = busbars.size();
  voltage = new double[nNode];
  delta = new double[nNode];
  activePowerGen = new double[nNode];
  activePowerLoad = new double[nNode];
  reactivePowerGen = new double[nNode];
  reactivePowerLoad = new double[nNode];
}

void Solver::initialValue(){

    //busbars = theNetwork->getBusbars();
    capacitors = theNetwork->getCapacitors();
  generators = theNetwork->getGenerators();
  branches = theNetwork->getBranches();
  baseMVA = theNetwork->getBase();
  reY = theNetwork->getReY();
  imY = theNetwork->getImY();
  bY = theNetwork->getBY();
  map<int, Busbar*>::iterator busIt = busbars.begin();
  map<int, Busbar*>::iterator busEnd = busbars.end();
  //nNode = busbars.size();
  //voltage = new double[nNode];
  //delta = new double[nNode];
  //activePowerGen = new double[nNode];
  //activePowerLoad = new double[nNode];
  //reactivePowerGen = new double[nNode];
  //reactivePowerLoad = new double[nNode];
  int n = 0;
  map<int,int> indexBus;
  for(;busIt!=busEnd;++busIt,++n){
    Busbar *bus = busIt->second;
    int type = bus->getType();
    int indexV = n;
    int index = bus->getBus_i();
    indexBus[index]=indexV;
    //cout << "index " << index << " " << type << " " << bus->getVm()<< " -- " << endl;
    if(type!=1){
      voltage[indexV]=bus->getVm();
    }else{
      voltage[indexV]=1.;
    }
    if(type!=3){
      delta[indexV]=0;
    }else{
      delta[indexV]=bus->getVa()/180.*acos(-1);
      toShed.insert(n);
      toShed.insert(n+nNode);
    }
    if(type==2){
      toShed.insert(n+nNode);
    }
    activePowerGen[index]=0.;
    reactivePowerGen[index]=0.;
    activePowerLoad[indexV]=-bus->getPdLoad()/baseMVA;
    reactivePowerLoad[indexV]=-bus->getQdLoad()/baseMVA;
  }

  map<int, Capacitor*>::iterator cIt = capacitors.begin();
  map<int, Capacitor*>::iterator cEnd = capacitors.end();
  for(;cIt!=cEnd;++cIt){
      Capacitor* capacitor = cIt->second;
      if(capacitor->getIsActive()){
	  Busbar* busbar = busbars.find(capacitor->getBus())->second;
	  int index = busbar->getBus_i();
	  int indexV = indexBus[index];
	  activePowerLoad[indexV]-=(capacitor->getPdLoad()/baseMVA);
	  reactivePowerLoad[indexV]-=(capacitor->getQdLoad()/baseMVA);
      }
  }

  map<int, Generator*>::iterator genIt = generators.begin();
  map<int, Generator*>::iterator genEnd = generators.end();
  for(;genIt!=genEnd;++genIt){
    Generator *gen = genIt->second;
    int index = gen->getBus();
    activePowerGen[index]=gen->getPg()/baseMVA;
    reactivePowerGen[index]=gen->getQg()/baseMVA;
  }
}

bool Solver::solve(){
  initialValue();
  isSolved = solvePF();
  calculateLinePF();
  calculateObjectiveFunction();
  passResultToNetwork();
//   delete voltage;
//   delete delta;
//   delete activePowerGen;
//   delete activePowerLoad;
//   delete reactivePowerGen;
//   delete reactivePowerLoad;
//   return isSolved;
}

void Solver::calculateObjectiveFunction(){
  theNetwork->setObjectiveValue(0);
}

void Solver::calculateLinePF(){

  map<int, Branch*>::iterator brIt = branches.begin();
  map<int, Branch*>::iterator brEnd = branches.end();
  for(;brIt!=brEnd;++brIt){
    Branch *branch = brIt->second;
    int indexI = branch->getFBus();
    int indexJ = branch->getTBus();
    int newIndex = indexI*nNode+indexJ;
    double tap;
    if(branch->getRatio()==0){
      tap = 1.;
    }else{
      tap = branch->getRatio();
    }
    
    double activePowerIn = voltage[indexI]*voltage[indexI]*(-branch->getReY()/tap)-voltage[indexI]*voltage[indexJ]*(-branch->getReY())*cos(delta[indexI]-delta[indexJ])-voltage[indexI]*voltage[indexJ]*(-branch->getImY())*sin(delta[indexI]-delta[indexJ]);
    double reactivePowerIn = -voltage[indexI]*voltage[indexI]*(-branch->getImY()+branch->getBY())/tap+voltage[indexI]*voltage[indexJ]*(-branch->getImY())*cos(delta[indexI]-delta[indexJ])-voltage[indexI]*voltage[indexJ]*(-branch->getReY()*sin(delta[indexI]-delta[indexJ]));
    double activePowerOut = voltage[indexJ] * voltage[indexJ] *(-branch->getReY())*tap-voltage[indexI] * voltage[indexJ] * -branch->getReY()*cos(delta[indexJ]-delta[indexI])- voltage[indexI] * voltage[indexJ] * -branch->getImY()*sin(delta[indexJ]-delta[indexI]);
    double reactivePowerOut = -voltage[indexJ]*voltage[indexJ]*(-branch->getImY()+branch->getBY())*tap+voltage[indexI]*voltage[indexJ]*(-branch->getImY())*cos(delta[indexJ]-delta[indexI])-voltage[indexJ]*voltage[indexI]*(-branch->getReY()*sin(delta[indexJ]-delta[indexI]));

    branch->setActivePowerIn(activePowerIn*baseMVA);
    branch->setReactivePowerIn(reactivePowerIn*baseMVA);
    branch->setActivePowerOut(activePowerOut*baseMVA);
    branch->setReactivePowerOut(reactivePowerOut*baseMVA);
  }
}

bool Solver::solvePF(){
  cout <<"Inside Solver" << endl;

}

void Solver::printSolution(){

  if(isSolved){
    cout << "Solution found: " << endl;
  }else{
    cout << "Solution not found! " << endl;
  }
  cout << "=======================================================================" << endl;
  cout << "| System Summary                                                      |" << endl;
  cout << "=======================================================================" << endl;

  cout << "=======================================================================" << endl;
  cout << "| Bus Data                                                            |" << endl;
  cout << "=======================================================================" << endl;
  cout << "Bus\tVoltage\t\t\tGeneration\t\tLoad" << endl;
  cout << " # \tMag(pu)\t\Ang(deg)\tP(MW)\tQ(MVAr)\t\tP(MW)\tQ(MVAr)"<<endl;
  cout << "-----\t-------\t-------\t\t-------\t-------\t\t-------\t-------" << endl;

  map<int, Generator*>::iterator genIt = generators.begin();
  map<int, Generator*>::iterator genEnd = generators.end();
  map<int, pair<double, double> > power;
  for(;genIt!=genEnd;++genIt){
    Generator* generator = genIt->second;
    int id = generator->getBus();
    power[id]=make_pair(generator->getPg(), generator->getQg());
  }
  map<int, Busbar*>::iterator busIt = busbars.begin();
  map<int, Busbar*>::iterator busEnd = busbars.end();
  cout << fixed << setprecision(3);
  for(;busIt!=busEnd;++busIt){
    int idBus = busIt->first;
    Busbar *busbar = busIt->second;
    cout << theNetwork->getBusbarsIDInv()[busbar->getBus_i()] << "\t"<<busbar->getVm()<<"\t"<<busbar->getVa()<<"\t\t";
    map<int, pair<double, double> >::iterator poIt;
    poIt = power.find(idBus);
    if(poIt==power.end()){
      cout << "-\t-\t\t";
    }else{
      cout << poIt->second.first << "\t" << poIt->second.second<<"\t\t";
    }
    if(busbar->getIsLoad()){
      cout << busbar->getPdLoad()<<"\t" << busbar->getQdLoad()<<endl;
    }else{
      cout << "-\t-"<<endl;
    }
  }
  
  cout << "=======================================================================" << endl;
  cout << "| Branch Data                                                         |" << endl;
  cout << "=======================================================================" << endl;
  cout << "Brnch\tFrom\tTo\tFrom Bus Injection\tTo Bus Injection"<<endl;
  cout << "#\tBus\tBus\tP(MW)\tQ(MVAr)\t\tP(MW)\tQ(MVAr)"<<endl;
  cout << "-----\t-----\t-----\t-------\t-------\t\t-------\t-------"<<endl;
  map<int, Branch*>::iterator brIt = branches.begin();
  map<int, Branch*>::iterator brEnd = branches.end();
  for(;brIt!=brEnd;++brIt){
    int idBranch = brIt->first;
    Branch* branch = brIt->second;
    cout << fixed << setprecision(2)<< branch->getBranch_i() << "\t" << theNetwork->getBusbarsIDInv()[branch->getFBus()] << "\t"<<theNetwork->getBusbarsIDInv()[branch->getTBus()] <<"\t"<< branch->getActivePowerIn()<< "\t"<< branch->getReactivePowerIn()<<"\t\t"<< branch->getActivePowerOut() << "\t"<< branch->getReactivePowerOut()<<endl;
  }
  cout << "=======================================================================" << endl;
  cout << "| Tap Data                                                         |" << endl;
  cout << "=======================================================================" << endl;
  cout << "Name\tTapLevel"<<endl;
  brIt = branches.begin();

  for(;brIt!=brEnd;++brIt){
    int idBranch = brIt->first;
    Branch* branch = brIt->second;
    if(branch->getIsTransformer()){
      cout <<  setprecision(2) << branch->getBranch_i() << "\t"<< (branch->getRatio()-1)*100 << endl;
    }
  }


  double costFunction = 0;
  map<int, GeneratorCostData*> gcd=theNetwork->getGCD();
  for(map<int,GeneratorCostData*>::iterator genIt=gcd.begin();genIt!=gcd.end();++genIt){
    int idGen = genIt->first;
    GeneratorCostData* genC =  genIt->second;
    Generator *gen = generators[idGen];
    double power = gen->getPg();
    costFunction+=genC->getX1()*power*power+genC->getX2()*power+genC->getX3(); 
  }
  cout << "=======================================================================" << endl;
  cout << "Objective Value : " << costFunction << endl;
  cout << "=======================================================================" << endl;

}

void Solver::passResultToNetwork(){
  if (!isSolved){
    //    return;
  }
  map<int, Busbar*>::iterator busIt = busbars.begin();
  map<int, Busbar*>::iterator busEnd = busbars.end();
  for(;busIt!=busEnd;++busIt){
    int idBus = busIt->first;
    Busbar *busbar = busIt->second;
    busbar->setVm(voltage[idBus]);
    busbar->setVa(delta[idBus]*180/acos(-1));
  }
  calculatePowerGenerators();
}

void Solver::calculatePowerGenerators(){
    map<int, Generator*>::iterator genIt = generators.begin();
    map<int, Generator*>::iterator genEnd = generators.end();
    for(;genIt!=genEnd;++genIt){
        int idGen = genIt->first;
        Generator *generator = genIt->second;
        int idBus = generator->getBus();
        map<int, Busbar*>::iterator busIt = busbars.find(idBus);
        if(busIt  == busbars.end()){
            assert("Busbar not found");
        }
        Busbar *busbar = busIt->second;
        int type = busbar->getType();
        if(type==2 || type == 3){
            double reactivePower = busbar->getQdLoad()/baseMVA;
            double activePower = busbar->getPdLoad()/baseMVA;
            map<int, Busbar*>::iterator busSIt = busbars.begin();
            map<int, Busbar*>::iterator busSEnd = busbars.end();
            for(;busSIt!=busSEnd;++busSIt){
                int idBusS = busSIt->first;
                pair<int,int> index(idBus, idBusS);
                activePower += voltage[idBus]*voltage[idBusS]*(reY.at(idBus,idBusS)*cos(delta[idBus]-delta[idBusS])+imY.at(idBus,idBusS)*sin(delta[idBus]-delta[idBusS]));
                reactivePower += voltage[idBus]*voltage[idBusS]*(reY.at(idBus,idBusS)*sin(delta[idBus]-delta[idBusS])-imY.at(idBus,idBusS)*cos(delta[idBus]-delta[idBusS]));
                
            }
            reactivePower=reactivePower*baseMVA;
            activePower=activePower*baseMVA;
            generator->setQg(reactivePower);
            if(type==3){
                generator->setPg(activePower);
            }
        }
    }
}
