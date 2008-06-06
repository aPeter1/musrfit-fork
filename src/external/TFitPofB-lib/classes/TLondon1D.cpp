/***************************************************************************

  TLondon1D.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/06/06

***************************************************************************/

#include "TLondon1D.h"
#include <iostream>
using namespace std;

#include <TSAXParser.h>
#include "TFitPofBStartupHandler.h"

ClassImp(TLondon1D)
ClassImp(TLondon1D1L)
ClassImp(TLondon1D2L)
ClassImp(TLondon1D3LS)

//------------------
// Destructor of the TLondon1D class -- cleaning up
//------------------

TLondon1D::~TLondon1D() {
    fPar.clear();
    fParForBofZ.clear();
    fParForPofB.clear();
    fParForPofT.clear();
    delete fImpProfile;
    fImpProfile = 0;
    delete fPofT;
    fPofT = 0;
}

//------------------
// Constructor of the TLondon1D1L class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TLondon1D1L::TLondon1D1L(const vector<double> &par) {

  // extract function parameters
  //  for(unsigned int i(0); i<parNo.size(); i++) {
  //    fPar.push_back(par[parNo[i]-1]);
  //  }

  fPar = par;

  // read startup file
  string startup_path_name("TFitPofB_startup.xml");

  TSAXParser *saxParser = new TSAXParser();
  TFitPofBStartupHandler *startupHandler = new TFitPofBStartupHandler();
  saxParser->ConnectToHandler("TFitPofBStartupHandler", startupHandler);
  int status (saxParser->ParseFile(startup_path_name.c_str()));
  // check for parse errors
  if (status) { // error
    cout << endl << "**WARNING** reading/parsing TFitPofB_startup.xml failed." << endl;
  }

  fNSteps = startupHandler->GetNSteps();
  fWisdom = startupHandler->GetWisdomFile();
  string rge_path(startupHandler->GetDataPath());
  vector<string> energy_vec(startupHandler->GetEnergyList());

  fParForPofT.push_back(fPar[0]);
  fParForPofT.push_back(startupHandler->GetDeltat());
  fParForPofT.push_back(startupHandler->GetDeltaB());

  for (unsigned int i(2); i<fPar.size(); i++)
    fParForBofZ.push_back(fPar[i]);

  fParForPofB.push_back(startupHandler->GetDeltat());
  fParForPofB.push_back(startupHandler->GetDeltaB());
  fParForPofB.push_back(fPar[1]);

  TTrimSPData *x = new TTrimSPData(rge_path, energy_vec);
  fImpProfile = x;
  x = 0;
  delete x;

  TPofTCalc *y = new TPofTCalc(fWisdom, fParForPofT);
  fPofT = y;
  y = 0;
  delete y;


  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }
}

//------------------
// TLondon1D1L-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TLondon1D1L
//------------------

double TLondon1D1L::operator()(double t, const vector<double> &par) const {

  // check if any parameter has changed

  bool par_changed(false);
  bool only_phase_changed(false);

  for (unsigned int i(0); i<fPar.size(); i++) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      par_changed = true;
      if (i == 0) {
        only_phase_changed = true;
      } else {
        only_phase_changed = false;
      }
    }
  }

  if (par_changed)
    fCalcNeeded = true;

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      for (unsigned int i(2); i<par.size(); i++)
        fParForBofZ[i-2] = par[i];

      fParForPofB[2] = par[1]; // energy

      TLondon1D_1L BofZ1(fNSteps, fParForBofZ);
      TPofBCalc PofB1(BofZ1, *fImpProfile, fParForPofB);
      fPofT->DoFFT(PofB1);

    } else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}

//------------------
// Constructor of the TLondon1D2L class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TLondon1D2L::TLondon1D2L(const vector<double> &par) : fLastTwoChanged(true) {

// extract function parameters
//  for(unsigned int i(0); i<parNo.size(); i++) {
//    fPar.push_back(par[parNo[i]-1]);
//  }

  fPar = par;

  // read startup file
  string startup_path_name("TFitPofB_startup.xml");

  TSAXParser *saxParser = new TSAXParser();
  TFitPofBStartupHandler *startupHandler = new TFitPofBStartupHandler();
  saxParser->ConnectToHandler("TFitPofBStartupHandler", startupHandler);
  int status (saxParser->ParseFile(startup_path_name.c_str()));
  // check for parse errors
  if (status) { // error
    cout << endl << "**WARNING** reading/parsing TFitPofB_startup.xml failed." << endl;
  }

  fNSteps = startupHandler->GetNSteps();
  fWisdom = startupHandler->GetWisdomFile();
  string rge_path(startupHandler->GetDataPath());
  vector<string> energy_vec(startupHandler->GetEnergyList());

  fParForPofT.push_back(fPar[0]);
  fParForPofT.push_back(startupHandler->GetDeltat());
  fParForPofT.push_back(startupHandler->GetDeltaB());

  for (unsigned int i(2); i<fPar.size(); i++)
    fParForBofZ.push_back(fPar[i]);

  fParForPofB.push_back(startupHandler->GetDeltat());
  fParForPofB.push_back(startupHandler->GetDeltaB());
  fParForPofB.push_back(fPar[1]);

  TTrimSPData *x = new TTrimSPData(rge_path, energy_vec);
  fImpProfile = x;
  x = 0;
  delete x;

  TPofTCalc *y = new TPofTCalc(fWisdom, fParForPofT);
  fPofT = y;
  y = 0;
  delete y;

  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }
}

//------------------
// TLondon1D2L-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TLondon1D2L
//------------------

double TLondon1D2L::operator()(double t, const vector<double> &par) const {

  // check if any parameter has changed

  bool par_changed(false);
  bool only_phase_changed(false);

  for (unsigned int i(0); i<fPar.size(); i++) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      par_changed = true;
      if (i == 0) {
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

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      for (unsigned int i(2); i<par.size(); i++)
        fParForBofZ[i-2] = par[i];

      fParForPofB[2] = par[1]; // energy

      if(fLastTwoChanged) {
        vector<double> interfaces;
        interfaces.push_back(par[3]+par[4]);

        vector<double> weights;
        for(unsigned int i(par.size()-2); i<par.size(); i++)
          weights.push_back(par[i]);

        cout << "Weighting has changed, re-calculating n(z) now..." << endl;
        fImpProfile->WeightLayers(par[1], interfaces, weights);
      }

      TLondon1D_2L BofZ2(fNSteps, fParForBofZ);
      TPofBCalc PofB2(BofZ2, *fImpProfile, fParForPofB);
      fPofT->DoFFT(PofB2);

    } else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
    fLastTwoChanged = false;
  }

  return fPofT->Eval(t);

}

//------------------
// Constructor of the TLondon1D3L class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TLondon1D3L::TLondon1D3L(const vector<double> &par) : fLastThreeChanged(true) {

  // extract function parameters
  // for(unsigned int i(0); i<parNo.size(); i++) {
  //   fPar.push_back(par[parNo[i]-1]);
  // }

  fPar = par;

  // read startup file
  string startup_path_name("TFitPofB_startup.xml");

  TSAXParser *saxParser = new TSAXParser();
  TFitPofBStartupHandler *startupHandler = new TFitPofBStartupHandler();
  saxParser->ConnectToHandler("TFitPofBStartupHandler", startupHandler);
  int status (saxParser->ParseFile(startup_path_name.c_str()));
  // check for parse errors
  if (status) { // error
    cout << endl << "**WARNING** reading/parsing TFitPofB_startup.xml failed." << endl;
  }

  fNSteps = startupHandler->GetNSteps();
  fWisdom = startupHandler->GetWisdomFile();
  string rge_path(startupHandler->GetDataPath());
  vector<string> energy_vec(startupHandler->GetEnergyList());

  fParForPofT.push_back(fPar[0]);
  fParForPofT.push_back(startupHandler->GetDeltat());
  fParForPofT.push_back(startupHandler->GetDeltaB());

  for (unsigned int i(2); i<fPar.size(); i++)
    fParForBofZ.push_back(fPar[i]);

  fParForPofB.push_back(startupHandler->GetDeltat());
  fParForPofB.push_back(startupHandler->GetDeltaB());
  fParForPofB.push_back(fPar[1]);

  TTrimSPData *x = new TTrimSPData(rge_path, energy_vec);
  fImpProfile = x;
  x = 0;
  delete x;

  TPofTCalc *y = new TPofTCalc(fWisdom, fParForPofT);
  fPofT = y;
  y = 0;
  delete y;

  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }

}

//------------------
// TLondon1D3L-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TLondon1D3L
//------------------

double TLondon1D3L::operator()(double t, const vector<double> &par) const {

  // check if any parameter has changed

  bool par_changed(false);
  bool only_phase_changed(false);

  for (unsigned int i(0); i<fPar.size(); i++) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      par_changed = true;
      if (i == 0) {
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

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      for (unsigned int i(2); i<par.size(); i++)
        fParForBofZ[i-2] = par[i];

      fParForPofB[2] = par[1]; // energy

/* DEBUG ---------------------------
      for(unsigned int i(0); i<fParForBofZ.size(); i++) {
        cout << "ParForBofZ[" << i << "] = " << fParForBofZ[i] << endl;
      }

      for(unsigned int i(0); i<fParForPofB.size(); i++) {
        cout << "ParForPofB[" << i << "] = " << fParForPofB[i] << endl;
      }

      for(unsigned int i(0); i<fParForPofT.size(); i++) {
        cout << "ParForPofT[" << i << "] = " << fParForPofT[i] << endl;
      }
------------------------------------*/

      if(fLastThreeChanged) {
        vector<double> interfaces;
        interfaces.push_back(par[3]+par[4]);
        interfaces.push_back(par[3]+par[4]+par[5]);

        vector<double> weights;
        for(unsigned int i(par.size()-3); i<par.size(); i++)
          weights.push_back(par[i]);

        cout << "Weighting has changed, re-calculating n(z) now..." << endl;
        fImpProfile->WeightLayers(par[1], interfaces, weights);
      }

      TLondon1D_3L BofZ3(fNSteps, fParForBofZ);
      TPofBCalc PofB3(BofZ3, *fImpProfile, fParForPofB);
      fPofT->DoFFT(PofB3);

    } else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
    fLastThreeChanged = false;
  }

  return fPofT->Eval(t);

}

//------------------
// Constructor of the TLondon1D3LS class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TLondon1D3LS::TLondon1D3LS(const vector<double> &par) : fLastThreeChanged(true) {

  // extract function parameters
  // for(unsigned int i(0); i<parNo.size(); i++) {
  //   fPar.push_back(par[parNo[i]-1]);
  // }

  fPar = par;

  // read startup file
  string startup_path_name("TFitPofB_startup.xml");

  TSAXParser *saxParser = new TSAXParser();
  TFitPofBStartupHandler *startupHandler = new TFitPofBStartupHandler();
  saxParser->ConnectToHandler("TFitPofBStartupHandler", startupHandler);
  int status (saxParser->ParseFile(startup_path_name.c_str()));
  // check for parse errors
  if (status) { // error
    cout << endl << "**WARNING** reading/parsing TFitPofB_startup.xml failed." << endl;
  }

  fNSteps = startupHandler->GetNSteps();
  fWisdom = startupHandler->GetWisdomFile();
  string rge_path(startupHandler->GetDataPath());
  vector<string> energy_vec(startupHandler->GetEnergyList());

  fParForPofT.push_back(fPar[0]);
  fParForPofT.push_back(startupHandler->GetDeltat());
  fParForPofT.push_back(startupHandler->GetDeltaB());

  for (unsigned int i(2); i<fPar.size(); i++)
    fParForBofZ.push_back(fPar[i]);

  fParForPofB.push_back(startupHandler->GetDeltat());
  fParForPofB.push_back(startupHandler->GetDeltaB());
  fParForPofB.push_back(fPar[1]);

  TTrimSPData *x = new TTrimSPData(rge_path, energy_vec);
  fImpProfile = x;
  x = 0;
  delete x;

  TPofTCalc *y = new TPofTCalc(fWisdom, fParForPofT);
  fPofT = y;
  y = 0;
  delete y;

  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }

}

//------------------
// TLondon1D3LS-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TLondon1D3LS
//------------------

double TLondon1D3LS::operator()(double t, const vector<double> &par) const {

  // check if any parameter has changed

  bool par_changed(false);
  bool only_phase_changed(false);

  for (unsigned int i(0); i<fPar.size(); i++) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      par_changed = true;
      if (i == 0) {
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

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      for (unsigned int i(2); i<par.size(); i++)
        fParForBofZ[i-2] = par[i];

      fParForPofB[2] = par[1]; // energy

      if(fLastThreeChanged) {
        vector<double> interfaces;
        interfaces.push_back(par[3]+par[4]);
        interfaces.push_back(par[3]+par[4]+par[5]);

        vector<double> weights;
        for(unsigned int i(par.size()-3); i<par.size(); i++)
          weights.push_back(par[i]);

        cout << "Weighting has changed, re-calculating n(z) now..." << endl;
        fImpProfile->WeightLayers(par[1], interfaces, weights);
      }

      TLondon1D_3LS BofZ3S(fNSteps, fParForBofZ);
      TPofBCalc PofB3S(BofZ3S, *fImpProfile, fParForPofB);
      fPofT->DoFFT(PofB3S);

    } else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
    fLastThreeChanged = false;
  }

  return fPofT->Eval(t);

}
