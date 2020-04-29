/***************************************************************************

  PFourier.h

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

#ifndef _PFOURIER_H_
#define _PFOURIER_H_

#include <vector>

#include "fftw3.h"

#include <TH1F.h>

#include "Minuit2/FCNBase.h"

#include "PMusr.h"

#define F_APODIZATION_NONE   1
#define F_APODIZATION_WEAK   2
#define F_APODIZATION_MEDIUM 3
#define F_APODIZATION_STRONG 4

/**
 * Re Fourier phase correction
 */
class PFTPhaseCorrection : public ROOT::Minuit2::FCNBase
{
  public:
    PFTPhaseCorrection(const Int_t minBin=-1, const Int_t maxBin=-1);
    PFTPhaseCorrection(std::vector<Double_t> &reFT, std::vector<Double_t> &imFT, const Int_t minBin=-1, const Int_t maxBin=-1);
    virtual ~PFTPhaseCorrection() {}

    virtual Bool_t IsValid() { return fValid; }
    virtual void Minimize();

    virtual void SetGamma(const Double_t gamma) { fGamma = gamma; }
    virtual void SetPh(const Double_t c0, const Double_t c1) { fPh_c0 = c0; fPh_c1 = c1; CalcPhasedFT(); CalcRealPhFTDerivative(); }

    virtual Double_t GetGamma() { return fGamma; }
    virtual Double_t GetPhaseCorrectionParam(UInt_t idx);
    virtual Double_t GetMinimum();

  private:
    Bool_t fValid;

    std::vector<Double_t> fReal; /// original real Fourier data set
    std::vector<Double_t> fImag; /// original imag Fourier data set
    mutable std::vector<Double_t> fRealPh;  /// phased real Fourier data set
    mutable std::vector<Double_t> fImagPh;  /// phased imag Fourier data set
    mutable std::vector<Double_t> fRealPhD; /// 1st derivative of fRealPh

    Int_t fMinBin; /// minimum bin from Fourier range to be used for the phase correction estimate
    Int_t fMaxBin; /// maximum bin from Fourier range to be used for the phase correction estimate
    mutable Double_t fPh_c0; /// constant part of the phase dispersion used for the phase correction
    mutable Double_t fPh_c1; /// linear part of the phase dispersion used for the phase correction
    Double_t fGamma; /// gamma parameter to balance between entropy and penalty
    Double_t fMin; /// keeps the minimum of the entropy/penalty minimization

    virtual void Init();
    virtual void CalcPhasedFT() const;
    virtual void CalcRealPhFTDerivative() const;
    virtual Double_t Penalty() const;
    virtual Double_t Entropy() const;

    virtual Double_t Up() const { return 1.0; }
    virtual Double_t operator()(const std::vector<Double_t>&) const;
};

/**
 * muSR Fourier class.
 */
class PFourier
{
  public:
    PFourier(TH1F *data, Int_t unitTag,
             Double_t startTime = 0.0, Double_t endTime = 0.0,
             Bool_t dcCorrected = false, UInt_t zeroPaddingPower = 0);
    virtual ~PFourier();

    virtual void Transform(UInt_t apodizationTag = 0);

    virtual const char* GetDataTitle() { return fData->GetTitle(); }
    virtual const Int_t GetUnitTag() { return fUnitTag; }
    virtual Double_t GetResolution() { return fResolution; }
    virtual Double_t GetMaxFreq();
    virtual TH1F* GetRealFourier(const Double_t scale = 1.0);
//as    virtual TH1F* GetPhaseOptRealFourier(std::vector<Double_t> &phase, const Double_t scale = 1.0, const Double_t min = -1.0, const Double_t max = -1.0);
    virtual TH1F* GetImaginaryFourier(const Double_t scale = 1.0);
    virtual TH1F* GetPowerFourier(const Double_t scale = 1.0);
    virtual TH1F* GetPhaseFourier(const Double_t scale = 1.0);

    static TH1F* GetPhaseOptRealFourier(const TH1F *re, const TH1F *im, std::vector<Double_t> &phase,
                                        const Double_t scale = 1.0, const Double_t min = -1.0, const Double_t max = -1.0);

    virtual Bool_t IsValid() { return fValid; }

  private:
    TH1F *fData; ///< data histogram to be Fourier transformed.

    Bool_t fValid; ///< true = all boundary conditions fullfilled and hence a Fourier transform can be performed.
    Int_t  fUnitTag; ///< 1=Field Units (G), 2=Field Units (T), 3=Frequency Units (MHz), 4=Angular Frequency Units (Mc/s)

    Int_t fApodization; ///< 0=none, 1=weak, 2=medium, 3=strong

    Double_t fTimeResolution; ///< time resolution of the data histogram in (us)
    Double_t fStartTime; ///< start time of the data histogram
    Double_t fEndTime; ///< end time of the data histogram
    Bool_t fDCCorrected; ///< if true, removed DC offset from signal before Fourier transformation, otherwise not
    UInt_t fZeroPaddingPower; ///< power for zero padding, if set < 0 no zero padding will be done
    Double_t fResolution; ///< Fourier resolution (field, frequency, or angular frequency)

    UInt_t fNoOfData; ///< number of bins in the time interval between fStartTime and fStopTime
    UInt_t fNoOfBins; ///< number of bins to be Fourier transformed. Might be different to fNoOfData due to zero padding
    fftw_plan fFFTwPlan; ///< fftw plan (see FFTW3 User Manual)
    fftw_complex *fIn; ///< real part of the Fourier transform
    fftw_complex *fOut; ///< imaginary part of the Fourier transform

//as    PFTPhaseCorrection *fPhCorrectedReFT;

    virtual void PrepareFFTwInputData(UInt_t apodizationTag);
    virtual void ApodizeData(Int_t apodizationTag);
};

#endif // _PFOURIER_H_
