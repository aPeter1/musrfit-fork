/***************************************************************************

  PFitter.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
 *   andreas.suter@psi.c                                                   *
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
#include <fstream>
#include <limits>
using namespace std;

#include <math.h>

#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnContours.h"
#include "Minuit2/MnHesse.h"
#include "Minuit2/MnMinimize.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/MnPlot.h"
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

  fParams = *(runInfo->GetMsrParamList());
  fCmdLines = *runInfo->GetMsrCommands();

  // init class variables
  fFitterFcn = 0;
  fFcnMin = 0;

  fScanAll = true;
  fScanParameter[0] = 0;
  fScanParameter[1] = 0;
  fScanNoPoints = 41; // minuit2 default
  fScanLow  = 0.0; // minuit2 default, i.e. 2 std deviations
  fScanHigh = 0.0; // minuit2 default, i.e. 2 std deviations

  // keep all the fit ranges in case RANGE command is present
  PMsrRunList *runs = fRunInfo->GetMsrRunList();
  PDoublePair range;
  for (UInt_t i=0; i<runs->size(); i++) {
    range.first  = (*runs)[i].GetFitRange(0);
    range.second = (*runs)[i].GetFitRange(1);
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

  if (fFcnMin) {
    delete fFcnMin;
    fFcnMin = 0;
  }

  if (fFitterFcn) {
    delete fFitterFcn;
    fFitterFcn = 0;
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
    Int_t ndf = fFitterFcn->GetTotalNoOfFittedBins() - usedParams;
    Double_t val = (*fFitterFcn)(param);
    if (fUseChi2) {
      cout << endl << endl << ">> chisq = " << val << ", NDF = " << ndf << ", chisq/NDF = " << val/ndf;
    } else { // max. log likelihood
      cout << endl << endl << ">> maxLH = " << val << ", NDF = " << ndf << ", maxLH/NDF = " << val/ndf;
    }
    cout << endl << endl;
    return true;
  }

  // debugging information
  #ifdef HAVE_GOMP
  cout << endl << ">> Number of available threads for the function optimization: " << omp_get_num_procs() << endl;
  #endif

  // real fit wanted
  if (fUseChi2)
    cout << endl << ">> Chi Square fit will be executed" << endl;
  else
    cout << endl << ">> Maximum Likelihood fit will be executed" << endl;

  Bool_t status = true;
  // init positive errors to default false, if minos is called, it will be set true there
  for (UInt_t i=0; i<fParams.size(); i++) {
    fRunInfo->SetMsrParamPosErrorPresent(i, false);
  }

  // walk through the command list and execute them
  for (UInt_t i=0; i<fCmdList.size(); i++) {
    switch (fCmdList[i].first) {
      case PMN_INTERACTIVE:
        cerr << endl << "**WARNING** from PFitter::DoFit() : the command INTERACTIVE is not yet implemented.";
        cerr << endl;
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
        cerr << endl << "**WARNING** from PFitter::DoFit() : the command EIGEN is not yet implemented.";
        cerr << endl;
        break;
      case PMN_HESSE:
        status = ExecuteHesse();
        break;
      case PMN_MACHINE_PRECISION:
        cerr << endl << "**WARNING** from PFitter::DoFit() : the command MACHINE_PRECISION is not yet implemented.";
        cerr << endl;
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
        status = ExecuteSave();
        break;
      case PMN_SCAN:
        status = ExecuteScan();
        break;
      case PMN_SIMPLEX:
        status = ExecuteSimplex();
        break;
      case PMN_USER_COVARIANCE:
        cerr << endl << "**WARNING** from PFitter::DoFit() : the command USER_COVARIANCE is not yet implemented.";
        cerr << endl;
        break;
      case PMN_USER_PARAM_STATE:
        cerr << endl << "**WARNING** from PFitter::DoFit() : the command USER_PARAM_STATE is not yet implemented.";
        cerr << endl;
        break;
      case PMN_PRINT:
        cerr << endl << "**WARNING** from PFitter::DoFit() : the command PRINT is not yet implemented.";
        cerr << endl;
        break;
      default:
        cerr << endl << "**PANIC ERROR**: PFitter::DoFit(): You should never have reached this point";
        cerr << endl;
        exit(0);
        break;
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
  for (it = fCmdLines.begin(); it != fCmdLines.end(); ++it) {
    if (it == fCmdLines.begin())
      cmdLineNo = 0;
    else
      cmdLineNo++;
    if (it->fLine.Contains("COMMANDS", TString::kIgnoreCase)) {
      continue;
    } else if (it->fLine.Contains("SET BATCH", TString::kIgnoreCase)) { // needed for backward compatibility
      continue;
    } else if (it->fLine.Contains("END RETURN", TString::kIgnoreCase)) {  // needed for backward compatibility
      continue;
    } else if (it->fLine.Contains("CHI_SQUARE", TString::kIgnoreCase)) {
      continue;
    } else if (it->fLine.Contains("MAX_LIKELIHOOD", TString::kIgnoreCase)) {
      continue;
    } else if (it->fLine.Contains("SCALE_N0_BKG", TString::kIgnoreCase)) {
      continue;
    } else if (it->fLine.Contains("INTERACTIVE", TString::kIgnoreCase)) {
      cmd.first  = PMN_INTERACTIVE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("CONTOURS", TString::kIgnoreCase)) {
      cmd.first  = PMN_CONTOURS;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
      // filter out possible parameters for scan
      TObjArray *tokens = 0;
      TObjString *ostr;
      TString str;
      UInt_t ival;

      tokens = it->fLine.Tokenize(", \t");

      for (Int_t i=0; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();

        if ((i==1) || (i==2)) { // parX / parY
          // check that token is a UInt_t
          if (!str.IsDigit()) {
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> parameter number is not number!";
            cerr << endl << ">> command syntax for CONTOURS is: CONTOURS parameter-X parameter-Y [# of points]";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          }
          ival = str.Atoi();
          // check that parameter is within range
          if ((ival < 1) || (ival > fParams.size())) {
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> parameter number is out of range [1," << fParams.size() << "]!";
            cerr << endl << ">> command syntax for CONTOURS is: CONTOURS parameter-X parameter-Y [# of points]";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
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
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> number of points is not number!";
            cerr << endl << ">> command syntax for CONTOURS is: CONTOURS parameter-X parameter-Y [# of points]";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          }
          ival = str.Atoi();
          if ((ival < 1) || (ival > 100)) {
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> number of scan points is out of range [1,100]!";
            cerr << endl << ">> command syntax for CONTOURS is: CONTOURS parameter-X parameter-Y [# of points]";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          }
          fScanNoPoints = ival;
        }
      }

      if (tokens) {
        delete tokens;
        tokens = 0;
      }
    } else if (it->fLine.Contains("EIGEN", TString::kIgnoreCase)) {
      cmd.first  = PMN_EIGEN;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("FIT_RANGE", TString::kIgnoreCase)) {
      // check the 3 options: FIT_RANGE RESET, FIT_RANGE start end, FIT_RANGE start1 end1 start2 end2 ... startN endN
      TObjArray *tokens = 0;
      TObjString *ostr;
      TString str;

      tokens = it->fLine.Tokenize(", \t");

      if (tokens->GetEntries() == 2) {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.Contains("RESET"), TString::kIgnoreCase) {
          cmd.first = PMN_FIT_RANGE;
          cmd.second = cmdLineNo;
          fCmdList.push_back(cmd);
        } else {
          cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
          cerr << endl << ">> " << it->fLine.Data();
          cerr << endl << ">> Syntax: FIT_RANGE RESET | FIT_RANGE start end | FIT_RANGE s1 e1 s2 e2 .. sN eN,";
          cerr << endl << ">>         with N the number of runs in the msr-file." << endl;
          cerr << endl << ">>         Found " << str.Data() << ", instead of RESET" << endl;
          fIsValid = false;
          if (tokens) {
            delete tokens;
            tokens = 0;
          }
          break;
        }
      } else if ((tokens->GetEntries() > 1) && (static_cast<UInt_t>(tokens->GetEntries()) % 2) == 1) {
        if ((tokens->GetEntries() > 3) && ((static_cast<UInt_t>(tokens->GetEntries())-1)) != 2*fRunInfo->GetMsrRunList()->size()) {
          cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
          cerr << endl << ">> " << it->fLine.Data();
          cerr << endl << ">> Syntax: FIT_RANGE RESET | FIT_RANGE start end | FIT_RANGE s1 e1 s2 e2 .. sN eN,";
          cerr << endl << ">>         with N the number of runs in the msr-file.";
          cerr << endl << ">>         Found N=" << (tokens->GetEntries()-1)/2 << ", # runs in msr-file=" << fRunInfo->GetMsrRunList()->size() << endl;
          fIsValid = false;
          if (tokens) {
            delete tokens;
            tokens = 0;
          }
          break;
        } else {
          // check that all range entries are numbers
          Int_t n=1;
          do {
            ostr = dynamic_cast<TObjString*>(tokens->At(n));
            str = ostr->GetString();
          } while ((++n < tokens->GetEntries()) && str.IsFloat());

          if (str.IsFloat()) { // everything is fine, last string was a floating point number
            cmd.first = PMN_FIT_RANGE;
            cmd.second = cmdLineNo;
            fCmdList.push_back(cmd);
          } else {
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> Syntax: FIT_RANGE RESET | FIT_RANGE <start> <end> | FIT_RANGE <s1> <e1> <s2> <e2> .. <sN> <eN>,";
            cerr << endl << ">>         with N the number of runs in the msr-file.";
            cerr << endl << ">>         Found token '" << str.Data() << "', which is not a floating point number." << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          }
        }
      } else {
        cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
        cerr << endl << ">> " << it->fLine.Data();
        cerr << endl << ">> Syntax: FIT_RANGE RESET | FIT_RANGE start end | FIT_RANGE s1 e1 s2 e2 .. sN eN,";
        cerr << endl << ">>         with N the number of runs in the msr-file." << endl;
        fIsValid = false;
        if (tokens) {
          delete tokens;
          tokens = 0;
        }
        break;
      }

      if (tokens) {
        delete tokens;
        tokens = 0;
      }
    } else if (it->fLine.Contains("FIX", TString::kIgnoreCase)) {
      // check if the given set of parameters (number or names) is present
      TObjArray *tokens = 0;
      TObjString *ostr;
      TString str;
      UInt_t ival;

      tokens = it->fLine.Tokenize(", \t");

      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();

        if (str.IsDigit()) { // token might be a parameter number
          ival = str.Atoi();
          // check that ival is in the parameter list
          if (ival > fParams.size()) {
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> Parameter " << ival << " is out of the Parameter Range [1," << fParams.size() << "]";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
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
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> Parameter '" << str.Data() << "' is NOT present as a parameter name";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          }
        }
      }

      if (tokens) {
        delete tokens;
        tokens = 0;
      }

      // everything looks fine, feed the command list
      cmd.first  = PMN_FIX;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("HESSE", TString::kIgnoreCase)) {
      fIsScanOnly = false;
      cmd.first  = PMN_HESSE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("MACHINE_PRECISION", TString::kIgnoreCase)) {
      cmd.first  = PMN_MACHINE_PRECISION;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("MIGRAD", TString::kIgnoreCase)) {
      fIsScanOnly = false;
      cmd.first  = PMN_MIGRAD;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("MINIMIZE", TString::kIgnoreCase)) {
      fIsScanOnly = false;
      cmd.first  = PMN_MINIMIZE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("MINOS", TString::kIgnoreCase)) {
      fIsScanOnly = false;
      cmd.first  = PMN_MINOS;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("MNPLOT", TString::kIgnoreCase)) {
      cmd.first  = PMN_PLOT;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("RELEASE", TString::kIgnoreCase)) {
      // check if the given set of parameters (number or names) is present
      TObjArray *tokens = 0;
      TObjString *ostr;
      TString str;
      UInt_t ival;

      tokens = it->fLine.Tokenize(", \t");

      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();

        if (str.IsDigit()) { // token might be a parameter number
          ival = str.Atoi();
          // check that ival is in the parameter list
          if (ival > fParams.size()) {
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> Parameter " << ival << " is out of the Parameter Range [1," << fParams.size() << "]";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
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
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> Parameter '" << str.Data() << "' is NOT present as a parameter name";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          }
        }
      }

      if (tokens) {
        delete tokens;
        tokens = 0;
      }
      cmd.first  = PMN_RELEASE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("RESTORE", TString::kIgnoreCase)) {
      cmd.first  = PMN_RESTORE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("SAVE", TString::kIgnoreCase)) {
      cmd.first  = PMN_SAVE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("SCAN", TString::kIgnoreCase)) {
      cmd.first  = PMN_SCAN;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
      // filter out possible parameters for scan
      TObjArray *tokens = 0;
      TObjString *ostr;
      TString str;
      UInt_t ival;

      tokens = it->fLine.Tokenize(", \t");

      for (Int_t i=0; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();
        if (i==1) { // get parameter number
          // check that token is a UInt_t
          if (!str.IsDigit()) {
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> parameter number is not number!";
            cerr << endl << ">> command syntax for SCAN is: SCAN [parameter no [# of points [low high]]]";
            cerr << endl;            
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          }
          ival = str.Atoi();
          // check that parameter is within range
          if ((ival < 1) || (ival > fParams.size())) {
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> parameter number is out of range [1," << fParams.size() << "]!";
            cerr << endl << ">> command syntax for SCAN is: SCAN [parameter no [# of points [low high]]]";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
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
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> number of points is not number!";
            cerr << endl << ">> command syntax for SCAN is: SCAN [parameter no [# of points [low high]]]";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          }
          ival = str.Atoi();
          if ((ival < 1) || (ival > 100)) {
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> number of scan points is out of range [1,100]!";
            cerr << endl << ">> command syntax for SCAN is: SCAN [parameter no [# of points [low high]]]";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          }
          fScanNoPoints = ival;
        }

        if (i==3) { // get low
          // check that token is a Double_t
          if (!str.IsFloat()) {
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> low is not a floating point number!";
            cerr << endl << ">> command syntax for SCAN is: SCAN [parameter no [# of points [low high]]]";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          }
          fScanLow = str.Atof();
        }

        if (i==4) { // get high
          // check that token is a Double_t
          if (!str.IsFloat()) {
            cerr << endl << ">> PFitter::CheckCommands: **ERROR** in line " << it->fLineNo;
            cerr << endl << ">> " << it->fLine.Data();
            cerr << endl << ">> high is not a floating point number!";
            cerr << endl << ">> command syntax for SCAN is: SCAN [parameter no [# of points [low high]]]";
            cerr << endl;
            fIsValid = false;
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          }
          fScanHigh = str.Atof();
        }
      }

      if (tokens) {
        delete tokens;
        tokens = 0;
      }
    } else if (it->fLine.Contains("SIMPLEX", TString::kIgnoreCase)) {
      cmd.first  = PMN_SIMPLEX;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("STRATEGY", TString::kIgnoreCase)) {
      TObjArray *tokens = 0;
      TObjString *ostr;
      TString str;

      tokens = it->fLine.Tokenize(" \t");
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
        tokens = 0;
      }
    } else if (it->fLine.Contains("USER_COVARIANCE", TString::kIgnoreCase)) {
      cmd.first  = PMN_USER_COVARIANCE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("USER_PARAM_STATE", TString::kIgnoreCase)) {
      cmd.first  = PMN_USER_PARAM_STATE;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else if (it->fLine.Contains("PRINT", TString::kIgnoreCase)) {
      cmd.first  = PMN_PRINT;
      cmd.second = cmdLineNo;
      fCmdList.push_back(cmd);
    } else { // unkown command
      cerr << endl << ">> PFitter::CheckCommands(): **FATAL ERROR** in line " << it->fLineNo << " an unkown command is found:";
      cerr << endl << ">> " << it->fLine.Data();
      cerr << endl << ">> Will stop ...";
      cerr << endl;
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
    if (it->fLine.Contains("FIX", TString::kIgnoreCase))
      fixFlag = true;
    else if (it->fLine.Contains("RELEASE", TString::kIgnoreCase) ||
        it->fLine.Contains("RESTORE", TString::kIgnoreCase))
      releaseFlag = true;
    else if (it->fLine.Contains("MINIMIZE", TString::kIgnoreCase) ||
             it->fLine.Contains("MIGRAD", TString::kIgnoreCase) ||
             it->fLine.Contains("SIMPLEX", TString::kIgnoreCase)) {
      if (releaseFlag)
        minimizerFlag = true;
    } else if (it->fLine.Contains("MINOS", TString::kIgnoreCase)) {
      if (fixFlag && releaseFlag && !minimizerFlag) {
        cerr << endl << ">> PFitter::CheckCommands(): **WARNING** RELEASE/RESTORE command present";
        cerr << endl << ">> without minimizer command (MINIMIZE/MIGRAD/SIMPLEX) between";
        cerr << endl << ">> RELEASE/RESTORE and MINOS. Behaviour might be different to the";
        cerr << endl << ">> expectation of the user ?!?" << endl;
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
      cerr << endl << "**WARNING** : Parameter No " << i+1 << " is not used at all, will fix it";
      cerr << endl;
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
  cout << ">> PFitter::ExecuteContours() ..." << endl;

  // if already some minimization is done use the minuit2 output as input
  if (!fFcnMin) {
    cerr << endl << "**WARNING**: CONTOURS musn't be called before any minimization (MINIMIZE/MIGRAD/SIMPLEX) is done!!";
    cerr << endl;
    return false;
  }

  // check if minimum was valid
  if (!fFcnMin->IsValid()) {
    cerr << endl << "**ERROR**: CONTOURS cannot started since the previous minimization failed :-(";
    cerr << endl;
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
  cout << ">> PFitter::ExecuteFitRange(): " << fCmdLines[lineNo].fLine.Data() << endl;

  TObjArray *tokens = 0;
  TObjString *ostr;
  TString str;

  tokens = fCmdLines[lineNo].fLine.Tokenize(", \t");

  PMsrRunList *runList = fRunInfo->GetMsrRunList();

  // execute command, no error  checking needed since this has been already carried out in CheckCommands()
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
  cout << ">> PFitter::ExecuteFix(): " << fCmdLines[lineNo].fLine.Data() << endl;

  TObjArray *tokens = 0;
  TObjString *ostr;
  TString str;

  tokens = fCmdLines[lineNo].fLine.Tokenize(", \t");

  // Check if there is already a function minimum, i.e. migrad, minimization, or simplex has been called previously.
  // If so, update minuit2 user parameters
  if (fFcnMin != 0) {
    if (fFcnMin->IsValid()) {
      fMnUserParams = fFcnMin->UserParameters();
    }
  }

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
    tokens = 0;
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
  cout << ">> PFitter::ExecuteHesse(): will call hesse ..." << endl;

  // if already some minimization is done use the minuit2 output as input
  if (fFcnMin)
    fMnUserParams = fFcnMin->UserParameters();

  // create the hesse object
  ROOT::Minuit2::MnHesse hesse;

  // specify maximal number of function calls
  UInt_t maxfcn = numeric_limits<UInt_t>::max();

  // call hesse
  ROOT::Minuit2::MnUserParameterState mnState = hesse((*fFitterFcn), fMnUserParams, maxfcn);

  if (!mnState.IsValid()) {
    cerr << endl << "**WARNING** PFitter::ExecuteHesse(): Hesse encountered some problems!";
    cerr << endl;
    return false;
  }

  // fill parabolic errors
  for (UInt_t i=0; i<fParams.size(); i++) {
    fRunInfo->SetMsrParamStep(i, mnState.Error(i));
    fRunInfo->SetMsrParamPosErrorPresent(i, false);
  }

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
  cout << ">> PFitter::ExecuteMigrad(): will call migrad ..." << endl;

  // create migrad object
  // strategy is by default = 'default'
  ROOT::Minuit2::MnMigrad migrad((*fFitterFcn), fMnUserParams, fStrategy);

  // minimize
  // maxfcn is MINUIT2 Default maxfcn
  UInt_t maxfcn = numeric_limits<UInt_t>::max();
  // tolerance = MINUIT2 Default tolerance
  Double_t tolerance = 0.1;
  ROOT::Minuit2::FunctionMinimum min = migrad(maxfcn, tolerance);
  if (!min.IsValid()) {
    cerr << endl << "**WARNING**: PFitter::ExecuteMigrad(): Fit did not converge, sorry ...";
    cerr << endl;
    fIsValid = false;
    return false;
  }

  // keep FunctionMinimum object
  if (fFcnMin) { // fFcnMin exist hence clean up first
    delete fFcnMin;
    fFcnMin = 0;
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
  cout << ">> PFitter::ExecuteMinimize(): will call minimize ..." << endl;

  // create minimizer object
  // strategy is by default = 'default'
  ROOT::Minuit2::MnMinimize minimize((*fFitterFcn), fMnUserParams, fStrategy);

  // minimize
  // maxfcn is MINUIT2 Default maxfcn
  UInt_t maxfcn = numeric_limits<UInt_t>::max();

  // tolerance = MINUIT2 Default tolerance
  Double_t tolerance = 0.1;
  ROOT::Minuit2::FunctionMinimum min = minimize(maxfcn, tolerance);
  if (!min.IsValid()) {
    cerr << endl << "**WARNING**: PFitter::ExecuteMinimize(): Fit did not converge, sorry ...";
    cerr << endl;
    fIsValid = false;
    return false;
  }

  // keep FunctionMinimum object
  if (fFcnMin) { // fFcnMin exist hence clean up first
    delete fFcnMin;
    fFcnMin = 0;
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
  cout << ">> PFitter::ExecuteMinos(): will call minos ..." << endl;

  // if already some minimization is done use the minuit2 output as input
  if (!fFcnMin) {
    cerr << endl << "**ERROR**: MINOS musn't be called before any minimization (MINIMIZE/MIGRAD/SIMPLEX) is done!!";
    cerr << endl;
    return false;
  }

  // check if minimum was valid
  if (!fFcnMin->IsValid()) {
    cerr << endl << "**ERROR**: MINOS cannot started since the previous minimization failed :-(";
    cerr << endl;
    return false;
  }

  fMnUserParams = fFcnMin->UserParameters();

  // make minos analysis
  ROOT::Minuit2::MnMinos minos((*fFitterFcn), (*fFcnMin));

  for (UInt_t i=0; i<fParams.size(); i++) {
    // only try to call minos if the parameter is not fixed!!
    // the 1st condition is from an user fixed variable,
    // the 2nd condition is from an all together unused variable
    // the 3rd condition is a variable fixed via the FIX command
    if ((fMnUserParams.Error(i) != 0) && (fRunInfo->ParameterInUse(i) != 0) && (!fMnUserParams.Parameters().at(i).IsFixed())) {
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
      cerr << endl << ">> PFitter::ExecuteMinos(): **WARNING** Parameter " << fMnUserParams.Name(i) << " (ParamNo " << i+1 << ") is fixed!";
      cerr << endl << ">>    Will set STEP to zero, i.e. making it a constant parameter";
      cerr << endl;
      fRunInfo->SetMsrParamStep(i, 0.0);
      fRunInfo->SetMsrParamPosErrorPresent(i, false);
    }
  }

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
  cout << ">> PFitter::ExecutePlot() ..." << endl;

  ROOT::Minuit2::MnPlot plot;
  plot(fScanData);

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
  TObjArray *tokens = 0;
  TObjString *ostr;
  TString str;

  tokens = fCmdLines[lineNo].fLine.Tokenize(", \t");

  cout << ">> PFitter::ExecuteRelease(): " << fCmdLines[lineNo].fLine.Data() << endl;

  for (Int_t i=1; i<tokens->GetEntries(); i++) {
    ostr = dynamic_cast<TObjString*>(tokens->At(i));
    str = ostr->GetString();

    if (str.IsDigit()) { // token is a parameter number
      fMnUserParams.Release(static_cast<UInt_t>(str.Atoi())-1);
    } else { // token is a parameter name
      fMnUserParams.Release(str.Data());
    }
  }

  // clean up
  if (tokens) {
    delete tokens;
    tokens = 0;
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
  cout << "PFitter::ExecuteRestore(): release all fixed parameters (RESTORE) ..." << endl;

  for (UInt_t i=0; i<fMnUserParams.Parameters().size(); i++) {
    if (fMnUserParams.Parameters().at(i).IsFixed())
      fMnUserParams.Release(i);
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
  cout << ">> PFitter::ExecuteScan(): will call scan ..." << endl;

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
 * <b>return:</b> true if the valid minuit2 state is found, otherwise returns false.
 */
Bool_t PFitter::ExecuteSave()
{
  // if any minimization was done, otherwise get out immediately
  if (!fFcnMin) {
    cout << endl << ">> PFitter::ExecuteSave(): nothing to be saved ...";
    return false;
  }

  ROOT::Minuit2::MnUserParameterState mnState = fFcnMin->UserState();

  // check if the user parameter state is valid
  if (!mnState.IsValid()) {
    cerr << endl << "**WARNING** Minuit2 User Parameter State is not valid, i.e. nothing to be saved";
    cerr << endl;
    return false;
  }

  cout << ">> PFitter::ExecuteSave(): will write minuit2 output file ..." << endl;

  ofstream fout;

  // open minuit2 output file
  fout.open("MINUIT2.OUTPUT", iostream::out);
  if (!fout.is_open()) {
    cerr << endl << "**ERROR** PFitter::ExecuteSave() couldn't open MINUIT2.OUTPUT file";
    cerr << endl;
    return false;
  }

  // write header
  TDatime dt;
  fout << endl << "*************************************************************************";
  fout << endl << " musrfit MINUIT2 output file from " << fRunInfo->GetFileName().Data() << " - " << dt.AsSQLString();
  fout << endl << "*************************************************************************";
  fout << endl;

  // write global informations
  fout << endl << " Fval() = " << mnState.Fval() << ", Edm() = " << mnState.Edm() << ", NFcn() = " << mnState.NFcn();
  fout << endl;
  fout << endl << "*************************************************************************";

  // identifiy the longest variable name for proper formating reasons
  Int_t maxLength = 10;
  for (UInt_t i=0; i<fParams.size(); i++) {
    if (fParams[i].fName.Length() > maxLength)
      maxLength = fParams[i].fName.Length() + 1;
  }

  // write parameters
  fParams = *(fRunInfo->GetMsrParamList()); // get the update parameters back
  fout << endl << " PARAMETERS";
  fout << endl << "-------------------------------------------------------------------------";
  fout << endl << "                         ";
  for (Int_t j=0; j<=maxLength-4; j++)
    fout << " ";
  fout << "Parabolic           Minos";
  fout << endl << " No   Name";
  for (Int_t j=0; j<=maxLength-4; j++)
    fout << " ";
  fout << "Value      Error      Negative    Positive    Limits";
  for (UInt_t i=0; i<fParams.size(); i++) {
    // write no
    fout.setf(ios::right, ios::adjustfield);
    fout.width(3);
    fout << endl << i+1 << "   ";
    // write name
    fout << fParams[i].fName.Data();
    for (Int_t j=0; j<=maxLength-fParams[i].fName.Length(); j++)
      fout << " ";
    // write value
    fout.setf(ios::left, ios::adjustfield);
    fout.precision(6);
    fout.width(10);
    fout << fParams[i].fValue << " ";
    // write parabolic error
    fout.setf(ios::left, ios::adjustfield);
    fout.precision(6);
    fout.width(10);
    if (fParams[i].fStep != 0.0)
      fout << fMnUserParams.Error(i) << " ";
    else
      fout << "---";
    // write minos errors
    if (fParams[i].fPosErrorPresent) {
      fout.setf(ios::left, ios::adjustfield);
      fout.precision(6);
      fout.width(12);
      fout << fParams[i].fStep; 
      fout.setf(ios::left, ios::adjustfield);
      fout.precision(6);
      fout.width(11);
      fout << fParams[i].fPosError << " ";
    } else {
      fout.setf(ios::left, ios::adjustfield);
      fout.width(12);
      fout << "---";
      fout.setf(ios::left, ios::adjustfield);
      fout.width(12);
      fout << "---";
    }
    // write limits
    if (fParams[i].fNoOfParams > 5) {
      fout.setf(ios::left, ios::adjustfield);
      fout.width(7);
      if (fParams[i].fLowerBoundaryPresent)
        fout << fParams[i].fLowerBoundary;
      else
        fout << "---";
      fout.setf(ios::left, ios::adjustfield);
      fout.width(7);
      if (fParams[i].fUpperBoundaryPresent)
        fout << fParams[i].fUpperBoundary;
      else
        fout << "---";
    } else {
      fout.setf(ios::left, ios::adjustfield);
      fout.width(7);
      fout << "---";
      fout.setf(ios::left, ios::adjustfield);
      fout.width(7);
      fout << "---";
    }
  }
  fout << endl;
  fout << endl << "*************************************************************************";

  // write covariance matrix
  fout << endl << " COVARIANCE MATRIX";
  fout << endl << "-------------------------------------------------------------------------";
  if (mnState.HasCovariance()) {
    ROOT::Minuit2::MnUserCovariance cov = mnState.Covariance();
    fout << endl << "from " << cov.Nrow() << " free parameters";
    for (UInt_t i=0; i<cov.Nrow(); i++) {
      fout << endl;
      for (UInt_t j=0; j<i; j++) {
        fout.setf(ios::left, ios::adjustfield);
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
    fout << endl << " no covariance matrix available";
  }
  fout << endl;
  fout << endl << "*************************************************************************";

  // write correlation matrix
  fout << endl << " CORRELATION COEFFICIENTS";
  fout << endl << "-------------------------------------------------------------------------";
  if (mnState.HasGlobalCC() && mnState.HasCovariance()) {
    ROOT::Minuit2::MnGlobalCorrelationCoeff corr = mnState.GlobalCC();
    ROOT::Minuit2::MnUserCovariance cov = mnState.Covariance();
    PIntVector parNo;
    fout << endl << " No   Global       ";
    for (UInt_t i=0; i<fParams.size(); i++) {
      // only free parameters, i.e. not fixed, and not unsed ones!
      if ((fParams[i].fStep != 0) && (fRunInfo->ParameterInUse(i) > 0) && (!fMnUserParams.Parameters().at(i).IsFixed())) {
        fout.setf(ios::left, ios::adjustfield);
        fout.width(9);
        fout << i+1;
        parNo.push_back(i);
      }
    }
    // check that there is a correspondens between minuit2 and musrfit book keeping
    if (parNo.size() != cov.Nrow()) {
      cerr << endl << "**SEVERE ERROR** in PFitter::ExecuteSave(): minuit2 and musrfit book keeping to not correspond! Unable to write correlation matrix.";
      cerr << endl;
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
        fout << endl << " ";
        fout.setf(ios::left, ios::adjustfield);
        fout.width(5);
        fout << parNo[i]+1;
        // global correlation coefficient
        fout.setf(ios::left, ios::adjustfield);
        fout.precision(6);
        fout.width(12);
        fout << corr.GlobalCC()[i];
        // correlations matrix
        for (UInt_t j=0; j<cov.Nrow(); j++) {
          fout.setf(ios::left, ios::adjustfield);
//          fout.precision(4);
          if (i==j) {
            fout.width(9);
            fout << " 1.0 ";
            hcorr->Fill((Double_t)i,(Double_t)i,1.0);
          } else {
            // check that errors are none zero
            if (fMnUserParams.Error(parNo[i]) == 0.0) {
              cerr << endl << "**SEVERE ERROR** in PFitter::ExecuteSave(): parameter no " << parNo[i]+1 << " has an error == 0. Cannot correctly handle the correlation matrix.";
              cerr << endl;
              dval = 0.0;
            } else if (fMnUserParams.Error(parNo[j]) == 0.0) {
              cerr << endl << "**SEVERE ERROR** in PFitter::ExecuteSave(): parameter no " << parNo[j]+1 << " has an error == 0. Cannot correctly handle the correlation matrix.";
              cerr << endl;
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
        ccorr = 0;
      }
      if (hcorr) {
        delete hcorr;
        hcorr = 0;
      }
    }
    parNo.clear(); // clean up
  } else {
    fout << endl << " no correlation coefficients available";
  }

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
  cout << ">> PFitter::ExecuteSimplex(): will call simplex ..." << endl;

  // create minimizer object
  // strategy is by default = 'default'
  ROOT::Minuit2::MnSimplex simplex((*fFitterFcn), fMnUserParams, fStrategy);

  // minimize
  // maxfcn is 10*MINUIT2 Default maxfcn
  UInt_t maxfcn = numeric_limits<UInt_t>::max();
  // tolerance = MINUIT2 Default tolerance
  Double_t tolerance = 0.1;
  ROOT::Minuit2::FunctionMinimum min = simplex(maxfcn, tolerance);
  if (!min.IsValid()) {
    cerr << endl << "**WARNING**: PFitter::ExecuteSimplex(): Fit did not converge, sorry ...";
    cerr << endl;
    fIsValid = false;
    return false;
  }

  // keep FunctionMinimum object
  if (fFcnMin) { // fFcnMin exist hence clean up first
    delete fFcnMin;
    fFcnMin = 0;
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

  return true;
}

//-------------------------------------------------------------------------------------------------
// end
//-------------------------------------------------------------------------------------------------
