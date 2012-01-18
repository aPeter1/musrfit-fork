/***************************************************************************

  write_musrRoot_runHeader.cpp

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

#include <cstdlib>
#include <ctime>
#include <cstring>

#include <iostream>
using namespace std;

#include <TROOT.h>
#include <TFile.h>
#include <TFolder.h>

#include "TMusrRunHeader.h"

void write_musrRoot_runHeader_syntax()
{
  cout << endl << "usage: write_musrRoot_runHeader <fileName>";
  cout << endl << "       <fileName> is the file name including the extention root, e.g. test.root";
  cout << endl << endl;
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    write_musrRoot_runHeader_syntax();
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
  header->Set("RunInfo/Run Number", 577);

  // run info - start/stop time and duration
  TString startTime("2011-04-19 14:25:22"), stopTime("2011-04-19 19:13:47");
  struct tm tm_start, tm_stop;
  header->Set("RunInfo/Run Start Time", startTime);
  header->Set("RunInfo/Run Stop Time", stopTime);
  // calculate run duration
  memset(&tm_start, 0, sizeof(tm_start));
  strptime(startTime.Data(), "%Y-%m-%d %H:%M:%S", &tm_start);
  memset(&tm_stop, 0, sizeof(tm_stop));
  strptime(stopTime.Data(), "%Y-%m-%d %H:%M:%S", &tm_stop);
  Double_t duration = difftime(mktime(&tm_stop), mktime(&tm_start));
  header->Set("RunInfo/Run Duration", (Int_t)duration);

  header->Set("RunInfo/Laboratory", "PSI");
  header->Set("RunInfo/Area", "piM3.2");
  header->Set("RunInfo/Instrument", "GPS");

  prop.Set("Muon Beam Momentum", 28.1, "MeV/c");
  header->Set("RunInfo/Muon Beam Momentum", prop);

  header->Set("RunInfo/Muon Species", "positive muon");
  header->Set("RunInfo/Setup", "a very special setup");
  header->Set("RunInfo/Comment", "nothing more to be said");
  header->Set("RunInfo/Sample Name", "the best ever");

  prop.Set("Sample Temperature", 3.2, 3.21, 0.05, "K", "CF1");
  header->Set("RunInfo/Sample Temperature", prop);

  prop.Set("Sample Magnetic Field", 350.0, 350.002, 0.005, "G", "WXY");
  header->Set("RunInfo/Sample Magnetic Field", prop);

  header->Set("RunInfo/No of Histos", 4);

  TStringVector detectorName;
  detectorName.push_back("forward");
  detectorName.push_back("top");
  detectorName.push_back("backward");
  detectorName.push_back("bottom");
  header->Set("RunInfo/Histo Names", detectorName);

  header->Set("RunInfo/Histo Length", 8192);

  prop.Set("Time Resolution", 0.1953125, "ns", "TDC 9999");
  header->Set("RunInfo/Time Resolution", prop);

  TDoubleVector t0;
  TIntVector ivec;
  for (UInt_t i=0; i<4; i++) t0.push_back(215.0+5.0*(Double_t)rand()/(Double_t)RAND_MAX);
  header->Set("RunInfo/Time Zero Bin", t0);
  for (UInt_t i=0; i<4; i++) ivec.push_back((Int_t)t0[i] + 12);
  header->Set("RunInfo/First Good Bin", ivec);
  for (UInt_t i=0; i<4; i++) ivec[i] = 8191;
  header->Set("RunInfo/Last Good Bin", ivec);

  TIntVector readGreenOffset;
  readGreenOffset.push_back(0);
  readGreenOffset.push_back(10);
  readGreenOffset.push_back(20);
  readGreenOffset.push_back(30);
  header->Set("RunInfo/Red-Green Offsets", readGreenOffset);

  TStringVector redGreenDescription;
  redGreenDescription.push_back("E-field/light off/off");
  redGreenDescription.push_back("E-field/light on/off");
  redGreenDescription.push_back("E-field/light off/on");
  redGreenDescription.push_back("E-field/light on/on");
  header->Set("RunInfo/Red-Green Description", redGreenDescription);

  TStringVector scHistoNames;
  scHistoNames.push_back("Sample Temperature");
  scHistoNames.push_back("Sample Magnetic Field");
  header->Set("RunInfo/Slow Control Histo Names", scHistoNames);

  TStringVector dummyTest;
  dummyTest.push_back("dummy1");
  dummyTest.push_back("dummy2");
  dummyTest.push_back("dummy3");
  header->Set("RunInfo/Dummy Test", dummyTest);

  // sample environment
  header->Set("SampleEnvironmentInfo/Cryo", "Konti-1");
  header->Set("SampleEnvironmentInfo/Insert", "X123");
  header->Set("SampleEnvironmentInfo/Orientation", "c-axis perp spin, perp field. spin perp field");

  prop.Set("CF2", 3.2, 3.22, 0.04, "K");
  header->Set("SampleEnvironmentInfo/CF2", prop);

  prop.Set("CF3", MRH_UNDEFINED, 3.27, 0.09, "K", "strange temperature");
  header->Set("SampleEnvironmentInfo/CF3", prop);

  prop.Set("CF4", 3.25, 3.28, "K");
  header->Set("SampleEnvironmentInfo/CF4", prop);

  prop.Set("CF5", 3.26, 3.29, "K", "another strange temperature");
  header->Set("SampleEnvironmentInfo/CF5", prop);

  prop.Set("Dummy Prop", -2.0, -2.001, 0.002, "SI-unit");
  header->Set("SampleEnvironmentInfo/Dummy Prop", prop);

  // magnetic field environment
  header->Set("MagneticFieldEnvironmentInfo/Name", "Bpar");
  prop.Set("Current", 1.34, "A");
  header->Set("MagneticFieldEnvironmentInfo/Current", prop);

  // detector forward
  header->Set("Detectors/Detector000/Name", "forward");
  header->Set("Detectors/Detector000/Histo Number", 0);
  header->Set("Detectors/Detector000/Histo Length", 8192);

  // detector backward
  header->Set("Detectors/Detector001/Name", "backward");
  header->Set("Detectors/Detector001/Histo Number", 0);
  header->Set("Detectors/Detector001/Histo Length", 8192);

  // beamline
  header->Set("BeamlineInfo/WSX61a", "DAC = 3289, ADC = 0.800");

  TIntVector dummyInt;
  for (UInt_t i=0; i<3; i++) dummyInt.push_back(i+1000);
  header->Set("BeamlineInfo/Dummy Int", dummyInt);


  // scaler
  header->Set("ScalerInfo/Ip", 12332123);

  // funny sub/sub/../sub-structure
  header->Set("aa/bb/cc/dd/ee/ff/name", "funny");
  header->Set("aa/bb/cc/dd/ee/ff/gg/xyz", 123);

  TFile *f = new TFile(argv[1], "RECREATE", "write_musrRoot_runHeader");
  if (f->IsZombie()) {
    delete f;
    return -1;
  }

  // root file header related things
  TFolder *runHeader = gROOT->GetRootFolder()->AddFolder("RunHeader", "MusrRoot Run Header Info");
  gROOT->GetListOfBrowsables()->Add(runHeader, "RunHeader");

  if (header->FillFolder(runHeader)) {
    runHeader->Write();
  }

  f->Close();

  // clean up
  delete f;
  delete header;

  return 0;
}
