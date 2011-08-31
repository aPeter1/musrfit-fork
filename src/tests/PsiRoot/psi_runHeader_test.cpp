/***************************************************************************

  psi_runHeader_test.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2011 by Andreas Suter                              *
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
using namespace std;

#include <TROOT.h>
#include <TFile.h>
#include <TFolder.h>

#include "TPsiRunHeader.h"

void psi_runHeader_test_syntax()
{
  cout << endl << "usage: psi_runHeader_test <fileName>";
  cout << endl << "       <fileName> is the file name including the extention root, e.g. test.root";
  cout << endl << endl;
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    psi_runHeader_test_syntax();
    return 1;
  }

  // PSI Run Header object
  TPsiRunHeader *header = new TPsiRunHeader();

  header->SetGenerator("psi_runHeader_test");
  header->SetFileName(argv[1]);
  header->SetRunTitle("This is a run title");
  header->SetRunNumber(12345);
  header->SetStartTime("2011-08-31 08:03:23");
  header->SetStopTime("2011-08-31 10:46:48");
  header->SetLaboratory("PSI");
  header->SetInstrument("LEM");
  header->SetArea("muE4");
  header->AddProperty("Muon Beam Momentum", 28.0, 28.0, 0.7, "MeV/c");
  header->SetMuonSpecies("positive Muon");
  header->SetSetup("Konti-4, WEW");
  header->SetSample("Eu2CuO4 MOD thin film");
  header->AddProperty("Sample Temperature", 30.0, 30.01, 0.05, "K");
  header->AddProperty("Sample Magnetic Field", 3.0, 3.0003, 0.000025, "T");
  header->SetOrientation("c-axis perp to spin");
  header->AddProperty("T1", 30.0, 30.003, 0.003, "K", "sample stick temperature", "/SampleEnv/");
  header->SetSampleCryo("Konti-4");
  header->SetSampleCryoInsert("n/a");
  header->SetSampleMagnetName("Bpar");
  header->AddProperty("Current", 1.54, 1.54, 0.003, "A", "Danfysik", "/MagFieldEnv/");

  header->SetNoOfHistos(8);
  header->SetHistoName("left/forward");
  header->SetHistoName("top/forward");
  header->SetHistoName("right/forward");
  header->SetHistoName("bottom/forward");
  header->SetHistoName("left/backward");
  header->SetHistoName("top/backward");
  header->SetHistoName("right/backward");
  header->SetHistoName("bottom/backward");
  header->SetHistoLength(66601);
  header->SetTimeResolution(0.1953125, "ns");

  for (Int_t i=0; i<header->GetNoOfHistos(); i++) {
    header->SetTimeZeroBin(3419);
    header->SetFirstGoodBin(3419);
    header->SetLastGoodBin(65000);
  }

  header->SetRedGreenHistogramOffset(0);
  header->SetRedGreenHistogramOffset(20);
  header->SetRedGreenDescription("NPP");
  header->SetRedGreenDescription("PPC");

  if (!header->IsValid()) {
    cerr << endl << ">> **ERROR** PSI-ROOT run header is not valid/complete." << endl;
    delete header;
    return -1;
  }

  TFile *f = new TFile(argv[1], "RECREATE", "psi_runHeader_test");
  if (f->IsZombie()) {
    delete f;
    return -1;
  }

  // root file header related things
  UInt_t count = 1;
  TFolder *runInfo = gROOT->GetRootFolder()->AddFolder("RunInfo", "PSI RunInfo");
  gROOT->GetListOfBrowsables()->Add(runInfo, "RunInfo");
  runInfo->Add(header->GetHeader(count));
  runInfo->Add(header->GetSampleEnv(count));
  runInfo->Add(header->GetMagFieldEnv(count));
  runInfo->Add(header->GetBeamline(count));
  runInfo->Add(header->GetScaler(count));
  runInfo->Write();

  f->Close();

  delete f;
  f = 0;

  header->DumpHeader();

  delete header;
  header = 0;

  cout << endl << ">> read back " << argv[1] << endl;

  // read the file back and extract the header info
  f = new TFile(argv[1], "READ", "psi_runHeader_test");
  if (f->IsZombie()) {
    delete f;
    return -1;
  }

  runInfo = 0;
  f->GetObject("RunInfo", runInfo);
  if (runInfo == 0) {
    cerr << endl << ">> **ERROR** Couldn't get top folder RunInfo";
    f->Close();
    return -1;
  }

  TObjArray *oarray = 0;
  header = new TPsiRunHeader();

  // get RunHeader
  oarray = (TObjArray*) runInfo->FindObjectAny("RunHeader");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get RunHeader" << endl;
  }
  header->ExtractHeaderInformation(oarray);

  // get SampleEnv
  oarray = (TObjArray*) runInfo->FindObjectAny("SampleEnv");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get SampleEnv" << endl;
  }
  header->ExtractHeaderInformation(oarray, "/SampleEnv/");

  // get MagFieldEnv
  oarray = (TObjArray*) runInfo->FindObjectAny("MagFieldEnv");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get MagFieldEnv" << endl;
  }
  header->ExtractHeaderInformation(oarray, "/MagFieldEnv/");

  // get Beamline
  oarray = (TObjArray*) runInfo->FindObjectAny("Beamline");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get Beamline" << endl;
  }
  header->ExtractHeaderInformation(oarray, "/Beamline/");

  // get Scaler
  oarray = (TObjArray*) runInfo->FindObjectAny("Scalers");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get Scalers" << endl;
  }
  header->ExtractHeaderInformation(oarray, "/Scalers/");

  header->DumpHeader();

  f->Close();
  delete f;

  return 0;
}
