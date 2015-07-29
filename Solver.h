#ifndef __Solver_h__
#define __Solver_h__

#include <set>
#include "Network.h"
#include "Busbar.h"
#include "Generator.h"
#include "Branch.h"
#include "armadillo"

using namespace std;
using namespace arma;
class Network;



class Solver{
 public:
  Solver();
  Solver(int i);
  Solver(Network* net);
  virtual ~Solver(){
//     cout << "close solver" << endl;
    delete voltage;
    delete delta;
    delete activePowerGen;
    delete activePowerLoad;
    delete reactivePowerGen;
    delete reactivePowerLoad;
  };
  bool solve();
  void printSolution();
  void updateNetwork(){
    initialValue();
  }
  bool getIsOptimal(){
    return isOptimal;
  }
  void setNetwork(Network *net){
    theNetwork = net;
  }
  
 protected:
  Network* theNetwork;
  int nNode;
  bool isSolved;
  bool isOptimal;
  double* voltage;
  double* delta;
  map<int, Busbar*> busbars;
  map<int, Generator*> generators;
  map<int, Branch*> branches;
  map<int, Capacitor*> capacitors;
  mat reY;
  mat imY;
  mat bY;
  //map<int, double> activePowerGen;
  //map<int, double> activePowerLoad;
  //map<int, double> reactivePowerGen;
  //map<int, double> reactivePowerLoad;
  double* activePowerGen;
  double* activePowerLoad;
  double* reactivePowerGen;
  double* reactivePowerLoad;
  set<int> toShed;
  int baseMVA;
  virtual bool solvePF();
  virtual void calculateLinePF();
  virtual void calculatePowerGenerators();
  virtual void calculateObjectiveFunction();
  void initialValue();
 private:
  virtual void passResultToNetwork();
};

#endif
