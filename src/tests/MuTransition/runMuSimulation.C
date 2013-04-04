/***************************************************************************

  runMuSimulation.C

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

void runMuSimulation()
{
  // load library
  gSystem->Load("$ROOTSYS/lib/libPSimulateMuTransition");
 
  // generate data
  TFolder *histosFolder;
  TFolder *decayAnaModule;
  TFolder *runInfo;

  histosFolder = gROOT->GetRootFolder()->AddFolder("histos", "Histograms");
  gROOT->GetListOfBrowsables()->Add(histosFolder, "histos");
  decayAnaModule = histosFolder->AddFolder("DecayAnaModule", "muSR decay histograms");

  //prepare to run simulation; here: isotropic Mu in Germanium
  UInt_t   runNo   = 9859;
  Double_t T       = 290.; //temperature
  Double_t capRate = 300.0;//*sqrt(T/200.); //assume that capture rate varies as sqrt(T), capRate = sigma*v*p , v ~ sqrt(T)  
  Double_t ionRate;  //assume Arrhenius behaviour ionRate = preFac*exp(-EA/kT)
  Double_t EA;       //activation energy (meV) 
  EA = 170.;
  ionRate = 2.9e7 * exp(-EA/(0.08625*T)); // Ge: 2.9*10^7MHz "attempt" frequency; 1K = 0.08625 meV 

  // feed run info header
  TString tstr;
  runInfo = gROOT->GetRootFolder()->AddFolder("RunInfo", "LEM RunInfo");  
  gROOT->GetListOfBrowsables()->Add(runInfo, "RunInfo");
  header = new TLemRunHeader();
  tstr  = TString("0");
  tstr += runNo;
  tstr += TString(" - Ge, Mu-frac 1.0, Mu12 737MHz (0.44), Mu34 -1622MHz(0.44), T=290K/EA=170meV, Cap. 300.0MHz, 100mT");

  header->SetRunTitle(tstr.Data());
  header->SetLemSetup("trivial");
  header->SetRunNumber(runNo);
  header->SetStartTime(0);
  header->SetStopTime(1);
  header->SetModeratorHV(32.0, 0.01);
  header->SetSampleHV(0.0, 0.01);
  header->SetImpEnergy(31.8);
  header->SetSampleTemperature(T, 0.001);
  header->SetSampleBField(1000.0, 0.1);
  header->SetTimeResolution(1.);
  header->SetNChannels(12001);
  header->SetNHist(2);
  header->SetOffsetPPCHistograms(20);
  header->SetCuts("none");
  header->SetModerator("none");
  Double_t tt0[2] = {0., 0.};
  header->SetTimeZero(tt0);
  runInfo->Add(header); //add header to RunInfo folder

  TH1F *histo[4];
  char str[128];
  for (UInt_t i=0; i<2; i++) {
    sprintf(str, "hDecay0%d", (Int_t)i);
    histo[i] = new TH1F(str, str, 12001, -0.5, 12000.5);
    sprintf(str, "hDecay2%d", (Int_t)i);
    histo[i+2] = new TH1F(str, str, 12001, -0.5, 12000.5);
  }

  PSimulateMuTransition *simulateMuTransition = new PSimulateMuTransition();
  if (!simulateMuTransition->IsValid()) {
    cerr << endl << "**ERROR** while invoking PSimulateTransition" << endl;
    return;
  }

  simulateMuTransition->SetMuPrecFreq12(737.3);   // MHz
  simulateMuTransition->SetMuPrecFreq34(-1622.2);  // MHz
  simulateMuTransition->SetMuPrecFreq23(2051.6);   // MHz
  simulateMuTransition->SetMuPrecFreq14(4111.2);  // MHz
  simulateMuTransition->SetMuFraction(1.0);    // initial Mu fraction
  simulateMuTransition->SetMuFractionState1(0.88);  // Mu in states 12, 34
  simulateMuTransition->SetMuFractionState2(0.12);  // Mu in states 23, 14
  simulateMuTransition->SetBfield(0.1);           // Tesla
  simulateMuTransition->SetCaptureRate(capRate);       // MHz
  simulateMuTransition->SetIonizationRate(ionRate);    // MHz
  simulateMuTransition->SetNmuons(1e7);
  simulateMuTransition->SetDecayAsymmetry(0.27);
  simulateMuTransition->SetDebugFlag(kFALSE); // to print time and phase during charge-changing cycle

  simulateMuTransition->PrintSettings();

  simulateMuTransition->Run(histo[0], histo[1]);

  for (UInt_t i=0; i<4; i++)
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
  cout << "Histograms written to " << tstr.Data() << endl;
  delete fout;

  delete [] histo;
}
