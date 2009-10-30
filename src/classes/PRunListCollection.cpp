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
PRunListCollection::PRunListCollection(PMsrHandler *msrInfo, PRunDataHandler *data) \
 : fMsrInfo(msrInfo), fData(data)
{
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
  for (UInt_t i=0; i<fRunSingleHistoList.size(); i++) {
    fRunSingleHistoList[i]->CleanUp();
    fRunSingleHistoList[i]->~PRunSingleHisto();
  }
  fRunSingleHistoList.clear();

//cout << endl << ">> fRunAsymmetryList.size() = " << fRunAsymmetryList.size();
  for (UInt_t i=0; i<fRunAsymmetryList.size(); i++) {
    fRunAsymmetryList[i]->CleanUp();
    fRunAsymmetryList[i]->~PRunAsymmetry();
  }
  fRunAsymmetryList.clear();

//cout << endl << ">> fRunRRFList.size() = " << fRunRRFList.size();
  for (UInt_t i=0; i<fRunRRFList.size(); i++) {
    fRunRRFList[i]->CleanUp();
    fRunRRFList[i]->~PRunRRF();
  }
  fRunRRFList.clear();

//cout << endl << ">> fRunNonMusrList.size() = " << fRunNonMusrList.size();
  for (UInt_t i=0; i<fRunNonMusrList.size(); i++) {
    fRunNonMusrList[i]->CleanUp();
    fRunNonMusrList[i]->~PRunNonMusr();
  }
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
Bool_t PRunListCollection::Add(Int_t runNo, EPMusrHandleTag tag)
{
  Bool_t success = true;

//  PMsrRunStructure *runList = &(*fMsrInfo->GetMsrRunList())[runNo];

//   cout << endl << "PRunListCollection::Add(): will add run no " << runNo;
//   cout << ", name = " << runList->fRunName.Data();
//   cout << ", type = " << runList->fFitType;

  Int_t fitType = (*fMsrInfo->GetMsrRunList())[runNo].fFitType;

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
Double_t PRunListCollection::GetSingleHistoChisq(const std::vector<Double_t>& par) const
{
  Double_t chisq = 0.0;

  for (UInt_t i=0; i<fRunSingleHistoList.size(); i++)
    chisq += fRunSingleHistoList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetAsymmetryChisq
//--------------------------------------------------------------------------
/**
 * <p>
 */
Double_t PRunListCollection::GetAsymmetryChisq(const std::vector<Double_t>& par) const
{
  Double_t chisq = 0.0;

  for (UInt_t i=0; i<fRunAsymmetryList.size(); i++)
    chisq += fRunAsymmetryList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetRRFChisq
//--------------------------------------------------------------------------
/**
 * <p>
 */
Double_t PRunListCollection::GetRRFChisq(const std::vector<Double_t>& par) const
{
  Double_t chisq = 0.0;

  for (UInt_t i=0; i<fRunRRFList.size(); i++)
    chisq += fRunRRFList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetNonMusrChisq
//--------------------------------------------------------------------------
/**
 * <p>
 */
Double_t PRunListCollection::GetNonMusrChisq(const std::vector<Double_t>& par) const
{
  Double_t chisq = 0.0;

  for (UInt_t i=0; i<fRunNonMusrList.size(); i++)
    chisq += fRunNonMusrList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetSingleHistoMaximumLikelihood
//--------------------------------------------------------------------------
/**
 * <p>
 */
Double_t PRunListCollection::GetSingleHistoMaximumLikelihood(const std::vector<Double_t>& par) const
{
  Double_t mlh = 0.0;

  for (UInt_t i=0; i<fRunSingleHistoList.size(); i++)
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
Double_t PRunListCollection::GetAsymmetryMaximumLikelihood(const std::vector<Double_t>& par) const
{
  Double_t mlh = 0.0;

  for (UInt_t i=0; i<fRunAsymmetryList.size(); i++)
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
Double_t PRunListCollection::GetRRFMaximumLikelihood(const std::vector<Double_t>& par) const
{
  Double_t mlh = 0.0;

  for (UInt_t i=0; i<fRunRRFList.size(); i++)
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
Double_t PRunListCollection::GetNonMusrMaximumLikelihood(const std::vector<Double_t>& par) const
{
  Double_t mlh = 0.0;

  for (UInt_t i=0; i<fRunNonMusrList.size(); i++)
    mlh += fRunNonMusrList[i]->CalcChiSquare(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetTotalNoOfBinsFitted
//--------------------------------------------------------------------------
/**
 * <p>
 */
UInt_t PRunListCollection::GetTotalNoOfBinsFitted() const
{
  UInt_t counts = 0;

  for (UInt_t i=0; i<fRunSingleHistoList.size(); i++)
    counts += fRunSingleHistoList[i]->GetNoOfFitBins();

  for (UInt_t i=0; i<fRunAsymmetryList.size(); i++)
    counts += fRunAsymmetryList[i]->GetNoOfFitBins();

  for (UInt_t i=0; i<fRunRRFList.size(); i++)
    counts += fRunRRFList[i]->GetNoOfFitBins();

  for (UInt_t i=0; i<fRunNonMusrList.size(); i++)
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
PRunData* PRunListCollection::GetSingleHisto(UInt_t index, EDataSwitch tag)
{
  PRunData *data = 0;

  switch (tag) {
    case kIndex:
      if ((index < 0) || (index > fRunSingleHistoList.size())) {
        cerr << endl << "PRunListCollection::GetSingleHisto: **ERROR** index = " << index << " out of bounds";
        cerr << endl;
        return 0;
      }

      fRunSingleHistoList[index]->CalcTheory();
      data = fRunSingleHistoList[index]->GetData();
      break;
    case kRunNo:
      for (UInt_t i=0; i<fRunSingleHistoList.size(); i++) {
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
PRunData* PRunListCollection::GetAsymmetry(UInt_t index, EDataSwitch tag)
{
  PRunData *data = 0;

  switch (tag) {
    case kIndex: // called from musrfit when dumping the data
      if ((index < 0) || (index > fRunAsymmetryList.size())) {
        cerr << endl << "PRunListCollection::GetAsymmetry: **ERROR** index = " << index << " out of bounds";
        cerr << endl;
        return 0;
      }

      fRunAsymmetryList[index]->CalcTheory();
      data = fRunAsymmetryList[index]->GetData();
      break;
    case kRunNo: // called from PMusrCanvas
      for (UInt_t i=0; i<fRunAsymmetryList.size(); i++) {
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
PRunData* PRunListCollection::GetRRF(UInt_t index, EDataSwitch tag)
{
  PRunData *data = 0;

  switch (tag) {
    case kIndex:
      if ((index < 0) || (index > fRunRRFList.size())) {
        cerr << endl << "PRunListCollection::GetRRF: **ERROR** index = " << index << " out of bounds";
        cerr << endl;
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
PRunData* PRunListCollection::GetNonMusr(UInt_t index, EDataSwitch tag)
{
  PRunData *data = 0;

  switch (tag) {
    case kIndex:
      if ((index < 0) || (index > fRunNonMusrList.size())) {
        cerr << endl << "PRunListCollection::GetNonMusr: **ERROR** index = " << index << " out of bounds";
        cerr << endl;
        return 0;
      }
      break;
    case kRunNo:
      for (UInt_t i=0; i<fRunNonMusrList.size(); i++) {
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
PDoublePairVector PRunListCollection::GetTemp(const TString &runName) const
{
  return fData->GetRunData(runName)->GetTemperature();
}

//--------------------------------------------------------------------------
// GetField
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runName
 */
Double_t PRunListCollection::GetField(const TString &runName) const
{
  return fData->GetRunData(runName)->GetField();
}

//--------------------------------------------------------------------------
// GetEnergy
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runName
 */
Double_t PRunListCollection::GetEnergy(const TString &runName) const
{
  return fData->GetRunData(runName)->GetEnergy();
}

//--------------------------------------------------------------------------
// GetSetup
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runName
 */
const Char_t* PRunListCollection::GetSetup(const TString &runName) const
{
  return fData->GetRunData(runName)->GetSetup()->Data();
}

//--------------------------------------------------------------------------
// GetXAxisTitle
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runName name of the run file
 * \param idx msr-file run index
 */
const Char_t* PRunListCollection::GetXAxisTitle(const TString &runName, const UInt_t idx) const
{
//cout << endl << ">> PRunListCollection::GetXAxisTitle: runName = " << runName.Data() << ", idx = " << idx;
//cout << endl << ">> PRunListCollection::GetXAxisTitle: fRunNonMusrList.size() = " << fRunNonMusrList.size();
//cout << endl;

  PRawRunData *runData = fData->GetRunData(runName);

  const Char_t *result = 0;

  if (runData->fDataNonMusr.FromAscii()) {
    result = runData->fDataNonMusr.GetLabels()->at(0).Data();
  } else {
    for (UInt_t i=0; i<fRunNonMusrList.size(); i++) {
      if (fRunNonMusrList[i]->GetRunNo() == idx) {
        Int_t index = fRunNonMusrList[i]->GetXIndex();
        result = runData->fDataNonMusr.GetLabels()->at(index).Data();
        break;
      }
    }
  }

  return result;
}

//--------------------------------------------------------------------------
// GetYAxisTitle
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runName name of the run file
 * \param idx msr-file run index
 */
const Char_t* PRunListCollection::GetYAxisTitle(const TString &runName, const UInt_t idx) const
{
//cout << endl << ">> PRunListCollection::GetYAxisTitle: runName = " << runName.Data() << ", idx = " << idx;
//cout << endl;

  PRawRunData *runData = fData->GetRunData(runName);

  const Char_t *result = 0;

  if (runData->fDataNonMusr.FromAscii()) {
    result = runData->fDataNonMusr.GetLabels()->at(1).Data();
  } else {
    for (UInt_t i=0; i<fRunNonMusrList.size(); i++) {
      if (fRunNonMusrList[i]->GetRunNo() == idx) {
        Int_t index = fRunNonMusrList[i]->GetYIndex();
        result = runData->fDataNonMusr.GetLabels()->at(index).Data();
        break;
      }
    }
  }

  return result;
}

