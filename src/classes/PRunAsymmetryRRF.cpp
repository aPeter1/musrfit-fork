/***************************************************************************

  PRunAsymmetryRRF.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2023 by Andreas Suter                              *
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
#include <fstream>

#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>

#include "PMusr.h"
#include "PRunAsymmetryRRF.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 */
PRunAsymmetryRRF::PRunAsymmetryRRF() : PRunBase()
{
  fNoOfFitBins  = 0;
  fRRFPacking = -1;
  fTheoAsData = false;

  // the 2 following variables are need in case fit range is given in bins, and since
  // the fit range can be changed in the command block, these variables need to be accessible
  fGoodBins[0] = -1;
  fGoodBins[1] = -1;
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
PRunAsymmetryRRF::PRunAsymmetryRRF(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag, Bool_t theoAsData) :
  PRunBase(msrInfo, rawData, runNo, tag), fTheoAsData(theoAsData)
{
  // the 2 following variables are need in case fit range is given in bins, and since
  // the fit range can be changed in the command block, these variables need to be accessible
  fGoodBins[0] = -1;
  fGoodBins[1] = -1;

  fRRFPacking = fMsrInfo->GetMsrGlobal()->GetRRFPacking();
  if (fRRFPacking == -1) { // this should NOT happen, somethin is severely wrong
    std::cerr << std::endl << ">> PRunAsymmetryRRF::PRunAsymmetryRRF(): **SEVERE ERROR**: Couldn't find any RRF packing information!";
    std::cerr << std::endl << ">> This is very bad :-(, will quit ...";
    std::cerr << std::endl;
    fValid = false;
    return;
  }

  // check if alpha and/or beta is fixed --------------------

  PMsrParamList *param = msrInfo->GetMsrParamList();

  // check if alpha is given
  if (fRunInfo->GetAlphaParamNo() == -1) { // no alpha given
    std::cerr << std::endl << ">> PRunAsymmetryRRF::PRunAsymmetryRRF(): **ERROR** no alpha parameter given! This is needed for an asymmetry fit!";
    std::cerr << std::endl;
    fValid = false;
    return;
  }
  // check if alpha parameter is within proper bounds
  if ((fRunInfo->GetAlphaParamNo() < 0) || (fRunInfo->GetAlphaParamNo() > static_cast<Int_t>(param->size()))) {
    std::cerr << std::endl << ">> PRunAsymmetryRRF::PRunAsymmetryRRF(): **ERROR** alpha parameter no = " << fRunInfo->GetAlphaParamNo();
    std::cerr << std::endl << ">> This is out of bound, since there are only " << param->size() << " parameters.";
    std::cerr << std::endl;
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
  } else if ((fRunInfo->GetBetaParamNo() < 0) || (fRunInfo->GetBetaParamNo() > static_cast<Int_t>(param->size()))) { // check if beta parameter is within proper bounds
    std::cerr << std::endl << ">> PRunAsymmetryRRF::PRunAsymmetryRRF(): **ERROR** beta parameter no = " << fRunInfo->GetBetaParamNo();
    std::cerr << std::endl << ">> This is out of bound, since there are only " << param->size() << " parameters.";
    std::cerr << std::endl;
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
PRunAsymmetryRRF::~PRunAsymmetryRRF()
{
  fForward.clear();
  fForwardErr.clear();
  fBackward.clear();
  fBackwardErr.clear();
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
Double_t PRunAsymmetryRRF::CalcChiSquare(const std::vector<Double_t>& par)
{
  Double_t chisq = 0.0;
  Double_t diff = 0.0;
  Double_t asymFcnValue = 0.0;
  Double_t a, b, f;

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par, fMetaData);
  }

  // calculate chi square
  Double_t time(1.0);
  Int_t i;

  // determine alpha/beta
  switch (fAlphaBetaTag) {
    case 1: // alpha == 1, beta == 1
      a = 1.0;
      b = 1.0;
      break;
    case 2: // alpha != 1, beta == 1
      if (fRunInfo->GetAlphaParamNo() < MSR_PARAM_FUN_OFFSET) { // alpha is a parameter
        a = par[fRunInfo->GetAlphaParamNo()-1];
      } else { // alpha is function
        // get function number
        UInt_t funNo = fRunInfo->GetAlphaParamNo()-MSR_PARAM_FUN_OFFSET;
        // evaluate function
        a = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
      }
      b = 1.0;
      break;
    case 3: // alpha == 1, beta != 1
      a = 1.0;
      if (fRunInfo->GetBetaParamNo() < MSR_PARAM_FUN_OFFSET) { // beta is a parameter
        b = par[fRunInfo->GetBetaParamNo()-1];
      } else { // beta is a function
        // get function number
        UInt_t funNo = fRunInfo->GetBetaParamNo()-MSR_PARAM_FUN_OFFSET;
        // evaluate function
        b = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
      }
      break;
    case 4: // alpha != 1, beta != 1
      if (fRunInfo->GetAlphaParamNo() < MSR_PARAM_FUN_OFFSET) { // alpha is a parameter
        a = par[fRunInfo->GetAlphaParamNo()-1];
      } else { // alpha is function
        // get function number
        UInt_t funNo = fRunInfo->GetAlphaParamNo()-MSR_PARAM_FUN_OFFSET;
        // evaluate function
        a = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
      }
      if (fRunInfo->GetBetaParamNo() < MSR_PARAM_FUN_OFFSET) { // beta is a parameter
        b = par[fRunInfo->GetBetaParamNo()-1];
      } else { // beta is a function
        // get function number
        UInt_t funNo = fRunInfo->GetBetaParamNo()-MSR_PARAM_FUN_OFFSET;
        // evaluate function
        b = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
      }
      break;
    default:
      a = 1.0;
      b = 1.0;
      break;
  }

  // Calculate the theory function once to ensure one function evaluation for the current set of parameters.
  // This is needed for the LF and user functions where some non-thread-save calculations only need to be calculated once
  // for a given set of parameters---which should be done outside of the parallelized loop.
  // For all other functions it means a tiny and acceptable overhead.
  asymFcnValue = fTheory->Func(time, par, fFuncValues);

  #ifdef HAVE_GOMP
  Int_t chunk = (fEndTimeBin - fStartTimeBin)/omp_get_num_procs();
  if (chunk < 10)
    chunk = 10;
  #pragma omp parallel for default(shared) private(i,time,diff,asymFcnValue,f) schedule(dynamic,chunk) reduction(+:chisq)
  #endif
  for (i=fStartTimeBin; i<fEndTimeBin; ++i) {
    time = fData.GetDataTimeStart() + static_cast<Double_t>(i)*fData.GetDataTimeStep();
    f = fTheory->Func(time, par, fFuncValues);
    asymFcnValue = (f*(a*b+1.0)-(a-1.0))/((a+1.0)-f*(a*b-1.0));
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
Double_t PRunAsymmetryRRF::CalcChiSquareExpected(const std::vector<Double_t>& par)
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
Double_t PRunAsymmetryRRF::CalcMaxLikelihood(const std::vector<Double_t>& par)
{
  std::cout << std::endl << "PRunAsymmetryRRF::CalcMaxLikelihood(): not implemented yet ..." << std::endl;

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
UInt_t PRunAsymmetryRRF::GetNoOfFitBins()
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
void PRunAsymmetryRRF::SetFitRangeBin(const TString fitRange)
{
  TObjArray *tok = nullptr;
  TObjString *ostr = nullptr;
  TString str;
  Ssiz_t idx = -1;
  Int_t offset = 0;

  tok = fitRange.Tokenize(" \t");

  if (tok->GetEntries() == 3) { // structure FIT_RANGE fgb+n0 lgb-n1
    // handle fgb+n0 entry
    ostr = static_cast<TObjString*>(tok->At(1));
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
    ostr = static_cast<TObjString*>(tok->At(2));
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
      std::cerr << std::endl << ">> PRunSingleHisto::SetFitRangeBin(): **ERROR** invalid FIT_RANGE command found: '" << fitRange << "'";
      std::cerr << std::endl << ">> will ignore it. Sorry ..." << std::endl;
    } else {
      // handle fgb+n0 entry
      ostr = static_cast<TObjString*>(tok->At(pos));
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
      ostr = static_cast<TObjString*>(tok->At(pos+1));
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
    std::cerr << std::endl << ">> PRunSingleHisto::SetFitRangeBin(): **ERROR** invalid FIT_RANGE command found: '" << fitRange << "'";
    std::cerr << std::endl << ">> will ignore it. Sorry ..." << std::endl;
  }

  // clean up
  if (tok) {
    delete tok;
  }
}

//--------------------------------------------------------------------------
// CalcNoOfFitBins (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate the number of fitted bins for the current fit range.
 */
void PRunAsymmetryRRF::CalcNoOfFitBins()
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
void PRunAsymmetryRRF::CalcTheory()
{
  // feed the parameter vector
  std::vector<Double_t> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (UInt_t i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par, fMetaData);
  }

  // calculate asymmetry
  Double_t asymFcnValue = 0.0;
  Double_t a, b, f;
  Double_t time;
  for (UInt_t i=0; i<fData.GetValue()->size(); i++) {
    time = fData.GetDataTimeStart() + static_cast<Double_t>(i)*fData.GetDataTimeStep();
    switch (fAlphaBetaTag) {
      case 1: // alpha == 1, beta == 1
        asymFcnValue = fTheory->Func(time, par, fFuncValues);
        break;
      case 2: // alpha != 1, beta == 1
        if (fRunInfo->GetAlphaParamNo() < MSR_PARAM_FUN_OFFSET) { // alpha is a parameter
          a = par[fRunInfo->GetAlphaParamNo()-1];
        } else { // alpha is function
          // get function number
          UInt_t funNo = fRunInfo->GetAlphaParamNo()-MSR_PARAM_FUN_OFFSET;
          // evaluate function
          a = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
        }
        f = fTheory->Func(time, par, fFuncValues);
        asymFcnValue = (f*(a+1.0)-(a-1.0))/((a+1.0)-f*(a-1.0));
        break;
      case 3: // alpha == 1, beta != 1
        if (fRunInfo->GetBetaParamNo() < MSR_PARAM_FUN_OFFSET) { // beta is a parameter
          b = par[fRunInfo->GetBetaParamNo()-1];
        } else { // beta is a function
          // get function number
          UInt_t funNo = fRunInfo->GetBetaParamNo()-MSR_PARAM_FUN_OFFSET;
          // evaluate function
          b = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
        }
        f = fTheory->Func(time, par, fFuncValues);
        asymFcnValue = f*(b+1.0)/(2.0-f*(b-1.0));
        break;
      case 4: // alpha != 1, beta != 1
        if (fRunInfo->GetAlphaParamNo() < MSR_PARAM_FUN_OFFSET) { // alpha is a parameter
          a = par[fRunInfo->GetAlphaParamNo()-1];
        } else { // alpha is function
          // get function number
          UInt_t funNo = fRunInfo->GetAlphaParamNo()-MSR_PARAM_FUN_OFFSET;
          // evaluate function
          a = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
        }
        if (fRunInfo->GetBetaParamNo() < MSR_PARAM_FUN_OFFSET) { // beta is a parameter
          b = par[fRunInfo->GetBetaParamNo()-1];
        } else { // beta is a function
          // get function number
          UInt_t funNo = fRunInfo->GetBetaParamNo()-MSR_PARAM_FUN_OFFSET;
          // evaluate function
          b = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
        }
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
Bool_t PRunAsymmetryRRF::PrepareData()
{
  if (!fValid)
    return false;

  // keep the Global block info
  PMsrGlobalBlock *globalBlock = fMsrInfo->GetMsrGlobal();

  // get forward/backward histo from PRunDataHandler object ------------------------
  // get the correct run
  PRawRunData *runData = fRawData->GetRunData(*(fRunInfo->GetRunName()));
  if (!runData) { // run not found
    std::cerr << std::endl << ">> PRunAsymmetryRRF::PrepareData(): **ERROR** Couldn't get run " << fRunInfo->GetRunName()->Data() << "!";
    std::cerr << std::endl;
    return false;
  }

  // keep the field from the meta-data from the data-file
  fMetaData.fField = runData->GetField();

  // keep the energy from the meta-data from the data-file
  fMetaData.fEnergy = runData->GetEnergy();

  // keep the temperature(s) from the meta-data from the data-file
  for (unsigned int i=0; i<runData->GetNoOfTemperatures(); i++)
    fMetaData.fTemp.push_back(runData->GetTemperature(i));

  // collect histogram numbers
  PUIntVector forwardHistoNo;
  PUIntVector backwardHistoNo;
  for (UInt_t i=0; i<fRunInfo->GetForwardHistoNoSize(); i++) {
    forwardHistoNo.push_back(fRunInfo->GetForwardHistoNo(i));

    if (!runData->IsPresent(forwardHistoNo[i])) {
      std::cerr << std::endl << ">> PRunAsymmetryRRF::PrepareData(): **PANIC ERROR**:";
      std::cerr << std::endl << ">> forwardHistoNo found = " << forwardHistoNo[i] << ", which is NOT present in the data file!?!?";
      std::cerr << std::endl << ">> Will quit :-(";
      std::cerr << std::endl;
      // clean up
      forwardHistoNo.clear();
      backwardHistoNo.clear();
      return false;
    }
  }
  for (UInt_t i=0; i<fRunInfo->GetBackwardHistoNoSize(); i++) {
    backwardHistoNo.push_back(fRunInfo->GetBackwardHistoNo(i));

    if (!runData->IsPresent(backwardHistoNo[i])) {
      std::cerr << std::endl << ">> PRunAsymmetryRRF::PrepareData(): **PANIC ERROR**:";
      std::cerr << std::endl << ">> backwardHistoNo found = " << backwardHistoNo[i] << ", which is NOT present in the data file!?!?";
      std::cerr << std::endl << ">> Will quit :-(";
      std::cerr << std::endl;
      // clean up
      forwardHistoNo.clear();
      backwardHistoNo.clear();
      return false;
    }
  }

  // keep the time resolution in (us)
  fTimeResolution = runData->GetTimeResolution()/1.0e3;
  std::cout.precision(10);
  std::cout << std::endl << ">> PRunAsymmetryRRF::PrepareData(): time resolution=" << std::fixed << runData->GetTimeResolution() << "(ns)" << std::endl;

  // get all the proper t0's and addt0's for the current RUN block
  if (!GetProperT0(runData, globalBlock, forwardHistoNo, backwardHistoNo)) {
    return false;
  }

  // keep the histo of each group at this point (addruns handled below)
  std::vector<PDoubleVector> forward, backward;
  forward.resize(forwardHistoNo.size());   // resize to number of groups
  for (UInt_t i=0; i<forwardHistoNo.size(); i++) {
    forward[i].resize(runData->GetDataBin(forwardHistoNo[i])->size());
    forward[i]  = *runData->GetDataBin(forwardHistoNo[i]);
  }
  backward.resize(backwardHistoNo.size()); // resize to number of groups
  for (UInt_t i=0; i<backwardHistoNo.size(); i++) {
    backward[i].resize(runData->GetDataBin(backwardHistoNo[i])->size());
    backward[i] = *runData->GetDataBin(backwardHistoNo[i]);
  }

  // check if addrun's are present, and if yes add data
  // check if there are runs to be added to the current one
  if (fRunInfo->GetRunNameSize() > 1) { // runs to be added present
    PRawRunData *addRunData;
    for (UInt_t i=1; i<fRunInfo->GetRunNameSize(); i++) {
      // get run to be added to the main one
      addRunData = fRawData->GetRunData(*(fRunInfo->GetRunName(i)));
      if (addRunData == nullptr) { // couldn't get run
        std::cerr << std::endl << ">> PRunAsymmetryRRF::PrepareData(): **ERROR** Couldn't get addrun " << fRunInfo->GetRunName(i)->Data() << "!";
        std::cerr << std::endl;
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
  fForward.resize(forward[0].size());
  fBackward.resize(backward[0].size());
  for (UInt_t i=0; i<fForward.size(); i++) {
    fForward[i]  = forward[0][i];
    fBackward[i] = backward[0][i];
  }

  // group histograms, add all the remaining forward histograms of the group
  for (UInt_t i=1; i<forwardHistoNo.size(); i++) { // loop over the groupings
    for (UInt_t j=0; j<runData->GetDataBin(forwardHistoNo[i])->size(); j++) { // loop over the bin indices
      // make sure that the index stays within proper range
      if (((Int_t)j+fT0s[2*i]-fT0s[0] >= 0) && (j+fT0s[2*i]-fT0s[0] < runData->GetDataBin(forwardHistoNo[i])->size())) {
        fForward[j] += forward[i][j+(Int_t)fT0s[2*i]-(Int_t)fT0s[0]];
      }
    }
  }

  // group histograms, add all the remaining backward histograms of the group
  for (UInt_t i=1; i<backwardHistoNo.size(); i++) { // loop over the groupings
    for (UInt_t j=0; j<runData->GetDataBin(backwardHistoNo[i])->size(); j++) { // loop over the bin indices
      // make sure that the index stays within proper range
      if ((j+fT0s[2*i+1]-fT0s[1] >= 0) && (j+fT0s[2*i+1]-fT0s[1] < runData->GetDataBin(backwardHistoNo[i])->size())) {
        fBackward[j] += backward[i][j+(Int_t)fT0s[2*i+1]-(Int_t)fT0s[1]];
      }
    }
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
      std::cerr << std::endl << ">> PRunAsymmetryRRF::PrepareData(): **WARNING** Neither fix background nor background bins are given!";
      std::cerr << std::endl << ">> Will try the following:";
      std::cerr << std::endl << ">> forward:  bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
      std::cerr << std::endl << ">> backward: bkg start = " << fRunInfo->GetBkgRange(2) << ", bkg end = " << fRunInfo->GetBkgRange(3);
      std::cerr << std::endl << ">> NO WARRANTY THAT THIS MAKES ANY SENSE! Better check ...";
      std::cerr << std::endl;
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
Bool_t PRunAsymmetryRRF::SubtractFixBkg()
{
  Double_t dval;
  for (UInt_t i=0; i<fForward.size(); i++) {
    // keep the error, and make sure that the bin is NOT empty
    if (fForward[i] != 0.0)
      dval = TMath::Sqrt(fForward[i]);
    else
      dval = 1.0;
    fForwardErr.push_back(dval);
    fForward[i] -= fRunInfo->GetBkgFix(0);

    // keep the error, and make sure that the bin is NOT empty
    if (fBackward[i] != 0.0)
      dval = TMath::Sqrt(fBackward[i]);
    else
      dval = 1.0;
    fBackwardErr.push_back(dval);
    fBackward[i] -= fRunInfo->GetBkgFix(1);
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
Bool_t PRunAsymmetryRRF::SubtractEstimatedBkg()
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
  UInt_t start[2] = {static_cast<UInt_t>(fRunInfo->GetBkgRange(0)), static_cast<UInt_t>(fRunInfo->GetBkgRange(2))};
  UInt_t end[2]   = {static_cast<UInt_t>(fRunInfo->GetBkgRange(1)), static_cast<UInt_t>(fRunInfo->GetBkgRange(3))};
  for (UInt_t i=0; i<2; i++) {
    if (end[i] < start[i]) {
      std::cout << std::endl << "PRunAsymmetryRRF::SubtractEstimatedBkg(): end = " << end[i] << " > start = " << start[i] << "! Will swap them!";
      UInt_t keep = end[i];
      end[i] = start[i];
      start[i] = keep;
    }
  }

  // calculate proper background range
  for (UInt_t i=0; i<2; i++) {
    if (beamPeriod != 0.0) {
      Double_t timeBkg = static_cast<Double_t>(end[i]-start[i])*fTimeResolution; // length of the background intervall in time
      UInt_t fullCycles = static_cast<UInt_t>(timeBkg/beamPeriod); // how many proton beam cylces can be placed within the proposed background intervall
      // correct the end of the background intervall such that the background is as close as possible to a multiple of the proton cylce
      end[i] = start[i] + static_cast<UInt_t>((fullCycles*beamPeriod)/fTimeResolution);
      std::cout << "PRunAsymmetryRRF::SubtractEstimatedBkg(): Background " << start[i] << ", " << end[i] << std::endl;
      if (end[i] == start[i])
        end[i] = fRunInfo->GetBkgRange(2*i+1);
    }
  }

  // check if start is within histogram bounds
  if ((start[0] >= fForward.size()) || (start[1] >= fBackward.size())) {
    std::cerr << std::endl << ">> PRunAsymmetryRRF::SubtractEstimatedBkg(): **ERROR** background bin values out of bound!";
    std::cerr << std::endl << ">> histo lengths (f/b)  = (" << fForward.size() << "/" << fBackward.size() << ").";
    std::cerr << std::endl << ">> background start (f/b) = (" << start[0] << "/" << start[1] << ").";
    return false;
  }

  // check if end is within histogram bounds
  if ((end[0] >= fForward.size()) || (end[1] >= fBackward.size())) {
    std::cerr << std::endl << ">> PRunAsymmetryRRF::SubtractEstimatedBkg(): **ERROR** background bin values out of bound!";
    std::cerr << std::endl << ">> histo lengths (f/b)  = (" << fForward.size() << "/" << fBackward.size() << ").";
    std::cerr << std::endl << ">> background end (f/b) = (" << end[0] << "/" << end[1] << ").";
    return false;
  }

  // calculate background
  Double_t bkg[2]    = {0.0, 0.0};
  Double_t errBkg[2] = {0.0, 0.0};

  // forward
  for (UInt_t i=start[0]; i<=end[0]; i++)
    bkg[0] += fForward[i];
  errBkg[0] = TMath::Sqrt(bkg[0])/(end[0] - start[0] + 1);
  bkg[0] /= static_cast<Double_t>(end[0] - start[0] + 1);
  std::cout << std::endl << ">> estimated forward histo background: " << bkg[0];

  // backward
  for (UInt_t i=start[1]; i<=end[1]; i++)
    bkg[1] += fBackward[i];
  errBkg[1] = TMath::Sqrt(bkg[1])/(end[1] - start[1] + 1);
  bkg[1] /= static_cast<Double_t>(end[1] - start[1] + 1);
  std::cout << std::endl << ">> estimated backward histo background: " << bkg[1] << std::endl;

  // correct error for forward, backward
  Double_t errVal = 0.0;
  for (UInt_t i=0; i<fForward.size(); i++) {
    if (fForward[i] > 0.0)
      errVal = TMath::Sqrt(fForward[i]+errBkg[0]*errBkg[0]);
    else
      errVal = 1.0;
    fForwardErr.push_back(errVal);
    if (fBackward[i] > 0.0)
      errVal = TMath::Sqrt(fBackward[i]+errBkg[1]*errBkg[1]);
    else
      errVal = 1.0;
    fBackwardErr.push_back(errVal);
  }

  // subtract background from data
  for (UInt_t i=0; i<fForward.size(); i++) {
    fForward[i]  -= bkg[0];
    fBackward[i] -= bkg[1];
  }

  fRunInfo->SetBkgEstimated(bkg[0], 0);
  fRunInfo->SetBkgEstimated(bkg[1], 1);

  return true;
}

//--------------------------------------------------------------------------
// PrepareFitData (protected)
//--------------------------------------------------------------------------
/**
 * <p>Take the pre-processed data (i.e. grouping and addrun are preformed, background correction already carried out)
 * and form the asymmetry for fitting.
 */
Bool_t PRunAsymmetryRRF::PrepareFitData()
{
  // transform raw histo data. At this point, the raw data are already background corrected.

  // 1st: form the asymmetry of the original data

  // forward and backward detectors might have different fgb-t0 offset. Take the maximum of both.
  Int_t fgbOffset = fGoodBins[0]-static_cast<Int_t>(fT0s[0]);
  if (fgbOffset < fGoodBins[2]-static_cast<Int_t>(fT0s[1]))
    fgbOffset = fGoodBins[2]-static_cast<Int_t>(fT0s[1]);
  // last good bin (lgb) is the minimum of forward/backward lgb
  Int_t lgb_offset = fGoodBins[1]-static_cast<Int_t>(fT0s[0])+fgbOffset;
  if (lgb_offset < fGoodBins[3]-static_cast<Int_t>(fT0s[1])+fgbOffset)
    lgb_offset = fGoodBins[3]-static_cast<Int_t>(fT0s[1])+fgbOffset;

  Int_t fgb = static_cast<Int_t>(fT0s[0])+fgbOffset;
  Int_t lgb = fgb + lgb_offset;
  Int_t dt0 = static_cast<Int_t>(fT0s[0])-static_cast<Int_t>(fT0s[1]);

  PDoubleVector asym;
  PDoubleVector asymErr;
  Double_t asymVal, asymValErr;
  Double_t ff, bb, eff, ebb;
  for (Int_t i=fgb; i<lgb; i++) {
    ff = fForward[i];
    bb = fBackward[i-dt0];
    if (ff+bb != 0.0)
      asymVal = (ff-bb)/(ff+bb);
    else
      asymVal = 0.0;
    asym.push_back(asymVal);
    eff = fForwardErr[i];
    ebb = fBackwardErr[i-dt0];
    if ((asymVal != 0.0) && (ff+bb) > 0.0)
      asymValErr = 2.0/pow((ff+bb),2.0)*sqrt(bb*bb*eff*eff+ff*ff*ebb*ebb);
    else
      asymValErr = 1.0;
    asymErr.push_back(asymValErr);
  }

  // 2nd: a_rrf = a * 2*cos(w_rrf*t + phi_rrf)
  PMsrGlobalBlock *globalBlock = fMsrInfo->GetMsrGlobal();
  Double_t wRRF = globalBlock->GetRRFFreq("Mc");
  Double_t phaseRRF = globalBlock->GetRRFPhase()*TMath::TwoPi()/180.0;

  Double_t startTime = fTimeResolution * static_cast<Double_t>(fgbOffset);
  Double_t time=0.0;
  for (UInt_t i=0; i<asym.size(); i++) {
    time = startTime + i*fTimeResolution;
    asym[i] *= 2.0*cos(wRRF*time+phaseRRF);
  }

  // 3rd: rrf packing
  PDoubleVector asymRRF;
  asymVal = 0.0;
  for (UInt_t i=0; i<asym.size(); i++) {
    if ((i+1) % fRRFPacking == 0) {
      asymRRF.push_back(asymVal/fRRFPacking);
      asymVal = 0.0;
    }
    asymVal += asym[i];
  }

  // 4th: rrf packing error
  PDoubleVector asymRRFErr;
  asymValErr = 0.0;
  for (UInt_t i=0; i<asymErr.size(); i++) {
    if ((i+1) % fRRFPacking == 0) {
      asymRRFErr.push_back(sqrt(2.0*asymValErr)/fRRFPacking); // factor of two is needed due to the rescaling
      asymValErr = 0.0;
    }
    asymValErr += asymErr[i]*asymErr[i];
  }

  fData.SetDataTimeStart(startTime+fTimeResolution*(static_cast<Double_t>(fRRFPacking-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*static_cast<Double_t>(fRRFPacking));

  for (UInt_t i=0; i<asymRRF.size(); i++) {
    fData.AppendValue(asymRRF[i]);
    fData.AppendErrorValue(asymRRFErr[i]);
  }

  CalcNoOfFitBins();

  // clean up
  fForward.clear();
  fForwardErr.clear();
  fBackward.clear();
  fBackwardErr.clear();
  asym.clear();
  asymErr.clear();
  asymRRF.clear();
  asymRRFErr.clear();

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
Bool_t PRunAsymmetryRRF::PrepareViewData(PRawRunData* runData, UInt_t histoNo[2])
{
  // feed the parameter vector
  std::vector<Double_t> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (UInt_t i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // first get start data, end data, and t0
  Int_t start[2] = {fGoodBins[0], fGoodBins[2]};
  Int_t end[2] = {fGoodBins[1], fGoodBins[3]};
  Int_t t0[2] = {static_cast<Int_t>(fT0s[0]), static_cast<Int_t>(fT0s[1])};

  // check if the data ranges and t0's between forward/backward are compatible
  Int_t fgb[2];
  if (start[0]-t0[0] != start[1]-t0[1]) { // wrong fgb aligning
    if (abs(start[0]-t0[0]) > abs(start[1]-t0[1])) {
      fgb[0] = start[0];
      fgb[1] = t0[1] + start[0]-t0[0];
      std::cerr << std::endl << ">> PRunAsymmetryRRF::PrepareViewData(): **WARNING** needed to shift backward fgb from ";
      std::cerr << start[1] << " to " << fgb[1] << std::endl;
    } else {
      fgb[0] = t0[0] + start[1]-t0[1];
      fgb[1] = start[1];
      std::cerr << std::endl << ">> PRunAsymmetryRRF::PrepareViewData(): **WARNING** needed to shift forward fgb from ";
      std::cerr << start[0] << " to " << fgb[0] << std::endl;
    }
  } else { // fgb aligning is correct
    fgb[0] = start[0];
    fgb[1] = start[1];
  }
  start[0] = fgb[0];
  start[1] = fgb[1];

  // make sure that there are equal number of bins in forward and backward
  UInt_t noOfBins0 = runData->GetDataBin(histoNo[0])->size()-start[0];
  UInt_t noOfBins1 = runData->GetDataBin(histoNo[1])->size()-start[1];
  if (noOfBins0 > noOfBins1)
    noOfBins0 = noOfBins1;
  end[0] = start[0] + noOfBins0;
  end[1] = start[1] + noOfBins0;

  // check if start, end, and t0 make any sense
  for (UInt_t i=0; i<2; i++) {
    // 1st check if start is within proper bounds
    if ((start[i] < 0) || (start[i] > static_cast<Int_t>(runData->GetDataBin(histoNo[i])->size()))) {
      std::cerr << std::endl << ">> PRunAsymmetryRRF::PrepareViewData(): **ERROR** start data bin doesn't make any sense!";
      std::cerr << std::endl;
      return false;
    }
    // 2nd check if end is within proper bounds
    if ((end[i] < 0) || (end[i] > static_cast<Int_t>(runData->GetDataBin(histoNo[i])->size()))) {
      std::cerr << std::endl << ">> PRunAsymmetryRRF::PrepareViewData(): **ERROR** end data bin doesn't make any sense!";
      std::cerr << std::endl;
      return false;
    }
    // 3rd check if t0 is within proper bounds
    if ((t0[i] < 0) || (t0[i] > static_cast<Int_t>(runData->GetDataBin(histoNo[i])->size()))) {
      std::cerr << std::endl << ">> PRunAsymmetryRRF::PrepareViewData(): **ERROR** t0 data bin doesn't make any sense!";
      std::cerr << std::endl;
      return false;
    }
  }

  // check if forward and backward histo have the same size, otherwise take the minimum size
  UInt_t noOfBins = fForward.size();
  if (noOfBins > fBackward.size()) {
    noOfBins = fBackward.size();
  }

  // form asymmetry including error propagation
  Double_t asym, error;
  Double_t f, b, ef, eb, alpha = 1.0, beta = 1.0;

  // get the proper alpha and beta
  switch (fAlphaBetaTag) {
    case 1: // alpha == 1, beta == 1
      alpha = 1.0;
      beta  = 1.0;
      break;
    case 2: // alpha != 1, beta == 1
      if (fRunInfo->GetAlphaParamNo() < MSR_PARAM_FUN_OFFSET) { // alpha is a parameter
        alpha = par[fRunInfo->GetAlphaParamNo()-1];
      } else { // alpha is function
        // get function number
        UInt_t funNo = fRunInfo->GetAlphaParamNo()-MSR_PARAM_FUN_OFFSET;
        // evaluate function
        alpha = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
      }
      beta  = 1.0;
      break;
    case 3: // alpha == 1, beta != 1
      alpha = 1.0;
      if (fRunInfo->GetBetaParamNo() < MSR_PARAM_FUN_OFFSET) { // beta is a parameter
        beta = par[fRunInfo->GetBetaParamNo()-1];
      } else { // beta is a function
        // get function number
        UInt_t funNo = fRunInfo->GetBetaParamNo()-MSR_PARAM_FUN_OFFSET;
        // evaluate function
        beta = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
      }
      break;
    case 4: // alpha != 1, beta != 1
      if (fRunInfo->GetAlphaParamNo() < MSR_PARAM_FUN_OFFSET) { // alpha is a parameter
        alpha = par[fRunInfo->GetAlphaParamNo()-1];
      } else { // alpha is function
        // get function number
        UInt_t funNo = fRunInfo->GetAlphaParamNo()-MSR_PARAM_FUN_OFFSET;
        // evaluate function
        alpha = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
      }
      if (fRunInfo->GetBetaParamNo() < MSR_PARAM_FUN_OFFSET) { // beta is a parameter
        beta = par[fRunInfo->GetBetaParamNo()-1];
      } else { // beta is a function
        // get function number
        UInt_t funNo = fRunInfo->GetBetaParamNo()-MSR_PARAM_FUN_OFFSET;
        // evaluate function
        beta = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
      }
      break;
    default:
      break;
  }

  PDoubleVector asymVec, asymErr;
  Int_t dtBin = start[1]-start[0];
  for (Int_t i=start[0]; i<end[0]; i++) {
    // to make the formulae more readable
    f  = fForward[i];
    b  = fBackward[i+dtBin];
    ef = fForwardErr[i];
    eb = fBackwardErr[i+dtBin];
    // check that there are indeed bins
    if (f+b != 0.0)
      asym = (alpha*f-b) / (alpha*beta*f+b);
    else
      asym = 0.0;
    asymVec.push_back(asym);
    // calculate the error
    if (f+b != 0.0)
      error = 2.0/((f+b)*(f+b))*TMath::Sqrt(b*b*ef*ef+eb*eb*f*f);
    else
      error = 1.0;
    asymErr.push_back(error);
  }

  // RRF transform
  // a_rrf = a * 2*cos(w_rrf*t + phi_rrf)
  PMsrGlobalBlock *globalBlock = fMsrInfo->GetMsrGlobal();
  Double_t wRRF = globalBlock->GetRRFFreq("Mc");
  Double_t phaseRRF = globalBlock->GetRRFPhase()*TMath::TwoPi()/180.0;
  Double_t startTime=fTimeResolution*(static_cast<Double_t>(start[0])-t0[0]);
  Double_t time = 0.0;
  for (UInt_t i=0; i<asymVec.size(); i++) {
    time = startTime + i * fTimeResolution;
    asymVec[i] *= 2.0*cos(wRRF*time+phaseRRF); // factor of 2 needed to keep the asymmetry
  }

  Double_t dval = 0.0;
  PDoubleVector asymRRF;
  for (UInt_t i=0; i<asymVec.size(); i++) {
    if ((i+1) % fRRFPacking == 0) {
      asymRRF.push_back(dval/fRRFPacking);
      dval = 0.0;
    }
    dval += asymVec[i];
  }

  // RRF packing error
  PDoubleVector asymRRFErr;
  dval = 0.0;
  for (UInt_t i=0; i<asymErr.size(); i++) {
    if ((i+1) % fRRFPacking == 0) {
      asymRRFErr.push_back(sqrt(2.0*dval)/fRRFPacking); // factor of two is needed due to the rescaling
      dval = 0.0;
    }
    dval += asymErr[i]*asymErr[i];
  }

  fData.SetDataTimeStart(startTime+fTimeResolution*(static_cast<Double_t>(fRRFPacking-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*static_cast<Double_t>(fRRFPacking));

  for (UInt_t i=0; i<asymRRF.size(); i++) {
    fData.AppendValue(asymRRF[i]);
    fData.AppendErrorValue(asymRRFErr[i]);
  }

  CalcNoOfFitBins();

  // clean up
  fForward.clear();
  fForwardErr.clear();
  fBackward.clear();
  fBackwardErr.clear();
  asymVec.clear();
  asymErr.clear();
  asymRRF.clear();
  asymRRFErr.clear();

  // fill theory vector for kView
  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par, fMetaData);
  }

  // calculate theory
  UInt_t size = runData->GetDataBin(histoNo[0])->size();
  Int_t factor = 8; // 8 times more points for the theory (if fTheoAsData == false)
  fData.SetTheoryTimeStart(fData.GetDataTimeStart());
  if (fTheoAsData) { // calculate theory only at the data points
    fData.SetTheoryTimeStep(fData.GetDataTimeStep());
  } else {
    // finer binning for the theory (8 times as many points = factor)
    size *= factor;
    fData.SetTheoryTimeStep(fData.GetDataTimeStep()/(Double_t)factor);
  }

  for (UInt_t i=0; i<size; i++) {
    time = fData.GetTheoryTimeStart() + static_cast<Double_t>(i)*fData.GetTheoryTimeStep();
    dval = fTheory->Func(time, par, fFuncValues);
    if (fabs(dval) > 10.0) {  // dirty hack needs to be fixed!!
      dval = 0.0;
    }
    fData.AppendTheoryValue(dval);
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
Bool_t PRunAsymmetryRRF::GetProperT0(PRawRunData* runData, PMsrGlobalBlock *globalBlock, PUIntVector &forwardHistoNo, PUIntVector &backwardHistoNo)
{
  // feed all T0's
  // first init T0's, T0's are stored as (forward T0, backward T0, etc.)
  fT0s.clear();
  // this strange fT0 size estimate is needed in case #forw histos != #back histos
  size_t size = 2*forwardHistoNo.size();
  if (backwardHistoNo.size() > forwardHistoNo.size())
    size = 2*backwardHistoNo.size();
  fT0s.resize(size);
  for (UInt_t i=0; i<fT0s.size(); i++) {
    fT0s[i] = -1.0;
  }

  // fill in the T0's from the msr-file (if present)
  for (UInt_t i=0; i<fRunInfo->GetT0BinSize(); i++) {
    fT0s[i] = fRunInfo->GetT0Bin(i);
  }

  // fill in the missing T0's from the GLOBAL block section (if present)
  for (UInt_t i=0; i<globalBlock->GetT0BinSize(); i++) {
    if (fT0s[i] == -1.0) { // i.e. not given in the RUN block section
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

      std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperT0(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
      std::cerr << std::endl << ">> run: " << fRunInfo->GetRunName()->Data();
      std::cerr << std::endl << ">> will try the estimated one: forward t0 = " << runData->GetT0BinEstimated(forwardHistoNo[i]);
      std::cerr << std::endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
      std::cerr << std::endl;
    }
  }
  for (UInt_t i=0; i<backwardHistoNo.size(); i++) {
    if (fT0s[2*i+1] == -1.0) { // i.e. not present in the msr-file and data file, use the estimated T0
      fT0s[2*i+1] = runData->GetT0BinEstimated(backwardHistoNo[i]);
      fRunInfo->SetT0Bin(fT0s[2*i+1], 2*i+1);

      std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperT0(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
      std::cerr << std::endl << ">> run: " << fRunInfo->GetRunName()->Data();
      std::cerr << std::endl << ">> will try the estimated one: backward t0 = " << runData->GetT0BinEstimated(backwardHistoNo[i]);
      std::cerr << std::endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
      std::cerr << std::endl;
    }
  }

  // check if t0 is within proper bounds
  for (UInt_t i=0; i<forwardHistoNo.size(); i++) {
    if ((fT0s[2*i] < 0) || (fT0s[2*i] > static_cast<Int_t>(runData->GetDataBin(forwardHistoNo[i])->size()))) {
      std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperT0(): **ERROR** t0 data bin (" << fT0s[2*i] << ") doesn't make any sense!";
      std::cerr << std::endl << ">> forwardHistoNo " << forwardHistoNo[i];
      std::cerr << std::endl;
      return false;
    }
  }
  for (UInt_t i=0; i<backwardHistoNo.size(); i++) {
    if ((fT0s[2*i+1] < 0) || (fT0s[2*i+1] > static_cast<Int_t>(runData->GetDataBin(backwardHistoNo[i])->size()))) {
      std::cerr << std::endl << ">> PRunAsymmetryRRF::PrepareData(): **ERROR** t0 data bin (" << fT0s[2*i+1] << ") doesn't make any sense!";
      std::cerr << std::endl << ">> backwardHistoNo " << backwardHistoNo[i];
      std::cerr << std::endl;
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
      if (addRunData == nullptr) { // couldn't get run
        std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperT0(): **ERROR** Couldn't get addrun " << fRunInfo->GetRunName(i)->Data() << "!";
        std::cerr << std::endl;
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
      for (Int_t j=0; j<fRunInfo->GetAddT0BinSize(i-1); j++) {
        fAddT0s[i-1][j] = fRunInfo->GetAddT0Bin(i-1, j);
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

          std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperT0(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
          std::cerr << std::endl << ">> run: " << fRunInfo->GetRunName(i)->Data();
          std::cerr << std::endl << ">> will try the estimated one: forward t0 = " << addRunData->GetT0BinEstimated(forwardHistoNo[j]);
          std::cerr << std::endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
          std::cerr << std::endl;
        }
      }
      for (UInt_t j=0; j<backwardHistoNo.size(); j++) {
        if (fAddT0s[i-1][2*j+1] == -1.0) { // i.e. not present in the msr-file and data file, use the estimated T0
          fAddT0s[i-1][2*j+1] = addRunData->GetT0BinEstimated(backwardHistoNo[j]);
          fRunInfo->SetAddT0Bin(fAddT0s[i-1][2*j+1], i-1, 2*j+1);

          std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperT0(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
          std::cerr << std::endl << ">> run: " << fRunInfo->GetRunName(i)->Data();
          std::cerr << std::endl << ">> will try the estimated one: backward t0 = " << runData->GetT0BinEstimated(backwardHistoNo[j]);
          std::cerr << std::endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
          std::cerr << std::endl;
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
Bool_t PRunAsymmetryRRF::GetProperDataRange(PRawRunData* runData, UInt_t histoNo[2])
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
  Int_t offset = static_cast<Int_t>(10.0e-3/fTimeResolution); // needed in case first good bin is not given, default = 10ns

  // check if data range has been provided, and if not try to estimate them
  if (start[0] < 0) {
    start[0] = static_cast<Int_t>(t0[0])+offset;
    fRunInfo->SetDataRange(start[0], 0);
    std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperDataRange(): **WARNING** data range (forward) was not provided, will try data range start = t0+" << offset << "(=10ns) = " << start[0] << ".";
    std::cerr << std::endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    std::cerr << std::endl;
  }
  if (start[1] < 0) {
    start[1] = static_cast<Int_t>(t0[1])+offset;
    fRunInfo->SetDataRange(start[1], 2);
    std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperDataRange(): **WARNING** data range (backward) was not provided, will try data range start = t0+" << offset << "(=10ns) = " << start[1] << ".";
    std::cerr << std::endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    std::cerr << std::endl;
  }
  if (end[0] < 0) {
    end[0] = runData->GetDataBin(histoNo[0])->size();
    fRunInfo->SetDataRange(end[0], 1);
    std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperDataRange(): **WARNING** data range (forward) was not provided, will try data range end = " << end[0] << ".";
    std::cerr << std::endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    std::cerr << std::endl;
  }
  if (end[1] < 0) {
    end[1] = runData->GetDataBin(histoNo[1])->size();
    fRunInfo->SetDataRange(end[1], 3);
    std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperDataRange(): **WARNING** data range (backward) was not provided, will try data range end = " << end[1] << ".";
    std::cerr << std::endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    std::cerr << std::endl;
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
    if ((start[i] < 0) || (start[i] > static_cast<Int_t>(runData->GetDataBin(histoNo[i])->size()))) {
      std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperDataRange(): **ERROR** start data bin doesn't make any sense!";
      std::cerr << std::endl;
      return false;
    }
    // 3rd check if end is within proper bounds
    if (end[i] < 0) {
      std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperDataRange(): **ERROR** end data bin (" << end[i] << ") doesn't make any sense!";
      std::cerr << std::endl;
      return false;
    }
    if (end[i] > static_cast<Int_t>(runData->GetDataBin(histoNo[i])->size())) {
      std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperDataRange(): **WARNING** end data bin (" << end[i] << ") > histo length (" << (Int_t)runData->GetDataBin(histoNo[i])->size() << ").";
      std::cerr << std::endl << ">>    Will set end = (histo length - 1). Consider to change it in the msr-file." << std::endl;
      std::cerr << std::endl;
      end[i] = static_cast<Int_t>(runData->GetDataBin(histoNo[i])->size())-1;
    }
    // 4th check if t0 is within proper bounds
    if ((t0[i] < 0) || (t0[i] > static_cast<Int_t>(runData->GetDataBin(histoNo[i])->size()))) {
      std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperDataRange(): **ERROR** t0 data bin doesn't make any sense!";
      std::cerr << std::endl;
      return false;
    }
  }

  // check that start-t0 is the same for forward as for backward, otherwise take max(start[i]-t0[i])
  if (fabs(static_cast<Double_t>(start[0])-t0[0]) > fabs(static_cast<Double_t>(start[1])-t0[1])){
    start[1] = static_cast<Int_t>(t0[1] + static_cast<Double_t>(start[0]) - t0[0]);
    end[1] = static_cast<Int_t>(t0[1] + static_cast<Double_t>(end[0]) - t0[0]);
    std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperDataRange **WARNING** needed to shift backward data range.";
    std::cerr << std::endl << ">> given: " << fRunInfo->GetDataRange(2) << ", " << fRunInfo->GetDataRange(3);
    std::cerr << std::endl << ">> used : " << start[1] << ", " << end[1];
    std::cerr << std::endl;
  }
  if (fabs(static_cast<Double_t>(start[0])-t0[0]) < fabs(static_cast<Double_t>(start[1])-t0[1])){
    start[0] = static_cast<Int_t>(t0[0] + static_cast<Double_t>(start[1]) - t0[1]);
    end[0] = static_cast<Int_t>(t0[0] + static_cast<Double_t>(end[1]) - t0[1]);
    std::cerr << std::endl << ">> PRunAsymmetryRRF::GetProperDataRange **WARNING** needed to shift forward data range.";
    std::cerr << std::endl << ">> given: " << fRunInfo->GetDataRange(0) << ", " << fRunInfo->GetDataRange(1);
    std::cerr << std::endl << ">> used : " << start[0] << ", " << end[0];
    std::cerr << std::endl;
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
void PRunAsymmetryRRF::GetProperFitRange(PMsrGlobalBlock *globalBlock)
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
    std::cerr << ">> PRunSingleHisto::GetProperFitRange(): **WARNING** Couldn't get fit start/end time!" << std::endl;
    std::cerr << ">>    Will set it to fgb/lgb which given in time is: " << fFitStartTime << "..." << fFitEndTime << " (usec)" << std::endl;
  }
}
