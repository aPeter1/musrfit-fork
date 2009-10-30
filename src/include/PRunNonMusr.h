/***************************************************************************

  PRunNonMusr.h

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

#ifndef _PRUNNONMUSR_H_
#define _PRUNNONMUSR_H_

#include "PMusr.h"
#include "PRunBase.h"

class PRunNonMusr : public PRunBase
{
  public:
    PRunNonMusr();
    PRunNonMusr(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag);
    virtual ~PRunNonMusr();

    virtual Double_t CalcChiSquare(const std::vector<Double_t>& par);
    virtual Double_t CalcMaxLikelihood(const std::vector<Double_t>& par);
    virtual void CalcTheory();

    virtual UInt_t GetNoOfFitBins() { return fNoOfFitBins; }

    virtual UInt_t GetXIndex();
    virtual UInt_t GetYIndex();

  protected:
    virtual Bool_t PrepareData();
    virtual Bool_t PrepareFitData();
    virtual Bool_t PrepareViewData();

  private:
    PRawRunData *fRawRunData;

    Double_t fFitStartTime;
    Double_t fFitStopTime;
    UInt_t fNoOfFitBins;
};

#endif // _PRUNNONMUSR_H_
