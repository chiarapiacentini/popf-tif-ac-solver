#ifndef __Generator_h__
#define __Generator_h__
#include <iostream>
#include <map>

using namespace std;

class Generator{

 public:
  Generator();
  Generator(string line, int id, map<int, int> busbarsID );
  void printGenerator(ostream & o) const;
  string getName() const {
    return name;
  } 
  ~Generator(){
    
  }
  double getPg() const {
    return Pg;
  }
  double getQg() const{
    return Qg;
  }
  double getBus() const{
    return bus;
  }
  int getGen_i() const{
    return gen_i;
  }
  double getPmax() const{
    return Pmax;
  }
  double getPmin() const{
    return Pmin;
  }
  double getQmax() const{
    return Qmax;
  }
  double getQmin() const{
    return Qmin;
  }
  void setPg(double n){
    Pg = n;
  }
  void setQg(double n){
    Qg = n;
  }
 private:
  string name;
  int gen_i;
  int bus;
  int realBus;
  double Pg;
  double Qg;
  double Qmax;
  double Qmin;
  double Vg;
  double mBase;
  double status;
  double Pmax;
  double Pmin;
  double Pc1;
  double Pc2;
  double Qc1min;
  double Qc1max;
  double Qc2min;
  double Qc2max;
  double ramp_agc;
  double ramp_10;
  double ramp_30;
  double ramp_q;
  double apf;
};

#endif
