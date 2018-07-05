/***************************************************************************

  musrview.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2014 by Andreas Suter                              *
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
using namespace std;

#include <TApplication.h>
#include <TSAXParser.h>
#include <TROOT.h>

#include "git-revision.h"
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
  cout << endl << "usage: musrview <msr-file> [Options]";
  cout << endl << "       <msr-file>: msr/mlog input file";
  cout << endl << "       Options:";
  cout << endl << "       --help    : display this help and exit.";
  cout << endl << "       --version : output version information and exit.";
  cout << endl << "       -f, --fourier: will directly present the Fourier transform of the <msr-file>.";
  cout << endl << "       -a, --avg: will directly present the averaged data/Fourier of the <msr-file>.";
  cout << endl << "       --<graphic-format-extension>: ";
  cout << endl << "           will produce a graphics-output-file without starting a root session.";
  cout << endl << "           the name is based on the <msr-file>, e.g. 3310.msr -> 3310_0.png";
  cout << endl << "           supported graphic-format-extension:";
  cout << endl << "           eps, pdf, gif, jpg, png, svg, xpm, root";
  cout << endl << "           example: musrview 3310.msr --png, will produce a files 3310_X.png";
  cout << endl << "                    where 'X' stands for the plot number (starting form 0)";
  cout << endl << "       --ascii: ";
  cout << endl << "           will produce an ascii dump of the data and fit as plotted.";
  cout << endl << "       --timeout <timeout>: <timeout> given in seconds after which musrview terminates.";
  cout << endl << "           If <timeout> <= 0, no timeout will take place. Default <timeout> is 0.";
  cout << endl << endl;
}

//--------------------------------------------------------------------------
/**
 * <p>The musrview program is used to show muSR fit results in graphical form.
 * From it also Fourier transforms, difference between data an theory can be formed.
 * For a detailed description/usage of the program, please see
 * \htmlonly <a href="http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#musrview">musrview online help</a>
 * \endhtmlonly
 * \latexonly musrview online help: \texttt{http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#musrview}
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
  bool fourier = false;
  bool avg = false;
  bool graphicsOutput = false;
  bool asciiOutput = false;
  char graphicsExtension[128];
  int  timeout = 0;

  memset(fileName, '\0', sizeof(fileName));

  // check input arguments
  if (argc == 1) {
    musrview_syntax();
    return PMUSR_SUCCESS;
  }
  for (int i=1; i<argc; i++) {
    if (strstr(argv[i], ".msr") || strstr(argv[i], ".mlog")) {
      if (strlen(fileName) == 0) {
        strcpy(fileName, argv[i]);
      } else {
        cerr << endl << "**ERROR** only one file name allowed." << endl;
        show_syntax = true;
        break;
      }
    } else if (!strcmp(argv[i], "--version")) {
#ifdef HAVE_CONFIG_H
      cout << endl << "musrview version: " << PACKAGE_VERSION << ", git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << endl << endl;
#else
      cout << endl << "musrview git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << endl << endl;
#endif
      return PMUSR_SUCCESS;
    } else if (!strcmp(argv[i], "--help")) {
      show_syntax = true;
      break;
    } else if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--fourier")) {
      fourier = true;
    } else if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--avg")) {
      avg = true;
    } else if (!strcmp(argv[i], "--eps") || !strcmp(argv[i], "--pdf") || !strcmp(argv[i], "--gif") ||
               !strcmp(argv[i], "--jpg") || !strcmp(argv[i], "--png") || !strcmp(argv[i], "--svg") ||
               !strcmp(argv[i], "--xpm") || !strcmp(argv[i], "--root")) {

      graphicsOutput = true;
      strcpy(graphicsExtension, argv[i]+2);
    } else if (!strcmp(argv[i], "--ascii")) {
      asciiOutput = true;
    } else if (!strcmp(argv[i], "--timeout")) {
      if (i+1 < argc) {
        TString str(argv[i+1]);
        if (str.IsFloat()) {
          timeout = str.Atoi();
        } else {
          show_syntax = true;
          break;
        }
        i++;
      } else {
        show_syntax = true;
        break;
      }
    } else {
      show_syntax = true;
      break;
    }
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
    //status = saxParser->ParseFile(startup_path_name);
    // parsing the file as above seems to lead to problems in certain environments;
    // use the parseXmlFile function instead (see PStartupHandler.cpp for the definition)
    status = parseXmlFile(saxParser, startup_path_name);
    // check for parse errors
    if (status) { // error
      cerr << endl << ">> musrview **WARNING** Reading/parsing musrfit_startup.xml failed.";
      cerr << endl << ">>                      Any graph will appear with random symbols and colors!";
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
      startupHandler->CheckLists();
    }
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

  dataHandler->ReadData();

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
    if (graphicsOutput || asciiOutput) {
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
                                     graphicsOutput||asciiOutput,
                                     fourier, avg);
      else
        musrCanvas = new PMusrCanvas(i, msrHandler->GetMsrTitle()->Data(), 
                                     10+i*100, 10+i*100, 800, 600,
                                     graphicsOutput||asciiOutput,
                                     fourier, avg);

      if (!musrCanvas->IsValid()) {
        cerr << endl << ">> musrview **SEVERE ERROR** Couldn't invoke all necessary objects, will quit.";
        cerr << endl;
        ok = false;
        break;
      }

      musrCanvas->SetTimeout(timeout);

      // ugly but rootcling cannot handle the spirit-parser framework
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

      if (asciiOutput) {
        // generate export data file name
        TString str(fileName);
        str.Remove(str.Last('.'));
        str += ".dat";
        // save data in batch mode
        musrCanvas->ExportData(str.Data());
        musrCanvas->Done(0);
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
