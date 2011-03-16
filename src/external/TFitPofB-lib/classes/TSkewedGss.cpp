/***************************************************************************

  TSkewedGss.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Bastian M. Wojek                                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "TSkewedGss.h"
#include <iostream>
#include <cassert>
using namespace std;

#include <TSAXParser.h>
#include "BMWStartupHandler.h"

ClassImp(TSkewedGss)

//------------------
// Destructor of the TSkewedGss class -- cleaning up
//------------------

TSkewedGss::~TSkewedGss() {
    fPar.clear();
    fParForPofB.clear();
    fParForPofT.clear();
    delete fPofB;
    fPofB = 0;
    delete fPofT;
    fPofT = 0;
}

//------------------
// Constructor of the TSkewedGss class -- creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TSkewedGss::TSkewedGss() : fCalcNeeded(true), fFirstCall(true) {

    // read startup file
    string startup_path_name("BMW_startup.xml");

    TSAXParser *saxParser = new TSAXParser();
    BMWStartupHandler *startupHandler = new BMWStartupHandler();
    saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
    int status (saxParser->ParseFile(startup_path_name.c_str()));
    // check for parse errors
    if (status) { // error
      cerr << endl << "**ERROR** reading/parsing " << startup_path_name << " failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
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

    fPofB = new TPofBCalc(fParForPofB);

    fPofT = new TPofTCalc(fPofB, fWisdom, fParForPofT);

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

      fPofB->Calculate("skg", fParForPofB);
      fPofT->DoFFT();

    }/* else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}

