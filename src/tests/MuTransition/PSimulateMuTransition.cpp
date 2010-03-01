/***************************************************************************

  PSimulateMuTransition.cpp

  Author: Thomas Prokscha
  Date: 25-Feb-2010

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010 by Thomas Prokscha, Paul Scherrer Institut         *
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
using namespace std;

#include <TMath.h>

#include "PSimulateMuTransition.h"

ClassImp(PSimulateMuTransition)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p> Constructor.
 *
 * \param seed for the random number generator
 */
PSimulateMuTransition::PSimulateMuTransition(UInt_t seed)
{
  fValid = true;

  fRandom = new TRandom2(seed);
  if (fRandom == 0) {
    fValid = false;
  }

  fNmuons           = 100;   // number of muons to simulate
  fMuPrecFreq1      = 4463.; // vacuum Mu hyperfine coupling constant
  fMuPrecFreq2      = 0.;    // Mu precession frequency of a 2nd Mu transition
  fBfield           = 0.01;  // magnetic field (T)
  fCaptureRate      = 0.01;  // Mu+ capture rate (MHz)
  fIonizationRate   = 10.;   // Mu0 ionization rate (MHz)
  fInitialPhase     = 0.;
  fMuonPhase        = fInitialPhase;
  fMuonDecayTime    = 0.;
  fAsymmetry        = 0.27;
  fMuFraction       = 0.;
  fMuFractionState1 = 0.;
  fMuFractionState2 = 0.;
  fDebugFlag        = kFALSE;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p> Destructor.
 *
 */
PSimulateMuTransition::~PSimulateMuTransition()
{
  if (fRandom) {
    delete fRandom;
    fRandom = 0;
  }
}
//--------------------------------------------------------------------------
// Output of current settings
//--------------------------------------------------------------------------
/*!
 * <p>Prints the current settings onto std output.
 */
void PSimulateMuTransition::PrintSettings() const
{
  cout << endl << "Mu precession frequency state1 (MHz)  = " << fMuPrecFreq1;
  cout << endl << "Mu precession frequency state2 (MHz)  = " << fMuPrecFreq2;
  cout << endl << "B field (T)                           = " << fBfield;
  cout << endl << "Mu+ electron capture rate (MHz)       = " << fCaptureRate;
  cout << endl << "Mu ionizatioan rate (MHz)             = " << fIonizationRate;
  cout << endl << "Decay asymmetry                       = " << fAsymmetry;
  cout << endl << "Muonium fraction                      = " << fMuFraction;
  cout << endl << "Muonium fraction state1               = " << fMuFractionState1;
  cout << endl << "Muonium fraction state2               = " << fMuFractionState2;
  cout << endl << "Number of particles to simulate       = " << fNmuons;
  cout << endl << "Initial muon spin phase (degree)      = " << fInitialPhase;
  cout << endl << "Debug flag                            = " << fDebugFlag;
  cout << endl << endl;
}

//--------------------------------------------------------------------------
// SetSeed (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the seed of the random generator.
 *
 * \param seed for the random number generator
 */
void PSimulateMuTransition::SetSeed(UInt_t seed)
{
  if (!fValid)
    return;

  fRandom->SetSeed(seed);
}

//--------------------------------------------------------------------------
// Run (public)
//--------------------------------------------------------------------------
/**
  * \param histoForward
 */
void PSimulateMuTransition::Run(TH1F *histoForward, TH1F *histoBackward)
{
  Int_t i;
  if (histoForward == 0 || histoBackward == 0)
    return;

  for (i = 0; i<fNmuons; i++){
    fMuonPhase         = TMath::TwoPi() * fInitialPhase/360.; // transform to radians
    fMuonDecayTime     = NextEventTime(fMuonDecayRate);
    // initial muon state Mu+ or Mu0?
    if (fRandom->Rndm() <= 1.-fMuFraction) 
     Event("Mu+");
    else
     Event("");

    // fill 50% in "forward", and 50% in "backward" detector to get independent
    // events in "forward" and "backward" histograms. This allows "normal" uSR
    // analysis of the data
    // change muon decay time to ns
    if (fRandom->Rndm() <= 0.5) 
      histoForward->Fill(fMuonDecayTime*1000., 1. + fAsymmetry*TMath::Cos(fMuonPhase));
    else
      histoBackward->Fill(fMuonDecayTime*1000., 1. - fAsymmetry*TMath::Cos(fMuonPhase));

    if ( (i%100000) == 0) cout << "number of events processed: " << i << endl;
  }
  cout << "number of events processed: " << i << endl;
  return;
}

//--------------------------------------------------------------------------
// NextEventTime (private)
//--------------------------------------------------------------------------
/**
 * <p>Determine time of next event, assuming "Poisson" distribution in time
 *
 * \param EventRate event rate in MHz; returns next event time in micro-seconds
 */
Double_t PSimulateMuTransition::NextEventTime(const Double_t &EventRate)
{
  if (EventRate <= 0.)
    return -1.; // signal error

  return -1./EventRate * TMath::Log(fRandom->Rndm());
}

//--------------------------------------------------------------------------
// Phase (private)
//--------------------------------------------------------------------------
/**
 * <p>Determines phase of the muon spin
 *
 * \param time duration of precession (us);
 * \param frequency muon spin precession frequency (MHz);
 */
Double_t PSimulateMuTransition::PrecessionPhase(const Double_t &time, const Double_t &frequency)
{
  return TMath::TwoPi()*frequency*time;
}

//--------------------------------------------------------------------------
// Event (private)
//--------------------------------------------------------------------------
/**
 * <p> Generates "one muon event": simulate muon phase under free precession at
 * external field and Mu precession
 *
 * \param muonString if eq. "Mu+" begin with Mu+ precession
 */
void PSimulateMuTransition::Event(const TString muonString)
{
  Double_t eventTime, eventDiffTime, captureTime, ionizationTime;
  Double_t muonPrecessionFreq, muoniumPrecessionFreq; // MHz
  Double_t rndm, frac1, frac2;

  muonPrecessionFreq = fMuonGyroRatio * fBfield; 

  // charge-exchange loop until muon decay
  eventTime     = 0.;
  eventDiffTime = 0.;

  if (fDebugFlag) cout << "Decay time = " << fMuonDecayTime << endl;
  //cout << muonString << endl;
  while (1) {
   if (muonString == "Mu+"){
     // Mu+ initial state; get next electron capture time
     captureTime = NextEventTime(fCaptureRate);
     eventTime += captureTime;
     if (fDebugFlag) cout << "Capture time = " << captureTime << " Phase = " << fMuonPhase << endl;
     if (eventTime < fMuonDecayTime)
       fMuonPhase += PrecessionPhase(captureTime, muonPrecessionFreq);
     else{ //muon decays; handle precession prior to muon decay
       eventDiffTime = fMuonDecayTime - (eventTime - captureTime);
       fMuonPhase += PrecessionPhase(eventDiffTime, muonPrecessionFreq);
       break;
     }

     // now, we have Mu0; get next ionization time
     ionizationTime = NextEventTime(fIonizationRate);
     eventTime += ionizationTime;
     // determine Mu state
     rndm = fRandom->Rndm();
     frac1 = 1. - fMuFractionState1 - fMuFractionState2; // non-precessing Mu states
     frac2 = 1. - fMuFractionState2;
     if ( rndm < frac1 )
       muoniumPrecessionFreq = 0.;
     else if (rndm >= frac1 && rndm <= frac2)
       muoniumPrecessionFreq = fMuPrecFreq1;
     else
       muoniumPrecessionFreq = fMuPrecFreq2;

     if (fDebugFlag) cout << "Ioniza. time = " << ionizationTime << " Freq = " << muoniumPrecessionFreq 
                          << " Phase = " << fMuonPhase << endl;
     if (eventTime < fMuonDecayTime)
       fMuonPhase += PrecessionPhase(ionizationTime, muoniumPrecessionFreq);
     else{ //muon decays; handle precession prior to muon decay
       eventDiffTime = fMuonDecayTime - (eventTime - ionizationTime);
       fMuonPhase += PrecessionPhase(eventDiffTime, muoniumPrecessionFreq);
       break;
     }
   }
   else{
     // Mu0 as initial state; get next ionization time
     ionizationTime = NextEventTime(fIonizationRate);
     eventTime += ionizationTime;
     // determine Mu state
     rndm = fRandom->Rndm();
     frac1 = 1. - fMuFractionState1 - fMuFractionState2; // non-precessing Mu states
     frac2 = 1. - fMuFractionState2;
     if ( rndm < frac1 )
       muoniumPrecessionFreq = 0.;
     else if (rndm >= frac1 && rndm <= frac2)
       muoniumPrecessionFreq = fMuPrecFreq1;
     else
       muoniumPrecessionFreq = fMuPrecFreq2;

     if (fDebugFlag) 
      cout << "Mu Ioniza. time = " << ionizationTime << " Freq = " << muoniumPrecessionFreq 
         << " Phase = " << fMuonPhase << endl;
     if (eventTime < fMuonDecayTime)
       fMuonPhase += PrecessionPhase(ionizationTime, muoniumPrecessionFreq);
     else{ //muon decays; handle precession prior to muon decay
       eventDiffTime = fMuonDecayTime - (eventTime - ionizationTime);
       fMuonPhase += PrecessionPhase(eventDiffTime, muoniumPrecessionFreq);
       break;
     }

     // Mu+ state; get next electron capture time
     captureTime = NextEventTime(fCaptureRate);
     eventTime += captureTime;
     if (fDebugFlag) cout << "Capture time = " << captureTime << " Phase = " << fMuonPhase << endl;
     if (eventTime < fMuonDecayTime)
       fMuonPhase += PrecessionPhase(captureTime, muonPrecessionFreq);
     else{ //muon decays; handle precession prior to muon decay
       eventDiffTime = fMuonDecayTime - (eventTime - captureTime);
       fMuonPhase += PrecessionPhase(eventDiffTime, muonPrecessionFreq);
       break;
     }
   }
  }

  if (fDebugFlag) cout << " Final Phase = " << fMuonPhase << endl;
  //fMuonPhase = TMath::ACos(TMath::Cos(fMuonPhase))*360./TMath::TwoPi(); //transform back to [0, 180] degree interval
  return;
}
