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
 * <p>Example  of a user defined function. This example implements a 3rd order polynom
 * \f[ = \sum_{k=0}^3 c_k t^k\f]
 *
 * <b>meaning of paramValues:</b> \f$c_0\f$, \f$c_1\f$, \f$c_2\f$, \f$c_3\f$
 *
 * <b>return:</b> function value
 *
 * \param z mean distance from the surface
 * \param param parameter vector
 */
Double_t PMeissnerNonMuSR::operator()(Double_t z, const std::vector<Double_t> &param) const
{
  // expected parameters: Bext, t=thickness/2, deadLayer, lambda_ab

  assert(param.size() == 4);

  // if z<deadLayer or z>2t-deadLayer
  Double_t result = param[0];
  if ((z > param[2]) && (z < 2.0*param[1]-param[2]))
    result = param[0]*cosh((param[1]-param[2]-(z-param[2]))/param[3])/cosh((param[1]-param[2])/param[3]);


  return result;
}
