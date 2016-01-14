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
  UInt_t   runNo    = 9903;
  Double_t T        = 300.; //temperature
  Double_t capRate  = 1.0;//*sqrt(T/200.); 
  //assume that capture rate varies as sqrt(T), capRate = sigma*v*p , v ~ sqrt(T)  
  Double_t ionRate;  //assume Arrhenius behaviour ionRate = preFac*exp(-EA/kT)
  Double_t EA       = 100.; //activation energy (meV) 
  ionRate           = 2.9e7 * exp(-EA/(0.08625*T)); // Ge: 2.9*10^7MHz "attempt" frequency; 1K = 0.08625 meV 
  Double_t B        = 100.; //field in G
  Double_t Freq12   = 4463; //Mu freq of the 12 transition
  Double_t Freq34   = 4463; //Mu freq of the 34 transition
  Double_t Freq23   = 4463; //Mu freq of the 23 transition
  Double_t Freq14   = 4463; //Mu freq of the 14 transition
  Double_t MuFrac   = 1.0;  //total Mu fraction
  Double_t MuFrac12 = 0.5;  //Mu in states 12 and 34
  Double_t MuFrac23 = 0.5;  //Mu in states 23 and 14
  Int_t    Nmuons   = 1e7;  //number of muons
  Double_t Asym     = 0.27; //muon decay asymmetry
  
  // feed run info header
  TString tstr;
  runInfo = gROOT->GetRootFolder()->AddFolder("RunInfo", "LEM RunInfo");  
  gROOT->GetListOfBrowsables()->Add(runInfo, "RunInfo");
  header = new TLemRunHeader();
  tstr  = TString("0");
  tstr += runNo;
  tstr += TString(" - Mu-frac 1.0, Mu12 -4463MHz (0.5), Mu34 -4463MHz(0.5), T=300K/EA=100meV, Cap. 1.0MHz, 10mT");

  header->SetRunTitle(tstr.Data());
  header->SetLemSetup("trivial");
  header->SetRunNumber(runNo);
  header->SetStartTime(0);
  header->SetStopTime(1);
  header->SetModeratorHV(32.0, 0.01);
  header->SetSampleHV(0.0, 0.01);
  header->SetImpEnergy(31.8);
  header->SetSampleTemperature(T, 0.001);
  header->SetSampleBField(B, 0.1);
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

  simulateMuTransition->SetMuPrecFreq12(Freq12);       // MHz
  simulateMuTransition->SetMuPrecFreq34(Freq34);       // MHz
  simulateMuTransition->SetMuPrecFreq23(Freq23);       // MHz
  simulateMuTransition->SetMuPrecFreq14(Freq14);       // MHz
  simulateMuTransition->SetMuFraction(MuFrac);         // initial Mu fraction
  simulateMuTransition->SetMuFractionState12(MuFrac12); // Mu in states 12, 34
  simulateMuTransition->SetMuFractionState23(MuFrac23); // Mu in states 23, 14
  simulateMuTransition->SetBfield(B/10000.);           // Tesla
  simulateMuTransition->SetCaptureRate(capRate);       // MHz
  simulateMuTransition->SetIonizationRate(ionRate);    // MHz
  simulateMuTransition->SetNmuons(Nmuons);
  simulateMuTransition->SetDecayAsymmetry(Asym);
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
