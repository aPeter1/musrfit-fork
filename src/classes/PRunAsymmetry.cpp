/***************************************************************************

  PRunAsymmetry.cpp

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
PRunAsymmetry::PRunAsymmetry(PMsrHandler *msrInfo, PRunDataHandler *rawData, unsigned int runNo, EPMusrHandleTag tag) : PRunBase(msrInfo, rawData, runNo, tag)
{
  // check if alpha and/or beta is fixed --------------------

  PMsrParamList *param = msrInfo->GetMsrParamList();

  // check if alpha is given
  if (fRunInfo->fAlphaParamNo == -1) { // no alpha given
    cout << endl << "PRunAsymmetry::PRunAsymmetry(): no alpha parameter given! This is needed for an asymmetry fit!";
    fValid = false;
    return;
  }
  // check if alpha parameter is within proper bounds
  if ((fRunInfo->fAlphaParamNo < 0) || (fRunInfo->fAlphaParamNo > (int)param->size())) {
    cout << endl << "PRunAsymmetry::PRunAsymmetry(): alpha parameter no = " << fRunInfo->fAlphaParamNo;
    cout << endl << "  This is out of bound, since there are only " << param->size() << " parameters.";
    fValid = false;
    return;
  }
  // check if alpha is fixed
  bool alphaFixedToOne = false;
//cout << endl << ">> alpha = " << (*param)[fRunInfo->fAlphaParamNo-1].fValue << ", " << (*param)[fRunInfo->fAlphaParamNo-1].fStep;
  if (((*param)[fRunInfo->fAlphaParamNo-1].fStep == 0.0) && 
      ((*param)[fRunInfo->fAlphaParamNo-1].fValue == 1.0))
    alphaFixedToOne = true;

  // check if beta is given
  bool betaFixedToOne = false;
  if (fRunInfo->fBetaParamNo == -1) { // no beta given hence assuming beta == 1
    betaFixedToOne = true;
  } else if ((fRunInfo->fBetaParamNo < 0) || (fRunInfo->fBetaParamNo > (int)param->size())) { // check if beta parameter is within proper bounds
    cout << endl << "PRunAsymmetry::PRunAsymmetry(): beta parameter no = " << fRunInfo->fBetaParamNo;
    cout << endl << "  This is out of bound, since there are only " << param->size() << " parameters.";
    fValid = false;
    return;
  } else { // check if beta is fixed
    if (((*param)[fRunInfo->fBetaParamNo-1].fStep == 0.0) && 
        ((*param)[fRunInfo->fBetaParamNo-1].fValue == 1.0))
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

//cout << endl << ">> PRunAsymmetry::PRunAsymmetry(): fAlphaBetaTag = " << fAlphaBetaTag;

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
double PRunAsymmetry::CalcChiSquare(const std::vector<double>& par)
{
  double chisq = 0.0;
  double diff = 0.0;
  double asymFcnValue = 0.0;
  double a, b, f;

  // calculate functions
  for (int i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), fRunInfo->fMap, par);
  }

  // calculate chisq
  double time;
  for (unsigned int i=0; i<fData.fValue.size(); i++) {
    time = fData.fDataTimeStart + (double)i*fData.fDataTimeStep;
    if ((time>=fFitStartTime) && (time<=fFitStopTime)) {
      switch (fAlphaBetaTag) {
        case 1: // alpha == 1, beta == 1
          asymFcnValue = fTheory->Func(time, par, fFuncValues);
          break;
        case 2: // alpha != 1, beta == 1
          a = par[fRunInfo->fAlphaParamNo-1];
          f = fTheory->Func(time, par, fFuncValues);
          asymFcnValue = (f*(a+1.0)-(a-1.0))/((a+1.0)-f*(a-1.0));
          break;
        case 3: // alpha == 1, beta != 1
          b = par[fRunInfo->fBetaParamNo-1];
          f = fTheory->Func(time, par, fFuncValues);
          asymFcnValue = f*(b+1.0)/(2.0-f*(b-1.0));
          break;
        case 4: // alpha != 1, beta != 1
          a = par[fRunInfo->fAlphaParamNo-1];
          b = par[fRunInfo->fBetaParamNo-1];
          f = fTheory->Func(time, par, fFuncValues);
          asymFcnValue = (f*(a*b+1.0)-(a-1.0))/((a+1.0)-f*(a*b-1.0));
          break;
        default:
          break;
      }
//if (i==0) cout << endl << "A(0) = " << asymFcnValue;
      diff = fData.fValue[i] - asymFcnValue;
      chisq += diff*diff / (fData.fError[i]*fData.fError[i]);
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
double PRunAsymmetry::CalcMaxLikelihood(const std::vector<double>& par)
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
  std::vector<double> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (unsigned int i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // calculate functions
  for (int i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), fRunInfo->fMap, par);
  }

  // calculate asymmetry
  double asymFcnValue = 0.0;
  double a, b, f;
  double time;
  for (unsigned int i=0; i<fData.fValue.size(); i++) {
    time = fData.fDataTimeStart + (double)i*fData.fDataTimeStep; 
    switch (fAlphaBetaTag) {
      case 1: // alpha == 1, beta == 1
        asymFcnValue = fTheory->Func(time, par, fFuncValues);
        break;
      case 2: // alpha != 1, beta == 1
        a = par[fRunInfo->fAlphaParamNo-1];
        f = fTheory->Func(time, par, fFuncValues);
        asymFcnValue = (f*(a+1.0)-(a-1.0))/((a+1.0)-f*(a-1.0));
        break;
      case 3: // alpha == 1, beta != 1
        b = par[fRunInfo->fBetaParamNo-1];
        f = fTheory->Func(time, par, fFuncValues);
        asymFcnValue = f*(b+1.0)/(2.0-f*(b-1.0));
        break;
      case 4: // alpha != 1, beta != 1
        a = par[fRunInfo->fAlphaParamNo-1];
        b = par[fRunInfo->fBetaParamNo-1];
        f = fTheory->Func(time, par, fFuncValues);
        asymFcnValue = (f*(a*b+1.0)-(a-1.0))/((a+1.0)-f*(a*b-1.0));
        break;
      default:
        asymFcnValue = 0.0;
        break;
    }
    fData.fTheory.push_back(asymFcnValue);
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
bool PRunAsymmetry::PrepareData()
{
//cout << endl << "in PRunAsymmetry::PrepareData(): will feed fData";

  // get forward/backward histo from PRunDataHandler object ------------------------
  // get the correct run
  PRawRunData *runData = fRawData->GetRunData(fRunInfo->fRunName[0]);
  if (!runData) { // run not found
    cout << endl << "PRunAsymmetry::PrepareData(): Couldn't get run " << fRunInfo->fRunName[0].Data() << "!";
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
      // keep the proper t0's. For asymmetry runs, forward/backward are holding the histo no
      // fForwardHistoNo starts with 1 not with 0 etc. ;-)
      fT0s.push_back(runData->fT0s[fRunInfo->fForwardHistoNo-1]);  // forward  t0
      fT0s.push_back(runData->fT0s[fRunInfo->fBackwardHistoNo-1]); // backward t0
    } else { // t0's are neither in the run data nor in the msr-file -> not acceptable!
      cout << endl << "PRunAsymmetry::PrepareData(): NO t0's found, neither in the run data nor in the msr-file!";
      return false;
    }
  } else { // t0's in the msr-file
    // check if t0's are given in the data file
    if (runData->fT0s.size() != 0) {
      // compare t0's of the msr-file with the one in the data file
      if (fabs(fRunInfo->fT0[0]-runData->fT0s[fRunInfo->fForwardHistoNo-1])>5.0) { // given in bins!!
        cout << endl << "PRunAsymmetry::PrepareData(): **WARNING**: forward histo";
        cout << endl << "  t0 from the msr-file is  " << fRunInfo->fT0[0];
        cout << endl << "  t0 from the data file is " << runData->fT0s[fRunInfo->fForwardHistoNo-1];
        cout << endl << "  This is quite a deviation! Is this done intentionally??";
        cout << endl;
      }
      if (fabs(fRunInfo->fT0[1]-runData->fT0s[fRunInfo->fBackwardHistoNo-1])>5.0) { // given in bins!!
        cout << endl << "PRunAsymmetry::PrepareData(): **WARNING**: backward histo";
        cout << endl << "  t0 from the msr-file is  " << fRunInfo->fT0[1];
        cout << endl << "  t0 from the data file is " << runData->fT0s[fRunInfo->fBackwardHistoNo-1];
        cout << endl << "  This is quite a deviation! Is this done intentionally??";
        cout << endl;
      }
    }
    fT0s.push_back(fRunInfo->fT0[0]); // forward  t0
    fT0s.push_back(fRunInfo->fT0[1]); // backward t0
  }

  // check if post pile up data shall be used
  unsigned int histoNo[2]; // forward/backward
  if (fRunInfo->fFileFormat[0].Contains("ppc")) {
    histoNo[0] = runData->fDataBin.size()/2 + fRunInfo->fForwardHistoNo-1;
    histoNo[1] = runData->fDataBin.size()/2 + fRunInfo->fBackwardHistoNo-1;
  } else {
    histoNo[0] = fRunInfo->fForwardHistoNo-1;
    histoNo[1] = fRunInfo->fBackwardHistoNo-1;
  }
  // first check if forward/backward given in the msr-file are valid
  if ((runData->fDataBin.size() < histoNo[0]+1) || (histoNo[0] < 0) ||
      (runData->fDataBin.size() < histoNo[1]+1) || (histoNo[1] < 0)) {
    cout << endl << "PRunAsymmetry::PrepareData(): **PANIC ERROR**:";
    cout << endl << "   forward/backward histo no found = " << histoNo[0]+1;
    cout << ", " << histoNo[1]+1 << ", but there are only " << runData->fDataBin.size() << " histo sets!?!?";
    cout << endl << "   Will quit :-(";
    cout << endl;
    return false;
  }

  // get raw forward/backward histo data
  for (unsigned int i=0; i<runData->fDataBin[histoNo[0]].size(); i++) {
    fForward.push_back(runData->fDataBin[histoNo[0]][i]);
    fBackward.push_back(runData->fDataBin[histoNo[1]][i]);
  }

  // check if addrun's are present, and if yes add data
  // check if there are runs to be added to the current one
  if (fRunInfo->fRunName.size() > 1) { // runs to be added present
    PRawRunData *addRunData;
    for (unsigned int i=1; i<fRunInfo->fRunName.size(); i++) {
      // get run to be added to the main one
      addRunData = fRawData->GetRunData(fRunInfo->fRunName[i]);
      if (addRunData == 0) { // couldn't get run
        cout << endl << "PRunAsymmetry::PrepareData(): **ERROR** Couldn't get addrun " << fRunInfo->fRunName[i].Data() << "!";
        return false;
      }

      // get T0's of the to be added run
      int t0Add[2] = {0, 0};
      // check if the t0's are given in the msr-file
      if (i >= fRunInfo->fT0.size()) { // t0's are NOT in the msr-file
        // check if the t0's are in the data file
        if (addRunData->fT0s.size() != 0) { // t0's in the run data
          // keep the proper t0's. For asymmetry runs, forward/backward are holding the histo no
          // fForwardHistoNo starts with 1 not with 0 etc. ;-)
          t0Add[0] = addRunData->fT0s[fRunInfo->fForwardHistoNo-1];  // forward  t0
          t0Add[1] = addRunData->fT0s[fRunInfo->fBackwardHistoNo-1]; // backward t0
        } else { // t0's are neither in the run data nor in the msr-file -> not acceptable!
          cout << endl << "PRunAsymmetry::PrepareData(): NO t0's found, neither in the addrun (" << fRunInfo->fRunName[i].Data() << ") data nor in the msr-file!";
          return false;
        }
      } else { // t0's in the msr-file
        // check if t0's are given in the data file
        if (2*i+1 < fRunInfo->fT0.size()) {
          t0Add[0] = fRunInfo->fT0[2*i];
          t0Add[1] = fRunInfo->fT0[2*i+1];
        } else {
          cout << endl << "PRunAsymmetry::PrepareData(): **WARNING** NO t0's found, neither in the addrun data (";
          cout << fRunInfo->fRunName[i].Data();
          cout << "), nor in the msr-file! Will try to use the T0 of the run data (";
          cout << fRunInfo->fRunName[i].Data();
          cout << ") without any warranty!";
          t0Add[0] = fRunInfo->fT0[0];
          t0Add[1] = fRunInfo->fT0[1];
        }
        if (addRunData->fT0s.size() != 0) {
          // compare t0's of the msr-file with the one in the data file
          if (fabs(t0Add[0]-addRunData->fT0s[fRunInfo->fForwardHistoNo-1])>5.0) { // given in bins!!
            cout << endl << "PRunAsymmetry::PrepareData(): **WARNING**: forward histo";
            cout << endl << "  t0 from the msr-file is  " << fRunInfo->fT0[2*i];
            cout << endl << "  t0 from the data file is " << addRunData->fT0s[fRunInfo->fForwardHistoNo-1];
            cout << endl << "  This is quite a deviation! Is this done intentionally??";
            cout << endl << "  addrun: " << fRunInfo->fRunName[i].Data();
            cout << endl;
          }
          if (fabs(t0Add[1]-addRunData->fT0s[fRunInfo->fBackwardHistoNo-1])>5.0) { // given in bins!!
            cout << endl << "PRunAsymmetry::PrepareData(): **WARNING**: backward histo";
            cout << endl << "  t0 from the msr-file is  " << fRunInfo->fT0[2*i+1];
            cout << endl << "  t0 from the data file is " << addRunData->fT0s[fRunInfo->fBackwardHistoNo-1];
            cout << endl << "  This is quite a deviation! Is this done intentionally??";
            cout << endl << "  addrun: " << fRunInfo->fRunName[i].Data();
            cout << endl;
          }
        }
      }

      // add forward run
      for (unsigned int j=0; j<runData->fDataBin[histoNo[0]].size(); j++) {
        // make sure that the index stays in the proper range        
        if ((j-t0Add[0]+fT0s[0] >= 0) && (j-t0Add[0]+fT0s[0] < addRunData->fDataBin[histoNo[0]].size())) {
          fForward[j] += addRunData->fDataBin[histoNo[0]][j-t0Add[0]+fT0s[0]];
        }
      }

      // add backward run
      for (unsigned int j=0; j<runData->fDataBin[histoNo[1]].size(); j++) {
        // make sure that the index stays in the proper range
        if ((j-t0Add[1]+fT0s[1] >= 0) && (j-t0Add[1]+fT0s[1] < addRunData->fDataBin[histoNo[1]].size())) {
          fBackward[j] += addRunData->fDataBin[histoNo[1]][j-t0Add[1]+fT0s[1]];
        }
      }
    }
  }

  // subtract background from histogramms ------------------------------------------
  if (fRunInfo->fBkgFix.size() == 0) { // no fixed background given
    if (fRunInfo->fBkgRange.size() != 0) {
      if (!SubtractEstimatedBkg())
        return false;
    } else { // no background given to do the job
      cout << endl << "PRunAsymmetry::PrepareData(): Neither fix background nor background bins are given!";
      cout << endl << "One of the two is needed! Will quit ...";
      return false;
    }
  } else { // fixed background given
    if (!SubtractFixBkg())
      return false;
  }

  // everything looks fine, hence fill data set
  bool status;
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
 * <p>Subtracts a fixed background from the raw data. The error propagation
 * is done the following way: it is assumed that the error of the background
 * is Poisson like, i.e. \f$\Delta\mathrm{bkg} = \sqrt{\mathrm{bkg}}\f$.
 *
 * Error propagation: 
 * \f[ \Delta f_i^{\rm c} = \pm\left[ (\Delta f_i)^2 + (\Delta \mathrm{bkg})^2 \right]^{1/2} =
 *      \pm\left[ f_i + \mathrm{bkg} \right]^{1/2}, \f]
 * where \f$ f_i^{\rm c} \f$ is the background corrected histogram, \f$ f_i \f$ the raw histogram
 * and \f$ \mathrm{bkg} \f$ the fix given background.
 */
bool PRunAsymmetry::SubtractFixBkg()
{
  for (unsigned int i=0; i<fForward.size(); i++) {
    fForwardErr.push_back(TMath::Sqrt(fForward[i]+fRunInfo->fBkgFix[0]));
    fForward[i] -= fRunInfo->fBkgFix[0];
    fBackwardErr.push_back(TMath::Sqrt(fBackward[i]+fRunInfo->fBkgFix[1]));
    fBackward[i] -= fRunInfo->fBkgFix[1];
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
bool PRunAsymmetry::SubtractEstimatedBkg()
{
  double beamPeriod = 0.0;

  // check if data are from PSI, RAL, or TRIUMF
  if (fRunInfo->fInstitute[0].Contains("psi"))
    beamPeriod = ACCEL_PERIOD_PSI;
  else if (fRunInfo->fInstitute[0].Contains("ral"))
    beamPeriod = ACCEL_PERIOD_RAL;
  else if (fRunInfo->fInstitute[0].Contains("triumf"))
    beamPeriod = ACCEL_PERIOD_TRIUMF;
  else
    beamPeriod = 0.0;

  // check if start and end are in proper order
  unsigned int start[2] = {fRunInfo->fBkgRange[0], fRunInfo->fBkgRange[2]};
  unsigned int end[2]   = {fRunInfo->fBkgRange[1], fRunInfo->fBkgRange[3]};
  for (unsigned int i=0; i<2; i++) {
    if (end[i] < start[i]) {
      cout << endl << "PRunAsymmetry::SubtractEstimatedBkg(): end = " << end[i] << " > start = " << start[i] << "! Will swap them!";
      unsigned int keep = end[i];
      end[i] = start[i];
      start[i] = keep;
    }
  }

  // calculate proper background range
  for (unsigned int i=0; i<2; i++) {
    if (beamPeriod != 0.0) {
      double beamPeriodBins = beamPeriod/fRunInfo->fPacking;
      unsigned int periods = (unsigned int)((double)(end[i] - start[i] + 1) / beamPeriodBins);
      end[i] = start[i] + (unsigned int)round((double)periods*beamPeriodBins);
      cout << endl << "PRunAsymmetry::SubtractEstimatedBkg(): Background " << start[i] << ", " << end[i];
      if (end[i] == start[i])
        end[i] = fRunInfo->fBkgRange[2*i+1];
    }
  }

  // check if start is within histogram bounds
  if ((start[0] < 0) || (start[0] >= fForward.size()) ||
      (start[1] < 0) || (start[1] >= fBackward.size())) {
    cout << endl << "PRunAsymmetry::SubtractEstimatedBkg(): background bin values out of bound!";
    cout << endl << "  histo lengths (f/b)  = (" << fForward.size() << "/" << fBackward.size() << ").";
    cout << endl << "  background start (f/b) = (" << start[0] << "/" << start[1] << ").";
    return false;
  }

  // check if end is within histogram bounds
  if ((end[0] < 0) || (end[0] >= fForward.size()) ||
      (end[1] < 0) || (end[1] >= fBackward.size())) {
    cout << endl << "PRunAsymmetry::SubtractEstimatedBkg(): background bin values out of bound!";
    cout << endl << "  histo lengths (f/b)  = (" << fForward.size() << "/" << fBackward.size() << ").";
    cout << endl << "  background end (f/b) = (" << end[0] << "/" << end[1] << ").";
    return false;
  }

  // calculate background
  double bkg[2]    = {0.0, 0.0};
  double errBkg[2] = {0.0, 0.0};

  // forward
  for (unsigned int i=start[0]; i<end[0]; i++)
    bkg[0] += fForward[i];
  errBkg[0] = TMath::Sqrt(bkg[0])/(end[0] - start[0] + 1);
  bkg[0] /= static_cast<double>(end[0] - start[0] + 1);

  // backward
  for (unsigned int i=start[1]; i<end[1]; i++)
    bkg[1] += fBackward[i];
  errBkg[1] = TMath::Sqrt(bkg[1])/(end[0] - start[0] + 1);
  bkg[1] /= static_cast<double>(end[1] - start[1] + 1);

  // correct error for forward, backward
  for (unsigned int i=0; i<fForward.size(); i++) {
    fForwardErr.push_back(TMath::Sqrt(fForward[i]+errBkg[0]*errBkg[0]));
    fBackwardErr.push_back(TMath::Sqrt(fBackward[i]+errBkg[1]*errBkg[1]));
  }

  // subtract background from data
  for (unsigned int i=0; i<fForward.size(); i++) {
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
bool PRunAsymmetry::PrepareFitData(PRawRunData* runData, unsigned int histoNo[2])
{
  // transform raw histo data. This is done the following way (for details see the manual):
  // first rebin the data, than calculate the asymmetry
  // first get start data, end data, and t0
  unsigned int start[2] = {fRunInfo->fDataRange[0], fRunInfo->fDataRange[2]};
  unsigned int end[2]   = {fRunInfo->fDataRange[1], fRunInfo->fDataRange[3]};
  double t0[2]          = {fT0s[0], fT0s[1]};
  // check if start, end, and t0 make any sense
  // 1st check if start and end are in proper order
  for (unsigned int i=0; i<2; i++) {
    if (end[i] < start[i]) { // need to swap them
      int keep = end[i];
      end[i] = start[i];
      start[i] = keep;
    }
    // 2nd check if start is within proper bounds
    if ((start[i] < 0) || (start[i] > runData->fDataBin[histoNo[i]].size())) {
      cout << endl << "PRunAsymmetry::PrepareFitData(): **ERROR** start data bin doesn't make any sense!";
      return false;
    }
    // 3rd check if end is within proper bounds
    if ((end[i] < 0) || (end[i] > runData->fDataBin[histoNo[i]].size())) {
      cout << endl << "PRunAsymmetry::PrepareFitData(): **ERROR** end data bin doesn't make any sense!";
      return false;
    }
    // 4th check if t0 is within proper bounds
    if ((t0[i] < 0) || (t0[i] > runData->fDataBin[histoNo[i]].size())) {
      cout << endl << "PRunAsymmetry::PrepareFitData(): **ERROR** t0 data bin doesn't make any sense!";
      return false;
    }
  }

  // everything looks fine, hence fill packed forward and backward histo
  PRunData forwardPacked;
  PRunData backwardPacked;
  double value = 0.0;
  double error = 0.0;
  // forward
  for (unsigned i=start[0]; i<end[0]; i++) {
    if (fRunInfo->fPacking == 1) {
      forwardPacked.fValue.push_back(fForward[i]);
      forwardPacked.fError.push_back(fForwardErr[i]);
    } else { // packed data, i.e. fRunInfo->fPacking > 1
      if (((i-start[0]) % fRunInfo->fPacking == 0) && (i != start[0])) { // fill data
        // in order that after rebinning the fit does not need to be redone (important for plots)
        // the value is normalize to per bin
        value /= fRunInfo->fPacking;
        forwardPacked.fValue.push_back(value);
        if (value == 0.0)
          forwardPacked.fError.push_back(1.0);
        else
          forwardPacked.fError.push_back(TMath::Sqrt(error)/fRunInfo->fPacking);
        value = 0.0;
        error = 0.0;
      }
      value += fForward[i];
      error += fForwardErr[i]*fForwardErr[i];
    }
  }
  // backward
  for (unsigned i=start[1]; i<end[1]; i++) {
    if (fRunInfo->fPacking == 1) {
      backwardPacked.fValue.push_back(fBackward[i]);
      backwardPacked.fError.push_back(fBackwardErr[i]);
    } else { // packed data, i.e. fRunInfo->fPacking > 1
      if (((i-start[1]) % fRunInfo->fPacking == 0) && (i != start[1])) { // fill data
        // in order that after rebinning the fit does not need to be redone (important for plots)
        // the value is normalize to per bin
        value /= fRunInfo->fPacking;
        backwardPacked.fValue.push_back(value);
        if (value == 0.0)
          backwardPacked.fError.push_back(1.0);
        else
          backwardPacked.fError.push_back(TMath::Sqrt(error)/fRunInfo->fPacking);
        value = 0.0;
        error = 0.0;
      }
      value += fBackward[i];
      error += fBackwardErr[i]*fBackwardErr[i];
    }
  }

  // check if packed forward and backward hist have the same size, otherwise take the minimum size
  unsigned int noOfBins = forwardPacked.fValue.size();
  if (forwardPacked.fValue.size() != backwardPacked.fValue.size()) {
    if (forwardPacked.fValue.size() > backwardPacked.fValue.size())
      noOfBins = backwardPacked.fValue.size();
  }

  // form asymmetry including error propagation
  double asym;
  double f, b, ef, eb;
  // fill data time start, and step
  // data start at data_start-t0
  fData.fDataTimeStart = fTimeResolution*(((double)start[0]-t0[0])+(double)fRunInfo->fPacking/2.0);
  fData.fDataTimeStep  = fTimeResolution*(double)fRunInfo->fPacking;
  for (unsigned int i=0; i<noOfBins; i++) {
    // to make the formulae more readable
    f  = forwardPacked.fValue[i];
    b  = backwardPacked.fValue[i];
    ef = forwardPacked.fError[i];
    eb = backwardPacked.fError[i];
    // check that there are indeed bins
    if (f+b != 0.0)
      asym = (f-b) / (f+b);
    else
      asym = 0.0;
    fData.fValue.push_back(asym);
    // calculate the error
    if (f+b != 0.0)
      error = 2.0/((f+b)*(f+b))*TMath::Sqrt(b*b*ef*ef+eb*eb*f*f);
    else
      error = 1.0;
    fData.fError.push_back(error);
  }

  // count the number of bins to be fitted
  double time;
  fNoOfFitBins=0;
  for (unsigned int i=0; i<fData.fValue.size(); i++) {
    time = fData.fDataTimeStart + (double)i * fData.fDataTimeStep;
    if ((time >= fFitStartTime) && (time <= fFitStopTime))
      fNoOfFitBins++;
  }

  // clean up
  forwardPacked.fValue.clear();
  forwardPacked.fError.clear();
  backwardPacked.fValue.clear();
  backwardPacked.fError.clear();
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
bool PRunAsymmetry::PrepareViewData(PRawRunData* runData, unsigned int histoNo[2])
{
  // feed the parameter vector
  std::vector<double> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (unsigned int i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // transform raw histo data. This is done the following way (for details see the manual):
  // first rebin the data, than calculate the asymmetry
  // first get start data, end data, and t0
  unsigned int start[2] = {(int)fT0s[0]-fRunInfo->fPacking*((int)fT0s[0]/fRunInfo->fPacking),
                           (int)fT0s[1]-fRunInfo->fPacking*((int)fT0s[1]/fRunInfo->fPacking)};
  unsigned int end[2];
  double t0[2]          = {fT0s[0], fT0s[1]};

  // make sure that there are equal number of rebinned bins in forward and backward
  unsigned int noOfBins0 = (runData->fDataBin[histoNo[0]].size()-start[0])/fRunInfo->fPacking;
  unsigned int noOfBins1 = (runData->fDataBin[histoNo[1]].size()-start[1])/fRunInfo->fPacking;
  if (noOfBins0 > noOfBins1)
    noOfBins0 = noOfBins1;
  end[0] = start[0] + noOfBins0 * fRunInfo->fPacking;
  end[1] = start[1] + noOfBins0 * fRunInfo->fPacking;

  // check if start, end, and t0 make any sense
  // 1st check if start and end are in proper order
  for (unsigned int i=0; i<2; i++) {
    if (end[i] < start[i]) { // need to swap them
      int keep = end[i];
      end[i] = start[i];
      start[i] = keep;
    }
    // 2nd check if start is within proper bounds
    if ((start[i] < 0) || (start[i] > runData->fDataBin[histoNo[i]].size())) {
      cout << endl << "PRunAsymmetry::PrepareViewData(): **ERROR** start data bin doesn't make any sense!";
      return false;
    }
    // 3rd check if end is within proper bounds
    if ((end[i] < 0) || (end[i] > runData->fDataBin[histoNo[i]].size())) {
      cout << endl << "PRunAsymmetry::PrepareViewData(): **ERROR** end data bin doesn't make any sense!";
      return false;
    }
    // 4th check if t0 is within proper bounds
    if ((t0[i] < 0) || (t0[i] > runData->fDataBin[histoNo[i]].size())) {
      cout << endl << "PRunAsymmetry::PrepareViewData(): **ERROR** t0 data bin doesn't make any sense!";
      return false;
    }
  }

  // everything looks fine, hence fill packed forward and backward histo
  PRunData forwardPacked;
  PRunData backwardPacked;
  double value = 0.0;
  double error = 0.0;
  // forward
  for (unsigned i=start[0]; i<end[0]; i++) {
    if (((i-start[0]) % fRunInfo->fPacking == 0) && (i != start[0])) { // fill data
      // in order that after rebinning the fit does not need to be redone (important for plots)
      // the value is normalize to per bin
      value /= fRunInfo->fPacking;
      forwardPacked.fValue.push_back(value);
      if (value == 0.0)
        forwardPacked.fError.push_back(1.0);
      else
        forwardPacked.fError.push_back(TMath::Sqrt(error)/fRunInfo->fPacking);
      value = 0.0;
      error = 0.0;
    }
    value += fForward[i];
    error += fForwardErr[i]*fForwardErr[i];
  }
  // backward
  for (unsigned i=start[1]; i<end[1]; i++) {
    if (((i-start[1]) % fRunInfo->fPacking == 0) && (i != start[1])) { // fill data
      // in order that after rebinning the fit does not need to be redone (important for plots)
      // the value is normalize to per bin
      value /= fRunInfo->fPacking;
      backwardPacked.fValue.push_back(value);
      if (value == 0.0)
        backwardPacked.fError.push_back(1.0);
      else
        backwardPacked.fError.push_back(TMath::Sqrt(error)/fRunInfo->fPacking);
      value = 0.0;
      error = 0.0;
    }
    value += fBackward[i];
    error += fBackwardErr[i]*fBackwardErr[i];
  }

  // check if packed forward and backward hist have the same size, otherwise take the minimum size
  unsigned int noOfBins = forwardPacked.fValue.size();
  if (forwardPacked.fValue.size() != backwardPacked.fValue.size()) {
    if (forwardPacked.fValue.size() > backwardPacked.fValue.size())
      noOfBins = backwardPacked.fValue.size();
  }

  // form asymmetry including error propagation
  double asym;
  double f, b, ef, eb, alpha = 1.0, beta = 1.0;
  // fill data time start, and step
  // data start at data_start-t0
  fData.fDataTimeStart = fTimeResolution*(((double)start[0]-t0[0])+(double)fRunInfo->fPacking/2.0);
  fData.fDataTimeStep  = fTimeResolution*(double)fRunInfo->fPacking;

  // get the proper alpha and beta
  switch (fAlphaBetaTag) {
    case 1: // alpha == 1, beta == 1
      alpha = 1.0;
      beta  = 1.0;
      break;
    case 2: // alpha != 1, beta == 1
      alpha = par[fRunInfo->fAlphaParamNo-1];
      beta  = 1.0;
      break;
    case 3: // alpha == 1, beta != 1
      alpha = 1.0;
      beta  = par[fRunInfo->fBetaParamNo-1];
      break;
    case 4: // alpha != 1, beta != 1
      alpha = par[fRunInfo->fAlphaParamNo-1];
      beta  = par[fRunInfo->fBetaParamNo-1];
      break;
    default:
      break;
  }
//cout << endl << ">> alpha = " << alpha << ", beta = " << beta;

  for (unsigned int i=0; i<noOfBins; i++) {
    // to make the formulae more readable
    f  = forwardPacked.fValue[i];
    b  = backwardPacked.fValue[i];
    ef = forwardPacked.fError[i];
    eb = backwardPacked.fError[i];
    // check that there are indeed bins
    if (f+b != 0.0)
      asym = (alpha*f-b) / (alpha*beta*f+b);
    else
      asym = 0.0;
    fData.fValue.push_back(asym);
    // calculate the error
    if (f+b != 0.0)
      error = 2.0/((f+b)*(f+b))*TMath::Sqrt(b*b*ef*ef+eb*eb*f*f);
    else
      error = 1.0;
    fData.fError.push_back(error);
  }

  // count the number of bins to be fitted
  double time;
  fNoOfFitBins=0;
  for (unsigned int i=0; i<fData.fValue.size(); i++) {
    time = fData.fDataTimeStart + (double)i * fData.fDataTimeStep;
    if ((time >= fFitStartTime) && (time <= fFitStopTime))
      fNoOfFitBins++;
  }

  // clean up
  forwardPacked.fValue.clear();
  forwardPacked.fError.clear();
  backwardPacked.fValue.clear();
  backwardPacked.fError.clear();
  fForward.clear();
  fForwardErr.clear();
  fBackward.clear();
  fBackwardErr.clear();

  // fill theory vector for kView
  // calculate functions
  for (int i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), fRunInfo->fMap, par);
  }

  // calculate theory
  unsigned int size = runData->fDataBin[histoNo[0]].size();
  double startTime  = -fT0s[0]*fTimeResolution;
  fData.fTheoryTimeStart = startTime;
  fData.fTheoryTimeStep  = fTimeResolution;
  for (unsigned int i=0; i<size; i++) {
    time = startTime + (double)i*fTimeResolution;
    value = fTheory->Func(time, par, fFuncValues);
    if (value > 10.0) {  // dirty hack needs to be fixed!!
      value = 0.0;
    }
    fData.fTheory.push_back(value);
  }

  // clean up
  par.clear();

  return true;
}
