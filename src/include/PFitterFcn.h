/***************************************************************************

  PFitterFcn.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2012 by Andreas Suter                              *
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

#ifndef _PFITTERFCN_H_
#define _PFITTERFCN_H_

#include <vector>

#include "Minuit2/FCNBase.h"

#include "PRunListCollection.h"

/**
 * <p>This is the minuit2 interface function class porviding the function to be optimized (chisq or log max-likelihood).
 */
class PFitterFcn : public ROOT::Minuit2::FCNBase
{
  public:
    PFitterFcn(PRunListCollection *runList, Bool_t useChi2);
    ~PFitterFcn();

    Double_t Up() const { return fUp; }
    Double_t operator()(const std::vector<Double_t> &par) const;

    UInt_t GetTotalNoOfFittedBins() { return fRunListCollection->GetTotalNoOfBinsFitted(); }
    UInt_t GetNoOfFittedBins(const UInt_t idx) { return fRunListCollection->GetNoOfBinsFitted(idx); }
    void CalcExpectedChiSquare(const std::vector<Double_t> &par, Double_t &totalExpectedChisq, std::vector<Double_t> &expectedChisqPerRun);

  private:
    Double_t fUp;     ///< for chisq == 1.0, i.e. errors are 1 std. deviation errors. for log max-likelihood == 0.5, i.e. errors are 1 std. deviation errors (for details see the minuit2 user manual).
    Bool_t fUseChi2;  ///< true = chisq fit, false = log max-likelihood fit
    PRunListCollection *fRunListCollection; ///< pre-processed data to be fitted
};

#endif // _PFITTERFCN_H_
