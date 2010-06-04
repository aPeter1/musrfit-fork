/***************************************************************************

  musrt0.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
using namespace std;

#include <TApplication.h>
#include <TSAXParser.h>
#include <TROOT.h>
#include <TSystem.h>

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
  cout << endl << "usage: musrt0 <msr-file> | --version | --help";
  cout << endl << "       <msr-file>: msr input file";
  cout << endl << "       'musrt0 <msr-file>' will execute musrt0";
  cout << endl << "       'musrt0' or 'musrt0 --help' will show this help";
  cout << endl << "       'musrt0 --version' will print the musrt0 version";
  cout << endl << endl;
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
Bool_t musrt0_item(TApplication &app, PMsrHandler *msrHandler, PMusrT0Data &data, UInt_t idx)
{
  PMusrT0 *musrT0 = new PMusrT0(data);

  // check if the musrT0 object could be invoked
  if (musrT0 == 0) {
    cerr << endl << ">> **ERROR** Couldn't invoke musrT0 ...";
    cerr << endl << ">> run name " << data.GetRawRunData(idx)->GetRunName()->Data();
    cerr << endl;
    return false;
  }

  // check if the musrT0 object is valid
  if (!musrT0->IsValid()) {
    cerr << endl << ">> **ERROR** invalid item found! (idx=" << idx << ")";
    cerr << endl;
    return false;
  }

  // set the msr-file handler. The handler cannot be transfered at construction time since rootcint is not able to handle the PMsrHandler class
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
  if (musrT0->GetStatus() == 1)
    result = false;
  else
    result = true;

  // disconnect all SIGNALS and SLOTS connected t0 musrT0
  musrT0->Disconnect(musrT0);

  // cleanup
  delete musrT0;
  musrT0 = 0;

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
    saxParser = 0;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }
  if (msrHandler) {
    delete msrHandler;
    msrHandler = 0;
  }
  if (dataHandler) {
    delete dataHandler;
    dataHandler = 0;
  }
}

//--------------------------------------------------------------------------
/**
 * <p>The musrt0 program is used to set graphically t0's, data- and background-ranges.
 * For a detailed description/usage of the program, please see
 * \htmlonly <a href="https://intranet.psi.ch/MUSR/MusrFit">musrt0 online help</a>
 * \endhtmlonly
 * \latexonly musrt0 online help: \texttt{https://intranet.psi.ch/MUSR/MusrFit}
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

  switch (argc) {
    case 1:
      show_syntax = true;
      break;
    case 2:
      if (strstr(argv[1], "--version")) {
        cout << endl << "musrt0 version: $Id$";
        cout << endl << endl;
        return PMUSR_SUCCESS;
      } else if (strstr(argv[1], "--help")) {
        show_syntax = true;
      } else {
        // check if filename has extension msr or mlog
        if (!strstr(argv[1], ".msr")) {
          cout << endl << "**ERROR** " << argv[1] << " is not a msr-file!" << endl;
          show_syntax = true;
        } else {
          strncpy(filename, argv[1], sizeof(filename));
        }
      }
      break;
    default:
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
    cerr << endl << "**WARNING** couldn't find " << startupHandler->GetStartupFilePath().Data();
    cerr << endl;
    // clean up
    if (saxParser) {
      delete saxParser;
      saxParser = 0;
    }
    if (startupHandler) {
      delete startupHandler;
      startupHandler = 0;
    }
  } else {
    strcpy(startup_path_name, startupHandler->GetStartupFilePath().Data());
    saxParser->ConnectToHandler("PStartupHandler", startupHandler);
    status = saxParser->ParseFile(startup_path_name);
    // check for parse errors
    if (status) { // error
      cerr << endl << "**WARNING** reading/parsing musrfit_startup.xml.";
      cerr << endl;
      // clean up
      if (saxParser) {
        delete saxParser;
        saxParser = 0;
      }
      if (startupHandler) {
        delete startupHandler;
        startupHandler = 0;
      }
    }
  }

  // read msr-file
  PMsrHandler *msrHandler = new PMsrHandler(filename);
  status = msrHandler->ReadMsrFile();
  if (status != PMUSR_SUCCESS) {
    switch (status) {
      case PMUSR_MSR_FILE_NOT_FOUND:
        cout << endl << "**ERROR** couldn't find '" << filename << "'" << endl << endl;
        break;
      case PMUSR_MSR_SYNTAX_ERROR:
        cout << endl << "**SYNTAX ERROR** in file " << filename << ", full stop here." << endl << endl;
        break;
      default:
        cout << endl << "**UNKNOWN ERROR** when trying to read the msr-file" << endl << endl;
        break;
    }
    return status;
  }
  msrHandler->CopyMsrStatisticBlock(); // just copy the statistics block since no fit is preformed

  // check if the fittype is not NonMusr
  PMsrRunList *runList = msrHandler->GetMsrRunList();
  for (UInt_t i=0; i<runList->size(); i++) {
    if (runList->at(i).GetFitType() == MSR_FITTYPE_NON_MUSR) {
      cout << endl << "**ERROR** t0 setting for NonMusr fittype doesn't make any sense, will quit ..." << endl;
      success = false;
      break;
    }
  }

  // read all the necessary runs (raw data)
  PRunDataHandler *dataHandler = 0;
  if (success) {
    if (startupHandler)
      dataHandler = new PRunDataHandler(msrHandler, startupHandler->GetDataPathList());
    else
      dataHandler = new PRunDataHandler(msrHandler);

    success = dataHandler->IsAllDataAvailable();
    if (!success) {
      cout << endl << "**ERROR** Couldn't read all data files, will quit ..." << endl;
    }
  }


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
    vector<PRawRunData*> rawRunData;
    PIntVector forwardHistos;
    PIntVector backwardHistos;
    // generate vector of all necessary PMusrT0 objects
    for (UInt_t i=0; i<runList->size(); i++) {
      switch (runList->at(i).GetFitType()) {
        case MSR_FITTYPE_SINGLE_HISTO:
          if ((runList->at(i).GetRunNameSize() == 1) && (runList->at(i).GetForwardHistoNoSize() == 1)) { // no addruns / no grouping
            // feed necessary data
            musrT0Data.InitData();
            musrT0Data.SetSingleHisto(true);
            rawRunData.clear();
            rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(0))));
            musrT0Data.SetRawRunData(rawRunData);
            musrT0Data.SetRunNo(i);
            musrT0Data.SetAddRunIdx(0); // no addruns
            musrT0Data.SetHistoNoIdx(0);
            forwardHistos.clear();
            forwardHistos.push_back(runList->at(i).GetForwardHistoNo(0));
            musrT0Data.SetHistoNo(forwardHistos);
            musrT0Data.SetDetectorTag(PMUSRT0_FORWARD);
            musrT0Data.SetCmdTag(PMUSRT0_GET_T0_DATA_AND_BKG_RANGE);
            // execute cmd
            if (!musrt0_item(app, msrHandler, musrT0Data, 0)) {
              musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
              exit(0);
            }
          } else {
            // get t0's
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
                // feed necessary data
                musrT0Data.SetAddRunIdx(j); // addruns
                if (!musrt0_item(app, msrHandler, musrT0Data, j)) {
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
                musrT0Data.SetHistoNoIdx(j);
                if (!musrt0_item(app, msrHandler, musrT0Data, 0)) {
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
                  musrT0Data.SetHistoNoIdx(k);
                  if (!musrt0_item(app, msrHandler, musrT0Data, j)) {
                    musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
                    exit(0);
                  }
                }
              }
            }
            // get data- and bkg-range
            musrT0Data.SetCmdTag(PMUSRT0_GET_DATA_AND_BKG_RANGE);
            // feed all t0's
            for (UInt_t j=0; j<runList->at(i).GetT0Size(); j++) {
              musrT0Data.SetT0(runList->at(i).GetT0(j), j);
              for (UInt_t k=0; k<runList->at(i).GetAddT0Entries(); k++) {
                musrT0Data.SetAddT0(runList->at(i).GetAddT0(k, j), k, j);
              }
            }
            if (!musrt0_item(app, msrHandler, musrT0Data, 0)) {
              musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
              exit(0);
            }
          }
          break;
        case MSR_FITTYPE_ASYM:
          if ((runList->at(i).GetRunNameSize() == 1) && (runList->at(i).GetForwardHistoNoSize() == 1)) { // no addruns / no grouping
            // feed necessary data forward
            musrT0Data.InitData();
            musrT0Data.SetSingleHisto(false);
            rawRunData.clear();
            rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(0))));
            musrT0Data.SetRawRunData(rawRunData);
            musrT0Data.SetRunNo(i);
            musrT0Data.SetAddRunIdx(0); // no addruns
            musrT0Data.SetHistoNoIdx(0);
            forwardHistos.clear();
            forwardHistos.push_back(runList->at(i).GetForwardHistoNo(0));
            musrT0Data.SetHistoNo(forwardHistos);
            musrT0Data.SetDetectorTag(PMUSRT0_FORWARD);
            musrT0Data.SetCmdTag(PMUSRT0_GET_T0_DATA_AND_BKG_RANGE);
            // execute cmd
            if (!musrt0_item(app, msrHandler, musrT0Data, 0)) {
              musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
              exit(0);
            }
            // feed necessary data backward
            musrT0Data.InitData();
            musrT0Data.SetSingleHisto(false);
            rawRunData.clear();
            rawRunData.push_back(dataHandler->GetRunData(*(runList->at(i).GetRunName(0))));
            musrT0Data.SetRawRunData(rawRunData);
            musrT0Data.SetRunNo(i);
            musrT0Data.SetAddRunIdx(0); // no addruns
            musrT0Data.SetHistoNoIdx(0);
            backwardHistos.clear();
            backwardHistos.push_back(runList->at(i).GetBackwardHistoNo(0));
            musrT0Data.SetHistoNo(backwardHistos);
            musrT0Data.SetDetectorTag(PMUSRT0_BACKWARD);
            musrT0Data.SetCmdTag(PMUSRT0_GET_T0_DATA_AND_BKG_RANGE);
            // execute cmd
            if (!musrt0_item(app, msrHandler, musrT0Data, 0)) {
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
                // feed necessary data
                musrT0Data.SetAddRunIdx(j); // addruns
                if (!musrt0_item(app, msrHandler, musrT0Data, j)) {
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
                // feed necessary data
                musrT0Data.SetAddRunIdx(j); // addruns
                if (!musrt0_item(app, msrHandler, musrT0Data, j)) {
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
                musrT0Data.SetHistoNoIdx(j);
                if (!musrt0_item(app, msrHandler, musrT0Data, 0)) {
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
                musrT0Data.SetHistoNoIdx(j);
                if (!musrt0_item(app, msrHandler, musrT0Data, 0)) {
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
                  musrT0Data.SetHistoNoIdx(k);
                  if (!musrt0_item(app, msrHandler, musrT0Data, j)) {
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
                  musrT0Data.SetHistoNoIdx(k);
                  if (!musrt0_item(app, msrHandler, musrT0Data, j)) {
                    musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
                    exit(0);
                  }
                }
              }
            }
            // get data- and bkg-range
            musrT0Data.SetCmdTag(PMUSRT0_GET_DATA_AND_BKG_RANGE);
            // feed all t0's
            for (UInt_t j=0; j<runList->at(i).GetT0Size(); j++) {
              musrT0Data.SetT0(runList->at(i).GetT0(j), j);
              for (UInt_t k=0; k<runList->at(i).GetAddT0Entries(); k++) {
                musrT0Data.SetAddT0(runList->at(i).GetAddT0(k, j), k, j);
              }
            }
            musrT0Data.SetHistoNo(forwardHistos);
            musrT0Data.SetDetectorTag(PMUSRT0_FORWARD);
            if (!musrt0_item(app, msrHandler, musrT0Data, 0)) {
              musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
              exit(0);
            }
            musrT0Data.SetHistoNo(backwardHistos);
            musrT0Data.SetDetectorTag(PMUSRT0_BACKWARD);
            if (!musrt0_item(app, msrHandler, musrT0Data, 0)) {
              musrt0_cleanup(saxParser, startupHandler, msrHandler, dataHandler);
              exit(0);
            }
          }
          break;
        case MSR_FITTYPE_MU_MINUS:
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
