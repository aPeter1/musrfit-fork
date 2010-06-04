/***************************************************************************

  PUserFcn.cpp

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

#include "PUserFcn.h"

ClassImp(PUserFcn)

//------------------------------------------------------
/**
 * <p>Constructor
 */
PUserFcn::PUserFcn()
{
}

//------------------------------------------------------
/**
 * <p>Destructor
 */
PUserFcn::~PUserFcn()
{
}

//------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 *
 * \param param parameter vector
 */
Double_t PUserFcn::operator()(Double_t t, const std::vector<Double_t> &param) const
{
  // expected parameters: c0, c1, c2, c3

  assert(param.size() == 4);

  return param[0] + param[1]*t + param[2]*t*t + param[3]*t*t*t;
}
