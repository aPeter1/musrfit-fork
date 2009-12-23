/***************************************************************************

  PRunBase.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
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
//#include <TFile.h>
#include <TFolder.h>

#include "TLemRunHeader.h"

#include "PRunBase.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p> needed otherwise vector's cannot be generated ;-)
 *
 */
PRunBase::PRunBase()
{
  fRunNo = -1;
  fRunInfo = 0;
  fRawData = 0;
  fTimeResolution = -1.0;

  fValid = true;
  fHandleTag = kEmpty;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param msrInfo pointer to the msr info structure
 * \param rawData
 * \param runNo
 * \param tag
 */
PRunBase::PRunBase(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag)
{
  fValid = true;
  fHandleTag = tag;

  fRunNo = static_cast<Int_t>(runNo);
  if ((runNo < 0) || (runNo > msrInfo->GetMsrRunList()->size())) {
    fRunInfo = 0;
    return;
  }

  // keep pointer to the msr-file handler
  fMsrInfo = msrInfo;

  // keep the run header info for this run
  fRunInfo = &(*msrInfo->GetMsrRunList())[runNo];

  // check the parameter and map range of the functions
  if (!fMsrInfo->CheckMapAndParamRange(fRunInfo->GetMap()->size(), fMsrInfo->GetNoOfParams())) {
    cerr << endl << "**SEVERE ERROR** PRunBase::PRunBase: map and/or parameter out of range in FUNCTIONS." << endl;
    exit(0);
  }

  // keep the raw data of the runs
  fRawData = rawData;

  // init private variables
  fTimeResolution = -1.0;
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++)
    fFuncValues.push_back(0.0);

  // generate theory
  fTheory = new PTheory(msrInfo, runNo);
  if (fTheory == 0) {
    cerr << endl << "**SEVERE ERROR** PRunBase::PRunBase: Couldn't create an instance of PTheory :-(, will quit" << endl;
    exit(0);
  }
  if (!fTheory->IsValid()) {
    cerr << endl << "**SEVERE ERROR** PRunBase::PRunBase: Theory is not valid :-(, will quit" << endl;
    exit(0);
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
PRunBase::~PRunBase()
{
  fParamNo.clear();

  fT0s.clear();

  fFuncValues.clear();
}

//--------------------------------------------------------------------------
// CleanUp
//--------------------------------------------------------------------------
/**
 * <p> Clean up all localy allocate memory
 *
 */
void PRunBase::CleanUp()
{
  if (fTheory) {
    delete fTheory;
    fTheory = 0;
  }
}

//--------------------------------------------------------------------------
// CalculateKaiserFilterCoeff (protected)
//--------------------------------------------------------------------------
/**
 * <p> Calculates the Kaiser filter coefficients for a low pass filter with
 * a cut off frequency wc.
 * For details see "Zeitdiskrete Signalverarbeitung", A.V. Oppenheim, R.W. Schafer, J.R. Buck. Pearson 2004.
 *
 * \param wc cut off frequency
 * \param A defined as \f[ A = -\log_{10}(\delta) \f], where \f[\delta\f] is the tolerance band.
 * \param dw defined as \f[ \Delta\omega = \omega_{\rm S} - \omega_{\rm P} \f], where \f[ \omega_{\rm S} \f] is the
 * stop band frequency, and \f[ \omega_{\rm P} \f] is the pass band frequency.
 */
void PRunBase::CalculateKaiserFilterCoeff(Double_t wc, Double_t A, Double_t dw)
{
  Double_t beta;
  Double_t dt = fData.GetTheoryTimeStep();
  UInt_t m;

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
  for (UInt_t i=0; i<=m; i++) {
    dval  = TMath::Sin(wc*(i-alpha)*dt)/(TMath::Pi()*(i-alpha)*dt);
    dval *= TMath::BesselI0(beta*TMath::Sqrt(1.0-TMath::Power((i-alpha)*dt/alpha, 2.0)))/TMath::BesselI0(beta);
    dsum += dval;
    fKaiserFilter.push_back(dval);
  }  
  for (UInt_t i=0; i<=m; i++) {
    fKaiserFilter[i] /= dsum;
  }

/*
for (UInt_t i=0; i<=m; i++)
cout << endl << ">> " << i << ", fKaiserFilter[" << i << "]=" << fKaiserFilter[i];
cout << endl;
*/
}

//--------------------------------------------------------------------------
// FilterData (protected)
//--------------------------------------------------------------------------
/**
 * <p> Filters the data with a Kaiser FIR filter.
 */
void PRunBase::FilterData()
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
