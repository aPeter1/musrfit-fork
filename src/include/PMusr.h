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
#define MSR_TAG_PLOT         6
#define MSR_TAG_STATISTIC    7

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
  int fXIndex;                    ///< index for the x-data vector
  int fYIndex;                    ///< index for the y-data vector
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
  double fTemp;                    ///< temperature during the run
  double fEnergy;                  ///< implantation energy of the muon
  double fTimeResolution;          ///< time resolution of the run
  PDoubleVector fT0s;              ///< vector of t0's of a run
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
  TString fRunName;       ///< name of the run file
  TString fBeamline;       ///< e.g. mue4, mue1, pim3, emu, m15, ... (former: run type)
  TString fInstitute;      ///< e.g. psi, ral, triumf (former: run format)
  TString fFileFormat;     ///< e.g. root, nexus, psi-bin, mud, ascii, db
  int fFitType;            ///< fit type: 0=single histo fit, 2=asymmetry fit, 4=asymmetry in RRF, 8=non muSR
  int fAlphaParamNo;       ///< alpha
  int fBetaParamNo;        ///<
  int fNormParamNo;        ///<
  int fBkgFitParamNo;      ///<
  int fPhaseParamNo;       ///<
  int fLifetimeParamNo;    ///<
  bool fLifetimeCorrection; ///<
  PIntVector fMap;         ///<
  int fForwardHistoNo;     ///<
  int fBackwardHistoNo;    ///<
  bool fBkgFixPresent[2];  ///< flag showing if a fixed background is present
  double fBkgFix[2];       ///<
  int fBkgRange[4];        ///<
  int fDataRange[4];       ///<
  int fT0[2];              ///<
  double fFitRange[2];     ///<
  int fPacking;            ///<
  double fRRFFreq;         ///< rotating reference frequency
  int fRRFPacking;         ///< rotating reference packing
  int fAlpha2ParamNo;      ///<
  int fBeta2ParamNo;       ///<
  int fRightHistoNo;       ///<
  int fLeftHistoNo;        ///<
  int fXYDataIndex[2];     ///< used to get the data indices when using db-files
  TString fXYDataLabel[2]; ///< used to get the indices via labels when using db-files
} PMsrRunStructure;

//-------------------------------------------------------------
/**
 * <p>  typedef to make to code more readable: list of runs with its parameters.
 */
typedef vector<PMsrRunStructure> PMsrRunList;

//-------------------------------------------------------------
/**
 * <p> Holds the information of a single plot block
 */
typedef struct {
  int    fPlotType;     ///< plot type
  PComplexVector fRuns; ///< list of runs to be plotted
  double fTmin;         ///< time minimum
  double fTmax;         ///< time maximum
  double fYmin;         ///< asymmetry/counts minimum
  double fYmax;         ///< asymmetry/counts maximum
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
  PMsrLines fStatLines;
  TString fDate;     ///< string holding fitting date and time
  bool   fChisq;     ///< flag telling if min = chi2 or min = max.likelyhood
  double fMin;       ///< chi2 or max. likelyhood
  unsigned int fNdf; ///< number of degrees of freedom
} PMsrStatisticStructure;

#endif // _PMUSR_H_
