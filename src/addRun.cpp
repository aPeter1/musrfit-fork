/***************************************************************************

  addRun.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2022 by Andreas Suter                              *
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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include <TString.h>

#ifdef HAVE_GIT_REV_H
#include "git-revision.h"
#endif

#include "PMusr.h"
#include "PStartupHandler.h"
#include "PRunDataHandler.h"
#include "PFindRun.h"

struct PAddRunInfo {
  std::string fPathFileName{""};
  PIntVector fT0;
  std::string fFileFormat{""};
};


//--------------------------------------------------------------------------
/**
 * <p>Sends the usage description to the standard output.
 */
void addRun_syntax()
{
  std::cout << std::endl;
  std::cout << "usage0: addRun [--help | -h] | [--version | -v]" << std::endl;
  std::cout << "usage1: addRun <options1> -rl <runList>" << std::endl;
  std::cout << "usage2: addRun <options2> -in <inputFile>" << std::endl;
  std::cout << std::endl;
  std::cout << "  <options1>:" << std::endl;
  std::cout << "    -t0  <ival>: <ival> is a comma separted list of global t0-bin's, or" << std::endl;
  std::cout << "                 <ival> is a comma separted list of '-1', then it is assumed that there is a prompt peak." << std::endl;
  std::cout << "                 Under this condition the t0-bin will be determined automatically by" << std::endl;
  std::cout << "                 the position of the max-value of the corresponing histograms." << std::endl;
  std::cout << "                 If t0's are not provided, t0-bin will be taken from the file." << std::endl;
  std::cout << "    -f <format>: <format> is the output file format to be used." << std::endl;
  std::cout << "                 For supported formats see below." << std::endl;
  std::cout << "    -y <year>  : the year at which runs were measured. Format yyyy." << std::endl;
  std::cout << "                 If not provided, the current year is used." << std::endl;
  std::cout << "    -i <instrument> : <instrument> is one of gps, ltf, flame, gpd, hifi, dolly, lem" << std::endl;
  std::cout << "    -m <dev>   : <dev> is pta or tdc (only needed for bulk). Default: tdc" << std::endl;
  std::cout << "    -o <fln>   : output file name." << std::endl;
  std::cout << "  -rl <runList> can be:" << std::endl ;
  std::cout << "    (i)   <run0> <run1> <run2> ... <runN> : run numbers, e.g. 123 124" << std::endl;
  std::cout << "    (ii)  <run0>-<runN> : a range, e.g. 123-125 -> 123 124 125" << std::endl;
  std::cout << "    (iii) <run0>:<runN>:<step> : a sequence, e.g. 123:127:2 -> 123 125 127" << std::endl;
  std::cout << "          <step> will give the step width and has to be a positive number!" << std::endl;
  std::cout << "     a <runList> can also combine (i)-(iii), e.g. 123 128-130 133, etc." << std::endl;
  std::cout << std::endl;
  std::cout << "  <options2>:" << std::endl;
  std::cout << "    -f <format>: <format> is file format of the output-file to be used." << std::endl;
  std::cout << "    -o <fln>   : output file name." << std::endl;
  std::cout << "  -in <inputFile>: the file name of the file containing the necessary run information" << std::endl;
  std::cout << "               to add runs with various t0's, fgb's, lgb's, different years, etc." << std::endl;
  std::cout << "               The structure of the <inputFile> is:" << std::endl;
  std::cout << "               Lines starting with a '%' and empty lines are ignored." << std::endl;
  std::cout << "               A single run needs to provide the following information:" << std::endl;
  std::cout << "               file <path-name>: needs to be a full path name" << std::endl;
  std::cout << "               t0 <t0-bin>     : needs to be the t0 bin or " << std::endl;
  std::cout << "                                 0 to take the t0 bin from the file, or" << std::endl;
  std::cout << "                                 -1 for automatic determination via prompt peak (see above)." << std::endl;
  std::cout << "      Example:" << std::endl;
  std::cout << "        % file 1. 6 histos present, hence 6 t0-bins" << std::endl;
  std::cout << "        file /home/test/data/deltat_tdc_gps_4324.bin" << std::endl;
  std::cout << "        t0  401, 400, 399, 400, 358, 400" << std::endl;
  std::cout << "        % file 2, take t0-bins from the file" << std::endl;
  std::cout << "        file /home/test/data/deltat_tdc_gps_4325.bin" << std::endl;
  std::cout << "        % file 3, deduce to t0-bin's from the prompt peak" << std::endl;
  std::cout << "        file /home/test/data/deltat_tdc_gps_4325.bin" << std::endl;
  std::cout << "        t0  -1, -1, -1, -1, -1, -1" << std::endl;
  std::cout << std::endl;
  std::cout << " Supported uSR file formats:" << std::endl;
  std::cout << "   MusrRoot, PSI-BIN, PSI-MDU, MUD, NeXus" << std::endl;
  std::cout << std::endl;
}

//--------------------------------------------------------------------------
/**
 * <p> check if the requested format is supported.
 *
 * @param format requested format string
 *
 * @return true if supported, false otherwise
 */
bool addRun_checkFormat(std::string &format) {
  bool result = false;
  boost::to_lower(format);

  if (format == "psi-bin")
    format = "psibin";
  if (format == "psi-mud")
    format = "psimdu";

  if (format == "musrroot") {
    result = true;
  } else if (format == "psibin") {
    result = true;
  } else if (format == "psimdu") {
    result = true;
  } else if (format == "mud") {
    result = true;
  } else if (format == "nexus") {
    result = true;
  } else if (format == "root") {
    result = true;
  }

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p>Reads the inputFile to extract the necessary information.
 * @param fileName
 * @param infoVec
 * @return
 */
bool addRun_readInputFiles(const std::string fileName, std::vector<PAddRunInfo> &infoVec)
{
  PAddRunInfo info;
  char buf[256], str[256];
  std::ifstream fin(fileName.c_str(), std::ifstream::in);
  std::string line;
  char *tok{nullptr};
  int status, ival;
  bool lastWasFile{false};
  while (fin.good()) {
    fin.getline(buf, 256);
    line = buf;
    boost::trim_left(line);
    if (line.empty())
      continue;
    if (line[0] == '%')
      continue;
    strcpy(buf, line.c_str());
    tok = strtok(buf, " ");
    if (!strcmp(tok, "file")) {
      if (lastWasFile) {
        infoVec.push_back(info);
        info.fPathFileName = "";
        info.fT0.clear();
      }
      tok = strtok(NULL, " ");
      if (tok == NULL) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found label 'file' without argument." << std::endl;
        std::cerr << std::endl;
        fin.close();
        return false;
      }
      info.fPathFileName = tok;
      lastWasFile = true;
    } else if (!strcmp(tok, "t0")) {
      while ((tok = strtok(NULL, ",")) != NULL) {
        status = sscanf(tok, "%d%s", &ival, str);
        if (status != 1) {
          std::cerr << std::endl;
          std::cerr << "**ERROR** found t0 argument '" << tok << "' which is not a number." << std::endl;
          std::cerr << std::endl;
          fin.close();
          return false;
        }
        info.fT0.push_back(ival);
      }
    } else {
      std::cerr << std::endl;
      std::cerr << "**ERROR** found unrecognized token '" << tok << "'." << std::endl;
      std::cerr << std::endl;
      fin.close();
      return false;
    }
  }
  infoVec.push_back(info);

  fin.close();

  // checks
  for (int i=0; i<infoVec.size(); i++) {
    if (infoVec[i].fPathFileName.empty()) {
      std::cerr << std::endl;
      std::cerr << "**ERROR** something is wrong with the inputFile. Found empty pathName." << std::endl;
      std::cerr << std::endl;
      return false;
    }
  }

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Automatically determines the t0-bin. This assumes that there is a
 * prompt peak in the data!
 *
 * @param vec histo data
 * @return maximum of the histo data
 */
UInt_t addRun_getPromptPeakPos(PDoubleVector *vec)
{
  UInt_t pos=0;
  Double_t max=vec->at(0);

  for (UInt_t i=0; i<vec->size(); i++) {
    if (max < vec->at(i)) {
      max = vec->at(i);
      pos = i;
    }
  }

  return pos;
}

//--------------------------------------------------------------------------
/**
 * <p>Filters the t0 arguments. Allowed is a comma separeted list of
 * integers > -2.
 *
 * @param argc argument counter
 * @param argv argument list
 * @param idx argument index from which to start
 * @param t0 vector
 *
 * @return true on success, false otherwise
 */
bool addRun_filter_t0(int argc, char *argv[], int &idx, PIntVector &t0)
{
  int pos{idx}, status, ival;
  PIntVector tt0;
  // collect run list string from input
  for (int i=idx; i<argc; i++) {
    pos = i;
    if ((argv[i][0] == '-') && isalpha(argv[i][1])) { // next command
      pos = i-1;
      break;
    } else {
      status = sscanf(argv[i], "%d", &ival);
      if (status != 1) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found t0 value '" << argv[i] << "' which is not an integer." << std::endl;
        std::cerr << std::endl;
        return false;
      }
      tt0.push_back(ival);
    }
  }

  // make sure that t0's > -2
  for (int i=0; i<tt0.size(); i++) {
    if (tt0[i] <= -2) {
      std::cerr << std::endl;
      std::cerr << "**ERROR** found t0 value '" << tt0[i] << "' which is out-of-range." << std::endl;
      std::cerr << std::endl;
      return false;
    }
  }

  idx = pos;
  t0 = tt0;

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>Filters the runList arguments. Allowed are: (i) run1 run2 ... runN
 * (ii) runStart-runEnd, and (iii) runStart:runEnd:step
 *
 * @param argc argument counter
 * @param argv argument list
 * @param idx argument index from which to start
 * @param runList vector
 *
 * @return true on success, false otherwise
 */
bool addRun_filter_runList(int argc, char *argv[], int &idx, PUIntVector &runList)
{
  int pos{idx};
  std::string runStr{""};
  // collect run list string from input
  for (int i=idx; i<argc; i++) {
    pos = i;
    if ((argv[i][0] == '-') && isalpha(argv[i][1])) { // next command
      pos = i-1;
      break;
    } else {
      runStr += argv[i];
      runStr += " ";
    }
  }
  // extract run list from string
  PStringNumberList rl(runStr);
  std::string errMsg{""};
  if (!rl.Parse(errMsg)) {
    std::cerr << "**ERROR** in run list: -rl " << runStr << std::endl;
    std::cerr << errMsg << std::endl;
    return false;
  }
  runList = rl.GetList();

  idx = pos;

  return true;
}

//--------------------------------------------------------------------------
/**
 * <p>addRun is used to add various runs.
 *
 * @param argc number of input arguments
 * @param argv list of input arguments
 *
 * @return PMUSR_SUCCESS if everthing went smooth, otherwise and error number
 */
int main(int argc, char *argv[])
{
  // check for --help or --version
  if (argc == 2) {
    if (!strncmp(argv[1], "--help", 128) || !strncmp(argv[1], "-h", 128)) {
      addRun_syntax();
      return PMUSR_SUCCESS;
    } else if (!strncmp(argv[1], "--version", 128) || !strncmp(argv[1], "-v", 128)) {
#ifdef HAVE_CONFIG_H
#ifdef HAVE_GIT_REV_H
      std::cout << std::endl << "addRun version: " << PACKAGE_VERSION << ", git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << " (" << BUILD_TYPE << "), ROOT version: " << ROOT_VERSION_USED << std::endl << std::endl;
#else
      std::cout << std::endl << "addRun version: " << PACKAGE_VERSION << " (" << BUILD_TYPE << "), ROOT version: " << ROOT_VERSION_USED << std::endl << std::endl;
#endif
#else
#ifdef HAVE_GIT_REV_H
      std::cout << std::endl << "addRun git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << std::endl << std::endl;
#else
      std::cout << std::endl << "addRun version: unkown." << std::endl << std::endl;
#endif
#endif
      return PMUSR_SUCCESS;
    } else {
      std::cerr << std::endl;
      std::cerr << "**ERROR** missing required input." << std::endl;
      std::cerr << std::endl;
      addRun_syntax();
      return -1;
    }
  }

  // filter arguments
  PIntVector t0;
  Int_t ival;
  UInt_t yearNum{0};
  std::string flnOut{""};
  std::string format{""}, year{""}, instrument{""}, dev{"tdc"};
  PUIntVector runList;
  std::string inputFln{""};
  int status;
  std::vector<PAddRunInfo> addRunInfo;

  for (int i=1; i<argc; i++) {
    if (!strcmp(argv[i], "-t0")) {
      if (i+1 >= argc) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found -t0 without value." << std::endl;
        std::cerr << std::endl;
        return -1;
      }
      // deal with run-list here
      int idx=i+1;
      if (!addRun_filter_t0(argc, argv, idx, t0)) {
        return -2;
      }
      i=idx;
    } else if (!strcmp(argv[i], "-f")) {
      if (i+1 >= argc) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found -f without argument." << std::endl;
        std::cerr << std::endl;
        return -1;
      }
      format = argv[i+1];
      if (!addRun_checkFormat(format)) {
        format = "";
        std::cerr << std::endl;
        std::cerr << "**ERROR** found -f with unsupported format: '" << format << "'" << std::endl;
        std::cerr << std::endl;
        return -2;
      }
      i++;
    } else if (!strcmp(argv[i], "-y")) {
      if (i+1 >= argc) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found -y without value." << std::endl;
        std::cerr << std::endl;
        return -1;
      }
      status = sscanf(argv[i+1], "%d", &ival);
      if (status != 1) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found invalid -y value: " << argv[i+1] << "." << std::endl;
        std::cerr << std::endl;
        return -2;
      }
      if (1900 - ival > 0) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found invalid -y value: " << argv[i+1] << "." << std::endl;
        std::cerr << "          Format has to be YYYY." << std::endl;
        std::cerr << std::endl;
        return -2;
      }
      yearNum = ival;
      year = argv[i+1];
      i++;
    } else if (!strcmp(argv[i], "-i")) {
      if (i+1 >= argc) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found -i without argument." << std::endl;
        std::cerr << std::endl;
        return -1;
      }
      instrument = argv[i+1];
      boost::to_lower(instrument);
      i++;
    } else if (!strcmp(argv[i], "-m")) {
      if (i+1 >= argc) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found -m without argument." << std::endl;
        std::cerr << std::endl;
        return -1;
      }
      std::string str(argv[i+1]);
      boost::to_lower(str);
      if ((str != "pta") && (str != "tdc")) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found -m with unsupported argument '" << argv[i+1] << "'. Possible arguments are 'pta' or 'tdc'." << std::endl;
        std::cerr << std::endl;
        return -2;
      }
      dev = str;
      i++;
    } else if (!strcmp(argv[i], "-o")) {
      if (i+1 >= argc) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found -o without argument." << std::endl;
        std::cerr << std::endl;
        return -1;
      }
      flnOut = argv[i+1];
      i++;
    } else if (!strcmp(argv[i], "-rl")) {
      if (i+1 >= argc) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found -rl without argument." << std::endl;
        std::cerr << std::endl;
        return -1;
      }
      // deal with run-list here
      int idx=i+1;
      if (!addRun_filter_runList(argc, argv, idx, runList)) {
        return -2;
      }
      i=idx;
    } else if (!strcmp(argv[i], "-in")) {
      if (i+1 >= argc) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** found -in without argument." << std::endl;
        std::cerr << std::endl;
        return -1;
      }
      inputFln = argv[i+1];
      i++;
    } else { // error
      std::cerr << std::endl;
      std::cerr << "**ERROR** found unexpected command line element '" << argv[i] << "'" << std::endl;
      std::cerr << std::endl;
      addRun_syntax();
      return -1;
    }
  }

  // test for usage1 or usage2
  if (inputFln.empty() && (runList.size() == 0)) {
    std::cerr << std::endl;
    std::cerr << "**ERROR** essential input for usage1 and usage2 is missing." << std::endl;
    std::cerr << std::endl;
    addRun_syntax();
    return -3;
  }
  if (!inputFln.empty() && (runList.size() > 0)) {
    std::cerr << std::endl;
    std::cerr << "**ERROR** cannot decide if usage1 or usage2." << std::endl;
    std::cerr << std::endl;
    addRun_syntax();
    return -3;
  }

  // read startup file
  char startup_path_name[128];
  TSAXParser *saxParser = new TSAXParser();
  PStartupHandler *startupHandler = new PStartupHandler();
  if (!startupHandler->StartupFileFound()) {
    std::cerr << std::endl << ">> addRun **WARNING** couldn't find " << startupHandler->GetStartupFilePath().Data();
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
    status = parseXmlFile(saxParser, startup_path_name);
    // check for parse errors
    if (status) { // error
      std::cerr << std::endl << ">> addRun **WARNING** Reading/parsing musrfit_startup.xml failed.";
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
      startupHandler->CheckLists();
    }
  }

  // additional tests needed for usage1 (currently for PSI use only)
  if (runList.size() > 0) {
    if (t0.empty()) {
      std::cout << ">> t0 not provided.";
    } else {
      std::cout << ">> t0: ";
      for (int i=0; i<t0.size(); i++)
        std::cout << t0[i] << ", ";
    }
    std::cout << std::endl;
    std::cout << ">> format: " << format << std::endl;
    std::cout << ">> year: " << year << std::endl;
    std::cout << ">> instrument: " << instrument << std::endl;
    std::cout << ">> fln out: " << flnOut << std::endl;
    std::cout << ">> runList: ";
    for (int i=0; i<runList.size(); i++)
      std::cout << runList[i] << ", ";
    std::cout << std::endl;

    PAddRunInfo addRun;
    addRun.fT0 = t0;
    addRun.fFileFormat = format;
    // construct file names
    for (int i=0; i<runList.size(); i++) {
      PFindRun findRun(startupHandler->GetDataPathList(), startupHandler->GetRunNameTemplateList(), instrument, yearNum, runList[i]);
      if (findRun.FoundPathName()) {
        std::cout << ">> found path name: " << findRun.GetPathName() << std::endl;
        addRun.fPathFileName = findRun.GetPathName();
        addRunInfo.push_back(addRun);
      } else {
        std::cout << "**WARNING** run: " << runList[i] << " for instrument '" << instrument << "' and year '" << year << "' not found" << std::endl;
      }
    }
  }

  // additional tests needed for usage2
  if (!inputFln.empty()) {
    // check if file exists
    if (!boost::filesystem::exists(inputFln)) {
      std::cerr << std::endl;
      std::cerr << "**ERROR** file '" << inputFln << "' seems not to exist." << std::endl;
      std::cerr << std::endl;
      return -1;
    }
    // read input-file and data sets
    if (!addRun_readInputFiles(inputFln, addRunInfo)) {
      return -2;
    }
  }

  for (int i=0; i<addRunInfo.size(); i++) {
    std::cout << ">> run " << i+1 << ": " << std::endl;
    std::cout << ">> fln : " << addRunInfo[i].fPathFileName << std::endl;
    if (addRunInfo[i].fT0.empty()) {
      std::cout << ">> t0 not provided.";
    } else {
      std::cout << ">> t0: ";
      for (int j=0; j<addRunInfo[i].fT0.size(); j++) {
        std::cout << addRunInfo[i].fT0[j] << ", ";
      }
    }
    std::cout << std::endl;
  }

  // load the files
  std::vector<PRunDataHandler*> runDataHandler;
  runDataHandler.resize(addRunInfo.size());
  Bool_t isGood{true};
  for (UInt_t i=0; i<runDataHandler.size(); i++) {
    if (startupHandler != nullptr) {
      runDataHandler[i] = new PRunDataHandler(addRunInfo[i].fPathFileName, addRunInfo[i].fFileFormat, startupHandler->GetDataPathList());
      if (runDataHandler[i] == nullptr) {
        isGood = false;
        std::cerr << std::endl;
        std::cerr << "**ERROR** couldn't invoke PRunDataHandler (i=" << i << ")." << std::endl;
        std::cerr << std::endl;
        break;
      }
      runDataHandler[i]->ReadData();
      if (!runDataHandler[i]->IsAllDataAvailable()) {
        isGood = false;
        std::cerr << std::endl;
        std::cerr << "**ERROR** couldn't read data for PRunDataHandler (i=" << i << ")." << std::endl;
        std::cerr << std::endl;
        break;
      }
    } else {
      runDataHandler[i] = new PRunDataHandler(addRunInfo[i].fPathFileName, addRunInfo[i].fFileFormat);
      if (runDataHandler[i] == nullptr) {
        isGood = false;
        std::cerr << std::endl;
        std::cerr << "**ERROR** couldn't invoke PRunDataHandler (i=" << i << ")." << std::endl;
        std::cerr << std::endl;
        break;
      }
      runDataHandler[i]->ReadData();
      if (!runDataHandler[i]->IsAllDataAvailable()) {
        isGood = false;
        std::cerr << std::endl;
        std::cerr << "**ERROR** couldn't read data for PRunDataHandler (i=" << i << ")." << std::endl;
        std::cerr << std::endl;
        break;
      }
    }
  }

  // make sure that the number of provided t0's are matching the number of histos from the run-file

  // 1st make sure all the runs have the same number run data (==1 here)
  PAny2ManyInfo *info{nullptr};
  PRunDataHandler *dataOut{nullptr};
  if (isGood) {
    for (UInt_t i=1; i<runDataHandler.size(); i++) {
      if (runDataHandler[0]->GetNoOfRunData() != runDataHandler[i]->GetNoOfRunData()) {
        isGood = false;
        std::cerr << std::endl;
        std::cerr << "**ERROR** can only handle same number of run data per run handler." << std::endl;
        std::cerr << std::endl;
        break;
      }
    }
    info = new PAny2ManyInfo();
    if (info == nullptr) {
      std::cerr << std::endl;
      std::cerr << "**ERROR** couldn't invoke PAny2ManyInfo." << std::endl;
      std::cerr << std::endl;
      isGood = false;
    }
  }

  if (isGood) {
    // prepare for the new added run data sets
    info->outFormat = format;
    info->year = year;
    info->outFileName = flnOut;
    dataOut = new PRunDataHandler(info);
    if (dataOut == nullptr) {
      std::cerr << std::endl;
      std::cerr << "**ERROR** couldn't invoke PRunDataHandler for the output file." << std::endl;
      std::cerr << std::endl;
      isGood = false;
    }
  }

  if (isGood) {
    // check that all runs have the same number of histograms
    for (UInt_t i=1; i<runDataHandler.size(); i++) {
      if (runDataHandler[0]->GetRunData()->GetNoOfHistos() != runDataHandler[0]->GetRunData()->GetNoOfHistos()) {
        std::cerr << std::endl;
        std::cerr << "**ERROR** can only add runs with the same number of histograms." << std::endl;
        std::cerr << std::endl;
        isGood = false;
      }
    }
  }

  if (isGood) {
    // add all the runs
    // take first run as the reference for the data
    std::vector<PDoubleVector*> addedHistos;
    addedHistos.resize(runDataHandler[0]->GetRunData()->GetNoOfHistos());
    for (UInt_t i=0; i<runDataHandler[0]->GetRunData()->GetNoOfHistos(); i++) {
      addedHistos[i] = runDataHandler[0]->GetRunData()->GetDataSet(i, false)->GetData();
    }
    // get the t0's for all the reference histos
    PIntVector t0Vec;
    t0Vec.resize(runDataHandler[0]->GetRunData()->GetNoOfHistos());
    if (addRunInfo[0].fT0.empty()) { // i.e. take t0's from data file
      for (UInt_t i=0; i<runDataHandler[0]->GetRunData()->GetNoOfHistos(); i++) {
        t0Vec[i] = runDataHandler[0]->GetRunData()->GetT0Bin(i+1);
      }
      addRunInfo[0].fT0 = t0Vec;
    } else { // t0 vector present
      // make sure that the number of t0's fit the number of histos
      if (addRunInfo[0].fT0.size() < runDataHandler[0]->GetRunData()->GetNoOfHistos()) {
        UInt_t counts=runDataHandler[0]->GetRunData()->GetNoOfHistos()-addRunInfo[0].fT0.size();
        for (UInt_t i=0; i<counts; i++)
          addRunInfo[0].fT0.push_back(0);
      }
      // check t0 data
      for (UInt_t i=0; i<addRunInfo[0].fT0.size(); i++) {
        if (addRunInfo[0].fT0[i] == 0) { // get t0 from file
          addRunInfo[0].fT0[i] = runDataHandler[0]->GetRunData()->GetT0Bin(i+1);
        } else if (addRunInfo[0].fT0[i] == -1) { // get t0 from prompt peak
          addRunInfo[0].fT0[i] = addRun_getPromptPeakPos(runDataHandler[0]->GetRunData()->GetDataSet(i, false)->GetData());
        }
      }
    }

    // loop over the remaining runs, determine t0's if needed and add the data
    for (int i=1; i<runDataHandler.size(); i++) {
      // get the t0's for all the histos of a run to be added
      PIntVector t0Vec;
      t0Vec.resize(runDataHandler[i]->GetRunData()->GetNoOfHistos());
      if (addRunInfo[i].fT0.empty()) { // i.e. take t0's from data file
        for (UInt_t j=0; j<runDataHandler[i]->GetRunData()->GetNoOfHistos(); j++) {
          t0Vec[j] = runDataHandler[i]->GetRunData()->GetT0Bin(j+1);
        }
        addRunInfo[i].fT0 = t0Vec;
      } else { // t0 vector present
        // make sure that the number of t0's fit the number of histos
        if (addRunInfo[i].fT0.size() < runDataHandler[i]->GetRunData()->GetNoOfHistos()) {
          UInt_t counts=runDataHandler[i]->GetRunData()->GetNoOfHistos()-addRunInfo[i].fT0.size();
          for (UInt_t j=0; j<counts; j++)
            addRunInfo[i].fT0.push_back(0);
        }
        // check t0 data
        for (UInt_t j=0; j<addRunInfo[i].fT0.size(); j++) {
          if (addRunInfo[i].fT0[j] == 0) { // get t0 from file
            addRunInfo[i].fT0[j] = runDataHandler[i]->GetRunData()->GetT0Bin(j+1);
          } else if (addRunInfo[i].fT0[j] == -1) { // get t0 from prompt peak
            addRunInfo[i].fT0[j] = addRun_getPromptPeakPos(runDataHandler[i]->GetRunData()->GetDataSet(j, false)->GetData());
          }
        }
      }

      // calculate the offset due to potential differences in t0's between runs
      PIntVector diff;
      diff.resize(addRunInfo[i].fT0.size());
      for (UInt_t j=0; j<diff.size(); j++) {
        diff[j] = addRunInfo[i].fT0[j] - addRunInfo[0].fT0[j];
      }

      // add all the to be added histos of all remaining runs
      PDoubleVector *addData{nullptr};
      Int_t idx;
      for (int j=0; j<runDataHandler[i]->GetRunData()->GetNoOfHistos(); j++) { // loop over all histos
        addData = runDataHandler[i]->GetRunData()->GetDataSet(j, false)->GetData();
        for (int k=0; k<addedHistos[j]->size(); k++) { // loop over all elements of a histo
          idx = k + diff[j];
          if ((idx >= 0) && (idx < addData->size())) {
            addedHistos[j]->at(k) += addData->at(idx);
          }
        }
      }
    }

    // feed all the necessary information for the data file
    PRawRunData *rawRunData = new PRawRunData();
    rawRunData = runDataHandler[0]->GetRunData(); // copy all
    rawRunData->SetGenerator("addRun");
    // overwrite the t0 values with the new ones
    for (UInt_t i=0; i<rawRunData->GetNoOfHistos(); i++) {
      rawRunData->GetDataSet(i, false)->SetTimeZeroBin(addRunInfo[0].fT0[i]);
    }
    // write histos
    for (UInt_t i=0; i<rawRunData->GetNoOfHistos(); i++) {
      rawRunData->GetDataSet(i, false)->SetData(*addedHistos[i]);
    }

    // feed run data handler with new data
    if (dataOut->SetRunData(rawRunData)) {
      // write output file
      dataOut->WriteData();
    }
  }

  // clean up
  if (startupHandler) {
    delete startupHandler;
  }
  if (info) {
    delete info;
  }
  if (dataOut) {
    delete dataOut;
  }
  for (int i=0; i<runDataHandler.size(); i++) {
    if (runDataHandler[i])
      delete runDataHandler[i];
  }
  runDataHandler.clear();  

  return PMUSR_SUCCESS;
}
