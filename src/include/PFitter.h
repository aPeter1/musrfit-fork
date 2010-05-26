/***************************************************************************

  PFitter.h

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
#define PMN_HESSE             3
#define PMN_MACHINE_PRECISION 4
#define PMN_MIGRAD            5
#define PMN_MINIMIZE          6
#define PMN_MINOS             7
#define PMN_PLOT              8
#define PMN_SAVE              9
#define PMN_SCAN              10
#define PMN_SIMPLEX           12
#define PMN_STRATEGY          13
#define PMN_USER_COVARIANCE   14
#define PMN_USER_PARAM_STATE  15
#define PMN_PRINT             16

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

    UInt_t fStrategy; ///< fitting strategy (see minuit2 manual).

    PMsrHandler *fRunInfo; ///< pointer to the msr-file handler

    PMsrParamList fParams; ///< msr-file parameters

    PMsrLines  fCmdLines; ///< all the Minuit commands from the msr-file
    PIntVector fCmdList;  ///< command list

    PFitterFcn *fFitterFcn; ///< pointer to the fitter function object
    ROOT::Minuit2::MnUserParameters fMnUserParams; ///< minuit2 input parameter list
    ROOT::Minuit2::FunctionMinimum *fFcnMin;       ///< function minimum object
    ROOT::Minuit2::MnUserParameterState *fMnUserParamState; ///< keeps the current user parameter state

    // minuit2 scan/contours command relate variables (see MnScan/MnContours in the minuit2 user manual)
    Bool_t fScanAll;           ///< flag. false: single parameter scan, true: not implemented yet (see MnScan/MnContours in the minuit2 user manual)
    UInt_t fScanParameter[2];  ///< scan parameter. idx=0: used for scan and contour, idx=1: used for contour (see MnScan/MnContours in the minuit2 user manual)
    UInt_t fScanNoPoints;      ///< number of points in a scan/contour (see MnScan/MnContours in the minuit2 user manual)
    Double_t fScanLow;         ///< scan range low. default=0.0 which means 2 std dev. (see MnScan/MnContours in the minuit2 user manual)
    Double_t fScanHigh;        ///< scan range high. default=0.0 which means 2 std dev. (see MnScan/MnContours in the minuit2 user manual)
    PDoublePairVector fScanData; ///< keeps the scan/contour data

    // commands
    Bool_t CheckCommands();
    Bool_t SetParameters();

    Bool_t ExecuteContours();
    Bool_t ExecuteHesse();
    Bool_t ExecuteMigrad();
    Bool_t ExecuteMinimize();
    Bool_t ExecuteMinos();
    Bool_t ExecutePlot();
    Bool_t ExecuteScan();
    Bool_t ExecuteSave();
    Bool_t ExecuteSimplex();
};

#endif // _PFITTER_H_
