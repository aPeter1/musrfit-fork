/***************************************************************************

  PRelax.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
 *   andreas.suter@psi.c                                                   *
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

#include <iostream>
using namespace std;

#include <cassert>
#include <cstdlib>
#include <cmath>

#include "PRelax.h"

ClassImp(PExp)

//------------------------------------------------------------------------------------
/**
 * <p> Constructor.
 */
PExp::PExp()
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
PExp::~PExp()
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
void PExp::SetGlobalPart(vector<void *> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) { // global user function not present, invoke it
    fGlobalUserFcn = new PExpGlobal();
    if (fGlobalUserFcn == 0) { // global user function object couldn't be invoked -> error
      fValid = false;
      cerr << endl << ">> PExp::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << endl;
    } else {  // global user function object could be invoked -> resize to global user function vector and keep the pointer to the corresponding object
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<PExpGlobal*>(fGlobalUserFcn);
      fValid = true;
      fInvokedGlobal = true;
    }
  } else { // global user function already present hence just retrieve a pointer to it
    fValid = true;
    fGlobalUserFcn = (PExpGlobal*)globalPart[fIdxGlobal];
  }
}

//------------------------------------------------------------------------------------
/**
 * <p> Used to check if the user function is OK.
 *
 * <b>return:</b> true if both, the user function and the global user function object are valid
 */
Bool_t PExp::GlobalPartIsValid() const
{
  return (fValid && fGlobalUserFcn->IsValid());
}

//------------------------------------------------------------------------------------
/**
 * <p> user function example: global exponential
 *
 * \f[ = exp(-t \lambda) \f]
 *
 * <b>meaning of paramValues:</b> \f$\lambda\f$ depol. rate
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param param parameter vector
 */
Double_t PExp::operator()(Double_t t, const std::vector<Double_t> &param) const
{
  // expected parameters: lambda

  assert(param.size() == 1);
  assert(fGlobalUserFcn);

  // call the global user function object
  fGlobalUserFcn->Calc(param);

  return exp(-t*param[0]);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImp(PExpGlobal)

//------------------------------------------------------------------------------------
/**
 * <p> Constructor
 */
PExpGlobal::PExpGlobal()
{
  fValid = true;
  fRandom = 0.0;
}

//------------------------------------------------------------------------------------
/**
 * <p> Destructor
 */
PExpGlobal::~PExpGlobal()
{
  fPrevParam.clear();
}

//------------------------------------------------------------------------------------
/**
 * <p> Check if the parameter vector has changed to the previous one, and if so,
 * do some global user function calculation (here calculate only a stupid random
 * number).
 *
 * \param param fit parameter vector
 */
void PExpGlobal::Calc(const std::vector<Double_t> &param)
{
  // check if previous parameters where ever fed
  if (fPrevParam.size() == 0) {
    fPrevParam = param;
    fRandom = rand() / (RAND_MAX+1.0);
    cout << endl << "debug> PExpGlobal::Calc(): fRandom = " << fRandom << " (first time)";
  }

  // check if param has changed, i.e. something needs to be done
  Bool_t newParam = false;
  for (UInt_t i=0; i<param.size(); i++) {
    if (param[i] != fPrevParam[i]) {
      newParam = true;
      break;
    }
  }

  if (newParam) {
    // do the global calculations (here only a stupid random number)
    fRandom = rand() / (RAND_MAX+1.0);
    // feed the previous parameter vector
    for (UInt_t i=0; i<param.size(); i++)
      fPrevParam[i] = param[i];
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImp(PGauss)

//------------------------------------------------------------------------------------
/**
 * <p> Constructor.
 */
PGauss::PGauss()
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
PGauss::~PGauss()
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
void PGauss::SetGlobalPart(vector<void *> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) {
    fGlobalUserFcn = new PGaussGlobal();
    if (fGlobalUserFcn == 0) {
      fValid = false;
      cerr << endl << ">> PGauss::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << endl;
    } else {
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<PGaussGlobal*>(fGlobalUserFcn);
      fValid = true;
      fInvokedGlobal = true;
    }
  } else {
    fValid = true;
    fGlobalUserFcn = (PGaussGlobal*)globalPart[fIdxGlobal];
  }
}

//------------------------------------------------------------------------------------
/**
 * <p> Used to check if the user function is OK.
 *
 * <b>return:</b> true if both, the user function and the global user function object are valid
 */
Bool_t PGauss::GlobalPartIsValid() const
{
  return (fValid && fGlobalUserFcn->IsValid());
}

//------------------------------------------------------------------------------------
/**
 * <p> user function example: global Gaussian
 *
 * \f[ = exp(-1/2 (t sigma)^2) \f]
 *
 * <b>meaning of paramValues:</b> \f$\sigma\f$ depol. rate
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param param parameter vector
 */
Double_t PGauss::operator()(Double_t t, const std::vector<Double_t> &param) const
{
  // expected parameters: sigma

  assert(param.size() == 1);
  assert(fGlobalUserFcn);

  fGlobalUserFcn->Calc(param);

  return exp(-0.5*pow(t*param[0],2.0));
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImp(PGaussGlobal)

//------------------------------------------------------------------------------------
/**
 * <p> Constructor
 */
PGaussGlobal::PGaussGlobal()
{
  fValid = true;
  fRandom = 0.0;
}

//------------------------------------------------------------------------------------
/**
 * <p> Destructor
 */
PGaussGlobal::~PGaussGlobal()
{
  fPrevParam.clear();
}

//------------------------------------------------------------------------------------
/**
 * <p> Check if the parameter vector has changed to the previous one, and if so,
 * do some global user function calculation (here calculate only a stupid random
 * number).
 *
 * \param param fit parameter vector
 */
void PGaussGlobal::Calc(const std::vector<Double_t> &param)
{
  // check if previous parameters where ever fed
  if (fPrevParam.size() == 0) {
    fPrevParam = param;
    fRandom = rand() / (RAND_MAX+1.0);
    cout << endl << "debug> PGaussGlobal::Calc(): fRandom = " << fRandom << " (first time)";
  }

  // check if param has changed, i.e. something needs to be done
  Bool_t newParam = false;
  for (UInt_t i=0; i<param.size(); i++) {
    if (param[i] != fPrevParam[i]) {
      newParam = true;
      break;
    }
  }

  if (newParam) {
    // do the global calculations (here only a stupid random number)
    fRandom = rand() / (RAND_MAX+1.0);
    // feed the previous parameter vector
    for (UInt_t i=0; i<param.size(); i++)
      fPrevParam[i] = param[i];
  }
}
