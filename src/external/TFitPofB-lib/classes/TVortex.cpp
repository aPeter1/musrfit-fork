/***************************************************************************

  TVortex.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/10/17

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

#include "TVortex.h"
#include <iostream>
#include <cassert>
#include <cmath>
using namespace std;

#include <TSAXParser.h>
#include "TFitPofBStartupHandler.h"

ClassImp(TBulkTriVortexLondon)
ClassImp(TBulkTriVortexML)
ClassImp(TBulkTriVortexAGL)
ClassImp(TBulkTriVortexNGL)

//------------------
// Destructor of the TBulkTriVortexLondon class -- cleaning up
//------------------

TBulkTriVortexLondon::~TBulkTriVortexLondon() {
    delete fPofT;
    fPofT = 0;
    delete fPofB;
    fPofT = 0;
    delete fVortex;
    fVortex = 0;
    fPar.clear();
    fParForVortex.clear();
    fParForPofB.clear();
    fParForPofT.clear();
}

//------------------
// Destructor of the TBulkTriVortexML class -- cleaning up
//------------------

TBulkTriVortexML::~TBulkTriVortexML() {
    delete fPofT;
    fPofT = 0;
    delete fPofB;
    fPofT = 0;
    delete fVortex;
    fVortex = 0;
    fPar.clear();
    fParForVortex.clear();
    fParForPofB.clear();
    fParForPofT.clear();
}

//------------------
// Destructor of the TBulkTriVortexAGL class -- cleaning up
//------------------

TBulkTriVortexAGL::~TBulkTriVortexAGL() {
    delete fPofT;
    fPofT = 0;
    delete fPofB;
    fPofT = 0;
    delete fVortex;
    fVortex = 0;
    fPar.clear();
    fParForVortex.clear();
    fParForPofB.clear();
    fParForPofT.clear();
}

//------------------
// Destructor of the TBulkTriVortexNGL class -- cleaning up
//------------------

TBulkTriVortexNGL::~TBulkTriVortexNGL() {
    delete fPofT;
    fPofT = 0;
    delete fPofB;
    fPofT = 0;
    delete fVortex;
    fVortex = 0;
    fPar.clear();
    fParForVortex.clear();
    fParForPofB.clear();
    fParForPofT.clear();
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

    fParForVortex.resize(3); // field, lambda, xi

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    TBulkTriVortexLondonFieldCalc *x = new TBulkTriVortexLondonFieldCalc(fWisdom, fGridSteps);
    fVortex = x;
    x = 0;

    TPofBCalc *y = new TPofBCalc(fParForPofB);
    fPofB = y;
    y = 0;

    TPofTCalc *z = new TPofTCalc(fPofB, fWisdom, fParForPofT);
    fPofT = z;
    z = 0;

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
// Parameters: all the parameters for the function to be fitted through TBulkTriVortexLondon (phase, av.field, lambda, xi, [not implemented: bkg weight])
//------------------

double TBulkTriVortexLondon::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 4 || par.size() == 5);

  if(t<0.0)
    return cos(par[0]*0.017453293);

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(0); i < 3; i++) {
      fParForVortex[i] = fPar[i+1];
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

      for (unsigned int i(0); i < 3; i++) {
        fParForVortex[i] = par[i+1];
      }

      fParForPofB[2] = par[1]; // Bkg-Field
      //fParForPofB[3] = 0.005; // Bkg-width (in principle zero)

      fVortex->SetParameters(fParForVortex);
      fVortex->CalculateGrid();
      fPofB->UnsetPBExists();
      fPofB->Calculate(fVortex, fParForPofB);
      fPofT->DoFFT();

    }/* else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}

//------------------
// Constructor of the TBulkTriVortexML class
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TBulkTriVortexML::TBulkTriVortexML() : fCalcNeeded(true), fFirstCall(true) {

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

    fParForVortex.resize(3); // field, lambda, xi

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    TBulkTriVortexMLFieldCalc *x = new TBulkTriVortexMLFieldCalc(fWisdom, fGridSteps);
    fVortex = x;
    x = 0;

    TPofBCalc *y = new TPofBCalc(fParForPofB);
    fPofB = y;
    y = 0;

    TPofTCalc *z = new TPofTCalc(fPofB, fWisdom, fParForPofT);
    fPofT = z;
    z = 0;

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
// TBulkTriVortexML-Method that calls the procedures to create B(x,y), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TBulkTriVortexML (phase, av.field, lambda, xi, [not implemented: bkg weight])
//------------------

double TBulkTriVortexML::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 4 || par.size() == 5);

  if(t<0.0)
    return cos(par[0]*0.017453293);

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(0); i < 3; i++) {
      fParForVortex[i] = fPar[i+1];
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

      for (unsigned int i(0); i < 3; i++) {
        fParForVortex[i] = par[i+1];
      }

      fParForPofB[2] = par[1]; // Bkg-Field
      //fParForPofB[3] = 0.005; // Bkg-width (in principle zero)

      fVortex->SetParameters(fParForVortex);
      fVortex->CalculateGrid();
      fPofB->UnsetPBExists();
      fPofB->Calculate(fVortex, fParForPofB);
      fPofT->DoFFT();

    }/* else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}


//------------------
// Constructor of the TBulkTriVortexAGL class
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TBulkTriVortexAGL::TBulkTriVortexAGL() : fCalcNeeded(true), fFirstCall(true) {

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

    fParForVortex.resize(3); // field, lambda, xi

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    TBulkTriVortexAGLFieldCalc *x = new TBulkTriVortexAGLFieldCalc(fWisdom, fGridSteps);
    fVortex = x;
    x = 0;

    TPofBCalc *y = new TPofBCalc(fParForPofB);
    fPofB = y;
    y = 0;

    TPofTCalc *z = new TPofTCalc(fPofB, fWisdom, fParForPofT);
    fPofT = z;
    z = 0;

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
// TBulkTriVortexAGL-Method that calls the procedures to create B(x,y), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TBulkTriVortexAGL (phase, av.field, lambda, xi, [not implemented: bkg weight])
//------------------

double TBulkTriVortexAGL::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 4 || par.size() == 5);

  if(t<0.0)
    return cos(par[0]*0.017453293);

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(0); i < 3; i++) {
      fParForVortex[i] = fPar[i+1];
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

      for (unsigned int i(0); i < 3; i++) {
        fParForVortex[i] = par[i+1];
      }

      fParForPofB[2] = par[1]; // Bkg-Field
      //fParForPofB[3] = 0.005; // Bkg-width (in principle zero)

      fVortex->SetParameters(fParForVortex);
      fVortex->CalculateGrid();
      fPofB->UnsetPBExists();
      fPofB->Calculate(fVortex, fParForPofB);
      fPofT->DoFFT();

    }/* else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}


//------------------
// Constructor of the TBulkTriVortexNGL class
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TBulkTriVortexNGL::TBulkTriVortexNGL() : fCalcNeeded(true), fFirstCall(true) {

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

    fParForVortex.resize(3); // field, lambda, xi

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    TBulkTriVortexNGLFieldCalc *x = new TBulkTriVortexNGLFieldCalc(fWisdom, fGridSteps);
    fVortex = x;
    x = 0;

    TPofBCalc *y = new TPofBCalc(fParForPofB);
    fPofB = y;
    y = 0;

    TPofTCalc *z = new TPofTCalc(fPofB, fWisdom, fParForPofT);
    fPofT = z;
    z = 0;

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
// TBulkTriVortexNGL-Method that calls the procedures to create B(x,y), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TBulkTriVortexNGL (phase, appl.field, lambda, xi, [not implemented: bkg weight])
//------------------

double TBulkTriVortexNGL::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 4 || par.size() == 5);

  if(t<0.0)
    return cos(par[0]*0.017453293);

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(0); i < 3; i++) {
      fParForVortex[i] = fPar[i+1];
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

      for (unsigned int i(0); i < 3; i++) {
        fParForVortex[i] = par[i+1];
      }

      fParForPofB[2] = par[1]; // Bkg-Field
      //fParForPofB[3] = 0.005; // Bkg-width (in principle zero)

      fVortex->SetParameters(fParForVortex);
      fVortex->CalculateGrid();
      fPofB->UnsetPBExists();
      fPofB->Calculate(fVortex, fParForPofB);
      fPofT->DoFFT();

    }/* else {
      cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}
