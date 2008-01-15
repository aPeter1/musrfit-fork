/***************************************************************************

  PRunBase.h

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

#ifndef _PRUNBASE_H_
#define _PRUNBASE_H_

#include <vector>
using namespace std;

#include <TString.h>

#include "PMusr.h"
#include "PMsrHandler.h"
#include "PRunDataHandler.h"
#include "PTheory.h"
//#include "PFunctions.h"

//------------------------------------------------------------------------------------------
/**
 * brauche ich eine base class um zwischen den verschiedenen run-modi unterscheiden zu können?
 * Ich meine:
 * - single histogram
 * - asymmetry
 * - RRF
 * - non muSR
 *
 * --> JA
 *
 * PTheory and PFunctions werden direkt für jeden run generiert, da man dann maps und functions
 * direkt für den spezifischen run umsetzen kann (da man eliminiert alle maps und functions). Dies
 * garantiert effiziente theory-Aufrufe da diese in chisq/maxlikelyhood x-fach aufgerufen werden.
 */
class PRunBase
{
  public:
    PRunBase();
    PRunBase(PMsrHandler *msrInfo, PRunDataHandler *rawData, unsigned int runNo);
    virtual ~PRunBase();

    virtual double CalcChiSquare(const vector<double>& par) = 0; // pure virtual, i.e. needs to be implemented by the deriving class!!
    virtual double CalcMaxLikelihood(const vector<double>& par) = 0; // pure virtual, i.e. needs to be implemented by the deriving class!!

    virtual void CalcTheory() = 0; // pure virtual, i.e. needs to be implemented by the deriving class!!

    virtual PRunData* GetData() { return &fData; }
    virtual void CleanUp();
    virtual bool IsValid() { return fValid; }

  protected:
    bool fValid;

    int fRunNo;                 ///< number of the run within the msr file
    PMsrHandler      *fMsrInfo; ///< msr-file handler
    PMsrRunStructure *fRunInfo; ///< run info used to filter out needed infos for the run
    PRunDataHandler  *fRawData; ///< holds the raw run data

    vector<int> fParamNo;        ///< vector of parameter numbers for the specifc run

    PRunData fData;         ///< data to be fitted
    double fTimeResolution; ///< time resolution
    vector<int> fT0s;       ///< all t0's of a run! The derived classes will handle it

    virtual bool PrepareData() = 0; // pure virtual, i.e. needs to be implemented by the deriving class!!

    vector<double> fFuncValues; ///< is keeping the values of the functions from the FUNCTIONS block
    PTheory *fTheory;       ///< theory needed to calculate chi-square
};

#endif // _PRUNBASE_H_
