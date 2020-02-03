/***************************************************************************

  PFitter.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2019 by Andreas Suter                              *
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

#ifdef HAVE_GOMP
#include <omp.h>
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>

#include <cmath>

#include <sys/time.h>

#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnContours.h"
#include "Minuit2/MnHesse.h"
#include "Minuit2/MnMinimize.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/MnPlot.h"
#include "Minuit2/MnPrint.h"
#include "Minuit2/MnScan.h"
#include "Minuit2/MnSimplex.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MinosError.h"

#include <TCanvas.h>
#include <TH2.h>
#include <TFile.h>
#include <TDatime.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>

#include "PFitter.h"


//+++ PSectorChisq class +++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 */
PSectorChisq::PSectorChisq(UInt_t noOfRuns) : fNoOfRuns(noOfRuns)
{
  // init
  fLast  = 0.0;
  fChisq = 0.0;
  fExpectedChisq = 0.0;
  fNDF   = 0;
  fFirst.resize(fNoOfRuns);
  fChisqRun.resize(fNoOfRuns);
  fExpectedChisqRun.resize(fNoOfRuns);
  fNDFRun.resize(fNoOfRuns);
  for (UInt_t i=0; i<fNoOfRuns; i++) {
    fFirst[i] = 0.0;
    fChisqRun[i] = 0.0;
    fExpectedChisqRun[i] = 0.0;
    fNDFRun[i] = 0;
  }
}

//--------------------------------------------------------------------------
// SetRunFirstTime
//--------------------------------------------------------------------------
/**
 * <p>Set the time of the fgb of a given RUN
 *
 * @param first time stamp of the fgb
 * @param idx index of the RUN
 */
void PSectorChisq::SetRunFirstTime(Double_t first, UInt_t idx)
{
  if (idx > fNoOfRuns) {
    std::cerr << "**WARNING** from PSectorChisq::SetRunFirstTime. It tries to set" << std::endl;
    std::cerr << "    a fgb time stamp with idx=" << idx << " which is larger than #RUNS=" << fNoOfRuns << "." << std::endl;
    std::cerr << "    Will ignore it, but you better check what is going on!" << std::endl;
    return;
  }

  fFirst[idx] = first;
}

//--------------------------------------------------------------------------
// SetChisq
//--------------------------------------------------------------------------
/**
 * <p>Set the chisq/maxLH for a given detector with index idx.
 *
 * @param chisq chisq/maxLH to be set
 * @param idx index of the run to be set
 */
void PSectorChisq::SetChisq(Double_t chisq, UInt_t idx)
{
  if (idx > fNoOfRuns) {
    std::cerr << "**WARNING** from PSectorChisq::SetChisq. It tries to set" << std::endl;
    std::cerr << "    a chisq with idx=" << idx << " which is larger than #RUNS=" << fNoOfRuns << "." << std::endl;
    std::cerr << "    Will ignore it, but you better check what is going on!" << std::endl;
    return;
  }

  fChisqRun[idx] = chisq;
}

//--------------------------------------------------------------------------
// SetExpectedChisq
//--------------------------------------------------------------------------
/**
 * <p>Set the expected chisq/maxLH for a given detector with index idx.
 *
 * @param chisq expected chisq/maxLH to be set
 * @param idx index of the run to be set
 */
void PSectorChisq::SetExpectedChisq(Double_t chisq, UInt_t idx)
{
  if (idx > fNoOfRuns) {
    std::cerr << "**WARNING** from PSectorChisq::SetExpectedChisq. It tries to set" << std::endl;
    std::cerr << "    a chisq with idx=" << idx << " which is larger than #RUNS=" << fNoOfRuns << "." << std::endl;
    std::cerr << "    Will ignore it, but you better check what is going on!" << std::endl;
    return;
  }

  fExpectedChisqRun[idx] = chisq;
}

//--------------------------------------------------------------------------
// SetNDF
//--------------------------------------------------------------------------
/**
 * <p>Set the NDF for a given detector with index idx.
 *
 * @param ndf to be set
 * @param idx index of the run to be set
 */
void PSectorChisq::SetNDF(UInt_t ndf, UInt_t idx)
{
  if (idx > fNoOfRuns) {
    std::cerr << "**WARNING** from PSectorChisq::SetNDF. It tries to set" << std::endl;
    std::cerr << "    a NDF with idx=" << idx << " which is larger than #RUNS=" << fNoOfRuns << "." << std::endl;
    std::cerr << "    Will ignore it, but you better check what is going on!" << std::endl;
    return;
  }

  fNDFRun[idx] = ndf;
}

//--------------------------------------------------------------------------
// GetTimeRangeFirst
//--------------------------------------------------------------------------
/**
 * <p>Get the fgb time of RUN with index idx. If idx is out-of-range
 * PMUSR_UNDEFINED is returned.
 *
 * @param idx index of the RUN
 *
 * <b>return:</b> return the fgb time of RUN with index idx.
 */
Double_t PSectorChisq::GetTimeRangeFirst(UInt_t idx)
{
  if (idx > fNoOfRuns)
    return PMUSR_UNDEFINED;

  return fFirst[idx];
}

//--------------------------------------------------------------------------
// GetChisq
//--------------------------------------------------------------------------
/**
 * <p>Get chisq/maxLH of the run with index idx
 *
 * @param idx index of the run
 *
 * <b>return:</b> chisq/maxLH of the requested run or -1.0 if the idx is out-of-range.
 */
Double_t PSectorChisq::GetChisq(UInt_t idx)
{
  if (idx >= fNoOfRuns)
    return -1.0;

  return fChisqRun[idx];
}

//--------------------------------------------------------------------------
// GetExpectedChisq
//--------------------------------------------------------------------------
/**
 * <p>Get expected chisq/maxLH of the run with index idx
 *
 * @param idx index of the run
 *
 * <b>return:</b> chisq/maxLH of the requested run or -1.0 if the idx is out-of-range.
 */
Double_t PSectorChisq::GetExpectedChisq(UInt_t idx)
{
  if (idx >= fNoOfRuns)
    return -1.0;

  return fExpectedChisqRun[idx];
}

//--------------------------------------------------------------------------
// GetNDF
//--------------------------------------------------------------------------
/**
 * <p>Get NDF of the run with index idx
 *
 * @param idx index of the run
 *
 * <b>return:</b> NDF of the requested run or 0.0 if the idx is out-of-range.
 */
UInt_t PSectorChisq::GetNDF(UInt_t idx)
{
  if (idx >= fNoOfRuns)
    return 0;

  return fNDFRun[idx];
}

//+++ PFitter class ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param runInfo pointer of the msr-file handler
 * \param runListCollection pointer of the run list collection (pre-processed historgrams)
 * \param chisq_only flag: true=calculate chisq only (no fitting)
 */
PFitter::PFitter(PMsrHandler *runInfo, PRunListCollection *runListCollection, Bool_t chisq_only) :
  fChisqOnly(chisq_only), fRunInfo(runInfo), fRunListCollection(runListCollection)
{
  // initialize variables
  fIsScanOnly = true;
  fConverged = false;
  fUseChi2 = true; // chi^2 is the default

  fStrategy = 1; // 0=low, 1=default, 2=high

  fSectorFlag = false;

  fParams = *(runInfo->GetMsrParamList());
  fCmdLines = *runInfo->GetMsrCommands();

  // init class variables
  fFitterFcn = nullptr;
  fFcnMin = nullptr;

  fScanAll = true;
  fScanParameter[0] = 0;
  fScanParameter[1] = 0;
  fScanNoPoints = 41; // minuit2 default
  fScanLow  = 0.0; // minuit2 default, i.e. 2 std deviations
  fScanHigh = 0.0; // minuit2 default, i.e. 2 std deviations
  fPrintLevel = 1.0;

  // keep all the fit ranges in case RANGE command is present
  PDoublePair rangeGlob;
  PMsrGlobalBlock *global = fRunInfo->GetMsrGlobal();
  rangeGlob.first  = global->GetFitRange(0);
  rangeGlob.second = global->GetFitRange(1);

  PMsrRunList *runs = fRunInfo->GetMsrRunList();
  PDoublePair range;
  for (UInt_t i=0; i<runs->size(); i++) {
    range.first  = (*runs)[i].GetFitRange(0);
    range.second = (*runs)[i].GetFitRange(1);
    if (range.first == PMUSR_UNDEFINED)
      fOriginalFitRange.push_back(rangeGlob);
    else
      fOriginalFitRange.push_back(range);
  }

  // check msr minuit commands
  if (!CheckCommands()) {
    return;
  }

  // create fit function object
  fFitterFcn = new PFitterFcn(runListCollection, fUseChi2);
  if (!fFitterFcn) {
    fIsValid = false;
    return;
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor.
 */
PFitter::~PFitter()
{
  fCmdList.clear();

  fScanData.clear();

  fElapsedTime.clear();

  if (fFcnMin) {
    delete fFcnMin;
    fFcnMin = nullptr;
  }

  if (fFitterFcn) {
    delete fFitterFcn;
    fFitterFcn = nullptr;
  }
}

//--------------------------------------------------------------------------
// DoFit
//--------------------------------------------------------------------------
/**
 * <p>Main calling routine to invoke minuit2, i.e. fitting etc.
 *
 * <b>return:</b> true if all commands could be executed successfully, otherwise returns false.
 */
Bool_t PFitter::DoFit()
{
  // feed minuit parameters
  SetParameters();

  // check if only chisq/maxLH shall be calculated once
  if (fChisqOnly) {
    std::vector<Double_t> param = fMnUserParams.Params();
    std::vector<Double_t> error = fMnUserParams.Errors();
    Int_t usedParams = 0;
    for (UInt_t i=0; i<error.size(); i++) {
      if (error[i] != 0.0)
        usedParams++;
    }
    UInt_t ndf = static_cast<int>(fFitterFcn->GetTotalNoOfFittedBins()) - usedParams;
    Double_t val = (*fFitterFcn)(param);
    if (fUseChi2) {
      // calculate expected chisq
      Double_t totalExpectedChisq = 0.0;
      PDoubleVector expectedChisqPerRun;
      fFitterFcn->CalcExpectedChiSquare(param, totalExpectedChisq, expectedChisqPerRun);
      // calculate chisq per run
      std::vector<Double_t> chisqPerRun;
      for (UInt_t i=0; i<fRunInfo->GetMsrRunList()->size(); i++) {
        chisqPerRun.push_back(fRunListCollection->GetSingleRunChisq(param, i));
      }

      std::cout << std::endl << std::endl << ">> chisq = " << val << ", NDF = " << ndf << ", chisq/NDF = " << val/ndf;

      if (totalExpectedChisq != 0.0) {
        std::cout << std::endl << ">> expected chisq = " << totalExpectedChisq << ", NDF = " << ndf << ", expected chisq/NDF = " << totalExpectedChisq/ndf;
        UInt_t ndf_run = 0;
        for (UInt_t i=0; i<expectedChisqPerRun.size(); i++) {
          ndf_run = fFitterFcn->GetNoOfFittedBins(i) - fRunInfo->GetNoOfFitParameters(i);
          if (ndf_run > 0)
            std::cout << std::endl << ">> run block " << i+1 << ": (NDF/red.chisq/red.chisq_e) = (" << ndf_run << "/" << chisqPerRun[i]/ndf_run << "/" << expectedChisqPerRun[i]/ndf_run << ")";
        }
      } else if (chisqPerRun.size() > 0) { // in case expected chisq is not applicable like for asymmetry fits
        UInt_t ndf_run = 0;
        for (UInt_t i=0; i<chisqPerRun.size(); i++) {
          ndf_run = fFitterFcn->GetNoOfFittedBins(i) - fRunInfo->GetNoOfFitParameters(i);
          if (ndf_run > 0)
            std::cout << std::endl << ">> run block " << i+1 << ": (NDF/red.chisq) = (" << ndf_run << "/" << chisqPerRun[i]/ndf_run << ")";
        }
      }

      // clean up
      chisqPerRun.clear();
      expectedChisqPerRun.clear();

      if (fSectorFlag) {
        PDoublePairVector secFitRange;
        secFitRange.resize(1);
        for (UInt_t k=0; k<fSector.size(); k++) {
          // set sector fit range
          secFitRange[0].first = fSector[k].GetTimeRangeFirst(0);
          secFitRange[0].second = fSector[k].GetTimeRangeLast();
          fRunListCollection->SetFitRange(secFitRange);
          // calculate chisq
          val = (*fFitterFcn)(param);
          // calculate NDF
          ndf = static_cast<UInt_t>(fFitterFcn->GetTotalNoOfFittedBins()) - usedParams;
          // calculate expected chisq
          totalExpectedChisq = 0.0;
          fFitterFcn->CalcExpectedChiSquare(param, totalExpectedChisq, expectedChisqPerRun);
          // calculate chisq per run
          for (UInt_t i=0; i<fRunInfo->GetMsrRunList()->size(); i++) {
            chisqPerRun.push_back(fRunListCollection->GetSingleRunChisq(param, i));
          }

          std::cout << std::endl;
          std::cout << "++++" << std::endl;
          std::cout << ">> Sector " << k+1 << ": FitRange: " << secFitRange[0].first << ", " << secFitRange[0].second << std::endl;
          std::cout << ">> chisq = " << val << ", NDF = " << ndf << ", chisq/NDF = " << val/ndf;

          if (totalExpectedChisq != 0.0) {
            std::cout << std::endl << ">> expected chisq = " << totalExpectedChisq << ", NDF = " << ndf << ", expected chisq/NDF = " << totalExpectedChisq/ndf;
            UInt_t ndf_run = 0;
            for (UInt_t i=0; i<expectedChisqPerRun.size(); i++) {
              ndf_run = fFitterFcn->GetNoOfFittedBins(i) - fRunInfo->GetNoOfFitParameters(i);
              if (ndf_run > 0)
                std::cout << std::endl << ">> run block " << i+1 << ": (NDF/red.chisq/red.chisq_e) = (" << ndf_run << "/" << chisqPerRun[i]/ndf_run << "/" << expectedChisqPerRun[i]/ndf_run << ")";
            }
          } else if (chisqPerRun.size() > 0) { // in case expected chisq is not applicable like for asymmetry fits
            UInt_t ndf_run = 0;
            for (UInt_t i=0; i<chisqPerRun.size(); i++) {
              ndf_run = fFitterFcn->GetNoOfFittedBins(i) - fRunInfo->GetNoOfFitParameters(i);
              if (ndf_run > 0)
                std::cout << std::endl << ">> run block " << i+1 << ": (NDF/red.chisq) = (" << ndf_run << "/" << chisqPerRun[i]/ndf_run << ")";
            }
          }
          // clean up
          chisqPerRun.clear();
          expectedChisqPerRun.clear();
        }
      }
    } else { // max. log likelihood
      // calculate expected maxLH
      Double_t totalExpectedMaxLH = 0.0;
      std::vector<Double_t> expectedMaxLHPerRun;
      fFitterFcn->CalcExpectedChiSquare(param, totalExpectedMaxLH, expectedMaxLHPerRun);
      // calculate maxLH per run
      std::vector<Double_t> maxLHPerRun;
      for (UInt_t i=0; i<fRunInfo->GetMsrRunList()->size(); i++) {
        maxLHPerRun.push_back(fRunListCollection->GetSingleRunMaximumLikelihood(param, i));
      }

      std::cout << std::endl << std::endl << ">> maxLH = " << val << ", NDF = " << ndf << ", maxLH/NDF = " << val/ndf;

      if (totalExpectedMaxLH != 0.0) {
        std::cout << std::endl << ">> expected maxLH = " << totalExpectedMaxLH << ", NDF = " << ndf << ", expected maxLH/NDF = " << totalExpectedMaxLH/ndf;
        UInt_t ndf_run = 0;
        for (UInt_t i=0; i<expectedMaxLHPerRun.size(); i++) {
          ndf_run = fFitterFcn->GetNoOfFittedBins(i) - fRunInfo->GetNoOfFitParameters(i);
          if (ndf_run > 0)
            std::cout << std::endl << ">> run block " << i+1 << ": (NDF/red.maxLH/red.maxLH_e) = (" << ndf_run << "/" << maxLHPerRun[i]/ndf_run << "/" << expectedMaxLHPerRun[i]/ndf_run << ")";
        }
      }

      // clean up
      maxLHPerRun.clear();
      expectedMaxLHPerRun.clear();

      if (fSectorFlag) {
        PDoublePairVector secFitRange;
        secFitRange.resize(1);
        for (UInt_t k=0; k<fSector.size(); k++) {
          // set sector fit range
          secFitRange[0].first = fSector[k].GetTimeRangeFirst(0);
          secFitRange[0].second = fSector[k].GetTimeRangeLast();
          fRunListCollection->SetFitRange(secFitRange);
          // calculate maxLH
          val = (*fFitterFcn)(param);
          // calculate NDF
          ndf = static_cast<int>(fFitterFcn->GetTotalNoOfFittedBins()) - usedParams;
          // calculate expected maxLH
          totalExpectedMaxLH = 0.0;
          fFitterFcn->CalcExpectedChiSquare(param, totalExpectedMaxLH, expectedMaxLHPerRun);
          // calculate maxLH per run
          for (UInt_t i=0; i<fRunInfo->GetMsrRunList()->size(); i++) {
            maxLHPerRun.push_back(fRunListCollection->GetSingleRunMaximumLikelihood(param, i));
          }

          std::cout << std::endl;
          std::cout << "++++" << std::endl;
          std::cout << ">> Sector " << k+1 << ": FitRange: " << secFitRange[0].first << ", " << secFitRange[0].second << std::endl;
          std::cout << ">> maxLH = " << val << ", NDF = " << ndf << ", maxLH/NDF = " << val/ndf;

          if (totalExpectedMaxLH != 0.0) {
            std::cout << std::endl << ">> expected maxLH = " << totalExpectedMaxLH << ", NDF = " << ndf << ", expected maxLH/NDF = " << totalExpectedMaxLH/ndf;
            UInt_t ndf_run = 0;
            for (UInt_t i=0; i<expectedMaxLHPerRun.size(); i++) {
              ndf_run = fFitterFcn->GetNoOfFittedBins(i) - fRunInfo->GetNoOfFitParameters(i);
              if (ndf_run > 0)
                std::cout << std::endl << ">> run block " << i+1 << ": (NDF/red.maxLH/red.maxLH_e) = (" << ndf_run << "/" << maxLHPerRun[i]/ndf_run << "/" << expectedMaxLHPerRun[i]/ndf_run << ")";
            }
          }

          // clean up
          maxLHPerRun.clear();
          expectedMaxLHPerRun.clear();
        }
      }
    }
    std::cout << std::endl << std::endl;
    return true;
  }

  // debugging information
  #ifdef HAVE_GOMP
  std::cout << std::endl << ">> Number of available threads for the function optimization: " << omp_get_num_procs() << std::endl;
  #endif

  // real fit wanted
  if (fUseChi2)
    std::cout << std::endl << ">> Chi Square fit will be executed" << std::endl;
  else
    std::cout << std::endl << ">> Maximum Likelihood fit will be executed" << std::endl;

  Bool_t status = true;
  // init positive errors to default false, if minos is called, it will be set true there
  for (UInt_t i=0; i<fParams.size(); i++) {
    fRunInfo->SetMsrParamPosErrorPresent(i, false);
  }

  // walk through the command list and execute them
  Bool_t firstSave = true;
  for (UInt_t i=0; i<fCmdList.size(); i++) {
    switch (fCmdList[i].first) {
      case PMN_INTERACTIVE:
        std::cerr << std::endl << "**WARNING** from PFitter::DoFit() : the command INTERACTIVE is not yet implemented.";
        std::cerr << std::endl;
        break;
      case PMN_CONTOURS:
        status = ExecuteContours();
        break;
      case PMN_FIT_RANGE:
        status = ExecuteFitRange(fCmdList[i].second);
        break;
      case PMN_FIX:
        status = ExecuteFix(fCmdList[i].second);
        break;
      case PMN_EIGEN:
        std::cerr  << std::endl << "**WARNING** from PFitter::DoFit() : the command EIGEN is not yet implemented.";
        std::cerr  << std::endl;
        break;
      case PMN_HESSE:
        status = ExecuteHesse();
        break;
      case PMN_MACHINE_PRECISION:
        std::cerr  << std::endl << "**WARNING** from PFitter::DoFit() : the command MACHINE_PRECISION is not yet implemented.";
        std::cerr  << std::endl;
        break;
      case PMN_MIGRAD:
        status = ExecuteMigrad();
        break;
      case PMN_MINIMIZE:
        status = ExecuteMinimize();
        break;
      case PMN_MINOS:
        status = ExecuteMinos();
        break;
      case PMN_PLOT:
        status = ExecutePlot();
        break;
      case PMN_RELEASE:
        status = ExecuteRelease(fCmdList[i].second);
        break;
      case PMN_RESTORE:
        status = ExecuteRestore();
        break;
      case PMN_SAVE:
        status = ExecuteSave(firstSave);
        if (firstSave)
          firstSave = false;
        break;
      case PMN_SCAN:
        status = ExecuteScan();
        break;
      case PMN_SECTOR:
        // nothing to be done here
        break;
      case PMN_SIMPLEX:
        status = ExecuteSimplex();
        break;
      case PMN_USER_COVARIANCE:
        std::cerr  << std::endl << "**WARNING** from PFitter::DoFit() : the command USER_COVARIANCE is not yet implemented.";
        std::cerr  << std::endl;
        break;
      case PMN_USER_PARAM_STATE:
        std::cerr  << std::endl << "**WARNING** from PFitter::DoFit() : the command USER_PARAM_STATE is not yet implemented.";
        std::cerr  << std::endl;
        break;
      case PMN_PRINT:
        status = ExecutePrintLevel(fCmdList[i].second);
        break;
      default:
        std::cerr  << std::endl << "**PANIC ERROR**: PFitter::DoFit(): You should never have reached this point";
        std::cerr  << std::endl;
        exit(0);
    }

    // check if command has been successful
    if (!status)
      break;
  }

  if (IsValid()) {
    fRunInfo->GetMsrStatistic()->fValid = true;
  } else {
    fRunInfo->GetMsrStatistic()->fValid = false;
  }

  return true;
}

//--------------------------------------------------------------------------
// CheckCommands
//--------------------------------------------------------------------------
/**
 * <p>Check the msr-file COMMAND's, fill the command queue and make sure that given parameters (if present)
 * do make any sense.
 *
 * <b>return:</b> true if the commands are valid, otherwise returns false.
 */
Bool_t PFitter::CheckCommands()
{
  fIsValid = true;

  // check if chisq or log max likelihood fit
  fUseChi2 = fRunInfo->GetMsrStatistic()->fChisq;

  // walk through the msr-file COMMAND block
  PIntPair cmd;
  PMsrLines::iterator it;
  UInt_t cmdLineNo = 0;
  TString line;
  Ssiz_t pos;
  for (it = fCmdLines.begin(); it != fCmdLines.end(); ++it) {
    if (it == fCmdLines.begin())
      cmdLineNo = 0;
    else
      cmdLineNo++;

    // strip potential comments
    line = it->fLine;
    pos = line.First('#');
    if (pos > 0) // comment present
      line.Remove(pos,line.Length()-pos);

    if (line.Contains("COMMANDS", TString::kIgnoreCase)) {
      continue;
    } else if (line.Contains("SET BATCH", TString::kIgnoreCase)) { // needed for backward compatibility
      continue;
    } else if (line.Contains("END RETURN", TString::kIgnoreCase)) {  // needed for backward compatibility
      continue;
    } else if (line.Contains("CHI_SQUARE", TString::kIgnoreCase)) {
      continue;
    } else if (line.Contains("MAX_LIKELIHOOD", TString::kIgnoreCase)) {
      continue;
    } else if (line.Contains("SCALE_N0_BKG", TString::kIgnoreCase)) {
      continue;
    } else if (line.Contains("INTERACTIVE", TString::kIgnoreCase)) {
      cmd.first  = PMN_INTERACTIVE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("CONTOURS", TString::kIgnoreCase)) {
      cmd.first  = PMN_CONTOURS;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
      // filter out possible parameters for scan
      TObjArray *tokens = nullptr;
      TObjString *ostr;
      TString str;
      UInt_t ival;

      tokens = line.Tokenize(", \t");

      for (Int_t i=0; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();

        if ((i==1) || (i==2)) { // parX / parY
          // check that token is a UInt_t
          if (!str.IsDigit()) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> parameter number is not number!";
            std::cerr  << std::endl << ">> command syntax for CONTOURS is: CONTOURS parameter-X parameter-Y [# of points]";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
          ival = str.Atoi();
          // check that parameter is within range
          if ((ival < 1) || (ival > fParams.size())) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> parameter number is out of range [1," << fParams.size() << "]!";
            std::cerr  << std::endl << ">> command syntax for CONTOURS is: CONTOURS parameter-X parameter-Y [# of points]";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
          // keep parameter
          fScanParameter[i-1] = ival-1; // internally parameter number starts at 0
          fScanAll = false;
        }

        if (i==3) {
          // check that token is a UInt_t
          if (!str.IsDigit()) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> number of points is not number!";
            std::cerr  << std::endl << ">> command syntax for CONTOURS is: CONTOURS parameter-X parameter-Y [# of points]";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
          ival = str.Atoi();
          if ((ival < 1) || (ival > 100)) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> number of scan points is out of range [1,100]!";
            std::cerr  << std::endl << ">> command syntax for CONTOURS is: CONTOURS parameter-X parameter-Y [# of points]";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
          fScanNoPoints = ival;
        }
      }

      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
    } else if (line.Contains("EIGEN", TString::kIgnoreCase)) {
      cmd.first  = PMN_EIGEN;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("FIT_RANGE", TString::kIgnoreCase)) {
      // check the 5 options:
      // (i)   FIT_RANGE RESET,
      // (ii)  FIT_RANGE start end,
      // (iii) FIT_RANGE start1 end1 start2 end2 ... startN endN
      // (iv)  FIT_RANGE fgb+n0 lgb-n1
      // (v)   FIT_RANGE fgb+n00 lgb-n01 fgb+n10 lgb-n11 ... fgb+nN0 lgb-nN1
      TObjArray *tokens = nullptr;
      TObjString *ostr;
      TString str;

      tokens = line.Tokenize(", \t");

      if (tokens->GetEntries() == 2) { // should only be RESET
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.Contains("RESET", TString::kIgnoreCase)) {
          cmd.first = PMN_FIT_RANGE;
          cmd.second = cmdLineNo;
          fCmdList.push_back(cmd);
        } else {
          std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
          std::cerr  << std::endl << ">> " << line.Data();
          std::cerr  << std::endl << ">> Syntax: FIT_RANGE RESET | FIT_RANGE start end | FIT_RANGE s1 e1 s2 e2 .. sN eN,";
          std::cerr  << std::endl << ">>         with N the number of runs in the msr-file." << std::endl;
          std::cerr  << std::endl << ">>         Found " << str.Data() << ", instead of RESET" << std::endl;
          fIsValid = false;
          if (tokens) {
            delete tokens;
            tokens = nullptr;
          }
          break;
        }
      } else if ((tokens->GetEntries() > 1) && (static_cast<UInt_t>(tokens->GetEntries()) % 2) == 1) {
        if ((tokens->GetEntries() > 3) && ((static_cast<UInt_t>(tokens->GetEntries())-1)) != 2*fRunInfo->GetMsrRunList()->size()) {
          std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
          std::cerr  << std::endl << ">> " << line.Data();
          std::cerr  << std::endl << ">> Syntax: FIT_RANGE RESET | FIT_RANGE <start> <end> | FIT_RANGE <s1> <e1> <s2> <e2> .. <sN> <eN> |";
          std::cerr  << std::endl << ">>         FIT_RANGE fgb+<n0> lgb-<n1> | FIT_RANGE fgb+<n00> lgb-<n01> fgb+<n10> lgb-<n11> ... fgb+<nN0> lgb-<nN1>,";
          std::cerr  << std::endl << ">>         with N the number of runs in the msr-file.";
          std::cerr  << std::endl << ">>         Found N=" << (tokens->GetEntries()-1)/2 << ", # runs in msr-file=" << fRunInfo->GetMsrRunList()->size() << std::endl;
          fIsValid = false;
          if (tokens) {
            delete tokens;
            tokens = nullptr;
          }
          break;
        } else {
          // check that all range entries are numbers or fgb+n0 / lgb-n1
          Bool_t ok = true;
          for (Int_t n=1; n<tokens->GetEntries(); n++) {
            ostr = dynamic_cast<TObjString*>(tokens->At(n));
            str = ostr->GetString();
            if (!str.IsFloat()) {
              if ((n%2 == 1) && (!str.Contains("fgb", TString::kIgnoreCase)))
                ok = false;
              if ((n%2 == 0) && (!str.Contains("lgb", TString::kIgnoreCase)))
                ok = false;
            }
            if (!ok)
              break;
          }

          if (ok) { // everything is fine
            cmd.first = PMN_FIT_RANGE;
            cmd.second = cmdLineNo;
            fCmdList.push_back(cmd);
          } else {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> Syntax: FIT_RANGE RESET | FIT_RANGE <start> <end> | FIT_RANGE <s1> <e1> <s2> <e2> .. <sN> <eN> |";
            std::cerr  << std::endl << ">>         FIT_RANGE fgb+<n0> lgb-<n1> | FIT_RANGE fgb+<n00> lgb-<n01> fgb+<n10> lgb-<n11> ... fgb+<nN0> lgb-<nN1>,";
            std::cerr  << std::endl << ">>         with N the number of runs in the msr-file.";
            std::cerr  << std::endl << ">>         Found token '" << str.Data() << "', which is not a floating point number." << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
        }
      } else {
        std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
        std::cerr  << std::endl << ">> " << line.Data();
        std::cerr  << std::endl << ">> Syntax: FIT_RANGE RESET | FIT_RANGE <start> <end> | FIT_RANGE <s1> <e1> <s2> <e2> .. <sN> <eN> |";
        std::cerr  << std::endl << ">>         FIT_RANGE fgb+<n0> lgb-<n1> | FIT_RANGE fgb+<n00> lgb-<n01> fgb+<n10> lgb-<n11> ... fgb+<nN0> lgb-<nN1>,";
        std::cerr  << std::endl << ">>         with N the number of runs in the msr-file.";
        fIsValid = false;
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
        break;
      }

      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
    } else if (line.Contains("FIX", TString::kIgnoreCase)) {
      // check if the given set of parameters (number or names) is present
      TObjArray *tokens = nullptr;
      TObjString *ostr;
      TString str;
      UInt_t ival;

      tokens = line.Tokenize(", \t");

      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();

        if (str.IsDigit()) { // token might be a parameter number
          ival = str.Atoi();
          // check that ival is in the parameter list
          if (ival > fParams.size()) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> Parameter " << ival << " is out of the Parameter Range [1," << fParams.size() << "]";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
        } else { // token might be a parameter name
          // check if token is present as parameter name
          Bool_t found = false;
          for (UInt_t j=0; j<fParams.size(); j++) {
            if (fParams[j].fName.CompareTo(str, TString::kIgnoreCase) == 0) { // found
              found = true;
              break;
            }
          }
          if (!found) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> Parameter '" << str.Data() << "' is NOT present as a parameter name";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
        }
      }

      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }

      // everything looks fine, feed the command list
      cmd.first  = PMN_FIX;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("HESSE", TString::kIgnoreCase)) {
      fIsScanOnly = false;
      cmd.first  = PMN_HESSE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("MACHINE_PRECISION", TString::kIgnoreCase)) {
      cmd.first  = PMN_MACHINE_PRECISION;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("MIGRAD", TString::kIgnoreCase)) {
      fIsScanOnly = false;
      cmd.first  = PMN_MIGRAD;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("MINIMIZE", TString::kIgnoreCase)) {
      fIsScanOnly = false;
      cmd.first  = PMN_MINIMIZE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("MINOS", TString::kIgnoreCase)) {
      fIsScanOnly = false;
      cmd.first  = PMN_MINOS;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("MNPLOT", TString::kIgnoreCase)) {
      cmd.first  = PMN_PLOT;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("PRINT_LEVEL", TString::kIgnoreCase)) {
      cmd.first = PMN_PRINT;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("RELEASE", TString::kIgnoreCase)) {
      // check if the given set of parameters (number or names) is present
      TObjArray *tokens = nullptr;
      TObjString *ostr;
      TString str;
      UInt_t ival;

      tokens = line.Tokenize(", \t");

      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();

        if (str.IsDigit()) { // token might be a parameter number
          ival = str.Atoi();
          // check that ival is in the parameter list
          if (ival > fParams.size()) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> Parameter " << ival << " is out of the Parameter Range [1," << fParams.size() << "]";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
        } else { // token might be a parameter name
          // check if token is present as parameter name
          Bool_t found = false;
          for (UInt_t j=0; j<fParams.size(); j++) {
            if (fParams[j].fName.CompareTo(str, TString::kIgnoreCase) == 0) { // found
              found = true;
              break;
            }
          }
          if (!found) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> Parameter '" << str.Data() << "' is NOT present as a parameter name";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
        }
      }

      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
      cmd.first  = PMN_RELEASE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("RESTORE", TString::kIgnoreCase)) {
      cmd.first  = PMN_RESTORE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("SAVE", TString::kIgnoreCase)) {
      cmd.first  = PMN_SAVE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("SCAN", TString::kIgnoreCase)) {
      cmd.first  = PMN_SCAN;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
      // filter out possible parameters for scan
      TObjArray *tokens = nullptr;
      TObjString *ostr;
      TString str;
      UInt_t ival;

      tokens = line.Tokenize(", \t");

      for (Int_t i=0; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();
        if (i==1) { // get parameter number
          // check that token is a UInt_t
          if (!str.IsDigit()) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> parameter number is not number!";
            std::cerr  << std::endl << ">> command syntax for SCAN is: SCAN [parameter no [# of points [low high]]]";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
          ival = str.Atoi();
          // check that parameter is within range
          if ((ival < 1) || (ival > fParams.size())) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> parameter number is out of range [1," << fParams.size() << "]!";
            std::cerr  << std::endl << ">> command syntax for SCAN is: SCAN [parameter no [# of points [low high]]]";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
          // keep parameter
          fScanParameter[0] = ival-1; // internally parameter number starts at 0
          fScanAll = false;
        }

        if (i==2) { // get number of points
          // check that token is a UInt_t
          if (!str.IsDigit()) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> number of points is not number!";
            std::cerr  << std::endl << ">> command syntax for SCAN is: SCAN [parameter no [# of points [low high]]]";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
          ival = str.Atoi();
          if ((ival < 1) || (ival > 100)) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> number of scan points is out of range [1,100]!";
            std::cerr  << std::endl << ">> command syntax for SCAN is: SCAN [parameter no [# of points [low high]]]";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
          fScanNoPoints = ival;
        }

        if (i==3) { // get low
          // check that token is a Double_t
          if (!str.IsFloat()) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> low is not a floating point number!";
            std::cerr  << std::endl << ">> command syntax for SCAN is: SCAN [parameter no [# of points [low high]]]";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
          fScanLow = str.Atof();
        }

        if (i==4) { // get high
          // check that token is a Double_t
          if (!str.IsFloat()) {
            std::cerr  << std::endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            std::cerr  << std::endl << ">> " << line.Data();
            std::cerr  << std::endl << ">> high is not a floating point number!";
            std::cerr  << std::endl << ">> command syntax for SCAN is: SCAN [parameter no [# of points [low high]]]";
            std::cerr  << std::endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = nullptr;
            }
            break;
          }
          fScanHigh = str.Atof();
        }
      }

      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
    } else if (line.Contains("SIMPLEX", TString::kIgnoreCase)) {
      cmd.first  = PMN_SIMPLEX;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("STRATEGY", TString::kIgnoreCase)) {
      TObjArray *tokens = nullptr;
      TObjString *ostr;
      TString str;

      tokens = line.Tokenize(" \t");
      if (tokens->GetEntries() == 2) {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.CompareTo("0") == 0) { // low
          fStrategy = 0;
        } else if (str.CompareTo("1") == 0) { // default
          fStrategy = 1;
        } else if (str.CompareTo("2") == 0) { // high
          fStrategy = 2;
        } else if (str.CompareTo("LOW") == 0) { // low
          fStrategy = 0;
        } else if (str.CompareTo("DEFAULT") == 0) { // default
          fStrategy = 1;
        } else if (str.CompareTo("HIGH") == 0) { // high
          fStrategy = 2;
        }
      }

      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
    } else if (line.Contains("USER_COVARIANCE", TString::kIgnoreCase)) {
      cmd.first  = PMN_USER_COVARIANCE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("USER_PARAM_STATE", TString::kIgnoreCase)) {
      cmd.first  = PMN_USER_PARAM_STATE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (line.Contains("SECTOR", TString::kIgnoreCase)) {
      fSectorFlag = true;
      cmd.first  = PMN_SECTOR;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);

      // check if the given sector arguments are valid time stamps, i.e. doubles and value < lgb time stamp
      TObjArray *tokens = nullptr;
      TObjString *ostr;
      TString str;

      tokens = line.Tokenize(" ,\t");

      if (tokens->GetEntries() == 1) { // no sector time stamps given -> issue an error
        std::cerr << std::endl << ">> PFitter::CheckCommands(): **FATAL ERROR** in line " << it->fLineNo;
        std::cerr << std::endl << ">> " << line.Data();
        std::cerr << std::endl << ">> At least one sector time stamp is expected.";
        std::cerr << std::endl << ">> Will stop ...";
        std::cerr << std::endl;
        // cleanup
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
        fIsValid = false;
        fSectorFlag = false;
        break;
      }

      Double_t dval;
      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        // keep time range of sector
        PSectorChisq sec(fRunInfo->GetNoOfRuns());
        // get parse tokens
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();
        if (str.IsFloat()) {
          dval = str.Atof();
          // check that the sector time stamp is smaller than all lgb time stamps
          for (UInt_t j=0; j<fOriginalFitRange.size(); j++) {
            if (dval > fOriginalFitRange[j].second) {
              std::cerr << std::endl << ">> PFitter::CheckCommands(): **FATAL ERROR** in line " << it->fLineNo;
              std::cerr << std::endl << ">> " << line.Data();
              std::cerr << std::endl << ">> The sector time stamp " << dval << " is > as the lgb time stamp (" << fOriginalFitRange[j].second << ") of run " << j << ".";
              std::cerr << std::endl << ">> Will stop ...";
              std::cerr << std::endl;
              // cleanup
              if (tokens) {
                delete tokens;
                tokens = nullptr;
              }
              fIsValid = false;
              fSectorFlag = false;
              return fIsValid;
            }
            sec.SetRunFirstTime(fOriginalFitRange[j].first, j); // keep fgb time stamp for sector
          }
          sec.SetSectorTime(dval);
          fSector.push_back(sec);
        } else { // sector element is NOT a float
          std::cerr << std::endl << ">> PFitter::CheckCommands(): **FATAL ERROR** in line " << it->fLineNo;
          std::cerr << std::endl << ">> " << line.Data();
          std::cerr << std::endl << ">> The sector time stamp '" << str << "' is not a number.";
          std::cerr << std::endl << ">> Will stop ...";
          std::cerr << std::endl;
          // cleanup
          if (tokens) {
            delete tokens;
            tokens = nullptr;
          }
          fIsValid = false;
          fSectorFlag = false;
          break;
        }
      }

      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
    } else { // unkown command
      std::cerr << std::endl << ">> PFitter::CheckCommands(): **FATAL ERROR** in line " << it->fLineNo << " an unkown command is found:";
      std::cerr << std::endl << ">> " << line.Data();
      std::cerr << std::endl << ">> Will stop ...";
      std::cerr << std::endl;
      fIsValid = false;
      break;
    }
  }

  // Check that in case release/restore is present, that it is followed by a minimizer before minos is called.
  // If this is not the case, place a warning
  Bool_t fixFlag = false;
  Bool_t releaseFlag = false;
  Bool_t minimizerFlag = false;
  for (it = fCmdLines.begin(); it != fCmdLines.end(); ++it) {
    if (line.Contains("FIX", TString::kIgnoreCase))
      fixFlag = true;
    else if (line.Contains("RELEASE", TString::kIgnoreCase) ||
             line.Contains("RESTORE", TString::kIgnoreCase))
      releaseFlag = true;
    else if (line.Contains("MINIMIZE", TString::kIgnoreCase) ||
             line.Contains("MIGRAD", TString::kIgnoreCase) ||
             line.Contains("SIMPLEX", TString::kIgnoreCase)) {
      if (releaseFlag)
        minimizerFlag = true;
    } else if (line.Contains("MINOS", TString::kIgnoreCase)) {
      if (fixFlag && releaseFlag && !minimizerFlag) {
        std::cerr  << std::endl << ">> PFitter::CheckCommands(): **WARNING** RELEASE/RESTORE command present";
        std::cerr  << std::endl << ">> without minimizer command (MINIMIZE/MIGRAD/SIMPLEX) between";
        std::cerr  << std::endl << ">> RELEASE/RESTORE and MINOS. Behaviour might be different to the";
        std::cerr  << std::endl << ">> expectation of the user ?!?" << std::endl;
      }
      fixFlag = false;
      releaseFlag = false;
      minimizerFlag = false;
    }
  }

  return fIsValid;
}

//--------------------------------------------------------------------------
// SetParameters
//--------------------------------------------------------------------------
/**
 * <p>Feeds the internal minuit2 fit parameters. It also makes sure that unused parameters
 * are fixed.
 *
 * <b>return:</b> true.
 */
Bool_t PFitter::SetParameters()
{
  for (UInt_t i=0; i<fParams.size(); i++) {
    // check if parameter is fixed
    if (fParams[i].fStep == 0.0) { // add fixed parameter
      fMnUserParams.Add(fParams[i].fName.Data(), fParams[i].fValue);
    } else { // add free parameter
      // check if boundaries are given
      if (fParams[i].fNoOfParams > 5) { // boundaries given
        if (fParams[i].fLowerBoundaryPresent && 
            fParams[i].fUpperBoundaryPresent) { // upper and lower boundaries given
          fMnUserParams.Add(fParams[i].fName.Data(), fParams[i].fValue, fParams[i].fStep,
                            fParams[i].fLowerBoundary, fParams[i].fUpperBoundary);
        } else if (fParams[i].fLowerBoundaryPresent && 
                   !fParams[i].fUpperBoundaryPresent) { // lower boundary limited
          fMnUserParams.Add(fParams[i].fName.Data(), fParams[i].fValue, fParams[i].fStep);
          fMnUserParams.SetLowerLimit(fParams[i].fName.Data(), fParams[i].fLowerBoundary);
        } else { // upper boundary limited
          fMnUserParams.Add(fParams[i].fName.Data(), fParams[i].fValue, fParams[i].fStep);
          fMnUserParams.SetUpperLimit(fParams[i].fName.Data(), fParams[i].fUpperBoundary);
        }
      } else { // no boundaries given
        fMnUserParams.Add(fParams[i].fName.Data(), fParams[i].fValue, fParams[i].fStep);
      }
    }
  }

  // check if there is an unused parameter, if so, fix it
  for (UInt_t i=0; i<fParams.size(); i++) {
    // parameter not used in the whole theory and not already fixed!!
    if ((fRunInfo->ParameterInUse(i) == 0) && (fParams[i].fStep != 0.0)) {
      fMnUserParams.Fix(i); // fix the unused parameter so that minuit will not vary it
      std::cerr  << std::endl << "**WARNING** : Parameter No " << i+1 << " is not used at all, will fix it";
      std::cerr  << std::endl;
    }
  }

  return true;
}

//--------------------------------------------------------------------------
// ExecuteContours
//--------------------------------------------------------------------------
/**
 * <p>Execute the minuit2 contour command. Makes sure that a valid minuit2 minimum is present.
 *
 * <b>return:</b> true if the contour command could be executed successfully, otherwise returns false.
 */
Bool_t PFitter::ExecuteContours()
{
  std::cout << ">> PFitter::ExecuteContours() ..." << std::endl;

  // if already some minimization is done use the minuit2 output as input
  if (!fFcnMin) {
    std::cerr  << std::endl << "**WARNING**: CONTOURS musn't be called before any minimization (MINIMIZE/MIGRAD/SIMPLEX) is done!!";
    std::cerr  << std::endl;
    return false;
  }

  // check if minimum was valid
  if (!fFcnMin->IsValid()) {
    std::cerr  << std::endl << "**ERROR**: CONTOURS cannot started since the previous minimization failed :-(";
    std::cerr  << std::endl;
    return false;
  }

  ROOT::Minuit2::MnContours contours((*fFitterFcn), *fFcnMin);

  fScanData = contours(fScanParameter[0], fScanParameter[1], fScanNoPoints);

  return true;
}

//--------------------------------------------------------------------------
// ExecuteFitRange
//--------------------------------------------------------------------------
/**
 * <p>Change the fit range via command block.
 *
 * \param lineNo the line number of the command block
 *
 * <b>return:</b> true if done, otherwise returns false.
 */
Bool_t PFitter::ExecuteFitRange(UInt_t lineNo)
{
  std::cout << ">> PFitter::ExecuteFitRange(): " << fCmdLines[lineNo].fLine.Data() << std::endl;

  if (fCmdLines[lineNo].fLine.Contains("fgb", TString::kIgnoreCase)) { // fit range given in bins
    fRunListCollection->SetFitRange(fCmdLines[lineNo].fLine);
    return true;
  }

  TObjArray *tokens = nullptr;
  TObjString *ostr;
  TString str;

  tokens = fCmdLines[lineNo].fLine.Tokenize(", \t");

  PMsrRunList *runList = fRunInfo->GetMsrRunList();

  // execute command, no error checking needed since this has been already carried out in CheckCommands()
  if (tokens->GetEntries() == 2) { // reset command
    fRunListCollection->SetFitRange(fOriginalFitRange);
  } else if (tokens->GetEntries() == 3) { // single fit range for all runs    
    Double_t start = 0.0, end = 0.0;
    PDoublePair fitRange;
    PDoublePairVector fitRangeVector;

    ostr = dynamic_cast<TObjString*>(tokens->At(1));
    str = ostr->GetString();
    start = str.Atof();
    ostr = dynamic_cast<TObjString*>(tokens->At(2));
    str = ostr->GetString();
    end = str.Atof();

    fitRange.first  = start;
    fitRange.second = end;
    fitRangeVector.push_back(fitRange);
    fRunListCollection->SetFitRange(fitRangeVector);

  } else { // individual fit ranges for each run
    Double_t start = 0.0, end = 0.0;
    PDoublePair fitRange;
    PDoublePairVector fitRangeVector;

    for (UInt_t i=0; i<runList->size(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(2*i+1));
      str = ostr->GetString();
      start = str.Atof();
      ostr = dynamic_cast<TObjString*>(tokens->At(2*i+2));
      str = ostr->GetString();
      end = str.Atof();

      fitRange.first  = start;
      fitRange.second = end;
      fitRangeVector.push_back(fitRange);
    }

    fRunListCollection->SetFitRange(fitRangeVector);
  }

  return true;
}

//--------------------------------------------------------------------------
// ExecuteFix
//--------------------------------------------------------------------------
/**
 * <p>Fix parameter list given at lineNo of the command block.
 *
 * \param lineNo the line number of the command block
 *
 * <b>return:</b> true if done, otherwise returns false.
 */
Bool_t PFitter::ExecuteFix(UInt_t lineNo)
{
  std::cout << ">> PFitter::ExecuteFix(): " << fCmdLines[lineNo].fLine.Data() << std::endl;

  TObjArray *tokens = nullptr;
  TObjString *ostr;
  TString str;

  tokens = fCmdLines[lineNo].fLine.Tokenize(", \t");

  for (Int_t i=1; i<tokens->GetEntries(); i++) {
    ostr = dynamic_cast<TObjString*>(tokens->At(i));
    str = ostr->GetString();

    if (str.IsDigit()) { // token is a parameter number
      fMnUserParams.Fix(static_cast<UInt_t>(str.Atoi())-1);
    } else { // token is a parameter name
      fMnUserParams.Fix(str.Data());
    }
  }

  // clean up
  if (tokens) {
    delete tokens;
    tokens = nullptr;
  }

  return true;
}

//--------------------------------------------------------------------------
// ExecuteHesse
//--------------------------------------------------------------------------
/**
 * <p>Execute the minuit2 hesse command.
 *
 * <b>return:</b> true if the hesse command could be executed successfully, otherwise returns false.
 */
Bool_t PFitter::ExecuteHesse()
{
  std::cout << ">> PFitter::ExecuteHesse(): will call hesse ..." << std::endl;

  // create the hesse object
  ROOT::Minuit2::MnHesse hesse;

  // specify maximal number of function calls
  UInt_t maxfcn = std::numeric_limits<UInt_t>::max();

  // call hesse
  Double_t start=0.0, end=0.0;
  start=MilliTime();
  ROOT::Minuit2::MnUserParameterState mnState = hesse((*fFitterFcn), fMnUserParams, maxfcn);
  end=MilliTime();
  std::cout << ">> PFitter::ExecuteMinimize(): execution time for Hesse = " << std::setprecision(3) << (end-start)/1.0e3 << " sec." << std::endl;
  TString str = TString::Format("Hesse:    %.3f sec", (end-start)/1.0e3);
  fElapsedTime.push_back(str);
  if (!mnState.IsValid()) {
    std::cerr  << std::endl << ">> PFitter::ExecuteHesse(): **WARNING** Hesse encountered a problem! The state found is invalid.";
    std::cerr  << std::endl;
    return false;
  }
  if (!mnState.HasCovariance()) {
    std::cerr  << std::endl << ">> PFitter::ExecuteHesse(): **WARNING** Hesse encountered a problem! No covariance matrix available.";
    std::cerr  << std::endl;
    return false;
  }

  // fill parabolic errors
  for (UInt_t i=0; i<fParams.size(); i++) {
    fRunInfo->SetMsrParamStep(i, mnState.Error(i));
    fRunInfo->SetMsrParamPosErrorPresent(i, false);
  }

  if (fPrintLevel >= 2)
    std::cout << mnState << std::endl;

  return true;
}

//--------------------------------------------------------------------------
// ExecuteMigrad
//--------------------------------------------------------------------------
/**
 * <p>Execute the minuit2 migrad command.
 *
 * <b>return:</b> true if the migrad command could be executed successfully, otherwise returns false.
 */
Bool_t PFitter::ExecuteMigrad()
{
  std::cout << ">> PFitter::ExecuteMigrad(): will call migrad ..." << std::endl;

  // create migrad object
  // strategy is by default = 'default'
  ROOT::Minuit2::MnMigrad migrad((*fFitterFcn), fMnUserParams, fStrategy);

  // minimize
  // maxfcn is MINUIT2 Default maxfcn
  UInt_t maxfcn = std::numeric_limits<UInt_t>::max();
  // tolerance = MINUIT2 Default tolerance
  Double_t tolerance = 0.1;
  // keep track of elapsed time
  Double_t start=0.0, end=0.0;
  start=MilliTime();
  ROOT::Minuit2::FunctionMinimum min = migrad(maxfcn, tolerance);
  end=MilliTime();
  std::cout << ">> PFitter::ExecuteMinimize(): execution time for Migrad = " << std::setprecision(3) << (end-start)/1.0e3 << " sec." << std::endl;
  TString str = TString::Format("Migrad:   %.3f sec", (end-start)/1.0e3);
  fElapsedTime.push_back(str);
  if (!min.IsValid()) {
    std::cerr  << std::endl << ">> PFitter::ExecuteMigrad(): **WARNING**: Fit did not converge, sorry ...";
    std::cerr  << std::endl;
    fIsValid = false;
    return false;
  }

  // keep FunctionMinimum object
  if (fFcnMin) { // fFcnMin exist hence clean up first
    delete fFcnMin;
    fFcnMin = nullptr;
  }
  fFcnMin = new ROOT::Minuit2::FunctionMinimum(min);

  // keep user parameters
  if (fFcnMin)
    fMnUserParams = fFcnMin->UserParameters();

  // fill run info
  for (UInt_t i=0; i<fParams.size(); i++) {
    fRunInfo->SetMsrParamValue(i, min.UserState().Value(i));
    fRunInfo->SetMsrParamStep(i, min.UserState().Error(i));
    fRunInfo->SetMsrParamPosErrorPresent(i, false);
  }

  // handle statistics
  Double_t minVal = min.Fval();
  UInt_t ndf = fFitterFcn->GetTotalNoOfFittedBins();
  // subtract number of varied parameters from total no of fitted bins -> ndf
  for (UInt_t i=0; i<fParams.size(); i++) {
    if ((min.UserState().Error(i) != 0.0) && !fMnUserParams.Parameters().at(i).IsFixed())
      ndf -= 1;
  }

  // feed run info with new statistics info
  fRunInfo->SetMsrStatisticMin(minVal);
  fRunInfo->SetMsrStatisticNdf(ndf);

  fConverged = true;

  if (fPrintLevel >= 2)
    std::cout << *fFcnMin << std::endl;

  return true;
}

//--------------------------------------------------------------------------
// ExecuteMinimize
//--------------------------------------------------------------------------
/**
 * <p>Execute the minuit2 minimize command.
 *
 * <b>return:</b> true if the minimize command could be executed successfully, otherwise returns false.
 */
Bool_t PFitter::ExecuteMinimize()
{
  std::cout << ">> PFitter::ExecuteMinimize(): will call minimize ..." << std::endl;

  // create minimizer object
  // strategy is by default = 'default'
  ROOT::Minuit2::MnMinimize minimize((*fFitterFcn), fMnUserParams, fStrategy);

  // minimize
  // maxfcn is MINUIT2 Default maxfcn
  UInt_t maxfcn = std::numeric_limits<UInt_t>::max();

  // tolerance = MINUIT2 Default tolerance
  Double_t tolerance = 0.1;
  // keep track of elapsed time
  Double_t start=0.0, end=0.0;
  start = MilliTime();
  ROOT::Minuit2::FunctionMinimum min = minimize(maxfcn, tolerance);
  end = MilliTime();
  std::cout << ">> PFitter::ExecuteMinimize(): execution time for Minimize = " << std::setprecision(3) << (end-start)/1.0e3 << " sec." << std::endl;
  TString str = TString::Format("Minimize: %.3f sec", (end-start)/1.0e3);
  fElapsedTime.push_back(str);
  if (!min.IsValid()) {
    std::cerr  << std::endl << ">> PFitter::ExecuteMinimize(): **WARNING**: Fit did not converge, sorry ...";
    std::cerr  << std::endl;
    fIsValid = false;
    return false;
  }

  // keep FunctionMinimum object
  if (fFcnMin) { // fFcnMin exist hence clean up first
    delete fFcnMin;
    fFcnMin = nullptr;
  }
  fFcnMin = new ROOT::Minuit2::FunctionMinimum(min);

  // keep user parameters
  if (fFcnMin)
    fMnUserParams = fFcnMin->UserParameters();

  // fill run info
  for (UInt_t i=0; i<fParams.size(); i++) {
    fRunInfo->SetMsrParamValue(i, min.UserState().Value(i));
    fRunInfo->SetMsrParamStep(i, min.UserState().Error(i));
    fRunInfo->SetMsrParamPosErrorPresent(i, false);
  }

  // handle statistics
  Double_t minVal = min.Fval();
  UInt_t ndf = fFitterFcn->GetTotalNoOfFittedBins();
  // subtract number of varied parameters from total no of fitted bins -> ndf
  for (UInt_t i=0; i<fParams.size(); i++) {
    if ((min.UserState().Error(i) != 0.0) && !fMnUserParams.Parameters().at(i).IsFixed())
      ndf -= 1;
  }

  // feed run info with new statistics info
  fRunInfo->SetMsrStatisticMin(minVal);
  fRunInfo->SetMsrStatisticNdf(ndf);

  fConverged = true;

  if (fPrintLevel >= 2)
    std::cout << *fFcnMin << std::endl;

  return true;
}

//--------------------------------------------------------------------------
// ExecuteMinos
//--------------------------------------------------------------------------
/**
 * <p>Execute the minuit2 minos command.
 *
 * <b>return:</b> true if the minos command could be executed successfully, otherwise returns false.
 */
Bool_t PFitter::ExecuteMinos()
{
  std::cout << ">> PFitter::ExecuteMinos(): will call minos ..." << std::endl;

  // if already some minimization is done use the minuit2 output as input
  if (!fFcnMin) {
    std::cerr  << std::endl << "**ERROR**: MINOS musn't be called before any minimization (MINIMIZE/MIGRAD/SIMPLEX) is done!!";
    std::cerr  << std::endl;
    return false;
  }

  // check if minimum was valid
  if (!fFcnMin->IsValid()) {
    std::cerr  << std::endl << "**ERROR**: MINOS cannot started since the previous minimization failed :-(";
    std::cerr  << std::endl;
    return false;
  }

  // make minos analysis
  Double_t start=0.0, end=0.0;
  start=MilliTime();
  ROOT::Minuit2::MnMinos minos((*fFitterFcn), (*fFcnMin));

  for (UInt_t i=0; i<fParams.size(); i++) {
    // only try to call minos if the parameter is not fixed!!
    // the 1st condition is from an user fixed variable,
    // the 2nd condition is from an all together unused variable
    // the 3rd condition is a variable fixed via the FIX command
    if ((fMnUserParams.Error(i) != 0.0) && (fRunInfo->ParameterInUse(i) != 0) && (!fMnUserParams.Parameters().at(i).IsFixed())) {
      std::cout << ">> PFitter::ExecuteMinos(): calculate errors for " << fParams[i].fName << std::endl;

      // 1-sigma MINOS errors
      ROOT::Minuit2::MinosError err = minos.Minos(i);

      if (err.IsValid()) {
        // fill msr-file structure
        fRunInfo->SetMsrParamStep(i, err.Lower());
        fRunInfo->SetMsrParamPosError(i, err.Upper());
        fRunInfo->SetMsrParamPosErrorPresent(i, true);
      } else {
        fRunInfo->SetMsrParamPosErrorPresent(i, false);
      }
    }

    if (fMnUserParams.Parameters().at(i).IsFixed()) {
      std::cerr  << std::endl << ">> PFitter::ExecuteMinos(): **WARNING** Parameter " << fMnUserParams.Name(i) << " (ParamNo " << i+1 << ") is fixed!";
      std::cerr  << std::endl << ">>    Will set STEP to zero, i.e. making it a constant parameter";
      std::cerr  << std::endl;
      fRunInfo->SetMsrParamStep(i, 0.0);
      fRunInfo->SetMsrParamPosErrorPresent(i, false);
    }
  }

  end=MilliTime();
  std::cout << ">> PFitter::ExecuteMinimize(): execution time for Minos = " << std::setprecision(3) << (end-start)/1.0e3 << " sec." << std::endl;
  TString str = TString::Format("Minos:    %.3f sec", (end-start)/1.0e3);
  fElapsedTime.push_back(str);

  return true;
}

//--------------------------------------------------------------------------
// ExecutePlot
//--------------------------------------------------------------------------
/**
 * <p>Execute the minuit2 plot command.
 *
 * <b>return:</b> true.
 */
Bool_t PFitter::ExecutePlot()
{
  std::cout << ">> PFitter::ExecutePlot() ..." << std::endl;

  ROOT::Minuit2::MnPlot plot;
  plot(fScanData);

  return true;
}

//--------------------------------------------------------------------------
// ExecutePrintLevel
//--------------------------------------------------------------------------
/**
 * <p>Set the print level.
 *
 * \param lineNo the line number of the command block
 *
 * <b>return:</b> true if done, otherwise returns false.
 */
Bool_t PFitter::ExecutePrintLevel(UInt_t lineNo)
{
  std::cout << ">> PFitter::ExecutePrintLevel(): " << fCmdLines[lineNo].fLine.Data() << std::endl;

  TObjArray *tokens = nullptr;
  TObjString *ostr;
  TString str;

  tokens = fCmdLines[lineNo].fLine.Tokenize(", \t");

  if (tokens->GetEntries() < 2) {
    std::cerr  << std::endl << "**ERROR** from PFitter::ExecutePrintLevel(): SYNTAX: PRINT_LEVEL <N>, where <N>=0-3" << std::endl << std::endl;
    return false;
  }

  ostr = (TObjString*)tokens->At(1);
  str = ostr->GetString();

  Int_t ival;
  if (str.IsDigit()) {
    ival = str.Atoi();
    if ((ival >=0) && (ival <= 3)) {
      fPrintLevel = (UInt_t) ival;
    } else {
      std::cerr  << std::endl << "**ERROR** from PFitter::ExecutePrintLevel(): SYNTAX: PRINT_LEVEL <N>, where <N>=0-3";
      std::cerr  << std::endl << "   found <N>=" << ival << std::endl << std::endl;
      return false;
    }
  } else {
    std::cerr  << std::endl << "**ERROR** from PFitter::ExecutePrintLevel(): SYNTAX: PRINT_LEVEL <N>, where <N>=0-3" << std::endl << std::endl;
    return false;
  }

  ROOT::Minuit2::MnPrint::SetLevel(fPrintLevel);

  // clean up
  if (tokens) {
    delete tokens;
    tokens = nullptr;
  }

  return true;
}

//--------------------------------------------------------------------------
// ExecuteRelease
//--------------------------------------------------------------------------
/**
 * <p>Release parameter list given at lineNo of the command block.
 *
 * \param lineNo the line number of the command block
 *
 * <b>return:</b> true if done, otherwise returns false.
 */
Bool_t PFitter::ExecuteRelease(UInt_t lineNo)
{
  TObjArray *tokens = nullptr;
  TObjString *ostr;
  TString str;

  tokens = fCmdLines[lineNo].fLine.Tokenize(", \t");

  std::cout << ">> PFitter::ExecuteRelease(): " << fCmdLines[lineNo].fLine.Data() << std::endl;

  for (Int_t i=1; i<tokens->GetEntries(); i++) {
    ostr = dynamic_cast<TObjString*>(tokens->At(i));
    str = ostr->GetString();

    if (str.IsDigit()) { // token is a parameter number
      fMnUserParams.Release(static_cast<UInt_t>(str.Atoi())-1);
      // set the error to 2% of the value when releasing
      fMnUserParams.SetError(static_cast<UInt_t>(str.Atoi())-1, 0.02*fMnUserParams.Value(static_cast<UInt_t>(str.Atoi())-1));
    } else { // token is a parameter name
      fMnUserParams.Release(str.Data());
      // set the error to 2% of the value when releasing
      fMnUserParams.SetError(str.Data(), 0.02*fMnUserParams.Value(str.Data()));
    }
  }

  // clean up
  if (tokens) {
    delete tokens;
    tokens = nullptr;
  }

  return true;
}

//--------------------------------------------------------------------------
// ExecuteRestore
//--------------------------------------------------------------------------
/**
 * <p>Release all fixed parameters
 *
 * <b>return:</b> true.
 */
Bool_t PFitter::ExecuteRestore()
{
  std::cout << "PFitter::ExecuteRestore(): release all fixed parameters (RESTORE) ..." << std::endl;

  for (UInt_t i=0; i<fMnUserParams.Parameters().size(); i++) {
    if (fMnUserParams.Parameters().at(i).IsFixed()) {
      fMnUserParams.Release(i);
      fMnUserParams.SetError(i, 0.02*fMnUserParams.Value(i));
    }
  }

  return true;
}

//--------------------------------------------------------------------------
// ExecuteScan
//--------------------------------------------------------------------------
/**
 * <p>Execute the minuit2 scan command.
 *
 * <b>return:</b> true.
 */
Bool_t PFitter::ExecuteScan()
{
  std::cout << ">> PFitter::ExecuteScan(): will call scan ..." << std::endl;

  ROOT::Minuit2::MnScan scan((*fFitterFcn), fMnUserParams);

  if (fScanAll) { // not clear at the moment what to be done here
    // TO BE IMPLEMENTED
  } else { // single parameter scan
    fScanData = scan.Scan(fScanParameter[0], fScanNoPoints, fScanLow, fScanHigh);
  }

  fConverged = true;

  return true;
}

//--------------------------------------------------------------------------
// ExecuteSave
//--------------------------------------------------------------------------
/**
 * <p>Execute the save command.
 *
 * \param firstSave flag indication if this is the first save call and hence write a fresh MINUIT2.OUTPUT
 *
 * <b>return:</b> true if the valid minuit2 state is found, otherwise returns false.
 */
Bool_t PFitter::ExecuteSave(Bool_t firstSave)
{
  // if any minimization was done, otherwise get out immediately
  if (!fFcnMin) {
    std::cout << std::endl << ">> PFitter::ExecuteSave(): nothing to be saved ...";
    return false;
  }

  ROOT::Minuit2::MnUserParameterState mnState = fFcnMin->UserState();

  // check if the user parameter state is valid
  if (!mnState.IsValid()) {
    std::cerr  << std::endl << ">> PFitter::ExecuteSave: **WARNING** Minuit2 User Parameter State is not valid, i.e. nothing to be saved";
    std::cerr  << std::endl;
    return false;
  }

  // handle expected chisq if applicable
  fParams = *(fRunInfo->GetMsrParamList()); // get the update parameters back

  // calculate expected chisq
  std::vector<Double_t> param;
  Double_t totalExpectedChisq = 0.0;
  std::vector<Double_t> expectedchisqPerRun;
  std::vector<UInt_t> ndfPerHisto;

  for (UInt_t i=0; i<fParams.size(); i++)
    param.push_back(fParams[i].fValue);

  // CalcExpectedChiSquare handles both, chisq and mlh
  fFitterFcn->CalcExpectedChiSquare(param, totalExpectedChisq, expectedchisqPerRun);

  // calculate chisq per run
  std::vector<Double_t> chisqPerRun;
  for (UInt_t i=0; i<fRunInfo->GetMsrRunList()->size(); i++) {
    if (fUseChi2)
      chisqPerRun.push_back(fRunListCollection->GetSingleRunChisq(param, i));
    else
      chisqPerRun.push_back(fRunListCollection->GetSingleRunMaximumLikelihood(param, i));
  }

  if (totalExpectedChisq != 0.0) { // i.e. applicable for single histogram fits only
    // get the ndf's of the histos
    UInt_t ndf_run;
    for (UInt_t i=0; i<expectedchisqPerRun.size(); i++) {
      ndf_run = fFitterFcn->GetNoOfFittedBins(i) - fRunInfo->GetNoOfFitParameters(i);
      ndfPerHisto.push_back(ndf_run);
    }

    // feed the msr-file handler
    PMsrStatisticStructure *statistics = fRunInfo->GetMsrStatistic();
    if (statistics) {
      statistics->fMinPerHisto = chisqPerRun;
      statistics->fMinExpected = totalExpectedChisq;
      statistics->fMinExpectedPerHisto = expectedchisqPerRun;
      statistics->fNdfPerHisto = ndfPerHisto;
    }
  } else if (chisqPerRun.size() > 1) { // in case expected chisq is not applicable like for asymmetry fits
    UInt_t ndf_run = 0;
    for (UInt_t i=0; i<chisqPerRun.size(); i++) {
      ndf_run = fFitterFcn->GetNoOfFittedBins(i) - fRunInfo->GetNoOfFitParameters(i);
      ndfPerHisto.push_back(ndf_run);
    }

    // feed the msr-file handler
    PMsrStatisticStructure *statistics = fRunInfo->GetMsrStatistic();
    if (statistics) {
      statistics->fMinPerHisto = chisqPerRun;
      statistics->fNdfPerHisto = ndfPerHisto;
    }
  }

  // check if sector command has been requested
  if (fSectorFlag) {
    PDoubleVector error;
    for (UInt_t i=0; i<fParams.size(); i++)
      error.push_back(fParams[i].fStep);

    PrepareSector(param, error);
  }

  // clean up
  param.clear();
  expectedchisqPerRun.clear();
  ndfPerHisto.clear();
  chisqPerRun.clear();

  std::cout << ">> PFitter::ExecuteSave(): will write minuit2 output file ..." << std::endl;

  std::ofstream fout;

  // open minuit2 output file
  if (firstSave)
    fout.open("MINUIT2.OUTPUT", std::iostream::out);
  else
    fout.open("MINUIT2.OUTPUT", std::iostream::out | std::iostream::app);

  if (!fout.is_open()) {
    std::cerr  << std::endl << "**ERROR** PFitter::ExecuteSave() couldn't open MINUIT2.OUTPUT file";
    std::cerr  << std::endl;
    return false;
  }

  // write header
  TDatime dt;
  fout << std::endl << "*************************************************************************";
  fout << std::endl << " musrfit MINUIT2 output file from " << fRunInfo->GetFileName().Data() << " - " << dt.AsSQLString();
  fout << std::endl << "*************************************************************************";
  fout << std::endl;

  // write elapsed times
  fout << std::endl << " elapsed times:";
  for (UInt_t i=0; i<fElapsedTime.size(); i++) {
    fout << std::endl << "   " << fElapsedTime[i];
  }
  fout << std::endl;
  fout << std::endl << "*************************************************************************";
  fout << std::endl;
  fElapsedTime.clear();

  // write global information
  fout << std::endl << " Fval() = " << mnState.Fval() << ", Edm() = " << mnState.Edm() << ", NFcn() = " << mnState.NFcn();
  fout << std::endl;
  fout << std::endl << "*************************************************************************";
  fout << std::endl;

  // identifiy the longest variable name for proper formating reasons
  Int_t maxLength = 10;
  for (UInt_t i=0; i<fParams.size(); i++) {
    if (fParams[i].fName.Length() > maxLength)
      maxLength = fParams[i].fName.Length() + 1;
  }

  // write parameters
  fParams = *(fRunInfo->GetMsrParamList()); // get the update parameters back
  fout << std::endl << " PARAMETERS";
  fout << std::endl << "-------------------------------------------------------------------------";
  fout << std::endl << "                         ";
  for (Int_t j=0; j<=maxLength-4; j++)
    fout << " ";
  fout << "Parabolic           Minos";
  fout << std::endl << " No   Name";
  for (Int_t j=0; j<=maxLength-4; j++)
    fout << " ";
  fout << "Value      Error      Negative    Positive    Limits";
  for (UInt_t i=0; i<fParams.size(); i++) {
    // write no
    fout.setf(std::ios::right, std::ios::adjustfield);
    fout.width(3);
    fout << std::endl << i+1 << "   ";
    // write name
    fout << fParams[i].fName.Data();
    for (Int_t j=0; j<=maxLength-fParams[i].fName.Length(); j++)
      fout << " ";
    // write value
    fout.setf(std::ios::left, std::ios::adjustfield);
    fout.precision(6);
    fout.width(10);
    fout << fParams[i].fValue << " ";
    // write parabolic error
    fout.setf(std::ios::left, std::ios::adjustfield);
    fout.precision(6);
    fout.width(10);
    if (fParams[i].fStep != 0.0)
      fout << fMnUserParams.Error(i) << " ";
    else
      fout << "---";
    // write minos errors
    if (fParams[i].fPosErrorPresent) {
      fout.setf(std::ios::left, std::ios::adjustfield);
      fout.precision(6);
      fout.width(12);
      fout << fParams[i].fStep; 
      fout.setf(std::ios::left, std::ios::adjustfield);
      fout.precision(6);
      fout.width(11);
      fout << fParams[i].fPosError << " ";
    } else {
      fout.setf(std::ios::left, std::ios::adjustfield);
      fout.width(12);
      fout << "---";
      fout.setf(std::ios::left, std::ios::adjustfield);
      fout.width(12);
      fout << "---";
    }
    // write limits
    if (fParams[i].fNoOfParams > 5) {
      fout.setf(std::ios::left, std::ios::adjustfield);
      fout.width(7);
      if (fParams[i].fLowerBoundaryPresent)
        fout << fParams[i].fLowerBoundary;
      else
        fout << "---";
      fout.setf(std::ios::left, std::ios::adjustfield);
      fout.width(7);
      if (fParams[i].fUpperBoundaryPresent)
        fout << fParams[i].fUpperBoundary;
      else
        fout << "---";
    } else {
      fout.setf(std::ios::left, std::ios::adjustfield);
      fout.width(7);
      fout << "---";
      fout.setf(std::ios::left, std::ios::adjustfield);
      fout.width(7);
      fout << "---";
    }
  }
  fout << std::endl;
  fout << std::endl << "*************************************************************************";

  // write covariance matrix
  fout << std::endl << " COVARIANCE MATRIX";
  fout << std::endl << "-------------------------------------------------------------------------";
  if (mnState.HasCovariance()) {
    ROOT::Minuit2::MnUserCovariance cov = mnState.Covariance();
    fout << std::endl << "from " << cov.Nrow() << " free parameters";
    for (UInt_t i=0; i<cov.Nrow(); i++) {
      fout << std::endl;
      for (UInt_t j=0; j<i; j++) {
        fout.setf(std::ios::left, std::ios::adjustfield);
        fout.precision(6);
        if (cov(i,j) > 0.0) {
          fout << " ";
          fout.width(13);
        } else {
          fout.width(14);
        }
        fout << cov(i,j);
      }
    }
  } else {
    fout << std::endl << " no covariance matrix available";
  }
  fout << std::endl;
  fout << std::endl << "*************************************************************************";

  // write correlation matrix
  fout << std::endl << " CORRELATION COEFFICIENTS";
  fout << std::endl << "-------------------------------------------------------------------------";
  if (mnState.HasGlobalCC() && mnState.HasCovariance()) {
    ROOT::Minuit2::MnGlobalCorrelationCoeff corr = mnState.GlobalCC();
    ROOT::Minuit2::MnUserCovariance cov = mnState.Covariance();
    PIntVector parNo;
    fout << std::endl << " No   Global       ";
    for (UInt_t i=0; i<fParams.size(); i++) {
      // only free parameters, i.e. not fixed, and not unsed ones!
      if ((fParams[i].fStep != 0) && (fRunInfo->ParameterInUse(i) > 0) && (!fMnUserParams.Parameters().at(i).IsFixed())) {
        fout.setf(std::ios::left, std::ios::adjustfield);
        fout.width(9);
        fout << i+1;
        parNo.push_back(i);
      }
    }
    // check that there is a correspondens between minuit2 and musrfit book keeping
    if (parNo.size() != cov.Nrow()) {
      std::cerr  << std::endl << "**SEVERE ERROR** in PFitter::ExecuteSave(): minuit2 and musrfit book keeping to not correspond! Unable to write correlation matrix.";
      std::cerr  << std::endl;
    } else { // book keeping is OK
      TString title("Minuit2 Output Correlation Matrix for ");
      title += fRunInfo->GetFileName();
      title += " - ";
      title += dt.AsSQLString();
      TCanvas *ccorr = new TCanvas("ccorr", "title", 500, 500);
      TH2D *hcorr = new TH2D("hcorr", title, cov.Nrow(), 0.0, cov.Nrow(), cov.Nrow(), 0.0, cov.Nrow());
      Double_t dval;
      for (UInt_t i=0; i<cov.Nrow(); i++) {
        // parameter number
        fout << std::endl << " ";
        fout.setf(std::ios::left, std::ios::adjustfield);
        fout.width(5);
        fout << parNo[i]+1;
        // global correlation coefficient
        fout.setf(std::ios::left, std::ios::adjustfield);
        fout.precision(6);
        fout.width(12);
        fout << corr.GlobalCC()[i];
        // correlations matrix
        for (UInt_t j=0; j<cov.Nrow(); j++) {
          fout.setf(std::ios::left, std::ios::adjustfield);
//          fout.precision(4);
          if (i==j) {
            fout.width(9);
            fout << " 1.0 ";
            hcorr->Fill((Double_t)i,(Double_t)i,1.0);
          } else {
            // check that errors are none zero
            if (fMnUserParams.Error(parNo[i]) == 0.0) {
              std::cerr  << std::endl << "**SEVERE ERROR** in PFitter::ExecuteSave(): parameter no " << parNo[i]+1 << " has an error == 0. Cannot correctly handle the correlation matrix.";
              std::cerr  << std::endl;
              dval = 0.0;
            } else if (fMnUserParams.Error(parNo[j]) == 0.0) {
              std::cerr  << std::endl << "**SEVERE ERROR** in PFitter::ExecuteSave(): parameter no " << parNo[j]+1 << " has an error == 0. Cannot correctly handle the correlation matrix.";
              std::cerr  << std::endl;
              dval = 0.0;
            } else {
              dval = cov(i,j)/(fMnUserParams.Error(parNo[i])*fMnUserParams.Error(parNo[j]));
            }
            hcorr->Fill((Double_t)i,(Double_t)j,dval);
            // handle precision, ugly but ...
            if (dval < 1.0e-2) {
              fout.precision(2);
            } else {
              fout.precision(4);
            }
            // handle sign
            if (dval > 0.0) {
              fout << " ";
              fout.width(7);
            } else {
              fout.width(8);
            }
            fout << dval << " ";
          }
        }
      }
      // write correlation matrix into a root file
      TFile ff("MINUIT2.root", "recreate");
      ccorr->Draw();
      if (cov.Nrow() <= 6)
        hcorr->Draw("COLZTEXT");
      else
        hcorr->Draw("COLZ");
      ccorr->Write("ccorr", TObject::kOverwrite, sizeof(ccorr));
      hcorr->Write("hcorr", TObject::kOverwrite, sizeof(hcorr));
      ff.Close();
      // clean up
      if (ccorr) {
        delete ccorr;
        ccorr = nullptr;
      }
      if (hcorr) {
        delete hcorr;
        hcorr = nullptr;
      }
    }
    parNo.clear(); // clean up
  } else {
    fout << std::endl << " no correlation coefficients available";
  }

  fout << std::endl;
  fout << std::endl << "*************************************************************************";
  fout << std::endl << " chisq/maxLH RESULT ";
  fout << std::endl << "*************************************************************************";
  PMsrStatisticStructure *statistics = fRunInfo->GetMsrStatistic();

  // get time range and write it
  Double_t fitStartTime = PMUSR_UNDEFINED, fitEndTime = PMUSR_UNDEFINED;
  // first check if there is a global block with a valid time range
  PMsrGlobalBlock *global = fRunInfo->GetMsrGlobal();
  fitStartTime = global->GetFitRange(0);
  fitEndTime = global->GetFitRange(1);
  if (fitStartTime == PMUSR_UNDEFINED) { // no global time range, hence take the one from the first run block
    PMsrRunList *run = fRunInfo->GetMsrRunList();
    fitStartTime = run->at(0).GetFitRange(0);
    fitEndTime = run->at(0).GetFitRange(1);
  }
  fout.setf(std::ios::floatfield);
  fout << std::endl << " Time Range: " << fitStartTime << ", " << fitEndTime << std::endl;
  if (fUseChi2) {
    fout.setf(std::ios::fixed, std::ios::floatfield);
    fout << std::endl << " chisq   = " << std::setprecision(4) << statistics->fMin << ", NDF = " << statistics->fNdf << ", chisq/NDF   = " << std::setprecision(6) << statistics->fMin/statistics->fNdf;
    if (statistics->fMinExpected > 0)
      fout << std::endl << " chisq_e = " << std::setprecision(4) << statistics->fMinExpected << ", NDF = " << statistics->fNdf << ", chisq_e/NDF = " << std::setprecision(6) << statistics->fMinExpected/statistics->fNdf;
  } else { // maxLH
    fout.setf(std::ios::fixed, std::ios::floatfield);
    fout << std::endl << " maxLH   = " << std::setprecision(4) << statistics->fMin << ", NDF = " << statistics->fNdf << ", maxLH/NDF   = " << std::setprecision(6) << statistics->fMin/statistics->fNdf;
    if (statistics->fMinExpected > 0)
      fout << std::endl << " maxLH_e = " << std::setprecision(4) << statistics->fMinExpected << ", NDF = " << statistics->fNdf << ", maxLH_e/NDF = " << std::setprecision(6) << statistics->fMinExpected/statistics->fNdf;
  }

  if (fSectorFlag)
    ExecuteSector(fout);

  fout << std::endl;
  fout << std::endl << "*************************************************************************";
  fout << std::endl << " DONE ";
  fout << std::endl << "*************************************************************************";
  fout << std::endl << std::endl;

  // close MINUIT2.OUTPUT file
  fout.close();

  return true;
}

//--------------------------------------------------------------------------
// ExecuteSimplex
//--------------------------------------------------------------------------
/**
 * <p>Execute the minuit2 simplex command.
 *
 * <b>return:</b> true if the simplex command could be executed successfully, otherwise returns false.
 */
Bool_t PFitter::ExecuteSimplex()
{
  std::cout << ">> PFitter::ExecuteSimplex(): will call simplex ..." << std::endl;

  // create minimizer object
  // strategy is by default = 'default'
  ROOT::Minuit2::MnSimplex simplex((*fFitterFcn), fMnUserParams, fStrategy);

  // minimize
  // maxfcn is 10*MINUIT2 Default maxfcn
  UInt_t maxfcn = std::numeric_limits<UInt_t>::max();
  // tolerance = MINUIT2 Default tolerance
  Double_t tolerance = 0.1;
  // keep track of elapsed time
  Double_t start=0.0, end=0.0;
  start=MilliTime();
  ROOT::Minuit2::FunctionMinimum min = simplex(maxfcn, tolerance);
  end=MilliTime();
  std::cout << ">> PFitter::ExecuteMinimize(): execution time for Simplex = " << std::setprecision(3) << (end-start)/1.0e3 << " sec." << std::endl;
  TString str = TString::Format("Simplex:  %.3f sec", (end-start)/1.0e3);
  fElapsedTime.push_back(str);
  if (!min.IsValid()) {
    std::cerr  << std::endl << ">> PFitter::ExecuteSimplex(): **WARNING**: Fit did not converge, sorry ...";
    std::cerr  << std::endl;
    fIsValid = false;
    return false;
  }

  // keep FunctionMinimum object
  if (fFcnMin) { // fFcnMin exist hence clean up first
    delete fFcnMin;
    fFcnMin = nullptr;
  }
  fFcnMin = new ROOT::Minuit2::FunctionMinimum(min);

  // keep user parameters
  if (fFcnMin)
    fMnUserParams = fFcnMin->UserParameters();

  // fill run info
  for (UInt_t i=0; i<fParams.size(); i++) {
    fRunInfo->SetMsrParamValue(i, min.UserState().Value(i));
    fRunInfo->SetMsrParamStep(i, min.UserState().Error(i));
    fRunInfo->SetMsrParamPosErrorPresent(i, false);
  }

  // handle statistics
  Double_t minVal = min.Fval();
  UInt_t ndf = fFitterFcn->GetTotalNoOfFittedBins();
  // subtract number of varied parameters from total no of fitted bins -> ndf
  for (UInt_t i=0; i<fParams.size(); i++) {
    if ((min.UserState().Error(i) != 0.0) && !fMnUserParams.Parameters().at(i).IsFixed())
      ndf -= 1;
  }

  // feed run info with new statistics info
  fRunInfo->SetMsrStatisticMin(minVal);
  fRunInfo->SetMsrStatisticNdf(ndf);

  fConverged = true;

  if (fPrintLevel >= 2)
    std::cout << *fFcnMin << std::endl;

  return true;
}

//--------------------------------------------------------------------------
// PrepareSector
//--------------------------------------------------------------------------
/**
 * <p>Collect all the necessary chisq/maxLH sector information.
 *
 * @param param parameter value vector of the converged fit.
 * @param error step value vector of the converged fit.
 */
void PFitter::PrepareSector(PDoubleVector &param, PDoubleVector &error)
{
  Double_t val;
  UInt_t ndf;

  Int_t usedParams = 0;
  for (UInt_t i=0; i<error.size(); i++) {
    if (error[i] != 0.0)
      usedParams++;
  }

  PDoublePairVector secFitRange;
  secFitRange.resize(1);

  if (fUseChi2) {
    Double_t totalExpectedChisq = 0.0;
    PDoubleVector expectedChisqPerRun;
    PDoubleVector chisqPerRun;
    for (UInt_t k=0; k<fSector.size(); k++) {
      // set sector fit range
      secFitRange[0].first = fSector[k].GetTimeRangeFirst(0);
      secFitRange[0].second = fSector[k].GetTimeRangeLast();
      fRunListCollection->SetFitRange(secFitRange);
      // calculate chisq
      val = (*fFitterFcn)(param);
      fSector[k].SetChisq(val);
      // calculate NDF
      ndf = static_cast<UInt_t>(fFitterFcn->GetTotalNoOfFittedBins()) - usedParams;
      fSector[k].SetNDF(ndf);
      // calculate expected chisq
      totalExpectedChisq = 0.0;
      fFitterFcn->CalcExpectedChiSquare(param, totalExpectedChisq, expectedChisqPerRun);
      fSector[k].SetExpectedChisq(totalExpectedChisq);
      // calculate chisq per run
      for (UInt_t i=0; i<fRunInfo->GetMsrRunList()->size(); i++) {
        chisqPerRun.push_back(fRunListCollection->GetSingleRunChisq(param, i));
        fSector[k].SetChisq(chisqPerRun[i], i);
        fSector[k].SetExpectedChisq(expectedChisqPerRun[i], i);
      }

      if (totalExpectedChisq != 0.0) {
        UInt_t ndf_run = 0;
        for (UInt_t i=0; i<expectedChisqPerRun.size(); i++) {
          ndf_run = fFitterFcn->GetNoOfFittedBins(i) - fRunInfo->GetNoOfFitParameters(i);
          if (ndf_run > 0) {
            fSector[k].SetNDF(ndf_run, i);
          }
        }
      } else if (chisqPerRun.size() > 0) { // in case expected chisq is not applicable like for asymmetry fits
        UInt_t ndf_run = 0;
        for (UInt_t i=0; i<chisqPerRun.size(); i++) {
          ndf_run = fFitterFcn->GetNoOfFittedBins(i) - fRunInfo->GetNoOfFitParameters(i);
          if (ndf_run > 0) {
            fSector[k].SetNDF(ndf_run, i);
          }
        }
      }
      // clean up
      chisqPerRun.clear();
      expectedChisqPerRun.clear();
    }
  } else {
    Double_t totalExpectedMaxLH = 0.0;
    PDoubleVector expectedMaxLHPerRun;
    PDoubleVector maxLHPerRun;
    for (UInt_t k=0; k<fSector.size(); k++) {
      // set sector fit range
      secFitRange[0].first = fSector[k].GetTimeRangeFirst(0);
      secFitRange[0].second = fSector[k].GetTimeRangeLast();
      fRunListCollection->SetFitRange(secFitRange);
      // calculate maxLH
      val = (*fFitterFcn)(param);
      fSector[k].SetChisq(val);
      // calculate NDF
      ndf = static_cast<UInt_t>(fFitterFcn->GetTotalNoOfFittedBins()) - usedParams;
      fSector[k].SetNDF(ndf);
      // calculate expected maxLH
      totalExpectedMaxLH = 0.0;
      fFitterFcn->CalcExpectedChiSquare(param, totalExpectedMaxLH, expectedMaxLHPerRun);
      fSector[k].SetExpectedChisq(totalExpectedMaxLH);
      // calculate maxLH per run
      for (UInt_t i=0; i<fRunInfo->GetMsrRunList()->size(); i++) {
        maxLHPerRun.push_back(fRunListCollection->GetSingleRunMaximumLikelihood(param, i));
        fSector[k].SetChisq(maxLHPerRun[i], i);
        fSector[k].SetExpectedChisq(expectedMaxLHPerRun[i], i);
      }

      if (totalExpectedMaxLH != 0.0) {
        UInt_t ndf_run = 0;
        for (UInt_t i=0; i<expectedMaxLHPerRun.size(); i++) {
          ndf_run = fFitterFcn->GetNoOfFittedBins(i) - fRunInfo->GetNoOfFitParameters(i);
          if (ndf_run > 0) {
            fSector[k].SetNDF(ndf_run, i);
          }
        }
      }

      // clean up
      maxLHPerRun.clear();
      expectedMaxLHPerRun.clear();
    }
  }
}

//--------------------------------------------------------------------------
// ExecuteSector
//--------------------------------------------------------------------------
/**
 * <p>Write all chisq/maxLH sector information to MINUIT.OUTPUT and dump it
 * to stdout.
 *
 * <b>return:</b> if the sector command was successful, otherwise return flase.
 */
Bool_t PFitter::ExecuteSector(std::ofstream &fout)
{
  fout << std::endl;
  fout << std::endl << "*************************************************************************";
  fout << std::endl << " SECTOR RESULTS";
  fout << std::endl << "*************************************************************************";

  if (fUseChi2) {
    for (UInt_t i=0; i<fSector.size(); i++) {
      fout << std::endl;
      fout << " Sector " << i+1 << ": FitRange: " << fSector[i].GetTimeRangeFirst(0) << ", " << fSector[i].GetTimeRangeLast() << std::endl;
      fout << " chisq = " << fSector[i].GetChisq() << ", NDF = " << fSector[i].GetNDF() << ", chisq/NDF = " << fSector[i].GetChisq()/fSector[i].GetNDF();

      if (fSector[i].GetExpectedChisq() > 0) {
        fout << std::endl << " expected chisq = " << fSector[i].GetExpectedChisq() << ", NDF = " << fSector[i].GetNDF() << ", expected chisq/NDF = " << fSector[i].GetExpectedChisq()/fSector[i].GetNDF();
        for (UInt_t j=0; j<fSector[i].GetNoRuns(); j++) {
          fout << std::endl << " run block " << j+1 << " (NDF/red.chisq/red.chisq_e) = (" << fSector[i].GetNDF(j) << "/" << fSector[i].GetChisq(j)/fSector[i].GetNDF(j) << "/" << fSector[i].GetExpectedChisq(j)/fSector[i].GetNDF(j) << ")";
        }
      } else if (fSector[i].GetNoRuns() > 0) { // in case expected chisq is not applicable like for asymmetry fits
        for (UInt_t j=0; j<fSector[i].GetNoRuns(); j++) {
          fout << std::endl << " run block " << j+1 << " (NDF/red.chisq) = (" << fSector[i].GetNDF(j) << "/" << fSector[i].GetChisq(j)/fSector[i].GetNDF(j) << ")";
        }
      }
      fout << std::endl << "++++";
    }
  } else { // max log likelihood
    for (UInt_t i=0; i<fSector.size(); i++) {
      fout << std::endl;
      fout << " Sector " << i+1 << ": FitRange: " << fSector[i].GetTimeRangeFirst(0) << ", " << fSector[i].GetTimeRangeLast() << std::endl;
      fout << " maxLH = " << fSector[i].GetChisq() << ", NDF = " << fSector[i].GetNDF() << ", maxLH/NDF = " << fSector[i].GetChisq()/fSector[i].GetNDF();

      if (fSector[i].GetExpectedChisq() > 0) {
        fout << std::endl << " expected maxLH = " << fSector[i].GetExpectedChisq() << ", NDF = " << fSector[i].GetNDF() << ", expected maxLH/NDF = " << fSector[i].GetExpectedChisq()/fSector[i].GetNDF();
        for (UInt_t j=0; j<fSector[i].GetNoRuns(); j++) {
          fout << std::endl << " run block " << j+1 << " (NDF/red.maxLH/red.maxLH_e) = (" << fSector[i].GetNDF(j) << "/" << fSector[i].GetChisq(j)/fSector[i].GetNDF(j) << "/" << fSector[i].GetExpectedChisq(j)/fSector[i].GetNDF(j) << ")";
        }
      } else if (fSector[i].GetNoRuns() > 0) { // in case expected chisq is not applicable like for asymmetry fits
        for (UInt_t j=0; j<fSector[i].GetNoRuns(); j++) {
          fout << std::endl << " run block " << j+1 << " (NDF/red.maxLH) = (" << fSector[i].GetNDF(j) << "/" << fSector[i].GetChisq(j)/fSector[i].GetNDF(j) << ")";
        }
      }
      fout << std::endl << "++++";
    }
  }

  return true;
}

//--------------------------------------------------------------------------
// MilliTime
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 */
Double_t PFitter::MilliTime()
{
  struct timeval now;
  gettimeofday(&now, nullptr);

  return ((Double_t)now.tv_sec * 1.0e6 + (Double_t)now.tv_usec)/1.0e3;
}

//-------------------------------------------------------------------------------------------------
// end
//-------------------------------------------------------------------------------------------------
