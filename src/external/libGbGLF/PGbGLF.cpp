/***************************************************************************

  PGbGLF.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2021 by Andreas Suter                              *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cassert>
#include <iostream>
#include <cmath>

#include "PMusr.h"
#include "PGbGLF.h"

#define TWO_PI 6.28318530717958647692528676656

ClassImp(PGbGLF)

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 * <p>Method returning the function value at a given time for a given set of parameters.
 *
 * \param t time
 * \param par
 */
Double_t PGbGLF::operator()(Double_t t, const std::vector<Double_t> &par) const
{
  // param: [0] Bext (G), [1] Delta0 (1/us), [2] Rb (1)
  assert(par.size()==3);

  if (t<0.0)
    return 1.0;

  Double_t dval = 0.0;
  Double_t wExt = TWO_PI * GAMMA_BAR_MUON * par[0];
  Double_t s0   = par[1];
  Double_t s1   = s0*par[2]; // sigma0 * Rb

  Double_t wExt2 = wExt*wExt;
  Double_t s02   = s0*s0;
  Double_t s12   = s1*s1;
  Double_t t2    = t*t;

  if (par[0] < 3.0) { // take ZF solution for Bext <= 3G. This is good enough
    Double_t aa = 1.0 + s12*t2;
    dval = 0.333333333333333333 + 0.666666666666667 / pow(aa, 1.5)*(1.0-s02*t2/aa)*exp(-0.5*s02*t2/aa);
  } else {
    // P_z^LF (GbG, 1st part)
    Double_t aa = 1.0+t2*s12;
    dval = 1.0 - 2.0*(s02+s12)/wExt2 + 2.0*(s02+s12*aa)/(wExt2*pow(aa,2.5))*cos(wExt*t)*exp(-0.5*s02*t2/aa);

    // P_z^LF (GbG, 2nd part)
    Double_t dt = t;
    Int_t n=1;
    Double_t sumT = dt * pz_GbG_2(t, par) * 0.5;
    Double_t sumM = 0.0;
    Double_t tt = 0.0;
    do {
      sumM = 0.0;
      for (Int_t i=0; i<n-1; i++) {
        tt = (static_cast<Double_t>(i) + 0.5) * dt;
        sumM += pz_GbG_2(tt, par);
      }
      sumM *= dt;
      sumT = (sumT + sumM)*0.5;
      dt /= 2.0;
      n *= 2;
    } while ((fabs(sumT-sumM) > 1.0e-5) && (n < 8192));

    dval += sumT;
  }

  return dval;
}

//--------------------------------------------------------------------------
// pz_GbG_2 (private)
//--------------------------------------------------------------------------
/**
 * <p>Integrand of the non-analytic part
 *
 * \param t time
 */
Double_t PGbGLF::pz_GbG_2(Double_t t, const std::vector<Double_t> &par) const
{
  Double_t wExt = TWO_PI * GAMMA_BAR_MUON * par[0];
  Double_t s0   = par[1];
  Double_t s1   = s0*par[2]; // sigma0 * Rb

  Double_t s02   = s0*s0;
  Double_t s12   = s1*s1;
  Double_t t2    = t*t;
  Double_t aa = 1.0+t2*s12;

  return 2.0*(s02*s02+3.0*s12*s12*aa*aa+6.0*s02*s12*aa)/(pow(wExt,3.0)*pow(aa,4.5))*exp(-0.5*s02*t2/aa)*sin(wExt*t);
}
