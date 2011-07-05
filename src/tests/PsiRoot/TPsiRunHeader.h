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
  TPsiRunProperty(TString &name, Double_t value, Double_t error, TString &unit);
  virtual ~TPsiRunProperty();

  virtual TString GetLabel() const { return fLabel; }
  virtual Double_t GetValue() const { return fValue; }
  virtual Double_t GetError() const { return fError; }
  virtual TString  GetUnit() const { return fUnit; }

  virtual void SetLabel(TString &label) { fLabel = label; }
  virtual void SetLabel(const char *label) { fLabel = label; }
  virtual void SetValue(Double_t val) { fValue = val; }
  virtual void SetError(Double_t err) { fError = err; }
  virtual void SetUnit(TString &unit) { fUnit = unit.Data(); }
  virtual void SetUnit(const char *unit) { fUnit = unit; }

private:
  TString  fLabel;
  Double_t fValue;
  Double_t fError;
  TString  fUnit;

  ClassDef(TPsiRunProperty, 1)
};

class TPsiRunHeader : public TObject
{
public:
  TPsiRunHeader();
  virtual ~TPsiRunHeader();

  virtual TString GetVersion() const { return fVersion; }
  virtual TString GetRunTitle() const { return fRunTitle; }
  virtual Int_t GetRunNumber() const { return fRunNumber; }
  virtual TString GetLab() const { return fLaboratory; }
  virtual TString GetInstrument() const { return fInstrument; }
  virtual TString GetSetup() const { return fSetup; }
  virtual TString GetSample() const { return fSample; }
  virtual TString GetOrientation() const { return fOrientation; }
  virtual vector<TPsiRunProperty> *GetProperties() { return &fProperties; }

  virtual TObjArray *GetHeader();

  virtual void SetRunTitle(TString runTitle) { fRunTitle = runTitle; }
  virtual void SetRunNumber(Int_t runNumber) { fRunNumber = runNumber; }
  virtual void SetLab(TString lab) { fLaboratory = lab; }
  virtual void SetInstrument(TString insturment) { fInstrument = insturment; }
  virtual void SetSetup(TString setup) { fSetup = setup; }
  virtual void SetSample(TString sample) { fSample = sample; }
  virtual void SetOrientation(TString orientation) { fOrientation = orientation; }
  virtual void AddProperty(TPsiRunProperty &property);
  virtual void AddProperty(TString name, Double_t value, Double_t error, TString unit);


  virtual void DumpHeader() const;
  virtual void DrawHeader() const;

private:
  TString fVersion;     /// SVN version of the TPsiRunHeader
  TString fRunTitle;    /// run title
  Int_t   fRunNumber;   /// run number
  TString fLaboratory;  /// laboratory: PSI
  TString fInstrument;  /// instrument name like: GPS, LEM, ....
  TString fSetup;       /// setup
  TString fSample;      /// sample name
  TString fOrientation; /// sample orientation
  vector<TPsiRunProperty> fProperties;

  TObjArray fHeader; /// header as TObjString array for dumping into a ROOT file

  UInt_t GetDecimalPlace(Double_t val);

  ClassDef(TPsiRunHeader, 1)
};

#endif // TPSIRUNHEADER_H
