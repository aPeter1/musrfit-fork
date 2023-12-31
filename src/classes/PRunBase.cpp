/***************************************************************************

  PRunBase.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2023 by Andreas Suter                              *
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

#include <iostream>

#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TFolder.h>

#include "PRunBase.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor. Needed otherwise vector's cannot be generated ;-)
 */
PRunBase::PRunBase()
{
  fRunNo = -1;
  fRunInfo = nullptr;
  fRawData = nullptr;
  fMetaData.fField  = PMUSR_UNDEFINED;
  fMetaData.fEnergy = PMUSR_UNDEFINED;
  fTimeResolution = -1.0;

  fFitStartTime = PMUSR_UNDEFINED;
  fFitEndTime   = PMUSR_UNDEFINED;

  fValid = true;
  fHandleTag = kEmpty;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param msrInfo pointer to the msr-file handler
 * \param rawData pointer to the raw-data handler
 * \param runNo msr-file run number
 * \param tag tag telling if fit, view, or rrf representation is whished.
 */
PRunBase::PRunBase(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag) :
    fHandleTag(tag), fMsrInfo(msrInfo), fRawData(rawData)
{
  fValid = true;

  fRunNo = static_cast<Int_t>(runNo);
  if (runNo > fMsrInfo->GetMsrRunList()->size()) {
    fRunInfo = nullptr;
    return;
  }

  // keep the run header info for this run
  fRunInfo = &(*fMsrInfo->GetMsrRunList())[runNo];

  // check the parameter and map range of the functions
  if (!fMsrInfo->CheckMapAndParamRange(static_cast<UInt_t>(fRunInfo->GetMap()->size()), fMsrInfo->GetNoOfParams())) {
    std::cerr << std::endl << "**SEVERE ERROR** PRunBase::PRunBase: map and/or parameter out of range in FUNCTIONS." << std::endl;
    exit(0);
  }

  // init private variables
  fMetaData.fField  = PMUSR_UNDEFINED;
  fMetaData.fEnergy = PMUSR_UNDEFINED;
  fTimeResolution = -1.0;
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++)
    fFuncValues.push_back(0.0);

  // generate theory
  fTheory = new PTheory(fMsrInfo, runNo);
  if (fTheory == nullptr) {
    std::cerr << std::endl << "**SEVERE ERROR** PRunBase::PRunBase: Couldn't create an instance of PTheory :-(, will quit" << std::endl;
    exit(0);
  }
  if (!fTheory->IsValid()) {
    std::cerr << std::endl << "**SEVERE ERROR** PRunBase::PRunBase: Theory is not valid :-(, will quit" << std::endl;
    exit(0);
  }

  // set fit time ranges
  fFitStartTime = PMUSR_UNDEFINED;
  fFitEndTime   = PMUSR_UNDEFINED;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor.
 */
PRunBase::~PRunBase()
{
  fT0s.clear();
  for (UInt_t i=0; i<fAddT0s.size(); i++)
    fAddT0s[i].clear();
  fAddT0s.clear();

  fFuncValues.clear();
}


//--------------------------------------------------------------------------
// SetFitRange (public)
//--------------------------------------------------------------------------
/**
 * <p> Sets the current fit range, and recalculated the number of fitted bins
 *
 * \param fitRange vector with fit ranges
 */
void PRunBase::SetFitRange(PDoublePairVector fitRange)
{
  Double_t start=0.0, end=0.0;

  assert(fitRange.size()); // make sure fitRange is not empty

  if (fitRange.size()==1) { // one fit range for all
    start = fitRange[0].first;
    end   = fitRange[0].second;
  } else {
    // check that fRunNo is found within fitRange
    UInt_t idx=static_cast<UInt_t>(fRunNo);
    if (idx < fitRange.size()) { // fRunNo present
      start = fitRange[idx].first;
      end   = fitRange[idx].second;
    } else { // fRunNo NOT present
      std::cerr << std::endl << ">> PRunBase::SetFitRange(): **ERROR** msr-file run entry " << fRunNo << " not present in fit range vector.";
      std::cerr << std::endl << ">>    Will not do anything! Please check, this shouldn't happen." << std::endl;
      return;
    }
  }

  // check that start is before end
  if (start > end) {
    std::cerr << std::endl << ">> PRunBase::SetFitRange(): **WARNING** start=" << start << " is > as end=" << end;
    std::cerr << std::endl << ">>    Will swap them, since otherwise chisq/logLH == 0.0" << std::endl;
    fFitStartTime = end;
    fFitEndTime   = start;
  } else {
    fFitStartTime = start;
    fFitEndTime   = end;
  }
}

//--------------------------------------------------------------------------
// CleanUp (public)
//--------------------------------------------------------------------------
/**
 * <p> Clean up all locally allocate memory
 */
void PRunBase::CleanUp()
{
  if (fTheory) {
    delete fTheory;
    fTheory = nullptr;
  }
}

//--------------------------------------------------------------------------
// CalculateKaiserFilterCoeff (protected)
//--------------------------------------------------------------------------
/**
 * <p>Calculates the Kaiser filter coefficients for a low pass filter with
 * a cut off frequency wc.
 * For details see "Zeitdiskrete Signalverarbeitung", A.V. Oppenheim, R.W. Schafer, J.R. Buck. Pearson 2004.
 *
 * \param wc cut off frequency
 * \param A defined as \f$ A = -\log_{10}(\delta) \f$, where \f$\delta\f$ is the tolerance band.
 * \param dw defined as \f$ \Delta\omega = \omega_{\rm S} - \omega_{\rm P} \f$, where \f$ \omega_{\rm S} \f$ is the
 * stop band frequency, and \f$ \omega_{\rm P} \f$ is the pass band frequency.
 */
void PRunBase::CalculateKaiserFilterCoeff(Double_t wc, Double_t A, Double_t dw)
{
  Double_t beta;
  Double_t dt = fData.GetTheoryTimeStep();
  Int_t m;

  // estimate beta (see reference above, p.574ff)
  if (A > 50.0) {
    beta = 0.1102*(A-8.7);
  } else if ((A >= 21.0) && (A <= 50.0)) {
    beta = 0.5842*TMath::Power(A-21.0, 0.4) + 0.07886*(A-21.0);
  } else {
    beta = 0.0;
  }

  m = TMath::FloorNint((A-8.0)/(2.285*dw*TMath::Pi()));
  // make sure m is odd
  if (m % 2 == 0)
    m++;

  Double_t alpha = static_cast<Double_t>(m)/2.0;
  Double_t dval;
  Double_t dsum = 0.0;
  for (Int_t i=0; i<=m; i++) {
    dval  = TMath::Sin(wc*(i-alpha)*dt)/(TMath::Pi()*(i-alpha)*dt);
    dval *= TMath::BesselI0(beta*TMath::Sqrt(1.0-TMath::Power((i-alpha)*dt/alpha, 2.0)))/TMath::BesselI0(beta);
    dsum += dval;
    fKaiserFilter.push_back(dval);
  }  
  for (UInt_t i=0; i<=static_cast<UInt_t>(m); i++) {
    fKaiserFilter[i] /= dsum;
  }
}

//--------------------------------------------------------------------------
// FilterTheo (protected)
//--------------------------------------------------------------------------
/**
 * <p>Filters the theory with a Kaiser FIR filter.
 */
void PRunBase::FilterTheo()
{
  PDoubleVector theoFiltered;
  Double_t dval = 0.0;
  const PDoubleVector *theo = fData.GetTheory();

  for (UInt_t i=0; i<theo->size(); i++) {
    for (UInt_t j=0; j<fKaiserFilter.size(); j++) {
      if (i<j)
        dval = 0.0;
      else
        dval += fKaiserFilter[j]*theo->at(i-j);
    }
    theoFiltered.push_back(dval);
    dval = 0.0;
  }

  fData.ReplaceTheory(theoFiltered);

  // shift time start by half the filter length
  dval = fData.GetTheoryTimeStart() - 0.5*static_cast<Double_t>(fKaiserFilter.size())*fData.GetTheoryTimeStep();
  fData.SetTheoryTimeStart(dval);

  theoFiltered.clear();
}
