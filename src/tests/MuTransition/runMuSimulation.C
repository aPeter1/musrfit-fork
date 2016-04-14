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

#include "/apps/cern/root-git/include/TMusrRunHeader.h"
#define NDECAYHISTS 2

void runMuSimulation()
{
  // load library
  gSystem->Load("$ROOTSYS/lib/libPSimulateMuTransition");
  
  // load TMusrRunHeader class if not already done during root startup
  if (!TClass::GetDict("TMusrRunHeader")) {
    gROOT->LoadMacro("$(ROOTSYS)/lib/libTMusrRunHeader.so");
  }
 
  char    titleStr[256];
  TFolder *histosFolder;
  TFolder *decayAnaModule, *scAnaModule;
  TFolder *gRunHeader;
  TString runTitle;
  TString histogramFileName;
  TObjArray Slist(0);
  TMusrRunPhysicalQuantity prop;
  
  //prepare to run simulation; here: isotropic Mu with A0 = 100.0 MHz
  UInt_t   runNo        = 7701;
  Double_t T            = 300.;   //temperature
  Double_t EA           = 100; //activation energy (meV)  
  Double_t spinFlipRate = 0.01; //if spinFlipRate > 0.001 only spin-flip processes will be simulated
  Double_t capRate      = 0.0001;//*sqrt(T/200.); //assume that capture rate varies as sqrt(T), capRate = sigma*v*p , v ~ sqrt(T)  
  Double_t ionRate;  //assume Arrhenius behaviour ionRate = preFac*exp(-EA/kT)
  ionRate           = 0.0001; //2.9e7 * exp(-EA/(0.08625*T)); // Ge: 2.9*10^7MHz "attempt" frequency; 1K = 0.08625 meV 
  Double_t B        = 106.5; //field in G
  Double_t Bvar     = 0.;   //field variance
  Double_t Freq12   = 40.433;  //Mu freq of the 12 transition
  Double_t Freq34   = 59.567; //Mu freq of the 34 transition
  Double_t Freq23   = 256.245;  //Mu freq of the 23 transition
  Double_t Freq14   = 356.245; //Mu freq of the 14 transition
  Double_t MuFrac   = 1.0;  //total Mu fraction
  Double_t MuFrac12 = 2*0.487;  //Mu in states 12 and 34
  Double_t MuFrac23 = 2*0.013;  //Mu in states 23 and 14
  Int_t    Nmuons   = 5e6;  //number of muons
  Double_t Asym     = 0.27; //muon decay asymmetry
  Int_t    debugFlag = 0; //print debug information on screen
  
  histogramFileName  = TString("0");
  histogramFileName += runNo;
  histogramFileName += TString(".root");

  sprintf(titleStr,"- complexMuPol, A0 100MHz, Mu-frac %3.2f, Mu12 %6.2f MHz(%3.2f), Mu23 %6.2f MHz(%3.2f), ionRate %8.3f MHz, capRate %6.3f MHz, SF rate %6.3f MHz, %5.1f G", MuFrac, Freq12, MuFrac12/2, Freq23, MuFrac23/2, ionRate, capRate, spinFlipRate, B);
  runTitle  = TString("0");
  runTitle += runNo;
  runTitle += TString(titleStr);

  PSimulateMuTransition *simulateMuTransition = new PSimulateMuTransition();
  if (!simulateMuTransition->IsValid()){
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
  simulateMuTransition->SetSpinFlipRate(spinFlipRate); // MHz
  simulateMuTransition->SetNmuons(Nmuons);
  simulateMuTransition->SetDecayAsymmetry(Asym);
  simulateMuTransition->SetDebugFlag(debugFlag); // to print time and phase during charge-changing cycle
   
  // feed run info header
  gRunHeader = gROOT->GetRootFolder()->AddFolder("RunHeader", "MuTransition Simulation Header Info");  
  gROOT->GetListOfBrowsables()->Add(gRunHeader, "RunHeader");
//   header = new TLemRunHeader();
  TMusrRunHeader *header = new TMusrRunHeader(true);
 
  header->Set("RunInfo/Generic Validator URL", "http://lmu.web.psi.ch/facilities/software/MusrRoot/validation/MusrRoot.xsd");
  header->Set("RunInfo/Specific Validator URL", "http://lmu.web.psi.ch/facilities/software/MusrRoot/validation/MusrRootLEM.xsd");
  header->Set("RunInfo/Generator", "runMuSimulation");
  
  header->Set("RunInfo/File Name", histogramFileName.Data());
  header->Set("RunInfo/Run Title", runTitle.Data());
  header->Set("RunInfo/Run Number", (Int_t) runNo);
  header->Set("RunInfo/Run Start Time", "2016-03-01 06:20:00");
  header->Set("RunInfo/Run Stop Time", "2016-03-01 06:20:11");

  prop.Set("Run Duration", 11.0, "sec");
  header->Set("RunInfo/Run Duration", prop);

  header->Set("RunInfo/Laboratory", "PSI");

  header->Set("RunInfo/Instrument", "MC-Simulation");
  prop.Set("Muon Beam Momentum", 0.0, "MeV/c");

  header->Set("RunInfo/Muon Beam Momentum", prop);
  header->Set("RunInfo/Muon Species", "positive muon and muonium");
  header->Set("RunInfo/Muon Source", "Simulation");
  header->Set("RunInfo/Setup", "Monte-Carlo setup");
  header->Set("RunInfo/Comment", "Testing effect of charge-exchange or Mu0 spin flip processes on uSR signal");
  header->Set("RunInfo/Sample Name", "Monte-Carlo");
 
  prop.Set("Sample Temperature", MRH_UNDEFINED, T, 0.01, "K");
  header->Set("RunInfo/Sample Temperature", prop);
 
  prop.Set("Sample Magnetic Field", MRH_UNDEFINED, B, Bvar, "G"); 
  header->Set("RunInfo/Sample Magnetic Field", prop);
 
  header->Set("RunInfo/No of Histos", 2);
 
  prop.Set("Time Resolution", 1.0, "ns", "Simulation");
  header->Set("RunInfo/Time Resolution", prop);

  prop.Set("Implantation Energy", 0, "keV");
  header->Set("RunInfo/Implantation Energy", prop);

  prop.Set("Muon Spin Angle", 0, "degree along x");
  header->Set("RunInfo/Muon Spin Angle", prop); 
  
  header->Set("DetectorInfo/Detector001/Name", "e+ forward");
  header->Set("DetectorInfo/Detector001/Histo Number", 1);
  header->Set("DetectorInfo/Detector001/Histo Length", 18001);
  header->Set("DetectorInfo/Detector001/Time Zero Bin", 0.001); //doesn't like 0.0 as time zero
  header->Set("DetectorInfo/Detector001/First Good Bin", 1);
  header->Set("DetectorInfo/Detector001/Last Good Bin", 18000);
   
  header->Set("DetectorInfo/Detector002/Name", "e+ backward");
  header->Set("DetectorInfo/Detector002/Histo Number", 2);
  header->Set("DetectorInfo/Detector002/Histo Length", 18001);
  header->Set("DetectorInfo/Detector002/Time Zero Bin", 0.001);
  header->Set("DetectorInfo/Detector002/First Good Bin", 1);
  header->Set("DetectorInfo/Detector002/Last Good Bin", 18000);
   
  // simulation parameters
  header->Set("Simulation/Mu0 Precession frequency 12", Freq12);
  header->Set("Simulation/Mu0 Precession frequency 34", Freq34);
  header->Set("Simulation/Mu0 Precession frequency 23", Freq23);
  header->Set("Simulation/Mu0 Precession frequency 14", Freq14);
  header->Set("Simulation/Mu0 Fraction", MuFrac);
  header->Set("Simulation/Mu0 Fraction 12", MuFrac12);
  header->Set("Simulation/Mu0 Fraction 23", MuFrac23);
  header->Set("Simulation/muon Capture Rate", capRate);
  header->Set("Simulation/Mu0 Ionization Rate", ionRate);
  header->Set("Simulation/Mu0 Spin Flip Rate", spinFlipRate);
  header->Set("Simulation/Number of Muons", Nmuons);
  header->Set("Simulation/Decay Asymmetry", Asym);

  header->Set("SampleEnvironmentInfo/Cryo", "no cryostat");
  header->Set("MagneticFieldEnvironmentInfo/Magnet Name", "Field along z");
  header->Set("BeamlineInfo/Name", "Monte-Carlo setup");

  histosFolder = gROOT->GetRootFolder()->AddFolder("histos", "Histograms");
  gROOT->GetListOfBrowsables()->Add(histosFolder, "histos");
  decayAnaModule = histosFolder->AddFolder("DecayAnaModule", "muSR decay histograms");
  scAnaModule    = histosFolder->AddFolder("SCAnaModule", "SlowControl histograms");
  
  TH1F *histo[NDECAYHISTS];
  char str[128];
  for (UInt_t i=0; i<NDECAYHISTS; i++) {
    sprintf(str, "hDecay00%d", (Int_t)i+1);
    histo[i] = new TH1F(str, str, 18001, -0.5, 18000.5);
  }

  for (i=0; i<NDECAYHISTS; i++)
    decayAnaModule->Add(histo[i]);
  
  // run simulation
  simulateMuTransition->PrintSettings();
  simulateMuTransition->Run(histo[0], histo[1]);

  // write file
  TFile *fout = new TFile(histogramFileName.Data(), "RECREATE", "Midas MC Histograms");
  if (fout == 0) {
    cout << endl << "**ERROR** Couldn't create ROOT file";
    cout << endl << endl;
    exit(0);
  }

  fout->cd();
  
  header->FillFolder(gRunHeader);
  gRunHeader->Add(&Slist);
  Slist.SetName("RunSummary");
  histosFolder->Write();
  gRunHeader->Write();
  fout->Close();
  cout << "Histograms written to " << histogramFileName.Data() << endl;
  delete fout;

  delete [] histo;
}
