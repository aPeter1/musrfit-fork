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
ClassImp(TBulkSqVortexLondon)
ClassImp(TBulkTriVortexML)
ClassImp(TBulkTriVortexAGL)
ClassImp(TBulkTriVortexNGL)
ClassImp(TBulkAnisotropicTriVortexLondonGlobal)
ClassImp(TBulkAnisotropicTriVortexLondon)
ClassImp(TBulkAnisotropicTriVortexMLGlobal)
ClassImp(TBulkAnisotropicTriVortexML)
ClassImp(TBulkAnisotropicTriVortexAGLGlobal)
ClassImp(TBulkAnisotropicTriVortexAGL)

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
// Destructor of the TBulkSqVortexLondon class -- cleaning up
//------------------

TBulkSqVortexLondon::~TBulkSqVortexLondon() {
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
      cerr << endl << "**ERROR** reading/parsing TFitPofB_startup.xml failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
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
    fParForPofB.push_back(0.0); // vortex-weighting || antiferromagnetic field
    fParForPofB.push_back(0.0); // vortex-weighting: 0.0 homogeneous, 1.0 Gaussian, 2.0 Lorentzian || 3.0 antiferromagnetic vortex-cores

    fVortex = new TBulkTriVortexLondonFieldCalc(fWisdom, fGridSteps);

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
// Constructor of the TBulkSqVortexLondon class
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TBulkSqVortexLondon::TBulkSqVortexLondon() : fCalcNeeded(true), fFirstCall(true) {

    // read startup file
    string startup_path_name("TFitPofB_startup.xml");

    TSAXParser *saxParser = new TSAXParser();
    TFitPofBStartupHandler *startupHandler = new TFitPofBStartupHandler();
    saxParser->ConnectToHandler("TFitPofBStartupHandler", startupHandler);
    int status (saxParser->ParseFile(startup_path_name.c_str()));
    // check for parse errors
    if (status) { // error
      cerr << endl << "**ERROR** reading/parsing TFitPofB_startup.xml failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
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

    fVortex = new TBulkSqVortexLondonFieldCalc(fWisdom, fGridSteps);

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
// TBulkTriVortexLondon-Method that calls the procedures to create B(x,y), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TBulkTriVortexLondon (phase, av.field, lambda, xi, [not implemented: bkg weight])
//------------------

double TBulkTriVortexLondon::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 4 || par.size() == 5 || par.size() == 7 || par.size() == 8); // normal, +BkgWeight, +VortexWeighting, +AFfield

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
      if (par.size() == 7) {
        fParForPofB[5] = par[5];
        assert((par[6] == 0.0) || (par[6] == 1.0) || (par[6] == 2.0));
        fParForPofB[6] = par[6];
      } else if (par.size() == 8) {
        fParForPofB[5] = par[5];
        assert(par[6] == 3.0);
        fParForPofB[6] = par[6];
        fParForPofB.resize(8);
        fParForPofB[7] = par[7]; // AF-width in vortex-lattice-units
      }

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
// TBulkSqVortexLondon-Method that calls the procedures to create B(x,y), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TBulkSqVortexLondon (phase, av.field, lambda, xi, [not implemented: bkg weight])
//------------------

double TBulkSqVortexLondon::operator()(double t, const vector<double> &par) const {

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
      cerr << endl << "**ERROR** reading/parsing TFitPofB_startup.xml failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
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

    fVortex = new TBulkTriVortexMLFieldCalc(fWisdom, fGridSteps);

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
      cerr << endl << "**ERROR** reading/parsing TFitPofB_startup.xml failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
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

    fVortex = new TBulkTriVortexAGLFieldCalc(fWisdom, fGridSteps);

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
      cerr << endl << "**ERROR** reading/parsing TFitPofB_startup.xml failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
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

    fVortex = new TBulkTriVortexNGLFieldCalc(fWisdom, fGridSteps);

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

//------------------
// Destructor of the TBulkAnisotropicTriVortexLondonGlobal class -- cleaning up
//------------------

TBulkAnisotropicTriVortexLondonGlobal::~TBulkAnisotropicTriVortexLondonGlobal() {
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
// Constructor of the TBulkAnisotropicTriVortexLondonGlobal class
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TBulkAnisotropicTriVortexLondonGlobal::TBulkAnisotropicTriVortexLondonGlobal() : fCalcNeeded(true), fFirstCall(true) {

    // read startup file
    string startup_path_name("TFitPofB_startup.xml");

    TSAXParser *saxParser = new TSAXParser();
    TFitPofBStartupHandler *startupHandler = new TFitPofBStartupHandler();
    saxParser->ConnectToHandler("TFitPofBStartupHandler", startupHandler);
    int status (saxParser->ParseFile(startup_path_name.c_str()));
    // check for parse errors
    if (status) { // error
      cerr << endl << "**ERROR** reading/parsing TFitPofB_startup.xml failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
    }

    fGridSteps = startupHandler->GetGridSteps();
    fWisdom = startupHandler->GetWisdomFile();

    fParForVortex.resize(5); // field, lambdaX, lambdaY, xiX, xiY

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    fVortex = new TBulkAnisotropicTriVortexLondonFieldCalc(fWisdom, fGridSteps);

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

void TBulkAnisotropicTriVortexLondonGlobal::Calc(const vector<double> &par) const {

  assert(par.size() == 6);
/*
  if(t<0.0)
    return cos(par[0]*0.017453293);
*/

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(0); i < 5; i++) {
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

  if (par_changed) {
    fCalcNeeded = true;
    fValid = false;
  }

  // if model parameters have changed, recalculate B(x,y), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

//      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      for (unsigned int i(0); i < 5; i++) {
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
    fValid = true;
  }
/*
  return fPofT->Eval(t);
*/
  return;
}

//------------------------------------------------------------------------------------
/**
 * <p> Constructor.
 */
TBulkAnisotropicTriVortexLondon::TBulkAnisotropicTriVortexLondon()
{
  fValid = false;
  fInvokedGlobal = false;
  fIdxGlobal = -1;
  fGlobalUserFcn = 0;
}

//------------------------------------------------------------------------------------
/**
 * <p> Destructor.
 */
TBulkAnisotropicTriVortexLondon::~TBulkAnisotropicTriVortexLondon()
{
  if ((fGlobalUserFcn != 0) && fInvokedGlobal) {
    delete fGlobalUserFcn;
    fGlobalUserFcn = 0;
  }
}

//------------------------------------------------------------------------------------
/**
 * <p> Used to invoke/retrieve the proper global user function
 *
 * \param globalPart reference to the global user function object vector
 * \param idx global user function index within the theory tree
 */
void TBulkAnisotropicTriVortexLondon::SetGlobalPart(vector<void *> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) { // global user function not present, invoke it
    fGlobalUserFcn = new TBulkAnisotropicTriVortexLondonGlobal();
    if (fGlobalUserFcn == 0) { // global user function object couldn't be invoked -> error
      fValid = false;
      cerr << endl << ">> TBulkAnisotropicTriVortexLondon::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << endl;
    } else {  // global user function object could be invoked -> resize to global user function vector and keep the pointer to the corresponding object
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<TBulkAnisotropicTriVortexLondonGlobal*>(fGlobalUserFcn);
      fValid = true;
      fInvokedGlobal = true;
    }
  } else { // global user function already present hence just retrieve a pointer to it
    fValid = true;
    fGlobalUserFcn = (TBulkAnisotropicTriVortexLondonGlobal*)globalPart[fIdxGlobal];
  }
}

//------------------------------------------------------------------------------------
/**
 * <p> Used to check if the user function is OK.
 *
 * <b>return:</b> true if both, the user function and the global user function object are valid
 */
bool TBulkAnisotropicTriVortexLondon::GlobalPartIsValid() const
{
  return (fValid && fGlobalUserFcn->IsValid());
}

//------------------------------------------------------------------------------------
/**
 * <p> function operator
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param param parameter vector
 */
double TBulkAnisotropicTriVortexLondon::operator()(double t, const std::vector<double> &param) const
{
  // expected parameters: phase, field, lambdaX, lambdaY, xiX, xiY

  assert(param.size() == 6);
  assert(fGlobalUserFcn);

  if(t<0.0)
    return cos(param[0]*0.017453293);

  // call the global user function object
  fGlobalUserFcn->Calc(param);

  return fGlobalUserFcn->GetPolarizationPointer()->Eval(t);
}

//------------------
// Destructor of the TBulkAnisotropicTriVortexMLGlobal class -- cleaning up
//------------------

TBulkAnisotropicTriVortexMLGlobal::~TBulkAnisotropicTriVortexMLGlobal() {
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
// Constructor of the TBulkAnisotropicTriVortexMLGlobal class
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TBulkAnisotropicTriVortexMLGlobal::TBulkAnisotropicTriVortexMLGlobal() : fCalcNeeded(true), fFirstCall(true) {

    // read startup file
    string startup_path_name("TFitPofB_startup.xml");

    TSAXParser *saxParser = new TSAXParser();
    TFitPofBStartupHandler *startupHandler = new TFitPofBStartupHandler();
    saxParser->ConnectToHandler("TFitPofBStartupHandler", startupHandler);
    int status (saxParser->ParseFile(startup_path_name.c_str()));
    // check for parse errors
    if (status) { // error
      cerr << endl << "**ERROR** reading/parsing TFitPofB_startup.xml failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
    }

    fGridSteps = startupHandler->GetGridSteps();
    fWisdom = startupHandler->GetWisdomFile();

    fParForVortex.resize(5); // field, lambdaX, lambdaY, xiX, xiY

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    fVortex = new TBulkAnisotropicTriVortexMLFieldCalc(fWisdom, fGridSteps);

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

void TBulkAnisotropicTriVortexMLGlobal::Calc(const vector<double> &par) const {

  assert(par.size() == 6);
/*
  if(t<0.0)
    return cos(par[0]*0.017453293);
*/

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(0); i < 5; i++) {
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

  if (par_changed) {
    fCalcNeeded = true;
    fValid = false;
  }

  // if model parameters have changed, recalculate B(x,y), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

//      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      for (unsigned int i(0); i < 5; i++) {
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
    fValid = true;
  }
/*
  return fPofT->Eval(t);
*/
  return;
}

//------------------------------------------------------------------------------------
/**
 * <p> Constructor.
 */
TBulkAnisotropicTriVortexML::TBulkAnisotropicTriVortexML()
{
  fValid = false;
  fInvokedGlobal = false;
  fIdxGlobal = -1;
  fGlobalUserFcn = 0;
}

//------------------------------------------------------------------------------------
/**
 * <p> Destructor.
 */
TBulkAnisotropicTriVortexML::~TBulkAnisotropicTriVortexML()
{
  if ((fGlobalUserFcn != 0) && fInvokedGlobal) {
    delete fGlobalUserFcn;
    fGlobalUserFcn = 0;
  }
}

//------------------------------------------------------------------------------------
/**
 * <p> Used to invoke/retrieve the proper global user function
 *
 * \param globalPart reference to the global user function object vector
 * \param idx global user function index within the theory tree
 */
void TBulkAnisotropicTriVortexML::SetGlobalPart(vector<void *> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) { // global user function not present, invoke it
    fGlobalUserFcn = new TBulkAnisotropicTriVortexMLGlobal();
    if (fGlobalUserFcn == 0) { // global user function object couldn't be invoked -> error
      fValid = false;
      cerr << endl << ">> TBulkAnisotropicTriVortexML::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << endl;
    } else {  // global user function object could be invoked -> resize to global user function vector and keep the pointer to the corresponding object
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<TBulkAnisotropicTriVortexMLGlobal*>(fGlobalUserFcn);
      fValid = true;
      fInvokedGlobal = true;
    }
  } else { // global user function already present hence just retrieve a pointer to it
    fValid = true;
    fGlobalUserFcn = (TBulkAnisotropicTriVortexMLGlobal*)globalPart[fIdxGlobal];
  }
}

//------------------------------------------------------------------------------------
/**
 * <p> Used to check if the user function is OK.
 *
 * <b>return:</b> true if both, the user function and the global user function object are valid
 */
bool TBulkAnisotropicTriVortexML::GlobalPartIsValid() const
{
  return (fValid && fGlobalUserFcn->IsValid());
}

//------------------------------------------------------------------------------------
/**
 * <p> function operator
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param param parameter vector
 */
double TBulkAnisotropicTriVortexML::operator()(double t, const std::vector<double> &param) const
{
  // expected parameters: phase, field, lambdaX, lambdaY, xiX, xiY

  assert(param.size() == 6);
  assert(fGlobalUserFcn);

  if(t<0.0)
    return cos(param[0]*0.017453293);

  // call the global user function object
  fGlobalUserFcn->Calc(param);

  return fGlobalUserFcn->GetPolarizationPointer()->Eval(t);
}

//------------------
// Destructor of the TBulkAnisotropicTriVortexAGLGlobal class -- cleaning up
//------------------

TBulkAnisotropicTriVortexAGLGlobal::~TBulkAnisotropicTriVortexAGLGlobal() {
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
// Constructor of the TBulkAnisotropicTriVortexAGLGlobal class
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TBulkAnisotropicTriVortexAGLGlobal::TBulkAnisotropicTriVortexAGLGlobal() : fCalcNeeded(true), fFirstCall(true) {

    // read startup file
    string startup_path_name("TFitPofB_startup.xml");

    TSAXParser *saxParser = new TSAXParser();
    TFitPofBStartupHandler *startupHandler = new TFitPofBStartupHandler();
    saxParser->ConnectToHandler("TFitPofBStartupHandler", startupHandler);
    int status (saxParser->ParseFile(startup_path_name.c_str()));
    // check for parse errors
    if (status) { // error
      cerr << endl << "**ERROR** reading/parsing TFitPofB_startup.xml failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
    }

    fGridSteps = startupHandler->GetGridSteps();
    fWisdom = startupHandler->GetWisdomFile();

    fParForVortex.resize(5); // field, lambdaX, lambdaY, xiX, xiY

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    fVortex = new TBulkAnisotropicTriVortexAGLFieldCalc(fWisdom, fGridSteps);

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

void TBulkAnisotropicTriVortexAGLGlobal::Calc(const vector<double> &par) const {

  assert(par.size() == 6);
/*
  if(t<0.0)
    return cos(par[0]*0.017453293);
*/

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(0); i < 5; i++) {
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

  if (par_changed) {
    fCalcNeeded = true;
    fValid = false;
  }

  // if model parameters have changed, recalculate B(x,y), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

//      cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << endl;

      for (unsigned int i(0); i < 5; i++) {
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
    fValid = true;
  }
/*
  return fPofT->Eval(t);
*/
  return;
}

//------------------------------------------------------------------------------------
/**
 * <p> Constructor.
 */
TBulkAnisotropicTriVortexAGL::TBulkAnisotropicTriVortexAGL()
{
  fValid = false;
  fInvokedGlobal = false;
  fIdxGlobal = -1;
  fGlobalUserFcn = 0;
}

//------------------------------------------------------------------------------------
/**
 * <p> Destructor.
 */
TBulkAnisotropicTriVortexAGL::~TBulkAnisotropicTriVortexAGL()
{
  if ((fGlobalUserFcn != 0) && fInvokedGlobal) {
    delete fGlobalUserFcn;
    fGlobalUserFcn = 0;
  }
}

//------------------------------------------------------------------------------------
/**
 * <p> Used to invoke/retrieve the proper global user function
 *
 * \param globalPart reference to the global user function object vector
 * \param idx global user function index within the theory tree
 */
void TBulkAnisotropicTriVortexAGL::SetGlobalPart(vector<void *> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) { // global user function not present, invoke it
    fGlobalUserFcn = new TBulkAnisotropicTriVortexAGLGlobal();
    if (fGlobalUserFcn == 0) { // global user function object couldn't be invoked -> error
      fValid = false;
      cerr << endl << ">> TBulkAnisotropicTriVortexAGL::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << endl;
    } else {  // global user function object could be invoked -> resize to global user function vector and keep the pointer to the corresponding object
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<TBulkAnisotropicTriVortexAGLGlobal*>(fGlobalUserFcn);
      fValid = true;
      fInvokedGlobal = true;
    }
  } else { // global user function already present hence just retrieve a pointer to it
    fValid = true;
    fGlobalUserFcn = (TBulkAnisotropicTriVortexAGLGlobal*)globalPart[fIdxGlobal];
  }
}

//------------------------------------------------------------------------------------
/**
 * <p> Used to check if the user function is OK.
 *
 * <b>return:</b> true if both, the user function and the global user function object are valid
 */
bool TBulkAnisotropicTriVortexAGL::GlobalPartIsValid() const
{
  return (fValid && fGlobalUserFcn->IsValid());
}

//------------------------------------------------------------------------------------
/**
 * <p> function operator
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param param parameter vector
 */
double TBulkAnisotropicTriVortexAGL::operator()(double t, const std::vector<double> &param) const
{
  // expected parameters: phase, field, lambdaX, lambdaY, xiX, xiY

  assert(param.size() == 6);
  assert(fGlobalUserFcn);

  if(t<0.0)
    return cos(param[0]*0.017453293);

  // call the global user function object
  fGlobalUserFcn->Calc(param);

  return fGlobalUserFcn->GetPolarizationPointer()->Eval(t);
}
