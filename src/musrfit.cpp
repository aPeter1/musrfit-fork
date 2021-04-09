/***************************************************************************

  musrfit.cpp

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

#include <cstdio>
#include <cstdlib>
//#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#include <iostream>
#include <fstream>

#include <TSAXParser.h>
#include <TString.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TSystem.h>
#include <TSystemFile.h>
#include <TThread.h>

#include "git-revision.h"
#include "PMusr.h"
#include "PStartupHandler.h"
#include "PMsrHandler.h"
#include "PRunDataHandler.h"
#include "PRunListCollection.h"
#include "PFitter.h"

//--------------------------------------------------------------------------

static int timeout = 3600; // given in (sec)

//--------------------------------------------------------------------------
/**
 * <p>Sometimes musrfit is not terminating properly for reasons still not pinned down, hence
 * this workaround. It should be removed asap. Especially since it is leading to memory leaks.
 */
void* musrfit_timeout(void *args)
{
  pid_t *pid = (pid_t*)(args);

  sleep(timeout);

  std::cerr << std::endl << ">> **FATAL ERROR** musrfit_timeout for task pid=" << *pid << " called! Will kill it!" << std::endl << std::endl;

  kill(*pid, SIGKILL);

  return (void*)nullptr;
}

//--------------------------------------------------------------------------
/**
 * <p>Sends the usage description to the standard output.
 */
void musrfit_syntax()
{
  std::cout << std::endl << "usage: musrfit [<msr-file> [-k, --keep-mn2-ouput] [-c, --chisq-only] [-t, --title-from-data-file]";
  std::cout << std::endl << "                            [-e, --estimateN0] [-p, --per-run-block-chisq]";
  std::cout << std::endl << "                            [--dump <type>] [--timeout <timeout_tag>] |";
  std::cout << std::endl << "                            --nexus-support | --show-dynamic-path | --version | --help";
  std::cout << std::endl << "       <msr-file>: msr input file";
  std::cout << std::endl << "       'musrfit <msr-file>' will execute musrfit";
  std::cout << std::endl << "       'musrfit' or 'musrfit --help' will show this help";
  std::cout << std::endl << "       'musrfit --version' will print the musrfit version";
  std::cout << std::endl << "       'musrfit --nexus-support' will print if NeXus support is available.";
  std::cout << std::endl << "       'musrfit --show-dynamic-path' will print the internal dynamic library search paths.";
  std::cout << std::endl << "       -k, --keep-mn2-output: will rename the files MINUIT2.OUTPUT and ";
  std::cout << std::endl << "              MINUIT2.root to <msr-file>-mn2.output and <msr-file>-mn2.root,";
  std::cout << std::endl << "              respectively,";
  std::cout << std::endl << "              e.g. <msr-file> = 147.msr -> 147-mn2.output, 147-mn2.root";
  std::cout << std::endl << "       -c, --chisq-only: instead of fitting the data, chisq is just calculated";
  std::cout << std::endl << "              once and the result is set to the stdout. This feature is useful";
  std::cout << std::endl << "              to adjust initial parameters.";
  std::cout << std::endl << "       -t, --title-from-data-file: will replace the <msr-file> run title by the";
  std::cout << std::endl << "              run title of the FIRST run of the <msr-file> run block, if a run title";
  std::cout << std::endl << "              is present in the data file.";
  std::cout << std::endl << "       -e, --estimateN0: estimate N0 for single histogram fits.";
  std::cout << std::endl << "       -p, --per-run-block-chisq: will write per run block chisq to the msr-file.";
  std::cout << std::endl << "       --dump <type> is writing a data file with the fit data and the theory";
  std::cout << std::endl << "              <type> can be 'ascii', 'root'";
  std::cout << std::endl << "       --timeout <timeout_tag>: overwrites to predefined timeout of " << timeout << " (sec).";
  std::cout << std::endl << "              <timeout_tag> <= 0 means timeout facility is not enabled. <timeout_tag> = nn";
  std::cout << std::endl << "              will set the timeout to nn (sec).";
  std::cout << std::endl;
  std::cout << std::endl << "       At the end of a fit, musrfit writes the fit results into an <mlog-file> and";
  std::cout << std::endl << "       swaps them, i.e. in the <msr-file> you will find the fit results and in the";
  std::cout << std::endl << "       <mlog-file> your initial guess values.";
  std::cout << std::endl << std::endl;
}

//--------------------------------------------------------------------------
/**
 * <p>Writes the fitted data- and theory-set in ascii format to disc.
 *
 * \param fln output file name
 * \param data processed-data handler
 * \param runCounter msr-file run counter needed to form the output file name
 */
void musrfit_write_ascii(TString fln, PRunData *data, int runCounter)
{
  // generate dump file name
  TString count("_");
  count += runCounter;
  Ssiz_t index = fln.Index(".");
  fln.Insert(index, count);

  std::ofstream f;

  // open dump-file
  f.open(fln.Data(), std::iostream::out);
  if (!f.is_open()) {
    std::cout << std::endl << "Couldn't open dump (" << fln.Data() << ") file for writting, sorry ...";
    std::cout << std::endl;
    return;
  }

  // dump data
  f << "% number of data values = " << data->GetValue()->size() << std::endl;
  f << "% time (us), value, error, theory" << std::endl;
  double time;
  for (unsigned int i=0; i<data->GetValue()->size(); i++) {
    time = data->GetDataTimeStart() + (double)i*data->GetDataTimeStep();
    f << time << ", " << data->GetValue()->at(i) << ", " << data->GetError()->at(i) << ", " << data->GetTheory()->at(i) << std::endl;
  }

  // close file
  f.close();
}

//--------------------------------------------------------------------------
/**
 * <p>Go through all msr-file runs and write each of them in ascii-format to disc
 * (used for diagnostics). Be aware that this output is <b>different</b> to what
 * you will get when using musrview!
 *
 * \param fileName file name
 * \param runList run list collection handler
 */
void musrfit_dump_ascii(char *fileName, PRunListCollection *runList)
{
  TString fln(fileName);
  fln.ReplaceAll(".msr", ".dat");

  // go through the run list, get the data and dump it in a file

  int runCounter = 0;
  PRunData *data;

  // single histos
  unsigned int size = runList->GetNoOfSingleHisto();
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

  // single histos
  size = runList->GetNoOfSingleHistoRRF();
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetSingleHistoRRF(i);
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

  // asymmetry RRF
  size = runList->GetNoOfAsymmetryRRF();
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetAsymmetryRRF(i);
      if (data) {
        // dump data
        musrfit_write_ascii(fln, data, runCounter);
        runCounter++;
      }
    }
  }

  // muMinus
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
 * <p>Writes the fitted data- and theory-set in root format to disc.
 *
 * \param f root file object
 * \param fln file name
 * \param data processed-data handler
 * \param runCounter msr-file run counter needed to form the output file name
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
    hdata = nullptr;
  }
  if (htheo) {
    delete htheo;
    htheo = nullptr;
  }
  if (c) {
    delete c;
    c = nullptr;
  }
}

//--------------------------------------------------------------------------
/**
 * <p>Go through all msr-file runs and write each of them in root format to disc
 * (used for diagnostics). Be aware that this output is <b>different</b> to what
 * you will get when using musrview!
 *
 * \param fileName file name
 * \param runList run list connection handler
 */
void musrfit_dump_root(char *fileName, PRunListCollection *runList)
{
  TString fln(fileName);
  fln.ReplaceAll(".msr", ".root");

  TFile f(fln.Data(), "recreate");

  // go through the run list, get the data and dump it in a file

  int runCounter = 0;
  PRunData *data;

  // single histos
  unsigned int size = runList->GetNoOfSingleHisto();
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

  // single histo RRF
  size = runList->GetNoOfSingleHistoRRF();
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetSingleHistoRRF(i);
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

  // asymmetry RRF
  size = runList->GetNoOfAsymmetryRRF();
  if (size > 0) {
    for (unsigned int i=0; i<size; i++) {
      data = runList->GetAsymmetryRRF(i);
      if (data) {
        // dump data
        musrfit_write_root(f, fln, data, runCounter);
        runCounter++;
      }
    }
  }

  // muMinus
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
/**
 * <p>The musrfit program is used to fit muSR data.
 * For a detailed description/usage of the program, please see
 * \htmlonly <a href="http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#musrfit">musrfit online help</a>
 * \endhtmlonly
 * \latexonly musrfit online help: \texttt{http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#musrfit}
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
  bool keep_mn2_output = false;
  bool chisq_only = false;
  bool title_from_data_file = false;
  bool timeout_enabled = true;
  PStartupOptions startup_options;
  startup_options.writeExpectedChisq = false;
  startup_options.estimateN0 = false;

  TString dump("");
  char filename[1024];

  // check syntax
  if (argc < 2) {
    musrfit_syntax();
    return PMUSR_WRONG_STARTUP_SYNTAX;
  }

  // add default shared library path /usr/local/lib if not already persent
  const char *dsp = gSystem->GetDynamicPath();
  if (strstr(dsp, "/usr/local/lib") == nullptr)
    gSystem->AddDynamicPath("/usr/local/lib");

  if (argc == 2) {
    if (!strcmp(argv[1], "--version")) {
#ifdef HAVE_CONFIG_H
      std::cout << std::endl << "musrfit version: " << PACKAGE_VERSION << ", git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << ", ROOT version: " << ROOT_VERSION_USED << std::endl << std::endl;
#else
      std::cout << std::endl << "musrfit git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1 << std::endl << std::endl;
#endif
      return PMUSR_SUCCESS;
    } else if (!strcmp(argv[1], "--nexus-support")) {
#ifdef PNEXUS_ENABLED
      std::cout << std::endl << ">> musrfit: NeXus support enabled." << std::endl << std::endl;
#else
      std::cout << std::endl << "musrfit: NeXus support NOT enabled." << std::endl << std::endl;
#endif
      return PMUSR_SUCCESS;
    } else if (!strcmp(argv[1], "--show-dynamic-path")) {
      std::cout << std::endl << "musrfit: internal dynamic search paths for shared libraries/root dictionaries:";
      std::cout << std::endl << "  '" << gSystem->GetDynamicPath() << "'" << std::endl << std::endl;
      return PMUSR_SUCCESS;
    } else if (!strcmp(argv[1], "--help")) {
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
        std::cerr << std::endl << "musrfit: **ERROR** found option --dump without <type>" << std::endl;
        show_syntax = true;
        break;
      }
    } else if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--estimateN0")) {
      startup_options.estimateN0 = true;
    } else if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--per-run-block-chisq")) {
      startup_options.writeExpectedChisq = true;
    } else if (!strcmp(argv[i], "--timeout")) {
      if (i<argc-1) {
        TString str(argv[i+1]);
        if (str.IsFloat()) {
          timeout = str.Atoi();
          if (timeout <= 0) {
            timeout_enabled = false;
            std::cout << std::endl << ">> musrfit: timeout disabled." << std::endl;
          }
        } else {
          std::cerr << std::endl << "musrfit: **ERROR** found option --timeout with unsupported <timeout_tag> = " << argv[i+1] << std::endl;
          show_syntax = true;
          break;
        }
        i++;
      } else {
        std::cerr << std::endl << "musrfit: **ERROR** found option --timeout without <timeout_tag>" << std::endl;
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
    std::cout << std::endl << ">> musrfit **ERROR** no msr-file present!" << std::endl;
  }

  if (show_syntax) {
    musrfit_syntax();
    return PMUSR_WRONG_STARTUP_SYNTAX;
  }

  // check if dump string does make sense
  if (!dump.IsNull()) {
    dump.ToLower();
    if (!dump.Contains("ascii") && !dump.Contains("root")) {
      std::cerr << std::endl << "musrfit: **ERROR** found option --dump with unsupported <type> = " << dump << std::endl;
      musrfit_syntax();
      return PMUSR_WRONG_STARTUP_SYNTAX;
    }
  }

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
    status = parseXmlFile(saxParser, startup_path_name);
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

  // read msr-file
  PMsrHandler *msrHandler = nullptr;
  if (startupHandler)
    msrHandler = new PMsrHandler(filename, &startup_options);
  else
    msrHandler = new PMsrHandler(filename);
  status = msrHandler->ReadMsrFile();
  if (status != PMUSR_SUCCESS) {
    switch (status) {
      case PMUSR_MSR_FILE_NOT_FOUND:
        std::cout << std::endl << ">> musrfit **ERROR** couldn't find " << filename << std::endl << std::endl;
        break;
      case PMUSR_MSR_SYNTAX_ERROR:
        std::cout << std::endl << ">> musrfit **SYNTAX ERROR** in file " << filename << ", full stop here." << std::endl << std::endl;
        break;
      default:
        std::cout << std::endl << ">> musrfit **UNKOWN ERROR** when trying to read the msr-file" << std::endl << std::endl;
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

  dataHandler->ReadData();

  bool success = dataHandler->IsAllDataAvailable();
  if (!success) {
    std::cout << std::endl << ">> musrfit **ERROR** Couldn't read all data files, will quit ..." << std::endl;
  }

  // if present, replace the run title of the <msr-file> with the run title of the FIRST run in the run block of the msr-file
  if (title_from_data_file && success) {
    PMsrRunList *rl  = msrHandler->GetMsrRunList();
    PRawRunData *rrd = dataHandler->GetRunData(*(rl->at(0).GetRunName()));
    if (rrd->GetRunTitle()->Length() > 0)
      msrHandler->SetMsrTitle(*rrd->GetRunTitle());
  }

  // generate the necessary fit histogramms for the fit
  PRunListCollection *runListCollection = nullptr;
  if (success) {
    // feed all the necessary histogramms for the fit   
    runListCollection = new PRunListCollection(msrHandler, dataHandler);
    for (unsigned int i=0; i < msrHandler->GetMsrRunList()->size(); i++) {
      success = runListCollection->Add(i, kFit);
      if (!success) {
        std::cout << std::endl << ">> musrfit **ERROR** Couldn't handle run no " << i+1 << ": ";
        std::cout << (*msrHandler->GetMsrRunList())[i].GetRunName()->Data();
        break;
      }
    }
  }

  // start timeout thread
  TThread *th = nullptr;
  if (timeout_enabled) {
    pid_t musrfit_pid = getpid();
    th = new TThread(musrfit_timeout, (void*)&musrfit_pid);
    if (th) {
      th->Run();
    }
  }

  // do fitting
  PFitter *fitter = nullptr;
  if (success) {
    fitter = new PFitter(msrHandler, runListCollection, chisq_only);
    if (fitter->IsValid()) {
      fitter->DoFit();
      if (!fitter->IsScanOnly())
        msrHandler->SetMsrStatisticConverged(fitter->HasConverged());
    }
  }

  // write log file
  if (success && !chisq_only) {
    if (!fitter->IsScanOnly()) {
      status = msrHandler->WriteMsrLogFile();
      if (status != PMUSR_SUCCESS) {
        switch (status) {
        case PMUSR_MSR_LOG_FILE_WRITE_ERROR:
          std::cout << std::endl << ">> musrfit **ERROR** couldn't write mlog-file" << std::endl << std::endl;
          break;
        case PMUSR_TOKENIZE_ERROR:
          std::cout << std::endl << ">> musrfit **ERROR** couldn't generate mlog-file name" << std::endl << std::endl;
          break;
        default:
          std::cout << std::endl << ">> musrfit **UNKOWN ERROR** when trying to write the mlog-file" << std::endl << std::endl;
          break;
        }
      }
    }
  }

  // check if dump is wanted
  if (success && !dump.IsNull()) {
    std::cout << std::endl << "will write dump file ..." << std::endl;
    dump.ToLower();
    if (dump.Contains("ascii"))
      musrfit_dump_ascii(filename, runListCollection);
    else if (dump.Contains("root"))
      musrfit_dump_root(filename, runListCollection);
    else
      std::cout << std::endl << "do not know format " << dump.Data() << ", sorry :-| " << std::endl;
  }

  // rename MINUIT2.OUTPUT and MINUIT2.root file if wanted
  if (success) {
    if (keep_mn2_output && !chisq_only && !fitter->IsScanOnly()) {
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
  }

  if (success) {
    if (!chisq_only && !fitter->IsScanOnly()) {
      // swap msr- and mlog-file
      std::cout << std::endl << ">> swapping msr-, mlog-file ..." << std::endl;
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
      TSystemFile tmp("__temp.msr", "./");
      tmp.Delete();
    }
  }

  // clean up
  if (th) {
    th->Delete();
  }
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
  if (runListCollection) {
    delete runListCollection;
    runListCollection = nullptr;
  }
  if (fitter) {
    delete fitter;
    fitter = nullptr;
  }

  std::cout << std::endl << "done ..." << std::endl;

  return PMUSR_SUCCESS;
}

// end ---------------------------------------------------------------------

