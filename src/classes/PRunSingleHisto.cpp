/***************************************************************************

  PRunSingleHisto.cpp

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

#include "PMusr.h"
#include "PRunSingleHisto.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
PRunSingleHisto::PRunSingleHisto() : PRunBase()
{
  fFitStartTime = 0.0;
  fFitStopTime  = 0.0;
  fNoOfFitBins  = 0;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param msrInfo pointer to the msr info structure
 * \param runNo number of the run of the msr-file
 */
PRunSingleHisto::PRunSingleHisto(PMsrHandler *msrInfo, PRunDataHandler *rawData, unsigned int runNo, EPMusrHandleTag tag) : PRunBase(msrInfo, rawData, runNo, tag)
{
  if (!PrepareData()) {
    cout << endl << "**SEVERE ERROR**: PRunSingleHisto::PRunSingleHisto: Couldn't prepare data for fitting!";
    cout << endl << "  This is very bad :-(, will quit ...";
    fValid = false;
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
PRunSingleHisto::~PRunSingleHisto()
{
}

//--------------------------------------------------------------------------
// CalcChiSquare
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * The return value is chisq * fRunInfo->fPacking, the reason is:
 * the data d_i and the theory t_i are scaled by the packing, i.e. d_i -> d_i / packing.
 * Since the error is 1/sqrt(d_i) and hence error^2 = d_i it follows that
 * (d_i - t_i)^2 ~ 1/packing^2 and error^2 ~ 1/packing, and hence the chisq needs
 * to be scaled by packing.
 *
 * \param par parameter vector iterated by minuit
 */
double PRunSingleHisto::CalcChiSquare(const std::vector<double>& par)
{
  double chisq     = 0.0;
  double diff      = 0.0;

  double N0;

  // check if norm is a parameter or a function
  if (fRunInfo->fNormParamNo < MSR_PARAM_FUN_OFFSET) { // norm is a parameter
    N0 = par[fRunInfo->fNormParamNo-1];
  } else { // norm is a function
    // get function number
    unsigned int funNo = fRunInfo->fNormParamNo-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo,fRunInfo->fMap,par);
  }

  // get tau
  double tau;
  if (fRunInfo->fLifetimeParamNo != -1)
    tau = par[fRunInfo->fLifetimeParamNo-1];
  else
    tau = PMUON_LIFETIME;

  // get background
  double bkg;
  if (fRunInfo->fBkgFitParamNo == -1) { // bkg not fitted
    if (isnan(fRunInfo->fBkgFix[0])) { // no fixed background given (background interval)
      bkg = fBackground;
    } else { // fixed bkg given
      bkg = fRunInfo->fBkgFix[0];
    }
  } else { // bkg fitted
    bkg = par[fRunInfo->fBkgFitParamNo-1];
  }

  // calculate functions
  for (int i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    int funcNo = fMsrInfo->GetFuncNo(i);
//cout << ">> i = " << i << ", funcNo = " << funcNo << endl;
    fFuncValues[i] = fMsrInfo->EvalFunc(funcNo, fRunInfo->fMap, par);
  }

  // calculate chi square
  double time;
  for (unsigned int i=0; i<fData.fValue.size(); i++) {
    time = fData.fDataTimeStart + (double)i*fData.fDataTimeStep;
    if ((time>=fFitStartTime) && (time<=fFitStopTime)) {
      diff = fData.fValue[i] -
            (N0*TMath::Exp(-time/tau)*(1+fTheory->Func(time, par, fFuncValues))+bkg);
      chisq += diff*diff / (fData.fError[i]*fData.fError[i]);
    }
  }

//cout << endl << "chisq=" << chisq*fRunInfo->fPacking;

  return chisq;
}

//--------------------------------------------------------------------------
// CalcMaxLikelihood
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param par parameter vector iterated by minuit
 */
double PRunSingleHisto::CalcMaxLikelihood(const std::vector<double>& par)
{
  double mllh      = 0.0; // maximum log likelihood assuming poisson distribution for the single bin

  double N0;

  // check if norm is a parameter or a function
  if (fRunInfo->fNormParamNo < MSR_PARAM_FUN_OFFSET) { // norm is a parameter
    N0 = par[fRunInfo->fNormParamNo-1];
  } else { // norm is a function
    // get function number
    unsigned int funNo = fRunInfo->fNormParamNo-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo,fRunInfo->fMap,par);
  }

  // get tau
  double tau;
  if (fRunInfo->fLifetimeParamNo != -1)
    tau = par[fRunInfo->fLifetimeParamNo-1];
  else
    tau = PMUON_LIFETIME;

  // get background
  double bkg;
  if (fRunInfo->fBkgFitParamNo == -1) { // bkg not fitted
    if (isnan(fRunInfo->fBkgFix[0])) { // no fixed background given (background interval)
      bkg = fBackground;
    } else { // fixed bkg given
      bkg = fRunInfo->fBkgFix[0];
    }
  } else { // bkg fitted
    bkg = par[fRunInfo->fBkgFitParamNo-1];
  }

  // calculate functions
  for (int i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    int funcNo = fMsrInfo->GetFuncNo(i);
    fFuncValues[i] = fMsrInfo->EvalFunc(funcNo, fRunInfo->fMap, par);
  }

  // calculate maximum log likelihood
  double theo;
  double data;
  double time;
  for (unsigned int i=0; i<fData.fValue.size(); i++) {
    time = fData.fDataTimeStart + (double)i*fData.fDataTimeStep;
    if ((time>=fFitStartTime) && (time<=fFitStopTime)) {
      // calculate theory for the given parameter set
      theo = N0*TMath::Exp(-time/tau)*(1+fTheory->Func(time, par, fFuncValues))+bkg;
      // check if data value is not too small
      if (fData.fValue[i] > 1.0e-9)
        data = fData.fValue[i];
      else
        data = 1.0e-9;
      // add maximum log likelihood contribution of bin i
      mllh -= data*TMath::Log(theo) - theo - TMath::LnGamma(data+1);
    }
  }

  return mllh;
}

//--------------------------------------------------------------------------
// CalcTheory
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PRunSingleHisto::CalcTheory()
{
  // feed the parameter vector
  std::vector<double> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (unsigned int i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // calculate asymmetry
  double N0;
  // check if norm is a parameter or a function
  if (fRunInfo->fNormParamNo < MSR_PARAM_FUN_OFFSET) { // norm is a parameter
    N0 = par[fRunInfo->fNormParamNo-1];
  } else { // norm is a function
    // get function number
    unsigned int funNo = fRunInfo->fNormParamNo-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo,fRunInfo->fMap,par);
  }

  // get tau
  double tau;
  if (fRunInfo->fLifetimeParamNo != -1)
    tau = par[fRunInfo->fLifetimeParamNo-1];
  else
    tau = PMUON_LIFETIME;

  // get background
  double bkg;
  if (fRunInfo->fBkgFitParamNo == -1) { // bkg not fitted
    if (isnan(fRunInfo->fBkgFix[0])) { // no fixed background given (background interval)
      bkg = fBackground;
    } else { // fixed bkg given
      bkg = fRunInfo->fBkgFix[0];
    }
  } else { // bkg fitted
    bkg = par[fRunInfo->fBkgFitParamNo-1];
  }

  // calculate functions
  for (int i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), fRunInfo->fMap, par);
  }

  // calculate theory
  unsigned int size  = fData.fValue.size();
  double start       = fData.fDataTimeStart;
  double resolution  = fData.fDataTimeStep;
  double time;
  for (unsigned int i=0; i<size; i++) {
    time = start + (double)i*resolution;
    fData.fTheory.push_back(N0*TMath::Exp(-time/tau)*(1+fTheory->Func(time, par, fFuncValues))+bkg);
  }

  // clean up
  par.clear();
}

//--------------------------------------------------------------------------
// PrepareData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunSingleHisto::PrepareData()
{
//  cout << endl << "in PRunSingleHisto::PrepareData(): will feed fData";
  bool success = true;

  if (fHandleTag == kFit)
    success = PrepareFitData();
  else if ((fHandleTag == kView) && !fRunInfo->fLifetimeCorrection)
    success = PrepareRawViewData();
  else if ((fHandleTag == kView) && fRunInfo->fLifetimeCorrection)
    success = PrepareViewData();
  else
    success = false;

  return success;
}

//--------------------------------------------------------------------------
// PrepareFitData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunSingleHisto::PrepareFitData()
{
  // get the proper run
  PRawRunData* runData = fRawData->GetRunData(fRunInfo->fRunName);
  if (!runData) { // couldn't get run
    cout << endl << "PRunSingleHisto::PrepareFitData(): **ERROR** Couldn't get run " << fRunInfo->fRunName.Data() << "!";
    return false;
  }

  // keep the time resolution in (us)
  fTimeResolution = runData->fTimeResolution/1.0e3;

  // keep start/stop time for fit
  fFitStartTime = fRunInfo->fFitRange[0];
  fFitStopTime  = fRunInfo->fFitRange[1];
//cout << endl << "start/stop (fit): " << fFitStartTime << ", " << fFitStopTime;


  // check if the t0's are given in the msr-file
  if (fRunInfo->fT0[0] == -1) { // t0's are NOT in the msr-file
    // check if the t0's are in the data file
    if (runData->fT0s.size() != 0) { // t0's in the run data
      // keep the proper t0's. For single histo runs, forward is holding the histo no
      // fForwardHistoNo starts with 1 not with 0 ;-)
      fT0s.push_back(runData->fT0s[fRunInfo->fForwardHistoNo-1]);
    } else { // t0's are neither in the run data nor in the msr-file -> not acceptable!
      cout << endl << "PRunSingleHisto::PrepareData(): **ERROR** NO t0's found, neither in the run data nor in the msr-file!";
      return false;
    }
  } else { // t0's in the msr-file
    // check if t0's are given in the data file
    if (runData->fT0s.size() != 0) {
      // compare t0's of the msr-file with the one in the data file
      if (fabs(fRunInfo->fT0[0]-runData->fT0s[fRunInfo->fForwardHistoNo-1])>5.0) { // given in bins!!
        cout << endl << "PRunSingleHisto::PrepareFitData(): **WARNING**:";
        cout << endl << "  t0 from the msr-file is  " << fRunInfo->fT0[0];
        cout << endl << "  t0 from the data file is " << runData->fT0s[fRunInfo->fForwardHistoNo-1];
        cout << endl << "  This is quite a deviation! Is this done intentionally??";
        cout << endl;
      }
    }
    fT0s.push_back(fRunInfo->fT0[0]);
  }

  // check if post pile up data shall be used
  unsigned int histoNo;
  if (fRunInfo->fFileFormat.Contains("ppc")) {
    histoNo = runData->fDataBin.size()/2 + fRunInfo->fForwardHistoNo-1;
  } else {
    histoNo = fRunInfo->fForwardHistoNo-1;
  }

  if ((runData->fDataBin.size() < histoNo) || (histoNo < 0)) {
    cout << endl << "PRunSingleHisto::PrepareFitData(): **PANIC ERROR**:";
    cout << endl << "   histoNo found = " << histoNo << ", but there are only " << runData->fDataBin.size() << " runs!?!?";
    cout << endl << "   Will quite :-(";
    cout << endl;
    return false;
  }

  // transform raw histo data. This is done the following way (for details see the manual):
  // for the single histo fit, just the rebinned raw data are copied
  // first get start data, end data, and t0
  unsigned int start;
  unsigned int end;
  double       t0    = fT0s[0];
  start = fRunInfo->fDataRange[0];
  end   = fRunInfo->fDataRange[1];
  // check if start, end, and t0 make any sense
  // 1st check if start and end are in proper order
  if (end < start) { // need to swap them
    int keep = end;
    end = start;
    start = keep;
  }
  // 2nd check if start is within proper bounds
  if ((start < 0) || (start > runData->fDataBin[histoNo].size())) {
    cout << endl << "PRunSingleHisto::PrepareFitData(): **ERROR** start data bin doesn't make any sense!";
    return false;
  }
  // 3rd check if end is within proper bounds
  if ((end < 0) || (end > runData->fDataBin[histoNo].size())) {
    cout << endl << "PRunSingleHisto::PrepareFitData(): **ERROR** end data bin doesn't make any sense!";
    return false;
  }
  // 4th check if t0 is within proper bounds
  if ((t0 < 0) || (t0 > runData->fDataBin[histoNo].size())) {
    cout << endl << "PRunSingleHisto::PrepareFitData(): **ERROR** t0 data bin doesn't make any sense!";
    return false;
  }

  // check how the background shall be handled
  if (fRunInfo->fBkgFitParamNo == -1) { // bkg shall **NOT** be fitted
    // subtract background from histogramms ------------------------------------------
    if (isnan(fRunInfo->fBkgFix[0])) { // no fixed background given
      if (fRunInfo->fBkgRange[0] != 0) {
        if (!EstimateBkg(histoNo))
          return false;
      } else { // no background given to do the job
        cout << endl << "PRunSingleHisto::PrepareData(): Neither fix background nor background bins are given!";
        cout << endl << "One of the two is needed! Will quit ...";
        return false;
      }
    }
  }

  // everything looks fine, hence fill data set
  double value = 0.0;
  // data start at data_start-t0
  // time shifted so that packing is included correctly, i.e. t0 == t0 after packing
  fData.fDataTimeStart = fTimeResolution*(((double)start-t0)+(double)fRunInfo->fPacking/2.0);
  fData.fDataTimeStep  = fTimeResolution*fRunInfo->fPacking;
  for (unsigned i=start; i<end; i++) {
    if (((i-start) % fRunInfo->fPacking == 0) && (i != start)) { // fill data
      // in order that after rebinning the fit does not need to be redone (important for plots)
      // the value is normalize to per bin
      value /= fRunInfo->fPacking;
      fData.fValue.push_back(value);
      if (value == 0.0)
        fData.fError.push_back(1.0);
      else
        fData.fError.push_back(TMath::Sqrt(value/fRunInfo->fPacking));
      // reset values
      value = 0.0;
    }
    value += runData->fDataBin[histoNo][i];
  }

  // count the number of bins to be fitted
  fNoOfFitBins=0;
  double time;
  for (unsigned int i=0; i<fData.fValue.size(); i++) {
    time = fData.fDataTimeStart + (double)i*fData.fDataTimeStep;
    if ((time >= fFitStartTime) && (time <= fFitStopTime))
      fNoOfFitBins++;
  }

  return true;
}

//--------------------------------------------------------------------------
// PrepareRawViewData
//--------------------------------------------------------------------------
/**
 * <p> Muon raw data
 *
 */
bool PRunSingleHisto::PrepareRawViewData()
{
  // get the proper run
  PRawRunData* runData = fRawData->GetRunData(fRunInfo->fRunName);
  if (!runData) { // couldn't get run
    cout << endl << "PRunSingleHisto::PrepareRawViewData(): **ERROR** Couldn't get run " << fRunInfo->fRunName.Data() << "!";
    return false;
  }

  // keep the time resolution in (us)
  fTimeResolution = runData->fTimeResolution/1.0e3;

  // check if the t0's are given in the msr-file
  if (fRunInfo->fT0[0] == -1) { // t0's are NOT in the msr-file
    // check if the t0's are in the data file
    if (runData->fT0s.size() != 0) { // t0's in the run data
      // keep the proper t0's. For single histo runs, forward is holding the histo no
      // fForwardHistoNo starts with 1 not with 0 ;-)
      fT0s.push_back(runData->fT0s[fRunInfo->fForwardHistoNo-1]);
    } else { // t0's are neither in the run data nor in the msr-file -> not acceptable!
      cout << endl << "PRunSingleHisto::PrepareRawViewData(): **ERROR** NO t0's found, neither in the run data nor in the msr-file!";
      return false;
    }
  } else { // t0's in the msr-file
    // check if t0's are given in the data file
    if (runData->fT0s.size() != 0) {
      // compare t0's of the msr-file with the one in the data file
      if (fabs(fRunInfo->fT0[0]-runData->fT0s[fRunInfo->fForwardHistoNo-1])>5.0) { // given in bins!!
        cout << endl << "PRunSingleHisto::PrepareRawViewData(): **WARNING**:";
        cout << endl << "  t0 from the msr-file is  " << fRunInfo->fT0[0];
        cout << endl << "  t0 from the data file is " << runData->fT0s[fRunInfo->fForwardHistoNo-1];
        cout << endl << "  This is quite a deviation! Is this done intentionally??";
        cout << endl;
      }
    }
    fT0s.push_back(fRunInfo->fT0[0]);
  }

  // check if post pile up data shall be used
  unsigned int histoNo;
  if (fRunInfo->fFileFormat.Contains("ppc")) {
    histoNo = runData->fDataBin.size()/2 + fRunInfo->fForwardHistoNo-1;
  } else {
    histoNo = fRunInfo->fForwardHistoNo-1;
  }

  if ((runData->fDataBin.size() < histoNo) || (histoNo < 0)) {
    cout << endl << "PRunSingleHisto::PrepareRawViewData(): **PANIC ERROR**:";
    cout << endl << "   histoNo found = " << histoNo << ", but there are only " << runData->fDataBin.size() << " runs!?!?";
    cout << endl << "   Will quite :-(";
    cout << endl;
    return false;
  }

  // transform raw histo data. This is done the following way (for details see the manual):
  // for the single histo fit, just the rebinned raw data are copied
  // first get start data, end data, and t0
  unsigned int start;
  unsigned int end;
  double       t0    = fT0s[0];
  // raw data, since PMusrCanvas is doing ranging etc.
  // start = the first bin which is a multiple of packing backward from t0
  start = (int)t0 - ((int)t0/fRunInfo->fPacking)*fRunInfo->fPacking;
  // end = last bin starting from start which is a multipl of packing and still within the data 
  end   = start + ((runData->fDataBin[histoNo].size()-start-1)/fRunInfo->fPacking)*fRunInfo->fPacking;
  // check if start, end, and t0 make any sense
  // 1st check if start and end are in proper order
  if (end < start) { // need to swap them
    int keep = end;
    end = start;
    start = keep;
  }
  // 2nd check if start is within proper bounds
  if ((start < 0) || (start > runData->fDataBin[histoNo].size())) {
    cout << endl << "PRunSingleHisto::PrepareRawViewData(): **ERROR** start data bin doesn't make any sense!";
    return false;
  }
  // 3rd check if end is within proper bounds
  if ((end < 0) || (end > runData->fDataBin[histoNo].size())) {
    cout << endl << "PRunSingleHisto::PrepareRawViewData(): **ERROR** end data bin doesn't make any sense!";
    return false;
  }
  // 4th check if t0 is within proper bounds
  if ((t0 < 0) || (t0 > runData->fDataBin[histoNo].size())) {
    cout << endl << "PRunSingleHisto::PrepareRawViewData(): **ERROR** t0 data bin doesn't make any sense!";
    return false;
  }

  // everything looks fine, hence fill data set
  double value = 0.0;
  // data start at data_start-t0
  // time shifted so that packing is included correctly, i.e. t0 == t0 after packing
  fData.fDataTimeStart = fTimeResolution*((double)start-t0+(double)fRunInfo->fPacking/2.0);
/*
cout << endl << ">> time resolution = " << fTimeResolution;
cout << endl << ">> start = " << start << ", t0 = " << t0 << ", packing = " << fRunInfo->fPacking;
cout << endl << ">> data start time = " << fData.fDataTimeStart;
*/
  fData.fDataTimeStep  = fTimeResolution*fRunInfo->fPacking;
  for (unsigned i=start; i<end; i++) {
    if (((i-start) % fRunInfo->fPacking == 0) && (i != start)) { // fill data
      // in order that after rebinning the fit does not need to be redone (important for plots)
      // the value is normalize to per bin
      value /= fRunInfo->fPacking;
      fData.fValue.push_back(value);
      if (value == 0.0)
        fData.fError.push_back(1.0);
      else
        fData.fError.push_back(TMath::Sqrt(value/fRunInfo->fPacking));
      // reset values
      value = 0.0;
    }
    value += runData->fDataBin[histoNo][i];
  }

  // count the number of bins
  fNoOfFitBins=0;

  double time;
  for (unsigned int i=0; i<fData.fValue.size(); i++) {
    time = fData.fDataTimeStart + (double)i*fData.fDataTimeStep;
    if ((time >= fFitStartTime) && (time <= fFitStopTime))
      fNoOfFitBins++;
  }

  // fill theory vector for kView
  // feed the parameter vector
  std::vector<double> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (unsigned int i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // calculate asymmetry
  double N0;
  // check if norm is a parameter or a function
  if (fRunInfo->fNormParamNo < MSR_PARAM_FUN_OFFSET) { // norm is a parameter
    N0 = par[fRunInfo->fNormParamNo-1];
  } else { // norm is a function
    // get function number
    unsigned int funNo = fRunInfo->fNormParamNo-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo,fRunInfo->fMap,par);
  }

  // get tau
  double tau;
  if (fRunInfo->fLifetimeParamNo != -1)
    tau = par[fRunInfo->fLifetimeParamNo-1];
  else
    tau = PMUON_LIFETIME;

  // get background
  double bkg;
  if (fRunInfo->fBkgFitParamNo == -1) { // bkg not fitted
    if (isnan(fRunInfo->fBkgFix[0])) { // no fixed background given (background interval)
      if (!EstimateBkg(histoNo))
        return false;
      bkg = fBackground;
    } else { // fixed bkg given
      bkg = fRunInfo->fBkgFix[0];
    }
  } else { // bkg fitted
    bkg = par[fRunInfo->fBkgFitParamNo-1];
  }

  // calculate functions
  for (int i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), fRunInfo->fMap, par);
  }

  // calculate theory
  unsigned int size = runData->fDataBin[histoNo].size();
  double startTime  = -fT0s[0]*fTimeResolution;
  double theoryValue;
  fData.fTheoryTimeStart = startTime;
//cout << endl << ">> theory start time = " << fData.fTheoryTimeStart;
  fData.fTheoryTimeStep  = fTimeResolution;
  for (unsigned int i=0; i<size; i++) {
    time = startTime + (double)i*fTimeResolution;
    theoryValue = fTheory->Func(time, par, fFuncValues);
    if (theoryValue > 10.0) {  // dirty hack needs to be fixed!!
      theoryValue = 0.0;
    }
    fData.fTheory.push_back(N0*TMath::Exp(-time/tau)*(1+theoryValue)+bkg);
  }

  // clean up
  par.clear();

  return true;
}

//--------------------------------------------------------------------------
// PrepareViewData
//--------------------------------------------------------------------------
/**
 * <p> Muon life time corrected data: Starting from
 * \f[ N(t) = N_0 e^{-t/\tau} [ 1 + A(t) ] + \mathrm{Bkg} \f]
 * it follows that
 * \f[ A(t) = (-1) + e^{+t/\tau}\, \frac{N(t)-\mathrm{Bkg}}{N_0}. \f]
 * For the error estimate only the statistical error of \f$ N(t) \f$ is used, and hence
 * \f[ \Delta A(t) = \frac{e^{t/\tau}}{N_0}\,\sqrt{\frac{N(t)}{p}} \f]
 * where \f$ p \f$ is the packing, and \f$ N(t) \f$ are the packed data, i.e.
 * \f[ N(t_i) = \frac{1}{p}\, \sum_{j=i}^{i+p} n_j \f]
 * with \f$ n_j \f$ the raw histogram data bins.
 */
bool PRunSingleHisto::PrepareViewData()
{
  // get the proper run
  PRawRunData* runData = fRawData->GetRunData(fRunInfo->fRunName);
  if (!runData) { // couldn't get run
    cout << endl << "PRunSingleHisto::PrepareViewData(): **ERROR** Couldn't get run " << fRunInfo->fRunName.Data() << "!";
    return false;
  }

  // keep the time resolution in (us)
  fTimeResolution = runData->fTimeResolution/1.0e3;

  // check if the t0's are given in the msr-file
  if (fRunInfo->fT0[0] == -1) { // t0's are NOT in the msr-file
    // check if the t0's are in the data file
    if (runData->fT0s.size() != 0) { // t0's in the run data
      // keep the proper t0's. For single histo runs, forward is holding the histo no
      // fForwardHistoNo starts with 1 not with 0 ;-)
      fT0s.push_back(runData->fT0s[fRunInfo->fForwardHistoNo-1]);
    } else { // t0's are neither in the run data nor in the msr-file -> not acceptable!
      cout << endl << "PRunSingleHisto::PrepareViewData(): **ERROR** NO t0's found, neither in the run data nor in the msr-file!";
      return false;
    }
  } else { // t0's in the msr-file
    // check if t0's are given in the data file
    if (runData->fT0s.size() != 0) {
      // compare t0's of the msr-file with the one in the data file
      if (fabs(fRunInfo->fT0[0]-runData->fT0s[fRunInfo->fForwardHistoNo-1])>5.0) { // given in bins!!
        cout << endl << "PRunSingleHisto::PrepareViewData(): **WARNING**:";
        cout << endl << "  t0 from the msr-file is  " << fRunInfo->fT0[0];
        cout << endl << "  t0 from the data file is " << runData->fT0s[fRunInfo->fForwardHistoNo-1];
        cout << endl << "  This is quite a deviation! Is this done intentionally??";
        cout << endl;
      }
    }
    fT0s.push_back(fRunInfo->fT0[0]);
  }

  // check if post pile up data shall be used
  unsigned int histoNo;
  if (fRunInfo->fFileFormat.Contains("ppc")) {
    histoNo = runData->fDataBin.size()/2 + fRunInfo->fForwardHistoNo-1;
  } else {
    histoNo = fRunInfo->fForwardHistoNo-1;
  }

  if ((runData->fDataBin.size() < histoNo) || (histoNo < 0)) {
    cout << endl << "PRunSingleHisto::PrepareViewData(): **PANIC ERROR**:";
    cout << endl << "   histoNo found = " << histoNo << ", but there are only " << runData->fDataBin.size() << " runs!?!?";
    cout << endl << "   Will quite :-(";
    cout << endl;
    return false;
  }

  // transform raw histo data. This is done the following way (for details see the manual):
  // for the single histo fit, just the rebinned raw data are copied
  // first get start data, end data, and t0
  unsigned int start;
  unsigned int end;
  double       t0    = fT0s[0];
  // raw data, since PMusrCanvas is doing ranging etc.
  // start = the first bin which is a multiple of packing backward from t0
  start = (int)t0 - ((int)t0/fRunInfo->fPacking)*fRunInfo->fPacking;
  // end = last bin starting from start which is a multipl of packing and still within the data 
  end   = start + ((runData->fDataBin[histoNo].size()-start-1)/fRunInfo->fPacking)*fRunInfo->fPacking;
  // check if start, end, and t0 make any sense
  // 1st check if start and end are in proper order
  if (end < start) { // need to swap them
    int keep = end;
    end = start;
    start = keep;
  }
  // 2nd check if start is within proper bounds
  if ((start < 0) || (start > runData->fDataBin[histoNo].size())) {
    cout << endl << "PRunSingleHisto::PrepareViewData(): **ERROR** start data bin doesn't make any sense!";
    return false;
  }
  // 3rd check if end is within proper bounds
  if ((end < 0) || (end > runData->fDataBin[histoNo].size())) {
    cout << endl << "PRunSingleHisto::PrepareViewData(): **ERROR** end data bin doesn't make any sense!";
    return false;
  }
  // 4th check if t0 is within proper bounds
  if ((t0 < 0) || (t0 > runData->fDataBin[histoNo].size())) {
    cout << endl << "PRunSingleHisto::PrepareViewData(): **ERROR** t0 data bin doesn't make any sense!";
    return false;
  }

  // everything looks fine, hence fill data set

  // feed the parameter vector
  std::vector<double> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (unsigned int i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // calculate asymmetry
  double N0;
  // check if norm is a parameter or a function
  if (fRunInfo->fNormParamNo < MSR_PARAM_FUN_OFFSET) { // norm is a parameter
    N0 = par[fRunInfo->fNormParamNo-1];
  } else { // norm is a function
    // get function number
    unsigned int funNo = fRunInfo->fNormParamNo-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo,fRunInfo->fMap,par);
  }
//cout << endl << ">> N0 = " << N0;

  // get tau
  double tau;
  if (fRunInfo->fLifetimeParamNo != -1)
    tau = par[fRunInfo->fLifetimeParamNo-1];
  else
    tau = PMUON_LIFETIME;
//cout << endl << ">> tau = " << tau;

  // get background
  double bkg;
  if (fRunInfo->fBkgFitParamNo == -1) { // bkg not fitted
    if (isnan(fRunInfo->fBkgFix[0])) { // no fixed background given (background interval)
      if (!EstimateBkg(histoNo))
        return false;
      bkg = fBackground;
    } else { // fixed bkg given
      bkg = fRunInfo->fBkgFix[0];
    }
  } else { // bkg fitted
    bkg = par[fRunInfo->fBkgFitParamNo-1];
  }
//cout << endl << ">> bkg = " << bkg;

  double value  = 0.0;
  double expval;
  double time;

  // data start at data_start-t0
  // time shifted so that packing is included correctly, i.e. t0 == t0 after packing
  fData.fDataTimeStart = fTimeResolution*((double)start-t0+(double)fRunInfo->fPacking/2.0);
  fData.fDataTimeStep  = fTimeResolution*fRunInfo->fPacking;
/*
cout << endl << ">> start time = " << fData.fDataTimeStart << ", step = " << fData.fDataTimeStep;
cout << endl << ">> start = " << start << ", end = " << end;
cout << endl << "--------------------------------";
*/
  for (unsigned int i=start; i<end; i++) {
    if (((i-start) % fRunInfo->fPacking == 0) && (i != start)) { // fill data
      // in order that after rebinning the fit does not need to be redone (important for plots)
      // the value is normalize to per bin
      value /= fRunInfo->fPacking;
      // 1st term start time, 2nd term offset to the center of the rebinned bin
      time = ((double)start-t0)*fTimeResolution +
             (double)(i-start-fRunInfo->fPacking/2.0)*fTimeResolution;
      expval = TMath::Exp(+time/tau)/N0;
      fData.fValue.push_back(-1.0+expval*(value-bkg));
//cout << endl << ">> i=" << i << ",time=" << time << ",expval=" << expval << ",value=" << value << ",bkg=" << bkg << ",expval*(value-bkg)-1=" << expval*(value-bkg)-1.0;
      fData.fError.push_back(expval*TMath::Sqrt(value/fRunInfo->fPacking));
//cout << endl << ">> " << time << ", " << expval << ", " << -1.0+expval*(value-bkg) << ", " << expval*TMath::Sqrt(value/fRunInfo->fPacking);
      value = 0.0;
    }
    value += runData->fDataBin[histoNo][i];
  }

  // count the number of bins to be fitted
  fNoOfFitBins=0;
  for (unsigned int i=0; i<fData.fValue.size(); i++) {
    time = fData.fDataTimeStart + (double)i*fData.fDataTimeStep;
    if ((time >= fFitStartTime) && (time <= fFitStopTime))
      fNoOfFitBins++;
  }

  // calculate functions
  for (int i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), fRunInfo->fMap, par);
  }

  // calculate theory
  unsigned int size = runData->fDataBin[histoNo].size();
  double startTime  = -fT0s[0]*fTimeResolution;
  double theoryValue;
  fData.fTheoryTimeStart = startTime;
  fData.fTheoryTimeStep  = fTimeResolution;
  for (unsigned int i=0; i<size; i++) {
    time = startTime + (double)i*fTimeResolution;
    theoryValue = fTheory->Func(time, par, fFuncValues);
    if (theoryValue > 10.0) { // dirty hack needs to be fixed!!
      theoryValue = 0.0;
    }
    fData.fTheory.push_back(theoryValue);
  }

  // clean up
  par.clear();

  return true;
}

//--------------------------------------------------------------------------
// EstimatBkg
//--------------------------------------------------------------------------
/**
 * <p>
 */
bool PRunSingleHisto::EstimateBkg(unsigned int histoNo)
{
  double beamPeriod = 0.0;

  // check if data are from PSI, RAL, or TRIUMF
  if (fRunInfo->fInstitute.Contains("psi"))
    beamPeriod = ACCEL_PERIOD_PSI;
  else if (fRunInfo->fInstitute.Contains("ral"))
    beamPeriod = ACCEL_PERIOD_RAL;
  else if (fRunInfo->fInstitute.Contains("triumf"))
    beamPeriod = ACCEL_PERIOD_TRIUMF;
  else
    beamPeriod = 0.0;

  // check if start and end are in proper order
  unsigned int start = fRunInfo->fBkgRange[0];
  unsigned int end   = fRunInfo->fBkgRange[1];
  if (end < start) {
    cout << endl << "PRunSingleHisto::EstimatBkg(): end = " << end << " > start = " << start << "! Will swap them!";
    unsigned int keep = end;
    end = start;
    start = keep;
  }

  // calculate proper background range
  if (beamPeriod != 0.0) {
    double beamPeriodBins = beamPeriod/fRunInfo->fPacking;
    unsigned int periods = (unsigned int)((double)(end - start + 1) / beamPeriodBins);
    end = start + (unsigned int)round((double)periods*beamPeriodBins);
    cout << endl << "PRunSingleHisto::EstimatBkg(): Background " << start << ", " << end;
    if (end == start)
      end = fRunInfo->fBkgRange[1];
  }

  // get the proper run
  PRawRunData* runData = fRawData->GetRunData(fRunInfo->fRunName);

  // check if start is within histogram bounds
  if ((start < 0) || (start >= runData->fDataBin[histoNo].size())) {
    cout << endl << "PRunSingleHisto::EstimatBkg(): background bin values out of bound!";
    cout << endl << "  histo lengths    = " << runData->fDataBin[histoNo].size();
    cout << endl << "  background start = " << start;
    return false;
  }

  // check if end is within histogram bounds
  if ((end < 0) || (end >= runData->fDataBin[histoNo].size())) {
    cout << endl << "PRunSingleHisto::EstimatBkg(): background bin values out of bound!";
    cout << endl << "  histo lengths  = " << runData->fDataBin[histoNo].size();
    cout << endl << "  background end = " << end;
    return false;
  }

  // calculate background
  double bkg    = 0.0;

  // forward
  for (unsigned int i=start; i<end; i++)
    bkg += runData->fDataBin[histoNo][i];
  bkg /= static_cast<double>(end - start + 1);

  fBackground = bkg; // keep background (per bin) for chisq, max.log.likelihood

  cout << endl << ">> fRunInfo->fRunName=" << fRunInfo->fRunName.Data() << ", histNo=" << histoNo << ", bkg=" << bkg;

  return true;
}
