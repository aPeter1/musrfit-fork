/***************************************************************************

  TSkewedGss.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/01/24

***************************************************************************/

#include "TSkewedGss.h"
#include <iostream>
#include <cassert>
using namespace std;

#include <TSAXParser.h>
#include "TFitPofBStartupHandler.h"

ClassImp(TSkewedGss)

//------------------
// Destructor of the TSkewedGss class -- cleaning up
//------------------

TSkewedGss::~TSkewedGss() {
    fPar.clear();
    fParForPofB.clear();
    fParForPofT.clear();
    delete fPofT;
    fPofT = 0;
}

//------------------
// Constructor of the TSkewedGss class -- creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TSkewedGss::TSkewedGss() : fCalcNeeded(true), fFirstCall(true) {

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

    fWisdom = startupHandler->GetWisdomFile();

    fParForPofT.push_back(0.0); // phase
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());
    fParForPofB.push_back(0.0); // nu0
    fParForPofB.push_back(0.0); // s-
    fParForPofB.push_back(0.0); // s+

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
// TSkewedGss-Method that calls the procedures to create p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TSkewedGss (phase,freq0,sigma-,sigma+)
//------------------

double TSkewedGss::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 4);

  if(t<0.0)
    return 1.0;

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;
    fFirstCall=false;
  }

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

  // if model parameters have changed, recalculate P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

//      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      fParForPofB[2] = par[1]; // nu0
      fParForPofB[3] = par[2]; // sigma-
      fParForPofB[4] = par[3]; // sigma+

      TPofBCalc PofB("skg", fParForPofB);
      fPofT->DoFFT(PofB);

    }/* else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}

