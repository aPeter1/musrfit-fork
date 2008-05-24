/***************************************************************************

  TFitPofB.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/24

***************************************************************************/

#include "TFitPofB.h"
#include <iostream>
using namespace std;

//------------------
// Constructor of the TFitPofB class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TFitPofB::TFitPofB(const vector<unsigned int> &parNo, const vector<double> &par) : fCalcNeeded(true) {

  for(unsigned int i(0); i<parNo.size(); i++) {
    fPar.push_back(par[parNo[i]-1]);
  }

  string rge_path("/home/l_wojek/nt/wojek/g/Bastian/ImplantationDepth/YBCO_PBCO-");
  string energy_arr[] = {"02_1", "02_5", "03_5", "05_0", "07_5", "10_0", "12_5", "15_0", "17_5", "19_0", "20_0", "22_5", "25_0"};

  vector<string> energy_vec(energy_arr, energy_arr+(sizeof(energy_arr)/sizeof(energy_arr[0])));

  vector<double> par_for_PofT;

  for (unsigned int i(1); i<4; i++)
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
// Destructor of the TFitPofB class -- cleaning up
//------------------

TFitPofB::~TFitPofB() {
    fPar.clear();
    delete fImpProfile;
    fImpProfile = 0;
    delete fPofT;
    fPofT = 0;
}

//------------------
// Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TFitPofB
//------------------

double TFitPofB::Eval(double t, const vector<double> &par) const {

  // check if any parameter has changed

  bool par_changed(false);
  bool only_phase_changed(false);

  for (unsigned int i(0); i<fPar.size(); i++) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      par_changed = true;
      if(i == 0 || i == 2 || i == 3 || i == 4) {
        cout << "You are varying the model-flag, dt, dB or E! These parameters have to be fixed! Quitting..." << endl; 
        exit(0);
      } else if (i == 1) {
        only_phase_changed = true;
      } else {
        only_phase_changed = false;
      }
    }
  }

  if (par_changed)
    fCalcNeeded = true;

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

/* DEBUGGING CODE COMMENTED -- quite a mess... sorry*/

  if (fCalcNeeded) {

    vector<double> par_for_PofT;

//    cout << "par_for_PofT: ";

    for (unsigned int i(1); i<4; i++) {
      par_for_PofT.push_back(par[i]);
//      cout << par[i] << " ";
    }
//    cout << endl;

    if(!only_phase_changed) {

      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      vector<double> par_for_BofZ;
      vector<double> par_for_PofB;

//      cout << "par_for_BofZ: ";

      for (unsigned int i(5); i<par.size(); i++) {
        par_for_BofZ.push_back(par[i]);
//        cout << par[i] << " ";
      }
//      cout << endl;

//      cout << "par_for_PofB: ";

      for (unsigned int i(2); i<5; i++) {
        par_for_PofB.push_back(par[i]);
//        cout << par[i] << " ";
      }
//      cout << endl;

      switch (int(par[0])) {
        case 1:
        {
//          cout << "Found the 1D-London model." << endl;
          TLondon1D_1L BofZ1(par_for_BofZ);
          TPofBCalc PofB1(BofZ1, *fImpProfile, par_for_PofB);
          fPofT->DoFFT(PofB1);
        }
          break;
        case 2:
        {
//          cout << "Found the 1D-London model.2L" << endl;
          TLondon1D_2L BofZ2(par_for_BofZ);
          TPofBCalc PofB2(BofZ2, *fImpProfile, par_for_PofB);
          fPofT->DoFFT(PofB2);
        }
          break;
        case 3:
        {
//          cout << "Found the 1D-London model.3L" << endl;
          TLondon1D_3L BofZ3(par_for_BofZ);
          TPofBCalc PofB3(BofZ3, *fImpProfile, par_for_PofB);
          fPofT->DoFFT(PofB3);
        }
          break;
        default:
          cout << "The user function you specified with the first parameter, does not exist. Quitting..." << endl;
          exit(0);
      }
    } else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }

    fPofT->CalcPol(par_for_PofT);

    fCalcNeeded = false;

  }

  return fPofT->Eval(t);

}

