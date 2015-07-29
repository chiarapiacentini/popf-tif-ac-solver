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


#include "ConnectedComponents.h"
#include <queue>
#include <list>

using namespace std;

ConnectedComponents::ConnectedComponents(mat g){
    
    graph = g;
    n = graph.n_cols;//as name indicates it returns number of vertices in graph
}

ConnectedComponents::ConnectedComponents(mat a, mat b, mat c)
{
    graph = abs(a) + abs(b) + abs(c);
    n = graph.n_cols;//as name indicates it returns number of vertices in graph
}

int ConnectedComponents::findNConnectedComponents()
{
    int components=0;
    queue<int> S;
    bool* visited = new bool[n];
    for(int i=1;i<n;i++)
        visited[i]=false;
    
    visited[0]=true;
    S.push(0);
    while(!S.empty())
    {
        int v = S.front();
        S.pop();
        list<int> x = getNeighbors(v);//as name indicates this function returns list of neighbours of given vertice
        if(!x.empty())
        {
            list<int>::iterator it;
            for (it=x.begin(); it!=x.end(); it++)
            {
                if(visited[*it]==false)
                {
                    S.push(*it);
                    visited[*it]=true;
                }
            }
        }
        
        if(S.empty())
        {
            components++;
            for(int i=1;i<n;i++)
            {
                if(visited[i]==false)
                {
                    S.push(i);
                    visited[i]=true;
                    break;
                }
            }
        }
    }
    
    return components;
}

list<set<int> > ConnectedComponents::findConnectedComponents()
{
    int components=0;
    queue<int> S;
    bool* visited = new bool[n];
    for(int i=1;i<n;i++)
        visited[i]=false;
    
    visited[0]=true;
    S.push(0);
    list<set<int> > connectedComponents;
    set<int> ccomp;
    ccomp.insert(0);
    while(!S.empty())
    {
        int v = S.front();
        S.pop();
        list<int> x = getNeighbors(v);//as name indicates this function returns list of neighbours of given vertice
        if(!x.empty())
        {
            list<int>::iterator it;
            for (it=x.begin(); it!=x.end(); it++)
            {
                if(visited[*it]==false)
                {
                    ccomp.insert(*it);
                    S.push(*it);
                    visited[*it]=true;
                }
            }
        }
        
        if(S.empty())
        {
            components++;
            connectedComponents.push_back(ccomp);
            ccomp.clear();
            for(int i=1;i<n;i++)
            {
                if(visited[i]==false)
                {
                    S.push(i);
                    ccomp.insert(i);
                    visited[i]=true;
                    break;
                }
            }
        }
    }
    
    return connectedComponents;
}
list< int > ConnectedComponents::getNeighbors(int v){
    
    list<int> x;
    if(v<0||v>=n)
        return x;
    
    for(int j=0;j<n;j++)
    {
        if(graph.at(v,j)!=0)
            x.push_front(j);
    }
    return x;
}
