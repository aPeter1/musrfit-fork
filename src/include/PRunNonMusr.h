/***************************************************************************

  PRunNonMusr.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2021 by Andreas Suter                              *
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

#ifndef _PRUNNONMUSR_H_
#define _PRUNNONMUSR_H_

#include "PMusr.h"
#include "PRunBase.h"

/**
 * <p>Class handling the non-muSR fit type
 */
class PRunNonMusr : public PRunBase
{
  public:
    PRunNonMusr();
    PRunNonMusr(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag);
    virtual ~PRunNonMusr();

    virtual Double_t CalcChiSquare(const std::vector<Double_t>& par);
    virtual Double_t CalcChiSquareExpected(const std::vector<Double_t>& par);
    virtual Double_t CalcMaxLikelihood(const std::vector<Double_t>& par);
    virtual void CalcTheory();

    virtual UInt_t GetNoOfFitBins();

    virtual void SetFitRangeBin(const TString fitRange) {}

    virtual UInt_t GetXIndex();
    virtual UInt_t GetYIndex();

  protected:
    virtual Bool_t PrepareData();
    virtual Bool_t PrepareFitData();
    virtual Bool_t PrepareViewData();

  private:
    PRawRunData *fRawRunData; ///< raw run data handler

    UInt_t fNoOfFitBins;    ///< number of bins to be be fitted
    Int_t  fPacking;        ///< packing for this particular run. Either given in the RUN- or GLOBAL-block.

    Int_t fStartTimeBin;    ///< bin at which the fit starts
    Int_t fEndTimeBin;      ///< bin at which the fit ends
};

#endif // _PRUNNONMUSR_H_
