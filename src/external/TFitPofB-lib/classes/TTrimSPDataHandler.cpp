/***************************************************************************

  TTrimSPDataHandler.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/05/15

***************************************************************************/

#include "TTrimSPDataHandler.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <cassert>
#include <algorithm>

using namespace std;

//--------------------
// Constructor of the TrimSPData class -- reading all available trim.SP-rge-files with a given name into std::vectors
// The rge-file names have to contain the Implantation energy just before the rge-extension in the format %02u_%1u
// Example: string path("/home/user/TrimSP/SomeSample-");
//          string energyArr[] = {"02_1", "02_5", "03_5", "05_0", "07_5", "10_0", "12_5"};
//          vector<string> energyVec(energyArr, energyArr+(sizeof(energyArr)/sizeof(energyArr[0])));
//
// This will read the files "/home/user/TrimSP/SomeSample-02_1.rge", "/home/user/TrimSP/SomeSample-02_5.rge" and so on.
//
// Alternative supported energy formats in the energyVec are, e.g. for E=2.1keV:
// <some_path>02-1.rge
// <some_path>02.1.rge
// <some_path>021.rge
//
// <some_path>21.rge is explicitly not possible since it is not clear, if this denotes 2.1keV or 21.0keV!
//
// Also always use the same format within one energyVec - otherwise sorting of the vector will not work properly!
//--------------------

TTrimSPData::TTrimSPData(const string &path, vector<string> &energyVec) {

  // sort the energies in ascending orders - this might be useful for later applications (energy-interpolations etc.)
  sort(energyVec.begin(), energyVec.end());

  double zz(0.0), nzz(0.0);
  vector<double> vzz, vnzz;
  string word, energyStr;

  for(unsigned int i(0); i<energyVec.size(); i++){

    energyStr = path + energyVec[i] + ".rge";

    ifstream *rgeFile = new ifstream(energyStr.c_str());
    if(! *rgeFile) {
      cout << "TTrimSPData::TTrimSPData: rge-file not found! Try next energy..." << endl;
      delete rgeFile;
      rgeFile = 0;
    } else {
      if (energyVec[i].length() == 4)
        fEnergy.push_back(atof(energyVec[i].replace(2,1,".").c_str()));
      else if (energyVec[i].length() == 3) {
        energyVec[i].insert(energyVec[i].end()-1, 1, '.');
        fEnergy.push_back(atof(energyVec[i].c_str()));
      } else {
        cout << "TTrimSPData::TTrimSPData: The energy cannot be correctly extracted from the rge-file name!" << endl;
        cout << "TTrimSPData::TTrimSPData: Please use file names in one of the following formats, e.g. for E=2.1keV use:" << endl;
        cout << "TTrimSPData::TTrimSPData: <some_path>02_1.rge" << endl;
        cout << "TTrimSPData::TTrimSPData: <some_path>02-1.rge" << endl;
        cout << "TTrimSPData::TTrimSPData: <some_path>02.1.rge" << endl;
        cout << "TTrimSPData::TTrimSPData: <some_path>021.rge" << endl;
        assert(false);
      }

      while(*rgeFile >> word)
        if(word == "PARTICLES") break;

      while(!rgeFile->eof()) {
        *rgeFile >> zz >> nzz;
        vzz.push_back(zz);
        vnzz.push_back(nzz);
      }

      fDZ.push_back(vzz[1]-vzz[0]);

      while(zz < 2100.0){
        zz += *(fDZ.end()-1);
        vzz.push_back(zz);
        vnzz.push_back(0.0);
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

  cout << "TTrimSPData::TTrimSPData: Read in " << fDataNZ.size() << " implantation profiles in total." << endl;

  fOrigDataNZ = fDataNZ;

  for(unsigned int i(0); i<fEnergy.size();i++)
    fIsNormalized.push_back(false);

  fEnergyIter = fEnergy.end();
}

void TTrimSPData::UseHighResolution(double e) {

  fEnergyIter = find(fEnergy.begin(), fEnergy.end(), e);

  if(fEnergyIter != fEnergy.end()) {
    unsigned int i(fEnergyIter - fEnergy.begin());
    vector<double> vecZ;
    vector<double> vecNZ;
    for(double zz(1.); zz<2100.; zz+=1.) {
      vecZ.push_back(zz);
      vecNZ.push_back(GetNofZ(zz/10.0, e));
    }
    fDataZ[i] = vecZ;
    fDataNZ[i] = vecNZ;
    fOrigDataNZ[i] = vecNZ;
    fDZ[i] = 1.;
    fIsNormalized[i] = false;
    return;
  }

  cout << "TTrimSPData::DataZ: No implantation profile available for the specified energy... Nothing happens." << endl;
  return;
}

//---------------------
// Method returning z-vector calculated by trim.SP for given energy[keV]
//---------------------

vector<double> TTrimSPData::DataZ(double e) const {

  fEnergyIter = find(fEnergy.begin(), fEnergy.end(), e);

  if(fEnergyIter != fEnergy.end()) {
    unsigned int i(fEnergyIter - fEnergy.begin());
    return fDataZ[i];
  }
  // default
  cout << "TTrimSPData::DataZ: No implantation profile available for the specified energy... You get back the first one." << endl;
  return fDataZ[0];
}

//---------------------
// Method returning actual n(z)-vector calculated by trim.SP and
// potentially altered by the WeightLayers- or the Normalize-method for given energy[keV]
//---------------------

vector<double> TTrimSPData::DataNZ(double e) const {

  fEnergyIter = find(fEnergy.begin(), fEnergy.end(), e);

  if(fEnergyIter != fEnergy.end()) {
    unsigned int i(fEnergyIter - fEnergy.begin());
    return fDataNZ[i];
  }
  // default
  cout << "TTrimSPData::DataNZ: No implantation profile available for the specified energy... You get back the first one." << endl;
  return fDataNZ[0];

}

//---------------------
// Method returning original n(z)-vector calculated by trim.SP for given energy[keV]
//---------------------

vector<double> TTrimSPData::OrigDataNZ(double e) const {

  fEnergyIter = find(fEnergy.begin(), fEnergy.end(), e);

  if(fEnergyIter != fEnergy.end()) {
    unsigned int i(fEnergyIter - fEnergy.begin());
    return fOrigDataNZ[i];
  }
  // default
  cout << "TTrimSPData::OrigDataNZ: No implantation profile available for the specified energy... You get back the first one." << endl;
  return fOrigDataNZ[0];

}

//---------------------
// Method returning fraction of muons implanted in the specified layer for a given energy[keV]
// Parameters: Energy[keV], LayerNumber[1], Interfaces[nm]
//---------------------

double TTrimSPData::LayerFraction(double e, unsigned int layno, const vector<double>& interface) const {

  if(layno < 1 && layno > (interface.size()+1)) {
    cout << "TTrimSPData::LayerFraction: No such layer available according to your specified interfaces... Returning 0.0!" << endl;
    return 0.0;
  }

  fEnergyIter = find(fEnergy.begin(), fEnergy.end(), e);

  if(fEnergyIter != fEnergy.end()) {
    unsigned int i(fEnergyIter - fEnergy.begin());
    // Because we do not know if the implantation profile is normalized or not, do not care about this and calculate the fraction from the beginning
    // Total "number of muons"
    double totalNumber(0.0);
    for(unsigned int j(0); j<fDataZ[i].size(); j++)
      totalNumber += fDataNZ[i][j];
    // "number of muons" in layer layno
    double layerNumber(0.0);
    if(!(layno-1)){
      for(unsigned int j(0); j<fDataZ[i].size(); j++)
        if(fDataZ[i][j] < interface[0]*10.0)
          layerNumber += fDataNZ[i][j];
    } else if(!(layno-interface.size()-1)){
      for(unsigned int j(0); j<fDataZ[i].size(); j++)
        if(fDataZ[i][j] >= *(interface.end()-1)*10.0)
          layerNumber += fDataNZ[i][j];
    } else {
      for(unsigned int j(0); j<fDataZ[i].size(); j++)
        if(fDataZ[i][j] >= interface[layno-2]*10.0 && fDataZ[i][j] < interface[layno-1]*10.0)
          layerNumber += fDataNZ[i][j];
    }
    // fraction of muons in layer layno
    // cout << "Fraction of muons in layer " << layno << ": " << layerNumber/totalNumber << endl;
    return layerNumber/totalNumber;
  }

  // default
  cout << "TTrimSPData::LayerFraction: No implantation profile available for the specified energy... Returning 0.0" << endl;
  return 0.0;

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
    cout << "TTrimSPData::WeightLayers: For the weighting the number of interfaces has to be one less than the number of weights!" << endl;
    cout << "TTrimSPData::WeightLayers: No weighting of the implantation profile will be done unless you take care of that!" << endl;
    return;
  }

  for(unsigned int i(0); i<interface.size(); i++) {
    if (interface[i]<0.0) {
      cout << "TTrimSPData::WeightLayers: One of your layer interfaces has a negative coordinate! - No weighting will be done!" << endl;
      return;
    }
    else if (i>1) {
      if (interface[i]<interface[i-1]) {
        cout << "TTrimSPData::WeightLayers: The specified interfaces appear to be not in ascending order! - No weighting will be done!" << endl;
        return;
      }
    }
  }

  for(unsigned int i(0); i<weight.size(); i++) {
    if (weight[i]>1.0 || weight[i]<0.0) {
      cout << "TTrimSPData::WeightLayers: At least one of the specified weights is out of range - no weighting will be done!" << endl;
      return;
    }
  }

  fEnergyIter = find(fEnergy.begin(), fEnergy.end(), e);

  // If all weights are equal to one, use the original n(z) vector
  for(unsigned int i(0); i<weight.size(); i++) {
    if(weight[i]-1.0)
      break;
    if(i == weight.size() - 1) {
      if(fEnergyIter != fEnergy.end()) {
        unsigned int j(fEnergyIter - fEnergy.begin());
        fDataNZ[j] = fOrigDataNZ[j];
        fIsNormalized[j] = false;
        return;
      }
    }
  }

  if(fEnergyIter != fEnergy.end()) {
    unsigned int i(fEnergyIter - fEnergy.begin());
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

  cout << "TTrimSPData::WeightLayers: No implantation profile available for the specified energy... No weighting done." << endl;
  return;
}

//---------------------
// Method returning n(z) for given z[nm] and energy[keV]
//---------------------

double TTrimSPData::GetNofZ(double zz, double e) const {

  vector<double> z, nz;

  fEnergyIter = find(fEnergy.begin(), fEnergy.end(), e);

  if(fEnergyIter != fEnergy.end()) {
    unsigned int i(fEnergyIter - fEnergy.begin());
    z = fDataZ[i];
    nz = fDataNZ[i];
  } else {
      cout << "TTrimSPData::GetNofZ: No implantation profile available for the specified energy... Quitting!" << endl;
      exit(-1);
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

  fEnergyIter = find(fEnergy.begin(), fEnergy.end(), e);

  if(fEnergyIter != fEnergy.end()) {
    unsigned int i(fEnergyIter - fEnergy.begin());
    double nZsum = 0.0;
    for (unsigned int j(0); j<fDataZ[i].size(); j++)
      nZsum += fDataNZ[i][j];
    nZsum *= fDZ[i];
    for (unsigned int j(0); j<fDataZ[i].size(); j++)
      fDataNZ[i][j] /= nZsum;

    fIsNormalized[i] = true;
    return;
  }
  // default
  cout << "TTrimSPData::Normalize: No implantation profile available for the specified energy... No normalization done." << endl;
  return;

}

//---------------------
// Method telling you if the n(z)-vector calculated by trim.SP for a given energy [keV] has been normalized
//---------------------

bool TTrimSPData::IsNormalized(double e) const {
  fEnergyIter = find(fEnergy.begin(), fEnergy.end(), e);

  if(fEnergyIter != fEnergy.end()) {
    unsigned int i(fEnergyIter - fEnergy.begin());
    return fIsNormalized[i];
  }

  cout << "TTrimSPData::IsNormalized: No implantation profile available for the specified energy... Returning false! Check your code!" << endl;
  return false;
}

//---------------------
// Calculate the mean range in (nm) for a given energy e
//---------------------

double TTrimSPData::MeanRange(double e) const {
  fEnergyIter = find(fEnergy.begin(), fEnergy.end(), e);

  if(fEnergyIter != fEnergy.end()) {
    unsigned int i(fEnergyIter - fEnergy.begin());
    if (!fIsNormalized[i])
      Normalize(e);
    double mean(0.0);
    for(unsigned int j(0); j<fDataNZ[i].size(); j++){
      mean += fDataNZ[i][j]*fDataZ[i][j];
    }
    mean *= fDZ[i]/10.0;
    return mean;
  }

  cout << "TTrimSPData::MeanRange: No implantation profile available for the specified energy... Returning -1! Check your code!" << endl;
  return -1.;
}

//---------------------
// Find the peak range in (nm) for a given energy e
//---------------------

double TTrimSPData::PeakRange(double e) const {

  fEnergyIter = find(fEnergy.begin(), fEnergy.end(), e);

  if(fEnergyIter != fEnergy.end()) {
    unsigned int i(fEnergyIter - fEnergy.begin());

    vector<double>::const_iterator nziter;
    nziter = max_element(fDataNZ[i].begin(),fDataNZ[i].end());

    if(nziter != fDataNZ[i].end()){
      unsigned int j(nziter - fDataNZ[i].begin());
      return fDataZ[i][j];
    }
    cout << "TTrimSPData::PeakRange: No maximum found in the implantation profile... Returning -1! Please check the profile!" << endl;
    return -1.;
  }

  cout << "TTrimSPData::PeakRange: No implantation profile available for the specified energy... Returning -1! Check your code!" << endl;
  return -1.;
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

  fEnergyIter = find(fEnergy.begin(), fEnergy.end(), e);

  if(fEnergyIter != fEnergy.end()) {
    unsigned int i(fEnergyIter - fEnergy.begin());
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

    fIsNormalized[i] = false;

    return;
  }

  cout << "TTrimSPData::ConvolveGss: No implantation profile available for the specified energy... No convolution done!" << endl;
  return;
}
