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

#include <iostream>
#include <iomanip>
using namespace std;

#include "TPsiRunHeader.h"

#include <TPaveText.h>
#include <TCanvas.h>

#define TPRH_VERSION_IDX     0
#define TPRH_RUN_TITLE_IDX   1
#define TPRH_RUN_NUMBER_IDX  2
#define TPRH_LABORATORY_IDX  3
#define TPRH_INSTRUMENT_IDX  4
#define TPRH_SETUP_IDX       5
#define TPRH_SAMPLE_IDX      6
#define TPRH_ORIENTATION_IDX 7

#define TPRH_OFFSET          9

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
  fValue = 0.0;
  fError = 0.0;
  fUnit  = "n/a";
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
TPsiRunProperty::TPsiRunProperty(TString &label, Double_t value, Double_t error, TString &unit) :
    fLabel(label), fValue(value), fError(error), fUnit(unit)
{
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor.
 */
TPsiRunProperty::~TPsiRunProperty()
{
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
  fRunTitle    = TString("n/a");
  fRunNumber   = -1;
  fLaboratory  = TString("n/a");
  fInstrument  = TString("n/a");
  fSetup       = TString("n/a");
  fSample      = TString("n/a");
  fOrientation = TString("n/a");

  fHeader.Expand(0); // init to size 0
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
  * <p>Destructor.
  */
TPsiRunHeader::~TPsiRunHeader()
{
  fProperties.clear();
  fHeader.Delete();
}

//--------------------------------------------------------------------------
// GetHeader (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  */
TObjArray* TPsiRunHeader::GetHeader()
{
  // make sure that previous header is removed
  fHeader.Expand(0);

  char str[1024], fmt[1024];
  TObjString *tostr;

  // add version
  sprintf(str, "%02d - Version: %s", TPRH_VERSION_IDX+1, fVersion.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add run title
  sprintf(str, "%02d - Run Title: %s", TPRH_RUN_TITLE_IDX+1, fRunTitle.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add run number
  sprintf(str, "%02d - Run Number: %d", TPRH_RUN_NUMBER_IDX+1, fRunNumber);
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add laboratory
  sprintf(str, "%02d - Laboratory: %s", TPRH_LABORATORY_IDX+1, fLaboratory.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add instrument
  sprintf(str, "%02d - Instrument: %s", TPRH_INSTRUMENT_IDX+1, fInstrument.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add setup
  sprintf(str, "%02d - Setup: %s", TPRH_SETUP_IDX+1, fSetup.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add sample
  sprintf(str, "%02d - Sample: %s", TPRH_SAMPLE_IDX+1, fSample.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add orientation
  sprintf(str, "%02d - Orientation: %s", TPRH_ORIENTATION_IDX+1, fOrientation.Data());
  tostr = new TObjString(str);
  fHeader.AddLast(tostr);

  // add properties
  UInt_t digit=0;
  for (UInt_t i=0; i<fProperties.size(); i++) {
    digit = GetDecimalPlace(fProperties[i].GetError());
    if (fProperties[i].GetUnit().CompareTo("n/a", TString::kIgnoreCase)) {
      sprintf(fmt, "%%02d - %%s: %%.%dlf +- (%%.%dlf) %%s", digit, digit);
      sprintf(str, fmt, TPRH_OFFSET+i, fProperties[i].GetLabel().Data(), fProperties[i].GetValue(), fProperties[i].GetError(), fProperties[i].GetUnit().Data());
    } else {
      sprintf(fmt, "%%02d - %%s: %%.%dlf +- (%%.%dlf)", digit, digit);
      sprintf(str, fmt, TPRH_OFFSET+i, fProperties[i].GetLabel().Data(), fProperties[i].GetValue(), fProperties[i].GetError());
    }
    tostr = new TObjString(str);
    fHeader.AddLast(tostr);
  }

  fHeader.SetName("RunHeader");

  return &fHeader;
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
void TPsiRunHeader::AddProperty(TString name, Double_t value, Double_t error, TString unit)
{
  TPsiRunProperty property(name, value, error, unit);
  fProperties.push_back(property);
}

//--------------------------------------------------------------------------
// DumpHeader (public)
//--------------------------------------------------------------------------
/**
 * <p>
 */
void TPsiRunHeader::DumpHeader() const
{
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

  // write run title
  cout << endl << setw(name_width) << left << "Run Title" << setw(old_width) << ": " << GetRunTitle().Data();

  // write run number
  cout << endl << setw(name_width) << left << "Run Number" << setw(old_width) << ": " << GetRunNumber();

  // write laboratory
  cout << endl << setw(name_width) << left << "Laboratory" << setw(old_width) << ": " << GetLab().Data();

  // write instrument
  cout << endl << setw(name_width) << left << "Instrument" << setw(old_width) << ": " << GetInstrument().Data();

  // write setup
  cout << endl << setw(name_width) << left << "Setup" << setw(old_width) << ": " << GetSetup().Data();

  // write sample
  cout << endl << setw(name_width) << left << "Sample" << setw(old_width) << ": " << GetSample().Data();

  // write orientation
  cout << endl << setw(name_width) << left << "Orientation" << setw(old_width) << ": " << GetOrientation().Data();

  for (UInt_t i=0; i<fProperties.size(); i++) {
    cout << endl << setw(name_width) << left << fProperties[i].GetLabel().Data() << setw(old_width) << ": " << fProperties[i].GetValue() << " +- " << fProperties[i].GetError();
    if (fProperties[i].GetUnit().CompareTo("n/a", TString::kIgnoreCase)) {
      cout << " " << fProperties[i].GetUnit().Data();
    }
  }
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
