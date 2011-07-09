/***************************************************************************

  PMsrHandler.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2011 by Andreas Suter                              *
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

#ifndef _PMSRHANDLER_H_
#define _PMSRHANDLER_H_

#include <TString.h>
#include <TComplex.h>

#include "PMusr.h"
#include "PFunctionHandler.h"
#include "PFunctionGrammar.h"
#include "PFunction.h"

//-------------------------------------------------------------
/**
 * <p>This class provides the routines needed to handle msr-files, i.e. reading, writing, parsing, etc.
 */
class PMsrHandler
{
  public:
    PMsrHandler(const Char_t *fileName, const Bool_t writeExpectedChisq=false);
    virtual ~PMsrHandler();

    virtual Int_t ReadMsrFile();
    virtual Int_t WriteMsrLogFile(const Bool_t messages = true);
    virtual Int_t WriteMsrFile(const Char_t *filename, map<UInt_t, TString> *commentsPAR = 0, map<UInt_t, TString> *commentsTHE = 0, \
                                                       map<UInt_t, TString> *commentsFUN = 0, map<UInt_t, TString> *commentsRUN = 0);

    virtual TString*                GetMsrTitle() { return &fTitle; }
    virtual PMsrParamList*          GetMsrParamList() { return &fParam; }
    virtual PMsrLines*              GetMsrTheory() { return &fTheory; }
    virtual PMsrLines*              GetMsrFunctions() { return &fFunctions; }
    virtual PMsrRunList*            GetMsrRunList() { return &fRuns; }
    virtual PMsrLines*              GetMsrCommands() { return &fCommands; }
    virtual PMsrFourierStructure*   GetMsrFourierList() { return &fFourier; }
    virtual PMsrPlotList*           GetMsrPlotList() { return &fPlots; }
    virtual PMsrStatisticStructure* GetMsrStatistic() { return &fStatistic; }

    virtual TString* GetMsrFileDirectoryPath() { return &fMsrFileDirectoryPath; }

    virtual UInt_t GetNoOfParams() { return fParam.size(); }
    virtual const TString& GetFileName() const { return fFileName; }

    virtual void SetMsrTitle(const TString &title) { fTitle = title; }

    virtual Bool_t SetMsrParamValue(UInt_t i, Double_t value);
    virtual Bool_t SetMsrParamStep(UInt_t i, Double_t value);
    virtual Bool_t SetMsrParamPosErrorPresent(UInt_t i, Bool_t value);
    virtual Bool_t SetMsrParamPosError(UInt_t i, Double_t value);

    virtual void SetMsrT0Entry(UInt_t runNo, UInt_t idx, Int_t bin);
    virtual void SetMsrAddT0Entry(UInt_t runNo, UInt_t addRunIdx, UInt_t histoIdx, Int_t bin);
    virtual void SetMsrDataRangeEntry(UInt_t runNo, UInt_t idx, Int_t bin);
    virtual void SetMsrBkgRangeEntry(UInt_t runNo, UInt_t idx, Int_t bin);

    virtual void CopyMsrStatisticBlock() { fCopyStatisticsBlock = true; }
    virtual void SetMsrStatisticConverged(Bool_t converged) { fStatistic.fValid = converged; }
    virtual void SetMsrStatisticMin(Double_t min) { fStatistic.fMin = min; }
    virtual void SetMsrStatisticNdf(UInt_t ndf) { fStatistic.fNdf = ndf; }

    virtual Int_t GetNoOfFuncs() { return fFuncHandler->GetNoOfFuncs(); }
    virtual UInt_t GetFuncNo(Int_t idx) { return fFuncHandler->GetFuncNo(idx); }
    virtual UInt_t GetFuncIndex(Int_t funNo) { return fFuncHandler->GetFuncIndex(funNo); }
    virtual Bool_t CheckMapAndParamRange(UInt_t mapSize, UInt_t paramSize)
                       { return fFuncHandler->CheckMapAndParamRange(mapSize, paramSize); }
    virtual Double_t EvalFunc(UInt_t i, vector<Int_t> map, vector<Double_t> param)
                       { return fFuncHandler->Eval(i,map,param); }

    virtual UInt_t GetNoOfFitParameters(UInt_t idx);
    virtual Int_t ParameterInUse(UInt_t paramNo);
    virtual Bool_t CheckRunBlockIntegrity();
    virtual Bool_t CheckUniquenessOfParamNames(UInt_t &parX, UInt_t &parY);
    virtual Bool_t CheckMaps();
    virtual Bool_t CheckFuncs();
    virtual Bool_t CheckHistoGrouping();
    virtual Bool_t CheckAddRunParameters();
    virtual void CheckMaxLikelihood();

  private:
    Bool_t                 fWriteExpectedChisq; ///< flag shows if expected chisq shall be written to the msr-file

    TString                fFileName;  ///< file name of the msr-file
    TString                fMsrFileDirectoryPath; ///< msr-file directory path
    TString                fTitle;     ///< holds the title string of the msr-file
    PMsrParamList          fParam;     ///< holds a list of the fit parameters
    PMsrLines              fTheory;    ///< holds the theory definition
    PMsrLines              fFunctions; ///< holds the user defined functions
    PMsrRunList            fRuns;      ///< holds a list of run information
    PMsrLines              fCommands;  ///< holds a list of the minuit commands
    PMsrFourierStructure   fFourier;   ///< holds the parameters used for the Fourier transform
    PMsrPlotList           fPlots;     ///< holds a list of the plot input parameters
    PMsrStatisticStructure fStatistic; ///< holds the statistic info

    Int_t  fMsrBlockCounter; ///< used to select the proper msr-block

    PFunctionHandler *fFuncHandler; ///< needed to parse functions

    PIntVector fParamInUse; ///< array holding the information if a particular parameter is used at all, i.e. if the theory is using it (perhaps via maps or functions)

    Bool_t fCopyStatisticsBlock; ///< flag, if true: just copy to old statistics block (musrt0), otherwise write a new one (musrfit)

    virtual Bool_t HandleFitParameterEntry(PMsrLines &line);
    virtual Bool_t HandleTheoryEntry(PMsrLines &line);
    virtual Bool_t HandleFunctionsEntry(PMsrLines &line);
    virtual Bool_t HandleRunEntry(PMsrLines &line);
    virtual Bool_t HandleCommandsEntry(PMsrLines &line);
    virtual Bool_t HandleFourierEntry(PMsrLines &line);
    virtual Bool_t HandlePlotEntry(PMsrLines &line);
    virtual Bool_t HandleStatisticEntry(PMsrLines &line);

    virtual void FillParameterInUse(PMsrLines &theory, PMsrLines &funcs, PMsrLines &run);

    virtual void InitFourierParameterStructure(PMsrFourierStructure &fourier);

    virtual Bool_t FilterNumber(TString str, const Char_t *filter, Int_t offset, Int_t &no);

    virtual UInt_t NeededPrecision(Double_t dval, UInt_t precLimit=13);
    virtual UInt_t LastSignificant(Double_t dval, UInt_t precLimit=6);
};

#endif // _PMSRHANDLER_H_
