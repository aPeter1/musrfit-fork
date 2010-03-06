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

  // feed run info header
  UInt_t runNo = 9102;
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

  //prepare to run simulation
  simulateMuTransition->SetMuPrecFreq1(41.);   // MHz
  simulateMuTransition->SetMuPrecFreq2(-35.);  // MHz
  simulateMuTransition->SetMuFraction(0.5);    // initial Mu fraction
  simulateMuTransition->SetMuFractionState1(0.42);  // 100% of Mu in state 1
  simulateMuTransition->SetMuFractionState2(0.32);  // 0% of Mu in state 2
  simulateMuTransition->SetBfield(0.01);           // Tesla
  simulateMuTransition->SetCaptureRate(1.5);       // MHz
  simulateMuTransition->SetIonizationRate(250.);    // MHz
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
