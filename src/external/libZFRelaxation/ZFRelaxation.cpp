/***************************************************************************

  ZFRelaxation.cpp

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
