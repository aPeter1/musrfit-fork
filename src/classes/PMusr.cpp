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
 * <p>Holds the data which will be fitted.
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
 * <p>Cleans up
 */
PRunData::~PRunData()
{
  fX.clear();        // only used in non-muSR
  fValue.clear();
  fError.clear();
  fXTheory.clear();  // only used in non-muSR
  fTheory.clear();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// implementation PNonMusrRawRunData
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
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
 * <p>Cleans up
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
    cerr << endl << "**WARNING** PNonMusrRawRunData::SetLabel: idx=" << idx << " is out of range (0.." << fLabels.size() << ")";
    cerr << endl;
    return;
  }
  fLabels[idx] = str;
}

//--------------------------------------------------------------------------
// AppendSubData
//--------------------------------------------------------------------------
/**
 * <p>Modifies the data set at position idx. In case idx is larges than the number
 * of data sets, only a warning will be sent to stderr.
 *
 * \param idx index at which position the label should be set
 * \param dval double value to be set
 */
void PNonMusrRawRunData::AppendSubData(const UInt_t idx, const Double_t dval)
{
  if (idx >= fData.size()) {
    cerr << endl << "**WARNING** PNonMusrRawRunData::AppendSubData: idx=" << idx << " is out of range (0.." << fData.size() << ")";
    cerr << endl;
    return;
  }
  fData[idx].push_back(dval);
}

//--------------------------------------------------------------------------
// AppendSubErrData
//--------------------------------------------------------------------------
/**
 * <p>Modifies the error-data set at position idx. In case idx is larges than the number
 * of error-data sets, only a warning will be sent to stderr.
 *
 * \param idx index at which position the label should be set
 * \param dval double value to be set
 */
void PNonMusrRawRunData::AppendSubErrData(const UInt_t idx, const Double_t dval)
{
  if (idx >= fErrData.size()) {
    cerr << endl << "**WARNING** PNonMusrRawRunData::AppendSubErrData: idx=" << idx << " is out of range (0.." << fErrData.size() << ")";
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
 * <p>
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
 * <p>
 */
PRawRunData::~PRawRunData()
{
  fTemp.clear();
  fRingAnode.clear();
  fT0s.clear();
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
 * <p> Returns a temperature
 *
 * \param idx index of the temperature whished
 */
const Double_t PRawRunData::GetTemperature(const UInt_t idx)
{
  if (idx >= fTemp.size()) {
    cerr << endl << "**WARNING** PRawRunData::GetTemperature: idx=" << idx << " is out of range (0.." << fTemp.size() << ").";
    cerr << endl;
    return PMUSR_UNDEFINED;
  }
  return fTemp[idx].first;
}

//--------------------------------------------------------------------------
// GetTempError
//--------------------------------------------------------------------------
/**
 * <p> Returns the error estimate of the temperature.
 *
 * \param idx index of the temperature whished
 */
const Double_t PRawRunData::GetTempError(const UInt_t idx)
{
  if (idx >= fTemp.size()) {
    cerr << endl << "**WARNING** PRawRunData::GetTempError: idx=" << idx << " is out of range (0.." << fTemp.size() << ").";
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
 * \param idx index of the ring anode whished
 */
const Double_t PRawRunData::GetRingAnode(const UInt_t idx)
{
  if (idx >= fRingAnode.size()) {
    cerr << endl << "**WARNING** PRawRunData::GetRingAnode: idx=" << idx << " is out of range (0.." << fRingAnode.size() << ").";
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
 * \param idx index of the T0 value whished
 */
const Int_t PRawRunData::GetT0(const UInt_t idx)
{
  if (idx >= fT0s.size()) {
    cerr << endl << "**WARNING** PRawRunData::GetT0: idx=" << idx << " is out of range (0.." << fT0s.size() << ").";
    cerr << endl;
    return -1;
  }
  return fT0s[idx];
}

//--------------------------------------------------------------------------
// GetBkgBin
//--------------------------------------------------------------------------
/**
 * <p>
 */
const PIntPair PRawRunData::GetBkgBin(const UInt_t idx)
{
  PIntPair pair(-1, -1);

  if (idx >= fBkgBin.size()) {
    cerr << endl << "**WARNING** PRawRunData::GetBkgBin: idx=" << idx << " is out of range (0.." << fBkgBin.size() << ").";
    cerr << endl;
    return pair;
  }
  return fBkgBin[idx];
}

//--------------------------------------------------------------------------
// GetGoodDataBin
//--------------------------------------------------------------------------
/**
 * <p>
 */
const PIntPair PRawRunData::GetGoodDataBin(const UInt_t idx)
{
  PIntPair pair(-1, -1);

  if (idx >= fGoodDataBin.size()) {
    cerr << endl << "**WARNING** PRawRunData::GetGoodDataBin: idx=" << idx << " is out of range (0.." << fGoodDataBin.size() << ").";
    cerr << endl;
    return pair;
  }
  return fGoodDataBin[idx];
}

//--------------------------------------------------------------------------
// GetDataBin
//--------------------------------------------------------------------------
/**
 * <p>
 */
const PDoubleVector* PRawRunData::GetDataBin(const UInt_t idx)
{
  if (idx >= fDataBin.size()) {
    cerr << endl << "**WARNING** PRawRunData::GetDataBin: idx=" << idx << " is out of range (0.." << fDataBin.size() << ").";
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
    cerr << endl << "**WARNING** PRawRunData::SetDataBin: histoNo=" << histoNo << " is out of range (0.." << fDataBin.size() << ").";
    cerr << endl;
    return;
  }

  if (bin > fDataBin[histoNo].size()) {
    cerr << endl << "**WARNING** PRawRunData::SetDataBin: bin=" << bin << " is out of range (0.." << fDataBin[histoNo].size() << ").";
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
    cerr << endl << "**WARNING** PRawRunData::AddDataBin: histoNo=" << histoNo << " is out of range (0.." << fDataBin.size() << ").";
    cerr << endl;
    return;
  }

  if (bin > fDataBin[histoNo].size()) {
    cerr << endl << "**WARNING** PRawRunData::AddDataBin: bin=" << bin << " is out of range (0.." << fDataBin[histoNo].size() << ").";
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
  fPhaseParamNo = -1; // undefined phase parameter number
  fLifetimeParamNo = -1; // undefined lifetime parameter number
  fLifetimeCorrection = true; // lifetime correction == true by default (used in single histogram musrview)
  fForwardHistoNo = -1; // undefined forward histogram number
  fBackwardHistoNo = -1; // undefined backward histogram number
  fFitRange[0] = PMUSR_UNDEFINED; // undefined start fit range
  fFitRange[1] = PMUSR_UNDEFINED; // undefined end fit range
  fPacking = -1; // undefined packing
  fRRFFreq = PMUSR_UNDEFINED; // undefined RRF frequency
  fRRFPacking = -1; // undefined RRF packing
  fAlpha2ParamNo = -1; // undefined alpha2 parameter number
  fBeta2ParamNo = -1; // undefined beta2 parameter number
  fRightHistoNo = -1; // undefined right histogram number
  fLeftHistoNo = -1; // undefined left histogram number
  fXYDataIndex[0] = -1; // undefined x data index (NonMusr)
  fXYDataIndex[1] = -1; // undefined y data index (NonMusr)
  fXYDataLabel[0] = TString("??"); // undefined x data label (NonMusr)
  fXYDataLabel[1] = TString("??"); // undefined y data label (NonMusr)
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
  fMap.clear();
  fBkgFix.clear();
  fBkgRange.clear();
  fDataRange.clear();
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
  fPhaseParamNo = -1; // undefined phase parameter number
  fLifetimeParamNo = -1; // undefined lifetime parameter number
  fLifetimeCorrection = true; // lifetime correction == true by default (used in single histogram musrview)
  fForwardHistoNo = -1; // undefined forward histogram number
  fBackwardHistoNo = -1; // undefined backward histogram number
  fFitRange[0] = PMUSR_UNDEFINED; // undefined start fit range
  fFitRange[1] = PMUSR_UNDEFINED; // undefined end fit range
  fPacking = -1; // undefined packing
  fRRFFreq = PMUSR_UNDEFINED; // undefined RRF frequency
  fRRFPacking = -1; // undefined RRF packing
  fAlpha2ParamNo = -1; // undefined alpha2 parameter number
  fBeta2ParamNo = -1; // undefined beta2 parameter number
  fRightHistoNo = -1; // undefined right histogram number
  fLeftHistoNo = -1; // undefined left histogram number
  fXYDataIndex[0] = -1; // undefined x data index (NonMusr)
  fXYDataIndex[1] = -1; // undefined y data index (NonMusr)
  fXYDataLabel[0] = TString("??"); // undefined x data label (NonMusr)
  fXYDataLabel[1] = TString("??"); // undefined y data label (NonMusr)

  fRunName.clear();
  fBeamline.clear();
  fInstitute.clear();
  fFileFormat.clear();
  fMap.clear();
  fBkgFix.clear();
  fBkgRange.clear();
  fDataRange.clear();
  fT0.clear();
}

//--------------------------------------------------------------------------
// GetRunName
//--------------------------------------------------------------------------
/**
 * <p> get run name at position i
 *
 * \param i index of the run name to be returned
 */
TString* PMsrRunBlock::GetRunName(UInt_t i)
{
  if (i>fRunName.size())
    return 0;

  return &fRunName[i];
}

//--------------------------------------------------------------------------
// SetRunName
//--------------------------------------------------------------------------
/**
 * <p> set run name at position i
 *
 * \param i index of the run name to be set
 */
void PMsrRunBlock::SetRunName(TString &str, UInt_t i)
{
  if (i>fRunName.size())
    fRunName.resize(i+1);

  fRunName[i] = str;
}

//--------------------------------------------------------------------------
// GetBeamline
//--------------------------------------------------------------------------
/**
 * <p> get beamline name at position i
 *
 * \param i index of the beamline to be returned
 */
TString* PMsrRunBlock::GetBeamline(UInt_t i)
{
  if (i>fBeamline.size())
    return 0;

  return &fBeamline[i];
}

//--------------------------------------------------------------------------
// SetBeamline
//--------------------------------------------------------------------------
/**
 * <p> set beamline name at position i
 *
 * \param i index of the beamline name to be set
 */
void PMsrRunBlock::SetBeamline(TString &str, UInt_t i)
{
  if (i>fBeamline.size())
    fBeamline.resize(i+1);

  fBeamline[i] = str;
}

//--------------------------------------------------------------------------
// GetInstitute
//--------------------------------------------------------------------------
/**
 * <p> get institute name at position i
 *
 * \param i index of the institute to be returned
 */
TString* PMsrRunBlock::GetInstitute(UInt_t i)
{
  if (i>fInstitute.size())
    return 0;

  return &fInstitute[i];
}

//--------------------------------------------------------------------------
// SetInstitute
//--------------------------------------------------------------------------
/**
 * <p> set institute name at position i
 *
 * \param i index of the run name to be set
 */
void PMsrRunBlock::SetInstitute(TString &str, UInt_t i)
{
  if (i>fInstitute.size())
    fInstitute.resize(i+1);

  fInstitute[i] = str;
}

//--------------------------------------------------------------------------
// GetFileFormat
//--------------------------------------------------------------------------
/**
 * <p> get file format name at position i
 *
 * \param i index of the file format to be returned
 */
TString* PMsrRunBlock::GetFileFormat(UInt_t i)
{
  if (i>fFileFormat.size())
    return 0;

  return &fFileFormat[i];
}

//--------------------------------------------------------------------------
// SetFileFormat
//--------------------------------------------------------------------------
/**
 * <p> set file format name at position i
 *
 * \param i index of the file format name to be set
 */
void PMsrRunBlock::SetFileFormat(TString &str, UInt_t i)
{
  if (i>fFileFormat.size())
    fFileFormat.resize(i+1);

  fFileFormat[i] = str;
}

//--------------------------------------------------------------------------
// GetMap
//--------------------------------------------------------------------------
/**
 * <p> get map value at position i
 *
 * \param i index of the map value to be returned
 */
Int_t PMsrRunBlock::GetMap(UInt_t i)
{
  if (i>fMap.size())
    return -1;

  return fMap[i];
}

//--------------------------------------------------------------------------
// SetMap
//--------------------------------------------------------------------------
/**
 * <p> set file format name at position i
 *
 * \param i index of the file format name to be set
 */
void PMsrRunBlock::SetMap(Int_t mapVal, UInt_t idx)
{
  if (idx>fMap.size())
    fMap.resize(idx+1);

  fMap[idx] = mapVal;
}
