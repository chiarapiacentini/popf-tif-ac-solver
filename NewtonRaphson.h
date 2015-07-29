#ifndef __NewtonRaphson_h__
#define __NewtonRaphson_h__

#include "Solver.h"

class NewtonRaphson: public Solver{
 public:
  NewtonRaphson(Network* net, int nI=40, double precision=0.001);
  virtual bool solvePF();
  private:
  int nIteration;
  double precision;
  void calculateJacobianAndDeltaX();
  void calculateJacobianElement(int i, int j);
  void calculateJacobianElement(int i, int j, double voltageI, double voltageJ, double deltaI, double deltaJ);
  bool newtonRaphson();
  bool solution();
  mat JJ;
  mat PP;
};
#endif
