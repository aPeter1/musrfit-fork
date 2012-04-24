/***************************************************************************

  PRunSingleHisto.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2012 by Andreas Suter                              *
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
#include <fstream>
using namespace std;

#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>

#include "PMusr.h"
#include "PRunSingleHisto.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 */
PRunSingleHisto::PRunSingleHisto() : PRunBase()
{
  fScaleN0AndBkg = true;
  fNoOfFitBins  = 0;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 *
 * \param msrInfo pointer to the msr-file handler
 * \param rawData raw run data
 * \param runNo number of the run within the msr-file
 * \param tag tag showing what shall be done: kFit == fitting, kView == viewing
 */
PRunSingleHisto::PRunSingleHisto(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag) : PRunBase(msrInfo, rawData, runNo, tag)
{
  fScaleN0AndBkg = IsScaleN0AndBkg();
  fNoOfFitBins  = 0;

  if (!PrepareData()) {
    cerr << endl << ">> PRunSingleHisto::PRunSingleHisto: **SEVERE ERROR**: Couldn't prepare data for fitting!";
    cerr << endl << ">> This is very bad :-(, will quit ...";
    cerr << endl;
    fValid = false;
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor
 */
PRunSingleHisto::~PRunSingleHisto()
{
  fForward.clear();
}

//--------------------------------------------------------------------------
// CalcChiSquare (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate chi-square.
 *
 * <b>return:</b>
 * - chisq value
 *
 * \param par parameter vector iterated by minuit2
 */
Double_t PRunSingleHisto::CalcChiSquare(const std::vector<Double_t>& par)
{
  Double_t chisq     = 0.0;
  Double_t diff      = 0.0;

  Double_t N0 = 0.0;

  // check if norm is a parameter or a function
  if (fRunInfo->GetNormParamNo() < MSR_PARAM_FUN_OFFSET) { // norm is a parameter
    N0 = par[fRunInfo->GetNormParamNo()-1];
  } else { // norm is a function
    // get function number
    UInt_t funNo = fRunInfo->GetNormParamNo()-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par);
  }

  // get tau
  Double_t tau;
  if (fRunInfo->GetLifetimeParamNo() != -1)
    tau = par[fRunInfo->GetLifetimeParamNo()-1];
  else
    tau = PMUON_LIFETIME;

  // get background
  Double_t bkg;
  if (fRunInfo->GetBkgFitParamNo() == -1) { // bkg not fitted
    if (fRunInfo->GetBkgFix(0) == PMUSR_UNDEFINED) { // no fixed background given (background interval)
      bkg = fBackground;
    } else { // fixed bkg given
      bkg = fRunInfo->GetBkgFix(0);
    }
  } else { // bkg fitted
    bkg = par[fRunInfo->GetBkgFitParamNo()-1];
  }

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    Int_t funcNo = fMsrInfo->GetFuncNo(i);
    fFuncValues[i] = fMsrInfo->EvalFunc(funcNo, *fRunInfo->GetMap(), par);
  }

  // calculate chi square
  Double_t time(1.0);
  Int_t i, N(static_cast<Int_t>(fData.GetValue()->size()));

  // In order not to have an IF in the next loop, determine the start and end bins for the fit range now
  Int_t startTimeBin = static_cast<Int_t>(ceil((fFitStartTime - fData.GetDataTimeStart())/fData.GetDataTimeStep()));
  if (startTimeBin < 0)
    startTimeBin = 0;
  Int_t endTimeBin = static_cast<Int_t>(floor((fFitEndTime - fData.GetDataTimeStart())/fData.GetDataTimeStep())) + 1;
  if (endTimeBin > N)
    endTimeBin = N;

  // Calculate the theory function once to ensure one function evaluation for the current set of parameters.
  // This is needed for the LF and user functions where some non-thread-save calculations only need to be calculated once
  // for a given set of parameters---which should be done outside of the parallelized loop.
  // For all other functions it means a tiny and acceptable overhead.
  time = fTheory->Func(time, par, fFuncValues);

  #ifdef HAVE_GOMP
  Int_t chunk = (endTimeBin - startTimeBin)/omp_get_num_procs();
  if (chunk < 10)
    chunk = 10;
  #pragma omp parallel for default(shared) private(i,time,diff) schedule(dynamic,chunk) reduction(+:chisq)
  #endif
  for (i=startTimeBin; i < endTimeBin; ++i) {
    time = fData.GetDataTimeStart() + (Double_t)i*fData.GetDataTimeStep();
    diff = fData.GetValue()->at(i) -
          (N0*TMath::Exp(-time/tau)*(1.0+fTheory->Func(time, par, fFuncValues))+bkg);
    chisq += diff*diff / (fData.GetError()->at(i)*fData.GetError()->at(i));
  }

  // the correction factor is need since the data scales like pack*t_res,
  // whereas the error scales like sqrt(pack*t_res)
  if (fScaleN0AndBkg)
    chisq *= fRunInfo->GetPacking() * (fTimeResolution * 1.0e3);

  return chisq;
}

//--------------------------------------------------------------------------
// CalcChiSquareExpected (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate expected chi-square.
 *
 * <b>return:</b>
 * - chisq value
 *
 * \param par parameter vector iterated by minuit2
 */
Double_t PRunSingleHisto::CalcChiSquareExpected(const std::vector<Double_t>& par)
{
  Double_t chisq = 0.0;
  Double_t diff  = 0.0;
  Double_t theo  = 0.0;

  Double_t N0 = 0.0;

  // check if norm is a parameter or a function
  if (fRunInfo->GetNormParamNo() < MSR_PARAM_FUN_OFFSET) { // norm is a parameter
    N0 = par[fRunInfo->GetNormParamNo()-1];
  } else { // norm is a function
    // get function number
    UInt_t funNo = fRunInfo->GetNormParamNo()-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par);
  }

  // get tau
  Double_t tau;
  if (fRunInfo->GetLifetimeParamNo() != -1)
    tau = par[fRunInfo->GetLifetimeParamNo()-1];
  else
    tau = PMUON_LIFETIME;

  // get background
  Double_t bkg;
  if (fRunInfo->GetBkgFitParamNo() == -1) { // bkg not fitted
    if (fRunInfo->GetBkgFix(0) == PMUSR_UNDEFINED) { // no fixed background given (background interval)
      bkg = fBackground;
    } else { // fixed bkg given
      bkg = fRunInfo->GetBkgFix(0);
    }
  } else { // bkg fitted
    bkg = par[fRunInfo->GetBkgFitParamNo()-1];
  }

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    Int_t funcNo = fMsrInfo->GetFuncNo(i);
    fFuncValues[i] = fMsrInfo->EvalFunc(funcNo, *fRunInfo->GetMap(), par);
  }

  // calculate chi square
  Double_t time(1.0);
  Int_t i, N(static_cast<Int_t>(fData.GetValue()->size()));

  // In order not to have an IF in the next loop, determine the start and end bins for the fit range now
  Int_t startTimeBin = static_cast<Int_t>(ceil((fFitStartTime - fData.GetDataTimeStart())/fData.GetDataTimeStep()));
  if (startTimeBin < 0)
    startTimeBin = 0;
  Int_t endTimeBin = static_cast<Int_t>(floor((fFitEndTime - fData.GetDataTimeStart())/fData.GetDataTimeStep())) + 1;
  if (endTimeBin > N)
    endTimeBin = N;

  // Calculate the theory function once to ensure one function evaluation for the current set of parameters.
  // This is needed for the LF and user functions where some non-thread-save calculations only need to be calculated once
  // for a given set of parameters---which should be done outside of the parallelized loop.
  // For all other functions it means a tiny and acceptable overhead.
  time = fTheory->Func(time, par, fFuncValues);

  #ifdef HAVE_GOMP
  Int_t chunk = (endTimeBin - startTimeBin)/omp_get_num_procs();
  if (chunk < 10)
    chunk = 10;
  #pragma omp parallel for default(shared) private(i,time,diff) schedule(dynamic,chunk) reduction(+:chisq)
  #endif
  for (i=startTimeBin; i < endTimeBin; ++i) {
    time = fData.GetDataTimeStart() + (Double_t)i*fData.GetDataTimeStep();
    theo = N0*TMath::Exp(-time/tau)*(1.0+fTheory->Func(time, par, fFuncValues))+bkg;
    diff = fData.GetValue()->at(i) - theo;
    chisq += diff*diff / theo;
  }

  // the correction factor is need since the data scales like pack*t_res,
  // whereas the error scales like sqrt(pack*t_res)
  if (fScaleN0AndBkg)
    chisq *= fRunInfo->GetPacking() * (fTimeResolution * 1.0e3);

  return chisq;
}

//--------------------------------------------------------------------------
// CalcMaxLikelihood (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate log maximum-likelihood.
 *
 * <b>return:</b>
 * - log maximum-likelihood value
 *
 * \param par parameter vector iterated by minuit2
 */
Double_t PRunSingleHisto::CalcMaxLikelihood(const std::vector<Double_t>& par)
{
  Double_t mllh = 0.0; // maximum log likelihood assuming poisson distribution for the single bin

  Double_t N0;

  // check if norm is a parameter or a function
  if (fRunInfo->GetNormParamNo() < MSR_PARAM_FUN_OFFSET) { // norm is a parameter
    N0 = par[fRunInfo->GetNormParamNo()-1];
  } else { // norm is a function
    // get function number
    UInt_t funNo = fRunInfo->GetNormParamNo()-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par);
  }

  // get tau
  Double_t tau;
  if (fRunInfo->GetLifetimeParamNo() != -1)
    tau = par[fRunInfo->GetLifetimeParamNo()-1];
  else
    tau = PMUON_LIFETIME;

  // get background
  Double_t bkg;
  if (fRunInfo->GetBkgFitParamNo() == -1) { // bkg not fitted
    if (fRunInfo->GetBkgFix(0) == PMUSR_UNDEFINED) { // no fixed background given (background interval)
      bkg = fBackground;
    } else { // fixed bkg given
      bkg = fRunInfo->GetBkgFix(0);
    }
  } else { // bkg fitted
    bkg = par[fRunInfo->GetBkgFitParamNo()-1];
  }

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    Int_t funcNo = fMsrInfo->GetFuncNo(i);
    fFuncValues[i] = fMsrInfo->EvalFunc(funcNo, *fRunInfo->GetMap(), par);
  }

  // calculate maximum log likelihood
  Double_t theo;
  Double_t data;
  Double_t time(1.0);
  Int_t i, N(static_cast<Int_t>(fData.GetValue()->size()));

  // norm is needed since there is no simple scaling like in chisq case to get the correct Max.Log.Likelihood value when normlizing N(t) to 1/ns
  Double_t normalizer = 1.0;

  if (fScaleN0AndBkg)
    normalizer = fRunInfo->GetPacking() * (fTimeResolution * 1.0e3);

  // In order not to have an IF in the next loop, determine the start and end bins for the fit range now
  Int_t startTimeBin = static_cast<Int_t>(ceil((fFitStartTime - fData.GetDataTimeStart())/fData.GetDataTimeStep()));
  if (startTimeBin < 0)
    startTimeBin = 0;
  Int_t endTimeBin = static_cast<Int_t>(floor((fFitEndTime - fData.GetDataTimeStart())/fData.GetDataTimeStep())) + 1;
  if (endTimeBin > N)
    endTimeBin = N;

  // Calculate the theory function once to ensure one function evaluation for the current set of parameters.
  // This is needed for the LF and user functions where some non-thread-save calculations only need to be calculated once
  // for a given set of parameters---which should be done outside of the parallelized loop.
  // For all other functions it means a tiny and acceptable overhead.
  time = fTheory->Func(time, par, fFuncValues);

  #ifdef HAVE_GOMP
  Int_t chunk = (endTimeBin - startTimeBin)/omp_get_num_procs();
  if (chunk < 10)
    chunk = 10;
  #pragma omp parallel for default(shared) private(i,time,theo,data) schedule(dynamic,chunk) reduction(-:mllh)
  #endif
  for (i=startTimeBin; i < endTimeBin; ++i) {
    time = fData.GetDataTimeStart() + (Double_t)i*fData.GetDataTimeStep();
    // calculate theory for the given parameter set
    theo = N0*TMath::Exp(-time/tau)*(1+fTheory->Func(time, par, fFuncValues))+bkg;
    theo *= normalizer;
    // check if data value is not too small
    if (fData.GetValue()->at(i) > 1.0e-9)
      data = normalizer*fData.GetValue()->at(i);
    else
      data = 1.0e-9;
    // add maximum log likelihood contribution of bin i
    mllh -= data*TMath::Log(theo) - theo - TMath::LnGamma(data+1);
  }

  return mllh;
}

//--------------------------------------------------------------------------
// CalcTheory (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate theory for a given set of fit-parameters.
 */
void PRunSingleHisto::CalcTheory()
{
  // feed the parameter vector
  std::vector<Double_t> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (UInt_t i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // calculate asymmetry
  Double_t N0;
  // check if norm is a parameter or a function
  if (fRunInfo->GetNormParamNo() < MSR_PARAM_FUN_OFFSET) { // norm is a parameter
    N0 = par[fRunInfo->GetNormParamNo()-1];
  } else { // norm is a function
    // get function number
    UInt_t funNo = fRunInfo->GetNormParamNo()-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par);
  }

  // get tau
  Double_t tau;
  if (fRunInfo->GetLifetimeParamNo() != -1)
    tau = par[fRunInfo->GetLifetimeParamNo()-1];
  else
    tau = PMUON_LIFETIME;

  // get background
  Double_t bkg;
  if (fRunInfo->GetBkgFitParamNo() == -1) { // bkg not fitted
    if (fRunInfo->GetBkgFix(0) == PMUSR_UNDEFINED) { // no fixed background given (background interval)
      bkg = fBackground;
    } else { // fixed bkg given
      bkg = fRunInfo->GetBkgFix(0);
    }
  } else { // bkg fitted
    bkg = par[fRunInfo->GetBkgFitParamNo()-1];
  }

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par);
  }

  // calculate theory
  UInt_t size = fData.GetValue()->size();
  Double_t start = fData.GetDataTimeStart();
  Double_t resolution = fData.GetDataTimeStep();
  Double_t time;
  for (UInt_t i=0; i<size; i++) {
    time = start + (Double_t)i*resolution;
    fData.AppendTheoryValue(N0*TMath::Exp(-time/tau)*(1.0+fTheory->Func(time, par, fFuncValues))+bkg);
  }

  // clean up
  par.clear();
}

//--------------------------------------------------------------------------
// GetNoOfFitBins (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate the number of fitted bins for the current fit range.
 *
 * <b>return:</b> number of fitted bins.
 */
UInt_t PRunSingleHisto::GetNoOfFitBins()
{
  CalcNoOfFitBins();

  return fNoOfFitBins;
}

//--------------------------------------------------------------------------
// CalcNoOfFitBins (private)
//--------------------------------------------------------------------------
/**
 * <p>Calculate the number of fitted bins for the current fit range.
 */
void PRunSingleHisto::CalcNoOfFitBins()
{
  // In order not having to loop over all bins and to stay consistent with the chisq method, calculate the start and end bins explicitly
  Int_t startTimeBin = static_cast<Int_t>(ceil((fFitStartTime - fData.GetDataTimeStart())/fData.GetDataTimeStep()));
  if (startTimeBin < 0)
    startTimeBin = 0;
  Int_t endTimeBin = static_cast<Int_t>(floor((fFitEndTime - fData.GetDataTimeStart())/fData.GetDataTimeStep())) + 1;
  if (endTimeBin > static_cast<Int_t>(fData.GetValue()->size()))
    endTimeBin = fData.GetValue()->size();

  if (endTimeBin > startTimeBin)
    fNoOfFitBins = endTimeBin - startTimeBin;
  else
    fNoOfFitBins = 0;
}

//--------------------------------------------------------------------------
// PrepareData (private)
//--------------------------------------------------------------------------
/**
 * <p>Prepare data for fitting or viewing. What is already processed at this stage:
 * -# get proper raw run data
 * -# get all needed forward histograms
 * -# get time resolution
 * -# get t0's and perform necessary cross checks (e.g. if t0 of msr-file (if present) are consistent with t0 of the data files, etc.)
 * -# add runs (if addruns are present)
 * -# group histograms (if grouping is present)
 *
 * <b>return:</b>
 * - true if everthing went smooth
 * - false, otherwise.
 */
Bool_t PRunSingleHisto::PrepareData()
{
  Bool_t success = true;

  // get the proper run
  PRawRunData* runData = fRawData->GetRunData(*fRunInfo->GetRunName());
  if (!runData) { // couldn't get run
    cerr << endl << ">> PRunSingleHisto::PrepareData(): **ERROR** Couldn't get run " << fRunInfo->GetRunName()->Data() << "!";
    cerr << endl;
    return false;
  }

  // collect histogram numbers
  PUIntVector histoNo; // histoNo = msr-file forward + redGreen_offset - 1
  for (UInt_t i=0; i<fRunInfo->GetForwardHistoNoSize(); i++) {
    histoNo.push_back(fRunInfo->GetForwardHistoNo(i));

    if (!runData->IsPresent(histoNo[i])) {
      cerr << endl << ">> PRunSingleHisto::PrepareData(): **PANIC ERROR**:";
      cerr << endl << ">> histoNo found = " << histoNo[i] << ", which is NOT present in the data file!?!?";
      cerr << endl << ">> Will quit :-(";
      cerr << endl;
      histoNo.clear();
      return false;
    }
  }

  // feed all T0's
  // first init T0's, T0's are stored as (forward T0, backward T0, etc.)
  fT0s.clear();
  fT0s.resize(histoNo.size());
  for (UInt_t i=0; i<fT0s.size(); i++) {
    fT0s[i] = -1.0;
  }

  // fill in the T0's from the msr-file (if present)
  for (UInt_t i=0; i<fRunInfo->GetT0BinSize(); i++) {
    fT0s[i] = fRunInfo->GetT0Bin(i);
  }

  // fill in the T0's from the data file, if not already present in the msr-file
  for (UInt_t i=0; i<histoNo.size(); i++) {
    if (fT0s[i] == -1.0) // i.e. not present in the msr-file, try the data file
      if (runData->GetT0Bin(histoNo[i]) > 0.0) {
        fT0s[i] = runData->GetT0Bin(histoNo[i]);
        fRunInfo->SetT0Bin(fT0s[i], i); // keep value for the msr-file
      }
  }

  // fill in the T0's gaps, i.e. in case the T0's are NOT in the msr-file and NOT in the data file
  for (UInt_t i=0; i<histoNo.size(); i++) {
    if (fT0s[i] == -1.0) { // i.e. not present in the msr-file and data file, use the estimated T0
      fT0s[i] = runData->GetT0BinEstimated(histoNo[i]);
      fRunInfo->SetT0Bin(fT0s[i], i); // keep value for the msr-file

      cerr << endl << ">> PRunSingleHisto::PrepareData(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
      cerr << endl << ">> run: " << fRunInfo->GetRunName();
      cerr << endl << ">> will try the estimated one: forward t0 = " << runData->GetT0BinEstimated(histoNo[i]);
      cerr << endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
      cerr << endl;
    }
  }

  // check if t0 is within proper bounds
  for (UInt_t i=0; i<fRunInfo->GetForwardHistoNoSize(); i++) {
    if ((fT0s[i] < 0) || (fT0s[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << ">> PRunSingleHisto::PrepareData(): **ERROR** t0 data bin (" << fT0s[i] << ") doesn't make any sense!";
      cerr << endl;
      return false;
    }
  }

  // keep the histo of each group at this point (addruns handled below)
  vector<PDoubleVector> forward;
  forward.resize(histoNo.size());   // resize to number of groups
  for (UInt_t i=0; i<histoNo.size(); i++) {
    forward[i].resize(runData->GetDataBin(histoNo[i])->size());
    forward[i] = *runData->GetDataBin(histoNo[i]);
  }

  // check if there are runs to be added to the current one
  if (fRunInfo->GetRunNameSize() > 1) { // runs to be added present
    PRawRunData *addRunData;
    for (UInt_t i=1; i<fRunInfo->GetRunNameSize(); i++) {

      // get run to be added to the main one
      addRunData = fRawData->GetRunData(*fRunInfo->GetRunName(i));
      if (addRunData == 0) { // couldn't get run
        cerr << endl << ">> PRunSingleHisto::PrepareData(): **ERROR** Couldn't get addrun " << fRunInfo->GetRunName(i)->Data() << "!";
        cerr << endl;
        return false;
      }

      // feed all T0's
      // first init T0's, T0's are stored as (forward T0, backward T0, etc.)
      PDoubleVector t0Add;
      t0Add.resize(histoNo.size());
      for (UInt_t j=0; j<t0Add.size(); j++) {
        t0Add[j] = -1.0;
      }

      // fill in the T0's from the msr-file (if present)
      for (UInt_t j=0; j<fRunInfo->GetT0BinSize(); j++) {
        t0Add[j] = fRunInfo->GetAddT0Bin(i-1,j); // addRunIdx starts at 0
      }

      // fill in the T0's from the data file, if not already present in the msr-file
      for (UInt_t j=0; j<histoNo.size(); j++) {
        if (t0Add[j] == -1.0) // i.e. not present in the msr-file, try the data file
          if (addRunData->GetT0Bin(histoNo[j]) > 0.0) {
            t0Add[j] = addRunData->GetT0Bin(histoNo[j]);
            fRunInfo->SetAddT0Bin(t0Add[j], i-1, j); // keep value for the msr-file
          }
      }

      // fill in the T0's gaps, i.e. in case the T0's are NOT in the msr-file and NOT in the data file
      for (UInt_t j=0; j<histoNo.size(); j++) {
        if (t0Add[j] == -1.0) { // i.e. not present in the msr-file and data file, use the estimated T0
          t0Add[j] = addRunData->GetT0BinEstimated(histoNo[j]);
          fRunInfo->SetAddT0Bin(t0Add[j], i-1, j); // keep value for the msr-file

          cerr << endl << ">> PRunSingleHisto::PrepareData(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
          cerr << endl << ">> run: " << fRunInfo->GetRunName();
          cerr << endl << ">> will try the estimated one: forward t0 = " << addRunData->GetT0BinEstimated(histoNo[j]);
          cerr << endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
          cerr << endl;
        }
      }

      // check if t0 is within proper bounds
      for (UInt_t j=0; j<fRunInfo->GetForwardHistoNoSize(); j++) {
        if ((t0Add[j] < 0) || (t0Add[j] > (Int_t)addRunData->GetDataBin(histoNo[j])->size())) {
          cerr << endl << ">> PRunSingleHisto::PrepareData(): **ERROR** addt0 data bin (" << t0Add[j] << ") doesn't make any sense!";
          cerr << endl;
          return false;
        }
      }

      // add forward run
      UInt_t addRunSize;
      for (UInt_t k=0; k<histoNo.size(); k++) { // fill each group
        addRunSize = addRunData->GetDataBin(histoNo[k])->size();
        for (UInt_t j=0; j<addRunData->GetDataBin(histoNo[k])->size(); j++) { // loop over the bin indices
          // make sure that the index stays in the proper range
          if ((j+(Int_t)t0Add[k]-(Int_t)fT0s[k] >= 0) && (j+(Int_t)t0Add[k]-(Int_t)fT0s[k] < addRunSize)) {
            forward[k][j] += addRunData->GetDataBin(histoNo[k])->at(j+(Int_t)t0Add[k]-(Int_t)fT0s[k]);
          }
        }
      }

      // clean up
      t0Add.clear();
    }
  }

  // set forward/backward histo data of the first group
  fForward.resize(forward[0].size());
  for (UInt_t i=0; i<fForward.size(); i++) {
    fForward[i]  = forward[0][i];
  }

  // group histograms, add all the remaining forward histograms of the group
  for (UInt_t i=1; i<histoNo.size(); i++) { // loop over the groupings
    for (UInt_t j=0; j<runData->GetDataBin(histoNo[i])->size(); j++) { // loop over the bin indices
      // make sure that the index stays within proper range
      if ((j+fT0s[i]-fT0s[0] >= 0) && (j+fT0s[i]-fT0s[0] < runData->GetDataBin(histoNo[i])->size())) {
        fForward[j] += forward[i][j+(Int_t)fT0s[i]-(Int_t)fT0s[0]];
      }
    }
  }

  // keep the time resolution in (us)
  fTimeResolution = runData->GetTimeResolution()/1.0e3;
  cout.precision(10);
  cout << endl << ">> PRunSingleHisto::PrepareData(): time resolution=" << fixed << runData->GetTimeResolution() << "(ns)" << endl;

  if (fHandleTag == kFit)
    success = PrepareFitData(runData, histoNo[0]);
  else if ((fHandleTag == kView) && !fRunInfo->IsLifetimeCorrected())
    success = PrepareRawViewData(runData, histoNo[0]);
  else if ((fHandleTag == kView) && fRunInfo->IsLifetimeCorrected())
    success = PrepareViewData(runData, histoNo[0]);
  else
    success = false;

  // cleanup
  histoNo.clear();

  return success;
}

//--------------------------------------------------------------------------
// PrepareFitData (private)
//--------------------------------------------------------------------------
/**
 * <p>Take the pre-processed data (i.e. grouping and addrun are preformed) and form the histogram for fitting.
 * The following steps are preformed:
 * -# get fit start/stop time
 * -# check that 'first good data bin', 'last good data bin', and 't0' make any sense
 * -# check how the background shall be handled, i.e. fitted, subtracted from background estimate data range, or subtacted from a given fixed background.
 * -# packing (i.e rebinning)
 *
 * <b>return:</b>
 * - true, if everything went smooth
 * - false, otherwise
 *
 * \param runData raw run data handler
 * \param histoNo forward histogram number
 */
Bool_t PRunSingleHisto::PrepareFitData(PRawRunData* runData, const UInt_t histoNo)
{
  // transform raw histo data. This is done the following way (for details see the manual):
  // for the single histo fit, just the rebinned raw data are copied

  // first get start data, end data, and t0
  Int_t start;
  Int_t end;
  start = fRunInfo->GetDataRange(0);
  end   = fRunInfo->GetDataRange(1);
  // check if data range has been provided, and if not try to estimate them
  if (start < 0) {
    Int_t offset = (Int_t)(10.0e-3/fTimeResolution);
    start = (Int_t)fT0s[0]+offset;
    fRunInfo->SetDataRange(start, 0);
    cerr << endl << ">> PRunSingleHisto::PrepareData(): **WARNING** data range was not provided, will try data range start = t0+" << offset << "(=10ns) = " << start << ".";
    cerr << endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  }
  if (end < 0) {
    end = fForward.size();
    fRunInfo->SetDataRange(end, 1);
    cerr << endl << ">> PRunSingleHisto::PrepareData(): **WARNING** data range was not provided, will try data range end = " << end << ".";
    cerr << endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  }

  // check if start and end make any sense
  // 1st check if start and end are in proper order
  if (end < start) { // need to swap them
    Int_t keep = end;
    end = start;
    start = keep;
  }
  // 2nd check if start is within proper bounds
  if ((start < 0) || (start > (Int_t)fForward.size())) {
    cerr << endl << ">> PRunSingleHisto::PrepareFitData(): **ERROR** start data bin doesn't make any sense!";
    cerr << endl;
    return false;
  }
  // 3rd check if end is within proper bounds
  if ((end < 0) || (end > (Int_t)fForward.size())) {
    cerr << endl << ">> PRunSingleHisto::PrepareFitData(): **ERROR** end data bin doesn't make any sense!";
    cerr << endl;
    return false;
  }

  // check how the background shall be handled
  if (fRunInfo->GetBkgFitParamNo() == -1) { // bkg shall **NOT** be fitted
    // subtract background from histogramms ------------------------------------------
    if (fRunInfo->GetBkgFix(0) == PMUSR_UNDEFINED) { // no fixed background given
      if (fRunInfo->GetBkgRange(0) >= 0) {
        if (!EstimateBkg(histoNo))
          return false;
      } else { // no background given to do the job, try estimate
        fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.1), 0);
        fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.6), 1);
        cerr << endl << ">> PRunSingleHisto::PrepareFitData(): **WARNING** Neither fix background nor background bins are given!";
        cerr << endl << ">> Will try the following: bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
        cerr << endl << ">> NO WARRANTY THAT THIS MAKES ANY SENSE! Better check ...";
        cerr << endl;
        if (!EstimateBkg(histoNo))
          return false;
      }
    } else { // fixed background given
      for (UInt_t i=0; i<fForward.size(); i++) {
        fForward[i] -= fRunInfo->GetBkgFix(0);
      }
    }
  }

  // everything looks fine, hence fill data set
  Int_t t0 = (Int_t)fT0s[0];
  Double_t value = 0.0;
  Double_t normalizer = 1.0;
  // in order that after rebinning the fit does not need to be redone (important for plots)
  // the value is normalize to per 1 nsec if scaling is whished
  if (fScaleN0AndBkg)
    normalizer = fRunInfo->GetPacking() * (fTimeResolution * 1.0e3); // fTimeResolution us->ns
  // data start at data_start-t0
  // time shifted so that packing is included correctly, i.e. t0 == t0 after packing
  fData.SetDataTimeStart(fTimeResolution*((Double_t)start-(Double_t)t0+(Double_t)(fRunInfo->GetPacking()-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*fRunInfo->GetPacking());
  for (Int_t i=start; i<end; i++) {
    if (fRunInfo->GetPacking() == 1) {
      value = fForward[i];
      value /= normalizer;
      fData.AppendValue(value);
      if (value == 0.0)
        fData.AppendErrorValue(1.0/normalizer);
      else
        fData.AppendErrorValue(TMath::Sqrt(value));
    } else { // packed data, i.e. fRunInfo->GetPacking() > 1
      if (((i-start) % fRunInfo->GetPacking() == 0) && (i != start)) { // fill data
        value /= normalizer;
        fData.AppendValue(value);
        if (value == 0.0)
          fData.AppendErrorValue(1.0/normalizer);
        else
          fData.AppendErrorValue(TMath::Sqrt(value));
        // reset values
        value = 0.0;
      }
      value += fForward[i];
    }
  }

  CalcNoOfFitBins();

  return true;
}

//--------------------------------------------------------------------------
// PrepareRawViewData (private)
//--------------------------------------------------------------------------
/**
 * <p>Take the pre-processed data (i.e. grouping and addrun are preformed) and form the histogram for viewing
 * without any life time correction.
 * <p>The following steps are preformed:
 * -# check if view packing is whished.
 * -# check that 'first good data bin', 'last good data bin', and 't0' makes any sense
 * -# packing (i.e. rebinnig)
 * -# calculate theory
 *
 * <b>return:</b>
 * - true, if everything went smooth
 * - false, otherwise.
 *
 * \param runData raw run data handler
 * \param histoNo forward histogram number
 */
Bool_t PRunSingleHisto::PrepareRawViewData(PRawRunData* runData, const UInt_t histoNo)
{
  // check if view_packing is wished
  Int_t packing = fRunInfo->GetPacking();
  if (fMsrInfo->GetMsrPlotList()->at(0).fViewPacking > 0) {
    packing = fMsrInfo->GetMsrPlotList()->at(0).fViewPacking;
  }

  // calculate necessary norms
  Double_t dataNorm = 1.0, theoryNorm = 1.0;
  if (fScaleN0AndBkg) {
    dataNorm = 1.0/ (packing * (fTimeResolution * 1.0e3)); // fTimeResolution us->ns
  } else if (!fScaleN0AndBkg && (fMsrInfo->GetMsrPlotList()->at(0).fViewPacking > 0)) {
    theoryNorm = (Double_t)fMsrInfo->GetMsrPlotList()->at(0).fViewPacking/(Double_t)fRunInfo->GetPacking();
  }

  // raw data, since PMusrCanvas is doing ranging etc.
  // start = the first bin which is a multiple of packing backward from first good data bin
  Int_t start = fRunInfo->GetDataRange(0) - (fRunInfo->GetDataRange(0)/packing)*packing;
  // end = last bin starting from start which is a multipl of packing and still within the data 
  Int_t end   = start + ((fForward.size()-start)/packing)*packing;
  // check if data range has been provided, and if not try to estimate them
  if (start < 0) {
    Int_t offset = (Int_t)(10.0e-3/fTimeResolution);
    start = ((Int_t)fT0s[0]+offset) - (((Int_t)fT0s[0]+offset)/packing)*packing;
    end = start + ((fForward.size()-start)/packing)*packing;
    cerr << endl << ">> PRunSingleHisto::PrepareData(): **WARNING** data range was not provided, will try data range start = " << start << ".";
    cerr << endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  }
  // check if start, end, and t0 make any sense
  // 1st check if start and end are in proper order
  if (end < start) { // need to swap them
    Int_t keep = end;
    end = start;
    start = keep;
  }
  // 2nd check if start is within proper bounds
  if ((start < 0) || (start > (Int_t)fForward.size())) {
    cerr << endl << ">> PRunSingleHisto::PrepareRawViewData(): **ERROR** start data bin doesn't make any sense!";
    cerr << endl;
    return false;
  }
  // 3rd check if end is within proper bounds
  if ((end < 0) || (end > (Int_t)fForward.size())) {
    cerr << endl << ">> PRunSingleHisto::PrepareRawViewData(): **ERROR** end data bin doesn't make any sense!";
    cerr << endl;
    return false;
  }

  // everything looks fine, hence fill data set
  Int_t t0 = (Int_t)fT0s[0];
  Double_t value = 0.0;
  // data start at data_start-t0
  // time shifted so that packing is included correctly, i.e. t0 == t0 after packing
  fData.SetDataTimeStart(fTimeResolution*((Double_t)start-(Double_t)t0+(Double_t)(packing-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*packing);

  for (Int_t i=start; i<end; i++) {
    if (((i-start) % packing == 0) && (i != start)) { // fill data
      value *= dataNorm;
      fData.AppendValue(value);
      if (value == 0.0)
        fData.AppendErrorValue(1.0);
      else
        fData.AppendErrorValue(TMath::Sqrt(value*dataNorm));
      // reset values
      value = 0.0;
    }
    value += fForward[i];
  }

  CalcNoOfFitBins();

  // fill theory vector for kView
  // feed the parameter vector
  std::vector<Double_t> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (UInt_t i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // calculate asymmetry
  Double_t N0;
  // check if norm is a parameter or a function
  if (fRunInfo->GetNormParamNo() < MSR_PARAM_FUN_OFFSET) { // norm is a parameter
    N0 = par[fRunInfo->GetNormParamNo()-1];
  } else { // norm is a function
    // get function number
    UInt_t funNo = fRunInfo->GetNormParamNo()-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par);
  }
  N0 *= theoryNorm;

  // get tau
  Double_t tau;
  if (fRunInfo->GetLifetimeParamNo() != -1)
    tau = par[fRunInfo->GetLifetimeParamNo()-1];
  else
    tau = PMUON_LIFETIME;

  // get background
  Double_t bkg;
  if (fRunInfo->GetBkgFitParamNo() == -1) { // bkg not fitted
    if (fRunInfo->GetBkgFix(0) == PMUSR_UNDEFINED) { // no fixed background given (background interval)
      if (fRunInfo->GetBkgRange(0) >= 0) { // background range given
        if (!EstimateBkg(histoNo))
          return false;
      } else { // no background given to do the job, try estimate
        fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.1), 0);
        fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.6), 1);
        cerr << endl << ">> PRunSingleHisto::PrepareData(): **WARNING** Neither fix background nor background bins are given!";
        cerr << endl << ">> Will try the following: bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
        cerr << endl << ">> NO WARRANTY THAT THIS MAKES ANY SENSE! Better check ...";
        cerr << endl;
        if (!EstimateBkg(histoNo))
          return false;
      }
      bkg = fBackground;
    } else { // fixed bkg given
      bkg = fRunInfo->GetBkgFix(0);
    }
  } else { // bkg fitted
    bkg = par[fRunInfo->GetBkgFitParamNo()-1];
  }
  bkg *= theoryNorm;

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par);
  }

  // calculate theory
  UInt_t size = fForward.size();
  Double_t factor = 1.0;
  if (fData.GetValue()->size() * 10 > fForward.size()) {
    size = fData.GetValue()->size() * 10;
    factor = (Double_t)fForward.size() / (Double_t)size;
  }
  Double_t time;
  Double_t theoryValue;
  fData.SetTheoryTimeStart(fData.GetDataTimeStart());
  fData.SetTheoryTimeStep(fTimeResolution*factor);
  for (UInt_t i=0; i<size; i++) {
    time = fData.GetTheoryTimeStart() + i*fData.GetTheoryTimeStep();
    theoryValue = fTheory->Func(time, par, fFuncValues);
    if (fabs(theoryValue) > 1.0e10) {  // dirty hack needs to be fixed!!
      theoryValue = 0.0;
    }
    fData.AppendTheoryValue(N0*TMath::Exp(-time/tau)*(1.0+theoryValue)+bkg);
  }

  // clean up
  par.clear();

  return true;
}

//--------------------------------------------------------------------------
// PrepareViewData (private)
//--------------------------------------------------------------------------
/**
 * <p>Take the pre-processed data (i.e. grouping and addrun are preformed) and form the histogram for viewing
 * with life time correction, i.e. the exponential decay is removed.
 * <p>The following steps are preformed:
 * -# check if view packing is whished.
 * -# check that 'first good data bin', 'last good data bin', and 't0' makes any sense
 * -# transform data sets (see below).
 * -# calculate theory
 *
 * <p> Muon life time corrected data: Starting from
 * \f[ N(t) = N_0 e^{-t/\tau} [ 1 + A(t) ] + \mathrm{Bkg} \f]
 * it follows that
 * \f[ A(t) = (-1) + e^{+t/\tau}\, \frac{N(t)-\mathrm{Bkg}}{N_0}. \f]
 * For the error estimate only the statistical error of \f$ N(t) \f$ is used, and hence
 * \f[ \Delta A(t) = \frac{e^{t/\tau}}{N_0}\,\sqrt{\frac{N(t)}{p}} \f]
 * where \f$ p \f$ is the packing, and \f$ N(t) \f$ are the packed data, i.e.
 * \f[ N(t_i) = \frac{1}{p}\, \sum_{j=i}^{i+p} n_j \f]
 * with \f$ n_j \f$ the raw histogram data bins.
 *
 * <b>return:</b>
 * - true, if everything went smooth
 * - false, otherwise
 *
 * \param runData raw run data handler
 * \param histoNo forward histogram number
 */
Bool_t PRunSingleHisto::PrepareViewData(PRawRunData* runData, const UInt_t histoNo)
{
  // check if view_packing is wished. This is a global option for all PLOT blocks!
  Int_t packing = fRunInfo->GetPacking();
  if (fMsrInfo->GetMsrPlotList()->at(0).fViewPacking > 0) {
    packing = fMsrInfo->GetMsrPlotList()->at(0).fViewPacking;
  }
  // check if rrf_packing is present. This is a global option for all PLOT blocks, since operated on a single set of data.
  if (fMsrInfo->GetMsrPlotList()->at(0).fRRFPacking > 0) {
    packing = fMsrInfo->GetMsrPlotList()->at(0).fRRFPacking;
  }

  // calculate necessary norms
  Double_t dataNorm = 1.0, theoryNorm = 1.0;
  if (fScaleN0AndBkg) {
    dataNorm = 1.0/ (packing * (fTimeResolution * 1.0e3)); // fTimeResolution us->ns
  } else if (!fScaleN0AndBkg && (fMsrInfo->GetMsrPlotList()->at(0).fViewPacking > 0)) {
    theoryNorm = (Double_t)fMsrInfo->GetMsrPlotList()->at(0).fViewPacking/(Double_t)fRunInfo->GetPacking();
  }

  // transform raw histo data. This is done the following way (for details see the manual):
  // for the single histo fit, just the rebinned raw data are copied
  // first get start data, end data, and t0
  Int_t t0 = (Int_t)fT0s[0];

  // start = the first bin which is a multiple of packing backward from first good data bin
  Int_t start = fRunInfo->GetDataRange(0) - (fRunInfo->GetDataRange(0)/packing)*packing;
  // end = last bin starting from start which is a multiple of packing and still within the data
  Int_t end   = start + ((fForward.size()-start)/packing)*packing;

  // check if data range has been provided, and if not try to estimate them
  if (start < 0) {
    Int_t offset = (Int_t)(10.0e-3/fTimeResolution);
    start = ((Int_t)fT0s[0]+offset) - (((Int_t)fT0s[0]+offset)/packing)*packing;
    end = start + ((fForward.size()-start)/packing)*packing;
    cerr << endl << ">> PRunSingleHisto::PrepareData(): **WARNING** data range was not provided, will try data range start = " << start << ".";
    cerr << endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  }

  // check if start, end, and t0 make any sense
  // 1st check if start and end are in proper order
  if (end < start) { // need to swap them
    Int_t keep = end;
    end = start;
    start = keep;
  }
  // 2nd check if start is within proper bounds
  if ((start < 0) || (start > (Int_t)fForward.size())) {
    cerr << endl << ">> PRunSingleHisto::PrepareViewData(): **ERROR** start data bin doesn't make any sense!";
    cerr << endl;
    return false;
  }
  // 3rd check if end is within proper bounds
  if ((end < 0) || (end > (Int_t)fForward.size())) {
    cerr << endl << ">> PRunSingleHisto::PrepareViewData(): **ERROR** end data bin doesn't make any sense!";
    cerr << endl;
    return false;
  }

  // everything looks fine, hence fill data set

  // feed the parameter vector
  std::vector<Double_t> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (UInt_t i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // calculate asymmetry
  Double_t N0;
  // check if norm is a parameter or a function
  if (fRunInfo->GetNormParamNo() < MSR_PARAM_FUN_OFFSET) { // norm is a parameter
    N0 = par[fRunInfo->GetNormParamNo()-1];
  } else { // norm is a function
    // get function number
    UInt_t funNo = fRunInfo->GetNormParamNo()-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par);
  }
  N0 *= theoryNorm;

  // get tau
  Double_t tau;
  if (fRunInfo->GetLifetimeParamNo() != -1)
    tau = par[fRunInfo->GetLifetimeParamNo()-1];
  else
    tau = PMUON_LIFETIME;

  // get background
  Double_t bkg;
  if (fRunInfo->GetBkgFitParamNo() == -1) { // bkg not fitted
    if (fRunInfo->GetBkgFix(0) == PMUSR_UNDEFINED) { // no fixed background given (background interval)
      if (fRunInfo->GetBkgRange(0) >= 0) { // background range given
        if (!EstimateBkg(histoNo))
          return false;
      } else { // no background given to do the job, try estimate
        fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.1), 0);
        fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.6), 1);
        cerr << endl << ">> PRunSingleHisto::PrepareData(): **WARNING** Neither fix background nor background bins are given!";
        cerr << endl << ">> Will try the following: bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
        cerr << endl << ">> NO WARRANTY THAT THIS MAKES ANY SENSE! Better check ...";
        cerr << endl;
        if (!EstimateBkg(histoNo))
          return false;
      }
      bkg = fBackground;
    } else { // fixed bkg given
      bkg = fRunInfo->GetBkgFix(0);
    }
  } else { // bkg fitted
    bkg = par[fRunInfo->GetBkgFitParamNo()-1];
  }
  bkg *= theoryNorm;

  Double_t value = 0.0;
  Double_t expval = 0.0;
  Double_t rrf_val = 0.0;
  Double_t time = 0.0;

  // data start at data_start-t0 shifted by (pack-1)/2
  fData.SetDataTimeStart(fTimeResolution*((Double_t)start-(Double_t)t0+(Double_t)(packing-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*packing);

  Double_t gammaRRF = 0.0, wRRF = 0.0, phaseRRF = 0.0;
  if (fMsrInfo->GetMsrPlotList()->at(0).fRRFFreq == 0.0) { // normal Data representation
    for (Int_t i=start; i<end; i++) {
      if (((i-start) % packing == 0) && (i != start)) { // fill data
        value *= dataNorm;
        time = (((Double_t)i-(Double_t)(packing-1)/2.0)-t0)*fTimeResolution;
        expval = TMath::Exp(+time/tau)/N0;
        fData.AppendValue(-1.0+expval*(value-bkg));
        fData.AppendErrorValue(expval*TMath::Sqrt(value*dataNorm));
        value = 0.0;
      }
      value += fForward[i];
    }
  } else { // RRF representation
    // check which units shall be used
    switch (fMsrInfo->GetMsrPlotList()->at(0).fRRFUnit) {
      case RRF_UNIT_kHz:
        gammaRRF = TMath::TwoPi()*1.0e-3;
        break;
      case RRF_UNIT_MHz:
        gammaRRF = TMath::TwoPi();
        break;
      case RRF_UNIT_Mcs:
        gammaRRF = 1.0;
        break;
      case RRF_UNIT_G:
        gammaRRF = 0.0135538817*TMath::TwoPi();
        break;
      case RRF_UNIT_T:
        gammaRRF = 0.0135538817*TMath::TwoPi()*1.0e4;
        break;
      default:
        gammaRRF = TMath::TwoPi();
        break;
    }
    wRRF = gammaRRF * fMsrInfo->GetMsrPlotList()->at(0).fRRFFreq;
    phaseRRF = fMsrInfo->GetMsrPlotList()->at(0).fRRFPhase / 180.0 * TMath::Pi();

    Double_t error = 0.0;
    for (Int_t i=start; i<end; i++) {
      if (((i-start) % packing == 0) && (i != start)) { // fill data
        fData.AppendValue(2.0*value/packing); // factor 2 needed because cos(a)cos(b) = 1/2(cos(a+b)+cos(a-b))
        fData.AppendErrorValue(expval*TMath::Sqrt(error/packing));
        value = 0.0;
        error = 0.0;
      }
      time = ((Double_t)i-t0)*fTimeResolution;
      expval = TMath::Exp(+time/tau)/N0;
      rrf_val = (-1.0+expval*(fForward[i]*dataNorm-bkg))*TMath::Cos(wRRF * time + phaseRRF);
      value += rrf_val;
      error += fForward[i]*dataNorm;
    }
  }

  CalcNoOfFitBins();

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par);
  }

  // calculate theory
  Double_t theoryValue;
  UInt_t size = fForward.size();
  Double_t factor = 1.0;
  UInt_t rebinRRF = 0;

  if (wRRF == 0) { // no RRF
    // check if a finer binning for the theory is needed
    if (fData.GetValue()->size() * 10 > fForward.size()) {
      size = fData.GetValue()->size() * 10;
      factor = (Double_t)fForward.size() / (Double_t)size;
    }
    fData.SetTheoryTimeStart(fData.GetDataTimeStart());
    fData.SetTheoryTimeStep(fTimeResolution*factor);
  } else { // RRF
    rebinRRF = static_cast<UInt_t>((TMath::Pi()/2.0/wRRF)/fTimeResolution); // RRF time resolution / data time resolution
    fData.SetTheoryTimeStart(fData.GetDataTimeStart());
    fData.SetTheoryTimeStep(TMath::Pi()/2.0/wRRF/rebinRRF); // = theory time resolution as close as possible to the data time resolution compatible with wRRF
  }

  for (UInt_t i=0; i<size; i++) {
    time = fData.GetTheoryTimeStart() + (Double_t)i*fData.GetTheoryTimeStep();
    theoryValue = fTheory->Func(time, par, fFuncValues);
    if (wRRF != 0.0) {
      theoryValue *= 2.0*TMath::Cos(wRRF * time + phaseRRF);
    }
    if (fabs(theoryValue) > 10.0) { // dirty hack needs to be fixed!!
      theoryValue = 0.0;
    }
    fData.AppendTheoryValue(theoryValue);
  }

  // if RRF filter the theory with a FIR Kaiser low pass filter
  if (wRRF != 0.0) {
    // rebin theory to the RRF frequency
    if (rebinRRF != 0) {
      Double_t dval = 0.0;
      PDoubleVector theo;
      for (UInt_t i=0; i<fData.GetTheory()->size(); i++) {
        if ((i % rebinRRF == 0) && (i != 0)) {
          theo.push_back(dval/rebinRRF);
          dval = 0.0;
        }
        dval += fData.GetTheory()->at(i);
      }
      fData.SetTheoryTimeStart(fData.GetTheoryTimeStart()+static_cast<Double_t>(rebinRRF-1)*fData.GetTheoryTimeStep()/2.0);
      fData.SetTheoryTimeStep(rebinRRF*fData.GetTheoryTimeStep());
      fData.ReplaceTheory(theo);
      theo.clear();
    }

    // filter theory
    CalculateKaiserFilterCoeff(wRRF, 60.0, 0.2); // w_c = wRRF, A = -20 log_10(delta), Delta w / w_c = (w_s - w_p) / (2 w_c)
    FilterTheo();
  }

  // clean up
  par.clear();

  return true;
}

//--------------------------------------------------------------------------
// EstimatBkg (private)
//--------------------------------------------------------------------------
/**
 * <p>Estimate the background for a given interval.
 *
 * <b>return:</b>
 * - true, if everything went smooth
 * - false, otherwise
 *
 * \param histoNo forward histogram number of the run
 */
Bool_t PRunSingleHisto::EstimateBkg(UInt_t histoNo)
{
  Double_t beamPeriod = 0.0;

  // check if data are from PSI, RAL, or TRIUMF
  if (fRunInfo->GetInstitute()->Contains("psi"))
    beamPeriod = ACCEL_PERIOD_PSI;
  else if (fRunInfo->GetInstitute()->Contains("ral"))
    beamPeriod = ACCEL_PERIOD_RAL;
  else if (fRunInfo->GetInstitute()->Contains("triumf"))
    beamPeriod = ACCEL_PERIOD_TRIUMF;
  else
    beamPeriod = 0.0;

  // check if start and end are in proper order
  UInt_t start = fRunInfo->GetBkgRange(0);
  UInt_t end   = fRunInfo->GetBkgRange(1);
  if (end < start) {
    cout << endl << "PRunSingleHisto::EstimatBkg(): end = " << end << " > start = " << start << "! Will swap them!";
    UInt_t keep = end;
    end = start;
    start = keep;
  }

  // calculate proper background range
  if (beamPeriod != 0.0) {
    Double_t timeBkg = (Double_t)(end-start)*(fTimeResolution*fRunInfo->GetPacking()); // length of the background intervall in time
    UInt_t fullCycles = (UInt_t)(timeBkg/beamPeriod); // how many proton beam cylces can be placed within the proposed background intervall
    // correct the end of the background intervall such that the background is as close as possible to a multiple of the proton cylce
    end = start + (UInt_t) ((fullCycles*beamPeriod)/(fTimeResolution*fRunInfo->GetPacking()));
    cout << endl << "PRunSingleHisto::EstimatBkg(): Background " << start << ", " << end;
    if (end == start)
      end = fRunInfo->GetBkgRange(1);
  }

  // check if start is within histogram bounds
  if ((start < 0) || (start >= fForward.size())) {
    cerr << endl << ">> PRunSingleHisto::EstimatBkg(): **ERROR** background bin values out of bound!";
    cerr << endl << ">> histo lengths    = " << fForward.size();
    cerr << endl << ">> background start = " << start;
    cerr << endl;
    return false;
  }

  // check if end is within histogram bounds
  if ((end < 0) || (end >= fForward.size())) {
    cerr << endl << ">> PRunSingleHisto::EstimatBkg(): **ERROR** background bin values out of bound!";
    cerr << endl << ">> histo lengths  = " << fForward.size();
    cerr << endl << ">> background end = " << end;
    cerr << endl;
    return false;
  }

  // calculate background
  Double_t bkg    = 0.0;

  // forward
  for (UInt_t i=start; i<end; i++)
    bkg += fForward[i];
  bkg /= static_cast<Double_t>(end - start + 1);

  if (fScaleN0AndBkg)
    fBackground = bkg / (fTimeResolution * 1e3); // keep background (per 1 nsec) for chisq, max.log.likelihood, fTimeResolution us->ns
  else
    fBackground = bkg * fRunInfo->GetPacking();  // keep background (per bin)

  fRunInfo->SetBkgEstimated(fBackground, 0);

  return true;
}

//--------------------------------------------------------------------------
// IsScaleN0AndBkg (private)
//--------------------------------------------------------------------------
/**
 * <p>Checks if N0/Bkg normalization to 1/ns is whished. The default is yes, since most of the users want to have it that way.
 * To overwrite this, one should add the line 'SCALE_N0_BKG FALSE' to the command block of the msr-file.
 *
 * <b>return:</b>
 * - true, if scaling of N0 and Bkg to 1/ns is whished
 * - false, otherwise
 *
 * \param histoNo forward histogram number of the run
 */
Bool_t PRunSingleHisto::IsScaleN0AndBkg()
{
  Bool_t willScale = true;

  PMsrLines *cmd = fMsrInfo->GetMsrCommands();
  for (UInt_t i=0; i<cmd->size(); i++) {
    if (cmd->at(i).fLine.Contains("SCALE_N0_BKG", TString::kIgnoreCase)) {
      TObjArray *tokens = 0;
      TObjString *ostr = 0;
      TString str;
      tokens = cmd->at(i).fLine.Tokenize(" \t");
      if (tokens->GetEntries() != 2) {
        cerr << endl << ">> PRunSingleHisto::IsScaleN0AndBkg(): **WARNING** Found uncorrect 'SCALE_N0_BKG' command, will ignore it.";
        cerr << endl << ">> Allowed commands: SCALE_N0_BKG TRUE | FALSE" << endl;
        return willScale;
      }
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      str = ostr->GetString();
      if (!str.CompareTo("FALSE", TString::kIgnoreCase)) {
        willScale = false;
      }
      // clean up
      if (tokens)
        delete tokens;
    }
  }

  return willScale;
}
