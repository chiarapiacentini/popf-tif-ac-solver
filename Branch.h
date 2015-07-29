#ifndef __Branch_h__
#define __Branch_h__

#include <iostream>
#include <map>

using namespace std;

class Branch{
 public:
  Branch();
  Branch(string line, int id, map<int, int> busbarID);
  void printBranch(ostream & o) const;
  string getName() const{
    return name;
  }
  int getBranch_i() const{
    return branch_i;
  }
  bool getIsTransformer() const{
    if(ratio!=0){
      return true;
    }
    return false;
  }
  double getRatio() const{
    return ratio;
  }
  int getFBus() const{
    return fbus;
  }
  int getTBus() const{
    return tbus;
  }
  double getR() const{
    return r;
  }
  double getX() const{
    return x;
  }
  double getB() const{
    return b;
  }
  double getAngle() const{
    return angle;
  }
  double getReY() const{
    return reY;
  }
  double getImY() const{
    return imY;
  }
  double getBY() const{
    return bY;
  }
  int getStatus() const{
      return status;
  }
  void setReY(double n){
    reY = n;
  }
  void setImY(double n){
    imY = n;
  }
  void setBY(double n){
    bY = n;
  }
  void setRatio(double n){
    ratio = n;
  }
  void setAngle(double n){
    angle = n;
  }
  void setActivePowerIn(double n){
    activePowerIn = n;
  }
  void setActivePowerOut(double n){
    activePowerOut = n;
  }
  void setReactivePowerIn(double n){
    reactivePowerIn = n;
  }
  void setReactivePowerOut(double n){
    reactivePowerOut = n;
  }
  void setStatus(int n){
      status = n;
  }
  double getActivePowerIn(){
    return activePowerIn;
  }
  double getActivePowerOut(){
    return activePowerOut;
  }
  double getReactivePowerIn(){
    return reactivePowerIn;
  }
  double getReactivePowerOut(){
    return reactivePowerOut;
  }
 private:
  string name;
  int branch_i;
  int fbus;
  int tbus;
  int realFBus;
  int realTBus;
  double r;
  double x;
  double b;
  double rateA;
  double rateB;
  double rateC;
  double ratio;
  double angle;
  int status;
  double angmin;
  double angmax;
  double reY;
  double imY;
  double bY;
  double activePowerIn;
  double activePowerOut;
  double reactivePowerIn;
  double reactivePowerOut;

};
#endif
