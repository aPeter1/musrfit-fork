/***************************************************************************

  PRunSingleHisto.h

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

#ifndef _PRUNSINGLEHISTO_H_
#define _PRUNSINGLEHISTO_H_

#include "PRunBase.h"

class PRunSingleHisto : public PRunBase
{
  public:
    PRunSingleHisto();
    PRunSingleHisto(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag);
    virtual ~PRunSingleHisto();

    virtual Double_t CalcChiSquare(const std::vector<Double_t>& par);
    virtual Double_t CalcMaxLikelihood(const std::vector<Double_t>& par);
    virtual void CalcTheory();

    virtual UInt_t GetNoOfFitBins() { return fNoOfFitBins; }

  protected:
    virtual Bool_t PrepareData();
    virtual Bool_t PrepareFitData(PRawRunData* runData, const UInt_t histoNo);
    virtual Bool_t PrepareRawViewData(PRawRunData* runData, const UInt_t histoNo);
    virtual Bool_t PrepareViewData(PRawRunData* runData, const UInt_t histoNo);

  private:
    Double_t fFitStartTime;
    Double_t fFitStopTime;
    UInt_t fNoOfFitBins;
    Double_t fBackground;         ///< needed if background range is given. In units per bin

    Bool_t EstimateBkg(UInt_t histoNo);
};

#endif // _PRUNSINGLEHISTO_H_
