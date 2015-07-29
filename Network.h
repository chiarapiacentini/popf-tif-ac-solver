#ifndef __Network_h__
#define __Network_h__

#include <iostream>
#include <map>
#include <complex>
#include <cassert>
#include <list>
#include <vector>
#include "Busbar.h"
#include "Generator.h"
#include "Branch.h"
#include "GeneratorCostData.h"
#include "Capacitor.h"
#include "Solver.h"
#include "armadillo"

using namespace std;
using namespace arma;

class Solver;

class Network{
 public:
  Network(string inputFile);
  Network();
  Network(int i){
    Network();
  };
  ~Network(){
    for(map<int,Generator*>::iterator genIt = generators.begin(); genIt!=generators.end();++genIt){
	Generator *gen = genIt->second;
	delete gen;
    }
    for(map<int, Busbar*>::iterator busIt = busbars.begin();busIt!=busbars.end();++busIt){
       Busbar *bus = busIt->second;
       delete bus;
    }
    for(map<int, Branch*>::iterator brIt = branches.begin();brIt!=branches.end();++brIt){
     Branch *branch = brIt->second; 
     delete branch;
    }
    for(map<int, GeneratorCostData*>::iterator gcdIt = generatorsCostData.begin();gcdIt!=generatorsCostData.end();++gcdIt){
      GeneratorCostData * gcd = gcdIt->second;
      delete gcd;
    }
  }
  void printNetwork(ostream & o) const;
  void printNetworkAsMatlabFile(ostream & o) const;
  void changeOneLoad(string name, double newPd, double newQd) const;
  void changeOneLoad(int idLoad, double newPd, double newQd) const;
  void changeLoads(double scaling) const;
  void changeLoadsFromFile(string inFile, string outFile, double constant = 0.5, double initial = 50000.) const;
  map<int, Busbar*> getBusbars() const{
    return busbars;
  };
  map<int, Generator*> getGenerators() const{
    return generators;
  };
  map<string, int> getNameLoads() const{
    return nameLoad;
  }
  map<int, Branch*> getBranches() const{
    return branches;
  }
  map<int, GeneratorCostData*> getGCD() const{
    return generatorsCostData;
  }
  map<string, int> getNameTransformers() const{
    return nameTransformer;
  }
  map<string, int> getNameCapacitors() const{
   return nameCapacitor; 
  }
  map<int, Capacitor*> getCapacitors() const{
   return capacitors; 
  }
  int getTypeGenerator(int i) const{
    int idBus = generators.find(i)->second->getBus();
    return busbars.find(idBus)->second->getType();
  } 
  mat getReY(){
    return reY;
  }
  mat getImY(){
    return imY;
  }
  mat getBY(){
    return bY;
  }
  double getBase(){
    return baseMVA;
  }
  int getNBus(){
    return nBus;
  }
  int getNGen(){
    return nGen;
  }
  int getNBranch(){
    return nBranch;
  }
  void solvePowerFlow(bool printResult = false);
  void solvePowerFlow(int i, bool printResult = false);
  bool isOptimal();
  void copyNetwork(const Network & net);
  Network & operator = (const Network & net);
  
  map<pair<int, int>, double> getMapTap(){
    return mapTap;
  }
  void disconnectLine(int idLine, bool isConnected = false);
  void recalculateSlack(int nComponentsBefore);
  void changeLoadParameters(map<string, pair<double, double> >newValues);
  void changeGeneratorParameters(map<string, pair<double, double> >newValues);
  void changeTransformerParameters(map<string, pair<double, double> >newValues);
  void changeCapacitorParameters(map<string, bool> newValues);
  void changeGeneratorsParameters(double scalingFactor);
  void changeSwitchParameters(map<string,int> newValues);
  pair<double, double> takeBusbarValue(string name){
    if(nameBusbar.find(name)==nameBusbar.end()){
      string error = "busbar  ";
      error+=name;
      error+=" not found";
       
      assert(error.c_str());
    }
    int idBus = nameBusbar[name];
    Busbar *busbar = busbars[idBus];
    pair<double, double> toReturn(busbar->getVm(), busbar->getVa());
    return toReturn;
  }
  pair<double, double> takeGeneratorValue(string name){
    if(nameGenerator.find(name)==nameGenerator.end()){
      string error = "generator ";
      error+=name;
      error+=" not found";
      assert(error.c_str());
    }
    int idGen = nameGenerator[name];
    Generator *generator = generators[idGen];
    pair<double, double> toReturn(generator->getPg(), generator->getQg());
    return toReturn;
  }
  pair<double, double> takeBranchValue(string name){
    if(nameBranch.find(name)==nameBranch.end()){
      string error = "branch ";
      error += name;
      error += " not found";
      assert(error.c_str());
    }
    int idBranch = nameBranch[name];
    Branch *branch = branches[idBranch];
    pair<double, double> toReturn(branch->getActivePowerIn(), branch->getReactivePowerIn());
    return toReturn;
  }

  void setSolver(Solver *sol){
    solver = sol;
  }
  map<string, int> getTaps(){
    return nameTransformer;
  }
  void setObjectiveValue(double n){
    objectiveValue=n;
  }
  double getObjectiveValue(){
    return objectiveValue;
  }
  
  map<int, int> getBusbarsID(){
   return busbarsID; 
  }
  
  map<int, int> getBusbarsIDInv(){
   return busbarsIDInv; 
  }

  void addCapacitors(string nameFile);
  void activateCapacitor(int idCapacitor);
  void deactivateCapacitor(int idCapacitor);
  void setTapTransformers(double voltageTarget);
  void setTapTransformer(string name, double voltageTarget);
private:
  string inFile;
  int nBus;
  int nGen;
  int nBranch;
  int nGenCost;
  int nLoad;
  int nTransformer;
  int nCapacitor;
  map<int, Busbar*> busbars;
  map<int, Generator*> generators;
  map<int, Branch*> branches;
  map<int, GeneratorCostData*> generatorsCostData;
  map<int, int> loads;
  map<int, Capacitor*> capacitors;
  map<int, int> transformers;
  map<int, int> busbarsID;
  map<int, int> busbarsIDInv;
  map<string, int> nameBusbar;
  map<string, int> nameGenerator;
  map<string, int> nameBranch;
  map<string, int> nameLoad;
  map<string, int> nameTransformer;
  map<string, int> nameCapacitor;
  map<pair<int, int>, double> mapTap;
  map<int, int> mapBusToBranch;
  mat reY;
  mat imY;
  mat bY; 
  Solver* solver;
  string mpc;
  string version;
  double baseMVA;
  double objectiveValue;
  void parsingMatlabFile(string line);
  string getAfterParameter(string line, string test);
  void initialisationNetwork();
  void populateLoad();
  void populateTransformer();
  void buildMapNames();
  void calculateYMatrix(); // in cartesian coordinate

};

ostream & operator<< (ostream &o, const Network & a);
ostream & operator<< (ostream &o, const Busbar & a);
ostream & operator<< (ostream &o, const Generator & a);
ostream & operator<< (ostream &o, const Branch & a);
ostream & operator<< (ostream &o, const GeneratorCostData & a);
ostream & operator<< (ostream &o, const Capacitor & a);
vector<string> findParameters(string line, int &n);
string GetStdoutFromCommand(string cmd);
#endif
