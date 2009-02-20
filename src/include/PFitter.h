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

#include "PMsrHandler.h"
#include "PRunListCollection.h"
#include "PFitterFcn.h"

/*
Will handle all the possible minuit commands and actually do things ...
*/

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

class PFitter
{
  public:
    PFitter(PMsrHandler *runInfo, PRunListCollection *runListCollection, bool chisq_only = false);
    virtual ~PFitter();

    bool IsValid() { return fIsValid; }
    bool DoFit();

  private:
    bool fIsValid;
    bool fChisqOnly;
    bool fUseChi2;

    PMsrHandler *fRunInfo;

    PMsrParamList fParams; ///< msr-file parameters

    PMsrLines  fCmdLines; ///< all the Minuit commands from the msr-file
    PIntVector fCmdList;  ///< command list

    PFitterFcn *fFitterFcn;
    ROOT::Minuit2::MnUserParameters fMnUserParams; ///< minuit2 input parameter list
    ROOT::Minuit2::FunctionMinimum *fFcnMin;       ///< function minimum object
    ROOT::Minuit2::MnUserParameterState *fMnUserParamState; ///< keeps the current user parameter state

    bool CheckCommands();
    bool SetParameters();

    bool ExecuteHesse();
    bool ExecuteMigrad();
    bool ExecuteMinimize();
    bool ExecuteMinos();
    bool ExecuteSave();
    bool ExecuteSimplex();
};

#endif // _PFITTER_H_
