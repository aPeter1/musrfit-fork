/***************************************************************************

  musrview.cpp

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

#include <iostream>
using namespace std;

#include <TApplication.h>
#include <TSAXParser.h>

#include "PMusr.h"
#include "PStartupHandler.h"
#include "PMsrHandler.h"
#include "PRunDataHandler.h"
#include "PRunListCollection.h"
#include "PMusrCanvas.h"


//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void musrview_syntax()
{
  cout << endl << "usage: musrview <msr-file> | --version | --help";
  cout << endl << "       <msr-file>: msr input file";
  cout << endl << "       'musrview <msr-file>' will execute msrfit";
  cout << endl << "       'musrview' or 'msrfit --help' will show this help";
  cout << endl << "       'musrview --version' will print the msrfit version";
  cout << endl << endl;
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
        cout << endl << "musrview version: $Id$";
        cout << endl << endl;
        return PMUSR_SUCCESS;
      } else if (strstr(argv[1], "--help")) {
        show_syntax = true;
      } else {
        // check if filename has extension msr
        if (!strstr(argv[1], ".msr")) {
          cout << endl << "**ERROR** " << argv[1] << " is not a msr-file!" << endl;
          show_syntax = true;
        }
      }
      break;
    default:
      show_syntax = true;
  }

  if (show_syntax) {
    musrview_syntax();
    return PMUSR_WRONG_STARTUP_SYNTAX;
  }

  // read startup file
  TSAXParser *saxParser = new TSAXParser();
  PStartupHandler *startupHandler = new PStartupHandler();
  saxParser->ConnectToHandler("PStartupHandler", startupHandler);
  status = saxParser->ParseFile("musrfit_startup.xml");
  // check for parse errors
  if (status) { // error
    cout << endl << "**ERROR** reading/parsing musrfit_startup.xml. Fix it.";
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
    return PMUSR_WRONG_STARTUP_SYNTAX;
  }

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
      default:
        cout << endl << "**UNKNOWN ERROR** when trying to read the msr-file" << endl << endl;
        break;
    }
    return status;
  }

  // read all the necessary runs (raw data)
  PRunDataHandler *dataHandler = new PRunDataHandler(msrHandler, startupHandler->GetDataPathList());
  success = dataHandler->IsAllDataAvailable();
  if (!success) {
    cout << endl << "**ERROR** Couldn't read all data files, will quit ..." << endl;
  }

  // generate the necessary fit histogramms for the view
  PRunListCollection *runListCollection = 0;
  if (success) {
    // feed all the necessary histogramms for the view
    runListCollection = new PRunListCollection(msrHandler, dataHandler);
    for (unsigned int i=0; i < msrHandler->GetMsrRunList()->size(); i++) {
      success = runListCollection->Add(i);
      if (!success) {
        cout << endl << "**ERROR** Couldn't handle run no " << i << " ";
        cout << (*msrHandler->GetMsrRunList())[i].fRunName.Data();
        break;
      }
    }
  }

  if (success) {
    // generate Root application needed for PMusrCanvas
    TApplication app("App", &argc, argv);

    PMusrCanvas *musrCanvas = new PMusrCanvas(msrHandler->GetMsrTitle()->Data(), 10, 10, 800, 600,
                                  startupHandler->GetMarkerList(), startupHandler->GetColorList());
    if (!musrCanvas->IsValid()) {
      cout << endl << "**SEVERE ERROR** Couldn't invoke all necessary objects, will quit.";
      cout << endl;
      return -1;
    }
    musrCanvas->SetParameterList(*msrHandler->GetMsrParamList());
    musrCanvas->SetTheoryList(*msrHandler->GetMsrTheory());
    musrCanvas->SetFunctionList(*msrHandler->GetMsrFunctions());
    musrCanvas->UpdateParamTheoryPad();

    musrCanvas->Connect("Done(Int_t)", "TApplication", &app, "Terminate(Int_t)");

    app.Run();

    // clean up
    if (musrCanvas) {
      delete musrCanvas;
      musrCanvas = 0;
    }
  }

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
  if (runListCollection) {
    delete runListCollection;
    runListCollection = 0;
  }

  return 0;
}
