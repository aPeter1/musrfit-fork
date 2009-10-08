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
// implementation PMsrRunStructure
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
