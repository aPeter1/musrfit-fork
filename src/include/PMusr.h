/***************************************************************************

  PMusr.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2010 by Andreas Suter                              *
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

#ifndef _PMUSR_H_
#define _PMUSR_H_

#include <vector>
#include <map>
using namespace std;

#include <TString.h>

#define PMUSR_SUCCESS                   0
#define PMUSR_WRONG_STARTUP_SYNTAX     -1
#define PMUSR_MSR_FILE_NOT_FOUND       -2
#define PMUSR_MSR_ALLOCATION_ERROR     -3
#define PMUSR_MSR_SYNTAX_ERROR         -4
#define PMUSR_TOKENIZE_ERROR           -5
#define PMUSR_MSR_LOG_FILE_WRITE_ERROR -6
#define PMUSR_MSR_FILE_WRITE_ERROR     -7

#define PRUN_SINGLE_HISTO 0
#define PRUN_ASYMMETRY    2
#define PRUN_MU_MINUS     4
#define PRUN_NON_MUSR     8

// muon life time in (us), see PRL99, 032001 (2007)
#define PMUON_LIFETIME 2.197019

// accelerator cycles in (us), needed to determine proper background
#define ACCEL_PERIOD_PSI     0.01975
#define ACCEL_PERIOD_TRIUMF  0.04337
#define ACCEL_PERIOD_RAL     0.0

// used to filter post pileup correct data histos from root files
#define POST_PILEUP_HISTO_OFFSET 20

// defines a value for 'undefined values'
#define PMUSR_UNDEFINED -9.9e99

//-------------------------------------------------------------
// msr block header tags
#define MSR_TAG_TITLE        0
#define MSR_TAG_FITPARAMETER 1
#define MSR_TAG_THEORY       2
#define MSR_TAG_FUNCTIONS    3
#define MSR_TAG_RUN          4
#define MSR_TAG_COMMANDS     5
#define MSR_TAG_FOURIER      6
#define MSR_TAG_PLOT         7
#define MSR_TAG_STATISTIC    8

//-------------------------------------------------------------
// msr fit type tags
#define MSR_FITTYPE_SINGLE_HISTO 0
#define MSR_FITTYPE_ASYM         2
#define MSR_FITTYPE_MU_MINUS     4
#define MSR_FITTYPE_NON_MUSR     8

//-------------------------------------------------------------
// msr plot type tags
#define MSR_PLOT_SINGLE_HISTO 0
#define MSR_PLOT_ASYM         2
#define MSR_PLOT_MU_MINUS     4
#define MSR_PLOT_NON_MUSR     8

//-------------------------------------------------------------
// map and fun offsets for parameter parsing
#define MSR_PARAM_MAP_OFFSET 10000
#define MSR_PARAM_FUN_OFFSET 20000

//-------------------------------------------------------------
// fourier related tags
#define FOURIER_UNIT_NOT_GIVEN 0
#define FOURIER_UNIT_FIELD     1
#define FOURIER_UNIT_FREQ      2
#define FOURIER_UNIT_CYCLES    3

#define FOURIER_APOD_NOT_GIVEN 0
#define FOURIER_APOD_NONE      1
#define FOURIER_APOD_WEAK      2
#define FOURIER_APOD_MEDIUM    3
#define FOURIER_APOD_STRONG    4

#define FOURIER_PLOT_NOT_GIVEN     0
#define FOURIER_PLOT_REAL          1
#define FOURIER_PLOT_IMAG          2
#define FOURIER_PLOT_REAL_AND_IMAG 3
#define FOURIER_PLOT_POWER         4
#define FOURIER_PLOT_PHASE         5

//-------------------------------------------------------------
// RRF related tags
#define RRF_UNIT_kHz 0
#define RRF_UNIT_MHz 1
#define RRF_UNIT_Mcs 2
#define RRF_UNIT_G   3
#define RRF_UNIT_T   4

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of a bool vector.
 */
typedef vector<Bool_t> PBoolVector;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of an unsigned int vector
 */
typedef vector<UInt_t> PUIntVector;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of an int vector
 */
typedef vector<Int_t> PIntVector;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of an int pair
 */
typedef pair<Int_t, Int_t> PIntPair;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of an int pair vector
 */
typedef vector<PIntPair> PIntPairVector;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of a double vector
 */
typedef vector<Double_t> PDoubleVector;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of a double pair
 */
typedef pair<Double_t, Double_t> PDoublePair;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of a double pair vector
 */
typedef vector<PDoublePair> PDoublePairVector;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of a string vector
 */
typedef vector<TString> PStringVector;

//-------------------------------------------------------------
/**
 * <p>data handling tag
 */
enum EPMusrHandleTag { kEmpty, kFit, kView };

//-------------------------------------------------------------
/**
 * <p>Handles the data which will be fitted, i.e. packed, background corrected, ...
 * This are <b>not</b> the raw histogram data of a run. This are the pre-processed data.
 */
class PRunData {
  public:
    PRunData();
    virtual ~PRunData();

    virtual Double_t GetDataTimeStart() { return fDataTimeStart; }
    virtual Double_t GetDataTimeStep() { return fDataTimeStep; }
    virtual Double_t GetTheoryTimeStart() { return fTheoryTimeStart; }
    virtual Double_t GetTheoryTimeStep() { return fTheoryTimeStep; }

    virtual const PDoubleVector* GetX() { return &fX; }
    virtual const PDoubleVector* GetValue() { return &fValue; }
    virtual const PDoubleVector* GetError() { return &fError; }
    virtual const PDoubleVector* GetXTheory() { return &fXTheory; }
    virtual const PDoubleVector* GetTheory() { return &fTheory; }

    virtual void SetDataTimeStart(Double_t dval) { fDataTimeStart = dval; }
    virtual void SetDataTimeStep(Double_t dval) { fDataTimeStep = dval; }
    virtual void SetTheoryTimeStart(Double_t dval) { fTheoryTimeStart = dval; }
    virtual void SetTheoryTimeStep(Double_t dval) { fTheoryTimeStep = dval; }

    virtual void AppendXValue(Double_t dval) { fX.push_back(dval); }
    virtual void AppendValue(Double_t dval) { fValue.push_back(dval); }
    virtual void AppendErrorValue(Double_t dval) { fError.push_back(dval); }
    virtual void AppendXTheoryValue(Double_t dval) { fXTheory.push_back(dval); }
    virtual void AppendTheoryValue(Double_t dval) { fTheory.push_back(dval); }

    virtual void SetTheoryValue(UInt_t i, Double_t dval);
    virtual void ReplaceTheory(const PDoubleVector &theo);

  private:
    // data related info
    Double_t fDataTimeStart; ///< start time for the data set
    Double_t fDataTimeStep;  ///< time step for the data set, i.e. the time length of a bin
    PDoubleVector fX;        ///< x-axis vector. Only used for non-muSR
    PDoubleVector fValue;    ///< data vector
    PDoubleVector fError;    ///< data error vector
    // theory related info
    Double_t fTheoryTimeStart; ///< start time of the theory
    Double_t fTheoryTimeStep;  ///< time step of the theory, i.e. the time length of a bin
    PDoubleVector fXTheory;    ///< x-axis vector. Only used for non-muSR
    PDoubleVector fTheory;     ///< theory vector
  };

//-------------------------------------------------------------
/**
 * <p>Handles Non-Musr raw data.
 */
class PNonMusrRawRunData {
  public:
    PNonMusrRawRunData();
    virtual ~PNonMusrRawRunData();

    virtual Bool_t FromAscii() { return fFromAscii; }
    virtual const PStringVector* GetLabels() { return &fLabels; }
    virtual const PStringVector* GetDataTags() { return &fDataTags; }
    virtual const vector<PDoubleVector>* GetData() { return &fData; }
    virtual const vector<PDoubleVector>* GetErrData() { return &fErrData; }

    virtual void SetFromAscii(const Bool_t bval) { fFromAscii = bval; }
    virtual void AppendLabel(const TString str) { fLabels.push_back(str); }
    virtual void SetLabel(const UInt_t idx, const TString str);
    virtual void AppendDataTag(const TString str) { fDataTags.push_back(str); }
    virtual void AppendData(const PDoubleVector &data) { fData.push_back(data); }
    virtual void AppendErrData(const PDoubleVector &data) { fErrData.push_back(data); }
    virtual void AppendSubData(const UInt_t idx, const Double_t dval);
    virtual void AppendSubErrData(const UInt_t idx, const Double_t dval);

  private:
    Bool_t fFromAscii;              ///< if true: data file was an ascii input file, otherwise it is a db input file
    PStringVector fLabels;          ///< vector of all labels (used for x-, y-axis title in view)
    PStringVector fDataTags;        ///< vector of all data tags
    vector<PDoubleVector> fData;    ///< vector of all data
    vector<PDoubleVector> fErrData; ///< vector of all data errors
};

//-------------------------------------------------------------
/**
 * <p>Handles raw data, both non-muSR data as well muSR histogram data.
 */
class PRawRunData {
  public:
    PRawRunData();
    virtual ~PRawRunData();

    virtual const TString* GetRunName() { return &fRunName; }
    virtual const Int_t GetRunNumber() { return fRunNumber; }
    virtual const TString* GetRunTitle() { return &fRunTitle; }
    virtual const TString* GetSetup() { return &fSetup; }
    virtual const TString* GetStartTime() { return &fStartTime; }
    virtual const TString* GetStartDate() { return &fStartDate; }
    virtual const time_t GetStartDateTime() { return fStartDateTimeSec; }
    virtual const TString* GetStopTime() { return &fStopTime; }
    virtual const TString* GetStopDate() { return &fStopDate; }
    virtual const time_t GetStopDateTime() { return fStopDateTimeSec; }
    virtual const TString* GetSample() { return &fSample; }
    virtual const TString* GetOrientation() { return &fOrientation; }
    virtual const Double_t GetField() { return fField; }
    virtual const UInt_t GetNoOfTemperatures() { return fTemp.size(); }
    virtual const PDoublePairVector* GetTemperature() const { return &fTemp; }
    virtual const Double_t GetTemperature(const UInt_t idx);
    virtual const Double_t GetTempError(const UInt_t idx);
    virtual const Double_t GetEnergy() { return fEnergy; }
    virtual const Double_t GetTransport() { return fTransport; }
    virtual const PDoubleVector GetRingAnode() { return fRingAnode; }
    virtual const Double_t GetRingAnode(const UInt_t idx);
    virtual const Double_t GetTimeResolution() { return fTimeResolution; }
    virtual const UInt_t GetT0Size() { return fT0s.size(); }
    virtual const Int_t GetT0(const UInt_t idx);
    virtual const UInt_t GetT0EstimatedSize() { return fT0Estimated.size(); }
    virtual const Int_t GetT0Estimated(const UInt_t idx);
    virtual const PIntPair GetBkgBin(const UInt_t idx);
    virtual const PIntPair GetGoodDataBin(const UInt_t idx);
    virtual const UInt_t GetNoOfHistos() { return fDataBin.size(); }
    virtual const PDoubleVector* GetDataBin(const UInt_t idx);
    virtual const PNonMusrRawRunData* GetDataNonMusr() { return &fDataNonMusr; }

    virtual void SetRunName(const TString &str) { fRunName = str; }
    virtual void SetRunNumber(const Int_t &val) { fRunNumber = val; }
    virtual void SetRunTitle(const TString str) { fRunTitle = str; }
    virtual void SetSetup(const TString str) { fSetup = str; }
    virtual void SetStartTime(const TString str) { fStartTime = str; }
    virtual void SetStartDate(const TString str) { fStartDate = str; }
    virtual void SetStartDateTime(const time_t val) { fStartDateTimeSec = val; }
    virtual void SetStopTime(const TString str) { fStopTime = str; }
    virtual void SetStopDate(const TString str) { fStopDate = str; }
    virtual void SetStopDateTime(const time_t val) { fStopDateTimeSec = val; }
    virtual void SetField(const Double_t dval) { fField = dval; }
    virtual void SetSample(const TString str) { fSample = str; }
    virtual void SetOrientation(const TString str) { fOrientation = str; }
    virtual void ClearTemperature() { fTemp.clear(); }
    virtual void SetTemperature(const UInt_t idx, const Double_t temp, const Double_t errTemp);
    virtual void SetTempError(const UInt_t idx, const Double_t errTemp);
    virtual void SetEnergy(const Double_t dval) { fEnergy = dval; }
    virtual void SetTransport(const Double_t dval) { fTransport = dval; }
    virtual void SetRingAnode(const UInt_t idx, const Double_t dval);
    virtual void SetTimeResolution(const Double_t dval) { fTimeResolution = dval; }
    virtual void AppendT0(const Int_t ival) { fT0s.push_back(ival); }
    virtual void AppendT0Estimated(const Int_t ival) { fT0Estimated.push_back(ival); }
    virtual void AppendBkgBin(PIntPair pair) { fBkgBin.push_back(pair); }
    virtual void AppendGoodDataBin(PIntPair pair) { fGoodDataBin.push_back(pair); }
    virtual void AppendDataBin(PDoubleVector data) { fDataBin.push_back(data); }
    virtual void SetDataBin(const UInt_t histoNo, const UInt_t bin, const Double_t dval);
    virtual void AddDataBin(const UInt_t histoNo, const UInt_t bin, const Double_t dval);

    PNonMusrRawRunData fDataNonMusr; ///< keeps all ascii- or db-file info in case of nonMusr fit

  private:
    TString fRunName;                ///< name of the run
    Int_t  fRunNumber;               ///< run number
    TString fRunTitle;               ///< run title
    TString fSetup;                  ///< description of the setup of this run
    TString fStartTime;              ///< start time of the run
    TString fStartDate;              ///< start date of the run
    time_t fStartDateTimeSec;        ///< start run given as time_t object
    TString fStopTime;               ///< stop time of the run
    TString fStopDate;               ///< stop date of the run
    time_t fStopDateTimeSec;         ///< stop run given as time_t object
    TString fSample;                 ///< description of the sample
    TString fOrientation;            ///< description of the orientation
    Double_t fField;                 ///< magnetic field value
    PDoublePairVector fTemp;         ///< measured temperatures and standard deviations during the run
    Double_t fEnergy;                ///< implantation energy of the muon
    Double_t fTransport;             ///< LEM transport settings (Moderator HV)
    PDoubleVector fRingAnode;        ///< LEM ring anode HVs (L,R[,T,B])
    Double_t fTimeResolution;        ///< time resolution of the run
    PIntVector fT0s;                 ///< vector of t0's of a run
    PIntVector fT0Estimated;         ///< vector of t0's estimated based on the maximum of the histogram
    PIntPairVector fBkgBin;          ///< background bins (first/last)
    PIntPairVector fGoodDataBin;     ///< data bins (first/last)
    vector<PDoubleVector> fDataBin;  ///< vector of all histos of a run
};

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. A vector of a raw musr run.
 */
typedef vector<PRawRunData> PRawRunDataList;

//-------------------------------------------------------------
/**
 * <p>Helper structure for parsing. Keeps a msr-file line string and the corresponding line number.
 */
typedef struct {
  Int_t   fLineNo; ///< original line number of the msr-file
  TString fLine;   ///< msr-file line
} PMsrLineStructure;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable: list of msr-file lines.
 */
typedef vector<PMsrLineStructure> PMsrLines;

//-------------------------------------------------------------
/**
 * <p>Handles the information of a parameter.
 */
typedef struct {
  Int_t fNoOfParams;              ///< how many parameters are given
  Int_t fNo;                      ///< parameter number
  TString fName;                  ///< name
  Double_t fValue;                ///< value
  Double_t fStep;                 ///< step / error / neg_error, depending on the situation
  Bool_t   fPosErrorPresent;      ///< positive error is defined (as a number)
  Double_t fPosError;             ///< positive error if present
  Bool_t   fLowerBoundaryPresent; ///< flag showing if a lower boundary is present
  Double_t fLowerBoundary;        ///< lower boundary for the fit parameter
  Bool_t   fUpperBoundaryPresent; ///< flag showing if an upper boundary is present
  Double_t fUpperBoundary;        ///< upper boundary for the fit parameter
  Bool_t   fIsGlobal;             ///< flag showing if the parameter is a global one (used for msr2data global)
} PMsrParamStructure;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable: vector of fit parameters.
 */
typedef vector<PMsrParamStructure> PMsrParamList;

//-------------------------------------------------------------
/**
 * <p>Handles the information of a single run block
 *
 */
class PMsrRunBlock {
  public:
    PMsrRunBlock();
    virtual ~PMsrRunBlock();
    virtual void CleanUp();

    virtual UInt_t GetRunNameSize() { return fRunName.size(); }
    virtual TString *GetRunName(UInt_t idx=0);
    virtual UInt_t GetBeamlineSize() { return fBeamline.size(); }
    virtual TString *GetBeamline(UInt_t idx=0);
    virtual UInt_t GetInstituteSize() { return fInstitute.size(); }
    virtual TString *GetInstitute(UInt_t idx=0);
    virtual UInt_t GetFileFormatSize() { return fFileFormat.size(); }
    virtual TString *GetFileFormat(UInt_t idx=0);
    virtual Int_t GetFitType() { return fFitType; }
    virtual Int_t GetAlphaParamNo() { return fAlphaParamNo; }
    virtual Int_t GetBetaParamNo() { return fBetaParamNo; }
    virtual Int_t GetNormParamNo() { return fNormParamNo; }
    virtual Int_t GetBkgFitParamNo() { return fBkgFitParamNo; }
    virtual Int_t GetLifetimeParamNo() { return fLifetimeParamNo; }
    virtual Bool_t IsLifetimeCorrected() { return fLifetimeCorrection; }
    virtual PIntVector* GetMap() { return &fMap; }
    virtual Int_t GetMap(UInt_t idx);
    virtual UInt_t GetForwardHistoNoSize() { return fForwardHistoNo.size(); }
    virtual Int_t GetForwardHistoNo(UInt_t idx=0);
    virtual UInt_t GetBackwardHistoNoSize() { return fBackwardHistoNo.size(); }
    virtual Int_t GetBackwardHistoNo(UInt_t idx=0);
    virtual Double_t GetBkgEstimated(UInt_t idx);
    virtual Double_t GetBkgFix(UInt_t idx);
    virtual Int_t GetBkgRange(UInt_t idx);
    virtual Int_t GetDataRange(UInt_t idx);
    virtual UInt_t GetT0Size() { return fT0.size(); }
    virtual Int_t GetT0(UInt_t idx=0);
    virtual UInt_t GetAddT0Entries() { return fAddT0.size(); }
    virtual Int_t GetAddT0Size(UInt_t addRunIdx);
    virtual Int_t GetAddT0(UInt_t addRunIdx, UInt_t histoIdx);
    virtual Double_t GetFitRange(UInt_t idx);
    virtual Int_t GetPacking() { return fPacking; }
    virtual Int_t GetXDataIndex() { return fXYDataIndex[0]; }
    virtual Int_t GetYDataIndex() { return fXYDataIndex[1]; }
    virtual TString* GetXDataLabel() { return &fXYDataLabel[0]; }
    virtual TString* GetYDataLabel() { return &fXYDataLabel[1]; }
    virtual map<TString, Int_t> *GetParGlobal() { return &fParGlobal; }
    virtual PIntVector *GetMapGlobal() { return &fMapGlobal; }

    virtual void SetRunName(TString &str, Int_t idx=-1);
    virtual void SetBeamline(TString &str, Int_t idx=-1);
    virtual void SetInstitute(TString &str, Int_t idx=-1);
    virtual void SetFileFormat(TString &str, Int_t idx=-1);
    virtual void SetFitType(Int_t ival) { fFitType = ival; }
    virtual void SetAlphaParamNo(Int_t ival) { fAlphaParamNo = ival; }
    virtual void SetBetaParamNo(Int_t ival) { fBetaParamNo = ival; }
    virtual void SetNormParamNo(Int_t ival) { fNormParamNo = ival; }
    virtual void SetBkgFitParamNo(Int_t ival) { fBkgFitParamNo = ival; }
    virtual void SetLifetimeParamNo(Int_t ival) { fLifetimeParamNo = ival; }
    virtual void SetLifetimeCorrection(Bool_t bval) { fLifetimeCorrection = bval; }
    virtual void SetMap(Int_t mapVal, Int_t idx=-1);
    virtual void SetForwardHistoNo(Int_t histoNo, Int_t idx=-1);
    virtual void SetBackwardHistoNo(Int_t histoNo, Int_t idx=-1);
    virtual void SetBkgEstimated(Double_t dval, Int_t idx);
    virtual void SetBkgFix(Double_t dval, Int_t idx);
    virtual void SetBkgRange(Int_t ival, Int_t idx);
    virtual void SetDataRange(Int_t ival, Int_t idx);
    virtual void SetT0(Int_t ival, Int_t idx=-1);
    virtual void SetAddT0(Int_t ival, UInt_t addRunIdx, UInt_t histoNoIdx);
    virtual void SetFitRange(Double_t dval, UInt_t idx);
    virtual void SetPacking(Int_t ival) { fPacking = ival; }
    virtual void SetXDataIndex(Int_t ival) { fXYDataIndex[0] = ival; }
    virtual void SetYDataIndex(Int_t ival) { fXYDataIndex[1] = ival; }
    virtual void SetXDataLabel(TString& str) { fXYDataLabel[0] = str; }
    virtual void SetYDataLabel(TString& str) { fXYDataLabel[1] = str; }
    virtual void SetParGlobal(const TString &str, Int_t ival);
    virtual void SetMapGlobal(UInt_t idx, Int_t ival);

  private:
    PStringVector fRunName;         ///< name of the run file
    PStringVector fBeamline;        ///< e.g. mue4, mue1, pim3, emu, m15, ... (former: run type)
    PStringVector fInstitute;       ///< e.g. psi, ral, triumf (former: run format)
    PStringVector fFileFormat;      ///< e.g. root, nexus, psi-bin, mud, ascii, db
    Int_t fFitType;                 ///< fit type: 0=single histo fit, 2=asymmetry fit, 4=mu^- single histo fit, 8=non muSR fit
    Int_t fAlphaParamNo;            ///< alpha parameter number (fit type 2, 4)
    Int_t fBetaParamNo;             ///< beta parameter number  (fit type 2, 4)
    Int_t fNormParamNo;             ///< N0 parameter number (fit type 0)
    Int_t fBkgFitParamNo;           ///< background fit parameter number (fit type 0)
    Int_t fLifetimeParamNo;         ///< muon lifetime parameter number (fit type 0)
    Bool_t fLifetimeCorrection;     ///< lifetime correction flag for viewing (fit type 0)
    PIntVector fMap;                ///< map vector needed to switch parameters for different runs within a single theory
    PIntVector fForwardHistoNo;     ///< forward histogram number (fit type 0, 2, 4)
    PIntVector fBackwardHistoNo;    ///< backward histogram number (fit type 2, 4)
    Double_t fBkgEstimated[2];      ///< keeps estimated background values (if present)
    Double_t fBkgFix[2];            ///< fixed background in (1/ns) (fit type 0, 2, 4)
    Int_t fBkgRange[4];             ///< background bin range (fit type 0, 2, 4)
    Int_t fDataRange[4];            ///< data bin range (fit type 0, 2, 4)
    PIntVector fT0;                 ///< t0 bins (fit type 0, 2, 4). if fit type 0 -> f0, f1, f2, ...; if fit type 2, 4 -> f0, b0, f1, b1, ...
    vector<PIntVector> fAddT0;      ///< t0 bins for addrun's
    Double_t fFitRange[2];          ///< fit range in (us)
    Int_t fPacking;                 ///< packing/rebinning
    Int_t fXYDataIndex[2];          ///< used to get the data indices when using db-files (fit type 8)
    TString fXYDataLabel[2];        ///< used to get the indices via labels when using db-files  (fit type 8)

    // Two members used for msr2data in the global mode: fParGlobal and fMapGlobal
    // These are intended to track global and run specific parameters used in the RUN blocks
    // Suggested keys for the std::map: (alpha, beta, norm, bkgfit, lifetime)
    // Suggested values for the std::map: 1 -> parameter is global
    //                                    0 -> parameter is run specific
    //                                    -1 -> tag not present in the RUN block
    // The information about global parameters in the map line is stored in an std::vector which should have the same length as the map-vector
    // The values in this std::vector can be the same as for the std::map of the other parameters.
    map<TString, Int_t> fParGlobal; ///< here is stored if the parameters used in the RUN block are global or not
    PIntVector fMapGlobal;          ///< here is stored if the maps used in the RUN block are global or not
};

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable: list of runs with its parameters.
 */
typedef vector<PMsrRunBlock> PMsrRunList;

//-------------------------------------------------------------
/**
 * <p>Holds the information of the Fourier block
 */
typedef struct {
  Bool_t fFourierBlockPresent; ///< flag indicating if a Fourier block is present in the msr-file
  Int_t  fUnits;               ///< flag used to indicate the units. 0=field units (G); 1=frequency units (MHz); 2=Mc/s
  Int_t  fFourierPower;        ///< i.e. zero padding up to 2^fFourierPower, default = 0 which means NO zero padding
  Int_t  fApodization;         ///< tag indicating the kind of apodization wished, 0=no appodization (default), 1=weak, 2=medium, 3=strong (for details see the docu)
  Int_t  fPlotTag;             ///< tag used for initial plot. 0=real, 1=imaginary, 2=real & imaginary (default), 3=power, 4=phase
  Int_t  fPhaseParamNo;        ///< parameter number if used instead of a phase value
  Double_t fPhase;             ///< phase
  Double_t fRangeForPhaseCorrection[2]; ///< field/frequency range for automatic phase correction
  Double_t fPlotRange[2];      ///< field/frequency plot range
  Double_t fPhaseIncrement;    ///< phase increment for manual phase optimization
} PMsrFourierStructure;

//-------------------------------------------------------------
/**
 * <p>Holds the information of a single plot block
 */
typedef struct {
  Int_t    fPlotType;      ///< plot type
  Bool_t   fUseFitRanges;  ///< yes -> use the fit ranges to plot the data, no (default) -> use range information if present
  Bool_t   fLogX;          ///< yes -> x-axis in log-scale, no (default) -> x-axis in lin-scale
  Bool_t   fLogY;          ///< yes -> y-axis in log-scale, no (default) -> y-axis in lin-scale
  Int_t    fViewPacking;   ///< -1 -> use the run packing to generate the view, otherwise is fViewPacking for the binning of ALL runs.
  PIntVector fRuns;        ///< list of runs to be plotted
  PDoubleVector  fTmin;    ///< time minimum
  PDoubleVector  fTmax;    ///< time maximum
  PDoubleVector  fYmin;    ///< asymmetry/counts minimum
  PDoubleVector  fYmax;    ///< asymmetry/counts maximum
  UInt_t fRRFPacking;      ///< rotating reference frame (RRF) packing
  Double_t fRRFFreq;       ///< RRF frequency
  UInt_t fRRFUnit;         ///< RRF frequency unit. 0=kHz, 1=MHz, 2=Mc/s, 3=Gauss, 4=Tesla
  Int_t  fRRFPhaseParamNo; ///< parameter number if used instead of a RRF phase value
  Double_t fRRFPhase;      ///< RRF phase
} PMsrPlotStructure;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable: list of plots.
 */
typedef vector<PMsrPlotStructure>  PMsrPlotList;

//-------------------------------------------------------------
/**
 * <p>Holds the informations for the statistics block.
 */
typedef struct {
  Bool_t fValid;     ///< flag showing if the statistics block is valid, i.e. a fit took place which converged
  PMsrLines fStatLines; ///< statistics block in msr-file clear text
  TString fDate;     ///< string holding fitting date and time
  Bool_t   fChisq;   ///< flag telling if min = chi2 or min = max.likelihood
  Double_t fMin;     ///< chisq or max. likelihood
  PDoubleVector fMinPerHisto; ///< chisq or max. likelihood per histo
  UInt_t fNdf;       ///< number of degrees of freedom
  Double_t fMinExpected; ///< expected total chi2 or max. likelihood
  PDoubleVector fMinExpectedPerHisto; ///< expected pre histo chi2 or max. likelihood
  PUIntVector fNdfPerHisto; ///< number of degrees of freedom per histo
} PMsrStatisticStructure;

//-------------------------------------------------------------
/**
 * <p>Holds the informations for the any2many converter program
 */
typedef struct {
  Bool_t useStandardOutput;  ///< flag showing if the converted shall be sent to the standard output
  TString inFormat;          ///< holds the information about the input data file format
  TString outFormat;         ///< holds the information about the output data file format
  TString inTemplate;        ///< holds the input file template
  TString outTemplate;       ///< holds the output file template
  TString year;              ///< holds the information about the year to be used
  PIntVector runList;        ///< holds the run number list to be converted
  PStringVector inFileName;  ///< holds the file name of the input data file
  PStringVector outPathFileName; ///< holds the out path/file name
  TString outPath;           ///< holds the output path
  UInt_t rebin;              ///< holds the number of bins to be packed
  UInt_t compressionTag;     ///< 0=no compression, 1=gzip compression, 2=bzip2 compression
  TString compressFileName;  ///< holds the name of the outputfile name in case of compression is used
  UInt_t idf;                ///< IDF version for NeXus files.
} PAny2ManyInfo;

#endif // _PMUSR_H_
