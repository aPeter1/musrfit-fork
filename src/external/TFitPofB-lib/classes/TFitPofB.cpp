/***************************************************************************

  TFitPofB.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/23

***************************************************************************/

#include "TFitPofB.h"
#include <iostream>
using namespace std;

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

TFitPofB::~TFitPofB() {
    fPar.clear();
    fImpProfile = 0;
    delete fImpProfile;
    fPofT = 0;
    delete fPofT;
}

double TFitPofB::Eval(double t, const vector<double> &par) const {

  // check if any parameter has changed

  bool par_changed(false);

  for (unsigned int i(0); i<fPar.size(); i++) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      par_changed = true;
      if(i == 2 || i == 3) {
        cout << "You are varying dt or dB! These parameters have to be fixed! Quitting..." << endl; 
        exit(0);
      }
    }
  }

  if (par_changed)
    fCalcNeeded = true;
    
  // if parameters have changed, recalculate B(z), P(B) and P(t)
    
  if (fCalcNeeded) {

    vector<double> par_for_BofZ;
    vector<double> par_for_PofB;
    vector<double> par_for_PofT;

    for (unsigned int i(5); i<par.size(); i++)
      par_for_BofZ.push_back(par[i]);

    for (unsigned int i(2); i<5; i++)
      par_for_PofB.push_back(par[i]);
      
    for (unsigned int i(1); i<4; i++)
      par_for_PofT.push_back(par[i]);

    switch (int(par[0])) {
      case 1:
      {
        TLondon1D_1L BofZ1(par_for_BofZ);
        TPofBCalc PofB1(BofZ1, *fImpProfile, par_for_PofB);
        fPofT->DoFFT(PofB1, par_for_PofT);
      }
        break;
      case 2:
      {
        TLondon1D_2L BofZ2(par_for_BofZ);
        TPofBCalc PofB2(BofZ2, *fImpProfile, par_for_PofB);
        fPofT->DoFFT(PofB2, par_for_PofT);
      }
        break;
      case 3:
      {
        TLondon1D_3L BofZ3(par_for_BofZ);
        TPofBCalc PofB3(BofZ3, *fImpProfile, par_for_PofB);
        fPofT->DoFFT(PofB3, par_for_PofT);
      }
        break;
      default:
        cout << "The user function you specified, does not exist. Quitting..." << endl;
        exit(0);
      }

  }
  
  return fPofT->Eval(t);
  
}

