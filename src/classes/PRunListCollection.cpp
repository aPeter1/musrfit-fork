/***************************************************************************

  PRunListCollection.cpp

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

#include <iostream>

#include "PRunListCollection.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param msrInfo pointer to the msr-file handler
 * \param data pointer to the run-data handler
 */
PRunListCollection::PRunListCollection(PMsrHandler *msrInfo, PRunDataHandler *data, Bool_t theoAsData) :
  fMsrInfo(msrInfo), fData(data), fTheoAsData(theoAsData)
{
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor
 */
PRunListCollection::~PRunListCollection()
{
  for (UInt_t i=0; i<fRunSingleHistoList.size(); i++) {
    fRunSingleHistoList[i]->CleanUp();
    fRunSingleHistoList[i]->~PRunSingleHisto();
  }
  fRunSingleHistoList.clear();

  for (UInt_t i=0; i<fRunSingleHistoRRFList.size(); i++) {
    fRunSingleHistoRRFList[i]->CleanUp();
    fRunSingleHistoRRFList[i]->~PRunSingleHistoRRF();
  }
  fRunSingleHistoRRFList.clear();

  for (UInt_t i=0; i<fRunAsymmetryList.size(); i++) {
    fRunAsymmetryList[i]->CleanUp();
    fRunAsymmetryList[i]->~PRunAsymmetry();
  }
  fRunAsymmetryList.clear();

  for (UInt_t i=0; i<fRunAsymmetryRRFList.size(); i++) {
    fRunAsymmetryRRFList[i]->CleanUp();
    fRunAsymmetryRRFList[i]->~PRunAsymmetryRRF();
  }
  fRunAsymmetryRRFList.clear();

  for (UInt_t i=0; i<fRunAsymmetryBNMRList.size(); i++) {
    fRunAsymmetryBNMRList[i]->CleanUp();
    fRunAsymmetryBNMRList[i]->~PRunAsymmetryBNMR();
  }
  fRunAsymmetryBNMRList.clear();

  for (UInt_t i=0; i<fRunMuMinusList.size(); i++) {
    fRunMuMinusList[i]->CleanUp();
    fRunMuMinusList[i]->~PRunMuMinus();
  }
  fRunMuMinusList.clear();

  for (UInt_t i=0; i<fRunNonMusrList.size(); i++) {
    fRunNonMusrList[i]->CleanUp();
    fRunNonMusrList[i]->~PRunNonMusr();
  }
  fRunNonMusrList.clear();
}

//--------------------------------------------------------------------------
// Add (public)
//--------------------------------------------------------------------------
/**
 * <p>Adds a processed set of data to the handler.
 *
 * <b>return:</b>
 * - true if a processed data set could be added successfully
 * - false otherwise
 *
 * \param runNo msr-file run number
 * \param tag tag showing what shall be done: kFit == fitting, kView == viewing
 */
Bool_t PRunListCollection::Add(Int_t runNo, EPMusrHandleTag tag)
{
  Bool_t success = true;

  // try to get the fit type from the RUN block
  Int_t fitType = (*fMsrInfo->GetMsrRunList())[runNo].GetFitType();
  if (fitType == -1) { // fit type NOT given in the RUN block, check the GLOBAL block
    fitType = (*fMsrInfo->GetMsrGlobal()).GetFitType();
  }

  switch (fitType) {
    case PRUN_SINGLE_HISTO:
      fRunSingleHistoList.push_back(new PRunSingleHisto(fMsrInfo, fData, runNo, tag, fTheoAsData));
      if (!fRunSingleHistoList[fRunSingleHistoList.size()-1]->IsValid())
        success = false;
      break;
    case PRUN_SINGLE_HISTO_RRF:
      fRunSingleHistoRRFList.push_back(new PRunSingleHistoRRF(fMsrInfo, fData, runNo, tag, fTheoAsData));
      if (!fRunSingleHistoRRFList[fRunSingleHistoRRFList.size()-1]->IsValid())
        success = false;
      break;
    case PRUN_ASYMMETRY:
      fRunAsymmetryList.push_back(new PRunAsymmetry(fMsrInfo, fData, runNo, tag, fTheoAsData));
      if (!fRunAsymmetryList[fRunAsymmetryList.size()-1]->IsValid())
        success = false;
      break;
    case PRUN_ASYMMETRY_RRF:
      fRunAsymmetryRRFList.push_back(new PRunAsymmetryRRF(fMsrInfo, fData, runNo, tag, fTheoAsData));
      if (!fRunAsymmetryRRFList[fRunAsymmetryRRFList.size()-1]->IsValid())
        success = false;
      break;
    case PRUN_ASYMMETRY_BNMR:
      fRunAsymmetryBNMRList.push_back(new PRunAsymmetryBNMR(fMsrInfo, fData, runNo, tag, fTheoAsData));
      if (!fRunAsymmetryBNMRList[fRunAsymmetryBNMRList.size()-1]->IsValid())
        success = false;
      break;
    case PRUN_MU_MINUS:
      fRunMuMinusList.push_back(new PRunMuMinus(fMsrInfo, fData, runNo, tag, fTheoAsData));
      if (!fRunMuMinusList[fRunMuMinusList.size()-1]->IsValid())
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
// SetFitRange (public)
//--------------------------------------------------------------------------
/**
 * <p>Set the current fit range in bins. The string has the structure:
 * 'fit_range fgb0+n00 lgb0-n01 [fgb1+n10 lgb-n11 fgb2+n20 lgb2-n21 .. fgbN+nN0 lgbN-nN1]'
 * where fgb is the first good bin, lgb is the last good bin. nXY are offsets in bins.
 * N is the number of runs in the msr-file.
 *
 * <p>This means there are 2 options: (i) a globle fit range in bins for <em>all</em> runs in the
 * msr-file, or (ii) each run block in the msr-file needs its individual range.
 *
 * \param fitRange string holding the fit range(s).
 */
void PRunListCollection::SetFitRange(const TString fitRange)
{
  for (UInt_t i=0; i<fRunSingleHistoList.size(); i++)
    fRunSingleHistoList[i]->SetFitRangeBin(fitRange);
  for (UInt_t i=0; i<fRunSingleHistoRRFList.size(); i++)
    fRunSingleHistoRRFList[i]->SetFitRangeBin(fitRange);
  for (UInt_t i=0; i<fRunAsymmetryList.size(); i++)
    fRunAsymmetryList[i]->SetFitRangeBin(fitRange);
  for (UInt_t i=0; i<fRunAsymmetryRRFList.size(); i++)
    fRunAsymmetryRRFList[i]->SetFitRangeBin(fitRange);
  for (UInt_t i=0; i<fRunAsymmetryBNMRList.size(); i++)
    fRunAsymmetryBNMRList[i]->SetFitRangeBin(fitRange);
  for (UInt_t i=0; i<fRunMuMinusList.size(); i++)
    fRunMuMinusList[i]->SetFitRangeBin(fitRange);
  for (UInt_t i=0; i<fRunNonMusrList.size(); i++)
    fRunNonMusrList[i]->SetFitRangeBin(fitRange);
}

//--------------------------------------------------------------------------
// SetFitRange (public)
//--------------------------------------------------------------------------
/**
 * <p>Set the current fit range in time. If fitRange.size()==1 the given fit range will be used for all the runs,
 * otherwise fitRange.size()==the number of runs in the msr-file, and for each run there will be an individual
 * fit range.
 *
 * \param fitRange vector holding the fit range(s).
 */
void PRunListCollection::SetFitRange(const PDoublePairVector fitRange)
{
  for (UInt_t i=0; i<fRunSingleHistoList.size(); i++) {
    fRunSingleHistoList[i]->SetFitRange(fitRange);
    fRunSingleHistoList[i]->CalcNoOfFitBins(); // needed to update fStartTimeBin, fEndTimeBin
  }
  for (UInt_t i=0; i<fRunSingleHistoRRFList.size(); i++) {
    fRunSingleHistoRRFList[i]->SetFitRange(fitRange);
    fRunSingleHistoRRFList[i]->CalcNoOfFitBins(); // needed to update fStartTimeBin, fEndTimeBin
  }
  for (UInt_t i=0; i<fRunAsymmetryList.size(); i++) {
    fRunAsymmetryList[i]->SetFitRange(fitRange);
    fRunAsymmetryList[i]->CalcNoOfFitBins(); // needed to update fStartTimeBin, fEndTimeBin
  }
  for (UInt_t i=0; i<fRunAsymmetryRRFList.size(); i++) {
    fRunAsymmetryRRFList[i]->SetFitRange(fitRange);
    fRunAsymmetryRRFList[i]->CalcNoOfFitBins(); // needed to update fStartTimeBin, fEndTimeBin
  }
  for (UInt_t i=0; i<fRunAsymmetryBNMRList.size(); i++) {
    fRunAsymmetryBNMRList[i]->SetFitRange(fitRange);
    fRunAsymmetryBNMRList[i]->CalcNoOfFitBins(); // needed to update fStartTimeBin, fEndTimeBin
  }
  for (UInt_t i=0; i<fRunMuMinusList.size(); i++) {
    fRunMuMinusList[i]->SetFitRange(fitRange);
    fRunMuMinusList[i]->CalcNoOfFitBins(); // needed to update fStartTimeBin, fEndTimeBin
  }
  for (UInt_t i=0; i<fRunNonMusrList.size(); i++)
    fRunNonMusrList[i]->SetFitRange(fitRange);
}

//--------------------------------------------------------------------------
// GetSingleHistoChisq (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates chi-square of <em>all</em> single histogram runs of a msr-file.
 *
 * <b>return:</b>
 * - chi-square of all single histogram runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetSingleHistoChisq(const std::vector<Double_t>& par) const
{
  Double_t chisq = 0.0;

  for (UInt_t i=0; i<fRunSingleHistoList.size(); i++)
    chisq += fRunSingleHistoList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetSingleHistoRRFChisq (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates chi-square of <em>all</em> single histogram RRF runs of a msr-file.
 *
 * <b>return:</b>
 * - chi-square of all single histogram RRF runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetSingleHistoRRFChisq(const std::vector<Double_t>& par) const
{
  Double_t chisq = 0.0;

  for (UInt_t i=0; i<fRunSingleHistoRRFList.size(); i++)
    chisq += fRunSingleHistoRRFList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetAsymmetryChisq (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates chi-square of <em>all</em> asymmetry runs of a msr-file.
 *
 * <b>return:</b>
 * - chi-square of all asymmetry runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetAsymmetryChisq(const std::vector<Double_t>& par) const
{
  Double_t chisq = 0.0;

  for (UInt_t i=0; i<fRunAsymmetryList.size(); i++)
    chisq += fRunAsymmetryList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetAsymmetryRRFChisq (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates chi-square of <em>all</em> asymmetry RRF runs of a msr-file.
 *
 * <b>return:</b>
 * - chi-square of all asymmetry RRF runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetAsymmetryRRFChisq(const std::vector<Double_t>& par) const
{
  Double_t chisq = 0.0;

  for (UInt_t i=0; i<fRunAsymmetryRRFList.size(); i++)
    chisq += fRunAsymmetryRRFList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetAsymmetryBNMRChisq (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates chi-square of <em>all</em> asymmetry BNMR runs of a msr-file.
 *
 * <b>return:</b>
 * - chi-square of all asymmetry BNMR runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetAsymmetryBNMRChisq(const std::vector<Double_t>& par) const
{
  Double_t chisq = 0.0;

  for (UInt_t i=0; i<fRunAsymmetryBNMRList.size(); i++)
    chisq += fRunAsymmetryBNMRList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetMuMinusChisq (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates chi-square of <em>all</em> mu minus runs of a msr-file.
 *
 * <b>return:</b>
 * - chi-square of all mu minus runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetMuMinusChisq(const std::vector<Double_t>& par) const
{
  Double_t chisq = 0.0;

  for (UInt_t i=0; i<fRunMuMinusList.size(); i++)
    chisq += fRunMuMinusList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetNonMusrChisq (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates chi-square of <em>all</em> non-muSR runs of a msr-file.
 *
 * <b>return:</b>
 * - chi-square of all non-muSR runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetNonMusrChisq(const std::vector<Double_t>& par) const
{
  Double_t chisq = 0.0;

  for (UInt_t i=0; i<fRunNonMusrList.size(); i++)
    chisq += fRunNonMusrList[i]->CalcChiSquare(par);

  return chisq;
}

//--------------------------------------------------------------------------
// GetSingleRunChisqExpected (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates expected chi-square of the run block index idx of a msr-file.
 *
 * <b>return:</b>
 * - expected chi-square of for a single run block
 *
 * \param par fit parameter vector
 * \param idx run block index
 */
Double_t PRunListCollection::GetSingleRunChisqExpected(const std::vector<Double_t>& par, const UInt_t idx) const
{
  Double_t expectedChisq = 0.0;

  if (idx > fMsrInfo->GetMsrRunList()->size()) {
    std::cerr << ">> PRunListCollection::GetSingleRunChisqExpected() **ERROR** idx=" << idx << " is out of range [0.." << fMsrInfo->GetMsrRunList()->size() << "[" << std::endl << std::endl;
    return expectedChisq;
  }

  UInt_t subIdx = 0;
  Int_t type = fMsrInfo->GetMsrRunList()->at(idx).GetFitType();
  if (type == -1) { // i.e. not found in the RUN block, try the GLOBAL block
    type = fMsrInfo->GetMsrGlobal()->GetFitType();
    subIdx = idx;
  } else { // found in the RUN block
    // count how many entries of this fit-type are present up to idx
    for (UInt_t i=0; i<idx; i++) {
      if (fMsrInfo->GetMsrRunList()->at(i).GetFitType() == type)
        subIdx++;
    }
  }


  // return the chisq of the single run
  switch (type) {
  case PRUN_SINGLE_HISTO:
    expectedChisq = fRunSingleHistoList[subIdx]->CalcChiSquareExpected(par);
    break;
  case PRUN_SINGLE_HISTO_RRF:
    expectedChisq = fRunSingleHistoRRFList[subIdx]->CalcChiSquareExpected(par);
    break;
  case PRUN_ASYMMETRY:
    expectedChisq = fRunAsymmetryList[subIdx]->CalcChiSquareExpected(par);
    break;
  case PRUN_ASYMMETRY_RRF:
    expectedChisq = fRunAsymmetryRRFList[subIdx]->CalcChiSquareExpected(par);
    break;
  case PRUN_ASYMMETRY_BNMR:
    expectedChisq = fRunAsymmetryBNMRList[subIdx]->CalcChiSquareExpected(par);
    break;
  case PRUN_MU_MINUS:
    expectedChisq = fRunMuMinusList[subIdx]->CalcChiSquareExpected(par);
    break;
  case PRUN_NON_MUSR:
    expectedChisq = fRunNonMusrList[subIdx]->CalcChiSquareExpected(par);
    break;
  default:
    break;
  }

  return expectedChisq;
}

//--------------------------------------------------------------------------
// GetSingleRunChisq (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates chi-square of a single run-block entry of the msr-file.
 *
 * <b>return:</b>
 * - chi-square of single run-block entry with index idx
 *
 * \param par fit parameter vector
 * \param idx run block index
 */
Double_t PRunListCollection::GetSingleRunChisq(const std::vector<Double_t>& par, const UInt_t idx) const
{
  Double_t chisq = 0.0;

  if (idx > fMsrInfo->GetMsrRunList()->size()) {
    std::cerr << ">> PRunListCollection::GetSingleRunChisq() **ERROR** idx=" << idx << " is out of range [0.." << fMsrInfo->GetMsrRunList()->size() << "[" << std::endl << std::endl;
    return chisq;
  }

  Int_t subIdx = 0;
  Int_t type = fMsrInfo->GetMsrRunList()->at(idx).GetFitType();
  if (type == -1) { // i.e. not found in the RUN block, try the GLOBAL block
    type = fMsrInfo->GetMsrGlobal()->GetFitType();
    subIdx = idx;
  } else { // found in the RUN block
    // count how many entries of this fit-type are present up to idx
    for (UInt_t i=0; i<idx; i++) {
      if (fMsrInfo->GetMsrRunList()->at(i).GetFitType() == type)
        subIdx++;
    }
  }

  // return the chisq of the single run
  switch (type) {
  case PRUN_SINGLE_HISTO:
    chisq = fRunSingleHistoList[subIdx]->CalcChiSquare(par);
    break;
  case PRUN_SINGLE_HISTO_RRF:
    chisq = fRunSingleHistoRRFList[subIdx]->CalcChiSquare(par);
    break;
  case PRUN_ASYMMETRY:
    chisq = fRunAsymmetryList[subIdx]->CalcChiSquare(par);
    break;
  case PRUN_ASYMMETRY_RRF:
    chisq = fRunAsymmetryRRFList[subIdx]->CalcChiSquare(par);
    break;
  case PRUN_ASYMMETRY_BNMR:
    chisq = fRunAsymmetryBNMRList[subIdx]->CalcChiSquare(par);
    break;
  case PRUN_MU_MINUS:
    chisq = fRunMuMinusList[subIdx]->CalcChiSquare(par);
    break;
  case PRUN_NON_MUSR:
    chisq = fRunNonMusrList[subIdx]->CalcChiSquare(par);
    break;
  default:
    break;
  }

  return chisq;
}

//--------------------------------------------------------------------------
// GetSingleHistoMaximumLikelihood (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates log max-likelihood of <em>all</em> single histogram runs of a msr-file.
 *
 * <b>return:</b>
 * - log max-likelihood of all single histogram runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetSingleHistoMaximumLikelihood(const std::vector<Double_t>& par) const
{
  Double_t mlh = 0.0;

  for (UInt_t i=0; i<fRunSingleHistoList.size(); i++)
    mlh += fRunSingleHistoList[i]->CalcMaxLikelihood(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetSingleHistoRRFMaximumLikelihood (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates log max-likelihood of <em>all</em> single histogram RRF runs of a msr-file.
 *
 * <b>return:</b>
 * - log max-likelihood of all single histogram runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetSingleHistoRRFMaximumLikelihood(const std::vector<Double_t>& par) const
{
  Double_t mlh = 0.0;

  for (UInt_t i=0; i<fRunSingleHistoRRFList.size(); i++)
    mlh += fRunSingleHistoRRFList[i]->CalcMaxLikelihood(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetAsymmetryMaximumLikelihood (public)
//--------------------------------------------------------------------------
/**
 * <p> Since it is not clear yet how to handle asymmetry fits with max likelihood
 * the chi square will be used!
 *
 * <b>return:</b>
 * - chi-square of all asymmetry runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetAsymmetryMaximumLikelihood(const std::vector<Double_t>& par) const
{
  Double_t mlh = 0.0;

  for (UInt_t i=0; i<fRunAsymmetryList.size(); i++)
    mlh += fRunAsymmetryList[i]->CalcChiSquare(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetAsymmetryRRFMaximumLikelihood (public)
//--------------------------------------------------------------------------
/**
 * <p> Since it is not clear yet how to handle asymmetry fits with max likelihood
 * the chi square will be used!
 *
 * <b>return:</b>
 * - chi-square of all asymmetry RRF runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetAsymmetryRRFMaximumLikelihood(const std::vector<Double_t>& par) const
{
  Double_t mlh = 0.0;

  for (UInt_t i=0; i<fRunAsymmetryRRFList.size(); i++)
    mlh += fRunAsymmetryRRFList[i]->CalcChiSquare(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetAsymmetryBNMRMaximumLikelihood (public)
//--------------------------------------------------------------------------
/**
 * <p> Since it is not clear yet how to handle asymmetry fits with max likelihood
 * the chi square will be used!
 *
 * <b>return:</b>
 * - chi-square of all asymmetry BNMR runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetAsymmetryBNMRMaximumLikelihood(const std::vector<Double_t>& par) const
{
  Double_t mlh = 0.0;

  for (UInt_t i=0; i<fRunAsymmetryBNMRList.size(); i++)
    mlh += fRunAsymmetryBNMRList[i]->CalcChiSquare(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetMuMinusMaximumLikelihood (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates log max-likelihood of <em>all</em> mu minus runs of a msr-file.
 *
 * <b>return:</b>
 * - log max-likelihood of all mu minus runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetMuMinusMaximumLikelihood(const std::vector<Double_t>& par) const
{
  Double_t mlh = 0.0;

  for (UInt_t i=0; i<fRunMuMinusList.size(); i++)
    mlh += fRunMuMinusList[i]->CalcMaxLikelihood(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetNonMusrMaximumLikelihood (public)
//--------------------------------------------------------------------------
/**
 * <p> Since it is not clear yet how to handle non musr fits with max likelihood
 * the chi square will be used!
 *
 * <b>return:</b>
 * - chi-square of all asymmetry runs of the msr-file
 *
 * \param par fit parameter vector
 */
Double_t PRunListCollection::GetNonMusrMaximumLikelihood(const std::vector<Double_t>& par) const
{
  Double_t mlh = 0.0;

  for (UInt_t i=0; i<fRunNonMusrList.size(); i++)
    mlh += fRunNonMusrList[i]->CalcChiSquare(par);

  return mlh;
}

//--------------------------------------------------------------------------
// GetSingleRunMaximumLikelihoodExpected (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates expected mlh of the run block index idx of a msr-file.
 *
 * <b>return:</b>
 * - expected mlh of for a single run block
 *
 * \param par fit parameter vector
 * \param idx run block index
 */
Double_t PRunListCollection::GetSingleRunMaximumLikelihoodExpected(const std::vector<Double_t>& par, const UInt_t idx) const
{
  Double_t expected_mlh = 0.0;

  if (idx > fMsrInfo->GetMsrRunList()->size()) {
    std::cerr << ">> PRunListCollection::GetSingleRunMaximumLikelihoodExpected() **ERROR** idx=" << idx << " is out of range [0.." << fMsrInfo->GetMsrRunList()->size() << "[" << std::endl << std::endl;
    return expected_mlh;
  }

  UInt_t subIdx = 0;
  Int_t type = fMsrInfo->GetMsrRunList()->at(idx).GetFitType();
  if (type == -1) { // i.e. not found in the RUN block, try the GLOBAL block
    type = fMsrInfo->GetMsrGlobal()->GetFitType();
    subIdx = idx;
  } else { // found in the RUN block
    // count how many entries of this fit-type are present up to idx
    for (UInt_t i=0; i<idx; i++) {
      if (fMsrInfo->GetMsrRunList()->at(i).GetFitType() == type)
        subIdx++;
    }
  }

  // return the mlh of the single run
  switch (type) {
  case PRUN_SINGLE_HISTO:
    expected_mlh = fRunSingleHistoList[subIdx]->CalcMaxLikelihoodExpected(par);
    break;
  default:
    break;
  }

  return expected_mlh;
}

//--------------------------------------------------------------------------
// GetSingleRunMaximumLikelihood (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculates mlh of a single run-block entry of the msr-file.
 *
 * <b>return:</b>
 * - mlh of single run-block entry with index idx
 *
 * \param par fit parameter vector
 * \param idx run block index
 */
Double_t PRunListCollection::GetSingleRunMaximumLikelihood(const std::vector<Double_t>& par, const UInt_t idx) const
{
  Double_t mlh = 0.0;

  if (idx > fMsrInfo->GetMsrRunList()->size()) {
    std::cerr << ">> PRunListCollection::GetSingleRunMaximumLikelihood() **ERROR** idx=" << idx << " is out of range [0.." << fMsrInfo->GetMsrRunList()->size() << "[" << std::endl << std::endl;
    return mlh;
  }

  Int_t subIdx = 0;
  Int_t type = fMsrInfo->GetMsrRunList()->at(idx).GetFitType();
  if (type == -1) { // i.e. not found in the RUN block, try the GLOBAL block
    type = fMsrInfo->GetMsrGlobal()->GetFitType();
    subIdx = idx;
  } else { // found in the RUN block
    // count how many entries of this fit-type are present up to idx
    for (UInt_t i=0; i<idx; i++) {
      if (fMsrInfo->GetMsrRunList()->at(i).GetFitType() == type)
        subIdx++;
    }
  }

  // return the mlh of the single run
  switch (type) {
  case PRUN_SINGLE_HISTO:
    mlh = fRunSingleHistoList[subIdx]->CalcMaxLikelihood(par);
    break;
  default:
    break;
  }

  return mlh;
}

//--------------------------------------------------------------------------
// GetNoOfBinsFitted (public)
//--------------------------------------------------------------------------
/**
 * <p>Number of bins in run block idx to be fitted. Only used for single histogram
 * fitting together with the expected chisq.
 *
 * <b>return:</b>
 * - number of bins fitted.
 *
 * \param idx run block index
 */
UInt_t PRunListCollection::GetNoOfBinsFitted(const UInt_t idx) const
{
  UInt_t result = 0;

  if (idx > fMsrInfo->GetMsrRunList()->size()) {
    std::cerr << ">> PRunListCollection::GetNoOfBinsFitted() **ERROR** idx=" << idx << " is out of range [0.." << fMsrInfo->GetMsrRunList()->size() << "[" << std::endl << std::endl;
    return result;
  }

  UInt_t subIdx = 0;
  Int_t type = fMsrInfo->GetMsrRunList()->at(idx).GetFitType();
  if (type == -1) { // i.e. not found in the RUN block, try the GLOBAL block
    type = fMsrInfo->GetMsrGlobal()->GetFitType();
    subIdx = idx;
  } else { // found in the RUN block
    // count how many entries of this fit-type are present up to idx
    for (UInt_t i=0; i<idx; i++) {
      if (fMsrInfo->GetMsrRunList()->at(i).GetFitType() == type)
        subIdx++;
    }
  }

  // return the chisq of the single run
  switch (type) {
  case PRUN_SINGLE_HISTO:
    result = fRunSingleHistoList[subIdx]->GetNoOfFitBins();
    break;
  case PRUN_SINGLE_HISTO_RRF:
    result = fRunSingleHistoRRFList[subIdx]->GetNoOfFitBins();
    break;
  case PRUN_ASYMMETRY:
    result = fRunAsymmetryList[subIdx]->GetNoOfFitBins();
    break;
  case PRUN_ASYMMETRY_RRF:
    result = fRunAsymmetryRRFList[subIdx]->GetNoOfFitBins();
    break;
  case PRUN_ASYMMETRY_BNMR:
    result = fRunAsymmetryBNMRList[subIdx]->GetNoOfFitBins();
    break;
  case PRUN_MU_MINUS:
    result = fRunMuMinusList[subIdx]->GetNoOfFitBins();
    break;
  case PRUN_NON_MUSR:
    result = fRunNonMusrList[subIdx]->GetNoOfFitBins();
    break;
  default:
    break;
  }

  return result;
}


//--------------------------------------------------------------------------
// GetTotalNoOfBinsFitted (public)
//--------------------------------------------------------------------------
/**
 * <p>Counts the total number of bins to be fitted.
 *
 * <b>return:</b>
 * - total number of bins fitted.
 */
UInt_t PRunListCollection::GetTotalNoOfBinsFitted() const
{
  UInt_t counts = 0;

  for (UInt_t i=0; i<fRunSingleHistoList.size(); i++)
    counts += fRunSingleHistoList[i]->GetNoOfFitBins();

  for (UInt_t i=0; i<fRunSingleHistoRRFList.size(); i++)
    counts += fRunSingleHistoRRFList[i]->GetNoOfFitBins();

  for (UInt_t i=0; i<fRunAsymmetryList.size(); i++)
    counts += fRunAsymmetryList[i]->GetNoOfFitBins();

  for (UInt_t i=0; i<fRunAsymmetryRRFList.size(); i++)
    counts += fRunAsymmetryRRFList[i]->GetNoOfFitBins();

  for (UInt_t i=0; i<fRunAsymmetryBNMRList.size(); i++)
    counts += fRunAsymmetryBNMRList[i]->GetNoOfFitBins();

  for (UInt_t i=0; i<fRunMuMinusList.size(); i++)
    counts += fRunMuMinusList[i]->GetNoOfFitBins();

  for (UInt_t i=0; i<fRunNonMusrList.size(); i++)
    counts += fRunNonMusrList[i]->GetNoOfFitBins();

  return counts;
}

//--------------------------------------------------------------------------
// GetSingleHisto (public)
//--------------------------------------------------------------------------
/**
 * <p>Get a processed single histogram data set.
 *
 * <b>return:</b>
 * - pointer to the run data set (processed data) if data set is found
 * - null pointer otherwise
 *
 * \param index msr-file run index
 * \param tag kIndex -> data at index, kRunNo -> data of given run no
 */
PRunData* PRunListCollection::GetSingleHisto(UInt_t index, EDataSwitch tag)
{
  PRunData *data = nullptr;

  switch (tag) {
    case kIndex:
      if (index >= fRunSingleHistoList.size()) {
        std::cerr << std::endl << ">> PRunListCollection::GetSingleHisto(): **ERROR** index = " << index << " out of bounds";
        std::cerr << std::endl;
        return nullptr;
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
// GetSingleHistoRRF (public)
//--------------------------------------------------------------------------
/**
 * <p>Get a processed single histogram RRF data set.
 *
 * <b>return:</b>
 * - pointer to the run data set (processed data) if data set is found
 * - null pointer otherwise
 *
 * \param index msr-file run index
 * \param tag kIndex -> data at index, kRunNo -> data of given run no
 */
PRunData* PRunListCollection::GetSingleHistoRRF(UInt_t index, EDataSwitch tag)
{
  PRunData *data = nullptr;

  switch (tag) {
    case kIndex:
      if (index >= fRunSingleHistoRRFList.size()) {
        std::cerr << std::endl << ">> PRunListCollection::GetSingleHistoRRF(): **ERROR** index = " << index << " out of bounds";
        std::cerr << std::endl;
        return nullptr;
      }

      fRunSingleHistoRRFList[index]->CalcTheory();
      data = fRunSingleHistoRRFList[index]->GetData();
      break;
    case kRunNo:
      for (UInt_t i=0; i<fRunSingleHistoRRFList.size(); i++) {
        if (fRunSingleHistoRRFList[i]->GetRunNo() == index) {
          data = fRunSingleHistoRRFList[i]->GetData();
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
// GetAsymmetry (public)
//--------------------------------------------------------------------------
/**
 * <p>Get a processed asymmetry data set.
 *
 * <b>return:</b>
 * - pointer to the run data set (processed data) if data set is found
 * - null pointer otherwise
 *
 * \param index msr-file run index
 * \param tag kIndex -> data at index, kRunNo -> data of given run no
 */
PRunData* PRunListCollection::GetAsymmetry(UInt_t index, EDataSwitch tag)
{
  PRunData *data = nullptr;

  switch (tag) {
    case kIndex: // called from musrfit when dumping the data
      if (index > fRunAsymmetryList.size()) {
        std::cerr << std::endl << ">> PRunListCollection::GetAsymmetry(): **ERROR** index = " << index << " out of bounds";
        std::cerr << std::endl;
        return nullptr;
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
// GetAsymmetryBNMR (public)
//--------------------------------------------------------------------------
/**
 * <p>Get a processed asymmetry from beta-NMR data set.
 *
 * <b>return:</b>
 * - pointer to the run data set (processed data) if data set is found
 * - null pointer otherwise
 *
 * \param index msr-file run index
 * \param tag kIndex -> data at index, kRunNo -> data of given run no
 */
PRunData* PRunListCollection::GetAsymmetryBNMR(UInt_t index, EDataSwitch tag)
{
  PRunData *data = 0;

  switch (tag) {
    case kIndex: // called from musrfit when dumping the data
      if (index > fRunAsymmetryBNMRList.size()) {
	std::cerr << std::endl << ">> PRunListCollection::GetAsymmetryBNMR(): **ERROR** index = " << index << " out of bounds";
        std::cerr << std::endl;
        return 0;
      }

      fRunAsymmetryBNMRList[index]->CalcTheory();
      data = fRunAsymmetryBNMRList[index]->GetData();
      break;
    case kRunNo: // called from PMusrCanvas
      for (UInt_t i=0; i<fRunAsymmetryBNMRList.size(); i++) {
        if (fRunAsymmetryBNMRList[i]->GetRunNo() == index) {
          data = fRunAsymmetryBNMRList[i]->GetData();
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
// GetAsymmetryRRF (public)
//--------------------------------------------------------------------------
/**
 * <p>Get a processed asymmetry RRF data set.
 *
 * <b>return:</b>
 * - pointer to the run data set (processed data) if data set is found
 * - null pointer otherwise
 *
 * \param index msr-file run index
 * \param tag kIndex -> data at index, kRunNo -> data of given run no
 */
PRunData* PRunListCollection::GetAsymmetryRRF(UInt_t index, EDataSwitch tag)
{
  PRunData *data = nullptr;

  switch (tag) {
    case kIndex: // called from musrfit when dumping the data
      if (index > fRunAsymmetryRRFList.size()) {
        std::cerr << std::endl << ">> PRunListCollection::GetAsymmetryRRF(): **ERROR** index = " << index << " out of bounds";
        std::cerr << std::endl;
        return nullptr;
      }

      fRunAsymmetryRRFList[index]->CalcTheory();
      data = fRunAsymmetryRRFList[index]->GetData();
      break;
    case kRunNo: // called from PMusrCanvas
      for (UInt_t i=0; i<fRunAsymmetryRRFList.size(); i++) {
        if (fRunAsymmetryRRFList[i]->GetRunNo() == index) {
          data = fRunAsymmetryRRFList[i]->GetData();
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
// GetMuMinus (public)
//--------------------------------------------------------------------------
/**
 * <p>Get a processed mu minus data set.
 *
 * <b>return:</b>
 * - pointer to the run data set (processed data) if data set is found
 * - null pointer otherwise
 *
 * \param index msr-file run index
 * \param tag kIndex -> data at index, kRunNo -> data of given run no
 */
PRunData* PRunListCollection::GetMuMinus(UInt_t index, EDataSwitch tag)
{
  PRunData *data = nullptr;

  switch (tag) {
    case kIndex:
      if (index > fRunMuMinusList.size()) {
        std::cerr << std::endl << ">> PRunListCollection::GetMuMinus(): **ERROR** index = " << index << " out of bounds";
        std::cerr << std::endl;
        return nullptr;
      }
      fRunMuMinusList[index]->CalcTheory();
      data = fRunMuMinusList[index]->GetData();
      break;
    case kRunNo:
      for (UInt_t i=0; i<fRunMuMinusList.size(); i++) {
        if (fRunMuMinusList[i]->GetRunNo() == index) {
          data = fRunMuMinusList[i]->GetData();
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
// GetNonMusr (public)
//--------------------------------------------------------------------------
/**
 * <p>Get a processed non-muSR data set.
 *
 * <b>return:</b>
 * - pointer to the run data set (processed data) if data set is found
 * - null pointer otherwise
 *
 * \param index msr-file run index
 * \param tag kIndex -> data at index, kRunNo -> data of given run no
 */
PRunData* PRunListCollection::GetNonMusr(UInt_t index, EDataSwitch tag)
{
  PRunData *data = nullptr;

  switch (tag) {
    case kIndex:
      if (index > fRunNonMusrList.size()) {
        std::cerr << std::endl << ">> PRunListCollection::GetNonMusr(): **ERROR** index = " << index << " out of bounds";
        std::cerr << std::endl;
        return nullptr;
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
// GetTemp (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the temperature from the data set.
 *
 * <b>return:</b>
 * - temperature pair (T, dT) vector from temperatures stored in the data file.
 *
 * \param runName name of the run from which to extract the temperature
 */
const PDoublePairVector* PRunListCollection::GetTemp(const TString &runName) const
{
  return fData->GetRunData(runName)->GetTemperature();
}

//--------------------------------------------------------------------------
// GetField (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the magnetic field from the data set.
 *
 * <b>return:</b>
 * - magnetic field stored in the data file.
 *
 * \param runName name of the run from which to extract the magnetic field
 */
Double_t PRunListCollection::GetField(const TString &runName) const
{
  return fData->GetRunData(runName)->GetField();
}

//--------------------------------------------------------------------------
// GetEnergy (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the muon implantation energy from the data set.
 *
 * <b>return:</b>
 * - muon implantation energy stored in the data file.
 *
 * \param runName name of the run from which to extract the muon implantation energy
 */
Double_t PRunListCollection::GetEnergy(const TString &runName) const
{
  return fData->GetRunData(runName)->GetEnergy();
}

//--------------------------------------------------------------------------
// GetSetup (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the setup information from the data set.
 *
 * <b>return:</b>
 * - setup information stored in the data file.
 *
 * \param runName name of the run from which to extract the setup information
 */
const Char_t* PRunListCollection::GetSetup(const TString &runName) const
{
  return fData->GetRunData(runName)->GetSetup()->Data();
}

//--------------------------------------------------------------------------
// GetXAxisTitle (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the x-axis title (used with non-muSR fit).
 *
 * <b>return:</b>
 * - x-axis title
 *
 * \param runName name of the run file
 * \param idx msr-file run index
 */
const Char_t* PRunListCollection::GetXAxisTitle(const TString &runName, const UInt_t idx) const
{
  PRawRunData *runData = fData->GetRunData(runName);

  const Char_t *result = nullptr;

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
// GetYAxisTitle (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the y-axis title (used with non-muSR fit).
 *
 * <b>return:</b>
 * - y-axis title
 *
 * \param runName name of the run file
 * \param idx msr-file run index
 */
const Char_t* PRunListCollection::GetYAxisTitle(const TString &runName, const UInt_t idx) const
{
  PRawRunData *runData = fData->GetRunData(runName);

  const Char_t *result = nullptr;

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

