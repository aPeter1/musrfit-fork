/***************************************************************************

  TMusrRunHeader.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2012 by Andreas Suter                              *
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

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
using namespace std;

#include "TMusrRunHeader.h"

#include <TPaveText.h>
#include <TCanvas.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>
#include <TList.h>
#include <TMap.h>
#include <TSAXParser.h>
#include <TXMLAttr.h>

ClassImp(TMusrRunPhysicalQuantity)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 */
TMusrRunPhysicalQuantity::TMusrRunPhysicalQuantity()
{
  fLabel  = "n/a";
  fDemand = MRRH_UNDEFINED;
  fValue  = MRRH_UNDEFINED;
  fError  = MRRH_UNDEFINED;
  fUnit   = "n/a";
  fDescription = "n/a";
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param label of the physical quantity, e.g. 'Sample Temperature'
 * \param demand value of the physical quantity
 * \param value measured value of the physical quantity
 * \param error estimated error of the physical quantity
 * \param unit of the physical quantity, e.g. 'K'.
 * \param description additional more detailed description of the physical quantity
 */
TMusrRunPhysicalQuantity::TMusrRunPhysicalQuantity(TString label, Double_t demand, Double_t value, Double_t error, TString unit, TString description) :
    fLabel(label), fDemand(demand), fValue(value), fError(error), fUnit(unit)
{
  if (description.IsWhitespace())
    fDescription = "n/a";
  else
    fDescription = description;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param label of the physical quantity, e.g. 'Sample Temperature'
 * \param demand value of the physical quantity
 * \param value measured value of the physical quantity
 * \param unit of the physical quantity, e.g. 'K'.
 * \param description additional more detailed description of the physical quantity
 */
TMusrRunPhysicalQuantity::TMusrRunPhysicalQuantity(TString label, Double_t demand, Double_t value, TString unit, TString description) :
    fLabel(label), fDemand(demand), fValue(value), fUnit(unit)
{
  fError = MRRH_UNDEFINED;
  if (description.IsWhitespace())
    fDescription = "n/a";
  else
    fDescription = description;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param label of the physical quantity, e.g. 'Sample Temperature'
 * \param value measured value of the physical quantity
 * \param unit of the physical quantity, e.g. 'K'.
 * \param description additional more detailed description of the physical quantity
 */
TMusrRunPhysicalQuantity::TMusrRunPhysicalQuantity(TString label, Double_t value, TString unit, TString description) :
    fLabel(label), fValue(value), fUnit(unit)
{
  fDemand = MRRH_UNDEFINED;
  fError  = MRRH_UNDEFINED;
  if (description.IsWhitespace())
    fDescription = "n/a";
  else
    fDescription = description;
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>set a physical quantity.
 *
 * \param label of the physical quantity, e.g. 'Sample Temperature'
 * \param demand value of the physical quantity
 * \param value measured value of the physical quantity
 * \param error estimated error of the physical quantity
 * \param unit of the physical quantity, e.g. 'K'.
 * \param description additional more detailed description of the physical quantity
 */
void TMusrRunPhysicalQuantity::Set(TString label, Double_t demand, Double_t value, Double_t error, TString unit, TString description)
{
  fLabel = label;
  fDemand = demand;
  fValue = value;
  fError = error;
  fUnit = unit;
  if (description.IsWhitespace())
    fDescription = "n/a";
  else
    fDescription = description;
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>set a physical quantity.
 *
 * \param label of the physical quantity, e.g. 'Sample Temperature'
 * \param demand value of the physical quantity
 * \param value measured value of the physical quantity
 * \param unit of the physical quantity, e.g. 'K'.
 * \param description additional more detailed description of the physical quantity
 */
void TMusrRunPhysicalQuantity::Set(TString label, Double_t demand, Double_t value, TString unit, TString description)
{
  fLabel = label;
  fDemand = demand;
  fValue = value;
  fError = MRRH_UNDEFINED;
  fUnit = unit;
  if (description.IsWhitespace())
    fDescription = "n/a";
  else
    fDescription = description;
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>set a physical quantity.
 *
 * \param label of the physical quantity, e.g. 'Sample Temperature'
 * \param value measured value of the physical quantity
 * \param unit of the physical quantity, e.g. 'K'.
 * \param description additional more detailed description of the physical quantity
 */
void TMusrRunPhysicalQuantity::Set(TString label, Double_t value, TString unit, TString description)
{
  fLabel = label;
  fDemand = MRRH_UNDEFINED;
  fValue = value;
  fError = MRRH_UNDEFINED;
  fUnit = unit;
  if (description.IsWhitespace())
    fDescription = "n/a";
  else
    fDescription = description;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImp(TMusrRunHeader)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
  * <p>Constructor.
  */
TMusrRunHeader::TMusrRunHeader()
{
  Init();
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
  * <p>Constructor.
  */
TMusrRunHeader::TMusrRunHeader(const char *fileName)
{
  Init();
  fFileName = TString(fileName);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
  * <p>Destructor.
  */
TMusrRunHeader::~TMusrRunHeader()
{
  fStringObj.clear();
  fIntObj.clear();
  fDoubleObj.clear();
  fMusrRunPhysQuantityObj.clear();
  fStringVectorObj.clear();
  fIntVectorObj.clear();
  fDoubleVectorObj.clear();

  fFolder.clear();

  fMap.DeleteAll();
}


//--------------------------------------------------------------------------
// Init (private)
//--------------------------------------------------------------------------
/**
  * <p>Initializes variables.
  */
void TMusrRunHeader::Init()
{
  fMap.DeleteAll();
  fMap.SetName("__map");

  fFileName = TString("n/a");
}

//--------------------------------------------------------------------------
// GetHeaderInfo (public)
//--------------------------------------------------------------------------
/**
 * <p>Get MUSR-ROOT header information of 'path'.
 *
 * \param requestedPath of the MUSR-ROOT header, e.g. RunInfo
 * \param content of the requested MUSR-ROOT header.
 */
void TMusrRunHeader::GetHeaderInfo(TString requestedPath, TObjArray &content)
{
  // make sure content is initialized
  content.Delete();
  content.Expand(0);

  static UInt_t count = 1;
  TString str(""), path(""), name(""), fmt(""), tstr("");
  TObjString *tostr;
  TMusrRunPhysicalQuantity prop;

  for (UInt_t i=0; i<fStringObj.size(); i++) {
    if (fStringObj[i].GetPathName().Contains(requestedPath)) { // correct path
      SplitPathName(fStringObj[i].GetPathName(), path, name);
      str.Form("%03d - %s: %s", count++, name.Data(), fStringObj[i].GetValue().Data());
      tostr = new TObjString(str);
      content.AddLast(tostr);
    }
  }
  for (UInt_t i=0; i<fIntObj.size(); i++) {
    if (fIntObj[i].GetPathName().Contains(requestedPath)) { // correct path
      SplitPathName(fIntObj[i].GetPathName(), path, name);
      str.Form("%03d - %s: %d", count++, name.Data(), fIntObj[i].GetValue());
      tostr = new TObjString(str);
      content.AddLast(tostr);
    }
  }
  for (UInt_t i=0; i<fDoubleObj.size(); i++) {
    if (fDoubleObj[i].GetPathName().Contains(requestedPath)) { // correct path
      SplitPathName(fDoubleObj[i].GetPathName(), path, name);
      str.Form("%03d - %s: %lf", count++, name.Data(), fDoubleObj[i].GetValue());
      tostr = new TObjString(str);
      content.AddLast(tostr);
    }
  }
  for (UInt_t i=0; i<fMusrRunPhysQuantityObj.size(); i++) {
    if (fMusrRunPhysQuantityObj[i].GetPathName().Contains(requestedPath)) { // correct path
      prop = fMusrRunPhysQuantityObj[i].GetValue();
      Int_t digit, digit_d;
      if ((prop.GetDemand() != MRRH_UNDEFINED) && (prop.GetValue() != MRRH_UNDEFINED) && (prop.GetError() != MRRH_UNDEFINED) &&
          (prop.GetUnit() != "n/a")) { // <value> +- <error> <unit>; SP: <demand> [; <description>]
        digit = GetDecimalPlace(prop.GetError());
        digit_d = GetLeastSignificantDigit(prop.GetDemand());
        if (prop.GetDescription() != "n/a") {
          fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit, digit_d);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(), prop.GetDemand(), prop.GetDescription().Data());
        } else {
          fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf", digit, digit, digit_d);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(), prop.GetDemand());
        }
      } else if ((prop.GetDemand() == MRRH_UNDEFINED) && (prop.GetValue() != MRRH_UNDEFINED) && (prop.GetError() != MRRH_UNDEFINED) &&
                 (prop.GetUnit() != "n/a")) { // <value> +- <error> <unit> [; <description>]
        digit = GetDecimalPlace(prop.GetError());
        if (prop.GetDescription() != "n/a") {
          fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; %%s", digit, digit);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(), prop.GetDescription().Data());
        } else {
          fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s", digit, digit);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data());
        }
      } else if ((prop.GetDemand() == MRRH_UNDEFINED) && (prop.GetValue() != MRRH_UNDEFINED) && (prop.GetError() == MRRH_UNDEFINED) &&
                 (prop.GetUnit() != "n/a")) { // <value> <unit> [; <description>]
        digit = GetLeastSignificantDigit(prop.GetValue());
        if (prop.GetDescription() != "n/a") {
          fmt.Form("%%s: %%.%dlf %%s; %%s", digit);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data(), prop.GetDescription().Data());
        } else {
          fmt.Form("%%s: %%.%dlf %%s", digit);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data());
        }
      } else if ((prop.GetDemand() != MRRH_UNDEFINED) && (prop.GetValue() != MRRH_UNDEFINED) && (prop.GetError() == MRRH_UNDEFINED) &&
                 (prop.GetUnit() != "n/a")) { // <value> <unit>; SP: <demand> [; <description>]
        digit = GetLeastSignificantDigit(prop.GetValue());
        digit_d = GetLeastSignificantDigit(prop.GetDemand());
        if (prop.GetDescription() != "n/a") {
          fmt.Form("%%s: %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit_d);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data(), prop.GetDemand(), prop.GetDescription().Data());
        } else {
          fmt.Form("%%s: %%.%dlf %%s; SP: %%.%dlf", digit, digit_d);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data(), prop.GetDemand());
        }
      }
      str.Form("%03d - %s", count++, tstr.Data());
      tostr = new TObjString(str);
      content.AddLast(tostr);
    }
  }
  for (UInt_t i=0; i<fStringVectorObj.size(); i++) {
    if (fStringVectorObj[i].GetPathName().Contains(requestedPath)) { // correct path
      SplitPathName(fStringVectorObj[i].GetPathName(), path, name);
      str.Form("%03d - %s: ", count++, name.Data());
      TStringVector vstr = fStringVectorObj[i].GetValue();
      for (UInt_t k=0; k<vstr.size()-1; k++)
        str += vstr[k] + "; ";
      str += vstr[vstr.size()-1];
      tostr = new TObjString(str);
      content.AddLast(tostr);
    }
  }
  for (UInt_t i=0; i<fIntVectorObj.size(); i++) {
    if (fIntVectorObj[i].GetPathName().Contains(requestedPath)) { // correct path
      SplitPathName(fIntVectorObj[i].GetPathName(), path, name);
      str.Form("%03d - %s: ", count++, name.Data());
      TIntVector vint = fIntVectorObj[i].GetValue();
      for (UInt_t k=0; k<vint.size()-1; k++) {
        str += vint[k];
        str += "; ";
      }
      str += vint[vint.size()-1];
      tostr = new TObjString(str);
      content.AddLast(tostr);
    }
  }
  for (UInt_t i=0; i<fDoubleVectorObj.size(); i++) {
    if (fDoubleVectorObj[i].GetPathName().Contains(requestedPath)) { // correct path
      SplitPathName(fDoubleVectorObj[i].GetPathName(), path, name);
      str.Form("%03d - %s: ", count++, name.Data());
      TDoubleVector dvec = fDoubleVectorObj[i].GetValue();
      for (UInt_t k=0; k<dvec.size()-1; k++) {
        str += dvec[k];
        str += "; ";
      }
      str += dvec[dvec.size()-1];
      tostr = new TObjString(str);
      content.AddLast(tostr);
    }
  }

  content.SetName(requestedPath);
}

//--------------------------------------------------------------------------
// GetValue (public)
//--------------------------------------------------------------------------
/**
 * <p>Get TString 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Run Title
 * \param value TString return value
 * \param ok flag telling if the TString value was found
 */
void TMusrRunHeader::GetValue(TString pathName, TString &value, Bool_t &ok)
{
  ok = false;

  for (UInt_t i=0; i<fStringObj.size(); i++) {
    if (fStringObj[i].GetPathName() == pathName) {
      value = fStringObj[i].GetValue();
      ok = true;
    }
  }
}

//--------------------------------------------------------------------------
// GetValue (public)
//--------------------------------------------------------------------------
/**
 * <p>Get Int_t 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Run Number
 * \param value Int_t return value
 * \param ok flag telling if the Int_t value was found
 */
void TMusrRunHeader::GetValue(TString pathName, Int_t &value, Bool_t &ok)
{
  ok = false;

  for (UInt_t i=0; i<fIntObj.size(); i++) {
    if (fIntObj[i].GetPathName() == pathName) {
      value = fIntObj[i].GetValue();
      ok = true;
    }
  }
}

//--------------------------------------------------------------------------
// GetValue (public)
//--------------------------------------------------------------------------
/**
 * <p>Get Double_t 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Time Zero Bin
 * \param value Double_t return value
 * \param ok flag telling if the Double_t value was found
 */
void TMusrRunHeader::GetValue(TString pathName, Double_t &value, Bool_t &ok)
{
  ok = false;

  for (UInt_t i=0; i<fDoubleObj.size(); i++) {
    if (fDoubleObj[i].GetPathName() == pathName) {
      value = fDoubleObj[i].GetValue();
      ok = true;
    }
  }
}

//--------------------------------------------------------------------------
// GetValue (public)
//--------------------------------------------------------------------------
/**
 * <p>Get TMusrRunPhysicalQuantity 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Run Title
 * \param value TMusrRunPhysicalQuantity return value
 * \param ok flag telling if the TMusrRunPhysicalQuantity value was found
 */
void TMusrRunHeader::GetValue(TString pathName, TMusrRunPhysicalQuantity &value, Bool_t &ok)
{
  ok = false;

  for (UInt_t i=0; i<fMusrRunPhysQuantityObj.size(); i++) {
    if (fMusrRunPhysQuantityObj[i].GetPathName() == pathName) {
      value = fMusrRunPhysQuantityObj[i].GetValue();
      ok = true;
    }
  }
}

//--------------------------------------------------------------------------
// GetValue (public)
//--------------------------------------------------------------------------
/**
 * <p>Get TStringVector 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Run Title
 * \param value TStringVector return value
 * \param ok flag telling if the TStringVector value was found
 */
void TMusrRunHeader::GetValue(TString pathName, TStringVector &value, Bool_t &ok)
{
  ok = false;

  for (UInt_t i=0; i<fStringVectorObj.size(); i++) {
    if (fStringVectorObj[i].GetPathName() == pathName) {
      value = fStringVectorObj[i].GetValue();
      ok = true;
    }
  }
}

//--------------------------------------------------------------------------
// GetValue (public)
//--------------------------------------------------------------------------
/**
 * <p>Get TIntVector 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Run Title
 * \param value TIntVector return value
 * \param ok flag telling if the TIntVector value was found
 */
void TMusrRunHeader::GetValue(TString pathName, TIntVector &value, Bool_t &ok)
{
  ok = false;

  for (UInt_t i=0; i<fIntVectorObj.size(); i++) {
    if (fIntVectorObj[i].GetPathName() == pathName) {
      value = fIntVectorObj[i].GetValue();
      ok = true;
    }
  }
}

//--------------------------------------------------------------------------
// GetValue (public)
//--------------------------------------------------------------------------
/**
 * <p>Get TDoubleVector 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Run Title
 * \param value TDoubleVector return value
 * \param ok flag telling if the TDoubleVector value was found
 */
void TMusrRunHeader::GetValue(TString pathName, TDoubleVector &value, Bool_t &ok)
{
  ok = false;

  for (UInt_t i=0; i<fDoubleVectorObj.size(); i++) {
    if (fDoubleVectorObj[i].GetPathName() == pathName) {
      value = fDoubleVectorObj[i].GetValue();
      ok = true;
    }
  }
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set map which links the entry to a data type.
 *
 * \param map pointer to the maps.
 */
void TMusrRunHeader::SetMap(TMap* map)
{
  // perform some checks
  if (map == 0) {
    cerr << endl << ">> TMusrRunHeader::SetMap(): **ERROR** map points to 0." << endl;
    return;
  }

  TMapIter iter(map);
  TObjString *key;

  // remove all previous entries
  fMap.DeleteAll();
  // set the name
  fMap.SetName("__map");

  while ((key = (TObjString*) iter.Next())) {
    fMap.Add(key, (TObjString*)map->GetValue(key->GetString()));
  }
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set TString 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Run Title
 * \param value of the entry
 * \param addAdminInfo if true add map information (needed for writting), otherwise do not add map information (needed for reading, since there the map is already present)
 */
void TMusrRunHeader::Set(TString pathName, TString value, Bool_t addAdminInfo)
{
  // check if pathName is already set, and if not add it as a new entry
  UInt_t i=0;
  for (i=0; i<fStringObj.size(); i++) {
    if (!fStringObj[i].GetPathName().CompareTo(pathName, TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** " << pathName.Data() << " already exists, will replace it." << endl;
      fStringObj[i].SetType("TString");
      fStringObj[i].SetValue(value);
      break;
    }
  }

  // if not found in the previous loop, it is a new object
  if (i == fStringObj.size()) {
    TMusrRunObject<TString> obj(pathName, "TString", value);
    fStringObj.push_back(obj);

    // check if new folder, and if yes keep it
    TString path(""), name("");
    SplitPathName(pathName, path, name);
    if (!FolderPresent(path))
      fFolder.push_back(path);

    // add the mapping information if needed
    if (addAdminInfo) {
      fMap.Add(new TObjString(pathName), new TObjString("TString"));

    }
  }
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set Int_t 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Run number
 * \param value of the entry
 * \param addMap if true add map information (needed for writting), otherwise do not add map information (needed for reading, since there the map is already present)
 */
void TMusrRunHeader::Set(TString pathName, Int_t value, Bool_t addMap)
{
  // check if pathName is already set, and if not add it as a new entry
  UInt_t i=0;
  for (i=0; i<fIntObj.size(); i++) {
    if (!fIntObj[i].GetPathName().CompareTo(pathName, TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** " << pathName.Data() << " already exists, will replace it." << endl;
      fIntObj[i].SetType("Int_t");
      fIntObj[i].SetValue(value);
      break;
    }
  }

  // if not found in the previous loop, it is a new object
  if (i == fIntObj.size()) {
    TMusrRunObject<Int_t> obj(pathName, "Int_t", value);
    fIntObj.push_back(obj);

    // check if new folder, and if yes keep it
    TString path(""), name("");
    SplitPathName(pathName, path, name);
    if (!FolderPresent(path))
      fFolder.push_back(path);

    // add the mapping information if needed
    if (addMap)
      fMap.Add(new TObjString(pathName), new TObjString("Int_t"));
  }
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set Double_t 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/DoubleValue
 * \param value of the entry
 * \param addMap if true add map information (needed for writting), otherwise do not add map information (needed for reading, since there the map is already present)
 */
void TMusrRunHeader::Set(TString pathName, Double_t value, Bool_t addMap)
{
  // check if pathName is already set, and if not add it as a new entry
  UInt_t i=0;
  for (i=0; i<fDoubleObj.size(); i++) {
    if (!fDoubleObj[i].GetPathName().CompareTo(pathName, TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** " << pathName.Data() << " already exists, will replace it." << endl;
      fDoubleObj[i].SetType("Double_t");
      fDoubleObj[i].SetValue(value);
      break;
    }
  }

  // if not found in the previous loop, it is a new object
  if (i == fDoubleObj.size()) {
    TMusrRunObject<Double_t> obj(pathName, "Double_t", value);
    fDoubleObj.push_back(obj);

    // check if new folder, and if yes keep it
    TString path(""), name("");
    SplitPathName(pathName, path, name);
    if (!FolderPresent(path))
      fFolder.push_back(path);

    // add the mapping information if needed
    if (addMap)
      fMap.Add(new TObjString(pathName), new TObjString("Double_t"));
  }
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set TMusrRunPhysicalQuantity 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Muon Beam Momentum
 * \param value of the entry
 * \param addMap if true add map information (needed for writting), otherwise do not add map information (needed for reading, since there the map is already present)
 */
void TMusrRunHeader::Set(TString pathName, TMusrRunPhysicalQuantity value, Bool_t addMap)
{
  // check if pathName is already set, and if not add it as a new entry
  UInt_t i=0;
  for (i=0; i<fMusrRunPhysQuantityObj.size(); i++) {
    if (!fMusrRunPhysQuantityObj[i].GetPathName().CompareTo(pathName, TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** " << pathName.Data() << " already exists, will replace it." << endl;
      fMusrRunPhysQuantityObj[i].SetType("TMusrRunHeader");
      fMusrRunPhysQuantityObj[i].SetValue(value);
      break;
    }
  }

  // if not found in the previous loop, it is a new object
  if (i == fMusrRunPhysQuantityObj.size()) {
    TMusrRunObject<TMusrRunPhysicalQuantity> obj(pathName, "TMusrRunPhysicalQuantity", value);
    fMusrRunPhysQuantityObj.push_back(obj);

    // check if new folder, and if yes keep it
    TString path(""), name("");
    SplitPathName(pathName, path, name);
    if (!FolderPresent(path))
      fFolder.push_back(path);

    // add the mapping information if needed
    if (addMap)
      fMap.Add(new TObjString(pathName), new TObjString("TMusrRunPhysicalQuantity"));
  }
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set TStringVector 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Histo names
 * \param value of the entry
 * \param addMap if true add map information (needed for writting), otherwise do not add map information (needed for reading, since there the map is already present)
 */
void TMusrRunHeader::Set(TString pathName, TStringVector value, Bool_t addMap)
{
  // check if pathName is already set, and if not add it as a new entry
  UInt_t i=0;
  for (i=0; i<fStringVectorObj.size(); i++) {
    if (!fStringVectorObj[i].GetPathName().CompareTo(pathName, TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** " << pathName.Data() << " already exists, will replace it." << endl;
      fStringVectorObj[i].SetType("TStringVector");
      fStringVectorObj[i].SetValue(value);
      break;
    }
  }

  // if not found in the previous loop, it is a new object
  if (i == fStringVectorObj.size()) {
    TMusrRunObject<TStringVector> obj(pathName, "TStringVector", value);
    fStringVectorObj.push_back(obj);

    // check if new folder, and if yes keep it
    TString path(""), name("");
    SplitPathName(pathName, path, name);
    if (!FolderPresent(path))
      fFolder.push_back(path);

    // add the mapping information if needed
    if (addMap)
      fMap.Add(new TObjString(pathName), new TObjString("TStringVector"));
  }
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set TIntVector 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Time Zero Bin
 * \param value of the entry
 * \param addMap if true add map information (needed for writting), otherwise do not add map information (needed for reading, since there the map is already present)
 */
void TMusrRunHeader::Set(TString pathName, TIntVector value, Bool_t addMap)
{
  // check if pathName is already set, and if not add it as a new entry
  UInt_t i=0;
  for (i=0; i<fIntVectorObj.size(); i++) {
    if (!fIntVectorObj[i].GetPathName().CompareTo(pathName, TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** " << pathName.Data() << " already exists, will replace it." << endl;
      fIntVectorObj[i].SetType("TIntVector");
      fIntVectorObj[i].SetValue(value);
      break;
    }
  }

  // if not found in the previous loop, it is a new object
  if (i == fIntVectorObj.size()) {
    TMusrRunObject<TIntVector> obj(pathName, "TIntVector", value);
    fIntVectorObj.push_back(obj);

    // check if new folder, and if yes keep it
    TString path(""), name("");
    SplitPathName(pathName, path, name);
    if (!FolderPresent(path))
      fFolder.push_back(path);

    // add the mapping information if needed
    if (addMap)
      fMap.Add(new TObjString(pathName), new TObjString("TIntVector"));
  }
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set TDoubleVector 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Time Zero Bin
 * \param value of the entry
 * \param addMap if true add map information (needed for writting), otherwise do not add map information (needed for reading, since there the map is already present)
 */
void TMusrRunHeader::Set(TString pathName, TDoubleVector value, Bool_t addMap)
{
  // check if pathName is already set, and if not add it as a new entry
  UInt_t i=0;
  for (i=0; i<fDoubleVectorObj.size(); i++) {
    if (!fDoubleVectorObj[i].GetPathName().CompareTo(pathName, TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** " << pathName.Data() << " already exists, will replace it." << endl;
      fDoubleVectorObj[i].SetType("TDoubleVector");
      fDoubleVectorObj[i].SetValue(value);
      break;
    }
  }

  // if not found in the previous loop, it is a new object
  if (i == fDoubleVectorObj.size()) {
    TMusrRunObject<TDoubleVector> obj(pathName, "TDoubleVector", value);
    fDoubleVectorObj.push_back(obj);

    // check if new folder, and if yes keep it
    TString path(""), name("");
    SplitPathName(pathName, path, name);
    if (!FolderPresent(path))
      fFolder.push_back(path);

    // add the mapping information if needed
    if (addMap)
      fMap.Add(new TObjString(pathName), new TObjString("TDoubleVector"));
  }
}

//--------------------------------------------------------------------------
// ExtractHeaderInformation (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param headerInfo
 * \param requestedPath
 */
Bool_t TMusrRunHeader::ExtractHeaderInformation(TObjArray *headerInfo, TString requestedPath)
{
  // make sure fMap has been set
  if (fMap.IsEmpty()) {
    cerr << endl << ">> TMusrRunHeader::ExtractHeaderInformation(): **FATAL ERROR** no fMap present.";
    cerr << endl << ">> Either the file is corrupted, or the reader routine forgot the read the __map entry first.";
    cerr << endl << endl;
    return false;
  }

  TString headerName(""), name(""), pathName(""), str(""), strValue(""), type("");
  TObjString *ostr = 0;
  TObjArray *tokens = 0;
  Ssiz_t idx1, idx2;
  Int_t  intValue;
  Double_t dval;
  Bool_t noMap = true;

  // go through all entries of this header information from the MUSR-ROOT file
  for (Int_t i=0; i<headerInfo->GetEntries(); i++) {
    ostr = dynamic_cast<TObjString*>(headerInfo->At(i));
    str = ostr->GetString();

    // get header name
    idx1 = str.First('-');
    idx2 = str.First(':');
    headerName = TString("");
    for (Int_t j=idx1+2; j<idx2; j++)
      headerName += str[j];

    // get the name
    idx1 = str.First('-');
    idx2 = str.First(':');
    name = TString("");
    for (Int_t i=idx1+2; i<idx2; i++)
      name += str[i];
    // get the 'value'
    strValue = TString("");
    for (Int_t i=idx2+2; i<str.Length(); i++)
      strValue += str[i];
    pathName = requestedPath + TString("/") + name;

    // get type from map
    type = ((TObjString*)fMap(pathName))->GetString();

    if (type == "TString") {
      Set(pathName, strValue, noMap);
    } else if (type == "Int_t") {
      intValue = strValue.Atoi();
      Set(pathName, intValue, noMap);
    } else if (type == "Double_t") {
      dval = strValue.Atof();
      Set(pathName, dval, noMap);
    } else if (type == "TMusrRunPhysicalQuantity") {
      TMusrRunPhysicalQuantity prop;
      prop.SetLabel(name);

      // 1st get the description if present
      tokens = strValue.Tokenize(";");
      if (tokens == 0) {
        cerr << endl << ">> **ERROR** Couldn't tokenize entry in Bool_t TMusrRunHeader::ExtractHeaderInformation(TObjArray *headerInfo, TString requestedPath)" << endl;
        return false;
      }

      switch (tokens->GetEntries()) {
      case 2:
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (!str.Contains("SP:")) { // make sure that it is not a demand value token
          prop.SetDescription(str);
        }
        break;
      case 3:
        ostr = dynamic_cast<TObjString*>(tokens->At(2));
        str = ostr->GetString();
        break;
      default:
        break;
      }

      if (tokens) {
        delete tokens;
        tokens = 0;
      }

      // 2nd collect all the other properties, this is easier when first a potential description is removed
      idx1 = strValue.Last(';');
      if (idx1 > 0) {
        TString last("");
        for (Int_t i=idx1+2; i<strValue.Length(); i++)
          last += strValue[i];
        // check if last is <description> or SP: <demand>
        if (!last.Contains("SP:")) {
          str = "";
          for (Int_t i=0; i<idx1; i++)
            str += strValue[i];
          strValue = str;
        }
      }

      tokens = strValue.Tokenize(" +;");
      if (tokens == 0) {
        cerr << endl << ">> **ERROR** Couldn't tokenize entry in Bool_t TMusrRunHeader::ExtractHeaderInformation(TObjArray *headerInfo, TString requestedPath)" << endl;
        return false;
      }

      switch (tokens->GetEntries()) {
      case 2: // <val> <unit>
        ostr = dynamic_cast<TObjString*>(tokens->At(0));
        str = ostr->GetString();
        prop.SetValue(str.Atof());
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        prop.SetUnit(str);
        break;
      case 4: // <val> +- <err> <unit>, or <val> <unit>; SP: <demand>
        ostr = dynamic_cast<TObjString*>(tokens->At(0));
        str = ostr->GetString();
        prop.SetValue(str.Atof());
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str == "-") { // <val> +- <err> <unit>
          ostr = dynamic_cast<TObjString*>(tokens->At(2));
          str = ostr->GetString();
          prop.SetError(str.Atof());
          ostr = dynamic_cast<TObjString*>(tokens->At(3));
          str = ostr->GetString();
          prop.SetUnit(str);
        } else { // <val> <unit>; SP: <demand>
          prop.SetUnit(str);
          ostr = dynamic_cast<TObjString*>(tokens->At(3));
          str = ostr->GetString();
          prop.SetDemand(str.Atof());
        }
        break;
      case 6: // <val> +- <err> <unit>; SP: <demand>
        ostr = dynamic_cast<TObjString*>(tokens->At(0));
        str = ostr->GetString();
        prop.SetValue(str.Atof());
        ostr = dynamic_cast<TObjString*>(tokens->At(2));
        str = ostr->GetString();
        prop.SetError(str.Atof());
        ostr = dynamic_cast<TObjString*>(tokens->At(3));
        str = ostr->GetString();
        prop.SetUnit(str);
        ostr = dynamic_cast<TObjString*>(tokens->At(5));
        str = ostr->GetString();
        prop.SetDemand(str.Atof());
        break;
      default:
        break;
      }

      if (tokens) {
        delete tokens;
        tokens = 0;
      }

      Set(pathName, prop, noMap);
    } else if (type == "TStringVector") {
      TStringVector svec;
      tokens = strValue.Tokenize(";");
      if (tokens == 0) {
        cerr << endl << ">> **ERROR** Couldn't tokenize entry in Bool_t TMusrRunHeader::ExtractHeaderInformation(TObjArray *headerInfo, TString requestedPath)" << endl;
        return false;
      }
      for (Int_t i=0; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();
        str.Remove(TString::kBoth, ' ');
        svec.push_back(str);
      }
      if (tokens) {
        delete tokens;
        tokens = 0;
      }
      Set(pathName, svec, noMap);
    } else if (type == "TIntVector") {
      TIntVector ivec;
      tokens = strValue.Tokenize(";");
      if (tokens == 0) {
        cerr << endl << ">> **ERROR** Couldn't tokenize entry in Bool_t TMusrRunHeader::ExtractHeaderInformation(TObjArray *headerInfo, TString requestedPath)" << endl;
        return false;
      }
      for (Int_t i=0; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        ivec.push_back(ostr->GetString().Atoi());
      }
      if (tokens) {
        delete tokens;
        tokens = 0;
      }
      Set(pathName, ivec, noMap);
    } else if (type == "TDoubleVector") {
      TDoubleVector dvec;
      tokens = strValue.Tokenize(";");
      if (tokens == 0) {
        cerr << endl << ">> **ERROR** Couldn't tokenize entry in Bool_t TMusrRunHeader::ExtractHeaderInformation(TObjArray *headerInfo, TString requestedPath)" << endl;
        return false;
      }
      for (Int_t i=0; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        dvec.push_back(ostr->GetString().Atoi());
      }
      if (tokens) {
        delete tokens;
        tokens = 0;
      }
      Set(pathName, dvec, noMap);
    }
  }

  return true;
}

//--------------------------------------------------------------------------
// DumpHeader (public)
//--------------------------------------------------------------------------
/**
 * <p>
 */
void TMusrRunHeader::DumpHeader()
{
  cout << endl << "***************************************";
  cout << endl << "header info of file : " << fFileName;
  cout << endl << "***************************************";

  TString tstr(""), fmt(""), path(""), name("");
  TMusrRunPhysicalQuantity prop;

  // go through all folders
  for (UInt_t i=0; i<fFolder.size(); i++) {
    cout << endl << fFolder[i] << ":";

    // TString header info
    for (UInt_t j=0; j<fStringObj.size(); j++) {
      if (fStringObj[j].GetPathName().Contains(fFolder[i])) {
        SplitPathName(fStringObj[j].GetPathName(), path, name);
        cout << endl << "  " << name.Data() << ": " << fStringObj[j].GetValue().Data();
      }
    }

    // Int_t header info
    for (UInt_t j=0; j<fIntObj.size(); j++) {
      if (fIntObj[j].GetPathName().Contains(fFolder[i])) {
        SplitPathName(fIntObj[j].GetPathName(), path, name);
        cout << endl << "  " << name.Data() << ": " << fIntObj[j].GetValue();
      }
    }

    // Double_t header info
    for (UInt_t j=0; j<fDoubleObj.size(); j++) {
      if (fDoubleObj[j].GetPathName().Contains(fFolder[i])) {
        SplitPathName(fDoubleObj[j].GetPathName(), path, name);
        cout << endl << "  " << name.Data() << ": " << fDoubleObj[j].GetValue();
      }
    }

    // TMusrRunPhysicalQuantity
    for (UInt_t j=0; j<fMusrRunPhysQuantityObj.size(); j++) {
      if (fMusrRunPhysQuantityObj[j].GetPathName().Contains(fFolder[i])) {
        tstr = "";
        // depending on the given properties dump
        prop = fMusrRunPhysQuantityObj[j].GetValue();
        Int_t digit, digit_d;
        if ((prop.GetDemand() != MRRH_UNDEFINED) && (prop.GetValue() != MRRH_UNDEFINED) && (prop.GetError() != MRRH_UNDEFINED) &&
            (prop.GetUnit() != "n/a")) {
          digit = GetDecimalPlace(prop.GetError());
          digit_d = GetLeastSignificantDigit(prop.GetDemand());
          if (prop.GetDescription() != "n/a") {
            fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit, digit_d);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(), prop.GetDemand(), prop.GetDescription().Data());
          } else {
            fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf", digit, digit, digit_d);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(), prop.GetDemand());
          }
        } else if ((prop.GetDemand() == MRRH_UNDEFINED) && (prop.GetValue() != MRRH_UNDEFINED) && (prop.GetError() != MRRH_UNDEFINED) &&
                   (prop.GetUnit() != "n/a")) {
          digit = GetDecimalPlace(prop.GetError());
          if (prop.GetDescription() != "n/a") {
            fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; %%s", digit, digit);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(), prop.GetDescription().Data());
          } else {
            fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s", digit, digit);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data());
          }
        } else if ((prop.GetDemand() == MRRH_UNDEFINED) && (prop.GetValue() != MRRH_UNDEFINED) && (prop.GetError() == MRRH_UNDEFINED) &&
                   (prop.GetUnit() != "n/a")) {
          digit = GetLeastSignificantDigit(prop.GetValue());
          if (prop.GetDescription() != "n/a") {
            fmt.Form("%%s: %%.%dlf %%s; %%s", digit);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data(), prop.GetDescription().Data());
          } else {
            fmt.Form("%%s: %%.%dlf %%s", digit);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data());
          }
        } else if ((prop.GetDemand() != MRRH_UNDEFINED) && (prop.GetValue() != MRRH_UNDEFINED) && (prop.GetError() == MRRH_UNDEFINED) &&
                   (prop.GetUnit() != "n/a")) {
          digit = GetLeastSignificantDigit(prop.GetValue());
          digit_d = GetLeastSignificantDigit(prop.GetDemand());
          if (prop.GetDescription() != "n/a") {
            fmt.Form("%%s: %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit_d);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data(), prop.GetDemand(), prop.GetDescription().Data());
          } else {
            fmt.Form("%%s: %%.%dlf %%s; SP: %%.%dlf", digit, digit_d);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data(), prop.GetDemand());
          }
        }
        cout << endl << "  " << tstr.Data();
      }
    }

    // TStringVector
    for (UInt_t j=0; j<fStringVectorObj.size(); j++) {
      if (fStringVectorObj[j].GetPathName().Contains(fFolder[i])) {
        SplitPathName(fStringVectorObj[j].GetPathName(), path, name);
        cout << endl << "  " << name.Data() << ": ";
        TStringVector vstr = fStringVectorObj[j].GetValue();
        for (UInt_t k=0; k<vstr.size()-1; k++)
          cout << vstr[k] << "; ";
        cout << vstr[vstr.size()-1];
        vstr.clear();
      }
    }

    // TIntVector
    for (UInt_t j=0; j<fIntVectorObj.size(); j++) {
      if (fIntVectorObj[j].GetPathName().Contains(fFolder[i])) {
        SplitPathName(fIntVectorObj[j].GetPathName(), path, name);
        cout << endl << "  " << name.Data() << ": ";
        TIntVector ivec = fIntVectorObj[j].GetValue();
        for (UInt_t k=0; k<ivec.size()-1; k++)
          cout << ivec[k] << "; ";
        cout << ivec[ivec.size()-1];
        ivec.clear();
      }
    }

    // TDoubleVector
    for (UInt_t j=0; j<fDoubleVectorObj.size(); j++) {
      if (fDoubleVectorObj[j].GetPathName().Contains(fFolder[i])) {
        SplitPathName(fDoubleVectorObj[j].GetPathName(), path, name);
        cout << endl << "  " << name.Data() << ": ";
        TDoubleVector dvec = fDoubleVectorObj[j].GetValue();
        for (UInt_t k=0; k<dvec.size()-1; k++)
          cout << dvec[k] << "; ";
        cout << dvec[dvec.size()-1];
        dvec.clear();
      }
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
void TMusrRunHeader::DrawHeader()
{
  TPaveText *pt;
  TCanvas *ca;

  ca = new TCanvas("MusrRoot RunHeader","MusrRoot RunHeader", 147,37,699,527);
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
UInt_t TMusrRunHeader::GetDecimalPlace(Double_t val)
{
  UInt_t digit = 0;

  if (val < 1.0) {
    UInt_t count=1;
    do {
      val *= 10.0;
      if (val > 1.0)
        digit = count;
      count++;
    } while ((digit == 0) && (count < 20));
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
UInt_t TMusrRunHeader::GetLeastSignificantDigit(Double_t val) const
{
  char cstr[1024];
  snprintf(cstr, sizeof(cstr), "%.10lf", val);

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
// SplitPathName (private)
//--------------------------------------------------------------------------
/**
 * <p>splits a path name string into the path and the name.
 *
 * \param pathName path name to be split
 * \param path of pathName
 * \param name of pathName
 */
void TMusrRunHeader::SplitPathName(TString pathName, TString &path, TString &name)
{
  path = TString("");
  name = TString("");
  Ssiz_t idx = pathName.Last('/');

  for (Int_t i=0; i<idx; i++)
    path += pathName[i];

  for (Int_t i=idx+1; i<pathName.Length(); i++)
    name += pathName[i];
}

//--------------------------------------------------------------------------
// FolderPresent (private)
//--------------------------------------------------------------------------
/**
 * <p>checks if a folder is present in the internal administration
 *
 * \param path of the folder
 */
Bool_t TMusrRunHeader::FolderPresent(TString &path)
{
  for (UInt_t i=0; i<fFolder.size(); i++)
    if (fFolder[i].Contains(path))
      return true;
  return false;
}

// end ---------------------------------------------------------------------
