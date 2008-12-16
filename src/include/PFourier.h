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

#define F_ESTIMATE_N0_AND_BKG true

#define F_SINGLE_HISTO_RAW 0
#define F_SINGLE_HISTO     1
#define F_ASYMMETRY        2

#define F_APODIZATION_NONE   0
#define F_APODIZATION_WEAK   1
#define F_APODIZATION_MEDIUM 2
#define F_APODIZATION_STRONG 3

// gamma_muon / (2 pi) = 1.355342e-5 (GHz/G)
#define F_GAMMA_BAR_MUON  1.355342e-5

class PFourier
{
  public:
    PFourier(int dataType, TH1F *data,
             double startTime = 0.0, double endTime = 0.0,
             unsigned int zeroPaddingPower = 0,
             bool estimateN0AndBkg = false);
    virtual ~PFourier();

    virtual void Transform(unsigned int apodizationTag = 0);

    virtual double GetFieldResolution() { return fFieldResolution; }
    virtual void GetRealFourier(TH1F *realFourier);
    virtual void GetImaginaryFourier(TH1F *imaginaryFourier);

    virtual bool IsValid() { return fValid; }

  private:
    bool fValid;

    int fDataType;    ///< 0=Single Histo Raw, 1=Single Histo Life Time Corrected, 2=Asymmetry
    int fApodization; ///< 0=none, 1=weak, 2=medium, 3=strong

    double fN0;
    double fBkg;

    double fTimeResolution;
    double fStartTime;
    double fEndTime;
    unsigned int fZeroPaddingPower;
    double fFieldResolution;

    TH1F *fData;

    unsigned int fNoOfData;
    unsigned int fNoOfBins;
    fftw_plan fFFTwPlan;
    fftw_complex *fIn;
    fftw_complex *fOut;

    virtual void PrepareSingleHistoFFTwInputData(unsigned int apodizationTag);
    virtual void PrepareFFTwInputData(unsigned int apodizationTag);
    virtual void ApodizeData(int apodizationTag);
    virtual void EstimateN0AndBkg();
};

#endif // _PFOURIER_H_
