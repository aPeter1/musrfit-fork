/***************************************************************************

  TPsiRunHeader.h

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

#ifndef TPSIRUNHEADER_H
#define TPSIRUNHEADER_H

#include <TDatime.h>
#include <TObject.h>
#include <TQObject.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TSAXParser.h>

#define PRH_UNDEFINED -9.99e99

typedef vector<Int_t> TIntVector;
typedef vector<TString> TStringVector;

//-------------------------------------------------------------------------
template <class T> class TPsiRunObject : public TObject
{
public:
  TPsiRunObject() { fPathName = "n/a"; fType = "n/a"; }
  TPsiRunObject(TString pathName, TString type, T value) : fPathName(pathName), fType(type), fValue(value) {}
  virtual ~TPsiRunObject() {}

  virtual TString GetPathName() { return fPathName; }
  virtual TString GetType() { return fType; }
  virtual T GetValue() { return fValue; }

  virtual void SetPathName(TString pathName) { fPathName = pathName; }
  virtual void SetType(TString type) { fType = type; }
  virtual void SetValue(T value) { fValue = value; }

private:
  TString fPathName; ///< path name of the variable, e.g. 'RunInfo/Run Number'
  TString fType;     ///< type of value, e.g. TString, or Int_t, etc.
  T fValue;          ///< value itself
};

//-------------------------------------------------------------------------
class TPsiRunProperty : public TObject
{
public:
  TPsiRunProperty();
  TPsiRunProperty(TString label, Double_t demand, Double_t value, Double_t error, TString unit, TString description = TString("n/a"));
  TPsiRunProperty(TString label, Double_t demand, Double_t value, TString unit, TString description = TString("n/a"));
  TPsiRunProperty(TString label, Double_t value, TString unit, TString description = TString("n/a"));
  virtual ~TPsiRunProperty() {}

  virtual TString  GetLabel() const { return fLabel; }
  virtual Double_t GetDemand() const { return fDemand; }
  virtual Double_t GetValue() const { return fValue; }
  virtual Double_t GetError() const { return fError; }
  virtual TString  GetUnit() const { return fUnit; }
  virtual TString  GetDescription() const { return fDescription; }

  virtual void Set(TString label, Double_t demand, Double_t value, Double_t error, TString unit, TString description = TString("n/a"));
  virtual void Set(TString label, Double_t demand, Double_t value, TString unit, TString description = TString("n/a"));
  virtual void Set(TString label, Double_t value, TString unit, TString description = TString("n/a"));
  virtual void SetLabel(TString &label) { fLabel = label; }
  virtual void SetLabel(const char *label) { fLabel = label; }
  virtual void SetDemand(Double_t val) { fDemand = val; }
  virtual void SetValue(Double_t val) { fValue = val; }
  virtual void SetError(Double_t err) { fError = err; }
  virtual void SetUnit(TString &unit) { fUnit = unit; }
  virtual void SetUnit(const char *unit) { fUnit = unit; }
  virtual void SetDescription(TString &str) { fDescription = str; }
  virtual void SetDescription(const char *str) { fDescription = str; }

private:
  TString  fLabel; ///<  property label, like ’Sample Temperature’ etc.
  Double_t fDemand; ///< demand value of the property, e.g. temperature setpoint
  Double_t fValue; ///<  measured value of the property
  Double_t fError; ///<  estimated error (standard deviation) of the measured property value
  TString  fUnit;   ///< unit of the property
  TString  fDescription; ///< a more detailed description of the property

  ClassDef(TPsiRunProperty, 1)
};

//-------------------------------------------------------------------------
class TPsiEntry
{
public:
  TPsiEntry() { fPathName = "n/a"; fType = "n/a"; }
  TPsiEntry(TString pathName, TString type) : fPathName(pathName), fType(type) {}
  virtual ~TPsiEntry() {}

  virtual TString GetPathName() { return fPathName; }
  virtual TString GetType() { return fType; }

  virtual void SetPathName(TString pathName) { fPathName = pathName; }
  virtual void SetType(TString type) { fType = type; }

private:
  TString fPathName;
  TString fType;
};

//-------------------------------------------------------------------------
class TPsiStartupHandler : public TObject, public TQObject
{
public:
  TPsiStartupHandler();
  virtual ~TPsiStartupHandler();

  virtual void OnStartDocument(); // SLOT
  virtual void OnEndDocument(); // SLOT
  virtual void OnStartElement(const Char_t*, const TList*); // SLOT
  virtual void OnEndElement(const Char_t*); // SLOT
  virtual void OnCharacters(const Char_t*); // SLOT
  virtual void OnComment(const Char_t*); // SLOT
  virtual void OnWarning(const Char_t*); // SLOT
  virtual void OnError(const Char_t*); // SLOT
  virtual void OnFatalError(const Char_t*); // SLOT
  virtual void OnCdataBlock(const Char_t*, Int_t); // SLOT

  virtual TStringVector GetFolders() { return fFolder; }
  virtual vector<TPsiEntry> GetEntries() { return fEntry; }

private:
  enum EKeyWords {eEmpty, eFolder, eEntry, eName, eType};

  EKeyWords fKey, fGroupKey; ///< xml filter key

  TStringVector fFolder;
  vector<TPsiEntry> fEntry;

  ClassDef(TPsiStartupHandler, 1)      
};

//-------------------------------------------------------------------------
class TPsiRunHeader : public TObject
{
public:
  TPsiRunHeader(const char *headerDefinition);
  virtual ~TPsiRunHeader();

  virtual Bool_t IsValid(Bool_t strict = false);

  virtual void GetHeaderInfo(TString path, TObjArray &content);

  virtual void GetValue(TString pathName, TString &value, Bool_t &ok);
  virtual void GetValue(TString pathName, Int_t &value, Bool_t &ok);
  virtual void GetValue(TString pathName, TPsiRunProperty &value, Bool_t &ok);
  virtual void GetValue(TString pathName, TStringVector &value, Bool_t &ok);
  virtual void GetValue(TString pathName, TIntVector &value, Bool_t &ok);

  virtual void Set(TString pathName, TString value);
  virtual void Set(TString pathName, Int_t value);
  virtual void Set(TString pathName, TPsiRunProperty value);
  virtual void Set(TString pathName, TStringVector value);
  virtual void Set(TString pathName, TIntVector value);

  virtual Bool_t ExtractHeaderInformation(TObjArray *headerInfo, TString path);

  virtual void DumpHeader();
  virtual void DrawHeader();

private:
  TString fHeaderDefinition;

  vector< TPsiRunObject<TString> > fStringObj;
  vector< TPsiRunObject<Int_t> >   fIntObj;
  vector< TPsiRunObject<TPsiRunProperty> > fPsiRunPropertyObj;
  vector< TPsiRunObject<TStringVector> > fStringVectorObj;
  vector< TPsiRunObject<TIntVector> > fIntVectorObj;

  TStringVector fFolder;
  vector<TPsiEntry> fEntry;

  virtual UInt_t GetDecimalPlace(Double_t val);
  virtual UInt_t GetLeastSignificantDigit(Double_t val) const;
  virtual void SplitPathName(TString pathName, TString &path, TString &name);

  ClassDef(TPsiRunHeader, 1)
};

#endif // TPSIRUNHEADER_H
