/***************************************************************************

  PRunAsymmetry.h

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

#ifndef _PRUNASYMMETRY_H_
#define _PRUNASYMMETRY_H_

#include "PRunBase.h"

//---------------------------------------------------------------------------
/**
 * <p>Class handling the asymmetry fit.
 */
class PRunAsymmetry : public PRunBase
{
  public:
    PRunAsymmetry();
    PRunAsymmetry(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag);
    virtual ~PRunAsymmetry();

    virtual Double_t CalcChiSquare(const std::vector<Double_t>& par);
    virtual Double_t CalcMaxLikelihood(const std::vector<Double_t>& par);
    virtual void CalcTheory();

    virtual UInt_t GetNoOfFitBins();

  protected:
    virtual void CalcNoOfFitBins();
    virtual Bool_t PrepareData();
    virtual Bool_t PrepareFitData(PRawRunData* runData, UInt_t histoNo[2]);
    virtual Bool_t PrepareViewData(PRawRunData* runData, UInt_t histoNo[2]);
    virtual Bool_t PrepareRRFViewData(PRawRunData* runData, UInt_t histoNo[2]);

  private:
    UInt_t fAlphaBetaTag; ///< \f$ 1 \to \alpha = \beta = 1\f$; \f$ 2 \to \alpha \neq 1, \beta = 1\f$; \f$ 3 \to \alpha = 1, \beta \neq 1\f$; \f$ 4 \to \alpha \neq 1, \beta \neq 1\f$.

    UInt_t fNoOfFitBins;    ///< number of bins to be be fitted

    PDoubleVector fForward;     ///< forward histo data
    PDoubleVector fForwardErr;  ///< forward histo errors
    PDoubleVector fBackward;    ///< backward histo data
    PDoubleVector fBackwardErr; ///< backward histo errors

    Bool_t SubtractFixBkg();
    Bool_t SubtractEstimatedBkg();
};

#endif // _PRUNASYMMETRY_H_
