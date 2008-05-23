/***************************************************************************

  TTrimSPDataHandler.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/23

***************************************************************************/

#include "TTrimSPDataHandler.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

// TrimSPData constructor -- reading all available trim.SP-rge-files into std::vectors

TTrimSPData::TTrimSPData(const string &path, vector<string> &energyVec) {

  //string energyArr[] = {"02_1", "02_5", "03_5", "05_0", "07_5", "10_0", "12_5", "15_0", "17_5", "19_0", "20_0", "22_5", "25_0"};
  
  double zz(0.0), nzz(0.0);
  vector<double> vzz, vnzz;
  string word, energyStr;
  
  for(unsigned int i(0); i<energyVec.size(); i++){
  
//    energyStr = "/home/l_wojek/nt/wojek/g/Bastian/ImplantationDepth/YBCO_PBCO-" + energyVec[i] + ".rge";
    
    energyStr = path + energyVec[i] + ".rge";
    
    fEnergy.push_back(atof(energyVec[i].replace(2,1,".").c_str()));
    
    ifstream *rgeFile = new ifstream(energyStr.c_str());
    if(! *rgeFile) {
      cout << "rge-file not found! Exit now." << endl;
      exit(-1);
    } else {
     
      while(*rgeFile >> word)
        if(word == "PARTICLES") break;

      while(!rgeFile->eof()) {
        *rgeFile >> zz >> nzz;
        vzz.push_back(zz);
        vnzz.push_back(nzz);
      }
      
      fDataZ.push_back(vzz);
      fDataNZ.push_back(vnzz);
      
      rgeFile->close();
      delete rgeFile;
      rgeFile = 0;
      vzz.clear();
      vnzz.clear();
      
    }
  }
}

// vector<double> DataZ(double) -- returning z-vector calculated by trim.SP for energy given energy

vector<double> TTrimSPData::DataZ(double e) const {
  
  for(unsigned int i(0); i<fEnergy.size(); i++) {
//    cout << tEnergy[i] << " " << e << " " << tEnergy[i] - e << endl;
    if(!(fEnergy[i] - e)) {
      return fDataZ[i];
    }
  }
 // default
  cout << "No implantation profile available for the specified energy... You got back the first one." << endl;
  return fDataZ[0];

}

// vector<double> DataNZ(double) -- returning n(z)-vector calculated by trim.SP for given energy

vector<double> TTrimSPData::DataNZ(double e) const {
  
  for(unsigned int i(0); i<fEnergy.size(); i++) {
    if(!(fEnergy[i] - e)) {
      return fDataNZ[i];
    }
  }
  // default
  cout << "No implantation profile available for the specified energy... You got back the first one." << endl;
  return fDataNZ[0];

}

// double GetNofZ(double, double) -- return NofZ for given z in nanometers

double TTrimSPData::GetNofZ(double zz, double e) const {
  
  vector<double> z, nz;
  
  for(unsigned int i(0); i<fEnergy.size(); i++) {
    if(!(fEnergy[i] - e)) {
      z = DataZ(e);
      nz = DataNZ(e);
      break;
    }
  }
  
  bool found = false;
  unsigned int i;
  for (i=0; i<z.size(); i++) {
    if (z[i]/10.0 >= zz) {
     found = true;
     break;
    }
  }

  if (!found)
    return -1.0;

  if (i == 0)
    return nz[0]*10.0*zz/z[0];

  return fabs(nz[i-1]+(nz[i]-nz[i-1])*(10.0*zz-z[i-1])/(z[i]-z[i-1]));
}
