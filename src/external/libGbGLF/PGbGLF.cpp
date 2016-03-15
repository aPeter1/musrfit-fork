/***************************************************************************

  PGbGLF.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2016 by Andreas Suter                              *
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
using namespace std;

#include "PMusr.h"
#include "PGbGLF.h"

#define TWO_PI 6.28318530717958647692528676656

ClassImp(PGbGLF)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PGbGLF::PGbGLF()
{
  fWs = 0;
  fWs = gsl_integration_workspace_alloc(65536);
  if (fWs == 0) {
    cerr << "debug> PGbGLF::PGbGLF(): **ERROR** couldn't allocate fWs ..." << endl;
  }

  fIntTab = 0;

  fGslFunParam.wExt = 0.0;
  fGslFunParam.s0   = 0.0;
  fGslFunParam.s1   = 0.0;

  fPrevParam[0] = 0.0;
  fPrevParam[1] = 0.0;
  fPrevParam[2] = 0.0;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PGbGLF::~PGbGLF()
{
  if (fWs)
    gsl_integration_workspace_free(fWs);
  if (fIntTab)
    gsl_integration_qawo_table_free(fIntTab);
}

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
  // param: [0] Bext (G), [1] Delta0 (1/us), [2] DeltaGbG (1/us)
  assert(par.size()==3);

  if (t<0.0)
    return 1.0;
  if (t>fTmax)
    t = fTmax;

  Double_t dval = 0.0;
  Double_t wExt = TWO_PI * GAMMA_BAR_MUON * par[0];
  Double_t s0   = par[1];
  Double_t s1   = par[2];

  Double_t wExt2 = wExt*wExt;
  Double_t s02   = s0*s0;
  Double_t s12   = s1*s1;
  Double_t t2    = t*t;

  Double_t result = 0.0;
  Double_t err = 0.0;

  // if integration table is not already setup, do it
  if (fIntTab == 0) {
    fIntTab = gsl_integration_qawo_table_alloc(wExt, fTmax, GSL_INTEG_SINE, 16);
    if (fIntTab == 0) {
      cerr << ">> PGbGLF::operator(): **ERROR** couldn't invoke qawo table." << endl;
      return 0.0;
    }
    fFun.function = &pz_GbG_2;
    fGslFunParam.wExt = wExt;
    fGslFunParam.s0 = s0;
    fGslFunParam.s1 = s1;
    fFun.params = &fGslFunParam;
  }
  // if parameter set has changed, adopted fFun parameters
  if (fPrevParam[1] != par[1]) {
    fPrevParam[0] = par[0];
    fPrevParam[1] = par[1];
    fPrevParam[2] = par[2];
    fGslFunParam.wExt = wExt;
    fGslFunParam.s0 = s0;
    fGslFunParam.s1 = s1;
    fFun.params = &fGslFunParam;
  }

  // P_z^LF (GbG, 1st part)
  Double_t aa = 1.0+t2*s12;
  dval = 1.0 - 2.0*(s02+s12)/wExt2 + 2.0*(s02+s12*aa)/(wExt2*pow(aa,2.5))*cos(wExt*t)*exp(-0.5*s02*t2/aa);

  // P_z^LF (GbG, 2nd part)
  gsl_integration_qawo_table_set_length(fIntTab, t);
  gsl_integration_qawo(&fFun, 0.0, 1.0e-9, 1.0e-9, 1024, fWs, fIntTab, &result, &err);
  dval += result;

  return dval;
}

//--------------------------------------------------------------------------
// pz_GbG_2 (private)
//--------------------------------------------------------------------------
/**
 * <p>Integrand of the non-analytic part of the GbG-LF polarization
 *
 * \param x
 * \param param
 */

double pz_GbG_2(double x, void *param)
{
  gslFunParam *p = (gslFunParam*) param;

  double s02 = p->s0 * p->s0;
  double s12 = p->s1 * p->s1;
  double x2 = x*x;
  double aa = 1.0+x2*s12;

  return 2.0*(s02*s02+3.0*s12*s12*aa*aa+6.0*s02*s12*aa)/(pow(p->wExt, 3.0)*pow(aa, 4.5))*exp(-0.5*s02*x2/aa);
}
