/***************************************************************************

  PPhotoMeissner.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013-2021 by Andreas Suter                              *
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

#include <gsl/gsl_sf_bessel.h>

#include "PMusr.h"
#include "PPhotoMeissner.h"


ClassImp(PPhotoMeissner) // for ROOT dictionary

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 */
PPhotoMeissner::PPhotoMeissner()
{
  // read XML startup file
  fRgeHandler = new PRgeHandler("./photoMeissner_startup.xml");

  // check if everything went fine with the startup handler
  if (!fRgeHandler->IsValid()) {
    std::cout << std::endl << ">> PPhotoMeissner::PPhotoMeissner **PANIC ERROR**";
    std::cout << std::endl << ">>   startup handler too unhappy. Will terminate unfriendly, sorry.";
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
PPhotoMeissner::~PPhotoMeissner()
{
  if (fRgeHandler)
    delete fRgeHandler;
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
Double_t PPhotoMeissner::operator()(Double_t t, const std::vector<Double_t> &par) const
{
  // expected parameters: (0) implantation energy (eV), (1) dead layer (nm), (2) Bext (G), (3) lambdaLondon (nm),
  //                      (4) lambdaPhoto (nm), (5) z0 (nm), (6) detector phase (°), [(7) layer thickness]
  assert((par.size() == 7) || (par.size() == 8));

  if (t < 0.0)
    return 1.0;

  Double_t result = 0.0;

  Double_t dz = 1.0; // go in 1A steps
  Double_t zz = 0.0;
  Double_t nn = 0.0;
  Double_t BB = 0.0;
  Double_t gamma = fTwoPi*GAMMA_BAR_MUON;
  int idx = fRgeHandler->GetEnergyIndex(par[0]);

  int done = 0;
  if (par.size() == 7) { // semi-infinite sample
    do {
      nn = fRgeHandler->Get_n(idx, zz);
      BB = FieldHalfSpace(zz, par);
      result += nn*cos(gamma*BB*t+fDegToRad*par[6]);
      zz += dz;
      if (nn == 0.0)
        done++;
    } while (done < 5);
  } else { // film
    do {
      nn = fRgeHandler->Get_n(idx, zz);
      BB = FieldFilm(zz, par);
      result += nn*cos(gamma*BB*t+fDegToRad*par[6]);
      zz += dz;
      if (nn == 0.0)
        done++;
    } while (done < 5);
  }


  return result;
}

//--------------------------------------------------------------------------
// InuMinus() : private
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param nu
 * \param x
 */
Double_t PPhotoMeissner::InuMinus(const Double_t nu, const Double_t x) const
{
  return gsl_sf_bessel_Inu(nu,x) + M_2_PI * sin(nu*M_PI) * gsl_sf_bessel_Knu(nu,x);
}


//--------------------------------------------------------------------------
// FieldFilm() : private
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param par
 */
Double_t PPhotoMeissner::FieldFilm(const Double_t z, const std::vector<Double_t> &par) const
{
  // prevent dividing by zero
  double lamL = par[3];
  double lamP = par[4];
  if (lamL < 1.0e-3) lamL = 1.0e-3;
  if (lamP < 1.0e-3) lamP = 1.0e-3;

  double b = 0;
  double nuPlus = 2.0*par[5]/lamL;  // 2*z0/lamL
  double nuPhoto = 2.0*par[5]/lamP; // 2*z0/lamP
  double beta = exp(-(par[7]/2.0-par[1])/par[5]); // par[7]=layer thickness, par[1]=deadLayer, par[5]=z0

  double NN = InuMinus(nuPlus, nuPhoto*beta)*gsl_sf_bessel_Inu(nuPlus, nuPhoto) -
              InuMinus(nuPlus, nuPhoto)*gsl_sf_bessel_Inu(nuPlus, nuPhoto*beta);

  if (fabs(NN) < 1.0e-2) // far from being good, hence get out of here
    return 5.0*par[2];

  if ((z < par[1]) || (z > par[7]-par[1])) {
    b = 1.0;
  } else {
    b = (InuMinus(nuPlus, nuPhoto*exp(-(z-par[1])/(2.0*par[5])))*(gsl_sf_bessel_Inu(nuPlus, nuPhoto)-gsl_sf_bessel_Inu(nuPlus, nuPhoto*beta))-
         gsl_sf_bessel_Inu(nuPlus, nuPhoto*exp(-(z-par[1])/(2.0*par[5])))*(InuMinus(nuPlus, nuPhoto)-InuMinus(nuPlus, nuPhoto*beta)))/NN;
  }

  return par[2]*b; // Bext*b
}

//--------------------------------------------------------------------------
// FieldHalfSpace() : private
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param par
 */
Double_t PPhotoMeissner::FieldHalfSpace(const Double_t z, const std::vector<Double_t> &par) const
{
  // prevent dividing by zero
  double lamL = par[3];
  double lamP = par[4];
  if (lamL < 1.0e-3) lamL = 1.0e-3;
  if (lamP < 1.0e-3) lamP = 1.0e-3;

  double nuPlus  = 2.0*par[5]/lamL; // 2*z0/lamL
  double nuPhoto = 2.0*par[5]/lamP; // 2*z0/lamP
  double b=0.0, b1=0.0;

  if (z < par[1]) { // z < deadLayer
    b = 1.0;
  } else {
    b1 = gsl_sf_bessel_Inu(nuPlus, nuPhoto);
    assert(b1>0.0);
    b = gsl_sf_bessel_Inu(nuPlus, nuPhoto * sqrt(exp(-(z-par[1])/par[5])))/b1;
  }

  return par[2]*b; // Bext*b
}
