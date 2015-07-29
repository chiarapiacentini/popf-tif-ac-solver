/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2014  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef NETWORKSOLVER_H
#define NETWORKSOLVER_H
#include "ExternalSolver.h"
#include "Network.h"
#include "Solver.h"

class NetworkSolver : public ExternalSolver{

public:
  NetworkSolver();
  ~NetworkSolver();
   virtual void loadSolver(string* parameters, int n);
   virtual map<string,double> callExternalSolver(map<string,double> initialState, bool isHeuristic); 
   virtual  list<string> getParameters();
   virtual  list<string> getDependencies();
private:
  Network theNetwork;
  Solver *theSolver;
  list<string> affected;
  list<string> dependencies;
  
};

#endif // NETWORKSOLVER_H
