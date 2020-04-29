/***************************************************************************

  PFourier.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2020 by Andreas Suter                              *
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

#include "TF1.h"
#include "TAxis.h"

#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnUserParameters.h"
#include "Minuit2/MnMinimize.h"

#include "PMusr.h"
#include "PFourier.h"

#define PI      3.14159265358979312
#define PI_HALF 1.57079632679489656

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// PFTPhaseCorrection
//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PFTPhaseCorrection::PFTPhaseCorrection(const Int_t minBin, const Int_t maxBin) :
  fMinBin(minBin), fMaxBin(maxBin)
{
  Init();
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PFTPhaseCorrection::PFTPhaseCorrection(std::vector<Double_t> &reFT, std::vector<Double_t> &imFT, const Int_t minBin, const Int_t maxBin) :
  fReal(reFT), fImag(imFT), fMinBin(minBin), fMaxBin(maxBin)
{
  Init();

  Int_t realSize = static_cast<Int_t>(fReal.size());
  if (fMinBin == -1)
    fMinBin = 0;
  if (fMaxBin == -1)
    fMaxBin = realSize;
  if (fMaxBin > realSize)
    fMaxBin = realSize;

  fRealPh.resize(fReal.size());
  fImagPh.resize(fReal.size());
}

//--------------------------------------------------------------------------
// Minimize (public)
//--------------------------------------------------------------------------
/**
 *
 */
void PFTPhaseCorrection::Minimize()
{
  // create Minuit2 parameters
  ROOT::Minuit2::MnUserParameters upar;

  upar.Add("c0", fPh_c0, 2.0);
  upar.Add("c1", fPh_c1, 2.0);

  // create minimizer
  ROOT::Minuit2::MnMinimize mn_min(*this, upar);

  // minimize
  ROOT::Minuit2::FunctionMinimum min = mn_min();

  if (min.IsValid()) {
    fPh_c0 = min.UserState().Value("c0");
    fPh_c1 = min.UserState().Value("c1");
    fMin = min.Fval();
  } else {
    fMin = -1.0;
    fValid = false;
    std::cout << std::endl << ">> **WARNING** minimize failed to find a minimum for the real FT phase correction ..." << std::endl;
    return;
  }
}

//--------------------------------------------------------------------------
// GetPhaseCorrectionParam (public)
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PFTPhaseCorrection::GetPhaseCorrectionParam(UInt_t idx)
{
  Double_t result=0.0;

  if (idx == 0)
    result = fPh_c0;
  else if (idx == 1)
    result = fPh_c1;
  else
    std::cerr << ">> **ERROR** requested phase correction parameter with index=" << idx << " does not exist!" << std::endl;

  return result;
}

//--------------------------------------------------------------------------
// GetMinimum (public)
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PFTPhaseCorrection::GetMinimum()
{
  if (!fValid) {
    std::cerr << ">> **ERROR** requested minimum is invalid!" << std::endl;
    return -1.0;
  }

  return fMin;
}

//--------------------------------------------------------------------------
// Init (private)
//--------------------------------------------------------------------------
/**
 *
 */
void PFTPhaseCorrection::Init()
{
  fValid = true;
  fPh_c0 = 0.0;
  fPh_c1 = 0.0;
  fGamma = 1.0;
  fMin   = -1.0;
}

//--------------------------------------------------------------------------
// CalcPhasedFT (private)
//--------------------------------------------------------------------------
/**
 *
 */
void PFTPhaseCorrection::CalcPhasedFT() const
{
  Double_t phi=0.0;
  Double_t w=0.0;

  for (UInt_t i=0; i<fRealPh.size(); i++) {
    w = static_cast<Double_t>(i) / static_cast<Double_t>(fReal.size());
    phi = fPh_c0 + fPh_c1 * w;
    fRealPh[i] = fReal[i]*cos(phi) - fImag[i]*sin(phi);
    fImagPh[i] = fReal[i]*sin(phi) + fImag[i]*cos(phi);
  }
}

//--------------------------------------------------------------------------
// CalcRealPhFTDerivative (private)
//--------------------------------------------------------------------------
/**
 *
 */
void PFTPhaseCorrection::CalcRealPhFTDerivative() const
{
  fRealPhD.resize(fRealPh.size());

  fRealPhD[0] = 1.0;
  fRealPhD[fRealPh.size()-1] = 1.0;

  for (UInt_t i=1; i<fRealPh.size()-1; i++)
    fRealPhD[i] = fRealPh[i+1]-fRealPh[i];
}

//--------------------------------------------------------------------------
// Penalty (private)
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PFTPhaseCorrection::Penalty() const
{
  Double_t penalty = 0.0;

  for (UInt_t i=fMinBin; i<fMaxBin; i++) {
    if (fRealPh[i] < 0.0)
      penalty += fRealPh[i]*fRealPh[i];
  }

  return fGamma*penalty;
}

//--------------------------------------------------------------------------
// Entropy (private)
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PFTPhaseCorrection::Entropy() const
{
  Double_t norm = 0.0;

  for (UInt_t i=fMinBin; i<fMaxBin; i++)
    norm += fabs(fRealPhD[i]);

  Double_t entropy = 0.0;
  Double_t dval = 0.0, hh = 0.0;

  for (UInt_t i=fMinBin; i<fMaxBin; i++) {
    dval = fabs(fRealPhD[i]);
    if (dval > 1.0e-15) {
      hh = dval / norm;
      entropy -= hh * log(hh);
    }
  }

  return entropy;
}

//--------------------------------------------------------------------------
// operator() (private)
//--------------------------------------------------------------------------
/**
 *
 */
double PFTPhaseCorrection::operator()(const std::vector<double> &par) const
{
  // par : [0]: c0, [1]: c1

  fPh_c0 = par[0];
  fPh_c1 = par[1];

  CalcPhasedFT();
  CalcRealPhFTDerivative();

  return Entropy()+Penalty();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// PFourier
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
  if (fData == nullptr) {
    std::cerr << std::endl << "**ERROR** PFourier::PFourier: no valid data" << std::endl << std::endl;
    fValid = false;
    return;
  }

  fValid = true;
  fIn  = nullptr;
  fOut = nullptr;
//as  fPhCorrectedReFT = 0;

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
  fNoOfData = static_cast<UInt_t>((fEndTime-fStartTime)/fTimeResolution);

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
  }

  // allocate necessary memory
  fIn  = static_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex)*fNoOfBins));
  fOut = static_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex)*fNoOfBins));

  // check if memory allocation has been successful
  if ((fIn == nullptr) || (fOut == nullptr)) {
    fValid = false;
    return;
  }

  // get the FFTW3 plan (see FFTW3 manual)
  fFFTwPlan = fftw_plan_dft_1d(static_cast<Int_t>(fNoOfBins), fIn, fOut, FFTW_FORWARD, FFTW_ESTIMATE);

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
//as  if (fPhCorrectedReFT)
//as    delete fPhCorrectedReFT;
}

//--------------------------------------------------------------------------
// Transform (public)
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
// GetMaxFreq (public)
//--------------------------------------------------------------------------
/**
 * <p>returns the maximal frequency in units choosen, i.e. Gauss, Tesla, MHz, Mc/s
 */
Double_t PFourier::GetMaxFreq()
{
  UInt_t noOfFourierBins = 0;
  if (fNoOfBins % 2 == 0)
    noOfFourierBins = fNoOfBins/2;
  else
    noOfFourierBins = (fNoOfBins+1)/2;

  return fResolution*noOfFourierBins;
}

//--------------------------------------------------------------------------
// GetRealFourier (public)
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
    return nullptr;

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

  TH1F *realFourier = new TH1F(name, title, static_cast<Int_t>(noOfFourierBins), -fResolution/2.0, static_cast<Double_t>(noOfFourierBins-1)*fResolution+fResolution/2.0);
  if (realFourier == nullptr) {
    fValid = false;
    std::cerr << std::endl << "**SEVERE ERROR** couldn't allocate memory for the real part of the Fourier transform." << std::endl;
    return nullptr;
  }

  // fill realFourier vector
  for (Int_t i=0; i<static_cast<Int_t>(noOfFourierBins); i++) {
    realFourier->SetBinContent(i+1, scale*fOut[i][0]);
    realFourier->SetBinError(i+1, 0.0);
  }
  return realFourier;
}

//--------------------------------------------------------------------------
// GetPhaseOptRealFourier (public, static)
//--------------------------------------------------------------------------
/**
 * <p>returns the phase corrected real Fourier transform.
 *
 * \return the TH1F histo of the phase 'optimzed' real Fourier transform.
 *
 * \param re real part Fourier histogram
 * \param im imaginary part Fourier histogram
 * \param phase return value of the optimal phase dispersion phase[0]+phase[1]*i/N
 * \param scale normalisation factor
 * \param min minimal freq / field from which to optimise. Given in the choosen unit.
 * \param max maximal freq / field up to which to optimise. Given in the choosen unit.
 */
TH1F* PFourier::GetPhaseOptRealFourier(const TH1F *re, const TH1F *im, std::vector<Double_t> &phase,
                                       const Double_t scale, const Double_t min, const Double_t max)
{
  if ((re == nullptr) || (im == nullptr))
    return nullptr;

  phase.resize(2); // c0, c1

  const TAxis *axis = re->GetXaxis();

  Int_t minBin = 1;
  Int_t maxBin = axis->GetNbins();
  Int_t noOfBins = axis->GetNbins();
  Double_t res = axis->GetBinWidth(1);

  // check if minimum frequency is given. If yes, get the proper minBin
  if (min != -1.0) {
    minBin = axis->FindFixBin(min);
    if ((minBin == 0) || (minBin > maxBin)) {
      minBin = 1;
      std::cerr << "**WARNING** minimum frequency/field out of range. Will adopted it." << std::endl;
    }
  }

  // check if maximum frequency is given. If yes, get the proper maxBin
  if (max != -1.0) {
    maxBin = axis->FindFixBin(max);
    if ((maxBin == 0) || (maxBin > axis->GetNbins())) {
      maxBin = axis->GetNbins();
      std::cerr << "**WARNING** maximum frequency/field out of range. Will adopted it." << std::endl;
    }
  }

  // copy the real/imag Fourier from min to max
  std::vector<Double_t> realF, imagF;
  for (Int_t i=minBin; i<=maxBin; i++) {
    realF.push_back(re->GetBinContent(i));
    imagF.push_back(im->GetBinContent(i));
  }

  // optimize real FT phase
  PFTPhaseCorrection *phCorrectedReFT = new PFTPhaseCorrection(realF, imagF);
  if (phCorrectedReFT == nullptr) {
    std::cerr << std::endl << "**SEVERE ERROR** couldn't invoke PFTPhaseCorrection object." << std::endl;
    return nullptr;
  }

  phCorrectedReFT->Minimize();
  if (!phCorrectedReFT->IsValid()) {
    std::cerr << std::endl << "**ERROR** could not find a valid phase correction minimum." << std::endl;
    return nullptr;
  }
  phase[0] = phCorrectedReFT->GetPhaseCorrectionParam(0);
  phase[1] = phCorrectedReFT->GetPhaseCorrectionParam(1);

  // clean up
  if (phCorrectedReFT) {
    delete phCorrectedReFT;
    phCorrectedReFT = nullptr;
  }
  realF.clear();
  imagF.clear();

  // invoke the real phase optimised histo to be filled. Caller is the owner!
  Char_t name[256];
  Char_t title[256];
  snprintf(name, sizeof(name), "%s_Fourier_PhOptRe", re->GetName());
  snprintf(title, sizeof(title), "%s_Fourier_PhOptRe", re->GetTitle());

  TH1F *realPhaseOptFourier = new TH1F(name, title, noOfBins, -res/2.0, static_cast<Double_t>(noOfBins-1)*res+res/2.0);
  if (realPhaseOptFourier == nullptr) {
    std::cerr << std::endl << "**SEVERE ERROR** couldn't allocate memory for the real part of the Fourier transform." << std::endl;
    return nullptr;
  }

  // fill realFourier vector
  Double_t ph;
  for (Int_t i=0; i<noOfBins; i++) {
    ph = phase[0] + phase[1] * static_cast<Double_t>(i-static_cast<Int_t>(minBin)) / static_cast<Double_t>(maxBin-minBin);
    realPhaseOptFourier->SetBinContent(i+1, scale*(re->GetBinContent(i+1)*cos(ph) - im->GetBinContent(i+1)*sin(ph)));
    realPhaseOptFourier->SetBinError(i+1, 0.0);
  }

  return realPhaseOptFourier;
}

//--------------------------------------------------------------------------
// GetImaginaryFourier (public)
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
    return nullptr;

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

  TH1F* imaginaryFourier = new TH1F(name, title, static_cast<Int_t>(noOfFourierBins), -fResolution/2.0, static_cast<Double_t>(noOfFourierBins-1)*fResolution+fResolution/2.0);
  if (imaginaryFourier == nullptr) {
    fValid = false;
    std::cerr << std::endl << "**SEVERE ERROR** couldn't allocate memory for the imaginary part of the Fourier transform." << std::endl;
    return nullptr;
  }

  // fill imaginaryFourier vector
  for (Int_t i=0; i<static_cast<Int_t>(noOfFourierBins); i++) {
    imaginaryFourier->SetBinContent(i+1, scale*fOut[i][1]);
    imaginaryFourier->SetBinError(i+1, 0.0);
  }

  return imaginaryFourier;
}

//--------------------------------------------------------------------------
// GetPowerFourier (public)
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
    return nullptr;

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

  TH1F* pwrFourier = new TH1F(name, title, static_cast<Int_t>(noOfFourierBins), -fResolution/2.0, static_cast<Double_t>(noOfFourierBins-1)*fResolution+fResolution/2.0);
  if (pwrFourier == nullptr) {
    fValid = false;
    std::cerr << std::endl << "**SEVERE ERROR** couldn't allocate memory for the power part of the Fourier transform." << std::endl;
    return nullptr;
  }

  // fill powerFourier vector
  for (Int_t i=0; i<static_cast<Int_t>(noOfFourierBins); i++) {
    pwrFourier->SetBinContent(i+1, scale*sqrt(fOut[i][0]*fOut[i][0]+fOut[i][1]*fOut[i][1]));
    pwrFourier->SetBinError(i+1, 0.0);
  }

  return pwrFourier;
}

//--------------------------------------------------------------------------
// GetPhaseFourier (public)
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
    return nullptr;

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

  TH1F* phaseFourier = new TH1F(name, title, static_cast<Int_t>(noOfFourierBins), -fResolution/2.0, static_cast<Double_t>(noOfFourierBins-1)*fResolution+fResolution/2.0);
  if (phaseFourier == nullptr) {
    fValid = false;
    std::cerr << std::endl << "**SEVERE ERROR** couldn't allocate memory for the phase part of the Fourier transform." << std::endl;
    return nullptr;
  }

  // fill phaseFourier vector
  Double_t value = 0.0;
  for (Int_t i=0; i<static_cast<Int_t>(noOfFourierBins); i++) {
    // calculate the phase
    if (fOut[i][0] == 0.0) {
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
// PrepareFFTwInputData (private)
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
      mean += fData->GetBinContent(static_cast<Int_t>(i+start));
    }
    mean /= static_cast<Double_t>(fNoOfData);
  }

  // 2nd fill fIn
  for (UInt_t i=0; i<fNoOfData; i++) {
    fIn[i][0] = fData->GetBinContent(static_cast<Int_t>(i+start)) - mean;
    fIn[i][1] = 0.0;
  }
  for (UInt_t i=fNoOfData; i<fNoOfBins; i++) {
    fIn[i][0] = 0.0;
    fIn[i][1] = 0.0;
  }

  // 3rd apodize data (if wished)
  ApodizeData(static_cast<Int_t>(apodizationTag));
}

//--------------------------------------------------------------------------
// ApodizeData (private)
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
      std::cerr << std::endl << ">> **ERROR** User Apodization tag " << apodizationTag << " unknown, sorry ..." << std::endl;
      break;
  }

  Double_t q;
  for (UInt_t i=0; i<fNoOfData; i++) {
    q = c[0];
    for (UInt_t j=1; j<5; j++) {
      q += c[j] * pow(static_cast<Double_t>(i)/static_cast<Double_t>(fNoOfData), 2.0*static_cast<Double_t>(j));
    }
    fIn[i][0] *= q;
  }
}
