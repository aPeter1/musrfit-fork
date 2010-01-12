/***************************************************************************

  musrfit.cpp

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
#include <fstream>
using namespace std;

#include <TSAXParser.h>
#include <TString.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>

#include "PMusr.h"
#include "PStartupHandler.h"
#include "PMsrHandler.h"
#include "PRunDataHandler.h"
#include "PRunListCollection.h"
#include "PFitter.h"

//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void musrfit_syntax()
{
  cout << endl << "usage: musrfit [<msr-file> [-k, --keep-mn2-ouput] [-c, --chisq-only] [-t, --title-from-data-file]";
  cout << endl << "                            [--dump <type>] | --version | --help";
  cout << endl << "       <msr-file>: msr input file";
  cout << endl << "       'musrfit <msr-file>' will execute musrfit";
  cout << endl << "       'musrfit' or 'musrfit --help' will show this help";
  cout << endl << "       'musrfit --version' will print the musrfit version";
  cout << endl << "       -k, --keep-mn2-output: will rename the files MINUIT2.OUTPUT and ";
  cout << endl << "              MINUIT2.root to <msr-file>-mn2.output and <msr-file>-mn2.root,";
  cout << endl << "              respectively,";
  cout << endl << "              e.g. <msr-file> = 147.msr -> 147-mn2.output, 147-mn2.root";
  cout << endl << "       -c, --chisq-only: instead of fitting the data, chisq is just calculated";
  cout << endl << "              once and the result is set to the stdout. This feature is useful";
  cout << endl << "              to adjust initial parameters.";
  cout << endl << "       -t, --title-from-data-file: will replace the <msr-file> run title by the";
  cout << endl << "              run title of the FIRST run of the <msr-file> run block, if a run title";
  cout << endl << "              is present in the data file.";
  cout << endl << "       --dump <type> is writing a data file with the fit data and the theory";
  cout << endl << "              <type> can be 'ascii', 'root'" << endl;
  cout << endl << "       At the end of a fit, musrfit writes the fit results into an <mlog-file> and";
  cout << endl << "       swaps them, i.e. in the <msr-file> you will find the fit results and in the";
  cout << endl << "       <mlog-file> your initial guess values.";
  cout << endl << endl;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param fln
 * \param data
 * \param runCounter
 */
void musrfit_write_ascii(TString fln, PRunData *data, int runCounter)
{
  // generate dump file name
  TString fileName = fln.Copy();
  TString count("_");
  count += runCounter;
  Ssiz_t index = fln.Index(".");
  fln.Insert(index, count);
//cout << endl << "fln = " << fln.Data();

  ofstream f;

  // open dump-file
  f.open(fln.Data(), iostream::out);
  if (!f.is_open()) {
    cout << endl << "Couldn't open dump (" << fln.Data() << ") file for writting, sorry ...";
    cout << endl;
    return;
  }

  // dump data
  f << "% number of data values = " << data->GetValue()->size() << endl;
  f << "% time (us), value, error, theory" << endl;
  double time;
  for (unsigned int i=0; i<data->GetValue()->size(); i++) {
    time = data->GetDataTimeStart() + (double)i*data->GetDataTimeStep();
    f << time << ", " << data->GetValue()->at(i) << ", " << data->GetError()->at(i) << ", " << data->GetTheory()->at(i) << endl;
  }

  // close file
  f.close();
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param fileName
 */
void musrfit_dump_ascii(char *fileName, PRunListCollection *runList)
{
  TString fln(fileName);
  fln.ReplaceAll(".msr", ".dat");

  // go through the run list, get the data and dump it in a file

  int runCounter = 0;

  // single histos
  unsigned int size = runList->GetNoOfSingleHisto();
  PRunData *data;
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetSingleHisto(i);
      if (data) {
        // dump data
        musrfit_write_ascii(fln, data, runCounter);
        runCounter++;
      }
    }
  }

  // asymmetry
  size = runList->GetNoOfAsymmetry();
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetAsymmetry(i);
      if (data) {
        // dump data
        musrfit_write_ascii(fln, data, runCounter);
        runCounter++;
      }
    }
  }

  // rrf
  size = runList->GetNoOfMuMinus();
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetMuMinus(i);
      if (data) {
        // dump data
        musrfit_write_ascii(fln, data, runCounter);
        runCounter++;
      }
    }
  }

  // nonMusr
  size = runList->GetNoOfNonMusr();
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetNonMusr(i);
      if (data) {
        // dump data
        musrfit_write_ascii(fln, data, runCounter);
        runCounter++;
      }
    }
  }
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param f
 * \param fln
 * \param data
 * \param runCounter
 */
void musrfit_write_root(TFile &f, TString fln, PRunData *data, int runCounter)
{
  char name[128];

  TString title = fln.Copy();
  sprintf(name, "_%d", runCounter);
  title.ReplaceAll(".root", name);

  sprintf(name, "c%d", runCounter);

  TCanvas *c = new TCanvas(name, title.Data(), 10, 10, 800, 600);

  // create histos
  Double_t diff = data->GetDataTimeStep();
  Double_t start = -diff/2.0;
  Double_t end = data->GetDataTimeStep()*data->GetValue()->size();
  TH1F *hdata = new TH1F("hdata", "run data", data->GetValue()->size(), start, end);
  TH1F *htheo = new TH1F("htheo", "run theory", data->GetValue()->size(), start, end);

  // fill data
  for (unsigned int i=0; i<data->GetValue()->size(); i++) {
    hdata->SetBinContent(i+1, data->GetValue()->at(i));
    hdata->SetBinError(i+1, data->GetError()->at(i));
    htheo->SetBinContent(i+1, data->GetTheory()->at(i));
  }

  hdata->SetMarkerStyle(20);
  hdata->Draw("*H HIST E1");

  htheo->SetLineColor(2);
  htheo->SetLineWidth(3);
  htheo->Draw("C SAME");

  f.WriteTObject(c);

  // clean up
  if (hdata) {
    delete hdata;
    hdata = 0;
  }
  if (htheo) {
    delete htheo;
    htheo = 0;
  }
  if (c) {
    delete c;
    c = 0;
  }
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param fileName
 */
void musrfit_dump_root(char *fileName, PRunListCollection *runList)
{
  TString fln(fileName);
  fln.ReplaceAll(".msr", ".root");

  TFile f(fln.Data(), "recreate");

  // go through the run list, get the data and dump it in a file

  int runCounter = 0;

  // single histos
  unsigned int size = runList->GetNoOfSingleHisto();
  PRunData *data;
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetSingleHisto(i);
      if (data) {
        // dump data
        musrfit_write_root(f, fln, data, runCounter);
        runCounter++;
      }
    }
  }

  // asymmetry
  size = runList->GetNoOfAsymmetry();
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetAsymmetry(i);
      if (data) {
        // dump data
        musrfit_write_root(f, fln, data, runCounter);
        runCounter++;
      }
    }
  }

  // rrf
  size = runList->GetNoOfMuMinus();
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetMuMinus(i);
      if (data) {
        // dump data
        musrfit_write_root(f, fln, data, runCounter);
        runCounter++;
      }
    }
  }

  // nonMusr
  size = runList->GetNoOfNonMusr();
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetNonMusr(i);
      if (data) {
        // dump data
        musrfit_write_root(f, fln, data, runCounter);
        runCounter++;
      }
    }
  }

  f.Close("R");
}

//--------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  bool show_syntax = false;
  int  status;
  bool keep_mn2_output = false;
  bool chisq_only = false;
  bool title_from_data_file = false;

  TString dump("");
  char filename[1024];

  // check syntax
  if (argc < 2) {
    musrfit_syntax();
    return PMUSR_WRONG_STARTUP_SYNTAX;
  }

  if (argc == 2) {
    if (!strcmp(argv[1], "--version")) {
      cout << endl << "musrfit version: $Id$";
      cout << endl << endl;
      return PMUSR_SUCCESS; 
    }

    if (!strcmp(argv[1], "--help")) {
      show_syntax = true;
    }
  }

  if (show_syntax) {
    musrfit_syntax();
    return PMUSR_WRONG_STARTUP_SYNTAX;
  }

  strcpy(filename, "");
  for (int i=1; i<argc; i++) {
    if (strstr(argv[i], ".msr")) {
      strncpy(filename, argv[i], sizeof(filename));
    } else if (!strcmp(argv[i], "-k") || !strcmp(argv[i], "--keep-mn2-output")) {
      keep_mn2_output = true;
    } else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--chisq-only")) {
      chisq_only = true;
    } else if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--title-from-data-file")) {
      title_from_data_file = true;
    } else if (!strcmp(argv[i], "--dump")) {
      if (i<argc-1) {
        dump = TString(argv[i+1]);
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

  // check if a filename is present
  if (strlen(filename) == 0) {
    show_syntax = true;
    cout << endl << "**ERROR** no msr-file present!" << endl;
  }

  if (show_syntax) {
    musrfit_syntax();
    return PMUSR_WRONG_STARTUP_SYNTAX;
  }

  // check if dump string does make sense
  if (!dump.IsNull()) {
    dump.ToLower();
    if (!dump.Contains("ascii") && !dump.Contains("root")) {
      musrfit_syntax();
      return PMUSR_WRONG_STARTUP_SYNTAX;
    }
  }

  // read startup file
  char startup_path_name[128];
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
        cout << endl << "**ERROR** couldn't find " << filename << endl << endl;
        break;
      case PMUSR_MSR_SYNTAX_ERROR:
        cout << endl << "**SYNTAX ERROR** in file " << filename << ", full stop here." << endl << endl;
        break;
      default:
        cout << endl << "**UNKOWN ERROR** when trying to read the msr-file" << endl << endl;
        break;
    }
    return status;
  }

  // read all the necessary runs (raw data)
  PRunDataHandler *dataHandler;
  if (startupHandler)
    dataHandler = new PRunDataHandler(msrHandler, startupHandler->GetDataPathList());
  else
    dataHandler = new PRunDataHandler(msrHandler);

  bool success = dataHandler->IsAllDataAvailable();
  if (!success) {
    cout << endl << "**ERROR** Couldn't read all data files, will quit ..." << endl;
  }

  // if present, replace the run title of the <msr-file> with the run title of the FIRST run in the run block of the msr-file
  if (title_from_data_file) {
    PMsrRunList *rl  = msrHandler->GetMsrRunList();
    PRawRunData *rrd = dataHandler->GetRunData(*(rl->at(0).GetRunName()));
    if (rrd->GetRunTitle()->Length() > 0)
      msrHandler->SetMsrTitle(*rrd->GetRunTitle());
  }

  // generate the necessary fit histogramms for the fit
  PRunListCollection *runListCollection = 0;
  if (success) {
    // feed all the necessary histogramms for the fit   
    runListCollection = new PRunListCollection(msrHandler, dataHandler);
    for (unsigned int i=0; i < msrHandler->GetMsrRunList()->size(); i++) {
      success = runListCollection->Add(i, kFit);
      if (!success) {
        cout << endl << "**ERROR** Couldn't handle run no " << i << " ";
        cout << (*msrHandler->GetMsrRunList())[i].GetRunName()->Data();
        break;
      }
    }
  }

  // do fitting
  PFitter *fitter = 0;
  if (success) {
    fitter = new PFitter(msrHandler, runListCollection, chisq_only);
    if (fitter->IsValid()) {
      fitter->DoFit();
      msrHandler->SetMsrStatisticConverged(fitter->HasConverged());
    }
  }

  // write log file
  if (success && !chisq_only) {
    status = msrHandler->WriteMsrLogFile();
    if (status != PMUSR_SUCCESS) {
      switch (status) {
        case PMUSR_MSR_LOG_FILE_WRITE_ERROR:
          cout << endl << "**ERROR** couldn't write mlog-file" << endl << endl;
          break;
        case PMUSR_TOKENIZE_ERROR:
          cout << endl << "**ERROR** couldn't generate mlog-file name" << endl << endl;
          break;
        default:
          cout << endl << "**UNKOWN ERROR** when trying to write the mlog-file" << endl << endl;
          break;
      }
    }
  }

  // check if dump is wanted
  if (success && !dump.IsNull()) {
    cout << endl << "will write dump file ..." << endl;
    dump.ToLower();
    if (dump.Contains("ascii"))
      musrfit_dump_ascii(filename, runListCollection);
    else if (dump.Contains("root"))
      musrfit_dump_root(filename, runListCollection);
    else
      cout << endl << "do not know format " << dump.Data() << ", sorry :-| " << endl;
  }

  // rename MINUIT2.OUTPUT and MINUIT2.root file if wanted
  if (keep_mn2_output && !chisq_only) {
    // 1st rename MINUIT2.OUTPUT
    TString fln = TString(filename);
    char ext[32];
    strcpy(ext, "-mn2.output");
    fln.ReplaceAll(".msr", 4, ext, strlen(ext));
    gSystem->CopyFile("MINUIT2.OUTPUT", fln.Data(), kTRUE);

    // 2nd rename MINUIT2.ROOT
    fln = TString(filename);
    strcpy(ext, "-mn2.root");
    fln.ReplaceAll(".msr", 4, ext, strlen(ext));
    gSystem->CopyFile("MINUIT2.root", fln.Data(), kTRUE);
  }

  if (!chisq_only) {
    // swap msr- and mlog-file
    cout << endl << ">> swapping msr-, mlog-file ..." << endl;
    // copy msr-file -> __temp.msr
    gSystem->CopyFile(filename, "__temp.msr", kTRUE);
    // copy mlog-file -> msr-file
    TString fln = TString(filename);
    char ext[32];
    strcpy(ext, ".mlog");
    fln.ReplaceAll(".msr", 4, ext, strlen(ext));
    gSystem->CopyFile(fln.Data(), filename, kTRUE);
    // copy __temp.msr -> mlog-file
    gSystem->CopyFile("__temp.msr", fln.Data(), kTRUE);
    // delete __temp.msr
    gSystem->Exec("rm __temp.msr");
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
  if (fitter) {
    delete fitter;
    fitter = 0;
  }

  cout << endl << "done ..." << endl;

  return PMUSR_SUCCESS;
}

// end ---------------------------------------------------------------------

