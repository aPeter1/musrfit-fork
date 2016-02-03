/***************************************************************************

  analyticFakeData.C

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2015 by Andreas Suter                              *
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
#include <vector>
using namespace std;

void analyticFakeData(const TString type, UInt_t runNo)
{
  // load library
  gSystem->Load("$ROOTSYS/lib/libTLemRunHeader");
  gSystem->Load("$ROOTSYS/lib/libTMusrRunHeader");
  gSystem->Load("$ROOTSYS/lib/libPAddPoissonNoise");

  // generate data
  TFolder *histosFolder;
  TFolder *decayAnaModule;
  TFolder *runHeader;
  TFolder *runInfo;
  UInt_t offset = 0;
  const UInt_t noOfHistos = 4;
  const UInt_t noOfChannels = 426600;
  const Double_t timeResolution = 0.025; // ns
  TRandom3 rand;

  histosFolder = gROOT->GetRootFolder()->AddFolder("histos", "Histograms");
  gROOT->GetListOfBrowsables()->Add(histosFolder, "histos");
  decayAnaModule = histosFolder->AddFolder("DecayAnaModule", "muSR decay histograms");

  TString fileName, tstr, label, tstr1;

  fileName.Form("0%d.root", (Int_t)runNo);

  cout << ">> define t0" << endl;
  vector<Double_t> t0;
  for (UInt_t i=0; i<noOfHistos; i++)
    t0.push_back(5000.0);

  cout << ">> define header" << endl;
  if (!type.CompareTo("TLemRunHeader")) {
    // feed run info header
    runInfo = gROOT->GetRootFolder()->AddFolder("RunInfo", "LEM RunInfo");
    gROOT->GetListOfBrowsables()->Add(runInfo, "RunInfo");
    TLemRunHeader *header = new TLemRunHeader();
    tstr.Form("0%d - test", runNo);
    header->SetRunTitle(tstr);
    header->SetLemSetup("trivial");
    header->SetRunNumber(runNo);
    header->SetStartTime(0);
    header->SetStopTime(1);
    header->SetModeratorHV(32.0, 0.01);
    header->SetSampleHV(0.0, 0.01);
    header->SetImpEnergy(31.8);
    header->SetSampleTemperature(0.2, 0.001);
    header->SetSampleBField(-1.0, 0.1);
    header->SetTimeResolution(timeResolution);
    header->SetNChannels(noOfChannels);
    header->SetNHist(8);
    header->SetCuts("none");
    header->SetModerator("none");
    header->SetTimeZero(t0);
    runInfo->Add(header); //add header to RunInfo folder
  } else { // TMusrRunHeader
    runHeader = gROOT->GetRootFolder()->AddFolder("RunHeader", "MusrRoot RunHeader");
    gROOT->GetListOfBrowsables()->Add(runHeader, "RunHeader");
    TMusrRunHeader *header = new TMusrRunHeader(true);
    TMusrRunPhysicalQuantity prop;

    // 1st write all the required RunInfo entries

    header->Set("RunInfo/Generic Validator URL", "http://lmu.web.psi.ch/facilities/software/MusrRoot/validation/MusrRoot.xsd");
    header->Set("RunInfo/Specific Validator URL", "http://lmu.web.psi.ch/facilities/software/MusrRoot/validation/MusrRootHAL9500.xsd");
    header->Set("RunInfo/Generator", "analyticFakeData");
    header->Set("RunInfo/File Name", fileName);
    tstr.Form("0%d - test", runNo);
    header->Set("RunInfo/Run Title", tstr);
    header->Set("RunInfo/Run Number", (Int_t)runNo);
    header->Set("RunInfo/Start Time", "1970-01-01 00:00:00");
    header->Set("RunInfo/Stop Time", "1970-01-01 00:00:02");
    prop.Set("Run Duration", 2, "sec");
    header->Set("RunInfo/Run Duration", prop);
    header->Set("RunInfo/Laboratory", "PSI");
    header->Set("RunInfo/Instrument", "virtual");
    prop.Set("Muon Beam Momentum", 28.1, "MeV/c");
    header->Set("RunInfo/Muon Beam Momentum", prop);
    header->Set("RunInfo/Muon Species", "positive muon");
    header->Set("RunInfo/Muon Source", "computer");
    header->Set("RunInfo/Setup", "virtual");
    header->Set("RunInfo/Comment", "fake data runs");
    header->Set("RunInfo/Sample Name", "virtual");
    prop.Set("Sample Temperature", 1.0, "mK");
    header->Set("RunInfo/Sample Temperature", prop);
    prop.Set("Sample Magnetic Field", 40.0, "T");
    header->Set("RunInfo/Sample Magnetic Field", prop);
    prop.Set("Sample Temperature", 1.0, "mK");
    header->Set("RunInfo/No of Histos", (Int_t)noOfHistos);
    prop.Set("Time Resolution", timeResolution, "ns");
    header->Set("RunInfo/Time Resolution", prop);
    vector<Int_t> ivec;
    ivec.push_back(0);
    header->Set("RunInfo/RedGreen Offsets", ivec);

    offset = 1;
    // 2nd write all the required DetectorInfo entries
    for (UInt_t i=0; i<4; i++) {
      tstr.Form("DetectorInfo/Detector%03d/", i+offset);
      label = tstr + TString("Name");
      tstr1.Form("Detector%03d", (Int_t)(i+offset));
      header->Set(label, tstr1);
      label = tstr + TString("Histo No");
      header->Set(label, (Int_t)(i+offset));
      label = tstr + TString("Histo Length");
      header->Set(label, (Int_t)noOfChannels);
      label = tstr + TString("Time Zero Bin");
      header->Set(label, t0[i]);
      label = tstr + TString("First Good Bin");
      header->Set(label, (Int_t)t0[i]);
      label = tstr + TString("Last Good Bin");
      header->Set(label, (Int_t)noOfChannels);
    }
    // 3rd write required SampleEnvironmentInfo entries
    header->Set("SampleEnvironmentInfo/Cryo", "virtual");

    // 4th write required MagneticFieldEnvironmentInfo entries
    header->Set("MagneticFieldEnvironmentInfo/Magnet Name", "virtual");

    // 5th write required BeamlineInfo entries
    header->Set("BeamlineInfo/Name", "piE3");
  }

  // create histos
  cout << ">> define n0" << endl;
  vector<Double_t> n0;
  for (UInt_t i=0; i<noOfHistos; i++)
    n0.push_back(25.0 + 2.0*rand.Rndm());

  cout << ">> define bkg" << endl;
  vector<Double_t> bkg;
  for (UInt_t i=0; i<noOfHistos; i++)
    bkg.push_back(0.05 + 0.02*rand.Rndm());

  const Double_t tau = 2197.019; // ns

  // asymmetry related stuff
  cout << ">> define a0" << endl;
  vector<Double_t> a0;
  for (UInt_t i=0; i<noOfHistos; i++)
    a0.push_back(0.25 + 0.01*rand.Rndm());

  cout << ">> define phase" << endl;
  vector<Double_t> phase;
  for (UInt_t i=0; i<noOfHistos; i++)
    phase.push_back((5.0 + 2.0*rand.Rndm())*TMath::Pi()/180.0 + TMath::TwoPi()/noOfHistos * (Double_t)i);

  const Double_t gamma = 0.0000135538817; // gamma/(2pi)
  Double_t bb0 = 5000.0; // field in Gauss
  Double_t rate0 = 7.0/1000.0; // in 1/ns
  Double_t frac0 = 0.5;
  Double_t bb1 = bb0 + 200.0; // field in Gauss
  Double_t rate1 = 0.75/1000.0; // in 1/ns
  Double_t frac1 = 0.2;
  Double_t bb2 = bb0 + 600.0; // field in Gauss
  Double_t rate2 = 0.25/1000.0; // in 1/ns

  // fake function parameters header info: only for test purposes
  cout << ">> write fake header for TMusrRoot" << endl;
  if (type.CompareTo("TLemRunHeader")) {
    TDoubleVector dvec;
    header->Set("FakeFct/Def", "N0 exp(-t/tau_mu) [1 + sum_{k=0}^2 frac_k A_0 exp(-1/2 (t sigma_k)^2) cos(gamma_mu B_k t + phi)] + bkg");
    for (UInt_t i=0; i<noOfHistos; i++)
      dvec.push_back(n0[i]);
    header->Set("FakeFct/N0", dvec);
    dvec.clear();
    for (UInt_t i=0; i<noOfHistos; i++)
      dvec.push_back(bkg[i]);
    header->Set("FakeFct/bkg", dvec);
    dvec.clear();
    for (UInt_t i=0; i<noOfHistos; i++)
      dvec.push_back(a0[i]);
    header->Set("FakeFct/A", dvec);
    dvec.clear();
    for (UInt_t i=0; i<noOfHistos; i++)
      dvec.push_back(phase[i]);
    header->Set("FakeFct/phase", dvec);
    prop.Set("B0", bb0, "G");
    header->Set("FakeFct/B0", prop);
    prop.Set("rate0", 1.0e3*rate0, "1/usec");
    header->Set("FakeFct/rate0", prop);
    header->Set("FakeFct/frac0", frac0);
    prop.Set("B1", bb1, "G");
    header->Set("FakeFct/B1", prop);
    prop.Set("rate1", 1.0e3*rate1, "1/usec");
    header->Set("FakeFct/rate1", prop);
    header->Set("FakeFct/frac1", frac1);
    prop.Set("B2", bb2, "G");
    header->Set("FakeFct/B2", prop);
    prop.Set("rate2", 1.0e3*rate2, "1/usec");
    header->Set("FakeFct/rate2", prop);
  }

  cout << ">> create histo objects" << endl;
  vector<TH1F*> histo;
  histo.resize(noOfHistos);
  char str[128];
  for (UInt_t i=0; i<noOfHistos; i++) {
    if (!type.CompareTo("TLemRunHeader"))
      sprintf(str, "hDecay%02d", (Int_t)(i+offset));
    else
      sprintf(str, "hDecay%03d", (Int_t)(i+offset));
    histo[i] = new TH1F(str, str, noOfChannels+1, -0.5, noOfChannels+0.5);
  }
  Double_t time;
  Double_t dval;
  cout << ">> fill histos" << endl;
  for (UInt_t i=0; i<noOfHistos; i++) {
    for (UInt_t j=0; j<noOfChannels; j++) {
      if (j<t0[i]) {
        histo[i]->SetBinContent(j+1, bkg[i]);
      } else {
        time = (Double_t)(j-t0[i])*timeResolution;
        dval = (Double_t)n0[i]*TMath::Exp(-time/tau)*(1.0+
                   frac0*a0[i]*TMath::Exp(-0.5*TMath::Power(time*rate0,2))*TMath::Cos(TMath::TwoPi()*gamma*bb0*time+phase[i]) +
                   frac1*a0[i]*TMath::Exp(-0.5*TMath::Power(time*rate1,2))*TMath::Cos(TMath::TwoPi()*gamma*bb1*time+phase[i]) +
                   (1.0-frac0-frac1)*a0[i]*TMath::Exp(-0.5*TMath::Power(time*rate2,2))*TMath::Cos(TMath::TwoPi()*gamma*bb2*time+phase[i]))+(Double_t)bkg[i];
        histo[i]->SetBinContent(j+1, dval);
      }
    }
  }

  cout << ">> add prompt peak" << endl;
  // add a promp peak
  Double_t ampl = 50.0;
  Double_t promptLambda = 500.0/1000.0; // Lorentzain in 1/ns
  if (ampl != 0.0) {
    for (UInt_t i=0; i<noOfHistos; i++) {
      for (UInt_t j=1; j<noOfChannels+1; j++) {
        dval = histo[i]->GetBinContent(j);
        time = (Double_t)(j-t0[i])*timeResolution;
        dval += ampl*TMath::Exp(-fabs(time)*promptLambda);
        histo[i]->SetBinContent(j, dval);
      }
    }
  }

  // add Poisson noise
  cout << ">> add Poisson noise" << endl;
  PAddPoissonNoise *addNoise = new PAddPoissonNoise();
  if (!addNoise->IsValid()) {
    cerr << endl << "**ERROR** while invoking PAddPoissonNoise" << endl;
    return;
  }
  for (UInt_t i=0; i<noOfHistos; i++) {
    addNoise->AddNoise(histo[i]);
  }
  delete addNoise;
  addNoise = 0;

  cout << ">> add histos to decayAnaModule" << endl;
  for (UInt_t i=0; i<noOfHistos; i++)
    decayAnaModule->Add(histo[i]);

  // write file
  cout << ">> write file" << endl;
  TFile *fout = new TFile(fileName.Data(), "RECREATE", "Midas Fake Histograms");
  if (fout == 0) {
    cout << endl << "**ERROR** Couldn't create ROOT file";
    cout << endl << endl;
    exit(0);
  }

  fout->cd();
  if (!type.CompareTo("TLemRunHeader")) {
    runInfo->Write();
  } else {
    if (header->FillFolder(runHeader))
      runHeader->Write();
  }

  histosFolder->Write();
  fout->Close();
  delete fout;

/*
  cout << ">> cleanup" << endl;
  for (UInt_t i=0; i<noOfHistos; i+1)
    delete histo[i];
*/
  cout << ">> done!" << endl;
}
