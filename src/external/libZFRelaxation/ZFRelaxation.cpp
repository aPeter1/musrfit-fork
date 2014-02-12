/***************************************************************************

  ZFRelaxation.cpp

  Author: Bastian M. Wojek

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

#define TWO_PI 6.28318530717958623
#define DEG_TO_RAD 0.0174532925199432955

#include "ZFRelaxation.h"

#include <cassert>
#include <cmath>
#include <vector>

using namespace std;

ClassImp(ZFMagGss)
ClassImp(ZFMagExp)

//--------------------------------------------------------------------------
// ZFMagGss::operator()
//--------------------------------------------------------------------------
/**
 * <p>Method returning the function value at a given time for a given set of parameters.
 *
 * \param t time \htmlonly (&mu;s) \endhtmlonly \latexonly ($\mu\mathrm{s}$) \endlatexonly
 * \param par parameters [\htmlonly &alpha; (1), &nu; (MHz), &sigma;<sub>T</sub>; (&mu;s<sup>-1</sup>), &sigma;<sub>L</sub>; (&mu;s<sup>-1</sup>)\endhtmlonly \latexonly $\alpha~(1)$, $\nu~(\mathrm{MHz})$, $\sigma_{\mathrm{T}}~(\mu\mathrm{s}^{-1})$, $\sigma_{\mathrm{L}}~(\mu\mathrm{s}^{-1})$ \endlatexonly]
 */
double ZFMagGss::operator()(double t, const vector<double> &par) const {
  assert(par.size()==4);
  double w(TWO_PI * par[1]), sst(par[2]*par[2]*t);
  return par[0] * (cos(w*t) - sst/w*sin(w*t)) * exp(-0.5*sst*t) + (1.0-par[0]) * exp(-0.5*par[3]*par[3]*t*t);
}

//--------------------------------------------------------------------------
// ZFMagExp::operator()
//--------------------------------------------------------------------------
/**
 * <p>Method returning the function value at a given time for a given set of parameters.
 *
 * \param t time \htmlonly (&mu;s) \endhtmlonly \latexonly ($\mu\mathrm{s}$) \endlatexonly
 * \param par parameters [\htmlonly &alpha; (1), &nu; (MHz), <i>a</i><sub>T</sub>; (&mu;s<sup>-1</sup>), <i>a</i><sub>L</sub>; (&mu;s<sup>-1</sup>)\endhtmlonly \latexonly $\alpha~(1)$, $\nu~(\mathrm{MHz})$, $a_{\mathrm{T}}~(\mu\mathrm{s}^{-1})$, $a_{\mathrm{L}}~(\mu\mathrm{s}^{-1})$ \endlatexonly]
 */
double ZFMagExp::operator()(double t, const vector<double> &par) const {
  assert(par.size()==4);
  double w(TWO_PI * par[1]);
  return par[0] * (cos(w*t) - par[2]/w*sin(w*t)) * exp(-par[2]*t) + (1.0-par[0]) * exp(-par[3]*t);
}

//--------------------------------------------------------------------------
// UniaxialStatGssKT::UniaxialStatGssKT()
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 */
UniaxialStatGssKT::UniaxialStatGssKT() {
  fIntFirst = new TFirstUniaxialGssKTIntegral();
  fIntSecond = new TSecondUniaxialGssKTIntegral();
}

//--------------------------------------------------------------------------
// UniaxialStatGssKT::~UniaxialStatGssKT()
//--------------------------------------------------------------------------
/**
 * <p>Destructor
 */
UniaxialStatGssKT::~UniaxialStatGssKT() {
  delete fIntFirst; fIntFirst = 0;
  delete fIntSecond; fIntSecond = 0;
}

//--------------------------------------------------------------------------
// UniaxialStatGssKT::operator()
//--------------------------------------------------------------------------
/**
 * <p>Method returning the function value at a given time for a given set of parameters.
 *
 * \param t time \htmlonly (&mu;s) \endhtmlonly \latexonly ($\mu\mathrm{s}$) \endlatexonly
 * \param par parameters [\htmlonly &sigma;<sub>1</sub> (&mu;s<sup>-1</sup>), &sigma;<sub>2</sub> (&mu;s<sup>-1</sup>), &Theta; (&deg;)\endhtmlonly \latexonly $\sigma_{1}~(\mu\mathrm{s}^{-1})$, $\sigma_{2}~(\mu\mathrm{s}^{-1})$, $\Theta~(^{\circ})$ \endlatexonly]
 */
double UniaxialStatGssKT::operator()(double t, const vector<double> &par) const {
  assert(par.size() == 3);

  if (t < 0.0)
    return 1.0;

  // set the parameters for the integrations
  vector<double> intParam(3);
  intParam[0] = par[0];
  intParam[1] = par[1];
  intParam[2] = t;

  if (((fabs(par[1]) < 1.0e-5) && (fabs(par[0]) > 1.0e-2)) || (fabs(par[0]/par[1]) > 1000.0)) {
    cerr << endl;
    cerr << ">> UniaxialStatGssKT: WARNING Ratio sigma1/sigma2 unreasonably large! Set it internally to 1000. Please check your parameters!";
    cerr << endl;
    intParam[1] = 1.0e-3*intParam[0];
  } else if (fabs(par[1]) < 1.0e-10) {
    cerr << endl;
    cerr << ">> UniaxialStatGssKT: WARNING sigma2 too small! Set it internally to 1.0E-10. Please check your parameters!";
    cerr << endl;
    intParam[1] = 1.0e-10;
  }

  double eps(intParam[0]*intParam[0]/(intParam[1]*intParam[1]) - 1.0);

  // check if the anisotropy is so small that the normal statGssKT can be used
  if (fabs(eps) < 1.0e-6) {
    double sSqtSq(intParam[0]*intParam[0]*t*t);
    return 0.333333333333333 + 0.666666666666667*(1.0 - sSqtSq)*exp(-0.5*sSqtSq);
  }

  double sinSqTh(pow(sin(DEG_TO_RAD*par[2]), 2.0)), cosSqTh(1.5*pow(cos(DEG_TO_RAD*par[2]), 2.0) - 0.5);

  // check if the limit sigma2 >> sigma1 applies
  if (eps < -0.999999) {
    return 0.5*sinSqTh + cosSqTh;
  }

  // otherwise calculate the full depolarization function
  double sqrtOnePlusEps(sqrt(1.0 + eps));
  double avg;
  if (eps > 0.0) {
    avg = 1.0 - sqrtOnePlusEps/eps*(sqrtOnePlusEps - log(sqrt(eps) + sqrtOnePlusEps)/sqrt(eps));
  } else {
    avg = 1.0 - sqrtOnePlusEps/eps*(sqrtOnePlusEps - asin(sqrt(-eps))/sqrt(-eps));
  }

  return sinSqTh*(0.5 + sqrtOnePlusEps*fIntSecond->IntegrateFunc(0.0, 1.0, intParam)) + cosSqTh*(avg + sqrtOnePlusEps*fIntFirst->IntegrateFunc(0.0, 1.0, intParam));

}
