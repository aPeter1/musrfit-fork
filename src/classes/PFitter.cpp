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
using namespace std;

#include <math.h>

#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnMinimize.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/MnSimplex.h"
#include "Minuit2/MnUserParameterState.h"

#include "TCanvas.h"
#include "TH2.h"
#include "TFile.h"
#include "TDatime.h"

#include "PFitter.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runInfo
 * \param runListCollection
 */
PFitter::PFitter(PMsrHandler *runInfo, PRunListCollection *runListCollection) :
  fRunInfo(runInfo)
{
  fUseChi2 = true; // chi^2 is the default

  fParams = *(runInfo->GetMsrParamList());
  fCmdLines = *runInfo->GetMsrCommands();

  // init class variables
  fFitterFcn = 0;
  fFcnMin = 0;

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
bool PFitter::DoFit()
{
  if (fUseChi2)
    cout << endl << "Chi Square fit will be executed" << endl;
  else
    cout << endl << "Maximum Likelihood fit will be executed" << endl;

  // feed minuit parameters
  SetParameters();

  bool status;
  for (unsigned int i=0; i<fCmdList.size(); i++) {
    switch (fCmdList[i]) {
      case PMN_INTERACTIVE:
        cout << endl << "**WARNING** from PFitter::DoFit() : the command INTERACTIVE is not yet implemented.";
        cout << endl;
        break;
      case PMN_CONTOURS:
        cout << endl << "**WARNING** from PFitter::DoFit() : the command CONTOURS is not yet implemented.";
        cout << endl;
        break;
      case PMN_EIGEN:
        cout << endl << "**WARNING** from PFitter::DoFit() : the command EIGEN is not yet implemented.";
        cout << endl;
        break;
      case PMN_HESSE:
        cout << endl << "**WARNING** from PFitter::DoFit() : the command HESSE is not yet implemented.";
        cout << endl;
        break;
      case PMN_MACHINE_PRECISION:
        cout << endl << "**WARNING** from PFitter::DoFit() : the command MACHINE_PRECISION is not yet implemented.";
        cout << endl;
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
        cout << endl << "**WARNING** from PFitter::DoFit() : the command PLOT is not yet implemented.";
        cout << endl;
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
        cout << endl << "**WARNING** from PFitter::DoFit() : the command USER_COVARIANCE is not yet implemented.";
        cout << endl;
        break;
      case PMN_USER_PARAM_STATE:
        cout << endl << "**WARNING** from PFitter::DoFit() : the command USER_PARAM_STATE is not yet implemented.";
        cout << endl;
        break;
      case PMN_PRINT:
        cout << endl << "**WARNING** from PFitter::DoFit() : the command PRINT is not yet implemented.";
        cout << endl;
        break;
      default:
        cout << endl << "**PANIC ERROR**: PFitter::DoFit(): You should never have reached this point" << endl;
        exit(0);
        break;
    }
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
bool PFitter::CheckCommands()
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
      fCmdList.push_back(PMN_STRATEGY);
    } else if (it->fLine.Contains("USER_COVARIANCE")) {
      fCmdList.push_back(PMN_USER_COVARIANCE);
    } else if (it->fLine.Contains("USER_PARAM_STATE")) {
      fCmdList.push_back(PMN_USER_PARAM_STATE);
    } else if (it->fLine.Contains("PRINT")) {
      fCmdList.push_back(PMN_PRINT);
    } else { // unkown command
      cout << endl << "FATAL ERROR:";
      cout << endl << "PFitter::CheckCommands(): In line " << it->fLineNo << " an unkown command is found:";
      cout << endl << "    " << it->fLine.Data();
      cout << endl << "Will stop ...";
      cout << endl;
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
bool PFitter::SetParameters()
{
  PMsrParamList::iterator it;

  for (it = fParams.begin(); it != fParams.end(); ++it) {
    // check if parameter is fixed
    if (it->fStep == 0.0) { // add fixed parameter
      fMnUserParams.Add(it->fName.Data(), it->fValue);
    } else { // add free parameter
      // check if boundaries are given
      if (it->fNoOfParams > 5) { // boundaries given
        fMnUserParams.Add(it->fName.Data(), it->fValue, it->fStep, it->fLowerBoundary, it->fUpperBoundary);
      } else { // no boundaries given
        fMnUserParams.Add(it->fName.Data(), it->fValue, it->fStep);
      }
    }
  }

  // check if there is an unused parameter, if so, fix it
  for (unsigned int i=0; i<fParams.size(); i++) {
    if (fRunInfo->ParameterInUse(i) == 0) { // parameter not used in the whole theory!!
      fMnUserParams.Fix(i); // fix the unused parameter so that minuit will not vary it
      cout << endl << "**WARNING** : Parameter No " << i+1 << " is not used at all, will fix it" << endl;
    }
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
bool PFitter::ExecuteMigrad()
{
  cout << "PFitter::ExecuteMigrad(): will call migrad ..." << endl;

  // if already some minimization is done use the minuit2 output as input
  if (fFcnMin)
    fMnUserParams = fFcnMin->UserParameters();

  // create migrad object
  ROOT::Minuit2::MnMigrad migrad((*fFitterFcn), fMnUserParams);

  // minimize
  ROOT::Minuit2::FunctionMinimum min = migrad();
  if (!min.IsValid()) {
    cout << endl << "**WARNING**: PFitter::ExecuteMigrad(): Fit did not converge, sorry ...";
    return false;
  }

  // keep FunctionMinimum object
  if (fFcnMin) { // fFcnMin exist hence clean up first
    delete fFcnMin;
  }
  fFcnMin = new ROOT::Minuit2::FunctionMinimum(min);

  // fill run info
  for (unsigned int i=0; i<fParams.size(); i++) {
    fRunInfo->SetMsrParamValue(i, min.UserState().Value(i));
    fRunInfo->SetMsrParamStep(i, min.UserState().Error(i));
  }

  // handle statistics
  double minVal = min.Fval();
  unsigned int ndf = fFitterFcn->GetTotalNoOfFittedBins();
  // subtract number of varied parameters from total no of fitted bins -> ndf
  for (unsigned int i=0; i<fParams.size(); i++) {
    if (min.UserState().Error(i) != 0.0)
      ndf -= 1;
  }

  // feed run info with new statistics info
  fRunInfo->SetMsrStatisticMin(minVal);
  fRunInfo->SetMsrStatisticNdf(ndf);

  return true;
}

//--------------------------------------------------------------------------
// ExecuteMinimize
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PFitter::ExecuteMinimize()
{
  cout << "PFitter::ExecuteMinimize(): will call minimize ..." << endl;

  // if already some minimization is done use the minuit2 output as input
  if (fFcnMin)
    fMnUserParams = fFcnMin->UserParameters();

  // create minimizer object
  ROOT::Minuit2::MnMinimize minimize((*fFitterFcn), fMnUserParams);

  // minimize
  ROOT::Minuit2::FunctionMinimum min = minimize(); 
  if (!min.IsValid()) {
    cout << endl << "**WARNING**: PFitter::ExecuteMinimize(): Fit did not converge, sorry ...";
    return false;
  }

  // keep FunctionMinimum object
  if (fFcnMin) { // fFcnMin exist hence clean up first
    delete fFcnMin;
  }
  fFcnMin = new ROOT::Minuit2::FunctionMinimum(min);

  // fill run info
  for (unsigned int i=0; i<fParams.size(); i++) {
    fRunInfo->SetMsrParamValue(i, min.UserState().Value(i));
    fRunInfo->SetMsrParamStep(i, min.UserState().Error(i));
  }

  // handle statistics
  double minVal = min.Fval();
  unsigned int ndf = fFitterFcn->GetTotalNoOfFittedBins();
  // subtract number of varied parameters from total no of fitted bins -> ndf
  for (unsigned int i=0; i<fParams.size(); i++) {
    if (min.UserState().Error(i) != 0.0)
      ndf -= 1;
  }

  // feed run info with new statistics info
  fRunInfo->SetMsrStatisticMin(minVal);
  fRunInfo->SetMsrStatisticNdf(ndf);

  return true;
}

//--------------------------------------------------------------------------
// ExecuteMinos
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PFitter::ExecuteMinos()
{
  cout << "PFitter::ExecuteMinos(): will call minos ..." << endl;

  // if already some minimization is done use the minuit2 output as input
  if (!fFcnMin) {
    cout << endl << "**ERROR**: MINOS musn't be called before any minimization (MINIMIZE/MIGRAD/SIMPLEX) is done!!";
    cout << endl;
    return false;
  }

  // check if minimum was valid
  if (!fFcnMin->IsValid()) {
    cout << endl << "**ERROR**: MINOS cannot started since the previews minimization faild :-(";
    cout << endl;
    return false;
  }

  fMnUserParams = fFcnMin->UserParameters();

  // make minos analysis
  ROOT::Minuit2::MnMinos minos((*fFitterFcn), (*fFcnMin));

  for (unsigned int i=0; i<fParams.size(); i++) {
    // only try to call minos if the parameter is not fixed!!
    // the 1st condition is from an user fixed variable,
    // the 2nd condition is from an all together unused variable
    if ((fMnUserParams.Error(i) != 0) && (fRunInfo->ParameterInUse(i) != 0)) {
      // 1-sigma MINOS errors
      std::pair<double, double> err = minos(i);

      // fill msr-file structure
      fRunInfo->SetMsrParamStep(i, err.first);
      fRunInfo->SetMsrParamPosError(i, err.second);
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
bool PFitter::ExecuteSave()
{
  cout << "PFitter::ExecuteSave(): will write minuit2 output file ..." << endl;

  // if any minimization was done, otherwise get out immediately
  if (!fFcnMin) {
    cout << endl << "PFitter::ExecuteSave(): nothing to be saved ...";
    return false;
  }

  ROOT::Minuit2::MnUserParameterState mnState = fFcnMin->UserState();

  // check if the user parameter state is valid
  if (!mnState.IsValid()) {
    cout << endl << "**WARNING** Minuit2 User Parameter State is not valid, i.e. nothing to be saved";
    cout << endl;
    return false;
  }

  ofstream fout;

  // open minuit2 output file
  fout.open("MINUIT2.OUTPUT", iostream::out);
  if (!fout.is_open()) {
    cout << endl << "**ERROR** PFitter::ExecuteSave() couldn't open MINUIT2.OUTPUT file";
    cout << endl;
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
  for (unsigned int i=0; i<fParams.size(); i++) {
    // write no
    fout.setf(ios::right, ios::adjustfield);
    fout.width(3);
    fout << endl << i+1 << "   ";
    // write name
    fout << fParams[i].fName.Data();
    for (int j=0; j<10-fParams[i].fName.Length(); j++)
      fout << " ";
    // write value
    fout.setf(ios::left, ios::adjustfield);
    fout.precision(6);
    fout.width(11);
    fout << fParams[i].fValue;
    // write parabolic error
    fout.setf(ios::left, ios::adjustfield);
    fout.precision(6);
    fout.width(11);
    fout << fMnUserParams.Error(i);
    // write minos errors
    if (fParams[i].fPosErrorPresent) {
      fout.setf(ios::left, ios::adjustfield);
      fout.precision(6);
      fout.width(12);
      fout << fParams[i].fStep; 
      fout.setf(ios::left, ios::adjustfield);
      fout.precision(6);
      fout.width(12);
      fout << fParams[i].fPosError;
    } else {
      fout.setf(ios::left, ios::adjustfield);
      fout.width(12);
      fout << "---";
      fout.setf(ios::left, ios::adjustfield);
      fout.width(12);
      fout << "---";
    }
    // write limits
    if (!isnan(fParams[i].fLowerBoundary)) {
      fout.setf(ios::left, ios::adjustfield);
      fout.width(7);
      fout << fParams[i].fLowerBoundary;
      fout.setf(ios::left, ios::adjustfield);
      fout.width(7);
      fout << fParams[i].fUpperBoundary;
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
    for (unsigned int i=0; i<cov.Nrow(); i++) {
      fout << endl;
      for (unsigned int j=0; j<i; j++) {
        if (cov(i,j) > 0.0)
          fout << " ";
        fout.setf(ios::left, ios::adjustfield);
        fout.precision(6);
        fout.width(14);
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
    fout << endl << " No   Global      ";
    for (unsigned int i=0; i<fParams.size(); i++) {
      fout.setf(ios::left, ios::adjustfield);
      fout.width(9);
      // only free parameters, i.e. not fixed, and not unsed ones!
      if ((fParams[i].fStep != 0) && fRunInfo->ParameterInUse(i) > 0) {
        fout << i+1;
        parNo.push_back(i);
      }
    }
    // check that there is a correspondens between minuit2 and musrfit book keeping
    if (parNo.size() != cov.Nrow()) {
      cout << endl << "**SEVERE ERROR** in PFitter::ExecuteSave(): minuit2 and musrfit book keeping to not correspond! Unable to write correlation matrix.";
      cout << endl;
    } else { // book keeping is OK
      TString title("Minuit2 Output Correlation Matrix for ");
      title += fRunInfo->GetFileName();
      title += " - ";
      title += dt.AsSQLString();
      TCanvas *ccorr = new TCanvas("ccorr", "title", 500, 500);
      TH2D *hcorr = new TH2D("hcorr", title, cov.Nrow(), 0.0, cov.Nrow(), cov.Nrow(), 0.0, cov.Nrow());
      double dval;
      for (unsigned int i=0; i<cov.Nrow(); i++) {
        // parameter number
        fout << endl << " ";
        fout.setf(ios::left, ios::adjustfield);
        fout.width(5);
        fout << parNo[i]+1;
        // global correlation coefficient
        fout.setf(ios::left, ios::adjustfield);
        fout.width(12);
        fout << corr.GlobalCC()[i];
        // correlations matrix
        for (unsigned int j=0; j<cov.Nrow(); j++) {
          fout.setf(ios::left, ios::adjustfield);
          fout.precision(4);
          fout.width(9);
          if (i==j) {
            fout << "1.0";
            hcorr->Fill((double)i,(double)i,1.0);
          } else {
            // check that errors are none zero
            if (fMnUserParams.Error(parNo[i]) == 0.0) {
              cout << endl << "**SEVERE ERROR** in PFitter::ExecuteSave(): parameter no " << parNo[i]+1 << " has an error == 0. Cannot correctly handle the correlation matrix.";
              cout << endl;
              dval = 0.0;
            } else if (fMnUserParams.Error(parNo[j]) == 0.0) {
              cout << endl << "**SEVERE ERROR** in PFitter::ExecuteSave(): parameter no " << parNo[j]+1 << " has an error == 0. Cannot correctly handle the correlation matrix.";
              cout << endl;
              dval = 0.0;
            } else {
              dval = cov(i,j)/(fMnUserParams.Error(parNo[i])*fMnUserParams.Error(parNo[j]));
            }
            hcorr->Fill((double)i,(double)j,dval);
            fout << dval;
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
      ccorr->Write();
      ff.Close();
      // clean up
      if (ccorr)
        delete ccorr;
      if (hcorr)
        delete hcorr;
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
bool PFitter::ExecuteSimplex()
{
  cout << "PFitter::ExecuteSimplex(): will call minimize ..." << endl;

  // if already some minimization is done use the minuit2 output as input
  if (fFcnMin)
    fMnUserParams = fFcnMin->UserParameters();

  // create minimizer object
  ROOT::Minuit2::MnSimplex simplex((*fFitterFcn), fMnUserParams);

  // minimize
  ROOT::Minuit2::FunctionMinimum min = simplex();
  if (!min.IsValid()) {
    cout << endl << "**WARNING**: PFitter::ExecuteSimplex(): Fit did not converge, sorry ...";
    return false;
  }

  // keep FunctionMinimum object
  if (fFcnMin) { // fFcnMin exist hence clean up first
    delete fFcnMin;
  }
  fFcnMin = new ROOT::Minuit2::FunctionMinimum(min);

  // fill run info
  for (unsigned int i=0; i<fParams.size(); i++) {
    fRunInfo->SetMsrParamValue(i, min.UserState().Value(i));
    fRunInfo->SetMsrParamStep(i, min.UserState().Error(i));
  }

  // handle statistics
  double minVal = min.Fval();
  unsigned int ndf = fFitterFcn->GetTotalNoOfFittedBins();
  // subtract number of varied parameters from total no of fitted bins -> ndf
  for (unsigned int i=0; i<fParams.size(); i++) {
    if (min.UserState().Error(i) != 0.0)
      ndf -= 1;
  }

  // feed run info with new statistics info
  fRunInfo->SetMsrStatisticMin(minVal);
  fRunInfo->SetMsrStatisticNdf(ndf);

  return true;
}
