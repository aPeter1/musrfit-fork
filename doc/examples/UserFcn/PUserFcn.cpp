/***************************************************************************

  PUserFcn.cpp

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

#include <iostream>
using namespace std;

#include <cassert>

#include "PUserFcn.h"

ClassImp(PUserFcn)

//------------------------------------------------------
/**
 * <p> user function example: polynome of 3rd order
 *
 * \f[ = \sum_{k=0}^3 c_k t^k \f]
 *
 * <b>meaning of paramValues:</b> \f$c_0\f$, \f$c_1\f$, \f$c_2\f$, \f$c_3\f$
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param param parameter vector
 */
Double_t PUserFcn::operator()(Double_t t, const std::vector<Double_t> &param) const
{
  // expected parameters: c0, c1, c2, c3

  assert(param.size() == 4);

  return param[0] + param[1]*t + param[2]*t*t + param[3]*t*t*t;
}
