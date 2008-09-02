/***************************************************************************

  PMusrFourier.h

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

#ifndef _PMUSRFOURIER_H_
#define _PMUSRFOURIER_H_

#include <vector>
using namespace std;

#include "fftw3.h"

#define F_ESTIMATE_N0_AND_BKG true

#define F_SINGLE_HISTO 0
#define F_ASYMMETRY    1

#define F_APODIZATION_NONE   0
#define F_APODIZATION_WEAK   1
#define F_APODIZATION_MEDIUM 2
#define F_APODIZATION_STRONG 3
#define F_APODIZATION_USER   4

#define F_FILTER_NONE                  0
#define F_FILTER_LOW_PASS              1
#define F_FILTER_LOW_PASS_BESSEL       2
#define F_FILTER_LOW_PASS_BUTTERWORTH  3
#define F_FILTER_LOW_PASS_CHEBYCHEV    4
#define F_FILTER_BAND_PASS             5
#define F_FILTER_BAND_PASS_BESSEL      6
#define F_FILTER_BAND_PASS_BUTTERWORTH 7
#define F_FILTER_BAND_PASS_CHEBYCHEV   8
#define F_FILTER_HIGH_PASS             9
#define F_FILTER_HIGH_PASS_BESSEL      10
#define F_FILTER_HIGH_PASS_BUTTERWORTH 11
#define F_FILTER_HIGH_PASS_CHEBYCHEV   12
#define F_FILTER_USER                  13

// gamma_muon / (2 pi) = 1.355342e-5 (GHz/G)
#define F_GAMMA_BAR_MUON  1.355342e-5

/**************************************************************
 !!! START: NEEDS TO BE REMOVED WHEN MERGING WITH MUSRVIEW !!!
***************************************************************/
// muon life time in (us), see PRL99, 032001 (2007)
#define PMUON_LIFETIME 2.197019

typedef vector<double> PDoubleVector;
/**************************************************************
 !!! END: NEEDS TO BE REMOVED WHEN MERGING WITH MUSRVIEW !!!
***************************************************************/

class PMusrFourier
{
  public:
    PMusrFourier(int dataType, PDoubleVector &data, double timeResolution, 
                 double startTime = 0.0, double endTime = 0.0, unsigned int binning = 1,
                 bool estimateN0AndBkg = false);
    virtual ~PMusrFourier();

    virtual void SetN0(double n0) { fN0 = n0; }
    virtual void SetBkg(double bkg) { fBkg = bkg; }

    virtual void Transform(int apodization = -1, int filter = -1);

    virtual double GetFieldResolution() { return fFieldResolution; }
    virtual double GetPhaseCorrection() { return fPhaseCorrection; }
    virtual void GetRealFourier(PDoubleVector &realFourier);
    virtual void GetImaginaryFourier(PDoubleVector &imaginaryFourier);
    virtual void GetPowerFourier(PDoubleVector &powerFourier);
    virtual void GetPhaseFourier(PDoubleVector &phaseFourier);

    virtual bool IsValid() { return fValid; }

  private:
    bool fValid;

    int fDataType; ///< 0=Single Histo, 1=Asymmetry
    int fUseApodization; ///< 0=no apod., 1=weak apod., 2=medium apod., 3=strong apod., 4=user apod.
    int fUseFilter; ///< 0=no filter, 1=low pass, 2=band pass, 3=high pass, 4=..., n=user filter

    double fN0;
    double fBkg;

    double fTimeResolution;
    double fStartTime;
    double fEndTime;
    unsigned int fRebin;
    double fFieldResolution;
    double fPhaseCorrection;

    PDoubleVector fData;
    PDoubleVector fDataRebinned;

    unsigned int fNoOfData;
    fftw_plan fFFTwPlan;
    fftw_complex *fIn;
    fftw_complex *fOut;

    virtual void PrepareFFTwInputData();
    virtual void EstimateN0AndBkg();
};

#endif // _PMUSRFOURIER_H_
