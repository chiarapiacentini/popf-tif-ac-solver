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


#ifndef EXTERNALSOLVER_H
#define EXTERNALSOLVER_H

#include <iostream>
#include <map>
#include <list>

using namespace std;

class ExternalSolver
{

public:
ExternalSolver();
virtual ~ExternalSolver();
virtual void loadSolver(string* parameters, int n)=0;
virtual map<string,double> callExternalSolver(map<string,double> initialState, bool isHeuristic)=0;
static bool isActive;
static bool isActiveHeuristic;
virtual list<string> getParameters()=0;
virtual list<string> getDependencies()=0;
static string name;
static ExternalSolver *externalSolver;
};

#endif // EXTERNALSOLVER_H
