/***************************************************************************

  PNeXus.cpp

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

#include<cstdio>
#include<cstring>
#include<ctime>
#include<cassert>

#include <iostream>
#include <sstream>
using namespace std;

#include "PNeXus.h"

#ifndef SIZE_FLOAT32
#    define SIZE_FLOAT32    4
#    define SIZE_FLOAT64    8
#    define SIZE_INT8       1
#    define SIZE_UINT8      1
#    define SIZE_INT16      2
#    define SIZE_UINT16     2
#    define SIZE_INT32      4
#    define SIZE_UINT32     4
#    define SIZE_INT64      8
#    define SIZE_UINT64     8
#    define SIZE_CHAR8      1
#    define SIZE_CHAR       1
#    define SIZE_UCHAR8     1
#    define SIZE_UCHAR      1
#    define SIZE_CHAR16     2
#    define SIZE_UCHAR16    2
#endif /* SIZE_FLOAT32 */

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// PNeXusProp Constructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusProp::PNeXusProp()
{
  fName = "n/a";
  fUnit = "n/a";
  fValue = 1.0e99;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// IsValid (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NXbeam data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXusBeam1::IsValid(bool strict)
{
  bool valid = true;

  if (fTotalCounts == 0) {
    cerr << ">> **WARNING** NXbeam total_counts not given" << endl;
  } else if (!fUnits.compare("n/a")) {
    cerr << ">> **WARNING** NXbeam total_counts units not given" << endl;
  }

  return valid;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// IsValid (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NXdetector data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXusDetector1::IsValid(bool strict)
{
  bool valid = true;

  if (fNumber == 0) {
    cerr << ">> **WARNING** NXdetector number of detectors not given" << endl;
  }

  return valid;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// IsValid (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NXinstrument data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXusInstrument1::IsValid(bool strict)
{
  bool valid = true;

  if (!fName.compare("n/a")) {
    cerr << ">> **ERROR** NXinstrument name not given" << endl;
    valid = false;
  } else if (!fDetector.IsValid(strict)) {
    valid = false;
  } else if (!fCollimator.IsValid(strict)) {
    valid = false;
  } else if (!fBeam.IsValid(strict)) {
    valid = false;
  }

  return valid;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// PNeXusSample1 Constructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusSample1::PNeXusSample1()
{
  fName = "n/a";
  fShape = "n/a";
  fMagneticFieldState = "n/a";
  fEnvironment = "n/a";
  fMagneticFieldVectorAvailable = -1;
  fMagneticFieldVectorUnits = "n/a";
  fMagneticFieldVectorCoordinateSystem = "n/a";
}

//------------------------------------------------------------------------------------------
// PNeXusSample1 Destructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusSample1::~PNeXusSample1()
{
  fPhysProp.clear();
  fMagneticFieldVector.clear();
}

//------------------------------------------------------------------------------------------
// IsValid (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NXsample data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXusSample1::IsValid(bool strict)
{
  bool valid = true;

  if (!fName.compare("n/a")) {
    cerr << ">> **ERROR** NXsample name not given." << endl;
    valid = false;
  } else if (!fShape.compare("n/a")) {
    cerr << ">> **WARNING** NXsample shape not given." << endl;
  }

  int count=0; // at the end count holds the number of required physiscal properties. Currently these are: temperature, magnetic_field
  for (unsigned int i=0; i<fPhysProp.size(); i++) {
    if (!fPhysProp[i].GetName().compare("temperature")) {
      count++;
      if (!fPhysProp[i].GetUnit().compare("n/a")) {
        cerr << ">> **ERROR** NXsample temperature unit not given." << endl;
        valid = false;
      }
      if (fPhysProp[i].GetValue() == 1.0e99) {
        cerr << ">> **ERROR** NXsample temperature value not given." << endl;
        valid = false;
      }
    }
    if (!fPhysProp[i].GetName().compare("magnetic_field")) {
      count++;
      if (!fPhysProp[i].GetUnit().compare("n/a")) {
        cerr << ">> **ERROR** NXsample magnetic_field unit not given." << endl;
        valid = false;
      }
      if (fPhysProp[i].GetValue() == 1.0e99) {
        cerr << ">> **ERROR** NXsample magnetic_field value not given." << endl;
        valid = false;
      }
    }
  }
  if (count < 2) {
    cerr << ">> **ERROR** not all required physical properties (e.g. temperature, magnetic_field) are given." << endl;
    valid = false;
  }

  return valid;
}

//------------------------------------------------------------------------------------------
// GetPhysPropValue (public)
//------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * \param name
 * \param ok
 */
double PNeXusSample1::GetPhysPropValue(string name, bool &ok)
{
  double dval=0.0;
  ok = false;

  for (unsigned int i=0; i<fPhysProp.size(); i++) {
    if (!fPhysProp[i].GetName().compare(name)) {
      dval = fPhysProp[i].GetValue();
      ok = true;
      break;
    }
  }

  return dval;
}

//------------------------------------------------------------------------------------------
// GetPhysPropUnit (public)
//------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * \param name
 * \param ok
 */
void PNeXusSample1::GetPhysPropUnit(string name, string &unit, bool &ok)
{
  unit = "n/a";
  ok = false;

  for (unsigned int i=0; i<fPhysProp.size(); i++) {
    if (!fPhysProp[i].GetName().compare(name)) {
      unit = fPhysProp[i].GetUnit();
      ok = true;
      break;
    }
  }
}

//------------------------------------------------------------------------------------------
// SetPhysProp (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set the physical property with 'name' and 'value' at index 'idx'. If idx==-1 add it at the
 * end, otherwise set it at index idx.
 *
 * \param name of the physical property
 * \param value of the physical property
 * \param idx index where to set the physical property
 */
void PNeXusSample1::SetPhysProp(string name, double value, string unit, int idx)
{
  PNeXusProp prop;

  prop.SetName(name);
  prop.SetValue(value);
  prop.SetUnit(unit);

  if (idx == -1) {
    fPhysProp.push_back(prop);
  } else if (idx >= (int)fPhysProp.size()) {
    fPhysProp.resize(idx+1);
    fPhysProp[idx] = prop;
  } else {
    fPhysProp[idx] = prop;
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// PNeXusUser1 Constructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusUser1::PNeXusUser1()
{
  fName = "n/a";
  fExperimentNumber = "n/a";
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// PNeXusEntry1 Constructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusEntry1::PNeXusEntry1()
{
  fProgramName = "n/a";
  fProgramVersion = "n/a";
  fRunNumber = -1;
  fTitle = "n/a";
  fNotes = "n/a";
  fAnalysis = "n/a";
  fLaboratory = "n/a";
  fBeamline = "n/a";
  fStartTime = "n/a";
  fStopTime = "n/a";
  fSwitchingState = -1;
}

//------------------------------------------------------------------------------------------
// IsValid (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NXentry data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXusEntry1::IsValid(bool strict)
{
  if (!fProgramName.compare("n/a")) {
    cerr << ">> **WARNING** NXentry: program_name not given." << endl;
  } else if (!fProgramVersion.compare("n/a")) {
    cerr << ">> **WARNING** NXentry: program_name/version not given." << endl;
  } else if (fRunNumber == -1) {
    cerr << ">> **ERROR** NXentry: number (i.e. run number) not given." << endl;
    return false;
  } else if (!fTitle.compare("n/a")) {
    cerr << ">> **ERROR** NXentry: title not given." << endl;
    return false;
  } else if (!fNotes.compare("n/a")) {
    cerr << ">> **WARNING** NXentry: notes not given." << endl;
  } else if (!fAnalysis.compare("n/a")) {
    cerr << ">> **ERROR** NXentry: analysis (i.e. type of muon experiment like 'TF', 'ALC', ...) not given." << endl;
    return false;
  } else if (!fLaboratory.compare("n/a")) {
    cerr << ">> **ERROR** NXentry: lab (e.g. 'PSI') not given." << endl;
    return false;
  } else if (!fBeamline.compare("n/a")) {
    cerr << ">> **ERROR** NXentry: beamline (e.g. 'piE3') not given." << endl;
    return false;
  } else if (!fStartTime.compare("n/a")) {
    cerr << ">> **ERROR** NXentry: start_time not given." << endl;
    return false;
  } else if (!fStopTime.compare("n/a")) {
    cerr << ">> **ERROR** NXentry: stop_time not given." << endl;
    return false;
  } else if (fSwitchingState == -1) {
    cerr << ">> **ERROR** NXentry: switching_state (i.e. '1' normal data taking, '2' red/green mode) not given." << endl;
    return false;
  } else if (!fUser.IsValid(strict)) {
    return false;
  } else if (!fSample.IsValid(strict)) {
    return false;
  } else if (!fInstrument.IsValid(strict)) {
    return false;
  } else if (!fData.IsValid(strict)) {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------------------
// SetStartTime (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set the start time string and check that it is ISO 8601 conform.
 *
 * \param time start time string
 */
int PNeXusEntry1::SetStartTime(string time)
{
  struct tm tm;
  memset(&tm, 0, sizeof(tm));
  strptime(time.c_str(), "%Y-%m-%d %H:%M:S", &tm);
  if (tm.tm_year == 0)
    strptime(time.c_str(), "%Y-%m-%dT%H:%M:S", &tm);
  if (tm.tm_year == 0)
    return NX_ERROR;

  fStartTime = time;

  return NX_OK;
}

//------------------------------------------------------------------------------------------
// SetStopTime (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set the stop time string and check that it is ISO 8601 conform.
 *
 * \param time stop time string
 */
int PNeXusEntry1::SetStopTime(string time)
{
  struct tm tm;
  memset(&tm, 0, sizeof(tm));
  strptime(time.c_str(), "%Y-%m-%d %H:%M:S", &tm);
  if (tm.tm_year == 0)
    strptime(time.c_str(), "%Y-%m-%dT%H:%M:S", &tm);
  if (tm.tm_year == 0)
    return NX_ERROR;

  fStopTime = time;

  return NX_OK;
}

//------------------------------------------------------------------------------------------
// SetSwitchingState (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set the switching state tag. '1' normal data collection, '2' Red/Green mode
 *
 * \param state switching state tag
 */
int PNeXusEntry1::SetSwitchingState(int state)
{
  if ((state != 1) && (state != 2))
    return NX_ERROR;

  fSwitchingState = state;

  return NX_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// PNeXusAlpha1 Constructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusAlpha1::PNeXusAlpha1()
{
   fGroupFirst=0;
   fGroupSecond=0;
   fAlphaVal=0.0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// PNeXusData1 Destructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusData1::~PNeXusData1()
{
  fT0.clear();
  fFirstGoodBin.clear();
  fLastGoodBin.clear();
  fHistoName.clear();
  for (unsigned int i=0; i<fHisto.size(); i++)
    fHisto[i].clear();
  fHisto.clear();
  fGrouping.clear();
  fAlpha.clear();
}

//------------------------------------------------------------------------------------------
// IsValid (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NXbeam data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXusData1::IsValid(bool strict)
{
  if (GetNoOfHistos() == 0) {
    cerr << ">> **ERROR** NXdata no histograms given." << endl;
    return false;
  } else if (fTimeResolution == 0.0) {
    cerr << ">> **ERROR** NXdata time resolution not given." << endl;
    return false;
  } else if (fT0.size() == 0) {
    cerr << ">> **WARNING** NXdata t0 not given." << endl;
  } else if (fFirstGoodBin.size() == 0) {
    cerr << ">> **WARNING** NXdata first good bin not given." << endl;
  } else if (fLastGoodBin.size() == 0) {
    cerr << ">> **WARNING** NXdata last good bin not given." << endl;
  }

  return true;
}


//------------------------------------------------------------------------------------------
// GetTimeResolution (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Get time resolution in the requested units. Allowed units are 'fs', 'ps', 'ns', 'us'.
 * If unsupported time units are given, a time resolution == 0.0 is returned. Internally the
 * time resolution is stored in 'ps'.
 *
 * \param units requested units for the time resolution.
 */
double PNeXusData1::GetTimeResolution(string units)
{
  double result=0.0;

  if (!units.compare("fs"))
    result = fTimeResolution * 1.0e3;
  else if (!units.compare("ps"))
    result = fTimeResolution;
  else if (!units.compare("ns"))
    result = fTimeResolution * 1.0e-3;
  else if (!units.compare("us"))
    result = fTimeResolution * 1.0e-6;
  else
    result = 0.0;

  return result;
}

//------------------------------------------------------------------------------------------
// GetT0 (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Returns the t0 at index 'idx' or -1 if the index is out of range.
 *
 * \param idx index of the requested t0
 */
int PNeXusData1::GetT0(unsigned int idx)
{
  if (idx >= fT0.size())
    return -1;

  return (int)fT0[idx];
}

//------------------------------------------------------------------------------------------
// GetFirstGoodBin (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Returns the first good bin at index 'idx' or -1 if the index is out of range.
 *
 * \param idx index of the requested first good bin
 */
int PNeXusData1::GetFirstGoodBin(unsigned int idx)
{
  if (idx >= fFirstGoodBin.size())
    return -1;

  return (int)fFirstGoodBin[idx];
}

//------------------------------------------------------------------------------------------
// GetLastGoodBin (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Returns the last good bin at index 'idx' or -1 if the index is out of range.
 *
 * \param idx index of the requested last good bin
 */
int PNeXusData1::GetLastGoodBin(unsigned int idx)
{
  if (idx >= fLastGoodBin.size())
    return -1;

  return (int)fLastGoodBin[idx];
}


//------------------------------------------------------------------------------------------
// GetHistoName (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Extract the histogram name at position 'idx'. If 'idx' is out of range, the 'ok' flag
 * is set to 'false' in which case the 'name' is undefined.
 *
 * \param idx index of the requested histogram name.
 * \param name histogram name if idx is within range
 * \param ok =true if idx was within range, otherwise false
 */
void PNeXusData1::GetHistoName(unsigned int idx, string &name, bool &ok)
{
  if (idx >= fHistoName.size()) {
    ok = false;
  } else {
    ok = true;
    name = fHistoName[idx];
  }
}

//------------------------------------------------------------------------------------------
// GetHistoLength (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Returns the length of histogram histoNo, or 0 if histoNo is out of bound.
 *
 * \param histoNo index of the histogram for which the size needs to be determined.
 */
unsigned int PNeXusData1::GetHistoLength(unsigned int histoNo)
{
  if (histoNo >= fHisto.size())
    return 0;

  return fHisto[histoNo].size();
}

//------------------------------------------------------------------------------------------
// GetHisto (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Returns the histogram with index 'histoNo' or 0 if 'histoNo' is out of range.
 *
 * \param histoNo index of the requested histogram
 */
vector<unsigned int> *PNeXusData1::GetHisto(unsigned int histoNo)
{
  if (histoNo >= fHisto.size())
    return 0;

  return &fHisto[histoNo];
}


//------------------------------------------------------------------------------------------
// SetTimeResolution (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set the time resolution. 'units' tells in which units 'val' is provided. Acteptable units
 * are 'fs', 'ps', 'ns', 'us'.
 *
 * \param val time resolution value
 * \param units time resolution units
 */
void PNeXusData1::SetTimeResolution(double val, string units)
{
  if (!units.compare("fs"))
    fTimeResolution = val * 1.0e-3;
  else if (!units.compare("ps"))
    fTimeResolution = val;
  else if (!units.compare("ns"))
    fTimeResolution = val * 1.0e3;
  else if (!units.compare("us"))
    fTimeResolution = val * 1.0e6;
}

//------------------------------------------------------------------------------------------
// SetT0 (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set the t0 value at index 'idx'. If 'idx'==-1, the t0 value will be appended
 *
 * \param t0 bin value
 * \param idx index where to set t0 bin value
 */
void PNeXusData1::SetT0(unsigned int t0, int idx)
{
  if (idx == -1) {
    fT0.push_back(t0);
  } else if (idx >= (int)fT0.size()) {
    fT0.resize(idx+1);
    fT0[idx] = t0;
  } else {
    fT0[idx] = t0;
  }
}

//------------------------------------------------------------------------------------------
// SetFirstGoodBin (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set the first good bin value at index 'idx'. If 'idx'==-1, the first good bin value will be appended
 *
 * \param fgb first good bin value
 * \param idx index where to set first good bin value
 */
void PNeXusData1::SetFirstGoodBin(unsigned int fgb, int idx)
{
  if (idx == -1) {
    fFirstGoodBin.push_back(fgb);
  } else if (idx >= (int)fFirstGoodBin.size()) {
    fFirstGoodBin.resize(idx+1);
    fFirstGoodBin[idx] = fgb;
  } else {
    fFirstGoodBin[idx] = fgb;
  }
}

//------------------------------------------------------------------------------------------
// SetLastGoodBin (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set the last good bin value at index 'idx'. If 'idx'==-1, the last good bin value will be appended
 *
 * \param lgb last good bin value
 * \param idx index where to set last good bin value
 */
void PNeXusData1::SetLastGoodBin(unsigned int lgb, int idx)
{
  if (idx == -1) {
    fLastGoodBin.push_back(lgb);
  } else if (idx >= (int)fLastGoodBin.size()) {
    fLastGoodBin.resize(idx+1);
    fLastGoodBin[idx] = lgb;
  } else {
    fLastGoodBin[idx] = lgb;
  }
}

//------------------------------------------------------------------------------------------
// FlushHistos (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Flushes all previously allocated histograms.
 *
 */
void PNeXusData1::FlushHistos()
{
  for (unsigned int i=0; i<fHisto.size(); i++)
    fHisto[i].clear();
  fHisto.clear();
}

//------------------------------------------------------------------------------------------
// SetHisto (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set a histogram at index 'histoNo'. If 'histoNo'==-1, the histogram will be appended
 *
 * \param data histogram vector
 * \param histoNo index where to set the histogram
 */
void PNeXusData1::SetHisto(vector<unsigned int> &data, int histoNo)
{
  if (histoNo == -1) {
    fHisto.push_back(data);
  } else if (histoNo >= (int)fHisto.size()) {
    fHisto.resize(histoNo+1);
    fHisto[histoNo] = data;
  } else {
    fHisto[histoNo] = data;
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// IsValid (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NXbeamline data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXusBeamline2::IsValid(bool strict)
{
  string msg("");

  if (!fName.compare("n/a")) {
    msg = "IDF2 NXbeamline 'name' not set.";
    if (strict) {
      cerr << ">> **ERROR** " << msg << endl;
      return false;
    } else {
      cerr << ">> **WARNING** " << msg << endl;
    }
  }

  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// PNeXusDetector2 Constructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusDetector2::PNeXusDetector2()
{
  fErrorMsg = "";

  fDescription = "n/a";
  fTimeResolution = 0;
  fNoOfPeriods = -1;
  fNoOfSpectra = -1;
  fNoOfBins = -1;

  fT0Tag = -1;
  fT0 = 0;
  fFirstGoodBin = 0;
  fLastGoodBin = 0;
  fHisto = 0;
}

//------------------------------------------------------------------------------------------
// PNeXusDetector2 Destructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusDetector2::~PNeXusDetector2()
{
  fSpectrumIndex.clear();
  fRawTime.clear();

  if (fT0) {
    delete [] fT0;
    fT0 = 0;
  }
  if (fFirstGoodBin) {
    delete [] fFirstGoodBin;
    fFirstGoodBin = 0;
  }
  if (fLastGoodBin) {
    delete [] fLastGoodBin;
    fLastGoodBin = 0;
  }
  if (fHisto) {
    delete [] fHisto;
    fHisto = 0;
  }
}

//------------------------------------------------------------------------------------------
// IsValid (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NXdetector data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXusDetector2::IsValid(bool strict)
{
  string msg("");

  if (!fDescription.compare("n/a")) {
    msg = "IDF2 NXdetector 'description' not set.";
    if (strict) {
      cerr << ">> **ERROR** " << msg << endl;
      return false;
    } else {
      cerr << ">> **WARNING** " << msg << endl;
    }
  }

  if (fTimeResolution == 0.0) {
    cerr << ">> **ERROR** IDF2 NXdetector 'histogram_resolution' is not set." << endl;
    return false;
  }

  if (fSpectrumIndex.size() == 0) {
    cerr << ">> **ERROR** IDF2 NXdetector 'spectrum_index' is not set." << endl;
    return false;
  }

  if ((fT0Tag != 1) && (fT0Tag !=2)) {
    cerr << ">> **ERROR** IDF2 NXdetector problem with t0/first_good_bin/last_good_bin/counts settings (tagging)." << endl;
    return false;
  }

  if (fT0 == 0) {
    cerr << ">> **ERROR** IDF2 NXdetector t0 settings pointer is null." << endl;
    return false;
  }

  if (fHisto == 0) {
    cerr << ">> **ERROR** IDF2 NXdetector counts settings pointer is null." << endl;
    return false;
  }

  if (fNoOfBins == -1) {
    cerr << ">> **ERROR** IDF2 NXdetector fNoOfBins==-1." << endl;
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------------------
// GetT0 (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Get T0 bin if present, otherwise return -1.
 *
 * \param idxp period index
 * \param idxs spectrum index
 */
int PNeXusDetector2::GetT0(int idxp, int idxs)
{
  int result = -1;

  if ((idxp < 0) && (idxs < 0)) { // assumption: there is only ONE t0 for all spectra
    if (fT0 != 0) {
      result = *fT0;
    }
  } else if ((idxp < 0) && (idxs >= 0)) { // assumption: t0's are represented as t0[ns]
    if (idxs < fNoOfSpectra) {
      result = *(fT0+idxs);
    }
  } else if ((idxp >= 0) && (idxs >= 0)) { // assumption: t0's are represented as t0[np][ns]
    if ((idxp < fNoOfPeriods) || (idxs < fNoOfSpectra)) {
      result = *(fT0+idxp*fNoOfSpectra+idxs);
    }
  } else {
    result = -1;
  }

  return result;
}

//------------------------------------------------------------------------------------------
// SetT0 (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set T0s.
 *
 * <p><b>return:</b>
 * -1 of everything is OK,
 * -0 if there was an error, in which case GetErrorMsg() will give the internal error message.
 *
 * \param t0 pointer to t0's.
 */
int PNeXusDetector2::SetT0(int *t0)
{
  if (t0 == 0) {
    fErrorMsg = "PNeXusDetector2::SetT0(int *t0): t0 pointer is null.";
    return 0;
  }

  int result = 1;
  unsigned int size=0;
  stringstream ss;

  switch (fT0Tag) {
    case -1:
      ss << "PNeXusDetector2::SetT0(int *t0): unkown fT0tag: " << fT0Tag << "!";
      fErrorMsg = ss.str();
      result = 0;
      break;
    case 1: // just one single t0
      size = 1;
      break;
    case 2: // t0[#histos]
      if (fNoOfSpectra <= 0) {
        fErrorMsg = "PNeXusDetector2::SetT0(int *t0): ask for t0 vector (ns), but ns <= 0!";
        result = 0;
      } else {
        size = fNoOfSpectra;
      }
      break;
    case 3: // t0[np][#histos]
      if ((fNoOfPeriods <= 0) || (fNoOfSpectra <= 0)) {
        fErrorMsg = "PNeXusDetector2::SetT0(int *t0): ask for t0 vector (np, ns), but either np or ns <= 0!";
        result = 0;
      } else {
        size = fNoOfPeriods * fNoOfSpectra;
      }
      break;
    default:
      ss << "PNeXusDetector2::SetT0(int *t0): unkown fT0tag: " << fT0Tag << "!";
      fErrorMsg = ss.str();
      result = 0;
      break;
  }

  // check for error
  if (!result)
    return result;

  // make sure fT0 memory is cleaned up before filled
  if (fT0) {
    delete [] fT0;
    fT0 = 0;
  }

  // allocate memory
  fT0 = new int[size];
  if (fT0 == 0) {
    fErrorMsg = "PNeXusDetector2::SetT0(int *t0): couldn't allocate necessary memory.";
    result = 0;
  } else {
    for (unsigned int i=0; i<size; i++)
      *(fT0+i) = *(t0+i);
  }

  return result;
}

//------------------------------------------------------------------------------------------
// GetFirstGoodBin (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Get first good bin if present, otherwise return -1.
 *
 * \param idxp period index
 * \param idxs spectrum index
 */
int PNeXusDetector2::GetFirstGoodBin(int idxp, int idxs)
{
  int result = -1;

  if ((idxp < 0) && (idxs < 0)) { // assumption: there is only ONE t0 for all spectra
    if (fFirstGoodBin != 0) {
      result = *fFirstGoodBin;
    }
  } else if ((idxp < 0) && (idxs >= 0)) { // assumptions: fgb's are represented as fgb[ns]
    if (idxs < fNoOfSpectra) {
      result = *(fFirstGoodBin+idxs);
    }
  } else if ((idxp >= 0) && (idxs >= 0)) { // assumption: fgb's are represented as fgb[np][ns]
    if ((idxp < fNoOfPeriods) || (idxs < fNoOfSpectra)) {
      result = *(fFirstGoodBin+idxp*fNoOfSpectra+idxs);
    }
  } else {
    result = -1;
  }

  return result;
}

//------------------------------------------------------------------------------------------
// SetFirstGoodBin (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set first good bins.
 *
 * <p><b>return:</b>
 * -1 of everything is OK,
 * -0 if there was an error, in which case GetErrorMsg() will give the internal error message.
 *
 * \param fgb is the pointer to the first good bin array.
 */
int PNeXusDetector2::SetFirstGoodBin(int *fgb)
{
  if (fgb == 0) {
    fErrorMsg = "PNeXusDetector2::SetFirstGoodBin(int *fgb): fgb pointer is null.";
    return 0;
  }

  int result = 1;
  unsigned int size=0;
  stringstream ss;

  switch (fT0Tag) {
    case -1:
      ss << "PNeXusDetector2::SetFirstGoodBin(int *fgb): unkown fT0tag: " << fT0Tag << "!";
      fErrorMsg = ss.str();
      result = 0;
      break;
    case 1: // single fgb
      size = 1;
      break;
    case 2: // fgb[#histos]
      if (fNoOfSpectra <= 0) {
        fErrorMsg = "PNeXusDetector2::SetFirstGoodBin(int *fgb): ask for fgb vector (ns), but ns <= 0!";
        result = 0;
      } else {
        size = fNoOfSpectra;
      }
      break;
    case 3: // fgb[np][#histos]
      if ((fNoOfPeriods <= 0) || (fNoOfSpectra <= 0)) {
        fErrorMsg = "PNeXusDetector2::SetFirstGoodBin(int *fgb): ask for fgb vector (np, ns), but either np or ns <= 0!";
        result = 0;
      } else {
        size = fNoOfPeriods * fNoOfSpectra;
      }
      break;
    default:
      ss << "PNeXusDetector2::SetFirstGoodBin(int *fgb): unkown fT0tag: " << fT0Tag << "!";
      fErrorMsg = ss.str();
      result = 0;
      break;
  }

  // check for error
  if (!result)
    return result;

  // make sure fFirstGoodBin memory is cleaned up before filled
  if (fFirstGoodBin) {
    delete [] fFirstGoodBin;
    fFirstGoodBin = 0;
  }

  // allocate memory
  fFirstGoodBin = new int[size];
  if (fFirstGoodBin == 0) {
    fErrorMsg = "PNeXusDetector2::SetFirstGoodBin(int *fgb): couldn't allocate necessary memory.";
    result = 0;
  } else {
    for (unsigned int i=0; i<size; i++)
      *(fFirstGoodBin+i) = *(fgb+i);
  }

  return result;
}

//------------------------------------------------------------------------------------------
// GetLastGoodBin (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Get last good bin if present, otherwise return -1.
 *
 * \param idxp period index
 * \param idxs spectrum index
 */
int PNeXusDetector2::GetLastGoodBin(int idxp, int idxs)
{
  int result = -1;

  if ((idxp < 0) && (idxs < 0)) { // assumption: there is only ONE t0 for all spectra
    if (fLastGoodBin != 0) {
      result = *fLastGoodBin;
    }
  } else if ((idxp < 0) && (idxs >= 0)) { // assumption: lgb's are represented as lgb[ns]
    if (idxs < fNoOfSpectra) {
      result = *(fLastGoodBin+idxs);
    }
  } else if ((idxp >= 0) && (idxs >= 0)) { // assumption: lgb's are represented as lgb[np][ns]
    if ((idxp < fNoOfPeriods) || (idxs < fNoOfSpectra)) {
      result = *(fLastGoodBin+idxp*fNoOfSpectra+idxs);
    }
  } else {
    result = -1;
  }

  return result;
}

//------------------------------------------------------------------------------------------
// SetLastGoodBin (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set last good bins.
 *
 * <p><b>return:</b>
 * -1 of everything is OK,
 * -0 if there was an error, in which case GetErrorMsg() will give the internal error message.
 *
 * \param lgb is the pointer to the first good bin array.
 */
int PNeXusDetector2::SetLastGoodBin(int *lgb)
{
  if (lgb == 0) {
    fErrorMsg = "PNeXusDetector2::SetLastGoodBin(int *lgb): fgb pointer is null.";
    return 0;
  }

  int result = 1;
  unsigned int size=0;
  stringstream ss;

  switch (fT0Tag) {
    case -1:
      ss << "PNeXusDetector2::SetLastGoodBin(int *lgb): unkown fT0tag: " << fT0Tag << "!";
      fErrorMsg = ss.str();
      result = 0;
      break;
    case 1: // single lgb only
      size = 1;
      break;
    case 2: // lgb[#histos]
      if (fNoOfSpectra <= 0) {
        fErrorMsg = "PNeXusDetector2::SetLastGoodBin(int *lgb): ask for lgb vector (ns), but ns <= 0!";
        result = 0;
      } else {
        size = fNoOfSpectra;
      }
      break;
    case 3: // lgb[np][#histos]
      if ((fNoOfPeriods <= 0) || (fNoOfSpectra <= 0)) {
        fErrorMsg = "PNeXusDetector2::SetLastGoodBin(int *lgb): ask for lgb vector (np, ns), but either np or ns <= 0!";
        result = 0;
      } else {
        size = fNoOfPeriods * fNoOfSpectra;
      }
      break;
    default:
      ss << "PNeXusDetector2::SetLastGoodBin(int *lgb): unkown fT0tag: " << fT0Tag << "!";
      fErrorMsg = ss.str();
      result = 0;
      break;
  }

  // check for error
  if (!result)
    return result;

  // make sure fLastGoodBin memory is cleaned up before filled
  if (fLastGoodBin) {
    delete [] fLastGoodBin;
    fLastGoodBin = 0;
  }

  // allocate memory
  fLastGoodBin = new int[size];
  if (fLastGoodBin == 0) {
    fErrorMsg = "PNeXusDetector2::SetLastGoodBin(int *lgb): couldn't allocate necessary memory.";
    result = 0;
  } else {
    for (unsigned int i=0; i<size; i++)
      *(fLastGoodBin+i) = *(lgb+i);
  }

  return result;
}

//------------------------------------------------------------------------------------------
// GetHistoValue (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Get histogram value. If any of the indices is out of range, -1 is returned. Since the
 * histogram can have the structures: histo[][][], histo[][], or histo[]
 *
 * \param idx_p period index
 * \param idx_s spectrum index
 * \param idx_b histogram bin index
 */
int PNeXusDetector2::GetHistoValue(int idx_p, int idx_s, int idx_b)
{
  int value = -1;

  if ((idx_b < 0) || (idx_b >= fNoOfBins)) { // make sure that idx_b is within proper bounds
    return -1;
  }

  if (fNoOfSpectra > 0) { // make sure that idx_s is within proper bounds if there are different spectra
    if ((idx_s < 0) || (idx_s >= fNoOfSpectra)) {
      return -1;
    }
  }

  if (fNoOfPeriods > 0) { // make sure that idx_p is within proper bounds if there are different periods
    if ((idx_p < 0) || (idx_p >= fNoOfPeriods)) {
      return -1;
    }
  }

  if (fNoOfPeriods > 0) {
    value = *(fHisto+idx_p*fNoOfSpectra*fNoOfBins+idx_s*fNoOfBins+idx_b);
  } else {
    if (fNoOfSpectra > 0) {
      value = *(fHisto+idx_s*fNoOfBins+idx_b);
    } else {
      value = *(fHisto+idx_b);
    }
  }

  return value;
}

//------------------------------------------------------------------------------------------
// SetHistos (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set values of all histograms.
 *
 * <p><b>return:</b>
 * - 1 if everything is OK.
 * - 0 something is wrong, check the internal error message via GetErrorMsg().
 *
 * \param histo pointer to the data.
 */
int PNeXusDetector2::SetHistos(int *histo)
{
  // make sure that histos are cleaned up before filled
  if (fHisto) {
    delete [] fHisto;
    fHisto = 0;
  }

  // check needed size and its consistency
  unsigned int size = 0;
  if (fNoOfPeriods > 0) { // (np, ns, nb)
    if ((fNoOfSpectra <= 0) || (fNoOfBins <= 0)) { // error
      fErrorMsg = "PNeXusDetector2::SetHistos(int *histo): claims format (np, ns, nb), but ns or nb < 0.";
      return 0;
    }
    size = fNoOfPeriods * fNoOfSpectra * fNoOfBins;
  } else { // (ns, nb) or (nb)
    if (fNoOfSpectra > 0) { // (ns, nb)
      if (fNoOfBins <= 0) { // error
        fErrorMsg = "PNeXusDetector2::SetHistos(int *histo): claims format (ns, nb), but nb < 0.";
        return 0;
      }
      size = fNoOfSpectra * fNoOfBins;
    } else { // (nb)
      if (fNoOfBins <= 0) {
        fErrorMsg = "PNeXusDetector2::SetHistos(int *histo): claims format (nb), but nb < 0.";
        return 0;
      }
      size = fNoOfBins;
    }
  }

  // allocate memory for fHisto
  fHisto = new int[size];
  if (fHisto == 0) {
    fErrorMsg = "PNeXusDetector2::SetHistos(int *histo): couldn't allocate necessary memory for fHisto.";
    return 0;
  }

  for (unsigned int i=0; i<size; i++)
    *(fHisto+i) = *(histo+i);

  return 1;
}

//------------------------------------------------------------------------------------------
// GetTimeResolution (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Get time resolution in the requested units. Allowed units are 'fs', 'ps', 'ns', 'us'.
 * If unsupported time units are given, a time resolution == 0.0 is returned. Internally the
 * time resolution is stored in 'ps'.
 *
 * \param units requested units for the time resolution.
 */
double PNeXusDetector2::GetTimeResolution(string units)
{
  double result=0.0;

  if (!units.compare("fs") || !units.compare("femto.second") || !units.compare("femto second") || !units.compare("femtoseconds"))
    result = fTimeResolution * 1.0e3;
  else if (!units.compare("ps") || !units.compare("pico.second") || !units.compare("pico second") || !units.compare("picoseconds"))
    result = fTimeResolution;
  else if (!units.compare("ns") || !units.compare("nano.second") || !units.compare("nano second") || !units.compare("nanoseconds"))
    result = fTimeResolution * 1.0e-3;
  else if (!units.compare("us") || !units.compare("micro.second") || !units.compare("micro second") || !units.compare("microseconds"))
    result = fTimeResolution * 1.0e-6;
  else
    result = 0.0;

  return result;
}

//------------------------------------------------------------------------------------------
// SetTimeResolution (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set the time resolution. 'units' tells in which units 'val' is provided. Acteptable units
 * are 'fs', 'ps', 'ns', 'us'.
 *
 * \param val time resolution value
 * \param units time resolution units
 */
void PNeXusDetector2::SetTimeResolution(double val, string units)
{
  if (!units.compare("fs") || !units.compare("femto.second") || !units.compare("femto second") || !units.compare("femtoseconds"))
    fTimeResolution = val * 1.0e-3;
  else if (!units.compare("ps") || !units.compare("pico.second") || !units.compare("pico second") || !units.compare("picoseconds"))
    fTimeResolution = val;
  else if (!units.compare("ns") || !units.compare("nano.second") || !units.compare("nano second") || !units.compare("nanoseconds"))
    fTimeResolution = val * 1.0e3;
  else if (!units.compare("us") || !units.compare("micro.second") || !units.compare("micro second") || !units.compare("microseconds"))
    fTimeResolution = val * 1.0e6;
}

//------------------------------------------------------------------------------------------
// SetRawTime (public)
//------------------------------------------------------------------------------------------
/**
 * <p>sets the raw time (deep copy).
 *
 * \param rawTime raw time vector.
 */
void PNeXusDetector2::SetRawTime(vector<double> &rawTime)
{
  for (unsigned int i=0; i<rawTime.size(); i++)
    fRawTime.push_back(rawTime[i]);
}

//------------------------------------------------------------------------------------------
// GetSpectrumIndex (public)
//------------------------------------------------------------------------------------------
/**
 * <p>returns the global spectrum index of index idx.
 *
 * \param idx spectrum index idx
 */
int PNeXusDetector2::GetSpectrumIndex(unsigned int idx)
{
  if (idx >= fSpectrumIndex.size())
    return -1;

  return fSpectrumIndex[idx];
}

//------------------------------------------------------------------------------------------
// SetSpectrumIndex (public)
//------------------------------------------------------------------------------------------
/**
 * <p>set the global spectrum index of index idx.
 *
 * \param spectIdx spectrum index value
 * \param idx spectrum index idx
 */
void PNeXusDetector2::SetSpectrumIndex(int spectIdx, int idx)
{
  if (idx < 0) {
    fSpectrumIndex.push_back(spectIdx);
  } else if (idx >= (int)fSpectrumIndex.size()) {
    fSpectrumIndex.resize(idx+1);
    fSpectrumIndex[idx] = spectIdx;
  } else {
    fSpectrumIndex[idx] = spectIdx;
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// IsValid (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NXinstrument data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXusInstrument2::IsValid(bool strict)
{
  string msg("");

  if (!fName.compare("n/a")) {
    msg = "IDF2 NXinstrument 'name' not set.";
    if (strict) {
      cerr << ">> **ERROR** " << msg << endl;
      return false;
    } else {
      cerr << ">> **WARNING** " << msg << endl;
    }
  }

  if (!fSource.IsValid(strict))
    return false;

  if (!fBeamline.IsValid(strict))
    return false;

  if (!fDetector.IsValid(strict))
    return false;

  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// PNeXusSource2 Constructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusSource2::PNeXusSource2()
{
  fName = "n/a";
  fType = "n/a";
  fProbe = "n/a";
}

//------------------------------------------------------------------------------------------
// IsValid (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NXsource data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXusSource2::IsValid(bool strict)
{
  string msg("");

  if (!fName.compare("n/a")) {
    msg = "IDF2 NXsample 'name' not set.";
    if (strict) {
      cerr << ">> **ERROR** " << msg << endl;
      return false;
    } else {
      cerr << ">> **WARNING** " << msg << endl;
    }
  }

  if (!fType.compare("n/a")) {
    msg = "IDF2 NXsample 'type' not set.";
    if (strict) {
      cerr << ">> **ERROR** " << msg << endl;
      return false;
    } else {
      cerr << ">> **WARNING** " << msg << endl;
    }
  }

  if (!fProbe.compare("n/a")) {
    msg = "IDF2 NXsample 'probe' not set.";
    if (strict) {
      cerr << ">> **ERROR** " << msg << endl;
      return false;
    } else {
      cerr << ">> **WARNING** " << msg << endl;
    }
  }

  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// PNeXusSample2 Constructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusSample2::PNeXusSample2()
{
  fName = "n/a";
  fDescription = "n/a";
  fMagneticFieldState = "n/a";
  fEnvironmentTemp = "n/a";
  fEnvironmentField = "n/a";
}

//------------------------------------------------------------------------------------------
// PNeXusSample2 Destructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusSample2::~PNeXusSample2()
{
  fPhysProp.clear();
}

//------------------------------------------------------------------------------------------
// IsValid (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NXsample data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXusSample2::IsValid(bool strict)
{
  string msg("");

  if (!fName.compare("n/a")) {
    cerr << ">> **WARNING** IDF2 NXsample 'name' not set." << msg << endl;
  }

  if (!fDescription.compare("n/a")) {
    cerr << endl << ">> **WARNING** IDF2 NXsample 'description' not set." << endl;
  }

  if (!fMagneticFieldState.compare("n/a")) {
    cerr << endl << ">> **WARNING** IDF2 NXsample 'magnetic_field_state' not set." << endl;
  }

  if (!fEnvironmentTemp.compare("n/a")) {
    cerr << endl << ">> **WARNING** IDF2 NXsample 'temperature_1_env' not set." << endl;
  }

  if (!fEnvironmentField.compare("n/a")) {
    cerr << endl << ">> **WARNING** IDF2 NXsample 'magnetic_field_1_env' not set." << endl;
  }

  bool ok;
  double dummy;

  dummy = GetPhysPropValue("temperature_1", ok);
  if (!ok) {
    cerr << endl << ">> **WARNING** IDF2 NXsample 'temperature_1' not set." << endl;
  }

  dummy = GetPhysPropValue("magnetic_field_1", ok);
  if (!ok) {
    cerr << endl << ">> **WARNING** IDF2 NXsample 'magnetic_field_1' not set." << endl;
  }

  return true;
}

//------------------------------------------------------------------------------------------
// GetPhysPropValue (public)
//------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * \param name
 * \param ok
 */
double PNeXusSample2::GetPhysPropValue(string name, bool &ok)
{
  double dval=0.0;
  ok = false;

  for (unsigned int i=0; i<fPhysProp.size(); i++) {
    if (!fPhysProp[i].GetName().compare(name)) {
      dval = fPhysProp[i].GetValue();
      ok = true;
      break;
    }
  }

  return dval;
}

//------------------------------------------------------------------------------------------
// GetPhysPropUnit (public)
//------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * \param name
 * \param ok
 */
void PNeXusSample2::GetPhysPropUnit(string name, string &unit, bool &ok)
{
  unit = "n/a";
  ok = false;

  for (unsigned int i=0; i<fPhysProp.size(); i++) {
    if (!fPhysProp[i].GetName().compare(name)) {
      unit = fPhysProp[i].GetUnit();
      ok = true;
      break;
    }
  }
}

//------------------------------------------------------------------------------------------
// SetPhysProp (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set the physical property with 'name' and 'value' at index 'idx'. If idx==-1 add it at the
 * end, otherwise set it at index idx.
 *
 * \param name of the physical property
 * \param value of the physical property
 * \param idx index where to set the physical property
 */
void PNeXusSample2::SetPhysProp(string name, double value, string unit, int idx)
{
  PNeXusProp prop;

  prop.SetName(name);
  prop.SetValue(value);
  prop.SetUnit(unit);

  if (idx == -1) {
    fPhysProp.push_back(prop);
  } else if (idx >= (int)fPhysProp.size()) {
    fPhysProp.resize(idx+1);
    fPhysProp[idx] = prop;
  } else {
    fPhysProp[idx] = prop;
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// PNeXusEntry2 Constructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXusEntry2::PNeXusEntry2()
{
  fErrorMsg = "";
  fDefinition = "n/a";
  fProgramName = "n/a";
  fProgramVersion = "n/a";
  fRunNumber = -1;
  fTitle = "n/a";
  fStartTime = "n/a";
  fStopTime = "n/a";
  fExperimentIdentifier = "n/a";
}

//------------------------------------------------------------------------------------------
// IsValid (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NXentry data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXusEntry2::IsValid(bool strict)
{
  string msg("");

  if (!fDefinition.compare("n/a")) {
    msg = "IDF2 NXentry definition not set.";
    if (strict) {
      cerr << endl << ">> **ERROR** " << msg << endl;
      return false;
    } else {
      cerr << endl << ">> **WARNING** " << msg << endl;
    }
  }

  if (fRunNumber == -1) {
    msg = "run number not set.";
    if (strict) {
      cerr << endl << ">> **ERROR** " << msg << endl;
      return false;
    } else {
      cerr << endl << ">> **WARNING** " << msg << endl;
    }
  }

  if (!fTitle.compare("n/a")) {
    msg = "run title not set.";
    if (strict) {
      cerr << endl << ">> **ERROR** " << msg << endl;
      return false;
    } else {
      cerr << endl << ">> **WARNING** " << msg << endl;
    }
  }

  if (!fStartTime.compare("n/a")) {
    msg = "start time not set.";
    if (strict) {
      cerr << endl << ">> **ERROR** " << msg << endl;
      return false;
    } else {
      cerr << endl << ">> **WARNING** " << msg << endl;
    }
  }

  if (!fStopTime.compare("n/a")) {
    msg = "end time not set.";
    if (strict) {
      cerr << endl << ">> **ERROR** " << msg << endl;
      return false;
    } else {
      cerr << endl << ">> **WARNING** " << msg << endl;
    }
  }

  if (!fExperimentIdentifier.compare("n/a")) {
    msg = "experiment identifier not set.";
    if (strict) {
      cerr << endl << ">> **ERROR** " << msg << endl;
      return false;
    } else {
      cerr << endl << ">> **WARNING** " << msg << endl;
    }
  }

  if (!fUser.IsValid(strict))
    return false;

  if (!fSample.IsValid(strict))
    return false;

  if (!fInstrument.IsValid(strict))
    return false;

  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------------------------------------
// SetStartTime (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set the start time string and check that it is ISO 8601 conform.
 *
 * \param time start time string
 */
int PNeXusEntry2::SetStartTime(string time)
{
  struct tm tm;
  memset(&tm, 0, sizeof(tm));
  strptime(time.c_str(), "%Y-%m-%d %H:%M:S", &tm);
  if (tm.tm_year == 0)
    strptime(time.c_str(), "%Y-%m-%dT%H:%M:S", &tm);
  if (tm.tm_year == 0) {
    fErrorMsg = "PNeXusEntry2::SetStartTime(): get year zero!";
    return NX_ERROR;
  }

  fStartTime = time;

  return NX_OK;
}

//------------------------------------------------------------------------------------------
// SetStopTime (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Set the stop time string and check that it is ISO 8601 conform.
 *
 * \param time stop time string
 */
int PNeXusEntry2::SetStopTime(string time)
{
  struct tm tm;
  memset(&tm, 0, sizeof(tm));
  strptime(time.c_str(), "%Y-%m-%d %H:%M:S", &tm);
  if (tm.tm_year == 0)
    strptime(time.c_str(), "%Y-%m-%dT%H:%M:S", &tm);
  if (tm.tm_year == 0) {
    fErrorMsg = "PNeXusEntry2::SetStopTime(): get year zero!";
    return NX_ERROR;
  }

  fStopTime = time;

  return NX_OK;
}

//------------------------------------------------------------------------------------------
// PNeXus Constructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXus::PNeXus()
{
  Init();
}

//------------------------------------------------------------------------------------------
// PNeXus Constructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * \param fileName
 */
PNeXus::PNeXus(const char* fileName)
{
  Init();

  fFileName = fileName;

  if (ReadFile(fileName) != NX_OK) {
    cerr << endl << fErrorMsg << " (error code=" << fErrorCode << ")" << endl << endl;
  } else {
    fValid = true;
  }
}

//------------------------------------------------------------------------------------------
// PNeXus Destructor
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
PNeXus::~PNeXus()
{
  if (fNxEntry1) {
    delete fNxEntry1;
    fNxEntry1 = 0;
  }
  if (fNxEntry2) {
    delete fNxEntry2;
    fNxEntry2 = 0;
  }

  for (unsigned int i=0; i<fGroupedHisto.size(); i++) {
    fGroupedHisto[i].clear();
  }
  fGroupedHisto.clear();
}

//------------------------------------------------------------------------------------------
// SetIdfVersion (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Define which IDF version shall be written/created.
 *
 * \param idf version number
 */
void PNeXus::SetIdfVersion(unsigned int idf)
{
  if ((idf != 1) && (idf != 2)) {
    cerr << endl << ">> **ERROR** Only IDF versions 1 and 2 are supported." << endl;
    return;
  }

  fIdfVersion = idf;

  if (idf == 1) { // IDF 1
    if (fNxEntry2) {
      delete fNxEntry2;
      fNxEntry2 = 0;
    }

    fNxEntry1 = new PNeXusEntry1();
    if (fNxEntry1 == 0) {
      cerr << endl << ">> **ERROR** couldn't invoke IDF 1 object PNeXusEntry1." << endl;
      return;
    }
  } else { // IDF 2
    if (fNxEntry1) {
      delete fNxEntry1;
      fNxEntry1 = 0;
    }

    fNxEntry2 = new PNeXusEntry2();
    if (fNxEntry2 == 0) {
      cerr << endl << ">> **ERROR** couldn't invoke IDF 2 object PNeXusEntry1." << endl;
      return;
    }
  }
}

//------------------------------------------------------------------------------------------
// Init (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Validates the NeXus data. A flag 'strict' controls the degree of validation. If 'strict' == true
 * a full NeXus validation is done, otherwise a much sloppier validation is performed. This sloppier
 * validation is needed when converting data.
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXus::IsValid(bool strict)
{
  bool valid = true;

  if (fIdfVersion == 1) // IDF Version 1
    valid = IsValidIdf1(strict);
  else if (fIdfVersion == 2) // IDF Version 2
    valid = IsValidIdf2(strict);
  else {
    cerr << endl << ">> **ERROR** found IDF Version " << fIdfVersion << ", which is not implemented yet." << endl << endl;
    valid = false;
  }

  return valid;
}

//------------------------------------------------------------------------------------------
// GetGroupedHisto (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Returns the grouped histogram if idx is within range, otherwise 0.
 *
 * \param idx index of the grouped histogram to be returned.
 */
vector<unsigned int>* PNeXus::GetGroupedHisto(unsigned int idx)
{
  if (idx >= fGroupedHisto.size())
    return 0;

  return &fGroupedHisto[idx];
}

//------------------------------------------------------------------------------------------
// ReadFile (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Read the NeXus file 'fileName'.
 *
 * <b>return:</b>
 * - NX_OK on successful reading
 * - NX_ERROR on error. The error code/message will give the details.
 *
 * \param fileName file name of the nexus file to be read
 */
int PNeXus::ReadFile(const char *fileName)
{
  fFileName = fileName;

  // open file
  NXstatus status;
  status = NXopen(fileName, NXACC_READ, &fFileHandle);
  if (status != NX_OK) {
    fErrorCode = PNEXUS_FILE_OPEN_ERROR;
    fErrorMsg  = "PNeXus::ReadFile() **ERROR** Couldn't open file "+string(fileName)+"!";
    return NX_ERROR;
  }

  // get idf in order to decide which read routine needs to be used.
  bool found = false;
  NXname nxname, nxclass;
  int dataType;
  // 1) get the first NXentry
  if (!SearchInGroup("NXentry", "class", nxname, nxclass, dataType)) {
    fErrorCode = PNEXUS_NXENTRY_NOT_FOUND;
    fErrorMsg  = "PNeXus::ReadFile() **ERROR** Couldn't find any NXentry!";
    return NX_ERROR;
  }
  // 2) open the NXentry group to obtain the IDF
  status = NXopengroup(fFileHandle, nxname, "NXentry");
  if (status != NX_OK) {
    fErrorCode = PNEXUS_GROUP_OPEN_ERROR;
    fErrorMsg  = "PNeXus::ReadFile() **ERROR** Couldn't open the NeXus group '" + string(nxname) + "'!";
    return NX_ERROR;
  }
  // 3) get the IDF
  // IDF
  found = false;
  do {
    status = NXgetnextentry(fFileHandle, nxname, nxclass, &dataType);
    if ((strstr(nxname, "IDF_version") != NULL) || (strstr(nxname, "idf_version") != NULL))
      found = true;
  } while (!found || (status == NX_EOD));

  if (!ErrorHandler(NXopendata(fFileHandle, nxname), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'IDF_version' nor 'idf_version' data!!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &fIdfVersion), PNEXUS_GET_DATA_ERROR, "couldn't read 'idf_version' data!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'IDF_version' data")) return NX_ERROR;
  // close group NXentry
  NXclosegroup(fFileHandle);

  // close file
  NXclose(&fFileHandle);

  switch (fIdfVersion) {
  case 1:
    status = ReadFileIdf1();
    if (status != NX_OK) {
      fErrorCode = PNEXUS_VAILD_READ_IDF1_FILE;
      fErrorMsg = ">> **ERROR** while reading IDF Version 1 file '" + fFileName + "'.";
    }
    break;
  case 2:
    status = ReadFileIdf2();
    if (status != NX_OK) {
      fErrorCode = PNEXUS_VAILD_READ_IDF2_FILE;
      fErrorMsg = ">> **ERROR** while reading IDF Version 2 file '" + fFileName + "'.";
    }
    break;
  default:
    fErrorCode = PNEXUS_IDF_NOT_IMPLEMENTED;
    stringstream ss;
    ss << ">> **ERROR** idf_version=" << fIdfVersion << " not yet implemented.";
    fErrorMsg = ss.str();
    status = NX_ERROR;
    break;
  }

  return status;
}

//------------------------------------------------------------------------------------------
// WriteFile (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Write the NeXus file 'fileName', with the 'fileType' (HDF4 | HDF5 | XML) and the instrument
 * definition (1 | 2).
 *
 * <b>return:</b>
 * - NX_OK on successfull reading
 * - NX_ERROR on error. The error code/message will give the details.
 *
 * \param fileName file name of the nexus file to be written
 * \param fileType = HDF4 | HDF5 | XML
 * \param idf = 1 or 2
 */
int PNeXus::WriteFile(const char *fileName, const char *fileType, const unsigned int idf)
{
  if (!IsValid()) {
    return NX_ERROR;
  }

  int status = NX_OK;
  NXaccess access=NXACC_CREATE4;

  if (!strcmp(fileType, "hdf4"))
    access=NXACC_CREATE4;
  else if (!strcmp(fileType, "hdf5"))
    access=NXACC_CREATE5;
  else if (!strcmp(fileType, "xml"))
    access=NXACC_CREATEXML;
  else
    access=NXACC_CREATE4;

  switch (idf) {
  case 1:
    status = WriteFileIdf1(fileName, access);
    break;
  case 2:
    status = WriteFileIdf2(fileName, access);
    break;
  default:
    break;
  }

  return status;
}

//------------------------------------------------------------------------------------------
// Dump (public)
//------------------------------------------------------------------------------------------
/**
 * <p>Write the content of the NeXus file to stdout. Used for debugging purposes.
 *
 */
void PNeXus::Dump()
{
  double dval;
  string str;
  bool ok;

  if (fIdfVersion == 1) {
    cout << endl << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    cout << endl << "NXfile:";
    cout << endl << "  NeXus_version  : " << fNeXusVersion;
    cout << endl << "  file format ver: " << fFileFormatVersion;
    cout << endl << "  file name      : " << fFileName;
    cout << endl << "  file time      : " << fFileTime;
    cout << endl << "  user           : " << fCreator;
    cout << endl << "NXentry:";
    cout << endl << "  idf version    : " << fIdfVersion;
    cout << endl << "  program name   : " << fNxEntry1->GetProgramName() << ", version: " << fNxEntry1->GetProgramVersion();
    cout << endl << "  run number     : " << fNxEntry1->GetRunNumber();
    cout << endl << "  title          : " << fNxEntry1->GetTitle();
    cout << endl << "  notes          : " << fNxEntry1->GetNotes();
    cout << endl << "  analysis       : " << fNxEntry1->GetAnalysis();
    cout << endl << "  laboratory     : " << fNxEntry1->GetLaboratory();
    cout << endl << "  beamline       : " << fNxEntry1->GetBeamline();
    cout << endl << "  start time     : " << fNxEntry1->GetStartTime();
    cout << endl << "  stop time      : " << fNxEntry1->GetStopTime();
    cout << endl << "  switching state: " << fNxEntry1->GetSwitchingState() << ", '1' normal data collection, '2' Red/Green mode";
    cout << endl << "NXuser:";
    cout << endl << "  name           : " << fNxEntry1->GetUser()->GetName();
    cout << endl << "  exp.number     : " << fNxEntry1->GetUser()->GetExperimentNumber();
    cout << endl << "NXsample:";
    cout << endl << "  name           : " << fNxEntry1->GetSample()->GetName();
    dval = fNxEntry1->GetSample()->GetPhysPropValue("temperature", ok);
    if (ok)
      cout << endl << "  temperature    : " << dval;
    fNxEntry1->GetSample()->GetPhysPropUnit("temperature", str, ok);
    if (ok)
      cout << " (" << str << ")";
    dval = fNxEntry1->GetSample()->GetPhysPropValue("magnetic_field", ok);
    if (ok)
      cout << endl << "  magnetic_field : " << dval;
    fNxEntry1->GetSample()->GetPhysPropUnit("magnetic_field", str, ok);
    if (ok)
      cout << " (" << str << ")";
    cout << endl << "  shape          : " << fNxEntry1->GetSample()->GetShape();
    cout << endl << "  mag.field state: " << fNxEntry1->GetSample()->GetMagneticFieldState();
    cout << endl << "  environment    : " << fNxEntry1->GetSample()->GetEnvironment();
    if (fNxEntry1->GetSample()->IsMagneticFieldVectorAvailable()) {
      cout << endl << "  magnetic field vector: ";
      for (unsigned int i=0; i<fNxEntry1->GetSample()->GetMagneticFieldVector().size(); i++) {
        cout << fNxEntry1->GetSample()->GetMagneticFieldVector().at(i) << ", ";
      }
      cout << "(" << fNxEntry1->GetSample()->GetMagneticFieldVectorUnits() << "), ";
      cout << fNxEntry1->GetSample()->GetMagneticFieldVectorCoordinateSystem();
    }
    cout << endl << "NXinstrument:";
    cout << endl << "  name           : " << fNxEntry1->GetInstrument()->GetName();
    cout << endl << "  NXdetector:";
    cout << endl << "    number of detectors: " << fNxEntry1->GetInstrument()->GetDetector()->GetNumber();
    cout << endl << "  NXcollimator:";
    cout << endl << "    type               : " << fNxEntry1->GetInstrument()->GetCollimator()->GetType();
    cout << endl << "  Nxbeam:";
    cout << endl << "    total_counts       : " << fNxEntry1->GetInstrument()->GetBeam()->GetTotalCounts() << " (" << fNxEntry1->GetInstrument()->GetBeam()->GetUnits() << ")";
    cout << endl << "NXdata:";
    cout << endl << "  number of histos     : " << fNxEntry1->GetData()->GetNoOfHistos();
    cout << endl << "  time resolution      : " << fNxEntry1->GetData()->GetTimeResolution("ns") << " (ns)";
    if (fNxEntry1->GetData()->GetGrouping()->size() != 0) {
     cout << endl << "  grouping             : ";
     for (unsigned int i=0; i<fNxEntry1->GetData()->GetGrouping()->size(); i++) {
       cout << "(" << i << "/" << fNxEntry1->GetData()->GetGrouping()->at(i) << "), ";
     }
    } else {
     cout << endl << "  grouping             : not available";
    }
    if (fNxEntry1->GetData()->GetT0s()->size() == 0) {
      cout << endl << "  t0's                 : not available";
    } else {
      cout << endl << "  t0's                 : ";
      for (unsigned int i=0; i<fNxEntry1->GetData()->GetT0s()->size(); i++) {
        cout << fNxEntry1->GetData()->GetT0(i) << ", ";
      }
    }
    if (fNxEntry1->GetData()->GetFirstGoodBins()->size() == 0) {
      cout << endl << "  first good bins      : not available";
    } else {
      cout << endl << "  first good bins      : ";
      for (unsigned int i=0; i<fNxEntry1->GetData()->GetFirstGoodBins()->size(); i++) {
        cout << fNxEntry1->GetData()->GetFirstGoodBin(i) << ", ";
      }
    }
    if (fNxEntry1->GetData()->GetLastGoodBins()->size() == 0) {
      cout << endl << "  last good bins       : not available";
    } else {
      cout << endl << "  last good bins       : ";
      for (unsigned int i=0; i<fNxEntry1->GetData()->GetLastGoodBins()->size(); i++) {
        cout << fNxEntry1->GetData()->GetLastGoodBin(i) << ", ";
      }
    }
    if (fNxEntry1->GetData()->GetNoOfHistos() == 0) {
      cout << endl << "  historgrams          : not available";
    } else {
      cout << endl << "  historgrams          : +++++++++++++";
      for (unsigned int i=0; i<fNxEntry1->GetData()->GetNoOfHistos(); i++) {
        cout << endl << "   histo " << i+1 << ": ";
        for (unsigned int j=0; j<15; j++) {
          cout << fNxEntry1->GetData()->GetHisto(i)->at(j) << ", ";
        }
        cout << "...";
      }
    }
    if (fNxEntry1->GetData()->GetAlpha()->size() == 0) {
      cout << endl << "  alpha                 : not available";
    } else {
      cout << endl << "  alpha                 : ";
      for (unsigned int i=0; i<fNxEntry1->GetData()->GetAlpha()->size(); i++)
        cout << "(" << fNxEntry1->GetData()->GetAlpha()->at(i).GetGroupFirst() << "/" << fNxEntry1->GetData()->GetAlpha()->at(i).GetGroupSecond() << "/" << fNxEntry1->GetData()->GetAlpha()->at(i).GetAlpha() << "), ";
    }
    if (fGroupedHisto.size() == 0) {
      cout << endl << "  grouped historgrams   : not available";
    } else {
      cout << endl << "  grouped historgrams   : +++++++++++++";
      for (unsigned int i=0; i<fGroupedHisto.size(); i++) {
        cout << endl << "   grouped histo " << i+1 << ": ";
        for (unsigned int j=0; j<15; j++) {
          cout << fGroupedHisto[i][j] << ", ";
        }
        cout << "...";
      }
    }
    cout << endl << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    cout << endl << "that's all!";
    cout << endl << endl;
  } else if (fIdfVersion == 2) {
    cout << endl << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    cout << endl << "NXfile:";
    cout << endl << "  file_name           : " << fFileName;
    cout << endl << "  file_time           : " << fFileTime;
    cout << endl << "  NeXus_version       : " << fNeXusVersion;
    cout << endl << "  file format version : " << fFileFormatVersion;
    cout << endl << "  creator             : " << fCreator;
    cout << endl << "NXentry:";
    cout << endl << "  idf version         : " << fIdfVersion;
    cout << endl << "  definition          : " << fNxEntry2->GetDefinition();
    if (!fNxEntry2->GetProgramName().empty())
      cout << endl << "  program name        : " << fNxEntry2->GetProgramName() << ", version: " << fNxEntry2->GetProgramVersion();
    cout << endl << "  run_number          : " << fNxEntry2->GetRunNumber();
    cout << endl << "  run_title           : " << fNxEntry2->GetTitle();
    cout << endl << "  start_time          : " << fNxEntry2->GetStartTime();
    cout << endl << "  end_time            : " << fNxEntry2->GetStopTime();
    cout << endl << "  exp.identifier      : " << fNxEntry2->GetExperimentIdentifier();
    cout << endl << "  NXuser:";
    cout << endl << "    name              : " << fNxEntry2->GetUser()->GetName();
    cout << endl << "  NXsample:";
    cout << endl << "    name              : " << fNxEntry2->GetSample()->GetName();
    cout << endl << "    description       : " << fNxEntry2->GetSample()->GetDescription();
    cout << endl << "    mag.field state   : " << fNxEntry2->GetSample()->GetMagneticFieldState();
    dval = fNxEntry2->GetSample()->GetPhysPropValue("temperature_1", ok);
    if (ok)
      cout << endl << "    temperature       : " << dval;
    fNxEntry2->GetSample()->GetPhysPropUnit("temperature_1", str, ok);
    if (ok)
      cout << " (" << str << ")";
    cout << endl << "    temp.environment  : " << fNxEntry2->GetSample()->GetEnvironmentTemp();
    dval = fNxEntry2->GetSample()->GetPhysPropValue("magnetic_field_1", ok);
    if (ok)
      cout << endl << "    magnetic_field    : " << dval;
    fNxEntry2->GetSample()->GetPhysPropUnit("magnetic_field_1", str, ok);
    if (ok)
      cout << " (" << str << ")";
    cout << endl << "    mag. field env.   : " << fNxEntry2->GetSample()->GetEnvironmentField();
    cout << endl << "  NXinstrument:";
    cout << endl << "    name              : " << fNxEntry2->GetInstrument()->GetName();
    cout << endl << "    NXsource:";
    cout << endl << "      name            : " << fNxEntry2->GetInstrument()->GetSource()->GetName();
    cout << endl << "      type            : " << fNxEntry2->GetInstrument()->GetSource()->GetType();
    cout << endl << "      probe           : " << fNxEntry2->GetInstrument()->GetSource()->GetProbe();
    cout << endl << "    NXbeamline:";
    cout << endl << "      name            : " << fNxEntry2->GetInstrument()->GetBeamline()->GetName();
    cout << endl << "    NXdetector:";
    cout << endl << "      description     : " << fNxEntry2->GetInstrument()->GetDetector()->GetDescription();
    cout << endl << "      time resolution : " << fNxEntry2->GetInstrument()->GetDetector()->GetTimeResolution("ns") << " (ns)";
    if (fNxEntry2->GetInstrument()->GetDetector()->IsT0Present()) {
      if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 1) { // only one t0 for all histograms
        cout << endl << "      t0              : " << fNxEntry2->GetInstrument()->GetDetector()->GetT0();
      } else if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 2) { // t0[ns]
        cout << endl << "      t0 (idx_s/t0)   : ";
        for (int i=0; i<fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra(); i++) {
          cout << "(" << i+1 << "/" << fNxEntry2->GetInstrument()->GetDetector()->GetT0(-1, i) << "), ";
        }
      } else { // t0 vector of the form t0[np][ns]
        cout << endl << "      t0 (idx_p/idx_s/t0): ";
        for (int i=0; i<fNxEntry2->GetInstrument()->GetDetector()->GetNoOfPeriods(); i++) {
          for (int j=0; j<fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra(); j++) {
            cout << "(" << i+1 << "/" << j+1 << "/" << fNxEntry2->GetInstrument()->GetDetector()->GetT0(i,j) << "), ";
          }
        }
      }
    } else {
      cout << endl << "      t0              : n/a";
    }
    if (fNxEntry2->GetInstrument()->GetDetector()->IsFirstGoodBinPresent()) {
      if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 1) { // only one fgb for all histograms
        cout << endl << "      first good bin  : " << fNxEntry2->GetInstrument()->GetDetector()->GetFirstGoodBin();
      } else if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 2) { // fgb[ns]
        cout << endl << "      fgb (idx_s/fgb) : ";
        for (int i=0; i<fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra(); i++) {
          cout << "(" << i+1 << "/" << fNxEntry2->GetInstrument()->GetDetector()->GetFirstGoodBin(-1,i) << ") , ";
        }
      } else { // fgb vector of the form fgb[np][ns]
        cout << endl << "      fgb (idx_p/idx_s/fgb): ";
        for (int i=0; i<fNxEntry2->GetInstrument()->GetDetector()->GetNoOfPeriods(); i++) {
          for (int j=0; j<fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra(); j++) {
            cout << "(" << i+1 << "/" << j+1 << "/" << fNxEntry2->GetInstrument()->GetDetector()->GetFirstGoodBin(i,j);
          }
        }
      }
    } else {
      cout << endl << "      first good bin  : n/a";
    }
    if (fNxEntry2->GetInstrument()->GetDetector()->IsLastGoodBinPresent()) {
      if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 1) { // only one lgb for all histograms
        cout << endl << "      last good bin   : " << fNxEntry2->GetInstrument()->GetDetector()->GetLastGoodBin();
      } else if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 2) { // lgb[ns]
        cout << endl << "      lgb (idx_s/lgb) : ";
        for (int i=0; i<fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra(); i++) {
          cout << "(" << i+1 << "/" << fNxEntry2->GetInstrument()->GetDetector()->GetLastGoodBin(-1,i) << "), ";
        }
      } else { // lgb vector of the form lgb[np][ns]
        cout << endl << "      lgb (idx_p/idx_s/lgb): ";
        for (int i=0; i<fNxEntry2->GetInstrument()->GetDetector()->GetNoOfPeriods(); i++) {
          for (int j=0; j<fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra(); j++) {
            cout << "(" << i+1 << "/" << j+1 << "/" << fNxEntry2->GetInstrument()->GetDetector()->GetLastGoodBin(i,j) << "), ";
          }
        }
      }
    } else {
      cout << endl << "      last good bin   : n/a";
    }
    cout << endl << "      spectrum_index  : ";
    for (unsigned int i=0; i<fNxEntry2->GetInstrument()->GetDetector()->GetSpectrumIndexSize(); i++)
      cout << fNxEntry2->GetInstrument()->GetDetector()->GetSpectrumIndex(i) << ", ";

    // dump data
    int maxDump = 15;
    cout << endl << "      counts    : ";
    if (fNxEntry2->GetInstrument()->GetDetector()->GetNoOfPeriods() > 0) { // counts[np][ns][ntc]
      for (int i=0; i<fNxEntry2->GetInstrument()->GetDetector()->GetNoOfPeriods(); i++) {
        cout << endl << "       period    : " << i+1;
        for (int j=0; j<fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra(); j++) {
          cout << endl << "        spectrum : " << j+1 << ", (#bins=" << fNxEntry2->GetInstrument()->GetDetector()->GetNoOfBins() << ")";
          if (maxDump > fNxEntry2->GetInstrument()->GetDetector()->GetNoOfBins())
            maxDump = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfBins();
          cout << endl << "         ";
          for (int k=0; k<maxDump; k++) {
            cout << fNxEntry2->GetInstrument()->GetDetector()->GetHistoValue(i,j,k) << ", ";
          }
          cout << "...";
        }
      }
    } else {
      if (fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra() > 0) {  // counts[ns][ntc]
        for (int j=0; j<fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra(); j++) {
          cout << endl << "       spectrum : " << j+1 << ", (#bins=" << fNxEntry2->GetInstrument()->GetDetector()->GetNoOfBins() << ")";
          if (maxDump > fNxEntry2->GetInstrument()->GetDetector()->GetNoOfBins())
            maxDump = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfBins();
          cout << endl << "        ";
          for (int k=0; k<maxDump; k++) {
            cout << fNxEntry2->GetInstrument()->GetDetector()->GetHistoValue(0,j,k) << ", ";
          }
          cout << "...";
        }
      } else {  // counts[ntc]
        cout << endl << "      (#bins=" << fNxEntry2->GetInstrument()->GetDetector()->GetNoOfBins() << ")";
        if (maxDump > fNxEntry2->GetInstrument()->GetDetector()->GetNoOfBins())
          maxDump = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfBins();
        cout << endl << "      ";
        for (int k=0; k<maxDump; k++) {
          cout << fNxEntry2->GetInstrument()->GetDetector()->GetHistoValue(0,0,k) << ", ";
        }
        cout << "...";
      }
    }
    cout << endl << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++";
    cout << endl << "that's all!";
    cout << endl << endl;
  }
}

//------------------------------------------------------------------------------------------
// Init (private)
//------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PNeXus::Init()
{
  fValid = false;
  fErrorCode = PNEXUS_NO_ERROR;
  fErrorMsg = "n/a";
  fNeXusVersion = NEXUS_VERSION;
  fFileFormatVersion = "n/a";
  fIdfVersion = 0;
  fFileName = "n/a";
  fFileTime = "n/a";
  fCreator = "n/a";

  fNxEntry1 = 0;
  fNxEntry2 = 0;
}

//-----------------------------------------------------------------------------------------------------
// ErrorHandler (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - true of no error occurred
 * - false on error
 *
 * \param status of the calling routine
 * \param errCode will set the fErrorCode of the class
 * \param errMsg will set the fErrorMsg of the class
 */
bool PNeXus::ErrorHandler(NXstatus status, int errCode, const string &errMsg)
{
  if (status != NX_OK) {
    fErrorCode = errCode;
    fErrorMsg = errMsg;
    cerr << endl << ">> **ERROR** " << fErrorMsg << endl;
    if (fFileHandle != 0) {
      NXclose(&fFileHandle);
    }
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------------------------------
// GetStringData (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - NX_OK on success
 * - NX_ERROR otherwiese
 *
 * \param str string to be fed
 */
NXstatus PNeXus::GetStringData(string &str)
{
  int i, status, rank, type, dims[32];
  char cstr[1024];
  NXname data_value;

  status = NXgetinfo(fFileHandle, &rank, dims, &type);
  if (status != NX_OK) {
    cerr << endl << ">> **ERROR** in NXgetinfo: couldn't get meta info!" << endl;
    fErrorCode = PNEXUS_GET_META_INFO_ERROR;
    fErrorMsg = "PNeXus::GetStringData() **ERROR** couldn't get meta info!";    
    return NX_ERROR;
  }

  if ((type != NX_CHAR) || (rank > 1) || (dims[0] >= (int)sizeof(cstr))) {
    cerr << endl << ">> **ERROR** in NXgetinfo: found wrong meta info!" << endl;
    fErrorCode = PNEXUS_WRONG_META_INFO;
    fErrorMsg = "PNeXus::GetStringData() **ERROR** found wrong meta info!";
    return NX_ERROR;
  }

  status = NXgetdata(fFileHandle, data_value);
  if (status != NX_OK) {
    cerr << endl << ">> **ERROR** in routine NXgetdata: couldn't get data for '" << str << "'!" << endl;
    fErrorCode = PNEXUS_GET_DATA_ERROR;
    fErrorMsg = "couldn't get data!";
    return NX_ERROR;
  }

  for (i = 0; i < dims[0]; i++)
    cstr[i] = *(data_value + i);
  cstr[i] = '\0';

  str = cstr;

  return NX_OK;
}

//-----------------------------------------------------------------------------------------------------
// GetStringAttr (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - NX_OK on success
 * - NX_ERROR otherwiese
 *
 * \param attr attribute tag
 * \param str string to be fed
 */
NXstatus PNeXus::GetStringAttr(string attr, string &str)
{
  int i, status, attlen, atttype;
  char cstr[VGNAMELENMAX];
  NXname data_value;

  attlen = VGNAMELENMAX - 1;
  atttype = NX_CHAR;
  status = NXgetattr(fFileHandle, (char *)attr.c_str(), data_value, &attlen, &atttype);
  if (status != NX_OK) {
    cerr << endl << ">> **ERROR** in routine NXgetattr: couldn't get attribute '" << attr << "'! status=" << status << endl << endl;
    fErrorCode = PNEXUS_GET_ATTR_ERROR;
    fErrorMsg = "couldn't get string attribute data!";
    return NX_ERROR;
  }

  for (i = 0; i < attlen; i++)
    cstr[i] = *(data_value + i);
  cstr[i] = '\0';

  str = cstr;

  return NX_OK;
}

//-----------------------------------------------------------------------------------------------------
// GetDataSize (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - size in bytes of the given type
 * - 0 if the type is not recognized
 *
 * \param type
 */
int PNeXus::GetDataSize(int type)
{
  int size;

  switch (type) {
    case NX_CHAR:
      size = SIZE_CHAR8;
      break;
    case NX_FLOAT32:
      size = SIZE_FLOAT32;
      break;
    case NX_FLOAT64:
      size = SIZE_FLOAT64;
      break;
    case NX_INT8:
      size = SIZE_INT8;
      break;
    case NX_UINT8:
      size = SIZE_UINT8;
      break;
    case NX_INT16:
      size = SIZE_INT16;
      break;
    case NX_UINT16:
      size = SIZE_UINT16;
      break;
    case NX_INT32:
      size = SIZE_INT32;
      break;
    case NX_UINT32:
      size = SIZE_UINT32;
      break;
    default:
      size = 0;
      break;
  }

  return size;
}

//-----------------------------------------------------------------------------------------------------
// GetDoubleVectorData (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - NX_OK on success
 * - NX_ERROR otherwiese
 *
 * \param data
 */
NXstatus PNeXus::GetDoubleVectorData(vector<double> &data)
{
  // get information of the current nexus entity
  int rank, type, dims[32], size, noOfElements;
  if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get data info!"))
    return NX_ERROR;

  // calculate the needed size
  size = dims[0];
  for (int i=1; i<rank; i++)
    size *= dims[i];
  noOfElements = size;
  size *= GetDataSize(type);

  // allocate locale memory to get the data
  char *data_ptr = new char[size];
  if (data_ptr == 0) {
    return NX_ERROR;
  }

  // get the data
  float *f_data_ptr = (float*) data_ptr;
  int status = NXgetdata(fFileHandle, f_data_ptr);
  if (status != NX_OK) {
    return NX_ERROR;
  }

  // copy the data into the vector
  data.clear();
  for (int i=0; i<noOfElements; i++) {
    data.push_back(*(f_data_ptr+i));
  }

  // clean up
  if (data_ptr) {
    delete [] data_ptr;
  }

  return NX_OK;
}

//-----------------------------------------------------------------------------------------------------
// GetIntVectorData (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - NX_OK on success
 * - NX_ERROR otherwiese
 *
 * \param data
 */
NXstatus PNeXus::GetIntVectorData(vector<int> &data)
{
  // get information of the current nexus entity
  int rank, type, dims[32], size, noOfElements;
  if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get data info!"))
    return NX_ERROR;

  // calculate the needed size
  size = dims[0];
  for (int i=1; i<rank; i++)
    size *= dims[i];
  noOfElements = size;
  size *= GetDataSize(type);

  // allocate locale memory to get the data
  char *data_ptr = new char[size];
  if (data_ptr == 0) {
    return NX_ERROR;
  }

  // get the data
  int *i_data_ptr = (int*) data_ptr;
  int status = NXgetdata(fFileHandle, i_data_ptr);
  if (status != NX_OK) {
    return NX_ERROR;
  }

  // copy the data into the vector
  data.clear();
  for (int i=0; i<noOfElements; i++) {
    data.push_back(*(i_data_ptr+i));
  }

  // clean up
  if (data_ptr) {
    delete [] data_ptr;
  }

  return NX_OK;
}

//------------------------------------------------------------------------------------------
// ReadFileIdf1 (private)
//------------------------------------------------------------------------------------------
/**
 * <p>Read the NeXus file of type IDF Version 1.
 *
 * <b>return:</b>
 * - NX_OK on successfull reading
 * - NX_ERROR on error. The error code/message will give the details.
 */
int PNeXus::ReadFileIdf1()
{
  cout << endl << ">> reading NeXus IDF Version 1 file ..." << endl;

  // create first the necessary NXentry object for IDF Version 1
  fNxEntry1 = new PNeXusEntry1();
  if (fNxEntry1 == 0) {
    fErrorCode = PNEXUS_OBJECT_INVOK_ERROR;
    fErrorMsg = ">> **ERROR** couldn't invoke PNeXusEntry1 object.";
    return NX_ERROR;
  }

  string str;
  char cstr[128];
  int ival;
  float fval;
  int attlen, atttype;
  NXname data_value, nxAttrName;

  // open file
  NXstatus status;
  status = NXopen(fFileName.c_str(), NXACC_READ, &fFileHandle);
  if (status != NX_OK) {
    fErrorCode = PNEXUS_FILE_OPEN_ERROR;
    fErrorMsg  = "PNeXus::ReadFile() **ERROR** Couldn't open file '"+fFileName+"' !";
    return NX_ERROR;
  }

  // collect the NXroot attribute information
  do {
    status = NXgetnextattr(fFileHandle, nxAttrName, &attlen, &atttype);
    if (status == NX_OK) {
      if (!strcmp(nxAttrName, "HDF_version")) {
        attlen = VGNAMELENMAX - 1;
        atttype = NX_CHAR;
        status = NXgetattr(fFileHandle, nxAttrName, data_value, &attlen, &atttype);
        if (status == NX_OK) {
          fFileFormatVersion = string(data_value);
        }
      } else if (!strcmp(nxAttrName, "HDF5_Version")) {
        attlen = VGNAMELENMAX - 1;
        atttype = NX_CHAR;
        status = NXgetattr(fFileHandle, nxAttrName, data_value, &attlen, &atttype);
        if (status == NX_OK) {
          fFileFormatVersion = string("HDF5: ")+string(data_value);
        }
      } else if (!strcmp(nxAttrName, "XML_version")) {
        attlen = VGNAMELENMAX - 1;
        atttype = NX_CHAR;
        status = NXgetattr(fFileHandle, nxAttrName, data_value, &attlen, &atttype);
        if (status == NX_OK) {
          fFileFormatVersion = string(data_value);
        }
      } else if (!strcmp(nxAttrName, "file_name")) {
        if (!ErrorHandler(GetStringAttr("file_name", str), PNEXUS_GET_ATTR_ERROR, "couldn't read NXroot 'file_name' attribute!")) return NX_ERROR;
        fFileName = str;
      } else if (!strcmp(nxAttrName, "file_time")) {
        if (!ErrorHandler(GetStringAttr("file_time", str), PNEXUS_GET_ATTR_ERROR, "couldn't read NXroot 'file_time' attribute!")) return NX_ERROR;
        fFileTime = str;
      } else if (!strcmp(nxAttrName, "user")) {
        if (!ErrorHandler(GetStringAttr("user", str), PNEXUS_GET_ATTR_ERROR, "couldn't read NXroot 'user' attribute!")) return NX_ERROR;
        fCreator = str;
      }
    }
  } while (status == NX_OK);

  // look for the first occurring NXentry
  bool found = false;
  NXname nxname, nxclass;
  int dataType;
  do {
    status = NXgetnextentry(fFileHandle, nxname, nxclass, &dataType);
    if (strstr(nxclass, "NXentry") != NULL)
      found = true;
  } while (!found || (status == NX_EOD));
  // make sure any NXentry has been found
  if (!found) {
    fErrorCode = PNEXUS_NXENTRY_NOT_FOUND;
    fErrorMsg  = ">> **ERROR** Couldn't find any NXentry!";
    return NX_ERROR;
  }
  // open the NXentry group to obtain the necessary stuff
  status = NXopengroup(fFileHandle, nxname, "NXentry");
  if (status != NX_OK) {
    fErrorCode = PNEXUS_GROUP_OPEN_ERROR;
    fErrorMsg  = "PNeXus::ReadFile() **ERROR** Couldn't open the NeXus group '" + string(nxname) + "'!";
    return NX_ERROR;
  }

  // program_name
  if (!ErrorHandler(NXopendata(fFileHandle, "program_name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'program_name' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'program_name' data in NXentry!")) return NX_ERROR;
  fNxEntry1->SetProgramName(str);
  if (!ErrorHandler(GetStringAttr("version", str), PNEXUS_GET_ATTR_ERROR, "couldn't read 'program_name' attribute in NXentry!")) return NX_ERROR;
  fNxEntry1->SetProgramVersion(str);
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'program_name' data in NXentry!")) return NX_ERROR;

  // run number
  if (!ErrorHandler(NXopendata(fFileHandle, "number"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'number' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &ival), PNEXUS_GET_DATA_ERROR, "couldn't read 'number' data in NXentry!")) return NX_ERROR;
  fNxEntry1->SetRunNumber(ival);
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'number' data in NXentry!")) return NX_ERROR;

  // title
  if (!ErrorHandler(NXopendata(fFileHandle, "title"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'title' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'title' data in NXentry!")) return NX_ERROR;
  fNxEntry1->SetTitle(str);
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'title' data in NXentry!")) return NX_ERROR;

  // notes
  if (!ErrorHandler(NXopendata(fFileHandle, "notes"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'notes' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'notes' data in NXentry!")) return NX_ERROR;
  fNxEntry1->SetNotes(str);
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'notes' data in NXentry!")) return NX_ERROR;

  // analysis tag
  if (!ErrorHandler(NXopendata(fFileHandle, "analysis"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'analysis' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'analysis' data in NXentry!")) return NX_ERROR;
  fNxEntry1->SetAnalysis(str);
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'analysis' data in NXentry!")) return NX_ERROR;

  // lab
  if (!ErrorHandler(NXopendata(fFileHandle, "lab"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'lab' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'lab' data in NXentry!")) return NX_ERROR;
  fNxEntry1->SetLaboratory(str);
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'lab' data in NXentry!")) return NX_ERROR;

  // beamline
  if (!ErrorHandler(NXopendata(fFileHandle, "beamline"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'beamline' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'beamline' data in NXentry!")) return NX_ERROR;
  fNxEntry1->SetBeamline(str);
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'beamline' data in NXentry!")) return NX_ERROR;

  // start time
  if (!ErrorHandler(NXopendata(fFileHandle, "start_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'start_time' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'start_time' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'start_time' data in NXentry!")) return NX_ERROR;
  if (fNxEntry1->SetStartTime(str) != NX_OK) {
    fErrorCode = PNEXUS_TIME_FORMAT_ERROR;
    fErrorMsg = "start time format is not ISO 8601 conform.";
  }

  // stop time
  if (!ErrorHandler(NXopendata(fFileHandle, "stop_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'start_time' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'start_time' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'start_time' data in NXentry!")) return NX_ERROR;
  if (fNxEntry1->SetStopTime(str) != NX_OK) {
    fErrorCode = PNEXUS_TIME_FORMAT_ERROR;
    fErrorMsg = "stop time format is not ISO 8601 conform..";
  }

  // switching state (red/green mode)
  if (!ErrorHandler(NXopendata(fFileHandle, "switching_states"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'switching_states' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &ival), PNEXUS_GET_DATA_ERROR, "couldn't read 'switching_states' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'switching_states' data in NXentry!")) return NX_ERROR;
  if (fNxEntry1->SetSwitchingState(ival)) {
    fErrorCode = PNEXUS_DATA_ERROR;
    fErrorMsg = "unkown switching state found.";
  }

  // open group NXuser
  if (!ErrorHandler(NXopengroup(fFileHandle, "user", "NXuser"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup 'user'!")) return NX_ERROR;

  // change to subgroup
  if (!ErrorHandler(NXinitgroupdir(fFileHandle), PNEXUS_INIT_GROUPDIR_ERROR, "couldn't init group directory")) return NX_ERROR;
  int numItems = 0;
  NXname groupName, className;
  if (!ErrorHandler(NXgetgroupinfo(fFileHandle, &numItems, groupName, className), PNEXUS_GET_GROUP_INFO_ERROR, "couldn't get user group info")) return NX_ERROR;

  // get all the user info
  // go through the user list and filter out the required items
  NXname nx_label;
  int    nx_dataType;
  for (int i=0; i<numItems; i++) {
    if (!ErrorHandler(NXgetnextentry(fFileHandle, nx_label, className, &nx_dataType), PNEXUS_GET_NEXT_ENTRY_ERROR, "couldn't get next entry")) return NX_ERROR;
    if (!ErrorHandler(NXopendata(fFileHandle, nx_label), PNEXUS_OPEN_DATA_ERROR, "couldn't open data")) return NX_ERROR;
    if (!strcmp(nx_label, "name")) {
      if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't get user name")) return NX_ERROR;
      fNxEntry1->GetUser()->SetName(str);
    }
    if (!strcmp(nx_label, "experiment_number")) {
      if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't get user name")) return NX_ERROR;
      fNxEntry1->GetUser()->SetExperimentNumber(str);
    }
    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close data")) return NX_ERROR;
  }

  // close group NXuser
  NXclosegroup(fFileHandle);

  // open group NXsample
  if (SearchInGroup("NXsample", "class", nxname, nxclass, dataType)) {
    if (!ErrorHandler(NXopengroup(fFileHandle, "sample", "NXsample"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup 'sample'!")) return NX_ERROR;
  } else {
    cout << endl << ">> **WARNING** unable to open subgroup NXsample, will try NXSample." << endl;
    if (!ErrorHandler(NXopengroup(fFileHandle, "sample", "NXSample"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup 'sample'!")) return NX_ERROR;
  }

  // read sample name
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'name' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'name' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'name' data in sample group")) return NX_ERROR;
  fNxEntry1->GetSample()->SetName(str);

  // read sample temperature
  if (!ErrorHandler(NXopendata(fFileHandle, "temperature"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'temperature' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &fval), PNEXUS_GET_DATA_ERROR, "couldn't read 'temperature' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read temperature units!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'temperature' data in sample group")) return NX_ERROR;
  fNxEntry1->GetSample()->SetPhysProp("temperature", (double)fval, str);

  // read sample magnetic field
  if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'magnetic_field' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &fval), PNEXUS_GET_DATA_ERROR, "couldn't read 'magnetic_field' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read magnetic_field units!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'magnetic_field' data in sample group")) return NX_ERROR;
  fNxEntry1->GetSample()->SetPhysProp("magnetic_field", (double)fval, str);

  // read sample shape, e.g. powder, single crystal, etc (THIS IS AN OPTIONAL ENTRY)
  if (SearchInGroup("shape", "name", nxname, nxclass, dataType)) {
    if (!ErrorHandler(NXopendata(fFileHandle, "shape"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'shape' data in sample group!")) return NX_ERROR;
    if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'shape' data in sample group!")) return NX_ERROR;
    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'shape' data in sample group")) return NX_ERROR;
    fNxEntry1->GetSample()->SetShape(str);
  }

  // read magnetic field state, e.g. TF, LF, ZF
  if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field_state"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'magnetic_field_state' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'magnetic_field_state' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'magnetic_field_state' data in sample group")) return NX_ERROR;
  fNxEntry1->GetSample()->SetMagneticFieldState(str);

  // read 'magnetic_field_vector' and 'coordinate_system' attribute
  if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field_vector"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'magnetic_field_vector' data in sample group!")) return NX_ERROR;
  int attLen = 1, attType = NX_INT32;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "available", sizeof(cstr));
  if (!ErrorHandler(NXgetattr(fFileHandle, cstr, &ival, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'magnetic_field_vector available' data in sample group!")) return NX_ERROR;
  fNxEntry1->GetSample()->SetMagneticFieldVectorAvailable(ival);
  if (fNxEntry1->GetSample()->IsMagneticFieldVectorAvailable()) {
    vector<double> magVec;
    if (!ErrorHandler(GetDoubleVectorData(magVec), PNEXUS_GET_DATA_ERROR, "couldn't get 'magnetic_field_vector' data!")) return NX_ERROR;
    fNxEntry1->GetSample()->SetMagneticFieldVector(magVec);
    magVec.clear();
    if (!ErrorHandler(GetStringAttr("coordinate_system", str), PNEXUS_GET_ATTR_ERROR, "couldn't read magnetic field coordinate_system!")) return NX_ERROR;
    fNxEntry1->GetSample()->SetMagneticFieldVectorCoordinateSystem(str);

    // workaround since not all ISIS IDF 1 files have the 'units' entry!!
    memset(cstr, '\0', sizeof(cstr));
    strncpy(cstr, "units", sizeof(cstr));
    if (SearchAttrInData(cstr, attLen, attType)) {
      status = NXgetattr(fFileHandle, cstr, data_value, &attLen, &attType);
      if (status == NX_OK) {
        strncpy(cstr, data_value, sizeof(cstr));
        str = cstr;
      } else {
        str = string("Gauss");
      }
    } else {
      str = string("Gauss");
    }
    fNxEntry1->GetSample()->SetMagneticFieldUnits(str);
  }
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'magnetic_field_vector' data in sample group")) return NX_ERROR;

  // read sample environment, e.g. CCR, LowTemp-2, etc.
  if (!ErrorHandler(NXopendata(fFileHandle, "environment"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'environment' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'environment' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'environment' data in sample group")) return NX_ERROR;
  fNxEntry1->GetSample()->SetEnvironment(str);

  // close group NXsample
  NXclosegroup(fFileHandle);

  // get required instrument information
  // open subgroup NXinstrument with subgroups detector, collimator, beam
  if (!ErrorHandler(NXopengroup(fFileHandle, "instrument", "NXinstrument"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup instrument!")) return NX_ERROR;

  // get instrument name
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'name' data in instrument group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'name' data in instrument group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'name' data in instrument group")) return NX_ERROR;
  fNxEntry1->GetInstrument()->SetName(str);

  // open subgroup NXdetector
  if (!ErrorHandler(NXopengroup(fFileHandle, "detector", "NXdetector"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup detector!")) return NX_ERROR;

  // get number of detectors
  if (!ErrorHandler(NXopendata(fFileHandle, "number"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'number' data in detector group!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &ival), PNEXUS_GET_DATA_ERROR, "couldn't read 'number' data in detector group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'number' data in detector group")) return NX_ERROR;
  fNxEntry1->GetInstrument()->GetDetector()->SetNumber(ival);

  // close subgroup NXdetector
  NXclosegroup(fFileHandle);

  // open subgroup NXcollimator
  if (!ErrorHandler(NXopengroup(fFileHandle, "collimator", "NXcollimator"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup collimator!")) return NX_ERROR;

  // get collimator type
  if (!ErrorHandler(NXopendata(fFileHandle, "type"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'type' data in collimator group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'type' data in collimator group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'type' data in collimator group")) return NX_ERROR;
  fNxEntry1->GetInstrument()->GetCollimator()->SetType(str);

  // close subgroup NXcollimator
  NXclosegroup(fFileHandle);

  // open subgroup NXbeam
  if (!ErrorHandler(NXopengroup(fFileHandle, "beam", "NXbeam"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup beam!")) return NX_ERROR;

  // get the total counts
  if (!ErrorHandler(NXopendata(fFileHandle, "total_counts"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'total_counts' data in beam group!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &fval), PNEXUS_GET_DATA_ERROR, "couldn't read 'total_counts' data in beam group!")) return NX_ERROR;
  fNxEntry1->GetInstrument()->GetBeam()->SetTotalCounts((double)fval);
  if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read total_counts units!")) return NX_ERROR;
  fNxEntry1->GetInstrument()->GetBeam()->SetUnits(str);
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'total_counts' data in beam group")) return NX_ERROR;

  // close subgroup NXbeam
  NXclosegroup(fFileHandle);

  // close subgroup NXinstrument
  NXclosegroup(fFileHandle);

  // find the first occuring NXdata class
  found = false;
  do {
    status = NXgetnextentry(fFileHandle, nxname, nxclass, &dataType);
    if (strstr(nxclass, "NXdata") != NULL)
      found = true;
  } while (!found || (status == NX_EOD));
  // make sure any NXentry has been found
  if (!found) {
    fErrorCode = PNEXUS_GROUP_OPEN_ERROR;
    fErrorMsg  = ">> **ERROR** Couldn't find any NXdata!";
    return NX_ERROR;
  }

  // open subgroup NXdata (this is for Version 1, only and is subject to change in the near future!)
  memset(cstr, '\0', sizeof(cstr));
  snprintf(cstr, sizeof(cstr), "couldn't open NeXus subgroup %s!", nxname);
  if (!ErrorHandler(NXopengroup(fFileHandle, nxname, "NXdata"), PNEXUS_GROUP_OPEN_ERROR, cstr)) return NX_ERROR;

  // get time resolution
  if (!ErrorHandler(NXopendata(fFileHandle, "resolution"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'resolution' data in NXdata group!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &ival), PNEXUS_GET_DATA_ERROR, "couldn't read 'resolution' data in NXdata group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read 'resolution' units!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'resolution' data in NXdata group")) return NX_ERROR;
  if (!strcmp(str.c_str(), "picoseconds")) {
    fNxEntry1->GetData()->SetTimeResolution((double)ival, "ps");
  } else if (!strcmp(str.c_str(), "femtoseconds")) {
    fNxEntry1->GetData()->SetTimeResolution((double)ival, "fs");
  }

  // get data, t0, first good bin, last good bin, data
  if (!ErrorHandler(NXopendata(fFileHandle, "counts"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'counts' data in NXdata group!")) return NX_ERROR;
  int histoLength=0;
  int noOfHistos=0;

  // get number of histos
  attLen = 1;
  attType = NX_INT32;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "number", sizeof(cstr));
  if (!ErrorHandler(NXgetattr(fFileHandle, cstr, &noOfHistos, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'number' data in NXdata group!")) return NX_ERROR;

  // get histo length
  attLen = 1;
  attType = NX_INT32;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "length", sizeof(cstr));
  if (!ErrorHandler(NXgetattr(fFileHandle, cstr, &histoLength, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'length' data in NXdata group!")) return NX_ERROR;

  // get t0
  attLen = 1;
  attType = NX_INT32;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "T0_bin", sizeof(cstr));
  if (SearchAttrInData(cstr, attLen, attType)) {
    if (!ErrorHandler(NXgetattr(fFileHandle, cstr, &ival, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'T0_bin' data in NXdata group!")) return NX_ERROR;
  } else {
    cout << endl << ">> **WARNING** didn't find attribute 'T0_bin' in NXdata/counts, will try 't0_bin'." << endl;
    memset(cstr, '\0', sizeof(cstr));
    strncpy(cstr, "t0_bin", sizeof(cstr));
    if (!ErrorHandler(NXgetattr(fFileHandle, cstr, &ival, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 't0_bin' data in NXdata group!")) return NX_ERROR;
  }
  fNxEntry1->GetData()->SetT0(ival);

  // get first good bin
  attLen = 1;
  attType = NX_INT32;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "first_good_bin", sizeof(cstr));
  if (!ErrorHandler(NXgetattr(fFileHandle, cstr, &ival, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'first_good_bin' data in NXdata group!")) return NX_ERROR;
  fNxEntry1->GetData()->SetFirstGoodBin(ival);

  // get last good bin
  attLen = 1;
  attType = NX_INT32;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "last_good_bin", sizeof(cstr));
  if (!ErrorHandler(NXgetattr(fFileHandle, cstr, &ival, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'last_good_bin' data in NXdata group!")) return NX_ERROR;
  fNxEntry1->GetData()->SetLastGoodBin(ival);

  // get data

  // get information of the current nexus entity
  int rank, type, dims[32], size, noOfElements;
  if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get data info!")) return NX_ERROR;

  // calculate the needed size
  size = dims[0];
  for (int i=1; i<rank; i++)
    size *= dims[i];
  noOfElements = size;
  size *= GetDataSize(type);

  // allocate locale memory to get the data
  char *data_ptr = new char[size];
  if (data_ptr == 0) {
    return NX_ERROR;
  }

  // get the data
  int *i_data_ptr = (int*) data_ptr;
  status = NXgetdata(fFileHandle, i_data_ptr);
  if (status != NX_OK) {
    return NX_ERROR;
  }

  // check that the amount of data is consistent with the attribute information
  if (noOfElements != noOfHistos * histoLength) {
    fErrorCode = PNEXUS_HISTO_ERROR;
    fErrorMsg = "inconsistent histogram info!";
    return NX_ERROR;
  }

  // copy the data into the vector
  fNxEntry1->GetData()->FlushHistos();
  vector<unsigned int> data;
  for (int i=0; i<noOfElements; i++) {
    if ((i % histoLength == 0) && (i>0)) {
      fNxEntry1->GetData()->SetHisto(data);
      data.clear();
      data.push_back(*(i_data_ptr+i));
    } else {
      data.push_back(*(i_data_ptr+i));
    }
  }
  fNxEntry1->GetData()->SetHisto(data);
  data.clear();

  // clean up
  if (data_ptr) {
    delete [] data_ptr;
  }

  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'counts' data in NXdata group")) return NX_ERROR;

  // get grouping
  if (!ErrorHandler(NXopendata(fFileHandle, "grouping"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'grouping' data in NXdata group!")) return NX_ERROR;
  attLen = 1;
  attType = NX_INT32;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "available", sizeof(cstr));
  if (!ErrorHandler(NXgetattr(fFileHandle, cstr, &ival, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'grouping available' data in NXdata group!")) return NX_ERROR;
  if (ival) {
    vector<int> grouping;
    if (!ErrorHandler(GetIntVectorData(grouping), PNEXUS_GET_DATA_ERROR, "couldn't read 'grouping' data in NXdata group!")) return NX_ERROR;
    fNxEntry1->GetData()->SetGrouping(grouping);
    grouping.clear();
  }
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'grouping' data in NXdata group")) return NX_ERROR;

  // if grouping has been available check for consistency
  bool ok=true;
  if (ival) { // i.e. grouping is available (see a few lines further up)
    // check grouping vector for consistency
    if (fNxEntry1->GetData()->GetGrouping()->size() != fNxEntry1->GetData()->GetNoOfHistos()) {
      fNxEntry1->GetData()->FlushGrouping();
      cerr << endl << ">> **WARNING** grouping vector size (" << fNxEntry1->GetData()->GetGrouping()->size()<< ") != no of histograms (" << fNxEntry1->GetData()->GetNoOfHistos() << ") which doesn't make sence, hence grouping will be ignored." << endl;
    }
    // check that the grouping values do make sense, i.e. allowed range is grouping value > 0 and grouping value <= # of histos
    for (unsigned int i=0; i<fNxEntry1->GetData()->GetGrouping()->size(); i++) {
      if ((fNxEntry1->GetData()->GetGrouping()->at(i) == 0) || (fNxEntry1->GetData()->GetGrouping()->at(i) > (int)fNxEntry1->GetData()->GetNoOfHistos())) {
        cerr << endl << ">> **WARNING** found grouping entry '" << fNxEntry1->GetData()->GetGrouping()->at(i) << "' which doesn't make sense, hence grouping will be ignored." << endl;
        ok = false;
        break;
      }
    }
    if (!ok)
      fNxEntry1->GetData()->FlushGrouping();
  }

  // get alpha
  if (!ErrorHandler(NXopendata(fFileHandle, "alpha"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'alpha' data in NXdata group!")) return NX_ERROR;
  attLen = 1;
  attType = NX_INT32;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "available", sizeof(cstr));
  if (!ErrorHandler(NXgetattr(fFileHandle, cstr, &ival, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'alpha available' data in NXdata group!")) return NX_ERROR;
  if (ival) {
    if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get alpha info!")) return NX_ERROR;
    // calculate the needed size
    size = dims[0];
    for (int i=1; i<rank; i++)
      size *= dims[i];
    noOfElements = size;
    size *= GetDataSize(type);

    // check that noOfElements is a multiple of 3: grp_1, grp_2, alpha_12, etc.
    if ((3*(noOfElements/3)-noOfElements) != 0) {
      fErrorCode = PNEXUS_GET_META_INFO_ERROR;
      fErrorMsg  = "alpha NeXus structure (grp_1, grp_2, alpha_12, etc.) violation.";
      cerr << endl << ">> **ERROR** " << fErrorMsg << endl;
      return NX_ERROR;
    }

    // allocate locale memory to get the data
    char *data_ptr = new char[size];
    if (data_ptr == 0) {
      return NX_ERROR;
    }

    // get the data
    float *f_data_ptr = (float*) data_ptr;
    status = NXgetdata(fFileHandle, f_data_ptr);
    if (status != NX_OK) {
      return NX_ERROR;
    }

    // copy the data into the vector
    fNxEntry1->GetData()->FlushAlpha();
    PNeXusAlpha1 alpha;
    vector<PNeXusAlpha1> alphaVec;
    for (int i=0; i<noOfElements; i+=3) {
      fval = *(f_data_ptr+i);
      alpha.SetGroupFirst((unsigned int) fval);
      fval = *(f_data_ptr+i+1);
      alpha.SetGroupSecond((unsigned int) fval);
      alpha.SetAlpha(*(f_data_ptr+i+2));
      alphaVec.push_back(alpha);
    }
    fNxEntry1->GetData()->SetAlpha(alphaVec);

    // clean up
    alphaVec.clear();
    if (data_ptr) {
      delete [] data_ptr;
    }
  }
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'alpha' data in NXdata group")) return NX_ERROR;

  // close subgroup NXdata
  NXclosegroup(fFileHandle);

  // close group NXentry
  NXclosegroup(fFileHandle);

  // close file
  NXclose(&fFileHandle);

  GroupHistoData();

  fValid = true;

  return NX_OK;
}

//------------------------------------------------------------------------------------------
// ReadFileIdf2 (private)
//------------------------------------------------------------------------------------------
/**
 * <p>Read the NeXus file of type IDF Version 2.
 *
 * <b>return:</b>
 * - NX_OK on successfull reading
 * - NX_ERROR on error. The error code/message will give the details.
 */
int PNeXus::ReadFileIdf2()
{
  cout << endl << ">> reading NeXus IDF Version 2 file ..." << endl;

  // create first the necessary NXentry object for IDF Version 1
  fNxEntry2 = new PNeXusEntry2();
  if (fNxEntry2 == 0) {
    fErrorCode = PNEXUS_OBJECT_INVOK_ERROR;
    fErrorMsg = ">> **ERROR** couldn't invoke PNeXusEntry2 object.";
    return NX_ERROR;
  }

  string str;
  int ival, attlen, atttype;
  float fval;
  NXname data_value, nxAttrName;
  int rank, type, dims[32], size, noOfElements;

  // open file
  NXstatus status;
  status = NXopen(fFileName.c_str(), NXACC_READ, &fFileHandle);
  if (status != NX_OK) {
    fErrorCode = PNEXUS_FILE_OPEN_ERROR;
    fErrorMsg  = "PNeXus::ReadFileIdf2() **ERROR** Couldn't open file '"+fFileName+"' !";
    return NX_ERROR;
  }

  // collect the NXroot attribute information
  do {
    status = NXgetnextattr(fFileHandle, nxAttrName, &attlen, &atttype);
    if (status == NX_OK) {
      if (!strcmp(nxAttrName, "HDF_version")) {
        attlen = VGNAMELENMAX - 1;
        atttype = NX_CHAR;
        status = NXgetattr(fFileHandle, nxAttrName, data_value, &attlen, &atttype);
        if (status == NX_OK) {
          fFileFormatVersion = string(data_value);
        }
      } else if (!strcmp(nxAttrName, "HDF5_Version")) {
        attlen = VGNAMELENMAX - 1;
        atttype = NX_CHAR;
        status = NXgetattr(fFileHandle, nxAttrName, data_value, &attlen, &atttype);
        if (status == NX_OK) {
          fFileFormatVersion = string("HDF5: ")+string(data_value);
        }
      } else if (!strcmp(nxAttrName, "XML_version")) {
        attlen = VGNAMELENMAX - 1;
        atttype = NX_CHAR;
        status = NXgetattr(fFileHandle, nxAttrName, data_value, &attlen, &atttype);
        if (status == NX_OK) {
          fFileFormatVersion = string(data_value);
        }
      } else if (!strcmp(nxAttrName, "file_name")) {
        if (!ErrorHandler(GetStringAttr("file_name", str), PNEXUS_GET_ATTR_ERROR, "couldn't read NXroot 'file_name' attribute!")) return NX_ERROR;
        fFileName = str;
      } else if (!strcmp(nxAttrName, "file_time")) {
        if (!ErrorHandler(GetStringAttr("file_time", str), PNEXUS_GET_ATTR_ERROR, "couldn't read NXroot 'file_time' attribute!")) return NX_ERROR;
        fFileTime = str;
      } else if (!strcmp(nxAttrName, "creator")) {
        if (!ErrorHandler(GetStringAttr("creator", str), PNEXUS_GET_ATTR_ERROR, "couldn't read NXroot 'creator' attribute!")) return NX_ERROR;
        fCreator = str;
      }
    }
  } while (status == NX_OK);

  // look for the first occurring NXentry which name ends on "_1"
  NXname nxname, nxclass;
  int dataType;
  bool found = false;
  size_t pos;
  do {
    status = NXgetnextentry(fFileHandle, nxname, nxclass, &dataType);
    if (!strcmp(nxclass, "NXentry")) {
      str = nxname;
      pos = str.find_last_of("_1");
      if (pos != str.npos)
        found = true;
    }
  } while (!found && (status == NX_OK));
  if (!found) {
    fErrorCode = PNEXUS_NXENTRY_NOT_FOUND;
    fErrorMsg  = ">> **ERROR** Couldn't find any NXentry on NXroot level!";
    return NX_ERROR;
  }

  // open the NXentry group to obtain the necessary stuff
  status = NXopengroup(fFileHandle, nxname, "NXentry");
  if (status != NX_OK) {
    fErrorCode = PNEXUS_GROUP_OPEN_ERROR;
    fErrorMsg  = "PNeXus::ReadFileIdf2() **ERROR** Couldn't open the NeXus group '" + string(nxname) + "'!";
    return NX_ERROR;
  }

  // definition
  if (!ErrorHandler(NXopendata(fFileHandle, "definition"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'definition' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'definition' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'definition' data in NXentry!")) return NX_ERROR;
  fNxEntry2->SetDefinition(str);

  // program_name and version
  if (SearchInGroup("program_name", "name", nxname, nxclass, dataType)) {
    if (!ErrorHandler(NXopendata(fFileHandle, "program_name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'program_name' data in NXentry!")) return NX_ERROR;
    if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'program_name' data in NXentry!")) return NX_ERROR;
    fNxEntry2->SetProgramName(str);
    if (!ErrorHandler(GetStringAttr("version", str), PNEXUS_GET_ATTR_ERROR, "couldn't read 'program_name' attribute in NXentry!")) return NX_ERROR;
    fNxEntry2->SetProgramVersion(str);
    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'program_name' data in NXentry!")) return NX_ERROR;
  }

  // run number
  if (!ErrorHandler(NXopendata(fFileHandle, "run_number"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'run_number' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &ival), PNEXUS_GET_DATA_ERROR, "couldn't read 'run_number' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'run_number' data in NXentry!")) return NX_ERROR;
  fNxEntry2->SetRunNumber(ival);

  // title
  if (!ErrorHandler(NXopendata(fFileHandle, "title"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'title' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'title' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'title' data in NXentry!")) return NX_ERROR;
  fNxEntry2->SetTitle(str);

  // start time
  if (!ErrorHandler(NXopendata(fFileHandle, "start_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'start_time' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'start_time' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'start_time' data in NXentry!")) return NX_ERROR;
  if (fNxEntry2->SetStartTime(str) != NX_OK) {
    fErrorCode = PNEXUS_TIME_FORMAT_ERROR;
    fErrorMsg = "start time format is not ISO 8601 conform.";
  }

  // end time
  if (!ErrorHandler(NXopendata(fFileHandle, "end_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'end_time' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'end_time' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'end_time' data in NXentry!")) return NX_ERROR;
  if (fNxEntry2->SetStopTime(str) != NX_OK) {
    fErrorCode = PNEXUS_TIME_FORMAT_ERROR;
    fErrorMsg = "stop time format is not ISO 8601 conform..";
  }

  // experiment identifier
  if (!ErrorHandler(NXopendata(fFileHandle, "experiment_identifier"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'experiment_identifier' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'experiment_identifier' data in NXentry!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'experiment_identifier' data in NXentry!")) return NX_ERROR;
  fNxEntry2->SetExperimentIdentifier(str);

  // find entry for NXuser
  if (SearchInGroup("NXuser", "class", nxname, nxclass, dataType)) {
    // open the NXuser
    status = NXopengroup(fFileHandle, nxname, "NXuser");
    if (status != NX_OK) {
      fErrorCode = PNEXUS_GROUP_OPEN_ERROR;
      fErrorMsg  = "PNeXus::ReadFileIdf2() **ERROR** Couldn't open NXuser '" + string(nxname) + "' in NXentry!";
      return NX_ERROR;
    }

    // user name
    if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'name' data in NXuser!")) return NX_ERROR;
    if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'name' data in NXuser!")) return NX_ERROR;
    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'name' data in NXuser!")) return NX_ERROR;
    fNxEntry2->GetUser()->SetName(str);

    // close group NXuser
    NXclosegroup(fFileHandle);
  }

  // find entry for NXsample
  if (SearchInGroup("NXsample", "class", nxname, nxclass, dataType)) {
    // open group NXsample
    if (!ErrorHandler(NXopengroup(fFileHandle, "sample", "NXsample"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup sample!")) return NX_ERROR;

    // sample name
    if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'name' data in NXsample!")) return NX_ERROR;
    if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'name' data in NXsample!")) return NX_ERROR;
    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'name' data in NXsample!")) return NX_ERROR;
    fNxEntry2->GetSample()->SetName(str);

    // sample description
    if (SearchInGroup("description", "name", nxname, nxclass, dataType)) {
      if (!ErrorHandler(NXopendata(fFileHandle, "description"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'description' data in NXsample!")) return NX_ERROR;
      if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'description' data in NXsample!")) return NX_ERROR;
      if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'description' data in NXsample!")) return NX_ERROR;
      fNxEntry2->GetSample()->SetDescription(str);
    }

    // temperature
    if (SearchInGroup("temperature_1", "name", nxname, nxclass, dataType)) {
      if (!ErrorHandler(NXopendata(fFileHandle, "temperature_1"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'temperature_1' data in NXsample!")) return NX_ERROR;
      if (!ErrorHandler(NXgetdata(fFileHandle, &fval), PNEXUS_GET_DATA_ERROR, "couldn't read 'temperature_1' data in NXsample!")) return NX_ERROR;
      if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read temperature units in NXsample!")) return NX_ERROR;
      if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'temperature_1' data in NXsample!")) return NX_ERROR;
      fNxEntry2->GetSample()->SetPhysProp("temperature_1", fval, str);
    }

    // temperature environment
    if (SearchInGroup("temperature_1_env", "name", nxname, nxclass, dataType)) {
      if (!ErrorHandler(NXopengroup(fFileHandle, "temperature_1_env", "NXenvironment"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup NXenvironment!")) return NX_ERROR;
      // temperature environment name
      if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'name' data in NXenvironment!")) return NX_ERROR;
      if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'name' data in NXenvironment!")) return NX_ERROR;
      if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'name' data in NXenvironment!")) return NX_ERROR;
      fNxEntry2->GetSample()->SetEnvironmentTemp(str);
      // close group NXenvironment
      NXclosegroup(fFileHandle);
    }

    // magnetic_field
    if (SearchInGroup("magnetic_field_1", "name", nxname, nxclass, dataType)) {
      if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field_1"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'magnetic_field_1' data in NXsample!")) return NX_ERROR;
      if (!ErrorHandler(NXgetdata(fFileHandle, &fval), PNEXUS_GET_DATA_ERROR, "couldn't read 'magnetic_field_1' data in NXsample!")) return NX_ERROR;
      if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read magnetic field units in NXsample!")) return NX_ERROR;
      if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'magnetic_field_1' data in NXsample!")) return NX_ERROR;
      fNxEntry2->GetSample()->SetPhysProp("magnetic_field_1", fval, str);
    }

    // magnetic field state
    if (SearchInGroup("magnetic_field_state", "name", nxname, nxclass, dataType)) {
      if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field_state"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'magnetic_field_state' data in NXsample!")) return NX_ERROR;
      if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'magnetic_field_state' data in NXsample!")) return NX_ERROR;
      if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'magnetic_field_state' data in NXsample!")) return NX_ERROR;
      fNxEntry2->GetSample()->SetMagneticFieldState(str);
    }

    // magnetic field environment
    if (SearchInGroup("magnetic_field_1_env", "name", nxname, nxclass, dataType)) {
      if (!ErrorHandler(NXopengroup(fFileHandle, "magnetic_field_1_env", "NXenvironment"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup NXenvironment!")) return NX_ERROR;
      // magnetic field environment name
      if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'name' data in NXenvironment!")) return NX_ERROR;
      if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'name' data in NXenvironment!")) return NX_ERROR;
      if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'name' data in NXenvironment!")) return NX_ERROR;
      fNxEntry2->GetSample()->SetEnvironmentField(str);
      // close group NXenvironment
      NXclosegroup(fFileHandle);
    }

    // close group NXsample
    NXclosegroup(fFileHandle);
  }

  // open group NXinstrument
  if (!ErrorHandler(NXopengroup(fFileHandle, "instrument", "NXinstrument"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup instrument in NXentry!")) return NX_ERROR;

  // name
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'name' data in NXinstrument!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'name' data in NXinstrument!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'name' data in NXinstrument!")) return NX_ERROR;
  fNxEntry2->GetInstrument()->SetName(str);

  // open group NXsource
  if (!ErrorHandler(NXopengroup(fFileHandle, "source", "NXsource"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup instrument in NXinstrument!")) return NX_ERROR;

  // name
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'name' data in NXsource!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'name' data in NXsource!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'name' data in NXsource!")) return NX_ERROR;
  fNxEntry2->GetInstrument()->GetSource()->SetName(str);

  // type
  if (!ErrorHandler(NXopendata(fFileHandle, "type"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'type' data in NXsource!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'type' data in NXsource!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'type' data in NXsource!")) return NX_ERROR;
  fNxEntry2->GetInstrument()->GetSource()->SetType(str);

  // probe
  if (!ErrorHandler(NXopendata(fFileHandle, "probe"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'probe' data in NXsource!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'probe' data in NXsource!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'probe' data in NXsource!")) return NX_ERROR;
  fNxEntry2->GetInstrument()->GetSource()->SetProbe(str);

  // close group NXsource
  NXclosegroup(fFileHandle);

  // open group NXbeamline
  if (!ErrorHandler(NXopengroup(fFileHandle, "beamline", "NXbeamline"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup beamline in NXinstrument!")) return NX_ERROR;

  // beamline name
  if (!ErrorHandler(NXopendata(fFileHandle, "beamline"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'beamline' data in NXbeamline!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'beamline' data in NXbeamline!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'beamline' data in NXbeamline!")) return NX_ERROR;
  fNxEntry2->GetInstrument()->GetBeamline()->SetName(str);

  // close group NXbeamline
  NXclosegroup(fFileHandle);

  // open group NXdetector
  if (!ErrorHandler(NXopengroup(fFileHandle, "detector_1", "NXdetector"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup detector_1 in NXinstrument!")) return NX_ERROR;

  // description
  if (!ErrorHandler(NXopendata(fFileHandle, "description"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'description' data in NXdetector!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'description' data in NXdetector!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'description' data in NXdetector!")) return NX_ERROR;
  fNxEntry2->GetInstrument()->GetDetector()->SetDescription(str);

  // get the time resolution. This is a little bit complicated since it is either present as 'histogram_resolution' or needs to be extracted from the 'raw_time' vector
  // 1st check if 'histogram_resolution' is found
  if (SearchInGroup("histogram_resolution", "name", nxname, nxclass, dataType)) {
    if (!ErrorHandler(NXopendata(fFileHandle, "histogram_resolution"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'histogram_resolution' data in NXdetector!")) return NX_ERROR;
    if (!ErrorHandler(NXgetdata(fFileHandle, &fval), PNEXUS_GET_DATA_ERROR, "couldn't read 'histogram_resolution' data in NXdetector!")) return NX_ERROR;
    if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read time resolution units in NXdetector!")) return NX_ERROR;
    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'histogram_resolution' data in NXdetector!")) return NX_ERROR;
    fNxEntry2->GetInstrument()->GetDetector()->SetTimeResolution((double)fval, str);
  } else if (SearchInGroup("resolution", "name", nxname, nxclass, dataType)) { // 2nd check if 'resolution' is found
    if (!ErrorHandler(NXopendata(fFileHandle, "resolution"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'resolution' data in NXdetector!")) return NX_ERROR;
    if (!ErrorHandler(NXgetdata(fFileHandle, &fval), PNEXUS_GET_DATA_ERROR, "couldn't read 'resolution' data in NXdetector!")) return NX_ERROR;
    if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read time resolution units in NXdetector!")) return NX_ERROR;
    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'resolution' data in NXdetector!")) return NX_ERROR;
    fNxEntry2->GetInstrument()->GetDetector()->SetTimeResolution((double)fval, str);
  } else { // 3nd 'histogram_resolution' is not present, hence extract the time resolution from the 'raw_time' vector
    if (!ErrorHandler(NXopendata(fFileHandle, "raw_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'raw_time' data in NXdetector!")) return NX_ERROR;
    vector<double> rawTime;
    if (!ErrorHandler(GetDoubleVectorData(rawTime), PNEXUS_GET_DATA_ERROR, "couldn't get 'raw_time' data in NXdetector!")) return NX_ERROR;
    if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read 'raw_time' units in NXdetector!")) return NX_ERROR;
    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'raw_time' data in NXdetector!")) return NX_ERROR;
    fNxEntry2->GetInstrument()->GetDetector()->SetTimeResolution(rawTime[1]-rawTime[0], str);
    rawTime.clear();
  }

  // get counts
  if (!ErrorHandler(NXopendata(fFileHandle, "counts"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'counts' data in NXdetector!")) return NX_ERROR;
  // check the dimensions of 'counts'
  if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get 'counts' info in NXdetector!")) return NX_ERROR;

  // calculate the needed size
  size = dims[0];
  for (int i=1; i<rank; i++)
    size *= dims[i];
  noOfElements = size;
  size *= GetDataSize(type);

  // allocate locale memory to get the data
  char *data_ptr = new char[size];
  if (data_ptr == 0) {
    return NX_ERROR;
  }

  // get the data
  int *i_data_ptr = (int*) data_ptr;
  status = NXgetdata(fFileHandle, i_data_ptr);
  if (status != NX_OK) {
    return NX_ERROR;
  }

  if (rank == 3) { // i.e. np, ns, ntc
    fNxEntry2->GetInstrument()->GetDetector()->SetNoOfPeriods(dims[0]);
    fNxEntry2->GetInstrument()->GetDetector()->SetNoOfSpectra(dims[1]);
    fNxEntry2->GetInstrument()->GetDetector()->SetNoOfBins(dims[2]);
  } else if (rank == 2) { // i.e. ns, ntc
    fNxEntry2->GetInstrument()->GetDetector()->SetNoOfPeriods(-1);
    fNxEntry2->GetInstrument()->GetDetector()->SetNoOfSpectra(dims[0]);
    fNxEntry2->GetInstrument()->GetDetector()->SetNoOfBins(dims[1]);
  } else if (rank == 1) { // i.e. ntc only
    fNxEntry2->GetInstrument()->GetDetector()->SetNoOfPeriods(-1);
    fNxEntry2->GetInstrument()->GetDetector()->SetNoOfSpectra(1);
    fNxEntry2->GetInstrument()->GetDetector()->SetNoOfBins(dims[0]);
  } else {
    fNxEntry2->GetInstrument()->GetDetector()->SetNoOfPeriods(-1);
    fNxEntry2->GetInstrument()->GetDetector()->SetNoOfSpectra(-1);
    fNxEntry2->GetInstrument()->GetDetector()->SetNoOfBins(-1);
    cerr << endl << ">> **ERROR** found rank=" << rank << " for NXinstrument:NXdetector:counts! Allowed ranks are 1, 2, or 3." << endl;
    return NX_ERROR;
  }

  if (!fNxEntry2->GetInstrument()->GetDetector()->SetHistos(i_data_ptr)) {
    cerr << endl << ">> **ERROR** " << fNxEntry2->GetInstrument()->GetDetector()->GetErrorMsg() << endl;
    return NX_ERROR;
  }

  // clean up
  if (data_ptr) {
    delete [] data_ptr;
    data_ptr = 0;
  }

  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'counts' data in NXdetector!")) return NX_ERROR;

  // handle spectrum_index
  if (!ErrorHandler(NXopendata(fFileHandle, "spectrum_index"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'spectrum_index' data in NXdetector!")) return NX_ERROR;
  // check the dimensions of 'spectrum_index'
  if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get 'spectrum_index' info in NXdetector!")) return NX_ERROR;
  if (rank > 1) {
    cerr << endl << ">> **ERROR** found rank=" << rank << " of spectrum_index in NXdetector. Rank needs to be == 1!" << endl;
    return NX_ERROR;
  }
  if (dims[0] != fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra()) {
    cerr << endl << ">> **ERROR** found spectrum_index dimension=" << dims[0] << " inconsistent with counts (" << fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra() << "!" << endl;
    return NX_ERROR;
  }
  // allocate locale memory to get the data
  data_ptr = new char[dims[0]*GetDataSize(type)];
  if (data_ptr == 0) {
    return NX_ERROR;
  }
  // get the data
  i_data_ptr = (int*) data_ptr;
  status = NXgetdata(fFileHandle, i_data_ptr);
  if (status != NX_OK) {
    return NX_ERROR;
  }
  for (int i=0; i<dims[0]; i++)
    fNxEntry2->GetInstrument()->GetDetector()->SetSpectrumIndex(*(i_data_ptr+i), i);

  // clean up
  if (data_ptr) {
    delete [] data_ptr;
    data_ptr = 0;
  }

  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'spectrum_index' data in NXdetector!")) return NX_ERROR;

  // handle t0's
  if (SearchInGroup("time_zero_bin", "name", nxname, nxclass, dataType)) { // check for 'time_zero_bin'
    if (!ErrorHandler(NXopendata(fFileHandle, "time_zero_bin"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'time_zero_bin' data in NXdetector!")) return NX_ERROR;

    // check the dimensions of the 'time_zero_bin' vector
    if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get 'time_zero_bin' info in NXdetector!")) return NX_ERROR;

    if ((rank == 1) && (dims[0] == 1)) { // single t0 entry
      fNxEntry2->GetInstrument()->GetDetector()->SetT0Tag(1);
    } else if ((rank == 1) && (dims[0] > 1)) { // t0 of the form t0[ns]
      fNxEntry2->GetInstrument()->GetDetector()->SetT0Tag(2);
    } else if (rank == 2) { // t0 of the form t0[np][ns]
      fNxEntry2->GetInstrument()->GetDetector()->SetT0Tag(3);
    } else {
      cerr << endl << ">> **ERROR** found 'time_zero_bin' info in NXdetector with rank=" << rank << ". Do not know how to handle." << endl;
      return NX_ERROR;
    }

    // calculate the needed size
    size = dims[0];
    for (int i=1; i<rank; i++)
      size *= dims[i];
    noOfElements = size;
    size *= GetDataSize(type);

    // allocate locale memory to get the data
    char *data_ptr = new char[size];
    if (data_ptr == 0) {
      return NX_ERROR;
    }

    // get the data
    int *i_data_ptr = (int*) data_ptr;
    int status = NXgetdata(fFileHandle, i_data_ptr);
    if (status != NX_OK) {
      return NX_ERROR;
    }

    if (!fNxEntry2->GetInstrument()->GetDetector()->SetT0(i_data_ptr)) {
      cerr << endl << ">> **ERROR** " << fNxEntry2->GetInstrument()->GetDetector()->GetErrorMsg() << endl;
      return NX_ERROR;
    }

    // clean up
    if (data_ptr) {
      delete [] data_ptr;
      data_ptr = 0;
    }

    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'time_zero_bin' data in NXdetector!")) return NX_ERROR;
  } else if (SearchInGroup("time_zero", "name", nxname, nxclass, dataType)) { // check for 'time_zero'
    if (!ErrorHandler(NXopendata(fFileHandle, "time_zero"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'time_zero' data in NXdetector!")) return NX_ERROR;
    // check the dimensions of the 'time_zero' vector
    if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get 'time_zero' info in NXdetector!")) return NX_ERROR;

    if ((rank == 1) && (dims[0] == 1)) { // single t0 entry
      fNxEntry2->GetInstrument()->GetDetector()->SetT0Tag(1);
    } else if ((rank == 1) && (dims[0] > 1)) { // t0 of the form t0[ns]
      fNxEntry2->GetInstrument()->GetDetector()->SetT0Tag(2);
    } else if (rank == 2) { // t0 of the form t0[np][ns]
      fNxEntry2->GetInstrument()->GetDetector()->SetT0Tag(3);
    } else {
      cerr << endl << ">> **ERROR** found 'time_zero' info in NXdetector with rank=" << rank << ". Do not know how to handle." << endl;
      return NX_ERROR;
    }

    // calculate the needed size
    size = dims[0];
    for (int i=1; i<rank; i++)
      size *= dims[i];
    noOfElements = size;
    size *= GetDataSize(type);

    // allocate locale memory to get the data
    char *data_ptr = new char[size];
    if (data_ptr == 0) {
      return NX_ERROR;
    }

    // get the data
    float *f_data_ptr = (float*) data_ptr;
    int status = NXgetdata(fFileHandle, f_data_ptr);
    if (status != NX_OK) {
      return NX_ERROR;
    }

    if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read 'time_zero' units in NXdetector!")) return NX_ERROR;

    // Set T0's
    // check that the necessary time resolution is present
    if (fNxEntry2->GetInstrument()->GetDetector()->GetTimeResolution(str) == 0.0) {
      cerr << endl << ">> **ERROR** trying to set T0's based on 'time_zero'. Need a valid time resolution to do so, but this is not given." << endl;
      return NX_ERROR;
    }
    // set the t0's based on the t0 time stamp and the time resolution
    int *pt0 = 0;
    if (rank == 1) {
      pt0 = new int;
      *pt0 = (int)(*f_data_ptr / (float)fNxEntry2->GetInstrument()->GetDetector()->GetTimeResolution(str));
    } else { // rank == 2
      pt0 = new int[noOfElements];
      for (int i=0; i<noOfElements; i++) {
        *(pt0+i) = (int)(*(f_data_ptr+i) / (float)fNxEntry2->GetInstrument()->GetDetector()->GetTimeResolution(str));
      }
    }

    if (!fNxEntry2->GetInstrument()->GetDetector()->SetT0(pt0)) {
      cerr << endl << ">> **ERROR** " << fNxEntry2->GetInstrument()->GetDetector()->GetErrorMsg() << endl;
      return NX_ERROR;
    }

    // clean up
    if (data_ptr) {
      delete [] data_ptr;
      data_ptr = 0;
    }

    cerr << endl << ">> **WARNING** found only 'time_zero' will convert it to 'time_zero_bin' values" << endl;
    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'time_zero' data in NXdetector!")) return NX_ERROR;
  } else {
    cerr << endl << ">> **WARNING** found neither 'time_zero_bin' nor 'time_zero' values ..." << endl;
  }

  // handle first good bin
  if (SearchInGroup("first_good_bin", "name", nxname, nxclass, dataType)) {
    if (!ErrorHandler(NXopendata(fFileHandle, "first_good_bin"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'first_good_bin' data in NXdetector!")) return NX_ERROR;

    // check the dimensions of the 'first_good_bin' vector
    if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get 'first_good_bin' info in NXdetector!")) return NX_ERROR;

    // calculate the needed size
    size = dims[0];
    for (int i=1; i<rank; i++)
      size *= dims[i];
    noOfElements = size;
    size *= GetDataSize(type);

    // allocate locale memory to get the data
    char *data_ptr = new char[size];
    if (data_ptr == 0) {
      return NX_ERROR;
    }

    // get the data
    int *i_data_ptr = (int*) data_ptr;
    int status = NXgetdata(fFileHandle, i_data_ptr);
    if (status != NX_OK) {
      return NX_ERROR;
    }

    if (!fNxEntry2->GetInstrument()->GetDetector()->SetFirstGoodBin(i_data_ptr)) {
      cerr << endl << ">> **ERROR** " << fNxEntry2->GetInstrument()->GetDetector()->GetErrorMsg() << endl;
      return NX_ERROR;
    }

    if (data_ptr) {
      delete [] data_ptr;
      data_ptr = 0;
    }

    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'first_good_bin' data in NXdetector!")) return NX_ERROR;
  } else if (SearchInGroup("first_good_time", "name", nxname, nxclass, dataType)) {
    if (!ErrorHandler(NXopendata(fFileHandle, "first_good_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'first_good_time' data in NXdetector!")) return NX_ERROR;

    // check the dimensions of the 'first_good_time' vector
    if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get 'first_good_time' info in NXdetector!")) return NX_ERROR;

    // calculate the needed size
    size = dims[0];
    for (int i=1; i<rank; i++)
      size *= dims[i];
    noOfElements = size;
    size *= GetDataSize(type);

    // allocate locale memory to get the data
    char *data_ptr = new char[size];
    if (data_ptr == 0) {
      return NX_ERROR;
    }

    // get the data
    float *f_data_ptr = (float*) data_ptr;
    int status = NXgetdata(fFileHandle, f_data_ptr);
    if (status != NX_OK) {
      return NX_ERROR;
    }

    // set the fgb's based on the fgb time stamp and the time resolution
    int *p_fgb = 0;
    if (rank == 1) {
      p_fgb = new int;
      *p_fgb = (int)(*f_data_ptr / (float)fNxEntry2->GetInstrument()->GetDetector()->GetTimeResolution(str));
    } else { // rank == 2
      p_fgb = new int[noOfElements];
      for (int i=0; i<noOfElements; i++) {
        *(p_fgb+i) = (int)(*(f_data_ptr+i) / (float)fNxEntry2->GetInstrument()->GetDetector()->GetTimeResolution(str));
      }
    }

    if (!fNxEntry2->GetInstrument()->GetDetector()->SetFirstGoodBin(p_fgb)) {
      cerr << endl << ">> **ERROR** " << fNxEntry2->GetInstrument()->GetDetector()->GetErrorMsg() << endl;
      return NX_ERROR;
    }

    // clean up
    if (p_fgb) {
      delete [] p_fgb;
      p_fgb = 0;
    }
    if (data_ptr) {
      delete [] data_ptr;
      data_ptr = 0;
    }

    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'first_good_time' data in NXdetector!")) return NX_ERROR;
  } else {
    cerr << endl << ">> **WARNING** found neither 'first_good_bin' nor 'first_good_time' values ..." << endl;
  }

  // handle last good bin
  if (SearchInGroup("last_good_bin", "name", nxname, nxclass, dataType)) {
    if (!ErrorHandler(NXopendata(fFileHandle, "last_good_bin"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'last_good_bin' data in NXdetector!")) return NX_ERROR;

    // check the dimensions of the 'last_good_bin' vector
    if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get 'last_good_bin' info in NXdetector!")) return NX_ERROR;

    // calculate the needed size
    size = dims[0];
    for (int i=1; i<rank; i++)
      size *= dims[i];
    noOfElements = size;
    size *= GetDataSize(type);

    // allocate locale memory to get the data
    char *data_ptr = new char[size];
    if (data_ptr == 0) {
      return NX_ERROR;
    }

    // get the data
    int *i_data_ptr = (int*) data_ptr;
    int status = NXgetdata(fFileHandle, i_data_ptr);
    if (status != NX_OK) {
      return NX_ERROR;
    }

    if (!fNxEntry2->GetInstrument()->GetDetector()->SetLastGoodBin(i_data_ptr)) {
      cerr << endl << ">> **ERROR** " << fNxEntry2->GetInstrument()->GetDetector()->GetErrorMsg() << endl;
      return NX_ERROR;
    }

    // clean up
    if (data_ptr) {
      delete [] data_ptr;
      data_ptr = 0;
    }

    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'last_good_bin' data in NXdetector!")) return NX_ERROR;
  } else if (SearchInGroup("last_good_time", "name", nxname, nxclass, dataType)) {
    if (!ErrorHandler(NXopendata(fFileHandle, "last_good_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'last_good_time' data in NXdetector!")) return NX_ERROR;

    // check the dimensions of the 'last_good_time' vector
    if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get 'last_good_time' info in NXdetector!")) return NX_ERROR;

    // calculate the needed size
    size = dims[0];
    for (int i=1; i<rank; i++)
      size *= dims[i];
    noOfElements = size;
    size *= GetDataSize(type);

    // allocate locale memory to get the data
    char *data_ptr = new char[size];
    if (data_ptr == 0) {
      return NX_ERROR;
    }

    // get the data
    float *f_data_ptr = (float*) data_ptr;
    int status = NXgetdata(fFileHandle, f_data_ptr);
    if (status != NX_OK) {
      return NX_ERROR;
    }

    // set the lgb's based on the lgb time stamp and the time resolution
    int *p_lgb = 0;
    if (rank == 1) {
      p_lgb = new int;
      *p_lgb = (int)(*f_data_ptr / (float)fNxEntry2->GetInstrument()->GetDetector()->GetTimeResolution(str));
    } else { // rank == 2
      p_lgb = new int[noOfElements];
      for (int i=0; i<noOfElements; i++) {
        *(p_lgb+i) = (int)(*(f_data_ptr+i) / (float)fNxEntry2->GetInstrument()->GetDetector()->GetTimeResolution(str));
      }
    }

    if (fNxEntry2->GetInstrument()->GetDetector()->SetFirstGoodBin(p_lgb)) {
      cerr << endl << ">> **ERROR** " << fNxEntry2->GetInstrument()->GetDetector()->GetErrorMsg() << endl;
      return NX_ERROR;
    }

    // clean up
    if (p_lgb) {
      delete [] p_lgb;
      p_lgb = 0;
    }
    if (data_ptr) {
      delete [] data_ptr;
      data_ptr = 0;
    }

    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'last_good_time' data in NXdetector!")) return NX_ERROR;
  } else {
    cerr << endl << ">> **WARNING** found neither 'last_good_bin' nor 'last_good_time' values ..." << endl;
  }

  // close group NXdetector
  NXclosegroup(fFileHandle);

  // close group NXinstrument
  NXclosegroup(fFileHandle);

  // GET DATA : STILL MISSING : The real data are found under NXentry:NXinstrument:NXdetector
  // CHECK if it is necessary to read ANY of these data

  // close group NXentry
  NXclosegroup(fFileHandle);

  // close file
  NXclose(&fFileHandle);

  fValid = true;

  return NX_OK;
}

//------------------------------------------------------------------------------------------
// GroupHistoData (private)
//------------------------------------------------------------------------------------------
/**
 * <p>Feed the grouped histo data, based on the grouping vector and the raw histo data.
 */
NXstatus PNeXus::GroupHistoData()
{
  if (fIdfVersion == 1) {
    // check if NO grouping is whished
    if (fNxEntry1->GetData()->GetGrouping()->size() == 0) {
      return NX_OK;
    }

    // check that the grouping size is equal to the number of histograms
    if (fNxEntry1->GetData()->GetGrouping()->size() != fNxEntry1->GetData()->GetNoOfHistos()) {
      fErrorCode = PNEXUS_HISTO_ERROR;
      fErrorMsg = "grouping vector size is unequal to the number of histos present!";
      return NX_ERROR;
    }

    // make a vector of all grouping present
    vector<unsigned int> groupingValue;
    bool newGroup = true;
    for (unsigned int i=0; i<fNxEntry1->GetData()->GetGrouping()->size(); i++) {
      newGroup = true;
      for (unsigned int j=0; j<groupingValue.size(); j++) {
        if ((int)groupingValue[j] == fNxEntry1->GetData()->GetGrouping()->at(i)) {
          newGroup = false;
          break;
        }
      }
      if (newGroup)
        groupingValue.push_back(fNxEntry1->GetData()->GetGrouping()->at(i));
    }

    // check that none of the grouping values is outside of the valid range
    for (unsigned int i=0; i<groupingValue.size(); i++) {
      if (groupingValue[i]-1 >= fNxEntry1->GetData()->GetNoOfHistos()) {
        fErrorCode = PNEXUS_HISTO_ERROR;
        fErrorMsg = "grouping values out of range";
        return NX_ERROR;
      }
    }

    // set fGroupedHisto to the proper size
    fGroupedHisto.clear();
    fGroupedHisto.resize(groupingValue.size());
    for (unsigned int i=0; i<fGroupedHisto.size(); i++) {
      fGroupedHisto[i].resize(fNxEntry1->GetData()->GetHisto(0)->size());
    }

    for (unsigned int i=0; i<fNxEntry1->GetData()->GetNoOfHistos(); i++) {
      for (unsigned int j=0; j<fNxEntry1->GetData()->GetHisto(i)->size(); j++) {
        fGroupedHisto[fNxEntry1->GetData()->GetGrouping()->at(i)-1][j] += fNxEntry1->GetData()->GetHisto(i)->at(j);
      }
    }

    // cleanup
    groupingValue.clear();
  } else if (fIdfVersion == 2) {
    // will probably do nothing here
  } else {
    fErrorCode = PNEXUS_HISTO_ERROR;
    fErrorMsg = "unsupported IDF";
    return NX_ERROR;
  }

  return NX_OK;
}

//------------------------------------------------------------------------------------------
// WriteFileIdf1 (private)
//------------------------------------------------------------------------------------------
/**
 * <p>Write the NeXus file of type IDF Version 1.
 *
 * <b>return:</b>
 * - NX_OK on successfull writting
 * - NX_ERROR on error. The error code/message will give the details.
 *
 * \param fileName file name
 * \param access flag needed to know in which format (HDF4, HDF5, or XML) the file shall be written.
 */
int PNeXus::WriteFileIdf1(const char* fileName, const NXaccess access)
{
  string str;
  char  cstr[1204];
  bool ok = false;
  int   size, idata;
  float fdata;

  memset(cstr, '\0', sizeof(cstr));
  snprintf(cstr, sizeof(cstr), "couldn't open file '%s' for writing", fileName);
  if (!ErrorHandler(NXopen(fileName, access, &fFileHandle), PNEXUS_FILE_OPEN_ERROR, cstr)) return NX_ERROR;

  // write NXfile attribute NeXus_version
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, NEXUS_VERSION, sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "NeXus_version", cstr, strlen(cstr), NX_CHAR), PNEXUS_SET_ATTR_ERROR, "couldn't set NXfile attribute 'NeXus_version'")) return NX_ERROR;

  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetUser()->GetName().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "user", cstr, strlen(cstr), NX_CHAR), PNEXUS_SET_ATTR_ERROR, "couldn't set NXfile attributes")) return NX_ERROR;

  // make group 'run'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "run", "NXentry"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'NXfile/run'.")) return NX_ERROR;
  // open group 'run'
  if (!ErrorHandler(NXopengroup(fFileHandle, "run", "NXentry"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'NXfile/run' for writting.")) return NX_ERROR;

  // write IDF_version
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "IDF_version", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXentry/IDF_version'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "IDF_version"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXentry/IDF_version' for writting.")) return NX_ERROR;
  idata = fIdfVersion;
  if (!ErrorHandler(NXputdata(fFileHandle, &idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXentry/IDF_version'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write program_name, and attribute version
  size = fNxEntry1->GetProgramName().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "program_name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXentry/program_name'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "program_name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXentry/program_name' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetProgramName().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXentry/program_name'.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetProgramVersion().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "version", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'version' for 'NXentry/program_name'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'number'
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "number", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXentry/number'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "number"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXentry/number' for writting.")) return NX_ERROR;
  idata = fNxEntry1->GetRunNumber();
  if (!ErrorHandler(NXputdata(fFileHandle, &idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXentry/number'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'title'
  size = fNxEntry1->GetTitle().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "title", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXentry/title'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "title"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXentry/title' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetTitle().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXentry/title'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'notes'
  size = fNxEntry1->GetNotes().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "notes", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXentry/notes'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "notes"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXentry/notes' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetNotes().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXentry/notes'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'analysis'
  size = fNxEntry1->GetAnalysis().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "analysis", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXentry/analysis'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "analysis"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXentry/analysis' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetAnalysis().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXentry/analysis'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'lab'
  size = fNxEntry1->GetLaboratory().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "lab", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXentry/lab'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "lab"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXentry/lab' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetLaboratory().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXentry/lab'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'beamline'
  size = fNxEntry1->GetBeamline().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "beamline", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXentry/beamline'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "beamline"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXentry/beamline' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetBeamline().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXentry/beamline'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'start_time'
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetStartTime().c_str(), sizeof(cstr));
  size = strlen(cstr);
  if (!ErrorHandler(NXmakedata(fFileHandle, "start_time", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXentry/start_time'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "start_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXentry/start_time' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXentry/start_time'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'stop_time'
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetStopTime().c_str(), sizeof(cstr));
  size = strlen(cstr);
  if (!ErrorHandler(NXmakedata(fFileHandle, "stop_time", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXentry/stop_time'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "stop_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXentry/stop_time' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXentry/stop_time'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'switching_states'
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "switching_states", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXentry/switching_states'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "switching_states"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXentry/switching_states' for writting.")) return NX_ERROR;
  idata = fNxEntry1->GetSwitchingState();
  if (!ErrorHandler(NXputdata(fFileHandle, &idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXentry/switching_states'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // make group 'user'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "user", "NXuser"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'NXuser/user'.")) return NX_ERROR;
  // open group 'user'
  if (!ErrorHandler(NXopengroup(fFileHandle, "user", "NXuser"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'NXuser/user' for writting.")) return NX_ERROR;

  // write user 'name'
  size = fNxEntry1->GetUser()->GetName().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXuser/name'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXuser/name' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetUser()->GetName().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXuser/name'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write user 'experiment_number'
  size = fNxEntry1->GetUser()->GetExperimentNumber().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "experiment_number", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXuser/experiment_number'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "experiment_number"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXuser/experiment_number' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetUser()->GetExperimentNumber().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXuser/experiment_number'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // close group 'user'
  NXclosegroup(fFileHandle);

  // make group 'sample'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "sample", "NXsample"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'NXsample/sample'.")) return NX_ERROR;
  // open group 'sample'
  if (!ErrorHandler(NXopengroup(fFileHandle, "sample", "NXsample"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'NXsample/sample' for writting.")) return NX_ERROR;

  // write sample 'name'
  size = fNxEntry1->GetSample()->GetName().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXsample/name'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXsample/name' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetSample()->GetName().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXsample/name'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write sample 'temperature'
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "temperature", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXsample/temperature'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "temperature"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXsample/temperature' for writting.")) return NX_ERROR;
  fdata = (float)fNxEntry1->GetSample()->GetPhysPropValue(string("temperature"), ok);
  if (!ErrorHandler(NXputdata(fFileHandle, &fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXsample/temperature'.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  str = string("n/a");
  fNxEntry1->GetSample()->GetPhysPropUnit(string("temperature"), str, ok);
  strncpy(cstr, str.c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'NXsample/temperature'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write sample 'magnetic_field'
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "magnetic_field", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXsample/magnetic_field'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXsample/magnetic_field' for writting.")) return NX_ERROR;
  fdata = (float)fNxEntry1->GetSample()->GetPhysPropValue(string("magnetic_field"), ok);
  if (!ErrorHandler(NXputdata(fFileHandle, &fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXsample/magnetic_field'.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  str = string("n/a");
  fNxEntry1->GetSample()->GetPhysPropUnit(string("magnetic_field"), str, ok);
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, str.c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'NXsample/magnetic_field'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write sample 'shape' only if populated with something different than 'n/a'
  if (fNxEntry1->GetSample()->GetShape() != "n/a") {
    size = fNxEntry1->GetSample()->GetShape().length();
    if (!ErrorHandler(NXmakedata(fFileHandle, "shape", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXsample/shape'.")) return NX_ERROR;
    if (!ErrorHandler(NXopendata(fFileHandle, "shape"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXsample/shape' for writting.")) return NX_ERROR;
    strncpy(cstr, fNxEntry1->GetSample()->GetShape().c_str(), sizeof(cstr));
    if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXsample/shape'.")) return NX_ERROR;
    NXclosedata(fFileHandle);
  }

  // write sample 'magnetic_field_state'
  size = fNxEntry1->GetSample()->GetMagneticFieldState().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "magnetic_field_state", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXsample/magnetic_field_state'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field_state"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXsample/magnetic_field_state' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetSample()->GetMagneticFieldState().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXsample/magnetic_field_state'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write sample 'magnetic_field_vector'
  float *magFieldVec;
  if (fNxEntry1->GetSample()->IsMagneticFieldVectorAvailable()) {
    idata = 1;
    size = fNxEntry1->GetSample()->GetMagneticFieldVector().size();
    magFieldVec = new float[size];
    for (int i=0; i<size; i++)
      magFieldVec[i] = (float)fNxEntry1->GetSample()->GetMagneticFieldVector().at(i);
  } else {
    idata = 0;
    size = 3;
    magFieldVec = new float[size];
    for (int i=0; i<size; i++)
      magFieldVec[i] = 0.0;
  }
  if (!ErrorHandler(NXmakedata(fFileHandle, "magnetic_field_vector", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXsample/magnetic_field_vector'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field_vector"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXsample/magnetic_field_vector' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, magFieldVec), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXsample/magnetic_field_vector'.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetSample()->GetMagneticFieldVectorCoordinateSystem().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "coordinate_system", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'coordinate_system' for 'NXsample/magnetic_field_vector'")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetSample()->GetMagneticFieldVectorUnits().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'NXsample/magnetic_field_vector'")) return NX_ERROR;
  if (!ErrorHandler(NXputattr(fFileHandle, "available", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'available' for 'NXsample/magnetic_field_vector'")) return NX_ERROR;
  NXclosedata(fFileHandle);
  if (magFieldVec) {
    delete [] magFieldVec;
    magFieldVec = 0;
  }

  // write sample 'environment'
  size = fNxEntry1->GetSample()->GetEnvironment().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "environment", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXsample/environment'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "environment"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXsample/environment' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetSample()->GetEnvironment().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXsample/environment'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // close group 'sample'
  NXclosegroup(fFileHandle);

  // make group 'instrument'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "instrument", "NXinstrument"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'NXinstrument/instrument'.")) return NX_ERROR;
  // open group 'instrument'
  if (!ErrorHandler(NXopengroup(fFileHandle, "instrument", "NXinstrument"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'NXinstrument/instrument' for writting.")) return NX_ERROR;

  // write instrument 'name'
  size = fNxEntry1->GetInstrument()->GetName().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXinstrument/name'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXinstrument/name' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetInstrument()->GetName().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXinstrument/name'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // make group 'detector'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "detector", "NXdetector"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'NXdetector/detector'.")) return NX_ERROR;
  // open group 'detector'
  if (!ErrorHandler(NXopengroup(fFileHandle, "detector", "NXdetector"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'NXdetector/detector' for writting.")) return NX_ERROR;

  // write detector 'number'
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "number", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXdetector/number'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "number"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXdetector/number' for writting.")) return NX_ERROR;
  idata = fNxEntry1->GetInstrument()->GetDetector()->GetNumber();
  if (!ErrorHandler(NXputdata(fFileHandle, &idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXdetector/number'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // close group 'detector'
  NXclosegroup(fFileHandle);

  // make group 'collimator'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "collimator", "NXcollimator"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'NXcollimator/collimator'.")) return NX_ERROR;
  // open group 'collimator'
  if (!ErrorHandler(NXopengroup(fFileHandle, "collimator", "NXcollimator"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'NXcollimator/collimator' for writting.")) return NX_ERROR;

  // write collimator 'type'
  size = fNxEntry1->GetInstrument()->GetCollimator()->GetType().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "type", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXcollimator/type'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "type"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXcollimator/type' for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetInstrument()->GetCollimator()->GetType().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXcollimator/type'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // close group 'collimator'
  NXclosegroup(fFileHandle);

  // make group 'beam'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "beam", "NXbeam"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'NXbeam/beam'.")) return NX_ERROR;
  // open group 'beam'
  if (!ErrorHandler(NXopengroup(fFileHandle, "beam", "NXbeam"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'NXbeam/beam' for writting.")) return NX_ERROR;

  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "total_counts", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXbeam/total_counts'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "total_counts"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXbeam/total_counts' for writting.")) return NX_ERROR;
  fdata = (float)fNxEntry1->GetInstrument()->GetBeam()->GetTotalCounts();
  if (!ErrorHandler(NXputdata(fFileHandle, &fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXbeam/total_counts'.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry1->GetInstrument()->GetBeam()->GetUnits().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'NXbeam/total_counts'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // close group 'beam'
  NXclosegroup(fFileHandle);

  // close group 'instrument'
  NXclosegroup(fFileHandle);

  // make group 'histogram_data_1'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "histogram_data_1", "NXdata"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'NXdata/histogram_data_1'.")) return NX_ERROR;
  // open group 'histogram_data_1'
  if (!ErrorHandler(NXopengroup(fFileHandle, "histogram_data_1", "NXdata"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'NXdata/histogram_data_1' for writting.")) return NX_ERROR;

  // write data 'counts'
  int *histo_data=0;
  int histo_size[2];
  if (fNxEntry1->GetData()->GetNoOfHistos() == 0) {
    histo_data = new int[1];
    histo_data[0] = -1;
    histo_size[0] = 1;
    histo_size[1] = 1;
  } else {
    int noOfHistos  = fNxEntry1->GetData()->GetNoOfHistos();
    int histoLength = fNxEntry1->GetData()->GetHisto(0)->size();
    histo_data = new int[noOfHistos*histoLength];
    for (int i=0; i<noOfHistos; i++)
      for (int j=0; j<histoLength; j++)
        histo_data[i*histoLength+j] = fNxEntry1->GetData()->GetHisto(i)->at(j);
    histo_size[0] = noOfHistos;
    histo_size[1] = histoLength;
  }

  if (!ErrorHandler(NXcompmakedata(fFileHandle, "counts", NX_INT32, 2, histo_size, NX_COMP_LZW, histo_size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXdata/counts'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "counts"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXdata/counts' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, (void*)histo_data), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXdata/counts'.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "counts", sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'NXdata/counts'")) return NX_ERROR;
  idata = 1;
  if (!ErrorHandler(NXputattr(fFileHandle, "signal", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'signal' for 'NXdata/counts'")) return NX_ERROR;
  idata = fNxEntry1->GetData()->GetNoOfHistos();
  if (!ErrorHandler(NXputattr(fFileHandle, "number", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'number' for 'NXdata/counts'")) return NX_ERROR;
  idata = fNxEntry1->GetData()->GetHistoLength();
  if (!ErrorHandler(NXputattr(fFileHandle, "length", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'length' for 'NXdata/counts'")) return NX_ERROR;
  idata = fNxEntry1->GetData()->GetT0(0);
  if (!ErrorHandler(NXputattr(fFileHandle, "t0_bin", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'T0_bin' for 'NXdata/counts'")) return NX_ERROR;
  idata = fNxEntry1->GetData()->GetFirstGoodBin(0);
  if (!ErrorHandler(NXputattr(fFileHandle, "first_good_bin", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'first_good_bin' for 'NXdata/counts'")) return NX_ERROR;
  idata = fNxEntry1->GetData()->GetLastGoodBin(0);
  if (!ErrorHandler(NXputattr(fFileHandle, "last_good_bin", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'last_good_bin' for 'NXdata/counts'")) return NX_ERROR;
  fdata = (float)fNxEntry1->GetData()->GetTimeResolution("ps")/2.0;
  if (!ErrorHandler(NXputattr(fFileHandle, "offset", &fdata, 1, NX_FLOAT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'offset' for 'NXdata/counts'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  if (histo_data) {
    delete [] histo_data;
  }

  // write data 'resolution'
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "resolution", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXdata/resolution'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "resolution"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXdata/resolution' for writting.")) return NX_ERROR;
  fdata = fNxEntry1->GetData()->GetTimeResolution("fs");
  idata = (int)fdata;
  if (!ErrorHandler(NXputdata(fFileHandle, &idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXdata/resolution'.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "femtoseconds", sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'NXdata/resolution'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write data 'time_zero' based on t0_bin which is the master!
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "time_zero", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXdata/time_zero'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "time_zero"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXdata/time_zero' for writting.")) return NX_ERROR;
  fdata = (float)fNxEntry1->GetData()->GetT0(0) * (float)fNxEntry1->GetData()->GetTimeResolution("us");
  if (!ErrorHandler(NXputdata(fFileHandle, &fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXdata/time_zero'.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "microseconds", sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'NXdata/time_zero'")) return NX_ERROR;
  idata = 1;
  if (!ErrorHandler(NXputattr(fFileHandle, "available", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'available' for 'NXdata/time_zero'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write data 'raw_time'
  int noOfHistos  = fNxEntry1->GetData()->GetNoOfHistos();
  int histoLength = fNxEntry1->GetData()->GetHisto(0)->size();
  if (noOfHistos == 0) {
    fErrorCode = PNEXUS_HISTO_ERROR;
    fErrorMsg  = "no data for writing present.";
    return NX_ERROR;
  }
  float *raw_time = new float[histoLength];
  for (int i=0; i<histoLength; i++) {
    raw_time[i] = (float)fNxEntry1->GetData()->GetTimeResolution("us") * (float)i; // raw time in (us)
  }
  size = histoLength;
  if (!ErrorHandler(NXmakedata(fFileHandle, "raw_time", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXdata/raw_time'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "raw_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXdata/raw_time' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, raw_time), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXdata/raw_time'.")) return NX_ERROR;
  idata = 1;
  if (!ErrorHandler(NXputattr(fFileHandle, "axis", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'axis' for 'NXdata/raw_time'")) return NX_ERROR;
  idata = 1;
  if (!ErrorHandler(NXputattr(fFileHandle, "primary", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'primary' for 'NXdata/raw_time'")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "microseconds", sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'NXdata/raw_time'")) return NX_ERROR;
  NXclosedata(fFileHandle);
  if (raw_time) {
    delete [] raw_time;
    raw_time = 0;
  }

  // write data 'corrected_time'
  float *corrected_time = new float[histoLength];
  for (int i=0; i<histoLength; i++) {
    corrected_time[i] = (float)fNxEntry1->GetData()->GetTimeResolution("us") * (float)((int)i-(int)fNxEntry1->GetData()->GetT0(0)+1); // raw time in (us)
  }
  size = histoLength;
  if (!ErrorHandler(NXmakedata(fFileHandle, "corrected_time", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXdata/corrected_time'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "corrected_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXdata/corrected_time' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, corrected_time), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXdata/corrected_time'.")) return NX_ERROR;
  idata = 1;
  if (!ErrorHandler(NXputattr(fFileHandle, "axis", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'axis' for 'NXdata/corrected_time'")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "microseconds", sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'NXdata/corrected_time'")) return NX_ERROR;
  NXclosedata(fFileHandle);
  if (corrected_time) {
    delete [] corrected_time;
    corrected_time = 0;
  }

  // write data 'grouping'
  int *grouping = new int[noOfHistos];
  vector<int> groupNo; // keep the number of different groupings
  if (noOfHistos == (int)fNxEntry1->GetData()->GetGrouping()->size()) { // grouping vector seems to be properly defined
    bool found;
    groupNo.push_back(fNxEntry1->GetData()->GetGrouping()->at(0));
    for (int i=0; i<noOfHistos; i++) {
      grouping[i] = fNxEntry1->GetData()->GetGrouping()->at(i);
      found = false;
      for (unsigned int j=0; j<groupNo.size(); j++) {
        if ((int)fNxEntry1->GetData()->GetGrouping()->at(i) == groupNo[j]) {
          found = true;
          break;
        }
      }
      if (!found) {
        groupNo.push_back(fNxEntry1->GetData()->GetGrouping()->at(i));
      }
    }
  } else { // grouping vector not available
    for (int i=0; i<noOfHistos; i++) {
      grouping[i] = 0;
    }
  }
  size = noOfHistos;
  if (!ErrorHandler(NXmakedata(fFileHandle, "grouping", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXdata/grouping'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "grouping"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXdata/grouping' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, grouping), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXdata/grouping'.")) return NX_ERROR;
  idata = groupNo.size();
  if (!ErrorHandler(NXputattr(fFileHandle, "available", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'available' for 'NXdata/grouping'")) return NX_ERROR;
  NXclosedata(fFileHandle);
  if (grouping) {
    delete [] grouping;
    grouping = 0;
  }
  groupNo.clear();

  // write data 'alpha'
  float *alpha;
  int array_size[2];
  if (fNxEntry1->GetData()->GetAlpha()->size() == 0) {
    alpha = new float[3];
    alpha[0] = -1.0;
    alpha[1] = -1.0;
    alpha[2] = -1.0;
    array_size[0] = 1;
    array_size[1] = 3;
  } else {
    alpha = new float[fNxEntry1->GetData()->GetAlpha()->size()*3];
    for (unsigned int i=0; i<fNxEntry1->GetData()->GetAlpha()->size(); i++) {
      alpha[i] = (float)fNxEntry1->GetData()->GetAlpha()->at(i).GetGroupFirst();
      alpha[i+1] = (float)fNxEntry1->GetData()->GetAlpha()->at(i).GetGroupSecond();
      alpha[i+2] = (float)(float)fNxEntry1->GetData()->GetAlpha()->at(i).GetAlpha();
    }
    array_size[0] = fNxEntry1->GetData()->GetAlpha()->size();
    array_size[1] = 3;
  }
  if (!ErrorHandler(NXmakedata(fFileHandle, "alpha", NX_FLOAT32, 2, array_size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'NXdata/alpha'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "alpha"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'NXdata/alpha' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, (void*)alpha), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'NXdata/alpha'.")) return NX_ERROR;
  idata = fNxEntry1->GetData()->GetAlpha()->size();
  if (!ErrorHandler(NXputattr(fFileHandle, "available", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'available' for 'NXdata/alpha'")) return NX_ERROR;
  if (alpha) {
    delete [] alpha;
  }

  // close group 'histogram_data_1'
  NXclosegroup(fFileHandle);

  // close group 'run'
  NXclosegroup(fFileHandle);

  NXclose(&fFileHandle);

  return NX_OK;
}

//------------------------------------------------------------------------------------------
// WriteFileIdf2 (private)
//------------------------------------------------------------------------------------------
/**
 * <p>Write the NeXus file of type IDF Version 2.
 *
 * <b>return:</b>
 * - NX_OK on successfull writting
 * - NX_ERROR on error. The error code/message will give the details.
 *
 * \param fileName file name
 * \param access flag needed to know in which format (HDF4, HDF5, or XML) the file shall be written.
 */
int PNeXus::WriteFileIdf2(const char* fileName, const NXaccess access)
{
  string str;
  char  cstr[1204];
  bool ok = false;
  int   size, idata;
  float fdata;
  double dval;
  NXlink nxLink;
  vector<NXlink> nxLinkVec;

  memset(cstr, '\0', sizeof(cstr));
  snprintf(cstr, sizeof(cstr), "couldn't open file '%s' for writing", fileName);
  if (!ErrorHandler(NXopen(fileName, access, &fFileHandle), PNEXUS_FILE_OPEN_ERROR, cstr)) return NX_ERROR;

  // write NXroot attribute 'file_name'
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fileName, sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "file_name", cstr, strlen(cstr), NX_CHAR), PNEXUS_SET_ATTR_ERROR, "couldn't set NXroot attribute 'file_name'")) return NX_ERROR;

  // write NXroot attribute 'creator'
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fCreator.c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "creator", cstr, strlen(cstr), NX_CHAR), PNEXUS_SET_ATTR_ERROR, "couldn't set NXroot attribute 'creator'")) return NX_ERROR;

  // make group 'raw_data_1'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "raw_data_1", "NXentry"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'raw_data_1' in NXroot.")) return NX_ERROR;
  // open group 'raw_data_1'
  if (!ErrorHandler(NXopengroup(fFileHandle, "raw_data_1", "NXentry"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'raw_data_1' in NXroot for writting.")) return NX_ERROR;

  // write idf_version
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "idf_version", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'idf_version' in NXentry.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "idf_version"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'idf_version' in NXentry for writting.")) return NX_ERROR;
  idata = fIdfVersion;
  if (!ErrorHandler(NXputdata(fFileHandle, &idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'idf_version' in NXentry.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'definition'
  size = fNxEntry2->GetDefinition().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "definition", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'definition' in NXentry.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "definition"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'definition' in NXentry for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry2->GetDefinition().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'definition' in NXentry.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write 'program_name' if present
  if (!fNxEntry2->GetProgramName().empty()) {
    size = fNxEntry2->GetProgramName().length();
    if (!ErrorHandler(NXmakedata(fFileHandle, "program_name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'program_name' in NXentry.")) return NX_ERROR;
    if (!ErrorHandler(NXopendata(fFileHandle, "program_name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'program_name' in NXentry for writting.")) return NX_ERROR;
    memset(cstr, '\0', sizeof(cstr));
    strncpy(cstr, fNxEntry2->GetProgramName().c_str(), sizeof(cstr));
    if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'program_name' in NXentry.")) return NX_ERROR;
    memset(cstr, '\0', sizeof(cstr));
    strncpy(cstr, fNxEntry2->GetProgramVersion().c_str(), sizeof(cstr));
    if (!ErrorHandler(NXputattr(fFileHandle, "version", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'version' for 'program_name' in NXentry.")) return NX_ERROR;
    NXclosedata(fFileHandle);
  }

  // write run 'run_number'
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "run_number", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'run_number' in NXentry.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "run_number"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'run_number' in NXentry for writting.")) return NX_ERROR;
  idata = fNxEntry2->GetRunNumber();
  if (!ErrorHandler(NXputdata(fFileHandle, &idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'run_number' in NXentry.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'title'
  size = fNxEntry2->GetTitle().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "title", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'title' in NXentry.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "title"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'title' in NXentry for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry2->GetTitle().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'title' in NXentry.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'start_time'
  size = fNxEntry2->GetStartTime().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "start_time", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'start_time' in NXentry.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "start_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'start_time' in NXentry for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry2->GetStartTime().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'start_time' in NXentry.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'end_time'
  size = fNxEntry2->GetStopTime().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "end_time", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'end_time' in NXentry.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "end_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'end_time' in NXentry for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry2->GetStopTime().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'end_time' in NXentry.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'experiment_identifier'
  size = fNxEntry2->GetExperimentIdentifier().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "experiment_identifier", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'experiment_identifier' in NXentry.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "experiment_identifier"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'experiment_identifier' in NXentry for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry2->GetExperimentIdentifier().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'experiment_identifier' in NXentry.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // user_1 (NXuser) will only be written if there is at least a user name present
  if (fNxEntry2->GetUser()->GetName() != "n/a") {
    // make group 'user_1'
    if (!ErrorHandler(NXmakegroup(fFileHandle, "user_1", "NXuser"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'user_1' in NXentry.")) return NX_ERROR;
    // open group 'user'
    if (!ErrorHandler(NXopengroup(fFileHandle, "user_1", "NXuser"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'user_1' in NXentry for writting.")) return NX_ERROR;

    // write user 'name'
    size = fNxEntry2->GetUser()->GetName().length();
    if (!ErrorHandler(NXmakedata(fFileHandle, "name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'name' in NXuser.")) return NX_ERROR;
    if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'name' in NXuser for writting.")) return NX_ERROR;
    memset(cstr, '\0', sizeof(cstr));
    strncpy(cstr, fNxEntry2->GetUser()->GetName().c_str(), sizeof(cstr));
    if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'name' in NXuser.")) return NX_ERROR;
    NXclosedata(fFileHandle);

    // close group 'user_1'
    NXclosegroup(fFileHandle);
  }

  // make group 'sample'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "sample", "NXsample"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'sample' in NXentry.")) return NX_ERROR;
  // open group 'sample'
  if (!ErrorHandler(NXopengroup(fFileHandle, "sample", "NXsample"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'sample' in NXentry for writting.")) return NX_ERROR;

  // write sample 'name'
  size = fNxEntry2->GetSample()->GetName().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'name' in NXsample.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'name' in NXsample for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry2->GetSample()->GetName().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'name' in NXsample.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // check if description is present
  if (fNxEntry2->GetSample()->GetDescription().compare("n/a")) {
    // write sample 'description'
    size = fNxEntry2->GetSample()->GetDescription().length();
    if (!ErrorHandler(NXmakedata(fFileHandle, "description", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'description' in NXsample.")) return NX_ERROR;
    if (!ErrorHandler(NXopendata(fFileHandle, "description"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'description' in NXsample for writting.")) return NX_ERROR;
    memset(cstr, '\0', sizeof(cstr));
    strncpy(cstr, fNxEntry2->GetSample()->GetDescription().c_str(), sizeof(cstr));
    if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'description' in NXsample.")) return NX_ERROR;
    NXclosedata(fFileHandle);
  }

  // check if temperature is present and if yes, write it into the file
  ok=false;
  dval = fNxEntry2->GetSample()->GetPhysPropValue("temperature_1", ok);
  if (ok) {
    fdata = (float)dval;
    fNxEntry2->GetSample()->GetPhysPropUnit("temperature_1", str, ok);
  }
  if (ok) {
    size = 1;
    if (!ErrorHandler(NXmakedata(fFileHandle, "temperature_1", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'temperature_1' in NXsample.")) return NX_ERROR;
    if (!ErrorHandler(NXopendata(fFileHandle, "temperature_1"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'temperature_1' in NXsample for writting.")) return NX_ERROR;
    if (!ErrorHandler(NXputdata(fFileHandle, (void*)&fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'temperature_1' in NXsample.")) return NX_ERROR;
    memset(cstr, '\0', sizeof(cstr));
    strncpy(cstr, str.c_str(), sizeof(cstr));
    if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'temperature_1' in NXsample.")) return NX_ERROR;
    NXclosedata(fFileHandle);
  }

  // check if temperature environment info is present
  if (fNxEntry2->GetSample()->GetEnvironmentTemp().compare("n/a")) {
    // make group 'temperature_1_env'
    if (!ErrorHandler(NXmakegroup(fFileHandle, "temperature_1_env", "NXenvironment"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'temperature_1_env' in NXsample.")) return NX_ERROR;
    // open group 'temperature_1_env'
    if (!ErrorHandler(NXopengroup(fFileHandle, "temperature_1_env", "NXenvironment"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'NXenvironment' in NXsample for writting.")) return NX_ERROR;
    // write sample 'temperature_1_env'
    size = fNxEntry2->GetSample()->GetEnvironmentTemp().length();
    if (!ErrorHandler(NXmakedata(fFileHandle, "name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'name' in 'temperature_1_env' in NXsample.")) return NX_ERROR;
    if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'name' in 'temperature_1_env' in NXsample for writting.")) return NX_ERROR;
    memset(cstr, '\0', sizeof(cstr));
    strncpy(cstr, fNxEntry2->GetSample()->GetEnvironmentTemp().c_str(), sizeof(cstr));
    if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'temperature_1_env' in NXsample.")) return NX_ERROR;
    NXclosedata(fFileHandle);
    // close group 'temperature_1_env'
    NXclosegroup(fFileHandle);
  }

  // check if magnetic field is present and if yes, write it into the file
  ok=false;
  dval = fNxEntry2->GetSample()->GetPhysPropValue("magnetic_field_1", ok);
  if (ok) {
    fdata = (float)dval;
    fNxEntry2->GetSample()->GetPhysPropUnit("magnetic_field_1", str, ok);
  }
  if (ok) {
    size = 1;
    if (!ErrorHandler(NXmakedata(fFileHandle, "magnetic_field_1", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'magnetic_field_1' in NXsample.")) return NX_ERROR;
    if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field_1"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'magnetic_field_1' in NXsample for writting.")) return NX_ERROR;
    if (!ErrorHandler(NXputdata(fFileHandle, (void*)&fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'magnetic_field_1' in NXsample.")) return NX_ERROR;
    memset(cstr, '\0', sizeof(cstr));
    strncpy(cstr, str.c_str(), sizeof(cstr));
    if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'magnetic_field_1' in NXsample.")) return NX_ERROR;
    NXclosedata(fFileHandle);
  }

  // check if magnetic field environment info is present
  if (fNxEntry2->GetSample()->GetEnvironmentField().compare("n/a")) {
    // make group 'magnetic_field_1_env'
    if (!ErrorHandler(NXmakegroup(fFileHandle, "magnetic_field_1_env", "NXenvironment"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'magnetic_field_1_env' in NXsample.")) return NX_ERROR;
    // open group 'magnetic_field_1_env'
    if (!ErrorHandler(NXopengroup(fFileHandle, "magnetic_field_1_env", "NXenvironment"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'NXenvironment' in NXsample for writting.")) return NX_ERROR;
    // write sample 'magnetic_field_1_env'
    size = fNxEntry2->GetSample()->GetEnvironmentField().length();
    if (!ErrorHandler(NXmakedata(fFileHandle, "magnetic_field_1_env", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'magnetic_field_1_env' in NXsample.")) return NX_ERROR;
    if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field_1_env"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'magnetic_field_1_env' in NXsample for writting.")) return NX_ERROR;
    memset(cstr, '\0', sizeof(cstr));
    strncpy(cstr, fNxEntry2->GetSample()->GetEnvironmentField().c_str(), sizeof(cstr));
    if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'magnetic_field_1_env' in NXsample.")) return NX_ERROR;
    NXclosedata(fFileHandle);
    // close group 'magnetic_field_1_env'
    NXclosegroup(fFileHandle);
  }

  // check if magnetic field state info is present
  if (fNxEntry2->GetSample()->GetMagneticFieldState().compare("n/a")) {
    // write sample 'magnetic_field_state'
    size = fNxEntry2->GetSample()->GetMagneticFieldState().length();
    if (!ErrorHandler(NXmakedata(fFileHandle, "magnetic_field_state", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'magnetic_field_state' in NXsample.")) return NX_ERROR;
    if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field_state"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'magnetic_field_state' in NXsample for writting.")) return NX_ERROR;
    memset(cstr, '\0', sizeof(cstr));
    strncpy(cstr, fNxEntry2->GetSample()->GetMagneticFieldState().c_str(), sizeof(cstr));
    if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'magnetic_field_state' in NXsample.")) return NX_ERROR;
    NXclosedata(fFileHandle);
  }

  // close group 'sample'
  NXclosegroup(fFileHandle);


  // make group 'instrument'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "instrument", "NXinstrument"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'instrument' in NXentry.")) return NX_ERROR;
  // open group 'instrument'
  if (!ErrorHandler(NXopengroup(fFileHandle, "instrument", "NXinstrument"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'instrument' in NXentry for writting.")) return NX_ERROR;

  // write instrument 'name'
  size = fNxEntry2->GetInstrument()->GetName().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'name' in NXinstrument.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'name' in NXinstrument for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry2->GetInstrument()->GetName().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'name' in NXinstrument.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // make group 'source'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "source", "NXsource"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'source' in NXinstrument.")) return NX_ERROR;
  // open group 'source'
  if (!ErrorHandler(NXopengroup(fFileHandle, "source", "NXsource"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'source' in NXentry for NXinstrument.")) return NX_ERROR;

  // write instrument 'name'
  size = fNxEntry2->GetInstrument()->GetSource()->GetName().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'name' in NXsource.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'name' in NXsource for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry2->GetInstrument()->GetSource()->GetName().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'name' in NXsource.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write instrument 'type'
  size = fNxEntry2->GetInstrument()->GetSource()->GetType().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "type", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'type' in NXsource.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "type"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'type' in NXsource for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry2->GetInstrument()->GetSource()->GetType().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'type' in NXsource.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write instrument 'probe'
  size = fNxEntry2->GetInstrument()->GetSource()->GetProbe().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "probe", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'probe' in NXsource.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "probe"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'probe' in NXsource for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry2->GetInstrument()->GetSource()->GetProbe().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'probe' in NXsource.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // close group 'source'
  NXclosegroup(fFileHandle);

  // make group 'beamline'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "beamline", "NXbeamline"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'beamline' in NXinstrument.")) return NX_ERROR;
  // open group 'beamline'
  if (!ErrorHandler(NXopengroup(fFileHandle, "beamline", "NXbeamline"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'beamline' in NXentry for NXinstrument.")) return NX_ERROR;

  // write beamline 'beamline'
  size = fNxEntry2->GetInstrument()->GetBeamline()->GetName().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "beamline", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'beamline' in NXbeamline.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "beamline"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'beamline' in NXbeamline for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry2->GetInstrument()->GetBeamline()->GetName().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'beamline' in NXbeamline.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // close group 'beamline'
  NXclosegroup(fFileHandle);

  // make group 'detector_1'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "detector_1", "NXdetector"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'detector_1' in NXinstrument.")) return NX_ERROR;
  // open group 'detector_1'
  if (!ErrorHandler(NXopengroup(fFileHandle, "detector_1", "NXdetector"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'detector_1' in NXinstrument.")) return NX_ERROR;

  // write detector_1 'description'
  size = fNxEntry2->GetInstrument()->GetDetector()->GetDescription().length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "description", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'description' in NXdetector.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "description"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'description' in NXdetector for writting.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, fNxEntry2->GetInstrument()->GetDetector()->GetDescription().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputdata(fFileHandle, cstr), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'description' in NXdetector.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write detector_1 'counts'
  int dims[3];
  if (fNxEntry2->GetInstrument()->GetDetector()->GetNoOfPeriods() > 0) { // counts[][][]
    dims[0] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfPeriods();
    dims[1] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra();
    dims[2] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfBins();
    if (!ErrorHandler(NXcompmakedata(fFileHandle, "counts", NX_INT32, 3, dims, NX_COMP_LZW, dims), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'counts' in NXdetector.")) return NX_ERROR;
  } else {
    if (fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra() > 0) { // counts[][]
      dims[0] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra();
      dims[1] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfBins();
      if (!ErrorHandler(NXcompmakedata(fFileHandle, "counts", NX_INT32, 2, dims, NX_COMP_LZW, dims), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'counts' in NXdetector.")) return NX_ERROR;
    } else { // counts[]
      dims[0] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfBins();
      if (!ErrorHandler(NXcompmakedata(fFileHandle, "counts", NX_INT32, 1, dims, NX_COMP_LZW, dims), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'counts' in NXdetector.")) return NX_ERROR;
    }
  }
  if (!ErrorHandler(NXopendata(fFileHandle, "counts"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'counts' in NXdetector for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, (void*)fNxEntry2->GetInstrument()->GetDetector()->GetHistos()), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'counts' in NXdetector.")) return NX_ERROR;

  // write 'counts' attributes
  idata = 1;
  if (!ErrorHandler(NXputattr(fFileHandle, "signal", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'signal' for 'counts' in NXdetector.")) return NX_ERROR;

  memset(cstr, '\0', sizeof(cstr));
  if (fNxEntry2->GetInstrument()->GetDetector()->GetNoOfPeriods() > 0) { // counts[][][]
    strncpy(cstr, "[period_index, spectrum_index, raw_time_bin]", sizeof(cstr));
  } else {
    if (fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra() > 0) { // counts[][]
      strncpy(cstr, "[spectrum_index, raw_time_bin]", sizeof(cstr));
    } else { // counts[]
      strncpy(cstr, "[raw_time_bin]", sizeof(cstr));
    }
  }
  if (!ErrorHandler(NXputattr(fFileHandle, "axes", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'axes' for 'counts' in NXdetector.")) return NX_ERROR;

  memset(cstr, '\0', sizeof(cstr));
  if (!fNxEntry2->GetInstrument()->GetSource()->GetProbe().compare("positive muons")) {
    strncpy(cstr, "positron counts", sizeof(cstr));
  } else if (!fNxEntry2->GetInstrument()->GetSource()->GetProbe().compare("negative muons")) {
    strncpy(cstr, "electron counts", sizeof(cstr));
  } else {
    strncpy(cstr, "n/a", sizeof(cstr));
  }
  if (!ErrorHandler(NXputattr(fFileHandle, "long_name", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'long_name' for 'counts' in NXdetector.")) return NX_ERROR;
  // create link of 'counts' for NXdata
  if (!ErrorHandler(NXgetdataID(fFileHandle, &nxLink), PNEXUS_LINKING_ERROR, "couldn't obtain link of 'counts' in NXdetector.")) return NX_ERROR;
  nxLinkVec.push_back(nxLink);
  NXclosedata(fFileHandle);

  // write time resolution
  fdata = (float)fNxEntry2->GetInstrument()->GetDetector()->GetTimeResolution("ns");
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "histogram_resolution", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'histogram_resolution' in NXdetector.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "histogram_resolution"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'histogram_resolution' in NXdetector for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, (void*)&fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'histogram_resolution' in NXdetector.")) return NX_ERROR;
  memset(cstr, '\0', sizeof(cstr));
  strncpy(cstr, "nano.second", sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'histogram_resolution' in NXdetector.")) return NX_ERROR;
  // create link of 'histogram_resolution' for NXdata
  if (!ErrorHandler(NXgetdataID(fFileHandle, &nxLink), PNEXUS_LINKING_ERROR, "couldn't obtain link of 'histogram_resolution' in NXdetector.")) return NX_ERROR;
  nxLinkVec.push_back(nxLink);
  NXclosedata(fFileHandle);

  // write detector_1 'raw_time'
  size = (int)fNxEntry2->GetInstrument()->GetDetector()->GetRawTime()->size();
  float *p_fdata = new float[size];
  assert(p_fdata);
  for (int i=0; i<size; i++)
    p_fdata[i] = (float)fNxEntry2->GetInstrument()->GetDetector()->GetRawTime()->at(i);
  if (!ErrorHandler(NXmakedata(fFileHandle, "raw_time", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'raw_time' in NXdetector.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "raw_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'raw_time' in NXdetector for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, (void*)p_fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'raw_time' in NXdetector.")) return NX_ERROR;
  strncpy(cstr, fNxEntry2->GetInstrument()->GetDetector()->GetRawTimeUnit().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'raw_time' in NXdetector.")) return NX_ERROR;
  strncpy(cstr, fNxEntry2->GetInstrument()->GetDetector()->GetRawTimeName().c_str(), sizeof(cstr));
  if (!ErrorHandler(NXputattr(fFileHandle, "long_name", cstr, strlen(cstr), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'long_name' for 'raw_time' in NXdetector.")) return NX_ERROR;
  // create link of 'raw_time' for NXdata
  if (!ErrorHandler(NXgetdataID(fFileHandle, &nxLink), PNEXUS_LINKING_ERROR, "couldn't obtain link of 'raw_time' in NXdetector.")) return NX_ERROR;
  nxLinkVec.push_back(nxLink);
  NXclosedata(fFileHandle);
  // clean up
  if (p_fdata) {
    delete [] p_fdata;
    p_fdata = 0;
  }

  // write detector_1 'spectrum_index'
  size = fNxEntry2->GetInstrument()->GetDetector()->GetSpectrumIndexSize();
  int *p_idata = new int[size];
  assert(p_idata);
  for (int i=0; i<size; i++)
    *(p_idata+i) = fNxEntry2->GetInstrument()->GetDetector()->GetSpectrumIndex(i);
  if (!ErrorHandler(NXmakedata(fFileHandle, "spectrum_index", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'spectrum_index' in NXdetector.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "spectrum_index"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'spectrum_index' in NXdetector for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, (void*)p_idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'spectrum_index' in NXdetector.")) return NX_ERROR;
  // create link of 'spectrum_index' for NXdata
  if (!ErrorHandler(NXgetdataID(fFileHandle, &nxLink), PNEXUS_LINKING_ERROR, "couldn't obtain link of 'spectrum_index' in NXdetector.")) return NX_ERROR;
  nxLinkVec.push_back(nxLink);
  NXclosedata(fFileHandle);
  if (p_idata) {
    delete [] p_idata;
    p_idata = 0;
  }

  // write detector_1 'time_zero_bin' if present
  if (fNxEntry2->GetInstrument()->GetDetector()->IsT0Present()) {
    if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 1) {
      size = 1;
      idata = fNxEntry2->GetInstrument()->GetDetector()->GetT0();
      if (!ErrorHandler(NXmakedata(fFileHandle, "time_zero_bin", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'time_zero_bin' in NXdetector.")) return NX_ERROR;
      if (!ErrorHandler(NXopendata(fFileHandle, "time_zero_bin"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'time_zero_bin' in NXdetector for writting.")) return NX_ERROR;
      if (!ErrorHandler(NXputdata(fFileHandle, (void*)&idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'time_zero_bin' in NXdetector.")) return NX_ERROR;
      NXclosedata(fFileHandle);
    } else if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 2) {
      int dims[1];
      dims[0] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra();
      if (!ErrorHandler(NXmakedata(fFileHandle, "time_zero_bin", NX_INT32, 1, dims), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'time_zero_bin' in NXdetector.")) return NX_ERROR;
      if (!ErrorHandler(NXopendata(fFileHandle, "time_zero_bin"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'time_zero_bin' in NXdetector for writting.")) return NX_ERROR;
      if (!ErrorHandler(NXputdata(fFileHandle, (void*)fNxEntry2->GetInstrument()->GetDetector()->GetT0s()), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'time_zero_bin' in NXdetector.")) return NX_ERROR;
      NXclosedata(fFileHandle);
    } else if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 3) {
      int dims[2];
      dims[0] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfPeriods();
      dims[1] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra();
      if (!ErrorHandler(NXmakedata(fFileHandle, "time_zero_bin", NX_INT32, 2, dims), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'time_zero_bin' in NXdetector.")) return NX_ERROR;
      if (!ErrorHandler(NXopendata(fFileHandle, "time_zero_bin"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'time_zero_bin' in NXdetector for writting.")) return NX_ERROR;
      if (!ErrorHandler(NXputdata(fFileHandle, (void*)fNxEntry2->GetInstrument()->GetDetector()->GetT0s()), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'time_zero_bin' in NXdetector.")) return NX_ERROR;
      NXclosedata(fFileHandle);
    } else {
      cerr << endl << ">> **WARNING** time_zero_bin with rank " << fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() << " requested. Do not know how to handle." << endl;
    }
  }

  // write detector_1 'first_good_bin' if present
  if (fNxEntry2->GetInstrument()->GetDetector()->IsFirstGoodBinPresent()) {
    if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 1) {
      size = 1;
      idata = fNxEntry2->GetInstrument()->GetDetector()->GetFirstGoodBin();
      if (!ErrorHandler(NXmakedata(fFileHandle, "first_good_bin", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'first_good_bin' in NXdetector.")) return NX_ERROR;
      if (!ErrorHandler(NXopendata(fFileHandle, "first_good_bin"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'first_good_bin' in NXdetector for writting.")) return NX_ERROR;
      if (!ErrorHandler(NXputdata(fFileHandle, (void*)&idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'first_good_bin' in NXdetector.")) return NX_ERROR;
      NXclosedata(fFileHandle);
    } else if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 2) {
      int dims[1];
      dims[0] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra();
      if (!ErrorHandler(NXmakedata(fFileHandle, "first_good_bin", NX_INT32, 1, dims), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'first_good_bin' in NXdetector.")) return NX_ERROR;
      if (!ErrorHandler(NXopendata(fFileHandle, "first_good_bin"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'first_good_bin' in NXdetector for writting.")) return NX_ERROR;
      if (!ErrorHandler(NXputdata(fFileHandle, (void*)fNxEntry2->GetInstrument()->GetDetector()->GetFirstGoodBins()), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'first_good_bin' in NXdetector.")) return NX_ERROR;
      NXclosedata(fFileHandle);
    } else if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 3) {
      int dims[2];
      dims[0] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfPeriods();
      dims[1] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra();
      if (!ErrorHandler(NXmakedata(fFileHandle, "first_good_bin", NX_INT32, 2, dims), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'first_good_bin' in NXdetector.")) return NX_ERROR;
      if (!ErrorHandler(NXopendata(fFileHandle, "first_good_bin"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'first_good_bin' in NXdetector for writting.")) return NX_ERROR;
      if (!ErrorHandler(NXputdata(fFileHandle, (void*)fNxEntry2->GetInstrument()->GetDetector()->GetFirstGoodBins()), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'first_good_bin' in NXdetector.")) return NX_ERROR;
      NXclosedata(fFileHandle);
    } else {
      cerr << endl << ">> **WARNING** first_good_bin with rank " << fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() << " requested. Do not know how to handle." << endl;
    }
  }

  // write detector_1 'last_good_bin' if present
  if (fNxEntry2->GetInstrument()->GetDetector()->IsLastGoodBinPresent()) {
    if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 1) {
      size = 1;
      idata = fNxEntry2->GetInstrument()->GetDetector()->GetLastGoodBin();
      if (!ErrorHandler(NXmakedata(fFileHandle, "last_good_bin", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'last_good_bin' in NXdetector.")) return NX_ERROR;
      if (!ErrorHandler(NXopendata(fFileHandle, "last_good_bin"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'last_good_bin' in NXdetector for writting.")) return NX_ERROR;
      if (!ErrorHandler(NXputdata(fFileHandle, (void*)&idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'last_good_bin' in NXdetector.")) return NX_ERROR;
      NXclosedata(fFileHandle);
    } else if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 2) {
      int dims[1];
      dims[0] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra();
      if (!ErrorHandler(NXmakedata(fFileHandle, "last_good_bin", NX_INT32, 1, dims), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'last_good_bin' in NXdetector.")) return NX_ERROR;
      if (!ErrorHandler(NXopendata(fFileHandle, "last_good_bin"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'last_good_bin' in NXdetector for writting.")) return NX_ERROR;
      if (!ErrorHandler(NXputdata(fFileHandle, (void*)fNxEntry2->GetInstrument()->GetDetector()->GetLastGoodBins()), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'last_good_bin' in NXdetector.")) return NX_ERROR;
      NXclosedata(fFileHandle);
    } else if (fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() == 3) {
      int dims[2];
      dims[0] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfPeriods();
      dims[1] = fNxEntry2->GetInstrument()->GetDetector()->GetNoOfSpectra();
      if (!ErrorHandler(NXmakedata(fFileHandle, "last_good_bin", NX_INT32, 2, dims), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'last_good_bin' in NXdetector.")) return NX_ERROR;
      if (!ErrorHandler(NXopendata(fFileHandle, "last_good_bin"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'last_good_bin' in NXdetector for writting.")) return NX_ERROR;
      if (!ErrorHandler(NXputdata(fFileHandle, (void*)fNxEntry2->GetInstrument()->GetDetector()->GetLastGoodBins()), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'last_good_bin' in NXdetector.")) return NX_ERROR;
      NXclosedata(fFileHandle);
    } else {
      cerr << endl << ">> **WARNING** last_good_bin with rank " << fNxEntry2->GetInstrument()->GetDetector()->GetT0Tag() << " requested. Do not know how to handle." << endl;
    }
  }

  // close group 'detector_1'
  NXclosegroup(fFileHandle);

  // close group 'instrument'
  NXclosegroup(fFileHandle);

  // make group 'detector_1' NXdata
  if (!ErrorHandler(NXmakegroup(fFileHandle, "detector_1", "NXdata"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'detector_1' in NXroot.")) return NX_ERROR;
  // open group 'detector_1' NXdata
  if (!ErrorHandler(NXopengroup(fFileHandle, "detector_1", "NXdata"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'detector_1' in NXroot.")) return NX_ERROR;

  for (unsigned int i=0; i<nxLinkVec.size(); i++) {
    str = string("couldn't create link to ") + string(nxLinkVec[i].targetPath);
    if (!ErrorHandler(NXmakelink(fFileHandle, &nxLinkVec[i]), PNEXUS_LINKING_ERROR, str.c_str())) return NX_ERROR;
  }

  // close group 'detector_1' NXdata
  NXclosegroup(fFileHandle);

  // close group 'raw_data_1'
  NXclosegroup(fFileHandle);

  NXclose(&fFileHandle);

  // clean up
  nxLinkVec.clear();

  return NX_OK;
}

//------------------------------------------------------------------------------------------
// IsValidIdf1 (private)
//------------------------------------------------------------------------------------------
/**
 * <p>Checks if the given data are representing a valid IDF Version 1.
 *
 * <b>return:</b>
 * - true if a valid IDF Version 1 representation is present
 * - false otherwise
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXus::IsValidIdf1(bool strict)
{
  bool valid = true;

  if (fIdfVersion != 1) {
    cerr << endl << ">> **ERROR** wrong IDF version found, namely IDF " << fIdfVersion << ", instead of IDF 1" << endl;
    return false;
  }

  if (!fNxEntry1->IsValid(strict))
    valid = false;

  return valid;
}

//------------------------------------------------------------------------------------------
// IsValidIdf2 (private)
//------------------------------------------------------------------------------------------
/**
 * <p>Checks if the given data are representing a valid IDF Version 2.
 *
 * <b>return:</b>
 * - true if a valid IDF Version 2 representation is present
 * - false otherwise
 *
 * \param strict flag if true a strict NeXus validation is performed.
 */
bool PNeXus::IsValidIdf2(bool strict)
{
  if (fIdfVersion != 2) {
    cerr << endl << ">> **ERROR** wrong IDF version found, namely IDF " << fIdfVersion << ", instead of IDF 2" << endl;
    return false;
  }

  if (!fCreator.compare("n/a")) {
    if (strict) {
      cerr << endl << ">> **ERROR** creator not set." << endl;
      return false;
    } else {
      cerr << endl << ">> **WARNING** creator not set." << endl;
    }
  }

  if (!fFileName.compare("n/a")) {
    if (strict) {
      cerr << endl << ">> **ERROR** file name not given." << endl;
      return false;
    } else {
      cerr << endl << ">> **WARNING** file name not given." << endl;
    }
  }

  if (!fFileTime.compare("n/a")) {
    if (strict) {
      cerr << endl << ">> **ERROR** no file creation time is given." << endl;
      return false;
    } else {
      cerr << endl << ">> **WARNING** no file creation time is given." << endl;
    }
  }

  if (!fNxEntry2->IsValid(strict)) {
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------------------
// SearchInGroup (private)
//------------------------------------------------------------------------------------------
/**
 * <p>Searches an entry (labelled by str) in the currently open group. If the tag=='name'
 * a label is looked for, if the tag=='class' a class is looked for.
 *
 * <b>return:</b>
 * - true if entry 'str' is found in the currently open group. nxname, nxclass keeps than the entry information.
 * - false otherwise
 *
 * \param str label/class of the entry to be looked for
 * \param tag either 'name'  or 'class'
 * \param nxname 'name' of the entry if entry is found
 * \param nxclass 'class name' of the entry if entry is found
 * \param dataType of the entry if entry is found
 */
bool PNeXus::SearchInGroup(string str, string tag, NXname &nxname, NXname &nxclass, int &dataType)
{
  bool found = false;
  int status;

  NXinitgroupdir(fFileHandle);
  do {
    status = NXgetnextentry(fFileHandle, nxname, nxclass, &dataType);
    if (!tag.compare("name")) {
      if (!str.compare(nxname)) {
        found = true;
        break;
      }
    } else if (!tag.compare("class")) {
      if (!str.compare(nxclass)) {
        found = true;
        break;
      }
    } else {
      cerr << endl << ">> **ERROR** found tag='" << tag << "' which is not handled!" << endl;
      break;
    }
  } while (!found && (status == NX_OK));

  return found;
}

//------------------------------------------------------------------------------------------
// SearchAttrInData (private)
//------------------------------------------------------------------------------------------
/**
 * <p>Searches an attribute (labelled by str) in the currently open data set.
 *
 * <b>return:</b>
 * - true if attribute 'str' is found in the currently open data set. nxname, nxclass keeps than the entry information.
 * - false otherwise
 *
 * \param str label of the attribute to be looked for
 * \param length of the attribute data it entry is found
 * \param dataType of the entry if entry is found
 */
bool PNeXus::SearchAttrInData(string str, int &length, int &dataType)
{
  bool found = false;
  int status;
  char name[128];

  memset(name, 0, sizeof(name));

  NXinitattrdir(fFileHandle);
  do {
    status = NXgetnextattr(fFileHandle, name, &length, &dataType);
    if (!str.compare(name)) {
      found = true;
      break;
    }
  } while (!found && (status == NX_OK));

  return found;
}
