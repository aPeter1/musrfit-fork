/***************************************************************************

  TLondon1D.cpp

  Author: Bastian M. Wojek

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

#include "TLondon1D.h"
#include <iostream>
#include <cassert>
#include <cmath>

#include <TSAXParser.h>
#include "BMWStartupHandler.h"

ClassImp(TLondon1DHS)
ClassImp(TLondon1D1L)
ClassImp(TLondon1D2L)
ClassImp(TProximity1D1LHS)
ClassImp(TLondon1D3L)
ClassImp(TLondon1D3LS)
// ClassImp(TLondon1D4L)



//------------------
// Destructor of the TLondon1DHS/1L/2L/3L/3LS classes -- cleaning up
//------------------

TLondon1DHS::~TLondon1DHS() {
    fPar.clear();
    fParForBofZ.clear();
    fParForPofB.clear();
    fParForPofT.clear();
    delete fImpProfile;
    fImpProfile = 0;
    delete fPofB;
    fPofB = 0;
    delete fPofT;
    fPofT = 0;
}

TLondon1D1L::~TLondon1D1L() {
    fPar.clear();
    fParForBofZ.clear();
    fParForPofB.clear();
    fParForPofT.clear();
    delete fImpProfile;
    fImpProfile = 0;
    delete fPofB;
    fPofB = 0;
    delete fPofT;
    fPofT = 0;
}

TLondon1D2L::~TLondon1D2L() {
    fPar.clear();
    fParForBofZ.clear();
    fParForPofB.clear();
    fParForPofT.clear();
    delete fImpProfile;
    fImpProfile = 0;
    delete fPofB;
    fPofB = 0;
    delete fPofT;
    fPofT = 0;
}

TProximity1D1LHS::~TProximity1D1LHS() {
    fPar.clear();
    fParForBofZ.clear();
    fParForPofB.clear();
    fParForPofT.clear();
    delete fImpProfile;
    fImpProfile = 0;
    delete fPofB;
    fPofB = 0;
    delete fPofT;
    fPofT = 0;
}

TLondon1D3L::~TLondon1D3L() {
    fPar.clear();
    fParForBofZ.clear();
    fParForPofB.clear();
    fParForPofT.clear();
    delete fImpProfile;
    fImpProfile = 0;
    delete fPofB;
    fPofB = 0;
    delete fPofT;
    fPofT = 0;
}

TLondon1D3LS::~TLondon1D3LS() {
    fPar.clear();
    fParForBofZ.clear();
    fParForPofB.clear();
    fParForPofT.clear();
    delete fImpProfile;
    fImpProfile = 0;
    delete fPofB;
    fPofB = 0;
    delete fPofT;
    fPofT = 0;
}

// TLondon1D4L::~TLondon1D4L() {
//     fPar.clear();
//     fParForBofZ.clear();
//     fParForPofB.clear();
//     fParForPofT.clear();
//     delete fImpProfile;
//     fImpProfile = 0;
//     delete fPofT;
//     fPofT = 0;
// }

//------------------
// Constructor of the TLondon1DHS class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TLondon1DHS::TLondon1DHS() : fCalcNeeded(true), fFirstCall(true) {

    // read startup file
    string startup_path_name("BMW_startup.xml");

    TSAXParser *saxParser = new TSAXParser();
    BMWStartupHandler *startupHandler = new BMWStartupHandler();
    saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
    //int status (saxParser->ParseFile(startup_path_name.c_str()));
    // parsing the file as above seems to lead to problems in certain environments;
    // use the parseXmlFile function instead (see PUserFcnBase.cpp for the definition)
    int status = parseXmlFile(saxParser, startup_path_name.c_str());
    // check for parse errors
    if (status) { // error
      cerr << endl << "**ERROR** Reading/parsing " << startup_path_name << " failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
    }

    fNSteps = startupHandler->GetNSteps();
    fWisdom = startupHandler->GetWisdomFile();
    string rge_path(startupHandler->GetDataPath());
    map<double, string> energy_vec(startupHandler->GetEnergies());

    fParForPofT.push_back(0.0); // phase
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());
    fParForPofB.push_back(0.0); // Energy
    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    fImpProfile = new TTrimSPData(rge_path, energy_vec, startupHandler->GetDebug());

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
// TLondon1DHS-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TLondon1DHS
//------------------

double TLondon1DHS::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 5);

  if(t<0.0)
    return cos(par[0]*0.017453293);

  bool dead_layer_changed(false);

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(2); i<fPar.size(); i++){
      fParForBofZ.push_back(fPar[i]);
    }
    fFirstCall = false;
    dead_layer_changed = true;
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
        if (i == 3) {
          dead_layer_changed = true;
        }
      }
    }
  }

  if (par_changed)
    fCalcNeeded = true;

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

//      std::cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << std::endl;

      for (unsigned int i(2); i<fPar.size(); i++)
        fParForBofZ[i-2] = par[i];

      fParForPofB[2] = par[1]; // energy
      fParForPofB[3] = par[2]; // Bkg-Field
      //fParForPofB[4] = 0.005; // Bkg-width (in principle zero)

      if(dead_layer_changed){
        vector<double> interfaces;
        interfaces.push_back(par[3]);// dead layer
        fParForPofB[5] = fImpProfile->LayerFraction(par[1], 1, interfaces); // Fraction of muons in the deadlayer
        interfaces.clear();
      }

      TLondon1D_HS BofZ(fParForBofZ);
      fPofB->UnsetPBExists();
      fPofB->Calculate(&BofZ, fImpProfile, fParForPofB);
      fPofT->DoFFT();

    }/* else {
      std::cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << std::endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}


//------------------
// Constructor of the TLondon1D1L class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TLondon1D1L::TLondon1D1L() : fCalcNeeded(true), fFirstCall(true) {

    // read startup file
    string startup_path_name("BMW_startup.xml");

    TSAXParser *saxParser = new TSAXParser();
    BMWStartupHandler *startupHandler = new BMWStartupHandler();
    saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
    //int status (saxParser->ParseFile(startup_path_name.c_str()));
    // parsing the file as above seems to lead to problems in certain environments;
    // use the parseXmlFile function instead (see PUserFcnBase.cpp for the definition)
    int status = parseXmlFile(saxParser, startup_path_name.c_str());
    // check for parse errors
    if (status) { // error
      cerr << endl << "**ERROR** Reading/parsing " << startup_path_name << " failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
    }

    fNSteps = startupHandler->GetNSteps();
    fWisdom = startupHandler->GetWisdomFile();
    string rge_path(startupHandler->GetDataPath());
    map<double, string> energy_vec(startupHandler->GetEnergies());

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());
    fParForPofB.push_back(0.0); // Energy
    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    fImpProfile = new TTrimSPData(rge_path, energy_vec, startupHandler->GetDebug());

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
// TLondon1D1L-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TLondon1D1L
//------------------

double TLondon1D1L::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 6 || par.size() == 8);

  // Debugging
  // Count the number of function calls
  //  fCallCounter++;

  if(t<0.0)
    return cos(par[0]*0.017453293);

  bool bkg_fraction_changed(false);
  bool weights_changed(false);

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(2); i<fPar.size(); i++){
      fParForBofZ.push_back(fPar[i]);
    }

    fFirstCall = false;
    bkg_fraction_changed = true;
    if (par.size() == 8)
      weights_changed = true;
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
        if (i == 3 || i == 4)
          bkg_fraction_changed = true;
        if (i == 6 || i == 7)
          weights_changed = true;
      }
    }
  }

  if (par_changed)
    fCalcNeeded = true;

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

//      std::cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << std::endl;

      for (unsigned int i(2); i<fPar.size(); i++)
        fParForBofZ[i-2] = par[i];

      fParForPofB[2] = par[1]; // energy
      fParForPofB[3] = par[2]; // Bkg-Field
      //fParForPofB[4] = 0.005; // Bkg-width (in principle zero)

      if(weights_changed) {
        vector<double> interfaces;
        interfaces.push_back(par[3]+par[4]);

        vector<double> weights;
        for(unsigned int i(6); i<8; i++)
          weights.push_back(par[i]);

//        std::cout << "Weighting has changed, re-calculating n(z) now..." << std::endl;
        fImpProfile->WeightLayers(par[1], interfaces, weights);

        interfaces.clear();
        weights.clear();
      }

      if(bkg_fraction_changed || weights_changed) {
        vector<double> interfaces;
        interfaces.push_back(par[3]);// dead layer
        interfaces.push_back(par[3] + par[4]);// dead layer + first layer
        fParForPofB[5] = fImpProfile->LayerFraction(par[1], 1, interfaces) + // Fraction of muons in the deadlayer
                         fImpProfile->LayerFraction(par[1], 3, interfaces); // Fraction of muons in the substrate
        interfaces.clear();
      }


      TLondon1D_1L BofZ1(fParForBofZ);
      fPofB->UnsetPBExists();
      fPofB->Calculate(&BofZ1, fImpProfile, fParForPofB);
      fPofT->DoFFT();

    }/* else {
      std::cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << std::endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }
  
//   // Debugging start
//   if (!(fCallCounter%10000)){
//     std::cout << fCallCounter-1 << "\t";
//     for (unsigned int i(0); i<fPar.size(); i++){
//       std::cout << fPar[i] << "\t";
//     }
//     std::cout << std::endl;
//   }
//   // Debugging end

  return fPofT->Eval(t);

}

//------------------
// Constructor of the TLondon1D2L class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TLondon1D2L::TLondon1D2L() : fCalcNeeded(true), fFirstCall(true) {
  // read startup file
    string startup_path_name("BMW_startup.xml");

    TSAXParser *saxParser = new TSAXParser();
    BMWStartupHandler *startupHandler = new BMWStartupHandler();
    saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
    //int status (saxParser->ParseFile(startup_path_name.c_str()));
    // parsing the file as above seems to lead to problems in certain environments;
    // use the parseXmlFile function instead (see PUserFcnBase.cpp for the definition)
    int status = parseXmlFile(saxParser, startup_path_name.c_str());
    // check for parse errors
    if (status) { // error
      cerr << endl << "**ERROR** Reading/parsing " << startup_path_name << " failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
    }

    fNSteps = startupHandler->GetNSteps();
    fWisdom = startupHandler->GetWisdomFile();
    string rge_path(startupHandler->GetDataPath());
    map<double, string> energy_vec(startupHandler->GetEnergies());

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());
    fParForPofB.push_back(0.0); // Energy
    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    fImpProfile = new TTrimSPData(rge_path, energy_vec, startupHandler->GetDebug());

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
// TLondon1D2L-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TLondon1D2L
//------------------

double TLondon1D2L::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 8 || par.size() == 11);

  if(t<0.0)
    return cos(par[0]*0.017453293);

  bool bkg_fraction_changed(false);
  bool weights_changed(false);

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(2); i<fPar.size(); i++){
      fParForBofZ.push_back(fPar[i]);
    }
    fFirstCall=false;
    bkg_fraction_changed = true;
    if (par.size() == 11)
      weights_changed = true;
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
        if (i == 3 || i == 4 || i == 5)
          bkg_fraction_changed = true;
        if (i == 8 || i == 9 || i == 10)
          weights_changed = true;
      }
    }
  }

  if (par_changed)
    fCalcNeeded = true;

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

//      std::cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << std::endl;

      for (unsigned int i(2); i<par.size(); i++)
        fParForBofZ[i-2] = par[i];

      fParForPofB[2] = par[1]; // energy
      fParForPofB[3] = par[2]; // Bkg-Field
      //fParForPofB[4] = 0.005; // Bkg-width (in principle zero)

      if(weights_changed) {
        vector<double> interfaces;
        interfaces.push_back(par[3]+par[4]);
        interfaces.push_back(par[3]+par[4]+par[5]);

        vector<double> weights;
        for(unsigned int i(8); i<11; i++)
          weights.push_back(par[i]);

//        std::cout << "Weighting has changed, re-calculating n(z) now..." << std::endl;
        fImpProfile->WeightLayers(par[1], interfaces, weights);

        interfaces.clear();
        weights.clear();
      }

      if(bkg_fraction_changed || weights_changed) {
        vector<double> interfaces;
        interfaces.push_back(par[3]);// dead layer
        interfaces.push_back(par[3] + par[4] + par[5]);// dead layer + first layer + second layer
        fParForPofB[5] = fImpProfile->LayerFraction(par[1], 1, interfaces) + // Fraction of muons in the deadlayer
                         fImpProfile->LayerFraction(par[1], 3, interfaces); // Fraction of muons in the substrate
        interfaces.clear();
      }

      TLondon1D_2L BofZ2(fParForBofZ);
      fPofB->UnsetPBExists();
      fPofB->Calculate(&BofZ2, fImpProfile, fParForPofB);
      fPofT->DoFFT();


    }/* else {
      std::cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << std::endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}

//------------------
// Constructor of the TProximity1D1LHS class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TProximity1D1LHS::TProximity1D1LHS() : fCalcNeeded(true), fFirstCall(true) {

    // read startup file
    string startup_path_name("BMW_startup.xml");

    TSAXParser *saxParser = new TSAXParser();
    BMWStartupHandler *startupHandler = new BMWStartupHandler();
    saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
    //int status (saxParser->ParseFile(startup_path_name.c_str()));
    // parsing the file as above seems to lead to problems in certain environments;
    // use the parseXmlFile function instead (see PUserFcnBase.cpp for the definition)
    int status = parseXmlFile(saxParser, startup_path_name.c_str());
    // check for parse errors
    if (status) { // error
      cerr << endl << "**ERROR** Reading/parsing " << startup_path_name << " failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
    }

    fNSteps = startupHandler->GetNSteps();
    fWisdom = startupHandler->GetWisdomFile();
    string rge_path(startupHandler->GetDataPath());
    map<double, string> energy_vec(startupHandler->GetEnergies());

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());
    fParForPofB.push_back(0.0); // Energy
    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.01); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    fImpProfile = new TTrimSPData(rge_path, energy_vec, startupHandler->GetDebug());

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
// TProximity1D1LHS-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TProximity1D1LHS
//------------------

double TProximity1D1LHS::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 7);

  if(t<0.0)
    return cos(par[0]*0.017453293);

  // check if the function is called the first time and if yes, read in parameters

  bool dead_layer_changed(false);

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(2); i<fPar.size(); i++){
      fParForBofZ.push_back(fPar[i]);
    }
    fFirstCall = false;
    dead_layer_changed = true;
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
        if (i == 4){
          dead_layer_changed = true;
        }
      }
    }
  }

  if (par_changed)
    fCalcNeeded = true;

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

//      std::cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << std::endl;

      for (unsigned int i(2); i<fPar.size(); i++)
        fParForBofZ[i-2] = par[i];

      fParForPofB[2] = par[1]; // energy
      fParForPofB[3] = par[2]; // Bkg-Field
      //fParForPofB[4] = 0.005; // Bkg-width (in principle zero)

      if(dead_layer_changed){
        vector<double> interfaces;
        interfaces.push_back(par[4]);// dead layer
        fParForPofB[5] = fImpProfile->LayerFraction(par[1], 1, interfaces); // Fraction of muons in the deadlayer
        interfaces.clear();
      }

      TProximity1D_1LHS BofZ(fParForBofZ);
      fPofB->UnsetPBExists();
      fPofB->Calculate(&BofZ, fImpProfile, fParForPofB);
      fPofT->DoFFT();


    }/* else {
      std::cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << std::endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}

//------------------
// Constructor of the TLondon1D3L class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TLondon1D3L::TLondon1D3L() : fCalcNeeded(true), fFirstCall(true) {
    // read startup file
    string startup_path_name("BMW_startup.xml");

    TSAXParser *saxParser = new TSAXParser();
    BMWStartupHandler *startupHandler = new BMWStartupHandler();
    saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
    //int status (saxParser->ParseFile(startup_path_name.c_str()));
    // parsing the file as above seems to lead to problems in certain environments;
    // use the parseXmlFile function instead (see PUserFcnBase.cpp for the definition)
    int status = parseXmlFile(saxParser, startup_path_name.c_str());
    // check for parse errors
    if (status) { // error
      cerr << endl << "**ERROR** Reading/parsing " << startup_path_name << " failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
    }

    fNSteps = startupHandler->GetNSteps();
    fWisdom = startupHandler->GetWisdomFile();
    string rge_path(startupHandler->GetDataPath());
    map<double, string> energy_vec(startupHandler->GetEnergies());

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());
    fParForPofB.push_back(0.0); // Energy
    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    fImpProfile = new TTrimSPData(rge_path, energy_vec, startupHandler->GetDebug());

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
// TLondon1D3L-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TLondon1D3L
//------------------

double TLondon1D3L::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 10 || par.size() == 14);

  if(t<0.0)
    return cos(par[0]*0.017453293);

  bool bkg_fraction_changed(false);
  bool weights_changed(false);

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(2); i<fPar.size(); i++){
      fParForBofZ.push_back(fPar[i]);
    }
    fFirstCall=false;
    bkg_fraction_changed = true;
    if (par.size() == 14)
      weights_changed = true;
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
        if (i == 3 || i == 4 || i == 5 || i == 6)
          bkg_fraction_changed = true;
        if (i == 10 || i == 11 || i == 12 || i == 13)
          weights_changed = true;
      }
    }
  }

  if (par_changed)
    fCalcNeeded = true;

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

//      std::cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << std::endl;

      for (unsigned int i(2); i<par.size(); i++)
        fParForBofZ[i-2] = par[i];

      fParForPofB[2] = par[1]; // energy
      fParForPofB[3] = par[2]; // Bkg-Field
      //fParForPofB[4] = 0.005; // Bkg-width (in principle zero)

      if(weights_changed) {
        vector<double> interfaces;
        interfaces.push_back(par[3]+par[4]);
        interfaces.push_back(par[3]+par[4]+par[5]);
        interfaces.push_back(par[3]+par[4]+par[5]+par[6]);

        vector<double> weights;
        for(unsigned int i(10); i<14; i++)
          weights.push_back(par[i]);

//        std::cout << "Weighting has changed, re-calculating n(z) now..." << std::endl;
        fImpProfile->WeightLayers(par[1], interfaces, weights);

        interfaces.clear();
        weights.clear();
      }

      if(bkg_fraction_changed || weights_changed) {
        vector<double> interfaces;
        interfaces.push_back(par[3]);// dead layer
        interfaces.push_back(par[3] + par[4] + par[5] + par[6]);// dead layer + first layer + second layer + third layer
        fParForPofB[5] = fImpProfile->LayerFraction(par[1], 1, interfaces) + // Fraction of muons in the deadlayer
                         fImpProfile->LayerFraction(par[1], 3, interfaces); // Fraction of muons in the substrate
        interfaces.clear();
      }

      TLondon1D_3L BofZ3(fParForBofZ);
      fPofB->UnsetPBExists();
      fPofB->Calculate(&BofZ3, fImpProfile, fParForPofB);
      fPofT->DoFFT();

    }/* else {
      std::cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << std::endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}

//------------------
// Constructor of the TLondon1D3LS class -- reading available implantation profiles and
// creates (a pointer to) the TPofTCalc object (with the FFT plan)
//------------------

TLondon1D3LS::TLondon1D3LS() : fCalcNeeded(true), fFirstCall(true) {
    // read startup file
    string startup_path_name("BMW_startup.xml");

    TSAXParser *saxParser = new TSAXParser();
    BMWStartupHandler *startupHandler = new BMWStartupHandler();
    saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
    //int status (saxParser->ParseFile(startup_path_name.c_str()));
    // parsing the file as above seems to lead to problems in certain environments;
    // use the parseXmlFile function instead (see PUserFcnBase.cpp for the definition)
    int status = parseXmlFile(saxParser, startup_path_name.c_str());
    // check for parse errors
    if (status) { // error
      cerr << endl << "**ERROR** Reading/parsing " << startup_path_name << " failed." \
           << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
           << endl;
      assert(false);
    }

    fNSteps = startupHandler->GetNSteps();
    fWisdom = startupHandler->GetWisdomFile();
    string rge_path(startupHandler->GetDataPath());
    map<double, string> energy_vec(startupHandler->GetEnergies());

    fParForPofT.push_back(0.0);
    fParForPofT.push_back(startupHandler->GetDeltat());
    fParForPofT.push_back(startupHandler->GetDeltaB());

    fParForPofB.push_back(startupHandler->GetDeltat());
    fParForPofB.push_back(startupHandler->GetDeltaB());
    fParForPofB.push_back(0.0); // Energy
    fParForPofB.push_back(0.0); // Bkg-Field
    fParForPofB.push_back(0.005); // Bkg-width
    fParForPofB.push_back(0.0); // Bkg-weight

    fImpProfile = new TTrimSPData(rge_path, energy_vec, startupHandler->GetDebug());

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
// TLondon1D3LS-Method that calls the procedures to create B(z), p(B) and P(t)
// It finally returns P(t) for a given t.
// Parameters: all the parameters for the function to be fitted through TLondon1D3LS
//------------------

double TLondon1D3LS::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 9 || par.size() == 13);

  if(t<0.0)
    return cos(par[0]*0.017453293);

  bool bkg_fraction_changed(false);
  bool weights_changed(false);

  // check if the function is called the first time and if yes, read in parameters

  if(fFirstCall){
    fPar = par;

    for (unsigned int i(2); i<fPar.size(); i++){
      fParForBofZ.push_back(fPar[i]);
    }
    fFirstCall=false;
    bkg_fraction_changed = true;
    if (par.size() == 13)
      weights_changed = true;
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
        if (i == 3 || i == 4 || i == 5 || i == 6)
          bkg_fraction_changed = true;
        if (i == 9 || i == 10 || i == 11 || i == 12)
          weights_changed = true;
      }
    }
  }

  if (par_changed)
    fCalcNeeded = true;

  // if model parameters have changed, recalculate B(z), P(B) and P(t)

  if (fCalcNeeded) {

    fParForPofT[0] = par[0]; // phase

    if(!only_phase_changed) {

//      std::cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << std::endl;

      for (unsigned int i(2); i<par.size(); i++)
        fParForBofZ[i-2] = par[i];

      fParForPofB[2] = par[1]; // energy
      fParForPofB[3] = par[2]; // Bkg-Field
      //fParForPofB[4] = 0.005; // Bkg-width (in principle zero)

      if(weights_changed) {
        vector<double> interfaces;
        interfaces.push_back(par[3]+par[4]);
        interfaces.push_back(par[3]+par[4]+par[5]);
        interfaces.push_back(par[3]+par[4]+par[5]+par[6]);

        vector<double> weights;
        for(unsigned int i(9); i<13; i++)
          weights.push_back(par[i]);

//        std::cout << "Weighting has changed, re-calculating n(z) now..." << std::endl;
        fImpProfile->WeightLayers(par[1], interfaces, weights);

        interfaces.clear();
        weights.clear();
      }

      if(bkg_fraction_changed || weights_changed) {
        vector<double> interfaces;
        interfaces.push_back(par[3]);// dead layer
        interfaces.push_back(par[3] + par[4] + par[5] + par[6]);// dead layer + first layer + second layer + third layer
        fParForPofB[5] = fImpProfile->LayerFraction(par[1], 1, interfaces) + // Fraction of muons in the deadlayer
                         fImpProfile->LayerFraction(par[1], 3, interfaces); // Fraction of muons in the substrate
        interfaces.clear();
      }

      TLondon1D_3LS BofZ3(fParForBofZ);
      fPofB->UnsetPBExists();
      fPofB->Calculate(&BofZ3, fImpProfile, fParForPofB);
      fPofT->DoFFT();

    }/* else {
      std::cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << std::endl;
    }*/

    fPofT->CalcPol(fParForPofT);

    fCalcNeeded = false;
  }

  return fPofT->Eval(t);

}

// //------------------
// // Constructor of the TLondon1D4L class -- reading available implantation profiles and
// // creates (a pointer to) the TPofTCalc object (with the FFT plan)
// //------------------
// 
// TLondon1D4L::TLondon1D4L() : fCalcNeeded(true), fFirstCall(true), fLastFourChanged(true) {
//     // read startup file
//     string startup_path_name("BMW_startup.xml");
// 
//     TSAXParser *saxParser = new TSAXParser();
//     BMWStartupHandler *startupHandler = new BMWStartupHandler();
//     saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
//     //int status (saxParser->ParseFile(startup_path_name.c_str()));
//     // parsing the file as above seems to lead to problems in certain environments;
//     // use the parseXmlFile function instead (see PUserFcnBase.cpp for the definition)
//     int status = parseXmlFile(saxParser, startup_path_name.c_str());
//     // check for parse errors
//     if (status) { // error
//       std::cout << endl << "**WARNING** Reading/parsing " << startup_path_name << " failed." << std::endl;
//     }
// 
//     fNSteps = startupHandler->GetNSteps();
//     fWisdom = startupHandler->GetWisdomFile();
//     vector< pair<double, string> > energy_vec(startupHandler->GetEnergies());
//     string rge_path(startupHandler->GetDataPath());
// 
//     fParForPofT.push_back(0.0);
//     fParForPofT.push_back(startupHandler->GetDeltat());
//     fParForPofT.push_back(startupHandler->GetDeltaB());
// 
//     fParForPofB.push_back(startupHandler->GetDeltat());
//     fParForPofB.push_back(startupHandler->GetDeltaB());
//     fParForPofB.push_back(0.0);
// 
//     fImpProfile = new TTrimSPData(rge_path, energy_vec, startupHandler->GetDebug());
// 
//     fPofT = new TPofTCalc(fWisdom, fParForPofT);
// 
//     // clean up
//     if (saxParser) {
//       delete saxParser;
//       saxParser = 0;
//     }
//     if (startupHandler) {
//       delete startupHandler;
//       startupHandler = 0;
//     }
// }
// 
// //------------------
// // TLondon1D4L-Method that calls the procedures to create B(z), p(B) and P(t)
// // It finally returns P(t) for a given t.
// // Parameters: all the parameters for the function to be fitted through TLondon1D4L
// //------------------
// 
// double TLondon1D4L::operator()(double t, const vector<double> &par) const {
// 
//   assert(par.size() == 16);
// 
//   if(t<0.0)
//     return 0.0;
// 
//   // check if the function is called the first time and if yes, read in parameters
// 
//   if(fFirstCall){
//     fPar = par;
// 
// /*    for (unsigned int i(0); i<fPar.size(); i++){
//       std::cout << "fPar[" << i << "] = " << fPar[i] << std::endl;
//     }
// */
//     for (unsigned int i(2); i<fPar.size(); i++){
//       fParForBofZ.push_back(fPar[i]);
// //      std::cout << "fParForBofZ[" << i-2 << "] = " << fParForBofZ[i-2] << std::endl;
//     }
//     fFirstCall=false;
//   }
// 
//   // check if any parameter has changed
// 
//   bool par_changed(false);
//   bool only_phase_changed(false);
// 
//   for (unsigned int i(0); i<fPar.size(); i++) {
//     if( fPar[i]-par[i] ) {
//       fPar[i] = par[i];
//       par_changed = true;
//       if (i == 0) {
//         only_phase_changed = true;
//       } else {
//         only_phase_changed = false;
//       }
//       if (i == fPar.size()-4 || i == fPar.size()-3 || i == fPar.size()-2 || i == fPar.size()-1)
//         fLastFourChanged = true;
//     }
//   }
// 
//   if (par_changed)
//     fCalcNeeded = true;
// 
//   // if model parameters have changed, recalculate B(z), P(B) and P(t)
// 
//   if (fCalcNeeded) {
// 
//     fParForPofT[0] = par[0]; // phase
// 
//     if(!only_phase_changed) {
// 
// //      std::cout << " Parameters have changed, (re-)calculating p(B) and P(t) now..." << std::endl;
// 
//       for (unsigned int i(2); i<par.size(); i++)
//         fParForBofZ[i-2] = par[i];
// 
//       fParForPofB[2] = par[1]; // energy
// 
// /* DEBUG ---------------------------
//       for(unsigned int i(0); i<fParForBofZ.size(); i++) {
//         std::cout << "ParForBofZ[" << i << "] = " << fParForBofZ[i] << std::endl;
//       }
// 
//       for(unsigned int i(0); i<fParForPofB.size(); i++) {
//         std::cout << "ParForPofB[" << i << "] = " << fParForPofB[i] << std::endl;
//       }
// 
//       for(unsigned int i(0); i<fParForPofT.size(); i++) {
//         std::cout << "ParForPofT[" << i << "] = " << fParForPofT[i] << std::endl;
//       }
// ------------------------------------*/
// 
//       if(fLastFourChanged) {
//         vector<double> interfaces;
//         interfaces.push_back(par[3]+par[4]);
//         interfaces.push_back(par[3]+par[4]+par[5]);
//         interfaces.push_back(par[3]+par[4]+par[5]+par[6]);
// 
//         vector<double> weights;
//         for(unsigned int i(par.size()-4); i<par.size(); i++)
//           weights.push_back(par[i]);
// 
// //        std::cout << "Weighting has changed, re-calculating n(z) now..." << std::endl;
//         fImpProfile->WeightLayers(par[1], interfaces, weights);
//       }
// 
//       TLondon1D_4L BofZ4(fNSteps, fParForBofZ);
//       TPofBCalc PofB4(BofZ4, *fImpProfile, fParForPofB);
//       fPofT->DoFFT(PofB4);
// 
//     }/* else {
//       std::cout << "Only the phase parameter has changed, (re-)calculating P(t) now..." << std::endl;
//     }*/
// 
//     fPofT->CalcPol(fParForPofT);
// 
//     fCalcNeeded = false;
//     fLastFourChanged = false;
//   }
// 
//   return fPofT->Eval(t);
// 
// }


