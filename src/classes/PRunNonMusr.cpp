/***************************************************************************

  PRunNonMusr.cpp

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

#include "PRunNonMusr.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
PRunNonMusr::PRunNonMusr() : PRunBase()
{
  fFitStartTime = 0.0;
  fFitStopTime  = 0.0;
  fNoOfFitBins  = 0;

  fHandleTag = kEmpty;
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
PRunNonMusr::PRunNonMusr(PMsrHandler *msrInfo, PRunDataHandler *rawData, unsigned int runNo, EPMusrHandleTag tag) : PRunBase(msrInfo, rawData, runNo, tag)
{
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
PRunNonMusr::~PRunNonMusr()
{
}

//--------------------------------------------------------------------------
// CalcChiSquare
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param par parameter vector iterated by minuit
 */
double PRunNonMusr::CalcChiSquare(const std::vector<double>& par)
{
  double chisq = 0.0;
  double diff = 0.0;

  // calculate functions
  for (int i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), fRunInfo->fMap, par);
  }

  // calculate chi square
  double x;
  for (unsigned int i=0; i<fData.fValue.size(); i++) {
    x = fData.fX[i];
    if ((x>=fFitStartTime) && (x<=fFitStopTime)) {
      diff = fData.fValue[i] - fTheory->Func(x, par, fFuncValues);
      chisq += diff*diff / (fData.fError[i]*fData.fError[i]);
    }
  }

//cout << endl << ">> chisq=" << chisq;

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
double PRunNonMusr::CalcMaxLikelihood(const std::vector<double>& par)
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
void PRunNonMusr::CalcTheory()
{
}

//--------------------------------------------------------------------------
// PrepareData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunNonMusr::PrepareData()
{
  bool success = true;

  cout << endl << "in PRunNonMusr::PrepareData(): will feed fFitData";

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
 * <p>
 *
 */
bool PRunNonMusr::PrepareFitData()
{
  bool success = true;

  // get the proper run
  PRawRunData* runData = fRawData->GetRunData(fRunInfo->fRunName);
  if (!runData) { // couldn't get run
    cout << endl << "PRunNonMusr::PrepareFitData(): **ERROR** Couldn't get run " << fRunInfo->fRunName.Data() << "!";
    return false;
  }

  // keep start/stop time for fit: here the meaning is of course start x, stop x
  fFitStartTime = fRunInfo->fFitRange[0];
  fFitStopTime  = fRunInfo->fFitRange[1];

  // pack the raw data
  double value  = 0.0;
  double err = 0.0;
cout << endl << ">> runData->fXData.size()=" << runData->fXData.size();
  for (unsigned int i=0; i<runData->fXData.size(); i++) {
cout << endl << ">> i=" << i << ", packing=" << fRunInfo->fPacking;
    if (fRunInfo->fPacking == 1) {
      fData.fX.push_back(runData->fXData[i]);
      fData.fValue.push_back(runData->fYData[i]);
      fData.fError.push_back(runData->fErrYData[i]);
    } else { // packed data, i.e. fRunInfo->fPacking > 1
      if ((i % fRunInfo->fPacking == 0) && (i != 0)) { // fill data
cout << endl << "-> i=" << i;
        fData.fX.push_back(runData->fXData[i]-(runData->fXData[i]-runData->fXData[i-fRunInfo->fPacking])/2.0);
        fData.fValue.push_back(value);
        fData.fError.push_back(TMath::Sqrt(err));
        value = 0.0;
        err = 0.0;
      }
      // sum raw data values
      value += runData->fYData[i];
      err += runData->fErrYData[i]*runData->fErrYData[i];
    }
  }
cout << endl << ">> fData.fValue.size()=" << fData.fValue.size();

  // count the number of bins to be fitted
  fNoOfFitBins=0;
  double x;
  for (unsigned int i=0; i<fData.fValue.size(); i++) {
    x = fData.fX[i];
    if ((x >= fFitStartTime) && (x <= fFitStopTime))
      fNoOfFitBins++;
  }
cout << endl << ">> fNoOfFitBins=" << fNoOfFitBins;

  return success;
}

//--------------------------------------------------------------------------
// PrepareViewData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PRunNonMusr::PrepareViewData()
{
  bool success = true;

  // get the proper run
  PRawRunData* runData = fRawData->GetRunData(fRunInfo->fRunName);
  if (!runData) { // couldn't get run
    cout << endl << "PRunNonMusr::PrepareViewData(): **ERROR** Couldn't get run " << fRunInfo->fRunName.Data() << "!";
    return false;
  }

  // fill data histo
  // pack the raw data
  double value  = 0.0;
  double err = 0.0;
cout << endl << ">> runData->fXData.size()=" << runData->fXData.size();
  for (unsigned int i=0; i<runData->fXData.size(); i++) {
cout << endl << ">> i=" << i << ", packing=" << fRunInfo->fPacking;
    if (fRunInfo->fPacking == 1) {
      fData.fX.push_back(runData->fXData[i]);
      fData.fValue.push_back(runData->fYData[i]);
      fData.fError.push_back(runData->fErrYData[i]);
    } else { // packed data, i.e. fRunInfo->fPacking > 1
      if ((i % fRunInfo->fPacking == 0) && (i != 0)) { // fill data
cout << endl << "-> i=" << i;
        fData.fX.push_back(runData->fXData[i]-(runData->fXData[i]-runData->fXData[i-fRunInfo->fPacking])/2.0);
        fData.fValue.push_back(value);
        fData.fError.push_back(TMath::Sqrt(err));
        value = 0.0;
        err = 0.0;
      }
      // sum raw data values
      value += runData->fYData[i];
      err += runData->fErrYData[i]*runData->fErrYData[i];
    }
  }
cout << endl << ">> fData.fValue.size()=" << fData.fValue.size();

  // count the number of bins to be fitted
  fNoOfFitBins = fData.fValue.size();
cout << endl << ">> fNoOfFitBins=" << fNoOfFitBins;

  // fill theory histo
  // feed the parameter vector
  std::vector<double> par;
  PMsrParamList *paramList = fMsrInfo->GetMsrParamList();
  for (unsigned int i=0; i<paramList->size(); i++)
    par.push_back((*paramList)[i].fValue);
  // calculate functions
  for (int i=0; i<fMsrInfo->GetNoOfFuncs(); i++) {
    fFuncValues[i] = fMsrInfo->EvalFunc(fMsrInfo->GetFuncNo(i), fRunInfo->fMap, par);
  }

cout << endl << ">> after parameter fill" << endl;
  // get plot range
  PMsrPlotList *plotList;
  PMsrPlotStructure plotBlock;
  plotList = fMsrInfo->GetMsrPlotList();
  // find the proper plot block
  // Here a small complication has to be handled: there are potentially multiple
  // run blocks and the run might be present in various of these run blocks. In
  // order to get a nice resolution on the theory the following procedure will be
  // followed: the smallest x-interval found will be used to for the fXTheory resolution
  // which is 1000 function points. The function will be calculated from the smallest
  // xmin found up to the largest xmax found.
  double xMin, xMax;
  double xAbsMin, xAbsMax;
  bool first = true;
cout << endl << ">> plotList->size()=" << plotList->size();
  for (unsigned int i=0; i<plotList->size(); i++) {
    plotBlock = plotList->at(i);
cout << endl << ">> plotBlock.fRuns.size()=" << plotBlock.fRuns.size() << endl;
    for (unsigned int j=0; j<plotBlock.fRuns.size(); j++) {
cout << endl << ">> j=" << j;
cout << endl << ">> fRunNo=" << fRunNo;
cout << endl << ">> plotBlock.fRuns[j].Re()=" << plotBlock.fRuns[j].Re();
cout << endl;
      if (fRunNo == plotBlock.fRuns[j].Re()-1) { // run found
        if (first) {
          first = false;
          xMin = plotBlock.fTmin;
          xMax = plotBlock.fTmax;
          xAbsMin = xMin;
          xAbsMax = xMax;
cout << endl << ">> first: xMin=" << xMin << ", xMax=" << xMax << endl;
        } else {
          if (fabs(xMax-xMin) > fabs(plotBlock.fTmax-plotBlock.fTmin)) {
            xMin = plotBlock.fTmin;
            xMax = plotBlock.fTmax;
          }
          if (xMin < xAbsMin)
            xAbsMin = xMin;
          if (xMax > xAbsMax)
            xAbsMax = xMax;
cout << endl << ">> !first: xMin=" << xMin << ", xMax=" << xMax << endl;
        }
cout << endl << ">> xMin=" << xMin << ", xMax=" << xMax << endl;
      }
    }
  }
cout << endl << ">> after the xmin/xmax loop." << endl;

  double xStep = (xMax-xMin)/1000.0;
  double xx = xAbsMin;
  do {
    // fill x-vector
    fData.fXTheory.push_back(xx);
    // fill y-vector
    fData.fTheory.push_back(fTheory->Func(xx, par, fFuncValues));
    // calculate next xx
    xx += xStep;
  } while (xx < xAbsMax);

  // clean up
  par.clear();

  return success;
}
