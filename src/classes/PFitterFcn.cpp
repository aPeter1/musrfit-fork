/***************************************************************************

  PFitterFcn.cpp

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

#include "PFitterFcn.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runList
 * \param fitType
 */
PFitterFcn::PFitterFcn(PRunListCollection *runList, bool useChi2)
{
  fUseChi2 = useChi2;

  if (fUseChi2)
    fUp = 1.0;
  else
    fUp = 0.5;

  fRunListCollection = runList;
  
  // check if max likelihood is used together with asymmetry/RRF/nonMusr data.
  // if yes place a warning since this option is not implemented and a fall back
  // to chi2 will be used.
  if (!fUseChi2) {
    if ((fRunListCollection->GetNoOfAsymmetry() > 0) ||
        (fRunListCollection->GetNoOfRRF() > 0) ||
        (fRunListCollection->GetNoOfNonMusr() > 0)) {
      cout << endl << "**WARNING**: Maximum Log Likelihood Fit is only implemented for Single Histogram Fit";
      cout << endl << "             Will fall back to Chi Square Fit.";
      cout << endl;
    }    
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 */
PFitterFcn::~PFitterFcn()
{
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param par
 */
double PFitterFcn::operator()(const std::vector<double>& par) const
{
  double value = 0.0;

  if (fUseChi2) { // chi square
    value += fRunListCollection->GetSingleHistoChisq(par);
    value += fRunListCollection->GetAsymmetryChisq(par);
    value += fRunListCollection->GetRRFChisq(par);
    value += fRunListCollection->GetNonMusrChisq(par);
  } else { // max likelihood
    value += fRunListCollection->GetSingleHistoMaximumLikelihood(par);
    value += fRunListCollection->GetAsymmetryMaximumLikelihood(par);
    value += fRunListCollection->GetRRFMaximumLikelihood(par);
    value += fRunListCollection->GetNonMusrMaximumLikelihood(par);
  }

// cout << endl;
// for (unsigned int i=0; i<par.size(); i++) {
//   cout << par[i] << ", ";
// }
//cout << endl << "chisq = " << value;
// cout << endl << "------" << endl;

  return value;
}

