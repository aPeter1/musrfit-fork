/***************************************************************************

  PSkewedLorentzian.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013-2023 by Andreas Suter                              *
 *   andreas.suter@psi.ch                                                  *
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

#include <cmath>
#include <cassert>
#include <iostream>

#include <TSAXParser.h>

#include "PMusr.h"

#include "../include/PSkewedLorentzian.h"


ClassImp(PSkewedLorentzian) // for ROOT dictionary

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 */
PSkewedLorentzian::PSkewedLorentzian()
{
  // init
  fStartupHandler = nullptr;
  fNoOfFields = 0;
  fRange = 0;
  fValid = true;

  // read XML startup file
  char startup_path_name[128];
  TSAXParser *saxParser = new TSAXParser();
  PStartupHandler_SV *fStartupHandler = new PStartupHandler_SV();
  strcpy(startup_path_name, fStartupHandler->GetStartupFilePath().Data());
  saxParser->ConnectToHandler("PStartupHandler_SV", fStartupHandler);
  //Int_t status = saxParser->ParseFile(startup_path_name);
  // parsing the file as above seems to lead to problems in certain environments;
  // use the parseXmlFile function instead (see PUserFcnBase.cpp for the definition)
  Int_t status = parseXmlFile(saxParser, startup_path_name);
  // check for parse errors
  if (status) { // error
    std::cout << std::endl << ">> PSkewedLorentzian::PSkewedLorentzian: **WARNING** Reading/parsing skewedLorentzian_startup.xml failed.";
    std::cout << std::endl;
    fValid = false;
  }

  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = nullptr;
  }

  // check if everything went fine with the startup handler
  if (!fStartupHandler->IsValid()) {
    std::cout << std::endl << ">> PSkewedLorentzian::PSkewedLorentzian **PANIC ERROR**";
    std::cout << std::endl << ">>   startup handler too unhappy. Will terminate unfriendly, sorry.";
    std::cout << std::endl;
    fValid = false;
  }

  // get parameters from XML-startup-file
  fNoOfFields = fStartupHandler->GetNoOfFields();
  fRange = fStartupHandler->GetRange();

  // check if the parameters potentially make sense
  if (fNoOfFields == 0) {
    std::cout << std::endl << ">> PSkewedLorentzian::PSkewedLorentzian **WARNING**";
    std::cout << std::endl << ">>   Found number of field value == 0. Doesn't make sense, will set it to 101." << std::endl;
    fNoOfFields = 101;
  } else if (fNoOfFields > 1000) {
    std::cout << std::endl << ">> PSkewedLorentzian::PSkewedLorentzian **WARNING**";
    std::cout << std::endl << ">>   Found number of field value == " << fNoOfFields << ".";
    std::cout << std::endl << ">>   This is a very large number! Are you sure?" << std::endl;
  }

  if (fRange < 1.0) {
    std::cout << std::endl << ">> PSkewedLorentzian::PSkewedLorentzian **WARNING**";
    std::cout << std::endl << ">>   Found range value < 1.0 (" << fRange << "). Doesn't make sense, will set it to 10.0." << std::endl;
    fRange = 10.0;
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PSkewedLorentzian::~PSkewedLorentzian()
{
  if (fStartupHandler)
    delete fStartupHandler;
}

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t
 * \param par
 */
Double_t PSkewedLorentzian::operator()(Double_t t, const std::vector<Double_t> &par) const
{
  // expected parameters: (0) B0: Peak Field, (1) beta: field width, (2) Delta: skewness width, (3) phi: detector phase
  assert(par.size() == 4);

  if (t < 0.0)
    return 1.0;

  // make sure that Delta != +- 1.0, prevent division by zero
  if (fabs(par[2]-1.0) < 1.0e-4)
    return 0.0;

  // calculate the field sampling points
  PDoubleVector BB;
  Double_t fieldRangeMinus = fRange * (par[1]*(1.0-par[2])); // Bj < B0
  Double_t fieldRangePlus  = fRange * (par[1]*(1.0+par[2])); // Bj > B0

  Double_t dB = 0.0;
  Double_t dval = 0.0;
  if (fNoOfFields % 2 == 0) { // even number of sampling points
    dB = fieldRangeMinus / (fNoOfFields/2 + 0.5);
    for (Int_t j=fNoOfFields/2-1; j>=0; j--) {
      dval = par[0] - dB*(static_cast<Double_t>(j)+0.5);
      if (dval > 0.0)
        BB.push_back(dval); // Bj = B0 - dB*(j+1/2) for Bj < B0
    }
    dB = fieldRangePlus / (fNoOfFields/2 + 0.5);
    for (Int_t j=0; j<static_cast<Int_t>(fNoOfFields/2); j++)
      BB.push_back(par[0] + dB*(static_cast<Double_t>(j)+0.5)); // Bj = B0 + dB*(j+1/2) for Bj > B0
  } else { // odd number of sampling points
    Int_t halfNoOfPoints = (fNoOfFields-1)/2;
    dB = fieldRangeMinus / halfNoOfPoints;
    for (Int_t j=halfNoOfPoints; j>0; j--) {
      dval = par[0] - dB*static_cast<Double_t>(j);
      if (dval > 0.0)
        BB.push_back(dval); // Bj = B0 - dB*j for Bj < B0
    }
    BB.push_back(par[0]); // Bj = B0
    dB = fieldRangePlus / halfNoOfPoints;
    for (Int_t j=1; j<=halfNoOfPoints; j++)
      BB.push_back(par[0] + dB*static_cast<Double_t>(j)); // Bj = B0 + dB*j for Bj > B0
  }

  // calculate the asymmetry
  Double_t ampl_i=0.0;
  Double_t norm=0.0;
  Double_t result = 0.0;
  Double_t cycl = 0.0;
  Double_t phase = fDegToRad*par[3];
  for (UInt_t i=0; i<BB.size(); i++) {
    if (BB[i] < par[0]) // Bj < B0
      dval = (BB[i]-par[0])/(par[1]*(1.0-par[2]));
    else // Bj > 0
      dval = (BB[i]-par[0])/(par[1]*(1.0+par[2]));
    cycl = fTwoPi*GAMMA_BAR_MUON*BB[i];
    ampl_i = 1.0/(1.0+dval*dval);
    norm += ampl_i;
    result += ampl_i*cos(cycl*t+phase);
  }
  result /= norm;

  // clean up
  BB.clear();

  return result;
}
