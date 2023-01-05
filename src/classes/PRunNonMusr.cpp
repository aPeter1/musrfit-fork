/***************************************************************************

  PRunNonMusr.cpp

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

#include <iostream>

#include "PRunNonMusr.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 */
PRunNonMusr::PRunNonMusr() : PRunBase()
{
  fNoOfFitBins  = 0;
  fPacking = 1;
  fStartTimeBin = 0;
  fEndTimeBin = 0;

  fHandleTag = kEmpty;

  fRawRunData = nullptr;
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
PRunNonMusr::PRunNonMusr(PMsrHandler *msrInfo, PRunDataHandler *rawData, UInt_t runNo, EPMusrHandleTag tag) : PRunBase(msrInfo, rawData, runNo, tag)
{
  // get the proper run
  fRawRunData = fRawData->GetRunData(*(fRunInfo->GetRunName()));
  if (!fRawRunData) { // couldn't get run
    std::cerr << std::endl << "PRunNonMusr::PRunNonMusr(): **ERROR** Couldn't get raw run  data!";
    std::cerr << std::endl;
    fValid = false;
  }

  // calculate fData
  if (!PrepareData())
    fValid = false;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor
 */
PRunNonMusr::~PRunNonMusr()
{
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
Double_t PRunNonMusr::CalcChiSquare(const std::vector<Double_t>& par)
{
  Double_t chisq = 0.0;
  Double_t diff = 0.0;

  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par, fMetaData);
  }

  // calculate chi square
  Double_t x(1.0);
  for (UInt_t i=fStartTimeBin; i<=fEndTimeBin; i++) {
    x = fData.GetX()->at(i);
    diff = fData.GetValue()->at(i) - fTheory->Func(x, par, fFuncValues);
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
Double_t PRunNonMusr::CalcChiSquareExpected(const std::vector<Double_t>& par)
{
  std::cout << std::endl << "PRunNonMusr::CalcChiSquareExpected(): not implemented yet ..." << std::endl;

  return 0.0;
}

//--------------------------------------------------------------------------
// CalcMaxLikelihood
//--------------------------------------------------------------------------
/**
 * <p>Calculate log maximum-likelihood. Currently not implemented since not clear what to be done.
 *
 * <b>return:</b>
 * - log maximum-likelihood value == 1.0
 *
 * \param par parameter vector iterated by minuit2
 */
Double_t PRunNonMusr::CalcMaxLikelihood(const std::vector<Double_t>& par)
{
  std::cout << std::endl << "PRunNonMusr::CalcMaxLikelihood(): not implemented yet ..." << std::endl;

  return 1.0;
}

//--------------------------------------------------------------------------
// CalcTheory
//--------------------------------------------------------------------------
/**
 * <p>Calculate theory for a given set of fit-parameters.
 */
void PRunNonMusr::CalcTheory()
{
}

//--------------------------------------------------------------------------
// GetNoOfFitBins (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate the number of fitted bins for the current fit range.
 *
 * <b>return:</b> number of fitted bins.
 */
UInt_t PRunNonMusr::GetNoOfFitBins()
{
  fNoOfFitBins=0;
  Double_t x;
  for (UInt_t i=0; i<fData.GetValue()->size(); i++) {
    x = fData.GetX()->at(i);
    if ((x >= fFitStartTime) && (x <= fFitEndTime))
      fNoOfFitBins++;
  }

  return fNoOfFitBins;
}

//--------------------------------------------------------------------------
// PrepareData
//--------------------------------------------------------------------------
/**
 * <p>Prepare data for fitting or viewing.
 *
 * <b>return:</b>
 * - true if everthing went smooth
 * - false, otherwise.
 */
Bool_t PRunNonMusr::PrepareData()
{
  Bool_t success = true;

  if (fRunInfo->GetRunNameSize() > 1) { // ADDRUN present which is not supported for NonMusr
    std::cerr << std::endl << ">> PRunNonMusr::PrepareData(): **WARNING** ADDRUN NOT SUPPORTED FOR THIS FIT TYPE, WILL IGNORE IT." << std::endl;
  }

  // get packing info
  fPacking = fRunInfo->GetPacking();
  if (fPacking == -1) { // packing not present in the RUN block, will try the GLOBAL block
    fPacking = fMsrInfo->GetMsrGlobal()->GetPacking();
  }
  if (fPacking == -1) { // packing NOT present, in neither the RUN block, nor in the GLOBAL block
    std::cerr << std::endl << ">> PRunNonMusr::PrepareData(): **ERROR** couldn't find any packing information." << std::endl;
    return false;
  }

  // get fit start/end time
  fFitStartTime = fRunInfo->GetFitRange(0);
  fFitEndTime   = fRunInfo->GetFitRange(1);
  if (fFitStartTime == PMUSR_UNDEFINED) { // not present in the RUN block, will try GLOBAL block
    fFitStartTime = fMsrInfo->GetMsrGlobal()->GetFitRange(0);
    fFitEndTime   = fMsrInfo->GetMsrGlobal()->GetFitRange(1);
  }

  if (fHandleTag == kFit)
    success = PrepareFitData();
  else if (fHandleTag == kView)
    success = PrepareViewData();
  else
    success = false;

  return success;
}

//--------------------------------------------------------------------------
// PrepareFitData
//--------------------------------------------------------------------------
/**
 * <p>Prepare data for fitting.
 *
 * <b>return:</b>
 * - true if everthing went smooth
 * - false, otherwise.
 */
Bool_t PRunNonMusr::PrepareFitData()
{
  Bool_t success = true;

  // get x-, y-index
  UInt_t xIndex = GetXIndex();
  UInt_t yIndex = GetYIndex();

  // pack the raw data
  Double_t value  = 0.0;
  Double_t err = 0.0;
  for (UInt_t i=0; i<fRawRunData->fDataNonMusr.GetData()->at(xIndex).size(); i++) {
    if (fPacking == 1) {
      fData.AppendXValue(fRawRunData->fDataNonMusr.GetData()->at(xIndex).at(i));
      fData.AppendValue(fRawRunData->fDataNonMusr.GetData()->at(yIndex).at(i));
      fData.AppendErrorValue(fRawRunData->fDataNonMusr.GetErrData()->at(yIndex).at(i));
    } else { // packed data, i.e. fPacking > 1
      if ((i % fPacking == 0) && (i != 0)) { // fill data
        fData.AppendXValue(fRawRunData->fDataNonMusr.GetData()->at(xIndex).at(i)-(fRawRunData->fDataNonMusr.GetData()->at(xIndex).at(i)-fRawRunData->fDataNonMusr.GetData()->at(xIndex).at(i-fPacking))/2.0);
        fData.AppendValue(value);
        fData.AppendErrorValue(TMath::Sqrt(err));
        value = 0.0;
        err = 0.0;
      }
      // sum raw data values
      value += fRawRunData->fDataNonMusr.GetData()->at(yIndex).at(i);
      err += fRawRunData->fDataNonMusr.GetErrData()->at(yIndex).at(i)*fRawRunData->fDataNonMusr.GetErrData()->at(yIndex).at(i);
    }
  }

  // count the number of bins to be fitted
  fNoOfFitBins=0;
  Double_t x;
  for (UInt_t i=0; i<fData.GetValue()->size(); i++) {
    x = fData.GetX()->at(i);
    if ((x >= fFitStartTime) && (x <= fFitEndTime))
      fNoOfFitBins++;
  }

  // get start/end bin
  const PDoubleVector *xx = fData.GetX();
  fStartTimeBin = 0;
  fEndTimeBin = xx->size()-1;
  for (UInt_t i=0; i<xx->size(); i++) {
    if (xx->at(i) < fFitStartTime)
      fStartTimeBin = i;
    if (xx->at(i) < fFitEndTime)
      fEndTimeBin = i;
  }

  return success;
}

//--------------------------------------------------------------------------
// PrepareViewData
//--------------------------------------------------------------------------
/**
 * <p>Prepare data for viewing.
 *
 * <b>return:</b>
 * - true if everthing went smooth
 * - false, otherwise.
 */
Bool_t PRunNonMusr::PrepareViewData()
{
  Bool_t success = true;

  // get x-, y-index
  UInt_t xIndex = GetXIndex();
  UInt_t yIndex = GetYIndex();

  // fill data histo
  // pack the raw data
  Double_t value  = 0.0;
  Double_t err = 0.0;
  for (UInt_t i=0; i<fRawRunData->fDataNonMusr.GetData()->at(xIndex).size(); i++) {
    if (fPacking == 1) {
      fData.AppendXValue(fRawRunData->fDataNonMusr.GetData()->at(xIndex).at(i));
      fData.AppendValue(fRawRunData->fDataNonMusr.GetData()->at(yIndex).at(i));
      fData.AppendErrorValue(fRawRunData->fDataNonMusr.GetErrData()->at(yIndex).at(i));
    } else { // packed data, i.e. fPacking > 1
      if ((i % fPacking == 0) && (i != 0)) { // fill data
        fData.AppendXValue(fRawRunData->fDataNonMusr.GetData()->at(xIndex).at(i)-(fRawRunData->fDataNonMusr.GetData()->at(xIndex).at(i)-fRawRunData->fDataNonMusr.GetData()->at(xIndex).at(i-fPacking))/2.0);
        fData.AppendValue(value);
        fData.AppendErrorValue(TMath::Sqrt(err));
        value = 0.0;
        err = 0.0;
      }
      // sum raw data values
      value += fRawRunData->fDataNonMusr.GetData()->at(yIndex).at(i);
      err += fRawRunData->fDataNonMusr.GetErrData()->at(yIndex).at(i)*fRawRunData->fDataNonMusr.GetErrData()->at(yIndex).at(i);
    }
  }

  // count the number of bins to be fitted
  fNoOfFitBins = fData.GetValue()->size();

  // fill theory histo
  // feed the parameter vector
  std::vector<Double_t> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (UInt_t i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);
  // calculate functions
  for (Int_t i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), *fRunInfo->GetMap(), par, fMetaData);
  }

  // get plot range
  PMsrPlotList *plotList;
  PMsrPlotStructure plotBlock;
  plotList = fMsrInfo->GetMsrPlotList();
  // find the proper plot block
  // Here a small complication to be handled: there are potentially multiple
  // run blocks and the run might be present in various of these run blocks. In
  // order to get a nice resolution on the theory the following procedure will be
  // followed: the smallest x-interval found will be used to for the fXTheory resolution
  // which is 1000 function points. The function will be calculated from the smallest
  // xmin found up to the largest xmax found.
  Double_t xMin = 0.0, xMax = 0.0;

  // init xMin/xMax, xAbsMin/xAbsMax
  plotBlock = plotList->at(0);
  if (plotBlock.fTmin.size() == 0) { // check if no range information is present
    PMsrRunList *runList = fMsrInfo->GetMsrRunList();
    xMin = runList->at(0).GetFitRange(0);
    xMax = runList->at(0).GetFitRange(1);
    for (UInt_t i=1; i<runList->size(); i++) {
      if (runList->at(i).GetFitRange(0) < xMin)
        xMin = runList->at(i).GetFitRange(0);
      if (runList->at(i).GetFitRange(1) > xMax)
        xMax = runList->at(i).GetFitRange(1);
    }
  } else if (plotBlock.fTmin.size() == 1) { // check if 'range' information is present
    xMin = plotBlock.fTmin[0];
    xMax = plotBlock.fTmax[0];
  } else if (plotBlock.fTmin.size() > 1) { // check if 'sub_ranges' information is present
    xMin = plotBlock.fTmin[0];
    xMax = plotBlock.fTmax[0];
    for (UInt_t i=1; i<plotBlock.fTmin.size(); i++) {
      if (plotBlock.fTmin[i] < xMin)
        xMin = plotBlock.fTmin[i];
      if (plotBlock.fTmax[i] > xMax)
        xMax = plotBlock.fTmax[i];
    }
  }

  if (plotBlock.fUseFitRanges) { // check if 'use_fit_ranges' information is present
    PMsrRunList *runList = fMsrInfo->GetMsrRunList();
    xMin = runList->at(0).GetFitRange(0);
    xMax = runList->at(0).GetFitRange(1);
    for (UInt_t i=1; i<runList->size(); i++) {
      if (runList->at(i).GetFitRange(0) < xMin)
        xMin = runList->at(i).GetFitRange(0);
      if (runList->at(i).GetFitRange(1) > xMax)
        xMax = runList->at(i).GetFitRange(1);
    }
  }

  for (UInt_t i=1; i<plotList->size(); i++) { // go through all the plot blocks
    plotBlock = plotList->at(i);

    if (plotBlock.fTmin.size() == 0) { // check if no range information is present
      PMsrRunList *runList = fMsrInfo->GetMsrRunList();
      for (UInt_t i=0; i<runList->size(); i++) {
        if (runList->at(i).GetFitRange(0) < xMin)
          xMin = runList->at(i).GetFitRange(0);
        if (runList->at(i).GetFitRange(1) > xMax)
          xMax = runList->at(i).GetFitRange(1);
      }
    } else if (plotBlock.fTmin.size() == 1) { // check if 'range' information is present
      if (plotBlock.fTmin[0] < xMin)
        xMin = plotBlock.fTmin[0];
      if (plotBlock.fTmax[0] > xMax)
        xMax = plotBlock.fTmax[0];
    } else if (plotBlock.fTmin.size() > 1) { // check if 'sub_ranges' information is present
      for (UInt_t i=0; i<plotBlock.fTmin.size(); i++) {
        if (plotBlock.fTmin[i] < xMin)
          xMin = plotBlock.fTmin[i];
        if (plotBlock.fTmax[i] > xMax)
          xMax = plotBlock.fTmax[i];
      }
    }

    if (plotBlock.fUseFitRanges) { // check if 'use_fit_ranges' information is present
      PMsrRunList *runList = fMsrInfo->GetMsrRunList();
      for (UInt_t i=0; i<runList->size(); i++) {
        if (runList->at(i).GetFitRange(0) < xMin)
          xMin = runList->at(i).GetFitRange(0);
        if (runList->at(i).GetFitRange(1) > xMax)
          xMax = runList->at(i).GetFitRange(1);
      }
    }
  }

  // typically take 1000 points to calculate the theory, except if there are more data points, than take that number
  Double_t xStep;
  if (fData.GetX()->size() > 1000.0)
    xStep = (xMax-xMin)/fData.GetX()->size();
  else
    xStep = (xMax-xMin)/1000.0;

  Double_t xx = xMin;
  do {
    // fill x-vector
    fData.AppendXTheoryValue(xx);
    // fill y-vector
    fData.AppendTheoryValue(fTheory->Func(xx, par, fFuncValues));
    // calculate next xx
    xx += xStep;
  } while (xx < xMax);

  // clean up
  par.clear();

  return success;
}

//--------------------------------------------------------------------------
// GetXIndex
//--------------------------------------------------------------------------
/**
 * <p>Returns the x-axis data index.
 *
 * <b>return:</b>
 * - x-index
 */
UInt_t PRunNonMusr::GetXIndex()
{
  UInt_t index = 0;
  Bool_t found = false;

  if (fRawRunData->fDataNonMusr.FromAscii()) { // ascii-file format
    index = 0;
    found = true;
  } else { // db-file format
    if (fRunInfo->GetXDataIndex() > 0) { // xy-data already indices
      index = fRunInfo->GetXDataIndex()-1; // since xy-data start with 1 ...
      found = true;
    } else { // xy-data data tags which needs to be converted to an index
      for (UInt_t i=0; i<fRawRunData->fDataNonMusr.GetDataTags()->size(); i++) {
        if (fRawRunData->fDataNonMusr.GetDataTags()->at(i).CompareTo(*fRunInfo->GetXDataLabel()) == 0) {
          index = i;
          found = true;
          break;
        }
      }
    }
  }

  if (!found) {
    std::cerr << std::endl << "PRunNonMusr::GetXIndex(): **ERROR** Couldn't obtain x-data index!";
    std::cerr << std::endl;
    assert(0);
  }

  return index;
}

//--------------------------------------------------------------------------
// GetYIndex
//--------------------------------------------------------------------------
/**
 * <p>Returns the y-axis data index.
 *
 * <b>return:</b>
 * - y-index
 */
UInt_t PRunNonMusr::GetYIndex()
{
  UInt_t index = 0;
  Bool_t found = false;

  if (fRawRunData->fDataNonMusr.FromAscii()) { // ascii-file format
    index = 1;
    found = true;
  } else { // db-file format
    if (fRunInfo->GetYDataIndex() > 0) { // xy-data already indices
      index = fRunInfo->GetYDataIndex()-1; // since xy-data start with 1 ...
      found = true;
    } else { // xy-data data tags which needs to be converted to an index
      for (UInt_t i=0; i<fRawRunData->fDataNonMusr.GetDataTags()->size(); i++) {
        if (fRawRunData->fDataNonMusr.GetDataTags()->at(i).CompareTo(*fRunInfo->GetYDataLabel()) == 0) {
          index = i;
          found = true;
          break;
        }
      }
    }
  }

  if (!found) {
    std::cerr << std::endl << "PRunNonMusr::GetYIndex(): **ERROR** Couldn't obtain y-data index!";
    std::cerr << std::endl;
    assert(0);
  }

  return index;
}
