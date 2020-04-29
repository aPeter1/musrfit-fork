/***************************************************************************

  PNeXus.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2020 by Andreas Suter                              *
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

#ifndef _PNEXUS_H_
#define _PNEXUS_H_

#include <string>
#include <vector>

#include "napi.h"

#ifndef VGNAMELENMAX
#define VGNAMELENMAX	    64
#endif /* VGNAMELENMAX */

#define PNEXUS_NO_ERROR             0
#define PNEXUS_NXENTRY_NOT_FOUND    1
#define PNEXUS_FILE_OPEN_ERROR      2
#define PNEXUS_GROUP_OPEN_ERROR     3
#define PNEXUS_OPEN_DATA_ERROR      4
#define PNEXUS_GET_DATA_ERROR       5
#define PNEXUS_GET_ATTR_ERROR       6
#define PNEXUS_CLOSE_DATA_ERROR     7
#define PNEXUS_GET_META_INFO_ERROR  8
#define PNEXUS_WRONG_META_INFO      9
#define PNEXUS_WRONG_DATE_FORMAT    10
#define PNEXUS_WRONG_TIME_FORMAT    11
#define PNEXUS_INIT_GROUPDIR_ERROR  12
#define PNEXUS_GET_GROUP_INFO_ERROR 13
#define PNEXUS_GET_NEXT_ENTRY_ERROR 14
#define PNEXUS_HISTO_ERROR          15
#define PNEXUS_SET_ATTR_ERROR       16
#define PNEXUS_CREATE_GROUP_ERROR   17
#define PNEXUS_MAKE_DATA_ERROR      18
#define PNEXUS_PUT_DATA_ERROR       19
#define PNEXUS_PUT_ATTR_ERROR       20
#define PNEXUS_IDF_NOT_IMPLEMENTED  21
#define PNEXUS_VAILD_READ_IDF1_FILE 22
#define PNEXUS_VAILD_READ_IDF2_FILE 23
#define PNEXUS_OBJECT_INVOK_ERROR   24
#define PNEXUS_TIME_FORMAT_ERROR    25
#define PNEXUS_DATA_ERROR           26
#define PNEXUS_NXUSER_NOT_FOUND     27
#define PNEXUS_LINKING_ERROR        28

class PNeXusProp {
  public:
    PNeXusProp();
    virtual ~PNeXusProp() {}

    virtual std::string GetName() { return fName; }
    virtual double GetValue() { return fValue; }
    virtual std::string GetUnit() { return fUnit; }

    virtual void SetName(std::string name) { fName = name; }
    virtual void SetValue(double val) { fValue = val; }
    virtual void SetUnit(std::string unit) { fUnit = unit; }

  private:
    std::string fName;
    double fValue;
    std::string fUnit;
};

class PNeXusBeam1 {
  public:
    PNeXusBeam1() { fTotalCounts = 0; fUnits = "n/a"; }
    virtual ~PNeXusBeam1() {}

    virtual bool IsValid(bool strict);

    virtual double GetTotalCounts() { return fTotalCounts; }
    virtual std::string GetUnits() { return fUnits; }

    virtual void SetTotalCounts(double counts) { fTotalCounts = counts; }
    virtual void SetUnits(std::string units) { fUnits = units; }

  private:
    double fTotalCounts; ///< total number of counts
    std::string fUnits; ///< 'units' in which total counts is given, e.g. 'Mev'
};

class PNeXusCollimator1 {
  public:
    PNeXusCollimator1() { fType = "n/a"; }
    virtual ~PNeXusCollimator1() {}

    virtual bool IsValid(bool strict) { return true; } // currently only a dummy

    virtual std::string GetType() { return fType; }
    virtual void SetType(std::string type) { fType = type; }

  private:
    std::string fType;
};

class PNeXusDetector1 {
  public:
    PNeXusDetector1() { fNumber = 0; }
    virtual ~PNeXusDetector1() {}

    virtual bool IsValid(bool strict);

    virtual int GetNumber() { return fNumber; }
    virtual void SetNumber(int number) { fNumber = number; }

  private:
    int fNumber;
};

class PNeXusInstrument1 {
  public:
    PNeXusInstrument1() { fName = "n/a"; }
    virtual ~PNeXusInstrument1() {}

    virtual bool IsValid(bool strict);

    virtual std::string GetName() { return fName; }
    virtual PNeXusDetector1* GetDetector() { return &fDetector; }
    virtual PNeXusCollimator1* GetCollimator() { return &fCollimator; }
    virtual PNeXusBeam1* GetBeam() { return &fBeam; }

    virtual void SetName(std::string name) { fName = name; }
    virtual void SetDetector(PNeXusDetector1 &detector) { fDetector = detector; }
    virtual void SetCollimator(PNeXusCollimator1 &collimator) { fCollimator = collimator; }
    virtual void SetBeam(PNeXusBeam1 &beam) { fBeam = beam; }

  private:
    std::string fName; ///< instrument name
    PNeXusDetector1 fDetector;
    PNeXusCollimator1 fCollimator;
    PNeXusBeam1 fBeam;
};

class PNeXusSample1 {
  public:
    PNeXusSample1();
    virtual ~PNeXusSample1();

    virtual bool IsValid(bool strict);

    virtual std::string GetName() { return fName; }
    virtual std::string GetShape() { return fShape; }
    virtual std::string GetMagneticFieldState() { return fMagneticFieldState; }
    virtual std::string GetEnvironment() { return fEnvironment; }
    virtual double GetPhysPropValue(std::string name, bool &ok);
    virtual void GetPhysPropUnit(std::string name, std::string &unit, bool &ok);
    virtual int IsMagneticFieldVectorAvailable() { return fMagneticFieldVectorAvailable; }
    virtual std::vector<double> GetMagneticFieldVector() { return fMagneticFieldVector; }
    virtual std::string GetMagneticFieldVectorUnits() { return fMagneticFieldVectorUnits; }
    virtual std::string GetMagneticFieldVectorCoordinateSystem() { return fMagneticFieldVectorCoordinateSystem; }

    virtual void SetName(std::string name) { fName = name; }
    virtual void SetShape(std::string shape) { fShape = shape; }
    virtual void SetMagneticFieldState(std::string magFieldState) { fMagneticFieldState = magFieldState; }
    virtual void SetEnvironment(std::string env) { fEnvironment = env; }
    virtual void SetPhysProp(std::string name, double value, std::string unit, int idx=-1);
    virtual void SetMagneticFieldVectorAvailable(int avail) { fMagneticFieldVectorAvailable = avail; }
    virtual void SetMagneticFieldVector(std::vector<double> &magVec) { fMagneticFieldVector = magVec; }
    virtual void SetMagneticFieldVectorCoordinateSystem(std::string coord) { fMagneticFieldVectorCoordinateSystem = coord; }
    virtual void SetMagneticFieldUnits(std::string units) { fMagneticFieldVectorUnits = units; }

  private:
    std::string fName; ///< sample name
    std::string fShape; ///< sample orientation
    std::string fMagneticFieldState; ///< magnetic field state, e.g. TF, ZF, ...
    std::string fEnvironment; ///< sample environment, e.g. CCR, Konti-1, ...
    std::vector<PNeXusProp> fPhysProp; ///< collects the temperature, magnetic field

    int fMagneticFieldVectorAvailable; ///< flag '0' magnetic field vector not available, '1' magnetic field vector available
    std::vector<double> fMagneticFieldVector; ///< magnetic field vector
    std::string fMagneticFieldVectorUnits; ///< units in which the magnetic field vector is given
    std::string fMagneticFieldVectorCoordinateSystem; ///< coordinate system, e.g. 'cartesian'
};

class PNeXusUser1 {
  public:
    PNeXusUser1();
    virtual ~PNeXusUser1() {}

    virtual bool IsValid(bool strict) { return true; } // currently only a dummy

    virtual std::string GetName() { return fName; }
    virtual std::string GetExperimentNumber() { return fExperimentNumber; }

    virtual void SetName(std::string name) { fName = name; }
    virtual void SetExperimentNumber(std::string expNum) { fExperimentNumber = expNum; }

  private:
    std::string fName; ///< user name
    std::string fExperimentNumber; ///< experiment number, RB number at ISIS
};

class PNeXusAlpha1 {
  public:
    PNeXusAlpha1();
    virtual ~PNeXusAlpha1() {}

    virtual unsigned int GetGroupFirst() { return fGroupFirst; }
    virtual unsigned int GetGroupSecond() { return fGroupSecond; }
    virtual double GetAlpha() { return fAlphaVal; }

    virtual void SetGroupFirst(unsigned int val) { fGroupFirst = val; }
    virtual void SetGroupSecond(unsigned int val) { fGroupSecond = val; }
    virtual void SetAlpha(double val) { fAlphaVal = val; }

  private:
    unsigned int fGroupFirst;
    unsigned int fGroupSecond;
    double fAlphaVal;
};

class PNeXusData1 {
  public:
    PNeXusData1() { fTimeResolution = 0.0; }
    virtual ~PNeXusData1();

    virtual bool IsValid(bool strict);

    virtual double GetTimeResolution(std::string units);
    virtual std::vector<unsigned int> *GetT0s() { return &fT0; }
    virtual int GetT0(unsigned int idx);
    virtual std::vector<unsigned int> *GetFirstGoodBins() { return &fFirstGoodBin; }
    virtual int GetFirstGoodBin(unsigned int idx);
    virtual std::vector<unsigned int> *GetLastGoodBins() { return &fLastGoodBin; }
    virtual int GetLastGoodBin(unsigned int idx);
    virtual std::vector<std::string> *GetHistoNames() { return &fHistoName; }
    virtual void GetHistoName(unsigned int idx, std::string &name, bool &ok);
    virtual unsigned int GetNoOfHistos() { return fHisto.size(); }
    virtual unsigned int GetHistoLength(unsigned int histoNo=0);
    virtual std::vector<unsigned int> *GetHisto(unsigned int histoNo);
    virtual std::vector<int> *GetGrouping() { return &fGrouping; }
    virtual std::vector<PNeXusAlpha1> *GetAlpha() { return &fAlpha; }

    virtual void SetTimeResolution(double val, std::string units);
    virtual void SetT0(unsigned int t0, int idx=-1);
    virtual void SetFirstGoodBin(unsigned int fgb, int idx=-1);
    virtual void SetLastGoodBin(unsigned int lgb, int idx=-1);
    virtual void FlushHistos();
    virtual void SetHisto(std::vector<unsigned int> &data, int histoNo=-1);
    virtual void FlushGrouping() { fGrouping.clear(); }
    virtual void SetGrouping(std::vector<int> &grouping) { fGrouping = grouping; }
    virtual void FlushAlpha() { fAlpha.clear(); }
    virtual void SetAlpha(std::vector<PNeXusAlpha1> &alpha) { fAlpha = alpha; }

  private:
    double fTimeResolution;                   ///< time resolution in (ps)
    std::vector<unsigned int> fT0;
    std::vector<unsigned int> fFirstGoodBin;
    std::vector<unsigned int> fLastGoodBin;
    std::vector<std::string> fHistoName;
    std::vector< std::vector<unsigned int> > fHisto;
    std::vector<int> fGrouping;
    std::vector<PNeXusAlpha1> fAlpha;
};

class PNeXusEntry1 {
  public:
    PNeXusEntry1();
    virtual ~PNeXusEntry1() {}

    virtual bool IsValid(bool strict);

    virtual std::string GetProgramName() { return fProgramName; }
    virtual std::string GetProgramVersion() { return fProgramVersion; }
    virtual int GetRunNumber() { return fRunNumber; }
    virtual std::string GetTitle() { return fTitle; }
    virtual std::string GetNotes() { return fNotes; }
    virtual std::string GetAnalysis() { return fAnalysis; }
    virtual std::string GetLaboratory() { return fLaboratory; }
    virtual std::string GetBeamline() { return fBeamline; }
    virtual std::string GetStartTime() { return fStartTime; }
    virtual std::string GetStopTime() { return fStopTime; }
    virtual int GetSwitchingState() { return fSwitchingState; }
    virtual PNeXusUser1* GetUser() { return &fUser; }
    virtual PNeXusSample1* GetSample()  { return &fSample; }
    virtual PNeXusInstrument1* GetInstrument() { return &fInstrument; }
    virtual PNeXusData1* GetData() { return &fData; }

    virtual void SetProgramName(std::string name) { fProgramName = name; }
    virtual void SetProgramVersion(std::string version) { fProgramVersion = version; }
    virtual void SetRunNumber(int number) { fRunNumber = number; }
    virtual void SetTitle(std::string title) { fTitle = title; }
    virtual void SetNotes(std::string notes) { fNotes = notes; }
    virtual void SetAnalysis(std::string analysis) { fAnalysis = analysis; }
    virtual void SetLaboratory(std::string lab) { fLaboratory = lab; }
    virtual void SetBeamline(std::string beamline) { fBeamline = beamline; }
    virtual int SetStartTime(std::string time);
    virtual int SetStopTime(std::string time);
    virtual int SetSwitchingState(int state);
    virtual void SetUser(PNeXusUser1 &user) { fUser = user; }
    virtual void SetSample(PNeXusSample1 &sample) { fSample = sample; }
    virtual void SetInstrument(PNeXusInstrument1 &instrument) { fInstrument = instrument; }
    virtual void SetData(PNeXusData1 &data) { fData = data; }

  private:
    std::string fProgramName;      ///< name of the creating program
    std::string fProgramVersion;   ///< version of the creating program
    int fRunNumber;           ///< run number
    std::string fTitle;            ///< string containing the run title
    std::string fNotes;            ///< comments
    std::string fAnalysis;         ///< type of muon experiment "muonTD", "ALC", ...
    std::string fLaboratory;       ///< name of the laboratory where the data are taken, e.g. PSI, triumf, ISIS, J-Parc
    std::string fBeamline;         ///< name of the beamline used for the experiment, e.g. muE4
    std::string fStartTime;        ///< start date/time of the run
    std::string fStopTime;         ///< stop date/time of the run
    int fSwitchingState;      ///< '1' normal data collection, '2' Red/Green mode
    PNeXusUser1 fUser;        ///< NXuser info IDF Version 1
    PNeXusSample1 fSample;    ///< NXsample info IDF Version 1
    PNeXusInstrument1 fInstrument; ///< NXinstrument info IDF Version 1
    PNeXusData1 fData;        ///< NXdata info IDF Version 1
};

class PNeXusSource2 {
  public:
    PNeXusSource2();
    virtual ~PNeXusSource2() {}

    virtual bool IsValid(bool strict);

    virtual std::string GetName() { return fName; }
    virtual std::string GetType() { return fType; }
    virtual std::string GetProbe() { return fProbe; }

    virtual void SetName(std::string name) { fName = name; }
    virtual void SetType(std::string type) { fType = type; }
    virtual void SetProbe(std::string probe) { fProbe = probe; }

  private:
    std::string fName; ///< facility name
    std::string fType; ///< continous muon source, pulsed muon source, low energy muon source, ...
    std::string fProbe; ///< positive muon, negative muon
};

class PNeXusBeamline2 {
  public:
    PNeXusBeamline2() { fName = "n/a"; }
    virtual ~PNeXusBeamline2() {}

    virtual bool IsValid(bool strict);

    virtual std::string GetName() { return fName; }

    virtual void SetName(std::string name) { fName = name; }

  private:
    std::string fName;
};

class PNeXusDetector2 {
  public:
    PNeXusDetector2();
    virtual ~PNeXusDetector2();    

    virtual bool IsValid(bool strict);
    virtual std::string GetErrorMsg() { return fErrorMsg; }

    virtual std::string GetDescription() { return fDescription; }
    virtual double GetTimeResolution(std::string units);
    virtual std::vector<double> *GetRawTime() { return &fRawTime; }
    virtual std::string GetRawTimeName() { return fRawTimeName; }
    virtual std::string GetRawTimeUnit() { return fRawTimeUnit; }
    virtual bool IsT0Present() { return (fT0 == nullptr) ? false : true; }
    virtual int GetT0Tag() { return fT0Tag; }
    virtual int GetT0(int idxp=-1, int idxs=-1);
    virtual int* GetT0s() { return fT0; }
    virtual bool IsFirstGoodBinPresent() { return (fFirstGoodBin == nullptr) ? false : true; }
    virtual int GetFirstGoodBin(int idxp=-1, int idxs=-1);
    virtual int* GetFirstGoodBins() { return fFirstGoodBin; }
    virtual bool IsLastGoodBinPresent() { return (fLastGoodBin == nullptr) ? false : true; }
    virtual int GetLastGoodBin(int idxp=-1, int idxs=-1);
    virtual int* GetLastGoodBins() { return fLastGoodBin; }
    virtual int GetNoOfPeriods() { return fNoOfPeriods; }
    virtual int GetNoOfSpectra() { return fNoOfSpectra; }
    virtual int GetNoOfBins() { return fNoOfBins; }
    virtual int GetHistoValue(int idx_p, int idx_s, int idx_b);
    virtual int* GetHistos() { return fHisto; }
    virtual unsigned int GetSpectrumIndexSize() { return fSpectrumIndex.size(); }
    virtual std::vector<int> *GetSpectrumIndex() { return &fSpectrumIndex; }
    virtual int GetSpectrumIndex(unsigned int idx);

    virtual void SetDescription(std::string description) { fDescription = description; }
    virtual void SetTimeResolution(double val, std::string units);
    virtual void SetRawTime(std::vector<double> &rawTime);
    virtual void SetRawTimeName(std::string rawTimeName) { fRawTimeName = rawTimeName; }
    virtual void SetRawTimeUnit(std::string rawTimeUnit) { fRawTimeUnit = rawTimeUnit; }
    virtual void SetT0Tag(int tag) { fT0Tag = tag; }
    virtual int  SetT0(int *t0);
    virtual int  SetFirstGoodBin(int *fgb);
    virtual int  SetLastGoodBin(int *lgb);
    virtual void SetNoOfPeriods(int val) { fNoOfPeriods = val; }
    virtual void SetNoOfSpectra(int val) { fNoOfSpectra = val; }
    virtual void SetNoOfBins(int val) { fNoOfBins = val; }
    virtual int  SetHistos(int *histo);
    virtual void SetSpectrumIndex(std::vector<int> spectIdx) { fSpectrumIndex = spectIdx; }
    virtual void SetSpectrumIndex(int spectIdx, int idx=-1);

  private:
    std::string fErrorMsg;      ///< internal error message
    std::string fDescription;   ///< description of the detector
    double fTimeResolution;     ///< keeps the time resolution in (ps)
    std::vector<int> fSpectrumIndex; ///< list of global spectra
    std::vector<double> fRawTime;    ///< keeps a raw time vector
    std::string fRawTimeName;   ///< name of the raw time vector
    std::string fRawTimeUnit;   ///< unit of the raw time vector

    int fNoOfPeriods; ///< number of periods or -1 if not defined
    int fNoOfSpectra; ///< number of spectra or -1 if not defined
    int fNoOfBins;    ///< number of bins of a spectrum or -1 if not defined

    int fT0Tag; ///< tag for t0, fgb, lgb structure. -1==not present, 1==NX_INT, 2==NX_INT[ns], 3==NX_INT[np][ns]
    int *fT0;
    int *fFirstGoodBin;
    int *fLastGoodBin;
    int *fHisto;
};

class PNeXusInstrument2 {
  public:
    PNeXusInstrument2() { fName = "n/a"; }
    virtual ~PNeXusInstrument2() {}

    virtual bool IsValid(bool strict);

    virtual std::string GetName() { return fName; }
    virtual PNeXusSource2* GetSource() { return &fSource; }
    virtual PNeXusBeamline2* GetBeamline() { return &fBeamline; }
    virtual PNeXusDetector2* GetDetector() { return &fDetector; }

    virtual void SetName(std::string name) { fName = name; }
    virtual void SetSource(PNeXusSource2 &source) { fSource = source; }
    virtual void SetBeamline(PNeXusBeamline2 &beamline) { fBeamline = beamline; }
    virtual void SetDetector(PNeXusDetector2 &detector) { fDetector = detector; }

  private:
    std::string fName; ///< name of the instrument
    PNeXusSource2 fSource; ///< details of the muon source used
    PNeXusBeamline2 fBeamline; ///< beamline description
    PNeXusDetector2 fDetector; ///< details of the detectors which also includes the data!!
};

class PNeXusSample2 {
  public:
    PNeXusSample2();
    virtual ~PNeXusSample2();

    virtual bool IsValid(bool strict);

    virtual std::string GetName() { return fName; }
    virtual std::string GetDescription() { return fDescription; }
    virtual std::string GetMagneticFieldState() { return fMagneticFieldState; }
    virtual std::string GetEnvironmentTemp() { return fEnvironmentTemp; }
    virtual std::string GetEnvironmentField() { return fEnvironmentField; }
    virtual double GetPhysPropValue(std::string name, bool &ok);
    virtual void GetPhysPropUnit(std::string name, std::string &unit, bool &ok);

    virtual void SetName(std::string name) { fName = name; }
    virtual void SetDescription(std::string description) { fDescription = description; }
    virtual void SetMagneticFieldState(std::string magFieldState) { fMagneticFieldState = magFieldState; }
    virtual void SetEnvironmentTemp(std::string env) { fEnvironmentTemp = env; }
    virtual void SetEnvironmentField(std::string env) { fEnvironmentField = env; }
    virtual void SetPhysProp(std::string name, double value, std::string unit, int idx=-1);

  private:
    std::string fName; ///< sample name
    std::string fDescription; ///< sample description
    std::string fMagneticFieldState; ///< magnetic field state, e.g. TF, ZF, ...
    std::string fEnvironmentTemp; ///< sample environment related to temperature, e.g. CCR, Konti-1, ...
    std::string fEnvironmentField; ///< sample environment related to field, e.g. WEW-Bruker
    std::vector<PNeXusProp> fPhysProp; ///< collects the temperature, magnetic field
};

class PNeXusUser2 {
  public:
    PNeXusUser2() { fName = "n/a"; }
    virtual ~PNeXusUser2() {}

    virtual bool IsValid(bool strict) { return true; } // currently only a dummy

    virtual std::string GetName() { return fName; }

    virtual void SetName(std::string name) { fName = name; }

  private:
    std::string fName; ///< user name
};

class PNeXusEntry2 {
  public:
    PNeXusEntry2();
    virtual ~PNeXusEntry2() {}

    virtual bool IsValid(bool strict);

    virtual std::string GetErrorMsg() { return fErrorMsg; }
    virtual std::string GetDefinition() { return fDefinition; }
    virtual std::string GetProgramName() { return fProgramName; }
    virtual std::string GetProgramVersion() { return fProgramVersion; }
    virtual int GetRunNumber() { return fRunNumber; }
    virtual std::string GetTitle() { return fTitle; }
    virtual std::string GetStartTime() { return fStartTime; }
    virtual std::string GetStopTime() { return fStopTime; }
    virtual std::string GetExperimentIdentifier() { return fExperimentIdentifier; }
    virtual PNeXusUser2* GetUser() { return &fUser; }
    virtual PNeXusSample2* GetSample() { return &fSample; }
    virtual PNeXusInstrument2* GetInstrument() { return &fInstrument; }

    virtual void SetDefinition(std::string def) { fDefinition = def; }
    virtual void SetProgramName(std::string name) { fProgramName = name; }
    virtual void SetProgramVersion(std::string version) { fProgramVersion = version; }
    virtual void SetRunNumber(int number) { fRunNumber = number; }
    virtual void SetTitle(std::string title) { fTitle = title; }
    virtual int SetStartTime(std::string time);
    virtual int SetStopTime(std::string time);
    virtual void SetExperimentIdentifier(std::string expId) { fExperimentIdentifier = expId; }
    virtual void SetUser(PNeXusUser2 &user) { fUser = user; }
    virtual void SetSample(PNeXusSample2 &sample) { fSample = sample; }
    virtual void SetInstrument(PNeXusInstrument2 &instrument) { fInstrument = instrument; }

  private:
    std::string fErrorMsg;    ///< internal error message
    std::string fDefinition;  ///< the template (DTD name) on which the entry was based, e.g. 'pulsedTD'
    std::string fProgramName; ///< name of the creating program
    std::string fProgramVersion; ///< version of the creating program
    int fRunNumber;           ///< run number
    std::string fTitle;       ///< string containing the run title
    std::string fStartTime;   ///< start date/time of the run
    std::string fStopTime;    ///< stop date/time of the run
    std::string fExperimentIdentifier; ///< experiment number, (for ISIS, the RB number)
    PNeXusUser2 fUser;        ///< NXuser info IDF Version 2
    PNeXusSample2 fSample;    ///< NXsample info IDF Version 2
    PNeXusInstrument2 fInstrument; ///< NXinstrument inf IDF Version 2
};

class PNeXus {
  public:
    PNeXus();
    PNeXus(const char* fileName);
    virtual ~PNeXus();

    virtual int GetIdfVersion() { return fIdfVersion; }
    virtual std::string GetFileName() { return fFileName; }
    virtual std::string GetFileTime() { return fFileTime; }

    virtual void SetIdfVersion(unsigned int idf);
    virtual void SetFileName(std::string name) { fFileName = name; }
    virtual void SetFileTime(std::string time) { fFileTime = time; }

    virtual PNeXusEntry1* GetEntryIdf1() { return fNxEntry1; }
    virtual PNeXusEntry2* GetEntryIdf2() { return fNxEntry2; }

    virtual bool IsValid(bool strict=false);
    virtual int GetErrorCode() { return fErrorCode; }
    virtual std::string GetErrorMsg() { return fErrorMsg; }

    virtual std::vector<unsigned int>* GetGroupedHisto(unsigned int idx);

    virtual int ReadFile(const char *fileName);
    virtual int WriteFile(const char *fileName, const char *fileType="hdf4", const unsigned int idf=2);

    virtual void SetCreator(std::string str) { fCreator = str; }

    virtual void Dump();

  private:
    bool fValid;
    int fErrorCode;
    std::string fErrorMsg;

    std::string fNeXusVersion; ///< version of the NeXus API used in writing the file
    std::string fFileFormatVersion; ///< version of the HDF, HDF5, or XML library used to create the file (IDF 2 only)

    unsigned int fIdfVersion; ///< version of the instrument definition
    std::string fFileName; ///< file name of the original NeXus file to assist identification if the external name has been changed
    std::string fFileTime; ///< date and time of file creating (IDF 2 only)
    NXhandle fFileHandle;

    std::string fCreator; ///< facility of program where the file originated

    PNeXusEntry1 *fNxEntry1; ///< NXentry for IDF 1
    PNeXusEntry2 *fNxEntry2; ///< NXentry for IDF 2

    std::vector< std::vector<unsigned int> > fGroupedHisto;

    virtual void Init();
    virtual bool ErrorHandler(NXstatus status, int errCode, const std::string &errMsg);
    virtual NXstatus GetStringData(std::string &str);
    virtual NXstatus GetStringAttr(std::string attr, std::string &str);
    virtual int GetDataSize(int type);
    virtual NXstatus GetDoubleVectorData(std::vector<double> &data);
    virtual NXstatus GetIntVectorData(std::vector<int> &data);

    virtual int ReadFileIdf1();
    virtual int ReadFileIdf2();

    virtual int WriteFileIdf1(const char* fileName, const NXaccess access);
    virtual int WriteFileIdf2(const char* fileName, const NXaccess access);

    virtual NXstatus GroupHistoData();

    virtual bool IsValidIdf1(bool strict);
    virtual bool IsValidIdf2(bool strict);

    virtual bool SearchInGroup(std::string str, std::string tag, NXname &nxname, NXname &nxclass, int &dataType);
    virtual bool SearchAttrInData(std::string str, int &length, int &dataType);
};

#endif // _PNEXUS_H_
