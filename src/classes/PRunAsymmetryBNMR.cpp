/***************************************************************************

  PRunAsymmetryBNMR.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2016 by Andreas Suter                              *
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

#include <stdio.h>

#include <iostream>
using namespace std;

#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>

#include "PMusr.h"
#include "PRunAsymmetryBNMR.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 */
PRunAsymmetryBNMR::PRunAsymmetryBNMR() : PRunBase()
{
  fNoOfFitBins  = 0;
  fPacking = -1;

  // the 2 following variables are need in case fit range is given in bins, and since
  // the fit range can be changed in the command block, these variables need to be accessible
  fGoodBins[0] = -1;
  fGoodBins[1] = -1;

  fStartTimeBin = -1;
  fEndTimeBin   = -1;
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
PRunAsymmetryBNMR::PRunAsymmetryBNMR(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag) : PRunBase(msrInfo, rawData, runNo, tag)
{
  // the 2 following variables are need in case fit range is given in bins, and since
  // the fit range can be changed in the command block, these variables need to be accessible
  fGoodBins[0] = -1;
  fGoodBins[1] = -1;

  fStartTimeBin = -1;
  fEndTimeBin   = -1;

  fPacking = fRunInfo->GetPacking();
  if (fPacking == -1) { // i.e. packing is NOT given in the RUN-block, it must be given in the GLOBAL-block
    fPacking = fMsrInfo->GetMsrGlobal()->GetPacking();
  }
  if (fPacking == -1) { // this should NOT happen, somethin is severely wrong
    cerr << endl << ">> PRunAsymmetryBNMR::PRunAsymmetryBNMR(): **SEVERE ERROR**: Couldn't find any packing information!";
    cerr << endl << ">> This is very bad :-(, will quit ...";
    cerr << endl;
    fValid = false;
    return;
  }

  // check if alpha and/or beta is fixed --------------------

  PMsrParamList *param = msrInfo->GetMsrParamList();

  // check if alpha is given
  if (fRunInfo->GetAlphaParamNo() == -1) { // no alpha given
    cerr << endl << ">> PRunAsymmetryBNMR::PRunAsymmetryBNMR(): **ERROR** no alpha parameter given! This is needed for an asymmetry fit!";
    cerr << endl;
    fValid = false;
    return;
  }
  // check if alpha parameter is within proper bounds
  if ((fRunInfo->GetAlphaParamNo() < 0) || (fRunInfo->GetAlphaParamNo() > (Int_t)param->size())) {
    cerr << endl << ">> PRunAsymmetryBNMR::PRunAsymmetryBNMR(): **ERROR** alpha parameter no = " << fRunInfo->GetAlphaParamNo();
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
    cerr << endl << ">> PRunAsymmetryBNMR::PRunAsymmetryBNMR(): **ERROR** beta parameter no = " << fRunInfo->GetBetaParamNo();
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
PRunAsymmetryBNMR::~PRunAsymmetryBNMR()
{
  fForwardp.clear();
  fForwardpErr.clear();
  fBackwardp.clear();
  fBackwardpErr.clear();

  fForwardm.clear();
  fForwardmErr.clear();
  fBackwardm.clear();
  fBackwardmErr.clear();
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
Double_t PRunAsymmetryBNMR::CalcChiSquare(const std::vector<Double_t>& par)
{
  Double_t chisq = 0.0;
  Double_t diff = 0.0;
  Double_t asymFcnValue = 0.0;
  Double_t a, b, f;

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par);
  }

  // calculate chi square
  Double_t time(1.0);
  Int_t i;

  // Calculate the theory function once to ensure one function evaluation for the current set of parameters.
  // This is needed for the LF and user functions where some non-thread-save calculations only need to be calculated once
  // for a given set of parameters---which should be done outside of the parallelized loop.
  // For all other functions it means a tiny and acceptable overhead.
  asymFcnValue = fTheory->Func(time, par, fFuncValues);

  #ifdef HAVE_GOMP
  Int_t chunk = (fEndTimeBin - fStartTimeBin)/omp_get_num_procs();
  if (chunk < 10)
    chunk = 10;
  #pragma omp parallel for default(shared) private(i,time,diff,asymFcnValue,a,b,f) schedule(dynamic,chunk) reduction(+:chisq)
  #endif
  for (i=fStartTimeBin; i<fEndTimeBin; ++i) {
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
    diff = fData.GetValue()->at(i) - asymFcnValue;
    chisq += diff*diff / (fData.GetError()->at(i)*fData.GetError()->at(i));
  }

  return chisq;
}

//--------------------------------------------------------------------------
// CalcChiSquareExpected (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate expected chi-square. Currently not implemented since not clear what to be done.
 *
 * <b>return:</b>
 * - chisq value == 0.0
 *
 * \param par parameter vector iterated by minuit2
 */
Double_t PRunAsymmetryBNMR::CalcChiSquareExpected(const std::vector<Double_t>& par)
{
  return 0.0;
}

//--------------------------------------------------------------------------
// CalcMaxLikelihood (public)
//--------------------------------------------------------------------------
/**
 * <p>NOT IMPLEMENTED!!
 *
 * \param par parameter vector iterated by minuit2
 */
Double_t PRunAsymmetryBNMR::CalcMaxLikelihood(const std::vector<Double_t>& par)
{
  cout << endl << "PRunAsymmetryBNMR::CalcMaxLikelihood(): not implemented yet ..." << endl;

  return 1.0;
}

//--------------------------------------------------------------------------
// GetNoOfFitBins (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate the number of fitted bins for the current fit range.
 *
 * <b>return:</b> number of fitted bins.
 */
UInt_t PRunAsymmetryBNMR::GetNoOfFitBins()
{
  CalcNoOfFitBins();

  return fNoOfFitBins;
}

//--------------------------------------------------------------------------
// SetFitRangeBin (public)
//--------------------------------------------------------------------------
/**
 * <p>Allows to change the fit range on the fly. Used in the COMMAND block.
 * The syntax of the string is: FIT_RANGE fgb[+n00] lgb[-n01] [fgb[+n10] lgb[-n11] ... fgb[+nN0] lgb[-nN1]].
 * If only one pair of fgb/lgb is given, it is used for all runs in the RUN block section.
 * If multiple fgb/lgb's are given, the number N has to be the number of RUN blocks in
 * the msr-file.
 *
 * <p>nXY are offsets which can be used to shift, limit the fit range.
 *
 * \param fitRange string containing the necessary information.
 */
void PRunAsymmetryBNMR::SetFitRangeBin(const TString fitRange)
{
  TObjArray *tok = 0;
  TObjString *ostr = 0;
  TString str;
  Ssiz_t idx = -1;
  Int_t offset = 0;

  tok = fitRange.Tokenize(" \t");

  if (tok->GetEntries() == 3) { // structure FIT_RANGE fgb+n0 lgb-n1
    // handle fgb+n0 entry
    ostr = (TObjString*) tok->At(1);
    str = ostr->GetString();
    // check if there is an offset present
    idx = str.First("+");
    if (idx != -1) { // offset present
      str.Remove(0, idx+1);
      if (str.IsFloat()) // if str is a valid number, convert is to an integer
        offset = str.Atoi();
    }
    fFitStartTime = (fGoodBins[0] + offset - fT0s[0]) * fTimeResolution;

    // handle lgb-n1 entry
    ostr = (TObjString*) tok->At(2);
    str = ostr->GetString();
    // check if there is an offset present
    idx = str.First("-");
    if (idx != -1) { // offset present
      str.Remove(0, idx+1);
      if (str.IsFloat()) // if str is a valid number, convert is to an integer
        offset = str.Atoi();
    }
    fFitEndTime = (fGoodBins[1] - offset - fT0s[0]) * fTimeResolution;
  } else if ((tok->GetEntries() > 3) && (tok->GetEntries() % 2 == 1)) { // structure FIT_RANGE fgb[+n00] lgb[-n01] [fgb[+n10] lgb[-n11] ... fgb[+nN0] lgb[-nN1]]
    Int_t pos = 2*(fRunNo+1)-1;

    if (pos + 1 >= tok->GetEntries()) {
      cerr << endl << ">> PRunAsymmetryBNMR::SetFitRangeBin(): **ERROR** invalid FIT_RANGE command found: '" << fitRange << "'";
      cerr << endl << ">> will ignore it. Sorry ..." << endl;
    } else {
      // handle fgb+n0 entry
      ostr = (TObjString*) tok->At(pos);
      str = ostr->GetString();
      // check if there is an offset present
      idx = str.First("+");
      if (idx != -1) { // offset present
        str.Remove(0, idx+1);
        if (str.IsFloat()) // if str is a valid number, convert is to an integer
          offset = str.Atoi();
      }
      fFitStartTime = (fGoodBins[0] + offset - fT0s[0]) * fTimeResolution;

      // handle lgb-n1 entry
      ostr = (TObjString*) tok->At(pos+1);
      str = ostr->GetString();
      // check if there is an offset present
      idx = str.First("-");
      if (idx != -1) { // offset present
        str.Remove(0, idx+1);
        if (str.IsFloat()) // if str is a valid number, convert is to an integer
          offset = str.Atoi();
      }
      fFitEndTime = (fGoodBins[1] - offset - fT0s[0]) * fTimeResolution;
    }
  } else { // error
    cerr << endl << ">> PRunAsymmetryBNMR::SetFitRangeBin(): **ERROR** invalid FIT_RANGE command found: '" << fitRange << "'";
    cerr << endl << ">> will ignore it. Sorry ..." << endl;
  }

  // clean up
  if (tok) {
    delete tok;
  }
}

//--------------------------------------------------------------------------
// CalcNoOfFitBins (protected)
//--------------------------------------------------------------------------
/**
 * <p>Calculate the number of fitted bins for the current fit range.
 */
void PRunAsymmetryBNMR::CalcNoOfFitBins()
{
  // In order not having to loop over all bins and to stay consistent with the chisq method, calculate the start and end bins explicitly
  fStartTimeBin = static_cast<Int_t>(ceil((fFitStartTime - fData.GetDataTimeStart())/fData.GetDataTimeStep()));
  if (fStartTimeBin < 0)
    fStartTimeBin = 0;
  fEndTimeBin = static_cast<Int_t>(floor((fFitEndTime - fData.GetDataTimeStart())/fData.GetDataTimeStep())) + 1;
  if (fEndTimeBin > static_cast<Int_t>(fData.GetValue()->size()))
    fEndTimeBin = fData.GetValue()->size();

  if (fEndTimeBin > fStartTimeBin)
    fNoOfFitBins = fEndTimeBin - fStartTimeBin;
  else
    fNoOfFitBins = 0;
}

//--------------------------------------------------------------------------
// CalcTheory (protected)
//--------------------------------------------------------------------------
/**
 * <p>Calculate theory for a given set of fit-parameters.
 */
void PRunAsymmetryBNMR::CalcTheory()
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
// PrepareData (protected)
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
Bool_t PRunAsymmetryBNMR::PrepareData()
{
  // keep the Global block info
  PMsrGlobalBlock *globalBlock = fMsrInfo->GetMsrGlobal();

  // get forward/backward histo from PRunDataHandler object ------------------------
  // get the correct run
  PRawRunData *runData = fRawData->GetRunData(*(fRunInfo->GetRunName()));
  if (!runData) { // run not found
    cerr << endl << ">> PRunAsymmetryBNMR::PrepareData(): **ERROR** Couldn't get run " << fRunInfo->GetRunName()->Data() << "!";
    cerr << endl;
    return false;
  }

  // collect histogram numbers
  PUIntVector forwardHistoNo;
  PUIntVector backwardHistoNo;
  for (UInt_t i=0; i<fRunInfo->GetForwardHistoNoSize(); i++) {
    forwardHistoNo.push_back(fRunInfo->GetForwardHistoNo(i));

    if (!runData->IsPresent(forwardHistoNo[i])) {
      cerr << endl << ">> PRunAsymmetryBNMR::PrepareData(): **PANIC ERROR**:";
      cerr << endl << ">> forwardHistoNo found = " << forwardHistoNo[i] << ", which is NOT present in the data file!?!?";
      cerr << endl << ">> Will quit :-(";
      cerr << endl;
      // clean up
      forwardHistoNo.clear();
      backwardHistoNo.clear();
      return false;
    }
  }
  for (UInt_t i=0; i<fRunInfo->GetBackwardHistoNoSize(); i++) {
    backwardHistoNo.push_back(fRunInfo->GetBackwardHistoNo(i));

    if (!runData->IsPresent(backwardHistoNo[i])) {
      cerr << endl << ">> PRunAsymmetryBNMR::PrepareData(): **PANIC ERROR**:";
      cerr << endl << ">> backwardHistoNo found = " << backwardHistoNo[i] << ", which is NOT present in the data file!?!?";
      cerr << endl << ">> Will quit :-(";
      cerr << endl;
      // clean up
      forwardHistoNo.clear();
      backwardHistoNo.clear();
      return false;
    }
  }
  if (forwardHistoNo.size() != backwardHistoNo.size()) {
    cerr << endl << ">> PRunAsymmetryBNMR::PrepareData(): **PANIC ERROR**:";
    cerr << endl << ">> # of forward histograms different from # of backward histograms.";
    cerr << endl << ">> Will quit :-(";
    cerr << endl;
    // clean up
    forwardHistoNo.clear();
    backwardHistoNo.clear();
    return false;
  }

  // keep the time resolution in (ms)
  // possibility to rescale for betaNMR
  fTimeResolution = runData->GetTimeResolution()/1.0e3;
  cout.precision(10);
  cout << endl << ">> PRunAsymmetryBNMR::PrepareData(): time resolution=" << fixed << runData->GetTimeResolution() << "(ms)" << endl;

  // get all the proper t0's and addt0's for the current RUN block
  if (!GetProperT0(runData, globalBlock, forwardHistoNo, backwardHistoNo)) {
    return false;
  }

  // keep the histo of each group at this point (addruns handled below)
  vector<PDoubleVector> forward, backward;
  forward.resize(forwardHistoNo.size());   // resize to number of groups
  backward.resize(backwardHistoNo.size()); // resize to numer of groups
  for (UInt_t i=0; i<forwardHistoNo.size(); i++) {
    forward[i].resize(runData->GetDataBin(forwardHistoNo[i])->size());
    backward[i].resize(runData->GetDataBin(backwardHistoNo[i])->size());
    forward[i]  = *runData->GetDataBin(forwardHistoNo[i]);
    backward[i] = *runData->GetDataBin(backwardHistoNo[i]);
  }

  // check if addrun's are present, and if yes add data
  // check if there are runs to be added to the current one
  if (fRunInfo->GetRunNameSize() > 1) { // runs to be added present
    PRawRunData *addRunData;
    for (UInt_t i=1; i<fRunInfo->GetRunNameSize(); i++) {
      // get run to be added to the main one
      addRunData = fRawData->GetRunData(*(fRunInfo->GetRunName(i)));
      if (addRunData == 0) { // couldn't get run
        cerr << endl << ">> PRunAsymmetryBNMR::PrepareData(): **ERROR** Couldn't get addrun " << fRunInfo->GetRunName(i)->Data() << "!";
        cerr << endl;
        return false;
      }

      // add forward run
      UInt_t addRunSize;      
      for (UInt_t k=0; k<forwardHistoNo.size(); k++) { // fill each group
        addRunSize = addRunData->GetDataBin(forwardHistoNo[k])->size();
        for (UInt_t j=0; j<addRunData->GetDataBin(forwardHistoNo[k])->size(); j++) { // loop over the bin indices
          // make sure that the index stays in the proper range
          if (((Int_t)j+(Int_t)fAddT0s[i-1][2*k]-(Int_t)fT0s[2*k] >= 0) && (j+(Int_t)fAddT0s[i-1][2*k]-(Int_t)fT0s[2*k] < addRunSize)) {
            forward[k][j] += addRunData->GetDataBin(forwardHistoNo[k])->at(j+(Int_t)fAddT0s[i-1][2*k]-(Int_t)fT0s[2*k]);
          }
        }
      }

      // add backward run
      for (UInt_t k=0; k<backwardHistoNo.size(); k++) { // fill each group
        addRunSize = addRunData->GetDataBin(backwardHistoNo[k])->size();
        for (UInt_t j=0; j<addRunData->GetDataBin(backwardHistoNo[k])->size(); j++) { // loop over the bin indices
          // make sure that the index stays in the proper range
          if (((Int_t)j+(Int_t)fAddT0s[i-1][2*k+1]-(Int_t)fT0s[2*k+1] >= 0) && (j+(Int_t)fAddT0s[i-1][2*k+1]-(Int_t)fT0s[2*k+1] < addRunSize)) {
            backward[k][j] += addRunData->GetDataBin(backwardHistoNo[k])->at(j+(Int_t)fAddT0s[i-1][2*k+1]-(Int_t)fT0s[2*k+1]);
          }
        }
      }
    }
  }

  // set forward/backward histo data of the first group
  fForwardp.resize(forward[0].size());
  fBackwardp.resize(backward[0].size());
  fForwardm.resize(forward[0].size());
  fBackwardm.resize(backward[0].size());
  for (UInt_t i=0; i<fForwardp.size(); i++) {
    fForwardp[i]  = forward[0][i];
    fBackwardp[i] = backward[0][i];
    fForwardm[i]  = forward[1][i];
    fBackwardm[i] = backward[1][i];
  }

  // subtract background from histogramms ------------------------------------------
  if (fRunInfo->GetBkgFix(0) == PMUSR_UNDEFINED) { // no fixed background given
    if (fRunInfo->GetBkgRange(0) >= 0) { // background range given
      if (!SubtractEstimatedBkg())
        return false;
    } else { // no background given to do the job, try to estimate it
      fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.1), 0);
      fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.6), 1);
      fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[1]*0.1), 2);
      fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[1]*0.6), 3);
      cerr << endl << ">> PRunAsymmetryBNMR::PrepareData(): **WARNING** Neither fix background nor background bins are given!";
      cerr << endl << ">> Will try the following:";
      cerr << endl << ">> forward:  bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
      cerr << endl << ">> backward: bkg start = " << fRunInfo->GetBkgRange(2) << ", bkg end = " << fRunInfo->GetBkgRange(3);
      cerr << endl << ">> NO GUARANTEE THAT THIS MAKES ANY SENSE! Better check ...";
      cerr << endl;
      if (!SubtractEstimatedBkg())
        return false;
    }
  } else { // fixed background given
    if (!SubtractFixBkg())
      return false;
  }

  UInt_t histoNo[2] = {forwardHistoNo[0], backwardHistoNo[0]};

  // get the data range (fgb/lgb) for the current RUN block
  if (!GetProperDataRange(runData, histoNo)) {
    return false;
  }

  // get the fit range for the current RUN block
  GetProperFitRange(globalBlock);

  // everything looks fine, hence fill data set
  Bool_t status;
  switch(fHandleTag) {
    case kFit:
      status = PrepareFitData();
      break;
    case kView:
      status = PrepareViewData(runData, histoNo);
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
// SubtractFixBkg (private)
//--------------------------------------------------------------------------
/**
 * <p>Subtracts a fixed background from the raw data. The background is given
 * in units of (1/bin); for the Asymmetry representation (1/ns) doesn't make too much sense.
 * The error propagation is done the following way: it is assumed that the error of the background
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
Bool_t PRunAsymmetryBNMR::SubtractFixBkg()
{
  Double_t dval;

  // Order in RunInfo structure Fp, Fm, Bp, Bm
  for (UInt_t i=0; i<fForwardp.size(); i++) {
    // keep the error, and make sure that the bin is NOT empty
    if (fForwardp[i] != 0.0)
      dval = TMath::Sqrt(fForwardp[i]);
    else
      dval = 1.0;
    fForwardpErr.push_back(dval);
    fForwardp[i] -= fRunInfo->GetBkgFix(0);

    // keep the error, and make sure that the bin is NOT empty
    if (fForwardm[i] != 0.0)
      dval = TMath::Sqrt(fForwardm[i]);
    else
      dval = 1.0;
    fForwardmErr.push_back(dval);
    fForwardm[i] -= fRunInfo->GetBkgFix(1);

    // keep the error, and make sure that the bin is NOT empty
    if (fBackwardp[i] != 0.0)
      dval = TMath::Sqrt(fBackwardp[i]);
    else
      dval = 1.0;
    fBackwardpErr.push_back(dval);
    fBackwardp[i] -= fRunInfo->GetBkgFix(2);

    // keep the error, and make sure that the bin is NOT empty
    if (fBackwardm[i] != 0.0)
      dval = TMath::Sqrt(fBackwardm[i]);
    else
      dval = 1.0;
    fBackwardmErr.push_back(dval);
    fBackwardm[i] -= fRunInfo->GetBkgFix(3);
  }

  return true;
}

//--------------------------------------------------------------------------
// SubtractEstimatedBkg (private)
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
Bool_t PRunAsymmetryBNMR::SubtractEstimatedBkg()
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
  Int_t start[2] = {fRunInfo->GetBkgRange(0), fRunInfo->GetBkgRange(2)};
  Int_t end[2]   = {fRunInfo->GetBkgRange(1), fRunInfo->GetBkgRange(3)};
  for (UInt_t i=0; i<2; i++) {
    if (end[i] < start[i]) {
      cout << endl << "PRunAsymmetryBNMR::SubtractEstimatedBkg(): end = " << end[i] << " > start = " << start[i] << "! Will swap them!";
      UInt_t keep = end[i];
      end[i] = start[i];
      start[i] = keep;
    }
  }

  // calculate proper background range
  for (UInt_t i=0; i<2; i++) {
    if (beamPeriod != 0.0) {
      Double_t timeBkg = (Double_t)(end[i]-start[i])*(fTimeResolution*fPacking); // length of the background intervall in time
      UInt_t fullCycles = (UInt_t)(timeBkg/beamPeriod); // how many proton beam cylces can be placed within the proposed background intervall
      // correct the end of the background intervall such that the background is as close as possible to a multiple of the proton cylce
      end[i] = start[i] + (UInt_t) ((fullCycles*beamPeriod)/(fTimeResolution*fPacking));
      cout << "PRunAsymmetryBNMR::SubtractEstimatedBkg(): Background " << start[i] << ", " << end[i] << endl;
      if (end[i] == start[i])
        end[i] = fRunInfo->GetBkgRange(2*i+1);
    }
  }

  // check if start is within histogram bounds
  if ((start[0] < 0) || (start[0] >= fForwardp.size()) ||
      (start[1] < 0) || (start[1] >= fBackwardp.size())) {
    cerr << endl << ">> PRunAsymmetryBNMR::SubtractEstimatedBkg(): **ERROR** background bin values out of bound!";
    cerr << endl << ">> histo lengths (f/b)  = (" << fForwardp.size() << "/" << fBackwardp.size() << ").";
    cerr << endl << ">> background start (f/b) = (" << start[0] << "/" << start[1] << ").";
    return false;
  }

  // check if end is within histogram bounds
  if ((end[0] < 0) || (end[0] >= fForwardp.size()) ||
      (end[1] < 0) || (end[1] >= fBackwardp.size())) {
    cerr << endl << ">> PRunAsymmetryBNMR::SubtractEstimatedBkg(): **ERROR** background bin values out of bound!";
    cerr << endl << ">> histo lengths (f/b)  = (" << fForwardp.size() << "/" << fBackwardp.size() << ").";
    cerr << endl << ">> background end (f/b) = (" << end[0] << "/" << end[1] << ").";
    return false;
  }

  // calculate background
  Double_t bkgp[2]    = {0.0, 0.0};
  Double_t errBkgp[2] = {0.0, 0.0};
  Double_t bkgm[2]    = {0.0, 0.0};
  Double_t errBkgm[2] = {0.0, 0.0};

  // forward
  for (UInt_t i=start[0]; i<=end[0]; i++) {
    bkgp[0] += fForwardp[i];
    bkgm[0] += fForwardm[i];
  }
  errBkgp[0] = TMath::Sqrt(bkgp[0])/(end[0] - start[0] + 1);
  bkgp[0] /= static_cast<Double_t>(end[0] - start[0] + 1);
  cout << endl << ">> estimated pos hel forward histo background: " << bkgp[0];
  errBkgm[0] = TMath::Sqrt(bkgp[0])/(end[0] - start[0] + 1);
  bkgm[0] /= static_cast<Double_t>(end[0] - start[0] + 1);
  cout << endl << ">> estimated neg hel forward histo background: " << bkgm[0];

  // backward
  for (UInt_t i=start[1]; i<=end[1]; i++) {
    bkgp[1] += fBackwardp[i];
    bkgm[1] += fBackwardm[i];
  }
  errBkgp[1] = TMath::Sqrt(bkgp[1])/(end[1] - start[1] + 1);
  bkgp[1] /= static_cast<Double_t>(end[1] - start[1] + 1);
  cout << endl << ">> estimated pos hel backward histo background: " << bkgp[1] << endl;
  errBkgm[1] = TMath::Sqrt(bkgm[1])/(end[1] - start[1] + 1);
  bkgm[1] /= static_cast<Double_t>(end[1] - start[1] + 1);
  cout << endl << ">> estimated neg hel backward histo background: " << bkgm[1] << endl;

  // correct error for forward, backward
  Double_t errVal = 0.0;
  for (UInt_t i=0; i<fForwardp.size(); i++) {
    if (fForwardp[i] > 0.0)
      errVal = TMath::Sqrt(fForwardp[i]+errBkgp[0]*errBkgp[0]);
    else
      errVal = 1.0;
    fForwardpErr.push_back(errVal);
    if (fBackwardp[i] > 0.0)
      errVal = TMath::Sqrt(fBackwardp[i]+errBkgp[1]*errBkgp[1]);
    else
      errVal = 1.0;
    fBackwardpErr.push_back(errVal);
    if (fForwardm[i] > 0.0)
      errVal = TMath::Sqrt(fForwardm[i]+errBkgm[0]*errBkgm[0]);
    else
      errVal = 1.0;
    fForwardmErr.push_back(errVal);
    if (fBackwardm[i] > 0.0)
      errVal = TMath::Sqrt(fBackwardm[i]+errBkgm[1]*errBkgm[1]);
    else
      errVal = 1.0;
    fBackwardmErr.push_back(errVal);
  }

  // subtract background from data
  for (UInt_t i=0; i<fForwardp.size(); i++) {
    fForwardp[i]  -= bkgp[0];
    fBackwardp[i] -= bkgp[1];
    fForwardm[i]  -= bkgm[0];
    fBackwardm[i] -= bkgm[1];
  }

  fRunInfo->SetBkgEstimated(bkgp[0], 0);
  fRunInfo->SetBkgEstimated(bkgp[1], 1);
  fRunInfo->SetBkgEstimated(bkgm[0], 3);
  fRunInfo->SetBkgEstimated(bkgm[1], 4);

  return true;
}

//--------------------------------------------------------------------------
// PrepareFitData (protected)
//--------------------------------------------------------------------------
/**
 * <p>Take the pre-processed data (i.e. grouping and addrun are preformed) and form the asymmetry for fitting.
 * Before forming the asymmetry, the following checks will be performed:
 * -# check if data range is given, if not try to estimate one.
 * -# check that if a data range is present, that it makes any sense.
 * -# check that 'first good bin'-'t0' is the same for forward and backward histogram. If not adjust it.
 * -# pack data (rebin).
 * -# if packed forward size != backward size, truncate the longer one such that an asymmetry can be formed.
 * -# calculate the asymmetry: \f$ A_i = (f_i^c-b_i^c)/(f_i^c+b_i^c) \f$
 * -# calculate the asymmetry errors: \f$ \delta A_i = 2 \sqrt{(b_i^c)^2 (\delta f_i^c)^2 + (\delta b_i^c)^2 (f_i^c)^2}/(f_i^c+b_i^c)^2\f$
 */
Bool_t PRunAsymmetryBNMR::PrepareFitData()
{
  // transform raw histo data. This is done the following way (for details see the manual):
  // first rebin the data, than calculate the asymmetry

  // everything looks fine, hence fill packed forward and backward histo
  PRunData forwardpPacked;
  PRunData backwardpPacked;
  PRunData forwardmPacked;
  PRunData backwardmPacked;
  Double_t valuep = 0.0;
  Double_t errorp = 0.0;
  Double_t valuem = 0.0;
  Double_t errorm = 0.0;
  // forward
  for (Int_t i=fGoodBins[0]; i<fGoodBins[1]; i++) {
    if (fPacking == 1) {
      forwardpPacked.AppendValue(fForwardp[i]);
      forwardpPacked.AppendErrorValue(fForwardpErr[i]);
      forwardmPacked.AppendValue(fForwardm[i]);
      forwardmPacked.AppendErrorValue(fForwardmErr[i]);
    } else { // packed data, i.e. fPacking > 1
      if (((i-fGoodBins[0]) % fPacking == 0) && (i != fGoodBins[0])) { // fill data
        // in order that after rebinning the fit does not need to be redone (important for plots)
        // the value is normalize to per bin
        valuep /= fPacking;
        valuem /= fPacking;
        forwardpPacked.AppendValue(valuep);
        forwardmPacked.AppendValue(valuem);
        if (valuep == 0.0) {
	  forwardpPacked.AppendErrorValue(1.0);
        } else {
          forwardpPacked.AppendErrorValue(TMath::Sqrt(errorp)/fPacking);
	}
	if (valuem == 0.0) {
	  forwardmPacked.AppendErrorValue(1.0);
        } else {
          forwardmPacked.AppendErrorValue(TMath::Sqrt(errorm)/fPacking);
	}
        valuep = 0.0;
        errorp = 0.0;
        valuem = 0.0;
        errorm = 0.0;
      }
      valuep += fForwardp[i];
      errorp += fForwardpErr[i]*fForwardpErr[i];
      valuem += fForwardm[i];
      errorm += fForwardmErr[i]*fForwardmErr[i];
    }
  }
  // backward
  for (Int_t i=fGoodBins[2]; i<fGoodBins[3]; i++) {
    if (fPacking == 1) {
      backwardpPacked.AppendValue(fBackwardp[i]);
      backwardpPacked.AppendErrorValue(fBackwardpErr[i]);
      backwardmPacked.AppendValue(fBackwardm[i]);
      backwardmPacked.AppendErrorValue(fBackwardmErr[i]);
    } else { // packed data, i.e. fPacking > 1
      if (((i-fGoodBins[2]) % fPacking == 0) && (i != fGoodBins[2])) { // fill data
        // in order that after rebinning the fit does not need to be redone (important for plots)
        // the value is normalize to per bin
        valuep /= fPacking;
        valuem /= fPacking;
        backwardpPacked.AppendValue(valuep);
        backwardmPacked.AppendValue(valuem);
        if (valuep == 0.0) {
          backwardpPacked.AppendErrorValue(1.0);
	} else {
          backwardpPacked.AppendErrorValue(TMath::Sqrt(errorp)/fPacking);
	}
	if (valuem == 0.0) {
          backwardmPacked.AppendErrorValue(1.0);
	} else {
          backwardmPacked.AppendErrorValue(TMath::Sqrt(errorm)/fPacking);
	}
        valuep = 0.0;
        errorp = 0.0;
        valuem = 0.0;
        errorm = 0.0;
      }
      valuep += fBackwardp[i];
      errorp += fBackwardpErr[i]*fBackwardpErr[i];
      valuem += fBackwardm[i];
      errorm += fBackwardmErr[i]*fBackwardmErr[i];
    }
  }

  // check if packed forward and backward hist have the same size, otherwise take the minimum size
  UInt_t noOfBins = forwardpPacked.GetValue()->size();
  if (forwardpPacked.GetValue()->size() != backwardpPacked.GetValue()->size()) {
    if (forwardpPacked.GetValue()->size() > backwardpPacked.GetValue()->size())
      noOfBins = backwardpPacked.GetValue()->size();
  }

  // form asymmetry including error propagation
  Double_t asym,error;
  Double_t fp, bp, efp, ebp;
  Double_t fm, bm, efm, ebm;
  // fill data time start, and step
  // data start at data_start-t0 shifted by (pack-1)/2
  fData.SetDataTimeStart(fTimeResolution*((Double_t)fGoodBins[0]-fT0s[0]+(Double_t)(fPacking-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*(Double_t)fPacking);
  for (UInt_t i=0; i<noOfBins; i++) {
    // to make the formulae more readable
    fp  = forwardpPacked.GetValue()->at(i);
    bp  = backwardpPacked.GetValue()->at(i);
    efp = forwardpPacked.GetError()->at(i);
    ebp = backwardpPacked.GetError()->at(i);
    fm  = forwardmPacked.GetValue()->at(i);
    bm  = backwardmPacked.GetValue()->at(i);
    efm = forwardmPacked.GetError()->at(i);
    ebm = backwardmPacked.GetError()->at(i);
    // check that there are indeed bins
    if (fp+bp != 0.0)
      asym = (fp-bp) / (fp+bp) - (fm-bm) / (fm+bm);
    else
      asym = 0.0;
    fData.AppendValue(asym);
    // calculate the error
    if (fp+bp != 0.0)
      errorp = 2.0/((fp+bp)*(fp+bp))*TMath::Sqrt(bp*bp*efp*efp+ebp*ebp*fp*fp);
    else
      errorp = 1.0;
    if (fm+bm != 0.0)
      errorm = 2.0/((fm+bm)*(fm+bm))*TMath::Sqrt(bm*bm*efm*efm+ebm*ebm*fm*fm);
    else
      errorp = 1.0;

    error = TMath::Sqrt(errorp*errorp+errorm*errorm);
    fData.AppendErrorValue(error);
  }

  CalcNoOfFitBins();

  // clean up
  fForwardp.clear();
  fForwardpErr.clear();
  fBackwardp.clear();
  fBackwardpErr.clear();
  fForwardm.clear();
  fForwardmErr.clear();
  fBackwardm.clear();
  fBackwardmErr.clear();

  return true;
}

//--------------------------------------------------------------------------
// PrepareViewData (protected)
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
Bool_t PRunAsymmetryBNMR::PrepareViewData(PRawRunData* runData, UInt_t histoNo[2])
{
  // check if view_packing is wished
  Int_t packing = fPacking;
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
  Int_t start[2] = {fGoodBins[0], fGoodBins[2]};
  Int_t end[2] = {fGoodBins[1], fGoodBins[3]};
  Int_t t0[2] = {(Int_t)fT0s[0], (Int_t)fT0s[1]};

  // check if the data ranges and t0's between forward/backward are compatible
  Int_t fgb[2];
  if (start[0]-t0[0] != start[1]-t0[1]) { // wrong fgb aligning
    if (abs(start[0]-t0[0]) > abs(start[1]-t0[1])) {
      fgb[0] = start[0];
      fgb[1] = t0[1] + start[0]-t0[0];
      cerr << endl << ">> PRunAsymmetryBNMR::PrepareViewData(): **WARNING** needed to shift backward fgb from ";
      cerr << start[1] << " to " << fgb[1] << endl;
    } else {
      fgb[0] = t0[0] + start[1]-t0[1];
      fgb[1] = start[1];
      cerr << endl << ">> PRunAsymmetryBNMR::PrepareViewData(): **WARNING** needed to shift forward fgb from ";
      cerr << start[0] << " to " << fgb[0] << endl;
    }
  } else { // fgb aligning is correct
    fgb[0] = start[0];
    fgb[1] = start[1];
  }

  Int_t val = fgb[0]-packing*(fgb[0]/packing);
  do {
    if (fgb[1] - fgb[0] < 0)
      val += packing;
  } while (val + fgb[1] - fgb[0] < 0);

  start[0] = val;
  start[1] = val + fgb[1] - fgb[0];

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
      cerr << endl << ">> PRunAsymmetryBNMR::PrepareViewData(): **ERROR** start data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 3rd check if end is within proper bounds
    if ((end[i] < 0) || (end[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << ">> PRunAsymmetryBNMR::PrepareViewData(): **ERROR** end data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 4th check if t0 is within proper bounds
    if ((t0[i] < 0) || (t0[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << ">> PRunAsymmetryBNMR::PrepareViewData(): **ERROR** t0 data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
  }

  // everything looks fine, hence fill packed forward and backward histo
  PRunData forwardpPacked;
  PRunData backwardpPacked;
  PRunData forwardmPacked;
  PRunData backwardmPacked;
  Double_t valuep = 0.0;
  Double_t errorp = 0.0;
  Double_t valuem = 0.0;
  Double_t errorm = 0.0;
  Double_t value = 0.0;
  Double_t error = 0.0;

  // forward
  for (Int_t i=start[0]; i<end[0]; i++) {
    if (packing == 1) {
      forwardpPacked.AppendValue(fForwardp[i]);
      forwardpPacked.AppendErrorValue(fForwardpErr[i]);
      forwardmPacked.AppendValue(fForwardm[i]);
      forwardmPacked.AppendErrorValue(fForwardmErr[i]);
    } else { // packed data, i.e. packing > 1
      if (((i-start[0]) % packing == 0) && (i != start[0])) { // fill data
        // in order that after rebinning the fit does not need to be redone (important for plots)
        // the value is normalize to per bin
        valuep /= packing;
        forwardpPacked.AppendValue(valuep);
        valuem /= packing;
        forwardmPacked.AppendValue(valuem);
        if (valuep == 0.0) {
          forwardpPacked.AppendErrorValue(1.0);
        } else {
          forwardpPacked.AppendErrorValue(TMath::Sqrt(errorp)/packing);
	}
        if (valuem == 0.0) {
          forwardmPacked.AppendErrorValue(1.0);
        } else {
          forwardmPacked.AppendErrorValue(TMath::Sqrt(errorm)/packing);
	}
        valuep = 0.0;
        errorp = 0.0;
        valuem = 0.0;
        errorm = 0.0;
      }
      valuep += fForwardp[i];
      errorp += fForwardpErr[i]*fForwardpErr[i];
      valuem += fForwardm[i];
      errorm += fForwardmErr[i]*fForwardmErr[i];
    }
  }

  // backward
  for (Int_t i=start[1]; i<end[1]; i++) {
    if (packing == 1) {
      backwardpPacked.AppendValue(fBackwardp[i]);
      backwardpPacked.AppendErrorValue(fBackwardpErr[i]);
      backwardmPacked.AppendValue(fBackwardm[i]);
      backwardmPacked.AppendErrorValue(fBackwardmErr[i]);
    } else { // packed data, i.e. packing > 1
      if (((i-start[1]) % packing == 0) && (i != start[1])) { // fill data
        // in order that after rebinning the fit does not need to be redone (important for plots)
        // the value is normalize to per bin
        valuep /= packing;
        valuem /= packing;
        backwardpPacked.AppendValue(valuep);
        backwardmPacked.AppendValue(valuem);
        if (valuep == 0.0) {
          backwardpPacked.AppendErrorValue(1.0);
        } else {
          backwardpPacked.AppendErrorValue(TMath::Sqrt(errorp)/packing);
	}
        if (valuem == 0.0) {
          backwardmPacked.AppendErrorValue(1.0);
        } else {
          backwardmPacked.AppendErrorValue(TMath::Sqrt(errorm)/packing);
	}
        valuep = 0.0;
        errorp = 0.0;
        valuem = 0.0;
        errorm = 0.0;
      }
      valuep += fBackwardp[i];
      errorp += fBackwardpErr[i]*fBackwardpErr[i];
      valuem += fBackwardm[i];
      errorm += fBackwardmErr[i]*fBackwardmErr[i];
    }
  }

  // check if packed forward and backward hist have the same size, otherwise take the minimum size
  UInt_t noOfBins = forwardpPacked.GetValue()->size();
  if (forwardpPacked.GetValue()->size() != backwardpPacked.GetValue()->size()) {
    if (forwardpPacked.GetValue()->size() > backwardpPacked.GetValue()->size())
      noOfBins = backwardpPacked.GetValue()->size();
  }

  // form asymmetry including error propagation
  Double_t asym;
  Double_t fp, bp, efp, ebp, alpha = 1.0, beta = 1.0;
  Double_t fm, bm, efm, ebm;
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

  for (UInt_t i=0; i<forwardpPacked.GetValue()->size(); i++) {
    // to make the formulae more readable
    fp  = forwardpPacked.GetValue()->at(i);
    bp  = backwardpPacked.GetValue()->at(i);
    efp = forwardpPacked.GetError()->at(i);
    ebp = backwardpPacked.GetError()->at(i);
    fm  = forwardmPacked.GetValue()->at(i);
    bm  = backwardmPacked.GetValue()->at(i);
    efm = forwardmPacked.GetError()->at(i);
    ebm = backwardmPacked.GetError()->at(i);
    // check that there are indeed bins
    if (fp+bp != 0.0)
      asym = (alpha*fp-bp) / (alpha*beta*fp+bp) - (alpha*fm-bm) / (alpha*beta*fm+bm);
    else
      asym = 0.0;
    fData.AppendValue(asym);
    // calculate the error
    if (fp+bp != 0.0)
      errorp = 2.0/((fp+bp)*(fp+bp))*TMath::Sqrt(bp*bp*efp*efp+ebp*ebp*fp*fp);
    else
      errorp = 1.0;
    if (fm+bm != 0.0)
      errorm = 2.0/((fm+bm)*(fm+bm))*TMath::Sqrt(bm*bm*efm*efm+ebm*ebm*fm*fm);
    else
      errorm = 1.0;
    error = TMath::Sqrt(errorp*errorp+errorm*errorm);
    fData.AppendErrorValue(error);
  }

  CalcNoOfFitBins();

  // clean up
  fForwardp.clear();
  fForwardpErr.clear();
  fBackwardp.clear();
  fBackwardpErr.clear();
  fForwardm.clear();
  fForwardmErr.clear();
  fBackwardm.clear();
  fBackwardmErr.clear();

  // fill theory vector for kView
  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par);
  }

  // calculate theory
  Double_t time;
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
// GetProperT0 (private)
//--------------------------------------------------------------------------
/**
 * <p>Get the proper t0 for the single histogram run.
 * -# the t0 vector size = number of detectors (grouping) for forward.
 * -# initialize t0's with -1
 * -# fill t0's from RUN block
 * -# if t0's are missing (i.e. t0 == -1), try to fill from the GLOBAL block.
 * -# if t0's are missing, try t0's from the data file
 * -# if t0's are missing, try to estimate them
 *
 * \param runData pointer to the current RUN block entry from the msr-file
 * \param globalBlock pointer to the GLOBLA block entry from the msr-file
 * \param forwardHistoNo histogram number vector of forward; forwardHistoNo = msr-file forward + redGreen_offset - 1
 * \param backwardHistoNo histogram number vector of backwardward; backwardHistoNo = msr-file backward + redGreen_offset - 1
 *
 * <b>return:</b>
 * - true if everthing went smooth
 * - false, otherwise.
 */
Bool_t PRunAsymmetryBNMR::GetProperT0(PRawRunData* runData, PMsrGlobalBlock *globalBlock, PUIntVector &forwardHistoNo, PUIntVector &backwardHistoNo)
{
  // feed all T0's
  // first init T0's, T0's are stored as (forward T0, backward T0, etc.)
  fT0s.clear();
  fT0s.resize(2*forwardHistoNo.size());
  for (UInt_t i=0; i<fT0s.size(); i++) {
    fT0s[i] = -1.0;
  }

  // fill in the T0's from the msr-file (if present)
  for (UInt_t i=0; i<fRunInfo->GetT0BinSize(); i++) {
    fT0s[i] = fRunInfo->GetT0Bin(i);
  }

  // fill in the missing T0's from the GLOBAL block section (if present)
  for (UInt_t i=0; i<globalBlock->GetT0BinSize(); i++) {
    if (fT0s[i] == -1) { // i.e. not given in the RUN block section
      fT0s[i] = globalBlock->GetT0Bin(i);
    }
  }

  // fill in the missing T0's from the data file, if not already present in the msr-file
  for (UInt_t i=0; i<forwardHistoNo.size(); i++) {
    if (fT0s[2*i] == -1.0) // i.e. not present in the msr-file, try the data file
      if (runData->GetT0Bin(forwardHistoNo[i]) > 0.0) {
        fT0s[2*i] = runData->GetT0Bin(forwardHistoNo[i]);
        fRunInfo->SetT0Bin(fT0s[2*i], 2*i);
      }
  }
  for (UInt_t i=0; i<backwardHistoNo.size(); i++) {
    if (fT0s[2*i+1] == -1.0) // i.e. not present in the msr-file, try the data file
      if (runData->GetT0Bin(backwardHistoNo[i]) > 0.0) {
        fT0s[2*i+1] = runData->GetT0Bin(backwardHistoNo[i]);
        fRunInfo->SetT0Bin(fT0s[2*i+1], 2*i+1);
      }
  }

  // fill in the T0's gaps, i.e. in case the T0's are NEITHER in the msr-file and NOR in the data file
  for (UInt_t i=0; i<forwardHistoNo.size(); i++) {
    if (fT0s[2*i] == -1.0) { // i.e. not present in the msr-file and data file, use the estimated T0
      fT0s[2*i] = runData->GetT0BinEstimated(forwardHistoNo[i]);
      fRunInfo->SetT0Bin(fT0s[2*i], 2*i);

      cerr << endl << ">> PRunAsymmetryBNMR::GetProperT0(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
      cerr << endl << ">> run: " << fRunInfo->GetRunName()->Data();
      cerr << endl << ">> will try the estimated one: forward t0 = " << runData->GetT0BinEstimated(forwardHistoNo[i]);
      cerr << endl << ">> NO GUARANTEE THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
      cerr << endl;
    }
  }
  for (UInt_t i=0; i<backwardHistoNo.size(); i++) {
    if (fT0s[2*i+1] == -1.0) { // i.e. not present in the msr-file and data file, use the estimated T0
      fT0s[2*i+1] = runData->GetT0BinEstimated(backwardHistoNo[i]);
      fRunInfo->SetT0Bin(fT0s[2*i+1], 2*i+1);

      cerr << endl << ">> PRunAsymmetryBNMR::GetProperT0(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
      cerr << endl << ">> run: " << fRunInfo->GetRunName()->Data();
      cerr << endl << ">> will try the estimated one: backward t0 = " << runData->GetT0BinEstimated(backwardHistoNo[i]);
      cerr << endl << ">> NO GUARANTEE THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
      cerr << endl;
    }
  }

  // check if t0 is within proper bounds
  for (UInt_t i=0; i<forwardHistoNo.size(); i++) {
    if ((fT0s[2*i] < 0) || (fT0s[2*i] > (Int_t)runData->GetDataBin(forwardHistoNo[i])->size())) {
      cerr << endl << ">> PRunAsymmetryBNMR::GetProperT0(): **ERROR** t0 data bin (" << fT0s[2*i] << ") doesn't make any sense!";
      cerr << endl << ">> forwardHistoNo " << forwardHistoNo[i];
      cerr << endl;
      return false;
    }
    if ((fT0s[2*i+1] < 0) || (fT0s[2*i+1] > (Int_t)runData->GetDataBin(backwardHistoNo[i])->size())) {
      cerr << endl << ">> PRunAsymmetryBNMR::PrepareData(): **ERROR** t0 data bin (" << fT0s[2*i+1] << ") doesn't make any sense!";
      cerr << endl << ">> backwardHistoNo " << backwardHistoNo[i];
      cerr << endl;
      return false;
    }
  }

  // check if addrun's are present, and if yes add the necessary t0's
  if (fRunInfo->GetRunNameSize() > 1) { // runs to be added present
    PRawRunData *addRunData;
    fAddT0s.resize(fRunInfo->GetRunNameSize()-1); // resize to the number of addruns
    for (UInt_t i=1; i<fRunInfo->GetRunNameSize(); i++) {
      // get run to be added to the main one
      addRunData = fRawData->GetRunData(*(fRunInfo->GetRunName(i)));
      if (addRunData == 0) { // couldn't get run
        cerr << endl << ">> PRunAsymmetryBNMR::GetProperT0(): **ERROR** Couldn't get addrun " << fRunInfo->GetRunName(i)->Data() << "!";
        cerr << endl;
        return false;
      }

      // feed all T0's
      // first init T0's, T0's are stored as (forward T0, backward T0, etc.)
      fAddT0s[i-1].clear();
      fAddT0s[i-1].resize(2*forwardHistoNo.size());
      for (UInt_t j=0; j<fAddT0s[i-1].size(); j++) {
        fAddT0s[i-1][j] = -1.0;
      }

      // fill in the T0's from the msr-file (if present)
      for (Int_t j=0; j<fRunInfo->GetAddT0BinSize(i); j++) {
        fAddT0s[i-1][j] = fRunInfo->GetAddT0Bin(i, j);
      }

      // fill in the T0's from the data file, if not already present in the msr-file
      for (UInt_t j=0; j<forwardHistoNo.size(); j++) {
        if (fAddT0s[i-1][2*j] == -1.0) // i.e. not present in the msr-file, try the data file
          if (addRunData->GetT0Bin(forwardHistoNo[j]) > 0.0) {
            fAddT0s[i-1][2*j] = addRunData->GetT0Bin(forwardHistoNo[j]);
            fRunInfo->SetAddT0Bin(fAddT0s[i-1][2*j], i-1, 2*j);
          }
      }
      for (UInt_t j=0; j<backwardHistoNo.size(); j++) {
        if (fAddT0s[i-1][2*j+1] == -1.0) // i.e. not present in the msr-file, try the data file
          if (addRunData->GetT0Bin(backwardHistoNo[j]) > 0.0) {
            fAddT0s[i-1][2*j+1] = addRunData->GetT0Bin(backwardHistoNo[j]);
            fRunInfo->SetAddT0Bin(fAddT0s[i-1][2*j+1], i-1, 2*j+1);
          }
      }

      // fill in the T0's gaps, i.e. in case the T0's are NOT in the msr-file and NOT in the data file
      for (UInt_t j=0; j<forwardHistoNo.size(); j++) {
        if (fAddT0s[i-1][2*j] == -1.0) { // i.e. not present in the msr-file and data file, use the estimated T0
          fAddT0s[i-1][2*j] = addRunData->GetT0BinEstimated(forwardHistoNo[j]);
          fRunInfo->SetAddT0Bin(fAddT0s[i-1][2*j], i-1, 2*j);

          cerr << endl << ">> PRunAsymmetryBNMR::GetProperT0(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
          cerr << endl << ">> run: " << fRunInfo->GetRunName(i)->Data();
          cerr << endl << ">> will try the estimated one: forward t0 = " << addRunData->GetT0BinEstimated(forwardHistoNo[j]);
          cerr << endl << ">> NO GUARANTEE THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
          cerr << endl;
        }
      }
      for (UInt_t j=0; j<backwardHistoNo.size(); j++) {
        if (fAddT0s[i-1][2*j+1] == -1.0) { // i.e. not present in the msr-file and data file, use the estimated T0
          fAddT0s[i-1][2*j+1] = addRunData->GetT0BinEstimated(backwardHistoNo[j]);
          fRunInfo->SetAddT0Bin(fAddT0s[i-1][2*j+1], i-1, 2*j+1);

          cerr << endl << ">> PRunAsymmetryBNMR::GetProperT0(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
          cerr << endl << ">> run: " << fRunInfo->GetRunName(i)->Data();
          cerr << endl << ">> will try the estimated one: backward t0 = " << runData->GetT0BinEstimated(backwardHistoNo[j]);
          cerr << endl << ">> NO GUARANTEE THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
          cerr << endl;
        }
      }
    }
  }

  return true;
}

//--------------------------------------------------------------------------
// GetProperDataRange (private)
//--------------------------------------------------------------------------
/**
 * <p>Get the proper data range, i.e. first/last good bin (fgb/lgb).
 * -# get fgb/lgb from the RUN block
 * -# if fgb/lgb still undefined, try to get it from the GLOBAL block
 * -# if fgb/lgb still undefined, try to estimate them.
 *
 * \param runData raw run data needed to perform some crosschecks
 * \param histoNo histogram number (within a run). histoNo[0]: forward histogram number, histNo[1]: backward histogram number
 *
 * <b>return:</b>
 * - true if everthing went smooth
 * - false, otherwise.
 */
Bool_t PRunAsymmetryBNMR::GetProperDataRange(PRawRunData* runData, UInt_t histoNo[2])
{
  // first get start/end data
  Int_t start[2] = {fRunInfo->GetDataRange(0), fRunInfo->GetDataRange(2)};
  Int_t end[2]   = {fRunInfo->GetDataRange(1), fRunInfo->GetDataRange(3)};
  // check if data range has been provided in the RUN block. If not, try the GLOBAL block
  if (start[0] == -1) {
    start[0] = fMsrInfo->GetMsrGlobal()->GetDataRange(0);
  }
  if (start[1] == -1) {
    start[1] = fMsrInfo->GetMsrGlobal()->GetDataRange(2);
  }
  if (end[0] == -1) {
    end[0] = fMsrInfo->GetMsrGlobal()->GetDataRange(1);
  }
  if (end[1] == -1) {
    end[1] = fMsrInfo->GetMsrGlobal()->GetDataRange(3);
  }

  Double_t t0[2] = {fT0s[0], fT0s[1]};
  Int_t offset = (Int_t)(10.0e-3/fTimeResolution); // needed in case first good bin is not given, default = 10ns

  // check if data range has been provided, and if not try to estimate them
  if (start[0] < 0) {
    start[0] = (Int_t)t0[0]+offset;
    fRunInfo->SetDataRange(start[0], 0);
    cerr << endl << ">> PRunAsymmetryBNMR::GetProperDataRange(): **WARNING** data range (forward) was not provided, will try data range start = t0+" << offset << "(=10ns) = " << start[0] << ".";
    cerr << endl << ">> NO GUARANTEE THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  }
  if (start[1] < 0) {
    start[1] = (Int_t)t0[1]+offset;
    fRunInfo->SetDataRange(start[1], 2);
    cerr << endl << ">> PRunAsymmetryBNMR::GetProperDataRange(): **WARNING** data range (backward) was not provided, will try data range start = t0+" << offset << "(=10ns) = " << start[1] << ".";
    cerr << endl << ">> NO GUARANTEE THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  }
  if (end[0] < 0) {
    end[0] = runData->GetDataBin(histoNo[0])->size();
    fRunInfo->SetDataRange(end[0], 1);
    cerr << endl << ">> PRunAsymmetryBNMR::GetProperDataRange(): **WARNING** data range (forward) was not provided, will try data range end = " << end[0] << ".";
    cerr << endl << ">> NO GUARANTEE THAT THIS DOES MAKE ANY SENSE.";
    cerr << endl;
  }
  if (end[1] < 0) {
    end[1] = runData->GetDataBin(histoNo[1])->size();
    fRunInfo->SetDataRange(end[1], 3);
    cerr << endl << ">> PRunAsymmetryBNMR::GetProperDataRange(): **WARNING** data range (backward) was not provided, will try data range end = " << end[1] << ".";
    cerr << endl << ">> NO GUARANTEE THAT THIS DOES MAKE ANY SENSE.";
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
      cerr << endl << ">> PRunAsymmetryBNMR::GetProperDataRange(): **ERROR** start data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
    // 3rd check if end is within proper bounds
    if (end[i] < 0) {
      cerr << endl << ">> PRunAsymmetryBNMR::GetProperDataRange(): **ERROR** end data bin (" << end[i] << ") doesn't make any sense!";
      cerr << endl;
      return false;
    }
    if (end[i] > (Int_t)runData->GetDataBin(histoNo[i])->size()) {
      cerr << endl << ">> PRunAsymmetryBNMR::GetProperDataRange(): **WARNING** end data bin (" << end[i] << ") > histo length (" << (Int_t)runData->GetDataBin(histoNo[i])->size() << ").";
      cerr << endl << ">>    Will set end = (histo length - 1). Consider to change it in the msr-file." << endl;
      cerr << endl;
      end[i] = (Int_t)runData->GetDataBin(histoNo[i])->size()-1;
    }
    // 4th check if t0 is within proper bounds
    if ((t0[i] < 0) || (t0[i] > (Int_t)runData->GetDataBin(histoNo[i])->size())) {
      cerr << endl << ">> PRunAsymmetryBNMR::GetProperDataRange(): **ERROR** t0 data bin doesn't make any sense!";
      cerr << endl;
      return false;
    }
  }

  // check that start-t0 is the same for forward as for backward, otherwise take max(start[i]-t0[i])
  if (fabs(static_cast<Double_t>(start[0])-t0[0]) > fabs(static_cast<Double_t>(start[1])-t0[1])){
    start[1] = static_cast<Int_t>(t0[1] + static_cast<Double_t>(start[0]) - t0[0]);
    end[1] = static_cast<Int_t>(t0[1] + static_cast<Double_t>(end[0]) - t0[0]);
    cerr << endl << ">> PRunAsymmetryBNMR::GetProperDataRange **WARNING** needed to shift backward data range.";
    cerr << endl << ">> given: " << fRunInfo->GetDataRange(2) << ", " << fRunInfo->GetDataRange(3);
    cerr << endl << ">> used : " << start[1] << ", " << end[1];
    cerr << endl;
  }
  if (fabs(static_cast<Double_t>(start[0])-t0[0]) < fabs(static_cast<Double_t>(start[1])-t0[1])){
    start[0] = static_cast<Int_t>(t0[0] + static_cast<Double_t>(start[1]) - t0[1]);
    end[0] = static_cast<Int_t>(t0[0] + static_cast<Double_t>(end[1]) - t0[1]);
    cerr << endl << ">> PRunAsymmetryBNMR::GetProperDataRange **WARNING** needed to shift forward data range.";
    cerr << endl << ">> given: " << fRunInfo->GetDataRange(0) << ", " << fRunInfo->GetDataRange(1);
    cerr << endl << ">> used : " << start[0] << ", " << end[0];
    cerr << endl;
  }

  // keep good bins for potential latter use
  fGoodBins[0] = start[0];
  fGoodBins[1] = end[0];
  fGoodBins[2] = start[1];
  fGoodBins[3] = end[1];

  return true;
}

//--------------------------------------------------------------------------
// GetProperFitRange (private)
//--------------------------------------------------------------------------
/**
 * <p>Get the proper fit range. There are two possible fit range commands:
 * fit <start> <end> given in (usec), or
 * fit fgb+offset_0 lgb-offset_1 given in (bins), therefore it works the following way:
 * -# get fit range assuming given in time from RUN block
 * -# if fit range in RUN block is given in bins, replace start/end
 * -# if fit range is NOT given yet, try fit range assuming given in time from GLOBAL block
 * -# if fit range in GLOBAL block is given in bins, replace start/end
 * -# if still no fit range is given, use fgb/lgb.
 *
 * \param globalBlock pointer to the GLOBAL block information form the msr-file.
 */
void PRunAsymmetryBNMR::GetProperFitRange(PMsrGlobalBlock *globalBlock)
{
  // set fit start/end time; first check RUN Block
  fFitStartTime = fRunInfo->GetFitRange(0);
  fFitEndTime   = fRunInfo->GetFitRange(1);
  // if fit range is given in bins (and not time), the fit start/end time can be calculated at this point now
  if (fRunInfo->IsFitRangeInBin()) {
    fFitStartTime = (fGoodBins[0] + fRunInfo->GetFitRangeOffset(0) - fT0s[0]) * fTimeResolution; // (fgb+n0-t0)*dt
    fFitEndTime = (fGoodBins[1] - fRunInfo->GetFitRangeOffset(1) - fT0s[0]) * fTimeResolution;   // (lgb-n1-t0)*dt
    // write these times back into the data structure. This way it is available when writting the log-file
    fRunInfo->SetFitRange(fFitStartTime, 0);
    fRunInfo->SetFitRange(fFitEndTime, 1);
  }
  if (fFitStartTime == PMUSR_UNDEFINED) { // fit start/end NOT found in the RUN block, check GLOBAL block
    fFitStartTime = globalBlock->GetFitRange(0);
    fFitEndTime   = globalBlock->GetFitRange(1);
    // if fit range is given in bins (and not time), the fit start/end time can be calculated at this point now
    if (globalBlock->IsFitRangeInBin()) {
      fFitStartTime = (fGoodBins[0] + globalBlock->GetFitRangeOffset(0) - fT0s[0]) * fTimeResolution; // (fgb+n0-t0)*dt
      fFitEndTime = (fGoodBins[1] - globalBlock->GetFitRangeOffset(1) - fT0s[0]) * fTimeResolution;   // (lgb-n1-t0)*dt
      // write these times back into the data structure. This way it is available when writting the log-file
      globalBlock->SetFitRange(fFitStartTime, 0);
      globalBlock->SetFitRange(fFitEndTime, 1);
    }
  }
  if ((fFitStartTime == PMUSR_UNDEFINED) || (fFitEndTime == PMUSR_UNDEFINED)) {
    fFitStartTime = (fGoodBins[0] - fT0s[0]) * fTimeResolution; // (fgb-t0)*dt
    fFitEndTime = (fGoodBins[1] - fT0s[0]) * fTimeResolution;   // (lgb-t0)*dt
    cerr << ">> PRunSingleHisto::GetProperFitRange(): **WARNING** Couldn't get fit start/end time!" << endl;
    cerr << ">>    Will set it to fgb/lgb which given in time is: " << fFitStartTime << "..." << fFitEndTime << " (usec)" << endl;
  }
}
