/***************************************************************************

  PRunMuMinus.h

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

#ifndef _PRUNMUMINUS_H_
#define _PRUNMUMINUS_H_

#include "PRunBase.h"

/**
 * <p>Will eventually handle the \f$\mu^{-}\f$ fitting etc.
 */
class PRunMuMinus : public PRunBase
{
  public:
    PRunMuMinus();
    PRunMuMinus(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag, Bool_t theoAsData);
    virtual ~PRunMuMinus();

    virtual Double_t CalcChiSquare(const std::vector<Double_t>& par);
    virtual Double_t CalcChiSquareExpected(const std::vector<Double_t>& par);
    virtual Double_t CalcMaxLikelihood(const std::vector<Double_t>& par);
    virtual void CalcTheory();

    virtual UInt_t GetNoOfFitBins();

    virtual void SetFitRangeBin(const TString fitRange);

    virtual Int_t GetStartTimeBin() { return fStartTimeBin; }
    virtual Int_t GetEndTimeBin() { return fEndTimeBin; }
    virtual Int_t GetPacking() { return fPacking; }

    virtual void CalcNoOfFitBins();

  protected:
    virtual Bool_t PrepareData();
    virtual Bool_t PrepareFitData(PRawRunData* runData, const UInt_t histoNo);
    virtual Bool_t PrepareRawViewData(PRawRunData* runData, const UInt_t histoNo);

  private:
    UInt_t fNoOfFitBins;    ///< number of bins to be fitted
    Int_t  fPacking;        ///< packing for this particular run. Either given in the RUN- or GLOBAL-block.
    Bool_t fTheoAsData;     ///< true=only calculate the theory points at the data points, false=calculate more points for the theory as compared to data are calculated which lead to 'nicer' Fouriers

    Int_t fGoodBins[2];     ///< keep first/last good bins. 0=fgb, 1=lgb

    PDoubleVector fForward; ///< forward histo data

    Int_t fStartTimeBin;    ///< bin at which the fit starts
    Int_t fEndTimeBin;      ///< bin at which the fit ends

    virtual Bool_t GetProperT0(PRawRunData* runData, PMsrGlobalBlock *globalBlock, PUIntVector &histoNo);
    virtual Bool_t GetProperDataRange();
    virtual void GetProperFitRange(PMsrGlobalBlock *globalBlock);
};

#endif // _PRUNMUMINUS_H_
