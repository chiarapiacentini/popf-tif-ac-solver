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


#include "ExternalSolver.h"
#include <dlfcn.h>

ExternalSolver::ExternalSolver()
{

}

ExternalSolver::~ExternalSolver()
{

}

bool ExternalSolver::isActive = false;
bool ExternalSolver::isActiveHeuristic = false;
list<string> ExternalSolver::parameters = list<string>();
list<string> ExternalSolver::dependencies = list<string>();
string ExternalSolver::name = "";
void* handle = dlopen(ExternalSolver::name.c_str(), RTLD_LAZY);
ExternalSolver *ExternalSolver::externalSolver = (ExternalSolver*)dlsym(handle,"create_object");