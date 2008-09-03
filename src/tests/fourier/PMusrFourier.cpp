/***************************************************************************

  PMusrFourier.cpp

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

#include "TH1F.h"
#include "TF1.h"

#include "TFile.h"

#include "PMusrFourier.h"

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
PMusrFourier::PMusrFourier(int dataType, PDoubleVector &data, double timeResolution,
                           double startTime, double endTime, unsigned int rebin,
                           unsigned int zeroPaddingPower, bool estimateN0AndBkg) :
                           fDataType(dataType), fTimeResolution(timeResolution),
                           fStartTime(startTime), fEndTime(endTime), fRebin(rebin),
                           fZeroPaddingPower(zeroPaddingPower)
{
  // init stuff
  fData = data;
  if (fData.empty()) {
    cout << endl << "**ERROR** PMusrFourier::PMusrFourier: no valid data" << endl << endl;
    fValid = false;
    return;
  }

  if ((fStartTime < 0.0) || (fEndTime < 0.0)) {
    cout << endl << "**ERROR** PMusrFourier::PMusrFourier: no valid start or end time." << endl << endl;
    fValid = false;
    return;
  }

  fValid = true;
  fIn  = 0;
  fOut = 0;

  // if endTime == 0 set is to the last time slot
  if (fEndTime == 0.0)
    fEndTime = (fData.size()-1)*fTimeResolution;
  else
    fEndTime *= 1000.0; // us -> ns

  fStartTime *= 1000.0; // us -> ns

  // swap start and end time if necessary
  if (fStartTime > fEndTime) {
    double keep = fStartTime;
    fStartTime = fEndTime;
    fEndTime = keep;
  }

  // rebin data if necessary
  if (fRebin > 1) {
    fTimeResolution *= fRebin;
    double dval = 0.0;
    unsigned int rebinCounter = 0;
    for (unsigned int i=0; i<fData.size(); i++) {
      dval += fData[i];
      rebinCounter++;
      if (rebinCounter == fRebin) {
        fDataRebinned.push_back(dval);
        dval = 0.0;
        rebinCounter = 0;
      }
    }
  }

cout << endl << "dB = " << 1.0/(F_GAMMA_BAR_MUON * (fEndTime-fStartTime)) << " (G), Bmax = " << 1.0/(F_GAMMA_BAR_MUON*fTimeResolution) << " (G)" << endl;


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

// has to be removed after testing
TH1F test_raw_in("test_raw_in", "test_raw_in", fNoOfData+1,
             fStartTime - fTimeResolution/2.0, fEndTime + fTimeResolution/2.0);
for (unsigned int i=0; i<fNoOfData; i++)
  test_raw_in.SetBinContent(i+1, fDataRebinned[i+start]);
TFile f("test_raw_in.root", "RECREATE");
test_raw_in.Write();
f.Close();

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
PMusrFourier::~PMusrFourier() 
{
cout << endl << "in ~PMusrFourier() ..." << endl;
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
 * \param apodization
 * \param filter
 * \param estimateN0AndBkg
 */
void PMusrFourier::Transform(int apodization, int filter)
{
  if (!fValid)
    return;

  if (fDataType == F_SINGLE_HISTO) {
    PrepareSingleHistoFFTwInputData();
  }

// for test only
// keep data
fftw_complex *data; 
data = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*fNoOfBins);
for (unsigned int i=0; i<fNoOfBins; i++) {
  data[i][0] = fIn[i][0];
  data[i][1] = 0.0;
}

// loop over the phase
double sum;
TH1F sumHist("sumHist", "sumHist", 361, -180.5, 180.5);
double dB   = 1.0/(2.0 * F_GAMMA_BAR_MUON * (fEndTime-fStartTime));
double Bmax = 1.0/(2.0 * F_GAMMA_BAR_MUON * fTimeResolution);
TH1F re("re", "re", fNoOfBins/2+1, -dB/2.0, Bmax+dB/2.0);
TH1F im("im", "im", fNoOfBins/2+1, -dB/2.0, Bmax+dB/2.0);
for (int p=-180; p<180; p++) {
  for (unsigned int i=0; i<fNoOfBins; i++) {
    // recalculate fIn including the phase
    fIn[i][0] = data[i][0]*cos(p/180.0*PI);
    fIn[i][1] = data[i][0]*sin(p/180.0*PI);
  }
  fftw_execute(fFFTwPlan);

  if (p==8) {
    for (unsigned int j=0; j<fNoOfBins/2; j++) {
      re.SetBinContent(j+1, fOut[j][0]);
      im.SetBinContent(j+1, fOut[j][1]);
    }
  }

  // calculate sum of the imaginary part of fOut
  sum = 0.0;
  for (unsigned int i=0; i<fNoOfBins/2; i++) {
    sum += fOut[i][1];
  }
  sumHist.SetBinContent(p+181, fabs(sum));
}
TFile f("test_out.root", "RECREATE");
re.Write();
im.Write();
sumHist.Write();
f.Close();

fftw_free(data);
}

//--------------------------------------------------------------------------
// GetRealFourier
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param realFourier
 */
void PMusrFourier::GetRealFourier(PDoubleVector &realFourier)
{
  // check if valid flag is set
  if (!fValid)
    return;

  // clear realFourier vector first
  realFourier.clear();

  // fill realFourier vector
  for (unsigned int i=0; i<fNoOfBins; i++) {
    realFourier.push_back(fOut[i][0]);
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
void PMusrFourier::GetImaginaryFourier(PDoubleVector &imaginaryFourier)
{
  // check if valid flag is set
  if (!fValid)
    return;

  // clear imaginaryFourier vector first
  imaginaryFourier.clear();

  // fill imaginaryFourier vector
  for (unsigned int i=0; i<fNoOfBins; i++) {
    imaginaryFourier.push_back(fOut[i][1]);
  }
}

//--------------------------------------------------------------------------
// GetPowerFourier
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param powerFourier
 */
void PMusrFourier::GetPowerFourier(PDoubleVector &powerFourier)
{
  // check if valid flag is set
  if (!fValid)
    return;

  // clear powerFourier vector first
  powerFourier.clear();

  // fill powerFourier vector
  for (unsigned int i=0; i<fNoOfBins; i++) {
    powerFourier.push_back(sqrt(fOut[i][0]*fOut[i][0]+fOut[i][0]*fOut[i][0]));
  }
}

//--------------------------------------------------------------------------
// GetPhaseFourier
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param powerFourier
 */
void PMusrFourier::GetPhaseFourier(PDoubleVector &phaseFourier)
{
  // check if valid flag is set
  if (!fValid)
    return;

  // clear phaseFourier vector first
  phaseFourier.clear();

  // fill phaseFourier vector
  double value = 0.0;
  for (unsigned int i=0; i<fNoOfBins; i++) {
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
          value += PI_HALF;
        else
          value -= PI_HALF;
      }
    }

    phaseFourier.push_back(value);
  }
}

//--------------------------------------------------------------------------
// EstimateN0AndBkg
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrFourier::EstimateN0AndBkg()
{
  TH1F summHisto("summHisto", "summHisto", fDataRebinned.size()+1,
                 -fTimeResolution/2.0, (fDataRebinned.size()-1)*fTimeResolution +
                 fTimeResolution/2.0);

  // fill summHisto
  double value = 0.0;
  for (unsigned int i=1; i<fDataRebinned.size(); i++) {
    value += fDataRebinned[i-1];
    summHisto.SetBinContent(i, value);
    summHisto.SetBinError(i, sqrt(value));
  }

cout << endl << ">> max.summHisto=" << summHisto.GetBinContent(fDataRebinned.size()-1) << endl << endl;

  // define fit function
  TF1 *func = new TF1("func", "[0]*(1-TMath::Exp(-x/[1]))+[2]*x", 
                      -fTimeResolution/2.0, (fDataRebinned.size()-1)*fTimeResolution +
                      fTimeResolution/2.0);
  // parameter 0 ~ N0 tau
  func->SetParameter(0, summHisto.GetBinContent(fDataRebinned.size()-1));
  // parameter 1 == tau
  func->FixParameter(1, PMUON_LIFETIME*1000.0);
  // parameter 2 ~ <Bkg>
  func->SetParameter(2, summHisto.GetBinContent(fDataRebinned.size()-1)/(PMUON_LIFETIME*1000.0)*0.05);

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
void PMusrFourier::PrepareSingleHistoFFTwInputData()
{
  // 1st fill fIn
  unsigned int start = (unsigned int)(fStartTime/fTimeResolution);
  for (unsigned int i=0; i<fNoOfData; i++) {
    fIn[i][0] = fDataRebinned[i+start];
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

// has to be removed after testing
TH1F test_in("test_in", "test_in", fNoOfBins,
             fStartTime - fTimeResolution/2.0, fNoOfBins*fTimeResolution + fTimeResolution/2.0);
for (unsigned int i=0; i<fNoOfBins; i++)
  test_in.SetBinContent(i, fIn[i][0]);
TFile f("test_in.root", "RECREATE");
test_in.Write();
f.Close();
}
