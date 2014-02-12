/***************************************************************************

  dump_header.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstring>
#include <ctime>
#include <cassert>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
using namespace std;

#include <TFile.h>

#include "git-revision.h"
#include "TMusrRunHeader.h"
#include "TLemRunHeader.h"
#include "MuSR_td_PSI_bin.h"
#include "mud.h"

#ifdef PNEXUS_ENABLED
#include "PNeXus.h"
#endif

#define DH_MUSR_ROOT 0
#define DH_LEM_ROOT  1

//------------------------------------------------------------------------
/**
 *
 */
void dump_header_syntax()
{
  cout << endl << "usage: dump_header <fileName> [--file_format <fileFormat>] | --help | --version";
  cout << endl << "       Dumps the header information of a given muSR data file onto the standard output.";
  cout << endl << "       If no <fileFormat> is given, it will try to fiddle out what <fileFormat> it might be.";
  cout << endl;
  cout << endl << "       <fileName>: muSR data file name.";
  cout << endl << "       --file_format <fileFormat>: where <fileFormat> can be:";
  cout << endl << "                     MusrRoot, NeXus, ROOT (old LEM), PSI-BIN, PSI-MDU, MUD, WKM";
  cout << endl << "                     NeXus is only supported if enabled.";
  cout << endl << "       --help, -h    : will show this help";
  cout << endl << "       --version, -v : will show the current version.";
  cout << endl << endl;
}

//------------------------------------------------------------------------
/**
 *
 */
int dump_header_root(const string fileName, const string fileFormat)
{
  TFile f(fileName.c_str());
  if (f.IsZombie()) {
    return false;
  }

  UInt_t fileType = DH_MUSR_ROOT;

  TFolder *folder;
  f.GetObject("RunInfo", folder); // try first LEM-ROOT style file (used until 2011).
  if (!folder) { // either something is wrong, or it is a MusrRoot file
    f.GetObject("RunHeader", folder);
    if (!folder) { // something is wrong!!
      cerr << endl << "**ERROR** Couldn't neither obtain RunInfo (LEM), nor RunHeader (MusrRoot) from " << fileName << endl;
      f.Close();
      return 1;
    } else {
      fileType = DH_MUSR_ROOT;
    }
  } else {
    fileType = DH_LEM_ROOT;
  }

  if (fileType == DH_LEM_ROOT) { // ROOT (LEM)
    // read header and check if some missing run info need to be fed
    TLemRunHeader *runHeader = dynamic_cast<TLemRunHeader*>(folder->FindObjectAny("TLemRunHeader"));

    // check if run header is valid
    if (!runHeader) {
      cerr << endl << "**ERROR** Couldn't obtain run header info from ROOT file " << fileName << endl;
      f.Close();
      return 1;
    }

    cout << endl << "-------------------";
    cout << endl << "fileName = " << fileName << ", fileFormat = " << fileFormat;
    cout << endl << "-------------------";
    cout << endl << "Run Title          : " << runHeader->GetRunTitle().GetString().Data();
    cout << endl << "Run Number         : " << runHeader->GetRunNumber();
    cout << endl << "Run Start Time     : " << runHeader->GetStartTimeString().GetString().Data();
    cout << endl << "Run Stop Time      : " << runHeader->GetStopTimeString().GetString().Data();
    cout << endl << "Laboratory         : PSI";
    cout << endl << "Instrument         : LEM";
    cout << endl << "Beamline           : muE4";
    cout << endl << "Muon Beam Momentum : 28 MeV/c";
    cout << endl << "Muon Species       : positive muons";
    cout << endl << "Muon Source        : Target E - slow muons";
    cout << endl << "Setup              : " << runHeader->GetLemSetup().GetString().Data();
    cout << endl << "Comment            : n/a";
    cout << endl << "Sample Name        : n/a";
    cout << endl << "Sample Orientation : n/a";
    cout << endl << "Sample Temperature : " << runHeader->GetSampleTemperature() << "+-" << runHeader->GetSampleTemperatureError() << " K";
    cout << endl << "Sample Mag. Field  : " << runHeader->GetSampleBField() << "+-" << runHeader->GetSampleBFieldError() << " G";
    cout << endl << "No of Histos       : " << runHeader->GetNHist();
    cout << endl << "Time Resolution    : " << runHeader->GetTimeResolution() << " ns";
    cout << endl << "-------------------";
    cout << endl << "LEM Specific Entries :";
    cout << endl << "Moderator          : " << runHeader->GetModerator().GetString().Data();
    cout << endl << "Moderator HV       : " << runHeader->GetModeratorHV() << " kV";
    cout << endl << "Sample HV          : " << runHeader->GetSampleHV() << " kV";
    cout << endl << "Impl. Energy       : " << runHeader->GetImpEnergy() << " keV";
    cout << endl << "-------------------";
    cout << endl << "Detector Info (for all detectors the same): ";
    cout << endl << "-------------------";
    cout << endl << "Histo Length       : " << runHeader->GetNChannels();
    double *timeZero;
    timeZero = runHeader->GetTimeZero();
    cout << endl << "Time Zero Bin      : " << timeZero[0];
    cout << endl << "First Good Bin     : " << timeZero[0];
    cout << endl << "Last Good Bin      : " << runHeader->GetNChannels()-1;
    cout << endl << "-------------------" << endl << endl;

    delete runHeader;
  } else { // MusrRoot
    // invoke the MusrRoot header object
    TMusrRunHeader *header = new TMusrRunHeader(fileName.c_str(), true); // read quite
    if (header == 0) {
      cerr << endl << "**ERROR** Couldn't invoke MusrRoot RunHeader in file:" << fileName;
      cerr << endl;
      f.Close();
      return 1;
    }

    // try to populate the MusrRoot header object
    if (!header->ExtractAll(folder)) {
      cerr << endl << "**ERROR** Couldn't invoke MusrRoot RunHeader in file:" << fileName;
      cerr << endl;
      f.Close();
      return 1;
    }

    header->DumpHeader();

    delete header;
  }

  f.Close();

  return 0;
}

//------------------------------------------------------------------------
/**
 *
 */
int dump_header_nexus(const string fileName) {

#ifdef PNEXUS_ENABLED
  PNeXus *nxs_file = new PNeXus(fileName.c_str());

  if (nxs_file->IsValid(false)) {
    nxs_file->Dump();
  }

  if (nxs_file)
    delete nxs_file;
#else
  cout << endl << "NeXus not enabled, hence the header information cannot be dumped." << endl << endl;
#endif

  return 0;
}

//------------------------------------------------------------------------
/**
 *
 */
vector<string> dump_header_instrument_info(string fileName)
{
  vector<string> result;
  string fln(fileName);
  boost::to_lower(fln);

  if (fln.find(".bin") != string::npos) { // PSI-BIN or PSI-MDU format
    if (fln.find("_gps_") != string::npos) {
      result.push_back("GPS");
      result.push_back("piM3.2");
      result.push_back("28 MeV/c");
      result.push_back("likely to be positive muons");
      result.push_back("Target M");
    } else if (fln.find("_ltf_") != string::npos) {
      result.push_back("LTF");
      result.push_back("piM3.1");
      result.push_back("28 MeV/c");
      result.push_back("likely to be positive muons");
      result.push_back("Target M");
    } else if (fln.find("_dolly_") != string::npos) {
      result.push_back("DOLLY");
      result.push_back("piE1");
      result.push_back("28 MeV/c");
      result.push_back("likely to be positive muons");
      result.push_back("Target E");
    } else if (fln.find("_alc_") != string::npos) {
      result.push_back("ALC");
      result.push_back("piE3");
      result.push_back("28 MeV/c");
      result.push_back("likely to be positive muons");
      result.push_back("Target E");
    } else if (fln.find("_hifi_") != string::npos) {
      result.push_back("HIFI");
      result.push_back("piE3");
      result.push_back("28 MeV/c");
      result.push_back("likely to be positive muons");
      result.push_back("Target E");
    } else if (fln.find("_gpd_") != string::npos) {
      result.push_back("GPD");
      result.push_back("muE1");
      result.push_back("60-125 MeV/c");
      result.push_back("likely to be positive muons");
      result.push_back("Target E");
    } else {
      result.push_back("???");
      result.push_back("???");
      result.push_back("??? MeV/c");
      result.push_back("???");
      result.push_back("???");
    }
  }

  return result;
}

//------------------------------------------------------------------------
/**
 *
 */
int dump_header_psi_bin(const string fileName, const string fileFormat)
{
  MuSR_td_PSI_bin psiBin;
  int status;
  bool success = false;

  // read psi bin file
  status = psiBin.read(fileName.c_str());
  switch (status) {
    case 0: // everything perfect
      success = true;
      break;
    case 1: // couldn't open file, or failed while reading the header
      cout << endl << "**ERROR** couldn't open psi-bin file, or failed while reading the header." << endl;
      success = false;
      break;
    case 2: // unsupported version of the data
      cout << endl << "**ERROR** psi-bin file: unsupported version of the data." << endl;
      success = false;
      break;
    case 3: // error when allocating data buffer
      cout << endl << "**ERROR** psi-bin file: error when allocating data buffer." << endl;
      success = false;
      break;
    case 4: // number of histograms/record not equals 1
      cout << endl << ">> **ERROR** psi-bin file: number of histograms/record not equals 1." << endl;
      success = false;
      break;
    default: // you never should have reached this point
      cout << endl << ">> **ERROR** psi-bin file: no clue why you reached this point." << endl;
      success = false;
      break;
  }

  // if any reading error happend, get out of here
  if (!success)
    return 1;

  vector<string> vstr;
  cout << endl << "-------------------";
  cout << endl << "fileName = " << fileName << ", fileFormat = " << fileFormat;
  cout << endl << "-------------------";
  cout << endl << "Run Title          : " << psiBin.get_comment();
  cout << endl << "Run Number         : " << psiBin.get_runNumber_int();
  vstr = psiBin.get_timeStart_vector();
  if (vstr.size() < 2) {
    cout << endl << "**ERROR** couldn't obtain \"Run Start Time\" will quit." << endl << endl;
    return 1;
  }
  cout << endl << "Run Start Time     : " << vstr[0] << "; " << vstr[1];
  vstr = psiBin.get_timeStop_vector();
  if (vstr.size() < 2) {
    cout << endl << "**ERROR** couldn't obtain \"Run Stop Time\" will quit." << endl << endl;
    return 1;
  }
  cout << endl << "Run Stop Time      : " << vstr[0] << "; " << vstr[1];
  cout << endl << "Laboratory         : PSI";
  vstr = dump_header_instrument_info(fileName);
  if (vstr.size() < 5) {
    cout << endl << "**ERROR** couldn't obtain \"Instrument\" will quit." << endl << endl;
    return 1;
  }
  cout << endl << "Instrument         : " << vstr[0];
  cout << endl << "Beamline           : " << vstr[1];
  cout << endl << "Muon Beam Momentum : " << vstr[2];
  cout << endl << "Muon Species       : " << vstr[3];
  cout << endl << "Muon Source        : " << vstr[4];
  cout << endl << "Setup              : " << psiBin.get_comment();
  cout << endl << "Comment            : n/a";
  cout << endl << "Sample Name        : " << psiBin.get_sample();
  cout << endl << "Sample Orientation : " << psiBin.get_orient();
  cout << endl << "Sample Temperature : " << psiBin.get_temp();
  cout << endl << "Sample Mag. Field  : " << psiBin.get_field();
  cout << endl << "No of Histos       : " << psiBin.get_numberHisto_int();
  cout << endl << "Time Resolution    : " << psiBin.get_binWidth_ns() << " ns";
  for (int i=0; i<psiBin.get_numberHisto_int(); i++) {
    cout << endl << "-------------------";
    cout << endl << "Histo No           : " << i;
    cout << endl << "Histo Name         : " << psiBin.get_nameHisto(i);
    cout << endl << "Histo Length       : " << psiBin.get_histoLength_bin();
    cout << endl << "Time Zero Bin      : " << psiBin.get_t0_int(i);
    cout << endl << "First Good Bin     : " << psiBin.get_firstGood_int(i);
    cout << endl << "Last Good Bin      : " << psiBin.get_firstGood_int(i);
  }
  cout << endl << "-------------------" << endl << endl;

  return 0;
}

//------------------------------------------------------------------------
/**
 *
 */
int dump_header_mud(const string fileName, const string fileFormat)
{
  int    fh;
  UINT32 type, val;
  char   str[1024];
  int    success;

  char fln[256];
  strncpy(fln, fileName.c_str(), sizeof(fln));
  fh = MUD_openRead(fln, &type);
  if (fh == -1) {
    cerr << endl << "**ERROR** Couldn't open mud-file " << fileName << ", sorry." << endl;
    return 1;
  }

  cout << endl << "-------------------";
  cout << endl << "fileName = " << fileName << ", fileFormat = " << fileFormat;
  cout << endl << "-------------------";
  // run title
  success = MUD_getTitle( fh, str, sizeof(str) );
  if (success)
    cout << endl << "Run Title          : " << str;
  else
    cout << endl << "Run Title          : ???";
  // run number
  success = MUD_getRunNumber( fh, &val );
  if (success)
    cout << endl << "Run Number         : " << val;
  else
    cout << endl << "Run Number         : ???";
  // start time
  time_t tval;
  struct tm *dt;
  success = MUD_getTimeBegin( fh, (UINT32*)&tval );
  if (success) {
    dt = localtime((const time_t*)&tval);
    assert(dt);
    strftime(str, sizeof(str), "%F; %T", dt);
    cout << endl << "Run Start Time     : " << str;
  } else {
    cout << endl << "Run Start Time     : ???";
  }
  // stop time
  success = MUD_getTimeEnd( fh, (UINT32*)&tval );
  if (success) {
    dt = localtime((const time_t*)&tval);
    assert(dt);
    strftime(str, sizeof(str), "%F; %T", dt);
    cout << endl << "Run Stop Time      : " << str;
  } else {
    cout << endl << "Run Stop Time      : ???";
  }
  // laboratory
  success = MUD_getLab( fh, str, sizeof(str) );
  if (success)
    cout << endl << "Laboratory         : " << str;
  else
    cout << endl << "Laboratory         : ???";
  // instrument
  success = MUD_getApparatus( fh, str, sizeof(str) );
  if (success)
    cout << endl << "Instrument         : " << str;
  else
    cout << endl << "Instrument         : ???";
  // beamline
  success = MUD_getArea( fh, str, sizeof(str) );
  if (success)
    cout << endl << "Beamline           : " << str;
  else
    cout << endl << "Beamline           : ???";

  cout << endl << "Muon Beam Momentum : ???";
  cout << endl << "Muon Species       : positive muon?";
  cout << endl << "Muon Source        : ???";

  // method
  success = MUD_getMethod( fh, str, sizeof(str) );
  if (success)
    cout << endl << "Method             : " << str;
  else
    cout << endl << "Method             : ???";

  // sample
  success = MUD_getSample( fh, str, sizeof(str) );
  if (success)
    cout << endl << "Sample Name        : " << str;
  else
    cout << endl << "Sample Name        : ???";

  // orientation
  success = MUD_getOrient( fh, str, sizeof(str) );
  if (success)
    cout << endl << "Sample Orientation : " << str;
  else
    cout << endl << "Sample Orientation : ???";

  // temperature
  success = MUD_getTemperature( fh, str, sizeof(str) );
  if (success)
    cout << endl << "Sample Temperature : " << str;
  else
    cout << endl << "Sample Temperature : ???";

  // magnetic field
  success = MUD_getField( fh, str, sizeof(str) );
  if (success)
    cout << endl << "Sample Magn. Field : " << str;
  else
    cout << endl << "Sample Magn. Field : ???";

  // number of histograms
  int noHistos = 0;
  success = MUD_getHists(fh, &type, &val);
  if (success) {
    cout << endl << "No of Histos       : " << val;
    noHistos = val;
  } else {
    cout << endl << "No of Histos       : ???";
  }

  // time resolution
  REAL64 timeResolution = 0.0; // in seconds!!
  success = MUD_getHistSecondsPerBin( fh, 1, &timeResolution );
  timeResolution *= 1.0e9; // sec -> nsec
  if (success)
    cout << endl << "Time Resolution    : " << timeResolution << " ns";
  else
    cout << endl << "Time Resolution    : ??? ns";

  // detector related stuff
  for (int i=0; i<noHistos; i++) {
    cout << endl << "-------------------";
    cout << endl << "Histo No " << i+1;

    success = MUD_getHistTitle( fh, i+1, str, 1023);
    if (success)
      cout << endl << "Histo Name     : " << str;
    else
      cout << endl << "Histo Name     : ???";

    success = MUD_getHistNumBins( fh, i+1, &val );
    if (success)
      cout << endl << "Histo Length   : " << val;
    else
      cout << endl << "Histo Length   : ???";

    success = MUD_getHistT0_Bin( fh, i+1, &val );
    if (success)
      cout << endl << "Time Zero Bin  : " << val;
    else
      cout << endl << "Time Zero Bin  : ???";

    success = MUD_getHistGoodBin1( fh, i+1, &val );
    if (success)
      cout << endl << "First Good Bin : " << val;
    else
      cout << endl << "First Good Bin : ???";

    success = MUD_getHistGoodBin2( fh, i+1, &val );
    if (success)
      cout << endl << "Last Good Bin  : " << val;
    else
      cout << endl << "Last Good Bin  : ???";
  }

  cout << endl << "-------------------" << endl << endl;

  MUD_closeRead(fh);

  return 0;
}

//------------------------------------------------------------------------
/**
 *
 */
int dump_header_wkm(const string fileName, const string fileFormat)
{
  ifstream fin(fileName.c_str(), ifstream::in);
  if (!fin.is_open()) {
    cout << endl << "**ERROR** haven't found \"" << fileName << "\", will quit." << endl << endl;
    return 1;
  }
  cout << endl << "-------------------";
  cout << endl << "fileName = " << fileName << ", fileFormat = " << fileFormat;
  cout << endl << "-------------------";
  char header[256];
  while (fin.good()) {
    fin.getline(header, 256);
    if (strlen(header) == 0)
      break;
    cout << endl << header;
  }
  cout << endl << "-------------------" << endl << endl;
  fin.close();

  return 0;
}

//------------------------------------------------------------------------
/**
 *
 */
int main(int argc, char *argv[])
{
  if (argc == 1) {
    dump_header_syntax();
    return 0;
  }

  string fileName("");
  string fileFormat("");
  int count=0;
  for (int i=1; i<argc; i++) {
    if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
      dump_header_syntax();
      return 0;
    } else if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v")) {
#ifdef HAVE_CONFIG_H
      cout << endl << "dump_header: version " << PACKAGE_VERSION << ", git-rev: " << GIT_REVISION << endl << endl;
#else
      cout << endl << "dump_header: git-rev: " << GIT_REVISION << endl << endl;
#endif
      return 0;
    } else if (!strcmp(argv[i], "--file_format")) {
      if (i+1 >= argc) {
        cout << endl << "**ERROR** found '--file_format' without <fileFormat>!" << endl;
        dump_header_syntax();
        return 1;
      }
      string ff(argv[i+1]);
      if (!boost::iequals(ff, "MusrRoot") && !boost::iequals(ff, "NeXus") && !boost::iequals(ff, "ROOT") &&
          !boost::iequals(ff, "PSI-BIN") && !boost::iequals(ff, "PSI-MDU")  && !boost::iequals(ff, "MUD") &&
          !boost::iequals(ff, "WKM")) { // none of the listed found
        cout << endl << "**ERROR** found unsupported muSR file data format: " << argv[i+1] << endl;
        dump_header_syntax();
        return 1;
      }
      fileFormat = argv[i+1];
      i++;
    } else {
      count++;
      fileName = argv[i];
    }
  }

  // check if more then one file name was given
  if (count != 1) {
    cout << endl << "**ERROR** (only) a single file name is needed!" << endl;
    dump_header_syntax();
    return 1;
  }

  // if file format is not given explicitly try to guess it based on the file name extension
  if (fileFormat == "") {
    string fln(fileName);
    boost::to_lower(fln);
    if (fln.find(".root") != string::npos)
      fileFormat = "MusrRoot"; // could be old ROOT (LEM) as well
    else if (fln.find(".nxs") != string::npos)
      fileFormat = "NeXus";
    else if (fln.find(".bin") != string::npos)
      fileFormat = "PSI-BIN";
    else if (fln.find(".msr") != string::npos)
      fileFormat = "MUD";
    else if ((fln.find(".nemu") != string::npos) || (fln.find(".wkm") != string::npos))
      fileFormat = "WKM";

    if (fileFormat != "")
      cout << endl << "**INFO** the guessed file format is " << fileFormat << endl;
  }
  if (fileFormat == "") {
    cout << endl << "**ERROR** Couldn't guess your file format. You will need to provide it explicitly" << endl << endl;
    return 1;
  }

  boost::to_lower(fileFormat);

  if (boost::iequals(fileFormat, "MusrRoot") || boost::iequals(fileFormat, "ROOT")) {
    dump_header_root(fileName, fileFormat);
  } else if (boost::iequals(fileFormat, "NeXus")) {
#ifdef PNEXUS_ENABLED
    dump_header_nexus(fileName);
#else
    cout << endl << "Sorry, NeXus is not enabled, hence I cannot help you." << endl;
#endif
  } else if (boost::iequals(fileFormat, "PSI-BIN") || boost::iequals(fileFormat, "PSI-MDU")) {
    dump_header_psi_bin(fileName, fileFormat);
  } else if (boost::iequals(fileFormat, "MUD")) {
    dump_header_mud(fileName, fileFormat);
  } else if (boost::iequals(fileFormat, "WKM")) {
    dump_header_wkm(fileName, fileFormat);
  }

  return 0;
}
