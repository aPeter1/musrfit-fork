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

#include <iostream>
#include <fstream>
#include <limits>
using namespace std;

#include <math.h>

#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnHesse.h"
#include "Minuit2/MnMinimize.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnMinos.h"
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
 * <p>
 *
 * \param runInfo
 * \param runListCollection
 * \param chisq_only
 */
PFitter::PFitter(PMsrHandler *runInfo, PRunListCollection *runListCollection, Bool_t chisq_only) :
  fChisqOnly(chisq_only), fRunInfo(runInfo)
{
  fConverged = false;
  fUseChi2 = true; // chi^2 is the default

  fStrategy = 1; // 0=low, 1=default, 2=high

  fParams = *(runInfo->GetMsrParamList());
  fCmdLines = *runInfo->GetMsrCommands();

  // init class variables
  fFitterFcn = 0;
  fFcnMin = 0;
  fMnUserParamState = 0;

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
 * <p>
 *
 */
PFitter::~PFitter()
{
  fCmdList.clear();

  if (fMnUserParamState) {
    delete fMnUserParamState;
    fMnUserParamState = 0;
  }

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
 * <p>
 *
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

  // real fit wanted
  if (fUseChi2)
    cout << endl << "Chi Square fit will be executed" << endl;
  else
    cout << endl << "Maximum Likelihood fit will be executed" << endl;

  Bool_t status = true;
  // init positive errors to default false, if minos is called, it will be set true there
  for (UInt_t i=0; i<fParams.size(); i++) {
    fRunInfo->SetMsrParamPosErrorPresent(i, false);
  }

  for (UInt_t i=0; i<fCmdList.size(); i++) {
    switch (fCmdList[i]) {
      case PMN_INTERACTIVE:
        cerr << endl << "**WARNING** from PFitter::DoFit() : the command INTERACTIVE is not yet implemented.";
        cerr << endl;
        break;
      case PMN_CONTOURS:
        cerr << endl << "**WARNING** from PFitter::DoFit() : the command CONTOURS is not yet implemented.";
        cerr << endl;
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
        // set positive errors true if minos has been successfull
        if (status) {
          for (UInt_t i=0; i<fParams.size(); i++) {
            fRunInfo->SetMsrParamPosErrorPresent(i, true);
          }
        }
        break;
      case PMN_PLOT:
        cerr << endl << "**WARNING** from PFitter::DoFit() : the command PLOT is not yet implemented.";
        cerr << endl;
        break;
      case PMN_SAVE:
        status = ExecuteSave();
        break;
      case PMN_SCAN:
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
 * <p>
 *
 */
Bool_t PFitter::CheckCommands()
{
  fIsValid = true;

  PMsrLines::iterator it;
  for (it = fCmdLines.begin(); it != fCmdLines.end(); ++it) {
    it->fLine.ToUpper();
    if (it->fLine.Contains("COMMANDS")) {
      continue;
    } else if (it->fLine.Contains("SET BATCH")) { // needed for backward compatibility
      continue;
    } else if (it->fLine.Contains("END RETURN")) {  // needed for backward compatibility
      continue;
    } else if (it->fLine.Contains("CHI_SQUARE")) {
      fUseChi2 = true;
    } else if (it->fLine.Contains("MAX_LIKELIHOOD")) {
      fUseChi2 = false;
    } else if (it->fLine.Contains("INTERACTIVE")) {
      fCmdList.push_back(PMN_INTERACTIVE);
    } else if (it->fLine.Contains("CONTOURS")) {
      fCmdList.push_back(PMN_CONTOURS);
    } else if (it->fLine.Contains("EIGEN")) {
      fCmdList.push_back(PMN_EIGEN);
    } else if (it->fLine.Contains("HESSE")) {
      fCmdList.push_back(PMN_HESSE);
    } else if (it->fLine.Contains("MACHINE_PRECISION")) {
      fCmdList.push_back(PMN_MACHINE_PRECISION);
    } else if (it->fLine.Contains("MIGRAD")) {
      fCmdList.push_back(PMN_MIGRAD);
    } else if (it->fLine.Contains("MINIMIZE")) {
      fCmdList.push_back(PMN_MINIMIZE);
    } else if (it->fLine.Contains("MINOS")) {
      fCmdList.push_back(PMN_MINOS);
    } else if (it->fLine.Contains("PLOT")) {
      fCmdList.push_back(PMN_PLOT);
    } else if (it->fLine.Contains("SAVE")) {
      fCmdList.push_back(PMN_SAVE);
    } else if (it->fLine.Contains("SCAN")) {
      fCmdList.push_back(PMN_SCAN);
    } else if (it->fLine.Contains("SIMPLEX")) {
      fCmdList.push_back(PMN_SIMPLEX);
    } else if (it->fLine.Contains("STRATEGY")) {
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
    } else if (it->fLine.Contains("USER_COVARIANCE")) {
      fCmdList.push_back(PMN_USER_COVARIANCE);
    } else if (it->fLine.Contains("USER_PARAM_STATE")) {
      fCmdList.push_back(PMN_USER_PARAM_STATE);
    } else if (it->fLine.Contains("PRINT")) {
      fCmdList.push_back(PMN_PRINT);
    } else { // unkown command
      cerr << endl << "**FATAL ERROR**";
      cerr << endl << "PFitter::CheckCommands(): In line " << it->fLineNo << " an unkown command is found:";
      cerr << endl << "    " << it->fLine.Data();
      cerr << endl << "Will stop ...";
      cerr << endl;
      fIsValid = false;
    }
  }

  return fIsValid;
}

//--------------------------------------------------------------------------
// SetParameters
//--------------------------------------------------------------------------
/**
 * <p>
 *
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
//cout << endl << ">> name=" << fParams[i].fName.Data() << ", lower=" << fParams[i].fLowerBoundaryPresent << ", upper=" << fParams[i].fUpperBoundaryPresent;
        if (fParams[i].fLowerBoundaryPresent && 
            fParams[i].fUpperBoundaryPresent) { // upper and lower boundaries given
//cout << endl << ">> lower and upper";
          fMnUserParams.Add(fParams[i].fName.Data(), fParams[i].fValue, fParams[i].fStep,
                            fParams[i].fLowerBoundary, fParams[i].fUpperBoundary);
        } else if (fParams[i].fLowerBoundaryPresent && 
                   !fParams[i].fUpperBoundaryPresent) { // lower boundary limited
//cout << endl << ">> lower only";
          fMnUserParams.Add(fParams[i].fName.Data(), fParams[i].fValue, fParams[i].fStep);
          fMnUserParams.SetLowerLimit(fParams[i].fName.Data(), fParams[i].fLowerBoundary);
        } else { // upper boundary limited
//cout << endl << ">> upper only";
          fMnUserParams.Add(fParams[i].fName.Data(), fParams[i].fValue, fParams[i].fStep);
          fMnUserParams.SetUpperLimit(fParams[i].fName.Data(), fParams[i].fUpperBoundary);
        }
      } else { // no boundaries given
        fMnUserParams.Add(fParams[i].fName.Data(), fParams[i].fValue, fParams[i].fStep);
      }
    }
  }
//cout << endl;

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
// ExecuteHesse
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
Bool_t PFitter::ExecuteHesse()
{
  cout << "PFitter::ExecuteHesse(): will call hesse ..." << endl;

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

  // keep the user parameter state
  if (fMnUserParamState) {
    delete fMnUserParamState;
  }
  fMnUserParamState = new ROOT::Minuit2::MnUserParameterState(mnState);

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
 * <p>
 *
 */
Bool_t PFitter::ExecuteMigrad()
{
  cout << "PFitter::ExecuteMigrad(): will call migrad ..." << endl;

  // if already some minimization is done use the minuit2 output as input
  if (fFcnMin)
    fMnUserParams = fFcnMin->UserParameters();

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
  }
  fFcnMin = new ROOT::Minuit2::FunctionMinimum(min);

  // keep user parameter state
  if (fMnUserParamState) {
    delete fMnUserParamState;
  }
  fMnUserParamState = new ROOT::Minuit2::MnUserParameterState(min.UserState());

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
    if (min.UserState().Error(i) != 0.0)
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
 * <p>
 *
 */
Bool_t PFitter::ExecuteMinimize()
{
  cout << "PFitter::ExecuteMinimize(): will call minimize ..." << endl;

  // if already some minimization is done use the minuit2 output as input
  if (fFcnMin)
    fMnUserParams = fFcnMin->UserParameters();

  // create minimizer object
  // strategy is by default = 'default'
  ROOT::Minuit2::MnMinimize minimize((*fFitterFcn), fMnUserParams, fStrategy);

  // minimize
  // maxfcn is MINUIT2 Default maxfcn
  UInt_t maxfcn = numeric_limits<UInt_t>::max();
//cout << endl << "maxfcn=" << maxfcn << endl;
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
  }
  fFcnMin = new ROOT::Minuit2::FunctionMinimum(min);

  // keep user parameter state
  if (fMnUserParamState) {
    delete fMnUserParamState;
  }
  fMnUserParamState = new ROOT::Minuit2::MnUserParameterState(min.UserState());

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
    if (min.UserState().Error(i) != 0.0)
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
 * <p>
 *
 */
Bool_t PFitter::ExecuteMinos()
{
  cout << "PFitter::ExecuteMinos(): will call minos ..." << endl;

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
    if ((fMnUserParams.Error(i) != 0) && (fRunInfo->ParameterInUse(i) != 0)) {
      // 1-sigma MINOS errors
      ROOT::Minuit2::MinosError err = minos.Minos(i);

      if (err.IsValid()) {
        // fill msr-file structure
        fRunInfo->SetMsrParamStep(i, err.Lower());
        fRunInfo->SetMsrParamPosError(i, err.Upper());
      } else {
        fRunInfo->SetMsrParamPosErrorPresent(i, false);
      }
    }
  }

  return true;
}

//--------------------------------------------------------------------------
// ExecuteSave
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
Bool_t PFitter::ExecuteSave()
{
  // if any minimization was done, otherwise get out immediately
  if (!fFcnMin) {
    cout << endl << "PFitter::ExecuteSave(): nothing to be saved ...";
    return false;
  }

  ROOT::Minuit2::MnUserParameterState mnState = fFcnMin->UserState();

  // check if the user parameter state is valid
  if (!mnState.IsValid()) {
    cerr << endl << "**WARNING** Minuit2 User Parameter State is not valid, i.e. nothing to be saved";
    cerr << endl;
    return false;
  }

  cout << "PFitter::ExecuteSave(): will write minuit2 output file ..." << endl;

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

  // write parameters
  fParams = *(fRunInfo->GetMsrParamList()); // get the update parameters back
  fout << endl << " PARAMETERS";
  fout << endl << "-------------------------------------------------------------------------";
  fout << endl << "                         Parabolic           Minos";
  fout << endl << " No   Name      Value      Error      Negative    Positive   Limits";
  for (UInt_t i=0; i<fParams.size(); i++) {
    // write no
    fout.setf(ios::right, ios::adjustfield);
    fout.width(3);
    fout << endl << i+1 << "   ";
    // write name
    fout << fParams[i].fName.Data();
    for (Int_t j=0; j<10-fParams[i].fName.Length(); j++)
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
    if (fMnUserParamState)
      fout << fMnUserParamState->Error(i) << " ";
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
      if ((fParams[i].fStep != 0) && fRunInfo->ParameterInUse(i) > 0) {
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
 * <p>
 *
 */
Bool_t PFitter::ExecuteSimplex()
{
  cout << "PFitter::ExecuteSimplex(): will call simplex ..." << endl;

  // if already some minimization is done use the minuit2 output as input
  if (fFcnMin)
    fMnUserParams = fFcnMin->UserParameters();

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
  }
  fFcnMin = new ROOT::Minuit2::FunctionMinimum(min);

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
    if (min.UserState().Error(i) != 0.0)
      ndf -= 1;
  }

  // feed run info with new statistics info
  fRunInfo->SetMsrStatisticMin(minVal);
  fRunInfo->SetMsrStatisticNdf(ndf);

  fConverged = true;

  return true;
}
