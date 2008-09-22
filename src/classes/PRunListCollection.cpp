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
 * \param data
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
 * \param tag
 */
bool PRunListCollection::Add(int runNo, EPMusrHandleTag tag)
{
  bool success = true;

//  PMsrRunStructure *runList = &(*fMsrInfo->GetMsrRunList())[runNo];

//   cout << endl << "PRunListCollection::Add(): will add run no " << runNo;
//   cout << ", name = " << runList->fRunName.Data();
//   cout << ", type = " << runList->fFitType;

  int fitType = (*fMsrInfo->GetMsrRunList())[runNo].fFitType;

  switch (fitType) {
    case PRUN_SINGLE_HISTO:
      fRunSingleHistoList.push_back(new PRunSingleHisto(fMsrInfo, fData, runNo, tag));
      if (!fRunSingleHistoList[fRunSingleHistoList.size()-1]->IsValid())
        success = false;
      break;
    case PRUN_ASYMMETRY:
      fRunAsymmetryList.push_back(new PRunAsymmetry(fMsrInfo, fData, runNo, tag));
      if (!fRunAsymmetryList[fRunAsymmetryList.size()-1]->IsValid())
        success = false;
      break;
    case PRUN_RRF:
      fRunRRFList.push_back(new PRunRRF(fMsrInfo, fData, runNo, tag));
      if (!fRunRRFList[fRunRRFList.size()-1]->IsValid())
        success = false;
      break;
    case PRUN_NON_MUSR:
      fRunNonMusrList.push_back(new PRunNonMusr(fMsrInfo, fData, runNo, tag));
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
 * <p> Since it is not clear yet how to handle asymmetry fits with max likelihood
 * the chi square will be used!
 */
double PRunListCollection::GetAsymmetryMaximumLikelihood(const std::vector<double>& par)
{
  double mlh = 0.0;

  for (unsigned int i=0; i<fRunAsymmetryList.size(); i++)
    mlh += fRunAsymmetryList[i]->CalcChiSquare(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetRRFMaximumLikelihood
//--------------------------------------------------------------------------
/**
 * <p> Since it is not clear yet how to handle RRF fits with max likelihood
 * the chi square will be used!
 */
double PRunListCollection::GetRRFMaximumLikelihood(const std::vector<double>& par)
{
  double mlh = 0.0;

  for (unsigned int i=0; i<fRunRRFList.size(); i++)
    mlh += fRunRRFList[i]->CalcChiSquare(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetNonMusrMaximumLikelihood
//--------------------------------------------------------------------------
/**
 * <p> Since it is not clear yet how to handle non musr fits with max likelihood
 * the chi square will be used!
 */
double PRunListCollection::GetNonMusrMaximumLikelihood(const std::vector<double>& par)
{
  double mlh = 0.0;

  for (unsigned int i=0; i<fRunNonMusrList.size(); i++)
    mlh += fRunNonMusrList[i]->CalcChiSquare(par);

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
 * \param tag kIndex -> data at index, kRunNo -> data of given run no
 */
PRunData* PRunListCollection::GetSingleHisto(unsigned int index, EDataSwitch tag)
{
  PRunData *data = 0;

  switch (tag) {
    case kIndex:
      if ((index < 0) || (index > fRunSingleHistoList.size())) {
        cout << endl << "PRunListCollection::GetSingleHisto: index = " << index << " out of bounds";
        return 0;
      }

      fRunSingleHistoList[index]->CalcTheory();
      data = fRunSingleHistoList[index]->GetData();
      break;
    case kRunNo:
      for (unsigned int i=0; i<fRunSingleHistoList.size(); i++) {
        if (fRunSingleHistoList[i]->GetRunNo() == index) {
          data = fRunSingleHistoList[i]->GetData();
          break;
        }
      }
      break;
    default: // error
      break;
  }

  return data;
}

//--------------------------------------------------------------------------
// GetAsymmetry
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param index
 * \param tag kIndex -> data at index, kRunNo -> data of given run no
 */
PRunData* PRunListCollection::GetAsymmetry(unsigned int index, EDataSwitch tag)
{
  PRunData *data = 0;

  switch (tag) {
    case kIndex:
      if ((index < 0) || (index > fRunAsymmetryList.size())) {
        cout << endl << "PRunListCollection::GetAsymmetry: index = " << index << " out of bounds";
        return 0;
      }

      fRunAsymmetryList[index]->CalcTheory();
      data = fRunAsymmetryList[index]->GetData();
      break;
    case kRunNo:
      for (unsigned int i=0; i<fRunAsymmetryList.size(); i++) {
        if (fRunAsymmetryList[i]->GetRunNo() == index) {
          data = fRunAsymmetryList[i]->GetData();
          break;
        }
      }
      break;
    default: // error
      break;
  }

  return data;
}

//--------------------------------------------------------------------------
// GetRRF
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param index
 * \param tag kIndex -> data at index, kRunNo -> data of given run no
 */
PRunData* PRunListCollection::GetRRF(unsigned int index, EDataSwitch tag)
{
  PRunData *data = 0;

  switch (tag) {
    case kIndex:
      if ((index < 0) || (index > fRunRRFList.size())) {
        cout << endl << "PRunListCollection::GetRRF: index = " << index << " out of bounds";
        return 0;
      }
      break;
    case kRunNo:
      break;
    default: // error
      break;
  }

  return data;
}

//--------------------------------------------------------------------------
// GetNonMusr
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param index
 * \param tag kIndex -> data at index, kRunNo -> data of given run no
 */
PRunData* PRunListCollection::GetNonMusr(unsigned int index, EDataSwitch tag)
{
  PRunData *data = 0;

  switch (tag) {
    case kIndex:
      if ((index < 0) || (index > fRunNonMusrList.size())) {
        cout << endl << "PRunListCollection::GetNonMusr: index = " << index << " out of bounds";
        return 0;
      }
      break;
    case kRunNo:
      for (unsigned int i=0; i<fRunNonMusrList.size(); i++) {
        if (fRunNonMusrList[i]->GetRunNo() == index) {
          data = fRunNonMusrList[i]->GetData();
          break;
        }
      }
      break;
    default: // error
      break;
  }

  return data;
}

//--------------------------------------------------------------------------
// GetTemp
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runName
 */
double PRunListCollection::GetTemp(TString &runName)
{
  return fData->GetRunData(runName)->fTemp;
}

//--------------------------------------------------------------------------
// GetField
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runName
 */
double PRunListCollection::GetField(TString &runName)
{
  return fData->GetRunData(runName)->fField;
}

//--------------------------------------------------------------------------
// GetEnergy
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runName
 */
double PRunListCollection::GetEnergy(TString &runName)
{
  return fData->GetRunData(runName)->fEnergy;
}

//--------------------------------------------------------------------------
// GetSetup
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runName
 */
const char* PRunListCollection::GetSetup(TString &runName)
{
  return fData->GetRunData(runName)->fSetup.Data();
}

//--------------------------------------------------------------------------
// GetXAxisTitle
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runName
 */
const char* PRunListCollection::GetXAxisTitle(TString &runName)
{
  PRawRunData *runData = fData->GetRunData(runName);
  int index = fData->GetRunData(runName)->fDataNonMusr.fXIndex;

// cout << endl << ">>PRunListCollection::GetXAxisTitle, x-index = " << index;
// cout << endl;

  return runData->fDataNonMusr.fLabels[index].Data();
}

//--------------------------------------------------------------------------
// GetYAxisTitle
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runName
 */
const char* PRunListCollection::GetYAxisTitle(TString &runName)
{
  PRawRunData *runData = fData->GetRunData(runName);
  int index = fData->GetRunData(runName)->fDataNonMusr.fYIndex;

// cout << endl << ">>PRunListCollection::GetYAxisTitle, y-index = " << index;
// cout << endl;

  return runData->fDataNonMusr.fLabels[index].Data();
}

