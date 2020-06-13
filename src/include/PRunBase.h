/***************************************************************************

  PRunBase.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2020 by Andreas Suter                              *
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

#ifndef _PRUNBASE_H_
#define _PRUNBASE_H_

#include <vector>

#include <TString.h>

#include "PMusr.h"
#include "PMsrHandler.h"
#include "PRunDataHandler.h"
#include "PTheory.h"

//------------------------------------------------------------------------------------------
/**
 * <p>The run base class is enforcing a common interface to all supported fit-types.
 */
class PRunBase
{
  public:
    PRunBase();
    PRunBase(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag);
    virtual ~PRunBase();

    virtual Double_t CalcChiSquare(const std::vector<Double_t>& par) = 0; ///< pure virtual, i.e. needs to be implemented by the deriving class!!
    virtual Double_t CalcMaxLikelihood(const std::vector<Double_t>& par) = 0; ///< pure virtual, i.e. needs to be implemented by the deriving class!!
    virtual void SetFitRange(PDoublePairVector fitRange);

    virtual void CalcTheory() = 0; ///< pure virtual, i.e. needs to be implemented by the deriving class!!

    virtual UInt_t GetRunNo() { return fRunNo; } ///< returns the number of runs of the msr-file
    virtual PRunData* GetData() { return &fData; } ///< returns the data to be fitted
    virtual void CleanUp();
    virtual Bool_t IsValid() { return fValid; } ///< returns if the state is valid

  protected:
    Bool_t fValid; ///< flag showing if the state of the class is valid

    EPMusrHandleTag fHandleTag; ///< tag telling whether this is used for fit, view, ...

    Int_t fRunNo;               ///< number of the run within the msr-file
    PMsrHandler      *fMsrInfo; ///< msr-file handler
    PMsrRunBlock     *fRunInfo; ///< run info used to filter out needed infos of a run
    PRunDataHandler  *fRawData; ///< holds the raw run data

    PRunData fData;             ///< data to be fitted, viewed, i.e. binned data
    Double_t fTimeResolution;   ///< time resolution in (us)
    Double_t fField;            ///< field from the meta-data of the data file
    PDoubleVector fTemp;        ///< temperature(s) from the meta-data of the data file : first = temp, second = std. dev. of the temperature
    PDoubleVector fT0s;         ///< all t0 bins of a run! The derived classes will handle it.
    std::vector<PDoubleVector> fAddT0s; ///< all t0 bins of all addrun's of a run! The derived classes will handle it.

    Double_t fFitStartTime;     ///< fit start time
    Double_t fFitEndTime;       ///< fit end time

    PDoubleVector fFuncValues;  ///< is keeping the values of the functions from the FUNCTIONS block
    PTheory *fTheory;           ///< theory needed to calculate chi-square

    PDoubleVector fKaiserFilter; ///< stores the Kaiser filter vector (needed for the RRF).

    virtual Bool_t PrepareData() = 0; ///< pure virtual, i.e. needs to be implemented by the deriving class!!

    virtual void CalculateKaiserFilterCoeff(Double_t wc, Double_t A, Double_t dw);
    virtual void FilterTheo();
};

#endif // _PRUNBASE_H_
