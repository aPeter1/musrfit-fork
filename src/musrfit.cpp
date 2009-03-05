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
  cout << endl << "usage: musrfit [<msr-file> [-k, --keep-mn2-ouput] [-c, --chisq-only] [--debug] [--dump <type>]] | --version | --help";
  cout << endl << "       <msr-file>: msr input file";
  cout << endl << "       'musrfit <msr-file>' will execute musrfit";
  cout << endl << "       'musrfit' or 'musrfit --help' will show this help";
  cout << endl << "       'musrfit --version' will print the musrfit version";
  cout << endl << "       -k, --keep-mn2-output: will rename the files MINUIT2.OUTPUT and ";
  cout << endl << "              MINUIT2.root to <msr-file>-mn2.output and <msr-file>-mn2.root, repectively,";
  cout << endl << "              e.g. <msr-file> = 147.msr -> 147-mn2.output, 147-mn2.root";
  cout << endl << "       -c, --chisq-only: instead of fitting the data, chisq is just calculated";
  cout << endl << "              once and the result is set to the stdout. This feature is useful";
  cout << endl << "              to adjust initial parameters.";
  cout << endl << "       --debug is used to print additional infos";
  cout << endl << "       --dump <type> is writing a data file with the fit data and the theory";
  cout << endl << "              <type> can be 'ascii', 'root'";
  cout << endl << endl;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param msrHandler
 */
void musrfit_debug_info(PMsrHandler* msrHandler)
{
  int i;

  // print title
  cout << endl << "----------";
  cout << endl << "title = " << msrHandler->GetMsrTitle()->Data();

  // print parameters
  cout << endl << "----------";
  cout << endl << "parameters";
  PMsrParamList::iterator param_it;
  for (param_it = msrHandler->GetMsrParamList()->begin(); param_it != msrHandler->GetMsrParamList()->end(); ++param_it) {
    cout << endl << param_it->fNo << ": ";
    cout << param_it->fName << ", ";
    cout << param_it->fValue << ", ";
    cout << param_it->fStep << ", ";
    switch (param_it->fNoOfParams) {
      case 5:
        cout << param_it->fPosError;
        break;
      case 6:
        cout << param_it->fLowerBoundary << ", ";
        cout << param_it->fUpperBoundary;
        break;
      case 7:
        cout << param_it->fPosError << ", ";
        cout << param_it->fLowerBoundary << ", ";
        cout << param_it->fUpperBoundary;
        break;
      default:
        break;
    }
  }

  // print theory
  cout << endl << "----------";
  PMsrLines::iterator theo_it;
  theo_it = msrHandler->GetMsrTheory()->begin();
  for (; theo_it != msrHandler->GetMsrTheory()->end(); ++theo_it)
    cout << endl << theo_it->fLine.Data();

  // print functions

  // print run
  cout << endl << "----------";
  cout << endl << "runs";
  PMsrRunList::iterator runs_it;
  int runNo = 1;
  for (runs_it = msrHandler->GetMsrRunList()->begin(); runs_it != msrHandler->GetMsrRunList()->end(); ++runs_it) {
    cout << endl << "******";
    cout << endl << "run no " << runNo++;
    cout << endl << "run (name, beamline, institute, data-file-format): ";
    cout << endl << "     " << runs_it->fRunName << ", " << runs_it->fBeamline << ", " << runs_it->fInstitute << ", " << runs_it->fFileFormat;
    cout << endl << "fittype    " << runs_it->fFitType;
    cout << endl << "alpha      " << runs_it->fAlphaParamNo;
    cout << endl << "beta       " << runs_it->fBetaParamNo;
    cout << endl << "norm       " << runs_it->fNormParamNo;
    cout << endl << "backgr.fit " << runs_it->fBkgFitParamNo;
    cout << endl << "rphase     " << runs_it->fPhaseParamNo;
    cout << endl << "lifetime   " << runs_it->fLifetimeParamNo;
    if (runs_it->fLifetimeCorrection)
      cout << endl << "lifetimecorrection true";
    else
      cout << endl << "lifetimecorrection false";
    cout << endl << "map        ";
    for (PIntVector::iterator it = runs_it->fMap.begin(); it != runs_it->fMap.end(); ++it)
      cout << *it << ", ";
    cout << endl << "forward    " << runs_it->fForwardHistoNo;
    cout << endl << "backward   " << runs_it->fBackwardHistoNo;
    cout << endl << "backgr.fix ";
    for (int i=0; i<2; i++)
      cout << runs_it->fBkgFix[i] << ", ";
    cout << endl << "background ";
    for (int i=0; i<4; i++)
      cout << runs_it->fBkgRange[i] << ", ";
    cout << endl << "data       ";
    for (int i=0; i<4; i++)
      cout << runs_it->fDataRange[i] << ", ";
    cout << endl << "t0         ";
    for (int i=0; i<2; i++)
      cout << runs_it->fT0[i] << ", ";
    cout << endl << "fit        ";
    for (int i=0; i<2; i++)
      cout << runs_it->fFitRange[i] << ", ";
    cout << endl << "packing    " << runs_it->fPacking;
    cout << endl << "rrffrequency " << runs_it->fRRFFreq;
    cout << endl << "rrfpacking   " << runs_it->fRRFPacking;
    cout << endl << "alpha2       " << runs_it->fAlpha2ParamNo;
    cout << endl << "beta2        " << runs_it->fBeta2ParamNo;
    cout << endl << "right        " << runs_it->fRightHistoNo;
    cout << endl << "left         " << runs_it->fLeftHistoNo;
  }

  // print commands
  cout << endl << "----------";
  cout << endl << "commands";
  PMsrLines::iterator commands_it;
  commands_it = msrHandler->GetMsrCommands()->begin();
  ++commands_it; // the first entry is just the COMMANDS block statment
  for (; commands_it != msrHandler->GetMsrCommands()->end(); ++commands_it)
    cout << endl << commands_it->fLine.Data();

  // print plot
  cout << endl << "----------";
  PMsrPlotList::iterator plot_it;
  i = 1;
  for (plot_it = msrHandler->GetMsrPlotList()->begin(); plot_it != msrHandler->GetMsrPlotList()->end(); ++plot_it) {
    cout << endl << i++ << ". plot " << plot_it->fPlotType;
    cout << endl << "runs = ";
    PComplexVector::iterator plot_run_it;
    for (plot_run_it = plot_it->fRuns.begin(); plot_run_it != plot_it->fRuns.end(); ++plot_run_it) {
      switch (plot_it->fPlotType) {
        case MSR_PLOT_SINGLE_HISTO:
        case MSR_PLOT_ASYM:
        case MSR_PLOT_NON_MUSR:
          cout << plot_run_it->Re() << ", ";
          break;
        case MSR_PLOT_ASYM_RRF:
          cout << "(" << plot_run_it->Re() << "," << plot_run_it->Im() << "), ";
          break;
        default:
          cout << endl << "??";
          break;
      }
    }
    if (plot_it->fTmin == -999.0) {
      cout << endl << "range = autorange";
    } else {
      if (plot_it->fYmin == -999.0) { // only time range given
        cout << endl << "range = [ " << plot_it->fTmin << ", " << plot_it->fTmax << "]";
      } else {
        cout << endl << "range = [ " << plot_it->fTmin << ", " << plot_it->fTmax << ", " << plot_it->fYmin << ", " << plot_it->fYmax << "]";
      }
    }
  }

  // print statistic
  cout << endl << "----------";
  cout << endl << "statistic";
  PMsrLines::iterator statistic_it;
  statistic_it = msrHandler->GetMsrStatistic()->fStatLines.begin();
  ++statistic_it; // the first entry is just the STATISTIC block statment
  for (; statistic_it != msrHandler->GetMsrStatistic()->fStatLines.end(); ++statistic_it)
    cout << endl << statistic_it->fLine.Data();

  cout << endl << "----------" << endl;
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
  f << "% number of data values = " << data->fValue.size() << endl;
  f << "% time (us), value, error, theory" << endl;
  double time;
  for (unsigned int i=0; i<data->fValue.size(); i++) {
    time = data->fDataTimeStart + (double)i*data->fDataTimeStep;
    f << time << ", " << data->fValue[i] << ", " << data->fError[i] << ", " << data->fTheory[i] << endl;
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
  size = runList->GetNoOfRRF();
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetRRF(i);
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
  Double_t diff = data->fDataTimeStep;
  Double_t start = -diff/2.0;
  Double_t end = data->fDataTimeStep*(data->fValue.size()-1)+diff/2.0;
  TH1F *hdata = new TH1F("hdata", "run data", data->fValue.size(), start, end);
  TH1F *htheo = new TH1F("htheo", "run theory", data->fValue.size(), start, end);

  // fill data
  for (unsigned int i=0; i<data->fValue.size(); i++) {
    hdata->SetBinContent(i, data->fValue[i]);
    hdata->SetBinError(i, data->fError[i]);
    htheo->SetBinContent(i, data->fTheory[i]);
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
  size = runList->GetNoOfRRF();
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetRRF(i);
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
}

//--------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  bool show_syntax = false;
  int  status;
  bool debug = false;
  bool keep_mn2_output = false;
  bool chisq_only = false;
  TString dump("");

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

  // check file name
  if (!strstr(argv[1], ".msr")) {
    cout << endl << "**ERROR** " << argv[1] << " is not a msr-file!" << endl;
    musrfit_syntax();
    return PMUSR_WRONG_STARTUP_SYNTAX;
  }

  for (int i=2; i<argc; i++) {
    if (!strcmp(argv[i], "-k") || !strcmp(argv[i], "--keep-mn2-output")) {
      keep_mn2_output = true;
    } else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--chisq-only")) {
      chisq_only = true;
    } else if (!strcmp(argv[i], "--debug")) {
      debug = true;
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


  // get default path (for the moment only linux like)
  char *pmusrpath;
  char musrpath[128];
  pmusrpath = getenv("MUSRFITPATH");
  if (pmusrpath == 0) { // not set, will try default one
    strcpy(musrpath, "/home/nemu/analysis/bin");
    cout << endl << "**WARNING** MUSRFITPATH environment variable not set will try " << musrpath << endl;
  } else {
    strncpy(musrpath, pmusrpath, sizeof(musrpath));
  }

  // read startup file
  char startup_path_name[128];
  sprintf(startup_path_name, "%s/musrfit_startup.xml", musrpath);
  TSAXParser *saxParser = new TSAXParser();
  PStartupHandler *startupHandler = new PStartupHandler();
  saxParser->ConnectToHandler("PStartupHandler", startupHandler);
  status = saxParser->ParseFile(startup_path_name);
  // check for parse errors
  if (status) { // error
    cout << endl << "**WARNING** reading/parsing musrfit_startup.xml.";
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
  }

  // read msr-file
  PMsrHandler *msrHandler = new PMsrHandler(argv[1]);
  status = msrHandler->ReadMsrFile();
  if (status != PMUSR_SUCCESS) {
    switch (status) {
      case PMUSR_MSR_FILE_NOT_FOUND:
        cout << endl << "**ERROR** couldn't find " << argv[1] << endl << endl;
        break;
      case PMUSR_MSR_SYNTAX_ERROR:
        cout << endl << "**SYNTAX ERROR** in file " << argv[1] << ", full stop here." << endl << endl;
        break;
      default:
        cout << endl << "**UNKOWN ERROR** when trying to read the msr-file" << endl << endl;
        break;
    }
    return status;
  }

  if (debug)
    musrfit_debug_info(msrHandler);

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

  // generate the necessary fit histogramms for the fit
  PRunListCollection *runListCollection = 0;
  if (success) {
    // feed all the necessary histogramms for the fit
    runListCollection = new PRunListCollection(msrHandler, dataHandler);
    for (unsigned int i=0; i < msrHandler->GetMsrRunList()->size(); i++) {
      success = runListCollection->Add(i, kFit);
      if (!success) {
        cout << endl << "**ERROR** Couldn't handle run no " << i << " ";
        cout << (*msrHandler->GetMsrRunList())[i].fRunName.Data();
        break;
      }
    }
  }

  // do fitting
  PFitter *fitter = 0;
  if (success) {
    fitter = new PFitter(msrHandler, runListCollection, chisq_only);
    success = fitter->IsValid();
    if (success)
      fitter->DoFit();
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
      musrfit_dump_ascii(argv[1], runListCollection);
    else if (dump.Contains("root"))
      musrfit_dump_root(argv[1], runListCollection);
    else
      cout << endl << "do not know format " << dump.Data() << ", sorry :-| " << endl;
  }

  // rename MINUIT2.OUTPUT and MINUIT2.root file if wanted
  if (keep_mn2_output && !chisq_only) {
    // 1st rename MINUIT2.OUTPUT
    TString fln = TString(argv[1]);
    char ext[32];
    strcpy(ext, "-mn2.output");
    fln.ReplaceAll(".msr", 4, ext, strlen(ext));
    gSystem->CopyFile("MINUIT2.OUTPUT", fln.Data(), kTRUE);

    // 2nd rename MINUIT2.ROOT
    fln = TString(argv[1]);
    strcpy(ext, "-mn2.root");
    fln.ReplaceAll(".msr", 4, ext, strlen(ext));
    gSystem->CopyFile("MINUIT2.root", fln.Data(), kTRUE);
  }

  // clean up
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

