/***************************************************************************

  PMagProximityFitter.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2011-2021 by Andreas Suter                              *
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

#include <cassert>
#include <cmath>

#include <iostream>

#include <TSAXParser.h>
#include <TMath.h>

#include "PMagProximityFitter.h"

#define GAMMA_MU   0.0851615503527
#define DEGREE2RAD 0.0174532925199

ClassImp(PMagProximityFitterGlobal)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PMagProximityFitterGlobal::PMagProximityFitterGlobal()
{
  fValid = true;
  fStartupHandler = 0;
  fRgeHandler = 0;
  fDz = 0.01; // 0.01nm

  // read XML startup file
  char startup_path_name[128];
  TSAXParser *saxParser = new TSAXParser();
  PMPStartupHandler *fStartupHandler = new PMPStartupHandler();
  strcpy(startup_path_name, fStartupHandler->GetStartupFilePath().Data());
  saxParser->ConnectToHandler("PMPStartupHandler", fStartupHandler);
  //Int_t status = saxParser->ParseFile(startup_path_name);
  // parsing the file as above seems to lead to problems in certain environments;
  // use the parseXmlFile function instead (see PUserFcnBase.cpp for the definition)
  Int_t status = parseXmlFile(saxParser, startup_path_name);
  // check for parse errors
  if (status) { // error
    std::cout << std::endl << ">> PMagProximityFitterGlobal::PMagProximityFitterGlobal: **WARNING** Reading/parsing mag_proximity_startup.xml failed.";
    std::cout << std::endl;
    fValid = false;
  }

  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }

  // check if everything went fine with the startup handler
  if (!fStartupHandler->IsValid()) {
    std::cout << std::endl << ">> PMagProximityFitterGlobal::PMagProximityFitterGlobal **PANIC ERROR**";
    std::cout << std::endl << ">>   startup handler too unhappy. Will terminate unfriendly, sorry.";
    std::cout << std::endl;
    fValid = false;
  }

  // load all the TRIM.SP rge-files
  fRgeHandler = new PRgeHandler();
  if (!fRgeHandler->IsValid()) {
    std::cout << std::endl << ">> PMagProximityFitterGlobal::PMagProximityFitterGlobal **PANIC ERROR**";
    std::cout << std::endl << ">>  rge data handler too unhappy. Will terminate unfriendly, sorry.";
    std::cout << std::endl;
    fValid = false;
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PMagProximityFitterGlobal::~PMagProximityFitterGlobal()
{
  fPreviousParam.clear();

  fField.clear();

  if (fRgeHandler) {
    delete fRgeHandler;
    fRgeHandler = nullptr;
  }
  if (fStartupHandler) {
    delete fStartupHandler;
    fStartupHandler = nullptr;
  }
}

//--------------------------------------------------------------------------
// CalculateField (public)
//--------------------------------------------------------------------------
/**
 *
 */
void PMagProximityFitterGlobal::CalculateField(const std::vector<Double_t> &param) const
{
  // param: [0] energy (keV), [1] zStart (nm), [2] zEnd (nm), [3] B0 (G), [4] B1 (G), [5] zeta (nm), [6] phase (°)
  assert(param.size() == 7);

  // check that param are new and hence a calculation is needed (except the phase, which is not needed here)
  Bool_t newParams = false;
  if (fPreviousParam.size() == 0) {
    for (UInt_t i=0; i<param.size(); i++)
      fPreviousParam.push_back(param[i]);
    newParams = true;
  } else {
    assert(param.size() == fPreviousParam.size());

    for (UInt_t i=0; i<param.size(); i++) {
      if (i == 6) // phase, nothing to be done
        continue;
      if (param[i] != fPreviousParam[i]) {
        newParams = true;
        break;
      }
    }
  }

  if (!newParams)
    return;

  // keep parameters
  for (UInt_t i=0; i<param.size(); i++)
    fPreviousParam[i] = param[i];

  // delete previous field vector
  fField.clear();

  // calculate the magnetic field with a 0.1nm step resolution
  Double_t z = param[1];
  Double_t dval = 0.0;
  do {
    if (param[5] == 0.0) { // zeta == 0
      dval = 0.0;
    } else {
      dval = param[4] * exp(-(param[2]-z)/param[5]) + param[3];
    }
    fField.push_back(dval);
    z += fDz;
  } while (z<=param[2]);
}

//--------------------------------------------------------------------------
// GetMagneticField
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PMagProximityFitterGlobal::GetMagneticField(const Double_t z) const
{
  Double_t result = -1.0;

  // check if z is out of range [zStart, zEnd]
  UInt_t idx=0;
  if ((z<fPreviousParam[1]) || (z>fPreviousParam[2])) {
    result = 0.0;
  } else {
    idx = (UInt_t)((z-fPreviousParam[1])/fDz);
    if (idx == fField.size()-1) {
      result = fField[idx];
    } else {
      result = fField[idx] + (fField[idx+1]-fField[idx])*((z-fPreviousParam[1])-idx*fDz)/fDz;
    }
  }

  return result;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImp(PMagProximityFitter)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PMagProximityFitter::PMagProximityFitter()
{
  fValid = false;
  fInvokedGlobal = false;
  fMagProximityFitterGlobal = nullptr;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PMagProximityFitter::~PMagProximityFitter()
{
  if ((fMagProximityFitterGlobal != 0) && fInvokedGlobal) {
    delete fMagProximityFitterGlobal;
    fMagProximityFitterGlobal = nullptr;
  }
}

//--------------------------------------------------------------------------
// SetGlobalPart (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 *
 * \param globalPart
 * \param idx
 */
void PMagProximityFitter::SetGlobalPart(std::vector<void*> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) {
    fMagProximityFitterGlobal = new PMagProximityFitterGlobal();
    if (fMagProximityFitterGlobal == 0) {
      fValid = false;
      std::cerr << std::endl << ">> PMagProximityFitter::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << std::endl;
    } else if (!fMagProximityFitterGlobal->IsValid()) {
      fValid = false;
      std::cerr << std::endl << ">> PMagProximityFitter::SetGlobalPart(): **ERROR** initialization of global user function object failed, sorry ..." << std::endl;
    } else {
      fValid = true;
      fInvokedGlobal = true;
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<PMagProximityFitterGlobal*>(fMagProximityFitterGlobal);
    }
  } else {
    fValid = true;
    fMagProximityFitterGlobal = (PMagProximityFitterGlobal*)globalPart[fIdxGlobal];
  }
}

//--------------------------------------------------------------------------
// GlobalPartIsValid (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 */
Bool_t PMagProximityFitter::GlobalPartIsValid() const
{
  return (fValid && fMagProximityFitterGlobal->IsValid());
}

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PMagProximityFitter::operator()(Double_t t, const std::vector<Double_t> &param) const
{
  // param: [0] energy (keV), [1] zStart (nm), [2] zEnd (nm), [3] B0 (G), [4] B1 (G), [5] zeta (nm), [6] phase (°)
  assert(param.size() == 7);

  // for negative time return polarization == 1
  if (t <= 0.0)
    return 1.0;

  // if zeta=param[5] has a negative value, return 1.0. This can be used to switch off the proximity effect but still being able to fit asymmetries etc.
  if (param[5] <= 0.0)
    return 1.0;

  // calculate field if parameter have changed
  fMagProximityFitterGlobal->CalculateField(param);
  Int_t energyIndex = fMagProximityFitterGlobal->GetEnergyIndex(param[0]);

  // calculate polarization
  Bool_t done = false;
  Double_t pol = 0.0, dPol = 0.0;
  Double_t z=param[1]; // set to zStart
  Int_t terminate = 0;
  Double_t dz = 0.1; // setp width == 0.1nm

  do {
    dPol = fMagProximityFitterGlobal->GetMuonStoppingDensity(energyIndex, z) * cos(GAMMA_MU * fMagProximityFitterGlobal->GetMagneticField(z) * t + param[6] * DEGREE2RAD);
    z += dz;
    pol += dPol;

    // change in polarization is very small hence start termination counting
    if (fabs(dPol) < 1.0e-5) {
      terminate++;
    } else {
      terminate = 0;
    }

    if (terminate > 10) // polarization died out hence one can stop
      done = true;
  } while (!done);

  return pol*dz;
}
