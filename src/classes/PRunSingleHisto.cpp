/***************************************************************************

  PRunSingleHisto.cpp

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
PRunSingleHisto::PRunSingleHisto(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag) : PRunBase(msrInfo, rawData, runNo, tag)
{
  if (!PrepareData()) {
    cerr << endl << "**SEVERE ERROR**: PRunSingleHisto::PRunSingleHisto: Couldn't prepare data for fitting!";
    cerr << endl << "  This is very bad :-(, will quit ...";
    cerr << endl;
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
 * The return value is chisq * fRunInfo->GetPacking(), the reason is:
 * the data d_i and the theory t_i are scaled by the packing, i.e. d_i -> d_i / packing.
 * Since the error is 1/sqrt(d_i) and hence error^2 = d_i it follows that
 * (d_i - t_i)^2 ~ 1/packing^2 and error^2 ~ 1/packing, and hence the chisq needs
 * to be scaled by packing.
 *
 * \param par parameter vector iterated by minuit
 */
Double_t PRunSingleHisto::CalcChiSquare(const std::vector<Double_t>& par)
{
  Double_t chisq     = 0.0;
  Double_t diff      = 0.0;

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
    if (fRunInfo->GetBkgFixSize() == 0) { // no fixed background given (background interval)
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
//cout << ">> i = " << i << ", funcNo = " << funcNo << endl;
    fFuncValues[i] = fMsrInfo->EvalFunc(funcNo, *fRunInfo->GetMap(), par);
  }

  // calculate chi square
  Double_t time;
  for (UInt_t i=0; i<fData.GetValue()->size(); i++) {
    time = fData.GetDataTimeStart() + (Double_t)i*fData.GetDataTimeStep();
    if ((time>=fFitStartTime) && (time<=fFitStopTime)) {
      diff = fData.GetValue()->at(i) -
            (N0*TMath::Exp(-time/tau)*(1.0+fTheory->Func(time, par, fFuncValues))+bkg);
      chisq += diff*diff / (fData.GetError()->at(i)*fData.GetError()->at(i));
    }
  }

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
Double_t PRunSingleHisto::CalcMaxLikelihood(const std::vector<Double_t>& par)
{
  Double_t mllh      = 0.0; // maximum log likelihood assuming poisson distribution for the single bin

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
    if (fRunInfo->GetBkgFixSize() == 0) { // no fixed background given (background interval)
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
  Double_t time;
  for (UInt_t i=0; i<fData.GetValue()->size(); i++) {
    time = fData.GetDataTimeStart() + (Double_t)i*fData.GetDataTimeStep();
    if ((time>=fFitStartTime) && (time<=fFitStopTime)) {
      // calculate theory for the given parameter set
      theo = N0*TMath::Exp(-time/tau)*(1+fTheory->Func(time, par, fFuncValues))+bkg;
      // check if data value is not too small
      if (fData.GetValue()->at(i) > 1.0e-9)
        data = fData.GetValue()->at(i);
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
    if (fRunInfo->GetBkgFixSize() == 0) { // no fixed background given (background interval)
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
  UInt_t size  = fData.GetValue()->size();
  Double_t start       = fData.GetDataTimeStart();
  Double_t resolution  = fData.GetDataTimeStep();
  Double_t time;
  for (UInt_t i=0; i<size; i++) {
    time = start + (Double_t)i*resolution;
    fData.AppendTheoryValue(N0*TMath::Exp(-time/tau)*(1+fTheory->Func(time, par, fFuncValues))+bkg);
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
Bool_t PRunSingleHisto::PrepareData()
{
//  cout << endl << "in PRunSingleHisto::PrepareData(): will feed fData";
  Bool_t success = true;

  // get the proper run
  PRawRunData* runData = fRawData->GetRunData(*fRunInfo->GetRunName());
  if (!runData) { // couldn't get run
    cerr << endl << "PRunSingleHisto::PrepareData(): **ERROR** Couldn't get run " << fRunInfo->GetRunName()->Data() << "!";
    cerr << endl;
    return false;
  }

  // check if post pile up data shall be used
  if (fRunInfo->GetForwardHistoNo() <= 0) {
    cerr << endl << "PRunSingleHisto::PrepareData(): **PANIC ERROR**:";
    cerr << endl << "   histoNo found = " << fRunInfo->GetForwardHistoNo() << ". Only histoNo > 0 are allowed.";
    cerr << endl << "   Will quit :-(";
    cerr << endl;
    return false;
  }

  UInt_t histoNo = fRunInfo->GetForwardHistoNo()-1;

  if (runData->GetNoOfHistos() <= histoNo) {
    cerr << endl << "PRunSingleHisto::PrepareData(): **PANIC ERROR**:";
    cerr << endl << "   histoNo found = " << histoNo+1 << ", but there are only " << runData->GetNoOfHistos() << " runs!?!?";
    cerr << endl << "   Will quit :-(";
    cerr << endl;
    return false;
  }

  // check if the t0's are given in the msr-file
  if (fRunInfo->GetT0Size() == 0) { // t0's are NOT in the msr-file
    // check if the t0's are in the data file
    if (runData->GetT0Size() != 0) { // t0's in the run data
      // keep the proper t0's. For single histo runs, forward is holding the histo no
      // fForwardHistoNo starts with 1 not with 0 ;-)
      fT0s.push_back(runData->GetT0(fRunInfo->GetForwardHistoNo()-1));
    } else { // t0's are neither in the run data nor in the msr-file -> will try estimated ones!
      fT0s.push_back(runData->GetT0Estimated(fRunInfo->GetForwardHistoNo()-1));
      cerr << endl << "PRunSingleHisto::PrepareData(): **WARNING** NO t0's found, neither in the run data nor in the msr-file!";
      cerr << endl << " run: " << fRunInfo->GetRunName()->Data();
      cerr << endl << " will try the estimated one: t0 = " << runData->GetT0Estimated(fRunInfo->GetForwardHistoNo()-1);
      cerr << endl << " NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
      cerr << endl;
    }
  } else { // t0's in the msr-file
    // check if t0's are given in the data file
    if (runData->GetT0Size() != 0) {
      // compare t0's of the msr-file with the one in the data file
      if (fabs(fRunInfo->GetT0(0)-runData->GetT0(fRunInfo->GetForwardHistoNo()-1))>5.0) { // given in bins!!
        cerr << endl << "PRunSingleHisto::PrepareData(): **WARNING**:";
        cerr << endl << "  t0 from the msr-file is  " << fRunInfo->GetT0(0);
        cerr << endl << "  t0 from the data file is " << runData->GetT0(fRunInfo->GetForwardHistoNo()-1);
        cerr << endl << "  This is quite a deviation! Is this done intentionally??";
        cerr << endl;
      }
    }
    fT0s.push_back(fRunInfo->GetT0(0));
  }

  // check if t0 is within proper bounds
  Int_t t0 = fT0s[0];
  if ((t0 < 0) || (t0 > (Int_t)runData->GetDataBin(histoNo)->size())) {
    cerr << endl << "PRunSingleHisto::PrepareData(): **ERROR** t0 data bin doesn't make any sense!";
    cerr << endl;
    return false;
  }

  // check if there are runs to be added to the current one
  if (fRunInfo->GetRunNameSize() > 1) { // runs to be added present
    PRawRunData *addRunData;
    for (UInt_t i=1; i<fRunInfo->GetRunNameSize(); i++) {

      // get run to be added to the main one
      addRunData = fRawData->GetRunData(*fRunInfo->GetRunName(i));
      if (addRunData == 0) { // couldn't get run
        cerr << endl << "PRunSingleHisto::PrepareData(): **ERROR** Couldn't get addrun " << fRunInfo->GetRunName(i)->Data() << "!";
        cerr << endl;
        return false;
      }

      // get T0's of the to be added run
      Int_t t0Add;
      // check if the t0's are given in the msr-file
      if (i >= fRunInfo->GetT0Size()) { // t0's are NOT in the msr-file
        // check if the t0's are in the data file
        if (addRunData->GetT0Size() != 0) { // t0's in the run data
          // keep the proper t0's. For single histo runs, forward is holding the histo no
          // fForwardHistoNo starts with 1 not with 0 ;-)
          t0Add = addRunData->GetT0(fRunInfo->GetForwardHistoNo()-1);
        } else { // t0's are neither in the run data nor in the msr-file -> will try estimated ones!
          t0Add = addRunData->GetT0Estimated(fRunInfo->GetForwardHistoNo()-1);
          cerr << endl << "PRunSingleHisto::PrepareData(): **WARNING** NO t0's found, neither in the addrun data nor in the msr-file!";
          cerr << endl << " addrun: " << fRunInfo->GetRunName(i)->Data();
          cerr << endl << " will try the estimated one: t0 = " << addRunData->GetT0Estimated(fRunInfo->GetForwardHistoNo()-1);
          cerr << endl << " NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
          cerr << endl;
        }
      } else { // t0's in the msr-file
        // check if t0's are given in the data file
        if (addRunData->GetT0Size() != 0) {
          // compare t0's of the msr-file with the one in the data file
          if (fabs(fRunInfo->GetT0(i)-addRunData->GetT0(fRunInfo->GetForwardHistoNo()-1))>5.0) { // given in bins!!
            cerr << endl << "PRunSingleHisto::PrepareData(): **WARNING**:";
            cerr << endl << "  t0 from the msr-file is  " << fRunInfo->GetT0(i);
            cerr << endl << "  t0 from the data file is " << addRunData->GetT0(fRunInfo->GetForwardHistoNo()-1);
            cerr << endl << "  This is quite a deviation! Is this done intentionally??";
            cerr << endl << "  addrun: " << fRunInfo->GetRunName(i)->Data();
            cerr << endl;
          }
        }
        if (i < fRunInfo->GetT0Size()) {
          t0Add = fRunInfo->GetT0(i);
        } else {
          cerr << endl << "PRunSingleHisto::PrepareData(): **WARNING** NO t0's found, neither in the addrun data (";
          cerr << fRunInfo->GetRunName(i)->Data();
          cerr << "), nor in the msr-file! Will try to use the T0 of the run data (";
          cerr << fRunInfo->GetRunName(i)->Data();
          cerr << ") without any warranty!";
          cerr << endl;
          t0Add = fRunInfo->GetT0(0);
        }
      }

      // add run
      for (UInt_t j=0; j<runData->GetDataBin(histoNo)->size(); j++) {
        // make sure that the index stays in the proper range
        if ((j-t0Add+t0 >= 0) && (j-t0Add+t0 < addRunData->GetDataBin(histoNo)->size())) {
          runData->AddDataBin(histoNo, j, addRunData->GetDataBin(histoNo)->at(j-t0Add+t0));
        }
      }
    }
  }

  // keep the time resolution in (us)
  fTimeResolution = runData->GetTimeResolution()/1.0e3;

  if (fHandleTag == kFit)
    success = PrepareFitData(runData, histoNo);
  else if ((fHandleTag == kView) && !fRunInfo->IsLifetimeCorrected())
    success = PrepareRawViewData(runData, histoNo);
  else if ((fHandleTag == kView) && fRunInfo->IsLifetimeCorrected())
    success = PrepareViewData(runData, histoNo);
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
Bool_t PRunSingleHisto::PrepareFitData(PRawRunData* runData, const UInt_t histoNo)
{
  // keep start/stop time for fit
  fFitStartTime = fRunInfo->GetFitRange(0);
  fFitStopTime  = fRunInfo->GetFitRange(1);
//cout << endl << "start/stop (fit): " << fFitStartTime << ", " << fFitStopTime;

  // transform raw histo data. This is done the following way (for details see the manual):
  // for the single histo fit, just the rebinned raw data are copied

  // first get start data, end data, and t0
  Int_t start;
  Int_t end;
  start = fRunInfo->GetDataRange(0);
  end   = fRunInfo->GetDataRange(1);
  // check if data range has been provided, and if not try to estimate them
  if (start < 0) {
    start = fT0s[0]+5;
    cerr << endl << "PRunSingleHisto::PrepareData(): **WARNING** data range was not provided, will try data range start = t0+5 = " << start << ".";
    cerr << endl << "NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  }
  if (end < 0) {
    end = runData->GetDataBin(histoNo)->size();
    cerr << endl << "PRunSingleHisto::PrepareData(): **WARNING** data range was not provided, will try data range end = " << end << ".";
    cerr << endl << "NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
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
  if ((start < 0) || (start > (Int_t)runData->GetDataBin(histoNo)->size())) {
    cerr << endl << "PRunSingleHisto::PrepareFitData(): **ERROR** start data bin doesn't make any sense!";
    cerr << endl;
    return false;
  }
  // 3rd check if end is within proper bounds
  if ((end < 0) || (end > (Int_t)runData->GetDataBin(histoNo)->size())) {
    cerr << endl << "PRunSingleHisto::PrepareFitData(): **ERROR** end data bin doesn't make any sense!";
    cerr << endl;
    return false;
  }

  // check how the background shall be handled
  if (fRunInfo->GetBkgFitParamNo() == -1) { // bkg shall **NOT** be fitted
    // subtract background from histogramms ------------------------------------------
    if (fRunInfo->GetBkgFixSize() == 0) { // no fixed background given
      if (fRunInfo->GetBkgRangeSize() != 0) {
        if (!EstimateBkg(histoNo))
          return false;
      } else { // no background given to do the job, try estimate
        fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.1), 0);
        fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.6), 1);
        cerr << endl << "PRunSingleHisto::PrepareFitData(): **WARNING** Neither fix background nor background bins are given!";
        cerr << endl << "Will try the following: bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
        cerr << endl << "NO WARRANTY THAT THIS MAKES ANY SENSE! Better check ...";
        cerr << endl;
        if (!EstimateBkg(histoNo))
          return false;
      }
    }
  }

  // everything looks fine, hence fill data set
  Int_t t0 = fT0s[0];
  Double_t value = 0.0;
  Double_t normalizer = 1.0;
  // data start at data_start-t0
  // time shifted so that packing is included correctly, i.e. t0 == t0 after packing
  fData.SetDataTimeStart(fTimeResolution*((Double_t)start-(Double_t)t0+(Double_t)(fRunInfo->GetPacking()-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*fRunInfo->GetPacking());
  for (Int_t i=start; i<end; i++) {
    if (fRunInfo->GetPacking() == 1) {
      value = runData->GetDataBin(histoNo)->at(i);
      normalizer = fRunInfo->GetPacking() * (fTimeResolution * 1e3); // fTimeResolution us->ns
      value /= normalizer;
      fData.AppendValue(value);
      if (value == 0.0)
        fData.AppendErrorValue(1.0);
      else
        fData.AppendErrorValue(TMath::Sqrt(value));
    } else { // packed data, i.e. fRunInfo->GetPacking() > 1
      if (((i-start) % fRunInfo->GetPacking() == 0) && (i != start)) { // fill data
        // in order that after rebinning the fit does not need to be redone (important for plots)
        // the value is normalize to per 1 nsec
        normalizer = fRunInfo->GetPacking() * (fTimeResolution * 1e3); // fTimeResolution us->ns
        value /= normalizer;
        fData.AppendValue(value);
        if (value == 0.0)
          fData.AppendErrorValue(1.0);
        else
          fData.AppendErrorValue(TMath::Sqrt(value/normalizer));
        // reset values
        value = 0.0;
      }
      value += runData->GetDataBin(histoNo)->at(i);
    }
  }

  // count the number of bins to be fitted
  fNoOfFitBins=0;
  Double_t time;
//cout << endl << ">> size=" << fData.GetValue()->size() << ", fDataTimeStart=" << fData.GetDataTimeStart() << ", fDataTimeStep=" << fData.GetDataTimeStep() << ", fFitStartTime=" << fFitStartTime << ", fFitStopTime=" << fFitStopTime;
  for (UInt_t i=0; i<fData.GetValue()->size(); i++) {
    time = fData.GetDataTimeStart() + (Double_t)i*fData.GetDataTimeStep();
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
Bool_t PRunSingleHisto::PrepareRawViewData(PRawRunData* runData, const UInt_t histoNo)
{
  // check if view_packing is wished
  Int_t packing = fRunInfo->GetPacking();
  if (fMsrInfo->GetMsrPlotList()->at(0).fViewPacking > 0) {
    packing = fMsrInfo->GetMsrPlotList()->at(0).fViewPacking;
  }

  // raw data, since PMusrCanvas is doing ranging etc.
  // start = the first bin which is a multiple of packing backward from first good data bin
  Int_t start = fRunInfo->GetDataRange(0) - (fRunInfo->GetDataRange(0)/packing)*packing;
  // end = last bin starting from start which is a multipl of packing and still within the data 
  Int_t end   = start + ((runData->GetDataBin(histoNo)->size()-start)/packing)*packing;
  // check if data range has been provided, and if not try to estimate them
  if (start < 0) {
    start = (fT0s[0]+5) - ((fT0s[0]+5)/packing)*packing;
    end = start + ((runData->GetDataBin(histoNo)->size()-start)/packing)*packing;
    cerr << endl << "PRunSingleHisto::PrepareData(): **WARNING** data range was not provided, will try data range start = " << start << ".";
    cerr << endl << "NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
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
  if ((start < 0) || (start > (Int_t)runData->GetDataBin(histoNo)->size())) {
    cerr << endl << "PRunSingleHisto::PrepareRawViewData(): **ERROR** start data bin doesn't make any sense!";
    cerr << endl;
    return false;
  }
  // 3rd check if end is within proper bounds
  if ((end < 0) || (end > (Int_t)runData->GetDataBin(histoNo)->size())) {
    cerr << endl << "PRunSingleHisto::PrepareRawViewData(): **ERROR** end data bin doesn't make any sense!";
    cerr << endl;
    return false;
  }

  // everything looks fine, hence fill data set
  Int_t t0 = fT0s[0];
  Double_t value = 0.0;
  // data start at data_start-t0
  // time shifted so that packing is included correctly, i.e. t0 == t0 after packing
  fData.SetDataTimeStart(fTimeResolution*((Double_t)start-(Double_t)t0+(Double_t)(packing-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*packing);

/*
cout << endl << ">> time resolution = " << fTimeResolution;
cout << endl << ">> start = " << start << ", t0 = " << t0 << ", packing = " << packing;
cout << endl << ">> data start time = " << fData.GetDataTimeStart();
*/

  Double_t normalizer = 1.0;
  for (Int_t i=start; i<end; i++) {
    if (((i-start) % packing == 0) && (i != start)) { // fill data
      // in order that after rebinning the fit does not need to be redone (important for plots)
      // the value is normalize to per 1 nsec
      normalizer = packing * (fTimeResolution * 1e3); // fTimeResolution us->ns
      value /= normalizer;
      fData.AppendValue(value);
      if (value == 0.0)
        fData.AppendErrorValue(1.0);
      else
        fData.AppendErrorValue(TMath::Sqrt(value/normalizer));
      // reset values
      value = 0.0;
    }
    value += runData->GetDataBin(histoNo)->at(i);
  }

  // count the number of bins
  fNoOfFitBins=0;

  Double_t time;
  for (UInt_t i=0; i<fData.GetValue()->size(); i++) {
    time = fData.GetDataTimeStart() + (Double_t)i*fData.GetDataTimeStep();
    if ((time >= fFitStartTime) && (time <= fFitStopTime))
      fNoOfFitBins++;
  }

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

  // get tau
  Double_t tau;
  if (fRunInfo->GetLifetimeParamNo() != -1)
    tau = par[fRunInfo->GetLifetimeParamNo()-1];
  else
    tau = PMUON_LIFETIME;

  // get background
  Double_t bkg;
  if (fRunInfo->GetBkgFitParamNo() == -1) { // bkg not fitted
    if (fRunInfo->GetBkgFixSize() == 0) { // no fixed background given (background interval)
      if (fRunInfo->GetBkgRangeSize() != 0) { // background range given
        if (!EstimateBkg(histoNo))
          return false;
      } else { // no background given to do the job, try estimate
        fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.1), 0);
        fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.6), 1);
        cerr << endl << "PRunSingleHisto::PrepareData(): **WARNING** Neither fix background nor background bins are given!";
        cerr << endl << "Will try the following: bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
        cerr << endl << "NO WARRANTY THAT THIS MAKES ANY SENSE! Better check ...";
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

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par);
  }

  // calculate theory
  UInt_t size = runData->GetDataBin(histoNo)->size();
  Double_t factor = 1.0;
  if (fData.GetValue()->size() * 10 > runData->GetDataBin(histoNo)->size()) {
    size = fData.GetValue()->size() * 10;
    factor = (Double_t)runData->GetDataBin(histoNo)->size() / (Double_t)size;
  }
//cout << endl << ">> runData->GetDataBin(histoNo).size() = " << runData->GetDataBin(histoNo)->size() << ",  fData.GetValue()->size() * 10 = " << fData.GetValue()->size() * 10 << ", size = " << size << ", factor = " << factor << endl;
  Double_t theoryValue;
  fData.SetTheoryTimeStart(fData.GetDataTimeStart());
  fData.SetTheoryTimeStep(fTimeResolution*factor);
  for (UInt_t i=0; i<size; i++) {
    time = fData.GetTheoryTimeStart() + i*fData.GetTheoryTimeStep();
    theoryValue = fTheory->Func(time, par, fFuncValues);
    if (fabs(theoryValue) > 10.0) {  // dirty hack needs to be fixed!!
      theoryValue = 0.0;
    }
    fData.AppendTheoryValue(N0*TMath::Exp(-time/tau)*(1+theoryValue)+bkg);
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

  // transform raw histo data. This is done the following way (for details see the manual):
  // for the single histo fit, just the rebinned raw data are copied
  // first get start data, end data, and t0
  Int_t t0 = fT0s[0];

  // start = the first bin which is a multiple of packing backward from first good data bin
  Int_t start = fRunInfo->GetDataRange(0) - (fRunInfo->GetDataRange(0)/packing)*packing;
  // end = last bin starting from start which is a multiple of packing and still within the data
  Int_t end   = start + ((runData->GetDataBin(histoNo)->size()-start)/packing)*packing;

  // check if data range has been provided, and if not try to estimate them
  if (start < 0) {
    start = (fT0s[0]+5) - ((fT0s[0]+5)/packing)*packing;
    end = start + ((runData->GetDataBin(histoNo)->size()-start)/packing)*packing;
    cerr << endl << "PRunSingleHisto::PrepareData(): **WARNING** data range was not provided, will try data range start = " << start << ".";
    cerr << endl << "NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
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
  if ((start < 0) || (start > (Int_t)runData->GetDataBin(histoNo)->size())) {
    cerr << endl << "PRunSingleHisto::PrepareViewData(): **ERROR** start data bin doesn't make any sense!";
    cerr << endl;
    return false;
  }
  // 3rd check if end is within proper bounds
  if ((end < 0) || (end > (Int_t)runData->GetDataBin(histoNo)->size())) {
    cerr << endl << "PRunSingleHisto::PrepareViewData(): **ERROR** end data bin doesn't make any sense!";
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

  // get tau
  Double_t tau;
  if (fRunInfo->GetLifetimeParamNo() != -1)
    tau = par[fRunInfo->GetLifetimeParamNo()-1];
  else
    tau = PMUON_LIFETIME;
//cout << endl << ">> tau = " << tau;

  // get background
  Double_t bkg;
  if (fRunInfo->GetBkgFitParamNo() == -1) { // bkg not fitted
    if (fRunInfo->GetBkgFixSize() == 0) { // no fixed background given (background interval)
      if (fRunInfo->GetBkgRangeSize() != 0) { // background range given
        if (!EstimateBkg(histoNo))
          return false;
      } else { // no background given to do the job, try estimate
        fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.1), 0);
        fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.6), 1);
        cerr << endl << "PRunSingleHisto::PrepareData(): **WARNING** Neither fix background nor background bins are given!";
        cerr << endl << "Will try the following: bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
        cerr << endl << "NO WARRANTY THAT THIS MAKES ANY SENSE! Better check ...";
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

  Double_t value = 0.0;
  Double_t expval = 0.0;
  Double_t rrf_val = 0.0;
  Double_t time = 0.0;

  // data start at data_start-t0 shifted by (pack-1)/2
  fData.SetDataTimeStart(fTimeResolution*((Double_t)start-(Double_t)t0+(Double_t)(packing-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*packing);

/*
cout << endl << ">> start time = " << fData.GetDataTimeStart() << ", step = " << fData.GetDataTimeStep();
cout << endl << ">> start = " << start << ", end = " << end;
cout << endl << "--------------------------------" << endl;
*/

  Double_t normalizer = 1.0;
  Double_t gammaRRF = 0.0, wRRF = 0.0, phaseRRF = 0.0;
  if (fMsrInfo->GetMsrPlotList()->at(0).fRRFFreq == 0.0) { // normal Data representation
    for (Int_t i=start; i<end; i++) {
      if (((i-start) % packing == 0) && (i != start)) { // fill data
        // in order that after rebinning the fit does not need to be redone (important for plots)
        // the value is normalize to per 1 nsec
        normalizer = packing * (fTimeResolution * 1.0e3); // fTimeResolution us->ns
        value /= normalizer;
        time = (((Double_t)i-(Double_t)(packing-1)/2.0)-t0)*fTimeResolution;
        expval = TMath::Exp(+time/tau)/N0;
        fData.AppendValue(-1.0+expval*(value-bkg));
        //cout << endl << ">> i=" << i << ",t0=" << t0 << ",time=" << time << ",expval=" << expval << ",value=" << value << ",bkg=" << bkg << ",expval*(value-bkg)-1=" << expval*(value-bkg)-1.0;
        fData.AppendErrorValue(expval*TMath::Sqrt(value/normalizer));
        //cout << endl << ">> " << time << ", " << expval << ", " << -1.0+expval*(value-bkg) << ", " << expval*TMath::Sqrt(value/packing);
        value = 0.0;
      }
      value += runData->GetDataBin(histoNo)->at(i);
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

    // in order that after rebinning the fit does not need to be redone (important for plots)
    // the value is normalize to per 1 nsec
    normalizer = (fTimeResolution * 1.0e3); // fTimeResolution us->ns

    Double_t error = 0.0;
    for (Int_t i=start; i<end; i++) {
      if (((i-start) % packing == 0) && (i != start)) { // fill data
        fData.AppendValue(2.0*value/packing); // factor 2 needed because cos(a)cos(b) = 1/2(cos(a+b)+cos(a-b))
        fData.AppendErrorValue(expval*TMath::Sqrt(error)/normalizer/packing);
        value = 0.0;
        error = 0.0;
      }
      time = ((Double_t)i-t0)*fTimeResolution;
      expval = TMath::Exp(+time/tau)/N0;
      rrf_val = (-1.0+expval*(runData->GetDataBin(histoNo)->at(i)/normalizer-bkg))*TMath::Cos(wRRF * time + phaseRRF);
      value += rrf_val;
      error += runData->GetDataBin(histoNo)->at(i);
    }
  }

  // count the number of bins to be fitted
  fNoOfFitBins=0;
  for (UInt_t i=0; i<fData.GetValue()->size(); i++) {
    time = fData.GetDataTimeStart() + (Double_t)i*fData.GetDataTimeStep();
    if ((time >= fFitStartTime) && (time <= fFitStopTime))
      fNoOfFitBins++;
  }

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par);
  }

  // calculate theory
  Double_t theoryValue;
  UInt_t size = runData->GetDataBin(histoNo)->size();
  Double_t factor = 1.0;
  UInt_t rebinRRF = 0;

  if (wRRF == 0) { // no RRF
    // check if a finer binning for the theory is needed
    if (fData.GetValue()->size() * 10 > runData->GetDataBin(histoNo)->size()) {
      size = fData.GetValue()->size() * 10;
      factor = (Double_t)runData->GetDataBin(histoNo)->size() / (Double_t)size;
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

    // filter data
    CalculateKaiserFilterCoeff(wRRF, 60.0, 0.2); // w_c = wRRF, A = -20 log_10(delta), Delta w / w_c = (w_s - w_p) / (2 w_c)
    FilterData();
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
    Double_t beamPeriodBins = beamPeriod/fRunInfo->GetPacking();
    UInt_t periods = (UInt_t)((Double_t)(end - start + 1) / beamPeriodBins);
    end = start + (UInt_t)round((Double_t)periods*beamPeriodBins);
    cout << endl << "PRunSingleHisto::EstimatBkg(): Background " << start << ", " << end;
    if (end == start)
      end = fRunInfo->GetBkgRange(1);
  }

  // get the proper run
  PRawRunData* runData = fRawData->GetRunData(*fRunInfo->GetRunName());

  // check if start is within histogram bounds
  if ((start < 0) || (start >= runData->GetDataBin(histoNo)->size())) {
    cerr << endl << "PRunSingleHisto::EstimatBkg(): **ERROR** background bin values out of bound!";
    cerr << endl << "  histo lengths    = " << runData->GetDataBin(histoNo)->size();
    cerr << endl << "  background start = " << start;
    cerr << endl;
    return false;
  }

  // check if end is within histogram bounds
  if ((end < 0) || (end >= runData->GetDataBin(histoNo)->size())) {
    cerr << endl << "PRunSingleHisto::EstimatBkg(): **ERROR** background bin values out of bound!";
    cerr << endl << "  histo lengths  = " << runData->GetDataBin(histoNo)->size();
    cerr << endl << "  background end = " << end;
    cerr << endl;
    return false;
  }

  // calculate background
  Double_t bkg    = 0.0;

  // forward
//cout << endl << ">> bkg start=" << start << ", end=" << end;
  for (UInt_t i=start; i<end; i++)
    bkg += runData->GetDataBin(histoNo)->at(i);
  bkg /= static_cast<Double_t>(end - start + 1);

  fBackground = bkg / (fTimeResolution * 1e3); // keep background (per 1 nsec) for chisq, max.log.likelihood, fTimeResolution us->ns

  cout << endl << ">> fRunInfo->fRunName=" << fRunInfo->GetRunName()->Data() << ", histNo=" << histoNo << ", fBackground=" << fBackground;

  return true;
}
