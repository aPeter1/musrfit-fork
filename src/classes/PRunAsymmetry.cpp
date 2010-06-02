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
 * <p>Constructor
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
 * <p>Constructor
 *
 * \param msrInfo pointer to the msr-file handler
 * \param rawData raw run data
 * \param runNo number of the run within the msr-file
 * \param tag tag showing what shall be done: kFit == fitting, kView == viewing
 */
PRunAsymmetry::PRunAsymmetry(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag) : PRunBase(msrInfo, rawData, runNo, tag)
{
  // check if alpha and/or beta is fixed --------------------

  PMsrParamList *param = msrInfo->GetMsrParamList();

  // check if alpha is given
  if (fRunInfo->GetAlphaParamNo() == -1) { // no alpha given
    cerr << endl << ">> PRunAsymmetry::PRunAsymmetry(): **ERROR** no alpha parameter given! This is needed for an asymmetry fit!";
    cerr << endl;
    fValid = false;
    return;
  }
  // check if alpha parameter is within proper bounds
  if ((fRunInfo->GetAlphaParamNo() < 0) || (fRunInfo->GetAlphaParamNo() > (Int_t)param->size())) {
    cerr << endl << ">> PRunAsymmetry::PRunAsymmetry(): **ERROR** alpha parameter no = " << fRunInfo->GetAlphaParamNo();
    cerr << endl << ">> This is out of bound, since there are only " << param->size() << " parameters.";
    cerr << endl;
    fValid = false;
    return;
  }
  // check if alpha is fixed
  Bool_t alphaFixedToOne = false;
  if (((*param)[fRunInfo->GetAlphaParamNo()-1].fStep == 0.0) &&
      ((*param)[fRunInfo->GetAlphaParamNo()-1].fValue == 1.0))
    alphaFixedToOne = true;

  // check if beta is given
  Bool_t betaFixedToOne = false;
  if (fRunInfo->GetBetaParamNo() == -1) { // no beta given hence assuming beta == 1
    betaFixedToOne = true;
  } else if ((fRunInfo->GetBetaParamNo() < 0) || (fRunInfo->GetBetaParamNo() > (Int_t)param->size())) { // check if beta parameter is within proper bounds
    cerr << endl << ">> PRunAsymmetry::PRunAsymmetry(): **ERROR** beta parameter no = " << fRunInfo->GetBetaParamNo();
    cerr << endl << ">> This is out of bound, since there are only " << param->size() << " parameters.";
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

  // calculate fData
  if (!PrepareData())
    fValid = false;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor.
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
 * <p>Calculate chi-square.
 *
 * <b>return:</b>
 * - chisq value
 *
 * \param par parameter vector iterated by minuit2
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
      diff = fData.GetValue()->at(i) - asymFcnValue;
      chisq += diff*diff / (fData.GetError()->at(i)*fData.GetError()->at(i));
    }
  }

  return chisq;
}

//--------------------------------------------------------------------------
// CalcMaxLikelihood
//--------------------------------------------------------------------------
/**
 * <p>NOT IMPLEMENTED!!
 *
 * \param par parameter vector iterated by minuit2
 */
Double_t PRunAsymmetry::CalcMaxLikelihood(const std::vector<Double_t>& par)
{
  cout << endl << "PRunAsymmetry::CalcMaxLikelihood(): not implemented yet ..." << endl;

  return 1.0;
}

//--------------------------------------------------------------------------
// CalcTheory
//--------------------------------------------------------------------------
/**
 * <p>Calculate theory for a given set of fit-parameters.
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
 * <p>Prepare data for fitting or viewing. What is already processed at this stage:
 * - get all needed forward/backward histograms
 * - get time resolution
 * - get start/stop fit time
 * - get t0's and perform necessary cross checks (e.g. if t0 of msr-file (if present) are consistent with t0 of the data files, etc.)
 * - add runs (if addruns are present)
 * - group histograms (if grouping is present)
 * - subtract background
 *
 * Error propagation for \f$ A_i = (f_i^{\rm c}-b_i^{\rm c})/(f_i^{\rm c}+b_i^{\rm c})\f$:
 * \f[ \Delta A_i = \pm\frac{2}{(f_i^{\rm c}+b_i^{\rm c})^2}\left[
 *     (b_i^{\rm c})^2 (\Delta f_i^{\rm c})^2 + 
 *      (\Delta b_i^{\rm c})^2 (f_i^{\rm c})^2\right]^{1/2}\f]
 *
 * <b>return:</b>
 * - true if everthing went smooth
 * - false, otherwise.
 */
Bool_t PRunAsymmetry::PrepareData()
{
  // get forward/backward histo from PRunDataHandler object ------------------------
  // get the correct run
  PRawRunData *runData = fRawData->GetRunData(*(fRunInfo->GetRunName()));
  if (!runData) { // run not found
    cerr << endl << ">> PRunAsymmetry::PrepareData(): **ERROR** Couldn't get run " << fRunInfo->GetRunName()->Data() << "!";
    cerr << endl;
    return false;
  }

  // keep the time resolution in (us)
  fTimeResolution = runData->GetTimeResolution()/1.0e3;

  // keep start/stop time for fit
  fFitStartTime = fRunInfo->GetFitRange(0);
  fFitStopTime  = fRunInfo->GetFitRange(1);

  // collect histogram numbers
  PUIntVector forwardHistoNo;
  PUIntVector backwardHistoNo;
  for (UInt_t i=0; i<fRunInfo->GetForwardHistoNoSize(); i++) {
    forwardHistoNo.push_back(fRunInfo->GetForwardHistoNo(i)-1);

    if (runData->GetNoOfHistos() <= forwardHistoNo[i]) {
      cerr << endl << ">> PRunAsymmetry::PrepareData(): **PANIC ERROR**:";
      cerr << endl << ">> forwardHistoNo found = " << forwardHistoNo[i]+1 << ", but there are only " << runData->GetNoOfHistos() << " runs!?!?";
      cerr << endl << ">> Will quit :-(";
      cerr << endl;
      // clean up
      forwardHistoNo.clear();
      backwardHistoNo.clear();
      return false;
    }
  }
  for (UInt_t i=0; i<fRunInfo->GetBackwardHistoNoSize(); i++) {
    backwardHistoNo.push_back(fRunInfo->GetBackwardHistoNo(i)-1);

    if (runData->GetNoOfHistos() <= backwardHistoNo[i]) {
      cerr << endl << ">> PRunAsymmetry::PrepareData(): **PANIC ERROR**:";
      cerr << endl << ">> backwardHistoNo found = " << backwardHistoNo[i]+1 << ", but there are only " << runData->GetNoOfHistos() << " runs!?!?";
      cerr << endl << ">> Will quit :-(";
      cerr << endl;
      // clean up
      forwardHistoNo.clear();
      backwardHistoNo.clear();
      return false;
    }
  }
  if (forwardHistoNo.size() != backwardHistoNo.size()) {
    cerr << endl << ">> PRunAsymmetry::PrepareData(): **PANIC ERROR**:";
    cerr << endl << ">> # of forward histograms different from # of backward histograms.";
    cerr << endl << ">> Will quit :-(";
    cerr << endl;
    // clean up
    forwardHistoNo.clear();
    backwardHistoNo.clear();
    return false;
  }

  // check if the t0's are given in the msr-file
  if (fRunInfo->GetT0Size() == 0) { // t0's are NOT in the msr-file
    // check if the t0's are in the data file
    if (runData->GetT0Size() != 0) { // t0's in the run data
      // keep the proper t0's. For asymmetry runs, forward/backward are holding the histo no
      // fForwardHistoNo starts with 1 not with 0 etc. ;-)
      for (UInt_t i=0; i<forwardHistoNo.size(); i++) {
        fT0s.push_back(runData->GetT0(fRunInfo->GetForwardHistoNo(i)-1));  // forward  t0
        fT0s.push_back(runData->GetT0(fRunInfo->GetBackwardHistoNo(i)-1)); // backward t0
      }
    } else { // t0's are neither in the run data nor in the msr-file -> will try estimated ones!
      for (UInt_t i=0; i<forwardHistoNo.size(); i++) {
        fT0s.push_back(runData->GetT0Estimated(fRunInfo->GetForwardHistoNo(i)-1));
        fT0s.push_back(runData->GetT0Estimated(fRunInfo->GetBackwardHistoNo(i)-1));
        cerr << endl << ">> PRunAsymmetry::PrepareData(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
        cerr << endl << ">> run: " << fRunInfo->GetRunName()->Data();
        cerr << endl << ">> will try the estimated one: forward t0 = " << runData->GetT0Estimated(fRunInfo->GetForwardHistoNo(i)-1);
        cerr << endl << ">> will try the estimated one: backward t0 = " << runData->GetT0Estimated(fRunInfo->GetBackwardHistoNo(i)-1);
        cerr << endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
        cerr << endl;
      }
    }
  } else { // t0's in the msr-file
    for (UInt_t i=0; i<forwardHistoNo.size(); i++) {
      // check if enough t0's are given in the msr-file, if not try to get the rest from the data file
      if ((fRunInfo->GetT0Size() <= 2*i) || (fRunInfo->GetT0Size() <= 2*i+1)) { // t0 for i not present in the msr-file, i.e. #t0's != #forward histos
        if (static_cast<Int_t>(runData->GetT0Size()) > fRunInfo->GetForwardHistoNo(i)-1) { // t0 for i present in the data file
          fT0s.push_back(runData->GetT0(fRunInfo->GetForwardHistoNo(i)-1));
        } else { // t0 is neither in the run data nor in the msr-file -> will try estimated ones!
          fT0s.push_back(runData->GetT0Estimated(fRunInfo->GetForwardHistoNo(i)-1));
          cerr << endl << ">> PRunSingleHisto::PrepareData(): **WARNING** NO t0's found, neither in the run data nor in the msr-file!";
          cerr << endl << ">> run: " << fRunInfo->GetRunName()->Data();
          cerr << endl << ">> will try the estimated one: t0 = " << runData->GetT0Estimated(fRunInfo->GetForwardHistoNo(i)-1);
          cerr << endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
          cerr << endl;
        }
        if (static_cast<Int_t>(runData->GetT0Size()) > fRunInfo->GetBackwardHistoNo(i)-1) { // t0 for i present in the data file
          fT0s.push_back(runData->GetT0(fRunInfo->GetBackwardHistoNo(i)-1));
        } else { // t0 is neither in the run data nor in the msr-file -> will try estimated ones!
          fT0s.push_back(runData->GetT0Estimated(fRunInfo->GetBackwardHistoNo(i)-1));
          cerr << endl << ">> PRunSingleHisto::PrepareData(): **WARNING** NO t0's found, neither in the run data nor in the msr-file!";
          cerr << endl << ">> run: " << fRunInfo->GetRunName()->Data();
          cerr << endl << ">> will try the estimated one: t0 = " << runData->GetT0Estimated(fRunInfo->GetBackwardHistoNo(i)-1);
          cerr << endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
          cerr << endl;
        }
      } else { // # of t0's in the msr-file == # of histos in forward
        fT0s.push_back(fRunInfo->GetT0(2*i));   // forward t0
        fT0s.push_back(fRunInfo->GetT0(2*i+1)); // backward t0

        // check if t0's are given in the data file
        if (runData->GetT0Size() != 0) {
          // compare t0's of the msr-file with the one in the data file
          if (fabs(fRunInfo->GetT0(2*i)-runData->GetT0(fRunInfo->GetForwardHistoNo(i)-1))>5.0) {
            cerr << endl << ">> PRunAsymmetry::PrepareData(): **WARNING**: forward histo #" << i+1;
            cerr << endl << ">> t0 from the msr-file is  " << fRunInfo->GetT0(2*i)+1;
            cerr << endl << ">> t0 from the data file is " << runData->GetT0(fRunInfo->GetForwardHistoNo(i)-1);
            cerr << endl << ">> This is quite a deviation! Is this done intentionally??";
            cerr << endl;
          }
          if (fabs(fRunInfo->GetT0(2*i+1)-runData->GetT0(fRunInfo->GetBackwardHistoNo(i)-1))>5.0) { // given in bins!!
            cerr << endl << ">> PRunAsymmetry::PrepareData(): **WARNING**: backward histo #" << i+1;
            cerr << endl << ">> t0 from the msr-file is  " << fRunInfo->GetT0(2*i+1)+1;
            cerr << endl << ">> t0 from the data file is " << runData->GetT0(fRunInfo->GetBackwardHistoNo(i)-1);
            cerr << endl << ">> This is quite a deviation! Is this done intentionally??";
            cerr << endl;
          }
        }
      }
    }
  }

  // get raw forward/backward histo data
  fForward.resize(runData->GetDataBin(forwardHistoNo[0])->size());
  fBackward.resize(runData->GetDataBin(backwardHistoNo[0])->size());
  fForward = *runData->GetDataBin(forwardHistoNo[0]);
  fBackward = *runData->GetDataBin(backwardHistoNo[0]);

  // check if addrun's are present, and if yes add data
  // check if there are runs to be added to the current one
  if (fRunInfo->GetRunNameSize() > 1) { // runs to be added present
    PRawRunData *addRunData;
    for (UInt_t i=1; i<fRunInfo->GetRunNameSize(); i++) {
      // get run to be added to the main one
      addRunData = fRawData->GetRunData(*(fRunInfo->GetRunName(i)));
      if (addRunData == 0) { // couldn't get run
        cerr << endl << ">> PRunAsymmetry::PrepareData(): **ERROR** Couldn't get addrun " << fRunInfo->GetRunName(i)->Data() << "!";
        cerr << endl;
        return false;
      }

      // get T0's of the to be added run
      PUIntVector t0Add;
      // check if the t0's are given in the msr-file
      if (i > fRunInfo->GetAddT0Entries()) { // t0's are NOT in the msr-file
        // check if the t0's are in the data file
        if (addRunData->GetT0Size() != 0) { // t0's in the run data
          // keep the proper t0's. For asymmetry runs, forward/backward are holding the histo no
          // fForwardHistoNo starts with 1 not with 0 etc. ;-)
          for (UInt_t j=0; j<fRunInfo->GetForwardHistoNoSize(); j++) {
            t0Add.push_back(addRunData->GetT0(fRunInfo->GetForwardHistoNo(j)-1));  // forward  t0
            t0Add.push_back(addRunData->GetT0(fRunInfo->GetBackwardHistoNo(j)-1)); // backward t0
          }
        } else { // t0's are neither in the run data nor in the msr-file -> will try estimated ones!
          for (UInt_t j=0; j<fRunInfo->GetForwardHistoNoSize(); j++) {
            t0Add.push_back(addRunData->GetT0Estimated(fRunInfo->GetForwardHistoNo(j)-1));
            t0Add.push_back(addRunData->GetT0Estimated(fRunInfo->GetBackwardHistoNo(j)-1));
            cerr << endl << ">> PRunAsymmetry::PrepareData(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
            cerr << endl << ">> addRun: " << fRunInfo->GetRunName(i)->Data();
            cerr << endl << ">> will try the estimated one: forward t0 = " << addRunData->GetT0Estimated(fRunInfo->GetForwardHistoNo(j)-1);
            cerr << endl << ">> will try the estimated one: backward t0 = " << addRunData->GetT0Estimated(fRunInfo->GetBackwardHistoNo(j)-1);
            cerr << endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
            cerr << endl;
          }
        }
      } else { // t0's in the msr-file
        for (UInt_t j=0; j<fRunInfo->GetForwardHistoNoSize(); j++) {
          // check if t0's are given in the data file
          if (addRunData->GetT0Size() != 0) {
            // compare t0's of the msr-file with the one in the data file
            if (fabs(fRunInfo->GetAddT0(i-1,2*j)-addRunData->GetT0(fRunInfo->GetForwardHistoNo(j)-1))>5.0) { // given in bins!!
              cerr << endl << ">> PRunAsymmetry::PrepareData(): **WARNING**:";
              cerr << endl << ">> t0 from the msr-file is  " << fRunInfo->GetAddT0(i-1,2*j)+1; // +1 since vector starts at 0
              cerr << endl << ">> t0 from the data file is " << addRunData->GetT0(fRunInfo->GetForwardHistoNo(j)-1);
              cerr << endl << ">> This is quite a deviation! Is this done intentionally??";
              cerr << endl << ">> addrun: " << fRunInfo->GetRunName(i)->Data();
              cerr << endl;
            }
            if (fabs(fRunInfo->GetAddT0(i-1,2*j+1)-addRunData->GetT0(fRunInfo->GetBackwardHistoNo(j)-1))>5.0) { // given in bins!!
              cerr << endl << ">> PRunAsymmetry::PrepareData(): **WARNING**:";
              cerr << endl << ">> t0 from the msr-file is  " << fRunInfo->GetAddT0(i-1,2*j+1)+1; // +1 since vector starts at 0
              cerr << endl << ">> t0 from the data file is " << addRunData->GetT0(fRunInfo->GetBackwardHistoNo(j)-1);
              cerr << endl << ">> This is quite a deviation! Is this done intentionally??";
              cerr << endl << ">> addrun: " << fRunInfo->GetRunName(i)->Data();
              cerr << endl;
            }
          }
          if (i <= fRunInfo->GetAddT0Entries()) {
            if (2*j+1 < static_cast<UInt_t>(fRunInfo->GetAddT0Size(i-1))) {
              t0Add.push_back(fRunInfo->GetAddT0(i-1,2*j));
              t0Add.push_back(fRunInfo->GetAddT0(i-1,2*j+1));
            } else {
              cerr << endl << ">> PRunSingleHisto::PrepareData(): **WARNING** NO t0's found, neither in the addrun data (";
              cerr << fRunInfo->GetRunName(i)->Data();
              cerr << "), nor in the msr-file! Will try to use the T0 of the run data (";
              cerr << fRunInfo->GetRunName(i)->Data();
              cerr << ") without any warranty!";
              cerr << endl;
              t0Add.push_back(fRunInfo->GetT0(2*j));
              t0Add.push_back(fRunInfo->GetT0(2*j+1));
            }
          }
        }
      }

      // add forward run
      UInt_t addRunSize;
      for (UInt_t k=0; k<forwardHistoNo.size(); k++) {
        addRunSize = addRunData->GetDataBin(forwardHistoNo[k])->size();
        for (UInt_t j=0; j<runData->GetDataBin(forwardHistoNo[k])->size(); j++) {
          // make sure that the index stays in the proper range
          if ((j+t0Add[2*k]-fT0s[2*k] >= 0) && (j+t0Add[2*k]-fT0s[2*k] < addRunSize)) {
            fForward[j] += addRunData->GetDataBin(forwardHistoNo[k])->at(j+t0Add[2*k]-fT0s[2*k]);
          }
        }
      }

      // add backward run
      for (UInt_t k=0; k<backwardHistoNo.size(); k++) {
        addRunSize = addRunData->GetDataBin(backwardHistoNo[k])->size();
        for (UInt_t j=0; j<runData->GetDataBin(backwardHistoNo[k])->size(); j++) {
          // make sure that the index stays in the proper range
          if ((j+t0Add[2*k+1]-fT0s[2*k+1] >= 0) && (j+t0Add[2*k+1]-fT0s[2*k+1] < addRunSize)) {
            fBackward[j] += addRunData->GetDataBin(backwardHistoNo[k])->at(j+t0Add[2*k+1]-fT0s[2*k+1]);
          }
        }
      }

      // clean up
      t0Add.clear();
    }
  }

  // group histograms, add all the forward histograms to the one with forwardHistoNo[0]
  for (UInt_t i=1; i<forwardHistoNo.size(); i++) {
    for (UInt_t j=0; j<runData->GetDataBin(forwardHistoNo[i])->size(); j++) {
      // make sure that the index stays within proper range
      if ((j+fT0s[2*i]-fT0s[0] >= 0) && (j+fT0s[2*i]-fT0s[0] < runData->GetDataBin(forwardHistoNo[i])->size())) {
        fForward[j] += runData->GetDataBin(forwardHistoNo[i])->at(j+fT0s[2*i]-fT0s[0]);
      }
    }
  }
  // group histograms, add all the backward histograms to the one with backwardHistoNo[0]
  for (UInt_t i=1; i<backwardHistoNo.size(); i++) {
    for (UInt_t j=0; j<runData->GetDataBin(backwardHistoNo[i])->size(); j++) {
      // make sure that the index stays within proper range
      if ((j+fT0s[2*i+1]-fT0s[1] >= 0) && (j+fT0s[2*i+1]-fT0s[1] < runData->GetDataBin(backwardHistoNo[i])->size())) {
        fBackward[j] += runData->GetDataBin(backwardHistoNo[i])->at(j+fT0s[2*i+1]-fT0s[1]);
      }
    }
  }

  // subtract background from histogramms ------------------------------------------
  if (fRunInfo->GetBkgFix(0) == PMUSR_UNDEFINED) { // no fixed background given
    if (fRunInfo->GetBkgRange(0) >= 0) {
      if (!SubtractEstimatedBkg())
        return false;
    } else { // no background given to do the job, try to estimate it
      fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.1), 0);
      fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.6), 1);
      fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[1]*0.1), 2);
      fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[1]*0.6), 3);
      cerr << endl << ">> PRunAsymmetry::PrepareData(): **WARNING** Neither fix background nor background bins are given!";
      cerr << endl << ">> Will try the following:";
      cerr << endl << ">> forward:  bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
      cerr << endl << ">> backward: bkg start = " << fRunInfo->GetBkgRange(2) << ", bkg end = " << fRunInfo->GetBkgRange(3);
      cerr << endl << ">> NO WARRANTY THAT THIS MAKES ANY SENSE! Better check ...";
      cerr << endl;
      if (!SubtractEstimatedBkg())
        return false;
    }
  } else { // fixed background given
    if (!SubtractFixBkg())
      return false;
  }

  // everything looks fine, hence fill data set
  UInt_t histoNo[2] = {forwardHistoNo[0], backwardHistoNo[0]};
  Bool_t status;
  switch(fHandleTag) {
    case kFit:
      status = PrepareFitData(runData, histoNo);
      break;
    case kView:
      if (fMsrInfo->GetMsrPlotList()->at(0).fRRFPacking == 0)
        status = PrepareViewData(runData, histoNo);
      else
        status = PrepareRRFViewData(runData, histoNo);
      break;
    default:
      status = false;
      break;
  }

  // clean up
  forwardHistoNo.clear();
  backwardHistoNo.clear();

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
 *
 * <b>return:</b>
 * - true
 *
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
 * <p>Subtracts the background which is estimated from a given interval (typically before t0).
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
 *
 * <b>return:</b>
 * - true
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
    cerr << endl << ">> PRunAsymmetry::SubtractEstimatedBkg(): **ERROR** background bin values out of bound!";
    cerr << endl << ">> histo lengths (f/b)  = (" << fForward.size() << "/" << fBackward.size() << ").";
    cerr << endl << ">> background start (f/b) = (" << start[0] << "/" << start[1] << ").";
    return false;
  }

  // check if end is within histogram bounds
  if ((end[0] < 0) || (end[0] >= fForward.size()) ||
      (end[1] < 0) || (end[1] >= fBackward.size())) {
    cerr << endl << ">> PRunAsymmetry::SubtractEstimatedBkg(): **ERROR** background bin values out of bound!";
    cerr << endl << ">> histo lengths (f/b)  = (" << fForward.size() << "/" << fBackward.size() << ").";
    cerr << endl << ">> background end (f/b) = (" << end[0] << "/" << end[1] << ").";
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

  // backward
  for (UInt_t i=start[1]; i<end[1]; i++)
    bkg[1] += fBackward[i];
  errBkg[1] = TMath::Sqrt(bkg[1])/(end[0] - start[0] + 1);
  bkg[1] /= static_cast<Double_t>(end[1] - start[1] + 1);

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
 * <p>Take the pre-processed data (i.e. grouping and addrun are preformed) and form the asymmetry for fitting.
 * Before forming the asymmetry, the following checks will be performed:
 * -# check if data range is given, if not try to estimate one.
 * -# check that data range is present, that it makes any sense.
 * -# check that 'first good bin'-'t0' is the same for forward and backward histogram. If not adjust it.
 * -# pack data (rebin).
 * -# if packed forward size != backward size, truncate the longer one such that an asymmetry can be formed.
 * -# calculate the asymmetry: \f$ A_i = (f_i^c-b_i^c)/(f_i^c+b_i^c) \f$
 * -# calculate the asymmetry errors: \f$ \delta A_i = 2 \sqrt{(b_i^c)^2 (\delta f_i^c)^2 + (\delta b_i^c)^2 (f_i^c)^2}/(f_i^c+b_i^c)^2\f$
 *
 * \param runData raw run data needed to perform some crosschecks
 * \param histoNo histogram number (within a run). histoNo[0]: forward histogram number, histNo[1]: backward histogram number
 */
Bool_t PRunAsymmetry::PrepareFitData(PRawRunData* runData, UInt_t histoNo[2])
{
  // transform raw histo data. This is done the following way (for details see the manual):
  // first rebin the data, than calculate the asymmetry
  // first get start data, end data, and t0
  Int_t start[2] = {fRunInfo->GetDataRange(0), fRunInfo->GetDataRange(2)};
  Int_t end[2]   = {fRunInfo->GetDataRange(1), fRunInfo->GetDataRange(3)};
  Double_t t0[2] = {fT0s[0], fT0s[1]};
  // check if data range has been provided, and if not try to estimate them
  if (start[0] < 0) {
    start[0] = (Int_t)t0[0]+5;
    cerr << endl << ">> PRunAsymmetry::PrepareData(): **WARNING** data range (forward) was not provided, will try data range start = t0+5 = " << start[0] << ".";
    cerr << endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  }
  if (start[1] < 0) {
    start[1] = (Int_t)t0[1]+5;
    cerr << endl << ">> PRunAsymmetry::PrepareData(): **WARNING** data range (backward) was not provided, will try data range start = t0+5 = " << start[1] << ".";
    cerr << endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  }
  if (end[0] < 0) {
    end[0] = runData->GetDataBin(histoNo[0])->size();
    cerr << endl << ">> PRunAsymmetry::PrepareData(): **WARNING** data range (forward) was not provided, will try data range end = " << end[0] << ".";
    cerr << endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  }
  if (end[1] < 0) {
    end[1] = runData->GetDataBin(histoNo[1])->size();
    cerr << endl << ">> PRunAsymmetry::PrepareData(): **WARNING** data range (backward) was not provided, will try data range end = " << end[1] << ".";
    cerr << endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  }
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
      cerr << endl << ">> PRunAsymmetry::PrepareFitData(): **ERROR** start data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 3rd check if end is within proper bounds
    if ((end[i] < 0) || (end[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << ">> PRunAsymmetry::PrepareFitData(): **ERROR** end data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 4th check if t0 is within proper bounds
    if ((t0[i] < 0) || (t0[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << ">> PRunAsymmetry::PrepareFitData(): **ERROR** t0 data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
  }

  // check that start-t0 is the same for forward as for backward, otherwise take max(start[i]-t0[i])
  if (fabs(static_cast<Double_t>(start[0])-t0[0]) > fabs(static_cast<Double_t>(start[1])-t0[1])){
    start[1] = static_cast<Int_t>(t0[1] + static_cast<Double_t>(start[0]) - t0[0]);
    end[1] = static_cast<Int_t>(t0[1] + static_cast<Double_t>(end[0]) - t0[0]);
    cerr << endl << ">> PRunAsymmetry::PrepareFitData **WARNING** needed to shift backward data range.";
    cerr << endl << ">> given: " << fRunInfo->GetDataRange(2) << ", " << fRunInfo->GetDataRange(3);
    cerr << endl << ">> used : " << start[1] << ", " << end[1];
    cerr << endl;
  }
  if (fabs(static_cast<Double_t>(start[0])-t0[0]) < fabs(static_cast<Double_t>(start[1])-t0[1])){
    start[0] = static_cast<Int_t>(t0[0] + static_cast<Double_t>(start[1]) - t0[1]);
    end[0] = static_cast<Int_t>(t0[0] + static_cast<Double_t>(end[1]) - t0[1]);
    cerr << endl << ">> PRunAsymmetry::PrepareFitData **WARNING** needed to shift forward data range.";
    cerr << endl << ">> given: " << fRunInfo->GetDataRange(0) << ", " << fRunInfo->GetDataRange(1);
    cerr << endl << ">> used : " << start[0] << ", " << end[0];
    cerr << endl;
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
 * <p>Take the pre-processed data (i.e. grouping and addrun are preformed) and form the asymmetry for view representation.
 * Before forming the asymmetry, the following checks will be performed:
 * -# check if view packing is whished.
 * -# check if data range is given, if not try to estimate one.
 * -# check that data range is present, that it makes any sense.
 * -# check that 'first good bin'-'t0' is the same for forward and backward histogram. If not adjust it.
 * -# pack data (rebin).
 * -# if packed forward size != backward size, truncate the longer one such that an asymmetry can be formed.
 * -# calculate the asymmetry: \f$ A_i = (\alpha f_i^c-b_i^c)/(\alpha \beta f_i^c+b_i^c) \f$
 * -# calculate the asymmetry errors: \f$ \delta A_i = 2 \sqrt{(b_i^c)^2 (\delta f_i^c)^2 + (\delta b_i^c)^2 (f_i^c)^2}/(f_i^c+b_i^c)^2\f$
 * -# calculate the theory vector.
 *
 * \param runData raw run data needed to perform some crosschecks
 * \param histoNo histogram number (within a run). histoNo[0]: forward histogram number, histNo[1]: backward histogram number
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
  Int_t start[2] = {0, 0};
  Int_t end[2] = {0, 0};
  Double_t t0[2] = {fT0s[0], fT0s[1]};
  // check if data range has been provided, and if not try to estimate them
  if (fRunInfo->GetDataRange(0) < 0) {
    start[0] = (static_cast<Int_t>(t0[0])+5) - ((static_cast<Int_t>(t0[0])+5)/packing)*packing;
    cerr << endl << ">> PRunAsymmetry::PrepareViewData(): **WARNING** data range (forward) was not provided, will try data range start = " << start[0] << ".";
    cerr << endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  } else if (fRunInfo->GetDataRange(2) < 0) {
    start[1] = (static_cast<Int_t>(t0[1])+5) - ((static_cast<Int_t>(t0[1])+5)/packing)*packing;
    cerr << endl << ">> PRunAsymmetry::PrepareViewData(): **WARNING** data range (backward) was not provided, will try data range start = " << start[1] << ".";
    cerr << endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  } else {
    // calculate the max(data range start[i]-t0[i])
    Int_t diff = fRunInfo->GetDataRange(0)-static_cast<Int_t>(t0[0]);
    if (abs(diff) < abs(fRunInfo->GetDataRange(2)-static_cast<Int_t>(t0[1])))
      diff = fRunInfo->GetDataRange(0)-static_cast<Int_t>(t0[0]);

    // calculate start position for plotting
    Int_t val = static_cast<Int_t>(t0[1])+diff-packing*((static_cast<Int_t>(t0[1])+diff)/packing);
    do {
      if (static_cast<Double_t>(val)+t0[1]-t0[0] < 0.0)
        val += packing;
    } while (static_cast<Double_t>(val) + t0[1] - t0[0] < 0.0);

    start[0] = val;
    start[1] = val + static_cast<Int_t>(t0[1] - t0[0]);
  }

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
      cerr << endl << ">> PRunAsymmetry::PrepareViewData(): **ERROR** start data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 3rd check if end is within proper bounds
    if ((end[i] < 0) || (end[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << ">> PRunAsymmetry::PrepareViewData(): **ERROR** end data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 4th check if t0 is within proper bounds
    if ((t0[i] < 0) || (t0[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << ">> PRunAsymmetry::PrepareViewData(): **ERROR** t0 data bin doesn't make any sense!";
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
      value += fForward[i];
      error += fForwardErr[i]*fForwardErr[i];
    }
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
  Double_t f, b, ef, eb, alpha = 1.0, beta = 1.0;
  // set data time start, and step
  // data start at data_start-t0
  fData.SetDataTimeStart(fTimeResolution*((Double_t)start[0]-t0[0]+(Double_t)(packing-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*(Double_t)packing);

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

//--------------------------------------------------------------------------
// PrepareRRFViewData
//--------------------------------------------------------------------------
/**
 * <p> Prepares the RRF data set for visual representation. This is done the following way:
 * -# make all necessary checks
 * -# build the asymmetry, \f$ A(t) \f$, WITHOUT packing.
 * -# \f$ A_R(t) = A(t) \cdot 2 \cos(\omega_R t + \phi_R) \f$
 * -# do the packing of \f$ A_R(t) \f$
 * -# calculate theory, \f$ T(t) \f$, as close as possible to the time resolution [compatible with the RRF frequency]
 * -# \f$ T_R(t) = T(t) \cdot 2 \cos(\omega_R t + \phi_R) \f$
 * -# do the packing of \f$ T_R(t) \f$
 * -# calculate the Kaiser FIR filter coefficients
 * -# filter \f$ T_R(t) \f$.
 *
 * \param runData raw run data needed to perform some crosschecks
 * \param histoNo array of the histo numbers form which to build the asymmetry
 */
Bool_t PRunAsymmetry::PrepareRRFViewData(PRawRunData* runData, UInt_t histoNo[2])
{
  // feed the parameter vector
  std::vector<Double_t> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (UInt_t i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // ------------------------------------------------------------
  // 1. make all necessary checks
  // ------------------------------------------------------------

  // first get start data, end data, and t0
  Int_t start[2] = {0, 0};
  Int_t end[2] = {0, 0};
  Double_t t0[2] = {fT0s[0], fT0s[1]};
  UInt_t packing = fMsrInfo->GetMsrPlotList()->at(0).fRRFPacking;
  // check if data range has been provided, and if not try to estimate them
  if (fRunInfo->GetDataRange(0) < 0) {
    start[0] = static_cast<Int_t>(t0[0])+5;
    cerr << endl << ">> PRunAsymmetry::PrepareRRFViewData(): **WARNING** data range (forward) was not provided, will try data range start = " << start[0] << ".";
    cerr << endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  } else if (fRunInfo->GetDataRange(2) < 0) {
    start[1] = static_cast<Int_t>(t0[1])+5;
    cerr << endl << ">> PRunAsymmetry::PrepareRRFViewData(): **WARNING** data range (backward) was not provided, will try data range start = " << start[1] << ".";
    cerr << endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  } else {
    Int_t val = fRunInfo->GetDataRange(0)-packing*(fRunInfo->GetDataRange(0)/packing);
    do {
      if (fRunInfo->GetDataRange(2) - fRunInfo->GetDataRange(0) < 0)
        val += packing;
    } while (val + fRunInfo->GetDataRange(2) - fRunInfo->GetDataRange(0) < 0);

    start[0] = val;
    start[1] = val + fRunInfo->GetDataRange(2) - fRunInfo->GetDataRange(0);
  }

  // make sure that there are equal number of rebinned bins in forward and backward
  UInt_t noOfBins0 = runData->GetDataBin(histoNo[0])->size()-start[0];
  UInt_t noOfBins1 = runData->GetDataBin(histoNo[1])->size()-start[1];
  if (noOfBins0 > noOfBins1)
    noOfBins0 = noOfBins1;
  end[0] = start[0] + noOfBins0;
  end[1] = start[1] + noOfBins0;

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
      cerr << endl << ">> PRunAsymmetry::PrepareRRFViewData(): **ERROR** start data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 3rd check if end is within proper bounds
    if ((end[i] < 0) || (end[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << ">> PRunAsymmetry::PrepareRRFViewData(): **ERROR** end data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 4th check if t0 is within proper bounds
    if ((t0[i] < 0) || (t0[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << ">> PRunAsymmetry::PrepareRRFViewData(): **ERROR** t0 data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
  }

  // ------------------------------------------------------------
  // 2. build the asymmetry [A(t)] WITHOUT packing.
  // ------------------------------------------------------------

  PDoubleVector forward, forwardErr;
  PDoubleVector backward, backwardErr;
  Double_t error = 0.0;
  // forward
  for (Int_t i=start[0]; i<end[0]; i++) {
    forward.push_back(fForward[i]);
    forwardErr.push_back(fForwardErr[i]);
  }
  // backward
  for (Int_t i=start[1]; i<end[1]; i++) {
    backward.push_back(fBackward[i]);
    backwardErr.push_back(fBackwardErr[i]);
  }

  // check if packed forward and backward hist have the same size, otherwise take the minimum size
  UInt_t noOfBins = forward.size();
  if (forward.size() != backward.size()) {
    if (forward.size() > backward.size())
      noOfBins = backward.size();
  }

  // form asymmetry including error propagation
  PDoubleVector asymmetry, asymmetryErr;
  Double_t asym;
  Double_t f, b, ef, eb, alpha = 1.0, beta = 1.0;

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

  for (UInt_t i=0; i<noOfBins; i++) {
    // to make the formulae more readable
    f  = forward[i];
    b  = backward[i];
    ef = forwardErr[i];
    eb = backwardErr[i];
    // check that there are indeed bins
    if (f+b != 0.0)
      asym = (alpha*f-b) / (alpha*beta*f+b);
    else
      asym = 0.0;
    asymmetry.push_back(asym);
    // calculate the error
    if (f+b != 0.0)
      error = 2.0/((f+b)*(f+b))*TMath::Sqrt(b*b*ef*ef+eb*eb*f*f);
    else
      error = 1.0;
    asymmetryErr.push_back(error);
  }

  // clean up
  fForward.clear();
  fForwardErr.clear();
  fBackward.clear();
  fBackwardErr.clear();


  // ------------------------------------------------------------
  // 3. A_R(t) = A(t) * 2 cos(w_R t + phi_R)
  // ------------------------------------------------------------

  // check which units shall be used
  Double_t gammaRRF = 0.0, wRRF = 0.0, phaseRRF = 0.0;
  Double_t time;

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

  for (UInt_t i=0; i<asymmetry.size(); i++) {
    time = fTimeResolution*(static_cast<Double_t>(start[0])-t0[0]+static_cast<Double_t>(i));
    asymmetry[i] *= 2.0*TMath::Cos(wRRF*time+phaseRRF);
  }

  // ------------------------------------------------------------
  // 4. do the packing of A_R(t)
  // ------------------------------------------------------------
  Double_t value = 0.0;
  error = 0.0;
  for (UInt_t i=0; i<asymmetry.size(); i++) {
    if ((i % packing == 0) && (i != 0)) {
      value /= packing;
      fData.AppendValue(value);
      fData.AppendErrorValue(TMath::Sqrt(error)/packing);

      value = 0.0;
      error = 0.0;
    }
    value += asymmetry[i];
    error += asymmetryErr[i]*asymmetryErr[i];
  }

  // set data time start, and step
  // data start at data_start-t0
  fData.SetDataTimeStart(fTimeResolution*(static_cast<Double_t>(start[0])-t0[0]+static_cast<Double_t>(packing-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*static_cast<Double_t>(packing));

  // ------------------------------------------------------------
  // 5. calculate theory [T(t)] as close as possible to the time resolution [compatible with the RRF frequency]
  // 6. T_R(t) = T(t) * 2 cos(w_R t + phi_R)
  // ------------------------------------------------------------
  UInt_t rebinRRF = static_cast<UInt_t>((TMath::Pi()/2.0/wRRF)/fTimeResolution); // RRF time resolution / data time resolution
  fData.SetTheoryTimeStart(fData.GetDataTimeStart());
  fData.SetTheoryTimeStep(TMath::Pi()/2.0/wRRF/rebinRRF); // = theory time resolution as close as possible to the data time resolution compatible with wRRF

/*
cout << endl << ">> rebinRRF = " << rebinRRF;
cout << endl << ">> theory time start = " << fData.GetTheoryTimeStart();
cout << endl << ">> theory time step  = " << fData.GetTheoryTimeStep();
cout << endl;
*/

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par);
  }

  Double_t theoryValue;
  for (UInt_t i=0; i<asymmetry.size(); i++) {
    time = fData.GetTheoryTimeStart() + static_cast<Double_t>(i)*fData.GetTheoryTimeStep();
    theoryValue = fTheory->Func(time, par, fFuncValues);
    theoryValue *= 2.0*TMath::Cos(wRRF * time + phaseRRF);

    if (fabs(theoryValue) > 10.0) { // dirty hack needs to be fixed!!
      theoryValue = 0.0;
    }

    fData.AppendTheoryValue(theoryValue);
  }

  // ------------------------------------------------------------
  // 7. do the packing of T_R(t)
  // ------------------------------------------------------------

  PDoubleVector theo;
  Double_t dval = 0.0;
  for (UInt_t i=0; i<fData.GetTheory()->size(); i++) {
    if ((i % rebinRRF == 0) && (i != 0)) {
//cout << endl << "time = " << fData.GetTheoryTimeStart() + i * fData.GetTheoryTimeStep() << ", theory value = " << dval;
      theo.push_back(dval/rebinRRF);
      dval = 0.0;
    }
    dval += fData.GetTheory()->at(i);
  }
  fData.ReplaceTheory(theo);
  theo.clear();

  // set the theory time start and step size
  fData.SetTheoryTimeStart(fData.GetTheoryTimeStart()+static_cast<Double_t>(rebinRRF-1)*fData.GetTheoryTimeStep()/2.0);
  fData.SetTheoryTimeStep(rebinRRF*fData.GetTheoryTimeStep());

  // ------------------------------------------------------------
  // 8. calculate the Kaiser FIR filter coefficients
  // ------------------------------------------------------------
  CalculateKaiserFilterCoeff(wRRF, 60.0, 0.2); // w_c = wRRF, A = -20 log_10(delta), Delta w / w_c = (w_s - w_p) / (2 w_c)

  // ------------------------------------------------------------
  // 9. filter T_R(t)
  // ------------------------------------------------------------
  FilterTheo();

  // clean up
  par.clear();
  forward.clear();
  forwardErr.clear();
  backward.clear();
  backwardErr.clear();
  asymmetry.clear();
  asymmetryErr.clear();

  return true;
}
