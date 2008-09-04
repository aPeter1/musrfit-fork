/***************************************************************************

  TTrimSPDataHandler.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/09/02

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

    ifstream *rgeFile = new ifstream(energyStr.c_str());
    if(! *rgeFile) {
      cout << "rge-file not found! Try next energy..." << endl;
      delete rgeFile;
    } else {
      fEnergy.push_back(atof(energyVec[i].replace(2,1,".").c_str()));

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

  cout << "Read in " << fDataNZ.size() << " implantation profiles in total." << endl;

  fOrigDataNZ = fDataNZ;

  for(unsigned int i(0); i<fEnergy.size();i++)
    fIsNormalized.push_back(false);

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
// Method returning actual n(z)-vector calculated by trim.SP and
// potentially altered by the WeightLayers- or the Normalize-method for given energy[keV]
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

  for(unsigned int i(0); i<weight.size(); i++) {
    if(weight[i]-1.0)
      break;
    if(i == weight.size() - 1) {
      for(unsigned int j(0); j<fEnergy.size(); j++) {
        if(!(fEnergy[j] - e)) {
          fDataNZ[j] = fOrigDataNZ[j];
          fIsNormalized[j] = false;
          return;
        }
      }
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
    fIsNormalized[i] = false;
    return;
    }
  }

  cout << "No implantation profile available for the specified energy... No weighting done." << endl;
  return;
}

//---------------------
// Method returning n(z) for given z[nm] and energy[keV]
//---------------------

double TTrimSPData::GetNofZ(double zz, double e) const {

  vector<double> z, nz;

  for(unsigned int i(0); i<fEnergy.size(); i++) {
    if(!(fEnergy[i] - e)) {
      z = fDataZ[i];
      nz = fDataNZ[i];
      break;
    }
    if(i == fEnergy.size() - 1) {
      cout << "No implantation profile available for the specified energy... Quitting!" << endl;
      exit(-1);
    }
  }

  if(zz < 0)
    return 0.0;

  bool found = false;
  unsigned int i;
  for (i=0; i<z.size(); i++) {
    if (z[i]/10.0 >= zz) {
     found = true;
     break;
    }
  }

  if (!found)
    return 0.0;

  if (i == 0)
    return nz[0]*10.0*zz/z[0];

  return fabs(nz[i-1]+(nz[i]-nz[i-1])*(10.0*zz-z[i-1])/(z[i]-z[i-1]));
}

//---------------------
// Method normalizing the n(z)-vector calculated by trim.SP for a given energy[keV]
//---------------------

void TTrimSPData::Normalize(double e) const {

  for(unsigned int i(0); i<fEnergy.size(); i++) {
    if(!(fEnergy[i] - e)) {
      double nZsum = 0.0;
      for (unsigned int j(0); j<fDataZ[i].size(); j++)
        nZsum += fDataNZ[i][j];
      nZsum *= (fDataZ[i][1]-fDataZ[i][0]);
      for (unsigned int j(0); j<fDataZ[i].size(); j++)
        fDataNZ[i][j] /= nZsum;

      fIsNormalized[i] = true;
      return;
    }
  }
  // default
  cout << "No implantation profile available for the specified energy... No normalization done." << endl;
  return;

}

//---------------------
// Method telling you if the n(z)-vector calculated by trim.SP for a given energy [keV] has been normalized
//---------------------

bool TTrimSPData::IsNormalized(double e) const {
  for(unsigned int i(0); i<fEnergy.size(); i++) {
    if(!(fEnergy[i] - e)) {
      return fIsNormalized[i];
    }
  }

  cout << "No implantation profile available for the specified energy... Returning false! Check your code!" << endl;
  return false;
}

//---------------------
// Convolve the n(z)-vector calculated by trim.SP for a given energy e [keV] with a gaussian exp(-z^2/(2*w^2))
// No normalization is done!
//---------------------

void TTrimSPData::ConvolveGss(double w, double e) const {
  if(!w)
    return;

  vector<double> z, nz, gss;
  double nn;

  for(unsigned int i(0); i<fEnergy.size(); i++) {
    if(!(fEnergy[i] - e)) {
      z = fDataZ[i];
      nz = fOrigDataNZ[i];

      for(unsigned int k(0); k<z.size(); k++) {
        gss.push_back(exp(-z[k]*z[k]/200.0/w/w));
      }

      for(unsigned int k(0); k<nz.size(); k++) {
        nn = 0.0;
        for(unsigned int j(0); j<nz.size(); j++) {
          nn += nz[j]*gss[abs(int(k)-int(j))];
        }
        fDataNZ[i][k] = nn;
      }

      return;
    }
  }

  cout << "No implantation profile available for the specified energy... No convolution done!" << endl;
  return;
}
