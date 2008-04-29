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
  bool success;

  // calculate fFitData
  if (success) {
    success = PrepareData();
  }
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
  for (unsigned int i=0; i<runData->fXData.size(); i++) {
    if ((i % fRunInfo->fPacking == 0) && (i != 0)) { // fill data
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

  // count the number of bins to be fitted
  fNoOfFitBins=0; // STILL MISSING!!

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

  return success;
}
