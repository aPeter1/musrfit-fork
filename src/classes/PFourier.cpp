/***************************************************************************

  PFourier.cpp

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

#include <cmath>

#include <iostream>
using namespace std;

#include "TH1F.h"
#include "TF1.h"

#include "TFile.h"

#include "PMusr.h"
#include "PFourier.h"

#define PI      3.14159265358979312
#define PI_HALF 1.57079632679489656

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
PFourier::PFourier(TH1F *data, int unitTag, double startTime, double endTime, unsigned int zeroPaddingPower) :
                   fData(data), fUnitTag(unitTag), fStartTime(startTime), fEndTime(endTime),
                   fZeroPaddingPower(zeroPaddingPower)
{
  // some necessary checks and initialization
  if (fData == 0) {
    cout << endl << "**ERROR** PFourier::PFourier: no valid data" << endl << endl;
    fValid = false;
    return;
  }

//cout << endl << "PFourier::PFourier: fData = " << fData;
/*
for (unsigned int i=0; i<10; i++) {
  cout << endl << "PFourier::PFourier: i=" << i << ", t=" << fData->GetBinCenter(i) << ", value=" << fData->GetBinContent(i);
}
*/
//cout << endl;

  fValid = true;
  fIn  = 0;
  fOut = 0;

  fApodization = F_APODIZATION_NONE;

  // calculate time resolution in (us)
  fTimeResolution = fData->GetBinWidth(1);
//cout << endl << ">> fTimeResolution = " << fTimeResolution;

  // if endTime == 0 set it to the last time slot
  if (fEndTime == 0.0) {
    int last = fData->GetNbinsX()-1;
    fEndTime = fData->GetBinCenter(last);
//cout << endl << ">> fEndTime = " << fEndTime;
  }

  // swap start and end time if necessary
  if (fStartTime > fEndTime) {
    double keep = fStartTime;
    fStartTime = fEndTime;
    fEndTime = keep;
  }

  // calculate start and end bin
  unsigned int start = (unsigned int)(fStartTime/fTimeResolution);
  unsigned int end = (unsigned int)(fEndTime/fTimeResolution);
  fNoOfData = end-start;

//cout << endl << ">> fNoOfData = " << fNoOfData;

// check if zero padding is whished
  if (fZeroPaddingPower > 0) {
    unsigned int noOfBins = static_cast<unsigned int>(pow(2.0, static_cast<double>(fZeroPaddingPower)));
    if (noOfBins > fNoOfData)
      fNoOfBins = noOfBins;
    else
      fNoOfBins = fNoOfData;
  } else {
    fNoOfBins = fNoOfData;
  }

//cout << endl << ">> fNoOfBins = " << fNoOfBins;

  // calculate fourier resolution
  double resolution = 1.0/(fTimeResolution*fNoOfBins); // in MHz
  switch (fUnitTag) {
    case FOURIER_UNIT_FIELD:
      fResolution = resolution/F_GAMMA_BAR_MUON;
      break;
    case FOURIER_UNIT_FREQ:
      fResolution = resolution;
      break;
    case FOURIER_UNIT_CYCLES:
      fResolution = 2.0*PI*resolution;
      break;
    default:
      fValid = false;
      return;
      break;
  }

//cout << endl << ">> fResolution = " << fResolution;

  // allocate necessary memory
  fIn  = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*fNoOfBins);
  fOut = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*fNoOfBins);

//cout << endl << ">> fIn  = " << fIn;
//cout << endl << ">> fOut = " << fOut;

  // check if memory allocation has been successful
  if ((fIn == 0) || (fOut == 0)) {
    fValid = false;
    return;
  }

//cout << endl << "PFourier::PFourier: fNoOfBins=" << fNoOfBins << endl;
  fFFTwPlan = fftw_plan_dft_1d(fNoOfBins, fIn, fOut, FFTW_FORWARD, FFTW_ESTIMATE);

  if (!fFFTwPlan) {
    fValid = false;
  }

//cout << endl;

}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
PFourier::~PFourier()
{
//cout << endl << "in ~PFourier() ..." << endl;
  if (fValid) {
    fftw_destroy_plan(fFFTwPlan);
    fftw_free(fIn);
    fftw_free(fOut);
  }
}

//--------------------------------------------------------------------------
// Transform
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param apodizationTag 0=no apod., 1=weak apod., 2=medium apod., 3=strong apod.
 */
void PFourier::Transform(unsigned int apodizationTag)
{
  if (!fValid)
    return;

//cout << endl << ">> PFourier::Transform (apodizationTag=" << apodizationTag << ") ..." << endl;

  PrepareFFTwInputData(apodizationTag);

/*
if (fNoOfBins < 200) {
for (unsigned int i=0; i<fNoOfBins; i++) {
  cout << endl << ">> PFourier::PrepareFFTwInputData: " << i << ": fIn[i][0] = " << fIn[i][0];
}
cout << endl;
}
*/

  fftw_execute(fFFTwPlan);

/*
for (unsigned int i=fNoOfBins-10; i<fNoOfBins; i++) {
  cout << endl << ">> PFourier::PrepareFFTwInputData: " << i << ": fOut[i][0] = " << fOut[i][0];
}
cout << endl;
*/
}

//--------------------------------------------------------------------------
// GetRealFourier
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param scale
 */
TH1F* PFourier::GetRealFourier(const double scale)
{
  // check if valid flag is set
  if (!fValid)
    return 0;

  // invoke realFourier
  char name[256];
  char title[256];
  snprintf(name, sizeof(name), "%s_Fourier_Re", fData->GetName());
  snprintf(title, sizeof(title), "%s_Fourier_Re", fData->GetTitle());

  TH1F *realFourier = new TH1F(name, title, fNoOfBins/2, -fResolution/2.0, fNoOfBins/2.0*fResolution+fResolution/2.0);
  if (realFourier == 0) {
    fValid = false;
    cout << endl << "**SEVERE ERROR** couldn't allocate memory for the real part of the Fourier transform." << endl;
    return 0;
  }

  // fill realFourier vector
  for (unsigned int i=0; i<fNoOfBins/2; i++) {
    realFourier->SetBinContent(i+1, scale*fOut[i][0]);
    realFourier->SetBinError(i+1, 0.0);
  }
//cout << endl << ">> realFourier = " << realFourier;
  return realFourier;
}

//--------------------------------------------------------------------------
// GetImaginaryFourier
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param scale
 */
TH1F* PFourier::GetImaginaryFourier(const double scale)
{
  // check if valid flag is set
  if (!fValid)
    return 0;

  // invoke imaginaryFourier
  char name[256];
  char title[256];
  snprintf(name, sizeof(name), "%s_Fourier_Im", fData->GetName());
  snprintf(title, sizeof(title), "%s_Fourier_Im", fData->GetTitle());

  TH1F* imaginaryFourier = new TH1F(name, title, fNoOfBins/2, -fResolution/2.0, fNoOfBins/2.0*fResolution+fResolution/2.0);
  if (imaginaryFourier == 0) {
    fValid = false;
    cout << endl << "**SEVERE ERROR** couldn't allocate memory for the imaginary part of the Fourier transform." << endl;
    return 0;
  }

  // fill imaginaryFourier vector
  for (unsigned int i=0; i<fNoOfBins/2; i++) {
    imaginaryFourier->SetBinContent(i+1, scale*fOut[i][1]);
    imaginaryFourier->SetBinError(i+1, 0.0);
  }

  return imaginaryFourier;
}

//--------------------------------------------------------------------------
// GetPowerFourier
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param scale
 */
TH1F* PFourier::GetPowerFourier(const double scale)
{
  // check if valid flag is set
  if (!fValid)
    return 0;

  // invoke powerFourier
  char name[256];
  char title[256];
  snprintf(name, sizeof(name), "%s_Fourier_Pwr", fData->GetName());
  snprintf(title, sizeof(title), "%s_Fourier_Pwr", fData->GetTitle());

  TH1F* pwrFourier = new TH1F(name, title, fNoOfBins/2, -fResolution/2.0, fNoOfBins/2.0*fResolution+fResolution/2.0);
  if (pwrFourier == 0) {
    fValid = false;
    cout << endl << "**SEVERE ERROR** couldn't allocate memory for the power part of the Fourier transform." << endl;
    return 0;
  }

  // fill powerFourier vector
  for (unsigned int i=0; i<fNoOfBins/2; i++) {
    pwrFourier->SetBinContent(i+1, scale*sqrt(fOut[i][0]*fOut[i][0]+fOut[i][1]*fOut[i][1]));
    pwrFourier->SetBinError(i+1, 0.0);
  }

  return pwrFourier;
}

//--------------------------------------------------------------------------
// GetPhaseFourier
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param scale
 */
TH1F* PFourier::GetPhaseFourier(const double scale)
{
  // check if valid flag is set
  if (!fValid)
    return 0;

  // invoke phaseFourier
  char name[256];
  char title[256];
  snprintf(name, sizeof(name), "%s_Fourier_Phase", fData->GetName());
  snprintf(title, sizeof(title), "%s_Fourier_Phase", fData->GetTitle());

  TH1F* phaseFourier = new TH1F(name, title, fNoOfBins/2, -fResolution/2.0, fNoOfBins/2.0*fResolution+fResolution/2.0);
  if (phaseFourier == 0) {
    fValid = false;
    cout << endl << "**SEVERE ERROR** couldn't allocate memory for the phase part of the Fourier transform." << endl;
    return 0;
  }

  // fill phaseFourier vector
  double value = 0.0;
  for (unsigned int i=0; i<fNoOfBins/2; i++) {
    // calculate the phase
    if (fOut[i][0] == 0) {
      if (fOut[i][1] >= 0.0)
        value = PI_HALF;
      else
        value = -PI_HALF;
    } else {
      value = atan(fOut[i][1]/fOut[i][0]);
      // check sector
      if (fOut[i][0] < 0.0) {
        if (fOut[i][1] > 0.0)
          value = PI + value;
        else
          value = PI - value;
      }
    }
    phaseFourier->SetBinContent(i+1, scale*value);
    phaseFourier->SetBinError(i+1, 0.0);
  }

  return phaseFourier;
}

//--------------------------------------------------------------------------
// PrepareFFTwInputData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PFourier::PrepareFFTwInputData(unsigned int apodizationTag)
{
  // 1st find t==0. fData start at times t<0!!
  int t0bin = -1;
//cout << ">> PFourier::PrepareFFTwInputData: fData=" << fData << ", fData->GetNbinsX() = " << fData->GetNbinsX();
    for (int i=0; i<fData->GetNbinsX(); i++) {
//if (i<20) cout << endl << ">> PFourier::PrepareFFTwInputData: i=" << i << ", fData->GetBinCenter(i)=" << fData->GetBinCenter(i);
    if (fData->GetBinCenter(i) >= 0.0) {
      t0bin = i;
      break;
    }
  }

  // 2nd fill fIn
  unsigned int start = (unsigned int)(fStartTime/fTimeResolution) + t0bin;
  for (unsigned int i=0; i<fNoOfData-start; i++) {
    fIn[i][0] = fData->GetBinContent(i+start+1);
    fIn[i][1] = 0.0;
  }
  for (unsigned int i=fNoOfData-start; i<fNoOfBins; i++) {
    fIn[i][0] = 0.0;
    fIn[i][1] = 0.0;
  }

//cout << ">> PFourier::PrepareFFTwInputData: t0bin = " << t0bin << ", start = " << start << endl;
  // 3rd apodize data (if wished)
  ApodizeData(apodizationTag);

/*
cout << ">> PFourier::PrepareFFTwInputData: fNoOfData = " << fNoOfData << ", fNoOfBins = " << fNoOfBins << endl;
for (unsigned int i=0; i<10; i++) {
  cout << endl << ">> PFourier::PrepareFFTwInputData: " << i << ": fIn[i][0] = " << fIn[i][0];
}
cout << endl;
*/
}

//--------------------------------------------------------------------------
// ApodizeData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param apodizationTag
 */
void PFourier::ApodizeData(int apodizationTag) {

  const double cweak[3]   = { 0.384093, -0.087577, 0.703484 };
  const double cmedium[3] = { 0.152442, -0.136176, 0.983734 };
  const double cstrong[3] = { 0.045335,  0.554883, 0.399782 };

  double c[5];
  for (unsigned int i=0; i<5; i++) {
    c[i] = 0.0;
  }

  switch (apodizationTag) {
    case F_APODIZATION_NONE:
      return;
      break;
    case F_APODIZATION_WEAK:
      c[0] = cweak[0]+cweak[1]+cweak[2];
      c[1] = -(cweak[1]+2.0*cweak[2]);
      c[2] = cweak[2];
      break;
    case F_APODIZATION_MEDIUM:
      c[0] = cmedium[0]+cmedium[1]+cmedium[2];
      c[1] = -(cmedium[1]+2.0*cmedium[2]);
      c[2] = cmedium[2];
      break;
    case F_APODIZATION_STRONG:
      c[0] = cstrong[0]+cstrong[1]+cstrong[2];
      c[1] = -2.0*(cstrong[1]+2.0*cstrong[2]);
      c[2] = cstrong[1]+6.0*cstrong[2];
      c[3] = -4.0*cstrong[2];
      c[4] = cstrong[2];
      break;
    default:
      cout << endl << ">> **ERROR** User Apodization tag " << apodizationTag << " unknown, sorry ..." << endl;
      break;
  }

  double q;
  for (unsigned int i=0; i<fNoOfData; i++) {
    q = c[0];
    for (unsigned int j=1; j<5; j++) {
      q += c[j] * pow((double)i/(double)fNoOfData, 2.0*(double)j);
    }
    fIn[i][0] *= q;
  }
}
