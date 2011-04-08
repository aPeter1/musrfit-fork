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
#include "PRunListCollection.h"
#include "PMusrCanvas.h"


//--------------------------------------------------------------------------
/**
 * <p>Sends the usage description to the standard output.
 */
void musrview_syntax()
{
  cout << endl << "usage: musrview <msr-file> [--<graphic-format-extension>] | --version | --help";
  cout << endl << "       <msr-file>: msr/mlog input file";
  cout << endl << "       'musrview <msr-file>' will execute musrview";
  cout << endl << "       --<graphic-format-extension>: ";
  cout << endl << "           will produce a graphics-output-file without starting a root session.";
  cout << endl << "           the name is based on the <msr-file>, e.g. 3310.msr -> 3310_0.png";
  cout << endl << "           supported graphic-format-extension:";
  cout << endl << "           eps, pdf, gif, jpg, png, svg, xpm, root";
  cout << endl << "           example: musrview 3310.msr --png, will produce a files 3310_X.png";
  cout << endl << "                    where 'X' stands for the plot number (starting form 0)";
  cout << endl << "       'musrview' or 'musrview --help' will show this help";
  cout << endl << "       'musrview --version' will print the musrview version";
  cout << endl << endl;
}

//--------------------------------------------------------------------------
/**
 * <p>The musrview program is used to show muSR fit results in graphical form.
 * From it also Fourier transforms, difference between data an theory can be formed.
 * For a detailed description/usage of the program, please see
 * \htmlonly <a href="https://intranet.psi.ch/MUSR/MusrFit">musrview online help</a>
 * \endhtmlonly
 * \latexonly musrview online help: \texttt{https://intranet.psi.ch/MUSR/MusrFit}
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
int main(int argc, char *argv[])
{
  bool show_syntax = false;
  int  status;
  bool success = true;
  char fileName[128];
  bool graphicsOutput = false;
  char graphicsExtension[128];


  // check input arguments
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
        // check if filename has extension msr or mlog
        if (!strstr(argv[1], ".msr") && !strstr(argv[1], ".mlog")) {
          cerr << endl << ">> musrview **ERROR** " << argv[1] << " is not a msr/mlog-file, nor is it a supported graphics extension." << endl;
          show_syntax = true;
        } else {
          strcpy(fileName, argv[1]);
        }
      }
      break;
    case 3:
      if (!strcmp(argv[1], "--eps") || !strcmp(argv[1], "--pdf") || !strcmp(argv[1], "--gif") ||
          !strcmp(argv[1], "--jpg") || !strcmp(argv[1], "--png") || !strcmp(argv[1], "--svg") ||
          !strcmp(argv[1], "--xpm") || !strcmp(argv[1], "--root")) {
        graphicsOutput = true;
        strcpy(graphicsExtension, argv[1]+2);
        // check if filename has extension msr or mlog
        if (!strstr(argv[2], ".msr") && !strstr(argv[2], ".mlog")) {
          cerr << endl << ">> musrview **ERROR** " << argv[2] << " is not a msr/mlog-file, nor is it a supported graphics extension." << endl;
          show_syntax = true;
        } else {
          strcpy(fileName, argv[2]);
        }
      } else if (!strcmp(argv[2], "--eps") || !strcmp(argv[2], "--pdf") || !strcmp(argv[2], "--gif") ||
                 !strcmp(argv[2], "--jpg") || !strcmp(argv[2], "--png") || !strcmp(argv[2], "--svg") ||
                 !strcmp(argv[2], "--xpm") || !strcmp(argv[2], "--root")) {
        graphicsOutput = true;
        strcpy(graphicsExtension, argv[2]+2);
        // check if filename has extension msr or mlog
        if (!strstr(argv[1], ".msr") && !strstr(argv[1], ".mlog")) {
          cerr << endl << ">> musrview **ERROR** " << argv[1] << " is not a msr/mlog-file, nor is it a supported graphics extension." << endl;
          show_syntax = true;
        } else {
          strcpy(fileName, argv[1]);
        }
      } else {
        show_syntax = true;
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
  char startup_path_name[128];
  TSAXParser *saxParser = new TSAXParser();
  PStartupHandler *startupHandler = new PStartupHandler();
  if (!startupHandler->StartupFileFound()) {
    cerr << endl << ">> musrview **WARNING** couldn't find " << startupHandler->GetStartupFilePath().Data();
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
      cerr << endl << ">> musrview **WARNING** reading/parsing musrfit_startup.xml.";
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
    startupHandler->CheckLists();
  }

  // read msr-file
  PMsrHandler *msrHandler = new PMsrHandler(fileName);
  status = msrHandler->ReadMsrFile();
  if (status != PMUSR_SUCCESS) {
    switch (status) {
      case PMUSR_MSR_FILE_NOT_FOUND:
        cerr << endl << ">> musrview **ERROR** couldn't find '" << fileName << "'" << endl << endl;
        break;
      case PMUSR_MSR_SYNTAX_ERROR:
        cerr << endl << ">> musrview **SYNTAX ERROR** in file " << fileName << ", full stop here." << endl << endl;
        break;
      default:
        cerr << endl << ">> musrview **UNKNOWN ERROR** when trying to read the msr-file" << endl << endl;
        break;
    }
    return status;
  }
  // make a plot list vector
  PMsrPlotList *msrPlotList = msrHandler->GetMsrPlotList();
  PIntVector plotList;
  bool runPresent;
  for (unsigned int i=0; i<msrPlotList->size(); i++) {
    for (unsigned int j=0; j<msrPlotList->at(i).fRuns.size(); j++) {
      // check that run is not already in the plotList
      runPresent = false;
      for (unsigned int k=0; k<plotList.size(); k++) {
        if (msrPlotList->at(i).fRuns[j] == static_cast<int>(plotList[k])) {
          runPresent = true;
          break;
        }
      }
      if (!runPresent) {
        plotList.push_back(static_cast<int>(msrPlotList->at(i).fRuns[j]));
      }
    }
  }

  // read all the necessary runs (raw data)
  PRunDataHandler *dataHandler;
  if (startupHandler)
    dataHandler = new PRunDataHandler(msrHandler, startupHandler->GetDataPathList());
  else
    dataHandler = new PRunDataHandler(msrHandler);

  success = dataHandler->IsAllDataAvailable();
  if (!success) {
    cerr << endl << ">> musrview **ERROR** Couldn't read all data files, will quit ..." << endl;
  }

  // generate the necessary histogramms for the view
  PRunListCollection *runListCollection = 0;
  if (success) {
    // feed all the necessary histogramms for the view
    runListCollection = new PRunListCollection(msrHandler, dataHandler);
    for (unsigned int i=0; i<msrHandler->GetMsrRunList()->size(); i++) {
      // if run is in plotList add it, otherwise go to the next
      runPresent = false;
      for (unsigned int j=0; j<plotList.size(); j++) {
        if (static_cast<unsigned int>(plotList[j]) == i+1) {
          runPresent = true;
          break;
        }
      }
      if (runPresent) {
        success = runListCollection->Add(i, kView);
        if (!success) {
          cerr << endl << ">> musrview **ERROR** Couldn't handle run no " << i << " ";
          cerr << (*msrHandler->GetMsrRunList())[i].GetRunName()->Data();
          break;
        }
      }
    }
  }

  if (success) {
    // generate Root application needed for PMusrCanvas
    if (graphicsOutput) {
      argv[argc] = (char*)malloc(16*sizeof(char));
      strcpy(argv[argc], "-b");
      argc++;
    }
    TApplication app("App", &argc, argv);

    vector<PMusrCanvas*> canvasVector;
    PMusrCanvas *musrCanvas;

    bool ok = true;
    for (unsigned int i=0; i<msrHandler->GetMsrPlotList()->size(); i++) {

      if (startupHandler)
        musrCanvas = new PMusrCanvas(i, msrHandler->GetMsrTitle()->Data(), 
                                     10+i*100, 10+i*100, 800, 600,
                                     startupHandler->GetFourierDefaults(),
                                     startupHandler->GetMarkerList(),
                                     startupHandler->GetColorList(),
                                     graphicsOutput);
      else
        musrCanvas = new PMusrCanvas(i, msrHandler->GetMsrTitle()->Data(), 
                                     10+i*100, 10+i*100, 800, 600, graphicsOutput);

      if (!musrCanvas->IsValid()) {
        cerr << endl << ">> musrview **SEVERE ERROR** Couldn't invoke all necessary objects, will quit.";
        cerr << endl;
        ok = false;
        break;
      }
      // connect signal/slot
      TQObject::Connect("TCanvas", "Closed()", "PMusrCanvas", musrCanvas, "LastCanvasClosed()");

      // ugly but rootcint cannot handle the spirit-parser framework
      musrCanvas->SetMsrHandler(msrHandler);
      musrCanvas->SetRunListCollection(runListCollection);

      musrCanvas->UpdateParamTheoryPad();
      musrCanvas->UpdateDataTheoryPad();
      musrCanvas->UpdateInfoPad();

      if (!musrCanvas->IsValid()) { // something went wrong
        ok = false;
        break;
      }

      musrCanvas->Connect("Done(Int_t)", "TApplication", &app, "Terminate(Int_t)");

      if (graphicsOutput) {
        musrCanvas->SaveGraphicsAndQuit(fileName, graphicsExtension);
      }

      // keep musrCanvas objects
      canvasVector.push_back(musrCanvas);
    }

    // check that everything is ok
    if (ok)
      app.Run(true); // true needed that Run will return after quit so that cleanup works

    // clean up
    char canvasName[32];
    for (unsigned int i=0; i<canvasVector.size(); i++) {
      // check if canvas is still there before calling the destructor **TO BE DONE**
      sprintf(canvasName, "fMainCanvas%d", i);
      if (gROOT->GetListOfCanvases()->FindObject(canvasName) != 0) {
        canvasVector[i]->~PMusrCanvas();
      } else {
      }
    }
    canvasVector.empty();
  }

  // clean up
  plotList.clear();
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

  return PMUSR_SUCCESS;
}
