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
  header->Set("RunInfo/Generic Validator URL", "http://lmu.web.psi.ch/facilities/software/MusrRoot/Validation/MusrRoot.xsd");
  header->Set("RunInfo/Specific Validator URL", "http://lmu.web.psi.ch/facilities/software/MusrRoot/Validation/MusrRootGPS.xsd");
  header->Set("RunInfo/Generator", "any2many");
  header->Set("RunInfo/File Name", "deltat_tdc_gps_2871.root");
  header->Set("RunInfo/Run Title", "here comes the run title");
  header->Set("RunInfo/Run Number", 2871);

  // run info - start/stop time and duration
  TString startTime("2012-04-19 14:25:22"), stopTime("2012-04-19 19:13:47");
  struct tm tm_start, tm_stop;
  header->Set("RunInfo/Run Start Time", startTime);
  header->Set("RunInfo/Run Stop Time", stopTime);
  // calculate run duration
  memset(&tm_start, 0, sizeof(tm_start));
  strptime(startTime.Data(), "%Y-%m-%d %H:%M:%S", &tm_start);
  memset(&tm_stop, 0, sizeof(tm_stop));
  strptime(stopTime.Data(), "%Y-%m-%d %H:%M:%S", &tm_stop);
  Double_t duration = difftime(mktime(&tm_stop), mktime(&tm_start));
  prop.Set("Run Duration", (Int_t)duration, "sec");
  header->Set("RunInfo/Run Duration", prop);

  header->Set("RunInfo/Laboratory", "PSI");
  header->Set("RunInfo/Instrument", "GPS");

  prop.Set("Muon Beam Momentum", 28.1, "MeV/c");
  header->Set("RunInfo/Muon Beam Momentum", prop);

  header->Set("RunInfo/Muon Species", "positive muon");
  header->Set("RunInfo/Muon Source", "target M");
  header->Set("RunInfo/Setup", "a very special setup with Heliox");
  header->Set("RunInfo/Comment", "nothing more to be said");
  header->Set("RunInfo/Sample Name", "the best ever");

  prop.Set("Sample Temperature", 3.2, 3.21, 0.05, "K", "CF1");
  header->Set("RunInfo/Sample Temperature", prop);

  prop.Set("Sample Magnetic Field", 350.0, 350.002, 0.005, "G", "WXY");
  header->Set("RunInfo/Sample Magnetic Field", prop);

  header->Set("RunInfo/No of Histos", 4);

  prop.Set("Time Resolution", 0.1953125, "ns", "TDC 9999");
  header->Set("RunInfo/Time Resolution", prop);

  header->Set("DetectorInfo/Detector000/Name", "Left - NPP");
  header->Set("DetectorInfo/Detector000/Histo Number", 0);
  header->Set("DetectorInfo/Detector000/Histo Length", 66661);
  header->Set("DetectorInfo/Detector000/Time Zero Bin", 3419.0);
  header->Set("DetectorInfo/Detector000/First Good Bin", 3419);
  header->Set("DetectorInfo/Detector000/Last Good Bin", 66661);

  header->Set("DetectorInfo/Detector001/Name", "Top - NPP");
  header->Set("DetectorInfo/Detector001/Histo Number", 1);
  header->Set("DetectorInfo/Detector001/Histo Length", 66661);
  header->Set("DetectorInfo/Detector001/Time Zero Bin", 3419.0);
  header->Set("DetectorInfo/Detector001/First Good Bin", 3419);
  header->Set("DetectorInfo/Detector001/Last Good Bin", 66661);

  header->Set("DetectorInfo/Detector002/Name", "Right - NPP");
  header->Set("DetectorInfo/Detector002/Histo Number", 2);
  header->Set("DetectorInfo/Detector002/Histo Length", 66661);
  header->Set("DetectorInfo/Detector002/Time Zero Bin", 3419.0);
  header->Set("DetectorInfo/Detector002/First Good Bin", 3419);
  header->Set("DetectorInfo/Detector002/Last Good Bin", 66661);

  header->Set("DetectorInfo/Detector003/Name", "Bottom - NPP");
  header->Set("DetectorInfo/Detector003/Histo Number", 3);
  header->Set("DetectorInfo/Detector003/Histo Length", 66661);
  header->Set("DetectorInfo/Detector003/Time Zero Bin", 3419.0);
  header->Set("DetectorInfo/Detector003/First Good Bin", 3419);
  header->Set("DetectorInfo/Detector003/Last Good Bin", 66661);

  header->Set("DetectorInfo/Detector004/Name", "Left - PPC");
  header->Set("DetectorInfo/Detector004/Histo Number", 20);
  header->Set("DetectorInfo/Detector004/Histo Length", 66661);
  header->Set("DetectorInfo/Detector004/Time Zero Bin", 3419.0);
  header->Set("DetectorInfo/Detector004/First Good Bin", 3419);
  header->Set("DetectorInfo/Detector004/Last Good Bin", 66661);

  header->Set("DetectorInfo/Detector005/Name", "Top - PPC");
  header->Set("DetectorInfo/Detector005/Histo Number", 21);
  header->Set("DetectorInfo/Detector005/Histo Length", 66661);
  header->Set("DetectorInfo/Detector005/Time Zero Bin", 3419.0);
  header->Set("DetectorInfo/Detector005/First Good Bin", 3419);
  header->Set("DetectorInfo/Detector005/Last Good Bin", 66661);

  header->Set("DetectorInfo/Detector006/Name", "Right - PPC");
  header->Set("DetectorInfo/Detector006/Histo Number", 22);
  header->Set("DetectorInfo/Detector006/Histo Length", 66661);
  header->Set("DetectorInfo/Detector006/Time Zero Bin", 3419.0);
  header->Set("DetectorInfo/Detector006/First Good Bin", 3419);
  header->Set("DetectorInfo/Detector006/Last Good Bin", 66661);

  header->Set("DetectorInfo/Detector007/Name", "Bottom - PPC");
  header->Set("DetectorInfo/Detector007/Histo Number", 23);
  header->Set("DetectorInfo/Detector007/Histo Length", 66661);
  header->Set("DetectorInfo/Detector007/Time Zero Bin", 3419.0);
  header->Set("DetectorInfo/Detector007/First Good Bin", 3419);
  header->Set("DetectorInfo/Detector007/Last Good Bin", 66661);

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
  header->Set("MagneticFieldEnvironmentInfo/Magnet Name", "Bpar");
  prop.Set("Current", 1.34, "A");
  header->Set("MagneticFieldEnvironmentInfo/Current", prop);

  // beamline
  header->Set("BeamlineInfo/Name", "piM3.2");
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
/*
  TFolder *runHeader = gROOT->GetRootFolder()->AddFolder("RunHeader", "MusrRoot Run Header Info");
  gROOT->GetListOfBrowsables()->Add(runHeader, "RunHeader");
*/
  TFolder *runHeader = new TFolder("RunHeader", "MusrRoot Run Header Info");

  if (header->FillFolder(runHeader)) {
    runHeader->Write();
  }

  f->Close();

  // clean up
  delete f;
  delete header;

  return 0;
}
