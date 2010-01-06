/***************************************************************************

  t0NotEqFirstGoodData.C

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Andreas Suter                                   *
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

void t0NotEqFirstGoodData()
{
  // load library
  gSystem->Load("$ROOTSYS/lib/libPAddPoissonNoise");

  // generate data
  TFolder *histosFolder;
  TFolder *decayAnaModule;
  TFolder *runInfo;

  histosFolder = gROOT->GetRootFolder()->AddFolder("histos", "Histograms");
  gROOT->GetListOfBrowsables()->Add(histosFolder, "histos");
  decayAnaModule = histosFolder->AddFolder("DecayAnaModule", "muSR decay histograms");

  // feed run info header
  UInt_t runNo = 10113;
  TString tstr;
  runInfo = gROOT->GetRootFolder()->AddFolder("RunInfo", "LEM RunInfo");  
  gROOT->GetListOfBrowsables()->Add(runInfo, "RunInfo");
  header = new TLemRunHeader();
  tstr  = TString("0");
  tstr += runNo;
  tstr += TString(" - test");
  header->SetRunTitle(tstr.Data());
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
  header->SetNHist(4);
  header->SetCuts("none");
  header->SetModerator("none");
  Double_t tt0[4] = {3419.0, 3419.0, 3419.0, 3419.0};
  header->SetTimeZero(tt0);
  runInfo->Add(header); //add header to RunInfo folder

  // create histos
  UInt_t t0[4] = {3419, 3419, 3419, 3419};
  UInt_t n0[4] = {200.0, 205.0, 203.0, 198.0};
  UInt_t bkg[4] = {11.0, 11.0, 5.0, 8.0};
  const Double_t tau = 2197.019; // ns
  // asymmetry related stuff
  const Double_t timeResolution = 0.1953125; // ns
  Double_t a0[4] = {0.26, 0.26, 0.26, 0.26};
  Double_t a1[4] = {0.0, 0.0, 0.0, 0.0};
  Double_t phase[4] = {0.0*TMath::Pi()/180.0, 90.0*TMath::Pi()/180.0, 180.0*TMath::Pi()/180.0, 270.0*TMath::Pi()/180.0};
  const Double_t gamma = 0.00001355; // gamma/(2pi)
  Double_t bb0 = 15000.0; // field in Gauss
  Double_t bb1 = 400.0; // field in Gauss
  Double_t sigma0 = 0.05/1000.0; // Gaussian sigma in 1/ns
  Double_t sigma1 = 0.005/1000.0; // Gaussian sigma in 1/ns

  TH1F *histo[8];
  char str[128];
  for (UInt_t i=0; i<4; i++) {
    sprintf(str, "hDecay0%d", (Int_t)i);
    histo[i] = new TH1F(str, str, 66601, -0.5, 66600.5);
    sprintf(str, "hDecay2%d", (Int_t)i);
    histo[i+4] = new TH1F(str, str, 66601, -0.5, 66600.5);
  }
  Double_t time;
  Double_t dval;
  for (UInt_t i=0; i<4; i++) {
    for (UInt_t j=1; j<66601; j++) {
      if (j<t0[i]) {
        histo[i]->SetBinContent(j, bkg[i]);
      } else {
        time = (Double_t)(j-t0[i])*timeResolution;
        dval = (Double_t)n0[i]*TMath::Exp(-time/tau)*(1.0+a0[i]*TMath::Exp(-0.5*TMath::Power(time*sigma0,1.2))*TMath::Cos(TMath::TwoPi()*gamma*bb0*time+phase[i])
                                                         +a1[i]*TMath::Exp(-0.5*TMath::Power(time*sigma1,2.0))*TMath::Cos(TMath::TwoPi()*gamma*bb1*time+phase[i]))+(Double_t)bkg[i];
        histo[i]->SetBinContent(j, (UInt_t)dval);
      }
    }
  }

  // add a promp peak
/*
  Double_t ampl = 500.0;
  Double_t promptLambda = 500.0/1000.0; // Lorentzain in 1/ns
  for (UInt_t i=0; i<4; i++) {
    for (UInt_t j=1; j<66601; j++) {
      dval = histo[i]->GetBinContent(j);
      time = (Double_t)(j-t0[i])*timeResolution;
      dval += ampl*TMath::Exp(-fabs(time)*promptLambda);
      histo[i]->SetBinContent(j, (UInt_t)dval);
      histo[i+4]->SetBinContent(j, (UInt_t)dval);
    }
  }
*/

  // add Poisson noise
  PAddPoissonNoise *addNoise = new PAddPoissonNoise();
  if (!addNoise->IsValid()) {
    cerr << endl << "**ERROR** while invoking PAddPoissonNoise" << endl;
    return;
  }
  for (UInt_t i=0; i<4; i++) {
    addNoise->AddNoise(histo[i]);
  }
  for (UInt_t i=0; i<4; i++) {
    for (UInt_t j=1; j<histo[i]->GetEntries(); j++) {
      histo[i+4]->SetBinContent(j, histo[i]->GetBinContent(j));
    }
  }

  for (UInt_t i=0; i<8; i++)
    decayAnaModule->Add(histo[i]);

  // write file
  tstr  = TString("0");
  tstr += runNo;
  tstr += TString(".root");
  TFile *fout = new TFile(tstr.Data(), "RECREATE", "Midas Fake Histograms");
  if (fout == 0) {
    cout << endl << "**ERROR** Couldn't create ROOT file";
    cout << endl << endl;
    exit(0);
  }

  fout->cd();
  runInfo->Write();
  histosFolder->Write();
  fout->Close();
  delete fout;

  delete [] histo;
}
