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

#include <TObject.h>
#include <TObjString.h>
#include <TObjArray.h>

class TPsiRunProperty : public TObject
{
public:
  TPsiRunProperty();
  TPsiRunProperty(TObjString &name, Double_t value, Double_t error, TObjString &unit);
  TPsiRunProperty(TString &name, Double_t value, Double_t error, TString &unit);
  virtual ~TPsiRunProperty();

  virtual TObjString GetName() { return fName; }
  virtual Double_t   GetValue() { return fValue; }
  virtual Double_t   GetError() { return fError; }
  virtual TObjString GetUnit() { return fUnit; }

  virtual void SetName(TObjString &name) { fName = name; }
  virtual void SetName(TString &name) { fName = name.Data(); }
  virtual void SetName(const char *name) { fName = name; }
  virtual void SetValue(Double_t val) { fValue = val; }
  virtual void SetError(Double_t err) { fError = err; }
  virtual void SetUnit(TObjString &unit) { fUnit = unit; }
  virtual void SetUnit(TString &unit) { fUnit = unit.Data(); }
  virtual void SetUnit(const char *unit) { fUnit = unit; }

private:
  TObjString fName;
  Double_t   fValue;
  Double_t   fError;
  TObjString fUnit;

  ClassDef(TPsiRunProperty, 1)
};

class TPsiRunHeader : public TObject
{
public:
  TPsiRunHeader();
  virtual ~TPsiRunHeader();

  virtual TString GetVersion() const;
  virtual TString GetRunTitle() const;
  virtual Int_t GetRunNumber() const;
  virtual TString GetLab() const;
  virtual TString GetInstrument() const;
  virtual TString GetSetup() const;
  virtual TString GetSample() const;
  virtual TString GetOrientation() const;
  virtual TObjArray *GetProperties() { return &fProperties; }

  virtual void SetRunTitle(TString runTitle);
  virtual void SetRunNumber(Int_t runNumber);
  virtual void SetLab(TString lab);
  virtual void SetInstrument(TString insturment);
  virtual void SetSetup(TString setup);
  virtual void SetSample(TString sample);
  virtual void SetOrientation(TString setup);
  virtual void AddProperty(TPsiRunProperty &property);
  virtual void AddProperty(TString name, Double_t value, Double_t error, TString unit);

  virtual void DumpHeader() const;
  virtual void DrawHeader() const;

private:
  TObjString fVersion;     /// SVN version of the TPsiRunHeader
  TObjString fRunTitle;    /// run title
  TObjString fRunNumber;   /// run number
  TObjString fLaboratory;  /// laboratory: PSI
  TObjString fInstrument;  /// instrument name like: GPS, LEM, ....
  TObjString fSetup;       /// setup
  TObjString fSample;      /// sample name
  TObjString fOrientation; /// sample orientation
  TObjArray  fPropertiesList;
  TObjArray  fProperties;

  virtual TString GetInfoString(const TObjString &tostr) const;

  ClassDef(TPsiRunHeader, 1)
};

#endif // TPSIRUNHEADER_H
