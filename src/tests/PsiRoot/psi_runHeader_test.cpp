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
  TPsiRunProperty *prop;

  // run info
  header->Set("RunInfo/Version", "TString", "$Id$");
  header->Set("RunInfo/Generator", "TString", "any2many");
  header->Set("RunInfo/File Name", "TString", "thisIsAFileName");
  header->Set("RunInfo/Run Title", "TString", "here comes the run title");
  header->Set("RunInfo/Run Number", "Int_t", 576);
  header->Set("RunInfo/Run Number", "Int_t", 577);
  header->Set("RunInfo/Run Start Time", "TString", "2011-04-19 14:25:22");
  header->Set("RunInfo/Run Stop Time", "TString", "2011-04-19 19:13:47");

  prop = new TPsiRunProperty("Time Resolution", 0.193525, "ns");
  header->Set("RunInfo/Time Resolution", "TPsiRunProperty", *prop);

  prop = new TPsiRunProperty("Sample Temperature", 3.2, 3.20, 0.05, "K", "CF1");
  header->Set("RunInfo/Sample Temperature", "TPsiRunProperty", *prop);

  prop = new TPsiRunProperty("Muon Beam Momentum", PRH_UNDEFINED, 28.1, PRH_UNDEFINED, "MeV/c");
  header->Set("RunInfo/Muon Beam Momentum", "TPsiRunProperty", *prop);

  TStringVector detectorName;
  detectorName.push_back("left_down");
  detectorName.push_back("left_up");
  detectorName.push_back("top_down");
  detectorName.push_back("top_up");
  detectorName.push_back("right_down");
  detectorName.push_back("right_up");
  detectorName.push_back("bottom_down");
  detectorName.push_back("bottom_up");
  header->Set("RunInfo/Histo Names", "TStringVector", detectorName);

  TIntVector t0;
  for (UInt_t i=0; i<8; i++) t0.push_back(3419);
  header->Set("RunInfo/Time Zero Bin", "TIntVector", t0);

  TStringVector dummyTest;
  dummyTest.push_back("dummy1");
  dummyTest.push_back("dummy2");
  dummyTest.push_back("dummy3");
  header->Set("RunInfo/Dummy Test", "TStringVector", dummyTest);

  // sample environment
  header->Set("SampleEnv/Cryo", "TString", "Konti-1");
  prop = new TPsiRunProperty("CF2", 3.2, 3.22, 0.04, "A");
  header->Set("SampleEnv/CF2", "TPsiRunProperty", *prop);

  prop = new TPsiRunProperty("Dummy Prop", -2.0, -2.001, 0.002, "SI unit");
  header->Set("SampleEnv/Dummy Prop", "TPsiRunProperty", *prop);

  // magnetic field environment
  header->Set("MagFieldEnv/Name", "TString", "Bpar");
  prop = new TPsiRunProperty("Current", 1.34, "A");
  header->Set("MagFieldEnv/Current", "TPsiRunProperty", *prop);

  // beamline
  header->Set("Beamline/WSX61a", "TString", "DAC = 3289, ADC = 0.800");

  TIntVector dummyInt;
  for (UInt_t i=0; i<3; i++) dummyInt.push_back(i+1000);
  header->Set("Beamline/Dummy Int", "TIntVector", dummyInt);


  // scaler
  header->Set("Scaler/Ip", "Int_t", 12332123);

  if (!header->IsValid()) {
    cerr << endl << ">> **ERROR** run header validation failed." << endl;
  }

  TFile *f = new TFile(argv[1], "RECREATE", "psi_runHeader_test");
  if (f->IsZombie()) {
    delete f;
    return -1;
  }

  // root file header related things
  TFolder *runHeader = gROOT->GetRootFolder()->AddFolder("RunHeaderInfo", "PSI Run Header Info");
  gROOT->GetListOfBrowsables()->Add(runHeader, "RunHeaderInfo");

  TObjArray runInfo;
  header->Get("RunInfo", runInfo);
  runHeader->Add(&runInfo);

  TObjArray sampleEnv;
  header->Get("SampleEnv", sampleEnv);
  runHeader->Add(&sampleEnv);

  TObjArray magFieldEnv;
  header->Get("MagFieldEnv", magFieldEnv);
  runHeader->Add(&magFieldEnv);

  TObjArray beamline;
  header->Get("Beamline", beamline);
  runHeader->Add(&beamline);

  TObjArray scaler;
  header->Get("Scaler", scaler);
  runHeader->Add(&scaler);

  runHeader->Write();

  f->Close();

  delete f;
  f = 0;

  header->DumpHeader();

  delete header;
  header = 0;

  cout << endl << "++++++++++++++++++++++++++++";
  cout << endl << ">> read back " << argv[1];
  cout << endl << "++++++++++++++++++++++++++++" << endl;

  // read the file back and extract the header info
  f = new TFile(argv[1], "READ", "psi_runHeader_test");
  if (f->IsZombie()) {
    delete f;
    return -1;
  }

  runInfo = 0;
  f->GetObject("RunHeaderInfo", runHeader);
  if (runHeader == 0) {
    cerr << endl << ">> **ERROR** Couldn't get top folder RunHeaderInfo";
    f->Close();
    return -1;
  }

  TObjArray *oarray = 0;
  header = new TPsiRunHeader();

  // get RunHeader
  oarray = (TObjArray*) runHeader->FindObjectAny("RunInfo");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get RunHeader" << endl;
  }
  header->ExtractHeaderInformation(oarray, "RunInfo");

  // get SampleEnv
  oarray = (TObjArray*) runHeader->FindObjectAny("SampleEnv");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get SampleEnv" << endl;
  }
  header->ExtractHeaderInformation(oarray, "SampleEnv");

  // get MagFieldEnv
  oarray = (TObjArray*) runHeader->FindObjectAny("MagFieldEnv");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get MagFieldEnv" << endl;
  }
  header->ExtractHeaderInformation(oarray, "MagFieldEnv");

  // get Beamline
  oarray = (TObjArray*) runHeader->FindObjectAny("Beamline");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get Beamline" << endl;
  }
  header->ExtractHeaderInformation(oarray, "Beamline");

  // get Scaler
  oarray = (TObjArray*) runHeader->FindObjectAny("Scaler");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get Scaler" << endl;
  }
  header->ExtractHeaderInformation(oarray, "Scaler");

  header->DumpHeader();

  f->Close();
  delete f;

  return 0;
}
