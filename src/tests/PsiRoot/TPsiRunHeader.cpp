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

#include <cstdlib>
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
#include <TList.h>
#include <TSAXParser.h>
#include <TXMLAttr.h>

ClassImp(TPsiRunProperty)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 */
TPsiRunProperty::TPsiRunProperty()
{
  fLabel  = "n/a";
  fDemand = PRH_UNDEFINED;
  fValue  = PRH_UNDEFINED;
  fError  = PRH_UNDEFINED;
  fUnit   = "n/a";
  fDescription = "n/a";
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param name
 * \param demand
 * \param value
 * \param error
 * \param unit
 * \param description
 */
TPsiRunProperty::TPsiRunProperty(TString label, Double_t demand, Double_t value, Double_t error, TString unit, TString description) :
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
 * \param name
 * \param value
 * \param unit
 */
TPsiRunProperty::TPsiRunProperty(TString label, Double_t value, TString unit) :
    fLabel(label), fValue(value), fUnit(unit)
{
  fDemand = PRH_UNDEFINED;
  fError  = PRH_UNDEFINED;
  fDescription = "n/a";
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImp(TPsiStartupHandler)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
  * <p>Constructor.
  */
TPsiStartupHandler::TPsiStartupHandler()
{
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
  * <p>Destructor.
  */
TPsiStartupHandler::~TPsiStartupHandler()
{
  fFolder.clear();
  fEntry.clear();
}

//--------------------------------------------------------------------------
// OnStartDocument
//--------------------------------------------------------------------------
/**
 * <p>Called on start of the XML file reading. Initializes all necessary variables.
 */
void TPsiStartupHandler::OnStartDocument()
{
  fKey = eEmpty;
  fGroupKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnEndDocument
//--------------------------------------------------------------------------
/**
 * <p>Called on end of XML file reading.
 */
void TPsiStartupHandler::OnEndDocument()
{

}

//--------------------------------------------------------------------------
// OnStartElement
//--------------------------------------------------------------------------
/**
 * <p>Called when a XML start element is found. Filters out the needed elements
 * and sets a proper key.
 *
 * \param str XML element name
 * \param attributes not used
 */
void TPsiStartupHandler::OnStartElement(const Char_t *str, const TList *attributes)
{
  if (!strcmp(str, "folder")) {
    fGroupKey = eFolder;
  } else if (!strcmp(str, "entry")) {
    fGroupKey = eEntry;
  } else if (!strcmp(str, "name")) {
    fKey = eName;
  } else if (!strcmp(str, "type")) {
    fKey = eType;
  }
}

//--------------------------------------------------------------------------
// OnEndElement
//--------------------------------------------------------------------------
/**
 * <p>Called when a XML end element is found. Resets the handler key.
 *
 * \param str not used
 */
void TPsiStartupHandler::OnEndElement(const Char_t *str)
{
  if (!strcmp(str, "folder") || !strcmp(str, "entry"))
    fGroupKey = eEmpty;
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnCharacters
//--------------------------------------------------------------------------
/**
 * <p>Content of a given XML element. Filters out the data and feeds them to
 * the internal variables.
 *
 * \param str XML element string
 */
void TPsiStartupHandler::OnCharacters(const Char_t *str)
{
  static TPsiEntry entry;
  static Int_t code = 0;

  switch (fGroupKey) {
  case eFolder:
    switch (fKey) {
    case eName:
      fFolder.push_back(str);
      break;
    default:
      break;
    }
    break;
  case eEntry:
    switch (fKey) {
    case eName:
      entry.SetPathName(str);
      code |= 1;
      break;
    case eType:
      entry.SetType(str);
      code |= 2;
      break;
    default:
      break;
    }
    if (code == 3) {
      fEntry.push_back(entry);
      code = 0;
    }
    break;
  default:
    break;
  }
}

//--------------------------------------------------------------------------
// OnComment
//--------------------------------------------------------------------------
/**
 * <p>Called when a XML comment is found. Not used.
 *
 * \param str not used.
 */
void TPsiStartupHandler::OnComment(const Char_t *str)
{
  // nothing to be done for now
}

//--------------------------------------------------------------------------
// OnWarning
//--------------------------------------------------------------------------
/**
 * <p>Called when the XML parser emits a warning.
 *
 * \param str warning string
 */
void TPsiStartupHandler::OnWarning(const Char_t *str)
{
  cerr << endl << ">> TPsiStartupHandler **WARNING** " << str;
  cerr << endl;
}

//--------------------------------------------------------------------------
// OnError
//--------------------------------------------------------------------------
/**
 * <p>Called when the XML parser emits an error.
 *
 * \param str error string
 */
void TPsiStartupHandler::OnError(const Char_t *str)
{
  cerr << endl << ">> TPsiStartupHandler **ERROR** " << str;
  cerr << endl;
}

//--------------------------------------------------------------------------
// OnFatalError
//--------------------------------------------------------------------------
/**
 * <p>Called when the XML parser emits a fatal error.
 *
 * \param str fatal error string
 */
void TPsiStartupHandler::OnFatalError(const Char_t *str)
{
  cerr << endl << ">> TPsiStartupHandler **FATAL ERROR** " << str;
  cerr << endl;
}

//--------------------------------------------------------------------------
// OnCdataBlock
//--------------------------------------------------------------------------
/**
 * <p>Not used.
 *
 * \param str not used
 * \param len not used
 */
void TPsiStartupHandler::OnCdataBlock(const Char_t *str, Int_t len)
{
  // nothing to be done for now
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
  TSAXParser *saxParser = new TSAXParser();
  TPsiStartupHandler *startupHandler = new TPsiStartupHandler();

  saxParser->ConnectToHandler("TPsiStartupHandler", startupHandler);
  Int_t status = saxParser->ParseFile("./psi_root.xml");

  if (status) { // error
    // not clear what to do yet ...
  } else {
    fFolder = startupHandler->GetFolders();
    fEntry = startupHandler->GetEntries();
  }

  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }

  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
  * <p>Destructor.
  */
TPsiRunHeader::~TPsiRunHeader()
{
  fStringObj.clear();
  fIntObj.clear();
  fPsiRunPropertyObj.clear();
  fStringVectorObj.clear();
  fIntVectorObj.clear();

  fFolder.clear();
  fEntry.clear();
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
  Bool_t result = true;
  Int_t  count = 0;

  for (UInt_t i=0; i<fEntry.size(); i++) {
    for (UInt_t j=0; j<fStringObj.size(); j++) {
      if (!fEntry[i].GetPathName().CompareTo(fStringObj[j].GetPathName())) {
        if (!fEntry[i].GetType().CompareTo("TString")) {
          count++;
          break;
        } else {
          cerr << endl << ">> **ERROR** found entry: " << fEntry[i].GetPathName() << " should be a 'TString' but is defined as " << fEntry[i].GetType().Data() << endl;
          return false;
        }
      }
    }
    for (UInt_t j=0; j<fIntObj.size(); j++) {
      if (!fEntry[i].GetPathName().CompareTo(fIntObj[j].GetPathName())) {
        if (!fEntry[i].GetType().CompareTo("Int_t")) {
          count++;
          break;
        } else {
          cerr << endl << ">> **ERROR** found entry: " << fEntry[i].GetPathName() << " should be a 'Int_t' but is defined as " << fEntry[i].GetType().Data() << endl;
          return false;
        }
      }
    }
    for (UInt_t j=0; j<fPsiRunPropertyObj.size(); j++) {
      if (!fEntry[i].GetPathName().CompareTo(fPsiRunPropertyObj[j].GetPathName())) {
        if (!fEntry[i].GetType().CompareTo("TPsiRunProperty")) {
          count++;
          break;
        } else {
          cerr << endl << ">> **ERROR** found entry: " << fEntry[i].GetPathName() << " should be a 'TPsiRunProperty' but is defined as " << fEntry[i].GetType().Data() << endl;
          return false;
        }
      }
    }
    for (UInt_t j=0; j<fStringVectorObj.size(); j++) {
      if (!fEntry[i].GetPathName().CompareTo(fStringVectorObj[j].GetPathName())) {
        if (!fEntry[i].GetType().CompareTo("TStringVector")) {
          count++;
          break;
        } else {
          cerr << endl << ">> **ERROR** found entry: " << fEntry[i].GetPathName() << " should be a 'TStringVector' but is defined as " << fEntry[i].GetType().Data() << endl;
          return false;
        }
      }
    }
    for (UInt_t j=0; j<fIntVectorObj.size(); j++) {
      if (!fEntry[i].GetPathName().CompareTo(fIntVectorObj[j].GetPathName())) {
        if (!fEntry[i].GetType().CompareTo("TIntVector")) {
          count++;
          break;
        } else {
          cerr << endl << ">> **ERROR** found entry: " << fEntry[i].GetPathName() << " should be a 'TIntVector' but is defined as " << fEntry[i].GetType().Data() << endl;
          return false;
        }
      }
    }
  }
  if (count != (Int_t)fEntry.size()) {
    if (strict) {
      result = false;
      cerr << endl << ">> **ERROR** only found " << count << " entries. " << fEntry.size() << " are required!" << endl;
    } else {
      cerr << endl << ">> **WARNING** only found " << count << " entries. " << fEntry.size() << " are required!" << endl;
    }
  }

  return result;
}

//--------------------------------------------------------------------------
// Get (public)
//--------------------------------------------------------------------------
/**
 * <p>Get PSI-ROOT header information of 'path'.
 *
 * \param requestedPath of the PSI-ROOT header, e.g. RunInfo
 * \param content of the requested PSI-ROOT header.
 */
void TPsiRunHeader::Get(TString requestedPath, TObjArray &content)
{
  // make sure content is initialized
  content.Delete();
  content.Expand(0);

  static UInt_t count = 1;
  TString str(""), path(""), name(""), fmt(""), tstr("");
  TObjString *tostr;
  TPsiRunProperty prop;

  // go first through all objects defined in psi_root.xml
  for (UInt_t i=0; i<fEntry.size(); i++) {
    if (fEntry[i].GetPathName().Contains(requestedPath)) { // entry found
      for (UInt_t j=0; j<fStringObj.size(); j++) {
        if (fStringObj[j].GetPathName() == fEntry[i].GetPathName()) {
          SplitPathName(fStringObj[j].GetPathName(), path, name);
          str.Form("%03d - %s: %s", count++, name.Data(), fStringObj[j].GetValue().Data());
          tostr = new TObjString(str);
          content.AddLast(tostr);
        }
      }
      for (UInt_t j=0; j<fIntObj.size(); j++) {
        if (fIntObj[j].GetPathName() == fEntry[i].GetPathName()) {
          SplitPathName(fIntObj[j].GetPathName(), path, name);
          str.Form("%03d - %s: %d", count++, name.Data(), fIntObj[j].GetValue());
          tostr = new TObjString(str);
          content.AddLast(tostr);
        }
      }
      for (UInt_t j=0; j<fPsiRunPropertyObj.size(); j++) {
        if (fPsiRunPropertyObj[j].GetPathName() == fEntry[i].GetPathName()) {
          prop = fPsiRunPropertyObj[j].GetValue();
          Int_t digit, digit_d;
          if ((prop.GetDemand() != PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() != PRH_UNDEFINED) &&
              (prop.GetUnit() != "n/a") && (prop.GetDescription() != "n/a")) {
            digit = GetDecimalPlace(prop.GetError());
            digit_d = GetLeastSignificantDigit(prop.GetDemand());
            fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit, digit_d);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(),
                      prop.GetDemand(), prop.GetDescription().Data());
          } else if ((prop.GetDemand() != PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() != PRH_UNDEFINED) &&
                     (prop.GetUnit() != "n/a") && (prop.GetDescription() == "n/a")) {
            digit = GetDecimalPlace(prop.GetError());
            digit_d = GetLeastSignificantDigit(prop.GetDemand());
            fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf", digit, digit, digit_d);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(),
                      prop.GetDemand());
          } else if ((prop.GetDemand() == PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() != PRH_UNDEFINED) &&
                     (prop.GetUnit() != "n/a") && (prop.GetDescription() == "n/a")) {
            digit = GetDecimalPlace(prop.GetError());
            fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s", digit, digit);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data());
          } else if ((prop.GetDemand() == PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() == PRH_UNDEFINED) &&
                     (prop.GetUnit() != "n/a") && (prop.GetDescription() == "n/a")) {
            digit = GetLeastSignificantDigit(prop.GetValue());
            fmt.Form("%%s: %%.%dlf %%s", digit);
            tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data());
          }
          str.Form("%03d - %s", count++, tstr.Data());
          tostr = new TObjString(str);
          content.AddLast(tostr);
        }
      }
      for (UInt_t j=0; j<fStringVectorObj.size(); j++) {
        if (fStringVectorObj[j].GetPathName() == fEntry[i].GetPathName()) {
          SplitPathName(fStringVectorObj[j].GetPathName(), path, name);
          str.Form("%03d - %s: ", count++, name.Data());
          TStringVector vstr = fStringVectorObj[j].GetValue();
          for (UInt_t k=0; k<vstr.size()-1; k++)
            str += vstr[k] + "; ";
          str += vstr[vstr.size()-1];
          tostr = new TObjString(str);
          content.AddLast(tostr);
        }
      }
      for (UInt_t j=0; j<fIntVectorObj.size(); j++) {
        if (fIntVectorObj[j].GetPathName() == fEntry[i].GetPathName()) {
          SplitPathName(fIntVectorObj[j].GetPathName(), path, name);
          str.Form("%03d - %s: ", count++, name.Data());
          TIntVector vint = fIntVectorObj[j].GetValue();
          for (UInt_t k=0; k<vint.size()-1; k++) {
            str += vint[k];
            str += "; ";
          }
          str += vint[vint.size()-1];
          tostr = new TObjString(str);
          content.AddLast(tostr);
        }
      }
    }
  }

  // go through all objects **NOT** defined in psi_root.xml
  // this is needed if a less strict validation is wanted
  for (UInt_t i=0; i<fStringObj.size(); i++) {
    if (fStringObj[i].GetPathName().Contains(requestedPath)) { // correct path
      // check if entry is found in psi_root.xml in which case nothing needs to be done
      UInt_t j;
      for (j=0; j<fEntry.size(); j++) {
        if (fEntry[j].GetPathName() == fStringObj[i].GetPathName())
          break;
      }
      if (j == fEntry.size()) { // entry **NOT** present in psi_root.xml
        SplitPathName(fStringObj[i].GetPathName(), path, name);
        str.Form("%03d - %s: %s", count++, name.Data(), fStringObj[i].GetValue().Data());
        tostr = new TObjString(str);
        content.AddLast(tostr);
      }
    }
  }
  for (UInt_t i=0; i<fIntObj.size(); i++) {
    if (fIntObj[i].GetPathName().Contains(requestedPath)) { // correct path
      // check if entry is found in psi_root.xml in which case nothing needs to be done
      UInt_t j;
      for (j=0; j<fEntry.size(); j++) {
        if (fEntry[j].GetPathName() == fIntObj[i].GetPathName())
          break;
      }
      if (j == fEntry.size()) { // entry **NOT** present in psi_root.xml
        SplitPathName(fIntObj[i].GetPathName(), path, name);
        str.Form("%03d - %s: %d", count++, name.Data(), fIntObj[i].GetValue());
        tostr = new TObjString(str);
        content.AddLast(tostr);
      }
    }
  }
  for (UInt_t i=0; i<fPsiRunPropertyObj.size(); i++) {
    if (fPsiRunPropertyObj[i].GetPathName().Contains(requestedPath)) { // correct path
      // check if entry is found in psi_root.xml in which case nothing needs to be done
      UInt_t j;
      for (j=0; j<fEntry.size(); j++) {
        if (fEntry[j].GetPathName() == fPsiRunPropertyObj[i].GetPathName())
          break;
      }
      if (j == fEntry.size()) { // entry **NOT** present in psi_root.xml
        prop = fPsiRunPropertyObj[i].GetValue();
        Int_t digit, digit_d;
        if ((prop.GetDemand() != PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() != PRH_UNDEFINED) &&
            (prop.GetUnit() != "n/a") && (prop.GetDescription() != "n/a")) {
          digit = GetDecimalPlace(prop.GetError());
          digit_d = GetLeastSignificantDigit(prop.GetDemand());
          fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit, digit_d);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(),
                    prop.GetDemand(), prop.GetDescription().Data());
        } else if ((prop.GetDemand() != PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() != PRH_UNDEFINED) &&
                   (prop.GetUnit() != "n/a") && (prop.GetDescription() == "n/a")) {
          digit = GetDecimalPlace(prop.GetError());
          digit_d = GetLeastSignificantDigit(prop.GetDemand());
          fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf", digit, digit, digit_d);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(),
                    prop.GetDemand());
        } else if ((prop.GetDemand() == PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() != PRH_UNDEFINED) &&
                   (prop.GetUnit() != "n/a") && (prop.GetDescription() == "n/a")) {
          digit = GetDecimalPlace(prop.GetError());
          fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s", digit, digit);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data());
        } else if ((prop.GetDemand() == PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() == PRH_UNDEFINED) &&
                   (prop.GetUnit() != "n/a") && (prop.GetDescription() == "n/a")) {
          digit = GetLeastSignificantDigit(prop.GetValue());
          fmt.Form("%%s: %%.%dlf %%s", digit);
          tstr.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data());
        }
        str.Form("%03d - %s", count++, tstr.Data());
        tostr = new TObjString(str);
        content.AddLast(tostr);
      }
    }
  }
  for (UInt_t i=0; i<fStringVectorObj.size(); i++) {
    if (fStringVectorObj[i].GetPathName().Contains(requestedPath)) { // correct path
      // check if entry is found in psi_root.xml in which case nothing needs to be done
      UInt_t j;
      for (j=0; j<fEntry.size(); j++) {
        if (fEntry[j].GetPathName() == fStringVectorObj[i].GetPathName())
          break;
      }
      if (j == fEntry.size()) { // entry **NOT** present in psi_root.xml
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
  }
  for (UInt_t i=0; i<fIntVectorObj.size(); i++) {
    if (fIntVectorObj[i].GetPathName().Contains(requestedPath)) { // correct path
      // check if entry is found in psi_root.xml in which case nothing needs to be done
      UInt_t j;
      for (j=0; j<fEntry.size(); j++) {
        if (fEntry[j].GetPathName() == fIntVectorObj[i].GetPathName())
          break;
      }
      if (j == fEntry.size()) { // entry **NOT** present in psi_root.xml
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
  }

  content.SetName(requestedPath);
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set TString 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Run Title
 * \param type of the object
 * \param value of the entry
 */
void TPsiRunHeader::Set(TString pathName, TString type, TString value)
{
  // check if pathName is already set, and if not add it as a new entry
  UInt_t i=0;
  for (i=0; i<fStringObj.size(); i++) {
    if (!fStringObj[i].GetPathName().CompareTo(pathName, TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** " << pathName.Data() << " already exists, will replace it." << endl;
      fStringObj[i].SetType(type);
      fStringObj[i].SetValue(value);
      break;
    }
  }

  // if not found in the previous loop, it is a new object
  if (i == fStringObj.size()) {
    TPsiRunObject<TString> obj(pathName, type, value);
    fStringObj.push_back(obj);
  }
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set Int_t 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Run number
 * \param type of the object
 * \param value of the entry
 */
void TPsiRunHeader::Set(TString pathName, TString type, Int_t value)
{
  // check if pathName is already set, and if not add it as a new entry
  UInt_t i=0;
  for (i=0; i<fIntObj.size(); i++) {
    if (!fIntObj[i].GetPathName().CompareTo(pathName, TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** " << pathName.Data() << " already exists, will replace it." << endl;
      fIntObj[i].SetType(type);
      fIntObj[i].SetValue(value);
      break;
    }
  }

  // if not found in the previous loop, it is a new object
  if (i == fIntObj.size()) {
    TPsiRunObject<Int_t> obj(pathName, type, value);
    fIntObj.push_back(obj);
  }
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set TPsiRunProperty 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Muon Beam Momentum
 * \param type of the object
 * \param value of the entry
 */
void TPsiRunHeader::Set(TString pathName, TString type, TPsiRunProperty value)
{
  // check if pathName is already set, and if not add it as a new entry
  UInt_t i=0;
  for (i=0; i<fPsiRunPropertyObj.size(); i++) {
    if (!fPsiRunPropertyObj[i].GetPathName().CompareTo(pathName, TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** " << pathName.Data() << " already exists, will replace it." << endl;
      fPsiRunPropertyObj[i].SetType(type);
      fPsiRunPropertyObj[i].SetValue(value);
      break;
    }
  }

  // if not found in the previous loop, it is a new object
  if (i == fPsiRunPropertyObj.size()) {
    TPsiRunObject<TPsiRunProperty> obj(pathName, type, value);
    fPsiRunPropertyObj.push_back(obj);
  }
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set TStringVector 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Histo names
 * \param type of the object
 * \param value of the entry
 */
void TPsiRunHeader::Set(TString pathName, TString type, TStringVector value)
{
  // check if pathName is already set, and if not add it as a new entry
  UInt_t i=0;
  for (i=0; i<fStringVectorObj.size(); i++) {
    if (!fStringVectorObj[i].GetPathName().CompareTo(pathName, TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** " << pathName.Data() << " already exists, will replace it." << endl;
      fStringVectorObj[i].SetType(type);
      fStringVectorObj[i].SetValue(value);
      break;
    }
  }

  // if not found in the previous loop, it is a new object
  if (i == fStringVectorObj.size()) {
    TPsiRunObject<TStringVector> obj(pathName, type, value);
    fStringVectorObj.push_back(obj);
  }
}

//--------------------------------------------------------------------------
// Set (public)
//--------------------------------------------------------------------------
/**
 * <p>Set TIntVector 'value'.
 *
 * \param pathName path/name within the header, e.g. RunInfo/Time Zero Bin
 * \param type of the object
 * \param value of the entry
 */
void TPsiRunHeader::Set(TString pathName, TString type, TIntVector value)
{
  // check if pathName is already set, and if not add it as a new entry
  UInt_t i=0;
  for (i=0; i<fIntVectorObj.size(); i++) {
    if (!fIntVectorObj[i].GetPathName().CompareTo(pathName, TString::kIgnoreCase)) {
      cerr << endl << ">> **WARNING** " << pathName.Data() << " already exists, will replace it." << endl;
      fIntVectorObj[i].SetType(type);
      fIntVectorObj[i].SetValue(value);
      break;
    }
  }

  // if not found in the previous loop, it is a new object
  if (i == fIntVectorObj.size()) {
    TPsiRunObject<TIntVector> obj(pathName, type, value);
    fIntVectorObj.push_back(obj);
  }
}

//--------------------------------------------------------------------------
// DumpHeader (public)
//--------------------------------------------------------------------------
/**
 * <p>
 */
void TPsiRunHeader::DumpHeader()
{
  TString tstr(""), tstr1(""), fmt(""), path(""), name("");
  TPsiRunProperty prop;

  // go first through all objects defined in psi_root.xml
  for (UInt_t i=0; i<fFolder.size(); i++) {
    cout << endl << fFolder[i].Data() << ":";
    for (UInt_t j=0; j<fEntry.size(); j++) {
      if (fEntry[j].GetPathName().Contains(fFolder[i], TString::kIgnoreCase)) {
        if (fEntry[j].GetType() == "TString") {
          for (UInt_t k=0; k<fStringObj.size(); k++) {
            if (!fEntry[j].GetPathName().CompareTo(fStringObj[k].GetPathName(), TString::kIgnoreCase)) {
              tstr.Form("  %03d - ", j+1);
              SplitPathName(fStringObj[k].GetPathName(), path, name);
              cout << endl << tstr.Data() << name.Data() << ": " << fStringObj[k].GetValue().Data();
              break;
            }
          }
        }
        if (fEntry[j].GetType() == "Int_t") {
          for (UInt_t k=0; k<fIntObj.size(); k++) {
            if (!fEntry[j].GetPathName().CompareTo(fIntObj[k].GetPathName(), TString::kIgnoreCase)) {
              tstr.Form("  %03d - ", j+1);
              SplitPathName(fIntObj[k].GetPathName(), path, name);
              cout << endl << tstr.Data() << name.Data() << ": " << fIntObj[k].GetValue();
              break;
            }
          }
        }
        if (fEntry[j].GetType() == "TPsiRunProperty") {
          for (UInt_t k=0; k<fPsiRunPropertyObj.size(); k++) {
            if (!fEntry[j].GetPathName().CompareTo(fPsiRunPropertyObj[k].GetPathName(), TString::kIgnoreCase)) {
              tstr.Form("  %03d - ", j+1);
              tstr1 = "";
              // depending on the given properties dump
              prop = fPsiRunPropertyObj[k].GetValue();
              Int_t digit, digit_d;
              if ((prop.GetDemand() != PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() != PRH_UNDEFINED) &&
                  (prop.GetUnit() != "n/a") && (prop.GetDescription() != "n/a")) {
                digit = GetDecimalPlace(prop.GetError());
                digit_d = GetLeastSignificantDigit(prop.GetDemand());
                fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit, digit_d);
                tstr1.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(),
                           prop.GetDemand(), prop.GetDescription().Data());
              } else if ((prop.GetDemand() != PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() != PRH_UNDEFINED) &&
                         (prop.GetUnit() != "n/a") && (prop.GetDescription() == "n/a")) {
                digit = GetDecimalPlace(prop.GetError());
                digit_d = GetLeastSignificantDigit(prop.GetDemand());
                fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf", digit, digit, digit_d);
                tstr1.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(),
                           prop.GetDemand());
              } else if ((prop.GetDemand() == PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() != PRH_UNDEFINED) &&
                         (prop.GetUnit() != "n/a") && (prop.GetDescription() == "n/a")) {
                digit = GetDecimalPlace(prop.GetError());
                fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s", digit, digit);
                tstr1.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data());
              } else if ((prop.GetDemand() == PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() == PRH_UNDEFINED) &&
                         (prop.GetUnit() != "n/a") && (prop.GetDescription() == "n/a")) {
                digit = GetLeastSignificantDigit(prop.GetValue());
                fmt.Form("%%s: %%.%dlf %%s", digit);
                tstr1.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data());
              }
              cout << endl << tstr.Data() << tstr1.Data();
              break;
            }
          }
        }
        if (fEntry[j].GetType() == "TStringVector") {
          for (UInt_t k=0; k<fStringVectorObj.size(); k++) {
            if (!fEntry[j].GetPathName().CompareTo(fStringVectorObj[k].GetPathName(), TString::kIgnoreCase)) {
              tstr.Form("  %03d - ", j+1);
              SplitPathName(fStringVectorObj[k].GetPathName(), path, name);
              cout << endl << tstr.Data() << name.Data() << ": ";
              TStringVector vstr = fStringVectorObj[k].GetValue();
              for (UInt_t m=0; m<vstr.size()-1; m++)
                cout << vstr[m].Data() << "; ";
              cout << vstr[vstr.size()-1];
              break;
            }
          }
        }
        if (fEntry[j].GetType() == "TIntVector") {
          for (UInt_t k=0; k<fIntVectorObj.size(); k++) {
            if (!fEntry[j].GetPathName().CompareTo(fIntVectorObj[k].GetPathName(), TString::kIgnoreCase)) {
              tstr.Form("  %03d - ", j+1);
              SplitPathName(fIntVectorObj[k].GetPathName(), path, name);
              cout << endl << tstr.Data() << name.Data() << ": ";
              TIntVector vint = fIntVectorObj[k].GetValue();
              for (UInt_t m=0; m<vint.size()-1; m++)
                cout << vint[m] << "; ";
              cout << vint[vint.size()-1];
              break;
            }
          }
        }
      }
    }
  }

  // go through all objects **NOT** defined in psi_root.xml
  // this is needed if a less strict validation is wanted
  cout << endl << endl << "---------------" << endl;
  cout << endl << "Entries which are **NOT** present in psi_root.xml";
  Int_t count = fEntry.size()+1;
  for (UInt_t i=0; i<fFolder.size(); i++) {
    cout << endl << fFolder[i].Data() << ":";
    for (UInt_t j=0; j<fStringObj.size(); j++) {
      // check if entry is found in psi_root.xml in which case nothing needs to be done
      UInt_t k;
      for (k=0; k<fEntry.size(); k++) {
        if (fEntry[k].GetPathName() == fStringObj[j].GetPathName())
          break;
      }
      if ((k == fEntry.size()) && fStringObj[j].GetPathName().Contains(fFolder[i])) { // entry **NOT** present in psi_root.xml
        tstr.Form("  %03d - ", count++);
        SplitPathName(fStringObj[j].GetPathName(), path, name);
        cout << endl << tstr.Data() << name.Data() << ": " << fStringObj[j].GetValue().Data();
      }
    }
    for (UInt_t j=0; j<fIntObj.size(); j++) {
      // check if entry is found in psi_root.xml in which case nothing needs to be done
      UInt_t k;
      for (k=0; k<fEntry.size(); k++) {
        if (fEntry[k].GetPathName() == fIntObj[j].GetPathName())
          break;
      }
      if ((k == fEntry.size()) && fIntObj[j].GetPathName().Contains(fFolder[i])) { // entry **NOT** present in psi_root.xml
        tstr.Form("  %03d - ", count++);
        SplitPathName(fIntObj[j].GetPathName(), path, name);
        cout << endl << tstr.Data() << name.Data() << ": " << fIntObj[j].GetValue();
      }
    }
    for (UInt_t j=0; j<fPsiRunPropertyObj.size(); j++) {
      // check if entry is found in psi_root.xml in which case nothing needs to be done
      UInt_t k;
      for (k=0; k<fEntry.size(); k++) {
        if (fEntry[k].GetPathName() == fPsiRunPropertyObj[j].GetPathName())
          break;
      }
      if ((k == fEntry.size()) && fPsiRunPropertyObj[j].GetPathName().Contains(fFolder[i])) { // entry **NOT** present in psi_root.xml
        // depending on the given properties dump
        prop = fPsiRunPropertyObj[j].GetValue();
        Int_t digit, digit_d;
        if ((prop.GetDemand() != PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() != PRH_UNDEFINED) &&
            (prop.GetUnit() != "n/a") && (prop.GetDescription() != "n/a")) {
          digit = GetDecimalPlace(prop.GetError());
          digit_d = GetLeastSignificantDigit(prop.GetDemand());
          fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf; %%s", digit, digit, digit_d);
          tstr1.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(),
                     prop.GetDemand(), prop.GetDescription().Data());
        } else if ((prop.GetDemand() != PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() != PRH_UNDEFINED) &&
                   (prop.GetUnit() != "n/a") && (prop.GetDescription() == "n/a")) {
          digit = GetDecimalPlace(prop.GetError());
          digit_d = GetLeastSignificantDigit(prop.GetDemand());
          fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s; SP: %%.%dlf", digit, digit, digit_d);
          tstr1.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data(),
                     prop.GetDemand());
        } else if ((prop.GetDemand() == PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() != PRH_UNDEFINED) &&
                   (prop.GetUnit() != "n/a") && (prop.GetDescription() == "n/a")) {
          digit = GetDecimalPlace(prop.GetError());
          fmt.Form("%%s: %%.%dlf +- %%.%dlf %%s", digit, digit);
          tstr1.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetError(), prop.GetUnit().Data());
        } else if ((prop.GetDemand() == PRH_UNDEFINED) && (prop.GetValue() != PRH_UNDEFINED) && (prop.GetError() == PRH_UNDEFINED) &&
                   (prop.GetUnit() != "n/a") && (prop.GetDescription() == "n/a")) {
          digit = GetLeastSignificantDigit(prop.GetValue());
          fmt.Form("%%s: %%.%dlf %%s", digit);
          tstr1.Form(fmt, prop.GetLabel().Data(), prop.GetValue(), prop.GetUnit().Data());
        }
        tstr.Form("  %03d - ", count++);
        cout << endl << tstr.Data() << tstr1.Data();
      }
    }
    for (UInt_t j=0; j<fStringVectorObj.size(); j++) {
      // check if entry is found in psi_root.xml in which case nothing needs to be done
      UInt_t k;
      for (k=0; k<fEntry.size(); k++) {
        if (fEntry[k].GetPathName() == fStringVectorObj[j].GetPathName())
          break;
      }
      if ((k == fEntry.size()) && fStringVectorObj[j].GetPathName().Contains(fFolder[i])) { // entry **NOT** present in psi_root.xml
        tstr.Form("  %03d - ", count++);
        SplitPathName(fStringVectorObj[j].GetPathName(), path, name);
        cout << endl << tstr.Data() << name.Data() << ": ";
        TStringVector vstr = fStringVectorObj[j].GetValue();
        for (UInt_t m=0; m<vstr.size()-1; m++)
          cout << vstr[m].Data() << "; ";
        cout << vstr[vstr.size()-1];
      }
    }
    for (UInt_t j=0; j<fIntVectorObj.size(); j++) {
      // check if entry is found in psi_root.xml in which case nothing needs to be done
      UInt_t k;
      for (k=0; k<fEntry.size(); k++) {
        if (fEntry[k].GetPathName() == fIntVectorObj[j].GetPathName())
          break;
      }
      if ((k == fEntry.size()) && fIntVectorObj[j].GetPathName().Contains(fFolder[i])) { // entry **NOT** present in psi_root.xml
        tstr.Form("  %03d - ", count++);
        SplitPathName(fIntVectorObj[j].GetPathName(), path, name);
        cout << endl << tstr.Data() << name.Data() << ": ";
        TIntVector vint = fIntVectorObj[j].GetValue();
        for (UInt_t m=0; m<vint.size()-1; m++)
          cout << vint[m] << "; ";
        cout << vint[vint.size()-1];
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
void TPsiRunHeader::DrawHeader()
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
UInt_t TPsiRunHeader::GetLeastSignificantDigit(Double_t val) const
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
void TPsiRunHeader::SplitPathName(TString pathName, TString &path, TString &name)
{
  path = TString("");
  name = TString("");
  Ssiz_t idx = pathName.Last('/');

  for (Int_t i=0; i<idx; i++)
    path += pathName[i];
  path += '\0';

  for (Int_t i=idx+1; i<pathName.Length(); i++)
    name += pathName[i];
  name += '\0';
}

// end ---------------------------------------------------------------------
