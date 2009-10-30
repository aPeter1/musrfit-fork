/***************************************************************************

  PFourier.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2008 by Andreas Suter                                   *
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

#ifndef _PFOURIER_H_
#define _PFOURIER_H_

#include "fftw3.h"

#define F_APODIZATION_NONE   1
#define F_APODIZATION_WEAK   2
#define F_APODIZATION_MEDIUM 3
#define F_APODIZATION_STRONG 4

// gamma_muon / (2 pi) = 1.355342e-2 (MHz/G)
#define F_GAMMA_BAR_MUON  1.355342e-2

class PFourier
{
  public:
    PFourier(TH1F *data, Int_t unitTag,
             Double_t startTime = 0.0, Double_t endTime = 0.0,
             UInt_t zeroPaddingPower = 0);
    virtual ~PFourier();

    virtual void Transform(UInt_t apodizationTag = 0);

    virtual Double_t GetResolution() { return fResolution; }
    virtual TH1F* GetRealFourier(const Double_t scale = 1.0);
    virtual TH1F* GetImaginaryFourier(const Double_t scale = 1.0);
    virtual TH1F* GetPowerFourier(const Double_t scale = 1.0);
    virtual TH1F* GetPhaseFourier(const Double_t scale = 1.0);

    virtual Bool_t IsValid() { return fValid; }

  private:
    TH1F *fData;

    Bool_t fValid;
    Int_t  fUnitTag; ///< 1=Field Units (G), 2=Frequency Units (MHz), 3=Angular Frequency Units (Mc/s)

    Int_t fApodization; ///< 0=none, 1=weak, 2=medium, 3=strong

    Double_t fTimeResolution;
    Double_t fStartTime;
    Double_t fEndTime;
    UInt_t fZeroPaddingPower;
    Double_t fResolution;

    UInt_t fNoOfData;
    UInt_t fNoOfBins;
    fftw_plan fFFTwPlan;
    fftw_complex *fIn;
    fftw_complex *fOut;

    virtual void PrepareFFTwInputData(UInt_t apodizationTag);
    virtual void ApodizeData(Int_t apodizationTag);
};

#endif // _PFOURIER_H_
