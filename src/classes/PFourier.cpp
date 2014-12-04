/***************************************************************************

  PFourier.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2014 by Andreas Suter                              *
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

#include <cmath>

#include <iostream>
#include <iomanip>
using namespace std;

#include "TH1F.h"
#include "TF1.h"
#include "TAxis.h"

//#include "TFile.h"

#include "PMusr.h"
#include "PFourier.h"

#define PI      3.14159265358979312
#define PI_HALF 1.57079632679489656

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param data data histogram
 * \param unitTag tag telling in which units the Fourier transform shall be represented. Possible tags are:
 *                FOURIER_UNIT_GAUSS, FOURIER_UNIT_TESLA, FOURIER_UNIT_FREQ, FOURIER_UNIT_CYCLES
 * \param startTime start time of the data time window
 * \param endTime end time of the data time window
 * \param dcCorrected if true, removed DC offset from signal before Fourier transformation, otherwise not
 * \param zeroPaddingPower if set to values > 0, there will be zero padding up to 2^zeroPaddingPower
 */
PFourier::PFourier(TH1F *data, Int_t unitTag, Double_t startTime, Double_t endTime, Bool_t dcCorrected, UInt_t zeroPaddingPower) :
                   fData(data), fUnitTag(unitTag), fStartTime(startTime), fEndTime(endTime),
                   fDCCorrected(dcCorrected), fZeroPaddingPower(zeroPaddingPower)
{
  // some necessary checks and initialization
  if (fData == 0) {
    cerr << endl << "**ERROR** PFourier::PFourier: no valid data" << endl << endl;
    fValid = false;
    return;
  }

  fValid = true;
  fIn  = 0;
  fOut = 0;

  fApodization = F_APODIZATION_NONE;

  // calculate time resolution in (us)
  fTimeResolution = fData->GetBinWidth(1);

  // if endTime == 0 set it to the last time slot
  if (fEndTime == 0.0) {
    Int_t last = fData->GetNbinsX()-1;
    fEndTime = fData->GetBinCenter(last);
  }

  // swap start and end time if necessary
  if (fStartTime > fEndTime) {
    Double_t keep = fStartTime;
    fStartTime = fEndTime;
    fEndTime = keep;
  }

  // calculate start and end bin
  fNoOfData = (UInt_t)((fEndTime-fStartTime)/fTimeResolution);

  // check if zero padding is whished
  if (fZeroPaddingPower > 0) {
    UInt_t noOfBins = static_cast<UInt_t>(pow(2.0, static_cast<Double_t>(fZeroPaddingPower)));
    if (noOfBins > fNoOfData)
      fNoOfBins = noOfBins;
    else
      fNoOfBins = fNoOfData;
  } else {
    fNoOfBins = fNoOfData;
  }

  // calculate fourier resolution, depending on the units
  Double_t resolution = 1.0/(fTimeResolution*fNoOfBins); // in MHz
  switch (fUnitTag) {
    case FOURIER_UNIT_GAUSS:
      fResolution = resolution/GAMMA_BAR_MUON;
      break;
    case FOURIER_UNIT_TESLA:
      fResolution = 1e-4*resolution/GAMMA_BAR_MUON;
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

  // allocate necessary memory
  fIn  = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*fNoOfBins);
  fOut = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*fNoOfBins);

  // check if memory allocation has been successful
  if ((fIn == 0) || (fOut == 0)) {
    fValid = false;
    return;
  }

  // get the FFTW3 plan (see FFTW3 manual)
  fFFTwPlan = fftw_plan_dft_1d(fNoOfBins, fIn, fOut, FFTW_FORWARD, FFTW_ESTIMATE);

  // check if a valid plan has been generated
  if (!fFFTwPlan) {
    fValid = false;
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor
 */
PFourier::~PFourier()
{
  if (fFFTwPlan)
    fftw_destroy_plan(fFFTwPlan);
  if (fIn)
    fftw_free(fIn);
  if (fOut)
    fftw_free(fOut);
}

//--------------------------------------------------------------------------
// Transform
//--------------------------------------------------------------------------
/**
 * <p>Carries out the Fourier transform. It is assumed that fStartTime is the time zero
 * for the Fourier frame. Hence if fStartTime != 0.0 the phase shift will be corrected.
 *
 * \param apodizationTag 0=no apod., 1=weak apod., 2=medium apod., 3=strong apod.
 */
void PFourier::Transform(UInt_t apodizationTag)
{
  if (!fValid)
    return;

  PrepareFFTwInputData(apodizationTag);

  fftw_execute(fFFTwPlan);

  // correct the phase for tstart != 0.0
  // find the first bin >= fStartTime
  Double_t shiftTime = 0.0;
  for (Int_t i=1; i<fData->GetXaxis()->GetNbins(); i++) {
    if (fData->GetXaxis()->GetBinCenter(i) >= fStartTime) {
      shiftTime = fData->GetXaxis()->GetBinCenter(i);
      break;
    }
  }

  Double_t phase, re, im;
  for (UInt_t i=0; i<fNoOfBins; i++) {
    phase = 2.0*PI/(fTimeResolution*fNoOfBins) * i * shiftTime;
    re =  fOut[i][0] * cos(phase) + fOut[i][1] * sin(phase);
    im = -fOut[i][0] * sin(phase) + fOut[i][1] * cos(phase);
    fOut[i][0] = re;
    fOut[i][1] = im;
  }
}

//--------------------------------------------------------------------------
// GetRealFourier
//--------------------------------------------------------------------------
/**
 * <p>returns the real part Fourier as a histogram.
 *
 * \param scale normalisation factor
 */
TH1F* PFourier::GetRealFourier(const Double_t scale)
{
  // check if valid flag is set
  if (!fValid)
    return 0;

  // invoke realFourier
  Char_t name[256];
  Char_t title[256];
  snprintf(name, sizeof(name), "%s_Fourier_Re", fData->GetName());
  snprintf(title, sizeof(title), "%s_Fourier_Re", fData->GetTitle());

  UInt_t noOfFourierBins = 0;
  if (fNoOfBins % 2 == 0)
    noOfFourierBins = fNoOfBins/2;
  else
    noOfFourierBins = (fNoOfBins+1)/2;

  TH1F *realFourier = new TH1F(name, title, noOfFourierBins, -fResolution/2.0, (Double_t)(noOfFourierBins-1)*fResolution+fResolution/2.0);
  if (realFourier == 0) {
    fValid = false;
    cerr << endl << "**SEVERE ERROR** couldn't allocate memory for the real part of the Fourier transform." << endl;
    return 0;
  }

  // fill realFourier vector
  for (UInt_t i=0; i<noOfFourierBins; i++) {
    realFourier->SetBinContent(i+1, scale*fOut[i][0]);
    realFourier->SetBinError(i+1, 0.0);
  }
  return realFourier;
}

//--------------------------------------------------------------------------
// GetImaginaryFourier
//--------------------------------------------------------------------------
/**
 * <p>returns the imaginary part Fourier as a histogram.
 *
 * \param scale normalisation factor
 */
TH1F* PFourier::GetImaginaryFourier(const Double_t scale)
{
  // check if valid flag is set
  if (!fValid)
    return 0;

  // invoke imaginaryFourier
  Char_t name[256];
  Char_t title[256];
  snprintf(name, sizeof(name), "%s_Fourier_Im", fData->GetName());
  snprintf(title, sizeof(title), "%s_Fourier_Im", fData->GetTitle());

  UInt_t noOfFourierBins = 0;
  if (fNoOfBins % 2 == 0)
    noOfFourierBins = fNoOfBins/2;
  else
    noOfFourierBins = (fNoOfBins+1)/2;

  TH1F* imaginaryFourier = new TH1F(name, title, noOfFourierBins, -fResolution/2.0, (Double_t)(noOfFourierBins-1)*fResolution+fResolution/2.0);
  if (imaginaryFourier == 0) {
    fValid = false;
    cerr << endl << "**SEVERE ERROR** couldn't allocate memory for the imaginary part of the Fourier transform." << endl;
    return 0;
  }

  // fill imaginaryFourier vector
  for (UInt_t i=0; i<noOfFourierBins; i++) {
    imaginaryFourier->SetBinContent(i+1, scale*fOut[i][1]);
    imaginaryFourier->SetBinError(i+1, 0.0);
  }

  return imaginaryFourier;
}

//--------------------------------------------------------------------------
// GetPowerFourier
//--------------------------------------------------------------------------
/**
 * <p>returns the Fourier power spectrum as a histogram.
 *
 * \param scale normalisation factor
 */
TH1F* PFourier::GetPowerFourier(const Double_t scale)
{
  // check if valid flag is set
  if (!fValid)
    return 0;

  // invoke powerFourier
  Char_t name[256];
  Char_t title[256];
  snprintf(name, sizeof(name), "%s_Fourier_Pwr", fData->GetName());
  snprintf(title, sizeof(title), "%s_Fourier_Pwr", fData->GetTitle());

  UInt_t noOfFourierBins = 0;
  if (fNoOfBins % 2 == 0)
    noOfFourierBins = fNoOfBins/2;
  else
    noOfFourierBins = (fNoOfBins+1)/2;

  TH1F* pwrFourier = new TH1F(name, title, noOfFourierBins, -fResolution/2.0, (Double_t)(noOfFourierBins-1)*fResolution+fResolution/2.0);
  if (pwrFourier == 0) {
    fValid = false;
    cerr << endl << "**SEVERE ERROR** couldn't allocate memory for the power part of the Fourier transform." << endl;
    return 0;
  }

  // fill powerFourier vector
  for (UInt_t i=0; i<noOfFourierBins; i++) {
    pwrFourier->SetBinContent(i+1, scale*sqrt(fOut[i][0]*fOut[i][0]+fOut[i][1]*fOut[i][1]));
    pwrFourier->SetBinError(i+1, 0.0);
  }

  return pwrFourier;
}

//--------------------------------------------------------------------------
// GetPhaseFourier
//--------------------------------------------------------------------------
/**
 * <p>returns the Fourier phase spectrum as a histogram.
 *
 * \param scale normalisation factor
 */
TH1F* PFourier::GetPhaseFourier(const Double_t scale)
{
  // check if valid flag is set
  if (!fValid)
    return 0;

  // invoke phaseFourier
  Char_t name[256];
  Char_t title[256];
  snprintf(name, sizeof(name), "%s_Fourier_Phase", fData->GetName());
  snprintf(title, sizeof(title), "%s_Fourier_Phase", fData->GetTitle());

  UInt_t noOfFourierBins = 0;
  if (fNoOfBins % 2 == 0)
    noOfFourierBins = fNoOfBins/2;
  else
    noOfFourierBins = (fNoOfBins+1)/2;

  TH1F* phaseFourier = new TH1F(name, title, noOfFourierBins, -fResolution/2.0, (Double_t)(noOfFourierBins-1)*fResolution+fResolution/2.0);
  if (phaseFourier == 0) {
    fValid = false;
    cerr << endl << "**SEVERE ERROR** couldn't allocate memory for the phase part of the Fourier transform." << endl;
    return 0;
  }

  // fill phaseFourier vector
  Double_t value = 0.0;
  for (UInt_t i=0; i<noOfFourierBins; i++) {
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
 * <p>Feeds the Fourier data and apply the apodization.
 *
 * \param apodizationTag apodization tag. Possible are currently: F_APODIZATION_NONE = no apodization,
 *                       F_APODIZATION_WEAK = weak apodization, F_APODIZATION_MEDIUM = intermediate apodization,
 *                       F_APODIZATION_STRONG = strong apodization
 */
void PFourier::PrepareFFTwInputData(UInt_t apodizationTag)
{
  // 1st find t==0. fData start at times t<0!!
  Int_t t0bin = -1;
  for (Int_t i=1; i<fData->GetNbinsX(); i++) {
    if (fData->GetBinCenter(i) >= 0.0) {
      t0bin = i;
      break;
    }
  }

  Int_t ival = static_cast<Int_t>(fStartTime/fTimeResolution) + t0bin;
  UInt_t start = 0;
  if (ival >= 0) {
    start = static_cast<UInt_t>(ival);
  }

  Double_t mean = 0.0;
  if (fDCCorrected) {
    for (UInt_t i=0; i<fNoOfData; i++) {
      mean += fData->GetBinContent(i+start);
    }
    mean /= (Double_t)fNoOfData;
  }

  // 2nd fill fIn
  for (UInt_t i=0; i<fNoOfData; i++) {
    fIn[i][0] = fData->GetBinContent(i+start) - mean;
    fIn[i][1] = 0.0;
  }
  for (UInt_t i=fNoOfData; i<fNoOfBins; i++) {
    fIn[i][0] = 0.0;
    fIn[i][1] = 0.0;
  }

  // 3rd apodize data (if wished)
  ApodizeData(apodizationTag);
}

//--------------------------------------------------------------------------
// ApodizeData
//--------------------------------------------------------------------------
/**
 * <p>Carries out the appodization of the data.
 *
 * \param apodizationTag apodization tag. Possible are currently: F_APODIZATION_NONE = no apodization,
 *                       F_APODIZATION_WEAK = weak apodization, F_APODIZATION_MEDIUM = intermediate apodization,
 *                       F_APODIZATION_STRONG = strong apodization
 */
void PFourier::ApodizeData(Int_t apodizationTag) {

  const Double_t cweak[3]   = { 0.384093, -0.087577, 0.703484 };
  const Double_t cmedium[3] = { 0.152442, -0.136176, 0.983734 };
  const Double_t cstrong[3] = { 0.045335,  0.554883, 0.399782 };

  Double_t c[5];
  for (UInt_t i=0; i<5; i++) {
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
      cerr << endl << ">> **ERROR** User Apodization tag " << apodizationTag << " unknown, sorry ..." << endl;
      break;
  }

  Double_t q;
  for (UInt_t i=0; i<fNoOfData; i++) {
    q = c[0];
    for (UInt_t j=1; j<5; j++) {
      q += c[j] * pow((Double_t)i/(Double_t)fNoOfData, 2.0*(Double_t)j);
    }
    fIn[i][0] *= q;
  }
}
