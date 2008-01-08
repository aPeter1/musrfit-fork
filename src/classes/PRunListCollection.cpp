/***************************************************************************

  PRunListCollection.cpp

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

#include "PRunListCollection.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param msrInfo pointer to the msr info structure
 * \param runNo number of the run of the msr-file
 */
PRunListCollection::PRunListCollection(PMsrHandler *msrInfo, PRunDataHandler *data)
{
  fMsrInfo = msrInfo;
  fData = data;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
PRunListCollection::~PRunListCollection()
{
//cout << endl << "in ~PRunListCollection() ..." << endl;
//cout << endl << ">> fRunSingleHistoList.size() = " << fRunSingleHistoList.size();
  for (unsigned int i=0; i<fRunSingleHistoList.size(); i++)
    fRunSingleHistoList[i]->CleanUp();
  fRunSingleHistoList.clear();

//cout << endl << ">> fRunAsymmetryList.size() = " << fRunAsymmetryList.size();
  for (unsigned int i=0; i<fRunAsymmetryList.size(); i++)
    fRunAsymmetryList[i]->CleanUp();
  fRunAsymmetryList.clear();

//cout << endl << ">> fRunRRFList.size() = " << fRunRRFList.size();
  for (unsigned int i=0; i<fRunRRFList.size(); i++)
    fRunRRFList[i]->CleanUp();
  fRunRRFList.clear();

//cout << endl << ">> fRunNonMusrList.size() = " << fRunNonMusrList.size();
  for (unsigned int i=0; i<fRunNonMusrList.size(); i++)
    fRunNonMusrList[i]->CleanUp();
  fRunNonMusrList.clear();
}

//--------------------------------------------------------------------------
// Add
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runNo
 */
bool PRunListCollection::Add(int runNo)
{
  bool success = true;

  PMsrRunStructure *runList = &(*fMsrInfo->GetMsrRunList())[runNo];

  cout << endl << "PRunListCollection::Add(): will add run no " << runNo;
  cout << ", name = " << runList->fRunName.Data();
  cout << ", type = " << runList->fFitType;

  int fitType = (*fMsrInfo->GetMsrRunList())[runNo].fFitType;

PRunAsymmetry* asym;
bool status;

  switch (fitType) {
    case PRUN_SINGLE_HISTO:
      fRunSingleHistoList.push_back(new PRunSingleHisto(fMsrInfo, fData, runNo));
      if (!fRunSingleHistoList[fRunSingleHistoList.size()-1]->IsValid())
        success = false;
      break;
    case PRUN_ASYMMETRY:
      fRunAsymmetryList.push_back(new PRunAsymmetry(fMsrInfo, fData, runNo));
      asym = fRunAsymmetryList[fRunAsymmetryList.size()-1];
      status = asym->IsValid();
      if (!fRunAsymmetryList[fRunAsymmetryList.size()-1]->IsValid())
        success = false;
      break;
    case PRUN_RRF:
      fRunRRFList.push_back(new PRunRRF(fMsrInfo, fData, runNo));
      if (!fRunRRFList[fRunRRFList.size()-1]->IsValid())
        success = false;
      break;
    case PRUN_NON_MUSR:
      fRunNonMusrList.push_back(new PRunNonMusr(fMsrInfo, fData, runNo));
      if (!fRunNonMusrList[fRunNonMusrList.size()-1]->IsValid())
        success = false;
      break;
    default:
      success = false;
      break;
  }

  return success;
}

//--------------------------------------------------------------------------
// GetSingleHistoChisq
//--------------------------------------------------------------------------
/**
 * <p>
 */
double PRunListCollection::GetSingleHistoChisq(const std::vector<double>& par)
{
  double chisq = 0.0;

  for (unsigned int i=0; i<fRunSingleHistoList.size(); i++)
    chisq += fRunSingleHistoList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetAsymmetryChisq
//--------------------------------------------------------------------------
/**
 * <p>
 */
double PRunListCollection::GetAsymmetryChisq(const std::vector<double>& par)
{
  double chisq = 0.0;

  for (unsigned int i=0; i<fRunAsymmetryList.size(); i++)
    chisq += fRunAsymmetryList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetRRFChisq
//--------------------------------------------------------------------------
/**
 * <p>
 */
double PRunListCollection::GetRRFChisq(const std::vector<double>& par)
{
  double chisq = 0.0;

  for (unsigned int i=0; i<fRunRRFList.size(); i++)
    chisq += fRunRRFList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetNonMusrChisq
//--------------------------------------------------------------------------
/**
 * <p>
 */
double PRunListCollection::GetNonMusrChisq(const std::vector<double>& par)
{
  double chisq = 0.0;

  for (unsigned int i=0; i<fRunNonMusrList.size(); i++)
    chisq += fRunNonMusrList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetSingleHistoMaximumLikelihood
//--------------------------------------------------------------------------
/**
 * <p>
 */
double PRunListCollection::GetSingleHistoMaximumLikelihood(const std::vector<double>& par)
{
  double mlh = 0.0;

  for (unsigned int i=0; i<fRunSingleHistoList.size(); i++)
    mlh += fRunSingleHistoList[i]->CalcMaxLikelihood(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetAsymmetryMaximumLikelihood
//--------------------------------------------------------------------------
/**
 * <p>
 */
double PRunListCollection::GetAsymmetryMaximumLikelihood(const std::vector<double>& par)
{
  double mlh = 0.0;

  for (unsigned int i=0; i<fRunAsymmetryList.size(); i++)
    mlh += fRunAsymmetryList[i]->CalcMaxLikelihood(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetRRFMaximumLikelihood
//--------------------------------------------------------------------------
/**
 * <p>
 */
double PRunListCollection::GetRRFMaximumLikelihood(const std::vector<double>& par)
{
  double mlh = 0.0;

  for (unsigned int i=0; i<fRunRRFList.size(); i++)
    mlh += fRunRRFList[i]->CalcMaxLikelihood(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetNonMusrMaximumLikelihood
//--------------------------------------------------------------------------
/**
 * <p>
 */
double PRunListCollection::GetNonMusrMaximumLikelihood(const std::vector<double>& par)
{
  double mlh = 0.0;

  for (unsigned int i=0; i<fRunNonMusrList.size(); i++)
    mlh += fRunNonMusrList[i]->CalcMaxLikelihood(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetTotalNoOfBinsFitted
//--------------------------------------------------------------------------
/**
 * <p>
 */
unsigned int PRunListCollection::GetTotalNoOfBinsFitted()
{
  unsigned int counts = 0;

  for (unsigned int i=0; i<fRunSingleHistoList.size(); i++)
    counts += fRunSingleHistoList[i]->GetNoOfFitBins();

  for (unsigned int i=0; i<fRunAsymmetryList.size(); i++)
    counts += fRunAsymmetryList[i]->GetNoOfFitBins();

  for (unsigned int i=0; i<fRunRRFList.size(); i++)
    counts += fRunRRFList[i]->GetNoOfFitBins();

  for (unsigned int i=0; i<fRunNonMusrList.size(); i++)
    counts += fRunNonMusrList[i]->GetNoOfFitBins();

// cout << endl << "Total No of Bins Fitted = " << counts;
  return counts;
}

//--------------------------------------------------------------------------
// GetSingleHisto
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param index
 */
PRunData* PRunListCollection::GetSingleHisto(unsigned int index)
{
  if ((index < 0) || (index > fRunSingleHistoList.size())) {
    cout << endl << "PRunListCollection::GetSingleHisto: index = " << index << " out of bounds";
    return 0;
  }

  fRunSingleHistoList[index]->CalcTheory();
  PRunData *data = fRunSingleHistoList[index]->GetData();

  return data;
}

//--------------------------------------------------------------------------
// GetAsymmetry
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param index
 */
PRunData* PRunListCollection::GetAsymmetry(unsigned int index)
{
  if ((index < 0) || (index > fRunAsymmetryList.size())) {
    cout << endl << "PRunListCollection::GetAsymmetry: index = " << index << " out of bounds";
    return 0;
  }

  fRunAsymmetryList[index]->CalcTheory();
  PRunData *data = fRunAsymmetryList[index]->GetData();

  return data;
}

//--------------------------------------------------------------------------
// GetRRF
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param index
 */
PRunData* PRunListCollection::GetRRF(unsigned int index)
{
  if ((index < 0) || (index > fRunRRFList.size())) {
    cout << endl << "PRunListCollection::GetRRF: index = " << index << " out of bounds";
    return 0;
  }

  return 0;
}

//--------------------------------------------------------------------------
// GetNonMusr
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param index
 */
PRunData* PRunListCollection::GetNonMusr(unsigned int index)
{
  if ((index < 0) || (index > fRunNonMusrList.size())) {
    cout << endl << "PRunListCollection::GetNonMusr: index = " << index << " out of bounds";
    return 0;
  }

  return 0;
}
