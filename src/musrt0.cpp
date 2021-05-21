/***************************************************************************

  musrt0.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2021 by Andreas Suter                              *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include <TApplication.h>
#include <TSAXParser.h>
#include <TROOT.h>
#include <TSystem.h>

#include "git-revision.h"
#include "PMusr.h"
#include "PStartupHandler.h"
#include "PMsrHandler.h"
#include "PRunDataHandler.h"
#include "PMusrT0.h"

//--------------------------------------------------------------------------
/**
 * <p>Sends the usage description to the standard output.
 */
void musrt0_syntax()
{
  std::cout << std::endl << "usage: musrt0 <msr-file> [{--getT0FromPromptPeak | -g} [<firstGoodBinOffset>]]";
  std::cout << std::endl << "       [--timeout <timeout>] | --show-dynamic-path | --version | --help";
  std::cout << std::endl << "       <msr-file>: msr input file";
  std::cout << std::endl << "       --getT0FromPromptPeak, -g with <firstGoodBinOffset>:";
  std::cout << std::endl << "         will, in non-interactive mode estimate the t0's from the prompt peak";
  std::cout << std::endl << "         and write it into the msr-file.";
  std::cout << std::endl << "         if <firstGoodBinOffset> is given, to first good bin will be t0+<firstGoodBinOffset>.";
  std::cout << std::endl << "         if no <firstGoodBinOffset> is given, only t0 will be set.";
  std::cout << std::endl << "       --timeout <timeout>: <timeout> given in seconds after which musrview terminates.";
  std::cout << std::endl << "           If <timeout> <= 0, no timeout will take place. Default <timeout> is 0.";
  std::cout << std::endl;
  std::cout << std::endl << "       'musrt0 <msr-file>' will execute musrt0";
  std::cout << std::endl << "       'musrt0 <msr-file> --timeout 180' will execute musrt0, but terminate it after";
  std::cout << std::endl << "                          180 sec if not already done so.";
  std::cout << std::endl << "       'musrt0' or 'musrt0 --help' will show this help";
  std::cout << std::endl << "       'musrt0 --version' will print the musrt0 version";
  std::cout << std::endl << "       'musrt0 --show-dynamic-path' dumps the dynamic search paths and exit.";
  std::cout << std::endl << std::endl;
}

//--------------------------------------------------------------------------
/**
 * <p>This routine sets up a raw single histogram canvas to graphically enter t0, data- and background-range
 * (depending on some given input flags).
 *
 * <b>return:</b>
 * - true, if everthing went smooth
 * - false, otherwise
 *
 * \param app main root application handler
 * \param msrHandler msr-file handler
 * \param data musrT0 data set handler
 * \param idx index to filter out the proper msr-file run
 */
Bool_t musrt0_item(TApplication &app, PMsrHandler *msrHandler, PMusrT0Data &data, UInt_t idx, Int_t timeout)
{
  PMusrT0 *musrT0 = new PMusrT0(data);

  // check if the musrT0 object could be invoked
  if (musrT0 == nullptr) {
    std::cerr << std::endl << ">> musrt0 **ERROR** Couldn't invoke musrT0 ...";
    std::cerr << std::endl << ">> run name " << data.GetRawRunData(idx)->GetRunName()->Data();
    std::cerr << std::endl;
    return false;
  }

  // check if the musrT0 object is valid
  if (!musrT0->IsValid()) {
    std::cerr << std::endl << ">> musrt0 **ERROR** invalid item found! (idx=" << idx << ")";
    std::cerr << std::endl;
    return false;
  }

  // set timeout
  musrT0->SetTimeout(timeout);

  // set the msr-file handler. The handler cannot be transfered at construction time since rootcling is not able to handle the PMsrHandler class
  musrT0->SetMsrHandler(msrHandler);

  // check if only t0, data-, and bkg-range is wished, if not, only initialize t0 at this point
  if (data.GetCmdTag() != PMUSRT0_GET_DATA_AND_BKG_RANGE)
    musrT0->InitT0();

  // check if only t0 is wished, if not, initialize data- and bkg-ranges
  if (data.GetCmdTag() != PMUSRT0_GET_T0)
    musrT0->InitDataAndBkg();

  // connect SIGNAL 'Done' of musrT0 with the SLOT 'Terminate' of app. This will terminate the main application if
  // the local musrT0 object emits 'Done'
  musrT0->Connect("Done(Int_t)", "TApplication", &app, "Terminate(Int_t)");

  app.Run(true); // true needed that Run will return after quit
  Bool_t result = true;
  if (musrT0->GetStatus() >= 1)
    result = false;
  else
    result = true;

  // disconnect all SIGNALS and SLOTS connected t0 musrT0
  musrT0->Disconnect(musrT0);

  // cleanup
  delete musrT0;
  musrT0 = nullptr;

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p>This routine cleans up the handlers.
 *
 * \param saxParser XML SAX parser
 * \param startupHandler startup handler
 * \param msrHandler msr-file handler
 * \param dataHandler raw run data handler
 */
void musrt0_cleanup(TSAXParser *saxParser, PStartupHandler *startupHandler, PMsrHandler *msrHandler, PRunDataHandler *dataHandler)
{
  if (saxParser) {
    delete saxParser;
    saxParser = nullptr;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = nullptr;
  }
  if (msrHandler) {
    delete msrHandler;
    msrHandler = nullptr;
  }
  if (dataHandler) {
    delete dataHandler;
    dataHandler = nullptr;
  }
}


//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param data
 */
Int_t musrt0_getMaxBin(const PDoubleVector *data)
{
  Int_t maxBin = -1;
  Double_t maxData = -999;

  for (UInt_t i=0; i<data->size(); i++) {
    if (data->at(i) > maxData) {
      maxData = data->at(i);
      maxBin = i;
    }
  }

  return maxBin;
}

//--------------------------------------------------------------------------
/**
 * <p>The musrt0 program is used to set graphically t0's, data- and background-ranges.
 * For a detailed description/usage of the program, please see
 * \htmlonly <a href="http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#musrt0">musrt0 online help</a>
 * \endhtmlonly
 * \latexonly musrt0 online help: \texttt{http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#musrt0}
 * \endlatexonly
 *
 * <b>return:</b>
 * - PMUSR_SUCCESS if everthing went smooth
 * - PMUSR_WRONG_STARTUP_SYNTAX if syntax error is encountered
 * - line number if an error in the msr-file was encountered which cannot be handled.
 *
 * \param argc number of input arguments
 * \param argv list of input arguments
 */
Int_t main(Int_t argc, Char_t *argv[])
{
  Bool_t show_syntax = false;
  Int_t  status;
  Bool_t success = true;
  Char_t filename[1024];
  Bool_t getT0FromPromptPeak = false;
  Bool_t firstGoodBinOffsetPresent = false;
  Int_t firstGoodBinOffset = 0;
  Int_t timeout = 0;
  Int_t fitType = -1;

  if (argc == 1) {
    musrt0_syntax();
    return PMUSR_SUCCESS;
  }

  // add default shared library path /usr/local/lib if not already persent
  const char *dsp = gSystem->GetDynamicPath();
  if (strstr(dsp, "/usr/local/lib") == nullptr)
    gSystem->AddDynamicPath("/usr/local/lib");

  memset(filename, '\0', sizeof(filename));
  for (int i=1; i<argc; i++) {
    if (!strcmp(argv[i], "--version")) {
#ifdef HAVE_CONFIG_H
      std::cout << std::endl << "musrt0 version: " << PACKAGE_VERSION << ", git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << " (" << BUILD_TYPE << "), ROOT version: " << ROOT_VERSION_USED << std::endl << std::endl;
#else
      std::cout << std::endl << "musrt0 git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << std::endl << std::endl;
#endif
      return PMUSR_SUCCESS;
    } else if (!strcmp(argv[i], "--help")) {
      musrt0_syntax();
      return PMUSR_SUCCESS;
    } else if (!strcmp(argv[i], "--show-dynamic-path")) {
      std::cout << std::endl << "musrt0: internal dynamic search paths for shared libraries/root dictionaries:";
      std::cout << std::endl << "  '" << gSystem->GetDynamicPath() << "'" << std::endl << std::endl;
      return PMUSR_SUCCESS;
    } else if (strstr(argv[i], ".msr")) { // check for filename
      if (strlen(filename) == 0) {
        strcpy(filename, argv[i]);
      } else {
        std::cout << std::endl << "**ERROR** only one file name allowed." << std::endl;
        show_syntax = true;
        break;
      }
    } else if (!strcmp(argv[i], "--getT0FromPromptPeak") || !strcmp(argv[i], "-g")) { // T0 from prompt peak option
      getT0FromPromptPeak = true;
      if (i+1 < argc) {
        TString offset(argv[i+1]);
        if (offset.IsFloat()) {
          firstGoodBinOffsetPresent = true;
          firstGoodBinOffset = offset.Atof();
          i++;
        }
      }
    } else if (!strcmp(argv[i], "--timeout")) {
      if (i+1 < argc) {
        TString numStr(argv[i+1]);
        if (numStr.IsDigit()) {
          timeout = numStr.Atoi();
        } else {
          std::cout << std::endl << "**ERROR** timeout '" << argv[i+1] << "' is not a number" << std::endl;
          show_syntax = true;
          break;
        }
        i++;
      } else {
        std::cout << std::endl << "**ERROR** no timeout given." << std::endl;
        show_syntax = true;
        break;
      }
    } else {
      show_syntax = true;
      break;
    }
  }

  if (strlen(filename) == 0) {
    std::cout  << std::endl << "**ERROR** msr-file missing!" << std::endl;
    show_syntax = true;
  }

  if (show_syntax) {
    musrt0_syntax();
    return PMUSR_WRONG_STARTUP_SYNTAX;
  }

  // read startup file
  Char_t startup_path_name[128];
  TSAXParser *saxParser = new TSAXParser();
  PStartupHandler *startupHandler = new PStartupHandler();
  if (!startupHandler->StartupFileFound()) {
    std::cerr << std::endl << ">> musrt0 **WARNING** couldn't find " << startupHandler->GetStartupFilePath().Data();
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
      std::cerr << std::endl << ">> musrt0 **WARNING** Reading/parsing musrfit_startup.xml failed.";
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

  // read msr-file
  PMsrHandler *msrHandler = new PMsrHandler(filename);
  status = msrHandler->ReadMsrFile();
  if (status != PMUSR_SUCCESS) {
    switch (status) {
      case PMUSR_MSR_FILE_NOT_FOUND:
        std::cout << std::endl << ">> musrt0 **ERROR** couldn't find '" << filename << "'" << std::endl << std::endl;
        break;
      case PMUSR_MSR_SYNTAX_ERROR:
        std::cout << std::endl << "**SYNTAX ERROR** in file " << filename << ", full stop here." << std::endl << std::endl;
        break;
      default:
        std::cout << std::endl << "**UNKNOWN ERROR** when trying to read the msr-file" << std::endl << std::endl;
        break;
    }
    return status;
  }
  msrHandler->CopyMsrStatisticBlock(); // just copy the statistics block since no fit is performed

  // check if the fittype is not NonMusr
  PMsrRunList *runList = msrHandler->GetMsrRunList();
  for (UInt_t i=0; i<runList->size(); i++) {
    fitType = runList->at(i).GetFitType();
    if (fitType == -1) { // i.e. not found in the RUN block, check the GLOBAL block
      fitType = msrHandler->GetMsrGlobal()->GetFitType();
    }
    if (fitType == MSR_FITTYPE_NON_MUSR) {
      std::cout << std::endl << ">> musrt0 **ERROR** t0 setting for NonMusr fit type doesn't make any sense, will quit ..." << std::endl;
      success = false;
      break;
    }
  }

  // read all the necessary runs (raw data)
  PRunDataHandler *dataHandler = nullptr;
  if (success) {
    if (startupHandler)
      dataHandler = new PRunDataHandler(msrHandler, startupHandler->GetDataPathList());
    else
      dataHandler = new PRunDataHandler(msrHandler);

    dataHandler->ReadData();

    success = dataHandler->IsAllDataAvailable();
    if (!success) {
      std::cout << std::endl << ">> musrt0 **ERROR** Couldn't read all data files, will quit ..." << std::endl;
    }
  }

  if (getT0FromPromptPeak && success) {

    Int_t  histoNo = -1;
    UInt_t t0Bin = 0;
    TString *runName = nullptr;
    UInt_t start, end;

    // go through all runs in the msr-file
    for (UInt_t i=0; i<runList->size(); i++) {
      fitType = runList->at(i).GetFitType();
      if (fitType == -1) { // i.e. not found in the RUN block, check the GLOBAL block
        fitType = msrHandler->GetMsrGlobal()->GetFitType();
      }
      switch (fitType) {
      case MSR_FITTYPE_SINGLE_HISTO:
      case MSR_FITTYPE_MU_MINUS:
        if ((runList->at(i).GetRunNameSize() == 1) && (runList->at(i).GetForwardHistoNoSize() == 1)) { // no addruns / no grouping
          // get histo number
          histoNo = runList->at(i).GetForwardHistoNo();
          runName = runList->at(i).GetRunName();
          // get bin position of maximal data
          t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
          // set t0 to maximum data position
          runList->at(i).SetT0Bin(t0Bin, 0);
          // set data range as well if firstGoodBinOffset is given
          if (firstGoodBinOffsetPresent) {
            start = t0Bin + firstGoodBinOffset;
            end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
            runList->at(i).SetDataRange(start, 0);
            runList->at(i).SetDataRange(end, 1);
          }
        } else if ((runList->at(i).GetRunNameSize() > 1) && (runList->at(i).GetForwardHistoNoSize() == 1)) { // addruns / no grouping
          // get histo number
          histoNo = runList->at(i).GetForwardHistoNo();
          runName = runList->at(i).GetRunName();
          // get bin position of maximal data
          t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
          // set t0 to maximum data position
          runList->at(i).SetT0Bin(t0Bin, 0);
          // set data range as well if firstGoodBinOffset is given
          if (firstGoodBinOffsetPresent) {
            start = t0Bin + firstGoodBinOffset;
            end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
            runList->at(i).SetDataRange(start, 0);
            runList->at(i).SetDataRange(end, 1);
          }
          // handle addruns
          for (UInt_t j=1; j<runList->at(i).GetRunNameSize(); j++) {
            runName = runList->at(i).GetRunName(j);
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetAddT0Bin(t0Bin, j-1, 0);
          }
        } else if ((runList->at(i).GetRunNameSize() == 1) && (runList->at(i).GetForwardHistoNoSize() > 1)) { // no addruns / grouping
          for (UInt_t j=0; j<runList->at(i).GetForwardHistoNoSize(); j++) {
            // get histo number
            histoNo = runList->at(i).GetForwardHistoNo(j);
            runName = runList->at(i).GetRunName();
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetT0Bin(t0Bin, j);
            if (firstGoodBinOffsetPresent && (j==0)) {
              start = t0Bin + firstGoodBinOffset;
              end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
              runList->at(i).SetDataRange(start, 0);
              runList->at(i).SetDataRange(end, 1);
            }
          }
        } else { // addruns / grouping
          for (UInt_t j=0; j<runList->at(i).GetForwardHistoNoSize(); j++) {
            // get histo number
            histoNo = runList->at(i).GetForwardHistoNo(j);
            runName = runList->at(i).GetRunName();
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetT0Bin(t0Bin, j);
            if (firstGoodBinOffsetPresent && (j==0)) {
              start = t0Bin + firstGoodBinOffset;
              end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
              runList->at(i).SetDataRange(start, 0);
              runList->at(i).SetDataRange(end, 1);
            }
            // handle addruns
            for (UInt_t k=1; k<runList->at(i).GetRunNameSize(); k++) {
              runName = runList->at(i).GetRunName(k);
              // get bin position of maximal data
              t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
              // set t0 to maximum data position
              runList->at(i).SetAddT0Bin(t0Bin, k-1, j);
            }
          }
        }
        break;
      case MSR_FITTYPE_ASYM:
        if ((runList->at(i).GetRunNameSize() == 1) && (runList->at(i).GetForwardHistoNoSize() == 1)) { // no addruns / no grouping
          // handle forward histo
          // get histo number
          histoNo = runList->at(i).GetForwardHistoNo();
          runName = runList->at(i).GetRunName();
          // get bin position of maximal data
          t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
          // set t0 to maximum data position
          runList->at(i).SetT0Bin(t0Bin, 0);
          // set data range as well if firstGoodBinOffset is given
          if (firstGoodBinOffsetPresent) {
            start = t0Bin + firstGoodBinOffset;
            end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
            runList->at(i).SetDataRange(start, 0);
            runList->at(i).SetDataRange(end, 1);
          }
          // handle backward histo
          // get histo number
          histoNo = runList->at(i).GetBackwardHistoNo();
          runName = runList->at(i).GetRunName();
          // get bin position of maximal data
          t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
          // set t0 to maximum data position
          runList->at(i).SetT0Bin(t0Bin, 1);
          // set data range as well if firstGoodBinOffset is given
          if (firstGoodBinOffsetPresent) {
            start = t0Bin + firstGoodBinOffset;
            end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
            runList->at(i).SetDataRange(start, 2);
            runList->at(i).SetDataRange(end, 3);
          }
        } else if ((runList->at(i).GetRunNameSize() > 1) && (runList->at(i).GetForwardHistoNoSize() == 1)) { // addruns / no grouping
          // handle forward histo
          // get histo number
          histoNo = runList->at(i).GetForwardHistoNo();
          runName = runList->at(i).GetRunName();
          // get bin position of maximal data
          t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
          // set t0 to maximum data position
          runList->at(i).SetT0Bin(t0Bin, 0);
          // set data range as well if firstGoodBinOffset is given
          if (firstGoodBinOffsetPresent) {
            start = t0Bin + firstGoodBinOffset;
            end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
            runList->at(i).SetDataRange(start, 0);
            runList->at(i).SetDataRange(end, 1);
          }
          // handle addruns
          for (UInt_t j=1; j<runList->at(i).GetRunNameSize(); j++) {
            runName = runList->at(i).GetRunName(j);
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetAddT0Bin(t0Bin, j-1, 0);
          }
          // handle backward histo
          // get histo number
          histoNo = runList->at(i).GetBackwardHistoNo();
          runName = runList->at(i).GetRunName();
          // get bin position of maximal data
          t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
          // set t0 to maximum data position
          runList->at(i).SetT0Bin(t0Bin, 1);
          // set data range as well if firstGoodBinOffset is given
          if (firstGoodBinOffsetPresent) {
            start = t0Bin + firstGoodBinOffset;
            end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
            runList->at(i).SetDataRange(start, 2);
            runList->at(i).SetDataRange(end, 3);
          }
          // handle addruns
          for (UInt_t j=1; j<runList->at(i).GetRunNameSize(); j++) {
            runName = runList->at(i).GetRunName(j);
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetAddT0Bin(t0Bin, j-1, 1);
          }
        } else if ((runList->at(i).GetRunNameSize() == 1) && (runList->at(i).GetForwardHistoNoSize() > 1)) { // no addruns / grouping
          // handle forward histo
          for (UInt_t j=0; j<runList->at(i).GetForwardHistoNoSize(); j++) {
            // get histo number
            histoNo = runList->at(i).GetForwardHistoNo(j);
            runName = runList->at(i).GetRunName();
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetT0Bin(t0Bin, 2*j);
            if (firstGoodBinOffsetPresent && (j==0)) {
              start = t0Bin + firstGoodBinOffset;
              end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
              runList->at(i).SetDataRange(start, 0);
              runList->at(i).SetDataRange(end, 1);
            }
          }
          // handle backward histo
          for (UInt_t j=0; j<runList->at(i).GetBackwardHistoNoSize(); j++) {
            // get histo number
            histoNo = runList->at(i).GetBackwardHistoNo(j);
            runName = runList->at(i).GetRunName();
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetT0Bin(t0Bin, 2*j+1);
            if (firstGoodBinOffsetPresent && (j==0)) {
              start = t0Bin + firstGoodBinOffset;
              end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
              runList->at(i).SetDataRange(start, 2);
              runList->at(i).SetDataRange(end, 3);
            }
          }
        } else { // addruns / grouping
          // handle forward histo
          for (UInt_t j=0; j<runList->at(i).GetForwardHistoNoSize(); j++) {
            // get histo number
            histoNo = runList->at(i).GetForwardHistoNo(j);
            runName = runList->at(i).GetRunName();
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetT0Bin(t0Bin, 2*j);
            if (firstGoodBinOffsetPresent && (j==0)) {
              start = t0Bin + firstGoodBinOffset;
              end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
              runList->at(i).SetDataRange(start, 0);
              runList->at(i).SetDataRange(end, 1);
            }
            // handle addruns
            for (UInt_t k=1; k<runList->at(i).GetRunNameSize(); k++) {
              runName = runList->at(i).GetRunName(k);
              // get bin position of maximal data
              t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
              // set t0 to maximum data position
              runList->at(i).SetAddT0Bin(t0Bin, k-1, 2*j);
            }
          }
          // handle backward histo
          for (UInt_t j=0; j<runList->at(i).GetBackwardHistoNoSize(); j++) {
            // get histo number
            histoNo = runList->at(i).GetBackwardHistoNo(j);
            runName = runList->at(i).GetRunName();
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetT0Bin(t0Bin, 2*j+1);
            if (firstGoodBinOffsetPresent && (j==0)) {
              start = t0Bin + firstGoodBinOffset;
              end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
              runList->at(i).SetDataRange(start, 2);
              runList->at(i).SetDataRange(end, 3);
            }
            // handle addruns
            for (UInt_t k=1; k<runList->at(i).GetRunNameSize(); k++) {
              runName = runList->at(i).GetRunName(k);
              // get bin position of maximal data
              t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
              // set t0 to maximum data position
              runList->at(i).SetAddT0Bin(t0Bin, k-1, 2*j+1);
            }
          }
        }
        break;
	      case MSR_FITTYPE_BNMR:
        if ((runList->at(i).GetRunNameSize() == 1) && (runList->at(i).GetForwardHistoNoSize() == 1)) { // no addruns / no grouping
          // handle forward histo
          // get histo number
          histoNo = runList->at(i).GetForwardHistoNo();
          runName = runList->at(i).GetRunName();
          // get bin position of maximal data
          t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
          // set t0 to maximum data position
          runList->at(i).SetT0Bin(t0Bin, 0);
          // set data range as well if firstGoodBinOffset is given
          if (firstGoodBinOffsetPresent) {
            start = t0Bin + firstGoodBinOffset;
            end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
            runList->at(i).SetDataRange(start, 0);
            runList->at(i).SetDataRange(end, 1);
          }
          // handle backward histo
          // get histo number
          histoNo = runList->at(i).GetBackwardHistoNo();
          runName = runList->at(i).GetRunName();
          // get bin position of maximal data
          t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
          // set t0 to maximum data position
          runList->at(i).SetT0Bin(t0Bin, 1);
          // set data range as well if firstGoodBinOffset is given
          if (firstGoodBinOffsetPresent) {
            start = t0Bin + firstGoodBinOffset;
            end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
            runList->at(i).SetDataRange(start, 2);
            runList->at(i).SetDataRange(end, 3);
          }
        } else if ((runList->at(i).GetRunNameSize() > 1) && (runList->at(i).GetForwardHistoNoSize() == 1)) { // addruns / no grouping
          // handle forward histo
          // get histo number
          histoNo = runList->at(i).GetForwardHistoNo();
          runName = runList->at(i).GetRunName();
          // get bin position of maximal data
          t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
          // set t0 to maximum data position
          runList->at(i).SetT0Bin(t0Bin, 0);
          // set data range as well if firstGoodBinOffset is given
          if (firstGoodBinOffsetPresent) {
            start = t0Bin + firstGoodBinOffset;
            end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
            runList->at(i).SetDataRange(start, 0);
            runList->at(i).SetDataRange(end, 1);
          }
          // handle addruns
          for (UInt_t j=1; j<runList->at(i).GetRunNameSize(); j++) {
            runName = runList->at(i).GetRunName(j);
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetAddT0Bin(t0Bin, j-1, 0);
          }
          // handle backward histo
          // get histo number
          histoNo = runList->at(i).GetBackwardHistoNo();
          runName = runList->at(i).GetRunName();
          // get bin position of maximal data
          t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
          // set t0 to maximum data position
          runList->at(i).SetT0Bin(t0Bin, 1);
          // set data range as well if firstGoodBinOffset is given
          if (firstGoodBinOffsetPresent) {
            start = t0Bin + firstGoodBinOffset;
            end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
            runList->at(i).SetDataRange(start, 2);
            runList->at(i).SetDataRange(end, 3);
          }
          // handle addruns
          for (UInt_t j=1; j<runList->at(i).GetRunNameSize(); j++) {
            runName = runList->at(i).GetRunName(j);
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetAddT0Bin(t0Bin, j-1, 1);
          }
        } else if ((runList->at(i).GetRunNameSize() == 1) && (runList->at(i).GetForwardHistoNoSize() > 1)) { // no addruns / grouping
          // handle forward histo
          for (UInt_t j=0; j<runList->at(i).GetForwardHistoNoSize(); j++) {
            // get histo number
            histoNo = runList->at(i).GetForwardHistoNo(j);
            runName = runList->at(i).GetRunName();
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetT0Bin(t0Bin, 2*j);
            if (firstGoodBinOffsetPresent && (j==0)) {
              start = t0Bin + firstGoodBinOffset;
              end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
              runList->at(i).SetDataRange(start, 0);
              runList->at(i).SetDataRange(end, 1);
            }
          }
          // handle backward histo
          for (UInt_t j=0; j<runList->at(i).GetBackwardHistoNoSize(); j++) {
            // get histo number
            histoNo = runList->at(i).GetBackwardHistoNo(j);
            runName = runList->at(i).GetRunName();
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetT0Bin(t0Bin, 2*j+1);
            if (firstGoodBinOffsetPresent && (j==0)) {
              start = t0Bin + firstGoodBinOffset;
              end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
              runList->at(i).SetDataRange(start, 2);
              runList->at(i).SetDataRange(end, 3);
            }
          }
        } else { // addruns / grouping
          // handle forward histo
          for (UInt_t j=0; j<runList->at(i).GetForwardHistoNoSize(); j++) {
            // get histo number
            histoNo = runList->at(i).GetForwardHistoNo(j);
            runName = runList->at(i).GetRunName();
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetT0Bin(t0Bin, 2*j);
            if (firstGoodBinOffsetPresent && (j==0)) {
              start = t0Bin + firstGoodBinOffset;
              end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
              runList->at(i).SetDataRange(start, 0);
              runList->at(i).SetDataRange(end, 1);
            }
            // handle addruns
            for (UInt_t k=1; k<runList->at(i).GetRunNameSize(); k++) {
              runName = runList->at(i).GetRunName(k);
              // get bin position of maximal data
              t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
              // set t0 to maximum data position
              runList->at(i).SetAddT0Bin(t0Bin, k-1, 2*j);
            }
          }
          // handle backward histo
          for (UInt_t j=0; j<runList->at(i).GetBackwardHistoNoSize(); j++) {
            // get histo number
            histoNo = runList->at(i).GetBackwardHistoNo(j);
            runName = runList->at(i).GetRunName();
            // get bin position of maximal data
            t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
            // set t0 to maximum data position
            runList->at(i).SetT0Bin(t0Bin, 2*j+1);
            if (firstGoodBinOffsetPresent && (j==0)) {
              start = t0Bin + firstGoodBinOffset;
              end   = dataHandler->GetRunData(*runName)->GetDataBin(histoNo)->size();
              runList->at(i).SetDataRange(start, 2);
              runList->at(i).SetDataRange(end, 3);
            }
            // handle addruns
            for (UInt_t k=1; k<runList->at(i).GetRunNameSize(); k++) {
              runName = runList->at(i).GetRunName(k);
              // get bin position of maximal data
              t0Bin = musrt0_getMaxBin(dataHandler->GetRunData(*runName)->GetDataBin(histoNo));
              // set t0 to maximum data position
              runList->at(i).SetAddT0Bin(t0Bin, k-1, 2*j+1);
            }
          }
        }
        break;
      default:
        break;
      }
    }
  } else {

    // set t0's, data-range and bkg-range. There are 4 different cases, namely:
    // 1. no addruns / no grouping of histos
    // 2. addruns / no grouping of histos
    // 3. no addruns / grouping of histos
    // 4. addruns / grouping of histos
    // case 1 is different form the others since t0, data-, and bkg-range can be collected for the given histogram
    // cases 2-4 the procedure will be the following:
    //   1) set for each given histogram the t0's
    //   2) build the added up histogram, i.e. add all runs and/or histograms for the msr-run.
    //   3) set the data-, and bkg-range
    if (success) {
      // generate Root application needed for PMusrCanvas
      TApplication app("App", &argc, argv);

      PMusrT0Data musrT0Data;
      std::vector<PRawRunData*> rawRunData;
      PIntVector forwardHistos;
      PIntVector backwardHistos;
      // generate vector of all necessary PMusrT0 objects
      for (UInt_t i=0; i<runList->size(); i++) {
        fitType = runList->at(i).GetFitType();
        if (fitType == -1) { // i.e. not found in the RUN block, check the GLOBAL block
          fitType = msrHandler->GetMsrGlobal()->GetFitType();
        }
        switch (fitType) {
        case MSR_FITTYPE_SINGLE_HISTO:
        case MSR_FITTYPE_SINGLE_HISTO_RRF:
        case MSR_FITTYPE_MU_MINUS:
          if ((runList->at(i).GetRunNameSize() == 1) && (runList->at(i).GetForwardHistoNoSize() == 1)) { // no addruns / no grouping
            // feed necessary data
            musrT0Data.InitData();
            musrT0Data.SetSingleHisto(true);
            rawRunData.clear();
            rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(0))));
            musrT0Data.SetRawRunData(rawRunData);
            // feed data t0 if present
            if (rawRunData[0]->IsPresent(runList->at(i).GetForwardHistoNo(0))) {
              musrT0Data.SetT0BinData((UInt_t)rawRunData[0]->GetT0Bin(runList->at(i).GetForwardHistoNo(0)));
            }
            musrT0Data.SetRunNo(i);
            musrT0Data.SetAddRunIdx(0); // no addruns
            musrT0Data.SetHistoNoIdx(0);
            forwardHistos.clear();
            forwardHistos.push_back(runList->at(i).GetForwardHistoNo(0));
            musrT0Data.SetHistoNo(forwardHistos);
            musrT0Data.SetDetectorTag(PMUSRT0_FORWARD);
            musrT0Data.SetCmdTag(PMUSRT0_GET_T0_DATA_AND_BKG_RANGE);
            // execute cmd
            if (!musrt0_item(app, msrHandler, musrT0Data, 0, timeout)) {
              musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
              exit(0);
            }
          } else {
            if ((runList->at(i).GetRunNameSize() > 1) && (runList->at(i).GetForwardHistoNoSize() == 1)) { // addruns / no grouping
              // feed necessary data
              musrT0Data.InitData();
              musrT0Data.SetSingleHisto(true);
              musrT0Data.SetRunNo(i);
              musrT0Data.SetHistoNoIdx(0);
              forwardHistos.clear();
              forwardHistos.push_back(runList->at(i).GetForwardHistoNo(0));
              musrT0Data.SetHistoNo(forwardHistos);
              musrT0Data.SetDetectorTag(PMUSRT0_FORWARD);
              musrT0Data.SetCmdTag(PMUSRT0_GET_T0);
              rawRunData.clear();
              for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++)
                rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(j))));
              musrT0Data.SetRawRunData(rawRunData);
              for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++) {
                // feed data t0 if present
                if (rawRunData[j]->IsPresent(runList->at(i).GetForwardHistoNo(0))) {
                  musrT0Data.SetT0BinData((UInt_t)rawRunData[j]->GetT0Bin(runList->at(i).GetForwardHistoNo(0)));
                }
                // feed necessary data
                musrT0Data.SetAddRunIdx(j); // addruns
                if (!musrt0_item(app, msrHandler, musrT0Data, j, timeout)) {
                  musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
                  exit(0);
                }
              }
            } else if ((runList->at(i).GetRunNameSize() == 1) && (runList->at(i).GetForwardHistoNoSize() > 1)) { // no addruns / grouping
              // feed necessary data
              musrT0Data.InitData();
              musrT0Data.SetSingleHisto(true);
              musrT0Data.SetRunNo(i);
              forwardHistos.clear();
              for (UInt_t j=0; j<runList->at(i).GetForwardHistoNoSize(); j++)
                forwardHistos.push_back(runList->at(i).GetForwardHistoNo(j));
              musrT0Data.SetHistoNo(forwardHistos);
              musrT0Data.SetDetectorTag(PMUSRT0_FORWARD);
              musrT0Data.SetCmdTag(PMUSRT0_GET_T0);
              rawRunData.clear();
              rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(0))));
              musrT0Data.SetRawRunData(rawRunData);
              musrT0Data.SetAddRunIdx(0);
              for (UInt_t j=0; j<runList->at(i).GetForwardHistoNoSize(); j++) {
                // feed data t0 if present
                if (rawRunData[0]->IsPresent(runList->at(i).GetForwardHistoNo(j))) {
                  musrT0Data.SetT0BinData((UInt_t)rawRunData[0]->GetT0Bin(runList->at(i).GetForwardHistoNo(j)));
                }
                // feed necessary data
                musrT0Data.SetHistoNoIdx(j);
                if (!musrt0_item(app, msrHandler, musrT0Data, 0, timeout)) {
                  musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
                  exit(0);
                }
              }
            } else { // addruns / grouping
              // feed necessary data
              musrT0Data.InitData();
              musrT0Data.SetSingleHisto(true);
              musrT0Data.SetRunNo(i);
              forwardHistos.clear();
              for (UInt_t j=0; j<runList->at(i).GetForwardHistoNoSize(); j++)
                forwardHistos.push_back(runList->at(i).GetForwardHistoNo(j));
              musrT0Data.SetHistoNo(forwardHistos);
              musrT0Data.SetDetectorTag(PMUSRT0_FORWARD);
              musrT0Data.SetCmdTag(PMUSRT0_GET_T0);
              rawRunData.clear();
              for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++)
                rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(j))));
              musrT0Data.SetRawRunData(rawRunData);
              for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++) { // addrun / grouping
                musrT0Data.SetAddRunIdx(j); // addruns
                for (UInt_t k=0; k<runList->at(i).GetForwardHistoNoSize(); k++) { // forward histo grouping
                  // feed data t0 if present
                  if (rawRunData[j]->IsPresent(runList->at(i).GetForwardHistoNo(k))) {
                    musrT0Data.SetT0BinData((UInt_t)rawRunData[j]->GetT0Bin(runList->at(i).GetForwardHistoNo(k)));
                  }
                  // feed necessary data
                  musrT0Data.SetHistoNoIdx(k);
                  if (!musrt0_item(app, msrHandler, musrT0Data, j, timeout)) {
                    musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
                    exit(0);
                  }
                }
              }
            }
            // get data- and bkg-range
            musrT0Data.SetCmdTag(PMUSRT0_GET_DATA_AND_BKG_RANGE);
            // feed all t0's
            for (UInt_t j=0; j<runList->at(i).GetT0BinSize(); j++) {
              musrT0Data.SetT0Bin((UInt_t)runList->at(i).GetT0Bin(j), j);
              for (UInt_t k=0; k<runList->at(i).GetAddT0BinEntries(); k++) {
                musrT0Data.SetAddT0Bin((UInt_t)runList->at(i).GetAddT0Bin(k, j), k, j);
              }
            }
            if (!musrt0_item(app, msrHandler, musrT0Data, 0, timeout)) {
              musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
              exit(0);
            }
          }
          break;
        case MSR_FITTYPE_ASYM:
        case MSR_FITTYPE_BNMR:
        case MSR_FITTYPE_ASYM_RRF:
          if ((runList->at(i).GetRunNameSize() == 1) && (runList->at(i).GetForwardHistoNoSize() == 1)) { // no addruns / no grouping
            // feed necessary data forward
            musrT0Data.InitData();
            musrT0Data.SetSingleHisto(false);
            rawRunData.clear();
            rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(0))));
            musrT0Data.SetRawRunData(rawRunData);
            // feed data t0 if present
            if (rawRunData[0]->IsPresent(runList->at(i).GetForwardHistoNo(0))) {
              musrT0Data.SetT0BinData((UInt_t)rawRunData[0]->GetT0Bin(runList->at(i).GetForwardHistoNo(0)));
            }
            musrT0Data.SetRunNo(i);
            musrT0Data.SetAddRunIdx(0); // no addruns
            musrT0Data.SetHistoNoIdx(0);
            forwardHistos.clear();
            forwardHistos.push_back(runList->at(i).GetForwardHistoNo(0));
            musrT0Data.SetHistoNo(forwardHistos);
            musrT0Data.SetDetectorTag(PMUSRT0_FORWARD);
            musrT0Data.SetCmdTag(PMUSRT0_GET_T0_DATA_AND_BKG_RANGE);
            // execute cmd
            if (!musrt0_item(app, msrHandler, musrT0Data, 0, timeout)) {
              musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
              exit(0);
            }
            // feed necessary data backward
            musrT0Data.InitData();
            musrT0Data.SetSingleHisto(false);
            rawRunData.clear();
            rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(0))));
            musrT0Data.SetRawRunData(rawRunData);
            // feed data t0 if present
            if (rawRunData[0]->IsPresent(runList->at(i).GetForwardHistoNo(0))) {
              musrT0Data.SetT0BinData((UInt_t)rawRunData[0]->GetT0Bin(runList->at(i).GetForwardHistoNo(0)));
            }
            musrT0Data.SetRunNo(i);
            musrT0Data.SetAddRunIdx(0); // no addruns
            musrT0Data.SetHistoNoIdx(0);
            backwardHistos.clear();
            backwardHistos.push_back(runList->at(i).GetBackwardHistoNo(0));
            musrT0Data.SetHistoNo(backwardHistos);
            musrT0Data.SetDetectorTag(PMUSRT0_BACKWARD);
            musrT0Data.SetCmdTag(PMUSRT0_GET_T0_DATA_AND_BKG_RANGE);
            // execute cmd
            if (!musrt0_item(app, msrHandler, musrT0Data, 0, timeout)) {
              musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
              exit(0);
            }
          } else {
            // get t0's
            if ((runList->at(i).GetRunNameSize() > 1) && (runList->at(i).GetForwardHistoNoSize() == 1)) { // addruns / no grouping
              // feed necessary forward data
              musrT0Data.InitData();
              musrT0Data.SetSingleHisto(false);
              musrT0Data.SetRunNo(i);
              musrT0Data.SetHistoNoIdx(0);
              forwardHistos.clear();
              forwardHistos.push_back(runList->at(i).GetForwardHistoNo(0));
              musrT0Data.SetHistoNo(forwardHistos);
              musrT0Data.SetDetectorTag(PMUSRT0_FORWARD);
              musrT0Data.SetCmdTag(PMUSRT0_GET_T0);
              rawRunData.clear();
              for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++)
                rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(j))));
              musrT0Data.SetRawRunData(rawRunData);
              for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++) {
                // feed data t0 if present
                if (rawRunData[j]->IsPresent(runList->at(i).GetForwardHistoNo(0))) {
                  musrT0Data.SetT0BinData((UInt_t)rawRunData[j]->GetT0Bin(runList->at(i).GetForwardHistoNo(0)));
                }
                // feed necessary data
                musrT0Data.SetAddRunIdx(j); // addruns
                if (!musrt0_item(app, msrHandler, musrT0Data, j, timeout)) {
                  musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
                  exit(0);
                }
              }
              // feed necessary backward data
              musrT0Data.InitData();
              musrT0Data.SetSingleHisto(false);
              musrT0Data.SetRunNo(i);
              musrT0Data.SetHistoNoIdx(0);
              backwardHistos.clear();
              backwardHistos.push_back(runList->at(i).GetBackwardHistoNo(0));
              musrT0Data.SetHistoNo(backwardHistos);
              musrT0Data.SetDetectorTag(PMUSRT0_BACKWARD);
              musrT0Data.SetCmdTag(PMUSRT0_GET_T0);
              rawRunData.clear();
              for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++)
                rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(j))));
              musrT0Data.SetRawRunData(rawRunData);
              for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++) {
                // feed data t0 if present
                if (rawRunData[j]->IsPresent(runList->at(i).GetForwardHistoNo(0))) {
                  musrT0Data.SetT0BinData((UInt_t)rawRunData[j]->GetT0Bin(runList->at(i).GetForwardHistoNo(0)));
                }
                // feed necessary data
                musrT0Data.SetAddRunIdx(j); // addruns
                if (!musrt0_item(app, msrHandler, musrT0Data, j, timeout)) {
                  musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
                  exit(0);
                }
              }
            } else if ((runList->at(i).GetRunNameSize() == 1) && (runList->at(i).GetForwardHistoNoSize() > 1)) { // no addruns / grouping
              // feed necessary forward data
              musrT0Data.InitData();
              musrT0Data.SetSingleHisto(false);
              musrT0Data.SetRunNo(i);
              forwardHistos.clear();
              for (UInt_t j=0; j<runList->at(i).GetForwardHistoNoSize(); j++)
                forwardHistos.push_back(runList->at(i).GetForwardHistoNo(j));
              musrT0Data.SetHistoNo(forwardHistos);
              musrT0Data.SetDetectorTag(PMUSRT0_FORWARD);
              musrT0Data.SetCmdTag(PMUSRT0_GET_T0);
              rawRunData.clear();
              rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(0))));
              musrT0Data.SetRawRunData(rawRunData);
              musrT0Data.SetAddRunIdx(0);
              for (UInt_t j=0; j<runList->at(i).GetForwardHistoNoSize(); j++) {
                // feed data t0 if present
                if (rawRunData[0]->IsPresent(runList->at(i).GetForwardHistoNo(j))) {
                  musrT0Data.SetT0BinData((UInt_t)rawRunData[0]->GetT0Bin(runList->at(i).GetForwardHistoNo(j)));
                }
                // feed necessary data
                musrT0Data.SetHistoNoIdx(j);
                if (!musrt0_item(app, msrHandler, musrT0Data, 0, timeout)) {
                  musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
                  exit(0);
                }
              }
              // feed necessary backward data
              musrT0Data.InitData();
              musrT0Data.SetSingleHisto(false);
              musrT0Data.SetRunNo(i);
              backwardHistos.clear();
              for (UInt_t j=0; j<runList->at(i).GetBackwardHistoNoSize(); j++)
                backwardHistos.push_back(runList->at(i).GetBackwardHistoNo(j));
              musrT0Data.SetHistoNo(backwardHistos);
              musrT0Data.SetDetectorTag(PMUSRT0_BACKWARD);
              musrT0Data.SetCmdTag(PMUSRT0_GET_T0);
              rawRunData.clear();
              rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(0))));
              musrT0Data.SetRawRunData(rawRunData);
              musrT0Data.SetAddRunIdx(0);
              for (UInt_t j=0; j<runList->at(i).GetBackwardHistoNoSize(); j++) {
                // feed data t0 if present
                if (rawRunData[0]->IsPresent(runList->at(i).GetForwardHistoNo(j))) {
                  musrT0Data.SetT0BinData((UInt_t)rawRunData[0]->GetT0Bin(runList->at(i).GetForwardHistoNo(j)));
                }
                // feed necessary data
                musrT0Data.SetHistoNoIdx(j);
                if (!musrt0_item(app, msrHandler, musrT0Data, 0, timeout)) {
                  musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
                  exit(0);
                }
              }
            } else { // addruns / grouping
              // feed necessary forward data
              musrT0Data.InitData();
              musrT0Data.SetSingleHisto(false);
              musrT0Data.SetRunNo(i);
              forwardHistos.clear();
              for (UInt_t j=0; j<runList->at(i).GetForwardHistoNoSize(); j++)
                forwardHistos.push_back(runList->at(i).GetForwardHistoNo(j));
              musrT0Data.SetHistoNo(forwardHistos);
              musrT0Data.SetDetectorTag(PMUSRT0_FORWARD);
              musrT0Data.SetCmdTag(PMUSRT0_GET_T0);
              rawRunData.clear();
              for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++)
                rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(j))));
              musrT0Data.SetRawRunData(rawRunData);
              for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++) { // addrun / grouping
                musrT0Data.SetAddRunIdx(j); // addruns
                for (UInt_t k=0; k<runList->at(i).GetForwardHistoNoSize(); k++) { // forward histo grouping
                  // feed data t0 if present
                  if (rawRunData[j]->IsPresent(runList->at(i).GetForwardHistoNo(k))) {
                    musrT0Data.SetT0BinData((UInt_t)rawRunData[j]->GetT0Bin(runList->at(i).GetForwardHistoNo(k)));
                  }
                  // feed necessary data
                  musrT0Data.SetHistoNoIdx(k);
                  if (!musrt0_item(app, msrHandler, musrT0Data, j, timeout)) {
                    musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
                    exit(0);
                  }
                }
              }
              // feed necessary backward data
              musrT0Data.InitData();
              musrT0Data.SetSingleHisto(false);
              musrT0Data.SetRunNo(i);
              backwardHistos.clear();
              for (UInt_t j=0; j<runList->at(i).GetBackwardHistoNoSize(); j++)
                backwardHistos.push_back(runList->at(i).GetBackwardHistoNo(j));
              musrT0Data.SetHistoNo(backwardHistos);
              musrT0Data.SetDetectorTag(PMUSRT0_BACKWARD);
              musrT0Data.SetCmdTag(PMUSRT0_GET_T0);
              rawRunData.clear();
              for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++)
                rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(j))));
              musrT0Data.SetRawRunData(rawRunData);
              for (UInt_t j=0; j<runList->at(i).GetRunNameSize(); j++) { // addrun / grouping
                musrT0Data.SetAddRunIdx(j); // addruns
                for (UInt_t k=0; k<runList->at(i).GetBackwardHistoNoSize(); k++) { // backward histo grouping
                  // feed data t0 if present
                  if (rawRunData[j]->IsPresent(runList->at(i).GetForwardHistoNo(k))) {
                    musrT0Data.SetT0BinData((UInt_t)rawRunData[j]->GetT0Bin(runList->at(i).GetForwardHistoNo(k)));
                  }
                  // feed necessary data
                  musrT0Data.SetHistoNoIdx(k);
                  if (!musrt0_item(app, msrHandler, musrT0Data, j, timeout)) {
                    musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
                    exit(0);
                  }
                }
              }
            }
            // get data- and bkg-range
            musrT0Data.SetCmdTag(PMUSRT0_GET_DATA_AND_BKG_RANGE);
            // feed all t0's
            for (UInt_t j=0; j<runList->at(i).GetT0BinSize(); j++) {
              musrT0Data.SetT0Bin((UInt_t)runList->at(i).GetT0Bin(j), j);
              for (UInt_t k=0; k<runList->at(i).GetAddT0BinEntries(); k++) {
                musrT0Data.SetAddT0Bin((UInt_t)runList->at(i).GetAddT0Bin(k, j), k, j);
              }
            }
            musrT0Data.SetHistoNo(forwardHistos);
            musrT0Data.SetDetectorTag(PMUSRT0_FORWARD);
            if (!musrt0_item(app, msrHandler, musrT0Data, 0, timeout)) {
              musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
              exit(0);
            }
            musrT0Data.SetHistoNo(backwardHistos);
            musrT0Data.SetDetectorTag(PMUSRT0_BACKWARD);
            if (!musrt0_item(app, msrHandler, musrT0Data, 0, timeout)) {
              musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
              exit(0);
            }
          }
          break;
        default:
          break;
        }
      }
      // cleanup
      rawRunData.clear();
      forwardHistos.clear();
      backwardHistos.clear();
    }
  }

  // write msr-file
  msrHandler->WriteMsrLogFile(false);

  // swap msr- and mlog-file
  // copy msr-file -> __temp.msr
  gSystem->CopyFile(filename, "__temp.msr", kTRUE);
  // copy mlog-file -> msr-file
  TString fln = TString(filename);
  Char_t ext[32];
  strcpy(ext, ".mlog");
  fln.ReplaceAll(".msr", 4, ext, strlen(ext));
  gSystem->CopyFile(fln.Data(), filename, kTRUE);
  // copy __temp.msr -> mlog-file
  gSystem->CopyFile("__temp.msr", fln.Data(), kTRUE);
  // delete __temp.msr
  gSystem->Exec("rm __temp.msr");

  // clean up
  musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);

  return PMUSR_SUCCESS;
}
