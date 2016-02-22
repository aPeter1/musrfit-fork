/***************************************************************************

  PSimulateMuTransition.cpp

  Author: Thomas Prokscha
  Date: 25-Feb-2010

  $Id$

  Use root macros runMuSimulation.C and testAnalysis.C to run the simulation
  and to get a quick look on the data. Data are saved to a root histogram file
  with a structure similar to LEM histogram files; musrfit can be used to
  analyze the simulated data.

  Description:
  Root class to simulate muon spin phase under successive Mu+/Mu0 charge-exchange
  processes by a Monte-Carlo method. Consider transverse field geometry, and assume
  initial muon spin direction in x, and field applied along z. For PxMu(t) in 
  muonium use the equation 8.22 of the muSR book of Yaounc and Dalmas de Réotier, in
  slightly modified form (see Senba, J. Phys. B 23, 1545 (1990)); note that PxMu(t) 
  is given by a superposition of the four frequencies "nu_12", "nu_34", "nu_23", "nu_14".
  These frequencies and the corresponding probabilities ("SetMuFractionState12" for
  transitions 12 and 34, "SetMuFractionState23" for states 23 and 14) can be calculated
  for a given field with the root macro AnisotropicMu.C
 
  Parameters:
  1) Precession frequencies of "nu_12", "nu_34", "nu_23", "nu_14"
  2) fractions of nu_12, nu_34; and nu_23 and nu_14
  3) total Mu0 fraction
  4) electron-capture rate
  5) Mu ionization rate
  6) initial muon spin phase
  7) total muon decay asymmetry
  8) number of muon decays to be generated.
  9) debug flag: if TRUE print capture/ionization events on screen

  Output:
  Two histograms ("forward" and "backward") are written to a root file.

  The muon event simulation with a sequence of charge-changing processes is
  done in Event():
  simulate muon spin phase under charge-exchange with "4 Mu transitions" 
  1) according to Mu+/Mu0 fraction begin either with a Mu+ state or Mu state
  2) Mu+: determine next electron-capture time t_c. If t_c is larger than decay time t_d
     calculate muon spin precession for t_d; else calculate spin precession for t_c.
  3) Determine next ionization time t_i; calculate Px(t_i) in Muonium; calculate the 
     muon spin phase by acos(Px(t_i)).
  4) get the next electron capture time, continue until t_d is reached; accumulate muon spin
     phase.

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
#include <TComplex.h>

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
  fMuPrecFreq34     = 4463.; // vacuum Mu hyperfine coupling constant
  fMuPrecFreq12     = 0.;    // Mu precession frequency of a 12 transition
  fMuPrecFreq23     = 0.;    // Mu precession frequency of a 23 transition
  fMuPrecFreq14     = 0.;    // Mu precession frequency of a 14 transition
  fMuonPrecFreq     = 0.;    // muon precession frequency
  fBfield           = 0.01;  // magnetic field (T)
  fCaptureRate      = 0.01;  // Mu+ capture rate (MHz)
  fIonizationRate   = 10.;   // Mu0 ionization rate (MHz)
  fSpinFlipRate     = 0.001; // Mu0 spin flip rate (MHz)
  fInitialPhase     = 0.;
  fMuonPhase        = fInitialPhase;
  fMuonDecayTime    = 0.;
  fAsymmetry        = 0.27;
  fMuFraction       = 0.;
  fMuFractionState12 = 0.;
  fMuFractionState23 = 0.;
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
  cout << endl << "Mu precession frequency 12 (MHz)  = " << fMuPrecFreq12;
  cout << endl << "Mu precession frequency 34 (MHz)  = " << fMuPrecFreq34;
  cout << endl << "Mu precession frequency 23 (MHz)  = " << fMuPrecFreq23;
  cout << endl << "Mu precession frequency 14 (MHz)  = " << fMuPrecFreq14;
  cout << endl << "B field (T)                           = " << fBfield;
  cout << endl << "Mu+ electron capture rate (MHz)       = " << fCaptureRate;
  cout << endl << "Mu0 ionizatioan rate (MHz)            = " << fIonizationRate;
  cout << endl << "Mu0 spin-flip rate (MHz)              = " << fSpinFlipRate;
  cout << endl << "!!! Note: if spin-flip rate > 0.001 only spin-flip process is considered!!!";
  cout << endl << "Decay asymmetry                       = " << fAsymmetry;
  cout << endl << "Muonium fraction                      = " << fMuFraction;
  cout << endl << "Muonium fraction state12              = " << fMuFractionState12;
  cout << endl << "Muonium fraction state23              = " << fMuFractionState23;
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
    
    if (fSpinFlipRate > 0.001){// consider only Mu0 spin-flip in this case
      fMuonPhase = TMath::ACos(GTSpinFlip(fMuonDecayTime));
    }
    else{
      // initial muon state Mu+ or Mu0?
      if (fRandom->Rndm() <= 1.-fMuFraction) 
        Event("Mu+");
      else
        Event("");
    }
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
 * \param chargeState charge state of Mu ("Mu+" or  "Mu0")
 */
Double_t PSimulateMuTransition::PrecessionPhase(const Double_t &time, const TString chargeState)
{
  Double_t muonPhaseX;
  Double_t muoniumPolX = 0;
  
  if (chargeState == "Mu+")
    muonPhaseX = TMath::TwoPi()*fMuonPrecFreq*time;
  else if (chargeState == "Mu0"){
    muoniumPolX = GTFunction(time).Re();
    muonPhaseX = TMath::ACos(muoniumPolX);
  }
  else
    muonPhaseX = 0.;
  
  return muonPhaseX;
}

//--------------------------------------------------------------------------
// Mu0 transverse field polarization function (private)
//--------------------------------------------------------------------------
/**
 * <p>Calculates Mu0 polarization in x direction by superposition of four Mu0 frequencies
 *
 * \param time  (us);
 */
TComplex PSimulateMuTransition::GTFunction(const Double_t &time)
{
  Double_t twoPi = TMath::TwoPi();

  TComplex complexPol = 0; 
  complexPol = 
    0.5 * fMuFractionState12 * 
   (TComplex::Exp(TComplex::I()*twoPi*fMuPrecFreq12*time) +
    TComplex::Exp(-TComplex::I()*twoPi*fMuPrecFreq34*time))
    +
    0.5 * fMuFractionState23 * 
   (TComplex::Exp(TComplex::I()*twoPi*fMuPrecFreq23*time) +
    TComplex::Exp(TComplex::I()*twoPi*fMuPrecFreq14*time));
    
  return complexPol;

//   Double_t muoniumPolX = 0;
//   muoniumPolX = 0.5 * 
//    (fMuFractionState12 * (TMath::Cos(twoPi*fMuPrecFreq12*time) + TMath::Cos(twoPi*fMuPrecFreq34*time)) + 
//     fMuFractionState23 * (TMath::Cos(twoPi*fMuPrecFreq23*time) + TMath::Cos(twoPi*fMuPrecFreq14*time)));
//   
//   return muoniumPolX;

}

//--------------------------------------------------------------------------
// Mu0 transverse field polarization function after n spin-flip collisions (private)
//--------------------------------------------------------------------------
/**
 * <p>Calculates Mu0 polarization in x direction after n spin flip collisions.
 * See M. Senba, J.Phys. B24, 3531 (1991), equation (17)
 *
 * \param time  (us);
 */
Double_t PSimulateMuTransition::GTSpinFlip(const Double_t &time)
{
  TComplex complexPolX = 1.0;
  Double_t muoniumPolX = 1.0; //initial polarization in x direction
  Double_t eventTime =  0;
  Double_t eventDiffTime = 0;
  Double_t lastEventTime = 0;
  
  eventTime += NextEventTime(fSpinFlipRate);
  if (eventTime >= time){
   muoniumPolX = GTFunction(time).Re(); 
  }
  else{
   while (eventTime < time){
     eventDiffTime = eventTime - lastEventTime;
     complexPolX = complexPolX * GTFunction(eventDiffTime);
     lastEventTime = eventTime;
     eventTime += NextEventTime(fSpinFlipRate);
   }
   // calculate for the last collision
   eventDiffTime = time - lastEventTime;
   complexPolX = complexPolX * GTFunction(eventDiffTime);
   muoniumPolX = complexPolX.Re();
  }
 
  return muoniumPolX;
}

//--------------------------------------------------------------------------
// Event (private)
//--------------------------------------------------------------------------
/**
 * <p> Generates "muon event": simulate muon spin phase under charge-exchange with
 *     a neutral muonium state in transverse field, where the polarization evolution
 *     PxMu(t) of the muon spin in muonium is determined by a superposition of the 
 *     four "Mu transitions" nu_12, nu_34, nu_23, and nu_14.
 *     1) according to Mu+/Mu0 fraction begin either with a Mu+ state or Mu state
 *     2) Mu+: determine next electron-capture time t_c. If t_c is larger than decay time t_d
 *        calculate muon spin precession for t_d; else calculate spin precession for t_c.
 *     3) Determine next ionization time t_i; calculate Px(t_i) in Muonium; calculate the 
 *        muon spin phase by acos(Px(t_i)).
 *     4) get the next electron capture time, continue until t_d is reached.
 *
 * <p> For isotropic muonium, TF:
 *     nu_12 and nu_34 with equal probabilities, probability for both states fMuFractionState12
 *     ni_23 and nu_14 with equal probabilities, probability for both states fMuFractionState23
 *
 * \param muonString if eq. "Mu+" begin with Mu+ precession
 */
void PSimulateMuTransition::Event(const TString muonString)
{
  Double_t eventTime, eventDiffTime, captureTime, ionizationTime;
//   Double_t muonPrecessionFreq, muoniumPrecessionFreq; // MHz
//   Double_t rndm, frac1, frac2;

  fMuonPrecFreq = fMuonGyroRatio * fBfield; 

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
       fMuonPhase += PrecessionPhase(captureTime, "Mu+");
     else{ //muon decays; handle precession prior to muon decay
       eventDiffTime = fMuonDecayTime - (eventTime - captureTime);
       fMuonPhase += PrecessionPhase(eventDiffTime, "Mu+");
       break;
     }

     // now, we have Mu0; get next ionization time
     ionizationTime = NextEventTime(fIonizationRate);
     eventTime += ionizationTime;
     // determine Mu state
//      rndm = fRandom->Rndm();
//      frac1 = 1. - fMuFractionState1 - fMuFractionState2; // non-precessing Mu states
//      frac2 = 1. - fMuFractionState2;
//      if ( rndm < frac1 )
//        muoniumPrecessionFreq = 0.;
//      else if (rndm >= frac1 && rndm <= frac2){
//        if (fRandom->Rndm() <= 0.5) 
// 	muoniumPrecessionFreq = fMuPrecFreq12;
//        else
//         muoniumPrecessionFreq = fMuPrecFreq34;
//      }
//      else{
//        if (fRandom->Rndm() <= 0.5)
//         muoniumPrecessionFreq = fMuPrecFreq23;
//        else
//         muoniumPrecessionFreq = fMuPrecFreq14;
//      }

     if (fDebugFlag) cout << "Ioniza. time = " << ionizationTime << " Phase = " << fMuonPhase << endl;
     if (eventTime < fMuonDecayTime)
       fMuonPhase += PrecessionPhase(ionizationTime, "Mu0");
     else{ //muon decays; handle precession prior to muon decay
       eventDiffTime = fMuonDecayTime - (eventTime - ionizationTime);
       fMuonPhase += PrecessionPhase(eventDiffTime, "Mu0");
       break;
     }
   }
   else{
     // Mu0 as initial state; get next ionization time
     ionizationTime = NextEventTime(fIonizationRate);
     eventTime += ionizationTime;
     // determine Mu state
//      rndm = fRandom->Rndm();
//      frac1 = 1. - fMuFractionState1 - fMuFractionState2; // non-precessing Mu states
//      frac2 = 1. - fMuFractionState2;
//      if ( rndm < frac1 )
//        muoniumPrecessionFreq = 0.;
//      else if (rndm >= frac1 && rndm <= frac2){
//        if (fRandom->Rndm() <= 0.5) 
// 	muoniumPrecessionFreq = fMuPrecFreq12;
//        else
//         muoniumPrecessionFreq = fMuPrecFreq34;
//      }
//      else{
//        if (fRandom->Rndm() <= 0.5)
//         muoniumPrecessionFreq = fMuPrecFreq23;
//        else
//         muoniumPrecessionFreq = fMuPrecFreq14;
//      }

     if (fDebugFlag) 
      cout << "Mu Ioniza. time = " << ionizationTime << " Phase = " << fMuonPhase << endl;
     if (eventTime < fMuonDecayTime)
       fMuonPhase += PrecessionPhase(ionizationTime, "Mu0");
     else{ //muon decays; handle precession prior to muon decay
       eventDiffTime = fMuonDecayTime - (eventTime - ionizationTime);
       fMuonPhase += PrecessionPhase(eventDiffTime, "Mu0");
       break;
     }

     // Mu+ state; get next electron capture time
     captureTime = NextEventTime(fCaptureRate);
     eventTime += captureTime;
     if (fDebugFlag) cout << "Capture time = " << captureTime << " Phase = " << fMuonPhase << endl;
     if (eventTime < fMuonDecayTime)
       fMuonPhase += PrecessionPhase(captureTime, "Mu+");
     else{ //muon decays; handle precession prior to muon decay
       eventDiffTime = fMuonDecayTime - (eventTime - captureTime);
       fMuonPhase += PrecessionPhase(eventDiffTime, "Mu+");
       break;
     }
   }
  }

  if (fDebugFlag) cout << " Final Phase = " << fMuonPhase << endl;
  //fMuonPhase = TMath::ACos(TMath::Cos(fMuonPhase))*360./TMath::TwoPi(); //transform back to [0, 180] degree interval
  return;
}
