/***************************************************************************

  PNeXus.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2011 by Andreas Suter                              *
 *   andreas.suter@psi.ch                                                   *
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

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "napi.h"

#ifndef VGNAMELENMAX
#define VGNAMELENMAX	    64
#endif /* VGNAMELENMAX */

#define PNEXUS_FILE_OPEN_ERROR      0
#define PNEXUS_GROUP_OPEN_ERROR     1
#define PNEXUS_OPEN_DATA_ERROR      2
#define PNEXUS_GET_DATA_ERROR       3
#define PNEXUS_GET_ATTR_ERROR       4
#define PNEXUS_CLOSE_DATA_ERROR     5
#define PNEXUS_GET_META_INFO_ERROR  6
#define PNEXUS_WRONG_META_INFO      7
#define PNEXUS_WRONG_DATE_FORMAT    8
#define PNEXUS_WRONG_TIME_FORMAT    9
#define PNEXUS_INIT_GROUPDIR_ERROR  10
#define PNEXUS_GET_GROUP_INFO_ERROR 11
#define PNEXUS_GET_NEXT_ENTRY_ERROR 12
#define PNEXUS_HISTO_ERROR          13
#define PNEXUS_SET_ATTR_ERROR       14
#define PNEXUS_CREATE_GROUP_ERROR   15
#define PNEXUS_MAKE_DATA_ERROR      16
#define PNEXUS_PUT_DATA_ERROR       17
#define PNEXUS_PUT_ATTR_ERROR       18

//-----------------------------------------------------------------------------
/**
 *
 */
typedef struct {
  string fUserName;
  string fExperimentNumber;
} PNeXusUser;

//-----------------------------------------------------------------------------
/**
 *
 */
typedef struct {
  string fName;
  double fTemperature;
  string fTemperatureUnit;
  double fMagneticField;
  string fMagneticFieldUnit;
  string fShape;
  string fMagneticFieldState; ///< ZF, TF, LF
  int fMagneticFieldVectorAvailable;
  vector<double> fMagneticFieldVector;
  string fMagneticFieldVectorUnits;
  string fMagneticFieldVectorCoordinateSystem;
  string fEnvironment;
} PNeXusSample;

//-----------------------------------------------------------------------------
/**
 * <p> currently only put in the requiered ones.
 */
typedef struct {
  int number;
} PNeXusDetector;

//-----------------------------------------------------------------------------
/**
 * <p> currently only put in the requiered ones.
 */
typedef struct {
  string type;
} PNeXusCollimator;

//-----------------------------------------------------------------------------
/**
 * <p> currently only put in the requiered ones.
 */
typedef struct {
  double total_counts;
  string total_counts_units;
} PNeXusBeam;

//-----------------------------------------------------------------------------
/**
 *
 */
typedef struct {
  string name;
  PNeXusDetector detector;
  PNeXusCollimator collimator;
  PNeXusBeam beam;
} PNeXusInstrument;

//-----------------------------------------------------------------------------
/**
 *
 */
typedef struct {
  unsigned int fGroupFirst;
  unsigned int fGroupSecond;
  double fAlphaVal;
} PNeXusAlpha;

//-----------------------------------------------------------------------------
/**
 *
 */
typedef struct {
  double fTimeResolution;                   ///< time resolution in (ns)
  unsigned int fNumberOfHistos;
  vector<unsigned int> fT0;
  vector<unsigned int> fFirstGoodBin;
  vector<unsigned int> fLastGoodBin;
  vector<string> fHistoName;
  vector< vector<unsigned int> > fHisto;
  vector<unsigned int> fGrouping;
  vector<PNeXusAlpha> fAlpha;
} PNeXusData;

//-----------------------------------------------------------------------------
/**
 *
 */
class PNeXus {
  public:
    PNeXus();
    PNeXus(const char* fileName);
    virtual ~PNeXus();

    virtual bool IsValid() { return fIsValid; }

    virtual int ReadFile(const char *fileName);
    virtual int WriteFile(const char *fileName, const char *fileType="hdf4");

    virtual void Dump();

    virtual int GetErrorCode() { return fErrorCode; }
    virtual string GetErrorMsg() { return fErrorMsg; }
    virtual string GetFileName() { return fFileName; }
    virtual int GetIDFVersion() { return fIDFVersion; }
    virtual string GetProgramName() { return fProgramName; }
    virtual string GetProgramVersion() { return fProgramVersion; }

    virtual int GetRunNumber() { return fRunNumber; }
    virtual string GetRunTitle() { return fRunTitle; }
    virtual string GetRunNotes() { return fNotes; }
    virtual string GetAnalysisTag() { return fAnalysisTag; }
    virtual string GetLab() { return fLab; }
    virtual string GetBeamline() { return fBeamLine; }
    virtual string GetStartDate() { return fStartDate; }
    virtual string GetStartTime() { return fStartTime; }
    virtual string GetStopDate() { return fStopDate; }
    virtual string GetStopTime() { return fStopTime; }
    virtual int GetSwitchingState() { return fSwitchingState; }
    virtual string GetUser() { return fUser.fUserName; }
    virtual string GetExperimentNumber() { return fUser.fExperimentNumber; }
    virtual string GetSampleName() { return fSample.fName; }
    virtual double GetSampleTemperature() { return fSample.fTemperature; }
    virtual string GetSampleTemperartureUnits() { return fSample.fTemperatureUnit; }
    virtual double GetMagneticField() { return fSample.fMagneticField; }
    virtual string GetMagneticFieldUnits() { return fSample.fMagneticFieldUnit; }
    virtual string GetMagneticFieldState() { return fSample.fMagneticFieldState; }
    virtual int IsMagneticFieldVectorAvailable() { return fSample.fMagneticFieldVectorAvailable; }
    virtual vector<double>& GetMagneticFieldVector() { return fSample.fMagneticFieldVector; }
    virtual string GetMagneticFieldVectorCoordSystem() { return fSample.fMagneticFieldVectorCoordinateSystem; }
    virtual string GetMagneticFieldVectorUnits() { return fSample.fMagneticFieldVectorUnits; }
    virtual string GetSampleEnvironment() { return fSample.fEnvironment; }
    virtual string GetSampleShape() { return fSample.fShape; }
    virtual string GetExperimentName() { return fInstrument.name; }
    virtual int GetNoDetectors() { return fInstrument.detector.number; }
    virtual string GetCollimatorType() { return fInstrument.collimator.type; }
    virtual double GetTotalNoEvents() { return fInstrument.beam.total_counts; }
    virtual string GetTotalNoEventsUnits() { return fInstrument.beam.total_counts_units; }
    virtual double GetTimeResolution() { return fData.fTimeResolution; }
    virtual int GetNoHistos() { return fData.fNumberOfHistos; }
    virtual int IsAlphaAvailable() { return fData.fAlpha.size(); }
    virtual vector<PNeXusAlpha> *GetAlpha() { if (fData.fAlpha.size()) return &fData.fAlpha; else return 0; }
    virtual unsigned int GetT0() { if (fData.fT0.size()) return fData.fT0[0]; else return 0; }
    virtual unsigned int GetFirstGoodBin() { if (fData.fFirstGoodBin.size()) return fData.fFirstGoodBin[0]; else return 0; }
    virtual unsigned int GetLastGoodBin() { if (fData.fLastGoodBin.size()) return fData.fLastGoodBin[0]; else return 0; }
    virtual vector<unsigned int> *GetHisto(unsigned int histoNo) { if (fData.fHisto.size() > histoNo) return &fData.fHisto[histoNo]; else return 0;}
    virtual vector<unsigned int> *GetGroupedHisto(unsigned int histoNo) { if (fGroupedData.size() > histoNo) return &fGroupedData[histoNo]; else return 0; }

    virtual void SetFileName(const char* fileName) { fFileName = fileName; }
    virtual void SetFileName(string fileName) { fFileName = fileName; }
    virtual void SetIDFVersion(int idfVersion) { fIDFVersion = idfVersion; }
    virtual void SetProgramName(const char* progName) { fProgramName = progName; }
    virtual void SetProgramName(string progName) { fProgramName = progName; }
    virtual void SetProgramVersion(int progVersion) { fProgramVersion = progVersion; }
    virtual void SetRunNumber(int runNumber) { fRunNumber = runNumber; }
    virtual void SetRunTitle(const char* runTitle) { fRunTitle = runTitle; }
    virtual void SetRunTitle(string runTitle) { fRunTitle = runTitle; }
    virtual void SetRunNotes(const char* notes) { fNotes = notes; }
    virtual void SetRunNotes(string notes) { fNotes = notes; }
    virtual void SetAnalysisTag(const char* analysisTag) { fAnalysisTag = analysisTag; }
    virtual void SetAnalysisTag(string analysisTag) { fAnalysisTag = analysisTag; }
    virtual void SetLab(const char *lab) { fLab = fLab; }
    virtual void SetLab(string lab) { fLab = fLab; }
    virtual void SetBeamline(const char *beamline) { fBeamLine = beamline; }
    virtual void SetBeamline(string beamline) { fBeamLine = beamline; }
    virtual bool SetStartDate(const char *date);
    virtual bool SetStartDate(string date);
    virtual bool SetStartTime(const char *time);
    virtual bool SetStartTime(string time);
    virtual bool SetStopDate(const char *date);
    virtual bool SetStopDate(string date);
    virtual bool SetStopTime(const char *time);
    virtual bool SetStopTime(string time);
    virtual void SetSwitchingState(int state) { fSwitchingState=state; }
    virtual void SetUser(string user) { fUser.fUserName = user; }
    virtual void SetExperimentNumber(string number) { fUser.fExperimentNumber = number; }
    virtual void SetSampleName(string name) { fSample.fName = name; }
    virtual void SetSampleTemperature(double temp) { fSample.fTemperature = temp; }
    virtual void SetSampleTemperatureUints(string units) { fSample.fTemperatureUnit = units; }
    virtual void SetMagneticField(double field) { fSample.fMagneticField = field; }
    virtual void SetMagneticFieldUnits(string units) { fSample.fMagneticFieldUnit = units; }
    virtual void SetMagneticFieldState(string state) { fSample.fMagneticFieldState = state; }
    virtual void SetSampleEnvironment(string env) { fSample.fEnvironment = env; }
    virtual void SetSampleShape(string shape) { fSample.fShape = shape; }
    virtual void SetMagneticFieldVectorAvailable(int available) { fSample.fMagneticFieldVectorAvailable = available; }
    virtual void SetMagneticFieldVector(vector<double>& vec) {fSample.fMagneticFieldVectorAvailable = 1; fSample.fMagneticFieldVector = vec;}
    virtual void SetMagneticFieldVectorCoordSystem(string coord) { fSample.fMagneticFieldVectorCoordinateSystem = coord; }
    virtual void SetMagneticFieldVectorUnits(string units) { fSample.fMagneticFieldVectorUnits = units; }
    virtual void SetExperimentName(string name) { fInstrument.name = name; }
    virtual void SetNoDetectors(unsigned int no) { fInstrument.detector.number = no; }
    virtual void SetCollimatorType(string type) { fInstrument.collimator.type = type; }
    virtual void SetTimeResolution(double timeResolution) { fData.fTimeResolution = timeResolution; } // given in ns
    virtual void SetT0(unsigned int t0) { fData.fT0.clear(); fData.fT0.push_back(t0); }
    virtual void SetFirstGoodBin(unsigned int fgb) { fData.fFirstGoodBin.clear(); fData.fFirstGoodBin.push_back(fgb); }
    virtual void SetLastGoodBin(unsigned int lgb) { fData.fLastGoodBin.clear(); fData.fLastGoodBin.push_back(lgb); }
    virtual void SetHisto(unsigned int histoNo, vector<unsigned int> &data);

  private:
    bool   fIsValid;
    string fFileName;     ///< read/write file name
    NXhandle fFileHandle; ///< nexus file handle

    string fErrorMsg;     ///< stores the current error message
    int    fErrorCode;    ///< indicates the current error code

    int    fIDFVersion;
    string fProgramName;
    string fProgramVersion;

    int    fRunNumber;
    string fRunTitle;
    string fNotes;
    string fAnalysisTag; ///< type of muon experiment
    string fLab;
    string fBeamLine;
    string fStartDate;
    string fStartTime;
    string fStopDate;
    string fStopTime;
    int    fSwitchingState; ///< red/green mode

    PNeXusUser fUser;
    PNeXusSample fSample;
    PNeXusInstrument fInstrument;
    PNeXusData fData;
    vector< vector<unsigned int> > fGroupedData;

    virtual void Init();
    virtual void CleanUp();
    virtual bool ErrorHandler(NXstatus status, int errCode, string errMsg);
    virtual NXstatus GetStringData(string &str);
    virtual NXstatus GetStringAttr(string attr, string &str);
    virtual int GetDataSize(int type);
    virtual NXstatus GetDoubleVectorData(vector<double> &data);
    virtual NXstatus GetIntVectorData(vector<int> &data);
    virtual NXstatus GroupHistoData();
};

#endif // _PNEXUS_H_
