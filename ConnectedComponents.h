/*
 *    <one line to give the program's name and a brief idea of what it does.>
 *    Copyright (C) 2014  <copyright holder> <email>
 * 
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef CONNECTEDCOMPONENTS_H
#define CONNECTEDCOMPONENTS_H

#include <armadillo>
#include <list>
#include <set>

using namespace arma;
using namespace std;

class ConnectedComponents
{
public:
    ConnectedComponents(mat g);
    ConnectedComponents(mat a, mat b, mat c);
    ~ConnectedComponents(){};
    int findNConnectedComponents();
    list<set<int> > findConnectedComponents();
    list<int> getNeighbors(int v);
private:
    mat graph;
    int n;
};

#endif // CONNECTEDCOMPONENTS_H
