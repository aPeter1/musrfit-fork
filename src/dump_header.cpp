/***************************************************************************

  dump_header.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2023 by Andreas Suter                              *
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

#include <unistd.h>
#include <cerrno>
#include <cctype>
#include <cstring>
#include <ctime>
#include <cassert>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <TObject.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TFile.h>
#include <TFolder.h>
#include <TString.h>
#include <TFolder.h>
#include <TH1F.h>

#ifdef HAVE_GIT_REV_H
#include "git-revision.h"
#endif

#include "PFindRun.h"
#include "PStartupHandler.h"
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
 * <p>dump help to stdout.
 */
void dump_header_syntax()
{
  std::cout << std::endl << "usage: dump_header [-rn <runNo> | -fn <fileName>] [-ff, --fileFormat <fileFormat>]";
  std::cout << std::endl << "                   [-y, --year <year>] [-s, --summary] [-i, --instrument <inst>]";
  std::cout << std::endl << "                   [-c, --counts] | ";
  std::cout << std::endl << "                   --help | --version";
  std::cout << std::endl;
  std::cout << std::endl << "       Dumps the header information of a given muSR data file onto the standard output.";
  std::cout << std::endl << "       If no <fileFormat> info is povided, it will try to guess what <fileFormat> it might be.";
  std::cout << std::endl << "       For <runNo> guessing of the file format is not possible. The default assumption here is 'MusrRoot'.";
  std::cout << std::endl;
  std::cout << std::endl << "       -rn, --runNo <runNo> : run number of the header to be dumped.";
  std::cout << std::endl << "       -fn, --fileName <fileName> : muSR data file name.";
  std::cout << std::endl << "       -ff, --fileFormat <fileFormat> : where <fileFormat> can be:";
  std::cout << std::endl << "                     MusrRoot (default), NeXus, ROOT (old LEM), PSI-BIN, PSI-MDU, MUD, WKM";
  std::cout << std::endl << "                     NeXus is only supported if enabled.";
  std::cout << std::endl << "       -y, --year <year> : <year> has to be 4 digit, e.g. 2005, if provided it is used to";
  std::cout << std::endl << "                     generate the file name for the given <runNo>, otherwise the current";
  std::cout << std::endl << "                     year is used. If a file name is given, this option has no effect.";
  std::cout << std::endl << "       -s, --summary : this option is used for LE-uSR data sets only. It will, additionally";
  std::cout << std::endl << "                     to the header information, print the summary file content.";
  std::cout << std::endl << "       -i, --instrument <inst> : where <inst> is the requested instrument:";
  std::cout << std::endl << "                     lem (default) | gps | ltf | dolly | gpd | hifi.";
  std::cout << std::endl << "       -c, --counts  : will show detector counts as well.";
  std::cout << std::endl << "       -h, --help    : will show this help";
  std::cout << std::endl << "       -v, --version : will show the current version.";
  std::cout << std::endl << std::endl;
}

//------------------------------------------------------------------------
/**
 * <p>dumps header of a ROOT file.
 *
 * @param fileName file name of the ROOT file
 * @param summary bool, if true dump the summary
 * @param counts bool, if true dump detector counts
 *
 * @return 0 on success, otherwise 1.
 */
int dump_header_root(const std::string fileName, const bool summary, const bool counts)
{
  TFile f(fileName.c_str());
  if (f.IsZombie()) {
    std::cerr << std::endl;
    std::cerr << "**ERROR** couldn't invoke ROOT/MusrRoot file object." << std::endl;
    std::cerr << std::endl;
    return 1;
  }

  UInt_t fileType = DH_MUSR_ROOT;
  UInt_t noOfHistos{0};
  PIntVector redGreenOffset;

  TFolder *folder;
  f.GetObject("RunInfo", folder); // try first LEM-ROOT style file (used until 2011).
  if (!folder) { // either something is wrong, or it is a MusrRoot file
    f.GetObject("RunHeader", folder);
    if (!folder) { // something is wrong!!
      std::cerr << std::endl << "**ERROR** Couldn't neither obtain RunInfo (LEM), nor RunHeader (MusrRoot) from " << fileName << std::endl;
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
      std::cerr << std::endl << "**ERROR** Couldn't obtain run header info from ROOT file " << fileName << std::endl;
      f.Close();
      return 1;
    }

    std::cout << std::endl << "-------------------";
    std::cout << std::endl << "fileName = " << fileName << ", fileFormat = ROOT (PSI LEM).";
    std::cout << std::endl << "-------------------";
    std::cout << std::endl << "Run Title          : " << runHeader->GetRunTitle().GetString().Data();
    std::cout << std::endl << "Run Number         : " << runHeader->GetRunNumber();
    std::cout << std::endl << "Run Start Time     : " << runHeader->GetStartTimeString().GetString().Data();
    std::cout << std::endl << "Run Stop Time      : " << runHeader->GetStopTimeString().GetString().Data();
    std::cout << std::endl << "Laboratory         : PSI";
    std::cout << std::endl << "Instrument         : LEM";
    std::cout << std::endl << "Beamline           : muE4";
    std::cout << std::endl << "Muon Beam Momentum : 28 MeV/c";
    std::cout << std::endl << "Muon Species       : positive muons";
    std::cout << std::endl << "Muon Source        : Target E - slow muons";
    std::cout << std::endl << "Setup              : " << runHeader->GetLemSetup().GetString().Data();
    std::cout << std::endl << "Comment            : n/a";
    std::cout << std::endl << "Sample Name        : n/a";
    std::cout << std::endl << "Sample Orientation : n/a";
    std::cout << std::endl << "Sample Temperature : " << runHeader->GetSampleTemperature() << "+-" << runHeader->GetSampleTemperatureError() << " K";
    std::cout << std::endl << "Sample Mag. Field  : " << runHeader->GetSampleBField() << "+-" << runHeader->GetSampleBFieldError() << " G";
    std::cout << std::endl << "No of Histos       : " << runHeader->GetNHist();
    std::cout << std::endl << "Time Resolution    : " << runHeader->GetTimeResolution() << " ns";
    std::cout << std::endl << "-------------------";
    std::cout << std::endl << "LEM Specific Entries :";
    std::cout << std::endl << "Moderator          : " << runHeader->GetModerator().GetString().Data();
    std::cout << std::endl << "Moderator HV       : " << runHeader->GetModeratorHV() << " kV";
    std::cout << std::endl << "Sample HV          : " << runHeader->GetSampleHV() << " kV";
    std::cout << std::endl << "Impl. Energy       : " << runHeader->GetImpEnergy() << " keV";
    std::cout << std::endl << "-------------------";
    std::cout << std::endl << "Detector Info (for all detectors the same): ";
    std::cout << std::endl << "-------------------";
    std::cout << std::endl << "Histo Length       : " << runHeader->GetNChannels();
    double *timeZero;
    timeZero = runHeader->GetTimeZero();
    std::cout << std::endl << "Time Zero Bin      : " << timeZero[0];
    std::cout << std::endl << "First Good Bin     : " << timeZero[0];
    std::cout << std::endl << "Last Good Bin      : " << runHeader->GetNChannels()-1;
    std::cout << std::endl << "-------------------" << std::endl << std::endl;

    delete runHeader;
  } else { // MusrRoot
    // invoke the MusrRoot header object
    TMusrRunHeader *header = new TMusrRunHeader(fileName.c_str(), true); // read quite
    if (header == 0) {
      std::cerr << std::endl << "**ERROR** Couldn't invoke MusrRoot RunHeader in file:" << fileName;
      std::cerr << std::endl;
      f.Close();
      return 1;
    }

    // try to populate the MusrRoot header object
    if (!header->ExtractAll(folder)) {
      std::cerr << std::endl << "**ERROR** Couldn't invoke MusrRoot RunHeader in file:" << fileName;
      std::cerr << std::endl;
      f.Close();
      return 1;
    }

    header->DumpHeader();

    if (counts) {
      bool ok;
      Int_t ival;
      PIntVector ivec;
      header->Get("RunInfo/No of Histos", ival, ok);
      if (ok)
        noOfHistos = ival;
      header->Get("RunInfo/RedGreen Offsets", ivec, ok);
      if (ok)
        redGreenOffset = ivec;
    }

    delete header;
  }

  // summary as well?
  if (summary && (fileType == DH_MUSR_ROOT)) {
    TObjArray *runSum=nullptr;
    runSum = static_cast<TObjArray*>(folder->FindObject("RunSummary"));
    if (!runSum) { // something is wrong!!
      std::cerr << std::endl << "**ERROR** Couldn't obtain RunSummary " << fileName << std::endl;
      f.Close();
      return 1;
    }
    std::cout << "++++++++++++++++++++" << std::endl;
    std::cout << " Run Summary" << std::endl;
    std::cout << "++++++++++++++++++++" << std::endl;
    TObjString *tstr;
    TString str;
    for (Int_t i=0; i<runSum->GetEntries(); i++) {
      tstr = static_cast<TObjString*>(runSum->At(i));
      str = tstr->String();
      std::cout << str;
    }
  }

  // detector counts as well?
  if (counts && (fileType == DH_MUSR_ROOT)) {
    // dump the detector counts
    std::cout << "Detector counts" << std::endl;
    f.GetObject("histos", folder);
    if (folder != nullptr) {
      char detectorLabel[64];
      TH1F *histo{nullptr};
      UInt_t total{0};
      for (UInt_t i=0; i<redGreenOffset.size(); i++) {
        std::cout << "  Group " << i+1 << " (Offset=" << redGreenOffset[i] << ") : " << std::endl;
        total = 0;
        for (UInt_t j=0; j<noOfHistos; j++) {
          snprintf(detectorLabel, sizeof(detectorLabel), "hDecay%03d", redGreenOffset[i]+j+1);
          histo = (TH1F*) folder->FindObjectAny(detectorLabel);
          if (histo != nullptr) {
            std::cout << "    " << histo->GetTitle() << ":\t " << histo->GetEntries() << std::endl;
            total += histo->GetEntries();
          }
        }
        if (i % 2 == 0)
          std::cout << "    total counts of group " << i+1 << ":\t\t\t " << total << std::endl;
        else
          std::cout << "    total counts of group " << i+1 << ":\t\t\t\t\t " << total << std::endl;
      }
    } else {
      std::cout << "Sorry, no histos folder found" << std::endl;
      f.Close();
      return 0;
    }
  }

  f.Close();

  return 0;
}

//------------------------------------------------------------------------
/**
 * <p>dumps the header information of a NeXus file.
 *
 * @param fileName file name of the NeXus file.
 * @param counts bool, if true dump detector counts
 *
 * @return  0 on success, 1 otherwise
 */
int dump_header_nexus(const std::string fileName, const bool counts) {

#ifdef PNEXUS_ENABLED
  PNeXus *nxs_file = new PNeXus(fileName.c_str());

  if (nxs_file == nullptr) {
    std::cerr << std::endl;
    std::cerr << "**ERROR** couldn't invoke NeXus file object." << std::endl;
    std::cerr << std::endl;
    return 1;
  }

  if (nxs_file->IsValid(false)) {
    nxs_file->Dump(counts);
  } else {
    std::cerr << std::endl;
    std::cerr << "**ERROR** found invalid NeXus file." << std::endl;
    std::cerr << std::endl;
    return 1;
  }

  if (nxs_file)
    delete nxs_file;
#else
  std::cout << std::endl << "NeXus not enabled, hence the header information cannot be dumped." << std::endl << std::endl;
#endif

  return 0;
}

//------------------------------------------------------------------------
/**
 * <p>dump the instrument specific info for the PSI-BIN format.
 *
 * @param fileName file name of the PSI-BIN.
 *
 * @return string vector with the instrument specific info.
 */
std::vector<std::string> dump_header_instrument_info(std::string fileName)
{
  std::vector<std::string> result;
  std::string fln(fileName);
  boost::to_lower(fln);

  if ((fln.find(".bin") != std::string::npos) || (fln.find(".mdu") != std::string::npos)) { // PSI-BIN or PSI-MDU format
    if (fln.find("_gps_") != std::string::npos) {
      result.push_back("GPS");
      result.push_back("piM3.2");
      result.push_back("28 MeV/c");
      result.push_back("likely to be positive muons");
      result.push_back("Target M");
    } else if (fln.find("_ltf_") != std::string::npos) {
      result.push_back("LTF");
      result.push_back("piM3.1");
      result.push_back("28 MeV/c");
      result.push_back("likely to be positive muons");
      result.push_back("Target M");
    } else if (fln.find("_dolly_") != std::string::npos) {
      result.push_back("DOLLY");
      result.push_back("piE1");
      result.push_back("28 MeV/c");
      result.push_back("likely to be positive muons");
      result.push_back("Target E");
    } else if (fln.find("_alc_") != std::string::npos) {
      result.push_back("ALC");
      result.push_back("piE3");
      result.push_back("28 MeV/c");
      result.push_back("likely to be positive muons");
      result.push_back("Target E");
    } else if (fln.find("_hifi_") != std::string::npos) {
      result.push_back("HAL9500");
      result.push_back("piE3");
      result.push_back("28 MeV/c");
      result.push_back("likely to be positive muons");
      result.push_back("Target E");
    } else if (fln.find("_gpd_") != std::string::npos) {
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
 * <p>dump the header information of a PSI-BIN file.
 *
 * @param fileName file name of the PSI-BIN
 * @param fileFormat either PSI-BIN or PSI-MDU
 * @param counts bool, if true dump detector counts
 *
 * @return 0 on success, 1 otherwise
 */
int dump_header_psi_bin(const std::string fileName, const std::string fileFormat, const bool counts)
{
  MuSR_td_PSI_bin psiBin;
  int status;
  bool success = false;

  // read psi bin file
  status = psiBin.Read(fileName.c_str());
  switch (status) {
    case 0: // everything perfect
      success = true;
      break;
    case 1: // couldn't open file, or failed while reading the header
      std::cout << std::endl << "**ERROR** couldn't open psi-bin file, or failed while reading the header." << std::endl;
      success = false;
      break;
    case 2: // unsupported version of the data
      std::cout << std::endl << "**ERROR** psi-bin file: unsupported version of the data." << std::endl;
      success = false;
      break;
    case 3: // error when allocating data buffer
      std::cout << std::endl << "**ERROR** psi-bin file: error when allocating data buffer." << std::endl;
      success = false;
      break;
    case 4: // number of histograms/record not equals 1
      std::cout << std::endl << ">> **ERROR** psi-bin file: number of histograms/record not equals 1." << std::endl;
      success = false;
      break;
    default: // you never should have reached this point
      std::cout << std::endl << ">> **ERROR** psi-bin file: no clue why you reached this point." << std::endl;
      success = false;
      break;
  }

  // if any reading error happend, get out of here
  if (!success)
    return 1;

  std::vector<std::string> vstr;
  std::vector<double> dVal, dErrVal;
  std::cout << std::endl << "-------------------";
  std::cout << std::endl << "fileName = " << fileName << ", fileFormat = " << fileFormat;
  std::cout << std::endl << "-------------------";
  std::cout << std::endl << "Run Title          : " << psiBin.GetComment();
  std::cout << std::endl << "Run Number         : " << psiBin.GetRunNumberInt();
  vstr = psiBin.GetTimeStartVector();
  if (vstr.size() < 2) {
    std::cout << std::endl << "**ERROR** couldn't obtain \"Run Start Time\" will quit." << std::endl << std::endl;
    return 1;
  }
  std::cout << std::endl << "Run Start Time     : " << vstr[0] << "; " << vstr[1];
  vstr = psiBin.GetTimeStopVector();
  if (vstr.size() < 2) {
    std::cout << std::endl << "**ERROR** couldn't obtain \"Run Stop Time\" will quit." << std::endl << std::endl;
    return 1;
  }
  std::cout << std::endl << "Run Stop Time      : " << vstr[0] << "; " << vstr[1];
  std::cout << std::endl << "Laboratory         : PSI";
  vstr = dump_header_instrument_info(fileName);
  if (vstr.size() < 5) {
    std::cout << std::endl << "**ERROR** couldn't obtain \"Instrument\" will quit." << std::endl << std::endl;
    return 1;
  }
  std::cout << std::endl << "Instrument         : " << vstr[0];
  std::cout << std::endl << "Beamline           : " << vstr[1];
  std::cout << std::endl << "Muon Beam Momentum : " << vstr[2];
  std::cout << std::endl << "Muon Species       : " << vstr[3];
  std::cout << std::endl << "Muon Source        : " << vstr[4];
  std::cout << std::endl << "Setup              : " << psiBin.GetComment();
  std::cout << std::endl << "Comment            : n/a";
  std::cout << std::endl << "Sample Name        : " << psiBin.GetSample();
  std::cout << std::endl << "Sample Orientation : " << psiBin.GetOrient();
  dVal = psiBin.GetTemperaturesVector();
  dErrVal = psiBin.GetDevTemperaturesVector();
  if (dVal.size() != dErrVal.size()) {
    std::cout << std::endl << "Sample Temperature : " << psiBin.GetTemp();
  } else {
    for (unsigned int i=0; i<dVal.size(); i++) {
      std::cout << std::endl << "Sample Temp. " << i+1 << "     : " << dVal[i] << " (" << dErrVal[i] << ") K";
    }
  }
  std::cout << std::endl << "Sample Mag. Field  : " << psiBin.GetField();
  std::cout << std::endl << "No of Histos       : " << psiBin.GetNumberHistoInt();
  std::cout << std::endl << "Time Resolution    : " << psiBin.GetBinWidthNanoSec() << " ns";
  for (int i=0; i<psiBin.GetNumberHistoInt(); i++) {
    std::cout << std::endl << "-------------------";
    std::cout << std::endl << "Histo No           : " << i;
    std::cout << std::endl << "Histo Name         : " << psiBin.GetNameHisto(i);
    std::cout << std::endl << "Histo Length       : " << psiBin.GetHistoLengthBin();
    std::cout << std::endl << "Time Zero Bin      : " << psiBin.GetT0Int(i);
    std::cout << std::endl << "First Good Bin     : " << psiBin.GetFirstGoodInt(i);
    std::cout << std::endl << "Last Good Bin      : " << psiBin.GetLastGoodInt(i);
    if (counts)
      std::cout << std::endl << "No of Events       : " << psiBin.GetEventsHistoLong(i);
  }
  std::cout << std::endl << "-------------------" << std::endl << std::endl;

  return 0;
}

//------------------------------------------------------------------------
/**
 * <p>dump the header information of a MUD file.
 * @param fileName file name of the MUD file
 * @param counts bool, if true dump detector counts
 *
 * @return 0 on success, 1 otherwise
 */
int dump_header_mud(const std::string fileName, const bool counts)
{
  int    fh;
  UINT32 type, val;
  char   str[1024];
  int    success;

  char fln[256];
  strncpy(fln, fileName.c_str(), sizeof(fln));
  fh = MUD_openRead(fln, &type);
  if (fh == -1) {
    std::cerr << std::endl << "**ERROR** Couldn't open mud-file " << fileName << ", sorry." << std::endl;
    return 1;
  }

  std::cout << std::endl << "-------------------";
  std::cout << std::endl << "fileName = " << fileName << ", fileFormat = MUD";
  std::cout << std::endl << "-------------------";
  // run title
  success = MUD_getTitle( fh, str, sizeof(str) );
  if (success)
    std::cout << std::endl << "Run Title          : " << str;
  else
    std::cout << std::endl << "Run Title          : ???";
  // run number
  success = MUD_getRunNumber( fh, &val );
  if (success)
    std::cout << std::endl << "Run Number         : " << val;
  else
    std::cout << std::endl << "Run Number         : ???";
  // start time
  time_t tval;
  struct tm *dt;
  success = MUD_getTimeBegin( fh, &val );
  tval = static_cast<time_t>(val);
  if (success) {
    dt = localtime(static_cast<const time_t*>(&tval));
    assert(dt);
    strftime(str, sizeof(str), "%F; %T", dt);
    std::cout << std::endl << "Run Start Time     : " << str;
  } else {
    std::cout << std::endl << "Run Start Time     : ???";
  }
  // stop time
  success = MUD_getTimeEnd( fh, &val );
  tval = static_cast<time_t>(val);
  if (success) {
    dt = localtime(static_cast<const time_t*>(&tval));
    assert(dt);
    strftime(str, sizeof(str), "%F; %T", dt);
    std::cout << std::endl << "Run Stop Time      : " << str;
  } else {
    std::cout << std::endl << "Run Stop Time      : ???";
  }
  // laboratory
  success = MUD_getLab( fh, str, sizeof(str) );
  if (success)
    std::cout << std::endl << "Laboratory         : " << str;
  else
    std::cout << std::endl << "Laboratory         : ???";
  // instrument
  success = MUD_getApparatus( fh, str, sizeof(str) );
  if (success)
    std::cout << std::endl << "Instrument         : " << str;
  else
    std::cout << std::endl << "Instrument         : ???";
  // beamline
  success = MUD_getArea( fh, str, sizeof(str) );
  if (success)
    std::cout << std::endl << "Beamline           : " << str;
  else
    std::cout << std::endl << "Beamline           : ???";

  std::cout << std::endl << "Muon Beam Momentum : ???";
  std::cout << std::endl << "Muon Species       : positive muon?";
  std::cout << std::endl << "Muon Source        : ???";

  // method
  success = MUD_getMethod( fh, str, sizeof(str) );
  if (success)
    std::cout << std::endl << "Method             : " << str;
  else
    std::cout << std::endl << "Method             : ???";

  // sample
  success = MUD_getSample( fh, str, sizeof(str) );
  if (success)
    std::cout << std::endl << "Sample Name        : " << str;
  else
    std::cout << std::endl << "Sample Name        : ???";

  // orientation
  success = MUD_getOrient( fh, str, sizeof(str) );
  if (success)
    std::cout << std::endl << "Sample Orientation : " << str;
  else
    std::cout << std::endl << "Sample Orientation : ???";

  // temperature
  success = MUD_getTemperature( fh, str, sizeof(str) );
  if (success)
    std::cout << std::endl << "Sample Temperature : " << str;
  else
    std::cout << std::endl << "Sample Temperature : ???";

  // magnetic field
  success = MUD_getField( fh, str, sizeof(str) );
  if (success)
    std::cout << std::endl << "Sample Magn. Field : " << str;
  else
    std::cout << std::endl << "Sample Magn. Field : ???";

  // number of histograms
  int noHistos = 0;
  success = MUD_getHists(fh, &type, &val);
  if (success) {
    std::cout << std::endl << "No of Histos       : " << val;
    noHistos = val;
  } else {
    std::cout << std::endl << "No of Histos       : ???";
  }

  // time resolution
  REAL64 timeResolution = 0.0; // in seconds!!
  success = MUD_getHistSecondsPerBin( fh, 1, &timeResolution );
  timeResolution *= 1.0e9; // sec -> nsec
  if (success)
    std::cout << std::endl << "Time Resolution    : " << timeResolution << " ns";
  else
    std::cout << std::endl << "Time Resolution    : ??? ns";

  // detector related stuff
  for (int i=0; i<noHistos; i++) {
    std::cout << std::endl << "-------------------";
    std::cout << std::endl << "Histo No " << i+1;

    success = MUD_getHistTitle( fh, i+1, str, 1023);
    if (success)
      std::cout << std::endl << "Histo Name     : " << str;
    else
      std::cout << std::endl << "Histo Name     : ???";

    success = MUD_getHistNumBins( fh, i+1, &val );
    if (success)
      std::cout << std::endl << "Histo Length   : " << val;
    else
      std::cout << std::endl << "Histo Length   : ???";

    success = MUD_getHistT0_Bin( fh, i+1, &val );
    if (success)
      std::cout << std::endl << "Time Zero Bin  : " << val;
    else
      std::cout << std::endl << "Time Zero Bin  : ???";

    success = MUD_getHistGoodBin1( fh, i+1, &val );
    if (success)
      std::cout << std::endl << "First Good Bin : " << val;
    else
      std::cout << std::endl << "First Good Bin : ???";

    success = MUD_getHistGoodBin2( fh, i+1, &val );
    if (success)
      std::cout << std::endl << "Last Good Bin  : " << val;
    else
      std::cout << std::endl << "Last Good Bin  : ???";
    if (counts) {
      success = MUD_getHistNumEvents( fh, i+1, &val );
      if (success) {
        std::cout << std::endl << "#Events        : " << val;
      }
    }
  }

  std::cout << std::endl << "-------------------" << std::endl << std::endl;

  MUD_closeRead(fh);

  return 0;
}

//------------------------------------------------------------------------
/**
 * <p>dump the header information of a WKM file.
 * @param fileName file name of the WKM file.
 *
 * @return 0 on success, 1 otherwise
 */
int dump_header_wkm(const std::string fileName)
{
  std::ifstream fin(fileName.c_str(), std::ifstream::in);
  if (!fin.is_open()) {
    std::cout << std::endl << "**ERROR** haven't found \"" << fileName << "\", will quit." << std::endl << std::endl;
    return 1;
  }
  std::cout << std::endl << "-------------------";
  std::cout << std::endl << "fileName = " << fileName << ", fileFormat = WKM";
  std::cout << std::endl << "-------------------";
  char header[256];
  while (fin.good()) {
    fin.getline(header, 256);
    if (strlen(header) == 0)
      break;
    std::cout << std::endl << header;
  }
  std::cout << std::endl << "-------------------" << std::endl << std::endl;
  fin.close();

  return 0;
}

//------------------------------------------------------------------------
/**
 * <p>checks if a string is a number
 *
 * @param s number string to be checked
 *
 * @return true if 's' is a number, false otherwise
 */
bool dump_is_number(const char *s)
{
  int i=0;

  if (s == nullptr) // make sure it is not a null pointer
    return false;

  while (isdigit(s[i]))
    i++;

  if (s[i] == '\0')
    return true;
  else
    return false;
}

//------------------------------------------------------------------------
/**
 * <p>reads the current year from the system and converts it to a string.
 *
 * @return the current year as a string.
 */
int dump_current_year()
{
  time_t rawtime;
  struct tm *timeinfo;
  char buffer[32];

  time (&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, 32, "%Y", timeinfo);

  return atoi(buffer);
}

//------------------------------------------------------------------------
/**
 * <p>dump_header allows to dump the header (meta) information for various
 * muSR specific file formats.
 *
 * @param argc argument count
 * @param argv argument list
 *
 * @return 0 on success, 1 otherwise
 */
int main(int argc, char *argv[])
{
  if (argc == 1) {
    dump_header_syntax();
    return 0;
  }

  std::string runNo("");
  std::string fileName("");
  std::string fileFormat("");
  std::string year("");
  std::string instrument("lem");
  bool summary(false);
  bool counts(false);

  for (int i=1; i<argc; i++) {
    if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
      dump_header_syntax();
      return 0;
    } else if (!strcmp(argv[i], "--version") || !strcmp(argv[i], "-v")) {
#ifdef HAVE_CONFIG_H
#ifdef HAVE_GIT_REV_H
      std::cout << std::endl << "dump_header version: " << PACKAGE_VERSION << ", git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << " (" << BUILD_TYPE << "), ROOT version: " << ROOT_VERSION_USED << std::endl << std::endl;
#else
      std::cout << std::endl << "dump_header version: " << PACKAGE_VERSION << " (" << BUILD_TYPE << "), ROOT version: " << ROOT_VERSION_USED << std::endl << std::endl;
#endif
#else
#ifdef HAVE_GIT_REV_H
      std::cout << std::endl << "dump_header git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << std::endl << std::endl;
#else
      std::cout << std::endl << "dump_header version: unknown" << std::endl << std::endl;
#endif
#endif
      return 0;
    } else if (!strcmp(argv[i], "-rn") || !strcmp(argv[i], "--runNo")) {
      if (i+1 >= argc) {
        std::cerr << std::endl << "**ERROR** found -rn, --runNo without <runNo>!" << std::endl;
        dump_header_syntax();
        return 1;
      }
      // make sure there is only one 'legal' run number
      int count = 1;
      while (dump_is_number(argv[i+count]) && (i+count < argc))
        count++;
      // make sure there is one and only one run number given
      if (count == 1) {
        std::cerr << std::endl << "**ERROR** found -rn, --runNo without <runNo>, or the provided <runNo> ('" << argv[i+1] << "') is not a number!" << std::endl;
        dump_header_syntax();
        return 1;
      }
      if (count > 2) {
        std::cerr << std::endl << "**ERROR** found -rn, --runNo with more than one <runNo>! This is not yet supported." << std::endl;
        dump_header_syntax();
        return 1;
      }
      runNo = argv[i+1];
      i++;
    } else if (!strcmp(argv[i], "-fn") || !strcmp(argv[i], "--fileName")) {
      if (i+1 >= argc) {
        std::cerr << std::endl << "**ERROR** found -fn, --fileName without <fileName>!" << std::endl;
        dump_header_syntax();
        return 1;
      }
      fileName = argv[i+1];
      i++;
    } else if (!strcmp(argv[i], "--fileFormat") || !strcmp(argv[i], "-ff")) {
      if (i+1 >= argc) {
        std::cerr << std::endl << "**ERROR** found -ff, --fileFormat without <fileFormat>!" << std::endl;
        dump_header_syntax();
        return 1;
      }
      std::string ff(argv[i+1]);
      if (!boost::iequals(ff, "MusrRoot") && !boost::iequals(ff, "NeXus") && !boost::iequals(ff, "ROOT") &&
          !boost::iequals(ff, "PSI-BIN") && !boost::iequals(ff, "PSI-MDU")  && !boost::iequals(ff, "MUD") &&
          !boost::iequals(ff, "WKM")) { // none of the listed found
        std::cerr << std::endl << "**ERROR** found unsupported muSR file data format: " << argv[i+1] << std::endl;
        dump_header_syntax();
        return 1;
      }
      fileFormat = argv[i+1];
      i++;
    } else if (!strcmp(argv[i], "-y") || !strcmp(argv[i], "--year")) {
      if (i+1 >= argc) {
        std::cerr << std::endl << "**ERROR** found -y, --year without <year>!" << std::endl;
        dump_header_syntax();
        return 1;
      }
      if (!dump_is_number(argv[i+1])) {
        std::cerr << std::endl << "**ERROR** found -y, --year with sensless <year> '" << argv[i+1] << "'!" << std::endl;
        dump_header_syntax();
        return 1;
      }
      int yy = static_cast<int>(strtod(argv[i+1], static_cast<char**>(nullptr)));
      if ((yy < 1950) || (yy > dump_current_year())) {
        std::cerr << std::endl << "**ERROR** found -y, --year with <year> '" << yy << "'!";
        std::cerr << std::endl << "     Well, cannot handle files in the pre-muSR time nor in the future." << std::endl;
        dump_header_syntax();
        return 1;
      }
      year = argv[i+1];
      i++;
    } else if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--summary")) {
      summary = true;
    } else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--counts")) {
      counts = true;
    } else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--instrument")) {
      if (i+1 >= argc) {
        std::cerr << std::endl << "**ERROR** found option --instrument without <instrument> input!" << std::endl;
        dump_header_syntax();
        return 1;
      }
      if (strcmp(argv[i+1], "lem") && strcmp(argv[i+1], "gps") && strcmp(argv[i+1], "ltf") &&
          strcmp(argv[i+1], "dolly") && strcmp(argv[i+1], "gpd") && strcmp(argv[i+1], "hifi")) {
        std::cerr << std::endl << "**ERROR** found --instrument with unkown instrument name: '" << argv[i+1] << "'!" << std::endl;
        dump_header_syntax();
        return 1;
      }
      instrument = argv[i+1];
      i++;
    } else {
      std::cerr << std::endl << "**ERROR** found unkown option '" << argv[i] << "'." << std::endl;
      dump_header_syntax();
      return 1;
    }
  }

  // if year is not provided, take the current one
  if (year.empty()) {
    std::stringstream ss;
    ss << dump_current_year();
    year = ss.str();
  }

  if ((runNo.length() != 0) && (fileName.length() != 0)) {
    std::cerr << std::endl << "**ERROR** currently only either runNo or fileName can be handled, not both simultanously." << std::endl;
    return 1;
  }

  // invoke the startup handler in order to get the default search paths to the data files
  // read startup file
  char startup_path_name[128];
  TSAXParser *saxParser = new TSAXParser();
  PStartupHandler *startupHandler = new PStartupHandler();
  if (!startupHandler->StartupFileFound()) {
    std::cerr << std::endl << ">> musrfit **WARNING** couldn't find " << startupHandler->GetStartupFilePath().Data();
    std::cerr << std::endl;
    // clean up
    if (saxParser) {
      delete saxParser;
      saxParser = nullptr;
    }
    if (startupHandler) {
      delete startupHandler;
      startupHandler = nullptr;
    }
  } else {
    strcpy(startup_path_name, startupHandler->GetStartupFilePath().Data());
    saxParser->ConnectToHandler("PStartupHandler", startupHandler);
    //status = saxParser->ParseFile(startup_path_name);
    // parsing the file as above seems to lead to problems in certain environments;
    // use the parseXmlFile function instead (see PStartupHandler.cpp for the definition)
    int status = parseXmlFile(saxParser, startup_path_name);
    // check for parse errors
    if (status) { // error
      std::cerr << std::endl << ">> musrfit **WARNING** Reading/parsing musrfit_startup.xml failed.";
      std::cerr << std::endl;
      // clean up
      if (saxParser) {
        delete saxParser;
        saxParser = nullptr;
      }
      if (startupHandler) {
        delete startupHandler;
        startupHandler = nullptr;
      }
    }
  }

  // try to find path-file-name via run name templates
  std::string pathFln("");
  if (fileName == "") { // only look for runs if the file name is not explicitly given
    int yy = static_cast<int>(strtod(year.c_str(), static_cast<char**>(nullptr)));
    int run = static_cast<int>(strtod(runNo.c_str(), static_cast<char**>(nullptr)));
    PFindRun findRun(startupHandler->GetDataPathList(), startupHandler->GetRunNameTemplateList(), instrument, yy, run);
    if (findRun.FoundPathName()) {
      pathFln = findRun.GetPathName().Data();
    } else {
      std::cout << "debug> Couldn't find run: " << run << " for instrument " << instrument << ", and year: " << year << std::endl;
      return 1;
    }
  } else { // file name explicitly provided, hence use this as pathFln
    pathFln = fileName;
  }

  // if file format is not given explicitly try to guess it based on the file name extension
  if (fileFormat == "") {
    std::string fln(pathFln);
    boost::to_lower(fln);
    if (fln.find(".root") != std::string::npos)
      fileFormat = "MusrRoot"; // could be old ROOT (LEM) as well
    else if (fln.find(".nxs") != std::string::npos)
      fileFormat = "NeXus";
    else if (fln.find(".bin") != std::string::npos)
      fileFormat = "PSI-BIN";
    else if (fln.find(".mdu") != std::string::npos)
      fileFormat = "PSI-MDU";
    else if (fln.find(".msr") != std::string::npos)
      fileFormat = "MUD";
    else if ((fln.find(".nemu") != std::string::npos) || (fln.find(".wkm") != std::string::npos))
      fileFormat = "WKM";

    if (fileFormat != "")
      std::cout << std::endl << "**INFO** the guessed file format is " << fileFormat << std::endl;
  }
  if (fileFormat == "") {
    std::cout << std::endl << "**ERROR** Couldn't guess your file format. You will need to provide it explicitly" << std::endl << std::endl;
    return 1;
  }

  boost::to_lower(fileFormat);

  if (boost::iequals(fileFormat, "MusrRoot") || boost::iequals(fileFormat, "ROOT")) {
    dump_header_root(pathFln, summary, counts);
  } else if (boost::iequals(fileFormat, "NeXus")) {
#ifdef PNEXUS_ENABLED
    dump_header_nexus(pathFln, counts);
#else
    std::cout << std::endl << "Sorry, NeXus is not enabled, hence I cannot help you." << std::endl;
#endif
  } else if (boost::iequals(fileFormat, "PSI-BIN") || boost::iequals(fileFormat, "PSI-MDU")) {
    dump_header_psi_bin(pathFln, fileFormat, counts);
  } else if (boost::iequals(fileFormat, "MUD")) {
    dump_header_mud(pathFln, counts);
  } else if (boost::iequals(fileFormat, "WKM")) {
    dump_header_wkm(pathFln);
  }

  // cleanup
  if (saxParser) {
    delete saxParser;
    saxParser = nullptr;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = nullptr;
  }

  return 0;
}
