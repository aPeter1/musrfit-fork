/***************************************************************************

  musrRoot_runHeader_test.cpp

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

#include <iostream>
using namespace std;

#include <TROOT.h>
#include <TFile.h>
#include <TFolder.h>

#include "TMusrRunHeader.h"

void musrRoot_runHeader_test_syntax()
{
  cout << endl << "usage: musrRoot_runHeader_test <fileName>";
  cout << endl << "       <fileName> is the file name including the extention root, e.g. test.root";
  cout << endl << endl;
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    musrRoot_runHeader_test_syntax();
    return 1;
  }

  // MusrRoot Run Header object
  TMusrRunHeader *header = new TMusrRunHeader(argv[1]);
  TMusrRunPhysicalQuantity prop;

  // run info
  header->Set("RunInfo/Version", "$Id$");
  header->Set("RunInfo/Generator", "any2many");
  header->Set("RunInfo/File Name", "thisIsAFileName");
  header->Set("RunInfo/Run Title", "here comes the run title");
  header->Set("RunInfo/Run Number", 576);
  header->Set("RunInfo/Run Number", 577);
  header->Set("RunInfo/Run Start Time", "2011-04-19 14:25:22");
  header->Set("RunInfo/Run Stop Time", "2011-04-19 19:13:47");

  prop.Set("Time Resolution", 0.1953125, "ns", "TDC 9999");
  header->Set("RunInfo/Time Resolution", prop);

  prop.Set("Sample Temperature", 3.2, 3.21, 0.05, "K", "CF1");
  header->Set("RunInfo/Sample Temperature", prop);

  prop.Set("Muon Beam Momentum", MRRH_UNDEFINED, 28.1, MRRH_UNDEFINED, "MeV/c");
  header->Set("RunInfo/Muon Beam Momentum", prop);

  TStringVector detectorName;
  detectorName.push_back("left_down");
  detectorName.push_back("left_up");
  detectorName.push_back("top_down");
  detectorName.push_back("top_up");
  detectorName.push_back("right_down");
  detectorName.push_back("right_up");
  detectorName.push_back("bottom_down");
  detectorName.push_back("bottom_up");
  header->Set("RunInfo/Histo Names", detectorName);

  TDoubleVector t0;
  for (UInt_t i=0; i<8; i++) t0.push_back(3419.0);
  header->Set("RunInfo/Time Zero Bin", t0);

  TStringVector dummyTest;
  dummyTest.push_back("dummy1");
  dummyTest.push_back("dummy2");
  dummyTest.push_back("dummy3");
  header->Set("RunInfo/Dummy Test", dummyTest);

  // sample environment
  header->Set("SampleEnv/Cryo", "Konti-1");

  prop.Set("CF2", 3.2, 3.22, 0.04, "K");
  header->Set("SampleEnv/CF2", prop);

  prop.Set("CF3", MRRH_UNDEFINED, 3.27, 0.09, "K", "strange temperature");
  header->Set("SampleEnv/CF3", prop);

  prop.Set("CF4", 3.25, 3.28, "K");
  header->Set("SampleEnv/CF4", prop);

  prop.Set("CF5", 3.26, 3.29, "K", "another strange temperature");
  header->Set("SampleEnv/CF5", prop);

  prop.Set("Dummy Prop", -2.0, -2.001, 0.002, "SI-unit");
  header->Set("SampleEnv/Dummy Prop", prop);

  // magnetic field environment
  header->Set("MagFieldEnv/Name", "Bpar");
  prop.Set("Current", 1.34, "A");
  header->Set("MagFieldEnv/Current", prop);

  // beamline
  header->Set("Beamline/WSX61a", "DAC = 3289, ADC = 0.800");

  TIntVector dummyInt;
  for (UInt_t i=0; i<3; i++) dummyInt.push_back(i+1000);
  header->Set("Beamline/Dummy Int", dummyInt);

  // scaler
  header->Set("Scaler/Ip", 12332123);

  TFile *f = new TFile(argv[1], "RECREATE", "musrRoot_runHeader_test");
  if (f->IsZombie()) {
    delete f;
    return -1;
  }

  // root file header related things
  TFolder *runHeader = gROOT->GetRootFolder()->AddFolder("RunHeader", "MusrRoot Run Header Info");
  gROOT->GetListOfBrowsables()->Add(runHeader, "RunHeader");

  TObjArray runInfo;
  header->GetHeaderInfo("RunInfo", runInfo);
  runHeader->Add(&runInfo);

  TObjArray sampleEnv;
  header->GetHeaderInfo("SampleEnv", sampleEnv);
  runHeader->Add(&sampleEnv);

  TObjArray magFieldEnv;
  header->GetHeaderInfo("MagFieldEnv", magFieldEnv);
  runHeader->Add(&magFieldEnv);

  TObjArray beamline;
  header->GetHeaderInfo("Beamline", beamline);
  runHeader->Add(&beamline);

  TObjArray scaler;
  header->GetHeaderInfo("Scaler", scaler);
  runHeader->Add(&scaler);

  TMap *map = header->GetMap();
  runHeader->Add(map);

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
  f = new TFile(argv[1], "READ", "musrRoot_runHeader_test");
  if (f->IsZombie()) {
    delete f;
    return -1;
  }

  runHeader = 0;
  f->GetObject("RunHeader", runHeader);
  if (runHeader == 0) {
    cerr << endl << ">> **ERROR** Couldn't get top folder RunHeader";
    f->Close();
    return -1;
  }

  TObjArray *oarray = 0;
  header = new TMusrRunHeader(argv[1]);

  map = (TMap*) runHeader->FindObjectAny("__map");
  if (map == 0) {
    cerr << endl << ">> **ERROR** couldn't find required __map :-(" << endl;
    f->Close();
    return -1;
  }
  header->SetMap(map);

  // get RunHeader
  oarray = (TObjArray*) runHeader->FindObjectAny("RunInfo");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get RunInfo" << endl;
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

  f->Close();
  delete f;

  header->DumpHeader();

  // get some information from the read file
  cout << endl << "++++++++++++++++++++++++++++";
  cout << endl << ">> get header infos " << argv[1];
  cout << endl << "++++++++++++++++++++++++++++" << endl;

  TString str("");
  TStringVector strVec;
  Int_t ival;
  TDoubleVector dvec;
  TMusrRunPhysicalQuantity prop1;
  Bool_t ok;

  header->GetValue("RunInfo/Run Title", str, ok);
  if (ok)
    cout << endl << "Run Title: " << str.Data();
  else
    cout << endl << "**ERROR** Couldn't obtain the 'Run Title'.";

  header->GetValue("RunInfo/Run Number", ival, ok);
  if (ok)
    cout << endl << "Run Number: " << ival;
  else
    cout << endl << "**ERROR** Couldn't obtain the 'Run Number'.";

  header->GetValue("RunInfo/Histo Names", strVec, ok);
  if (ok) {
    cout << endl << "Histo Names: ";
    for (UInt_t i=0; i<strVec.size()-1; i++) {
      cout << strVec[i].Data() << ", ";
    }
    cout << strVec[strVec.size()-1].Data();
  } else {
    cout << endl << "**ERROR** Couldn't obtain the 'Histo Names'.";
  }

  header->GetValue("RunInfo/Time Zero Bin", dvec, ok);
  if (ok) {
    cout << endl << "Time Zero Bin: ";
    for (UInt_t i=0; i<dvec.size()-1; i++) {
      cout << dvec[i] << ", ";
    }
    cout << dvec[dvec.size()-1];
  } else {
    cout << endl << "**ERROR** Couldn't obtain the 'Time Zero Bin'.";
  }

  header->GetValue("RunInfo/Sample Temperature", prop1, ok);
  if (ok) {
    cout << endl << "Sample Temperature: " << prop1.GetValue() << " +- " << prop1.GetError() << " " << prop1.GetUnit().Data() << "; SP: " << prop1.GetDemand() << "; " << prop1.GetDescription().Data();
  } else {
    cout << endl << "**ERROR** Couldn't obtain the 'Sample Temperature'.";
  }

  cout << endl << endl;

  return 0;
}
