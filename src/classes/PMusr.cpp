/***************************************************************************

  PMusr.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2014 by Andreas Suter                              *
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

#include <cassert>
#include <iostream>
using namespace std;

#include <boost/algorithm/string.hpp>
using namespace boost;

#include "TMath.h"

#include "PMusr.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implementation PRunData
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 */
PRunData::PRunData()
{
  fDataTimeStart   = PMUSR_UNDEFINED;
  fDataTimeStep    = PMUSR_UNDEFINED;
  fX.clear();        // only used in non-muSR
  fValue.clear();
  fError.clear();
  fTheoryTimeStart = PMUSR_UNDEFINED;
  fTheoryTimeStep  = PMUSR_UNDEFINED;
  fXTheory.clear();  // only used in non-muSR
  fTheory.clear();
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor.
 */
PRunData::~PRunData()
{
  fX.clear();        // only used in non-muSR
  fValue.clear();
  fError.clear();
  fXTheory.clear();  // only used in non-muSR
  fTheory.clear();
}

//--------------------------------------------------------------------------
// SetTheoryValue (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets a value of the theory vector
 *
 * \param idx index of the theory vector
 * \param dval value to be set.
 */
void PRunData::SetTheoryValue(UInt_t idx, Double_t dval)
{
  if (idx > fTheory.size())
    fTheory.resize(idx+1);

  fTheory[idx] = dval;
}

//--------------------------------------------------------------------------
// ReplaceTheory (public)
//--------------------------------------------------------------------------
/**
 * <p>Replaces the theory vector.
 *
 * \param theo vector which is replacing the current theory vector
 */
void PRunData::ReplaceTheory(const PDoubleVector &theo)
{
  fTheory = theo;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implementation PNonMusrRawRunData
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 */
PNonMusrRawRunData::PNonMusrRawRunData()
{
  fFromAscii = true;
  fLabels.clear();
  fDataTags.clear();

  for (UInt_t i=0; i<fData.size(); i++)
    fData[i].clear();
  fData.clear();

  for (UInt_t i=0; i<fErrData.size(); i++)
    fErrData[i].clear();
  fErrData.clear();
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor.
 */
PNonMusrRawRunData::~PNonMusrRawRunData()
{
  fLabels.clear();
  fDataTags.clear();

  for (UInt_t i=0; i<fData.size(); i++)
    fData[i].clear();
  fData.clear();

  for (UInt_t i=0; i<fErrData.size(); i++)
    fErrData[i].clear();
  fErrData.clear();
}

//--------------------------------------------------------------------------
// SetLabel (public)
//--------------------------------------------------------------------------
/**
 * <p>Modifies a label at position idx. In case idx is larges than the number
 * of labels, only a warning will be sent to stderr.
 *
 * \param idx index at which position the label should be set
 * \param str string to be set
 */
void PNonMusrRawRunData::SetLabel(const UInt_t idx, const TString str)
{
  if (idx >= fLabels.size()) {
    cerr << endl << ">> PNonMusrRawRunData::SetLabel: **WARNING** idx=" << idx << " is out of range [0," << fLabels.size() << "[.";
    cerr << endl;
    return;
  }
  fLabels[idx] = str;
}

//--------------------------------------------------------------------------
// AppendSubData (public)
//--------------------------------------------------------------------------
/**
 * <p>Modifies the data set at position idx. In case idx is larger than the number
 * of data sets, only a warning will be sent to stderr.
 *
 * \param idx index at which position the label should be set
 * \param dval double value to be set
 */
void PNonMusrRawRunData::AppendSubData(const UInt_t idx, const Double_t dval)
{
  if (idx >= fData.size()) {
    cerr << endl << ">> PNonMusrRawRunData::AppendSubData: **WARNING** idx=" << idx << " is out of range [0," << fData.size() << "[.";
    cerr << endl;
    return;
  }
  fData[idx].push_back(dval);
}

//--------------------------------------------------------------------------
// AppendSubErrData (public)
//--------------------------------------------------------------------------
/**
 * <p>Modifies the error-data set at position idx. In case idx is larger than the number
 * of error-data sets, only a warning will be sent to stderr.
 *
 * \param idx index at which position the label should be set
 * \param dval double value to be set
 */
void PNonMusrRawRunData::AppendSubErrData(const UInt_t idx, const Double_t dval)
{
  if (idx >= fErrData.size()) {
    cerr << endl << ">> PNonMusrRawRunData::AppendSubErrData: **WARNING** idx=" << idx << " is out of range [0," << fErrData.size() << "[.";
    cerr << endl;
    return;
  }
  fErrData[idx].push_back(dval);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implementation PRawRunDataSet
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 */
PRawRunDataSet::PRawRunDataSet()
{
  Clear();
}

//--------------------------------------------------------------------------
// Clear (public)
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 */
void PRawRunDataSet::Clear()
{
  fName = TString("n/a");
  fHistoNo = -1;
  fTimeZeroBin = 0.0;
  fTimeZeroBinEstimated = 0.0;
  fFirstGoodBin = 0;
  fLastGoodBin = 0;
  fFirstBkgBin = 0;
  fLastBkgBin = 0;
  fData.clear();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implementation PRawRunDataVector
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// IsPresent (public)
//--------------------------------------------------------------------------
/**
 * <p>Checks if the histogram set with histgram number histoNo is present.
 *
 * <p><b>return:</b>
 * - true if found
 * - false otherwise
 *
 * \param histoNo histogram number requested
 */
Bool_t PRawRunDataVector::IsPresent(UInt_t histoNo)
{
  Bool_t found=false;

  for (UInt_t i=0; i<fDataVec.size(); i++) {
    if (fDataVec[i].GetHistoNo() == (Int_t)histoNo) {
      found = true;
      break;
    }
  }

  return found;
}

//--------------------------------------------------------------------------
// GetSet (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the raw data set with the index idx. This routine is different to
 * PRawRunDataVector::Get(UInt_t histoNo) where the internal saved histogram
 * number is used.
 *
 * <p><b>return:</b>
 * - pointer to the raw data set if found
 * - 0 otherwise
 *
 * \param idx data set index
 */
PRawRunDataSet* PRawRunDataVector::GetSet(UInt_t idx)
{
  PRawRunDataSet *result=0;

  if (idx >= fDataVec.size())
    return result;

  return &fDataVec[idx];
}

//--------------------------------------------------------------------------
// Get (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the raw data set with histogram number histoNo.
 *
 * <p><b>return:</b>
 * - pointer to the raw data set if found
 * - 0 otherwise
 *
 * \param histoNo histogram number requested
 */
PRawRunDataSet* PRawRunDataVector::Get(UInt_t histoNo)
{
  PRawRunDataSet *result=0;

  for (UInt_t i=0; i<fDataVec.size(); i++) {
    if (fDataVec[i].GetHistoNo() == (Int_t)histoNo) {
      result = &fDataVec[i];
      break;
    }
  }

  return result;
}

//--------------------------------------------------------------------------
// operator[] (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the raw data set with histogram number histoNo.
 *
 * <p><b>return:</b>
 * - pointer to the raw data set if found
 * - 0 otherwise
 *
 * \param histoNo histogram number requested
 */
PRawRunDataSet* PRawRunDataVector::operator[](UInt_t histoNo)
{
  return Get(histoNo);
}

//--------------------------------------------------------------------------
// GetData (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the raw data set data with histogram number histoNo.
 *
 * <p><b>return:</b>
 * - pointer to the raw data set if found
 * - 0 otherwise
 *
 * \param histoNo histogram number requested
 */
PDoubleVector* PRawRunDataVector::GetData(UInt_t histoNo)
{
  PDoubleVector *result=0;

  for (UInt_t i=0; i<fDataVec.size(); i++) {
    if (fDataVec[i].GetHistoNo() == (Int_t)histoNo) {
      result = fDataVec[i].GetData();
      break;
    }
  }

  return result;
}

//--------------------------------------------------------------------------
// GetT0Bin (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the T0 bin of raw run data with histogram number histoNo.
 *
 * <p><b>return:</b>
 * - T0 bin if found
 * - PMUSR_UNDEFINED otherwise
 *
 * \param histoNo histogram number requested
 */
Double_t PRawRunDataVector::GetT0Bin(UInt_t histoNo)
{
  Double_t result=-1.0; // undefined

  for (UInt_t i=0; i<fDataVec.size(); i++) {
    if (fDataVec[i].GetHistoNo() == (Int_t)histoNo) {
      result = fDataVec[i].GetTimeZeroBin();
      break;
    }
  }

  return result;
}

//--------------------------------------------------------------------------
// GetT0BinEstimated (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the estimated T0 bin of raw run data with histogram number histoNo.
 *
 * <p><b>return:</b>
 * - estimated T0 bin if found
 * - PMUSR_UNDEFINED otherwise
 *
 * \param histoNo histogram number requested
 */
Double_t PRawRunDataVector::GetT0BinEstimated(UInt_t histoNo)
{
  Double_t result=PMUSR_UNDEFINED;

  for (UInt_t i=0; i<fDataVec.size(); i++) {
    if (fDataVec[i].GetHistoNo() == (Int_t)histoNo) {
      result = fDataVec[i].GetTimeZeroBinEstimated();
      break;
    }
  }

  return result;
}

//--------------------------------------------------------------------------
// GetBkgBin (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the background bins (start, end).
 *
 * <p><b>return:</b>
 * - background bins if found
 * - (-1, -1) otherwise
 *
 * \param histoNo histogram number requested
 */
PIntPair PRawRunDataVector::GetBkgBin(UInt_t histoNo)
{
  PIntPair bkg(-1,-1);

  for (UInt_t i=0; i<fDataVec.size(); i++) {
    if (fDataVec[i].GetHistoNo() == (Int_t)histoNo) {
      bkg.first  = fDataVec[i].GetFirstBkgBin();
      bkg.second = fDataVec[i].GetLastBkgBin();
      break;
    }
  }

  return bkg;
}

//--------------------------------------------------------------------------
// GetGoodDataBin (public)
//--------------------------------------------------------------------------
/**
 * <p>Get the first/last good data bin (fgb, lgb).
 *
 * <p><b>return:</b>
 * - good data bins if found
 * - (-1, -1) otherwise
 *
 * \param histoNo histogram number requested
 */
PIntPair PRawRunDataVector::GetGoodDataBin(UInt_t histoNo)
{
  PIntPair gdb(-1,-1);

  for (UInt_t i=0; i<fDataVec.size(); i++) {
    if (fDataVec[i].GetHistoNo() == (Int_t)histoNo) {
      gdb.first  = fDataVec[i].GetFirstGoodBin();
      gdb.second = fDataVec[i].GetLastGoodBin();
      break;
    }
  }

  return gdb;
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set a data set at idx. If idx == -1 append it at the end (default),
 * otherwise check if idx is within bounds and if yes, place it there,
 * otherwise resize the data vector and place it where requested.
 *
 * \param dataSet data set to be set
 * \param idx index at which to place the data set.
 */
void PRawRunDataVector::Set(PRawRunDataSet dataSet, Int_t idx)
{
  if (idx == -1) { // data set to be appended
    fDataVec.push_back(dataSet);
  } else {
    if (idx >= (Int_t)fDataVec.size())
      fDataVec.resize(idx+1);
    fDataVec[idx] = dataSet;
  }
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implementation PRawRunData
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 */
PRawRunData::PRawRunData()
{
  fVersion = TString("n/a");
  fGenericValidatorURL = TString("n/a");
  fSpecificValidatorURL = TString("n/a");
  fGenerator = TString("n/a");
  fComment = TString("n/a");
  fFileName = TString("n/a");
  fLaboratory = TString("n/a");
  fBeamline = TString("n/a");
  fInstrument = TString("n/a");
  fMuonSource = TString("n/a");
  fMuonSpecies = TString("n/a");
  fMuonBeamMomentum = PMUSR_UNDEFINED;
  fMuonSpinAngle = PMUSR_UNDEFINED;
  fRunName = TString("n/a");
  fRunNumber = -1;
  fRunTitle = TString("n/a");
  fSetup = TString("n/a");
  fStartTime = TString("n/a");
  fStartDate = TString("n/a");
  fStartDateTimeSec = 0;
  fStopTime = TString("n/a");
  fStopDate = TString("n/a");
  fStopDateTimeSec = 0;
  fCryo = TString("n/a");
  fSample = TString("n/a");
  fOrientation = TString("n/a");
  fMagnet = TString("n/a");
  fField = PMUSR_UNDEFINED;
  fEnergy = PMUSR_UNDEFINED;
  fTransport = PMUSR_UNDEFINED;
  fTimeResolution = PMUSR_UNDEFINED;
  fRedGreenOffset.push_back(0);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor.
 */
PRawRunData::~PRawRunData()
{
  fTemp.clear();
  fRingAnode.clear();
  fRedGreenOffset.clear();
}

//--------------------------------------------------------------------------
// GetTemperature (public)
//--------------------------------------------------------------------------
/**
 * <p>Returns the temperature of a muSR run.
 *
 * <b>return:</b>
 * - temperature value, if idx is within proper boundaries
 * - PMUSR_UNDEFINED, otherwise
 *
 * \param idx index of the temperature whished
 */
const Double_t PRawRunData::GetTemperature(const UInt_t idx)
{
  if (idx >= fTemp.size()) {
    cerr << endl << ">> PRawRunData::GetTemperature: **WARNING** idx=" << idx << " is out of range [0," << fTemp.size() << "[.";
    cerr << endl;
    return PMUSR_UNDEFINED;
  }
  return fTemp[idx].first;
}

//--------------------------------------------------------------------------
// GetTempError (public)
//--------------------------------------------------------------------------
/**
 * <p>Returns the error estimate of the temperature of a muSR run.
 *
 * <b>return:</b>
 * - temperature error value, if idx is within proper boundaries
 * - PMUSR_UNDEFINED, otherwise
 *
 * \param idx index of the temperature error whished
 */
const Double_t PRawRunData::GetTempError(const UInt_t idx)
{
  if (idx >= fTemp.size()) {
    cerr << endl << ">> PRawRunData::GetTempError: **WARNING** idx=" << idx << " is out of range [0," << fTemp.size() << "[.";
    cerr << endl;
    return PMUSR_UNDEFINED;
  }
  return fTemp[idx].second;
}

//--------------------------------------------------------------------------
// GetRingAnode (public)
//--------------------------------------------------------------------------
/**
 * <p> Returns the value of a ring anode high voltage. (idx = 0: RAL, 1: RAR, 2: RAT, 3: RAB)
 *
 * <b>return:</b>
 * - ring anode HV value, if idx is within proper boundaries
 * - PMUSR_UNDEFINED, otherwise
 *
 * \param idx index of the ring anode whished
 */
const Double_t PRawRunData::GetRingAnode(const UInt_t idx)
{
  if (idx >= fRingAnode.size()) {
    cerr << endl << ">> PRawRunData::GetRingAnode: **WARNING** idx=" << idx << " is out of range [0," << fRingAnode.size() << "[.";
    cerr << endl;
    return PMUSR_UNDEFINED;
  }
  return fRingAnode[idx];
}

//--------------------------------------------------------------------------
// GetDataSet (public)
//--------------------------------------------------------------------------
/**
 * <p>Returns a raw muSR run data set.
 *
 * <b>return:</b>
 * - pointer of the data vector, if idx is within proper boundaries
 * - 0, otherwise
 *
 * \param idx either the histogram number or the index, depending in wantHistoNo-flag
 * \param wantHistoNo flag indicating if idx is the histoNo or the index. Default is wantHistoNo==true
 */
PRawRunDataSet* PRawRunData::GetDataSet(const UInt_t idx, Bool_t wantHistoNo)
{
  if (wantHistoNo)
    return fData[idx];
  else
    return fData.GetSet(idx);
}


//--------------------------------------------------------------------------
// SetRingAnode (public)
//--------------------------------------------------------------------------
/**
 * <p> Store the ring anode value at index position idx.
 *
 * \param idx position at which to store the temperature value
 * \param dval ring anode value
 */
void PRawRunData::SetRingAnode(const UInt_t idx, const Double_t dval)
{
  if (idx >= fRingAnode.size())
    fRingAnode.resize(idx+1);
  fRingAnode[idx] = dval;
}

//--------------------------------------------------------------------------
// SetTemperature (public)
//--------------------------------------------------------------------------
/**
 * <p> Store the temperature with its error at index position idx.
 *
 * \param idx position at which to store the temperature value
 * \param temp temperature value
 * \param errTemp error estimate of the temperature value
 */
void PRawRunData::SetTemperature(const UInt_t idx, const Double_t temp, const Double_t errTemp)
{
  if (idx >= fTemp.size()) {
    fTemp.resize(idx+1);
  }
  fTemp[idx].first  = temp;
  fTemp[idx].second = errTemp;
}

//--------------------------------------------------------------------------
// SetTempError (public)
//--------------------------------------------------------------------------
/**
 * <p> Store the temperature error at index position idx.
 *
 * \param idx position at which to store the temperature value
 * \param errTemp error estimate of the temperature value
 */
void PRawRunData::SetTempError(const UInt_t idx, const Double_t errTemp)
{
  if (idx >= fTemp.size()) {
    fTemp.resize(idx+1);
  }
  fTemp[idx].first  = PMUSR_UNDEFINED;
  fTemp[idx].second = errTemp;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implementation PMsrGlobalBlock
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// PMsrGlobalBlock
//--------------------------------------------------------------------------
/**
 * <p> Constructor
 */
PMsrGlobalBlock::PMsrGlobalBlock()
{
  fGlobalPresent = false;
  fRRFFreq = 0.0; // rotating reference frequency in units given by fRRFUnitTag. Only needed for fittype 1
  fRRFUnitTag = RRF_UNIT_MHz; // RRF unit tag. Default: MHz
  fRRFPhase = 0.0;
  fRRFPacking = -1; // undefined RRF packing/rebinning
  fFitType = -1; // undefined fit type
  for (UInt_t i=0; i<4; i++) {
    fDataRange[i] = -1; // undefined data bin range
  }
  fFitRangeInBins = false; // default is that fit range is given in time NOT bins
  fFitRange[0] = PMUSR_UNDEFINED; // undefined start fit range
  fFitRange[1] = PMUSR_UNDEFINED; // undefined end fit range
  fFitRangeOffset[0] = -1; // undefined start fit range offset
  fFitRangeOffset[1] = -1; // undefined end fit range offset
  fPacking = -1; // undefined packing/rebinning
}

//--------------------------------------------------------------------------
// GetRRFFreq (public)
//--------------------------------------------------------------------------
/**
 * <p> get RRF frequency value in specific units. If units is unknown, 0.0 will be returned.
 *
 * \param unit unit string in which the units shall be given
 */
Double_t PMsrGlobalBlock::GetRRFFreq(const char *unit)
{
  Double_t freq = 0.0;

  // check that the units given make sense
  TString unitStr = unit;
  Int_t unitTag = RRF_UNIT_UNDEF;
  if (!unitStr.CompareTo("MHz", TString::kIgnoreCase))
    unitTag = RRF_UNIT_MHz;
  else if (!unitStr.CompareTo("Mc", TString::kIgnoreCase))
    unitTag = RRF_UNIT_Mcs;
  else if (!unitStr.CompareTo("T", TString::kIgnoreCase))
    unitTag = RRF_UNIT_T;
  else {
    cerr << endl << ">> PMsrGlobalBlock::GetRRFFreq: **ERROR** found undefined RRF unit '" << unit << "'!" << endl;
    return freq;
  }

  // calc the conversion factor
  if (unitTag == fRRFUnitTag)
    freq = fRRFFreq;
  else if ((unitTag == RRF_UNIT_MHz) && (fRRFUnitTag == RRF_UNIT_Mcs))
    freq = fRRFFreq/TMath::TwoPi();
  else if ((unitTag == RRF_UNIT_MHz) && (fRRFUnitTag == RRF_UNIT_T))
    freq = fRRFFreq*1e4*GAMMA_BAR_MUON; // 1e4 need for T -> G since GAMMA_BAR_MUON is given in MHz/G
  else if ((unitTag == RRF_UNIT_Mcs) && (fRRFUnitTag == RRF_UNIT_MHz))
    freq = fRRFFreq*TMath::TwoPi();
  else if ((unitTag == RRF_UNIT_Mcs) && (fRRFUnitTag == RRF_UNIT_T))
    freq = fRRFFreq*1e4*TMath::TwoPi()*GAMMA_BAR_MUON; // 1e4 need for T -> G since GAMMA_BAR_MUON is given in MHz/G
  else if ((unitTag == RRF_UNIT_T) && (fRRFUnitTag == RRF_UNIT_MHz))
    freq = fRRFFreq/GAMMA_BAR_MUON*1e-4; // 1e-4 need for G -> T since GAMMA_BAR_MUON is given in MHz/G
  else if ((unitTag == RRF_UNIT_T) && (fRRFUnitTag == RRF_UNIT_Mcs))
    freq = fRRFFreq/(TMath::TwoPi()*GAMMA_BAR_MUON)*1e-4; // 1e-4 need for G -> T since GAMMA_BAR_MUON is given in MHz/G

  return freq;
}

//--------------------------------------------------------------------------
// SetRRFFreq (public)
//--------------------------------------------------------------------------
/**
 * <p> set RRF frequency value in specific units. If units is unknown, 0.0 will be set.
 *
 * \param RRF frequency value
 * \param unit unit string in which the units shall be given
 */
void PMsrGlobalBlock::SetRRFFreq(Double_t freq, const char *unit)
{
  // check that the units given make sense
  TString unitStr = unit;
  Int_t unitTag = RRF_UNIT_UNDEF;
  if (!unitStr.CompareTo("MHz", TString::kIgnoreCase))
    unitTag = RRF_UNIT_MHz;
  else if (!unitStr.CompareTo("Mc", TString::kIgnoreCase))
    unitTag = RRF_UNIT_Mcs;
  else if (!unitStr.CompareTo("T", TString::kIgnoreCase))
    unitTag = RRF_UNIT_T;
  else {
    cerr << endl << ">> PMsrGlobalBlock::SetRRFFreq: **ERROR** found undefined RRF unit '" << unit << "'!";
    cerr << endl << ">>                              Will set RRF frequency to 0.0." << endl;
    fRRFFreq = 0.0;
    fRRFUnitTag = RRF_UNIT_UNDEF;
  }

  fRRFFreq = freq;
  fRRFUnitTag = unitTag;
}

//--------------------------------------------------------------------------
// GetRRFUnit (public)
//--------------------------------------------------------------------------
/**
 * <p> returns RRF frequency unit.
 */
TString PMsrGlobalBlock::GetRRFUnit()
{
  TString unit;

  switch (fRRFUnitTag) {
    case RRF_UNIT_UNDEF:
      unit = TString("??");
      break;
    case RRF_UNIT_kHz:
      unit = TString("kHz");
      break;
    case RRF_UNIT_MHz:
      unit = TString("MHz");
      break;
    case RRF_UNIT_Mcs:
      unit = TString("Mc");
      break;
    case RRF_UNIT_G:
      unit = TString("G");
      break;
    case RRF_UNIT_T:
      unit = TString("T");
      break;
    default:
      unit = TString("??");
      break;
  }

  return unit;
}

//--------------------------------------------------------------------------
// SetRRFPacking (public)
//--------------------------------------------------------------------------
/**
 * <p> set RRF packing.
 *
 * \param RRF packing
 */
void PMsrGlobalBlock::SetRRFPacking(Int_t pack)
{
  if (pack <= 0) {
    cerr << endl << "PMsrGlobalBlock::SetRRFPacking: **WARNING** found RRF packing <= 0. Likely doesn't make any sense." << endl;
    fRRFPacking = -1; // set to undefined
  }

  fRRFPacking = pack;
}

//--------------------------------------------------------------------------
// GetDataRange (public)
//--------------------------------------------------------------------------
/**
 * <p> get data range at position idx
 *
 * <b>return:</b>
 * - data range value, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the data range to be returned
 */
Int_t PMsrGlobalBlock::GetDataRange(UInt_t idx)
{
  if (idx >= 4)
    return -1;

  return fDataRange[idx];
}

//--------------------------------------------------------------------------
// SetDataRange (public)
//--------------------------------------------------------------------------
/**
 * <p> set data range element at position idx
 *
 * \param ival data range element
 * \param idx index of the data range element to be set.
 */
void PMsrGlobalBlock::SetDataRange(Int_t ival, Int_t idx)
{
  if (idx >= 4) {
    cerr << endl << ">> PMsrGlobalBlock::SetDataRange: **WARNING** idx=" << idx << ", only idx=0..3 are sensible.";
    cerr << endl;
    return;
  }

  fDataRange[idx] = ival;
}

//--------------------------------------------------------------------------
// GetT0Bin (public)
//--------------------------------------------------------------------------
/**
 * <p> get T0 bin at position idx
 *
 * <b>return:</b>
 * - T0 bin, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the T0 bin to be returned
 */
Double_t PMsrGlobalBlock::GetT0Bin(UInt_t idx)
{
  if (idx >= fT0.size())
    return -1;

  return fT0[idx];
}

//--------------------------------------------------------------------------
// SetT0Bin (public)
//--------------------------------------------------------------------------
/**
 * <p> set T0 bin at position idx
 *
 * \param ival T0 bin
 * \param idx index of the T0 bin to be set. If idx==-1, append value
 */
void PMsrGlobalBlock::SetT0Bin(Double_t dval, Int_t idx)
{
  if (idx == -1) {
    fT0.push_back(dval);
    return;
  }

  if (idx >= static_cast<Int_t>(fT0.size()))
    fT0.resize(idx+1);

  fT0[idx] = dval;
}

//--------------------------------------------------------------------------
// GetAddT0BinSize (public)
//--------------------------------------------------------------------------
/**
 * <p> get add T0 size of the addrun at index addRunIdx
 *
 * <b>return:</b>
 * - addt0 value, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param addRunIdx index of the addrun
 */
Int_t PMsrGlobalBlock::GetAddT0BinSize(UInt_t addRunIdx)
{
  if (fAddT0.empty())
    return -1;

  if (addRunIdx >= fAddT0.size())
    return -1;

  return fAddT0[addRunIdx].size();
}

//--------------------------------------------------------------------------
// GetAddT0Bin (public)
//--------------------------------------------------------------------------
/**
 * <p> get add T0 of the addrun (index addRunIdx) at index histoIdx
 *
 * <b>return:</b>
 * - addt0 value, if indices are within proper boundaries
 * - -1, otherwise
 *
 * \param addRunIdx index of the addrun
 * \param histoIdx index of the add backward histo number value
 */
Double_t PMsrGlobalBlock::GetAddT0Bin(UInt_t addRunIdx, UInt_t histoIdx)
{
  if (fAddT0.empty())
    return -1.0;

  if (addRunIdx >= fAddT0.size())
    return -1.0;

  if (fAddT0[addRunIdx].empty())
    return -1.0;

  if (histoIdx >= fAddT0[addRunIdx].size())
    return -1.0;

  return fAddT0[addRunIdx][histoIdx];
}

//--------------------------------------------------------------------------
// SetAddT0Bin (public)
//--------------------------------------------------------------------------
/**
 * <p> set add T0 bin of the addrun at index histoNoIdx
 *
 * \param ival T0 bin
 * \param addRunIdx addrun index
 * \param histoNoIdx index whithin the fAddT0 vector where to set the value.
 */
void PMsrGlobalBlock::SetAddT0Bin(Double_t dval, UInt_t addRunIdx, UInt_t histoNoIdx)
{
  if (addRunIdx >= fAddT0.size())
    fAddT0.resize(addRunIdx+1);

  if (histoNoIdx >= fAddT0[addRunIdx].size())
    fAddT0[addRunIdx].resize(histoNoIdx+1);

  fAddT0[addRunIdx][histoNoIdx] = dval;
}

//--------------------------------------------------------------------------
// GetFitRange (public)
//--------------------------------------------------------------------------
/**
 * <p> get fit range value at position idx. idx: 0=fit range start, 1=fit range end.
 *
 * <b>return:</b>
 * - fit range value, if idx is within proper boundaries
 * - PMUSR_UNDEFINED, otherwise
 *
 * \param idx index of the fit range value to be returned
 */
Double_t PMsrGlobalBlock::GetFitRange(UInt_t idx)
{
  if (idx >= 2)
    return PMUSR_UNDEFINED;

  return fFitRange[idx];
}

//--------------------------------------------------------------------------
// SetFitRange (public)
//--------------------------------------------------------------------------
/**
 * <p> set fit range value at position idx
 *
 * \param dval value to be set
 * \param idx index of the fit range value to be set
 */
void PMsrGlobalBlock::SetFitRange(Double_t dval, UInt_t idx)
{
  if (idx >= 2)
    return;

  fFitRange[idx] = dval;
}

//--------------------------------------------------------------------------
// GetFitRangeOffset (public)
//--------------------------------------------------------------------------
/**
 * <p> get fit range offset value at position idx. idx: 0=fit range offset start, 1=fit range offset end.
 *
 * <b>return:</b>
 * - fit range offset value, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the fit range value to be returned
 */
Int_t PMsrGlobalBlock::GetFitRangeOffset(UInt_t idx)
{
  if (idx >= 2)
    return -1;

  return fFitRangeOffset[idx];
}

//--------------------------------------------------------------------------
// SetFitRangeOffset (public)
//--------------------------------------------------------------------------
/**
 * <p> set fit range offset value at position idx. Illegale values will be ignored.
 *
 * \param ival value to be set
 * \param idx index of the fit range value to be set
 */
void PMsrGlobalBlock::SetFitRangeOffset(Int_t ival, UInt_t idx)
{
  if (idx >= 2)
    return;

  fFitRangeOffset[idx] = ival;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implementation PMsrRunBlock
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// PMsrRunBlock
//--------------------------------------------------------------------------
/**
 * <p> Constructor
 */
PMsrRunBlock::PMsrRunBlock()
{
  fFitType = -1; // undefined fit type  
  fAlphaParamNo = -1; // undefined alpha parameter number
  fBetaParamNo = -1; // undefined beta parameter number
  fNormParamNo = -1; // undefined norm parameter number
  fBkgFitParamNo = -1; // undefined background parameter number
  fLifetimeParamNo = -1; // undefined lifetime parameter number
  fLifetimeCorrection = false; // lifetime correction == false by default (used in single histogram musrview)
  for (UInt_t i=0; i<2; i++) {
    fBkgEstimated[i] = PMUSR_UNDEFINED;
    fBkgFix[i] = PMUSR_UNDEFINED;
  }
  for (UInt_t i=0; i<4; i++) {
    fBkgRange[i] = -1; // undefined start background range
    fDataRange[i] = -1; // undefined start data range
  }
  fFitRangeInBins = false; // default is that fit range is given in time NOT bins
  fFitRange[0] = PMUSR_UNDEFINED; // undefined start fit range
  fFitRange[1] = PMUSR_UNDEFINED; // undefined end fit range
  fFitRangeOffset[0] = -1; // undefined start fit range offset
  fFitRangeOffset[1] = -1; // undefined end fit range offset
  fPacking = -1; // undefined packing
  fXYDataIndex[0] = -1; // undefined x data index (NonMusr)
  fXYDataIndex[1] = -1; // undefined y data index (NonMusr)
  fXYDataLabel[0] = TString(""); // undefined x data label (NonMusr)
  fXYDataLabel[1] = TString(""); // undefined y data label (NonMusr)
}

//--------------------------------------------------------------------------
// ~PMsrRunBlock
//--------------------------------------------------------------------------
/**
 * <p> Destructor
 */
PMsrRunBlock::~PMsrRunBlock()
{
  fRunName.clear();
  fBeamline.clear();
  fInstitute.clear();
  fFileFormat.clear();
  fForwardHistoNo.clear();
  fBackwardHistoNo.clear();
  fMap.clear();
  fT0.clear();
  fParGlobal.clear();
  fMapGlobal.clear();
}

//--------------------------------------------------------------------------
// CleanUp (public)
//--------------------------------------------------------------------------
/**
 * <p> Clean up data structure.
 */
void PMsrRunBlock::CleanUp()
{
  fFitType = -1; // undefined fit type
  fAlphaParamNo = -1; // undefined alpha parameter number
  fBetaParamNo = -1; // undefined beta parameter number
  fNormParamNo = -1; // undefined norm parameter number
  fBkgFitParamNo = -1; // undefined background parameter number
  fLifetimeParamNo = -1; // undefined lifetime parameter number
  fLifetimeCorrection = false; // lifetime correction == false by default (used in single histogram musrview)
  fBkgFix[0] = PMUSR_UNDEFINED; // undefined fixed background for forward
  fBkgFix[1] = PMUSR_UNDEFINED; // undefined fixed background for backward
  for (UInt_t i=0; i<4; i++) {
    fBkgRange[i] = -1; // undefined background range
    fDataRange[i] = -1; // undefined data range
  }
  fFitRangeInBins = false; // default is that fit range is given in time NOT bins
  fFitRange[0] = PMUSR_UNDEFINED; // undefined start fit range
  fFitRange[1] = PMUSR_UNDEFINED; // undefined end fit range
  fFitRangeOffset[0] = -1; // undefined start fit range offset
  fFitRangeOffset[1] = -1; // undefined end fit range offset
  fPacking = -1; // undefined packing
  fXYDataIndex[0] = -1; // undefined x data index (NonMusr)
  fXYDataIndex[1] = -1; // undefined y data index (NonMusr)
  fXYDataLabel[0] = TString(""); // undefined x data label (NonMusr)
  fXYDataLabel[1] = TString(""); // undefined y data label (NonMusr)

  fRunName.clear();
  fBeamline.clear();
  fInstitute.clear();
  fFileFormat.clear();
  fForwardHistoNo.clear();
  fBackwardHistoNo.clear();
  fMap.clear();
  fT0.clear();
  for (UInt_t i=0; i<fAddT0.size(); i++)
    fAddT0[i].clear();
  fAddT0.clear();

  fParGlobal.clear();
  fMapGlobal.clear();
}

//--------------------------------------------------------------------------
// GetRunName (public)
//--------------------------------------------------------------------------
/**
 * <p> get run name at position idx
 *
 * <b>return:</b>
 * - pointer of the run name string, if idx is within proper boundaries
 * - 0, otherwise
 *
 * \param idx index of the run name to be returned
 */
TString* PMsrRunBlock::GetRunName(UInt_t idx)
{
  if (idx>fRunName.size())
    return 0;

  return &fRunName[idx];
}

//--------------------------------------------------------------------------
// SetRunName (public)
//--------------------------------------------------------------------------
/**
 * <p> set run name at position idx
 *
 * \param str run name string
 * \param idx index of the run name to be set. If idx == -1 append run name
 */
void PMsrRunBlock::SetRunName(TString &str, Int_t idx)
{
  if (idx == -1) {
    fRunName.push_back(str);
    return;
  }

  if (idx >= static_cast<Int_t>(fRunName.size()))
    fRunName.resize(idx+1);

  fRunName[idx] = str;
}

//--------------------------------------------------------------------------
// GetBeamline (public)
//--------------------------------------------------------------------------
/**
 * <p> get beamline name at position idx
 *
 * <b>return:</b>
 * - pointer of the beamline string, if idx is within proper boundaries
 * - 0, otherwise
 *
 * \param idx index of the beamline to be returned
 */
TString* PMsrRunBlock::GetBeamline(UInt_t idx)
{
  if (idx>fBeamline.size())
    return 0;

  return &fBeamline[idx];
}

//--------------------------------------------------------------------------
// SetBeamline (public)
//--------------------------------------------------------------------------
/**
 * <p> set beamline name at position idx
 *
 * \param str beamline name string
 * \param idx index of the beamline name to be set. If idx == -1, append beamline
 */
void PMsrRunBlock::SetBeamline(TString &str, Int_t idx)
{
  if (idx == -1) {
    fBeamline.push_back(str);
    return;
  }

  if (idx >= static_cast<Int_t>(fBeamline.size()))
    fBeamline.resize(idx+1);

  fBeamline[idx] = str;
}

//--------------------------------------------------------------------------
// GetInstitute (public)
//--------------------------------------------------------------------------
/**
 * <p> get institute name at position idx
 *
 * <b>return:</b>
 * - pointer of the institute string, if idx is within proper boundaries
 * - 0, otherwise
 *
 * \param idx index of the institute to be returned
 */
TString* PMsrRunBlock::GetInstitute(UInt_t idx)
{
  if (idx>fInstitute.size())
    return 0;

  return &fInstitute[idx];
}

//--------------------------------------------------------------------------
// SetInstitute (public)
//--------------------------------------------------------------------------
/**
 * <p> set institute name at position idx
 *
 * \param str institute name string
 * \param idx index of the run name to be set. If idx == -1, append institute name
 */
void PMsrRunBlock::SetInstitute(TString &str, Int_t idx)
{
  if (idx == -1) {
    fInstitute.push_back(str);
    return;
  }

  if (idx >= static_cast<Int_t>(fInstitute.size()))
    fInstitute.resize(idx+1);

  fInstitute[idx] = str;
}

//--------------------------------------------------------------------------
// GetFileFormat (public)
//--------------------------------------------------------------------------
/**
 * <p> get file format name at position idx
 *
 * <b>return:</b>
 * - pointer of the file format string, if idx is within proper boundaries
 * - 0, otherwise
 *
 * \param idx index of the file format to be returned
 */
TString* PMsrRunBlock::GetFileFormat(UInt_t idx)
{
  if (idx>fFileFormat.size())
    return 0;

  return &fFileFormat[idx];
}

//--------------------------------------------------------------------------
// SetFileFormat (public)
//--------------------------------------------------------------------------
/**
 * <p> set file format name at position idx
 *
 * \param str file format string
 * \param idx index of the file format name to be set. If idx == -1, append file format
 */
void PMsrRunBlock::SetFileFormat(TString &str, Int_t idx)
{
  if (idx == -1) {
    fFileFormat.push_back(str);
    return;
  }

  if (idx >= static_cast<Int_t>(fFileFormat.size()))
    fFileFormat.resize(idx+1);

  fFileFormat[idx] = str;
}

//--------------------------------------------------------------------------
// GetForwardHistoNo (public)
//--------------------------------------------------------------------------
/**
 * <p> get forward histogram value at position idx
 *
 * <b>return:</b>
 * - forward histogram number, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the forward histogram value to be returned
 */
Int_t PMsrRunBlock::GetForwardHistoNo(UInt_t idx)
{
  if (fForwardHistoNo.empty())
    return -1;

  if (idx>fForwardHistoNo.size())
    return -1;

  return fForwardHistoNo[idx];
}

//--------------------------------------------------------------------------
// SetForwardHistoNo (public)
//--------------------------------------------------------------------------
/**
 * <p> set forward histogram value at index idx
 *
 * \param histoNo histogram value
 * \param idx index whithin the fForwardHistoNo vector where to set the value.
 *            If idx == -1, then append the value to the vector.
 */
void PMsrRunBlock::SetForwardHistoNo(Int_t histoNo, Int_t idx)
{
  if (idx == -1) { // i.e. append forward histo no
    fForwardHistoNo.push_back(histoNo);
  } else {
    if (idx >= static_cast<Int_t>(fForwardHistoNo.size()))
      fForwardHistoNo.resize(idx+1);
    fForwardHistoNo[idx] = histoNo;
  }
}

//--------------------------------------------------------------------------
// GetBackwardHistoNo (public)
//--------------------------------------------------------------------------
/**
 * <p> get backward histogram value at position idx
 *
 * <b>return:</b>
 * - backward histogram number, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the map value to be returned
 */
Int_t PMsrRunBlock::GetBackwardHistoNo(UInt_t idx)
{
  if (fBackwardHistoNo.empty())
    return -1;

  if (idx>fBackwardHistoNo.size())
    return -1;

  return fBackwardHistoNo[idx];
}

//--------------------------------------------------------------------------
// SetBackwardHistoNo (public)
//--------------------------------------------------------------------------
/**
 * <p> set backward histogram value at position pos
 *
 * \param histoNo histogram value
 * \param idx index whithin the fBackwardHistoNo vector where to set the value.
 *            If idx == -1, then append the value to the vector.
 */
void PMsrRunBlock::SetBackwardHistoNo(Int_t histoNo, Int_t idx)
{
  if (idx == -1) { // i.e. append forward histo no
    fBackwardHistoNo.push_back(histoNo);
  } else {
    if (idx >= static_cast<Int_t>(fBackwardHistoNo.size()))
      fBackwardHistoNo.resize(idx+1);
    fBackwardHistoNo[idx] = histoNo;
  }
}

//--------------------------------------------------------------------------
// GetMap (public)
//--------------------------------------------------------------------------
/**
 * <p> get map value at position idx
 *
 * <b>return:</b>
 * - map value, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the map value to be returned
 */
Int_t PMsrRunBlock::GetMap(UInt_t idx)
{
  if (idx>fMap.size())
    return -1;

  return fMap[idx];
}

//--------------------------------------------------------------------------
// SetMap (public)
//--------------------------------------------------------------------------
/**
 * <p> set map value at position idx
 *
 * \param mapVal map value
 * \param idx index of the map value to be set. If idx == -1, append map value.
 */
void PMsrRunBlock::SetMap(Int_t mapVal, Int_t idx)
{
  if (idx == -1) {
    fMap.push_back(mapVal);
    return;
  }

  if (idx >= static_cast<Int_t>(fMap.size()))
    fMap.resize(idx+1);

  fMap[idx] = mapVal;
}

//--------------------------------------------------------------------------
// GetBkgEstimated (public)
//--------------------------------------------------------------------------
/**
 * <p> get estimated background value at position idx. If not present,
 * PMUSR_UNDEFINED is returned.
 *
 * <b>return:</b>
 * - estimated background value, if idx is within proper boundaries
 * - PMUSR_UNDEFINED, otherwise
 *
 * \param idx index of the estimated background value to be returned
 */
Double_t PMsrRunBlock::GetBkgEstimated(UInt_t idx)
{
  if (idx >= 2)
    return PMUSR_UNDEFINED;

  return fBkgEstimated[idx];
}


//--------------------------------------------------------------------------
// SetBkgEstimated (public)
//--------------------------------------------------------------------------
/**
 * <p> set estimated background value at position idx
 *
 * \param dval estimated background value
 * \param idx index of the estimated background value to be set.
 */
void PMsrRunBlock::SetBkgEstimated(Double_t dval, Int_t idx)
{
  if (idx >= 2) {
    cerr << endl << ">> PMsrRunBlock::SetBkgEstimated: **WARNING** idx=" << idx << ", only idx=0,1 are sensible.";
    cerr << endl;
    return;
  }

  fBkgEstimated[idx] = dval;
}

//--------------------------------------------------------------------------
// GetBkgFix (public)
//--------------------------------------------------------------------------
/**
 * <p> get background fixed value at position idx
 *
 * <b>return:</b>
 * - fixed background value, if idx is within proper boundaries
 * - PMUSR_UNDEFINED, otherwise
 *
 * \param idx index of the background fixed value to be returned
 */
Double_t PMsrRunBlock::GetBkgFix(UInt_t idx)
{
  if (idx >= 2)
    return PMUSR_UNDEFINED;

  return fBkgFix[idx];
}

//--------------------------------------------------------------------------
// SetBkgFix (public)
//--------------------------------------------------------------------------
/**
 * <p> set background fixed value at position idx
 *
 * \param dval fixed background value
 * \param idx index of the background fixed value to be set.
 */
void PMsrRunBlock::SetBkgFix(Double_t dval, Int_t idx)
{
  if (idx >= 2) {
    cerr << endl << ">> PMsrRunBlock::SetBkgFix: **WARNING** idx=" << idx << ", only idx=0,1 are sensible.";
    cerr << endl;
    return;
  }

  fBkgFix[idx] = dval;
}

//--------------------------------------------------------------------------
// GetBkgRange (public)
//--------------------------------------------------------------------------
/**
 * <p> get background range at position idx
 *
 * <b>return:</b>
 * - background range value, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the background range to be returned
 */
Int_t PMsrRunBlock::GetBkgRange(UInt_t idx)
{
  if (idx >= 4) {
    return -1;
  }

  return fBkgRange[idx];
}

//--------------------------------------------------------------------------
// SetBkgRange (public)
//--------------------------------------------------------------------------
/**
 * <p> set background range element at position idx
 *
 * \param ival background bin value
 * \param idx index of the background range element to be set. If idx==-1, append value.
 */
void PMsrRunBlock::SetBkgRange(Int_t ival, Int_t idx)
{
  if (idx >= 4) {
    cerr << endl << ">> PMsrRunBlock::SetBkgRange: **WARNING** idx=" << idx << ", only idx=0..3 are sensible.";
    cerr << endl;
    return;
  }

  fBkgRange[idx] = ival;
}


//--------------------------------------------------------------------------
// GetDataRange (public)
//--------------------------------------------------------------------------
/**
 * <p> get data range at position idx
 *
 * <b>return:</b>
 * - data range value, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the data range to be returned
 */
Int_t PMsrRunBlock::GetDataRange(UInt_t idx)
{
  if (idx >= 4) {
    return -1;
  }

  return fDataRange[idx];
}

//--------------------------------------------------------------------------
// SetDataRange (public)
//--------------------------------------------------------------------------
/**
 * <p> set data range element at position idx
 *
 * \param ival data range element
 * \param idx index of the data range element to be set. If idx==-1, append value
 */
void PMsrRunBlock::SetDataRange(Int_t ival, Int_t idx)
{
  if (idx >= 4) {
    cerr << endl << ">> PMsrRunBlock::SetDataRange: **WARNING** idx=" << idx << ", only idx=0..3 are sensible.";
    cerr << endl;
    return;
  }

  fDataRange[idx] = ival;
}

//--------------------------------------------------------------------------
// GetT0Bin (public)
//--------------------------------------------------------------------------
/**
 * <p> get T0 bin at position idx
 *
 * <b>return:</b>
 * - T0 bin, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the T0 bin to be returned
 */
Double_t PMsrRunBlock::GetT0Bin(UInt_t idx)
{
  if (idx >= fT0.size())
    return -1;

  return fT0[idx];
}

//--------------------------------------------------------------------------
// SetT0Bin (public)
//--------------------------------------------------------------------------
/**
 * <p> set T0 bin at position idx
 *
 * \param ival T0 bin
 * \param idx index of the T0 bin to be set. If idx==-1, append value
 */
void PMsrRunBlock::SetT0Bin(Double_t dval, Int_t idx)
{
  if (idx == -1) {
    fT0.push_back(dval);
    return;
  }

  if (idx >= static_cast<Int_t>(fT0.size()))
    fT0.resize(idx+1);

  fT0[idx] = dval;
}

//--------------------------------------------------------------------------
// GetAddT0BinSize (public)
//--------------------------------------------------------------------------
/**
 * <p> get add T0 size of the addrun at index addRunIdx
 *
 * <b>return:</b>
 * - addt0 value, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param addRunIdx index of the addrun
 */
Int_t PMsrRunBlock::GetAddT0BinSize(UInt_t addRunIdx)
{
  if (fAddT0.empty())
    return -1;

  if (addRunIdx >= fAddT0.size())
    return -1;

  return fAddT0[addRunIdx].size();
}

//--------------------------------------------------------------------------
// GetAddT0Bin (public)
//--------------------------------------------------------------------------
/**
 * <p> get add T0 of the addrun (index addRunIdx) at index histoIdx
 *
 * <b>return:</b>
 * - addt0 value, if indices are within proper boundaries
 * - -1, otherwise
 *
 * \param addRunIdx index of the addrun
 * \param histoIdx index of the add backward histo number value
 */
Double_t PMsrRunBlock::GetAddT0Bin(UInt_t addRunIdx, UInt_t histoIdx)
{
  if (fAddT0.empty())
    return -1.0;

  if (addRunIdx >= fAddT0.size())
    return -1.0;

  if (fAddT0[addRunIdx].empty())
    return -1.0;

  if (histoIdx >= fAddT0[addRunIdx].size())
    return -1.0;

  return fAddT0[addRunIdx][histoIdx];
}

//--------------------------------------------------------------------------
// SetAddT0Bin (public)
//--------------------------------------------------------------------------
/**
 * <p> set add T0 bin of the addrun at index histoNoIdx
 *
 * \param ival T0 bin
 * \param addRunIdx addrun index
 * \param histoNoIdx index whithin the fAddT0 vector where to set the value.
 */
void PMsrRunBlock::SetAddT0Bin(Double_t dval, UInt_t addRunIdx, UInt_t histoNoIdx)
{
  if (addRunIdx >= fAddT0.size())
    fAddT0.resize(addRunIdx+1);

  if (histoNoIdx >= fAddT0[addRunIdx].size())
    fAddT0[addRunIdx].resize(histoNoIdx+1);

  fAddT0[addRunIdx][histoNoIdx] = dval;
}

//--------------------------------------------------------------------------
// GetFitRange (public)
//--------------------------------------------------------------------------
/**
 * <p> get fit range value at position idx. idx: 0=fit range start, 1=fit range end.
 *
 * <b>return:</b>
 * - fit range value, if idx is within proper boundaries
 * - PMUSR_UNDEFINED, otherwise
 *
 * \param idx index of the fit range value to be returned
 */
Double_t PMsrRunBlock::GetFitRange(UInt_t idx)
{
  if (idx >= 2)
    return PMUSR_UNDEFINED;

  return fFitRange[idx];
}

//--------------------------------------------------------------------------
// SetFitRange (public)
//--------------------------------------------------------------------------
/**
 * <p> set fit range value at position idx
 *
 * \param dval value to be set
 * \param idx index of the fit range value to be set
 */
void PMsrRunBlock::SetFitRange(Double_t dval, UInt_t idx)
{
  if (idx >= 2)
    return;

  fFitRange[idx] = dval;
}

//--------------------------------------------------------------------------
// GetFitRangeOffset (public)
//--------------------------------------------------------------------------
/**
 * <p> get fit range offset value at position idx. idx: 0=fit range offset start, 1=fit range offset end.
 *
 * <b>return:</b>
 * - fit range offset value, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the fit range value to be returned
 */
Int_t PMsrRunBlock::GetFitRangeOffset(UInt_t idx)
{
  if (idx >= 2)
    return -1;

  return fFitRangeOffset[idx];
}

//--------------------------------------------------------------------------
// SetFitRangeOffset (public)
//--------------------------------------------------------------------------
/**
 * <p> set fit range offset value at position idx. Illegale values will be ignored.
 *
 * \param ival value to be set
 * \param idx index of the fit range value to be set
 */
void PMsrRunBlock::SetFitRangeOffset(Int_t ival, UInt_t idx)
{
  if (idx >= 2)
    return;

  fFitRangeOffset[idx] = ival;
}

//--------------------------------------------------------------------------
// SetParGlobal (public)
//--------------------------------------------------------------------------
/**
 * <p> store the information that a certain parameter used in the block is global
 *
 * \param str key (label) telling how the parameter is addressed
 * \param ival value to be set (global == 1, run specific == 0, tag not present == -1)
 */
void PMsrRunBlock::SetParGlobal(const TString &str, Int_t ival)
{
  fParGlobal[str] = ival; // will either create a new entry or overwrite an old one if the key "str" is present
  return;
}

//--------------------------------------------------------------------------
// SetMapGlobal (public)
//--------------------------------------------------------------------------
/**
 * <p> store the information that a certain mapped parameter in the block is global
 *
 * \param idx map-index (0, 1, 2, 3, ...)
 * \param ival value to be set (global == 1, run specific == 0, tag not present == -1)
 */
void PMsrRunBlock::SetMapGlobal(UInt_t idx, Int_t ival)
{
  if (fMapGlobal.size() != fMap.size())
    fMapGlobal.resize(fMap.size(), -1);
  if (idx < fMap.size() && fMap[idx] > 0)
    fMapGlobal[idx] = ival;
  // else do nothing at the moment
  return;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implementation PStringNumberList
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Parse (public)
//--------------------------------------------------------------------------
/**
 * <p>Helper class which parses list of numbers of the following 3 forms and its combination.
 * (i) list of integers separted by spaces, e.g. 1 3 7 14
 * (ii) a range of integers of the form nS-nE, e.g. 13-27 which will generate 13, 14, 15, .., 26, 27
 * (iii) a sequence of integers of the form nS:nE:nStep, e.g. 10:20:2  which will generate 10, 12, 14, .., 18, 20
 *
 * \param errorMsg error message
 * \param ignoreFirstToken if true, the first parse token will be ignored
 *
 * @return true if parse has been successful, otherwise false
 */
bool PStringNumberList::Parse(string &errorMsg, bool ignoreFirstToken)
{
  bool result=true;
  vector<string> splitVec;
  int ival;

  // before checking tokens, remove 'forbidden' " - " and " : "
  StripSpaces();

  // split string into space separated tokens
  split(splitVec, fString, is_any_of(" "), token_compress_on);

  unsigned int start=0;
  if (ignoreFirstToken)
    start=1;

  for (unsigned int i=start; i<splitVec.size(); i++) {
    if (splitVec[i].length() != 0) { // ignore empty tokens
      if (splitVec[i].find("-") != string::npos) { // check for potential range
        vector<string> subSplitVec;
        // split potential nS-nE token
        split(subSplitVec, splitVec[i], is_any_of("-"),  token_compress_on);

        int start=-1, end=-1;
        unsigned int count=0;
        for (unsigned int j=0; j<subSplitVec.size(); j++) {
          if (subSplitVec[j].length() != 0) { // ignore empty tokens
            if (!IsNumber(subSplitVec[j])) {
              result = false;
            } else {
              count++;
              if (count == 1)
                start = atoi(subSplitVec[j].c_str());
              else if (count == 2)
                end = atoi(subSplitVec[j].c_str());
              else
                result = false;
            }
          }
        }
        if ((start < 0) || (end < 0)) { // check that there is a vaild start and end
          errorMsg = "**ERROR** start or end of a range is not valid";
          result = false;
        }
        if (result) { // no error, hence check start and end
          if (start > end) {
            int swap = end;
            cerr << "**WARNING** start=" << start << " > end=" << end << ", hence I will swap them" << endl;
            end = start;
            start = swap;
          }
          for (int j=start; j<=end; j++)
            fList.push_back(j);
        }
      } else if (splitVec[i].find(":") != string::npos) { // check for potential sequence
        vector<string> subSplitVec;
        // split potential rStart:rEnd:rStep token
        split(subSplitVec, splitVec[i], is_any_of(":"),  token_compress_on);

        int start=-1, end=-1, step=-1;
        unsigned int count=0;
        for (unsigned int j=0; j<subSplitVec.size(); j++) {
          if (subSplitVec[j].length() != 0) { // ignore empty tokens
            if (!IsNumber(subSplitVec[j])) {
              result = false;
            } else {
              count++;
              if (count == 1)
                start = atoi(subSplitVec[j].c_str());
              else if (count == 2)
                end = atoi(subSplitVec[j].c_str());
              else if (count == 3)
                step = atoi(subSplitVec[j].c_str());
              else
                result = false;
            }
          }
        }
        if ((start < 0) || (end < 0) || (step < 0)) { // check that there is a vaild start and end
          errorMsg = "**ERROR** start, end, or step of a sequence is not valid";
          result = false;
        }
        if (result) { // no error, hence check start and end
          if (start > end) {
            int swap = end;
            cerr << "**WARNING** start=" << start << " > end=" << end << ", hence I will swap them" << endl;
            end = start;
            start = swap;
          }
          for (int j=start; j<=end; j+=step)
            fList.push_back(j);
        }
      } else if (IsNumber(splitVec[i])) {
        ival = atoi(splitVec[i].c_str());
        fList.push_back(ival);
      } else {
        errorMsg = "**ERROR** invalid token: " + splitVec[i];
        result = false;
      }
    }
  }

  return result;
}

//--------------------------------------------------------------------------
// StripSpaces (private)
//--------------------------------------------------------------------------
/**
 * <p>This routine removes arbitray number of spaces between '-' and ':',
 * e.g. 123    -    125  will be converted to 123-125, etc.
 */
void PStringNumberList::StripSpaces()
{
  string str=fString;
  int pos=-1;

  // backward scan
  for (int i=str.size(); i>=0; --i) { // check if first space is found
    if ((str[i] == ' ') && (pos == -1)) {
      pos = i;
    } else if ((str[i] == '-') || (str[i] == ':')) { // check for '-' or ':'
      if (pos != -1) {
        str.erase(i+1, pos-i);
      }
    } else if (str[i] != ' ') { // anything but different than a space leads to a reset of the pos counter
      pos = -1;
    }
  }
  // forward scan
  for (unsigned int i=0; i<str.size(); i++) { // check if first space is found
    if ((str[i] == ' ') && (pos == -1)) {
      pos = i;
    } else if ((str[i] == '-') || (str[i] == ':')) { // check for '-' or ':'
      if (pos != -1) {
        str.erase(pos, i-pos);
        i = pos;
      }
    } else if (str[i] != ' ') { // anything but different than a space leads to a reset of the pos counter
      pos = -1;
    }
  }

  fString = str;
}
