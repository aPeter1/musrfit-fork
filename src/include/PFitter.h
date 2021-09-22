/***************************************************************************

  PFitter.h

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

#ifndef _PFITTER_H_
#define _PFITTER_H_

#include "Minuit2/MnUserParameters.h"
#include "Minuit2/FunctionMinimum.h"

#include "PMusr.h"
#include "PMsrHandler.h"
#include "PRunListCollection.h"
#include "PFitterFcn.h"

#define PMN_INTERACTIVE       0
#define PMN_CONTOURS          1
#define PMN_EIGEN             2
#define PMN_FIT_RANGE         3
#define PMN_FIX               4
#define PMN_HESSE             5
#define PMN_MACHINE_PRECISION 6
#define PMN_MIGRAD            7
#define PMN_MINIMIZE          8
#define PMN_MINOS             9
#define PMN_PLOT              10
#define PMN_RELEASE           11
#define PMN_RESTORE           12
#define PMN_SAVE              13
#define PMN_SCAN              14
#define PMN_SIMPLEX           15
#define PMN_STRATEGY          16
#define PMN_USER_COVARIANCE   17
#define PMN_USER_PARAM_STATE  18
#define PMN_PRINT             19
#define PMN_SECTOR            20

//-----------------------------------------------------------------------------
/**
 * <p>The PSectorChisq class is needed to store the chisq/maxLH of a sector.
 * A sector is a time window from fgb to fLast (fLast < lgb). It also stores
 * these information for each run of the msr-file.
 */
class PSectorChisq
{
  public:
    PSectorChisq(UInt_t noOfRuns);

    void SetRunFirstTime(Double_t first, UInt_t idx);
    void SetSectorTime(Double_t last) { fLast = last; }
    void SetChisq(Double_t chisq) { fChisq = chisq; }
    void SetChisq(Double_t chisq, UInt_t idx);
    void SetExpectedChisq(Double_t expChisq) { fExpectedChisq = expChisq; }
    void SetExpectedChisq(Double_t chisq, UInt_t idx);
    void SetNDF(UInt_t ndf) { fNDF = ndf; }
    void SetNDF(UInt_t ndf, UInt_t idx);

    Double_t GetTimeRangeFirst(UInt_t idx);
    Double_t GetTimeRangeLast() { return fLast; }
    Double_t GetChisq() { return fChisq; }
    Double_t GetChisq(UInt_t idx);
    Double_t GetExpectedChisq() { return fExpectedChisq; }
    Double_t GetExpectedChisq(UInt_t idx);
    UInt_t   GetNDF() { return fNDF; }
    UInt_t   GetNDF(UInt_t idx);
    UInt_t   GetNoRuns() { return fNoOfRuns; }

  private:
    UInt_t   fNoOfRuns; ///< number of runs presesent
    Double_t fLast;  ///< requested time stamp
    Double_t fChisq; ///< chisq or maxLH for the sector
    Double_t fExpectedChisq; ///< keep the expected chisq or maxLH for the sector
    UInt_t   fNDF;   ///< NDF for the sector
    PDoubleVector fFirst; ///< time stamp for fgb for a given run
    PDoubleVector fChisqRun; ///< chisq or maxLH for the sector and run
    PDoubleVector fExpectedChisqRun; ///< expected chisq or maxLH for the sector and run
    PUIntVector fNDFRun; ///< NDF for the sector and run
};

//-----------------------------------------------------------------------------
/**
 * <p>Interface class to minuit2.
 */
class PFitter
{
  public:
    PFitter(PMsrHandler *runInfo, PRunListCollection *runListCollection, Bool_t chisq_only = false);
    virtual ~PFitter();

    Bool_t IsValid() { return fIsValid; }
    Bool_t IsScanOnly() { return fIsScanOnly; }
    Bool_t HasConverged() { return fConverged; }
    Bool_t DoFit();

  private:
    Bool_t fIsValid;     ///< flag. true: the fit is valid.
    Bool_t fIsScanOnly;  ///< flag. true: scan along some parameters (no fitting).
    Bool_t fConverged;   ///< flag. true: the fit has converged.
    Bool_t fChisqOnly;   ///< flag. true: calculate chi^2 only (no fitting).
    Bool_t fUseChi2;     ///< flag. true: chi^2 fit. false: log-max-likelihood
    UInt_t fPrintLevel;  ///< tag, showing the level of messages whished. 0=minimum, 1=standard, 2=maximum

    UInt_t fStrategy; ///< fitting strategy (see minuit2 manual).

    PMsrHandler *fRunInfo; ///< pointer to the msr-file handler
    PRunListCollection *fRunListCollection; ///< pointer to the run list collection

    PMsrParamList fParams; ///< msr-file parameters

    PMsrLines      fCmdLines; ///< all the Minuit commands from the msr-file
    PIntPairVector fCmdList;  ///< command list, first=cmd, second=cmd line index

    PFitterFcn *fFitterFcn; ///< pointer to the fitter function object

    ROOT::Minuit2::MnUserParameters fMnUserParams; ///< minuit2 input parameter list
    ROOT::Minuit2::FunctionMinimum *fFcnMin;       ///< function minimum object

    // minuit2 scan/contours command relate variables (see MnScan/MnContours in the minuit2 user manual)
    Bool_t fScanAll;           ///< flag. false: single parameter scan, true: not implemented yet (see MnScan/MnContours in the minuit2 user manual)
    UInt_t fScanParameter[2];  ///< scan parameter. idx=0: used for scan and contour, idx=1: used for contour (see MnScan/MnContours in the minuit2 user manual)
    UInt_t fScanNoPoints;      ///< number of points in a scan/contour (see MnScan/MnContours in the minuit2 user manual)
    Double_t fScanLow;         ///< scan range low. default=0.0 which means 2 std dev. (see MnScan/MnContours in the minuit2 user manual)
    Double_t fScanHigh;        ///< scan range high. default=0.0 which means 2 std dev. (see MnScan/MnContours in the minuit2 user manual)
    PDoublePairVector fScanData; ///< keeps the scan/contour data

    PDoublePairVector fOriginalFitRange; ///< keeps the original fit range in case there is a range command in the COMMAND block

    PStringVector fElapsedTime;

    Bool_t fSectorFlag; ///< sector command present flag    
    std::vector<PSectorChisq> fSector; ///< stores all chisq/maxLH sector information

    std::vector<bool> fPhase; ///< flag array in which an entry is true if the related parameter value is a phase

    // phase related functions
    void GetPhaseParams();
    PIntVector GetParFromFun(const TString funStr);
    PIntVector GetParFromMap(const TString mapStr);

    // commands
    Bool_t CheckCommands();
    Bool_t SetParameters();

    Bool_t ExecuteContours();
    Bool_t ExecuteFitRange(UInt_t lineNo);
    Bool_t ExecuteFix(UInt_t lineNo);
    Bool_t ExecuteHesse();
    Bool_t ExecuteMigrad();
    Bool_t ExecuteMinimize();
    Bool_t ExecuteMinos();
    Bool_t ExecutePlot();
    Bool_t ExecutePrintLevel(UInt_t lineNo);
    Bool_t ExecuteRelease(UInt_t lineNo);
    Bool_t ExecuteRestore();
    Bool_t ExecuteScan();
    Bool_t ExecuteSave(Bool_t first);
    Bool_t ExecuteSimplex();
    void   PrepareSector(PDoubleVector &param, PDoubleVector &error);
    Bool_t ExecuteSector(std::ofstream &fout);

    Double_t MilliTime();
};

#endif // _PFITTER_H_
