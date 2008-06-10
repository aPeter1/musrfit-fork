/***************************************************************************

  PRunBase.cpp

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

#include <TROOT.h>
#include <TSystem.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TFile.h>
#include <TFolder.h>

#include "TLemRunHeader.h"

#include "PRunBase.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p> needed otherwise vector's cannot be generated ;-)
 *
 */
PRunBase::PRunBase()
{
  fRunNo = -1;
  fRunInfo = 0;
  fRawData = 0;
  fTimeResolution = -1.0;

  fValid = true;
  fHandleTag = kEmpty;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param msrInfo pointer to the msr info structure
 * \param rawData
 * \param runNo
 * \param tag
 */
PRunBase::PRunBase(PMsrHandler *msrInfo, PRunDataHandler *rawData, unsigned int runNo, EPMusrHandleTag tag)
{
  fValid = true;
  fHandleTag = tag;

  fRunNo = static_cast<int>(runNo);
  if ((runNo < 0) || (runNo > msrInfo->GetMsrRunList()->size())) {
    fRunInfo = 0;
    return;
  }

  // keep pointer to the msr-file handler
  fMsrInfo = msrInfo;

  // keep the run header info for this run
  fRunInfo = &(*msrInfo->GetMsrRunList())[runNo];

  // check the parameter and map range of the functions
  if (!fMsrInfo->CheckMapAndParamRange(fRunInfo->fMap.size(), fMsrInfo->GetNoOfParams())) {
    cout << endl << "**SEVERE ERROR** PRunBase::PRunBase: map and/or parameter out of range in FUNCTIONS." << endl;
    exit(0);
  }

  // keep the raw data of the runs
  fRawData = rawData;

  // init private variables
  fTimeResolution = -1.0;
  for (int i=0; i<fMsrInfo->GetNoOfFuncs(); i++)
    fFuncValues.push_back(0.0);

  // generate theory
  fTheory = new PTheory(msrInfo, runNo);
  if (fTheory == 0) {
    cout << endl << "**SEVERE ERROR** PRunBase::PRunBase: Couldn't create an instance of PTheory :-(, will quit" << endl;
    exit(0);
  }
  if (!fTheory->IsValid()) {
    cout << endl << "**SEVERE ERROR** PRunBase::PRunBase: Theory is not valid :-(, will quit" << endl;
    exit(0);
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
PRunBase::~PRunBase()
{
  fParamNo.clear();

  fData.fValue.clear();
  fData.fError.clear();
  fData.fTheory.clear();

  fT0s.clear();

  fFuncValues.clear();
}

//--------------------------------------------------------------------------
// CleanUp
//--------------------------------------------------------------------------
/**
 * <p> Clean up all localy allocate memory
 *
 */
void PRunBase::CleanUp()
{
//cout << endl << "PRunBase::CleanUp() ..." << endl;
  if (fTheory) {
    delete fTheory;
    fTheory = 0;
  }
}
