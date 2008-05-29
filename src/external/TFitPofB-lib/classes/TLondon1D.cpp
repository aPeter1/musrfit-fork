/***************************************************************************

  TLondon1D.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/29

***************************************************************************/

#include "TLondon1D.h"
#include <iostream>
using namespace std;

ClassImp(TLondon1D)
ClassImp(TLondon1D1L)
ClassImp(TLondon1D2L)
ClassImp(TLondon1D3LS)

//------------------
// Destructor of the TLondon1D class -- cleaning up
//------------------

TLondon1D::~TLondon1D() {
    fPar.clear();
    delete fImpProfile;
    fImpProfile = 0;
    delete fPofT;
    fPofT = 0;
}

//------------------
// Constructor of the TLondon1D1L class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TLondon1D1L::TLondon1D1L(const vector<unsigned int> &parNo, const vector<double> &par) {

  for(unsigned int i(0); i<parNo.size(); i++) {
    fPar.push_back(par[parNo[i]-1]);
  }

//  string rge_path("/home/l_wojek/nt/wojek/g/Bastian/ImplantationDepth/YBCO_PBCO-");
//  string energy_arr[] = {"02_1", "02_5", "03_5", "05_0", "07_5", "10_0", "12_5", "15_0", "17_5", "19_0", "20_0", "22_5", "25_0"};

  string rge_path("/home/l_wojek/TrimSP/AuYBCO_2005/AuYBCO-500000-");
  string energy_arr[] = {"04_6", "09_6", "14_6", "18_6", "21_6", "24_6", "28_7"};

  vector<string> energy_vec(energy_arr, energy_arr+(sizeof(energy_arr)/sizeof(energy_arr[0])));

  vector<double> par_for_PofT;

  for (unsigned int i(0); i<3; i++)
    par_for_PofT.push_back(fPar[i]);

  TTrimSPData *x = new TTrimSPData(rge_path, energy_vec);
  fImpProfile = x;
  x = 0;
  delete x;

  TPofTCalc *y = new TPofTCalc(par_for_PofT);
  fPofT = y;
  y = 0;
  delete y;

}

//------------------
// TLondon1D1L-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TLondon1D1L
//------------------

double TLondon1D1L::Eval(double t, const vector<double> &par) const {

  // check if any parameter has changed

  bool par_changed(false);
  bool only_phase_changed(false);

  for (unsigned int i(0); i<fPar.size(); i++) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      par_changed = true;
      if(i == 1 || i == 2) {
        cout << "You are varying dt or dB! These parameters have to be fixed! Quitting..." << endl; 
        exit(-1);
      } else if (i == 0) {
        only_phase_changed = true;
      } else {
        only_phase_changed = false;
      }
    }
  }

  if (par_changed)
    fCalcNeeded = true;

/* DEBUGGING CODE COMMENTED -- quite a mess... sorry*/

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    vector<double> par_for_PofT;

//    cout << "par_for_PofT: ";

    for (unsigned int i(0); i<3; i++) {
      par_for_PofT.push_back(par[i]);
//      cout << par[i] << " ";
    }
//    cout << endl;

    if(!only_phase_changed) {

      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      vector<double> par_for_BofZ;
      vector<double> par_for_PofB;

//      cout << "par_for_BofZ: ";

      for (unsigned int i(4); i<par.size(); i++) {
        par_for_BofZ.push_back(par[i]);
//        cout << par[i] << " ";
      }
//      cout << endl;

//      cout << "par_for_PofB: ";

      for (unsigned int i(1); i<4; i++) {
        par_for_PofB.push_back(par[i]);
//        cout << par[i] << " ";
      }
//      cout << endl;

      TLondon1D_1L BofZ1(par_for_BofZ);
      TPofBCalc PofB1(BofZ1, *fImpProfile, par_for_PofB);
      fPofT->DoFFT(PofB1);

    } else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }

    fPofT->CalcPol(par_for_PofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}

//------------------
// Constructor of the TLondon1D2L class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TLondon1D2L::TLondon1D2L(const vector<unsigned int> &parNo, const vector<double> &par) : fLastTwoChanged(true) {

  for(unsigned int i(0); i<parNo.size(); i++) {
    fPar.push_back(par[parNo[i]-1]);
  }

//  string rge_path("/home/l_wojek/nt/wojek/g/Bastian/ImplantationDepth/YBCO_PBCO-");
//  string energy_arr[] = {"02_1", "02_5", "03_5", "05_0", "07_5", "10_0", "12_5", "15_0", "17_5", "19_0", "20_0", "22_5", "25_0"};

  string rge_path("/home/l_wojek/TrimSP/AuYBCO_2005/AuYBCO-500000-");
  string energy_arr[] = {"04_6", "09_6", "14_6", "18_6", "21_6", "24_6", "28_7"};

  vector<string> energy_vec(energy_arr, energy_arr+(sizeof(energy_arr)/sizeof(energy_arr[0])));

  vector<double> par_for_PofT;

  for (unsigned int i(0); i<3; i++)
    par_for_PofT.push_back(fPar[i]);

  TTrimSPData *x = new TTrimSPData(rge_path, energy_vec);
  fImpProfile = x;
  x = 0;
  delete x;

  TPofTCalc *y = new TPofTCalc(par_for_PofT);
  fPofT = y;
  y = 0;
  delete y;

}

//------------------
// TLondon1D2L-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TLondon1D1L
//------------------

double TLondon1D2L::Eval(double t, const vector<double> &par) const {

  // check if any parameter has changed

  bool par_changed(false);
  bool only_phase_changed(false);

  for (unsigned int i(0); i<fPar.size(); i++) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      par_changed = true;
      if(i == 1 || i == 2) {
        cout << "You are varying dt or dB! These parameters have to be fixed! Quitting..." << endl; 
        exit(-1);
      } else if (i == 0) {
        only_phase_changed = true;
      } else {
        only_phase_changed = false;
      }
      if (i == fPar.size()-2 || i == fPar.size()-1)
        fLastTwoChanged = true;
    }
  }

  if (par_changed)
    fCalcNeeded = true;

/* DEBUGGING CODE COMMENTED -- quite a mess... sorry*/

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    vector<double> par_for_PofT;

//    cout << "par_for_PofT: ";

    for (unsigned int i(0); i<3; i++) {
      par_for_PofT.push_back(par[i]);
//      cout << par[i] << " ";
    }
//    cout << endl;

    if(!only_phase_changed) {

      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      vector<double> par_for_BofZ;
      vector<double> par_for_PofB;

//      cout << "par_for_BofZ: ";

      for (unsigned int i(4); i<par.size()-2; i++) {
        par_for_BofZ.push_back(par[i]);
//        cout << par[i] << " ";
      }
//      cout << endl;

//      cout << "par_for_PofB: ";

      for (unsigned int i(1); i<4; i++) {
        par_for_PofB.push_back(par[i]);
//        cout << par[i] << " ";
      }
//      cout << endl;

      if(fLastTwoChanged) {
        vector<double> interfaces;
        interfaces.push_back(par[5]+par[6]);

        vector<double> weights;
        for(unsigned int i(par.size()-2); i<par.size(); i++)
          weights.push_back(par[i]);

        cout << "Weighting has changed, re-calculating n(z) now..." << endl;
        fImpProfile->WeightLayers(par[3], interfaces, weights);
      }

      TLondon1D_2L BofZ2(par_for_BofZ);
      TPofBCalc PofB2(BofZ2, *fImpProfile, par_for_PofB);
      fPofT->DoFFT(PofB2);

    } else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }

    fPofT->CalcPol(par_for_PofT);

    fCalcNeeded = false;
    fLastTwoChanged = false;
  }

  return fPofT->Eval(t);

}

//------------------
// Constructor of the TLondon1D3LS class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TLondon1D3LS::TLondon1D3LS(const vector<unsigned int> &parNo, const vector<double> &par) : fLastThreeChanged(true) {

  for(unsigned int i(0); i<parNo.size(); i++) {
    fPar.push_back(par[parNo[i]-1]);
  }

//  string rge_path("/home/l_wojek/nt/wojek/g/Bastian/ImplantationDepth/YBCO_PBCO-");
//  string energy_arr[] = {"02_1", "02_5", "03_5", "05_0", "07_5", "10_0", "12_5", "15_0", "17_5", "19_0", "20_0", "22_5", "25_0"};

  string rge_path("/home/l_wojek/TrimSP/AuYBCO_2005/AuYBCO-500000-");
  string energy_arr[] = {"04_6", "09_6", "14_6", "18_6", "21_6", "24_6", "28_7"};

  vector<string> energy_vec(energy_arr, energy_arr+(sizeof(energy_arr)/sizeof(energy_arr[0])));

  vector<double> par_for_PofT;

  for (unsigned int i(0); i<3; i++)
    par_for_PofT.push_back(fPar[i]);

  TTrimSPData *x = new TTrimSPData(rge_path, energy_vec);
  fImpProfile = x;
  x = 0;
  delete x;

  TPofTCalc *y = new TPofTCalc(par_for_PofT);
  fPofT = y;
  y = 0;
  delete y;

}

//------------------
// TLondon1D3LS-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TLondon1D1L
//------------------

double TLondon1D3LS::Eval(double t, const vector<double> &par) const {

  // check if any parameter has changed

  bool par_changed(false);
  bool only_phase_changed(false);

  for (unsigned int i(0); i<fPar.size(); i++) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      par_changed = true;
      if(i == 1 || i == 2) {
        cout << "You are varying dt or dB! These parameters have to be fixed! Quitting..." << endl; 
        exit(-1);
      } else if (i == 0) {
        only_phase_changed = true;
      } else {
        only_phase_changed = false;
      }
      if (i == fPar.size()-3 || i == fPar.size()-2 || i == fPar.size()-1)
        fLastThreeChanged = true;
    }
  }

  if (par_changed)
    fCalcNeeded = true;

/* DEBUGGING CODE COMMENTED -- quite a mess... sorry*/

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    vector<double> par_for_PofT;

//    cout << "par_for_PofT: ";

    for (unsigned int i(0); i<3; i++) {
      par_for_PofT.push_back(par[i]);
//      cout << par[i] << " ";
    }
//    cout << endl;

    if(!only_phase_changed) {

      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      vector<double> par_for_BofZ;
      vector<double> par_for_PofB;

//      cout << "par_for_BofZ: ";

      for (unsigned int i(4); i<par.size()-2; i++) {
        par_for_BofZ.push_back(par[i]);
//        cout << par[i] << " ";
      }
//      cout << endl;

//      cout << "par_for_PofB: ";

      for (unsigned int i(1); i<4; i++) {
        par_for_PofB.push_back(par[i]);
//        cout << par[i] << " ";
      }
//      cout << endl;

      if(fLastThreeChanged) {
        vector<double> interfaces;
        interfaces.push_back(par[5]+par[6]);
        interfaces.push_back(par[5]+par[6]+par[7]);

        vector<double> weights;
        for(unsigned int i(par.size()-3); i<par.size(); i++)
          weights.push_back(par[i]);

        cout << "Weighting has changed, re-calculating n(z) now..." << endl;
        fImpProfile->WeightLayers(par[3], interfaces, weights);
      }

      TLondon1D_3L BofZ3(par_for_BofZ);
      TPofBCalc PofB3(BofZ3, *fImpProfile, par_for_PofB);
      fPofT->DoFFT(PofB3);

    } else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }

    fPofT->CalcPol(par_for_PofT);

    fCalcNeeded = false;
    fLastThreeChanged = false;
  }

  return fPofT->Eval(t);

}
