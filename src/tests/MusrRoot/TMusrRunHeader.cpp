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
  fDemand = MRH_UNDEFINED;
  fValue  = MRH_UNDEFINED;
  fError  = MRH_UNDEFINED;
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
  fError = MRH_UNDEFINED;
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
  fDemand = MRH_UNDEFINED;
  fError  = MRH_UNDEFINED;
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
  fError = MRH_UNDEFINED;
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
  fDemand = MRH_UNDEFINED;
  fValue = value;
  fError = MRH_UNDEFINED;
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
  Init(TString(fileName));
}

//--------------------------------------------------------------------------
// Init (private)
//--------------------------------------------------------------------------
/**
  * <p>Initializer
  *
  * \param fileName file name of the caller.
  */
void TMusrRunHeader::Init(TString fileName)
{
  fFileName = fileName;
  fVersion = TString("$Id$");
  Set("RunInfo/Version", fVersion);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
  * <p>Destructor.
  */
TMusrRunHeader::~TMusrRunHeader()
{
  CleanUp();
}

//--------------------------------------------------------------------------
// CleanUp (private)
//--------------------------------------------------------------------------
/**
  * <p>Clean up internal stuff.
  */
void TMusrRunHeader::CleanUp()
{
  fStringObj.clear();
  fIntObj.clear();
  fDoubleObj.clear();
  fMusrRunPhysQuantityObj.clear();
  fStringVectorObj.clear();
  fIntVectorObj.clear();
  fDoubleVectorObj.clear();

  fPathNameOrder.clear();
}

//--------------------------------------------------------------------------
// FillFolder (public)
//--------------------------------------------------------------------------
/**
 * <p>Fills the RunHeader folder. This is needed to write it to a ROOT file.
 * It walks through all information and attaches it to folder.
 *
 * \param folder to be filled
 */
Bool_t TMusrRunHeader::FillFolder(TFolder *folder)
{
  TObjArray  *oarray, *tokens;
  vector<TObjArray*> content;
  TObjString ostr;
  TString path, name, pathName;

  if (folder == 0) {
    cerr << endl << ">> TMusrRunHeader::FillFolder(): **ERROR** folder == 0!!" << endl;
    return false;
  }

  folder->SetOwner(); // folder takes ownership of all added objects!

  for (UInt_t i=0; i<fPathNameOrder.size(); i++) {
    tokens = fPathNameOrder[i].Tokenize('/');
    if (tokens == 0) {
      cout << endl << ">> TMusrRunHeader::FillFolder(): **ERROR** couldn't tokenize string " << fPathNameOrder[i] << endl;
      return false;
    }

    Int_t idx;
    if (tokens->GetEntries() == 2) { // <path>/<value>
      path = ((TObjString*)tokens->At(0))->GetString();
      idx = ObjectPresent(content, path);
      if (idx == -1) {
        // add object array
        oarray = new TObjArray();
        oarray->SetName(path);
        content.push_back(oarray);
        idx = content.size()-1;
      }
      // add value
      ostr = GetHeaderString(i);
      content[idx]->AddLast(ostr.Clone());
    } else { // <path>/../<path>/<value>
      path = ((TObjString*)tokens->At(0))->GetString();
      idx = ObjectPresent(content, path);
      if (idx == -1) {
        // add object array
        oarray = new TObjArray();
        oarray->SetName(path);
        content.push_back(oarray);
        idx = content.size()-1;
      }

      // add necessary sub object arrays
      pathName = fPathNameOrder[i];
      RemoveFirst(pathName, '/');
      AddSubTrees(content[idx], pathName);

      // get header string
      ostr = GetHeaderString(i);

      // set object string on the right position within content
      SetSubTreeObject(content[idx], ostr, i);
    }

    // clean up
    if (tokens) {
      delete tokens;
      tokens = 0;
    }
  }

  // fill folder with all run header information
  for (UInt_t i=0; i<content.size(); i++) {
    folder->Add(content[i]);
  }

  // clean up
  content.clear();

  return true;
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
 * <p>Set TString 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Run Title
 * \param value of the entry
 */
void TMusrRunHeader::Set(TString pathName, TString value)
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
    // feed object
    TMusrRunObject<TString> obj(pathName, "TString", value);
    fStringObj.push_back(obj);

    // feed path-name to keep track of the order
    fPathNameOrder.push_back(pathName);
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
 */
void TMusrRunHeader::Set(TString pathName, Int_t value)
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
    // feed object
    TMusrRunObject<Int_t> obj(pathName, "Int_t", value);
    fIntObj.push_back(obj);

    // feed path-name to keep track of the order
    fPathNameOrder.push_back(pathName);
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
 */
void TMusrRunHeader::Set(TString pathName, Double_t value)
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
    // feed object
    TMusrRunObject<Double_t> obj(pathName, "Double_t", value);
    fDoubleObj.push_back(obj);

    // feed path-name to keep track of the order
    fPathNameOrder.push_back(pathName);
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
 */
void TMusrRunHeader::Set(TString pathName, TMusrRunPhysicalQuantity value)
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
    // feed object
    TMusrRunObject<TMusrRunPhysicalQuantity> obj(pathName, "TMusrRunPhysicalQuantity", value);
    fMusrRunPhysQuantityObj.push_back(obj);

    // feed path-name to keep track of the order
    fPathNameOrder.push_back(pathName);
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
 */
void TMusrRunHeader::Set(TString pathName, TStringVector value)
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
    // feed object
    TMusrRunObject<TStringVector> obj(pathName, "TStringVector", value);
    fStringVectorObj.push_back(obj);

    // feed path-name to keep track of the order
    fPathNameOrder.push_back(pathName);
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
 */
void TMusrRunHeader::Set(TString pathName, TIntVector value)
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
    // feed object
    TMusrRunObject<TIntVector> obj(pathName, "TIntVector", value);
    fIntVectorObj.push_back(obj);

    // feed path-name to keep track of the order
    fPathNameOrder.push_back(pathName);
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
 */
void TMusrRunHeader::Set(TString pathName, TDoubleVector value)
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
    // feed object
    TMusrRunObject<TDoubleVector> obj(pathName, "TDoubleVector", value);
    fDoubleVectorObj.push_back(obj);

    // feed path-name to keep track of the order
    fPathNameOrder.push_back(pathName);
  }
}


//--------------------------------------------------------------------------
// ExtractAll (public)
//--------------------------------------------------------------------------
/**
 * <p>Reads all data from an open ROOT-file structure and feeds all the necessary
 * internal data objects.
 *
 * \param folder
 */
Bool_t TMusrRunHeader::ExtractAll(TFolder *folder)
{
  TIter next(folder->GetListOfFolders());
  TObjArray* entry;

  // clean up all internal structures - just in case this is called multiple times
  CleanUp();

  while ((entry = (TObjArray*)next())) {
    ExtractHeaderInformation(entry, entry->GetName());
  }
  return true;
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
  TString label(""), path(""), pathName(""), str(""), strValue(""), type("");
  TObjString *ostr = 0;
  TObjArray *tokens = 0;
  Ssiz_t idx1;
  Int_t  intValue;
  Double_t dval;

  // go through all entries of this header information from the MUSR-ROOT file
  for (Int_t i=0; i<headerInfo->GetEntries(); i++) {
    // check if entry is a TObjArray, i.e. a sub tree
    str = TString(headerInfo->At(i)->ClassName());
    if (str == "TObjArray") { // sub tree
      path = requestedPath + "/" + TString(headerInfo->At(i)->GetName());
      ExtractHeaderInformation((TObjArray*)headerInfo->At(i), path);
    } else { // handle all the rest, i.e. already data

      ostr = dynamic_cast<TObjString*>(headerInfo->At(i));
      str = ostr->GetString();

      // get the run header label
      label = GetLabel(str);
      if (label == "n/a")
        return false;

      // get the run header 'value'
      strValue = GetStrValue(str);
      if (strValue == "n/a")
        return false;

      // construct path name
      pathName = requestedPath + TString("/") + label;

      // get type from map
      type = GetType(str);

      if (type == "TString") {
        Set(pathName, strValue);
      } else if (type == "Int_t") {
        intValue = strValue.Atoi();
        Set(pathName, intValue);
      } else if (type == "Double_t") {
        dval = strValue.Atof();
        Set(pathName, dval);
      } else if (type == "TMusrRunPhysicalQuantity") {
        TMusrRunPhysicalQuantity prop;
        prop.SetLabel(label);

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

        Set(pathName, prop);
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
        Set(pathName, svec);
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
        Set(pathName, ivec);
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
        Set(pathName, dvec);
      }
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

  TString str(""), tstr(""), fmt(""), path(""), name(""), currentPath("");
  TMusrRunPhysicalQuantity prop;

  for (UInt_t i=0; i<fPathNameOrder.size(); i++) {
    // print path if new
    SplitPathName(fPathNameOrder[i], path, name);
    if (path != currentPath) {
      currentPath = path;
      cout << endl << currentPath;
    }

    // go through all objects and try to find it
    // 1st check TString
    for (UInt_t j=0; j<fStringObj.size(); j++) {
      if (fStringObj[j].GetPathName() == fPathNameOrder[i]) { // found correct object
        SplitPathName(fStringObj[j].GetPathName(), path, name);
        str.Form("  %03d - %s: %s -@%d", i, name.Data(), fStringObj[j].GetValue().Data(), MRH_TSTRING);
        cout << endl << str;
      }
    }
    // 2nd check Int_t
    for (UInt_t j=0; j<fIntObj.size(); j++) {
      if (fIntObj[j].GetPathName() == fPathNameOrder[i]) { // found correct object
        SplitPathName(fIntObj[j].GetPathName(), path, name);
        str.Form("  %03d - %s: %d -@%d", i, name.Data(), fIntObj[j].GetValue(), MRH_INT);
        cout << endl << str;
      }
    }
    // 3rd check Double_t
    for (UInt_t j=0; j<fDoubleObj.size(); j++) {
      if (fDoubleObj[j].GetPathName() == fPathNameOrder[i]) { // found correct object
        SplitPathName(fDoubleObj[j].GetPathName(), path, name);
        fmt.Form("  %%03d - %%s: %%.%dlf -@%%d", MRH_DOUBLE_PREC);
        str.Form(fmt, i, name.Data(), fDoubleObj[j].GetValue(), MRH_DOUBLE);
        cout << endl << str;
      }
    }
    // 4th check TMusrRunPhysicalQuantity
    for (UInt_t j=0; j<fMusrRunPhysQuantityObj.size(); j++) {
      if (fMusrRunPhysQuantityObj[j].GetPathName() == fPathNameOrder[i]) { // found correct object
        prop = fMusrRunPhysQuantityObj[j].GetValue();
        Int_t digit, digit_d;
        if ((prop.GetDemand() != MRH_UNDEFINED) && (prop.GetValue() != MRH_UNDEFINED) && (prop.GetError() != MRH_UNDEFINED) &&
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
        } else if ((prop.GetDemand() == MRH_UNDEFINED) && (prop.GetValue() != MRH_UNDEFINED) && (prop.GetError() != MRH_UNDEFINED) &&
                   (prop.GetUnit() != "n/a")) { // <value> +- <error> <unit> [; <description>]
          digit = GetDecimalPlace(prop.GetError());
          if (prop.GetDescription() != "n/a") {
            fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; %%s", digit, digit);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(), prop.GetDescription().Data());
          } else {
            fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s", digit, digit);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data());
          }
        } else if ((prop.GetDemand() == MRH_UNDEFINED) && (prop.GetValue() != MRH_UNDEFINED) && (prop.GetError() == MRH_UNDEFINED) &&
                   (prop.GetUnit() != "n/a")) { // <value> <unit> [; <description>]
          digit = GetLeastSignificantDigit(prop.GetValue());
          if (prop.GetDescription() != "n/a") {
            fmt.Form("%%s: %%.%dlf %%s; %%s", digit);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data(), prop.GetDescription().Data());
          } else {
            fmt.Form("%%s: %%.%dlf %%s", digit);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data());
          }
        } else if ((prop.GetDemand() != MRH_UNDEFINED) && (prop.GetValue() != MRH_UNDEFINED) && (prop.GetError() == MRH_UNDEFINED) &&
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
        str.Form("  %03d - %s -@%d", i, tstr.Data(), MRH_TMUSR_RUN_PHYSICAL_QUANTITY);
        cout << endl << str;
      }
    }
    // 5th check TStringVector
    for (UInt_t j=0; j<fStringVectorObj.size(); j++) {
      if (fStringVectorObj[j].GetPathName() == fPathNameOrder[i]) { // found correct object
        SplitPathName(fStringVectorObj[j].GetPathName(), path, name);
        str.Form("  %03d - %s: ", i, name.Data());
        TStringVector vstr = fStringVectorObj[j].GetValue();
        for (UInt_t k=0; k<vstr.size()-1; k++)
          str += vstr[k] + "; ";
        str += vstr[vstr.size()-1];
        str += " -@";
        str += MRH_TSTRING_VECTOR;
        cout << endl << str;
      }
    }
    // 6th check TIntVector
    for (UInt_t j=0; j<fIntVectorObj.size(); j++) {
      if (fIntVectorObj[j].GetPathName() == fPathNameOrder[i]) { // found correct object
        SplitPathName(fIntVectorObj[j].GetPathName(), path, name);
        str.Form("  %03d - %s: ", i, name.Data());
        TIntVector vint = fIntVectorObj[j].GetValue();
        for (UInt_t k=0; k<vint.size()-1; k++) {
          str += vint[k];
          str += "; ";
        }
        str += vint[vint.size()-1];
        str += " -@";
        str += MRH_INT_VECTOR;
        cout << endl << str;
      }
    }
    // 7th check TDoubleVector
    for (UInt_t j=0; j<fDoubleVectorObj.size(); j++) {
      if (fDoubleVectorObj[j].GetPathName() == fPathNameOrder[i]) { // found correct object
        SplitPathName(fDoubleVectorObj[j].GetPathName(), path, name);
        str.Form("  %03d - %s: ", i, name.Data());
        TDoubleVector dvec = fDoubleVectorObj[j].GetValue();
        TString subStr("");
        fmt.Form("%%.%dlf", MRH_DOUBLE_PREC);
        for (UInt_t k=0; k<dvec.size()-1; k++) {
          subStr.Form(fmt, dvec[k]);
          str += subStr;
          str += "; ";
        }
        subStr.Form(fmt, dvec.size()-1);
        str += subStr;
        str += " -@";
        str += MRH_DOUBLE_VECTOR;
        cout << endl << str;
      }
    }
  }

  cout << endl;
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
// GetLabel (private)
//--------------------------------------------------------------------------
/**
 * <p>extracts form the run header line, as written to the MusrROOT file, the run header label, e.g. Run Number.
 *
 * \param str runHeader string as written to the MusrROOT file.
 */
TString TMusrRunHeader::GetLabel(TString str)
{
  // get run header label
  // the string 'str' should have the structure
  // <number> - <runHeader label>: <runHeader string value> -@<encoded data type>

  TString label = TString("n/a");
  Ssiz_t idx1 = str.First('-');
  Ssiz_t idx2 = str.First(':');
  if ((idx1 == -1) || (idx2 == -1)) {
    cerr << endl << ">> TMusrRunHeader::GetLabel(): **ERROR** str='" << str << "', seems not correctly encoded." << endl;
    return label;
  }

  label = TString("");
  for (Int_t j=idx1+2; j<idx2; j++)
    label += str[j];

  return label;
}

//--------------------------------------------------------------------------
// GetStrValue (private)
//--------------------------------------------------------------------------
/**
 * <p>extracts form the run header line, as written to the MusrROOT file, the run header str value, e.g. 557 (for Run Number).
 *
 * \param str runHeader string as written to the MusrROOT file.
 */
TString TMusrRunHeader::GetStrValue(TString str)
{
  // get run header string value
  // the string 'str' should have the structure
  // <number> - <runHeader label>: <runHeader string value> -@<encoded data type>

  TString strValue = TString("n/a");
  Ssiz_t idx1 = str.First(':');
  Ssiz_t idx2 = str.Last('-');
  if ((idx1 == -1) || (idx2 == -1)) {
    cerr << endl << ">> TMusrRunHeader::GetStrValue(): **ERROR** str='" << str << "', seems not correctly encoded." << endl;
    return strValue;
  }

  strValue = TString("");
  for (Int_t j=idx1+2; j<idx2-1; j++)
    strValue += str[j];

  return strValue;
}

//--------------------------------------------------------------------------
// GetType (private)
//--------------------------------------------------------------------------
/**
 * <p>extracts form the run header line, as written to the MusrROOT file, the encoded type and retruns it.
 *
 * \param str runHeader string with encoded type
 */
TString TMusrRunHeader::GetType(TString str)
{
  TString result = "n/a";

  // the string 'str' should have the structure
  // <number> - <runHeader label>: <runHeader string value> -@<encoded data type>
  Ssiz_t pos = str.Last('@');

  if (pos == -1) { // i.e. NOT found
    cerr << endl << ">> TMusrRunHeader::GetType(): **ERROR** str=" << str << " seems to be an invalid MusrROOT run header string." << endl;
    return result;
  }

  // filter out the encoded type declaration, i.e. -@<num>, where <num> is the encoded type
  TString typeStr(str);
  typeStr.Remove(0, pos+1);
  Int_t typeVal;
  if (!typeStr.IsDigit()) {
    cerr << endl << ">> TMusrRunHeader::GetType(): **ERROR** typeStr=" << typeStr << " is not supported." << endl;
    return result;
  }

  typeVal = typeStr.Atoi();

  switch (typeVal) {
  case MRH_TSTRING:
    result = "TString";
    break;
  case MRH_INT:
    result = "Int_t";
    break;
  case MRH_DOUBLE:
    result = "Double_t";
    break;
  case MRH_TMUSR_RUN_PHYSICAL_QUANTITY:
    result = "TMusrRunPhysicalQuantity";
    break;
  case MRH_TSTRING_VECTOR:
    result = "TStringVector";
    break;
  case MRH_INT_VECTOR:
    result = "TIntVector";
    break;
  case MRH_DOUBLE_VECTOR:
    result = "TDoubleVector";
    break;
  default:
    cerr << endl << ">> TMusrRunHeader::GetType(): **ERROR** found unsupport type encoded with: " << typeVal << "." << endl;
    break;
  }

  return result;
}

//--------------------------------------------------------------------------
// ObjectPresent (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param content
 * \param path
 */
Int_t TMusrRunHeader::ObjectPresent(vector<TObjArray*> &content, TString &path)
{
  for (UInt_t i=0; i<content.size(); i++) {
    if (content[i]->GetName() == path) {
      return (Int_t)i;
    }
  }

  return -1;
}

//--------------------------------------------------------------------------
// GetHeaderString (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param pathName
 */
TObjString TMusrRunHeader::GetHeaderString(UInt_t idx)
{
  TObjString tostr("n/a");

  TString str(""), path(""), name(""), fmt(""), tstr("");
  TMusrRunPhysicalQuantity prop;

  // go through all objects and try to find it
  // 1st check TString
  for (UInt_t j=0; j<fStringObj.size(); j++) {
    if (fStringObj[j].GetPathName() == fPathNameOrder[idx]) { // found correct object
      SplitPathName(fStringObj[j].GetPathName(), path, name);
      str.Form("%03d - %s: %s -@%d", idx, name.Data(), fStringObj[j].GetValue().Data(), MRH_TSTRING);
      tostr.SetString(str);
    }
  }
  // 2nd check Int_t
  for (UInt_t j=0; j<fIntObj.size(); j++) {
    if (fIntObj[j].GetPathName() == fPathNameOrder[idx]) { // found correct object
      SplitPathName(fIntObj[j].GetPathName(), path, name);
      str.Form("%03d - %s: %d -@%d", idx, name.Data(), fIntObj[j].GetValue(), MRH_INT);
      tostr.SetString(str);
    }
  }
  // 3rd check Double_t
  for (UInt_t j=0; j<fDoubleObj.size(); j++) {
    if (fDoubleObj[j].GetPathName() == fPathNameOrder[idx]) { // found correct object
      SplitPathName(fDoubleObj[j].GetPathName(), path, name);
      fmt.Form("%%03d - %%s: %%.%dlf -@%%d", MRH_DOUBLE_PREC);
      str.Form(fmt, idx, name.Data(), fDoubleObj[j].GetValue(), MRH_DOUBLE);
      tostr.SetString(str);
    }
  }
  // 4th check TMusrRunPhysicalQuantity
  for (UInt_t j=0; j<fMusrRunPhysQuantityObj.size(); j++) {
    if (fMusrRunPhysQuantityObj[j].GetPathName() == fPathNameOrder[idx]) { // found correct object
      prop = fMusrRunPhysQuantityObj[j].GetValue();
      Int_t digit, digit_d;
      if ((prop.GetDemand() != MRH_UNDEFINED) && (prop.GetValue() != MRH_UNDEFINED) && (prop.GetError() != MRH_UNDEFINED) &&
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
      } else if ((prop.GetDemand() == MRH_UNDEFINED) && (prop.GetValue() != MRH_UNDEFINED) && (prop.GetError() != MRH_UNDEFINED) &&
                 (prop.GetUnit() != "n/a")) { // <value> +- <error> <unit> [; <description>]
        digit = GetDecimalPlace(prop.GetError());
        if (prop.GetDescription() != "n/a") {
          fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; %%s", digit, digit);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(), prop.GetDescription().Data());
        } else {
          fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s", digit, digit);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data());
        }
      } else if ((prop.GetDemand() == MRH_UNDEFINED) && (prop.GetValue() != MRH_UNDEFINED) && (prop.GetError() == MRH_UNDEFINED) &&
                 (prop.GetUnit() != "n/a")) { // <value> <unit> [; <description>]
        digit = GetLeastSignificantDigit(prop.GetValue());
        if (prop.GetDescription() != "n/a") {
          fmt.Form("%%s: %%.%dlf %%s; %%s", digit);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data(), prop.GetDescription().Data());
        } else {
          fmt.Form("%%s: %%.%dlf %%s", digit);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data());
        }
      } else if ((prop.GetDemand() != MRH_UNDEFINED) && (prop.GetValue() != MRH_UNDEFINED) && (prop.GetError() == MRH_UNDEFINED) &&
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
      str.Form("%03d - %s -@%d", idx, tstr.Data(), MRH_TMUSR_RUN_PHYSICAL_QUANTITY);
      tostr.SetString(str);
    }
  }
  // 5th check TStringVector
  for (UInt_t j=0; j<fStringVectorObj.size(); j++) {
    if (fStringVectorObj[j].GetPathName() == fPathNameOrder[idx]) { // found correct object
      SplitPathName(fStringVectorObj[j].GetPathName(), path, name);
      str.Form("%03d - %s: ", idx, name.Data());
      TStringVector vstr = fStringVectorObj[j].GetValue();
      for (UInt_t k=0; k<vstr.size()-1; k++)
        str += vstr[k] + "; ";
      str += vstr[vstr.size()-1];
      str += " -@";
      str += MRH_TSTRING_VECTOR;
      tostr.SetString(str);
    }
  }
  // 6th check TIntVector
  for (UInt_t j=0; j<fIntVectorObj.size(); j++) {
    if (fIntVectorObj[j].GetPathName() == fPathNameOrder[idx]) { // found correct object
      SplitPathName(fIntVectorObj[j].GetPathName(), path, name);
      str.Form("%03d - %s: ", idx, name.Data());
      TIntVector vint = fIntVectorObj[j].GetValue();
      for (UInt_t k=0; k<vint.size()-1; k++) {
        str += vint[k];
        str += "; ";
      }
      str += vint[vint.size()-1];
      str += " -@";
      str += MRH_INT_VECTOR;
      tostr.SetString(str);
    }
  }
  // 7th check TDoubleVector
  for (UInt_t j=0; j<fDoubleVectorObj.size(); j++) {
    if (fDoubleVectorObj[j].GetPathName() == fPathNameOrder[idx]) { // found correct object
      SplitPathName(fDoubleVectorObj[j].GetPathName(), path, name);
      str.Form("%03d - %s: ", idx, name.Data());
      TDoubleVector dvec = fDoubleVectorObj[j].GetValue();
      TString subStr("");
      fmt.Form("%%.%dlf", MRH_DOUBLE_PREC);
      for (UInt_t k=0; k<dvec.size()-1; k++) {
        subStr.Form(fmt, dvec[k]);
        str += subStr;
        str += "; ";
      }
      subStr.Form(fmt, dvec.size()-1);
      str += subStr;
      str += " -@";
      str += MRH_DOUBLE_VECTOR;
      tostr.SetString(str);
    }
  }

  return tostr;
}

//--------------------------------------------------------------------------
// RemoveFirst (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 * \param splitter
 */
void TMusrRunHeader::RemoveFirst(TString &str, const char splitter)
{
  Ssiz_t idx = str.First(splitter);
  str.Remove(0, idx+1);
}

//--------------------------------------------------------------------------
// GetFirst (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 * \param splitter
 */
TString TMusrRunHeader::GetFirst(TString &str, const char splitter)
{
  TString result = str;

  Ssiz_t idx = str.First(splitter);
  result.Remove(idx, str.Length());

  return result;
}

//--------------------------------------------------------------------------
// AddSubTrees (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param content
 * \param pathName
 */
void TMusrRunHeader::AddSubTrees(TObjArray *content, TString pathName)
{
  // check if element is already present
  TString objName = GetFirst(pathName, '/');
  if (!content->FindObject(objName.Data())) { // object array not present yet, add it
    TObjArray *oarray = new TObjArray();
    oarray->SetName(objName);
    content->AddLast(oarray);
  }

  // check if more sub trees are needed
  TObjArray *tok = pathName.Tokenize('/');

  if (tok->GetEntries() > 2) { // still sub trees present
    RemoveFirst(pathName, '/');
    AddSubTrees((TObjArray*)content->FindObject(objName.Data()), pathName);
  }

  if (tok)
    delete tok;
}

//--------------------------------------------------------------------------
// SetSubTreeObject (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param content
 * \param ostr
 */
void TMusrRunHeader::SetSubTreeObject(TObjArray *content, TObjString ostr, Int_t idx)
{
  TObjArray *tok = fPathNameOrder[idx].Tokenize('/');
  TObjArray *pos = content;

  for (Int_t i=1; i<tok->GetEntries()-1; i++) {
    pos = (TObjArray*)pos->FindObject(((TObjString*)tok->At(i))->GetString()); // go down the proper tree
    if (pos == 0) {
      cerr << endl << "TMusrRunHeader::SetSubTreeObject(): **ERROR** couldn't reach requested path: " << fPathNameOrder[idx];
      if (tok)
        delete tok;
      return;
    }
  }
  TObjString *value = new TObjString();
  value = (TObjString*)ostr.Clone();
  pos->AddLast(value);

  if (tok)
    delete tok;
}

// end ---------------------------------------------------------------------