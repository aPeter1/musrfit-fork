/***************************************************************************

  PMsrHandler.h

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

#ifndef _PMSRHANDLER_H_
#define _PMSRHANDLER_H_

/*
#include <vector>
using namespace std;
*/

#include <TString.h>
#include <TComplex.h>

#include "PMusr.h"
#include "PFunctionHandler.h"
#include "PFunctionGrammar.h"
#include "PFunction.h"

//-------------------------------------------------------------
/**
 * <p>
 */
class PMsrHandler
{
  public:
    PMsrHandler(char *fileName);
    virtual ~PMsrHandler();

    virtual int ReadMsrFile();
    virtual int WriteMsrLogFile();

    virtual TString*                GetMsrTitle() { return &fTitle; }
    virtual PMsrParamList*          GetMsrParamList() { return &fParam; }
    virtual PMsrLines*              GetMsrTheory() { return &fTheory; }
    virtual PMsrLines*              GetMsrFunctions() { return &fFunctions; }
    virtual PMsrRunList*            GetMsrRunList() { return &fRuns; }
    virtual PMsrLines*              GetMsrCommands() { return &fCommands; }
    virtual PMsrPlotList*           GetMsrPlotList() { return &fPlots; }
    virtual PMsrStatisticStructure* GetMsrStatistic() { return &fStatistic; }

    virtual unsigned int GetNoOfParams() { return fParam.size(); }
    virtual const TString& GetFileName() const { return fFileName; }

    virtual bool SetMsrParamValue(unsigned int i, double value);
    virtual bool SetMsrParamStep(unsigned int i, double value);
    virtual bool SetMsrParamPosError(unsigned int i, double value);

    virtual void SetMsrStatisticMin(double min) { fStatistic.fMin = min; }
    virtual void SetMsrStatisticNdf(unsigned int ndf) { fStatistic.fNdf = ndf; }

    virtual int GetNoOfFuncs() { return fFuncHandler->GetNoOfFuncs(); }
    virtual unsigned int GetFuncNo(int idx) { return fFuncHandler->GetFuncNo(idx); }
    virtual unsigned int GetFuncIndex(int funNo) { return fFuncHandler->GetFuncIndex(funNo); }
    virtual bool CheckMapAndParamRange(unsigned int mapSize, unsigned int paramSize)
                       { return fFuncHandler->CheckMapAndParamRange(mapSize, paramSize); }
    virtual double EvalFunc(unsigned int i, vector<int> map, vector<double> param)
                       { return fFuncHandler->Eval(i,map,param); }

    virtual int ParameterInUse(unsigned int paramNo);
    virtual bool CheckUniquenessOfParamNames(unsigned int &parX, unsigned int &parY);

  private:
    TString                fFileName;
    TString                fTitle;     ///< holds the title string of the msr-file
    PMsrParamList          fParam;     ///< holds a list of the fit parameters
    PMsrLines              fTheory;    ///< holds the theory definition
    PMsrLines              fFunctions; ///< holds the user defined functions
    PMsrRunList            fRuns;      ///< holds a list of run information
    PMsrLines              fCommands;  ///< holds a list of the minuit commands
    PMsrPlotList           fPlots;     ///< holds a list of the plot input parameters
    PMsrStatisticStructure fStatistic; ///< holds the statistic info

    int  fMsrBlockCounter; ///< used to select the proper msr-block

    PFunctionHandler *fFuncHandler; ///< needed to parse functions

    PIntVector fParamInUse; ///< array holding the information if a particular parameter is used at all, i.e. if the theory is using it (perhaps via maps or functions)

    virtual bool HandleFitParameterEntry(PMsrLines &line);
    virtual bool HandleTheoryEntry(PMsrLines &line);
    virtual bool HandleFunctionsEntry(PMsrLines &line);
    virtual bool HandleRunEntry(PMsrLines &line);
    virtual bool HandleCommandsEntry(PMsrLines &line);
    virtual bool HandlePlotEntry(PMsrLines &line);
    virtual bool HandleStatisticEntry(PMsrLines &line);

    virtual void FillParameterInUse(PMsrLines &theory, PMsrLines &funcs, PMsrLines &run);

    virtual void InitRunParameterStructure(PMsrRunStructure &param);
    virtual bool FilterFunMapNumber(TString str, const char *filter, int &no);
};

#endif // _PMSRHANDLER_H_
