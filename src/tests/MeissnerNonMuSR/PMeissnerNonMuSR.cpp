/***************************************************************************

  PMeissnerNonMuSR.cpp

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
#include <cmath>

#include "PMeissnerNonMuSR.h"

ClassImp(PMeissnerNonMuSR)

//------------------------------------------------------
/**
 * <p>Constructor
 */
PMeissnerNonMuSR::PMeissnerNonMuSR()
{
}

//------------------------------------------------------
/**
 * <p>Destructor
 */
PMeissnerNonMuSR::~PMeissnerNonMuSR()
{
}

//------------------------------------------------------
/**
 * <p>Example of a thin film Meissner screening user function (\f$B_{\rm G}\f$ versus \f$\langle z \rangle\f$).
 *
 * \f[ B_{\rm G}(\langle z \rangle\) = B_0 \cosh((t-d-[z-d])/\lambda_{ab}) / \cos((t-d)/\lambda_{ab}) \f]
 *
 * - \f$B_{\rm G}\f$ is the screened field value obtained from a Gaussian fit
 * - \f$\langle z \rangle\f$ is the mean muon stopping range obtained from trim.sp.
 *
 * <b>meaning of param:</b> \f$B_0\f$, \f$2 t\f$ = film thickness, \f$d\f$ = dead layer thickness, \f$\lambda_{ab}\f$
 *
 * <b>return:</b> function value
 *
 * \param z mean distance from the surface
 * \param param parameter vector
 */
Double_t PMeissnerNonMuSR::operator()(Double_t z, const std::vector<Double_t> &param) const
{
  // expected parameters: Bext, t=thickness, deadLayer, lambda_ab

  assert(param.size() == 4);

  // if z<deadLayer or z>2tt-deadLayer
  Double_t tt = param[1]/2.0;
  Double_t result = param[0];
  if ((z > param[2]) && (z < param[1]-param[2]))
    result = param[0]*cosh((tt-z)/param[3])/cosh((tt-param[2])/param[3]);

  return result;
}
