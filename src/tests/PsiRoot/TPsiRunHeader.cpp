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
  fName  = "n/a";
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
TPsiRunProperty::TPsiRunProperty(TObjString &name, Double_t value, Double_t error, TObjString &unit) :
    fName(name), fValue(value), fError(error), fUnit(unit)
{
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
TPsiRunProperty::TPsiRunProperty(TString &name, Double_t value, Double_t error, TString &unit) : fName(name), fValue(value), fError(error), fUnit(unit)
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
  fVersion.SetString("00 - version: $Id$");
  fRunTitle.SetString("01 - run title: n/a");
  fRunNumber.SetString("02 - run number: -1");
  fLaboratory.SetString("03 - laboratory: n/a");
  fInstrument.SetString("04 - instrument: n/a");
  fSetup.SetString("05 - setup: n/a");
  fSample.SetString("06 - sample: n/a");
  fOrientation.SetString("07 - orientation: n/a");
  fPropertiesList.Expand(0); // init to size 0
  fProperties.Expand(0);  // init to size 0
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
  * <p>Destructor.
  */
TPsiRunHeader::~TPsiRunHeader()
{
  fPropertiesList.Delete();
  fProperties.Delete();
}

//--------------------------------------------------------------------------
// GetVersion (public)
//--------------------------------------------------------------------------
/**
  * <p>
  */
TString TPsiRunHeader::GetVersion() const
{
  TString str("??");

  Int_t idx = fVersion.GetString().First(":");
  if ((idx < 0) || (idx+2 > fVersion.GetString().Length()))
    return str;

  str = fVersion.GetString();
  str.Replace(0, idx+2, 0, 0);

  return str;
}

//--------------------------------------------------------------------------
// GetRunTitle (public)
//--------------------------------------------------------------------------
/**
  * <p>
  */
TString TPsiRunHeader::GetRunTitle() const
{
  return GetInfoString(fRunTitle);
}

//--------------------------------------------------------------------------
// SetRunTitle (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  * \param runTitle
  */
void TPsiRunHeader::SetRunTitle(TString runTitle)
{
  char str[1024];
  sprintf(str, "%02d - run title: %s", TPRH_RUN_TITLE_IDX, runTitle.Data());

  fRunTitle.SetString(str);
}

//--------------------------------------------------------------------------
// GetRunNumber (public)
//--------------------------------------------------------------------------
/**
  * <p>
  */
Int_t TPsiRunHeader::GetRunNumber() const
{
  TObjArray *tokens = 0;
  TObjString *ostr = 0;
  TString str("");
  Int_t ival=-999;

  tokens = fRunNumber.GetString().Tokenize(":");
  if (!tokens) { // couldn't get tokens
    return ival;
  }
  if (tokens->GetEntries() != 2) { // wrong number of tokens
    delete tokens;
    return ival;
  }

  ostr = dynamic_cast<TObjString*>(tokens->At(1));
  str = ostr->GetString();
  if (str.IsDigit())
    ival = str.Atoi();

  delete tokens;

  return ival;
}

//--------------------------------------------------------------------------
// SetRunNumber (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  * \param runNumber
  */
void TPsiRunHeader::SetRunNumber(Int_t runNumber)
{
  char str[1024];
  sprintf(str, "%02d - run number: %06d", TPRH_RUN_NUMBER_IDX, runNumber);

  fRunNumber.SetString(str);
}

//--------------------------------------------------------------------------
// GetLab (public)
//--------------------------------------------------------------------------
/**
  * <p>
  */
TString TPsiRunHeader::GetLab() const
{
  return GetInfoString(fLaboratory);
}

//--------------------------------------------------------------------------
// SetLab (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  * \param lab
  */
void TPsiRunHeader::SetLab(TString lab)
{
  char str[1024];
  sprintf(str, "%02d - laboratory: %s", TPRH_LABORATORY_IDX, lab.Data());

  fLaboratory.SetString(str);
}

//--------------------------------------------------------------------------
// GetInstrument (public)
//--------------------------------------------------------------------------
/**
  * <p>
  */
TString TPsiRunHeader::GetInstrument() const
{
  return GetInfoString(fInstrument);
}

//--------------------------------------------------------------------------
// SetInstrument (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  * \param instrument
  */
void TPsiRunHeader::SetInstrument(TString instrument)
{
  char str[1024];
  sprintf(str, "%02d - instrument: %s", TPRH_INSTRUMENT_IDX, instrument.Data());

  fInstrument.SetString(str);
}

//--------------------------------------------------------------------------
// GetSetup (public)
//--------------------------------------------------------------------------
/**
  * <p>
  */
TString TPsiRunHeader::GetSetup() const
{
  return GetInfoString(fSetup);
}

//--------------------------------------------------------------------------
// SetSetup (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  * \param setup
  */
void TPsiRunHeader::SetSetup(TString setup)
{
  char str[1024];
  sprintf(str, "%02d - setup: %s", TPRH_SETUP_IDX, setup.Data());

  fSetup.SetString(str);
}

//--------------------------------------------------------------------------
// GetSample (public)
//--------------------------------------------------------------------------
/**
  * <p>
  */
TString TPsiRunHeader::GetSample() const
{
  return GetInfoString(fSample);
}

//--------------------------------------------------------------------------
// SetSample (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  * \param sample
  */
void TPsiRunHeader::SetSample(TString sample)
{
  char str[1024];
  sprintf(str, "%02d - sample: %s", TPRH_SAMPLE_IDX, sample.Data());

  fSample.SetString(str);
}

//--------------------------------------------------------------------------
// GetOrientation (public)
//--------------------------------------------------------------------------
/**
  * <p>
  */
TString TPsiRunHeader::GetOrientation() const
{
  return GetInfoString(fOrientation);
}

//--------------------------------------------------------------------------
// SetOrientation (public)
//--------------------------------------------------------------------------
/**
  * <p>
  *
  * \param orientation
  */
void TPsiRunHeader::SetOrientation(TString orientation)
{
  char str[1024];
  sprintf(str, "%02d - orientation: %s", TPRH_ORIENTATION_IDX, orientation.Data());

  fOrientation.SetString(str);
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
  TPsiRunProperty *prop = (TPsiRunProperty*) property.Clone();
  fProperties.AddLast(prop);

  char str[1024];
  TObjString *tostr;
  sprintf(str, "%02d - %s: %lf +- %lf (%s)", TPRH_OFFSET+fPropertiesList.GetLast(),
          prop->GetName().GetString().Data(), prop->GetValue(), prop->GetError(), prop->GetUnit().GetString().Data());
  tostr = new TObjString(str);
  fPropertiesList.AddLast(tostr);
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
  TPsiRunProperty prop(name, value, error, unit);
  TPsiRunProperty *p_prop = (TPsiRunProperty*) prop.Clone();

  fProperties.AddLast(p_prop);

  char str[1024];
  TObjString *tostr;
  sprintf(str, "%02d - %s: %lf +- %lf (%s)", TPRH_OFFSET+fPropertiesList.GetLast(),
          name.Data(), value, error, unit.Data());
  tostr = new TObjString(str);
  fPropertiesList.AddLast(tostr);
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
  TPsiRunProperty *prop;

  // get maximal length of the property names
  int name_width = 8; // init to max. length of fixed names like: version, etc.
  for (Int_t i=0; i<=fProperties.GetLast(); i++) {
    prop = (TPsiRunProperty*)(fProperties[i]);
    if (prop->GetName().GetString().Length() > name_width)
      name_width = prop->GetName().GetString().Length();
  }
  name_width++;

  // write SVN versions
  cout << endl << setw(name_width) << left << "version" << setw(old_width) << ": " << GetVersion().Data();

  // write run title
  cout << endl << setw(name_width) << left << "run title" << setw(old_width) << ": " << GetRunTitle().Data();

  // write run number
  cout << endl << setw(name_width) << left << "run number" << setw(old_width) << ": " << GetRunNumber();

  // write laboratory
  cout << endl << setw(name_width) << left << "laboratory" << setw(old_width) << ": " << GetLab().Data();

  // write instrument
  cout << endl << setw(name_width) << left << "instrument" << setw(old_width) << ": " << GetInstrument().Data();

  // write setup
  cout << endl << setw(name_width) << left << "setup" << setw(old_width) << ": " << GetSetup().Data();

  // write sample
  cout << endl << setw(name_width) << left << "sample" << setw(old_width) << ": " << GetSample().Data();

  // write orientation
  cout << endl << setw(name_width) << left << "orientation" << setw(old_width) << ": " << GetOrientation().Data();

  for (Int_t i=0; i<=fProperties.GetLast(); i++) {
    prop = (TPsiRunProperty*)(fProperties[i]);
    cout << endl << setw(name_width) << left << prop->GetName().GetString().Data() << setw(old_width) << ": " << prop->GetValue() << " +- " << prop->GetError();
    if (prop->GetUnit().GetString().CompareTo("n/a", TString::kIgnoreCase)) {
      cout << " " << prop->GetUnit().GetString().Data();
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
// GetInfoString (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param tostr a TObjString with a content structure "%s: %s".
 */
TString TPsiRunHeader::GetInfoString(const TObjString &tostr) const
{
  TObjArray *tokens = 0;
  TObjString *ostr = 0;
  TString str("??");

  tokens = tostr.GetString().Tokenize(":");
  if (!tokens) { // couldn't get tokens
    return str;
  }
  if (tokens->GetEntries() != 2) { // wrong number of tokens
    delete tokens;
    return str;
  }

  ostr = dynamic_cast<TObjString*>(tokens->At(1));
  str = ostr->GetString();
  str.Remove(TString::kLeading, ' ');

  delete tokens;

  return str;
}
