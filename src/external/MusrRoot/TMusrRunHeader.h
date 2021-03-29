/***************************************************************************

  TMusrRunHeader.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2021 by Andreas Suter                              *
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

#include <vector>

#include <TDatime.h>
#include <TObject.h>
#include <TQObject.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TFolder.h>

#define MRH_UNDEFINED -9.99e99

#define MRH_DOUBLE_PREC 6

#define MRH_TSTRING                     0
#define MRH_INT                         1
#define MRH_DOUBLE                      2
#define MRH_TMUSR_RUN_PHYSICAL_QUANTITY 3
#define MRH_TSTRING_VECTOR              4
#define MRH_INT_VECTOR                  5
#define MRH_DOUBLE_VECTOR               6

typedef std::vector<Int_t> TIntVector;
typedef std::vector<Double_t> TDoubleVector;
typedef std::vector<TString> TStringVector;

//-------------------------------------------------------------------------
template <class T> class TMusrRunObject : public TObject
{
public:
  TMusrRunObject() : TObject() { fPathName = "n/a"; fType = "n/a"; }
  TMusrRunObject(TString pathName, TString type, T value) : TObject(), fPathName(pathName), fType(type), fValue(value) {}
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
  TMusrRunHeader(bool quiet=false);
  TMusrRunHeader(const char *fileName, bool quiet=false);
  virtual ~TMusrRunHeader();

  virtual TString GetFileName() { return fFileName; }

  virtual Bool_t FillFolder(TFolder *folder);

  virtual Bool_t ExtractAll(TFolder *folder);
  virtual Bool_t ExtractHeaderInformation(TObjArray *headerInfo, TString path);

  virtual TString GetTypeOfPath(TString pathName);

  virtual void Get(TString pathName, TString &value, Bool_t &ok);
  virtual void Get(TString pathName, Int_t &value, Bool_t &ok);
  virtual void Get(TString pathName, Double_t &value, Bool_t &ok);
  virtual void Get(TString pathName, TMusrRunPhysicalQuantity &value, Bool_t &ok);
  virtual void Get(TString pathName, TStringVector &value, Bool_t &ok);
  virtual void Get(TString pathName, TIntVector &value, Bool_t &ok);
  virtual void Get(TString pathName, TDoubleVector &value, Bool_t &ok);

  virtual void SetFileName(TString fln) { fFileName = fln; }

  virtual void Set(TString pathName, TString value);
  virtual void Set(TString pathName, Int_t value);
  virtual void Set(TString pathName, Double_t value);
  virtual void Set(TString pathName, TMusrRunPhysicalQuantity value);
  virtual void Set(TString pathName, TStringVector value);
  virtual void Set(TString pathName, TIntVector value);
  virtual void Set(TString pathName, TDoubleVector value);

  virtual void DumpHeader();
  virtual void DrawHeader();

private:
  bool fQuiet;
  TString fFileName;
  TString fVersion;

  std::vector< TMusrRunObject<TString> >  fStringObj;
  std::vector< TMusrRunObject<Int_t> >    fIntObj;
  std::vector< TMusrRunObject<Double_t> > fDoubleObj;
  std::vector< TMusrRunObject<TMusrRunPhysicalQuantity> > fMusrRunPhysQuantityObj;
  std::vector< TMusrRunObject<TStringVector> > fStringVectorObj;
  std::vector< TMusrRunObject<TIntVector> > fIntVectorObj;
  std::vector< TMusrRunObject<TDoubleVector> > fDoubleVectorObj;

  std::vector< TString > fPathNameOrder; ///< keeps the path-name as they were created in ordered to keep ordering

  virtual void Init(TString str="n/a");
  virtual void CleanUp();

  virtual UInt_t GetDecimalPlace(Double_t val);
  virtual UInt_t GetLeastSignificantDigit(Double_t val) const;
  virtual void SplitPathName(TString pathName, TString &path, TString &name);

  virtual TString GetLabel(TString str);
  virtual TString GetStrValue(TString str);
  virtual TString GetType(TString str);

  virtual bool UpdateFolder(TObject *treeObj, TString path);
  virtual TObject* FindObject(TObject *treeObj, TString path);
  virtual TObjString GetHeaderString(UInt_t idx);

  virtual bool RemoveFirst(TString &str, const char splitter);
  virtual TString GetFirst(TString &str, const char splitter);

  ClassDef(TMusrRunHeader, 1)
};

#endif // TMUSRRUNHEADER_H
