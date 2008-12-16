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
 * \dataType tag indicating if data is histogram, asymmetry, ...
 * \data vector with the real data
 */
PFourier::PFourier(int dataType, TH1F *data,
                   double startTime, double endTime,
                   unsigned int zeroPaddingPower, bool estimateN0AndBkg) :
                   fDataType(dataType), fData(data),
                   fStartTime(startTime), fEndTime(endTime),
                   fZeroPaddingPower(zeroPaddingPower)
{
  // some necessary checks and initialization
  if (fData == 0) {
    cout << endl << "**ERROR** PFourier::PFourier: no valid data" << endl << endl;
    fValid = false;
    return;
  }

  if ((fStartTime < 0.0) || (fEndTime < 0.0)) {
    cout << endl << "**ERROR** PFourier::PFourier: no valid start or end time." << endl << endl;
    fValid = false;
    return;
  }

  fValid = true;
  fIn  = 0;
  fOut = 0;

  fApodization = F_APODIZATION_NONE;

  // calculate time resolution in (ns)
  fTimeResolution = fData->GetBinWidth(1) * 1000.0;

  // if endTime == 0 set it to the last time slot
  if (fEndTime == 0.0) {
    int last = fData->GetNbinsX()-1;
    fEndTime = fData->GetBinCenter(last);
  } else {
    fEndTime *= 1000.0; // us -> ns
  }

  fStartTime *= 1000.0; // us -> ns

  // swap start and end time if necessary
  if (fStartTime > fEndTime) {
    double keep = fStartTime;
    fStartTime = fEndTime;
    fEndTime = keep;
  }

cout << endl << "dB = " << 1.0/(2.0 * F_GAMMA_BAR_MUON * (fEndTime-fStartTime)) << " (G), Bmax = " << 1.0/(2.0 * F_GAMMA_BAR_MUON * fTimeResolution) << " (G)" << endl;

  // try to estimate N0 and Bkg just out of the raw data
  if (estimateN0AndBkg) {
    EstimateN0AndBkg();
  }

  // calculate start and end bin
  unsigned int start = (unsigned int)(fStartTime/fTimeResolution);
  unsigned int end = (unsigned int)(fEndTime/fTimeResolution);
  fNoOfData = end-start;

  // check if zero padding is whished
  if (fZeroPaddingPower > 0) {
    fNoOfBins = static_cast<unsigned int>(pow(2.0, static_cast<double>(fZeroPaddingPower)));
  } else {
    fNoOfBins = fNoOfData;
  }

  // allocate necessary memory
  fIn  = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*fNoOfBins);
  fOut = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*fNoOfBins);

  // check if memory allocation has been successful
  if ((fIn == 0) || (fOut == 0)) {
    fValid = false;
    return;
  }

  fFFTwPlan = fftw_plan_dft_1d(fNoOfBins, fIn, fOut, FFTW_FORWARD, FFTW_ESTIMATE);

  if (!fFFTwPlan) {
    fValid = false;
  }
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
cout << endl << "in ~PFourier() ..." << endl;
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
 * \param apodizationTag 0=no apod., 1=weak apod., 2=medium apod., 3=strong apod., 4=user apod.
 */
void PFourier::Transform(unsigned int apodizationTag)
{
  if (!fValid)
    return;

  if (fDataType == F_SINGLE_HISTO) {
    PrepareSingleHistoFFTwInputData(apodizationTag);
  } else {
    PrepareFFTwInputData(apodizationTag);
  }

  fftw_execute(fFFTwPlan);
}

//--------------------------------------------------------------------------
// GetRealFourier
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param realFourier
 */
void PFourier::GetRealFourier(TH1F *realFourier)
{
  // check if valid flag is set
  if (!fValid)
    return;

  // reallocate realFourier
  char name[256];
  char title[256];
  strncpy(name, realFourier->GetName(), sizeof(name));
  strncpy(title, realFourier->GetTitle(), sizeof(title));
  if (realFourier) {
    delete realFourier;
    realFourier = 0;
  }
  realFourier = new TH1F(name, title, fNoOfBins, -fFieldResolution/2.0, fNoOfBins*fFieldResolution+fFieldResolution/2.0);
  if (realFourier == 0) {
    fValid = false;
    cout << endl << "**SEVERE ERROR** couldn't allocate memory for the real part of the Fourier transform." << endl;
    return;
  }

  // fill realFourier vector
  for (unsigned int i=0; i<fNoOfBins; i++) {
    realFourier->SetBinContent(i+1, fOut[i][0]);
  }
}

//--------------------------------------------------------------------------
// GetImaginaryFourier
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param imaginaryFourier
 */
void PFourier::GetImaginaryFourier(TH1F *imaginaryFourier)
{
  // check if valid flag is set
  if (!fValid)
    return;

  // reallocate imaginaryFourier
  char name[256];
  char title[256];
  strncpy(name, imaginaryFourier->GetName(), sizeof(name));
  strncpy(title, imaginaryFourier->GetTitle(), sizeof(title));
  if (imaginaryFourier) {
    delete imaginaryFourier;
    imaginaryFourier = 0;
  }
  imaginaryFourier = new TH1F(name, title, fNoOfBins, -fFieldResolution/2.0, fNoOfBins*fFieldResolution+fFieldResolution/2.0);
  if (imaginaryFourier == 0) {
    fValid = false;
    cout << endl << "**SEVERE ERROR** couldn't allocate memory for the imaginary part of the Fourier transform." << endl;
    return;
  }

  // fill imaginaryFourier vector
  for (unsigned int i=0; i<fNoOfBins; i++) {
    imaginaryFourier->SetBinContent(i+1, fOut[i][1]);
  }
}

//--------------------------------------------------------------------------
// EstimateN0AndBkg
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PFourier::EstimateN0AndBkg()
{
  int noOfBins = fData->GetNbinsX();

  TH1F summHisto("summHisto", "summHisto", noOfBins,
                 -fTimeResolution/2.0, (noOfBins-1)*fTimeResolution + fTimeResolution/2.0);

  // fill summHisto
  double value = 0.0;
  for (int i=1; i<noOfBins; i++) {
    value += fData->GetBinContent(i);
    summHisto.SetBinContent(i, value);
    summHisto.SetBinError(i, sqrt(value));
  }

cout << endl << ">> max.summHisto=" << summHisto.GetBinContent(noOfBins-1) << endl << endl;

  // define fit function
  TF1 *func = new TF1("func", "[0]*(1-TMath::Exp(-x/[1]))+[2]*x", 
                      -fTimeResolution/2.0, (noOfBins-1)*fTimeResolution +
                       fTimeResolution/2.0);
  // parameter 0 ~ N0 tau
  func->SetParameter(0, summHisto.GetBinContent(noOfBins-1));
  // parameter 1 == tau
  func->FixParameter(1, PMUON_LIFETIME*1000.0);
  // parameter 2 ~ <Bkg>
  func->SetParameter(2, summHisto.GetBinContent(noOfBins-1)/(PMUON_LIFETIME*1000.0)*0.05);

  // do the fit
  summHisto.Fit(func, "0QR"); // 0->no Graph, Q->quite, R->time range from function

  // get out the parameters
  double A = func->GetParameter(0);
  double B = func->GetParameter(2);

cout << endl << ">> A=" << A  << ", B=" << B;
cout << endl << ">> N0/per bin=" << A/(PMUON_LIFETIME*1000.0)*fTimeResolution << ", <Bkg> per bin=" << B*fTimeResolution << endl << endl; 

  fN0  = A/(PMUON_LIFETIME*1000.0)*fTimeResolution;
  fBkg = B*fTimeResolution;

  // clean up
  if (func) {
    delete func;
    func = 0;
  }
}

//--------------------------------------------------------------------------
// PrepareSingleHistoFFTwInputData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PFourier::PrepareSingleHistoFFTwInputData(unsigned int apodizationTag)
{
  // 1st fill fIn
  unsigned int start = (unsigned int)(fStartTime/fTimeResolution);
  for (unsigned int i=0; i<fNoOfData-start; i++) {
    fIn[i][0] = fData->GetBinContent(i+start+1);
    fIn[i][1] = 0.0;
  }
  for (unsigned int i=fNoOfData; i<fNoOfBins; i++) {
    fIn[i][0] = 0.0;
    fIn[i][1] = 0.0;
  }

  // 2nd subtract the Bkg from the data
  for (unsigned int i=0; i<fNoOfData; i++)
    fIn[i][0] -= fBkg;

  // 3rd remove the lifetime term
  for (unsigned int i=0; i<fNoOfData; i++)
    fIn[i][0] *= exp((start+i)*fTimeResolution/(PMUON_LIFETIME*1000.0));

  // 4th remove the constant N0 term
  for (unsigned int i=0; i<fNoOfData; i++)
    fIn[i][0] -= fN0;

  // 5th apodize data (if wished)
  ApodizeData(apodizationTag);
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
  // 1st fill fIn
  unsigned int start = (unsigned int)(fStartTime/fTimeResolution);
  for (unsigned int i=0; i<fNoOfData-start; i++) {
    fIn[i][0] = fData->GetBinContent(i+start+1);
    fIn[i][1] = 0.0;
  }
  for (unsigned int i=fNoOfData; i<fNoOfBins; i++) {
    fIn[i][0] = 0.0;
    fIn[i][1] = 0.0;
  }

  // 2nd apodize data (if wished)
  ApodizeData(apodizationTag);
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
