/***************************************************************************

  PRunAsymmetryRRF.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2023 by Andreas Suter                              *
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

#ifndef _PRUNASYMMETRYRRF_H_
#define _PRUNASYMMETRYRRF_H_

#include "PRunBase.h"

//---------------------------------------------------------------------------
/**
 * <p>Class handling the asymmetry fit.
 */
class PRunAsymmetryRRF : public PRunBase
{
  public:
    PRunAsymmetryRRF();
    PRunAsymmetryRRF(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag, Bool_t theoAsData);
    virtual ~PRunAsymmetryRRF();

    virtual Double_t CalcChiSquare(const std::vector<Double_t>& par);
    virtual Double_t CalcChiSquareExpected(const std::vector<Double_t>& par);
    virtual Double_t CalcMaxLikelihood(const std::vector<Double_t>& par);
    virtual void CalcTheory();

    virtual UInt_t GetNoOfFitBins();

    virtual void SetFitRangeBin(const TString fitRange);

    virtual Int_t GetStartTimeBin() { return fStartTimeBin; }
    virtual Int_t GetEndTimeBin() { return fEndTimeBin; }

    virtual void CalcNoOfFitBins();

  protected:
    virtual Bool_t PrepareData();
    virtual Bool_t PrepareFitData();
    virtual Bool_t PrepareViewData(PRawRunData* runData, UInt_t histoNo[2]);

  private:
    UInt_t fAlphaBetaTag; ///< \f$ 1 \to \alpha = \beta = 1\f$; \f$ 2 \to \alpha \neq 1, \beta = 1\f$; \f$ 3 \to \alpha = 1, \beta \neq 1\f$; \f$ 4 \to \alpha \neq 1, \beta \neq 1\f$.
    UInt_t fNoOfFitBins;  ///< number of bins to be be fitted
    Int_t fRRFPacking;    ///< RRF packing for this particular run. Given in the GLOBAL-block.
    Bool_t fTheoAsData;   ///< true=only calculate the theory points at the data points, false=calculate more points for the theory as compared to data are calculated which lead to 'nicer' Fouriers

    PDoubleVector fForward;     ///< forward histo data
    PDoubleVector fForwardErr;  ///< forward histo errors
    PDoubleVector fBackward;    ///< backward histo data
    PDoubleVector fBackwardErr; ///< backward histo errors

    Int_t fGoodBins[4];   ///< keep first/last good bins. 0=fgb, 1=lgb (forward); 2=fgb, 3=lgb (backward)

    Int_t fStartTimeBin;    ///< bin at which the fit starts
    Int_t fEndTimeBin;      ///< bin at which the fit ends

    Bool_t SubtractFixBkg();
    Bool_t SubtractEstimatedBkg();

    virtual Bool_t GetProperT0(PRawRunData* runData, PMsrGlobalBlock *globalBlock, PUIntVector &forwardHisto, PUIntVector &backwardHistoNo);
    virtual Bool_t GetProperDataRange(PRawRunData* runData, UInt_t histoNo[2]);
    virtual void GetProperFitRange(PMsrGlobalBlock *globalBlock);
};

#endif // _PRUNASYMMETRYRRF_H_
