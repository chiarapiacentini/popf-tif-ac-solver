#ifndef __Busbar_h__
#define __Busbar_h__
#include <iostream>
#include <map>

using namespace std;

class Busbar{

 public:
  Busbar();
  Busbar(string line);
  Busbar(string line, int id);
  void printBusbar(ostream & o) const;
  void changeLoad(double newPd, double newQd);
  void changePdLoad(double newPd);
  void changeQdLoad(double newQd);
  bool getIsLoad() const {
    return isLoad;
  }
  int getBus_i() const {
    return bus_i;
  }
  int getBusRealName() const {
    return bus_realName;
  }
  string getName() const {
    return name;
  }
  double getPdLoad() const {
    return Pd;
  }
  double getQdLoad() const {
    return Qd;
  }
  double getGs() const {
    return Gs;
  }
  double getBs() const {
    return Bs;
  }
  double getType() const {
    return type;
  }
  double getVm() const{
    return Vm;
  }
  double getVa() const{
    return Va;
  }
  void setVm(double n){
    Vm = n;
  }
  void setVa(double n){
    Va = n;
  }
  double getVmax(){
    return Vmax;
  }
  double getVmin(){
    return Vmin;
  }
  int getPreviousType() const {
      return previousType;
  }
  void setPd(double n){
    Pd = n;
  }
  void setQd(double n){
    Qd = n;
  }
  void addConnectedCapacitor(int id){
   connectedCapacitor.push_back(id); 
  }
  list<int> getConnectedCapacitor(){
   return connectedCapacitor; 
  }
  void setType(int n){
      //       cout << "type : " << type << endl;
      if(n!=type){
          cout << "changing " << endl;
          previousType = type;
          type = n;
          if(n==3){
              Va = 0; // not sure about this
          }
      }
  }
 private:
  string name;
  int bus_i;
  int bus_realName;
  int type;
  int previousType;
  double Pd;
  double Qd;
  double Gs;
  double Bs;
  int area;
  double Vm;
  double Va;
  double baseKV;
  int zone;
  double Vmax;
  double Vmin;
  bool isLoad;
  list<int> connectedCapacitor;
};


#endif
