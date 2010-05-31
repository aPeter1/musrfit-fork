/***************************************************************************

  PMusr.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
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
// SetTheoryValue
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
// ReplaceTheory
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
// SetLabel
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
    cerr << endl << "**WARNING** PNonMusrRawRunData::SetLabel: idx=" << idx << " is out of range [0," << fLabels.size() << "[.";
    cerr << endl;
    return;
  }
  fLabels[idx] = str;
}

//--------------------------------------------------------------------------
// AppendSubData
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
    cerr << endl << "**WARNING** PNonMusrRawRunData::AppendSubData: idx=" << idx << " is out of range [0," << fData.size() << "[.";
    cerr << endl;
    return;
  }
  fData[idx].push_back(dval);
}

//--------------------------------------------------------------------------
// AppendSubErrData
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
    cerr << endl << "**WARNING** PNonMusrRawRunData::AppendSubErrData: idx=" << idx << " is out of range [0," << fErrData.size() << "[.";
    cerr << endl;
    return;
  }
  fErrData[idx].push_back(dval);
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
  fRunName = TString("");
  fRunTitle = TString("");
  fSetup = TString("");
  fField = PMUSR_UNDEFINED;
  fEnergy = PMUSR_UNDEFINED;
  fTransport = PMUSR_UNDEFINED;
  fTimeResolution = PMUSR_UNDEFINED;
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
  fT0s.clear();
  fT0Estimated.clear();
  fBkgBin.clear();
  fGoodDataBin.clear();
  for (UInt_t i=0; i<fDataBin.size(); i++)
    fDataBin[i].clear();
  fDataBin.clear();
}

//--------------------------------------------------------------------------
// GetTemperature
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
    cerr << endl << "**WARNING** PRawRunData::GetTemperature: idx=" << idx << " is out of range [0," << fTemp.size() << "[.";
    cerr << endl;
    return PMUSR_UNDEFINED;
  }
  return fTemp[idx].first;
}

//--------------------------------------------------------------------------
// GetTempError
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
    cerr << endl << "**WARNING** PRawRunData::GetTempError: idx=" << idx << " is out of range [0," << fTemp.size() << "[.";
    cerr << endl;
    return PMUSR_UNDEFINED;
  }
  return fTemp[idx].second;
}

//--------------------------------------------------------------------------
// GetRingAnode
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
    cerr << endl << "**WARNING** PRawRunData::GetRingAnode: idx=" << idx << " is out of range [0," << fRingAnode.size() << "[.";
    cerr << endl;
    return PMUSR_UNDEFINED;
  }
  return fRingAnode[idx];
}

//--------------------------------------------------------------------------
// GetT0
//--------------------------------------------------------------------------
/**
 * <p> Returns a T0 value.
 *
 * <b>return:</b>
 * - t0 value, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the T0 value whished
 */
const Int_t PRawRunData::GetT0(const UInt_t idx)
{
  if (idx >= fT0s.size()) {
    cerr << endl << "**WARNING** PRawRunData::GetT0: idx=" << idx << " is out of range [0," << fT0s.size() << "[.";
    cerr << endl;
    return -1;
  }
  return fT0s[idx];
}

//--------------------------------------------------------------------------
// GetT0Estimated
//--------------------------------------------------------------------------
/**
 * <p>Returns an estimated T0 value.
 *
 * <b>return:</b>
 * - estimated t0 value, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the T0 value whished
 */
const Int_t PRawRunData::GetT0Estimated(const UInt_t idx)
{
  if (idx >= fT0Estimated.size()) {
    cerr << endl << "**WARNING** PRawRunData::GetT0Estimated: idx=" << idx << " is out of range [0," << fT0Estimated.size() << "[.";
    cerr << endl;
    return -1;
  }
  return fT0Estimated[idx];
}

//--------------------------------------------------------------------------
// GetBkgBin
//--------------------------------------------------------------------------
/**
 * <p>Returns the background bin range (start, stop) from the data file.
 * Currently only used in mud-files.
 *
 * <b>return:</b>
 * - (start, stop) values, if idx is within proper boundaries
 * - (-1, -1), otherwise
 *
 * \param idx index of the background range.
 */
const PIntPair PRawRunData::GetBkgBin(const UInt_t idx)
{
  PIntPair pair(-1, -1);

  if (idx >= fBkgBin.size()) {
    cerr << endl << "**WARNING** PRawRunData::GetBkgBin: idx=" << idx << " is out of range [0," << fBkgBin.size() << "[.";
    cerr << endl;
    return pair;
  }

  pair.first  = fBkgBin[idx].first;
  pair.second = fBkgBin[idx].second;

  return pair;
}

//--------------------------------------------------------------------------
// GetGoodDataBin
//--------------------------------------------------------------------------
/**
 * <p>Returns the data range (first good bin, last good bin) from the data file.
 * Currently only used in mud-files.
 *
 * <b>return:</b>
 * - (first good bin, last good bin) values, if idx is within proper boundaries
 * - (-1, -1), otherwise
 *
 * \param idx index of the data range
 */
const PIntPair PRawRunData::GetGoodDataBin(const UInt_t idx)
{
  PIntPair pair(-1, -1);

  if (idx >= fGoodDataBin.size()) {
    cerr << endl << "**WARNING** PRawRunData::GetGoodDataBin: idx=" << idx << " is out of range [0," << fGoodDataBin.size() << "[.";
    cerr << endl;
    return pair;
  }

  pair.first  = fGoodDataBin[idx].first;
  pair.second = fGoodDataBin[idx].second;

  return pair;
}

//--------------------------------------------------------------------------
// GetDataBin
//--------------------------------------------------------------------------
/**
 * <p>Returns a raw muSR run histogram.
 *
 * <b>return:</b>
 * - pointer of the data vector, if idx is within proper boundaries
 * - 0, otherwise
 *
 * \param idx histo number index
 */
const PDoubleVector* PRawRunData::GetDataBin(const UInt_t idx)
{
  if (idx >= fDataBin.size()) {
    cerr << endl << "**WARNING** PRawRunData::GetDataBin: idx=" << idx << " is out of range [0," << fDataBin.size() << "[.";
    cerr << endl;
    return 0;
  }

  return &fDataBin[idx];
}


//--------------------------------------------------------------------------
// SetRingAnode
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
// SetTemperature
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
// SetTempError
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

//--------------------------------------------------------------------------
// SetDataBin
//--------------------------------------------------------------------------
/**
 * <p> sets a value in a data set.
 *
 * \param histoNo histogram number index
 * \param bin number index
 * \param dval value to be set
 */
void PRawRunData::SetDataBin(const UInt_t histoNo, const UInt_t bin, const Double_t dval)
{
  if (histoNo > fDataBin.size()) {
    cerr << endl << "**WARNING** PRawRunData::SetDataBin: histoNo=" << histoNo << " is out of range [0," << fDataBin.size() << "].";
    cerr << endl;
    return;
  }

  if (bin > fDataBin[histoNo].size()) {
    cerr << endl << "**WARNING** PRawRunData::SetDataBin: bin=" << bin << " is out of range [0," << fDataBin[histoNo].size() << "].";
    cerr << endl;
    return;
  }

  fDataBin[histoNo][bin] = dval;
}

//--------------------------------------------------------------------------
// AddDataBin
//--------------------------------------------------------------------------
/**
 * <p> add a value within a data set.
 *
 * \param histoNo histogram number index
 * \param bin number index
 * \param dval value to be set
 */
void PRawRunData::AddDataBin(const UInt_t histoNo, const UInt_t bin, const Double_t dval)
{
  if (histoNo > fDataBin.size()) {
    cerr << endl << "**WARNING** PRawRunData::AddDataBin: histoNo=" << histoNo << " is out of range [0," << fDataBin.size() << "].";
    cerr << endl;
    return;
  }

  if (bin > fDataBin[histoNo].size()) {
    cerr << endl << "**WARNING** PRawRunData::AddDataBin: bin=" << bin << " is out of range [0," << fDataBin[histoNo].size() << "].";
    cerr << endl;
    return;
  }

  fDataBin[histoNo][bin] += dval;
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
  for (UInt_t i=0; i<2; i++)
    fBkgFix[i] = PMUSR_UNDEFINED;
  for (UInt_t i=0; i<4; i++) {
    fBkgRange[i] = -1; // undefined start background range
    fDataRange[i] = -1; // undefined start data range
  }
  fFitRange[0] = PMUSR_UNDEFINED; // undefined start fit range
  fFitRange[1] = PMUSR_UNDEFINED; // undefined end fit range
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
}

//--------------------------------------------------------------------------
// CleanUp
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
  fFitRange[0] = PMUSR_UNDEFINED; // undefined start fit range
  fFitRange[1] = PMUSR_UNDEFINED; // undefined end fit range
  fPacking = -1; // undefined packing
  fXYDataIndex[0] = -1; // undefined x data index (NonMusr)
  fXYDataIndex[1] = -1; // undefined y data index (NonMusr)
  fXYDataLabel[0] = TString("??"); // undefined x data label (NonMusr)
  fXYDataLabel[1] = TString("??"); // undefined y data label (NonMusr)

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
}

//--------------------------------------------------------------------------
// GetRunName
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
// SetRunName
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
// GetBeamline
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
// SetBeamline
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
// GetInstitute
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
// SetInstitute
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
// GetFileFormat
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
// SetFileFormat
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
// GetForwardHistoNo
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
// SetForwardHistoNo
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
// GetBackwardHistoNo
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
// SetBackwardHistoNo
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
// GetMap
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
// SetMap
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
// GetBkgFix
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
// SetBkgFix
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
    cerr << endl << "PMsrRunBlock::SetBkgFix: **WARNING** idx=" << idx << ", only idx=0,1 are sensible.";
    cerr << endl;
    return;
  }

  fBkgFix[idx] = dval;
}

//--------------------------------------------------------------------------
// GetBkgRange
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
// SetBkgRange
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
    cerr << endl << "PMsrRunBlock::SetBkgRange: **WARNING** idx=" << idx << ", only idx=0..3 are sensible.";
    cerr << endl;
    return;
  }

  fBkgRange[idx] = ival;
}


//--------------------------------------------------------------------------
// GetDataRange
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
// SetDataRange
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
    cerr << endl << "PMsrRunBlock::SetDataRange: **WARNING** idx=" << idx << ", only idx=0..3 are sensible.";
    cerr << endl;
    return;
  }

  fDataRange[idx] = ival;
}

//--------------------------------------------------------------------------
// GetT0
//--------------------------------------------------------------------------
/**
 * <p> get T0 value at position idx
 *
 * <b>return:</b>
 * - t0 value, if idx is within proper boundaries
 * - -1, otherwise
 *
 * \param idx index of the T0 value to be returned
 */
Int_t PMsrRunBlock::GetT0(UInt_t idx)
{
  if (idx >= fT0.size())
    return -1;

  return fT0[idx];
}

//--------------------------------------------------------------------------
// SetT0
//--------------------------------------------------------------------------
/**
 * <p> set T0 value at position idx
 *
 * \param ival t0 value
 * \param idx index of the T0 value to be set. If idx==-1, append value
 */
void PMsrRunBlock::SetT0(Int_t ival, Int_t idx)
{
  if (idx == -1) {
    fT0.push_back(ival);
    return;
  }

  if (idx >= static_cast<Int_t>(fT0.size()))
    fT0.resize(idx+1);

  fT0[idx] = ival;
}

//--------------------------------------------------------------------------
// GetAddT0Size
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
Int_t PMsrRunBlock::GetAddT0Size(UInt_t addRunIdx)
{
  if (fAddT0.empty())
    return -1;

  if (addRunIdx >= fAddT0.size())
    return -1;

  return fAddT0[addRunIdx].size();
}

//--------------------------------------------------------------------------
// GetAddT0
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
Int_t PMsrRunBlock::GetAddT0(UInt_t addRunIdx, UInt_t histoIdx)
{
  if (fAddT0.empty())
    return -1;

  if (addRunIdx >= fAddT0.size())
    return -1;

  if (fAddT0[addRunIdx].empty())
    return -1;

  if (histoIdx >= fAddT0[addRunIdx].size())
    return -1;

  return fAddT0[addRunIdx][histoIdx];
}

//--------------------------------------------------------------------------
// SetAddT0
//--------------------------------------------------------------------------
/**
 * <p> set add t0 value of the addrun at index histoNoIdx
 *
 * \param ival t0 value
 * \param addRunIdx addrun index
 * \param histoNoIdx index whithin the fAddT0 vector where to set the value.
 */
void PMsrRunBlock::SetAddT0(Int_t ival, UInt_t addRunIdx, UInt_t histoNoIdx)
{
  if (addRunIdx >= fAddT0.size())
    fAddT0.resize(addRunIdx+1);

  if (histoNoIdx >= fAddT0[addRunIdx].size())
    fAddT0[addRunIdx].resize(histoNoIdx+1);

  fAddT0[addRunIdx][histoNoIdx] = ival;

}

//--------------------------------------------------------------------------
// GetFitRange
//--------------------------------------------------------------------------
/**
 * <p> get fit range value at position idx
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
// SetFitRange
//--------------------------------------------------------------------------
/**
 * <p> set fit range value at position idx
 *
 * \param idx index of the fit range value to be set
 */
void PMsrRunBlock::SetFitRange(Double_t dval, UInt_t idx)
{
  if (idx >= 2)
    return;

  fFitRange[idx] = dval;
}
