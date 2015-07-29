#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <iomanip> 
#include <cassert> 
#include <complex>
#include <cmath>
#include <vector>
#include "Network.h"
#include "Busbar.h"
#include "Generator.h"
#include "Branch.h"
#include "GeneratorCostData.h"
#include "Capacitor.h"
#include "Solver.h"
#include "NewtonRaphson.h"
#include "ConnectedComponents.h"

using namespace std;

string smpc = "function mpc = ";
string sversion = "mpc.version = ";
string sbase = "mpc.baseMVA = ";
string sbus = "mpc.bus = [";
string sclose = "];";
string sgen = "mpc.gen = [";
string sbranch = "mpc.branch = [";
string sgencost = "mpc.gencost = [";
string scapacitor = "mpc.capacitor = [";
string scomment = "%";


vector<string> findParameters(string line, int &n){
    int nPos = 0;
    while(nPos!=-1){
        nPos=line.find("\t",nPos+1);
        n++;
    }
    vector<string> toReturn(n);
    nPos = 0;
    int i=0;
    while(nPos!=-1){
        int nPrevious = nPos;
        nPos=line.find("\t",nPos+1);
        string tmp =line.substr(nPrevious+1,nPos-nPrevious);
        if (nPos==-1){
            tmp=tmp.substr(0,tmp.length()-1);
        }
        toReturn[i]=tmp;
        i++;
    }
    
    if(toReturn[0].find("\t")){
        toReturn[0]=toReturn[0].substr(0,toReturn[0].find("\t"));
    }
    
    return toReturn;
}

Busbar::Busbar(){
    
}

Busbar::Busbar(string line){
    int n=0;
    vector <string> token = findParameters(line,n);
    if(n!=13){
        cerr << "Error while parsing busbar" << endl;
    }
    name="busbar"+token[0];
    bus_i = atoi(token[0].c_str());
    type = atoi(token[1].c_str());
    previousType = type;
    Pd = atof(token[2].c_str());
    Qd = atof(token[3].c_str());
    Gs = atof(token[4].c_str());
    Bs = atof(token[5].c_str());
    area = atoi(token[6].c_str());
    Vm = atof(token[7].c_str());
    Va = atof(token[8].c_str());
    baseKV = atof(token[9].c_str());
    zone = atoi(token[10].c_str());
    Vmax = atof(token[11].c_str());
    Vmin = atof(token[12].c_str());
    if(Pd!=0 || Qd!=0){
        isLoad = true;
    }else{
        isLoad = false;
    }
}

Busbar::Busbar(string line, int id){
    int n=0;
    vector<string> token = findParameters(line,n);
    if(n!=13){
        cerr << "Error while parsing busbar" << endl;
    }
    name="busbar"+token[0];
    bus_i = id;
    bus_realName = atoi(token[0].c_str());
    type = atoi(token[1].c_str());
    Pd = atof(token[2].c_str());
    Qd = atof(token[3].c_str());
    Gs = atof(token[4].c_str());
    Bs = atof(token[5].c_str());
    area = atoi(token[6].c_str());
    Vm = atof(token[7].c_str());
    Va = atof(token[8].c_str());
    baseKV = atof(token[9].c_str());
    zone = atoi(token[10].c_str());
    Vmax = atof(token[11].c_str());
    Vmin = atof(token[12].c_str());
    if(Pd!=0 || Qd!=0){
        isLoad = true;
    }else{
        isLoad = false;
    }
}
void Busbar::printBusbar(ostream & o) const{
    o<<"\t"<<bus_realName <<"\t"<<type<<"\t"<<Pd<<"\t"<<Qd<<"\t"<<Gs<<"\t"<<Bs<<"\t"<<area<<"\t"<<Vm<<"\t"<<Va;
    o<<"\t"<<baseKV<<"\t"<<zone<<"\t"<<Vmax<<"\t"<<Vmin<<";\n";
}

void Busbar::changeLoad(double newPd, double newQd) {
    if(isLoad){
        Pd = newPd;
        Qd = newQd;
    }else{
        cerr << "Busbar " << name << " is not a load" << endl;
    }
}

void Busbar::changePdLoad(double newPd) {
    if(isLoad){
        Pd = newPd;
    }else{
        cerr << "Busbar " <<  name << " is not a load" << endl;
    }
}

void Busbar::changeQdLoad(double newQd) {
    if(isLoad){
        Qd = newQd;
    }else{
        cerr << "Busbar " << name << " is not a load" << endl;
    }
}

Generator::Generator(){
    
}

Generator::Generator(string line, int id, map<int, int> busbarsID) {
    int n=0;
    vector<string> token = findParameters(line,n);
    if(n!=21){
        cerr << "Error while parsing generator" << endl;
    }
    
    std::ostringstream ostr;
    ostr<<"gen"<<id;
    name = ostr.str();
    gen_i = id;
    bus = busbarsID[atoi(token[0].c_str())];
    realBus = atoi(token[0].c_str());
    Pg = atof(token[1].c_str());
    Qg = atof(token[2].c_str());
    Qmax = atof(token[3].c_str());
    Qmin = atof(token[4].c_str());
    Vg = atof(token[5].c_str());
    mBase = atof(token[6].c_str());
    status = atof(token[7].c_str());
    Pmax = atof(token[8].c_str());
    Pmin = atof(token[9].c_str());
    Pc1 = atof(token[10].c_str());
    Pc2 = atof(token[11].c_str());
    Qc1min = atof(token[12].c_str());
    Qc1max = atof(token[13].c_str());
    Qc2min = atof(token[14].c_str());
    Qc2max = atof(token[15].c_str());
    ramp_agc = atof(token[16].c_str());
    ramp_10 = atof(token[17].c_str());
    ramp_30 = atof(token[18].c_str());
    ramp_q = atof(token[19].c_str());
    apf = atof(token[20].c_str());
    
}

void Generator::printGenerator(ostream & o) const{
    o << "\t"<<realBus<<"\t"<<Pg<<"\t"<<Qg<<"\t"<<Qmax<<"\t"<<Qmin<<"\t"<<Vg<<"\t"<<mBase<<"\t"<<status;
    o << "\t"<<Pmax<<"\t"<<Pmin<<"\t"<<Pc1<<"\t"<<Pc2<<"\t"<<Qc1min<<"\t"<<Qc1max<<"\t"<<Qc2min;
    o << "\t"<<Qc2max<<"\t"<<ramp_agc<<"\t"<<ramp_10<<"\t"<<ramp_30<<"\t"<<ramp_q<<"\t"<<apf<<";\n";
}

Branch::Branch(){
    
}

Branch::Branch(string line, int id, map<int, int> busbarsID){
    int n=0;
    vector<string> token = findParameters(line,n);
    if(n!=13){
        cerr << "Error while parsing branch" << endl;
    }
    
    std::ostringstream ostr;
    ostr<<"line"<<id;
    name = ostr.str();
    branch_i = id;
    fbus = busbarsID[atoi(token[0].c_str())];
    tbus = busbarsID[atoi(token[1].c_str())];
    realFBus = atoi(token[0].c_str());
    realTBus = atoi(token[1].c_str());
    r = atof(token[2].c_str());
    x = atof(token[3].c_str());
    b = atof(token[4].c_str());
    rateA = atof(token[5].c_str());
    rateB = atof(token[6].c_str());
    rateC = atof(token[7].c_str());
    ratio = atof(token[8].c_str());
    angle = atof(token[9].c_str());
    status = atoi(token[10].c_str());
    angmin = atof(token[11].c_str());
    angmax = atof(token[12].c_str());
}

void Branch::printBranch(ostream & o) const{
    o <<"\t"<<realFBus<<"\t"<<realTBus<<"\t"<<r<<"\t"<<x<<"\t"<<b<<"\t"<<rateA<<"\t"<<rateB<<"\t"<<rateC
    << "\t"<<ratio<<"\t"<<angle<<"\t"<<status<<"\t"<<angmin<<"\t"<<angmax<<";\n";
}

GeneratorCostData::GeneratorCostData(){
    
}

GeneratorCostData::GeneratorCostData(string line, int id, map<int, int> busbarsID){
    int n=0;
    vector<string> token = findParameters(line,n);
    if(n!=7){
        cerr << "Error while parsing generator cost data" << endl;
    }
    std::ostringstream ostr;
    ostr<<"gcd"<<id;
    name = ostr.str();
    gcd_i = id;
    status = atoi(token[0].c_str());
    startup = atof(token[1].c_str());
    shutdown = atof(token[2].c_str());
    gcd_n = atoi(token[3].c_str());
    x1 = atof(token[4].c_str());
    x2 = atof(token[5].c_str());
    x3 = atof(token[6].c_str());
    
}

void GeneratorCostData::printGeneratorCostData(ostream & o) const{
    
    o<<"\t"<<status<<"\t"<<startup<<"\t"<<shutdown<<"\t"<<gcd_n<<"\t"<<x1<<"\t"<<x2<<"\t"<<x3<<";\n";
}

void Network::parsingMatlabFile(string line){
    
    //  cout <<"... Parsing " << line << " file ..." << endl;
    ifstream inputFile(line.c_str());
    inFile = line;
    if (inputFile.is_open()) {
        while (inputFile.good() ){
            string lineFile;
            getline (inputFile,lineFile);
            if (lineFile.find(smpc)!=-1){
                mpc = getAfterParameter(lineFile, smpc);
                continue;
            }
            if (lineFile.find(sversion)!=-1){
                version = getAfterParameter(lineFile, sversion);
                continue;
            }
            if(lineFile.find(scomment)==0){
                continue;
            }
            if(lineFile.find(sbase)!=-1){
                string tmp = getAfterParameter(lineFile,sbase);
                tmp = tmp.substr(0,tmp.length()-1);
                baseMVA = atoi(tmp.c_str());
            }else{
                if(lineFile==sbus){
                    string substring;
                    getline(inputFile,substring);
                    while(substring!=sclose){
                        
                        Busbar *busbar = new Busbar(substring,nBus);
                        busbars.insert(std::pair<int,Busbar*>(busbar->getBus_i(),busbar));
                        busbarsID.insert(std::pair<int, int>(busbar->getBusRealName(),busbar->getBus_i()));
                        busbarsIDInv.insert(std::pair<int, int>(busbar->getBus_i(),busbar->getBusRealName()));
                        nBus++;
                        getline(inputFile,substring);
                        if(substring==sclose){
                            continue;
                        }
                    } 
                }else if(lineFile==sgen){
                    string substring;
                    getline(inputFile, substring);
                    while(substring!=sclose){
                        
                        Generator *generator = new Generator(substring,nGen,busbarsID);
                        generators.insert(pair<int,Generator*>(generator->getGen_i(),generator));
                        nGen++;
                        getline(inputFile,substring);
                        if(substring==sclose){
                            continue;
                        }
                    } 
                    
                }else  if(lineFile==sbranch){
                    string substring;
                    getline(inputFile, substring);
                    while(substring!=sclose){
                        
                        Branch *branch = new Branch(substring,nBranch,busbarsID);
                        branches.insert(std::pair<int,Branch*>(branch->getBranch_i(),branch));
                        mapBusToBranch.insert(pair<int, int>(branch->getFBus()+nBus*branch->getTBus(),branch->getBranch_i()));
                        mapBusToBranch.insert(pair<int, int>(branch->getTBus()+nBus*branch->getFBus(),branch->getBranch_i()));
                        nBranch++;
                        getline(inputFile,substring);
                        if(substring==sclose){
                            continue;
                        }
                    } 
                    
                }else  if(lineFile==sgencost){
                    string substring;
                    getline(inputFile, substring);
                    while(substring!=sclose){
                        
                        GeneratorCostData *generatorCostData = new GeneratorCostData(substring,nGenCost,busbarsID);
                        generatorsCostData.insert(std::pair<int,GeneratorCostData*>(nGenCost,generatorCostData));
                        nGenCost++;
                        
                        getline(inputFile,substring);
                        if(substring==sclose){
                            continue;
                        }
                    } 
                }
            }
        }
    }
    inputFile.close();
    
}

Capacitor::Capacitor(){
    
}

Capacitor::Capacitor(string line){
    int n = 0;
    vector<string> token = findParameters(line,n);
    if(n!=5){
        cerr << "Wrong format for capacitor" << endl;
        // busbar connected to\treactive power\tis active
    }
    bus = atoi(token[0].c_str());
    Pd = atof(token[1].c_str());
    Qd = atof(token[2].c_str());
    isActive = atoi(token[3].c_str())==1?true:false;
    wasActive = atoi(token[3].c_str())==!isActive;
    isAlreadyIncluded = atoi(token[4].c_str())==1?true:false;
}

void Capacitor::printCapacitor(ostream& o) const{
    o << name << " " << isActive << " " << isAlreadyIncluded << " " << Pd << " " << Qd << endl;
}

string Network::getAfterParameter(string line, string test){
    int start = line.find(test)+test.length();
    int end = line.length()-start;
    return line.substr(start,end);
}

Network::Network(string line){
    inFile = "-";
    nBus = 0;
    nGen = 0;
    nBranch = 0;
    nGenCost = 0;
    mpc = "";
    version = "";
    baseMVA = 1;
    parsingMatlabFile(line);
    initialisationNetwork();
    objectiveValue=0;
}

Network::Network(){
    
    nBus = 0;
    nGen = 0;
    nBranch = 0;
    nGenCost = 0;
    mpc = "";
    version = "";
    baseMVA = 1;
    objectiveValue=0;
}

void Network::printNetwork(ostream &o) const{
    
    o << "Network " << mpc << endl;
    o << "version " << version << endl;
    o << "baseMVA " << baseMVA << endl;
    o << "nBus " << nBus << endl;
    o << "nGen " << nGen << endl;
    o << "nBranch " << nBranch << endl;
    o << "nLoad " << nLoad << endl;
    
}

void Network::printNetworkAsMatlabFile(ostream & o) const{
    o << fixed << setprecision(8)<< smpc << mpc << endl;
    o << scomment <<" file generated from " << inFile<<endl;
    o << sversion << version << endl;
    o << "%%-----  Power Flow Data  -----%%" << endl;
    o << "%% system MVA base" << endl;
    o << sbase << baseMVA << " "<<  endl;
    o << "%% bus data" << endl;
    o << "%       bus_i   type    Pd      Qd      Gs      Bs      area    Vm      Va      baseKV  zone    Vmax    Vmin" << endl;
    o << sbus << endl;
    map<int,Busbar*>::const_iterator busIt = busbars.begin();
    map<int,Busbar*>::const_iterator busEnd = busbars.end();
    for(;busIt!=busEnd;++busIt){
        Busbar* bus=busIt->second;
        o << *bus;
    }
    o << sclose << endl << endl;
    o << "%% generator data" << endl;
    o << sgen << endl;
    map<int,Generator*>::const_iterator genIt = generators.begin();
    map<int,Generator*>::const_iterator genEnd = generators.end();
    for(;genIt!=genEnd;++genIt){
        Generator* gen = genIt->second;
        o << *gen;
    }
    o << sclose << endl << endl;
    o << "%% branch data" << endl;
    o << "%       fbus    tbus    r       x       b       rateA   rateB   rateC   ratio   angle   status  angmin  angmax" << endl;
    o << sbranch << endl;
    map<int,Branch*>::const_iterator brIt = branches.begin();
    map<int,Branch*>::const_iterator brEnd = branches.end();
    for(;brIt!=brEnd;++brIt){
        Branch* branch = brIt->second;
        o << *branch;
    }
    o << sclose << endl << endl;
    o << "%%-----  OPF Data  -----%%" << endl;
    o << "%% generator cost data" << endl;
    o << sgencost << endl;
    map<int,GeneratorCostData*>::const_iterator gcdIt = generatorsCostData.begin();
    map<int,GeneratorCostData*>::const_iterator gcdEnd = generatorsCostData.end();
    for(;gcdIt!=gcdEnd;++gcdIt){
        GeneratorCostData* gcd = gcdIt->second;
        o << *gcd;
    }
    o << sclose << endl << endl;
}

void Network::initialisationNetwork(){
    reY.reshape(nBus,nBus);
    imY.reshape(nBus,nBus);
    bY.reshape(nBus,nBus);
    populateLoad();
    populateTransformer();
    buildMapNames();
    calculateYMatrix();
    
}

void Network::populateLoad(){
    
    map<int, Busbar*>::const_iterator busIt = busbars.begin();
    const  map<int, Busbar*>::const_iterator busEnd = busbars.end();
    int idLoad = 0;
    for(;busIt!=busEnd;++busIt){
        int id = busIt->first;
        Busbar* busbar = busIt->second;
        bool isLoad = busbar->getIsLoad();
        if(isLoad){
            loads[idLoad]=id;
            idLoad++;
        }
    }
    nLoad = idLoad;
}

void Network::populateTransformer(){
    map<int, Branch*>::const_iterator brIt = branches.begin();
    const  map<int, Branch*>::const_iterator brEnd = branches.end();
    int idTransformer = 0;
    for(;brIt!=brEnd;++brIt){
        int id = brIt->first;
        Branch *branch = brIt->second;
        bool isTransformer = branch->getIsTransformer();
        if(isTransformer){
            transformers[idTransformer]=id;
            idTransformer++;
        }
    }
    nTransformer = idTransformer;
}

void Network::buildMapNames(){
    
    map<int, Busbar*>::const_iterator busIt = busbars.begin();
    const map<int, Busbar*>::const_iterator busEnd = busbars.end();
    for(;busIt!=busEnd;++busIt){
        int id = busIt->first;
        Busbar* busbar = busIt->second;
        string name = busbar->getName();
        nameBusbar[name]=id;
    }
    
    map<int, Generator*>::const_iterator genIt = generators.begin();
    const map<int, Generator*>::const_iterator genEnd = generators.end();
    for(;genIt!=genEnd;++genIt){
        int id = genIt->first;
        Generator *generator = genIt->second;
        string name = generator->getName();
        nameGenerator[name]=id;
    }
    
    map<int, Branch*>::const_iterator brIt = branches.begin();
    const map<int, Branch*>::const_iterator brEnd = branches.end();
    for(;brIt!=brIt;++brIt){
        int id = brIt->first;
        Branch *branch = brIt->second;
        string name = "line" + branch->getName();
        nameBranch[name] = id;
    }
    
    map<int, int>::iterator loIt = loads.begin();
    const map<int, int>::iterator loEnd = loads.end();
    for(;loIt!=loEnd;++loIt){
        int id = loIt->second;
        std::ostringstream ostr;
        ostr<<"load"<<loIt->first;
        string name = ostr.str();
        nameLoad[name]=id;
    }
    
    map<int, int>::iterator trIt = transformers.begin();
    const map<int, int>::iterator trEnd = transformers.end();
    for(;trIt!=trEnd;++trIt){
        int id = trIt->second;
        std::ostringstream ostr;
        ostr<<"tap"<<trIt->first;
        string name = ostr.str();
        nameTransformer[name]=id;
    }
    
}

void Network::changeOneLoad(string name, double newPd, double newQd) const{
    
    int idLoad = nameLoad.find(name)->second;
    changeOneLoad(idLoad, newPd, newQd);
}

void Network::changeOneLoad(int idLoad, double newPd, double newQd) const{
    
    if (idLoad>=nLoad){
        cerr<< "Error : " << idLoad << " is not in the network" << endl;
        return;
    }
    int idBus = loads.find(idLoad)->second;
    busbars.find(idBus)->second->changeLoad(newPd,newQd);
}

void Network::changeLoads(double scaling) const{
    
    for(int i=0;i<nLoad;i++){
        double newPd, newQd;
        newPd = busbars.find(loads.find(i)->second)->second->getPdLoad()*scaling;
        newQd = busbars.find(loads.find(i)->second)->second->getQdLoad()*scaling;
        changeOneLoad(i, newPd, newQd);
    }
}

void Network::changeLoadsFromFile(string inFile, string outDir, double constant, double initial) const{
    
    string basen;
    string command = "basename " + inFile;
    basen = GetStdoutFromCommand(command);
    int positionDot = basen.find(".");
    if (positionDot!=-1){
        basen = basen.substr(0,positionDot);
    }
    ifstream inputFile(inFile.c_str());
    double previous;
    int nLine = 0;
    if (inputFile.is_open()) {
        while (inputFile.good() ){
            
            string lineFile;
            getline (inputFile,lineFile);
            int middle = lineFile.find("\t");
            if (middle!=-1){
                double time = atof(lineFile.substr(0,middle).c_str());
                double power = atof(lineFile.substr(middle+1,-1).c_str());
                if(nLine==0){
                    previous = initial/constant;
                }
                double scaling = power/previous;
                changeLoads(scaling);
                previous = power;
                stringstream outName;
                outName << outDir << "t" << std::setw(5) << std::setfill('0') << time*1000 << ".m";
                string finalOutName = outName.str();
                ofstream outFile;
                outFile.open(finalOutName.c_str());
                if(!outFile.is_open()){
                    cerr << "Error in opening " << finalOutName << endl;
                    return;
                }else{
                    printNetworkAsMatlabFile(outFile);
                    cout << finalOutName << " written " << endl;
                }
                nLine++;
            }else{
                cerr << "wrong format in line " << lineFile << " of file " << inFile << endl;
            }
        }
    }
}

void Network::calculateYMatrix(){
    map<int, Branch*>::iterator brIt = branches.begin();
    map<int, Branch*>::iterator brEnd = branches.end();
    for(;brIt!=brEnd;++brIt){
        Branch* branch = brIt->second;
        int i = branch->getFBus();
        int j = branch->getTBus();
        pair<int,int> index(i,j);
        pair<int,int> simmIndex(j,i);
        pair<int,int> iIndex(i,i);
        pair<int,int> jIndex(j,j);
        complex<double> impedance(branch->getR(),branch->getX());
        complex<double> susceptance(0,branch->getB()*0.5);
        complex<double> admittance;
        complex<double> y;
        complex<double> tap(branch->getRatio(),0);
        if(branch->getRatio()==0){
            tap.real() = 1.;
        }
        
        admittance=1./impedance;
        y=admittance + susceptance;
        
        branch->setReY(-(admittance/conj(tap)).real());
        branch->setImY(-(admittance/conj(tap)).imag());
        branch->setBY(branch->getB()*0.5);
        //branch->setReY(-(admittance).real());
        //branch->setImY(-(admittance).imag());
        //branch->setBY(branch->getB()*0.5);
        
        reY.at(i,i)+=y.real()/abs(tap)/abs(tap);
        imY.at(i,i)+=y.imag()/abs(tap)/abs(tap);
        reY.at(j,j)+=y.real();
        imY.at(j,j)+=y.imag();
        reY.at(i,j)-=(admittance/conj(tap)).real();
        imY.at(i,j)-=(admittance/conj(tap)).imag();
        reY.at(j,i)-=(admittance/tap).real();
        imY.at(j,i)-=(admittance/tap).imag();
        bY.at(i,j)+=branch->getB()*0.5;
        bY.at(j,i)+=branch->getB()*0.5;
        mapTap[index]=tap.real();
        mapTap[simmIndex]=1./tap.real();
    }
    map<int, Busbar*>::iterator busIt = busbars.begin();
    map<int, Busbar*>::iterator busEnd = busbars.end();
    for(;busIt!=busEnd;++busIt){
        int i = busIt->first;
        Busbar *busbar = busIt->second;
        pair<int,int> index(i,i);
        reY.at(i,i)+=busbar->getGs()/baseMVA;
        imY.at(i,i)+=busbar->getBs()/baseMVA;
    }
    
}

void Network::solvePowerFlow(bool printResult){
    
    bool isSolved = solver->solve();
    if (printResult)
        solver->printSolution();
}

void Network::solvePowerFlow(int i, bool printResult){
    //  Solver* solver;
    solver = new NewtonRaphson(this);
    bool isSolved = solver->solve();
    if (printResult)
        solver->printSolution();
    //  delete solver;
}

bool Network::isOptimal(){
    return solver->getIsOptimal();
}

void Network::setTapTransformer(string name, double voltageTarget){
    int idTap = nameTransformer[name];
    //cout << name << " " << idTap << endl;
    Branch *branch = branches[idTap];
    Busbar *from = busbars[branch->getFBus()];
    Busbar *to = busbars[branch->getTBus()];
    double voltageFrom = from->getVm();
    double voltageTo = to->getVm();
    double ratio = voltageFrom/voltageTo;
    int position = (ratio-1)*100;
    cout << name << " " << idTap << " " << position << endl; 
    map<string, pair<double, double> > mapTap;
    mapTap[name] = make_pair(position,0);
    changeTransformerParameters(mapTap);
    
}

void Network::setTapTransformers(double voltageTarget){
    Solver *solver = new NewtonRaphson(this);
    setSolver(solver);
    solvePowerFlow(false);
    for(map<string,int>::iterator itTap = nameTransformer.begin();itTap!=nameTransformer.end();++itTap){
        //     cout << itTap->first << " " << itTap->second <<endl;
        string nameTap = itTap->first; 
        setTapTransformer(nameTap,voltageTarget);
    }
    
}
void Network::copyNetwork(const Network& net){
    busbars = net.getBusbars();
    generators = net.getGenerators();
    branches = net.getBranches();
    capacitors = net.getCapacitors();
}

Network& Network::operator = (const Network& net){
    inFile=net.inFile;
    nBus = net.nBus;
    nGen = net.nGen;
    nBranch = net.nBranch;
    nGenCost = net.nGenCost;
    nLoad = net.nLoad;
    nTransformer = net.nTransformer;
    loads = net.loads;
    transformers = net.transformers;
    nameBusbar = net.nameBusbar;
    nameGenerator = net.nameGenerator;
    nameBranch = net.nameBranch;
    nameLoad = net.nameLoad;
    nameTransformer = net.nameTransformer;
    nameCapacitor = net.nameCapacitor;
    mapTap = net.mapTap;
    mapBusToBranch = net.mapBusToBranch;
    mpc = net.mpc;
    version = net.version;
    baseMVA = net.baseMVA;
    solver = net.solver;
    busbarsID = net.busbarsID;
    busbarsIDInv = net.busbarsIDInv;
    //   objectiveValue = net.objectiveValue;
    reY = net.reY;
    imY = net.imY;
    bY = net.bY;
    
    map<int, Busbar*>::const_iterator busIt = net.busbars.begin();
    map<int, Busbar*>::const_iterator busEnd = net.busbars.end();
    for(;busIt!=busEnd;++busIt){
        int first = busIt->first;
        Busbar* busbar = busIt->second;
        Busbar* toCopy = new Busbar();
        *toCopy = *busbar;
        busbars[first]=toCopy;
    }
    
    map<int, Generator*>::const_iterator genIt = net.generators.begin();
    map<int, Generator*>::const_iterator genEnd = net.generators.end();
    for(;genIt!=genEnd;++genIt){
        int first = genIt->first;
        Generator* generator = genIt->second;
        Generator* toCopy = new Generator();
        *toCopy = *generator;
        generators[first]=toCopy;
    }
    
    map<int, Branch*>::const_iterator brIt = net.branches.begin();
    map<int, Branch*>::const_iterator brEnd = net.branches.end();
    for(;brIt!=brEnd;++brIt){
        int first = brIt->first;
        Branch* branch = brIt->second;
        Branch* toCopy = new Branch();
        *toCopy = *branch;
        branches[first] = toCopy;
    }
    
    map<int, GeneratorCostData*>::const_iterator gcdIt = net.generatorsCostData.begin();
    map<int, GeneratorCostData*>::const_iterator gcdEnd = net.generatorsCostData.end();
    for(;gcdIt!=gcdEnd;++gcdIt){
        int first = gcdIt->first;
        GeneratorCostData* gcd = gcdIt->second;
        GeneratorCostData* toCopy = new GeneratorCostData();
        *toCopy = *gcd;
        generatorsCostData[first] = toCopy;
    }
    
    map<int, Capacitor*>::const_iterator cIt = net.capacitors.begin();
    map<int, Capacitor*>::const_iterator cEnd = net.capacitors.end();
    for(;cIt!=cEnd;++cIt){
        int first = cIt->first;
        Capacitor* c = cIt->second;
        Capacitor* toCopy = new Capacitor();
        *toCopy = *c;
        capacitors[first] = toCopy;
    }
}
void Network::disconnectLine(int idLine, bool isConnected){
    
    int nComponentsBefore = ConnectedComponents(reY,imY,bY).findNConnectedComponents();
    Branch* branch = branches[idLine];
    int i = branch->getFBus();
    int j = branch->getTBus();
    pair<int,int> index(i,j);
    pair<int,int> simmIndex(j,i);
    pair<int,int> iIndex(i,i);
    pair<int,int> jIndex(j,j);
    complex<double> impedance(branch->getR(),branch->getX());
    complex<double> susceptance(0,branch->getB()*0.5);
    complex<double> admittance;
    complex<double> y;
    complex<double> tap(branch->getRatio(),0);
    if(branch->getRatio()==0){
        tap.real() = 1.;
    }        
    admittance=1./impedance;
    y=admittance + susceptance;
    
    int flag =(isConnected?-1:1);
    reY.at(i,i)-= flag*y.real()/abs(tap)/abs(tap);
    imY.at(i,i)-= flag*y.imag()/abs(tap)/abs(tap);
    reY.at(j,j)-= flag*y.real();
    imY.at(j,j)-= flag*y.imag();
    reY.at(i,j)+= flag*(admittance/conj(tap)).real();
    imY.at(i,j)+= flag*(admittance/conj(tap)).imag();
    reY.at(j,i)+= flag*(admittance/tap).real();
    imY.at(j,i)+= flag*(admittance/tap).imag();
    bY.at(i,j)-= flag*branch->getB()*0.5;
    bY.at(j,i)-= flag*branch->getB()*0.5;
    
    recalculateSlack(nComponentsBefore);
}

void Network::recalculateSlack(int nComponentsBefore)
{
//     cout << "recalculating slack" << endl;
    ConnectedComponents cc(reY,imY,bY);
    list<set<int> > connectedComponents = cc.findConnectedComponents();
    int nComponentsAfter = connectedComponents.size();
//     cout << nComponentsBefore << " " << nComponentsAfter << endl;
    if(nComponentsAfter>nComponentsBefore){
        list<set<int> >::iterator subNetwork = connectedComponents.begin();
        list<set<int> >::iterator snEnd = connectedComponents.end();
        for(;subNetwork!=snEnd;++subNetwork){
            bool thereIsSlack = false;
            for(set<int>::iterator sIt=subNetwork->begin();sIt!=subNetwork->end();++sIt){
                Busbar* bus = busbars[*sIt];
                if(bus->getType()==3){
                    thereIsSlack=true;
                    break;
                }
            }
            if(thereIsSlack)
                continue;
            //             cout << endl;
            map<int,Generator*>::iterator gIt = generators.begin();
            map<int,Generator*>::iterator gEnd = generators.end();
            double maxGenP = 0;
            int maxGen = -1;
            for(;gIt!=gEnd;++gIt){
                //                 cout << "generator " << gIt->first << " is connectedd to bus " << busbarsIDInv[gIt->second->getBus()] << endl;
                int idBus = gIt->second->getBus();
                if (subNetwork->find(idBus)!=subNetwork->end()){
                    if(maxGenP<gIt->second->getPmax()){
                        maxGenP = gIt->second->getPmax();
                        maxGen = idBus;
                    }
                }
            }
            if(maxGen!=-1){
                busbars[maxGen]->setType(3);
                //                 cout << "busbar " << maxGen << " is a new slack" <<endl;
            }
        }
    }else if(nComponentsAfter<nComponentsBefore){
        cout << "we are here!" << endl;
        
        list<set<int> >::iterator subNetwork = connectedComponents.begin();
        list<set<int> >::iterator snEnd = connectedComponents.end();
        for(;subNetwork!=snEnd;++subNetwork){
            int thereIsSlack = 0;
            for(set<int>::iterator sIt=subNetwork->begin();sIt!=subNetwork->end();++sIt){
                Busbar* bus = busbars[*sIt];
                if(bus->getType()==3){
                    thereIsSlack++;
                }
            }
            cout << "how many slacks " << thereIsSlack << endl;
            if(thereIsSlack>1){
                for(set<int>::iterator sIt=subNetwork->begin();sIt!=subNetwork->end();++sIt){
                    Busbar* bus = busbars[*sIt];
                    if(bus->getType()==3){
                        cout << "\tbus " << *sIt << " has previous type " << bus->getPreviousType() << endl;
                        bus->setType(bus->getPreviousType());
                    }
                }   
            }
        }
        
    }
}

void Network::changeLoadParameters(map<string, pair<double, double> >newValues){
    map<string, pair<double, double> >::iterator it=newValues.begin();
    const map<string, pair<double, double> >::iterator itEnd = newValues.end();
    for(;it!=itEnd;++it){
        string name = it->first;
        double nap = it->second.first;
        double nrp = it->second.second;
        if(nameLoad.find(name)==nameLoad.end()){
            cout << name << " not found" << endl;
            continue;
        }
        int idBus = nameLoad[name];
        Busbar *busbar = busbars[idBus];
        busbar->setPd(nap);
        busbar->setQd(nrp);
/*        list<int> connectedCapacitor = busbar->getConnectedCapacitor();
        list<int>::iterator ccIt = connectedCapacitor.begin();
        list<int>::iterator ccEnd = connectedCapacitor.end();
        for(;ccIt!=ccEnd;++ccIt){
            Capacitor *capacitor = capacitors.find(*ccIt)->second;
            if (capacitor->getIsActive()){
                busbar->setQd(busbar->getQdLoad()+capacitor->getQdLoad()); 
            }
	    }*/
    } 
}

void Network::changeGeneratorParameters(map<string, pair<double, double> >newValues){
    map<string, pair<double, double> >::iterator it=newValues.begin();
    const map<string, pair<double, double> >::iterator itEnd = newValues.end();
    for(;it!=itEnd;++it){
        string name = it->first;
        double nap = it->second.first;
        double nrp = it->second.second;
        int idGen = nameGenerator[name];
        if(nameGenerator.find(name)==nameGenerator.end()){
            cout << name << " not found" << endl;
            continue;
        }
        Generator *generator = generators[idGen];
        generator->setPg(nap);
        generator->setQg(nrp);
    }
}

void Network::changeGeneratorsParameters(double scalingFactor){
    map<int, Generator*>::iterator genIt = generators.begin();
    map<int, Generator*>::iterator genEnd = generators.end();
    for(;genIt!=genEnd;++genIt){
        Generator* gen = genIt->second;
        Busbar* bus = busbars[gen->getBus()];
        if(bus->getType()!=3 /*&& gen->getName()=="gen1"*/){
            gen->setPg(gen->getPg()*scalingFactor);
            gen->setQg(gen->getQg()*scalingFactor);
        }/*else{
            gen->setPg(0);
            gen->setQg(0);
    }*/
    }
}


void Network::changeTransformerParameters(map<string, pair<double, double> >newValues){
    map<string, pair<double, double> >::iterator it=newValues.begin();
    const map<string, pair<double, double> >::iterator itEnd = newValues.end();
    for(;it!=itEnd;++it){
        string name = it->first;
        double nratio = it->second.first;
        double nangle = it->second.second;
        if(nameTransformer.find(name)==nameTransformer.end()){
            cout << name << " not found" << endl;
            continue;
        }
        int idBranch = nameTransformer[name];
        //     cout << name << " " << idBranch << endl;
        Branch *branch = branches[idBranch];
        double oldTapRatio = branch->getRatio();
        double newTapRatio = 1+nratio/100.;
        branch->setRatio(newTapRatio);
        int i = branch->getFBus();
        int j = branch->getTBus();
        pair<int,int> index(i,j);
        pair<int,int> simmIndex(j,i);
        pair<int,int> iIndex(i,i);
        pair<int,int> jIndex(j,j);
        complex<double> impedance(branch->getR(),branch->getX());
        complex<double> susceptance(0,branch->getB()*0.5);
        complex<double> admittance;
        complex<double> y;
        
        complex<double>tap(newTapRatio,0);
        complex<double>oldTap(oldTapRatio,0);
        admittance=1./impedance;
        y=admittance + susceptance;
        
        branch->setReY(-(admittance/conj(tap)).real());
        branch->setImY(-(admittance/conj(tap)).imag());
        branch->setBY(branch->getB()*0.5);
        // old
        reY.at(i,i)-=y.real()/abs(oldTap)/abs(oldTap);
        imY.at(i,i)-=y.imag()/abs(oldTap)/abs(oldTap);
        reY.at(i,j)+=(admittance/conj(oldTap)).real();
        imY.at(i,j)+=(admittance/conj(oldTap)).imag();
        reY.at(j,i)+=(admittance/oldTap).real();
        imY.at(j,i)+=(admittance/oldTap).imag();
        // new
        reY.at(i,i)+=y.real()/abs(tap)/abs(tap);
        imY.at(i,i)+=y.imag()/abs(tap)/abs(tap);
        reY.at(i,j)-=(admittance/conj(tap)).real();
        imY.at(i,j)-=(admittance/conj(tap)).imag();
        reY.at(j,i)-=(admittance/tap).real();
        imY.at(j,i)-=(admittance/tap).imag();
        mapTap[index]=tap.real();
        mapTap[simmIndex]=1./tap.real();       
    }
}

void Network::changeCapacitorParameters(std::map< string, bool > newValues){
    map<string, bool>::iterator it = newValues.begin();
    const map<string, bool>::iterator itEnd = newValues.end();
    for(;it!=itEnd;++it){
        string name = it->first;
        bool isActive = it->second;
        if(nameCapacitor.find(name)==nameCapacitor.end()){
            cout << name << " not found" << endl;
            continue;
        }
        int idCapacitor = nameCapacitor.find(name)->second;
        if(isActive){
            activateCapacitor(idCapacitor);
        }else{
            deactivateCapacitor(idCapacitor);
        }
    }
}
void Network::changeSwitchParameters(std::map< string, int > newValues){
    map<string, int>::iterator it = newValues.begin();
    const map<string, int>::iterator itEnd = newValues.end();
    for(;it!=itEnd;++it){
        string name = it->first;
        int status = it->second;
        if(nameBranch.find(name)==nameBranch.end()){
            cout << name << " not found" << endl;
            continue;
        }
        int idBranch = nameBranch.find(name)->second;
        Branch* branch = branches[idBranch];
        if (branch->getStatus()!=status){
            bool disconnecting = status==1?false:true;
            disconnectLine(idBranch,disconnecting);
            branch->setStatus(status);
        }
    }
}

void Network::addCapacitors(string nameFile){
    cout <<"... Parsing " << nameFile << " file ..." << endl;
    ifstream inputFile(nameFile.c_str());
    nCapacitor = 0;
    if (inputFile.is_open()) {
        while (inputFile.good() ){
            string lineFile;
            getline (inputFile,lineFile);
            if (lineFile=="")
                continue;
            Capacitor *capacitor = new Capacitor(lineFile);
            ostringstream convert;   // stream used for the conversion
            convert << nCapacitor;
            string name = "capacitor" + convert.str();
            capacitor->setName(name);
            capacitors.insert(std::pair<int,Capacitor*>(nCapacitor,capacitor));
            nameCapacitor.insert(std::pair<string,int>(name,nCapacitor));
            Busbar* busbar = busbars.find(capacitor->getBus())->second;
            busbar->addConnectedCapacitor(nCapacitor);
            /*if (capacitor->getIsActive() && !capacitor->getIsAlreadyIncluded()){
                busbar->setPd(busbar->getPdLoad()+capacitor->getPdLoad());
                busbar->setQd(busbar->getQdLoad()+capacitor->getQdLoad());
		} */ 
            nCapacitor++;
        }
    }
}

void Network::activateCapacitor(int idCapacitor){
    Capacitor* capacitor = capacitors.find(idCapacitor)->second;
    //   if(!capacitor->getIsActive()){
	capacitor->setIsActive(true);
//	Busbar* busbar = busbars.find(capacitor->getBus())->second;
//	busbar->setPd(busbar->getPdLoad()+capacitor->getPdLoad());
//	busbar->setQd(busbar->getQdLoad()+capacitor->getQdLoad());
//    }
}

void Network::deactivateCapacitor(int idCapacitor){
    Capacitor* capacitor = capacitors.find(idCapacitor)->second;
    //if(capacitor->getIsActive()){
	capacitor->setIsActive(false);
//	Busbar* busbar = busbars.find(capacitor->getBus())->second;
	//       busbar->setPd(busbar->getPdLoad()-capacitor->getPdLoad());
//        busbar->setQd(busbar->getQdLoad()-capacitor->getQdLoad());
	//   }
}

ostream & operator<< (ostream &o, const Network & a) {
    a.printNetwork(o);
    return o;
}

ostream & operator<< (ostream &o, const Busbar & a) {
    a.printBusbar(o);
    return o;
}

ostream & operator<< (ostream &o, const Generator & a) {
    a.printGenerator(o);
    return o;
}

ostream & operator<< (ostream &o, const Branch & a) {
    a.printBranch(o);
    return o;
}

ostream & operator<< (ostream &o, const GeneratorCostData & a) {
    a.printGeneratorCostData(o);
    return o;
}

ostream & operator<< (ostream &o, const Capacitor & a){
    a.printCapacitor(o);
    return o;
}

string GetStdoutFromCommand(string cmd) {
    
    string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");
    
    stream = popen(cmd.c_str(), "r");
    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
            pclose(stream);
    }
    return data;
}
