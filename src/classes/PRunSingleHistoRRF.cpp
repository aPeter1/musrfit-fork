/***************************************************************************

  PRunSingleHistoRRF.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2019 by Andreas Suter                              *
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
#include <TH1F.h>

#include "PMusr.h"
#include "PFourier.h"
#include "PRunSingleHistoRRF.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 */
PRunSingleHistoRRF::PRunSingleHistoRRF() : PRunBase()
{
  fNoOfFitBins  = 0;
  fBackground = 0.0;
  fBkgErr = 1.0;
  fRRFPacking = -1;

  // the 2 following variables are need in case fit range is given in bins, and since
  // the fit range can be changed in the command block, these variables need to be accessible
  fGoodBins[0] = -1;
  fGoodBins[1] = -1;
  
  fN0EstimateEndTime = 1.0; // end time in (us) over which N0 is estimated.
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
PRunSingleHistoRRF::PRunSingleHistoRRF(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag) : PRunBase(msrInfo, rawData, runNo, tag)
{
  fNoOfFitBins  = 0;

  PMsrGlobalBlock *global = msrInfo->GetMsrGlobal();

  if (!global->IsPresent()) {
    std::cerr << std::endl << ">> PRunSingleHistoRRF::PRunSingleHistoRRF(): **SEVERE ERROR**: no GLOBAL-block present!";
    std::cerr << std::endl << ">> For Single Histo RRF the GLOBAL-block is mandatory! Please fix this first.";
    std::cerr << std::endl;
    fValid = false;
    return;
  }

  if (!global->GetRRFUnit().CompareTo("??")) {
    std::cerr << std::endl << ">> PRunSingleHistoRRF::PRunSingleHistoRRF(): **SEVERE ERROR**: no RRF-Frequency found!";
    std::cerr << std::endl;
    fValid = false;
    return;
  }

  fRRFPacking = global->GetRRFPacking();
  if (fRRFPacking == -1) {
    std::cerr << std::endl << ">> PRunSingleHistoRRF::PRunSingleHistoRRF(): **SEVERE ERROR**: no RRF-Packing found!";
    std::cerr << std::endl;
    fValid = false;
    return;
  }

  // the 2 following variables are need in case fit range is given in bins, and since
  // the fit range can be changed in the command block, these variables need to be accessible
  fGoodBins[0] = -1;
  fGoodBins[1] = -1;

  fN0EstimateEndTime = 1.0; // end time in (us) over which N0 is estimated.
 
  if (!PrepareData()) {
    std::cerr << std::endl << ">> PRunSingleHistoRRF::PRunSingleHistoRRF(): **SEVERE ERROR**: Couldn't prepare data for fitting!";
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
PRunSingleHistoRRF::~PRunSingleHistoRRF()
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
Double_t PRunSingleHistoRRF::CalcChiSquare(const std::vector<Double_t>& par)
{
  Double_t chisq     = 0.0;
  Double_t diff      = 0.0;

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    UInt_t funcNo = fMsrInfo->GetFuncNo(i);
    fFuncValues[i] = fMsrInfo->EvalFunc(funcNo, *fRunInfo->GetMap(), par);
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
    diff = fData.GetValue()->at(i) - fTheory->Func(time, par, fFuncValues);
    chisq += diff*diff / (fData.GetError()->at(i)*fData.GetError()->at(i));
  }

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
Double_t PRunSingleHistoRRF::CalcChiSquareExpected(const std::vector<Double_t>& par)
{
  Double_t chisq = 0.0;
  Double_t diff  = 0.0;
  Double_t theo  = 0.0;

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    UInt_t funcNo = fMsrInfo->GetFuncNo(i);
    fFuncValues[i] = fMsrInfo->EvalFunc(funcNo, *fRunInfo->GetMap(), par);
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
  for (i=fStartTimeBin; i < fEndTimeBin; ++i) {
    time = fData.GetDataTimeStart() + static_cast<Double_t>(i)*fData.GetDataTimeStep();
    theo = fTheory->Func(time, par, fFuncValues);
    diff = fData.GetValue()->at(i) - theo;
    chisq += diff*diff / theo;
  }

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
Double_t PRunSingleHistoRRF::CalcMaxLikelihood(const std::vector<Double_t>& par)
{
  // not yet implemented

  return 0.0;
}

//--------------------------------------------------------------------------
// CalcTheory (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate theory for a given set of fit-parameters.
 */
void PRunSingleHistoRRF::CalcTheory()
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

  // calculate theory
  UInt_t size = fData.GetValue()->size();
  Double_t start = fData.GetDataTimeStart();
  Double_t resolution = fData.GetDataTimeStep();
  Double_t time;
  for (UInt_t i=0; i<size; i++) {
    time = start + static_cast<Double_t>(i)*resolution;
    fData.AppendTheoryValue(fTheory->Func(time, par, fFuncValues));
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
UInt_t PRunSingleHistoRRF::GetNoOfFitBins()
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
void PRunSingleHistoRRF::SetFitRangeBin(const TString fitRange)
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
      std::cerr << std::endl << ">> PRunSingleHistoRRF::SetFitRangeBin(): **ERROR** invalid FIT_RANGE command found: '" << fitRange << "'";
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
    std::cerr << std::endl << ">> PRunSingleHistoRRF::SetFitRangeBin(): **ERROR** invalid FIT_RANGE command found: '" << fitRange << "'";
    std::cerr << std::endl << ">> will ignore it. Sorry ..." << std::endl;
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
void PRunSingleHistoRRF::CalcNoOfFitBins()
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
Bool_t PRunSingleHistoRRF::PrepareData()
{
  Bool_t success = true;

  // keep the Global block info
  PMsrGlobalBlock *globalBlock = fMsrInfo->GetMsrGlobal();

  // get the proper run
  PRawRunData* runData = fRawData->GetRunData(*fRunInfo->GetRunName());
  if (!runData) { // couldn't get run
    std::cerr << std::endl << ">> PRunSingleHistoRRF::PrepareData(): **ERROR** Couldn't get run " << fRunInfo->GetRunName()->Data() << "!";
    std::cerr << std::endl;
    return false;
  }

  // collect histogram numbers
  PUIntVector histoNo; // histoNo = msr-file forward + redGreen_offset - 1
  for (UInt_t i=0; i<fRunInfo->GetForwardHistoNoSize(); i++) {
    histoNo.push_back(fRunInfo->GetForwardHistoNo(i));

    if (!runData->IsPresent(histoNo[i])) {
      std::cerr << std::endl << ">> PRunSingleHistoRRF::PrepareData(): **PANIC ERROR**:";
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
        std::cerr << std::endl << ">> PRunSingleHistoRRF::PrepareData(): **ERROR** Couldn't get addrun " << fRunInfo->GetRunName(i)->Data() << "!";
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
      if ((static_cast<Int_t>(j)+static_cast<Int_t>(fT0s[i])-static_cast<Int_t>(fT0s[0]) >= 0) &&
          (j+static_cast<Int_t>(fT0s[i])-static_cast<Int_t>(fT0s[0]) < runData->GetDataBin(histoNo[i])->size())) {
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

  // do the more fit/view specific stuff
  if (fHandleTag == kFit)
    success = PrepareFitData(runData, histoNo[0]);
  else if (fHandleTag == kView)
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
 * <p>Take the pre-processed data (i.e. grouping and addrun are preformed) and form the RRF histogram for fitting.
 * The following steps are preformed:
 * -# get fit start/stop time
 * -# check that 'first good data bin', 'last good data bin', and 't0' make any sense
 * -# check how the background shall be handled, i.e. fitted, subtracted from background estimate data range, or subtacted from a given fixed background.
 * -# estimate N0
 * -# RRF transformation
 * -# packing (i.e rebinning)
 *
 * <b>return:</b>
 * - true, if everything went smooth
 * - false, otherwise
 *
 * \param runData raw run data handler
 * \param histoNo forward histogram number
 */
Bool_t PRunSingleHistoRRF::PrepareFitData(PRawRunData* runData, const UInt_t histoNo)
{
  // keep the raw data for the RRF asymmetry error estimate for later
  PDoubleVector rawNt;
  for (UInt_t i=0; i<fForward.size(); i++) {
    rawNt.push_back(fForward[i]);  // N(t) without any corrections
  }
  Double_t freqMax = GetMainFrequency(rawNt);
  std::cout << "info> freqMax=" << freqMax << " (MHz)" << std::endl;

  // "optimal packing"
  Double_t optNoPoints = 8;
  if (freqMax < 271.0) // < 271 MHz, i.e ~ 2T
    optNoPoints = 5;
  std::cout << "info> optimal packing: " << static_cast<Int_t>(1.0 / (fTimeResolution*(freqMax - fMsrInfo->GetMsrGlobal()->GetRRFFreq("MHz"))) / optNoPoints);

  // initially fForward is the "raw data set" (i.e. grouped histo and raw runs already added) to be fitted. This means fForward = N(t) at this point.

  // 1) check how the background shall be handled
  // subtract background from histogramms ------------------------------------------
  if (fRunInfo->GetBkgFix(0) == PMUSR_UNDEFINED) { // no fixed background given
    if (fRunInfo->GetBkgRange(0) >= 0) {
      if (!EstimateBkg(histoNo))
        return false;
    } else { // no background given to do the job, try estimate
      fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.1), 0);
      fRunInfo->SetBkgRange(static_cast<Int_t>(fT0s[0]*0.6), 1);
      std::cerr << std::endl << ">> PRunSingleHistoRRF::PrepareFitData(): **WARNING** Neither fix background nor background bins are given!";
      std::cerr << std::endl << ">> Will try the following: bkg start = " << fRunInfo->GetBkgRange(0) << ", bkg end = " << fRunInfo->GetBkgRange(1);
      std::cerr << std::endl << ">> NO WARRANTY THAT THIS MAKES ANY SENSE! Better check ...";
      std::cerr << std::endl;
      if (!EstimateBkg(histoNo))
        return false;
    }
    // subtract background from fForward
    for (UInt_t i=0; i<fForward.size(); i++)
      fForward[i] -= fBackground;
  } else { // fixed background given
    for (UInt_t i=0; i<fForward.size(); i++) {
      fForward[i] -= fRunInfo->GetBkgFix(0);
    }
    fBackground = fRunInfo->GetBkgFix(0);
  }
  // here fForward = N(t) - Nbkg

  Int_t t0 = static_cast<Int_t>(fT0s[0]);

  // 2) N(t) - Nbkg -> exp(+t/tau) [N(t)-Nbkg]
  Double_t startTime = fTimeResolution * (static_cast<Double_t>(fGoodBins[0]) - static_cast<Double_t>(t0));

  Double_t time_tau=0.0;
  Double_t exp_t_tau=0.0;
  for (Int_t i=fGoodBins[0]; i<fGoodBins[1]; i++) {
    time_tau = (startTime + fTimeResolution * (i - fGoodBins[0])) / PMUON_LIFETIME;
    exp_t_tau = exp(time_tau);
    fForward[i] *= exp_t_tau;
    fM.push_back(fForward[i]);   // i.e. M(t) = [N(t)-Nbkg] exp(+t/tau); needed to estimate N0 later on
    fMerr.push_back(exp_t_tau*sqrt(rawNt[i]+fBkgErr*fBkgErr));
  }

  // calculate weights
  for (UInt_t i=0; i<fMerr.size(); i++) {
    if (fMerr[i] > 0.0)
      fW.push_back(1.0/(fMerr[i]*fMerr[i]));
    else
      fW.push_back(1.0);
  }
  // now fForward = exp(+t/tau) [N(t)-Nbkg] = M(t)

  // 3) estimate N0
  Double_t errN0 = 0.0;
  Double_t n0 = EstimateN0(errN0, freqMax);

  // 4a) A(t) = exp(+t/tau) [N(t)-Nbkg] / N0 - 1.0
  for (Int_t i=fGoodBins[0]; i<=fGoodBins[1]; i++) {
    fForward[i] = fForward[i] / n0 - 1.0;
  }

  // 4b) error estimate of A(t): errA(t) = exp(+t/tau)/N0 sqrt( N(t) + ([N(t)-N_bkg]/N0)^2 errN0^2 )
  for (Int_t i=fGoodBins[0]; i<=fGoodBins[1]; i++) {
    time_tau = (startTime + fTimeResolution * (i - fGoodBins[0])) / PMUON_LIFETIME;
    exp_t_tau = exp(time_tau);
    fAerr.push_back(exp_t_tau/n0*sqrt(rawNt[i]+pow(((rawNt[i]-fBackground)/n0)*errN0,2.0)));
  }

  // 5) rotate A(t): A(t) -> 2* A(t) * cos(wRRF t + phiRRF), the factor 2.0 is needed since the high frequency part is suppressed.
  PMsrGlobalBlock *globalBlock = fMsrInfo->GetMsrGlobal();
  Double_t wRRF = globalBlock->GetRRFFreq("Mc");
  Double_t phaseRRF = globalBlock->GetRRFPhase()*TMath::TwoPi()/180.0;
  Double_t time = 0.0;
  for (Int_t i=fGoodBins[0]; i<=fGoodBins[1]; i++) {
    time = startTime + fTimeResolution * (static_cast<Double_t>(i) - static_cast<Double_t>(fGoodBins[0]));
    fForward[i] *= 2.0*cos(wRRF * time + phaseRRF);
  }

  // 6) RRF packing
  Double_t dval=0.0;
  for (Int_t i=fGoodBins[0]; i<=fGoodBins[1]; i++) {
    if (fRRFPacking == 1) {
      fData.AppendValue(fForward[i]);
    } else { // RRF packing > 1
      if (((i-fGoodBins[0]) % fRRFPacking == 0) && (i != fGoodBins[0])) { // fill data
        dval /= fRRFPacking;
        fData.AppendValue(dval);
        // reset dval
        dval = 0.0;
      }
      dval += fForward[i];
    }
  }

  // 7) estimate packed RRF errors (see log-book p.204)
  //    the error estimate of the unpacked RRF asymmetry is: errA_RRF(t) \simeq exp(t/tau)/N0 sqrt( [N(t) + ((N(t)-N_bkg)/N0)^2 errN0^2] )
  dval = 0.0;
  // the packed RRF asymmetry error
  for (Int_t i=fGoodBins[0]; i<=fGoodBins[1]; i++) {
    if (((i-fGoodBins[0]) % fRRFPacking == 0) && (i != fGoodBins[0])) { // fill data
      fData.AppendErrorValue(sqrt(2.0*dval)/fRRFPacking); // the factor 2.0 is needed since the high frequency part is suppressed.
      dval = 0.0;
    }
    dval += fAerr[i-fGoodBins[0]]*fAerr[i-fGoodBins[0]];
  }

  // set start time and time step
  fData.SetDataTimeStart(fTimeResolution*(static_cast<Double_t>(fGoodBins[0])-static_cast<Double_t>(t0)+static_cast<Double_t>(fRRFPacking-1)/2.0));
  fData.SetDataTimeStep(fTimeResolution*fRRFPacking);

  CalcNoOfFitBins();

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
 * <b>return:</b>
 * - true, if everything went smooth
 * - false, otherwise
 *
 * \param runData raw run data handler
 * \param histoNo forward histogram number
 */
Bool_t PRunSingleHistoRRF::PrepareViewData(PRawRunData* runData, const UInt_t histoNo)
{
  // --------------
  // prepare data
  // --------------

  // prepare RRF single histo
  PrepareFitData(runData, histoNo);

  // check for view packing
  Int_t viewPacking = fMsrInfo->GetMsrPlotList()->at(0).fViewPacking;
  if (viewPacking > 0) {
    if (viewPacking < fRRFPacking) {
      std::cerr << ">> PRunSingleHistoRRF::PrepareViewData(): **WARNING** Found View Packing (" << viewPacking << ") < RRF Packing (" << fRRFPacking << ").";
      std::cerr << ">> Will ignore View Packing." << std::endl;
    } else {
      // STILL MISSING
    }
  }

  // --------------
  // prepare theory
  // --------------

  // feed the parameter vector
  std::vector<Double_t> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (UInt_t i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par);
  }

  // check if a finer binning for the theory is needed
  UInt_t size = fForward.size();
  Double_t factor = 1.0;
  Double_t time = 0.0;
  Double_t theoryValue = 0.0;

  if (fData.GetValue()->size() * 10 > fForward.size()) {
    size = fData.GetValue()->size() * 10;
    factor = static_cast<Double_t>(fForward.size()) / static_cast<Double_t>(size);
  }
  fData.SetTheoryTimeStart(fData.GetDataTimeStart());
  fData.SetTheoryTimeStep(fTimeResolution*factor);

  // calculate theory
  for (UInt_t i=0; i<size; i++) {
    time = fData.GetTheoryTimeStart() + static_cast<Double_t>(i)*fData.GetTheoryTimeStep();
    theoryValue = fTheory->Func(time, par, fFuncValues);
    if (fabs(theoryValue) > 10.0) { // dirty hack needs to be fixed!!
      theoryValue = 0.0;
    }
    fData.AppendTheoryValue(theoryValue);
  }

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
Bool_t PRunSingleHistoRRF::GetProperT0(PRawRunData* runData, PMsrGlobalBlock *globalBlock, PUIntVector &histoNo)
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

      std::cerr << std::endl << ">> PRunSingleHistoRRF::GetProperT0(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
      std::cerr << std::endl << ">> run: " << fRunInfo->GetRunName()->Data();
      std::cerr << std::endl << ">> will try the estimated one: forward t0 = " << runData->GetT0BinEstimated(histoNo[i]);
      std::cerr << std::endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
      std::cerr << std::endl;
    }
  }

  // check if t0 is within proper bounds
  for (UInt_t i=0; i<fRunInfo->GetForwardHistoNoSize(); i++) {
    if ((fT0s[i] < 0.0) || (fT0s[i] > static_cast<Int_t>(runData->GetDataBin(histoNo[i])->size()))) {
      std::cerr << std::endl << ">> PRunSingleHistoRRF::GetProperT0(): **ERROR** t0 data bin (" << fT0s[i] << ") doesn't make any sense!";
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
        std::cerr << std::endl << ">> PRunSingleHistoRRF::GetProperT0(): **ERROR** Couldn't get addrun " << fRunInfo->GetRunName(i)->Data() << "!";
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

          std::cerr << std::endl << ">> PRunSingleHistoRRF::GetProperT0(): **WARRNING** NO t0's found, neither in the run data nor in the msr-file!";
          std::cerr << std::endl << ">> run: " << fRunInfo->GetRunName(i)->Data();
          std::cerr << std::endl << ">> will try the estimated one: forward t0 = " << addRunData->GetT0BinEstimated(histoNo[j]);
          std::cerr << std::endl << ">> NO WARRANTY THAT THIS OK!! For instance for LEM this is almost for sure rubbish!";
          std::cerr << std::endl;
        }
      }

      // check if t0 is within proper bounds
      for (UInt_t j=0; j<fRunInfo->GetForwardHistoNoSize(); j++) {
        if ((fAddT0s[i-1][j] < 0) || (fAddT0s[i-1][j] > static_cast<Int_t>(addRunData->GetDataBin(histoNo[j])->size()))) {
          std::cerr << std::endl << ">> PRunSingleHistoRRF::GetProperT0(): **ERROR** addt0 data bin (" << fAddT0s[i-1][j] << ") doesn't make any sense!";
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
Bool_t PRunSingleHistoRRF::GetProperDataRange()
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
    std::cerr << std::endl << ">> PRunSingleHistoRRF::GetProperDataRange(): **WARNING** data range was not provided, will try data range start = t0+" << offset << "(=10ns) = " << start << ".";
    std::cerr << std::endl << ">> NO WARRANTY THAT THIS DOES MAKE ANY SENSE.";
    std::cerr << std::endl;
  }
  if (end < 0) {
    end = fForward.size();
    fRunInfo->SetDataRange(end, 1);
    std::cerr << std::endl << ">> PRunSingleHistoRRF::GetProperDataRange(): **WARNING** data range was not provided, will try data range end = " << end << ".";
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
    std::cerr << std::endl << ">> PRunSingleHistoRRF::GetProperDataRange(): **ERROR** start data bin (" << start << ") doesn't make any sense!";
    std::cerr << std::endl;
    return false;
  }
  // 3rd check if end is within proper bounds
  if (end < 0) {
    std::cerr << std::endl << ">> PRunSingleHistoRRF::GetProperDataRange(): **ERROR** end data bin (" << end << ") doesn't make any sense!";
    std::cerr << std::endl;
    return false;
  }
  if (end > static_cast<Int_t>(fForward.size())) {
    std::cerr << std::endl << ">> PRunSingleHistoRRF::GetProperDataRange(): **WARNING** end data bin (" << end << ") > histo length (" << fForward.size() << ").";
    std::cerr << std::endl << ">>    Will set end = (histo length - 1). Consider to change it in the msr-file." << std::endl;
    std::cerr << std::endl;
    end = static_cast<Int_t>(fForward.size()-1);
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
void PRunSingleHistoRRF::GetProperFitRange(PMsrGlobalBlock *globalBlock)
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
    std::cerr << ">> PRunSingleHistoRRF::GetProperFitRange(): **WARNING** Couldn't get fit start/end time!" << std::endl;
    std::cerr << ">>    Will set it to fgb/lgb which given in time is: " << fFitStartTime << "..." << fFitEndTime << " (usec)" << std::endl;
  }
}

//--------------------------------------------------------------------------
// GetMainFrequency (private)
//--------------------------------------------------------------------------
/**
 * <p>gets the maximum frequency of the given data.
 *
 * \param raw data set.
 */
Double_t PRunSingleHistoRRF::GetMainFrequency(PDoubleVector &data)
{
  Double_t freqMax = 0.0;

  // create histo
  Double_t startTime = (fGoodBins[0]-fT0s[0]) * fTimeResolution;
  Int_t noOfBins = fGoodBins[1]-fGoodBins[0]+1;
  TH1F *histo = new TH1F("data", "data", noOfBins, startTime-fTimeResolution/2.0, startTime+fTimeResolution/2.0+noOfBins*fTimeResolution);
  for (Int_t i=fGoodBins[0]; i<=fGoodBins[1]; i++) {
    histo->SetBinContent(i-fGoodBins[0]+1, data[i]);
  }

  // Fourier transform
  PFourier *ft = new PFourier(histo, FOURIER_UNIT_FREQ);
  ft->Transform(F_APODIZATION_STRONG);

  // find frequency maximum
  TH1F *power = ft->GetPowerFourier();
  Double_t maxFreqVal = 0.0;
  for (Int_t i=1; i<power->GetNbinsX(); i++) {
    // ignore dc part at 0 frequency
    if (i<power->GetNbinsX()-1) {
      if (power->GetBinContent(i)>power->GetBinContent(i+1))
        continue;
    }
    // ignore everything below 10 MHz
    if (power->GetBinCenter(i) < 10.0)
      continue;
    // check for maximum
    if (power->GetBinContent(i) > maxFreqVal) {
      maxFreqVal = power->GetBinContent(i);
      freqMax = power->GetBinCenter(i);
    }
  }

  // clean up
  if (power)
    delete power;
  if (ft)
    delete ft;
  if (histo)
    delete histo;

  return freqMax;
}

//--------------------------------------------------------------------------
// EstimateN0 (private)
//--------------------------------------------------------------------------
/**
 * <p>Estimate the N0 for the given run.
 *
 * \param errN0
 */
Double_t PRunSingleHistoRRF::EstimateN0(Double_t &errN0, Double_t freqMax)
{
  // endBin is estimated such that the number of full cycles (according to the maximum frequency of the data)
  // is approximately the time fN0EstimateEndTime.
  Int_t endBin = static_cast<Int_t>(round(ceil(fN0EstimateEndTime*freqMax/TMath::TwoPi()) * (TMath::TwoPi()/freqMax) / fTimeResolution));

  Double_t n0 = 0.0;
  Double_t wN = 0.0;
  for (Int_t i=0; i<endBin; i++) {
//    n0 += fW[i]*fM[i];
    n0 += fM[i];
    wN += fW[i];
  }
//  n0 /= wN;
  n0 /= endBin;

  errN0 = 0.0;
  for (Int_t i=0; i<endBin; i++) {
    errN0 += fW[i]*fW[i]*fMerr[i]*fMerr[i];
  }
  errN0 = sqrt(errN0)/wN;

  std::cout << "info> PRunSingleHistoRRF::EstimateN0(): N0=" << n0 << "(" << errN0 << ")" << std::endl;

  return n0;
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
Bool_t PRunSingleHistoRRF::EstimateBkg(UInt_t histoNo)
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
    std::cout << std::endl << "PRunSingleHistoRRF::EstimatBkg(): end = " << end << " > start = " << start << "! Will swap them!";
    UInt_t keep = end;
    end = start;
    start = keep;
  }

  // calculate proper background range
  if (beamPeriod != 0.0) {
    Double_t timeBkg = static_cast<Double_t>(end-start)*fTimeResolution; // length of the background intervall in time
    UInt_t fullCycles = static_cast<UInt_t>(timeBkg/beamPeriod); // how many proton beam cylces can be placed within the proposed background intervall
    // correct the end of the background intervall such that the background is as close as possible to a multiple of the proton cylce
    end = start + static_cast<UInt_t>((fullCycles*beamPeriod)/fTimeResolution);
    std::cout << std::endl << "PRunSingleHistoRRF::EstimatBkg(): Background " << start << ", " << end;
    if (end == start)
      end = fRunInfo->GetBkgRange(1);
  }

  // check if start is within histogram bounds
  if (start >= fForward.size()) {
    std::cerr << std::endl << ">> PRunSingleHistoRRF::EstimatBkg(): **ERROR** background bin values out of bound!";
    std::cerr << std::endl << ">> histo lengths    = " << fForward.size();
    std::cerr << std::endl << ">> background start = " << start;
    std::cerr << std::endl;
    return false;
  }

  // check if end is within histogram bounds
  if (end >= fForward.size()) {
    std::cerr << std::endl << ">> PRunSingleHistoRRF::EstimatBkg(): **ERROR** background bin values out of bound!";
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

  fBackground = bkg;  // keep background (per bin)

  bkg = 0.0;
  for (UInt_t i=start; i<end; i++)
    bkg += pow(fForward[i]-fBackground, 2.0);
  fBkgErr = sqrt(bkg/(static_cast<Double_t>(end - start)));

  std::cout << std::endl << "info> fBackground=" << fBackground << "(" << fBkgErr << ")" << std::endl;

  fRunInfo->SetBkgEstimated(fBackground, 0);

  return true;
}
