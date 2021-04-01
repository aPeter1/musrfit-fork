/***************************************************************************

  PRunSingleHisto.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2021 by Andreas Suter                              *
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

#include <cmath>
#include <iostream>
#include <fstream>

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
  fBackground = 0;
  fPacking = -1;
  fTheoAsData = false;

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
PRunSingleHisto::PRunSingleHisto(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag, Bool_t theoAsData) :
  PRunBase(msrInfo, rawData, runNo, tag), fTheoAsData(theoAsData)
{
  fScaleN0AndBkg = IsScaleN0AndBkg();
  fNoOfFitBins  = 0;
  fBackground = 0;

  fPacking = fRunInfo->GetPacking();
  if (fPacking == -1) { // i.e. packing is NOT given in the RUN-block, it must be given in the GLOBAL-block
    fPacking = fMsrInfo->GetMsrGlobal()->GetPacking();
  }
  if (fPacking == -1) { // this should NOT happen, somethin is severely wrong
    std::cerr << std::endl << ">> PRunSingleHisto::PRunSingleHisto: **SEVERE ERROR**: Couldn't find any packing information!";
    std::cerr << std::endl << ">> This is very bad :-(, will quit ...";
    std::cerr << std::endl;
    fValid = false;
    return;
  }

  // the 2 following variables are need in case fit range is given in bins, and since
  // the fit range can be changed in the command block, these variables need to be accessible
  fGoodBins[0] = -1;
  fGoodBins[1] = -1;

  fStartTimeBin = -1;
  fEndTimeBin   = -1;

  if (!PrepareData()) {
    std::cerr << std::endl << ">> PRunSingleHisto::PRunSingleHisto: **SEVERE ERROR**: Couldn't prepare data for fitting!";
    std::cerr << std::endl << ">> This is very bad :-(, will quit ...";
    std::cerr << std::endl;
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
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
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
    UInt_t funcNo = fMsrInfo->GetFuncNo(i);
    fFuncValues[i] = fMsrInfo->EvalFunc(funcNo, *fRunInfo->GetMap(), par, fMetaData);
  }

  // calculate chi square
  Double_t time(1.0);
  Int_t i;

  // Calculate the theory function once to ensure one function evaluation for the current set of parameters.
  // This is needed for the LF and user functions where some non-thread-save calculations only need to be calculated once
  // for a given set of parameters---which should be done outside of the parallelized loop.
  // For all other functions it means a tiny and acceptable overhead.
  time = fTheory->Func(time, par, fFuncValues);

  #ifdef HAVE_GOMP
  Int_t chunk = (fEndTimeBin - fStartTimeBin)/omp_get_num_procs();
  if (chunk < 10)
    chunk = 10;
  #pragma omp parallel for default(shared) private(i,time,diff) schedule(dynamic,chunk) reduction(+:chisq)
  #endif
  for (i=fStartTimeBin; i<fEndTimeBin; ++i) {
    time = fData.GetDataTimeStart() + static_cast<Double_t>(i)*fData.GetDataTimeStep();
    diff = fData.GetValue()->at(i) -
          (N0*TMath::Exp(-time/tau)*(1.0+fTheory->Func(time, par, fFuncValues))+bkg);
    chisq += diff*diff / (fData.GetError()->at(i)*fData.GetError()->at(i));
  }

  // the correction factor is need since the data scales like pack*t_res,
  // whereas the error scales like sqrt(pack*t_res)
  if (fScaleN0AndBkg)
    chisq *= fPacking * (fTimeResolution * 1.0e3);

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
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
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
    fFuncValues[i] = fMsrInfo->EvalFunc(funcNo, *fRunInfo->GetMap(), par, fMetaData);
  }

  // calculate chi square
  Double_t time(1.0);
  Int_t i;

  // Calculate the theory function once to ensure one function evaluation for the current set of parameters.
  // This is needed for the LF and user functions where some non-thread-save calculations only need to be calculated once
  // for a given set of parameters---which should be done outside of the parallelized loop.
  // For all other functions it means a tiny and acceptable overhead.
  time = fTheory->Func(time, par, fFuncValues);

  #ifdef HAVE_GOMP
  Int_t chunk = (fEndTimeBin - fStartTimeBin)/omp_get_num_procs();
  if (chunk < 10)
    chunk = 10;
  #pragma omp parallel for default(shared) private(i,time,theo,diff) schedule(dynamic,chunk) reduction(+:chisq)
  #endif
  for (i=fStartTimeBin; i<fEndTimeBin; ++i) {
    time = fData.GetDataTimeStart() + static_cast<Double_t>(i)*fData.GetDataTimeStep();
    theo = N0*TMath::Exp(-time/tau)*(1.0+fTheory->Func(time, par, fFuncValues))+bkg;
    diff = fData.GetValue()->at(i) - theo;
    chisq += diff*diff / theo;
  }

  // the correction factor is need since the data scales like pack*t_res,
  // whereas the error scales like sqrt(pack*t_res)
  if (fScaleN0AndBkg)
    chisq *= fPacking * (fTimeResolution * 1.0e3);

  return chisq;
}

//--------------------------------------------------------------------------
// CalcMaxLikelihood (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate log maximum-likelihood. See http://pdg.lbl.gov/index.html
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
    Int_t funNo = fRunInfo->GetNormParamNo()-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
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
    UInt_t funcNo = fMsrInfo->GetFuncNo(i);
    fFuncValues[i] = fMsrInfo->EvalFunc(funcNo, *fRunInfo->GetMap(), par, fMetaData);
  }

  // calculate maximum log likelihood
  Double_t theo;
  Double_t data;
  Double_t time(1.0);
  Int_t i;

  // norm is needed since there is no simple scaling like in chisq case to get the correct Max.Log.Likelihood value when normlizing N(t) to 1/ns
  Double_t normalizer = 1.0;

  if (fScaleN0AndBkg)
    normalizer = fPacking * (fTimeResolution * 1.0e3);

  // Calculate the theory function once to ensure one function evaluation for the current set of parameters.
  // This is needed for the LF and user functions where some non-thread-save calculations only need to be calculated once
  // for a given set of parameters---which should be done outside of the parallelized loop.
  // For all other functions it means a tiny and acceptable overhead.
  time = fTheory->Func(time, par, fFuncValues);

  #ifdef HAVE_GOMP
  Int_t chunk = (fEndTimeBin - fStartTimeBin)/omp_get_num_procs();
  if (chunk < 10)
    chunk = 10;
  #pragma omp parallel for default(shared) private(i,time,theo,data) schedule(dynamic,chunk) reduction(-:mllh)
  #endif
  for (i=fStartTimeBin; i<fEndTimeBin; ++i) {
    time = fData.GetDataTimeStart() + static_cast<Double_t>(i)*fData.GetDataTimeStep();
    // calculate theory for the given parameter set
    theo = N0*TMath::Exp(-time/tau)*(1.0+fTheory->Func(time, par, fFuncValues))+bkg;

    data = fData.GetValue()->at(i);

    if (theo <= 0.0) {
      std::cerr << ">> PRunSingleHisto::CalcMaxLikelihood: **WARNING** NEGATIVE theory!!" << std::endl;
      continue;
    }

    if (data > 1.0e-9) {
      mllh += (theo-data) + data*log(data/theo);
    } else {
      mllh += (theo-data);
    }
  }

  return normalizer*2.0*mllh;
}

//--------------------------------------------------------------------------
// CalcMaxLikelihoodExpected (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate expected log maximum-likelihood.
 *
 * <b>return:</b>
 * - log maximum-likelihood value
 *
 * \param par parameter vector iterated by minuit2
 */
Double_t PRunSingleHisto::CalcMaxLikelihoodExpected(const std::vector<Double_t>& par)
{
  Double_t mllh = 0.0; // maximum log likelihood assuming poisson distribution for the single bin

  Double_t N0;

  // check if norm is a parameter or a function
  if (fRunInfo->GetNormParamNo() < MSR_PARAM_FUN_OFFSET) { // norm is a parameter
    N0 = par[fRunInfo->GetNormParamNo()-1];
  } else { // norm is a function
    // get function number
    Int_t funNo = fRunInfo->GetNormParamNo()-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
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
    UInt_t funcNo = fMsrInfo->GetFuncNo(i);
    fFuncValues[i] = fMsrInfo->EvalFunc(funcNo, *fRunInfo->GetMap(), par, fMetaData);
  }

  // calculate maximum log likelihood
  Double_t theo;
  Double_t data;
  Double_t time(1.0);
  Int_t i;

  // norm is needed since there is no simple scaling like in chisq case to get the correct Max.Log.Likelihood value when normlizing N(t) to 1/ns
  Double_t normalizer = 1.0;

  if (fScaleN0AndBkg)
    normalizer = fPacking * (fTimeResolution * 1.0e3);

  // Calculate the theory function once to ensure one function evaluation for the current set of parameters.
  // This is needed for the LF and user functions where some non-thread-save calculations only need to be calculated once
  // for a given set of parameters---which should be done outside of the parallelized loop.
  // For all other functions it means a tiny and acceptable overhead.
  time = fTheory->Func(time, par, fFuncValues);

  #ifdef HAVE_GOMP
  Int_t chunk = (fEndTimeBin - fStartTimeBin)/omp_get_num_procs();
  if (chunk < 10)
    chunk = 10;
  #pragma omp parallel for default(shared) private(i,time,theo,data) schedule(dynamic,chunk) reduction(-:mllh)
  #endif
  for (i=fStartTimeBin; i<fEndTimeBin; ++i) {
    time = fData.GetDataTimeStart() + static_cast<Double_t>(i)*fData.GetDataTimeStep();
    // calculate theory for the given parameter set
    theo = N0*TMath::Exp(-time/tau)*(1.0+fTheory->Func(time, par, fFuncValues))+bkg;

    data = fData.GetValue()->at(i);

    if (theo <= 0.0) {
      std::cerr << ">> PRunSingleHisto::CalcMaxLikelihood: **WARNING** NEGATIVE theory!!" << std::endl;
      continue;
    }

    if (data > 1.0e-9) { // is this correct?? needs to be checked. See G-test
      mllh += data*log(data/theo);
    }
  }

  return normalizer*2.0*mllh;
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
    Int_t funNo = fRunInfo->GetNormParamNo()-MSR_PARAM_FUN_OFFSET;
    // evaluate function
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
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
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par, fMetaData);
  }

  // calculate theory
  UInt_t size = fData.GetValue()->size();
  Double_t start = fData.GetDataTimeStart();
  Double_t resolution = fData.GetDataTimeStep();
  Double_t time;
  for (UInt_t i=0; i<size; i++) {
    time = start + static_cast<Double_t>(i)*resolution;
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
void PRunSingleHisto::SetFitRangeBin(const TString fitRange)
{
  TObjArray *tok = nullptr;
  TObjString *ostr = nullptr;
  TString str;
  Ssiz_t idx = -1;
  Int_t offset = 0;

  tok = fitRange.Tokenize(" \t");

  if (tok->GetEntries() == 3) { // structure FIT_RANGE fgb+n0 lgb-n1
    // handle fgb+n0 entry
    ostr = dynamic_cast<TObjString*>(tok->At(1));
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
    ostr = dynamic_cast<TObjString*>(tok->At(2));
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
      ostr = dynamic_cast<TObjString*>(tok->At(pos));
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
      ostr = dynamic_cast<TObjString*>(tok->At(pos+1));
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
void PRunSingleHisto::CalcNoOfFitBins()
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
// PrepareData (protected)
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

  // keep the Global block info
  PMsrGlobalBlock *globalBlock = fMsrInfo->GetMsrGlobal();

  // get the proper run
  PRawRunData* runData = fRawData->GetRunData(*fRunInfo->GetRunName());
  if (!runData) { // couldn't get run
    std::cerr << std::endl << ">> PRunSingleHisto::PrepareData(): **ERROR** Couldn't get run " << fRunInfo->GetRunName()->Data() << "!";
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
  PUIntVector histoNo; // histoNo = msr-file forward + redGreen_offset - 1
  for (UInt_t i=0; i<fRunInfo->GetForwardHistoNoSize(); i++) {
    histoNo.push_back(fRunInfo->GetForwardHistoNo(i));

    if (!runData->IsPresent(histoNo[i])) {
      std::cerr << std::endl << ">> PRunSingleHisto::PrepareData(): **PANIC ERROR**:";
      std::cerr << std::endl << ">> histoNo found = " << histoNo[i] << ", which is NOT present in the data file!?!?";
      std::cerr << std::endl << ">> Will quit :-(";
      std::cerr << std::endl;
      histoNo.clear();
      return false;
    }
  }

  // keep the time resolution in (us)
  fTimeResolution = runData->GetTimeResolution()/1.0e3;
  std::cout.precision(10);
  std::cout << std::endl << ">> PRunSingleHisto::PrepareData(): time resolution=" << std::fixed << runData->GetTimeResolution() << "(ns)" << std::endl;

  // get all the proper t0's and addt0's for the current RUN block
  if (!GetProperT0(runData, globalBlock, histoNo)) {
    return false;
  }

  // keep the histo of each group at this point (addruns handled below)
  std::vector<PDoubleVector> forward;
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
      if (addRunData == nullptr) { // couldn't get run
        std::cerr << std::endl << ">> PRunSingleHisto::PrepareData(): **ERROR** Couldn't get addrun " << fRunInfo->GetRunName(i)->Data() << "!";
        std::cerr << std::endl;
        return false;
      }

      // add forward run
      UInt_t addRunSize;
      for (UInt_t k=0; k<histoNo.size(); k++) { // fill each group
        addRunSize = addRunData->GetDataBin(histoNo[k])->size();
        for (UInt_t j=0; j<addRunData->GetDataBin(histoNo[k])->size(); j++) { // loop over the bin indices
          // make sure that the index stays in the proper range
          if ((static_cast<Int_t>(j)+static_cast<Int_t>(fAddT0s[i-1][k])-static_cast<Int_t>(fT0s[k]) >= 0) &&
              (j+static_cast<Int_t>(fAddT0s[i-1][k])-static_cast<Int_t>(fT0s[k]) < addRunSize)) {
            forward[k][j] += addRunData->GetDataBin(histoNo[k])->at(j+static_cast<Int_t>(fAddT0s[i-1][k])-static_cast<Int_t>(fT0s[k]));
          }
        }
      }
    }
  }

  // set forward histo data of the first group
  fForward.resize(forward[0].size());
  for (UInt_t i=0; i<fForward.size(); i++) {
    fForward[i] = forward[0][i];
  }

  // group histograms, add all the remaining forward histograms of the group
  for (UInt_t i=1; i<histoNo.size(); i++) { // loop over the groupings
    for (UInt_t j=0; j<runData->GetDataBin(histoNo[i])->size(); j++) { // loop over the bin indices
      // make sure that the index stays within proper range
      if ((static_cast<Int_t>(j)+fT0s[i]-fT0s[0] >= 0) && (j+fT0s[i]-fT0s[0] < runData->GetDataBin(histoNo[i])->size())) {
        fForward[j] += forward[i][j+static_cast<Int_t>(fT0s[i])-static_cast<Int_t>(fT0s[0])];
      }
    }
  }

  // get the data range (fgb/lgb) for the current RUN block
  if (!GetProperDataRange()) {
    return false;
  }

  // get the fit range for the current RUN block
  GetProperFitRange(globalBlock);

  // get the lifetimecorrection flag
  Bool_t lifetimecorrection = false;
  PMsrPlotList *plot = fMsrInfo->GetMsrPlotList();
  lifetimecorrection = plot->at(0).fLifeTimeCorrection;

  // do the more fit/view specific stuff
  if (fHandleTag == kFit)
    success = PrepareFitData(runData, histoNo[0]);
  else if ((fHandleTag == kView) && !lifetimecorrection)
    success = PrepareRawViewData(runData, histoNo[0]);
  else if ((fHandleTag == kView) && lifetimecorrection)
    success = PrepareViewData(runData, histoNo[0]);
  else
    success = false;

  // cleanup
  histoNo.clear();

  return success;
}

//--------------------------------------------------------------------------
// PrepareFitData (protected)
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
  if (fMsrInfo->EstimateN0()) {
    EstimateN0();
  }

  // transform raw histo data. This is done the following way (for details see the manual):
  // for the single histo fit, just the rebinned raw data are copied

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
        std::cerr << std::endl << ">> PRunSingleHisto::PrepareFitData(): **WARNING** Neither fix background nor background bins are given!";
        std::cerr << std::endl << ">> Will try the following: bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
        std::cerr << std::endl << ">> NO WARRANTY THAT THIS MAKES ANY SENSE! Better check ...";
        std::cerr << std::endl;
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
  Int_t t0 = static_cast<Int_t>(fT0s[0]);
  Double_t value = 0.0;
  Double_t normalizer = 1.0;
  // in order that after rebinning the fit does not need to be redone (important for plots)
  // the value is normalize to per 1 nsec if scaling is whished
  if (fScaleN0AndBkg)
    normalizer = fPacking * (fTimeResolution * 1.0e3); // fTimeResolution us->ns
  // data start at data_start-t0
  // time shifted so that packing is included correctly, i.e. t0 == t0 after packing
  fData.SetDataTimeStart(fTimeResolution*(static_cast<Double_t>(fGoodBins[0])-static_cast<Double_t>(t0)+static_cast<Double_t>(fPacking-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*fPacking);
  for (Int_t i=fGoodBins[0]; i<fGoodBins[1]; i++) {
    if (fPacking == 1) {
      value = fForward[i];
      value /= normalizer;
      fData.AppendValue(value);
      if (value == 0.0)
        fData.AppendErrorValue(1.0/normalizer);
      else
        fData.AppendErrorValue(TMath::Sqrt(value));
    } else { // packed data, i.e. fPacking > 1
      if (((i-fGoodBins[0]) % fPacking == 0) && (i != fGoodBins[0])) { // fill data
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
// PrepareRawViewData (protected)
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
  Int_t packing = fPacking;
  if (fMsrInfo->GetMsrPlotList()->at(0).fViewPacking > 0) {
    packing = fMsrInfo->GetMsrPlotList()->at(0).fViewPacking;
  }

  // calculate necessary norms
  Double_t dataNorm = 1.0, theoryNorm = 1.0;
  if (fScaleN0AndBkg) {
    dataNorm = 1.0/ (packing * (fTimeResolution * 1.0e3)); // fTimeResolution us->ns
  } else if (!fScaleN0AndBkg && (fMsrInfo->GetMsrPlotList()->at(0).fViewPacking > 0)) {
    theoryNorm = static_cast<Double_t>(fMsrInfo->GetMsrPlotList()->at(0).fViewPacking)/static_cast<Double_t>(fPacking);
  }

  // raw data, since PMusrCanvas is doing ranging etc.
  // start = the first bin which is a multiple of packing backward from first good data bin
  Int_t start = fGoodBins[0] - (fGoodBins[0]/packing)*packing;
  // end = last bin starting from start which is a multiple of packing and still within the data
  Int_t end   = start + ((fForward.size()-start)/packing)*packing;
  // check if data range has been provided, and if not try to estimate them
  if (start < 0) {
    Int_t offset = static_cast<Int_t>(10.0e-3/fTimeResolution);
    start = (static_cast<Int_t>(fT0s[0])+offset) - ((static_cast<Int_t>(fT0s[0])+offset)/packing)*packing;
    end = start + ((fForward.size()-start)/packing)*packing;
    std::cerr << std::endl << ">> PRunSingleHisto::PrepareRawViewData(): **WARNING** data range was not provided, will try data range start = " << start << ".";
    std::cerr << std::endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    std::cerr << std::endl;
  }
  // check if start, end, and t0 make any sense
  // 1st check if start and end are in proper order
  if (end < start) { // need to swap them
    Int_t keep = end;
    end = start;
    start = keep;
  }
  // 2nd check if start is within proper bounds
  if ((start < 0) || (start > static_cast<Int_t>(fForward.size()))) {
    std::cerr << std::endl << ">> PRunSingleHisto::PrepareRawViewData(): **ERROR** start data bin doesn't make any sense!";
    std::cerr << std::endl;
    return false;
  }
  // 3rd check if end is within proper bounds
  if ((end < 0) || (end > static_cast<Int_t>(fForward.size()))) {
    std::cerr << std::endl << ">> PRunSingleHisto::PrepareRawViewData(): **ERROR** end data bin doesn't make any sense!";
    std::cerr << std::endl;
    return false;
  }

  // everything looks fine, hence fill data set
  Int_t t0 = static_cast<Int_t>(fT0s[0]);
  Double_t value = 0.0;
  // data start at data_start-t0
  // time shifted so that packing is included correctly, i.e. t0 == t0 after packing
  fData.SetDataTimeStart(fTimeResolution*(static_cast<Double_t>(start)-static_cast<Double_t>(t0)+static_cast<Double_t>(packing-1)/2.0));
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
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
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
        std::cerr << std::endl << ">> PRunSingleHisto::PrepareRawViewData(): **WARNING** Neither fix background nor background bins are given!";
        std::cerr << std::endl << ">> Will try the following: bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
        std::cerr << std::endl << ">> NO WARRANTY THAT THIS MAKES ANY SENSE! Better check ...";
        std::cerr << std::endl;
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
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par, fMetaData);
  }

  // calculate theory
  UInt_t size = fForward.size();
  Int_t factor = 8; // 8 times more points for the theory (if fTheoAsData == false)
  fData.SetTheoryTimeStart(fData.GetDataTimeStart());
  if (fTheoAsData) { // cacluate theory only at the data points
    fData.SetTheoryTimeStep(fData.GetDataTimeStep());
  } else {
    // finer binning for the theory (8 times as many points = factor)
    size *= factor;
    fData.SetTheoryTimeStep(fData.GetDataTimeStep()/(Double_t)factor);
  }

  Double_t time;
  Double_t theoryValue;
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
// PrepareViewData (protected)
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
  Int_t packing = fPacking;
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
    theoryNorm = static_cast<Double_t>(fMsrInfo->GetMsrPlotList()->at(0).fViewPacking)/static_cast<Double_t>(fPacking);
  }

  // transform raw histo data. This is done the following way (for details see the manual):
  // for the single histo fit, just the rebinned raw data are copied
  // first get start data, end data, and t0
  Int_t t0 = static_cast<Int_t>(fT0s[0]);

  // start = the first bin which is a multiple of packing backward from first good data bin
  Int_t start = fGoodBins[0] - (fGoodBins[0]/packing)*packing;
  // end = last bin starting from start which is a multiple of packing and still within the data
  Int_t end   = start + ((fForward.size()-start)/packing)*packing;

  // check if data range has been provided, and if not try to estimate them
  if (start < 0) {
    Int_t offset = static_cast<Int_t>(10.0e-3/fTimeResolution);
    start = (static_cast<Int_t>(fT0s[0])+offset) - ((static_cast<Int_t>(fT0s[0])+offset)/packing)*packing;
    end = start + ((fForward.size()-start)/packing)*packing;
    std::cerr << std::endl << ">> PRunSingleHisto::PrepareViewData(): **WARNING** data range was not provided, will try data range start = " << start << ".";
    std::cerr << std::endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    std::cerr << std::endl;
  }

  // check if start, end, and t0 make any sense
  // 1st check if start and end are in proper order
  if (end < start) { // need to swap them
    Int_t keep = end;
    end = start;
    start = keep;
  }
  // 2nd check if start is within proper bounds
  if ((start < 0) || (start > static_cast<Int_t>(fForward.size()))) {
    std::cerr << std::endl << ">> PRunSingleHisto::PrepareViewData(): **ERROR** start data bin doesn't make any sense!";
    std::cerr << std::endl;
    return false;
  }
  // 3rd check if end is within proper bounds
  if ((end < 0) || (end > static_cast<Int_t>(fForward.size()))) {
    std::cerr << std::endl << ">> PRunSingleHisto::PrepareViewData(): **ERROR** end data bin doesn't make any sense!";
    std::cerr << std::endl;
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
    N0 = fMsrInfo->EvalFunc(funNo, *fRunInfo->GetMap(), par, fMetaData);
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
        std::cerr << std::endl << ">> PRunSingleHisto::PrepareViewData(): **WARNING** Neither fix background nor background bins are given!";
        std::cerr << std::endl << ">> Will try the following: bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
        std::cerr << std::endl << ">> NO WARRANTY THAT THIS MAKES ANY SENSE! Better check ...";
        std::cerr << std::endl;
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
  fData.SetDataTimeStart(fTimeResolution*(static_cast<Double_t>(start)-static_cast<Double_t>(t0)+static_cast<Double_t>(packing-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*packing);

  // data is always normalized to (per nsec!!)
  Double_t gammaRRF = 0.0, wRRF = 0.0, phaseRRF = 0.0;
  if (fMsrInfo->GetMsrPlotList()->at(0).fRRFFreq == 0.0) { // normal Data representation
    for (Int_t i=start; i<end; i++) {
      if (((i-start) % packing == 0) && (i != start)) { // fill data
        value *= dataNorm;
        time = ((static_cast<Double_t>(i)-static_cast<Double_t>(packing-1)/2.0)-t0)*fTimeResolution;
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
        gammaRRF = GAMMA_BAR_MUON*TMath::TwoPi();
        break;
      case RRF_UNIT_T:
        gammaRRF = GAMMA_BAR_MUON*TMath::TwoPi()*1.0e4;
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
      time = (static_cast<Double_t>(i)-t0)*fTimeResolution;
      expval = TMath::Exp(+time/tau)/N0;
      rrf_val = (-1.0+expval*(fForward[i]/(fTimeResolution*1.0e3)-bkg))*TMath::Cos(wRRF * time + phaseRRF);
      value += rrf_val;
      error += fForward[i]*dataNorm;
    }
  }

  CalcNoOfFitBins();

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par, fMetaData);
  }

  // calculate theory
  Double_t theoryValue;
  UInt_t size = fForward.size()/packing;
  Int_t factor = 8; // 8 times more points for the theory (if fTheoAsData == false)
  UInt_t rebinRRF = 0;

  if (wRRF == 0) { // no RRF
    fData.SetTheoryTimeStart(fData.GetDataTimeStart());
    if (fTheoAsData) { // cacluate theory only at the data points
      fData.SetTheoryTimeStep(fData.GetDataTimeStep());
    } else {
      // finer binning for the theory (8 times as many points = factor)
      size *= factor;
      fData.SetTheoryTimeStep(fData.GetDataTimeStep()/(Double_t)factor);
    }
  } else { // RRF
    rebinRRF = static_cast<UInt_t>((TMath::Pi()/2.0/wRRF)/fTimeResolution); // RRF time resolution / data time resolution
    fData.SetTheoryTimeStart(fData.GetDataTimeStart());
    fData.SetTheoryTimeStep(TMath::Pi()/2.0/wRRF/rebinRRF); // = theory time resolution as close as possible to the data time resolution compatible with wRRF
  }

  for (UInt_t i=0; i<size; i++) {
    time = fData.GetTheoryTimeStart() + static_cast<Double_t>(i)*fData.GetTheoryTimeStep();
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
 * \param histoNo histogram number vector of forward; histoNo = msr-file forward + redGreen_offset - 1
 *
 * <b>return:</b>
 * - true if everthing went smooth
 * - false, otherwise.
 */
Bool_t PRunSingleHisto::GetProperT0(PRawRunData* runData, PMsrGlobalBlock *globalBlock, PUIntVector &histoNo)
{
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

  // fill in the T0's from the GLOBAL block section (if present)
  for (UInt_t i=0; i<globalBlock->GetT0BinSize(); i++) {
    if (fT0s[i] == -1.0) { // i.e. not given in the RUN block section
      fT0s[i] = globalBlock->GetT0Bin(i);
    }
  }

  // fill in the T0's from the data file, if not already present in the msr-file
  for (UInt_t i=0; i<histoNo.size(); i++) {
    if (fT0s[i] == -1.0) { // i.e. not present in the msr-file, try the data file
      if (runData->GetT0Bin(histoNo[i]) > 0.0) {
        fT0s[i] = runData->GetT0Bin(histoNo[i]);
        fRunInfo->SetT0Bin(fT0s[i], i); // keep value for the msr-file
      }
    }
  }

  // fill in the T0's gaps, i.e. in case the T0's are NOT in the msr-file and NOT in the data file
  for (UInt_t i=0; i<histoNo.size(); i++) {
    if (fT0s[i] == -1.0) { // i.e. not present in the msr-file and data file, use the estimated T0
      fT0s[i] = runData->GetT0BinEstimated(histoNo[i]);
      fRunInfo->SetT0Bin(fT0s[i], i); // keep value for the msr-file

      std::cerr << std::endl << ">> PRunSingleHisto::GetProperT0(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
      std::cerr << std::endl << ">> run: " << fRunInfo->GetRunName()->Data();
      std::cerr << std::endl << ">> will try the estimated one: forward t0 = " << runData->GetT0BinEstimated(histoNo[i]);
      std::cerr << std::endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
      std::cerr << std::endl;
    }
  }

  // check if t0 is within proper bounds
  for (UInt_t i=0; i<fRunInfo->GetForwardHistoNoSize(); i++) {
    if ((fT0s[i] < 0.0) || (fT0s[i] > static_cast<Int_t>(runData->GetDataBin(histoNo[i])->size()))) {
      std::cerr << std::endl << ">> PRunSingleHisto::GetProperT0(): **ERROR** t0 data bin (" << fT0s[i] << ") doesn't make any sense!";
      std::cerr << std::endl;
      return false;
    }
  }

  // check if there are runs to be added to the current one. If yes keep the needed t0's
  if (fRunInfo->GetRunNameSize() > 1) { // runs to be added present
    PRawRunData *addRunData;
    fAddT0s.resize(fRunInfo->GetRunNameSize()-1); // resize to the number of addruns
    for (UInt_t i=1; i<fRunInfo->GetRunNameSize(); i++) {

      // get run to be added to the main one
      addRunData = fRawData->GetRunData(*fRunInfo->GetRunName(i));
      if (addRunData == nullptr) { // couldn't get run
        std::cerr << std::endl << ">> PRunSingleHisto::GetProperT0(): **ERROR** Couldn't get addrun " << fRunInfo->GetRunName(i)->Data() << "!";
        std::cerr << std::endl;
        return false;
      }

      // feed all T0's
      // first init T0's, T0's are stored as (forward T0, backward T0, etc.)
      fAddT0s[i-1].resize(histoNo.size());
      for (UInt_t j=0; j<fAddT0s[i-1].size(); j++) {
        fAddT0s[i-1][j] = -1.0;
      }

      // fill in the T0's from the msr-file (if present)
      for (UInt_t j=0; j<fRunInfo->GetT0BinSize(); j++) {
        fAddT0s[i-1][j] = fRunInfo->GetAddT0Bin(i-1,j); // addRunIdx starts at 0
      }

      // fill in the T0's from the data file, if not already present in the msr-file
      for (UInt_t j=0; j<histoNo.size(); j++) {
        if (fAddT0s[i-1][j] == -1.0) // i.e. not present in the msr-file, try the data file
          if (addRunData->GetT0Bin(histoNo[j]) > 0.0) {
            fAddT0s[i-1][j] = addRunData->GetT0Bin(histoNo[j]);
            fRunInfo->SetAddT0Bin(fAddT0s[i-1][j], i-1, j); // keep value for the msr-file
          }
      }

      // fill in the T0's gaps, i.e. in case the T0's are NOT in the msr-file and NOT in the data file
      for (UInt_t j=0; j<histoNo.size(); j++) {
        if (fAddT0s[i-1][j] == -1.0) { // i.e. not present in the msr-file and data file, use the estimated T0
          fAddT0s[i-1][j] = addRunData->GetT0BinEstimated(histoNo[j]);
          fRunInfo->SetAddT0Bin(fAddT0s[i-1][j], i-1, j); // keep value for the msr-file

          std::cerr << std::endl << ">> PRunSingleHisto::GetProperT0(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
          std::cerr << std::endl << ">> run: " << fRunInfo->GetRunName(i)->Data();
          std::cerr << std::endl << ">> will try the estimated one: forward t0 = " << addRunData->GetT0BinEstimated(histoNo[j]);
          std::cerr << std::endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
          std::cerr << std::endl;
        }
      }

      // check if t0 is within proper bounds
      for (UInt_t j=0; j<fRunInfo->GetForwardHistoNoSize(); j++) {
        if ((fAddT0s[i-1][j] < 0.0) || (fAddT0s[i-1][j] > static_cast<Int_t>(addRunData->GetDataBin(histoNo[j])->size()))) {
          std::cerr << std::endl << ">> PRunSingleHisto::GetProperT0(): **ERROR** addt0 data bin (" << fAddT0s[i-1][j] << ") doesn't make any sense!";
          std::cerr << std::endl;
          return false;
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
 * <b>return:</b>
 * - true if everthing went smooth
 * - false, otherwise.
 */
Bool_t PRunSingleHisto::GetProperDataRange()
{
  // get start/end data
  Int_t start;
  Int_t end;
  start = fRunInfo->GetDataRange(0);
  end   = fRunInfo->GetDataRange(1);

  // check if data range has been given in the RUN block, if not try to get it from the GLOBAL block
  if (start < 0) {
    start = fMsrInfo->GetMsrGlobal()->GetDataRange(0);
  }
  if (end < 0) {
    end = fMsrInfo->GetMsrGlobal()->GetDataRange(1);
  }

  // check if data range has been provided, and if not try to estimate them
  if (start < 0) {
    Int_t offset = static_cast<Int_t>(10.0e-3/fTimeResolution);
    start = static_cast<Int_t>(fT0s[0])+offset;
    fRunInfo->SetDataRange(start, 0);
    std::cerr << std::endl << ">> PRunSingleHisto::GetProperDataRange(): **WARNING** data range was not provided, will try data range start = t0+" << offset << "(=10ns) = " << start << ".";
    std::cerr << std::endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    std::cerr << std::endl;
  }
  if (end < 0) {
    end = fForward.size();
    fRunInfo->SetDataRange(end, 1);
    std::cerr << std::endl << ">> PRunSingleHisto::GetProperDataRange(): **WARNING** data range was not provided, will try data range end = " << end << ".";
    std::cerr << std::endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    std::cerr << std::endl;
  }

  // check if start and end make any sense
  // 1st check if start and end are in proper order
  if (end < start) { // need to swap them
    Int_t keep = end;
    end = start;
    start = keep;
  }
  // 2nd check if start is within proper bounds
  if ((start < 0) || (start > static_cast<Int_t>(fForward.size()))) {
    std::cerr << std::endl << ">> PRunSingleHisto::GetProperDataRange(): **ERROR** start data bin (" << start << ") doesn't make any sense!";
    std::cerr << std::endl;
    return false;
  }
  // 3rd check if end is within proper bounds
  if (end < 0) {
    std::cerr << std::endl << ">> PRunSingleHisto::GetProperDataRange(): **ERROR** end data bin (" << end << ") doesn't make any sense!";
    std::cerr << std::endl;
    return false;
  }
  if (end > static_cast<Int_t>(fForward.size())) {
    std::cerr << std::endl << ">> PRunSingleHisto::GetProperDataRange(): **WARNING** end data bin (" << end << ") > histo length (" << fForward.size() << ").";
    std::cerr << std::endl << ">>    Will set end = (histo length - 1). Consider to change it in the msr-file." << std::endl;
    std::cerr << std::endl;
    end = static_cast<Int_t>(fForward.size())-1;
  }

  // keep good bins for potential later use
  fGoodBins[0] = start;
  fGoodBins[1] = end;

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
void PRunSingleHisto::GetProperFitRange(PMsrGlobalBlock *globalBlock)
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

//--------------------------------------------------------------------------
// EstimateN0 (private)
//--------------------------------------------------------------------------
/**
 * <p>Estimate the N0 for the given run.
 */
void PRunSingleHisto::EstimateN0()
{
  // check that 'norm' in the msr-file run block is indeed a parameter number.
  // in case it is a function, nothing will be done.
  UInt_t paramNo = fRunInfo->GetNormParamNo();
  if (paramNo > 10000) // i.e. fun or map
    return;

  // get the parameters
  PMsrParamList *param = fMsrInfo->GetMsrParamList();
  assert(param);

  if (paramNo > param->size()) {
    std::cerr << std::endl << ">> PRunSingleHisto::EstimateN0: **ERROR** found parameter number " << paramNo << ", which is larger than the number of parameters = " << param->size() << std::endl;
    return;
  }

  // check if N0 is fixed. If this is the case, do NOT estimate N0
  if (param->at(paramNo-1).fStep == 0.0) // N0 parameter fixed
    return;


  // check that 'backgr.fit' in the msr-file run block is indeed a parameter number.
  // in case it is a function, nothing will be done.
  Int_t paramNoBkg = fRunInfo->GetBkgFitParamNo();
  Bool_t scaleBkg = true;
  Double_t bkg=0.0, errBkg=1.0;
  if ((paramNoBkg > 10000) || (paramNoBkg == -1)) { // i.e. fun or map
    scaleBkg = false;
  } else {
    if (paramNoBkg-1 < static_cast<Int_t>(param->size())) {
      bkg = param->at(paramNoBkg-1).fValue;
      errBkg = param->at(paramNoBkg-1).fStep;
    }
  }

  // estimate N0
  Double_t dt = fTimeResolution;
  Double_t tau = PMUON_LIFETIME;

  UInt_t t0 = static_cast<UInt_t>(round(fT0s[0]));
  Double_t dval = 0.0;
  Double_t nom = 0.0;
  Double_t denom = 0.0;
  Double_t xx = 0.0;

  // calc nominator
  for (UInt_t i=t0; i<fForward.size(); i++) {
    xx = exp(-dt*static_cast<Double_t>(i-t0)/tau);
    nom += xx;
  }

  // calc denominator
  for (UInt_t i=t0; i<fForward.size(); i++) {
    xx = exp(-dt*static_cast<Double_t>(i-t0)/tau);
    dval = fForward[i];
    if (dval > 0)
      denom += xx*xx/dval;
  }
  Double_t N0 = nom/denom;  

  if (fScaleN0AndBkg) {
    N0 /= fTimeResolution*1.0e3;
  } else {
    N0 *= fPacking;
  }

  Double_t rescale = 1;
  if ((param->at(paramNo-1).fValue != 0.0) && scaleBkg) {
    rescale = N0 / param->at(paramNo-1).fValue;
    bkg *= rescale;
    errBkg *= rescale;
  }

  std::cout << ">> PRunSingleHisto::EstimateN0: found N0=" << param->at(paramNo-1).fValue << ", will set it to N0=" << N0 << std::endl;
  if (scaleBkg)
    std::cout << ">> PRunSingleHisto::EstimateN0: found Bkg=" << param->at(paramNoBkg-1).fValue << ", will set it to Bkg=" << bkg << std::endl;
  fMsrInfo->SetMsrParamValue(paramNo-1, N0);
  fMsrInfo->SetMsrParamStep(paramNo-1, sqrt(fabs(N0)));
  if (scaleBkg) {
    fMsrInfo->SetMsrParamValue(paramNoBkg-1, bkg);
    fMsrInfo->SetMsrParamStep(paramNoBkg-1, errBkg);
  }
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
  Int_t start = fRunInfo->GetBkgRange(0);
  Int_t end   = fRunInfo->GetBkgRange(1);
  if (end < start) {
    std::cout << std::endl << "PRunSingleHisto::EstimatBkg(): end = " << end << " > start = " << start << "! Will swap them!";
    Int_t keep = end;
    end = start;
    start = keep;
  }

  // calculate proper background range
  if (beamPeriod != 0.0) {
    Double_t timeBkg = static_cast<Double_t>(end-start)*(fTimeResolution*fPacking); // length of the background intervall in time
    UInt_t fullCycles = static_cast<UInt_t>(timeBkg/beamPeriod); // how many proton beam cylces can be placed within the proposed background intervall
    // correct the end of the background intervall such that the background is as close as possible to a multiple of the proton cylce
    end = start + static_cast<UInt_t>((fullCycles*beamPeriod)/(fTimeResolution*fPacking));
    std::cout << std::endl << "PRunSingleHisto::EstimatBkg(): Background " << start << ", " << end;
    if (end == start)
      end = fRunInfo->GetBkgRange(1);
  }

  // check if start is within histogram bounds
  if (start >= fForward.size()) {
    std::cerr << std::endl << ">> PRunSingleHisto::EstimatBkg(): **ERROR** background bin values out of bound!";
    std::cerr << std::endl << ">> histo lengths    = " << fForward.size();
    std::cerr << std::endl << ">> background start = " << start;
    std::cerr << std::endl;
    return false;
  }

  // check if end is within histogram bounds
  if (end >= fForward.size()) {
    std::cerr << std::endl << ">> PRunSingleHisto::EstimatBkg(): **ERROR** background bin values out of bound!";
    std::cerr << std::endl << ">> histo lengths  = " << fForward.size();
    std::cerr << std::endl << ">> background end = " << end;
    std::cerr << std::endl;
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
    fBackground = bkg * fPacking;  // keep background (per bin)

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
      TObjArray *tokens = nullptr;
      TObjString *ostr = nullptr;
      TString str;
      tokens = cmd->at(i).fLine.Tokenize(" \t");
      if (tokens->GetEntries() != 2) {
        std::cerr << std::endl << ">> PRunSingleHisto::IsScaleN0AndBkg(): **WARNING** Found uncorrect 'SCALE_N0_BKG' command, will ignore it.";
        std::cerr << std::endl << ">> Allowed commands: SCALE_N0_BKG TRUE | FALSE" << std::endl;
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
