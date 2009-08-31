/***************************************************************************

  PMusr.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
 *   andreas.suter@psi.c                                                   *
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
typedef vector<int> PIntVector;

//-------------------------------------------------------------
/**
 * <p>
 */
typedef vector<double> PDoubleVector;

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
 * <p> Predominantly used in PRunBase.
 */
typedef struct {
  // data related info
  double fDataTimeStart;
  double fDataTimeStep;
  PDoubleVector fX;       // only used for non-muSR
  PDoubleVector fValue;
  PDoubleVector fError;
  // theory related info
  double fTheoryTimeStart;
  double fTheoryTimeStep;
  PDoubleVector fXTheory; // only used for non-muSR
  PDoubleVector fTheory;
} PRunData;

//-------------------------------------------------------------
/**
 * <p>
 */
typedef struct {
  bool fFromAscii;                ///< if true: data file was an ascii input file, otherwise it is a db input file
  PStringVector fLabels;          ///< vector of all labels (used for x-, y-axis title in view)
  PStringVector fDataTags;        ///< vector of all data tags
  vector<PDoubleVector> fData;    ///< vector of all data
  vector<PDoubleVector> fErrData; ///< vector of all data errors
} PNonMusrRawRunData;

//-------------------------------------------------------------
/**
 * <p>
 */
typedef struct {
  TString fRunName;                ///< name of the run
  TString fRunTitle;               ///< run title
  TString fSetup;                  ///< description of the setup of this run
  double fField;                   ///< magnetic field value
//  double fTemp;                    ///< temperature during the run
  vector< pair<double, double> > fTemp;    ///< measured temperatures and standard deviations during the run
  double fEnergy;                  ///< implantation energy of the muon
  double fTransport;               ///< LEM transport settings (Moderator HV)
  PDoubleVector fRingAnode;        ///< LEM ring anode HVs (L,R[,T,B])
  double fTimeResolution;          ///< time resolution of the run
  PIntVector fT0s;                 ///< vector of t0's of a run
  vector<PDoubleVector> fDataBin;  ///< vector of all histos of a run
  PNonMusrRawRunData fDataNonMusr; ///< keeps all ascii- or db-file info in case of nonMusr fit
} PRawRunData;

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
  int     fLineNo; ///< original line number of the msr-file
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
  int fNoOfParams;              ///< how many parameters are given
  int fNo;                      ///< parameter number
  TString fName;                ///< name
  double fValue;                ///< value
  double fStep;                 ///< step / error / neg_error, depending on the situation
  bool   fPosErrorPresent;      ///< positive error is defined (as a number)
  double fPosError;             ///< positive error if present
  bool   fLowerBoundaryPresent; ///< flag showing if a lower boundary is present
  double fLowerBoundary;        ///< lower boundary for the fit parameter
  bool   fUpperBoundaryPresent; ///< flag showing if an upper boundary is present
  double fUpperBoundary;        ///< upper boundary for the fit parameter
} PMsrParamStructure;

//-------------------------------------------------------------
/**
 * <p>  typedef to make to code more readable: vector of fit parameters.
 */
typedef vector<PMsrParamStructure> PMsrParamList;

//-------------------------------------------------------------
/**
 * <p> Holds the information of a single plot block
 *
 */
typedef struct {
  PStringVector fRunName;     ///< name of the run file
  PStringVector fBeamline;    ///< e.g. mue4, mue1, pim3, emu, m15, ... (former: run type)
  PStringVector fInstitute;   ///< e.g. psi, ral, triumf (former: run format)
  PStringVector fFileFormat;  ///< e.g. root, nexus, psi-bin, mud, ascii, db
  int fFitType;               ///< fit type: 0=single histo fit, 2=asymmetry fit, 4=asymmetry in RRF, 8=non muSR
  int fAlphaParamNo;          ///< alpha parameter number (fit type 2, 4)
  int fBetaParamNo;           ///< beta parameter number  (fit type 2, 4)
  int fNormParamNo;           ///< N0 parameter number (fit type 0)
  int fBkgFitParamNo;         ///< background fit parameter number (fit type 0)
  int fPhaseParamNo;          ///< ??? NEEDED ??? NEEDS TO BE CHECKED !!!
  int fLifetimeParamNo;       ///< muon lifetime parameter number (fit type 0)
  bool fLifetimeCorrection;   ///< lifetime correction flag for viewing (fit type 0)
  PIntVector fMap;            ///< map vector needed to switch parameters for different runs within a single theory
  int fForwardHistoNo;        ///< forward histogram number (fit type 0, 2, 4)
  int fBackwardHistoNo;       ///< backward histogram number (fit type 2, 4)
  PDoubleVector fBkgFix;      ///< fixed background in (1/ns) (fit type 0, 2, 4)
  PIntVector fBkgRange;       ///< background bin range (fit type 0, 2, 4)
  PIntVector fDataRange;      ///< data bin range (fit type 0, 2, 4)
  PIntVector fT0;             ///< t0 bins (fit type 0, 2, 4). if fit type 0 -> f0, f1, f2, ...; if fit type 2, 4 -> f0, b0, f1, b1, ...
  double fFitRange[2];        ///< fit range in (us)
  int fPacking;               ///< packing/rebinning
  double fRRFFreq;            ///< rotating reference frequency (fit type 4)
  int fRRFPacking;            ///< rotating reference packing (fit type 4)
  int fAlpha2ParamNo;         ///< rotating reference alpha2 (fit type 4)
  int fBeta2ParamNo;          ///< rotating reference beta2 (fit type 4)
  int fRightHistoNo;          ///< rotating reference right histogram number (fit type 4)
  int fLeftHistoNo;           ///< rotating reference left histogram number (fit type 4)
  int fXYDataIndex[2];        ///< used to get the data indices when using db-files (fit type 8)
  TString fXYDataLabel[2];    ///< used to get the indices via labels when using db-files  (fit type 8)
} PMsrRunStructure;

//-------------------------------------------------------------
/**
 * <p>  typedef to make to code more readable: list of runs with its parameters.
 */
typedef vector<PMsrRunStructure> PMsrRunList;

//-------------------------------------------------------------
/**
 * <p> Holds the information of the Fourier block
 */
typedef struct {
  bool fFourierBlockPresent; ///< flag indicating if a Fourier block is present in the msr-file
  int  fUnits;               ///< flag used to indicate the units. 0=field units (G); 1=frequency units (MHz); 2=Mc/s
  int  fFourierPower;        ///< i.e. zero padding up to 2^fFourierPower, default = 0 which means NO zero padding
  int  fApodization;         ///< tag indicating the kind of apodization wished, 0=no appodization (default), 1=weak, 2=medium, 3=strong (for details see the docu)
  int  fPlotTag;             ///< tag used for initial plot. 0=real, 1=imaginary, 2=real & imaginary (default), 3=power, 4=phase
  int  fPhaseParamNo;        ///< parameter number if used instead of a phase value
  double fPhase;             ///< phase
  double fRangeForPhaseCorrection[2]; ///< field/frequency range for automatic phase correction
  double fPlotRange[2];      ///< field/frequency plot range
  double fPhaseIncrement;    ///< phase increment for manual phase optimization
} PMsrFourierStructure;

//-------------------------------------------------------------
/**
 * <p> Holds the information of a single plot block
 */
typedef struct {
  int    fPlotType;     ///< plot type
  bool   fUseFitRanges; ///< yes -> use the fit ranges to plot the data, no (default) -> use range information if present
  bool   fLogX;         ///< yes -> x-axis in log-scale, no (default) -> x-axis in lin-scale
  bool   fLogY;         ///< yes -> y-axis in log-scale, no (default) -> y-axis in lin-scale
  int    fViewPacking;  ///< -1 -> use the run packing to generate the view, otherwise is fViewPacking for the binning of ALL runs.
  PComplexVector fRuns; ///< list of runs to be plotted
  PDoubleVector  fTmin; ///< time minimum
  PDoubleVector  fTmax; ///< time maximum
  PDoubleVector  fYmin; ///< asymmetry/counts minimum
  PDoubleVector  fYmax; ///< asymmetry/counts maximum
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
  bool fValid;
  PMsrLines fStatLines;
  TString fDate;     ///< string holding fitting date and time
  bool   fChisq;     ///< flag telling if min = chi2 or min = max.likelyhood
  double fMin;       ///< chi2 or max. likelyhood
  unsigned int fNdf; ///< number of degrees of freedom
} PMsrStatisticStructure;

#endif // _PMUSR_H_
