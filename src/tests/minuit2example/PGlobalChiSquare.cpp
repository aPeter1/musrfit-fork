/***************************************************************************

  PGlobalChiSquare.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
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

#include <math.h>

#include <iostream>

#include "PGlobalChiSquare.h"

namespace ROOT {
  namespace Minuit2 {

//--------------------------------------------------------------------------
/**
 * <p>
 */
PGlobalChiSquare::PGlobalChiSquare(PRunList &runList)
{
  fRunList = runList;
}

//--------------------------------------------------------------------------
/**
 * <p>
 */
PGlobalChiSquare::~PGlobalChiSquare()
{
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param par
 */
double PGlobalChiSquare::operator()(const std::vector<double>& par) const
{
  double chi2 = 0.0;
  double funcValue;
  double diff;

  // calculate chi2 for the given model
  for (unsigned int i=0; i<fRunList.size(); i++) { // run loop
    for (unsigned int j=0; j<fRunList[i].fTime.size(); j++) { // data loop
      funcValue = par[i]*exp(-par[2]*fRunList[i].fTime[j]); // par[i] since par[0] = ampl of run 0, etc.
      diff = funcValue - fRunList[i].fValue[j];
      chi2 += diff*diff/(fRunList[i].fError[j]*fRunList[i].fError[j]);
    }
  }

  return chi2;
}

  } // namespace Minuit2
} // namespace ROOT

