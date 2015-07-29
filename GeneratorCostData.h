#ifndef __GeneratorCostData_h__
#define __GeneratorCostData_h__

#include <iostream>
#include <map>

class GeneratorCostData{

 public:
  GeneratorCostData();
  GeneratorCostData(string line,int id, map<int, int> busbarsID);
  void printGeneratorCostData(ostream & o) const;
  int getGcd_i(){
    return gcd_i;
  }
  double getX1(){
    return x1;
  }
  double getX2(){
    return x2;
  }
  double getX3(){
    return x3;
  }
 private:
  string name;
  int gcd_i;
  int status;
  double startup;
  double shutdown;
  int gcd_n;
  double x1;
  double x2;
  double x3;
};

#endif
