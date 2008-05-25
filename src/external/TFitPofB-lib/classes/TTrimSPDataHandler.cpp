/***************************************************************************

  TTrimSPDataHandler.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/25

***************************************************************************/

#include "TTrimSPDataHandler.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;

//--------------------
// Constructor of the TrimSPData class -- reading all available trim.SP-rge-files with a given name into std::vectors
// The rge-file names have to contain the Implantation energy just before the rge-extension in the format %02u_%1u
// Example: string path("/home/user/TrimSP/SomeSample-");
//          string energyArr[] = {"02_1", "02_5", "03_5", "05_0", "07_5", "10_0", "12_5"};
//          vector<string> energyVec(energyArr, energyArr+(sizeof(energyArr)/sizeof(energyArr[0])));
//
// This will read the files "/home/user/TrimSP/SomeSample-02_1.rge", "/home/user/TrimSP/SomeSample-02_5.rge" and so on.
//--------------------

TTrimSPData::TTrimSPData(const string &path, vector<string> &energyVec) {

  double zz(0.0), nzz(0.0);
  vector<double> vzz, vnzz;
  string word, energyStr;

  for(unsigned int i(0); i<energyVec.size(); i++){

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

  fOrigDataNZ = fDataNZ;
}

//---------------------
// Method returning z-vector calculated by trim.SP for given energy[keV]
//---------------------

vector<double> TTrimSPData::DataZ(double e) const {

  for(unsigned int i(0); i<fEnergy.size(); i++) {
//    cout << tEnergy[i] << " " << e << " " << tEnergy[i] - e << endl;
    if(!(fEnergy[i] - e)) {
      return fDataZ[i];
    }
  }
  // default
  cout << "No implantation profile available for the specified energy... You get back the first one." << endl;
  return fDataZ[0];

}

//---------------------
// Method returning actual n(z)-vector calculated by trim.SP and potentially altered by the WeightLayers-method for given energy[keV]
//---------------------

vector<double> TTrimSPData::DataNZ(double e) const {

  for(unsigned int i(0); i<fEnergy.size(); i++) {
    if(!(fEnergy[i] - e)) {
      return fDataNZ[i];
    }
  }
  // default
  cout << "No implantation profile available for the specified energy... You get back the first one." << endl;
  return fDataNZ[0];

}

//---------------------
// Method returning original n(z)-vector calculated by trim.SP for given energy[keV]
//---------------------

vector<double> TTrimSPData::OrigDataNZ(double e) const {

  for(unsigned int i(0); i<fEnergy.size(); i++) {
    if(!(fEnergy[i] - e)) {
      return fOrigDataNZ[i];
    }
  }
  // default
  cout << "No implantation profile available for the specified energy... You get back the first one." << endl;
  return fOrigDataNZ[0];

}

//---------------------
// Method putting different weight to different layers of your thin film
// Parameters: Implantation Energy[keV], Interfaces[nm], Weights [0.0 <= w[i] <= 1.0]
// Example: 25.0, (50, 100), (1.0, 0.33, 1.0)
//          at 25keV consider 3 layers, where the first ends after 50nm, the second after 100nm (these are NOT the layer thicknesses!!)
//          the first and last layers get the full n(z), where only one third of the muons in the second layer will be taken into account
//---------------------

void TTrimSPData::WeightLayers(double e, const vector<double>& interface, const vector<double>& weight) const {

  if(weight.size()-interface.size()-1) {
    cout << "For the weighting the number of interfaces has to be one less than the number of weights!" << endl;
    cout << "No weighting of the implantation profile will be done unless you take care of that!" << endl;
    return;
  }

  for(unsigned int i(0); i<interface.size(); i++) {
    if (interface[i]<0.0) {
      cout << "One of your layer interfaces has a negative coordinate! - No weighting will be done!" << endl;
      return;
    }
    else if (i>1) {
      if (interface[i]<interface[i-1]) {
        cout << "The specified interfaces appear to be not in ascending order! - No weighting will be done!" << endl;
        return;
      }
    }
  }

  for(unsigned int i(0); i<weight.size(); i++) {
    if (weight[i]>1.0 || weight[i]<0.0) {
      cout << "At least one of the specified weights is out of range - no weighting will be done!" << endl;
      return;
    }
  }

  for(unsigned int i(0); i<fEnergy.size(); i++) {
    if(!(fEnergy[i] - e)) {
      unsigned int k(0);
      for(unsigned int j(0); j<fDataZ[i].size(); j++) {
        if(k<interface.size()) {
          if(fDataZ[i][j] < interface[k]*10.0)
            fDataNZ[i][j] = fOrigDataNZ[i][j]*weight[k];
          else {
            k++;
            fDataNZ[i][j] = fOrigDataNZ[i][j]*weight[k];
          }
        }
        else
          fDataNZ[i][j] = fOrigDataNZ[i][j]*weight[k];
      }
    }
  }

}

//---------------------
// Method returning n(z) for given z[nm] and energy[keV]
//---------------------

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
