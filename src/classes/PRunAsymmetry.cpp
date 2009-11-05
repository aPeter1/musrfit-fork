/***************************************************************************

  PRunAsymmetry.cpp

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

#include <iostream>

#include "PMusr.h"
#include "PRunAsymmetry.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
PRunAsymmetry::PRunAsymmetry() : PRunBase()
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
PRunAsymmetry::PRunAsymmetry(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag) : PRunBase(msrInfo, rawData, runNo, tag)
{
  // check if alpha and/or beta is fixed --------------------

  PMsrParamList *param = msrInfo->GetMsrParamList();

  // check if alpha is given
  if (fRunInfo->GetAlphaParamNo() == -1) { // no alpha given
    cerr << endl << "PRunAsymmetry::PRunAsymmetry(): **ERROR** no alpha parameter given! This is needed for an asymmetry fit!";
    cerr << endl;
    fValid = false;
    return;
  }
  // check if alpha parameter is within proper bounds
  if ((fRunInfo->GetAlphaParamNo() < 0) || (fRunInfo->GetAlphaParamNo() > (Int_t)param->size())) {
    cerr << endl << "PRunAsymmetry::PRunAsymmetry(): **ERROR** alpha parameter no = " << fRunInfo->GetAlphaParamNo();
    cerr << endl << "  This is out of bound, since there are only " << param->size() << " parameters.";
    cerr << endl;
    fValid = false;
    return;
  }
  // check if alpha is fixed
  Bool_t alphaFixedToOne = false;
//cout << endl << ">> alpha = " << (*param)[fRunInfo->GetAlphaParamNo()-1].fValue << ", " << (*param)[fRunInfo->GetAlphaParamNo()-1].fStep;
  if (((*param)[fRunInfo->GetAlphaParamNo()-1].fStep == 0.0) &&
      ((*param)[fRunInfo->GetAlphaParamNo()-1].fValue == 1.0))
    alphaFixedToOne = true;

  // check if beta is given
  Bool_t betaFixedToOne = false;
  if (fRunInfo->GetBetaParamNo() == -1) { // no beta given hence assuming beta == 1
    betaFixedToOne = true;
  } else if ((fRunInfo->GetBetaParamNo() < 0) || (fRunInfo->GetBetaParamNo() > (Int_t)param->size())) { // check if beta parameter is within proper bounds
    cerr << endl << "PRunAsymmetry::PRunAsymmetry(): **ERROR** beta parameter no = " << fRunInfo->GetBetaParamNo();
    cerr << endl << "  This is out of bound, since there are only " << param->size() << " parameters.";
    cerr << endl;
    fValid = false;
    return;
  } else { // check if beta is fixed
    if (((*param)[fRunInfo->GetBetaParamNo()-1].fStep == 0.0) &&
        ((*param)[fRunInfo->GetBetaParamNo()-1].fValue == 1.0))
      betaFixedToOne = true;
  }

  // set fAlphaBetaTag
  if (alphaFixedToOne && betaFixedToOne)       // alpha == 1, beta == 1
    fAlphaBetaTag = 1;
  else if (!alphaFixedToOne && betaFixedToOne) // alpha != 1, beta == 1
    fAlphaBetaTag = 2;
  else if (alphaFixedToOne && !betaFixedToOne) // alpha == 1, beta != 1
    fAlphaBetaTag = 3;
  else
    fAlphaBetaTag = 4;

//cout << endl << ">> PRunAsymmetry::PRunAsymmetry(): fAlphaBetaTag = " << fAlphaBetaTag << endl;

  // calculate fData
  if (!PrepareData())
    fValid = false;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
PRunAsymmetry::~PRunAsymmetry()
{
  fForward.clear();
  fForwardErr.clear();
  fBackward.clear();
  fBackwardErr.clear();
}

//--------------------------------------------------------------------------
// CalcChiSquare
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param par parameter vector iterated by minuit
 */
Double_t PRunAsymmetry::CalcChiSquare(const std::vector<Double_t>& par)
{
  Double_t chisq = 0.0;
  Double_t diff = 0.0;
  Double_t asymFcnValue = 0.0;
  Double_t a, b, f;

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par);
  }

  // calculate chisq
  Double_t time;
  for (UInt_t i=0; i<fData.GetValue()->size(); i++) {
    time = fData.GetDataTimeStart() + (Double_t)i*fData.GetDataTimeStep();
    if ((time>=fFitStartTime) && (time<=fFitStopTime)) {
      switch (fAlphaBetaTag) {
        case 1: // alpha == 1, beta == 1
          asymFcnValue = fTheory->Func(time, par, fFuncValues);
          break;
        case 2: // alpha != 1, beta == 1
          a = par[fRunInfo->GetAlphaParamNo()-1];
          f = fTheory->Func(time, par, fFuncValues);
          asymFcnValue = (f*(a+1.0)-(a-1.0))/((a+1.0)-f*(a-1.0));
          break;
        case 3: // alpha == 1, beta != 1
          b = par[fRunInfo->GetBetaParamNo()-1];
          f = fTheory->Func(time, par, fFuncValues);
          asymFcnValue = f*(b+1.0)/(2.0-f*(b-1.0));
          break;
        case 4: // alpha != 1, beta != 1
          a = par[fRunInfo->GetAlphaParamNo()-1];
          b = par[fRunInfo->GetBetaParamNo()-1];
          f = fTheory->Func(time, par, fFuncValues);
          asymFcnValue = (f*(a*b+1.0)-(a-1.0))/((a+1.0)-f*(a*b-1.0));
          break;
        default:
          break;
      }
//if (i==0) cout << endl << "A(0) = " << asymFcnValue;
      diff = fData.GetValue()->at(i) - asymFcnValue;
      chisq += diff*diff / (fData.GetError()->at(i)*fData.GetError()->at(i));
    }
  }

//cout << endl << ">> chisq = " << chisq;

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
Double_t PRunAsymmetry::CalcMaxLikelihood(const std::vector<Double_t>& par)
{
  cout << endl << "PRunSingleHisto::CalcMaxLikelihood(): not implemented yet ..." << endl;

  return 1.0;
}

//--------------------------------------------------------------------------
// CalcTheory
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PRunAsymmetry::CalcTheory()
{
  // feed the parameter vector
  std::vector<Double_t> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (UInt_t i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par);
  }

  // calculate asymmetry
  Double_t asymFcnValue = 0.0;
  Double_t a, b, f;
  Double_t time;
  for (UInt_t i=0; i<fData.GetValue()->size(); i++) {
    time = fData.GetDataTimeStart() + (Double_t)i*fData.GetDataTimeStep();
    switch (fAlphaBetaTag) {
      case 1: // alpha == 1, beta == 1
        asymFcnValue = fTheory->Func(time, par, fFuncValues);
        break;
      case 2: // alpha != 1, beta == 1
        a = par[fRunInfo->GetAlphaParamNo()-1];
        f = fTheory->Func(time, par, fFuncValues);
        asymFcnValue = (f*(a+1.0)-(a-1.0))/((a+1.0)-f*(a-1.0));
        break;
      case 3: // alpha == 1, beta != 1
        b = par[fRunInfo->GetBetaParamNo()-1];
        f = fTheory->Func(time, par, fFuncValues);
        asymFcnValue = f*(b+1.0)/(2.0-f*(b-1.0));
        break;
      case 4: // alpha != 1, beta != 1
        a = par[fRunInfo->GetAlphaParamNo()-1];
        b = par[fRunInfo->GetBetaParamNo()-1];
        f = fTheory->Func(time, par, fFuncValues);
        asymFcnValue = (f*(a*b+1.0)-(a-1.0))/((a+1.0)-f*(a*b-1.0));
        break;
      default:
        asymFcnValue = 0.0;
        break;
    }
    fData.AppendTheoryValue(asymFcnValue);
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
 * Error propagation for \f$ A_i = (f_i^{\rm c}-b_i^{\rm c})/(f_i^{\rm c}+b_i^{\rm c})\f$:
 * \f[ \Delta A_i = \pm\frac{2}{(f_i^{\rm c}+b_i^{\rm c})^2}\left[
 *     (b_i^{\rm c})^2 (\Delta f_i^{\rm c})^2 + 
 *      (\Delta b_i^{\rm c})^2 (f_i^{\rm c})^2\right]^{1/2}\f]
 */
Bool_t PRunAsymmetry::PrepareData()
{
//cout << endl << "in PRunAsymmetry::PrepareData(): will feed fData" << endl;

  // get forward/backward histo from PRunDataHandler object ------------------------
  // get the correct run
  PRawRunData *runData = fRawData->GetRunData(*(fRunInfo->GetRunName()));
  if (!runData) { // run not found
    cerr << endl << "PRunAsymmetry::PrepareData(): **ERROR** Couldn't get run " << fRunInfo->GetRunName()->Data() << "!";
    cerr << endl;
    return false;
  }

  // keep the time resolution in (us)
  fTimeResolution = runData->GetTimeResolution()/1.0e3;

  // keep start/stop time for fit
  fFitStartTime = fRunInfo->GetFitRange(0);
  fFitStopTime  = fRunInfo->GetFitRange(1);
//cout << endl << "start/stop (fit): " << fFitStartTime << ", " << fFitStopTime << endl;


  // check if the t0's are given in the msr-file
  if (fRunInfo->GetT0Size() == 0) { // t0's are NOT in the msr-file
    // check if the t0's are in the data file
    if (runData->GetT0s().size() != 0) { // t0's in the run data
      // keep the proper t0's. For asymmetry runs, forward/backward are holding the histo no
      // fForwardHistoNo starts with 1 not with 0 etc. ;-)
      fT0s.push_back(runData->GetT0(fRunInfo->GetForwardHistoNo()-1));  // forward  t0
      fT0s.push_back(runData->GetT0(fRunInfo->GetBackwardHistoNo()-1)); // backward t0
    } else { // t0's are neither in the run data nor in the msr-file -> not acceptable!
      cerr << endl << "PRunAsymmetry::PrepareData(): **ERROR** NO t0's found, neither in the run data nor in the msr-file!";
      cerr << endl;
      return false;
    }
  } else { // t0's in the msr-file
    // check if t0's are given in the data file
    if (runData->GetT0s().size() != 0) {
      // compare t0's of the msr-file with the one in the data file
      if (fabs(fRunInfo->GetT0(0)-runData->GetT0(fRunInfo->GetForwardHistoNo()-1))>5.0) { // given in bins!!
        cerr << endl << "PRunAsymmetry::PrepareData(): **WARNING**: forward histo";
        cerr << endl << "  t0 from the msr-file is  " << fRunInfo->GetT0(0);
        cerr << endl << "  t0 from the data file is " << runData->GetT0(fRunInfo->GetForwardHistoNo()-1);
        cerr << endl << "  This is quite a deviation! Is this done intentionally??";
        cerr << endl;
      }
      if (fabs(fRunInfo->GetT0(1)-runData->GetT0(fRunInfo->GetBackwardHistoNo()-1))>5.0) { // given in bins!!
        cerr << endl << "PRunAsymmetry::PrepareData(): **WARNING**: backward histo";
        cerr << endl << "  t0 from the msr-file is  " << fRunInfo->GetT0(1);
        cerr << endl << "  t0 from the data file is " << runData->GetT0(fRunInfo->GetBackwardHistoNo()-1);
        cerr << endl << "  This is quite a deviation! Is this done intentionally??";
        cerr << endl;
      }
    }
    fT0s.push_back(fRunInfo->GetT0(0)); // forward  t0
    fT0s.push_back(fRunInfo->GetT0(1)); // backward t0
  }

  // check if post pile up data shall be used
  UInt_t histoNo[2]; // forward/backward
  histoNo[0] = fRunInfo->GetForwardHistoNo()-1;
  histoNo[1] = fRunInfo->GetBackwardHistoNo()-1;
  // first check if forward/backward given in the msr-file are valid
  if ((runData->GetNoOfHistos() < histoNo[0]+1) || (histoNo[0] < 0) ||
      (runData->GetNoOfHistos() < histoNo[1]+1) || (histoNo[1] < 0)) {
    cerr << endl << "PRunAsymmetry::PrepareData(): **PANIC ERROR**:";
    cerr << endl << "   forward/backward histo no found = " << histoNo[0]+1;
    cerr << ", " << histoNo[1]+1 << ", but there are only " << runData->GetNoOfHistos() << " histo sets!?!?";
    cerr << endl << "   Will quit :-(";
    cerr << endl;
    return false;
  }

  // get raw forward/backward histo data
  fForward.resize(runData->GetDataBin(histoNo[0])->size());
  fBackward.resize(runData->GetDataBin(histoNo[0])->size());
  fForward = *runData->GetDataBin(histoNo[0]);
  fBackward = *runData->GetDataBin(histoNo[1]);

  // check if addrun's are present, and if yes add data
  // check if there are runs to be added to the current one
  if (fRunInfo->GetRunNameSize() > 1) { // runs to be added present
    PRawRunData *addRunData;
    for (UInt_t i=1; i<fRunInfo->GetRunNameSize(); i++) {
      // get run to be added to the main one
      addRunData = fRawData->GetRunData(*(fRunInfo->GetRunName(i)));
      if (addRunData == 0) { // couldn't get run
        cerr << endl << "PRunAsymmetry::PrepareData(): **ERROR** Couldn't get addrun " << fRunInfo->GetRunName(i)->Data() << "!";
        cerr << endl;
        return false;
      }

      // get T0's of the to be added run
      Int_t t0Add[2] = {0, 0};
      // check if the t0's are given in the msr-file
      if (2*i+1 >= fRunInfo->GetT0Size()) { // t0's are NOT in the msr-file
        // check if the t0's are in the data file
        if (addRunData->GetT0s().size() != 0) { // t0's in the run data
          // keep the proper t0's. For asymmetry runs, forward/backward are holding the histo no
          // fForwardHistoNo starts with 1 not with 0 etc. ;-)
          t0Add[0] = addRunData->GetT0(fRunInfo->GetForwardHistoNo()-1);  // forward  t0
          t0Add[1] = addRunData->GetT0(fRunInfo->GetBackwardHistoNo()-1); // backward t0
        } else { // t0's are neither in the run data nor in the msr-file -> not acceptable!
          cerr << endl << "PRunAsymmetry::PrepareData(): **ERROR** NO t0's found, neither in the addrun (" << fRunInfo->GetRunName(i)->Data() << ") data nor in the msr-file!";
          cerr << endl;
          return false;
        }
      } else { // t0's in the msr-file
        // check if t0's are given in the data file
        if (2*i+1 < fRunInfo->GetT0Size()) {
          t0Add[0] = fRunInfo->GetT0(2*i);
          t0Add[1] = fRunInfo->GetT0(2*i+1);
        } else {
          cerr << endl << "PRunAsymmetry::PrepareData(): **WARNING** NO t0's found, neither in the addrun data (";
          cerr << fRunInfo->GetRunName(i)->Data();
          cerr << "), nor in the msr-file! Will try to use the T0 of the run data (";
          cerr << fRunInfo->GetRunName(i)->Data();
          cerr << ") without any warranty!";
          t0Add[0] = fRunInfo->GetT0(0);
          t0Add[1] = fRunInfo->GetT0(1);
        }
        if (addRunData->GetT0s().size() != 0) {
          // compare t0's of the msr-file with the one in the data file
          if (fabs(t0Add[0]-addRunData->GetT0(fRunInfo->GetForwardHistoNo()-1))>5.0) { // given in bins!!
            cerr << endl << "PRunAsymmetry::PrepareData(): **WARNING**: forward histo";
            cerr << endl << "  t0 from the msr-file is  " << fRunInfo->GetT0(2*i);
            cerr << endl << "  t0 from the data file is " << addRunData->GetT0(fRunInfo->GetForwardHistoNo()-1);
            cerr << endl << "  This is quite a deviation! Is this done intentionally??";
            cerr << endl << "  addrun: " << fRunInfo->GetRunName(i)->Data();
            cerr << endl;
          }
          if (fabs(t0Add[1]-addRunData->GetT0(fRunInfo->GetBackwardHistoNo()-1))>5.0) { // given in bins!!
            cerr << endl << "PRunAsymmetry::PrepareData(): **WARNING**: backward histo";
            cerr << endl << "  t0 from the msr-file is  " << fRunInfo->GetT0(2*i+1);
            cerr << endl << "  t0 from the data file is " << addRunData->GetT0(fRunInfo->GetBackwardHistoNo()-1);
            cerr << endl << "  This is quite a deviation! Is this done intentionally??";
            cerr << endl << "  addrun: " << fRunInfo->GetRunName(i)->Data();
            cerr << endl;
          }
        }
      }

      // add forward run
      UInt_t addRunSize = addRunData->GetDataBin(histoNo[0])->size();
      for (UInt_t j=0; j<runData->GetDataBin(histoNo[0])->size(); j++) {
        // make sure that the index stays in the proper range
        if ((j-t0Add[0]+fT0s[0] >= 0) && (j-t0Add[0]+fT0s[0] < addRunSize)) {
          fForward[j] += addRunData->GetDataBin(histoNo[0])->at(j-t0Add[0]+fT0s[0]);
        }
      }

      // add backward run
      addRunSize = addRunData->GetDataBin(histoNo[1])->size();
      for (UInt_t j=0; j<runData->GetDataBin(histoNo[1])->size(); j++) {
        // make sure that the index stays in the proper range
        if ((j-t0Add[1]+fT0s[1] >= 0) && (j-t0Add[1]+fT0s[1] < addRunSize)) {
          fBackward[j] += addRunData->GetDataBin(histoNo[1])->at(j-t0Add[1]+fT0s[1]);
        }
      }
    }
  }

  // subtract background from histogramms ------------------------------------------
  if (fRunInfo->GetBkgFixSize() == 0) { // no fixed background given
    if (fRunInfo->GetBkgRangeSize() != 0) {
      if (!SubtractEstimatedBkg())
        return false;
    } else { // no background given to do the job
      cerr << endl << "PRunAsymmetry::PrepareData(): **ERROR** Neither fix background nor background bins are given!";
      cerr << endl << "One of the two is needed! Will quit ...";
      return false;
    }
  } else { // fixed background given
    if (!SubtractFixBkg())
      return false;
  }

  // everything looks fine, hence fill data set
  Bool_t status;
  switch(fHandleTag) {
    case kFit:
      status = PrepareFitData(runData, histoNo);
      break;
    case kView:
      status = PrepareViewData(runData, histoNo);
      break;
    default:
      status = false;
      break;
  }

  return status;
}

//--------------------------------------------------------------------------
// SubtractFixBkg
//--------------------------------------------------------------------------
/**
 * <p>Subtracts a fixed background from the raw data. The background is given
 * in units of (1/ns). The error propagation
 * is done the following way: it is assumed that the error of the background
 * is Poisson like, i.e. \f$\Delta\mathrm{bkg} = \sqrt{\mathrm{bkg}}\f$.
 *
 * Error propagation: 
 * \f[ \Delta f_i^{\rm c} = \pm\left[ (\Delta f_i)^2 + (\Delta \mathrm{bkg})^2 \right]^{1/2} =
 *      \pm\left[ f_i + \mathrm{bkg} \right]^{1/2}, \f]
 * where \f$ f_i^{\rm c} \f$ is the background corrected histogram, \f$ f_i \f$ the raw histogram
 * and \f$ \mathrm{bkg} \f$ the fix given background.
 */
Bool_t PRunAsymmetry::SubtractFixBkg()
{  
  for (UInt_t i=0; i<fForward.size(); i++) {
    fForwardErr.push_back(TMath::Sqrt(fForward[i]+fRunInfo->GetBkgFix(0) * fTimeResolution * 1.0e3));
    fForward[i] -= fRunInfo->GetBkgFix(0) * fTimeResolution * 1.0e3; // bkg per ns -> bkg per bin; 1.0e3: us -> ns
    fBackwardErr.push_back(TMath::Sqrt(fBackward[i]+fRunInfo->GetBkgFix(1) * fTimeResolution * 1.0e3));
    fBackward[i] -= fRunInfo->GetBkgFix(1) * fTimeResolution * 1.0e3; // bkg per ns -> bkg per bin; 1.0e3: us -> ns
  }
  
  return true;
}

//--------------------------------------------------------------------------
// SubtractEstimatedBkg
//--------------------------------------------------------------------------
/**
 * <p>Subtracts the background given ...
 *
 * The background corrected histogramms are:
 * \f$ f_i^{\rm c} = f_i - \mathrm{bkg} \f$, where \f$ f_i \f$ is the raw data histogram,
 * \f$ \mathrm{bkg} \f$ the background estimate, and \f$ f_i^{\rm c} \f$ background corrected
 * histogram. The error on \f$ f_i^{\rm c} \f$ is 
 * \f[ \Delta f_i^{\rm c} = \pm \sqrt{ (\Delta f_i)^2 + (\Delta \mathrm{bkg})^2 } =
 *    \pm \sqrt{f_i + (\Delta \mathrm{bkg})^2} \f]
 * The background error \f$ \Delta \mathrm{bkg} \f$ is
 * \f[ \Delta \mathrm{bkg} = \pm\frac{1}{N}\left[\sum_{i=0}^N (\Delta f_i)^2\right]^{1/2} =
 * \pm\frac{1}{N}\left[\sum_{i=0}^N f_i \right]^{1/2},\f]
 * where \f$N\f$ is the number of bins over which the background is formed.
 */
Bool_t PRunAsymmetry::SubtractEstimatedBkg()
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
  UInt_t start[2] = {fRunInfo->GetBkgRange(0), fRunInfo->GetBkgRange(2)};
  UInt_t end[2]   = {fRunInfo->GetBkgRange(1), fRunInfo->GetBkgRange(3)};
  for (UInt_t i=0; i<2; i++) {
    if (end[i] < start[i]) {
      cout << endl << "PRunAsymmetry::SubtractEstimatedBkg(): end = " << end[i] << " > start = " << start[i] << "! Will swap them!";
      UInt_t keep = end[i];
      end[i] = start[i];
      start[i] = keep;
    }
  }

  // calculate proper background range
  for (UInt_t i=0; i<2; i++) {
    if (beamPeriod != 0.0) {
      Double_t beamPeriodBins = beamPeriod/fRunInfo->GetPacking();
      UInt_t periods = (UInt_t)((Double_t)(end[i] - start[i] + 1) / beamPeriodBins);
      end[i] = start[i] + (UInt_t)round((Double_t)periods*beamPeriodBins);
      cout << "PRunAsymmetry::SubtractEstimatedBkg(): Background " << start[i] << ", " << end[i] << endl;
      if (end[i] == start[i])
        end[i] = fRunInfo->GetBkgRange(2*i+1);
    }
  }

  // check if start is within histogram bounds
  if ((start[0] < 0) || (start[0] >= fForward.size()) ||
      (start[1] < 0) || (start[1] >= fBackward.size())) {
    cerr << endl << "PRunAsymmetry::SubtractEstimatedBkg(): **ERROR** background bin values out of bound!";
    cerr << endl << "  histo lengths (f/b)  = (" << fForward.size() << "/" << fBackward.size() << ").";
    cerr << endl << "  background start (f/b) = (" << start[0] << "/" << start[1] << ").";
    return false;
  }

  // check if end is within histogram bounds
  if ((end[0] < 0) || (end[0] >= fForward.size()) ||
      (end[1] < 0) || (end[1] >= fBackward.size())) {
    cerr << endl << "PRunAsymmetry::SubtractEstimatedBkg(): **ERROR** background bin values out of bound!";
    cerr << endl << "  histo lengths (f/b)  = (" << fForward.size() << "/" << fBackward.size() << ").";
    cerr << endl << "  background end (f/b) = (" << end[0] << "/" << end[1] << ").";
    return false;
  }

  // calculate background
  Double_t bkg[2]    = {0.0, 0.0};
  Double_t errBkg[2] = {0.0, 0.0};

  // forward
  for (UInt_t i=start[0]; i<end[0]; i++)
    bkg[0] += fForward[i];
  errBkg[0] = TMath::Sqrt(bkg[0])/(end[0] - start[0] + 1);
  bkg[0] /= static_cast<Double_t>(end[0] - start[0] + 1);
//cout << endl << ">> bkg[0] = " << bkg[0];

  // backward
  for (UInt_t i=start[1]; i<end[1]; i++)
    bkg[1] += fBackward[i];
  errBkg[1] = TMath::Sqrt(bkg[1])/(end[0] - start[0] + 1);
  bkg[1] /= static_cast<Double_t>(end[1] - start[1] + 1);
//cout << endl << ">> bkg[1] = " << bkg[1] << endl;

  // correct error for forward, backward
  for (UInt_t i=0; i<fForward.size(); i++) {
    fForwardErr.push_back(TMath::Sqrt(fForward[i]+errBkg[0]*errBkg[0]));
    fBackwardErr.push_back(TMath::Sqrt(fBackward[i]+errBkg[1]*errBkg[1]));
  }

  // subtract background from data
  for (UInt_t i=0; i<fForward.size(); i++) {
    fForward[i]  -= bkg[0];
    fBackward[i] -= bkg[1];
  }

  return true;
}

//--------------------------------------------------------------------------
// PrepareFitData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
Bool_t PRunAsymmetry::PrepareFitData(PRawRunData* runData, UInt_t histoNo[2])
{
  // transform raw histo data. This is done the following way (for details see the manual):
  // first rebin the data, than calculate the asymmetry
  // first get start data, end data, and t0
  Int_t start[2] = {fRunInfo->GetDataRange(0), fRunInfo->GetDataRange(2)};
  Int_t end[2]   = {fRunInfo->GetDataRange(1), fRunInfo->GetDataRange(3)};
  Double_t t0[2] = {fT0s[0], fT0s[1]};
  // check if start, end, and t0 make any sense
  // 1st check if start and end are in proper order
  for (UInt_t i=0; i<2; i++) {
    if (end[i] < start[i]) { // need to swap them
      Int_t keep = end[i];
      end[i] = start[i];
      start[i] = keep;
    }
    // 2nd check if start is within proper bounds
    if ((start[i] < 0) || (start[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << "PRunAsymmetry::PrepareFitData(): **ERROR** start data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 3rd check if end is within proper bounds
    if ((end[i] < 0) || (end[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << "PRunAsymmetry::PrepareFitData(): **ERROR** end data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 4th check if t0 is within proper bounds
    if ((t0[i] < 0) || (t0[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << "PRunAsymmetry::PrepareFitData(): **ERROR** t0 data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
  }

  // everything looks fine, hence fill packed forward and backward histo
  PRunData forwardPacked;
  PRunData backwardPacked;
  Double_t value = 0.0;
  Double_t error = 0.0;
  // forward
  for (Int_t i=start[0]; i<end[0]; i++) {
    if (fRunInfo->GetPacking() == 1) {
      forwardPacked.AppendValue(fForward[i]);
      forwardPacked.AppendErrorValue(fForwardErr[i]);
    } else { // packed data, i.e. fRunInfo->GetPacking() > 1
      if (((i-start[0]) % fRunInfo->GetPacking() == 0) && (i != start[0])) { // fill data
        // in order that after rebinning the fit does not need to be redone (important for plots)
        // the value is normalize to per bin
        value /= fRunInfo->GetPacking();
        forwardPacked.AppendValue(value);
        if (value == 0.0)
          forwardPacked.AppendErrorValue(1.0);
        else
          forwardPacked.AppendErrorValue(TMath::Sqrt(error)/fRunInfo->GetPacking());
        value = 0.0;
        error = 0.0;
      }
      value += fForward[i];
      error += fForwardErr[i]*fForwardErr[i];
    }
  }
  // backward
  for (Int_t i=start[1]; i<end[1]; i++) {
    if (fRunInfo->GetPacking() == 1) {
      backwardPacked.AppendValue(fBackward[i]);
      backwardPacked.AppendErrorValue(fBackwardErr[i]);
    } else { // packed data, i.e. fRunInfo->GetPacking() > 1
      if (((i-start[1]) % fRunInfo->GetPacking() == 0) && (i != start[1])) { // fill data
        // in order that after rebinning the fit does not need to be redone (important for plots)
        // the value is normalize to per bin
        value /= fRunInfo->GetPacking();
        backwardPacked.AppendValue(value);
        if (value == 0.0)
          backwardPacked.AppendErrorValue(1.0);
        else
          backwardPacked.AppendErrorValue(TMath::Sqrt(error)/fRunInfo->GetPacking());
        value = 0.0;
        error = 0.0;
      }
      value += fBackward[i];
      error += fBackwardErr[i]*fBackwardErr[i];
    }
  }

  // check if packed forward and backward hist have the same size, otherwise take the minimum size
  UInt_t noOfBins = forwardPacked.GetValue()->size();
  if (forwardPacked.GetValue()->size() != backwardPacked.GetValue()->size()) {
    if (forwardPacked.GetValue()->size() > backwardPacked.GetValue()->size())
      noOfBins = backwardPacked.GetValue()->size();
  }

  // form asymmetry including error propagation
  Double_t asym;
  Double_t f, b, ef, eb;
  // fill data time start, and step
  // data start at data_start-t0 shifted by (pack-1)/2
  fData.SetDataTimeStart(fTimeResolution*((Double_t)start[0]-t0[0]+(Double_t)(fRunInfo->GetPacking()-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*(Double_t)fRunInfo->GetPacking());
  for (UInt_t i=0; i<noOfBins; i++) {
    // to make the formulae more readable
    f  = forwardPacked.GetValue()->at(i);
    b  = backwardPacked.GetValue()->at(i);
    ef = forwardPacked.GetError()->at(i);
    eb = backwardPacked.GetError()->at(i);
    // check that there are indeed bins
    if (f+b != 0.0)
      asym = (f-b) / (f+b);
    else
      asym = 0.0;
    fData.AppendValue(asym);
    // calculate the error
    if (f+b != 0.0)
      error = 2.0/((f+b)*(f+b))*TMath::Sqrt(b*b*ef*ef+eb*eb*f*f);
    else
      error = 1.0;
    fData.AppendErrorValue(error);
  }

  // count the number of bins to be fitted
  Double_t time;
  fNoOfFitBins=0;
  for (UInt_t i=0; i<fData.GetValue()->size(); i++) {
    time = fData.GetDataTimeStart() + (Double_t)i * fData.GetDataTimeStep();
    if ((time >= fFitStartTime) && (time <= fFitStopTime))
      fNoOfFitBins++;
  }

  // clean up
  fForward.clear();
  fForwardErr.clear();
  fBackward.clear();
  fBackwardErr.clear();

  return true;
}

//--------------------------------------------------------------------------
// PrepareViewData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
Bool_t PRunAsymmetry::PrepareViewData(PRawRunData* runData, UInt_t histoNo[2])
{
  // check if view_packing is wished
  Int_t packing = fRunInfo->GetPacking();
  if (fMsrInfo->GetMsrPlotList()->at(0).fViewPacking > 0) {
    packing = fMsrInfo->GetMsrPlotList()->at(0).fViewPacking;
  }

  // feed the parameter vector
  std::vector<Double_t> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (UInt_t i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // transform raw histo data. This is done the following way (for details see the manual):
  // first rebin the data, than calculate the asymmetry
  // first get start data, end data, and t0
  Int_t val = fRunInfo->GetDataRange(0)-packing*(fRunInfo->GetDataRange(0)/packing);
  do {
    if (fRunInfo->GetDataRange(2) - fRunInfo->GetDataRange(0) < 0)
      val += packing;
  } while (val + fRunInfo->GetDataRange(2) - fRunInfo->GetDataRange(0) < 0);

  Int_t start[2] = {val, val + fRunInfo->GetDataRange(2) - fRunInfo->GetDataRange(0)};
  Int_t end[2];
  Double_t t0[2] = {fT0s[0], fT0s[1]};

/*
cout << endl << ">> start[0]=" << start[0] << ", end[0]=" << end[0];
cout << endl << ">> start[1]=" << start[1] << ", end[1]=" << end[1];
cout << endl;
*/

  // make sure that there are equal number of rebinned bins in forward and backward
  UInt_t noOfBins0 = (runData->GetDataBin(histoNo[0])->size()-start[0])/packing;
  UInt_t noOfBins1 = (runData->GetDataBin(histoNo[1])->size()-start[1])/packing;
  if (noOfBins0 > noOfBins1)
    noOfBins0 = noOfBins1;
  end[0] = start[0] + noOfBins0 * packing;
  end[1] = start[1] + noOfBins0 * packing;

  // check if start, end, and t0 make any sense
  // 1st check if start and end are in proper order
  for (UInt_t i=0; i<2; i++) {
    if (end[i] < start[i]) { // need to swap them
      Int_t keep = end[i];
      end[i] = start[i];
      start[i] = keep;
    }
    // 2nd check if start is within proper bounds
    if ((start[i] < 0) || (start[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << "PRunAsymmetry::PrepareViewData(): **ERROR** start data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 3rd check if end is within proper bounds
    if ((end[i] < 0) || (end[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << "PRunAsymmetry::PrepareViewData(): **ERROR** end data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 4th check if t0 is within proper bounds
    if ((t0[i] < 0) || (t0[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << "PRunAsymmetry::PrepareViewData(): **ERROR** t0 data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
  }

  // everything looks fine, hence fill packed forward and backward histo
  PRunData forwardPacked;
  PRunData backwardPacked;
  Double_t value = 0.0;
  Double_t error = 0.0;
  // forward
  for (Int_t i=start[0]; i<end[0]; i++) {
    if (packing == 1) {
      forwardPacked.AppendValue(fForward[i]);
      forwardPacked.AppendErrorValue(fForwardErr[i]);
    } else { // packed data, i.e. packing > 1
      if (((i-start[0]) % packing == 0) && (i != start[0])) { // fill data
        // in order that after rebinning the fit does not need to be redone (important for plots)
        // the value is normalize to per bin
        value /= packing;
        forwardPacked.AppendValue(value);
        if (value == 0.0)
          forwardPacked.AppendErrorValue(1.0);
        else
          forwardPacked.AppendErrorValue(TMath::Sqrt(error)/packing);
        value = 0.0;
        error = 0.0;
      }
    }
    value += fForward[i];
    error += fForwardErr[i]*fForwardErr[i];
  }
  // backward
  for (Int_t i=start[1]; i<end[1]; i++) {
    if (packing == 1) {
      backwardPacked.AppendValue(fBackward[i]);
      backwardPacked.AppendErrorValue(fBackwardErr[i]);
    } else { // packed data, i.e. packing > 1
      if (((i-start[1]) % packing == 0) && (i != start[1])) { // fill data
        // in order that after rebinning the fit does not need to be redone (important for plots)
        // the value is normalize to per bin
        value /= packing;
        backwardPacked.AppendValue(value);
        if (value == 0.0)
          backwardPacked.AppendErrorValue(1.0);
        else
          backwardPacked.AppendErrorValue(TMath::Sqrt(error)/packing);
        value = 0.0;
        error = 0.0;
      }
    }
    value += fBackward[i];
    error += fBackwardErr[i]*fBackwardErr[i];
  }

  // check if packed forward and backward hist have the same size, otherwise take the minimum size
  UInt_t noOfBins = forwardPacked.GetValue()->size();
  if (forwardPacked.GetValue()->size() != backwardPacked.GetValue()->size()) {
    if (forwardPacked.GetValue()->size() > backwardPacked.GetValue()->size())
      noOfBins = backwardPacked.GetValue()->size();
  }

  // form asymmetry including error propagation
  Double_t asym;
  Double_t f, b, ef, eb, alpha = 1.0, beta = 1.0;
  // fill data time start, and step
  // data start at data_start-t0
  fData.SetDataTimeStart(fTimeResolution*((Double_t)start[0]-t0[0]+(Double_t)(packing-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*(Double_t)packing);

/*
cout << endl << ">> start time = " << fData.GetDataTimeStart() << ", step = " << fData.GetDataTimeStep();
cout << endl << "--------------------------------" << endl;
*/

  // get the proper alpha and beta
  switch (fAlphaBetaTag) {
    case 1: // alpha == 1, beta == 1
      alpha = 1.0;
      beta  = 1.0;
      break;
    case 2: // alpha != 1, beta == 1
      alpha = par[fRunInfo->GetAlphaParamNo()-1];
      beta  = 1.0;
      break;
    case 3: // alpha == 1, beta != 1
      alpha = 1.0;
      beta  = par[fRunInfo->GetBetaParamNo()-1];
      break;
    case 4: // alpha != 1, beta != 1
      alpha = par[fRunInfo->GetAlphaParamNo()-1];
      beta  = par[fRunInfo->GetBetaParamNo()-1];
      break;
    default:
      break;
  }
//cout << endl << ">> alpha = " << alpha << ", beta = " << beta;

  for (UInt_t i=0; i<forwardPacked.GetValue()->size(); i++) {
    // to make the formulae more readable
    f  = forwardPacked.GetValue()->at(i);
    b  = backwardPacked.GetValue()->at(i);
    ef = forwardPacked.GetError()->at(i);
    eb = backwardPacked.GetError()->at(i);
    // check that there are indeed bins
    if (f+b != 0.0)
      asym = (alpha*f-b) / (alpha*beta*f+b);
    else
      asym = 0.0;
    fData.AppendValue(asym);
    // calculate the error
    if (f+b != 0.0)
      error = 2.0/((f+b)*(f+b))*TMath::Sqrt(b*b*ef*ef+eb*eb*f*f);
    else
      error = 1.0;
    fData.AppendErrorValue(error);
  }

  // count the number of bins to be fitted
  Double_t time;
  fNoOfFitBins=0;
  for (UInt_t i=0; i<fData.GetValue()->size(); i++) {
    time = fData.GetDataTimeStart() + (Double_t)i * fData.GetDataTimeStep();
    if ((time >= fFitStartTime) && (time <= fFitStopTime))
      fNoOfFitBins++;
  }

  // clean up
  fForward.clear();
  fForwardErr.clear();
  fBackward.clear();
  fBackwardErr.clear();

  // fill theory vector for kView
  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par);
  }

  // calculate theory
  UInt_t size = runData->GetDataBin(histoNo[0])->size();
  Double_t factor = 1.0;
  if (fData.GetValue()->size() * 10 > runData->GetDataBin(histoNo[0])->size()) {
    size = fData.GetValue()->size() * 10;
    factor = (Double_t)runData->GetDataBin(histoNo[0])->size() / (Double_t)size;
  }
//cout << endl << ">> runData->fDataBin[histoNo[0]].size() = " << runData->fDataBin[histoNo[0]].size() << ",  fData.GetValue()->size() * 10 = " << fData.GetValue()->size() * 10 << ", size = " << size << ", factor = " << factor << endl;
  fData.SetTheoryTimeStart(fData.GetDataTimeStart());
  fData.SetTheoryTimeStep(fTimeResolution*factor);
  for (UInt_t i=0; i<size; i++) {
    time = fData.GetTheoryTimeStart() + (Double_t)i*fTimeResolution*factor;
    value = fTheory->Func(time, par, fFuncValues);
    if (fabs(value) > 10.0) {  // dirty hack needs to be fixed!!
      value = 0.0;
    }
    fData.AppendTheoryValue(value);
  }

  // clean up
  par.clear();

  return true;
}
