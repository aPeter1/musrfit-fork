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
#include <TObjString.h>
#include <TObjArray.h>

class TPsiRunProperty : public TObject
{
public:
  TPsiRunProperty();
  TPsiRunProperty(TString &name, Double_t demand, Double_t value, Double_t error, TString &unit, TString &description, TString &path);
  virtual ~TPsiRunProperty() {}

  virtual TString  GetLabel() const { return fLabel; }
  virtual Double_t GetDemand() const { return fDemand; }
  virtual Double_t GetValue() const { return fValue; }
  virtual Double_t GetError() const { return fError; }
  virtual TString  GetUnit() const { return fUnit; }
  virtual TString  GetDescription() const { return fDescription; }
  virtual TString  GetPath() const { return fPath; }

  virtual void SetLabel(TString &label) { fLabel = label; }
  virtual void SetLabel(const char *label) { fLabel = label; }
  virtual void SetDemand(Double_t val) { fDemand = val; }
  virtual void SetValue(Double_t val) { fValue = val; }
  virtual void SetError(Double_t err) { fError = err; }
  virtual void SetUnit(TString &unit) { fUnit = unit; }
  virtual void SetUnit(const char *unit) { fUnit = unit; }
  virtual void SetDescription(TString &str) { fDescription = str; }
  virtual void SetDescription(const char *str) { fDescription = str; }
  virtual void SetPath(TString &str) { fPath = str; }
  virtual void SetPath(const char *str) { fPath = str; }

private:
  TString  fLabel; ///<  property label, like ’Sample Temperature’ etc.
  Double_t fDemand; ///< demand value of the property, e.g. temperature setpoint
  Double_t fValue; ///<  measured value of the property
  Double_t fError; ///<  estimated error (standard deviation) of the measured property value
  TString  fUnit;   ///< unit of the property
  TString  fDescription; ///< a more detailed description of the property
  TString  fPath; ///< ROOT file path where to place the physical property

  ClassDef(TPsiRunProperty, 1)
};

class TPsiRunHeader : public TObject
{
public:
  TPsiRunHeader();
  virtual ~TPsiRunHeader();

  virtual Bool_t IsValid(Bool_t strict = false);

  virtual TString GetVersion() const { return fVersion; }
  virtual TString GetGenerator() const { return fGenerator; }
  virtual TString GetFileName() const { return fFileName; }
  virtual TString GetRunTitle() const { return fRunTitle; }
  virtual Int_t GetRunNumber() const { return fRunNumber; }
  virtual TDatime GetStartTime() const { return fStartTime; }
  virtual const char* GetStartTimeString() const { return fStartTime.AsSQLString(); }
  virtual TDatime GetStopTime() const { return fStopTime; }
  virtual const char* GetStopTimeString() const { return fStopTime.AsSQLString(); }
  virtual TString GetLaboratory() const { return fLaboratory; }
  virtual TString GetArea() const { return fArea; }
  virtual TString GetInstrument() const { return fInstrument; }
  virtual TString GetMuonSpecies() const { return fMuonSpecies; }
  virtual TString GetSetup() const { return fSetup; }
  virtual TString GetComment() const { return fComment; }
  virtual TString GetSample() const { return fSample; }
  virtual TString GetOrientation() const { return fOrientation; }
  virtual TString GetSampleCryo() const { return fSampleCryo; }
  virtual TString GetSampleCryoInsert() const { return fSampleCryoInsert; }
  virtual TString GetSampleMagnetName() const { return fMagnetName; }
  virtual Int_t GetNoOfHistos() const { return fNoOfHistos; }
  virtual UInt_t GetNoOfHistoNames() const { return fHistoName.size(); }
  virtual const vector<TString>* GetHistoNames() const { return &fHistoName; }
  virtual TString GetHistoName(UInt_t idx, Bool_t &ok) const;
  virtual Int_t GetHistoLength() const { return fHistoLength; }
  virtual Double_t GetTimeResolution(const char *units) const;
  virtual UInt_t GetNoOfTimeZeroBins() const { return fTimeZeroBin.size(); }
  virtual const vector<UInt_t>* GetTimeZeroBins() const { return &fTimeZeroBin; }
  virtual UInt_t GetTimeZeroBin(UInt_t idx, Bool_t &ok) const;
  virtual const vector<UInt_t>* GetFirstGoodBins() const { return &fFirstGoodBin;}
  virtual UInt_t GetFirstGoodBin(UInt_t idx, Bool_t &ok) const;
  virtual const vector<UInt_t>* GetLastGoodBins() const { return &fLastGoodBin;}
  virtual UInt_t GetLastGoodBin(UInt_t idx, Bool_t &ok) const;
  virtual UInt_t GetNoOfRedGreenHistoOffsets() const { return fRedGreenOffset.size(); }
  virtual const vector<UInt_t>* GetRedGreenHistoOffsets() const { return &fRedGreenOffset; }
  virtual UInt_t GetRedGreenHistoOffset(UInt_t idx, Bool_t &ok) const;
  virtual const vector<TString>* GetRedGreenHistoDescriptions() const { return &fRedGreenDescription; }
  virtual TString GetRedGreenHistoDescription(UInt_t idx, Bool_t &ok) const;
  virtual const TPsiRunProperty* GetProperty(TString name) const;
  virtual const vector<TPsiRunProperty> *GetProperties() const { return &fProperties; }

  virtual TObjArray *GetHeader(UInt_t &count);
  virtual TObjArray *GetSampleEnv(UInt_t &count);
  virtual TObjArray *GetMagFieldEnv(UInt_t &count);
  virtual TObjArray *GetBeamline(UInt_t &count);
  virtual TObjArray *GetScaler(UInt_t &count);

  virtual Bool_t ExtractHeaderInformation(TObjArray *headerInfo, TString path="/");

  virtual void SetGenerator(TString generator) { fGenerator = generator; }
  virtual void SetFileName(TString fileName) { fFileName = fileName; }
  virtual void SetRunTitle(TString runTitle) { fRunTitle = runTitle; }
  virtual void SetRunNumber(Int_t runNumber) { fRunNumber = runNumber; }
  virtual void SetStartTime(TString startTime);
  virtual void SetStopTime(TString stopTime);
  virtual void SetLaboratory(TString lab) { fLaboratory = lab; }
  virtual void SetArea(TString area) { fArea = area;}
  virtual void SetInstrument(TString insturment) { fInstrument = insturment; }
  virtual void SetMuonSpecies(TString muonSpecies) { fMuonSpecies = muonSpecies; }
  virtual void SetSetup(TString setup) { fSetup = setup; }
  virtual void SetComment(TString comment) { fComment = comment; }
  virtual void SetSample(TString sample) { fSample = sample; }
  virtual void SetOrientation(TString orientation) { fOrientation = orientation; }
  virtual void SetSampleCryo(TString cryoName) { fSampleCryo = cryoName; }
  virtual void SetSampleCryoInsert(TString cryoInsert) { fSampleCryoInsert = cryoInsert; }
  virtual void SetSampleMagnetName(TString name) { fMagnetName = name; }
  virtual void SetNoOfHistos(UInt_t noHistos) { fNoOfHistos = noHistos; }
  virtual void SetHistoNames(vector<TString> names) { fHistoName = names; }
  virtual void SetHistoName(TString name, Int_t idx=-1);
  virtual void SetHistoLength(UInt_t length) { fHistoLength = (Int_t)length; }
  virtual void SetTimeResolution(Double_t value, TString units);
  virtual void SetTimeZeroBins(vector<UInt_t> timeZeroBins) { fTimeZeroBin = timeZeroBins; }
  virtual void SetTimeZeroBin(UInt_t timeZeroBin, Int_t idx=-1);
  virtual void SetFirstGoodBins(vector<UInt_t> fgb) { fFirstGoodBin = fgb; }
  virtual void SetFirstGoodBin(UInt_t fgb, Int_t idx=-1);
  virtual void SetLastGoodBins(vector<UInt_t> lgb) { fLastGoodBin = lgb; }
  virtual void SetLastGoodBin(UInt_t lgb, Int_t idx=-1);
  virtual void SetRedGreenHistogramOffsets(vector<UInt_t> offsets) { fRedGreenOffset = offsets; }
  virtual void SetRedGreenHistogramOffset(UInt_t offset, Int_t idx=-1);
  virtual void SetRedGreenDescriptions(vector<TString> description) { fRedGreenDescription = description; }
  virtual void SetRedGreenDescription(TString description, Int_t idx=-1);
  virtual void AddProperty(TPsiRunProperty &property);
  virtual void AddProperty(TString name, Double_t demand, Double_t value, Double_t error, TString unit, TString desciption=TString(""), TString path=TString("/"));


  virtual void DumpHeader() const;
  virtual void DrawHeader() const;

private:
  TString  fVersion;     ///< SVN version of the TPsiRunHeader
  TString  fGenerator;   ///< program which generated the PSI-ROOT file
  TString  fFileName;    ///< file name of the PSI-ROOT file
  TString  fRunTitle;    ///< run title
  Int_t    fRunNumber;   ///< run number
  TDatime  fStartTime;   ///< run start time
  TDatime  fStopTime;    ///< run stop time
  TString  fLaboratory;  ///< laboratory: PSI
  TString  fArea;        ///< secondary beamline label, e.g. piM3.2
  TString  fInstrument;  ///< instrument name like: GPS, LEM, ....
  TString  fMuonSpecies; ///< postive muon or negative muon
  TString  fSetup;       ///< setup
  TString  fComment;     ///< additional comment
  TString  fSample;      ///< sample name
  TString  fOrientation; ///< sample orientation
  TString  fSampleCryo;  ///< sample cryo
  TString  fSampleCryoInsert; ///< sample cryo insert
  TString  fMagnetName;  ///< name of the magnet used
  Int_t    fNoOfHistos;  ///< number of histos
  vector<TString> fHistoName; ///< names of the individual histograms
  Int_t    fHistoLength; ///< length of the histograms in bins
  Double_t fTimeResolution; ///< time resolution in ps
  vector<UInt_t> fTimeZeroBin; ///< time zero bins
  vector<UInt_t> fFirstGoodBin; ///< first good bins
  vector<UInt_t> fLastGoodBin; ///< last good bins
  vector<UInt_t> fRedGreenOffset; ///< red/green mode histogram offsets
  vector<TString> fRedGreenDescription; ///< red/green mode description
  vector<TPsiRunProperty> fProperties;

  TObjArray fHeader;      ///< header as TObjString array for dumping into a ROOT file
  TObjArray fSampleEnv;   ///< sample environment as TObjString array for dumping into a ROOT file
  TObjArray fMagFieldEnv; ///< sample magnetic field environment as TObjString array for dumping into a ROOT file
  TObjArray fBeamline;    ///< beamline info as TObjString array for dumping into a ROOT file
  TObjArray fScalers;     ///< scaler info as TObjString array for dumping into a ROOT file

  virtual Bool_t DecodePhyscialPorperty(TObjString *ostr, TPsiRunProperty &prop, TString &path);

  virtual UInt_t GetDecimalPlace(Double_t val);
  virtual UInt_t GetLeastSignificantDigit(Double_t val) const;

  virtual void CleanUp(TObject *obj);

  ClassDef(TPsiRunHeader, 1)
};

#endif // TPSIRUNHEADER_H
