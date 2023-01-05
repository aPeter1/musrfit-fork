/***************************************************************************

  PDummyUserFcn.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2023 by Andreas Suter                              *
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

#include <TMath.h>

#include "PDummyUserFcn.h"


ClassImp(PDummyUserFcnGlobal)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Here would go all the stuff which will be needed by the global part.
 * For instance specific information which come from a startup XML file.
 * This could be case if muon-stopping information is needed for the
 * calculation, or any other user function specific stuff which is not
 * part of the parameters which will be propagated from the msr-file.
 */
PDummyUserFcnGlobal::PDummyUserFcnGlobal()
{
  // nothing needed here for the dummy
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PDummyUserFcnGlobal::~PDummyUserFcnGlobal()
{
  fPreviousParam.clear();
  fPol.clear();
}

//--------------------------------------------------------------------------
// CalculatePol (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates the polarization function for the dummy user function.
 * For the Dummy this is just an exp(-lambda t), where lambda is comming
 * from the param.
 *
 * \param param the parameter for this user function
 */
void PDummyUserFcnGlobal::CalculatePol(const std::vector<Double_t> &param) const
{
  // param: [0] lambda

  // check that param is new and hence a calculation is needed
  Bool_t newParams = false;
  if (fPreviousParam.size() == 0) {
    for (UInt_t i=0; i<param.size(); i++)
      fPreviousParam.push_back(param[i]);
    newParams = true;
  } else {
    assert(param.size() == fPreviousParam.size());

    for (UInt_t i=0; i<param.size(); i++) {
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

  // calculate the "polarization" in the time interval t = [0, 20] us
  Double_t dt = fTimeStep, t = 0.0;
  Int_t noOfPoints = 20.0 / dt;
  fPol.resize(noOfPoints); // this makes sure that the polarization vector is long enough
  for (Int_t i=0; i<noOfPoints; i++) {
    t = i*dt;
    fPol[i] = exp(-param[0] * i * dt);
  }
}

//--------------------------------------------------------------------------
// GetMagneticField
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PDummyUserFcnGlobal::GetPolValue(const Double_t t) const
{
  Double_t result = -1.0;

  if (t < 0)
    return 1.0;

  if (t >= 20.0)
    return 0.0;

  Int_t idx = (Int_t)(t/fTimeStep);

  if ((idx >=0) && (idx < fPol.size()))
    result = fPol[idx];

  return result;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImp(PDummyUserFcn)

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PDummyUserFcn::~PDummyUserFcn()
{
  if ((fDummyUserFcnGlobal != nullptr) && fInvokedGlobal) {
    delete fDummyUserFcnGlobal;
    fDummyUserFcnGlobal = nullptr;
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
void PDummyUserFcn::SetGlobalPart(std::vector<void*> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) {
    fDummyUserFcnGlobal = new PDummyUserFcnGlobal();
    if (fDummyUserFcnGlobal == nullptr) {
      fValid = false;
      std::cerr << std::endl << ">> PDummyUserFcn::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << std::endl;
    } else if (!fDummyUserFcnGlobal->IsValid()) {
      fValid = false;
      std::cerr << std::endl << ">> PDummyUserFcn::SetGlobalPart(): **ERROR** initialization of global user function object failed, sorry ..." << std::endl;
    } else {
      fValid = true;
      fInvokedGlobal = true;
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<PDummyUserFcnGlobal*>(fDummyUserFcnGlobal);
    }
  } else {
    fValid = true;
    fDummyUserFcnGlobal = (PDummyUserFcnGlobal*)globalPart[fIdxGlobal];
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
Bool_t PDummyUserFcn::GlobalPartIsValid() const
{
  return (fValid && fDummyUserFcnGlobal->IsValid());
}

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PDummyUserFcn::operator()(Double_t t, const std::vector<Double_t> &param) const
{
  // param: [0] lambda
  assert(param.size() == 1);

  // for negative time return polarization == 1
  if (t <= 0.0)
    return 1.0;

  // calculate field if parameter have changed
  fDummyUserFcnGlobal->CalculatePol(param);

  // return the lookup table value
  return fDummyUserFcnGlobal->GetPolValue(t);
}
