/***************************************************************************

  PSimulateMuTransition.h

  Author: Thomas Prokscha
  Date:   25-Feb-2010

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

#ifndef _PSIMULATEMUTRANSITION_H_
#define _PSIMULATEMUTRANSITION_H_

#include <TObject.h>
#include <TH1F.h>
#include <TRandom2.h>
#include <TComplex.h>

// global constants
const Double_t fMuonGyroRatio = 135.54; //!< muon gyromagnetic ratio (MHz/T)
const Double_t fMuonDecayRate = 0.4551; //!< muon decay rate (1/tau_mu, MHz)

class PSimulateMuTransition : public TObject
{
  public:
    PSimulateMuTransition(UInt_t seed = 0);
    virtual ~PSimulateMuTransition();

    virtual void PrintSettings() const;
    virtual void SetNmuons(Int_t value)        { fNmuons = value; }       //!< number of muons
    virtual void SetDebugFlag(Bool_t value)    { fDebugFlag = value; }    //!< debug flag
    virtual void SetBfield(Double_t value)     { fBfield = value; }       //!< sets magnetic field (T)
    virtual void SetMuPrecFreq12(Double_t value) { fMuPrecFreq12 = value; } //!< sets Mu transition frequency (MHz)
    virtual void SetMuPrecFreq34(Double_t value) { fMuPrecFreq34 = value; } //!< sets Mu transition frequency (MHz)
    virtual void SetMuPrecFreq23(Double_t value) { fMuPrecFreq23 = value; } //!< sets Mu transition frequency (MHz)
    virtual void SetMuPrecFreq14(Double_t value) { fMuPrecFreq14 = value; } //!< sets Mu transition frequency (MHz)
    virtual void SetCaptureRate(Double_t value){ fCaptureRate = value; }  //!< sets Mu+ electron capture rate (MHz)
    virtual void SetIonizationRate(Double_t value){ fIonizationRate = value; } //!< sets Mu0 ionization rate (MHz)
    virtual void SetSpinFlipRate(Double_t value){ fSpinFlipRate = value; }     //!< sets Mu0 spin flip rate (MHz)
    virtual void SetDecayAsymmetry(Double_t value){ fAsymmetry = value; }      //!< muon decay asymmetry
    virtual void SetMuFraction(Double_t value){ fMuFraction = value; }         //!< Muonium fraction
    virtual void SetMuFractionState12(Double_t value){ fMuFractionState12 = value; }  
    virtual void SetMuFractionState23(Double_t value){ fMuFractionState23 = value; }  

    virtual Bool_t IsValid() { return fValid; }
    virtual void   SetSeed(UInt_t seed);

    virtual Double_t GetBfield()      { return fBfield; }            //!< returns the magnetic field (T)
    virtual Double_t GetCaptureRate() { return fCaptureRate; }       //!< returns Mu+ electron capture rate (MHz)
    virtual Double_t GetIonizationRate() { return fIonizationRate; } //!< returns Mu0 ionization rate (MHz)
    virtual void     Run(TH1F *histoForward, TH1F *histoBackward);

  private:
    Bool_t   fValid;
    TRandom2 *fRandom;

    Double_t  fBfield;          //!< magnetic field (T)
    Double_t  fMuPrecFreq12;    //!< Mu transition frequency 12 (MHz)
    Double_t  fMuPrecFreq34;    //!< Mu transition frequency 34 (MHz)
    Double_t  fMuPrecFreq23;    //!< Mu transition frequency 23 (MHz)
    Double_t  fMuPrecFreq14;    //!< Mu transition frequency 14 (MHz)
    Double_t  fMuonPrecFreq;    //!< muon precession frequency (MHz)
    Double_t  fCaptureRate;     //!< Mu+ electron capture rate (MHz)
    Double_t  fIonizationRate;  //!< Mu0 ionization rate (MHz)
    Double_t  fSpinFlipRate;    //!< Mu0 spin-flip rate (MHz)  
    Double_t  fInitialPhase;    //!< initial muon spin phase
    Double_t  fMuonDecayTime;   //!< muon decay time (us)
    Double_t  fMuonPhase;       //!< phase of muon spin
    Double_t  fAsymmetry;       //!< muon decay asymmetry
    Double_t  fMuFraction;      //!< total Mu fraction [0,1]
    Double_t  fMuFractionState12; //!< fraction of Mu in state 12, 34
    Double_t  fMuFractionState23; //!< fraction of Mu in state 23, 14
    Int_t     fNmuons;          //!< number of muons to simulate
    Bool_t    fDebugFlag;       //!< debug flag

    virtual Double_t NextEventTime(const Double_t &EventRate);
//     virtual Double_t PrecessionPhase(const Double_t &time, const TString chargeState);
    virtual TComplex GTFunction(const Double_t &time, const TString chargeState); //!< transverse field polarization function of Mu0 or Mu+
    virtual Double_t GTSpinFlip(const Double_t &time); //!< transverse field polarization function after spin-flip collisions
    virtual Double_t Event(const TString muonString);
 
  ClassDef(PSimulateMuTransition, 0)
};

#endif // _PSIMULATEMUTRANSITION_H_
