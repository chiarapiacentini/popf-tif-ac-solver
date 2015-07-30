#ifndef __Capacitor_h__
#define __Capacitor_h__
#include <iostream>
#include <map>

using namespace std;

class Capacitor{

 public:
  Capacitor();
  Capacitor(string line);
  void printCapacitor(ostream & o) const;
  bool getIsActive() const {
    return isActive;
  }
  bool getWasActive() const {
    return wasActive;
  }
  bool getIsAlreadyIncluded() const{
    return isAlreadyIncluded;
  }
  int getBus() const {
    return bus;
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
  void setPd(double n){
    Pd = n;
  }
  void setQd(double n){
    Qd = n;
  }
  void setName(string n){
    name = n;
  }
  void setIsAlreadyIncluded(bool isai){
    isAlreadyIncluded=isai;
  }
  void setIsActive(bool is){
    isActive = is;
  }
  void setWasActive(bool is){
    isActive = is;
  }
 private:
  string name;
  int bus;
  double Pd;
  double Qd;
  bool isActive;
  bool wasActive;
  bool isAlreadyIncluded;
};


#endif
