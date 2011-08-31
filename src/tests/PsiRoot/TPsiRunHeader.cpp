/***************************************************************************

  TPsiRunHeader.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2011 by Andreas Suter                              *
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

#include <ctime>
#include <iostream>
#include <iomanip>
using namespace std;

#include "TPsiRunHeader.h"

#include <TPaveText.h>
#include <TCanvas.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>

ClassImp(TPsiRunProperty)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 */
TPsiRunProperty::TPsiRunProperty()
{
  fLabel = "n/a";
  fDemand = 0.0;
  fValue = 0.0;
  fError = 0.0;
  fUnit  = "n/a";
  fDescription = "n/a";
  fPath = "n/a";
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param name
 * \param value
 * \param error
 * \param unit
 */
TPsiRunProperty::TPsiRunProperty(TString &label, Double_t demand, Double_t value, Double_t error, TString &unit, TString &description, TString &path) :
    fLabel(label), fDemand(demand), fValue(value), fError(error), fUnit(unit)
{
  if (description.IsWhitespace())
    fDescription = "n/a";
  else
    fDescription = description;

  fPath = path;
  if (path.IsWhitespace())
    fPath = "/";
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImp(TPsiRunHeader)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
  * <p>Constructor.
  */
TPsiRunHeader::TPsiRunHeader()
{
  fVersion     = TString("$Id$");
  fGenerator   = TString("n/a");
  fFileName    = TString("n/a");
  fRunTitle    = TString("n/a");
  fRunNumber   = -1;
  fStartTime   = TDatime("1995-01-01 00:00:00");
  fStopTime    = TDatime("1995-01-01 00:00:00");
  fLaboratory  = TString("n/a");
  fArea        = TString("n/a");
  fInstrument  = TString("n/a");
  fMuonSpecies = TString("n/a");
  fSetup       = TString("n/a");
  fComment     = TString("n/a");
  fSample      = TString("n/a");
  fOrientation = TString("n/a");
  fSampleCryo  = TString("n/a");
  fSampleCryoInsert = TString("n/a");
  fMagnetName  = TString("n/a");
  fNoOfHistos  = -1;
  fHistoLength = -1;
  fTimeResolution = 0.0;

  fHeader.Expand(0);      // init to size 0
  fSampleEnv.Expand(0);   // init to size 0
  fMagFieldEnv.Expand(0); // init to size 0
  fBeamline.Expand(0);    // init to size 0
  fScalers.Expand(0);     // init to size 0
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
  * <p>Destructor.
  */
TPsiRunHeader::~TPsiRunHeader()
{
  fHistoName.clear();
  fTimeZeroBin.clear();
  fFirstGoodBin.clear();
  fLastGoodBin.clear();
  fRedGreenOffset.clear();
  fRedGreenDescription.clear();
  fProperties.clear();
  fHeader.Delete();
  fSampleEnv.Delete();
  fMagFieldEnv.Delete();
  fBeamline.Delete();
  fScalers.Delete();
}

//--------------------------------------------------------------------------
// IsValid (public)
//--------------------------------------------------------------------------
/**
 * <p>validates the currently set header information. If strict is set to true
 * a strict validation is carried out, otherwise a more sloppy one.
 *
 * <p><b>return:</b>
 * - true, if valid header information are present.
 * - false, otherwise
 *
 * \param strict flag needed to tell on which level the validation has to be carried out.
 */
Bool_t TPsiRunHeader::IsValid(Bool_t strict)
{
  TString startTime = TString(fStartTime.AsSQLString());
  TString stopTime  = TString(fStopTime.AsSQLString());

  if (strict) {
    if (!fGenerator.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Generator' not set." << endl;
      return false;
    } else if (!fFileName.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'File Name' not set." << endl;
      return false;
    } else if (!fRunTitle.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Run Title' not set." << endl;
      return false;
    } else if (fRunNumber == -1) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Run Number' not set." << endl;
      return false;
    } else if (!startTime.CompareTo("1995-01-01 00:00:00", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Run Start Times' not set." << endl;
      return false;
    } else if (!stopTime.CompareTo("1995-01-01 00:00:00", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Run Stop Times' not set." << endl;
      return false;
    } else if (!fLaboratory.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Laboratory' not set." << endl;
      return false;
    } else if (!fArea.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Area' not set." << endl;
      return false;
    } else if (!fInstrument.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Instrument' not set." << endl;
      return false;
    } else if (!fMuonSpecies.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Muon Species' not set." << endl;
      return false;
    } else if (!fSetup.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Setup' not set." << endl;
      return false;
    } else if (!fComment.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Comment' not set." << endl;
      return false;
    } else if (!fSample.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Sample' not set." << endl;
      return false;
    } else if (!fOrientation.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'SampleEnv/Orientation' not set." << endl;
      return false;
    } else if (!fSampleCryo.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'SampleEnv/Cryo' not set." << endl;
      return false;
    } else if (!fSampleCryoInsert.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'SampleEnv/Insert' not set." << endl;
      return false;
    } else if (!fMagnetName.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'MagFieldEnv/Name' not set." << endl;
      return false;
    } else if (fNoOfHistos == -1) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'No of Histos' not set." << endl;
      return false;
    } else if (fHistoName.size() == 0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Histo Names' not set." << endl;
      return false;
    } else if ((Int_t)fHistoName.size() != fNoOfHistos) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): No of 'Histo Names' != 'No of Histos'!" << endl;
      return false;
    } else if (fHistoLength == -1) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Histo Length' not set." << endl;
      return false;
    } else if (fTimeResolution == 0.0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Time Resolution' not set." << endl;
      return false;
    } else if (fTimeZeroBin.size() == 0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Time Zero Bin' not set." << endl;
      return false;
    } else if ((Int_t)fTimeZeroBin.size() != fNoOfHistos) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): No of 'Time Zero Bin' != 'No Of Histos'." << endl;
      return false;
    } else if (fFirstGoodBin.size() == 0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'First Good Bin' not set." << endl;
      return false;
    } else if ((Int_t)fFirstGoodBin.size() != fNoOfHistos) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): No of 'First Good Bin' != 'No Of Histos'." << endl;
      return false;
    } else if (fLastGoodBin.size() == 0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Last Good Bin' not set." << endl;
      return false;
    } else if ((Int_t)fLastGoodBin.size() != fNoOfHistos) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): No of 'Last Good Bin' != 'No Of Histos'." << endl;
      return false;
    } else if (fRedGreenOffset.size() != fRedGreenDescription.size()) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): No of 'Red/Green offsets' != 'Red/Green descriptions'." << endl;
      return false;
    }

    // check all the required physical properties

    // check 'Sample Temperature'
    if (GetProperty("Muon Beam Momentum") == 0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Muon Beam Momentum' not set." << endl;
      return false;
    } else if (GetProperty("Sample Temperature") == 0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Sample Temperature' not set." << endl;
      return false;
    } else if (GetProperty("Sample Magnetic Field") == 0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Sample Magnetic Field' not set." << endl;
      return false;
    } else if (GetProperty("Current") == 0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'MagFieldEnv/Current' not set." << endl;
      return false;
    }
  } else { // not quite so strict
    if (!fGenerator.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'Generator' not set." << endl;
    }
    if (!fFileName.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'File Name' not set." << endl;
      return false;
    }
    if (!fRunTitle.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Run Title' not set." << endl;
      return false;
    }
    if (fRunNumber == -1) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Run Number' not set." << endl;
      return false;
    }
    if (!startTime.CompareTo("1995-01-01 00:00:00", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Run Start Times' not set." << endl;
      return false;
    }
    if (!stopTime.CompareTo("1995-01-01 00:00:00", TString::kIgnoreCase)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Run Stop Times' not set." << endl;
      return false;
    }
    if (!fLaboratory.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'Laboratory' not set." << endl;
    }
    if (!fArea.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'Area' not set." << endl;
    }
    if (!fInstrument.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'Instrument' not set." << endl;
    }
    if (!fMuonSpecies.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'Muon Species' not set." << endl;
    }
    if (!fSetup.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'Setup' not set." << endl;
    }
    if (!fComment.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'Comment' not set." << endl;
    }
    if (!fSample.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'Sample' not set." << endl;
    }
    if (!fOrientation.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'SampleEnv/Orientation' not set." << endl;
    }
    if (!fSampleCryo.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'SampleEnv/Cryo' not set." << endl;
    }
    if (!fSampleCryoInsert.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'SampleEnv/Insert' not set." << endl;
    }
    if (!fMagnetName.CompareTo("n/a", TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'MagFieldEnv/Name' not set." << endl;
    }
    if (fNoOfHistos == -1) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'No of Histos' not set." << endl;
      return false;
    }
    if (fHistoName.size() == 0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Histo Names' not set." << endl;
      return false;
    }
    if ((Int_t)fHistoName.size() != fNoOfHistos) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): No of 'Histo Names' != 'No of Histos'!" << endl;
      return false;
    }
    if (fHistoLength == -1) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Histo Length' not set." << endl;
      return false;
    }
    if (fTimeResolution == 0.0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Time Resolution' not set." << endl;
      return false;
    }
    if (fTimeZeroBin.size() == 0) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'Time Zero Bin' not set." << endl;
    }
    if ((fTimeZeroBin.size() > 0) && ((Int_t)fTimeZeroBin.size() != fNoOfHistos)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): No of 'Time Zero Bin' != 'No Of Histos'." << endl;
      return false;
    }
    if (fFirstGoodBin.size() == 0) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'First Good Bin' not set." << endl;
    }
    if ((fFirstGoodBin.size() > 0) && ((Int_t)fFirstGoodBin.size() != fNoOfHistos)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): No of 'First Good Bin' != 'No Of Histos'." << endl;
      return false;
    }
    if (fLastGoodBin.size() == 0) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'Last Good Bin' not set." << endl;
    }
    if ((fLastGoodBin.size() > 0) && ((Int_t)fLastGoodBin.size() != fNoOfHistos)) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): No of 'Last Good Bin' != 'No Of Histos'." << endl;
      return false;
    }
    if (fRedGreenOffset.size() != fRedGreenDescription.size()) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): No of 'Red/Green offsets' != 'Red/Green descriptions'." << endl;
      return false;
    }

    // check all the required physical properties

    // check 'Sample Temperature'
    if (GetProperty("Muon Beam Momentum") == 0) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'Muon Beam Momentum' not set." << endl;
    }
    if (GetProperty("Sample Temperature") == 0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Sample Temperature' not set." << endl;
      return false;
    }
    if (GetProperty("Sample Magnetic Field") == 0) {
      cerr << endl << ">> **ERROR** TPsiRunHeader::IsValid(): 'Sample Magnetic Field' not set." << endl;
      return false;
    }
    if (GetProperty("Current") == 0) {
      cerr << endl << ">> **WARNING** TPsiRunHeader::IsValid(): 'MagFieldEnv/Current' not set." << endl;
    }
  }

  return true;
}

//--------------------------------------------------------------------------
// GetHistoName (public)
//--------------------------------------------------------------------------
/**
 * <p>returns the histogram name at index 'idx'. If 'idx' is out of range,
 * an empty string is returned and 'ok' is set to false.
 *
 * \param idx index for which the histogram name is whished
 * \param ok true, if a valid name was found, false otherwise
 */
TString TPsiRunHeader::GetHistoName(UInt_t idx, Bool_t &ok) const
{
  if (idx >= fHistoName.size()) {
    ok = false;
    return TString("");
  }

  ok = true;
  return fHistoName[idx];
}

//--------------------------------------------------------------------------
// GetTimeResolution (public)
//--------------------------------------------------------------------------
/**
 * <p>returns the time resolution in the requested 'units'. Allowed 'units'
 * are: fs, ps, ns, us
 *
 * \param units in which the time resolution shall be returned.
 */
Double_t TPsiRunHeader::GetTimeResolution(const char *units) const
{
  Double_t factor = 1.0;
  TString str(units);

  if (!str.CompareTo("fs", TString::kIgnoreCase))
    factor = 1.0e3;
  else if (!str.CompareTo("ps", TString::kIgnoreCase))
    factor = 1.0;
  else if (!str.CompareTo("ns", TString::kIgnoreCase))
    factor = 1.0e-3;
  else if (!str.CompareTo("us", TString::kIgnoreCase))
    factor = 1.0e-6;
  else
    factor = 0.0;

  return factor*fTimeResolution;
}

//--------------------------------------------------------------------------
// GetTimeZeroBin (public)
//--------------------------------------------------------------------------
/**
 * <p>returns the time zero bin of index 'idx'. If 'idx' is out of range,
 * 'ok' is set to false.
 *
 * \param idx index for which time zero bin shall be returned
 * \param ok flag showing if the returned value is valid
 */
UInt_t TPsiRunHeader::GetTimeZeroBin(UInt_t idx, Bool_t &ok) const
{
  if (idx >= fTimeZeroBin.size()) {
    ok = false;
    return 0;
  }

  ok = true;
  return fTimeZeroBin[idx];
}

//--------------------------------------------------------------------------
// GetFirstGoodBin (public)
//--------------------------------------------------------------------------
/**
 * <p>returns the first good bin of index 'idx'. If 'idx' is out of range,
 * 'ok' is set to false.
 *
 * \param idx index for which first good bin shall be returned
 * \param ok flag showing if the returned value is valid
 */
UInt_t TPsiRunHeader::GetFirstGoodBin(UInt_t idx, Bool_t &ok) const
{
  if (idx >= fFirstGoodBin.size()) {
    ok = false;
    return 0;
  }

  ok = true;
  return fFirstGoodBin[idx];
}

//--------------------------------------------------------------------------
// GetLastGoodBin (public)
//--------------------------------------------------------------------------
/**
 * <p>returns the last good bin of index 'idx'. If 'idx' is out of range,
 * 'ok' is set to false.
 *
 * \param idx index for which last good bin shall be returned
 * \param ok flag showing if the returned value is valid
 */
UInt_t TPsiRunHeader::GetLastGoodBin(UInt_t idx, Bool_t &ok) const
{
  if (idx >= fLastGoodBin.size()) {
    ok = false;
    return 0;
  }

  ok = true;
  return fLastGoodBin[idx];
}

//--------------------------------------------------------------------------
// GetRedGreenHistoOffset (public)
//--------------------------------------------------------------------------
/**
 * <p>returns the red/green mode histogram offset at index 'idx'. If 'idx' is out of range,
 * 'ok' is set to false.
 *
 * \param idx index for which red/green mode histogram offset shall be returned
 * \param ok flag showing if the returned value is valid
 */
UInt_t TPsiRunHeader::GetRedGreenHistoOffset(UInt_t idx, Bool_t &ok) const
{
  if (idx >= fRedGreenOffset.size()) {
    ok = false;
    return 0;
  }

  ok = true;
  return fRedGreenOffset[idx];
}

//--------------------------------------------------------------------------
// GetRedGreenHistoDescription (public)
//--------------------------------------------------------------------------
/**
 * <p>returns the red/green mode description at index 'idx'. If 'idx' is out of range,
 * 'ok' is set to false.
 *
 * \param idx index for which red/green mode description shall be returned
 * \param ok flag showing if the returned value is valid
 */
TString TPsiRunHeader::GetRedGreenHistoDescription(UInt_t idx, Bool_t &ok) const
{
  if (idx >= fRedGreenDescription.size()) {
    ok = false;
    return TString("");
  }

  ok = true;
  return fRedGreenDescription[idx];
}

//--------------------------------------------------------------------------
// GetProperty (public)
//--------------------------------------------------------------------------
/**
 * <p>Searches the property given by 'name'. If it is found, this property is
 * returned, otherwise 0 is returned.
 *
 * \param name property name to look for.
 */
const TPsiRunProperty* TPsiRunHeader::GetProperty(TString name) const
{
  UInt_t i=0;

  for (i=0; i<fProperties.size(); i++) {
    if (!fProperties[i].GetLabel().CompareTo(name, TString::kIgnoreCase))
      break;
  }

  if (i<fProperties.size())
    return &fProperties[i];
  else
    return 0;
}

//--------------------------------------------------------------------------
// GetHeader (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  */
TObjArray* TPsiRunHeader::GetHeader(UInt_t &count)
{
  // make sure that previous header is removed
  fHeader.Delete();
  fHeader.Expand(0);

  TString str, fmt;
  TObjString *tostr;
  const TPsiRunProperty *prop;
  UInt_t digit=0, digit_d=0;

  // add version
  str.Form("%03d - Version: %s", count++, fVersion.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add generator
  str.Form("%03d - Generator: %s", count++, fGenerator.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add file name
  str.Form("%03d - File Name: %s", count++, fFileName.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add run title
  str.Form("%03d - Run Title: %s", count++, fRunTitle.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add run number
  str.Form("%03d - Run Number: %d", count++, fRunNumber);
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add run start time
  str.Form("%03d - Run Start Time: %s", count++, fStartTime.AsSQLString());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add run stop time
  str.Form("%03d - Run Stop Time: %s", count++, fStopTime.AsSQLString());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add laboratory
  str.Form("%03d - Laboratory: %s", count++, fLaboratory.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add area
  str.Form("%03d - Area: %s", count++, fArea.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add instrument
  str.Form("%03d - Instrument: %s", count++, fInstrument.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add muon momentum
  prop = GetProperty("Muon Beam Momentum");
  if (prop) {
    digit = GetDecimalPlace(prop->GetError());
    digit_d = GetLeastSignificantDigit(prop->GetDemand());
    if (prop->GetDescription().CompareTo("n/a")) {
      fmt.Form("%%03d - %%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit, digit_d);
      str.Form(fmt.Data(), count++, prop->GetLabel().Data(), prop->GetValue(), prop->GetError(),
               prop->GetUnit().Data(), prop->GetDemand(), prop->GetDescription().Data());
    } else {
      fmt.Form("%%03d - %%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf", digit, digit, digit_d);
      str.Form(fmt.Data(), count++, prop->GetLabel().Data(), prop->GetValue(), prop->GetError(),
               prop->GetUnit().Data(), prop->GetDemand());
    }
    tostr = new TObjString(str);
    fHeader.AddLast(tostr);
  }

  // add muon species
  str.Form("%03d - Muon Species: %s", count++, fMuonSpecies.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);


  // add setup
  str.Form("%03d - Setup: %s", count++, fSetup.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add comment
  str.Form("%03d - Comment: %s", count++, fComment.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add sample
  str.Form("%03d - Sample: %s", count++, fSample.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add sample temperature
  prop = GetProperty("Sample Temperature");
  if (prop) {
    digit = GetDecimalPlace(prop->GetError());
    digit_d = GetLeastSignificantDigit(prop->GetDemand());
    if (prop->GetDescription().CompareTo("n/a")) {
      fmt.Form("%%03d - %%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit, digit_d);
      str.Form(fmt.Data(), count++, prop->GetLabel().Data(), prop->GetValue(), prop->GetError(),
               prop->GetUnit().Data(), prop->GetDemand(), prop->GetDescription().Data());
    } else {
      fmt.Form("%%03d - %%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf", digit, digit, digit_d);
      str.Form(fmt.Data(), count++, prop->GetLabel().Data(), prop->GetValue(), prop->GetError(),
               prop->GetUnit().Data(), prop->GetDemand());
    }
    tostr = new TObjString(str);
    fHeader.AddLast(tostr);
  }

  // add sample magnetic field
  prop = GetProperty("Sample Magnetic Field");
  if (prop) {
    digit = GetDecimalPlace(prop->GetError());
    digit_d = GetLeastSignificantDigit(prop->GetDemand());
    if (prop->GetDescription().CompareTo("n/a")) {
      fmt.Form("%%03d - %%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit, digit_d);
      str.Form(fmt.Data(), count++, prop->GetLabel().Data(), prop->GetValue(), prop->GetError(),
               prop->GetUnit().Data(), prop->GetDemand(), prop->GetDescription().Data());
    } else {
      fmt.Form("%%03d - %%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf", digit, digit, digit_d);
      str.Form(fmt.Data(), count++, prop->GetLabel().Data(), prop->GetValue(), prop->GetError(),
               prop->GetUnit().Data(), prop->GetDemand());
    }
    tostr = new TObjString(str);
    fHeader.AddLast(tostr);
  }

  // add number of histograms
  str.Form("%03d - No of Histos: %d", count++, fNoOfHistos);
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add histogram names
  str.Form("%03d - Histo Names: ", count++);
  for (UInt_t i=0; i<fHistoName.size()-1; i++)
    str += fHistoName[i] + TString("; ");
  str += fHistoName[fHistoName.size()-1];
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add histogram length
  str.Form("%03d - Histo Length: %d", count++, fHistoLength);
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add time resolution
  digit_d = GetLeastSignificantDigit(GetTimeResolution("ns"));
  fmt.Form("%%03d - Time Resolution: %%.%dlf ns", digit_d);
  str.Form(fmt, count++, GetTimeResolution("ns"));
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add time zero bins
  str.Form("%03d - Time Zero Bin: ", count++);
  for (UInt_t i=0; i<fTimeZeroBin.size()-1; i++) {
    str += fTimeZeroBin[i];
    str += TString("; ");
  }
  str += fTimeZeroBin[fTimeZeroBin.size()-1];
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add first good bins
  str.Form("%03d - First Good Bin: ", count++);
  for (UInt_t i=0; i<fFirstGoodBin.size()-1; i++) {
    str += fFirstGoodBin[i];
    str += TString("; ");
  }
  str += fFirstGoodBin[fFirstGoodBin.size()-1];
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add last good bins
  str.Form("%03d - Last Good Bin: ", count++);
  for (UInt_t i=0; i<fLastGoodBin.size()-1; i++) {
    str += fLastGoodBin[i];
    str += TString("; ");
  }
  str += fLastGoodBin[fLastGoodBin.size()-1];
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add red/green mode histogram offsets if present
  if (fRedGreenOffset.size() > 0) {
    str.Form("%03d - Red/Green offsets: ", count++);
    for (UInt_t i=0; i<fRedGreenOffset.size()-1; i++) {
      str += fRedGreenOffset[i];
      str += TString("; ");
    }
    str += fRedGreenOffset[fRedGreenOffset.size()-1];
    tostr = new TObjString(str);
    fHeader.AddLast(tostr);
  }

  // add red/green mode descriptions if present
  if (fRedGreenDescription.size() > 0) {
    str.Form("%03d - Red/Green description: ", count++);
    for (UInt_t i=0; i<fRedGreenDescription.size()-1; i++) {
      str += fRedGreenDescription[i] + TString("; ");
    }
    str += fRedGreenDescription[fRedGreenDescription.size()-1];
    tostr = new TObjString(str);
    fHeader.AddLast(tostr);
  }

  fHeader.SetName("RunHeader");

  return &fHeader;
}

//--------------------------------------------------------------------------
// GetSampleEnv (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  */
TObjArray* TPsiRunHeader::GetSampleEnv(UInt_t &count)
{
  // make sure that previous sample environment info is removed
  fSampleEnv.Delete();
  fSampleEnv.Expand(0);

  TString str, fmt;
  TObjString *tostr;
  UInt_t digit=0, digit_d=0;

  // add cryo
  str.Form("%03d - Cryo: %s", count++, fSampleCryo.Data());
  tostr = new TObjString(str);
  fSampleEnv.AddLast(tostr);

  // add insert
  str.Form("%03d - Insert: %s", count++, fSampleCryoInsert.Data());
  tostr = new TObjString(str);
  fSampleEnv.AddLast(tostr);

  // add orientation
  str.Form("%03d - Orientation: %s", count++, fOrientation.Data());
  tostr = new TObjString(str);
  fSampleEnv.AddLast(tostr);

  // check if there are additional physical properties present
  for (UInt_t i=0; i<fProperties.size(); i++) {
    if (!fProperties[i].GetPath().CompareTo("/SampleEnv/", TString::kIgnoreCase)) {
      digit = GetDecimalPlace(fProperties[i].GetError());
      digit_d = GetLeastSignificantDigit(fProperties[i].GetDemand());
      if (fProperties[i].GetDescription().CompareTo("n/a")) {
        fmt.Form("%%03d - %%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit, digit_d);
        str.Form(fmt.Data(), count++, fProperties[i].GetLabel().Data(), fProperties[i].GetValue(), fProperties[i].GetError(),
                 fProperties[i].GetUnit().Data(), fProperties[i].GetDemand(), fProperties[i].GetDescription().Data());
      } else {
        fmt.Form("%%03d - %%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf", digit, digit, digit_d);
        str.Form(fmt.Data(), count++, fProperties[i].GetLabel().Data(), fProperties[i].GetValue(), fProperties[i].GetError(),
                 fProperties[i].GetUnit().Data(), fProperties[i].GetDemand());
      }
      tostr = new TObjString(str);
      fSampleEnv.AddLast(tostr);
    }
  }

  fSampleEnv.SetName("SampleEnv");

  return &fSampleEnv;
}

//--------------------------------------------------------------------------
// GetMagFieldEnv (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  */
TObjArray* TPsiRunHeader::GetMagFieldEnv(UInt_t &count)
{
  // make sure that previous sample magnetic field environment info is removed
  fMagFieldEnv.Delete();
  fMagFieldEnv.Expand(0);

  TString str, fmt;
  TObjString *tostr;
  UInt_t digit=0, digit_d=0;

  // add version
  str.Form("%03d - Magnet Name: %s", count++, fMagnetName.Data());
  tostr = new TObjString(str);
  fMagFieldEnv.AddLast(tostr);

  // check if there are additional physical properties present
  for (UInt_t i=0; i<fProperties.size(); i++) {
    if (!fProperties[i].GetPath().CompareTo("/MagFieldEnv/", TString::kIgnoreCase)) {
      digit = GetDecimalPlace(fProperties[i].GetError());
      digit_d = GetLeastSignificantDigit(fProperties[i].GetDemand());
      if (fProperties[i].GetDescription().CompareTo("n/a")) {
        fmt.Form("%%03d - %%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit, digit_d);
        str.Form(fmt.Data(), count++, fProperties[i].GetLabel().Data(), fProperties[i].GetValue(), fProperties[i].GetError(),
                 fProperties[i].GetUnit().Data(), fProperties[i].GetDemand(), fProperties[i].GetDescription().Data());
      } else {
        fmt.Form("%%03d - %%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf", digit, digit, digit_d);
        str.Form(fmt.Data(), count++, fProperties[i].GetLabel().Data(), fProperties[i].GetValue(), fProperties[i].GetError(),
                 fProperties[i].GetUnit().Data(), fProperties[i].GetDemand());
      }
      tostr = new TObjString(str);
      fMagFieldEnv.AddLast(tostr);
    }
  }

  fMagFieldEnv.SetName("MagFieldEnv");

  return &fMagFieldEnv;
}

//--------------------------------------------------------------------------
// GetBeamline (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  */
TObjArray* TPsiRunHeader::GetBeamline(UInt_t &count)
{
  // make sure that previous beamline info is removed
  fBeamline.Delete();
  fBeamline.Expand(0);

  TString str, fmt;
  TObjString *tostr;

  // add version
  str.Form("%03d - To Be Defined Yet.", count++);
  tostr = new TObjString(str);
  fBeamline.AddLast(tostr);

  fBeamline.SetName("Beamline");

  return &fBeamline;
}

//--------------------------------------------------------------------------
// GetScaler (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  */
TObjArray* TPsiRunHeader::GetScaler(UInt_t &count)
{
  // make sure that previous scaler info is removed
  fScalers.Delete();
  fScalers.Expand(0);

  TString str, fmt;
  TObjString *tostr;

  // add version
  str.Form("%03d - To Be Defined Yet.", count++);
  tostr = new TObjString(str);
  fScalers.AddLast(tostr);

  fScalers.SetName("Scalers");

  return &fScalers;
}

//--------------------------------------------------------------------------
// ExtractHeaderInformation (public)
//--------------------------------------------------------------------------
/**
  * <p> Extracts from an array of TObjStrings containing the header information
  * all the necessary parameters.
  *
  * \param headerInfo an array of TObjStrings containing the header information
  * \param path
  */
Bool_t TPsiRunHeader::ExtractHeaderInformation(TObjArray *headerInfo, TString path)
{
  // check if there is an object pointer is present
  if (headerInfo == 0) {
    cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** headerInfo object pointer is 0" << endl << endl;
    return false;
  }

  // start extracting entries
  TObjArray *tokens, *tokens1;
  TObjString *ostr;
  TString str("");
  Int_t idx;
  Double_t dval;

  // not TPsiRunProperty header variables
  for (Int_t i=0; i<headerInfo->GetEntries(); i++) {
    ostr = dynamic_cast<TObjString*>(headerInfo->At(i));
    str = ostr->GetString();

    if (str.Contains("- Version: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fVersion = str;
    } else if (str.Contains("- Generator: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fGenerator = str;
    } else if (str.Contains("- File Name: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fFileName = str;
    } else if (str.Contains("- Run Title: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fRunTitle = str;
    } else if (str.Contains("- Run Number: ")) {
      tokens = str.Tokenize(":");
      if (tokens->GetEntries() < 2) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " couldn't tokenize it." << endl << endl;
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      if (!ostr->GetString().IsDigit()) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " doesn't contain a valid run number" << endl << endl;
        return false;
      }
      fRunNumber = ostr->GetString().Atoi();
      // clean up
      CleanUp(tokens);
    } else if (str.Contains("- Run Start Time: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fStartTime = TDatime(str);
    } else if (str.Contains("- Run Stop Time: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fStopTime = TDatime(str);
    } else if (str.Contains("- Laboratory: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fLaboratory = str;
    } else if (str.Contains("- Area: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fArea = str;
    } else if (str.Contains("- Instrument: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fInstrument = str;
    } else if (str.Contains("- Muon Species: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fMuonSpecies = str;
    } else if (str.Contains("- Setup: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fSetup = str;
    } else if (str.Contains("- Comment: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fComment = str;
    } else if (str.Contains("- Sample: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fSample = str;
    } else if (str.Contains("- Cryo: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fSampleCryo = str;
    } else if (str.Contains("- Orientation: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fOrientation = str;
    } else if (str.Contains("- Insert: ")) {
      idx = str.Index(":");
      str.Remove(0, idx+2);
      fSampleCryoInsert = str;
    } else if (str.Contains("- No of Histos: ")) {
      tokens = str.Tokenize(":");
      if (tokens->GetEntries() < 2) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " couldn't tokenize it." << endl << endl;
        // clean up
        CleanUp(tokens);
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      if (!ostr->GetString().IsDigit()) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " doesn't contain a valid number" << endl << endl;
        return false;
      }
      fNoOfHistos = ostr->GetString().Atoi();
      // clean up
      CleanUp(tokens);
    } else if (str.Contains("- Histo Names: ")) {
      tokens = str.Tokenize(":");
      if (tokens->GetEntries() < 2) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " couldn't tokenize it." << endl << endl;
        // clean up
        CleanUp(tokens);
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      tokens1 = ostr->GetString().Tokenize(";");
      for (Int_t i=0; i<tokens1->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens1->At(i));
        str = ostr->GetString();
        str.Remove(TString::kBoth, ' ');
        fHistoName.push_back(str);
      }
      // clean up
      CleanUp(tokens1);
      CleanUp(tokens);
    } else if (str.Contains("- Histo Length: ")) {
      tokens = str.Tokenize(":");
      if (tokens->GetEntries() < 2) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " couldn't tokenize it." << endl << endl;
        // clean up
        CleanUp(tokens);
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      if (!ostr->GetString().IsDigit()) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " doesn't contain a valid number" << endl << endl;
        return false;
      }
      fHistoLength = ostr->GetString().Atoi();
      // clean up
      CleanUp(tokens);
    } else if (str.Contains("- Time Resolution: ")) {
      tokens = str.Tokenize(":");
      if (tokens->GetEntries() < 2) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " couldn't tokenize it." << endl << endl;
        // clean up
        CleanUp(tokens);
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tokens->At(1)); // <timeResolution> <unit>
      tokens1 = ostr->GetString().Tokenize(" ");
      if (tokens1->GetEntries() < 2) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " couldn't tokenize it." << endl << endl;
        // clean up
        CleanUp(tokens1);
        CleanUp(tokens);
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tokens1->At(0)); // <timeResolution>
      if (!ostr->GetString().IsFloat()) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " seems not be a valid time resolution." << endl << endl;
        // clean up
        CleanUp(tokens1);
        CleanUp(tokens);
        return false;
      }
      dval = ostr->GetString().Atof();
      ostr = dynamic_cast<TObjString*>(tokens1->At(1)); // <units>
      SetTimeResolution(dval, ostr->GetString());
      // clean up
      CleanUp(tokens1);
      CleanUp(tokens);
    } else if (str.Contains("- Time Zero Bin: ")) {
      tokens = str.Tokenize(":");
      if (tokens->GetEntries() < 2) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " couldn't tokenize it." << endl << endl;
        // clean up
        CleanUp(tokens);
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      tokens1 = ostr->GetString().Tokenize(";");
      for (Int_t i=0; i<tokens1->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens1->At(i));
        fTimeZeroBin.push_back(ostr->GetString().Atoi());
      }
      // clean up
      CleanUp(tokens1);
      CleanUp(tokens);
    } else if (str.Contains("- First Good Bin: ")) {
      tokens = str.Tokenize(":");
      if (tokens->GetEntries() < 2) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " couldn't tokenize it." << endl << endl;
        // clean up
        CleanUp(tokens);
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      tokens1 = ostr->GetString().Tokenize(";");
      for (Int_t i=0; i<tokens1->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens1->At(i));
        fFirstGoodBin.push_back(ostr->GetString().Atoi());
      }
      // clean up
      CleanUp(tokens1);
      CleanUp(tokens);
    } else if (str.Contains("- Last Good Bin: ")) {
      tokens = str.Tokenize(":");
      if (tokens->GetEntries() < 2) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " couldn't tokenize it." << endl << endl;
        // clean up
        CleanUp(tokens);
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      tokens1 = ostr->GetString().Tokenize(";");
      for (Int_t i=0; i<tokens1->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens1->At(i));
        fLastGoodBin.push_back(ostr->GetString().Atoi());
      }
      // clean up
      CleanUp(tokens1);
      CleanUp(tokens);
    } else if (str.Contains("- Red/Green offsets: ")) {
      tokens = str.Tokenize(":");
      if (tokens->GetEntries() < 2) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " couldn't tokenize it." << endl << endl;
        // clean up
        CleanUp(tokens);
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      tokens1 = ostr->GetString().Tokenize(";");
      for (Int_t i=0; i<tokens1->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens1->At(i));
        fRedGreenOffset.push_back(ostr->GetString().Atoi());
      }
      // clean up
      CleanUp(tokens1);
      CleanUp(tokens);
    } else if (str.Contains("- Red/Green description: ")) {
      tokens = str.Tokenize(":");
      if (tokens->GetEntries() < 2) {
        cerr << endl << ">> TPsiRunHeader::ExtractHeaderInformation(..) **ERROR** " << str.Data() << " couldn't tokenize it." << endl << endl;
        // clean up
        CleanUp(tokens);
        return false;
      }
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      tokens1 = ostr->GetString().Tokenize(";");
      for (Int_t i=0; i<tokens1->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens1->At(i));
        str = ostr->GetString();
        str.Remove(TString::kBoth, ' ');
        fRedGreenDescription.push_back(str);
      }
      // clean up
      CleanUp(tokens1);
      CleanUp(tokens);
    }
  }

  // TPsiRunProperty header variables
  TPsiRunProperty prop;
  for (Int_t i=0; i<headerInfo->GetEntries(); i++) {
    ostr = dynamic_cast<TObjString*>(headerInfo->At(i));
    if (ostr->GetString().Contains("+-")) {
      if (DecodePhyscialPorperty(ostr, prop, path)) {
        AddProperty(prop);
      } else {
        return false;
      }
    }
  }
  return true;
}

//--------------------------------------------------------------------------
// SetStartTime (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the run start time. It validates that the 'startTime' is a ISO 8601
 * time/date. If 'startTime' is not ISO 8601 compatible, an error message is
 * sent to stderr but the run start time is not set.
 *
 * \param startTime ISO 8601 run start time
 */
void TPsiRunHeader::SetStartTime(TString startTime)
{
  // check that startTime is ISO 8601 compatible
  struct tm tm;
  memset(&tm, 0, sizeof(tm));
  strptime(startTime.Data(), "%Y-%m-%d %H:%M:S", &tm);
  if (tm.tm_year == 0)
    strptime(startTime.Data(), "%Y-%m-%dT%H:%M:S", &tm);
  if (tm.tm_year == 0) {
    cerr << endl << ">> **ERRO** TPsiRunHeader::SetStartTime(): " << startTime.Data() << " is not a ISO 8601 compatible date/time. Will not set it." << endl;
    return;
  }

  fStartTime.Set(startTime);
}

//--------------------------------------------------------------------------
// SetStopTime (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the run stop time. It validates that the 'stopTime' is a ISO 8601
 * time/date. If 'stopTime' is not ISO 8601 compatible, an error message is
 * sent to stderr but the run stop time is not set.
 *
 * \param stopTime ISO 8601 run stop time
 */
void TPsiRunHeader::SetStopTime(TString stopTime)
{
  // check that stopTime is ISO 8601 compatible
  struct tm tm;
  memset(&tm, 0, sizeof(tm));
  strptime(stopTime.Data(), "%Y-%m-%d %H:%M:S", &tm);
  if (tm.tm_year == 0)
    strptime(stopTime.Data(), "%Y-%m-%dT%H:%M:S", &tm);
  if (tm.tm_year == 0) {
    cerr << endl << ">> **ERRO** TPsiRunHeader::SetStopTime(): " << stopTime.Data() << " is not a ISO 8601 compatible date/time. Will not set it." << endl;
    return;
  }

  fStopTime.Set(stopTime);
}

//--------------------------------------------------------------------------
// SetHistoName (public)
//--------------------------------------------------------------------------
/**
 * <p>Set the histogram name at index 'idx'. If 'idx' == -1, the name is appended.
 *
 * \param name histogram name
 * \param idx histogram index
 */
void TPsiRunHeader::SetHistoName(TString name, Int_t idx)
{
  if (idx == -1) {
    fHistoName.push_back(name);
  } else if (idx >= (Int_t)fHistoName.size()) {
    fHistoName.resize(idx+1);
    fHistoName[idx] = name;
  } else {
    fHistoName[idx] = name;
  }
}

//--------------------------------------------------------------------------
// SetTimeResolution (public)
//--------------------------------------------------------------------------
/**
 * <p>Set the time resolution. 'value' is given in 'units'. Allowed 'units'
 * are fs, ps, ns, us.
 *
 * \param value time resolution value
 * \param unit time resolution units
 */
void TPsiRunHeader::SetTimeResolution(Double_t value, TString units)
{
  Double_t factor = 0.0;

  if (!units.CompareTo("fs", TString::kIgnoreCase)) {
    factor = 1.0e-3;
  } else if (!units.CompareTo("ps", TString::kIgnoreCase)) {
    factor = 1.0;
  } else if (!units.CompareTo("ns", TString::kIgnoreCase)) {
    factor = 1.0e3;
  } else if (!units.CompareTo("us", TString::kIgnoreCase)) {
    factor = 1.0e6;
  } else {
    factor = 0.0;
  }

  fTimeResolution = factor * value;
}

//--------------------------------------------------------------------------
// SetTimeZeroBin (public)
//--------------------------------------------------------------------------
/**
 * <p>Set the time zero bin at index 'idx'. If 'idx' == -1, the time zero bin is appended.
 *
 * \param timeZeroBin time zero bin
 * \param idx histogram index
 */
void TPsiRunHeader::SetTimeZeroBin(UInt_t timeZeroBin, Int_t idx)
{
  if (idx == -1) {
    fTimeZeroBin.push_back(timeZeroBin);
  } else if (idx >= (Int_t)fTimeZeroBin.size()) {
    fTimeZeroBin.resize(idx+1);
    fTimeZeroBin[idx] = timeZeroBin;
  } else {
    fTimeZeroBin[idx] = timeZeroBin;
  }
}

//--------------------------------------------------------------------------
// SetFirstGoodBin (public)
//--------------------------------------------------------------------------
/**
 * <p>Set the first good bin at index 'idx'. If 'idx' == -1, the first good bin is appended.
 *
 * \param fgb first good bin
 * \param idx histogram index
 */
void TPsiRunHeader::SetFirstGoodBin(UInt_t fgb, Int_t idx)
{
  if (idx == -1) {
    fFirstGoodBin.push_back(fgb);
  } else if (idx >= (Int_t)fFirstGoodBin.size()) {
    fFirstGoodBin.resize(idx+1);
    fFirstGoodBin[idx] = fgb;
  } else {
    fFirstGoodBin[idx] = fgb;
  }
}

//--------------------------------------------------------------------------
// SetLastGoodBin (public)
//--------------------------------------------------------------------------
/**
 * <p>Set the last good bin at index 'idx'. If 'idx' == -1, the last good bin is appended.
 *
 * \param lgb last good bin
 * \param idx histogram index
 */
void TPsiRunHeader::SetLastGoodBin(UInt_t lgb, Int_t idx)
{
  if (idx == -1) {
    fLastGoodBin.push_back(lgb);
  } else if (idx >= (Int_t)fLastGoodBin.size()) {
    fLastGoodBin.resize(idx+1);
    fLastGoodBin[idx] = lgb;
  } else {
    fLastGoodBin[idx] = lgb;
  }
}

//--------------------------------------------------------------------------
// SetRedGreenHistogramOffset (public)
//--------------------------------------------------------------------------
/**
 * <p>Set the red/green mode histogram offset at index 'idx'. If 'idx' == -1, the red/green mode histogram offset is appended.
 *
 * \param offset red/green mode histogram offset
 * \param idx histogram index
 */
void TPsiRunHeader::SetRedGreenHistogramOffset(UInt_t offset, Int_t idx)
{
  if (idx == -1) {
    fRedGreenOffset.push_back(offset);
  } else if (idx >= (Int_t)fRedGreenOffset.size()) {
    fRedGreenOffset.resize(idx+1);
    fRedGreenOffset[idx] = offset;
  } else {
    fRedGreenOffset[idx] = offset;
  }
}

//--------------------------------------------------------------------------
// SetRedGreenDescription (public)
//--------------------------------------------------------------------------
/**
 * <p>Set the red/green mode description at index 'idx'. If 'idx' == -1, the red/green mode description is appended.
 *
 * \param description red/green mode description
 * \param idx histogram index
 */
void TPsiRunHeader::SetRedGreenDescription(TString description, Int_t idx)
{
  if (idx == -1) {
    fRedGreenDescription.push_back(description);
  } else if (idx >= (Int_t)fRedGreenDescription.size()) {
    fRedGreenDescription.resize(idx+1);
    fRedGreenDescription[idx] = description;
  } else {
    fRedGreenDescription[idx] = description;
  }
}

//--------------------------------------------------------------------------
// AddProperty (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  * \param property
  */
void TPsiRunHeader::AddProperty(TPsiRunProperty &property)
{
  fProperties.push_back(property);
}

//--------------------------------------------------------------------------
// AddProperty (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  * \param name
  * \param value
  * \param error
  * \param unit
  */
void TPsiRunHeader::AddProperty(TString name, Double_t demand, Double_t value, Double_t error, TString unit, TString description, TString path)
{
  TPsiRunProperty prop(name, demand, value, error, unit, description, path);
  fProperties.push_back(prop);
}

//--------------------------------------------------------------------------
// DumpHeader (public)
//--------------------------------------------------------------------------
/**
 * <p>
 */
void TPsiRunHeader::DumpHeader() const
{
  const TPsiRunProperty *prop;

  int old_width = cout.width();

  // get maximal length of the property names
  int name_width = 8; // init to max. length of fixed names like: version, etc.
  for (UInt_t i=0; i<fProperties.size(); i++) {
    if (fProperties[i].GetLabel().Length() > name_width)
      name_width = fProperties[i].GetLabel().Length();
  }
  name_width++;

  // write SVN versions
  cout << endl << setw(name_width) << left << "Version" << setw(old_width) << ": " << GetVersion().Data();

  // write generator
  cout << endl << setw(name_width) << left << "Generator" << setw(old_width) << ": " << GetGenerator().Data();

  // write file name
  cout << endl << setw(name_width) << left << "File Name" << setw(old_width) << ": " << GetFileName().Data();

  // write run title
  cout << endl << setw(name_width) << left << "Run Title" << setw(old_width) << ": " << GetRunTitle().Data();

  // write run number
  cout << endl << setw(name_width) << left << "Run Number" << setw(old_width) << ": " << GetRunNumber();

  // write start time
  cout << endl << setw(name_width) << left << "Run Start Time" << setw(old_width) << ": " << GetStartTimeString();

  // write stop time
  cout << endl << setw(name_width) << left << "Run Stop Time" << setw(old_width) << ": " << GetStopTimeString();

  // write laboratory
  cout << endl << setw(name_width) << left << "Laboratory" << setw(old_width) << ": " << GetLaboratory().Data();

  // write area
  cout << endl << setw(name_width) << left << "Area" << setw(old_width) << ": " << GetArea().Data();

  // write instrument
  cout << endl << setw(name_width) << left << "Instrument" << setw(old_width) << ": " << GetInstrument().Data();

  // write muon momentum
  prop = GetProperty("Muon Beam Momentum");
  if (prop != 0) {
    cout << endl << setw(name_width) << left << prop->GetLabel().Data() << setw(old_width) << ": " << prop->GetValue() << " +- " << prop->GetError() << " " << prop->GetUnit().Data();
    cout << "; SP: " << prop->GetDemand();
    if (prop->GetDescription().CompareTo("n/a", TString::kIgnoreCase)) {
      cout << "; " << prop->GetDescription().Data();
    }
  }

  // write muon species
  cout << endl << setw(name_width) << left << "Muon Species" << setw(old_width) << ": " << GetMuonSpecies().Data();

  // write setup
  cout << endl << setw(name_width) << left << "Setup" << setw(old_width) << ": " << GetSetup().Data();

  // write comment
  cout << endl << setw(name_width) << left << "Comment" << setw(old_width) << ": " << GetComment().Data();

  // write sample
  cout << endl << setw(name_width) << left << "Sample" << setw(old_width) << ": " << GetSample().Data();

  // write sample temperature
  prop = GetProperty("Sample Temperature");
  if (prop != 0) {
    cout << endl << setw(name_width) << left << prop->GetLabel().Data() << setw(old_width) << ": " << prop->GetValue() << " +- " << prop->GetError() << " " << prop->GetUnit().Data();
    cout << "; SP: " << prop->GetDemand();
    if (prop->GetDescription().CompareTo("n/a", TString::kIgnoreCase)) {
      cout << "; " << prop->GetDescription().Data();
    }
  }

  // write sample magnetic field
  prop = GetProperty("Sample Magnetic Field");
  if (prop != 0) {
    cout << endl << setw(name_width) << left << prop->GetLabel().Data() << setw(old_width) << ": " << prop->GetValue() << " +- " << prop->GetError() << " " << prop->GetUnit().Data();
    cout << "; SP: " << prop->GetDemand();
    if (prop->GetDescription().CompareTo("n/a", TString::kIgnoreCase)) {
      cout << "; " << prop->GetDescription().Data();
    }
  }

  // write number of histograms
  cout << endl << setw(name_width) << left << "No of Histos" << setw(old_width) << ": " << GetNoOfHistos();

  // write histogram names
  if (fHistoName.size() > 0) {
    cout << endl << setw(name_width) << left << "Histo Names" << setw(old_width) << ": ";
    for (UInt_t i=0; i<fHistoName.size()-1; i++) {
      cout << fHistoName[i] << "; ";
    }
    cout << fHistoName[fHistoName.size()-1];
  }

  // write histogram length
  cout << endl << setw(name_width) << left << "Histo Length" << setw(old_width) << ": " << GetHistoLength();

  // write time resolution
  UInt_t neededPrec = GetLeastSignificantDigit(GetTimeResolution("ns"));
  UInt_t defaultPrec = cout.precision(neededPrec);  
  cout << endl << setw(name_width) << left << "Time Resolution" << setw(old_width) << ": " << GetTimeResolution("ns") << " ns";
  cout.precision(defaultPrec);

  // write time zero bins
  if (fTimeZeroBin.size() > 0) {
    cout << endl << setw(name_width) << left << "Time Zero Bin" << setw(old_width) << ": ";
    for (UInt_t i=0; i<fTimeZeroBin.size()-1; i++) {
      cout << fTimeZeroBin[i] << "; ";
    }
    cout << fTimeZeroBin[fTimeZeroBin.size()-1];
  }

  // write first good bins
  if (fFirstGoodBin.size() > 0) {
    cout << endl << setw(name_width) << left << "First Good Bin" << setw(old_width) << ": ";
    for (UInt_t i=0; i<fFirstGoodBin.size()-1; i++) {
      cout << fFirstGoodBin[i] << "; ";
    }
    cout << fFirstGoodBin[fFirstGoodBin.size()-1];
  }

  // write last good bins
  if (fLastGoodBin.size() > 0) {
    cout << endl << setw(name_width) << left << "Last Good Bin" << setw(old_width) << ": ";
    for (UInt_t i=0; i<fLastGoodBin.size()-1; i++) {
      cout << fLastGoodBin[i] << "; ";
    }
    cout << fLastGoodBin[fLastGoodBin.size()-1];
  }

  // write red/green mode histogram offsets
  if (fRedGreenOffset.size() > 0) {
    cout << endl << setw(name_width) << left << "Red/Green offsets" << setw(old_width) << ": ";
    for (UInt_t i=0; i<fRedGreenOffset.size()-1; i++) {
      cout << fRedGreenOffset[i] << "; ";
    }
    cout << fRedGreenOffset[fRedGreenOffset.size()-1];
  }

  // write red/green description
  if (fRedGreenDescription.size() > 0) {
    cout << endl << setw(name_width) << left << "Red/Green description" << setw(old_width) << ": ";
    for (UInt_t i=0; i<fRedGreenDescription.size()-1; i++) {
      cout << fRedGreenDescription[i] << "; ";
    }
    cout << fRedGreenDescription[fRedGreenDescription.size()-1];
  }

  // write sample environment
  cout << endl << "SampleEnv:";

  // sample environment: cryo
  cout << endl << setw(name_width) << left << "  Cryo" << setw(old_width) << ": " << GetSampleCryo().Data();

  // sample environment: cryo
  cout << endl << setw(name_width) << left << "  Insert" << setw(old_width) << ": " << GetSampleCryoInsert().Data();

  // sample environment: orientation
  cout << endl << setw(name_width) << left << "  Orientation" << setw(old_width) << ": " << GetOrientation().Data();

  // sample environment: check for physical properties
  for (UInt_t i=0; i<fProperties.size(); i++) {
    if (!fProperties[i].GetPath().CompareTo("/SampleEnv/", TString::kIgnoreCase)) {
      cout << endl << "  " << setw(name_width-2) << left << fProperties[i].GetLabel().Data() << setw(old_width) << ": ";
      cout << fProperties[i].GetValue() << " +- " << fProperties[i].GetError() << " " << fProperties[i].GetUnit().Data();
      cout << "; SP: " << fProperties[i].GetDemand();
      if (fProperties[i].GetDescription().CompareTo("n/a", TString::kIgnoreCase)) {
        cout << "; " << fProperties[i].GetDescription().Data();
      }
    }
  }

  // write magnetic field sample environment
  cout << endl << "MagFieldEnv:";

  // magnetic field sample environment: name
  cout << endl << setw(name_width) << left << "  Magnet Name" << setw(old_width) << ": " << GetSampleMagnetName().Data();

  // magnetic field sample environment: check for physical properties
  for (UInt_t i=0; i<fProperties.size(); i++) {
    if (!fProperties[i].GetPath().CompareTo("/MagFieldEnv/", TString::kIgnoreCase)) {
      cout << endl << "  " << setw(name_width-2) << left << fProperties[i].GetLabel().Data() << setw(old_width) << ": ";
      cout << fProperties[i].GetValue() << " +- " << fProperties[i].GetError() << " " << fProperties[i].GetUnit().Data();
      cout << "; SP: " << fProperties[i].GetDemand();
      if (fProperties[i].GetDescription().CompareTo("n/a", TString::kIgnoreCase)) {
        cout << "; " << fProperties[i].GetDescription().Data();
      }
    }
  }

  // write beamline settings
  cout << endl << "Beamline:";

  // beamline settings
  cout << endl << setw(name_width) << left << "  Still Missing ..." << setw(old_width);

  // write scalers
  cout << endl << "Scalers:";

  // scalers
  cout << endl << setw(name_width) << left << "  Still Missing ..." << setw(old_width);

  cout << endl << endl;
}

//--------------------------------------------------------------------------
// DrawHeader (public)
//--------------------------------------------------------------------------
/**
 * <p>
 */
void TPsiRunHeader::DrawHeader() const
{
  TPaveText *pt;
  TCanvas *ca;

  ca = new TCanvas("PSI RunHeader","PSI RunHeader", 147,37,699,527);
  ca->Range(0., 0., 100., 100.);

  pt = new TPaveText(10.,10.,90.,90.,"br");
  pt->SetFillColor(19);
  pt->SetTextAlign(12);

  pt->Draw();

  ca->Modified(kTRUE);
}

//--------------------------------------------------------------------------
// DecodePhyscialPorperty (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param ostr
 * \param prop
 * \param path
 */
Bool_t TPsiRunHeader::DecodePhyscialPorperty(TObjString *oprop, TPsiRunProperty &prop, TString &path)
{
  TObjArray *tokens = oprop->GetString().Tokenize("-:");
  TObjArray *tokens1 = 0;
  TObjString *ostr = 0;
  TString str("");

  if (tokens == 0) {
    cerr << endl << ">> **ERROR** Couldn't tokenize physical property string '" << ostr->GetString().Data() << "' (1)." << endl;
    return false;
  }

  // get property name
  ostr = dynamic_cast<TObjString*>(tokens->At(1));
  str = ostr->GetString();
  str.Remove(TString::kLeading, ' ');
  prop.SetLabel(str);

  CleanUp(tokens);

  // get measured value
  tokens = oprop->GetString().Tokenize(":");
  if (tokens == 0) {
    cerr << endl << ">> **ERROR** Couldn't tokenize physical property string '" << ostr->GetString().Data() << "' (2)." << endl;
    return false;
  }
  ostr = dynamic_cast<TObjString*>(tokens->At(1));
  str = ostr->GetString();
  tokens1 = str.Tokenize(" ;");
  if (tokens1 == 0) {
    cerr << endl << ">> **ERROR** Couldn't tokenize physical property string '" << ostr->GetString().Data() << "' (3)." << endl;
    CleanUp(tokens);
    return false;
  }
  if (tokens1->GetEntries() < 4) {
    cerr << endl << ">> **ERROR** not enough tokens from physical property string '" << ostr->GetString().Data() << "' (4)." << endl;
    CleanUp(tokens1);
    CleanUp(tokens);
    return false;
  }

  // get measured value
  ostr = dynamic_cast<TObjString*>(tokens1->At(0));
  if (ostr->GetString().IsFloat()) {
    prop.SetValue(ostr->GetString().Atof());
  } else {
    cerr << endl << ">> **ERROR** unexpected measured value. Found " << ostr->GetString().Data() << ", expected float." << endl;
    CleanUp(tokens);
    return false;
  }

  // get estimated err
  ostr = dynamic_cast<TObjString*>(tokens1->At(2));
  if (ostr->GetString().IsFloat()) {
    prop.SetError(ostr->GetString().Atof());
  } else {
    cerr << endl << ">> **ERROR** unexpected estimated error. Found " << ostr->GetString().Data() << ", expected float." << endl;
    CleanUp(tokens);
    return false;
  }

  // get unit
  ostr = dynamic_cast<TObjString*>(tokens1->At(3));
  str = ostr->GetString();
  str.Remove(TString::kLeading, ' ');
  prop.SetUnit(str);

  CleanUp(tokens1);

  ostr = dynamic_cast<TObjString*>(tokens->At(2));
  str = ostr->GetString();
  tokens1 = str.Tokenize(";");
  if (tokens1 == 0) {
    cerr << endl << ">> **ERROR** Couldn't tokenize physical property string '" << ostr->GetString().Data() << "' (4)." << endl;
    CleanUp(tokens);
    return false;
  }

  // get demand value
  ostr = dynamic_cast<TObjString*>(tokens1->At(0));
  if (ostr->GetString().IsFloat()) {
    prop.SetDemand(ostr->GetString().Atof());
  } else {
    cerr << endl << ">> **ERROR** unexpected demand value. Found " << ostr->GetString().Data() << ", expected float." << endl;
    CleanUp(tokens);
    return false;
  }

  if (tokens1->GetEntries() > 1) { // with description
    ostr = dynamic_cast<TObjString*>(tokens1->At(1));
    str = ostr->GetString();
    str.Remove(TString::kLeading, ' ');
    prop.SetDescription(str);
  }

  prop.SetPath(path);

  // clean up
  CleanUp(tokens1);
  CleanUp(tokens);

  return true;
}

//--------------------------------------------------------------------------
// GetDecimalPlace (private)
//--------------------------------------------------------------------------
/**
 * <p>Check decimal place of val. If val > 1.0, the function will return 0, otherwise
 * the first decimal place found will be returned.
 *
 * \param val value from which the first significant digit shall be determined
 */
UInt_t TPsiRunHeader::GetDecimalPlace(Double_t val)
{
  UInt_t digit = 0;

  if (val < 1.0) {
    UInt_t count=1;
    do {
      val *= 10.0;
      if (val > 1.0)
        digit = count;
      count++;
    } while ((digit == 0) || (count > 20));
  }

  return digit;
}

//--------------------------------------------------------------------------
// GetLeastSignificantDigit (private)
//--------------------------------------------------------------------------
/**
 * <p>returns the number of significant digits
 *
 * \param val value from which the lowest significant digit shall be determined
 */
UInt_t TPsiRunHeader::GetLeastSignificantDigit(Double_t val) const
{
  char cstr[1024];
  snprintf(cstr, sizeof(cstr), "%.20lf", val);

  int i=0, j=0;
  for (i=strlen(cstr)-1; i>=0; i--) {
    if (cstr[i] != '0')
      break;
  }

  for (j=strlen(cstr)-1; j>=0; j--) {
    if (cstr[j] == '.')
      break;
  }
  if (j==0) // no decimal point present, e.g. 321
    j=i;

  return i-j;
}

//--------------------------------------------------------------------------
// CleanUp (private)
//--------------------------------------------------------------------------
/**
 * <p>Cleans up ROOT objects.
 *
 * \param obj pointer to the object to be cleaned up.
 */
void TPsiRunHeader::CleanUp(TObject *obj)
{
  if (obj) {
    delete obj;
    obj = 0;
  }
}
