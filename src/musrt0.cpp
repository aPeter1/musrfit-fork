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

#include "PMusr.h"
#include "PStartupHandler.h"
#include "PMsrHandler.h"
#include "PRunDataHandler.h"
#include "PMusrT0.h"


//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void musrt0_syntax()
{
  cout << endl << "usage: musrt0 <msr-file> | --version | --help";
  cout << endl << "       <msr-file>: msr/mlog input file";
  cout << endl << "       'musrt0 <msr-file>' will execute musrt0";
  cout << endl << "       'musrt0' or 'musrt0 --help' will show this help";
  cout << endl << "       'musrt0 --version' will print the musrt0 version";
  cout << endl << endl;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param app
 * \param msrHandler
 * \param rawRunData
 * \param histoNo
 * \param runNo
 * \param detectorTag 0=forward, 1=backward, 2=left, 3=right
 */
bool musrt0_item(TApplication &app, PMsrHandler *msrHandler, PRawRunData *rawRunData,
                 unsigned int histoNo, unsigned int runNo, unsigned int detectorTag)
{
  PMusrT0 *musrT0 = new PMusrT0(rawRunData, histoNo, runNo, detectorTag);

  if (musrT0 == 0) {
    cout << endl << "**ERROR** Couldn't invoke musrT0 ...";
    cout << endl << "          run name " << rawRunData->fRunName.Data();
    cout << endl << "          histo No " << histoNo;
    cout << endl;
    return false;
  }

  musrT0->SetMsrHandler(msrHandler);

  musrT0->Connect("Done(Int_t)", "TApplication", &app, "Terminate(Int_t)");

  app.Run(true); // true needed that Run will return after quit
  delete musrT0;
  musrT0 = 0;

  return true;
}

//--------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  bool show_syntax = false;
  int  status;
  bool success = true;

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
        if (!strstr(argv[1], ".msr") && !strstr(argv[1], ".mlog")) {
          cout << endl << "**ERROR** " << argv[1] << " is not a msr/mlog-file!" << endl;
          show_syntax = true;
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
  char startup_path_name[128];
  TSAXParser *saxParser = new TSAXParser();
  PStartupHandler *startupHandler = new PStartupHandler();
  strcpy(startup_path_name, startupHandler->GetStartupFilePath().Data());
  saxParser->ConnectToHandler("PStartupHandler", startupHandler);
  status = saxParser->ParseFile(startup_path_name);
  // check for parse errors
  if (status) { // error
    cout << endl << "**WARNING** reading/parsing musrfit_startup.xml failed.";
    cout << endl;
/*
    // clean up
    if (saxParser) {
      delete saxParser;
      saxParser = 0;
    }
    if (startupHandler) {
      delete startupHandler;
      startupHandler = 0;
    }
*/
  }
  startupHandler->CheckLists();

  // read msr-file
  PMsrHandler *msrHandler = new PMsrHandler(argv[1]);
  status = msrHandler->ReadMsrFile();
  if (status != PMUSR_SUCCESS) {
    switch (status) {
      case PMUSR_MSR_FILE_NOT_FOUND:
        cout << endl << "**ERROR** couldn't find '" << argv[1] << "'" << endl << endl;
        break;
      case PMUSR_MSR_SYNTAX_ERROR:
        cout << endl << "**SYNTAX ERROR** in file " << argv[1] << ", full stop here." << endl << endl;
        break;
      default:
        cout << endl << "**UNKNOWN ERROR** when trying to read the msr-file" << endl << endl;
        break;
    }
    return status;
  }

  // check if the fittype is not NonMusr
  PMsrRunList *runList = msrHandler->GetMsrRunList();
  for (unsigned int i=0; i<runList->size(); i++) {
    if (runList->at(i).fFitType == MSR_FITTYPE_NON_MUSR) {
      cout << endl << "**ERROR** t0 setting for NonMusr fittype doesn't make any sense, will quit ..." << endl;
      success = false;
      break;
    }
  }

  // read all the necessary runs (raw data)
  PRunDataHandler *dataHandler;
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


  if (success) {
    // generate Root application needed for PMusrCanvas
    TApplication app("App", &argc, argv);

    // generate vector of all necessary PMusrT0 objects
    for (unsigned int i=0; i<runList->size(); i++) {
      switch (runList->at(i).fFitType) {
        case MSR_FITTYPE_SINGLE_HISTO:
          for (unsigned int j=0; j<runList->at(i).fRunName.size(); j++) { // necessary in case of ADDRUN
            if (!musrt0_item(app, msrHandler, dataHandler->GetRunData(runList->at(i).fRunName[j]), i, runList->at(i).fForwardHistoNo, 0))
              exit(0);
          }
          break;
        case MSR_FITTYPE_ASYM:
          for (unsigned int j=0; j<runList->at(i).fRunName.size(); j++) { // necessary in case of ADDRUN
            if (!musrt0_item(app, msrHandler, dataHandler->GetRunData(runList->at(i).fRunName[j]), i, runList->at(i).fForwardHistoNo, 0))
              exit(0);
            if (!musrt0_item(app, msrHandler, dataHandler->GetRunData(runList->at(i).fRunName[j]), i, runList->at(i).fBackwardHistoNo, 1))
              exit(0);
          }
          break;
        case MSR_FITTYPE_ASYM_RRF:
          for (unsigned int j=0; j<runList->at(i).fRunName.size(); j++) { // necessary in case of ADDRUN
            if (!musrt0_item(app, msrHandler, dataHandler->GetRunData(runList->at(i).fRunName[j]), i, runList->at(i).fForwardHistoNo, 0))
              exit(0);
            if (!musrt0_item(app, msrHandler, dataHandler->GetRunData(runList->at(i).fRunName[j]), i, runList->at(i).fBackwardHistoNo, 1))
              exit(0);
            if (!musrt0_item(app, msrHandler, dataHandler->GetRunData(runList->at(i).fRunName[j]), i, runList->at(i).fRightHistoNo, 2))
              exit(0);
            if (!musrt0_item(app, msrHandler, dataHandler->GetRunData(runList->at(i).fRunName[j]), i, runList->at(i).fLeftHistoNo, 3))
              exit(0);
          }
          break;
        default:
          break;
      }
    }

/*
    vector<PMusrT0*> t0CanvasVector;
    PMusrT0 *t0Canvas;

    bool ok = true;
    for (unsigned int i=0; i<msrHandler->GetMsrPlotList()->size(); i++) {

      if (startupHandler)
        musrCanvas = new PMusrCanvas(i, msrHandler->GetMsrTitle()->Data(), 
                                     10+i*100, 10+i*100, 800, 600,
                                     startupHandler->GetFourierDefaults(),
                                     startupHandler->GetMarkerList(),
                                     startupHandler->GetColorList());
      else
        musrCanvas = new PMusrCanvas(i, msrHandler->GetMsrTitle()->Data(), 
                                     10+i*100, 10+i*100, 800, 600);

      if (!musrCanvas->IsValid()) {
        cout << endl << "**SEVERE ERROR** Couldn't invoke all necessary objects, will quit.";
        cout << endl;
        ok = false;
        break;
      }
      // connect signal/slot
      TQObject::Connect("TCanvas", "Closed()", "PMusrCanvas", musrCanvas, "LastCanvasClosed()");

      // ugly but rootcint cannot handle the spirit-parser framework
      musrCanvas->SetMsrHandler(msrHandler);
      musrCanvas->SetRunListCollection(runListCollection);

      if (!musrCanvas->IsValid()) { // something went wrong
        ok = false;
        break;
      }

      musrCanvas->Connect("Done(Int_t)", "TApplication", &app, "Terminate(Int_t)");

      // keep musrCanvas objects
      canvasVector.push_back(musrCanvas);
    }

    // check that everything is ok
    if (ok)
      app.Run(true); // true needed that Run will return after quit so that cleanup works

    // clean up
cout << endl << "clean up canvas vector ...";
    char canvasName[32];
    for (unsigned int i=0; i<canvasVector.size(); i++) {
      // check if canvas is still there before calling the destructor **TO BE DONE**
      sprintf(canvasName, "fMainCanvas%d", i);
cout << endl << ">> canvasName=" << canvasName << ", canvasVector[" << i << "]=" << canvasVector[i];
      if (gROOT->GetListOfCanvases()->FindObject(canvasName) != 0) {
cout << endl << ">> canvasName=" << canvasName << ", found ...";
cout << endl;
        canvasVector[i]->~PMusrCanvas();
      } else {
cout << endl << ">> canvasName=" << canvasName << ", NOT found ...";
cout << endl;
      }
    }
    canvasVector.empty();
*/
  }
cout << endl;

  // clean up
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

  return 0;
}
