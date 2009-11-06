/***************************************************************************

  PMusr.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
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
using namespace std;

#include <TComplex.h>
#include <TString.h>

#define PMUSR_SUCCESS                   0
#define PMUSR_WRONG_STARTUP_SYNTAX     -1
#define PMUSR_MSR_FILE_NOT_FOUND       -2
#define PMUSR_MSR_ALLOCATION_ERROR     -3
#define PMUSR_MSR_SYNTAX_ERROR         -4
#define PMUSR_TOKENIZE_ERROR           -5
#define PMUSR_MSR_LOG_FILE_WRITE_ERROR -6

#define PRUN_SINGLE_HISTO 0
#define PRUN_ASYMMETRY    2
#define PRUN_RRF          4
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
#define MSR_FITTYPE_ASYM_RRF     4
#define MSR_FITTYPE_NON_MUSR     8

//-------------------------------------------------------------
// msr plot type tags
#define MSR_PLOT_SINGLE_HISTO 0
#define MSR_PLOT_ASYM         2
#define MSR_PLOT_ASYM_RRF     4
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
/**
 * <p> typedef to make to code more readable.
 */
typedef vector<Int_t> PIntVector;

//-------------------------------------------------------------
/**
 * <p>
 */
typedef pair<Int_t, Int_t> PIntPair;

//-------------------------------------------------------------
/**
 * <p>
 */
typedef vector<PIntPair> PIntPairVector;

//-------------------------------------------------------------
/**
 * <p>
 */
typedef vector<Double_t> PDoubleVector;

//-------------------------------------------------------------
/**
 * <p>
 */
typedef vector< pair<Double_t, Double_t> > PDoublePairVector;

//-------------------------------------------------------------
/**
 * <p> typedef to make to code more readable.
 */
typedef vector<TComplex> PComplexVector;

//-------------------------------------------------------------
/**
 * <p> typedef to make to code more readable.
 */
typedef vector<TString> PStringVector;

//-------------------------------------------------------------
/**
 * <p> data handling tag
 */
enum EPMusrHandleTag { kEmpty, kFit, kView };

//-------------------------------------------------------------
/**
 * <p>Holds the data which will be fitted, i.e. packed, background corrected, ...
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

  private:
    // data related info
    Double_t fDataTimeStart;
    Double_t fDataTimeStep;
    PDoubleVector fX;       // only used for non-muSR
    PDoubleVector fValue;
    PDoubleVector fError;
    // theory related info
    Double_t fTheoryTimeStart;
    Double_t fTheoryTimeStep;
    PDoubleVector fXTheory; // only used for non-muSR
    PDoubleVector fTheory;
  };

//-------------------------------------------------------------
/**
 * <p>Non-Musr raw data.
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
 * <p>Histogram raw muSR data.
 */
class PRawRunData {
  public:
    PRawRunData();
    virtual ~PRawRunData();

    virtual const TString* GetRunName() { return &fRunName; }
    virtual const TString* GetRunTitle() { return &fRunTitle; }
    virtual const TString* GetSetup() { return &fSetup; }
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
    virtual void SetRunTitle(const TString str) { fRunTitle = str; }
    virtual void SetSetup(const TString str) { fSetup = str; }
    virtual void SetField(const Double_t dval) { fField = dval; }
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
    TString fRunTitle;               ///< run title
    TString fSetup;                  ///< description of the setup of this run
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
 * <p>
 */
typedef vector<PRawRunData> PRawRunDataList;

//-------------------------------------------------------------
/**
 * <p> Helper structure for parsing. Keeps a msr-file line string and the corresponding line number.
 */
typedef struct {
  Int_t   fLineNo; ///< original line number of the msr-file
  TString fLine;   ///< msr-file line
} PMsrLineStructure;

//-------------------------------------------------------------
/**
 * <p>  typedef to make to code more readable: list of msr-file lines.
 */
typedef vector<PMsrLineStructure> PMsrLines;

//-------------------------------------------------------------
/**
 * <p> Holds the information of a parameter.
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
} PMsrParamStructure;

//-------------------------------------------------------------
/**
 * <p>  typedef to make to code more readable: vector of fit parameters.
 */
typedef vector<PMsrParamStructure> PMsrParamList;

//-------------------------------------------------------------
/**
 * <p> Holds the information of a single run block
 *
 */
class PMsrRunBlock {
  public:
    PMsrRunBlock();
    virtual ~PMsrRunBlock();
    virtual void CleanUp();

    virtual UInt_t GetRunNameSize() { return fRunName.size(); }
    virtual TString *GetRunName(UInt_t i=0);
    virtual UInt_t GetBeamlineSize() { return fBeamline.size(); }
    virtual TString *GetBeamline(UInt_t i=0);
    virtual UInt_t GetInstituteSize() { return fInstitute.size(); }
    virtual TString *GetInstitute(UInt_t i=0);
    virtual UInt_t GetFileFormatSize() { return fFileFormat.size(); }
    virtual TString *GetFileFormat(UInt_t i=0);
    virtual Int_t GetFitType() { return fFitType; }
    virtual Int_t GetAlphaParamNo() { return fAlphaParamNo; }
    virtual Int_t GetBetaParamNo() { return fBetaParamNo; }
    virtual Int_t GetNormParamNo() { return fNormParamNo; }
    virtual Int_t GetBkgFitParamNo() { return fBkgFitParamNo; }
    virtual Int_t GetPhaseParamNo() { return fPhaseParamNo; }
    virtual Int_t GetLifetimeParamNo() { return fLifetimeParamNo; }
    virtual Bool_t IsLifetimeCorrected() { return fLifetimeCorrection; }
    virtual PIntVector* GetMap() { return &fMap; }
    virtual Int_t GetMap(UInt_t idx);
    virtual Int_t GetForwardHistoNo() { return fForwardHistoNo; }
    virtual Int_t GetBackwardHistoNo() { return fBackwardHistoNo; }
    virtual UInt_t GetBkgFixSize() { return fBkgFix.size(); }
    virtual Double_t GetBkgFix(UInt_t i=0);
    virtual UInt_t GetBkgRangeSize() { return fBkgRange.size(); }
    virtual Int_t GetBkgRange(UInt_t i=0);
    virtual UInt_t GetDataRangeSize() { return fDataRange.size(); }
    virtual Int_t GetDataRange(UInt_t i=0);
    virtual UInt_t GetT0Size() { return fT0.size(); }
    virtual Int_t GetT0(UInt_t i=0);
    virtual Double_t GetFitRange(UInt_t i);
    virtual Int_t GetPacking() { return fPacking; }
    virtual Double_t GetRRFFreq() { return fRRFFreq; }
    virtual Int_t GetRRFPacking() { return fRRFPacking; }
    virtual Int_t GetAlpha2ParamNo() { return fAlpha2ParamNo; }
    virtual Int_t GetBeta2ParamNo() { return fBeta2ParamNo; }
    virtual Int_t GetRightHistoNo() { return fRightHistoNo; }
    virtual Int_t GetLeftHistoNo() { return fLeftHistoNo; }
    virtual Int_t GetXDataIndex() { return fXYDataIndex[0]; }
    virtual Int_t GetYDataIndex() { return fXYDataIndex[1]; }
    virtual TString* GetXDataLabel() { return &fXYDataLabel[0]; }
    virtual TString* GetYDataLabel() { return &fXYDataLabel[1]; }

    virtual void AppendRunName(TString str) { fRunName.push_back(str); }
    virtual void SetRunName(TString &str, UInt_t i);
    virtual void AppendBeamline(TString str) { fBeamline.push_back(str); }
    virtual void SetBeamline(TString &str, UInt_t i);
    virtual void AppendInstitute(TString str) { fInstitute.push_back(str); }
    virtual void SetInstitute(TString &str, UInt_t i);
    virtual void AppendFileFormat(TString str) { fFileFormat.push_back(str); }
    virtual void SetFileFormat(TString &str, UInt_t i);
    virtual void SetFitType(Int_t ival) { fFitType = ival; }
    virtual void SetAlphaParamNo(Int_t ival) { fAlphaParamNo = ival; }
    virtual void SetBetaParamNo(Int_t ival) { fBetaParamNo = ival; }
    virtual void SetNormParamNo(Int_t ival) { fNormParamNo = ival; }
    virtual void SetBkgFitParamNo(Int_t ival) { fBkgFitParamNo = ival; }
    virtual void SetPhaseParamNo(Int_t ival) { fPhaseParamNo = ival; }
    virtual void SetLifetimeParamNo(Int_t ival) { fLifetimeParamNo = ival; }
    virtual void SetLifetimeCorrection(Bool_t bval) { fLifetimeCorrection = bval; }
    virtual void AppendMap(Int_t ival) { fMap.push_back(ival); }
    virtual void SetMap(Int_t mapVal, UInt_t idx);
    virtual void SetForwardHistoNo(Int_t ival) { fForwardHistoNo = ival; }
    virtual void SetBackwardHistoNo(Int_t ival) { fBackwardHistoNo = ival; }
    virtual void AppendBkgFix(Double_t dval) { fBkgFix.push_back(dval); }
    virtual void SetBkgFix(Double_t dval, UInt_t idx);
    virtual void AppendBkgRange(Int_t ival) { fBkgRange.push_back(ival); }
    virtual void SetBkgRange(Int_t ival, UInt_t idx);
    virtual void AppendDataRange(Int_t ival) { fDataRange.push_back(ival); }
    virtual void SetDataRange(Int_t ival, UInt_t idx);
    virtual void AppendT0(Int_t ival) { fT0.push_back(ival); }
    virtual void SetT0(Int_t ival, UInt_t idx);
    virtual void SetFitRange(Double_t dval, UInt_t idx);
    virtual void SetPacking(Int_t ival) { fPacking = ival; }
    virtual void SetRRFFreq(Double_t dval) { fRRFFreq = dval; }
    virtual void SetRRFPacking(Int_t ival) { fRRFPacking = ival; }
    virtual void SetAlpha2ParamNo(Int_t ival) { fAlpha2ParamNo = ival; }
    virtual void SetBeta2ParamNo(Int_t ival) { fBeta2ParamNo = ival; }
    virtual void SetRightHistoNo(Int_t ival) { fRightHistoNo = ival; }
    virtual void SetLeftHistoNo(Int_t ival) { fLeftHistoNo = ival; }
    virtual void SetXDataIndex(Int_t ival) { fXYDataIndex[0] = ival; }
    virtual void SetYDataIndex(Int_t ival) { fXYDataIndex[1] = ival; }
    virtual void SetXDataLabel(TString& str) { fXYDataLabel[0] = str; }
    virtual void SetYDataLabel(TString& str) { fXYDataLabel[1] = str; }

  private:
    PStringVector fRunName;       ///< name of the run file
    PStringVector fBeamline;      ///< e.g. mue4, mue1, pim3, emu, m15, ... (former: run type)
    PStringVector fInstitute;     ///< e.g. psi, ral, triumf (former: run format)
    PStringVector fFileFormat;    ///< e.g. root, nexus, psi-bin, mud, ascii, db
    Int_t fFitType;               ///< fit type: 0=single histo fit, 2=asymmetry fit, 4=asymmetry in RRF, 8=non muSR
    Int_t fAlphaParamNo;          ///< alpha parameter number (fit type 2, 4)
    Int_t fBetaParamNo;           ///< beta parameter number  (fit type 2, 4)
    Int_t fNormParamNo;           ///< N0 parameter number (fit type 0)
    Int_t fBkgFitParamNo;         ///< background fit parameter number (fit type 0)
    Int_t fPhaseParamNo;          ///< ??? NEEDED ??? NEEDS TO BE CHECKED !!!
    Int_t fLifetimeParamNo;       ///< muon lifetime parameter number (fit type 0)
    Bool_t fLifetimeCorrection;   ///< lifetime correction flag for viewing (fit type 0)
    PIntVector fMap;              ///< map vector needed to switch parameters for different runs within a single theory
    Int_t fForwardHistoNo;        ///< forward histogram number (fit type 0, 2, 4)
    Int_t fBackwardHistoNo;       ///< backward histogram number (fit type 2, 4)
    PDoubleVector fBkgFix;        ///< fixed background in (1/ns) (fit type 0, 2, 4)
    PIntVector fBkgRange;         ///< background bin range (fit type 0, 2, 4)
    PIntVector fDataRange;        ///< data bin range (fit type 0, 2, 4)
    PIntVector fT0;               ///< t0 bins (fit type 0, 2, 4). if fit type 0 -> f0, f1, f2, ...; if fit type 2, 4 -> f0, b0, f1, b1, ...
    Double_t fFitRange[2];        ///< fit range in (us)
    Int_t fPacking;               ///< packing/rebinning
    Double_t fRRFFreq;            ///< rotating reference frequency (fit type 4)
    Int_t fRRFPacking;            ///< rotating reference packing (fit type 4)
    Int_t fAlpha2ParamNo;         ///< rotating reference alpha2 (fit type 4)
    Int_t fBeta2ParamNo;          ///< rotating reference beta2 (fit type 4)
    Int_t fRightHistoNo;          ///< rotating reference right histogram number (fit type 4)
    Int_t fLeftHistoNo;           ///< rotating reference left histogram number (fit type 4)
    Int_t fXYDataIndex[2];        ///< used to get the data indices when using db-files (fit type 8)
    TString fXYDataLabel[2];      ///< used to get the indices via labels when using db-files  (fit type 8)
};

//-------------------------------------------------------------
/**
 * <p>  typedef to make to code more readable: list of runs with its parameters.
 */
typedef vector<PMsrRunBlock> PMsrRunList;

//-------------------------------------------------------------
/**
 * <p> Holds the information of the Fourier block
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
 * <p> Holds the information of a single plot block
 */
typedef struct {
  Int_t    fPlotType;     ///< plot type
  Bool_t   fUseFitRanges; ///< yes -> use the fit ranges to plot the data, no (default) -> use range information if present
  Bool_t   fLogX;         ///< yes -> x-axis in log-scale, no (default) -> x-axis in lin-scale
  Bool_t   fLogY;         ///< yes -> y-axis in log-scale, no (default) -> y-axis in lin-scale
  Int_t    fViewPacking;  ///< -1 -> use the run packing to generate the view, otherwise is fViewPacking for the binning of ALL runs.
  PComplexVector fRuns;   ///< list of runs to be plotted
  PDoubleVector  fTmin;   ///< time minimum
  PDoubleVector  fTmax;   ///< time maximum
  PDoubleVector  fYmin;   ///< asymmetry/counts minimum
  PDoubleVector  fYmax;   ///< asymmetry/counts maximum
} PMsrPlotStructure;

//-------------------------------------------------------------
/**
 * <p>  typedef to make to code more readable: list of plots.
 */
typedef vector<PMsrPlotStructure>  PMsrPlotList;

//-------------------------------------------------------------
/**
 * <p>
 */
typedef struct {
  Bool_t fValid;
  PMsrLines fStatLines;
  TString fDate;     ///< string holding fitting date and time
  Bool_t   fChisq;   ///< flag telling if min = chi2 or min = max.likelyhood
  Double_t fMin;     ///< chi2 or max. likelyhood
  UInt_t fNdf;       ///< number of degrees of freedom
} PMsrStatisticStructure;

#endif // _PMUSR_H_
