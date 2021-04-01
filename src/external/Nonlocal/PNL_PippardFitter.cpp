/***************************************************************************

  PNL_PippardFitter.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2021 by Andreas Suter                              *
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

#include "PNL_PippardFitter.h"

#define GAMMA_MU   0.0851615503527
#define DEGREE2RAD 0.0174532925199

ClassImp(PNL_PippardFitterGlobal)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PNL_PippardFitterGlobal::PNL_PippardFitterGlobal()
{
  // read XML startup file
  char startup_path_name[128];
  TSAXParser *saxParser = new TSAXParser();
  PNL_StartupHandler *fStartupHandler = new PNL_StartupHandler();
  strcpy(startup_path_name, fStartupHandler->GetStartupFilePath().Data());
  saxParser->ConnectToHandler("PNL_StartupHandler", fStartupHandler);
  //Int_t status = saxParser->ParseFile(startup_path_name);
  // parsing the file as above seems to lead to problems in certain environments;
  // use the parseXmlFile function instead (see PUserFcnBase.cpp for the definition)
  Int_t status = parseXmlFile(saxParser, startup_path_name);
  // check for parse errors
  if (status) { // error
    std::cout << std::endl << ">> PNL_PippardFitterGlobal::PNL_PippardFitterGlobal: **WARNING** Reading/parsing nonlocal_startup.xml failed.";
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
    std::cout << std::endl << ">> PNL_PippardFitterGlobal::PNL_PippardFitterGlobal **PANIC ERROR**";
    std::cout << std::endl << ">>   startup handler too unhappy. Will terminate unfriendly, sorry.";
    std::cout << std::endl;
    fValid = false;
  }

  fFourierPoints = fStartupHandler->GetFourierPoints();

  // load all the TRIM.SP rge-files
  fRgeHandler = new PRgeHandler("./nonlocal_startup.xml");
  if (!fRgeHandler->IsValid()) {
    std::cout << std::endl << ">> PNL_PippardFitterGlobal::PNL_PippardFitterGlobal **PANIC ERROR**";
    std::cout << std::endl << ">>  rge data handler too unhappy. Will terminate unfriendly, sorry.";
    std::cout << std::endl;
    fValid = false;
  }

  fPlanPresent = false;
  fFieldq = 0;
  fFieldB = 0;
  fShift  = 0;

  f_nn = 4.0;

  f_dx = 0.02;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PNL_PippardFitterGlobal::~PNL_PippardFitterGlobal()
{
  fPreviousParam.clear();

  if (fPlanPresent) {
    fftw_destroy_plan(fPlan);
  }
  if (fFieldq) {
    fftw_free(fFieldq);
    fFieldq = 0;
  }

  if (fFieldB) {
    fftw_free(fFieldq);
    fFieldB = 0;
  }

  if (fRgeHandler) {
    delete fRgeHandler;
    fRgeHandler = 0;
  }
  if (fStartupHandler) {
    delete fStartupHandler;
    fStartupHandler = 0;
  }
}

//--------------------------------------------------------------------------
// CalculateField (public)
//--------------------------------------------------------------------------
/**
 *
 */
void PNL_PippardFitterGlobal::CalculateField(const std::vector<Double_t> &param) const
{
  // param: [0] energy, [1] temp, [2] thickness, [3] meanFreePath, [4] xi0, [5] lambdaL, [6] Bext, [7] phase, [8] dead-layer

  // check that param are new and hence a calculation is needed (except the energy and phase, which is not needed here)
  Bool_t newParams = false;
  if (fPreviousParam.size() == 0) {
    for (UInt_t i=0; i<param.size(); i++)
      fPreviousParam.push_back(param[i]);
    newParams = true;
  } else {
    assert(param.size() == fPreviousParam.size());

    for (UInt_t i=0; i<param.size(); i++) {
      if ((i == 0) || (i >= 6)) // for energy, Bext, phase, and dead-layer, nothing to be done here
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

  f_dz = XiP_T(param[4], param[3], param[1])*TMath::TwoPi()/fFourierPoints/f_dx; // see lab-book p.137, used for specular reflection boundary conditions (default)

  // check if it is necessary to allocate memory
  if (fFieldq == 0) {
    fFieldq = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * fFourierPoints);
    if (fFieldq == 0) {
      std::cout << std::endl << "PPippard::CalculateField(): **ERROR** couldn't allocate memory for fFieldq";
      std::cout << std::endl;
      return;
    }
  }
  if (fFieldB == 0) {
    fFieldB = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * fFourierPoints);
    if (fFieldB == 0) {
      std::cout << std::endl << "PPippard::CalculateField(): **ERROR** couldn't allocate memory for fFieldB";
      std::cout << std::endl;
      return;
    }
  }

  // calculate the prefactor of the reduced kernel
  Double_t xiP = XiP_T(param[4], param[3], param[1]);
  Double_t preFactor = pow(xiP/(LambdaL_T(param[5], param[1])),2.0)*xiP/param[4];

  // calculate the fFieldq vector, which is x/(x^2 + alpha k(x)), with alpha = xiP(T)^3/(lambdaL(T)^2 xiP(0)), and
  // k(x) = 3/2 [(1+x^2) arctan(x) - x]/x^3, see lab-book p.137
  Double_t x;
  fFieldq[0][0] = 0.0;
  fFieldq[0][1] = 0.0;
  for (Int_t i=1; i<fFourierPoints; i++) {
    x = i * f_dx;
    fFieldq[i][0] = x/(pow(x,2.0)+preFactor*(1.5*((1.0+pow(x,2.0))*atan(x)-x)/pow(x,3.0)));
    fFieldq[i][1] = 0.0;
  }

  // Fourier transform
  if (!fPlanPresent) {
    fPlan = fftw_plan_dft_1d(fFourierPoints, fFieldq, fFieldB, FFTW_FORWARD, FFTW_ESTIMATE);
    fPlanPresent = true;
  }

  fftw_execute(fPlan);

  // normalize fFieldB
  Double_t norm = 0.0;
  fShift=0;
  for (Int_t i=0; i<fFourierPoints/2; i++) {
    if (fabs(fFieldB[i][1]) > fabs(norm)) {
      norm = fFieldB[i][1];
      fShift = i;
    }
  }

  for (Int_t i=0; i<fFourierPoints; i++) {
    fFieldB[i][1] /= norm;
  }

  if (param[2] < fFourierPoints/2.0*f_dz) {
    // B(z) = b(z)+b(D-z)/(1+b(D)) is the B(z) result
    Int_t idx = (Int_t)(param[2]/f_dz);
    norm = 1.0 + fFieldB[idx+fShift][1];
    for (Int_t i=0; i<fFourierPoints; i++) {
      fFieldB[i][0] = 0.0;
    }
    for (Int_t i=fShift; i<idx+fShift; i++) {
      fFieldB[i][0] = (fFieldB[i][1] + fFieldB[idx-i+2*fShift][1])/norm;
    }
    for (Int_t i=0; i<fFourierPoints; i++) {
      fFieldB[i][1] = fFieldB[i][0];
    }
  }
}

//--------------------------------------------------------------------------
// GetMagneticField
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PNL_PippardFitterGlobal::GetMagneticField(const Double_t z) const
{
  Double_t result = -1.0;

  if (fFieldB == 0)
    return -1.0;

  if (z <= 0.0)
    return 1.0;

  if (z > f_dz*fFourierPoints/2.0)
    return 0.0;

  Int_t bin = (Int_t)(z/f_dz);

  result = fFieldB[bin+fShift][1];

  return result;
}

//--------------------------------------------------------------------------
// DeltaBCS
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PNL_PippardFitterGlobal::DeltaBCS(const Double_t t) const
{
  Double_t result = 0.0;

  // reduced temperature table
  Double_t tt[] = {1, 0.98, 0.96, 0.94, 0.92, 0.9, 0.88, 0.86, 0.84, 0.82,
                   0.8, 0.78, 0.76, 0.74, 0.72, 0.7, 0.68, 0.66, 0.64, 0.62,
                   0.6, 0.58, 0.56, 0.54, 0.52, 0.5, 0.48, 0.46, 0.44, 0.42,
                   0.4, 0.38, 0.36, 0.34, 0.32, 0.3, 0.28, 0.26, 0.24, 0.22,
                   0.2, 0.18, 0.16, 0.14};

  // gap table from Muehlschlegel
  Double_t ee[] = {0, 0.2436, 0.3416, 0.4148, 0.4749, 0.5263, 0.5715, 0.6117,
                   0.648, 0.681, 0.711, 0.7386, 0.764, 0.7874, 0.8089, 0.8288,
                   0.8471, 0.864, 0.8796, 0.8939, 0.907, 0.919, 0.9299, 0.9399,
                   0.9488, 0.9569, 0.9641, 0.9704, 0.976, 0.9809, 0.985, 0.9885,
                   0.9915, 0.9938, 0.9957, 0.9971, 0.9982, 0.9989, 0.9994, 0.9997,
                   0.9999, 1, 1, 1, 1};

  if (t>1.0)
    result = 0.0;
  else if (t<0.14)
    result = 1.0;
  else {
     // find correct bin for t
     UInt_t i=0;
     for (i=0; i<sizeof(tt); i++) {
       if (tt[i]<=t) break;
     }
     // interpolate linear between 2 bins
     result = ee[i]-(tt[i]-t)*(ee[i]-ee[i-1])/(tt[i]-tt[i-1]);
  }

  return result;
}

//--------------------------------------------------------------------------
// LambdaL_T
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PNL_PippardFitterGlobal::LambdaL_T(const Double_t lambdaL, const Double_t t) const
{
  return lambdaL/sqrt(1.0-pow(t, f_nn));
}

//--------------------------------------------------------------------------
// XiP_T
//--------------------------------------------------------------------------
/**
 * <p> Approximated xi_P(T). The main approximation is that (lamdaL(T)/lambdaL(0))^2 = 1/(1-t^2). This way
 * xi_P(T) is close the the BCS xi_BCS(T).
 */
Double_t PNL_PippardFitterGlobal::XiP_T(const Double_t xi0, const Double_t meanFreePath, Double_t t) const
{
  if (t>0.96)
    t=0.96;

  Double_t J0T = DeltaBCS(t)/(1.0-pow(t,2.0)) * tanh(0.881925 * DeltaBCS(t) / t);

  return xi0*meanFreePath/(meanFreePath*J0T+xi0);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImp(PNL_PippardFitter)

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PNL_PippardFitter::~PNL_PippardFitter()
{
  if ((fPippardFitterGlobal != 0) && fInvokedGlobal) {
    delete fPippardFitterGlobal;
    fPippardFitterGlobal = 0;
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
void PNL_PippardFitter::SetGlobalPart(std::vector<void*> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) {
    fPippardFitterGlobal = new PNL_PippardFitterGlobal();
    if (fPippardFitterGlobal == nullptr) {
      fValid = false;
      std::cerr << std::endl << ">> PNL_PippardFitter::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << std::endl;
    } else if (!fPippardFitterGlobal->IsValid()) {
      fValid = false;
      std::cerr << std::endl << ">> PNL_PippardFitter::SetGlobalPart(): **ERROR** initialization of global user function object failed, sorry ..." << std::endl;
    } else {
      fValid = true;
      fInvokedGlobal = true;
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<PNL_PippardFitterGlobal*>(fPippardFitterGlobal);
    }
  } else {
    fValid = true;
    fPippardFitterGlobal = (PNL_PippardFitterGlobal*)globalPart[fIdxGlobal];
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
Bool_t PNL_PippardFitter::GlobalPartIsValid() const
{
  return (fValid && fPippardFitterGlobal->IsValid());
}

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PNL_PippardFitter::operator()(Double_t t, const std::vector<Double_t> &param) const
{
  // param: [0] energy, [1] temp, [2] thickness, [3] meanFreePath, [4] xi0, [5] lambdaL, [6] Bext, [7] phase, [8] dead-layer
  //        optionally [9] nn, where nn is the exponent in lambda = lambda_L / sqrt(1-t^nn). Default is nn=4.
  assert((param.size() == 9) || (param.size() == 10));

  if (param.size() == 10) // nn given, hence set it
    fPippardFitterGlobal->SetTempExponent(param[9]);

  // for negative time return polarization == 1
  if (t <= 0.0)
    return 1.0;

  // calculate field if parameter have changed
  fPippardFitterGlobal->CalculateField(param);
  Int_t energyIndex = fPippardFitterGlobal->GetEnergyIndex(param[0]);
  if (energyIndex == -1) { // energy not found
    std::cerr << std::endl << ">> PNL_PippardFitter::operator() energy " << param[0] << " not found. Will terminate." << std::endl;
    assert(0);
  }

  // calcualte polarization
  Bool_t done = false;
  Double_t pol = 0.0, dPol = 0.0;
  Double_t z=0.0;
  Int_t terminate = 0;
  Double_t dz = 1.0;

  do {
    if (z < param[8]) { // z < dead-layer
      dPol = fPippardFitterGlobal->GetMuonStoppingDensity(energyIndex, z) * cos(GAMMA_MU * param[6] * t + param[7] * DEGREE2RAD);
    } else {
      dPol = fPippardFitterGlobal->GetMuonStoppingDensity(energyIndex, z) * cos(GAMMA_MU * param[6] * fPippardFitterGlobal->GetMagneticField(z-param[8]) * t + param[7] * DEGREE2RAD);
    }
    z += dz;
    pol += dPol;

    // change in polarization is very small hence start termination counting
    if (fabs(dPol) < 1.0e-9) {
      terminate++;
    } else {
      terminate = 0;
    }

    if (terminate > 10) // polarization died out hence one can stop
      done = true;
  } while (!done);

  return pol*dz;
}
