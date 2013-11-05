/***************************************************************************

  analyticFakeData.C

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2012 by Andreas Suter                              *
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

void analyticFakeData(const TString type)
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

  histosFolder = gROOT->GetRootFolder()->AddFolder("histos", "Histograms");
  gROOT->GetListOfBrowsables()->Add(histosFolder, "histos");
  decayAnaModule = histosFolder->AddFolder("DecayAnaModule", "muSR decay histograms");

  UInt_t runNo = 9042;
  TString fileName, tstr, label, tstr1;
  fileName.Form("0%d.root", (Int_t)runNo);

//  Double_t t0[8] = {3419.0, 3520.0, 3520.0, 3421.0, 3517.0, 3418.0, 3522.0, 3623.0}; // runNo: 1000 & 2000
  Double_t t0[8] = {3519.0, 3420.0, 3520.0, 3621.0, 3417.0, 3518.0, 3422.0, 3423.0}; // runNo: 1001 & 2001

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
    header->SetTimeResolution(0.1953125);
    header->SetNChannels(66601);
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
    header->Set("RunInfo/Specific Validator URL", "http://lmu.web.psi.ch/facilities/software/MusrRoot/validation/MusrRootLEM.xsd");
    header->Set("RunInfo/Generator", "analyticFakeData");
    header->Set("RunInfo/File Name", fileName);
    tstr.Form("0%d - test", runNo);
    header->Set("RunInfo/Run Title", tstr);
    header->Set("RunInfo/Run Number", (Int_t)runNo);
    header->Set("RunInfo/Start Time", "1970-01-01 00:00:00");
    header->Set("RunInfo/Stop Time", "1970-01-01 00:00:00");
    prop.Set("Run Duration", 0, "sec");
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
    prop.Set("Sample Magnetic Field", 0.0, "G");
    header->Set("RunInfo/Sample Magnetic Field", prop);
    prop.Set("Sample Temperature", 1.0, "mK");
    header->Set("RunInfo/No of Histos", 8);
    prop.Set("Time Resolution", 0.1953125, "ns");
    header->Set("RunInfo/Time Resolution", prop);
    vector<Int_t> ivec;
    ivec.push_back(0);
    ivec.push_back(20);
    header->Set("RunInfo/RedGreen Offsets", ivec);

    offset = 1;
    // 2nd write all the required DetectorInfo entries
    for (UInt_t i=0; i<8; i++) {
      tstr.Form("DetectorInfo/Detector%03d/", i+offset);
      label = tstr + TString("Name");
      tstr1.Form("Detector%03d", (Int_t)(i+offset));
      header->Set(label, tstr1);
      label = tstr + TString("Histo No");
      header->Set(label, (Int_t)(i+offset));
      label = tstr + TString("Histo Length");
      header->Set(label, 66601);
      label = tstr + TString("Time Zero Bin");
      header->Set(label, t0[i]);
      label = tstr + TString("First Good Bin");
      header->Set(label, (Int_t)t0[i]);
      label = tstr + TString("Last Good Bin");
      header->Set(label, 66600);
    }
    for (UInt_t i=0; i<8; i++) {
      tstr.Form("DetectorInfo/Detector%03d/", 20+i+offset);
      label = tstr + TString("Name");
      tstr1.Form("Detector%03d", (Int_t)(20+i+offset));
      header->Set(label, tstr1);
      label = tstr + TString("Histo No");
      header->Set(label, (Int_t)(20+i+offset));
      label = tstr + TString("Histo Length");
      header->Set(label, 66601);
      label = tstr + TString("Time Zero Bin");
      header->Set(label, t0[i]);
      label = tstr + TString("First Good Bin");
      header->Set(label, (Int_t)t0[i]);
      label = tstr + TString("Last Good Bin");
      header->Set(label, 66600);
    }

    // 3rd write required SampleEnvironmentInfo entries
    header->Set("SampleEnvironmentInfo/Cryo", "virtual");

    // 4th write required MagneticFieldEnvironmentInfo entries
    header->Set("MagneticFieldEnvironmentInfo/Magnet Name", "virtual");

    // 5th write required BeamlineInfo entries
    header->Set("BeamlineInfo/Name", "muE4");
  }

  // create histos
  Double_t n0[8] = {200.0, 200.0, 200.0, 200.0, 200.0, 200.0, 200.0, 200.0};
  Double_t bkg[8] = {1.3, 1.5, 1.0, 1.3, 1.2, 1.1, 1.0, 1.4};
  const Double_t tau = 2197.019; // ns

  // asymmetry related stuff
  const Double_t timeResolution = 0.1953125; // ns
//  Double_t a0[8] = {0.12, 0.12, 0.12, 0.12, 0.12, 0.12, 0.12, 0.12};
  Double_t a0[8] = {0.21, 0.22, 0.22, 0.24, 0.22, 0.21, 0.22, 0.23};
//  Double_t a1[8] = {0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05};
  Double_t a1[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  Double_t phase[8] = {5.0*TMath::Pi()/180.0, 50.0*TMath::Pi()/180.0, 95.0*TMath::Pi()/180.0, 140.0*TMath::Pi()/180.0,
                       185.0*TMath::Pi()/180.0, 230.0*TMath::Pi()/180.0, 275.0*TMath::Pi()/180.0, 320.0*TMath::Pi()/180.0,};
  const Double_t gamma = 0.0000135538817; // gamma/(2pi)
  Double_t bb0 = 200.0; // field in Gauss
  Double_t bb1 = 400.0; // field in Gauss
  Double_t rate0 = 0.10/1000.0; // in 1/ns
  Double_t rate1 = 0.15/1000.0; // in 1/ns

  // fake function parameters header info: only for test purposes
  if (type.CompareTo("TLemRunHeader")) {
    TDoubleVector dvec;
    header->Set("FakeFct/Def", "N0 exp(-t/tau_mu) [1 + A exp(-t lambda) cos(gamma_mu B t + phi)] + bkg");
    for (UInt_t i=0; i<8; i++)
      dvec.push_back(n0[i]);
    header->Set("FakeFct/N0", dvec);
    dvec.clear();
    for (UInt_t i=0; i<8; i++)
      dvec.push_back(bkg[i]);
    header->Set("FakeFct/bkg", dvec);
    dvec.clear();
    for (UInt_t i=0; i<8; i++)
      dvec.push_back(a0[i]);
    header->Set("FakeFct/A", dvec);
    dvec.clear();
    for (UInt_t i=0; i<8; i++)
      dvec.push_back(phase[i]);
    header->Set("FakeFct/phase", dvec);
    prop.Set("B", bb0, "G");
    header->Set("FakeFct/B", prop);
    prop.Set("lambda", rate0, "1/usec");
    header->Set("FakeFct/lambda", prop);
  }

  TH1F *histo[16];
  char str[128];
  for (UInt_t i=0; i<8; i++) {
    if (!type.CompareTo("TLemRunHeader"))
      sprintf(str, "hDecay%02d", (Int_t)(i+offset));
    else
      sprintf(str, "hDecay%03d", (Int_t)(i+offset));
    histo[i] = new TH1F(str, str, 66601, -0.5, 66600.5);
    if (!type.CompareTo("TLemRunHeader"))
      sprintf(str, "hDecay%02d", (Int_t)(20+i+offset));
    else
      sprintf(str, "hDecay%03d", (Int_t)(20+i+offset));
    histo[i+8] = new TH1F(str, str, 66601, -0.5, 66600.5);
  }
  Double_t time;
  Double_t dval, dval1;
  for (UInt_t i=0; i<8; i++) {
    for (UInt_t j=0; j<66600; j++) {
      if (j<t0[i]) {
        histo[i]->SetBinContent(j+1, bkg[i]);
      } else {
        time = (Double_t)(j-t0[i])*timeResolution;
        dval = (Double_t)n0[i]*TMath::Exp(-time/tau)*(1.0+a0[i]*TMath::Exp(-time*rate0)*TMath::Cos(TMath::TwoPi()*gamma*bb0*time+phase[i]))+(Double_t)bkg[i];
        histo[i]->SetBinContent(j+1, (UInt_t)dval);
      }
    }
  }

  // add a promp peak
  Double_t ampl = 0.0;
  Double_t promptLambda = 500.0/1000.0; // Lorentzain in 1/ns
  for (UInt_t i=0; i<8; i++) {
    for (UInt_t j=1; j<66601; j++) {
      dval = histo[i]->GetBinContent(j);
      dval1 = dval*0.9; // simulate a PPC
      time = (Double_t)(j-t0[i])*timeResolution;
      dval += ampl*TMath::Exp(-fabs(time)*promptLambda);
      dval1 += ampl*TMath::Exp(-fabs(time)*promptLambda);
      histo[i]->SetBinContent(j, (UInt_t)dval);
      histo[i+8]->SetBinContent(j, (UInt_t)dval1);
    }
  }

  // add Poisson noise
  PAddPoissonNoise *addNoise = new PAddPoissonNoise();
  if (!addNoise->IsValid()) {
    cerr << endl << "**ERROR** while invoking PAddPoissonNoise" << endl;
    return;
  }
  for (UInt_t i=0; i<16; i++) {
    addNoise->AddNoise(histo[i]);
  }
/*
  for (UInt_t i=0; i<8; i++) {
    for (UInt_t j=1; j<histo[i]->GetEntries(); j++) {
      histo[i+8]->SetBinContent(j, histo[i]->GetBinContent(j));
    }
  }
*/
  for (UInt_t i=0; i<16; i++)
    decayAnaModule->Add(histo[i]);

  // write file
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

  delete [] histo;
}
