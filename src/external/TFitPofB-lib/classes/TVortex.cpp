/***************************************************************************

  TVortex.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/10/17

***************************************************************************/

#include "TVortex.h"
#include <iostream>
#include <cassert>
#include <cmath>
using namespace std;

#include <TSAXParser.h>
#include "TFitPofBStartupHandler.h"

ClassImp(TBulkTriVortexLondon)

//------------------
// Destructor of the TBulkTriVortexLondon class -- cleaning up
//------------------

TBulkTriVortexLondon::~TBulkTriVortexLondon() {
    fPar.clear();
    fParForVortex.clear();
    fParForPofB.clear();
    fParForPofT.clear();
    delete fPofT;
    fPofT = 0;
}

//------------------
// Constructor of the TBulkTriVortexLondon class
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TBulkTriVortexLondon::TBulkTriVortexLondon() : fCalcNeeded(true), fFirstCall(true) {

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

    fGridSteps = startupHandler->GetGridSteps();
    fWisdom = startupHandler->GetWisdomFile();
    fVortexFourierComp = startupHandler->GetVortexFourierComp();

    fParForVortex.clear();

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    TPofTCalc *y = new TPofTCalc(fWisdom, fParForPofT);
    fPofT = y;
    y = 0;

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
// TBulkTriVortexLondon-Method that calls the procedures to create B(x,y), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TBulkTriVortexLondon (phase, appl.field, lambda, xi)
//------------------

double TBulkTriVortexLondon::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 4 || par.size() == 5);

  if(t<0.0)
    return cos(par[0]*0.017453293);

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(1); i < 4; i++){
      fParForVortex.push_back(fPar[i]);
    }
    fFirstCall = false;
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

  // if model parameters have changed, recalculate B(x,y), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

//      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      fParForVortex[0] = par[2]; // lambda
      fParForVortex[1] = par[3]; // xi
      fParForVortex[2] = par[1]; // field

      fParForPofB[2] = par[1]; // Bkg-Field
      //fParForPofB[3] = 0.005; // Bkg-width (in principle zero)

      TBulkTriVortexLondonFieldCalc vortexLattice(fParForVortex, fGridSteps, fVortexFourierComp);
      TPofBCalc PofB(vortexLattice, fParForPofB);
      fPofT->DoFFT(PofB);

    }/* else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}
