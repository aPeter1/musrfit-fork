/***************************************************************************

  TMusrRunHeader.h

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

#ifndef TMUSRRUNHEADER_H
#define TMUSRRUNHEADER_H

#include <TDatime.h>
#include <TObject.h>
#include <TQObject.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TMap.h>
#include <TSAXParser.h>

#define MRRH_UNDEFINED -9.99e99

typedef vector<Int_t> TIntVector;
typedef vector<Double_t> TDoubleVector;
typedef vector<TString> TStringVector;

//-------------------------------------------------------------------------
template <class T> class TMusrRunObject : public TObject
{
public:
  TMusrRunObject() { fPathName = "n/a"; fType = "n/a"; }
  TMusrRunObject(TString pathName, TString type, T value) : fPathName(pathName), fType(type), fValue(value) {}
  virtual ~TMusrRunObject() {}

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
class TMusrRunPhysicalQuantity : public TObject
{
public:
  TMusrRunPhysicalQuantity();
  TMusrRunPhysicalQuantity(TString label, Double_t demand, Double_t value, Double_t error, TString unit, TString description = TString("n/a"));
  TMusrRunPhysicalQuantity(TString label, Double_t demand, Double_t value, TString unit, TString description = TString("n/a"));
  TMusrRunPhysicalQuantity(TString label, Double_t value, TString unit, TString description = TString("n/a"));
  virtual ~TMusrRunPhysicalQuantity() {}

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
  Double_t fDemand; ///< demand value of the physical quantity, e.g. temperature setpoint
  Double_t fValue; ///<  measured value of the physical quantity
  Double_t fError; ///<  estimated error (standard deviation) of the measured value
  TString  fUnit;   ///< unit of the physical quantity
  TString  fDescription; ///< a more detailed description of the physical quantity

  ClassDef(TMusrRunPhysicalQuantity, 1)
};

//-------------------------------------------------------------------------
class TMusrRunHeader : public TObject
{
public:
  TMusrRunHeader();
  TMusrRunHeader(const char *fileName);
  virtual ~TMusrRunHeader();

  virtual TString GetFileName() { return fFileName; }

  virtual void GetHeaderInfo(TString path, TObjArray &content);

  virtual void GetValue(TString pathName, TString &value, Bool_t &ok);
  virtual void GetValue(TString pathName, Int_t &value, Bool_t &ok);
  virtual void GetValue(TString pathName, Double_t &value, Bool_t &ok);
  virtual void GetValue(TString pathName, TMusrRunPhysicalQuantity &value, Bool_t &ok);
  virtual void GetValue(TString pathName, TStringVector &value, Bool_t &ok);
  virtual void GetValue(TString pathName, TIntVector &value, Bool_t &ok);
  virtual void GetValue(TString pathName, TDoubleVector &value, Bool_t &ok);

  virtual TMap* GetMap() { return &fMap; }

  virtual void SetFileName(TString fln) { fFileName = fln; }
  virtual void SetMap(TMap* map);

  virtual void Set(TString pathName, TString value, Bool_t addMap=true);
  virtual void Set(TString pathName, Int_t value, Bool_t addMap=true);
  virtual void Set(TString pathName, Double_t value, Bool_t addMap=true);
  virtual void Set(TString pathName, TMusrRunPhysicalQuantity value, Bool_t addMap=true);
  virtual void Set(TString pathName, TStringVector value, Bool_t addMap=true);
  virtual void Set(TString pathName, TIntVector value, Bool_t addMap=true);
  virtual void Set(TString pathName, TDoubleVector value, Bool_t addMap=true);

  virtual Bool_t ExtractHeaderInformation(TObjArray *headerInfo, TString path);

  virtual void DumpHeader();
  virtual void DrawHeader();

private:
  TString fFileName;

  vector< TMusrRunObject<TString> >  fStringObj;
  vector< TMusrRunObject<Int_t> >    fIntObj;
  vector< TMusrRunObject<Double_t> > fDoubleObj;
  vector< TMusrRunObject<TMusrRunPhysicalQuantity> > fMusrRunPhysQuantityObj;
  vector< TMusrRunObject<TStringVector> > fStringVectorObj;
  vector< TMusrRunObject<TIntVector> > fIntVectorObj;
  vector< TMusrRunObject<TDoubleVector> > fDoubleVectorObj;

  TStringVector fFolder;

  TMap fMap; ///< maps run header label to its root type, e.g. 'Run Number' -> 'Int_t' or 'Time Resolution' -> 'TMusrRunPhysicalQuantity'

  virtual void Init();
  virtual UInt_t GetDecimalPlace(Double_t val);
  virtual UInt_t GetLeastSignificantDigit(Double_t val) const;
  virtual void SplitPathName(TString pathName, TString &path, TString &name);
  virtual Bool_t FolderPresent(TString &path);

  ClassDef(TMusrRunHeader, 1)
};

#endif // TMUSRRUNHEADER_H
