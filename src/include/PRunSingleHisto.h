/***************************************************************************

  PRunSingleHisto.h

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

#ifndef _PRUNSINGLEHISTO_H_
#define _PRUNSINGLEHISTO_H_

#include "PRunBase.h"

/**
 * <p>Class handling single histogram fit type.
 */
class PRunSingleHisto : public PRunBase
{
  public:
    PRunSingleHisto();
    PRunSingleHisto(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag);
    virtual ~PRunSingleHisto();

    virtual Double_t CalcChiSquare(const std::vector<Double_t>& par);
    virtual Double_t CalcChiSquareExpected(const std::vector<Double_t>& par);
    virtual Double_t CalcMaxLikelihood(const std::vector<Double_t>& par);
    virtual void CalcTheory();

    virtual UInt_t GetNoOfFitBins();

    virtual void SetFitRangeBin(const TString fitRange);

    virtual Double_t GetBackground() { return fBackground; }

    virtual Int_t GetStartTimeBin() { return fStartTimeBin; }
    virtual Int_t GetEndTimeBin() { return fEndTimeBin; }
    virtual Int_t GetPacking() { return fPacking; }
    virtual Bool_t GetScaleN0AndBkg() { return fScaleN0AndBkg; }

  protected:
    virtual void CalcNoOfFitBins();
    virtual Bool_t PrepareData();
    virtual Bool_t PrepareFitData(PRawRunData* runData, const UInt_t histoNo);
    virtual Bool_t PrepareRawViewData(PRawRunData* runData, const UInt_t histoNo);
    virtual Bool_t PrepareViewData(PRawRunData* runData, const UInt_t histoNo);

  private:
    Bool_t fScaleN0AndBkg;  ///< true=scale N0 and background to 1/ns, otherwise 1/bin
    UInt_t fNoOfFitBins;    ///< number of bins to be fitted
    Double_t fBackground;   ///< needed if background range is given (units: 1/bin)
    Int_t fPacking;         ///< packing for this particular run. Either given in the RUN- or GLOBAL-block.

    Int_t fGoodBins[2];     ///< keep first/last good bins. 0=fgb, 1=lgb

    PDoubleVector fForward; ///< forward histo data

    Int_t fStartTimeBin;    ///< bin at which the fit starts
    Int_t fEndTimeBin;      ///< bin at which the fit ends

    virtual Bool_t GetProperT0(PRawRunData* runData, PMsrGlobalBlock *globalBlock, PUIntVector &histoNo);
    virtual Bool_t GetProperDataRange();
    virtual void GetProperFitRange(PMsrGlobalBlock *globalBlock);
    virtual void EstimateN0();
    virtual Bool_t EstimateBkg(UInt_t histoNo);
    virtual Bool_t IsScaleN0AndBkg();
};

#endif // _PRUNSINGLEHISTO_H_
