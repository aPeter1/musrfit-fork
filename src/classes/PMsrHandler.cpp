/***************************************************************************

  PMsrHandler.cpp

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

#include <math.h>

#include <string>
#include <iostream>
#include <fstream>

#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TDatime.h>

#include "PMusr.h"
#include "PMsrHandler.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 *
 * \param fileName name of a msr-file.
 */
PMsrHandler::PMsrHandler(const Char_t *fileName, PStartupOptions *startupOptions, const Bool_t fourierOnly) :
    fFourierOnly(fourierOnly), fStartupOptions(startupOptions), fFileName(fileName)
{ 
  // init variables
  fMsrBlockCounter = 0;

  fTitle = "";

  fCopyStatisticsBlock = false;
  fStatistic.fValid = false;
  fStatistic.fChisq = true;
  fStatistic.fMin   = -1.0;
  fStatistic.fNdf   = 0;
  fStatistic.fMinExpected = 0.0;
  fStatistic.fMinExpectedPerHisto.clear();
  fStatistic.fNdfPerHisto.clear();

  fFuncHandler = nullptr;

  // check if the file name given is a path-file-name, and if yes, split it into path and file name.
  if (fFileName.Contains("/")) {
    Int_t idx = -1;
    while (fFileName.Index("/", idx+1) != -1) {
      idx = fFileName.Index("/", idx+1);
    }
    fMsrFileDirectoryPath = fFileName;
    fMsrFileDirectoryPath.Remove(idx+1);
  } else {
    fMsrFileDirectoryPath = "./";
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor
 */
PMsrHandler::~PMsrHandler()
{
  fParam.clear();
  fTheory.clear();
  fFunctions.clear();
  fRuns.clear();
  fCommands.clear();
  fPlots.clear();
  fStatistic.fStatLines.clear();
  fStatistic.fMinExpectedPerHisto.clear();
  fStatistic.fNdfPerHisto.clear();
  fParamInUse.clear();

  if (fFuncHandler) {
    delete fFuncHandler;
    fFuncHandler = nullptr;
  }
}

//--------------------------------------------------------------------------
// ReadMsrFile (public)
//--------------------------------------------------------------------------
/**
 * <p>Reads and parses a msr-file.
 *
 * <p><b>return:</b>
 * - PMUSR_SUCCESS if everything is OK
 * - line number if an error in the MSR file was encountered which cannot be handled.
 */
Int_t PMsrHandler::ReadMsrFile()
{
  std::ifstream f;
  std::string str;
  TString line;
  Int_t line_no = 0;
  Int_t result = PMUSR_SUCCESS;

  PMsrLineStructure current;

  PMsrLines fit_parameter;
  PMsrLines theory;
  PMsrLines functions;
  PMsrLines global;
  PMsrLines run;
  PMsrLines commands;
  PMsrLines fourier;
  PMsrLines plot;
  PMsrLines statistic;

  // init stuff
  InitFourierParameterStructure(fFourier);

  // open msr-file
  f.open(fFileName.Data(), std::iostream::in);
  if (!f.is_open()) {
    return PMUSR_MSR_FILE_NOT_FOUND;
  }

  fMsrBlockCounter = -1; // no msr block

  // read msr-file
  while (!f.eof()) {

    // read a line
    getline(f, str);
    line = str.c_str();
    line_no++;

    current.fLineNo = line_no;
    current.fLine   = line;

    if (line.BeginsWith("#") || line.IsWhitespace()) { // if the line is a comment/empty go to the next one
      continue;
    }

    // remove leading spaces
    line.Remove(TString::kLeading, ' ');

    if (!line.IsWhitespace()) { // if not an empty line, handle it
      // check for a msr block
      if (line_no == 1) { // title
        fTitle = line;
      } else if (line.BeginsWith("FITPARAMETER")) { // FITPARAMETER block tag
        fMsrBlockCounter = MSR_TAG_FITPARAMETER;
      } else if (line.BeginsWith("THEORY")) {       // THEORY block tag
        fMsrBlockCounter = MSR_TAG_THEORY;
        theory.push_back(current);
      } else if (line.BeginsWith("FUNCTIONS")) {    // FUNCTIONS block tag
        fMsrBlockCounter = MSR_TAG_FUNCTIONS;
        functions.push_back(current);
      } else if (line.BeginsWith("GLOBAL")) {       // GLOBAL block tag
        fMsrBlockCounter = MSR_TAG_GLOBAL;
        global.push_back(current);
      } else if (line.BeginsWith("RUN")) {          // RUN block tag
        fMsrBlockCounter = MSR_TAG_RUN;
        run.push_back(current);
      } else if (line.BeginsWith("COMMANDS")) {     // COMMANDS block tag
        fMsrBlockCounter = MSR_TAG_COMMANDS;
        commands.push_back(current);
      } else if (line.BeginsWith("FOURIER")) {      // FOURIER block tag
        fMsrBlockCounter = MSR_TAG_FOURIER;
        fourier.push_back(current);
      } else if (line.BeginsWith("PLOT")) {         // PLOT block tag
        fMsrBlockCounter = MSR_TAG_PLOT;
        plot.push_back(current);
      } else if (line.BeginsWith("STATISTIC")) {    // STATISTIC block tag
        fMsrBlockCounter = MSR_TAG_STATISTIC;
        statistic.push_back(current);
      } else { // the read line is some real stuff

        switch (fMsrBlockCounter) {
          case MSR_TAG_FITPARAMETER: // FITPARAMETER block
            fit_parameter.push_back(current);
            break;
          case MSR_TAG_THEORY:       // THEORY block
            theory.push_back(current);
            break;
          case MSR_TAG_FUNCTIONS:    // FUNCTIONS block
            functions.push_back(current);
            break;
          case MSR_TAG_GLOBAL:       // GLOBAL block
            global.push_back(current);
            break;
          case MSR_TAG_RUN:          // RUN block
            run.push_back(current);
            break;
          case MSR_TAG_COMMANDS:     // COMMANDS block
            commands.push_back(current);
            break;
          case MSR_TAG_FOURIER:      // FOURIER block
            fourier.push_back(current);
            break;
          case MSR_TAG_PLOT:         // PLOT block
            plot.push_back(current);
            break;
          case MSR_TAG_STATISTIC:    // STATISTIC block
            statistic.push_back(current);
            break;
          default:
            break;
        }
      }
    }
  }

  // close msr-file
  f.close();

  // execute handler of the various blocks
  if (!HandleFitParameterEntry(fit_parameter))
    result = PMUSR_MSR_SYNTAX_ERROR;
  if (result == PMUSR_SUCCESS)
    if (!HandleTheoryEntry(theory))
      result = PMUSR_MSR_SYNTAX_ERROR;
  if (result == PMUSR_SUCCESS)
    if (!HandleFunctionsEntry(functions))
      result = PMUSR_MSR_SYNTAX_ERROR;
  if ((result == PMUSR_SUCCESS) && (global.size()>0))
    if (!HandleGlobalEntry(global))
      result = PMUSR_MSR_SYNTAX_ERROR;
  if (result == PMUSR_SUCCESS)
    if (!HandleRunEntry(run))
      result = PMUSR_MSR_SYNTAX_ERROR;
  if (result == PMUSR_SUCCESS)
    if (!HandleCommandsEntry(commands))
      result = PMUSR_MSR_SYNTAX_ERROR;
  if (result == PMUSR_SUCCESS)
    if (!HandleFourierEntry(fourier))
      result = PMUSR_MSR_SYNTAX_ERROR;
  if (result == PMUSR_SUCCESS)
    if (!HandlePlotEntry(plot))
      result = PMUSR_MSR_SYNTAX_ERROR;
  if (result == PMUSR_SUCCESS)
    if (!HandleStatisticEntry(statistic))
      result = PMUSR_MSR_SYNTAX_ERROR;

  // check if chisq or max.log likelihood
  fStatistic.fChisq = true;
  for (UInt_t i=0; i<fCommands.size(); i++) {
    if (fCommands[i].fLine.Contains("MAX_LIKELIHOOD"))
      fStatistic.fChisq = false; // max.log likelihood
  }

  // fill parameter-in-use vector
  if ((result == PMUSR_SUCCESS) && !fFourierOnly)
    FillParameterInUse(theory, functions, run);

  // check that each run fulfills the minimum requirements
  if (result == PMUSR_SUCCESS)
    if (!CheckRunBlockIntegrity())
      result = PMUSR_MSR_SYNTAX_ERROR;

  // check that parameter names are unique
  if ((result == PMUSR_SUCCESS) && !fFourierOnly) {
    UInt_t parX, parY;
    if (!CheckUniquenessOfParamNames(parX, parY)) {
      std::cerr << std::endl << ">> PMsrHandler::ReadMsrFile: **SEVERE ERROR** parameter name " << fParam[parX].fName.Data() << " is identical for parameter no " << fParam[parX].fNo << " and " << fParam[parY].fNo << "!";
      std::cerr << std::endl << ">>   Needs to be fixed first!";
      std::cerr << std::endl;
      result = PMUSR_MSR_SYNTAX_ERROR;
    }
  }

  // check that if maps are present in the theory- and/or function-block,
  // that there are really present in the run block
  if ((result == PMUSR_SUCCESS) && !fFourierOnly)
    if (!CheckMaps())
      result = PMUSR_MSR_SYNTAX_ERROR;


  // check that if functions are present in the theory- and/or run-block, that they
  // are really present in the function block
  if (result == PMUSR_SUCCESS)
    if (!CheckFuncs())
      result = PMUSR_MSR_SYNTAX_ERROR;

  // check that if histogram grouping is present that it makes any sense
  if (result == PMUSR_SUCCESS)
    if (!CheckHistoGrouping())
      result = PMUSR_MSR_SYNTAX_ERROR;

  // check that if addrun is present that the given parameter make any sense
  if (result == PMUSR_SUCCESS)
    if (!CheckAddRunParameters())
      result = PMUSR_MSR_SYNTAX_ERROR;

  // check that if RRF settings are present, the RUN block settings do correspond
  if (result == PMUSR_SUCCESS)
    if (!CheckRRFSettings())
      result = PMUSR_MSR_SYNTAX_ERROR;

  if (result == PMUSR_SUCCESS) {
    CheckMaxLikelihood(); // check if the user wants to use max likelihood with asymmetry/non-muSR fit (which is not implemented)
    CheckLegacyLifetimecorrection(); // check if lifetimecorrection is found in RUN blocks, if yes transfer it to PLOT blocks
  }

  // check if the given phases in the Fourier block are in agreement with the Plot block settings
  if ((fFourier.fPhase.size() > 1) && (fPlots.size() > 0)) {
    if (fFourier.fPhase.size() != fPlots[0].fRuns.size()) {
      std::cerr << std::endl << ">> PMsrHandler::ReadMsrFile: **ERROR** if more than one phase is given in the Fourier block,";
      std::cerr << std::endl << ">>     it needs to correspond to the number of runs in the Plot block!" << std::endl;
      result = PMUSR_MSR_SYNTAX_ERROR;
    }
  }

  // clean up
  fit_parameter.clear();
  theory.clear();
  functions.clear();
  global.clear();
  run.clear();
  commands.clear();
  fourier.clear();
  plot.clear();
  statistic.clear();

  return result;
}

//--------------------------------------------------------------------------
// WriteMsrLogFile (public)
//--------------------------------------------------------------------------
/**
 * <p>Writes a mlog-file.
 *
 * <p><b>return:</b>
 * - PMUSR_SUCCESS everything is OK
 * - PMUSR_MSR_LOG_FILE_WRITE_ERROR msr-/mlog-file couldn't be opened
 * - PMUSR_MSR_SYNTAX_ERROR a syntax error has been encountered
 *
 * \param messages if true some additional messages concerning the statistic block are written.
 *                 When using musrt0, messages will be set to false.
 */
Int_t PMsrHandler::WriteMsrLogFile(const Bool_t messages)
{
  const UInt_t prec = 6; // default output precision for float/doubles
  UInt_t neededPrec = 0;
  UInt_t neededWidth = 9;  

  Int_t tag, lineNo = 0, number;
  Int_t runNo = -1, addRunNo = 0;
  Int_t plotNo = -1;
  std::string line;
  TString logFileName, str, sstr, *pstr;
  TObjArray *tokens = nullptr;
  TObjString *ostr = nullptr;
  Bool_t found = false;
  Bool_t statisticBlockFound = false;
  Bool_t partialStatisticBlockFound = true;

  PBoolVector t0TagMissing; // needed for proper musrt0 handling
  for (UInt_t i=0; i<fRuns.size(); i++) {
    t0TagMissing.push_back(true);
  }
  std::vector<PBoolVector> addt0TagMissing; // needed for proper musrt0 handling
  PBoolVector bvec;
  for (UInt_t i=0; i<fRuns.size(); i++) {
    bvec.clear();
    for (UInt_t j=0; j<fRuns[i].GetAddT0BinEntries(); j++)
      bvec.push_back(true);
    addt0TagMissing.push_back(bvec);
  }
  PBoolVector backgroundTagMissing; // needed for proper musrt0 handling
  for (UInt_t i=0; i<fRuns.size(); i++) {
    backgroundTagMissing.push_back(true);
  }
  PBoolVector dataTagMissing; // needed for proper musrt0 handling
  for (UInt_t i=0; i<fRuns.size(); i++) {
    dataTagMissing.push_back(true);
  }

  // add some counters needed in connection to addruns
  Int_t addT0Counter = 0;
  Int_t addT0GlobalCounter = 0;

  std::ifstream fin;
  std::ofstream fout;

  // check msr-file for any missing tags first
  // open msr-file for reading
  fin.open(fFileName.Data(), std::iostream::in);
  if (!fin.is_open()) {
    return PMUSR_MSR_LOG_FILE_WRITE_ERROR;
  }
  while (!fin.eof()) {
    // read a line
    getline(fin, line);
    str = line.c_str();

    if (str.BeginsWith("RUN")) {
      runNo++;
      continue;
    }

    if (runNo == -1)
      continue;

    if (str.BeginsWith("t0"))
      t0TagMissing[runNo] = false;
    else if (str.BeginsWith("background"))
      backgroundTagMissing[runNo] = false;
    else if (str.BeginsWith("data"))
      dataTagMissing[runNo] = false;
  }
  fin.close();

  // construct log file name
  // first find the last '.' in the filename
  Int_t idx = -1;
  while (fFileName.Index(".", idx+1) != -1) {
    idx = fFileName.Index(".", idx+1);
  }
  if (idx == -1)
    return PMUSR_MSR_SYNTAX_ERROR;

  // remove extension
  logFileName = fFileName;
  logFileName.Remove(idx+1);
  logFileName += "mlog";

  // open msr-file for reading
  fin.open(fFileName.Data(), std::iostream::in);
  if (!fin.is_open()) {
    return PMUSR_MSR_LOG_FILE_WRITE_ERROR;
  }

  // open mlog-file for writing
  fout.open(logFileName.Data(), std::iostream::out);
  if (!fout.is_open()) {
    return PMUSR_MSR_LOG_FILE_WRITE_ERROR;
  }

  tag = MSR_TAG_TITLE;
  lineNo = 0;
  runNo = -1;
  // read msr-file
  while (!fin.eof()) {

    // read a line
    getline(fin, line);
    str = line.c_str();
    lineNo++;

    // check for tag
    if (str.BeginsWith("FITPARAMETER")) { // FITPARAMETER block tag
      tag = MSR_TAG_FITPARAMETER;
    } else if (str.BeginsWith("THEORY")) {       // THEORY block tag
      tag = MSR_TAG_THEORY;
      fout << str.Data() << std::endl;
      continue;
    } else if (str.BeginsWith("FUNCTIONS")) {    // FUNCTIONS block tag
      tag = MSR_TAG_FUNCTIONS;
      fout << str.Data() << std::endl;
      continue;
    } else if (str.BeginsWith("GLOBAL")) {       // GLOBAL block tag
      tag = MSR_TAG_GLOBAL;
      fout << str.Data() << std::endl;
      continue;
    } else if (str.BeginsWith("RUN")) {          // RUN block tag
      tag = MSR_TAG_RUN;
      runNo++;

      addT0Counter = 0; // reset counter
    } else if (str.BeginsWith("COMMANDS")) {     // COMMANDS block tag
      tag = MSR_TAG_COMMANDS;
      fout << str.Data() << std::endl;
      continue;
    } else if (str.BeginsWith("FOURIER")) {      // FOURIER block tag
      tag = MSR_TAG_FOURIER;
      fout << str.Data() << std::endl;
      continue;
    } else if (str.BeginsWith("PLOT")) {         // PLOT block tag
      tag = MSR_TAG_PLOT;
      plotNo++;
    } else if (str.BeginsWith("STATISTIC")) {    // STATISTIC block tag
      tag = MSR_TAG_STATISTIC;
    }

    // handle blocks
    switch (tag) {
      case MSR_TAG_TITLE:
        if (lineNo == 1)
          fout << fTitle.Data() << std::endl;
        else
          fout << str.Data() << std::endl;
        break;
      case MSR_TAG_FITPARAMETER:
        tokens = str.Tokenize(" \t");
        if (tokens->GetEntries() == 0) { // not a parameter line
          fout << str.Data() << std::endl;
        } else {
          ostr = dynamic_cast<TObjString*>(tokens->At(0));
          sstr = ostr->GetString();
          if (sstr.IsDigit()) { // parameter
            number = sstr.Atoi();
            number--;
            // make sure number makes sense
            assert ((number >= 0) && (number < (Int_t)fParam.size()));
            // parameter no
            fout.width(9);
            fout << std::right << fParam[number].fNo;
            fout << " ";
            // parameter name
            fout.width(11);
            fout << std::left << fParam[number].fName.Data();
            fout << " ";
            // value of the parameter
            if (fParam[number].fStep == 0.0) // if fixed parameter take all significant digits
              neededPrec = LastSignificant(fParam[number].fValue);
            else // step/neg.error given hence they will limited the output precission of the value
              neededPrec = NeededPrecision(fParam[number].fStep)+1;
            if ((fParam[number].fStep != 0.0) && fParam[number].fPosErrorPresent && (NeededPrecision(fParam[number].fPosError)+1 > neededPrec))
              neededPrec = NeededPrecision(fParam[number].fPosError)+1;
            if (neededPrec < 6)
              neededWidth = 9;
            else
              neededWidth = neededPrec + 3;
            fout.width(neededWidth);
            fout.setf(std::ios::fixed, std::ios::floatfield);
            fout.precision(neededPrec);
            fout << std::left << fParam[number].fValue;
            fout << " ";
            // value of step/error/neg.error
            fout.width(11);
            fout.setf(std::ios::fixed);
            if (fParam[number].fStep == 0.0)
              neededPrec = 0;
            fout.precision(neededPrec);
            fout << std::left << fParam[number].fStep;
            fout << " ";
            fout.width(11);
            fout.setf(std::ios::fixed);
            fout.precision(neededPrec);
            if ((fParam[number].fNoOfParams == 5) || (fParam[number].fNoOfParams == 7)) // pos. error given
              if (fParam[number].fPosErrorPresent && (fParam[number].fStep != 0)) // pos error is a number
                fout << std::left << fParam[number].fPosError;
            else // pos error is a none
              fout << std::left << "none";
            else // no pos. error
              fout << std::left << "none";
            fout << " ";
            fout.unsetf(std::ios::floatfield);
            // boundaries
            if (fParam[number].fNoOfParams > 5) {
              fout.width(7);
              fout.precision(prec);
              if (fParam[number].fLowerBoundaryPresent)
                fout << std::left << fParam[number].fLowerBoundary;
              else
                fout << std::left << "none";
              fout << " ";
              fout.width(7);
              fout.precision(prec);
              if (fParam[number].fUpperBoundaryPresent)
                fout << std::left << fParam[number].fUpperBoundary;
              else
                fout << std::left << "none";
              fout << " ";
            }
            fout << std::endl;
          } else { // not a parameter, hence just copy it
            fout << str.Data() << std::endl;
          }
          // clean up tokens
          delete tokens;
        }
        break;
      case MSR_TAG_THEORY:
        found = false;
        for (UInt_t i=0; i<fTheory.size(); i++) {
          if (fTheory[i].fLineNo == lineNo) {
            fout << fTheory[i].fLine.Data() << std::endl;
            found = true;
          }
        }
        if (!found) {
          fout << str.Data() << std::endl;
        }
        break;
      case MSR_TAG_FUNCTIONS:
        sstr = str;
        sstr.Remove(TString::kLeading, ' ');
        if (str.BeginsWith("fun")) {
          if (FilterNumber(sstr, "fun", 0, number)) {
            idx = GetFuncIndex(number); // get index of the function number
            sstr = fFuncHandler->GetFuncString(idx);
            sstr.ToLower();
            fout << sstr.Data() << std::endl;
          }
        } else {
          fout << str.Data() << std::endl;
        }
        break;
      case MSR_TAG_GLOBAL:
        sstr = str;
        if (sstr.BeginsWith("fittype")) {
          fout.width(16);
          switch (fGlobal.GetFitType()) {
            case MSR_FITTYPE_SINGLE_HISTO:
              fout << std::left << "fittype" << MSR_FITTYPE_SINGLE_HISTO << "         (single histogram fit)" << std::endl;
              break;
            case MSR_FITTYPE_SINGLE_HISTO_RRF:
              fout << std::left << "fittype" << MSR_FITTYPE_SINGLE_HISTO_RRF << "         (single histogram RRF fit)" << std::endl;
              break;
            case MSR_FITTYPE_ASYM:
              fout << std::left << "fittype" << MSR_FITTYPE_ASYM << "         (asymmetry fit)" << std::endl ;
              break;
            case MSR_FITTYPE_ASYM_RRF:
              fout << std::left << "fittype" << MSR_FITTYPE_ASYM_RRF << "         (asymmetry RRF fit)" << std::endl ;
              break;
            case MSR_FITTYPE_MU_MINUS:
              fout << std::left << "fittype" << MSR_FITTYPE_MU_MINUS << "         (mu minus fit)" << std::endl ;
              break;
            case MSR_FITTYPE_BNMR:
              fout << std::left << "fittype" << MSR_FITTYPE_BNMR << "         (beta-NMR fit)" << std::endl ;
              break;
            case MSR_FITTYPE_NON_MUSR:
              fout << std::left << "fittype" << MSR_FITTYPE_NON_MUSR << "         (non muSR fit)" << std::endl ;
              break;
            default:
              break;
          }
        } else if (sstr.BeginsWith("rrf_freq", TString::kIgnoreCase) && (fGlobal.GetFitType() == MSR_FITTYPE_SINGLE_HISTO_RRF)) {
          fout.width(16);
          fout << std::left << "rrf_freq ";
          fout.width(8);
          neededPrec = LastSignificant(fGlobal.GetRRFFreq(fGlobal.GetRRFUnit().Data()),10);
          fout.precision(neededPrec);
          fout << std::left << std::fixed << fGlobal.GetRRFFreq(fGlobal.GetRRFUnit().Data());
          fout << " " << fGlobal.GetRRFUnit();
          fout << std::endl;
        } else if (sstr.BeginsWith("rrf_phase", TString::kIgnoreCase) && (fGlobal.GetFitType() == MSR_FITTYPE_SINGLE_HISTO_RRF)) {
          fout.width(16);
          fout << "rrf_phase ";
          fout.width(8);
          fout << std::left << fGlobal.GetRRFPhase();
          fout << std::endl;
        } else if (sstr.BeginsWith("rrf_packing", TString::kIgnoreCase) && (fGlobal.GetFitType() == MSR_FITTYPE_SINGLE_HISTO_RRF)) {
          fout.width(16);
          fout << "rrf_packing ";
          fout.width(8);
          fout << std::left << fGlobal.GetRRFPacking();
          fout << std::endl;
        } else if (sstr.BeginsWith("data")) {
          fout.width(16);
          fout << std::left << "data";
          for (UInt_t j=0; j<4; j++) {
            if (fGlobal.GetDataRange(j) > 0) {
              fout.width(8);
              fout << std::left << fGlobal.GetDataRange(j);
            }
          }
          fout << std::endl;
        } else if (sstr.BeginsWith("t0")) {
          fout.width(16);
          fout << std::left << "t0";
          for (UInt_t j=0; j<fGlobal.GetT0BinSize(); j++) {
            fout.width(8);
            fout.precision(1);
            fout.setf(std::ios::fixed,std::ios::floatfield);
            fout << std::left << fGlobal.GetT0Bin(j);
          }
          fout << std::endl;
        } else if (sstr.BeginsWith("addt0")) {
          fout.width(16);
          fout << std::left << "addt0";
          for (Int_t j=0; j<fGlobal.GetAddT0BinSize(addT0GlobalCounter); j++) {
            fout.width(8);
            fout.precision(1);
            fout.setf(std::ios::fixed,std::ios::floatfield);
            fout << std::left << fGlobal.GetAddT0Bin(addT0GlobalCounter, j);
          }
          fout << std::endl;
          addT0GlobalCounter++;
        } else if (sstr.BeginsWith("fit")) {
          fout.width(16);
          fout << std::left << "fit";
          if (fGlobal.IsFitRangeInBin()) { // fit range given in bins
            fout << "fgb";
            if (fGlobal.GetFitRangeOffset(0) > 0)
              fout << "+" << fGlobal.GetFitRangeOffset(0);
            fout << "   lgb";
            if (fGlobal.GetFitRangeOffset(1) > 0)
              fout << "-" << fGlobal.GetFitRangeOffset(1);
            neededPrec = LastSignificant(fGlobal.GetFitRange(0));
            if (LastSignificant(fGlobal.GetFitRange(1)) > neededPrec)
              neededPrec = LastSignificant(fGlobal.GetFitRange(1));
            fout.precision(neededPrec);
            fout << "  # in time: " << fGlobal.GetFitRange(0) << ".." << fGlobal.GetFitRange(1) << " (usec)";
          } else { // fit range given in time
            for (UInt_t j=0; j<2; j++) {
              if (fGlobal.GetFitRange(j) == -1)
                break;
              neededWidth = 7;
              neededPrec = LastSignificant(fGlobal.GetFitRange(j));
              fout.width(neededWidth);
              fout.precision(neededPrec);
              fout << std::left << std::fixed << fGlobal.GetFitRange(j);
              if (j==0)
                fout << " ";
            }
          }
          fout << std::endl;
        } else if (sstr.BeginsWith("packing")) {
          fout.width(16);
          fout << std::left << "packing";
          fout << fGlobal.GetPacking() << std::endl;
        } else {
          fout << str.Data() << std::endl;
        }
        break;
      case MSR_TAG_RUN:
        sstr = str;
        sstr.Remove(TString::kLeading, ' ');
        if (sstr.BeginsWith("RUN")) {
          addRunNo = 0; // reset counter
          fout << "RUN " << fRuns[runNo].GetRunName()->Data() << " ";
          pstr = fRuns[runNo].GetBeamline();
          if (pstr == nullptr) {
            std::cerr << std::endl << ">> PMsrHandler::WriteMsrLogFile: **ERROR** Couldn't obtain beamline data." << std::endl;
            assert(0);
          }
          pstr->ToUpper();
          fout << pstr->Data() << " ";
          pstr = fRuns[runNo].GetInstitute();
          if (pstr == nullptr) {
            std::cerr << std::endl << ">> PMsrHandler::WriteMsrLogFile: **ERROR** Couldn't obtain institute data." << std::endl;
            assert(0);
          }
          pstr->ToUpper();
          fout << pstr->Data() << " ";
          pstr = fRuns[runNo].GetFileFormat();
          if (pstr == nullptr) {
            std::cerr << std::endl << ">> PMsrHandler::WriteMsrLogFile: **ERROR** Couldn't obtain file format data." << std::endl;
            assert(0);
          }
          pstr->ToUpper();
          fout << pstr->Data() << "   (name beamline institute data-file-format)" << std::endl;
        } else if (sstr.BeginsWith("ADDRUN")) {
          addRunNo++;
          fout << "ADDRUN " << fRuns[runNo].GetRunName(addRunNo)->Data() << " ";
          pstr = fRuns[runNo].GetBeamline(addRunNo);
          if (pstr == nullptr) {
            std::cerr << std::endl << ">> PMsrHandler::WriteMsrLogFile: **ERROR** Couldn't obtain beamline data (addrun)." << std::endl;
            assert(0);
          }
          pstr->ToUpper();
          fout << pstr->Data() << " ";
          pstr = fRuns[runNo].GetInstitute(addRunNo);
          if (pstr == nullptr) {
            std::cerr << std::endl << ">> PMsrHandler::WriteMsrLogFile: **ERROR** Couldn't obtain institute data (addrun)." << std::endl;
            assert(0);
          }
          pstr->ToUpper();
          fout << pstr->Data() << " ";
          pstr = fRuns[runNo].GetFileFormat(addRunNo);
          if (pstr == nullptr) {
            std::cerr << std::endl << ">> PMsrHandler::WriteMsrLogFile: **ERROR** Couldn't obtain file format data (addrun)." << std::endl;
            assert(0);
          }
          pstr->ToUpper();
          fout << pstr->Data() << "   (name beamline institute data-file-format)" << std::endl;
        } else if (sstr.BeginsWith("fittype")) {
          fout.width(16);
          switch (fRuns[runNo].GetFitType()) {
          case MSR_FITTYPE_SINGLE_HISTO:
            fout << std::left << "fittype" << MSR_FITTYPE_SINGLE_HISTO << "         (single histogram fit)" << std::endl;
            break;
          case MSR_FITTYPE_SINGLE_HISTO_RRF:
            fout << std::left << "fittype" << MSR_FITTYPE_SINGLE_HISTO_RRF << "         (single histogram RRF fit)" << std::endl;
            break;
          case MSR_FITTYPE_ASYM:
            fout << std::left << "fittype" << MSR_FITTYPE_ASYM << "         (asymmetry fit)" << std::endl ;
            break;
          case MSR_FITTYPE_ASYM_RRF:
            fout << std::left << "fittype" << MSR_FITTYPE_ASYM_RRF << "         (asymmetry RRF fit)" << std::endl ;
            break;
          case MSR_FITTYPE_MU_MINUS:
            fout << std::left << "fittype" << MSR_FITTYPE_MU_MINUS << "         (mu minus fit)" << std::endl ;
            break;
          case MSR_FITTYPE_BNMR:
            fout << std::left << "fittype" << MSR_FITTYPE_BNMR << "         (beta-NMR fit)" << std::endl ;
            break;
          case MSR_FITTYPE_NON_MUSR:
            fout << std::left << "fittype" << MSR_FITTYPE_NON_MUSR << "         (non muSR fit)" << std::endl ;
            break;
          default:
            break;
          }
        } else if (sstr.BeginsWith("alpha ")) {
          fout.width(16);
          fout << std::left << "alpha";
          // check of alpha is given as a function
          if (fRuns[runNo].GetAlphaParamNo() >= MSR_PARAM_FUN_OFFSET)
            fout << "fun" << fRuns[runNo].GetAlphaParamNo()-MSR_PARAM_FUN_OFFSET;
          else
            fout << fRuns[runNo].GetAlphaParamNo();
          fout << std::endl;
        } else if (sstr.BeginsWith("beta ")) {
          fout.width(16);
          fout << std::left << "beta";
          if (fRuns[runNo].GetBetaParamNo() >= MSR_PARAM_FUN_OFFSET)
            fout << "fun" << fRuns[runNo].GetBetaParamNo()-MSR_PARAM_FUN_OFFSET;
          else
            fout << fRuns[runNo].GetBetaParamNo();
          fout << std::endl;
        } else if (sstr.BeginsWith("norm")) {
          fout.width(16);
          fout << std::left << "norm";
          // check if norm is given as a function
          if (fRuns[runNo].GetNormParamNo() >= MSR_PARAM_FUN_OFFSET)
            fout << "fun" << fRuns[runNo].GetNormParamNo()-MSR_PARAM_FUN_OFFSET;
          else
            fout << fRuns[runNo].GetNormParamNo();
          fout << std::endl;
        } else if (sstr.BeginsWith("backgr.fit")) {
          fout.width(16);
          fout << std::left << "backgr.fit";
          fout << fRuns[runNo].GetBkgFitParamNo() << std::endl;
        } else if (sstr.BeginsWith("lifetime ")) {
          fout.width(16);
          fout << std::left << "lifetime";
          fout << fRuns[runNo].GetLifetimeParamNo() << std::endl;
        } else if (sstr.BeginsWith("lifetimecorrection")) {
          // obsolate, hence do nothing here
        } else if (sstr.BeginsWith("map")) {
          fout << "map         ";
          for (UInt_t j=0; j<fRuns[runNo].GetMap()->size(); j++) {
            fout.width(5);
            fout << std::right << fRuns[runNo].GetMap(j);
          }
          // if there are less maps then 10 fill with zeros
          if (fRuns[runNo].GetMap()->size() < 10) {
            for (UInt_t j=fRuns[runNo].GetMap()->size(); j<10; j++)
              fout << "    0";
          }
          fout << std::endl;
        } else if (sstr.BeginsWith("forward")) {
          if (fRuns[runNo].GetForwardHistoNoSize() == 0) {
            std::cerr << std::endl << ">> PMsrHandler::WriteMsrLogFile: **WARNING** 'forward' tag without any data found!";
            std::cerr << std::endl << ">> Something is VERY fishy, please check your msr-file carfully." << std::endl;
          } else {
            TString result("");
            PIntVector forward;
            for (UInt_t i=0; i<fRuns[runNo].GetForwardHistoNoSize(); i++)
              forward.push_back(fRuns[runNo].GetForwardHistoNo(i));
            MakeDetectorGroupingString("forward", forward, result);
            forward.clear();
            fout << result.Data() << std::endl;
          }
        } else if (sstr.BeginsWith("backward")) {
          if (fRuns[runNo].GetBackwardHistoNoSize() == 0) {
            std::cerr << std::endl << ">> PMsrHandler::WriteMsrLogFile: **WARNING** 'backward' tag without any data found!";
            std::cerr << std::endl << ">> Something is VERY fishy, please check your msr-file carfully." << std::endl;
          } else {
            TString result("");
            PIntVector backward;
            for (UInt_t i=0; i<fRuns[runNo].GetBackwardHistoNoSize(); i++)
              backward.push_back(fRuns[runNo].GetBackwardHistoNo(i));
            MakeDetectorGroupingString("backward", backward, result);
            backward.clear();
            fout << result.Data() << std::endl;
          }
        } else if (sstr.BeginsWith("backgr.fix")) {
          fout.width(16);
          fout << std::left << "backgr.fix";
          for (UInt_t j=0; j<2; j++) {
            if (fRuns[runNo].GetBkgFix(j) != PMUSR_UNDEFINED) {
              fout.precision(prec);
              fout.width(12);
              fout << std::left << fRuns[runNo].GetBkgFix(j);
            }
          }
          fout << std::endl;
        } else if (sstr.BeginsWith("background")) {
          backgroundTagMissing[runNo] = false;
          fout.width(16);
          fout << std::left << "background";
          for (UInt_t j=0; j<4; j++) {
            if (fRuns[runNo].GetBkgRange(j) > 0) {
              fout.width(8);
              fout << std::left << fRuns[runNo].GetBkgRange(j);
            }
          }
          if (fRuns[runNo].GetBkgEstimated(0) != PMUSR_UNDEFINED) {
            Int_t precision=4;
            if ((Int_t)log10(fRuns[runNo].GetBkgEstimated(0))+1 >= 4)
              precision = 2;
            fout << "   # estimated bkg: ";
            fout << std::fixed;
            fout.precision(precision);
            fout << fRuns[runNo].GetBkgEstimated(0);
            if (fRuns[runNo].GetBkgEstimated(1) != PMUSR_UNDEFINED) {
              fout << " / ";
              fout << std::fixed;
              fout.precision(precision);
              fout << fRuns[runNo].GetBkgEstimated(1);
            }
          }
          fout << std::endl;
        } else if (sstr.BeginsWith("data")) {
          dataTagMissing[runNo] = false;
          fout.width(16);
          fout << std::left << "data";
          for (UInt_t j=0; j<4; j++) {
            if (fRuns[runNo].GetDataRange(j) > 0) {
              fout.width(8);
              fout << std::left << fRuns[runNo].GetDataRange(j);
            }
          }
          fout << std::endl;
        } else if (sstr.BeginsWith("t0")) {
          t0TagMissing[runNo] = false;
          fout.width(16);          
          fout << std::left << "t0";
          for (UInt_t j=0; j<fRuns[runNo].GetT0BinSize(); j++) {
            fout.width(8);
            fout.precision(1);
            fout.setf(std::ios::fixed,std::ios::floatfield);
            fout << std::left << fRuns[runNo].GetT0Bin(j);
          }
          fout << std::endl;
        } else if (sstr.BeginsWith("addt0")) {          
          addt0TagMissing[runNo][addT0Counter] = false;
          if (fRuns[runNo].GetAddT0BinSize(addT0Counter) <=0) {
            std::cerr << std::endl << ">> PMsrHandler::WriteMsrLogFile: **WARNING** 'addt0' tag without any data found!";
            std::cerr << std::endl << ">> Something is VERY fishy, please check your msr-file carfully." << std::endl;
          } else {
            fout.width(16);
            fout << std::left << "addt0";
            for (Int_t j=0; j<fRuns[runNo].GetAddT0BinSize(addT0Counter); j++) {
              fout.width(8);
              fout.precision(1);
              fout.setf(std::ios::fixed,std::ios::floatfield);
              fout << std::left << fRuns[runNo].GetAddT0Bin(addT0Counter, j);
            }
            fout << std::endl;
            addT0Counter++;
          }
        } else if (sstr.BeginsWith("xy-data")) {
          if (fRuns[runNo].GetXDataIndex() != -1) { // indices
            fout.width(16);
            fout << std::left << "xy-data";
            fout.width(8);
            fout.precision(2);
            fout << std::left << std::fixed << fRuns[runNo].GetXDataIndex();
            fout.width(8);
            fout.precision(2);
            fout << std::left << std::fixed << fRuns[runNo].GetYDataIndex();
            fout << std::endl;
          } else if (!fRuns[runNo].GetXDataLabel()->IsWhitespace()) { // labels
            fout.width(16);
            fout << std::left << "xy-data";
            fout.width(8);
            fout << std::left << std::fixed << fRuns[runNo].GetXDataLabel()->Data();
            fout << " ";
            fout.width(8);
            fout << std::left << std::fixed << fRuns[runNo].GetYDataLabel()->Data();
            fout << std::endl;
          }
        } else if (sstr.BeginsWith("fit")) {
          // check if missing t0/addt0/background/data tag are present eventhough the values are present, if so write these data values
          // if ISIS data, do not do anything
          if (t0TagMissing[runNo] && fRuns[runNo].GetInstitute()->CompareTo("isis", TString::kIgnoreCase)) {
            if (fRuns[runNo].GetT0BinSize() > 0) {
              fout.width(16);
              fout << std::left << "t0";
              for (UInt_t j=0; j<fRuns[runNo].GetT0BinSize(); j++) {
                fout.width(8);
                fout.precision(1);
                fout.setf(std::ios::fixed,std::ios::floatfield);
                fout << std::left << fRuns[runNo].GetT0Bin(j);
              }
              fout << std::endl;
            }
          }
          for (UInt_t i=0; i<fRuns[runNo].GetAddT0BinEntries(); i++) {
            if (addt0TagMissing[runNo][i]  && fRuns[runNo].GetInstitute()->CompareTo("isis", TString::kIgnoreCase)) {
              if (fRuns[runNo].GetAddT0BinSize(i) > 0) {
                fout.width(16);
                fout << std::left << "addt0";
                for (Int_t j=0; j<fRuns[runNo].GetAddT0BinSize(i); j++) {
                  fout.width(8);
                  fout.precision(1);
                  fout.setf(std::ios::fixed,std::ios::floatfield);
                  fout << std::left << fRuns[runNo].GetAddT0Bin(i, j);
                }
                fout << std::endl;
              }
            }
          }
          if (backgroundTagMissing[runNo]) {
            if (fRuns[runNo].GetBkgRange(0) >= 0) {
              fout.width(16);
              fout << std::left << "background";
              for (UInt_t j=0; j<4; j++) {
                if (fRuns[runNo].GetBkgRange(j) > 0) {
                  fout.width(8);
                  fout << std::left << fRuns[runNo].GetBkgRange(j);
                }
              }
              fout << std::endl;
            }
          }
          if (dataTagMissing[runNo]) {
            if (fRuns[runNo].GetDataRange(0) >= 0) {
              fout.width(16);
              fout << std::left << "data";
              for (UInt_t j=0; j<4; j++) {
                if (fRuns[runNo].GetDataRange(j) > 0) {
                  fout.width(8);
                  fout << std::left << fRuns[runNo].GetDataRange(j);
                }
              }
              fout << std::endl;
            }
          }
          // write fit range line
          fout.width(16);
          fout << std::left << "fit";
          if (fRuns[runNo].IsFitRangeInBin()) { // fit range given in bins
            fout << "fgb";
            if (fRuns[runNo].GetFitRangeOffset(0) > 0)
              fout << "+" << fRuns[runNo].GetFitRangeOffset(0);
            fout << "   lgb";
            if (fRuns[runNo].GetFitRangeOffset(1) > 0)
              fout << "-" << fRuns[runNo].GetFitRangeOffset(1);
            neededPrec = LastSignificant(fRuns[runNo].GetFitRange(0));
            if (LastSignificant(fRuns[runNo].GetFitRange(1)) > neededPrec)
              neededPrec = LastSignificant(fRuns[runNo].GetFitRange(1));
            fout.precision(neededPrec);
            fout << "  # in time: " << fRuns[runNo].GetFitRange(0) << ".." << fRuns[runNo].GetFitRange(1) << " (usec)";
          } else { // fit range given in time
            for (UInt_t j=0; j<2; j++) {
              if (fRuns[runNo].GetFitRange(j) == -1)
                break;
              neededWidth = 7;
              neededPrec = LastSignificant(fRuns[runNo].GetFitRange(j));
              fout.width(neededWidth);
              fout.precision(neededPrec);
              fout << std::left << std::fixed << fRuns[runNo].GetFitRange(j);
              if (j==0)
                fout << " ";
            }
          }
          fout << std::endl;
        } else if (sstr.BeginsWith("packing")) {
          fout.width(16);
          fout << std::left << "packing";
          fout << fRuns[runNo].GetPacking() << std::endl;
        } else {
          fout << str.Data() << std::endl;
        }
        break;
      case MSR_TAG_COMMANDS:
        fout << str.Data() << std::endl;
        break;
      case MSR_TAG_FOURIER:
        sstr = str;
        sstr.Remove(TString::kLeading, ' ');
        if (sstr.BeginsWith("units")) {
          fout << "units            ";
          if (fFourier.fUnits == FOURIER_UNIT_GAUSS) {
            fout << "Gauss";
          } else if (fFourier.fUnits == FOURIER_UNIT_TESLA) {
            fout << "Tesla";
          } else if (fFourier.fUnits == FOURIER_UNIT_FREQ) {
            fout << "MHz ";
          } else if (fFourier.fUnits == FOURIER_UNIT_CYCLES) {
            fout << "Mc/s";
          }
          fout << "   # units either 'Gauss', 'Tesla', 'MHz', or 'Mc/s'";
          fout << std::endl;
        } else if (sstr.BeginsWith("fourier_power")) {
          fout << "fourier_power    " << fFourier.fFourierPower << std::endl;
        } else if (sstr.BeginsWith("dc-corrected")) {
          fout << "dc-corrected     ";
          if (fFourier.fDCCorrected == true)
            fout << "true" << std::endl;
          else
            fout << "false" << std::endl;
        } else if (sstr.BeginsWith("apodization")) {
          fout << "apodization      ";
          if (fFourier.fApodization == FOURIER_APOD_NONE) {
            fout << "NONE  ";
          } else if (fFourier.fApodization == FOURIER_APOD_WEAK) {
            fout << "WEAK  ";
          } else if (fFourier.fApodization == FOURIER_APOD_MEDIUM) {
            fout << "MEDIUM";
          } else if (fFourier.fApodization == FOURIER_APOD_STRONG) {
            fout << "STRONG";
          }
          fout << "  # NONE, WEAK, MEDIUM, STRONG";
          fout << std::endl;
        } else if (sstr.BeginsWith("plot")) {
          fout << "plot             ";
          if (fFourier.fPlotTag == FOURIER_PLOT_REAL) {
            fout << "REAL ";
          } else if (fFourier.fPlotTag == FOURIER_PLOT_IMAG) {
            fout << "IMAG ";
          } else if (fFourier.fPlotTag == FOURIER_PLOT_REAL_AND_IMAG) {
            fout << "REAL_AND_IMAG";
          } else if (fFourier.fPlotTag == FOURIER_PLOT_POWER) {
            fout << "POWER";
          } else if (fFourier.fPlotTag == FOURIER_PLOT_PHASE) {
            fout << "PHASE";
          } else if (fFourier.fPlotTag == FOURIER_PLOT_PHASE_OPT_REAL) {
            fout << "PHASE_OPT_REAL";
          }
          fout << "   # REAL, IMAG, REAL_AND_IMAG, POWER, PHASE, PHASE_OPT_REAL";
          fout << std::endl;
        } else if (sstr.BeginsWith("phase")) {
          if (fFourier.fPhaseParamNo.size() > 0) {
            TString phaseParamStr = BeautifyFourierPhaseParameterString();
            fout << "phase            " << phaseParamStr << std::endl;
          } else if (fFourier.fPhase.size() > 0) {
            fout << "phase            ";
            for (UInt_t i=0; i<fFourier.fPhase.size()-1; i++) {
              fout << fFourier.fPhase[i] << ", ";
            }
            fout << fFourier.fPhase[fFourier.fPhase.size()-1] << std::endl;
          }
        } else if (sstr.BeginsWith("range_for_phase_correction")) {
          fout << "range_for_phase_correction  " << fFourier.fRangeForPhaseCorrection[0] << "    " << fFourier.fRangeForPhaseCorrection[1] << std::endl;
        } else if (sstr.BeginsWith("range ")) {
          fout.setf(std::ios::fixed,std::ios::floatfield);
          neededPrec = LastSignificant(fFourier.fPlotRange[0]);
          if (LastSignificant(fFourier.fPlotRange[1]) > neededPrec)
            neededPrec = LastSignificant(fFourier.fPlotRange[1]);
          fout.precision(neededPrec);
          fout << "range            " << fFourier.fPlotRange[0] << "    " << fFourier.fPlotRange[1] << std::endl;
        } else {
          fout << str.Data() << std::endl;
        }
        break;
      case MSR_TAG_PLOT:
        sstr = str;
        sstr.Remove(TString::kLeading, ' ');
        if (sstr.BeginsWith("PLOT")) {
          switch (fPlots[plotNo].fPlotType) {
          case MSR_PLOT_SINGLE_HISTO:
            fout << "PLOT " << fPlots[plotNo].fPlotType << "   (single histo plot)" << std::endl;
            break;
          case MSR_PLOT_SINGLE_HISTO_RRF:
            fout << "PLOT " << fPlots[plotNo].fPlotType << "   (single histo RRF plot)" << std::endl;
            break;
          case MSR_PLOT_ASYM:
            fout << "PLOT " << fPlots[plotNo].fPlotType << "   (asymmetry plot)" << std::endl;
            break;
          case MSR_PLOT_ASYM_RRF:
            fout << "PLOT " << fPlots[plotNo].fPlotType << "   (asymmetry RRF plot)" << std::endl;
            break;
          case MSR_PLOT_MU_MINUS:
            fout << "PLOT " << fPlots[plotNo].fPlotType << "   (mu minus plot)" << std::endl;
            break;
          case MSR_PLOT_BNMR:
            fout << "PLOT " << fPlots[plotNo].fPlotType << "   (beta-NMR asymmetry plot)" << std::endl;
            break;
          case MSR_PLOT_NON_MUSR:
            fout << "PLOT " << fPlots[plotNo].fPlotType << "   (non muSR plot)" << std::endl;
            break;
          default:
            break;
          }
          if (fPlots[plotNo].fLifeTimeCorrection) {
            fout << "lifetimecorrection" << std::endl;
          }
        } else if (sstr.BeginsWith("lifetimecorrection")) {
          // do nothing, since it is already handled in the lines above.
          // The reason why this handled that oddly is due to legacy issues
          // of this flag, i.e. transfer from RUN -> PLOT
        } else if (sstr.BeginsWith("runs")) {
          fout << "runs     ";
          fout.precision(0);
          for (UInt_t j=0; j<fPlots[plotNo].fRuns.size(); j++) {
            fout.width(4);
            fout << fPlots[plotNo].fRuns[j];
          }
          fout << std::endl;
        } else if (sstr.BeginsWith("range")) {
          fout << "range    ";
          neededPrec = LastSignificant(fPlots[plotNo].fTmin[0]);
          fout.precision(neededPrec);
          fout << fPlots[plotNo].fTmin[0];
          fout << "   ";
          neededPrec = LastSignificant(fPlots[plotNo].fTmax[0]);
          fout.precision(neededPrec);
          fout << fPlots[plotNo].fTmax[0];
          if (fPlots[plotNo].fYmin.size() > 0) {
            fout << "   ";
            neededPrec = LastSignificant(fPlots[plotNo].fYmin[0]);
            fout.precision(neededPrec);
            fout << fPlots[plotNo].fYmin[0] << "   ";
            neededPrec = LastSignificant(fPlots[plotNo].fYmax[0]);
            fout.precision(neededPrec);
            fout << fPlots[plotNo].fYmax[0];
          }
          fout << std::endl;
        } else {
          fout << str.Data() << std::endl;
        }
        break;
      case MSR_TAG_STATISTIC:
        statisticBlockFound = true;
        sstr = str;
        sstr.Remove(TString::kLeading, ' ');
        if (sstr.BeginsWith("STATISTIC")) {
          TDatime dt;
          fout << "STATISTIC --- " << dt.AsSQLString() << std::endl;
        } else if (sstr.BeginsWith("chisq") || sstr.BeginsWith("maxLH")) {
          partialStatisticBlockFound = false;
          if (fStatistic.fValid) { // valid fit result
            if (fStatistic.fChisq) {
              str.Form("  chisq = %.1lf, NDF = %d, chisq/NDF = %lf", fStatistic.fMin, fStatistic.fNdf, fStatistic.fMin / fStatistic.fNdf);
            } else {
              str.Form("  maxLH = %.1lf, NDF = %d, maxLH/NDF = %lf", fStatistic.fMin, fStatistic.fNdf, fStatistic.fMin / fStatistic.fNdf);
            }
            fout << str.Data() << std::endl;
            if (messages)
              std::cout << std::endl << str.Data() << std::endl;

            // check if expected chisq needs to be written
            if (fStatistic.fMinExpected != 0.0) {
              if (fStatistic.fChisq) {
                str.Form("  expected chisq = %.1lf, NDF = %d, expected chisq/NDF = %lf",
                         fStatistic.fMinExpected, fStatistic.fNdf, fStatistic.fMinExpected/fStatistic.fNdf);
              } else {
                str.Form("  expected maxLH = %.1lf, NDF = %d, expected maxLH/NDF = %lf",
                         fStatistic.fMinExpected, fStatistic.fNdf, fStatistic.fMinExpected/fStatistic.fNdf);
              }
              if (fStartupOptions) {
                if (fStartupOptions->writeExpectedChisq)
                  fout << str.Data() << std::endl;
              }
              if (messages)
                std::cout << std::endl << str.Data() << std::endl;

              for (UInt_t i=0; i<fStatistic.fMinExpectedPerHisto.size(); i++) {
                if (fStatistic.fNdfPerHisto[i] > 0) {
                  if (fStatistic.fChisq) {
                    str.Form("  run block %d: (NDF/red.chisq/red.chisq_e) = (%d/%lf/%lf)",
                             i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i], fStatistic.fMinExpectedPerHisto[i]/fStatistic.fNdfPerHisto[i]);
                  } else {
                    str.Form("  run block %d: (NDF/red.maxLH/red.maxLH_e) = (%d/%lf/%lf)",
                             i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i], fStatistic.fMinExpectedPerHisto[i]/fStatistic.fNdfPerHisto[i]);
                  }
                  if (fStartupOptions) {
                    if (fStartupOptions->writeExpectedChisq)
                      fout << str.Data() << std::endl;
                  }

                  if (messages)
                    std::cout << str.Data() << std::endl;
                }
              }
            } else if (fStatistic.fNdfPerHisto.size() > 1) { // check if per run chisq needs to be written
              for (UInt_t i=0; i<fStatistic.fNdfPerHisto.size(); i++) {
                if (fStatistic.fChisq) {
                  str.Form("  run block %d: (NDF/red.chisq) = (%d/%lf)",
                           i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i]);
                } else {
                  str.Form("  run block %d: (NDF/maxLH.chisq) = (%d/%lf)",
                           i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i]);
                }
                if (fStartupOptions) {
                  if (fStartupOptions->writeExpectedChisq)
                    fout << str.Data() << std::endl;
                }

                if (messages)
                  std::cout << str.Data() << std::endl;
              }
            }
          } else {
           fout << "*** FIT DID NOT CONVERGE ***" << std::endl;
           if (messages)
             std::cout << std::endl << "*** FIT DID NOT CONVERGE ***" << std::endl;
          }
        } else if (sstr.BeginsWith("*** FIT DID NOT CONVERGE ***")) {
          partialStatisticBlockFound = false;
          if (fStatistic.fValid) { // valid fit result
            if (fStatistic.fChisq) { // chisq
              str.Form("  chisq = %.1lf, NDF = %d, chisq/NDF = %lf", fStatistic.fMin, fStatistic.fNdf, fStatistic.fMin / fStatistic.fNdf);
            } else {
              str.Form("  maxLH = %.1lf, NDF = %d, maxLH/NDF = %lf", fStatistic.fMin, fStatistic.fNdf, fStatistic.fMin / fStatistic.fNdf);
            }
            fout << str.Data() << std::endl;
            if (messages)
              std::cout << std::endl << str.Data() << std::endl;

            // check if expected chisq needs to be written
            if (fStatistic.fMinExpected != 0.0) {
              if (fStatistic.fChisq) { // chisq
                str.Form("  expected chisq = %.1lf, NDF = %d, expected chisq/NDF = %lf",
                         fStatistic.fMinExpected, fStatistic.fNdf, fStatistic.fMinExpected/fStatistic.fNdf);
              } else {
                str.Form("  expected maxLH = %.1lf, NDF = %d, expected maxLH/NDF = %lf",
                         fStatistic.fMinExpected, fStatistic.fNdf, fStatistic.fMinExpected/fStatistic.fNdf);
              }
              if (fStartupOptions) {
                if (fStartupOptions->writeExpectedChisq)
                  fout << str.Data() << std::endl;
              }
              if (messages)
                std::cout << str.Data() << std::endl;

              for (UInt_t i=0; i<fStatistic.fMinExpectedPerHisto.size(); i++) {
                if (fStatistic.fNdfPerHisto[i] > 0) {
                  if (fStatistic.fChisq) { // chisq
                    str.Form("  run block %d: (NDF/red.chisq/red.chisq_e) = (%d/%lf/%lf)",
                             i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i], fStatistic.fMinExpectedPerHisto[i]/fStatistic.fNdfPerHisto[i]);
                  } else {
                    str.Form("  run block %d: (NDF/red.maxLH/red.maxLH_e) = (%d/%lf/%lf)",
                             i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i], fStatistic.fMinExpectedPerHisto[i]/fStatistic.fNdfPerHisto[i]);
                  }
                  if (fStartupOptions) {
                    if (fStartupOptions->writeExpectedChisq)
                      fout << str.Data() << std::endl;
                  }

                  if (messages)
                    std::cout << str.Data() << std::endl;
                }
              }
            } else if (fStatistic.fNdfPerHisto.size() > 1) { // check if per run chisq needs to be written
              for (UInt_t i=0; i<fStatistic.fNdfPerHisto.size(); i++) {
                if (fStatistic.fChisq) { // chisq
                  str.Form("  run block %d: (NDF/red.chisq) = (%d/%lf)",
                           i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i]);
                } else {
                  str.Form("  run block %d: (NDF/red.maxLH) = (%d/%lf)",
                           i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i]);
                }
                if (fStartupOptions) {
                  if (fStartupOptions->writeExpectedChisq)
                    fout << str.Data() << std::endl;
                }

                if (messages)
                  std::cout << str.Data() << std::endl;
              }
            }
          } else {
           fout << "*** FIT DID NOT CONVERGE ***" << std::endl;
           if (messages)
             std::cout << std::endl << "*** FIT DID NOT CONVERGE ***" << std::endl;
          }
        } else {
          if (str.Length() > 0) {
            sstr = str;
            sstr.Remove(TString::kLeading, ' ');
            if (!sstr.BeginsWith("expected chisq") && !sstr.BeginsWith("expected maxLH") && !sstr.BeginsWith("run block"))
              fout << str.Data() << std::endl;
          } else { // only write endl if not eof is reached. This is preventing growing msr-files, i.e. more and more empty lines at the end of the file
            if (!fin.eof())
              fout << std::endl;
          }
        }
        break;
      default:
        break;
    }
  }

  // there was no statistic block present in the msr-input-file
  if (!statisticBlockFound) {
    partialStatisticBlockFound = false;
    std::cerr << std::endl << ">> PMsrHandler::WriteMsrLogFile: **WARNING** no STATISTIC block present, will write a default one" << std::endl;
    fout << "###############################################################" << std::endl;
    TDatime dt;
    fout << "STATISTIC --- " << dt.AsSQLString() << std::endl;
    if (fStatistic.fValid) { // valid fit result
      if (fStatistic.fChisq) {
        str.Form("  chisq = %.1lf, NDF = %d, chisq/NDF = %lf", fStatistic.fMin, fStatistic.fNdf, fStatistic.fMin / fStatistic.fNdf);
      } else {
        str.Form("  maxLH = %.1lf, NDF = %d, maxLH/NDF = %lf", fStatistic.fMin, fStatistic.fNdf, fStatistic.fMin / fStatistic.fNdf);
      }
      fout << str.Data() << std::endl;
      if (messages)
        std::cout << std::endl << str.Data() << std::endl;

      // check if expected chisq needs to be written
      if (fStatistic.fMinExpected != 0.0) {
        if (fStatistic.fChisq) {
          str.Form("  expected chisq = %.1lf, NDF = %d, expected chisq/NDF = %lf",
                   fStatistic.fMinExpected, fStatistic.fNdf, fStatistic.fMinExpected/fStatistic.fNdf);
        } else {
          str.Form("  expected maxLH = %.1lf, NDF = %d, expected maxLH/NDF = %lf",
                   fStatistic.fMinExpected, fStatistic.fNdf, fStatistic.fMinExpected/fStatistic.fNdf);
        }
        if (fStartupOptions) {
          if (fStartupOptions->writeExpectedChisq)
            fout << str.Data() << std::endl;
        }
        if (messages)
          std::cout << str.Data() << std::endl;

        for (UInt_t i=0; i<fStatistic.fMinExpectedPerHisto.size(); i++) {
          if (fStatistic.fNdfPerHisto[i] > 0) {
            if (fStatistic.fChisq) {
              str.Form("  run block %d: (NDF/red.chisq/red.chisq_e) = (%d/%lf/%lf)",
                       i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i], fStatistic.fMinExpectedPerHisto[i]/fStatistic.fNdfPerHisto[i]);
            } else {
              str.Form("  run block %d: (NDF/red.maxLH/red.maxLH_e) = (%d/%lf/%lf)",
                       i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i], fStatistic.fMinExpectedPerHisto[i]/fStatistic.fNdfPerHisto[i]);
            }
            if (fStartupOptions) {
              if (fStartupOptions->writeExpectedChisq)
                fout << str.Data() << std::endl;
            }

            if (messages)
              std::cout << str.Data() << std::endl;
          }
        }
      } else if (fStatistic.fNdfPerHisto.size() > 1) { // check if per run chisq needs to be written
        for (UInt_t i=0; i<fStatistic.fNdfPerHisto.size(); i++) {
          if (fStatistic.fChisq) {
            str.Form("  run block %d: (NDF/red.chisq) = (%d/%lf)",
                     i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i]);
          } else {
            str.Form("  run block %d: (NDF/red.maxLH) = (%d/%lf)",
                     i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i]);
          }
          if (fStartupOptions) {
            if (fStartupOptions->writeExpectedChisq)
              fout << str.Data() << std::endl;
          }

          if (messages)
            std::cout << str.Data() << std::endl;
        }
      }
    }  else {
      fout << "*** FIT DID NOT CONVERGE ***" << std::endl;
      if (messages)
        std::cout << std::endl << "*** FIT DID NOT CONVERGE ***" << std::endl;
    }
  }

  // there was only a partial statistic block present in the msr-input-file
  if (partialStatisticBlockFound) {
    std::cerr << std::endl << ">> PMsrHandler::WriteMsrLogFile: **WARNING** garbage STATISTIC block present in the msr-input file.";
    std::cerr << std::endl << ">> ** WILL ADD SOME SENSIBLE STUFF, BUT YOU HAVE TO CHECK IT SINCE I AM **NOT** REMOVING THE GARBAGE! **" << std::endl;
    TDatime dt;
    fout << "STATISTIC --- " << dt.AsSQLString() << std::endl;
    if (fStatistic.fValid) { // valid fit result
      if (fStatistic.fChisq) { // chisq
        str.Form("  chisq = %.1lf, NDF = %d, chisq/NDF = %lf", fStatistic.fMin, fStatistic.fNdf, fStatistic.fMin / fStatistic.fNdf);
      } else {
        str.Form("  maxLH = %.1lf, NDF = %d, maxLH/NDF = %lf", fStatistic.fMin, fStatistic.fNdf, fStatistic.fMin / fStatistic.fNdf);
      }
      fout << str.Data() << std::endl;
      if (messages)
        std::cout << std::endl << str.Data() << std::endl;

      // check if expected chisq needs to be written
      if (fStatistic.fMinExpected != 0.0) {
        if (fStatistic.fChisq) { // chisq
          str.Form("  expected chisq = %.1lf, NDF = %d, expected chisq/NDF = %lf",
                   fStatistic.fMinExpected, fStatistic.fNdf, fStatistic.fMinExpected/fStatistic.fNdf);
        } else {
          str.Form("  expected maxLH = %.1lf, NDF = %d, expected maxLH/NDF = %lf",
                   fStatistic.fMinExpected, fStatistic.fNdf, fStatistic.fMinExpected/fStatistic.fNdf);
        }
        if (fStartupOptions) {
          if (fStartupOptions->writeExpectedChisq)
            fout << str.Data() << std::endl;
        }
        if (messages)
          std::cout << str.Data() << std::endl;

        for (UInt_t i=0; i<fStatistic.fMinExpectedPerHisto.size(); i++) {
          if (fStatistic.fNdfPerHisto[i] > 0) {
            if (fStatistic.fChisq) { // chisq
              str.Form("  run block %d: (NDF/red.chisq/red.chisq_e) =(%d/%lf/%lf)",
                       i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinExpectedPerHisto[i]/fStatistic.fNdfPerHisto[i], fStatistic.fMinExpectedPerHisto[i]/fStatistic.fNdfPerHisto[i]);
            } else {
              str.Form("  run block %d: (NDF/red.maxLH/red.maxLH_e) =(%d/%lf/%lf)",
                       i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinExpectedPerHisto[i]/fStatistic.fNdfPerHisto[i], fStatistic.fMinExpectedPerHisto[i]/fStatistic.fNdfPerHisto[i]);
            }
            if (fStartupOptions) {
              if (fStartupOptions->writeExpectedChisq)
                fout << str.Data() << std::endl;
            }

            if (messages)
              std::cout << str.Data() << std::endl;
          }
        }
      } else if (fStatistic.fNdfPerHisto.size() > 1) { // check if per run chisq needs to be written
        for (UInt_t i=0; i<fStatistic.fNdfPerHisto.size(); i++) {
          if (fStatistic.fChisq) { // chisq
            str.Form("  run block %d: (NDF/red.chisq) = (%d/%lf)",
                     i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i]);
          } else {
            str.Form("  run block %d: (NDF/red.maxLH) = (%d/%lf)",
                     i+1, fStatistic.fNdfPerHisto[i], fStatistic.fMinPerHisto[i]/fStatistic.fNdfPerHisto[i]);
          }
          if (fStartupOptions) {
            if (fStartupOptions->writeExpectedChisq)
              fout << str.Data() << std::endl;
          }

          if (messages)
            std::cout << str.Data() << std::endl;
        }
      }
    } else {
      fout << "*** FIT DID NOT CONVERGE (4) ***" << std::endl;
      if (messages)
        std::cout << std::endl << "*** FIT DID NOT CONVERGE ***" << std::endl;
    }
  }

  // close files
  fout.close();
  fin.close();

  // clean up
  t0TagMissing.clear();
  backgroundTagMissing.clear();
  dataTagMissing.clear();

  return PMUSR_SUCCESS;
}

//--------------------------------------------------------------------------
// WriteMsrFile (public)
//--------------------------------------------------------------------------
/**
 * <p>Writes a msr-file from the internal data structures
 *
 * <p><b>return:</b>
 * - PMUSR_SUCCESS everything is OK
 * - PMUSR_MSR_FILE_WRITE_ERROR msr output file couldn't be opened
 *
 * \param filename The name of the output file.
 */
Int_t PMsrHandler::WriteMsrFile(const Char_t *filename, std::map<UInt_t, TString> *commentsPAR, \
                                                        std::map<UInt_t, TString> *commentsTHE, \
                                                        std::map<UInt_t, TString> *commentsFUN, \
                                                        std::map<UInt_t, TString> *commentsRUN)
{
  const UInt_t prec = 6; // output precision for float/doubles
  const TString hline = "###############################################################";
  UInt_t i = 0;
  std::map<UInt_t, TString>::iterator iter;
  TString str, *pstr;

  // open output file for writing
  std::ofstream fout(filename);
  if (!fout) {
    return PMUSR_MSR_FILE_WRITE_ERROR;
  }

  // write TITLE
  fout << fTitle.Data() << std::endl;
  fout << hline.Data() << std::endl;

  // write FITPARAMETER block
  fout << "FITPARAMETER" << std::endl;
  fout << "#      No Name        Value     Step        Pos_Error   Boundaries" << std::endl;

  for (i = 0; i < fParam.size(); ++i) {
    if (commentsPAR) {
      iter = commentsPAR->find(i+1);
      if (iter != commentsPAR->end()) {
        fout << std::endl;
        fout << "# " << iter->second.Data() << std::endl;
        fout << std::endl;
        commentsPAR->erase(iter);
      }
    }
    // parameter no
    fout.width(9);
    fout << std::right << fParam[i].fNo;
    fout << " ";
    // parameter name
    fout.width(11);
    fout << std::left << fParam[i].fName.Data();
    fout << " ";
    // value of the parameter
    fout.width(9);
    fout.precision(prec);
    fout << std::left << fParam[i].fValue;
    fout << " ";
    // value of step/error/neg.error
    fout.width(11);
    fout.precision(prec);
    fout << std::left << fParam[i].fStep;
    fout << " ";
    fout.width(11);
    fout.precision(prec);
    if ((fParam[i].fNoOfParams == 5) || (fParam[i].fNoOfParams == 7)) // pos. error given
      if (fParam[i].fPosErrorPresent && (fParam[i].fStep != 0)) // pos error is a number
        fout << std::left << fParam[i].fPosError;
    else // pos error is a none
      fout << std::left << "none";
    else // no pos. error
      fout << std::left << "none";
    fout << " ";
    // boundaries
    if (fParam[i].fNoOfParams > 5) {
      fout.width(7);
      fout.precision(prec);
      if (fParam[i].fLowerBoundaryPresent)
        fout << std::left << fParam[i].fLowerBoundary;
      else
        fout << std::left << "none";
      fout << " ";
      fout.width(7);
      fout.precision(prec);
      if (fParam[i].fUpperBoundaryPresent)
        fout << std::left << fParam[i].fUpperBoundary;
      else
        fout << std::left << "none";
      fout << " ";
    }
    fout << std::endl;
  }
  if (commentsPAR && !commentsPAR->empty()) {
    fout << std::endl;
    for(iter = commentsPAR->begin(); iter != commentsPAR->end(); ++iter) {
      fout << "# " << iter->second.Data() << std::endl;
    }
    commentsPAR->clear();
  }
  fout << std::endl;
  fout << hline.Data() << std::endl;

  // write THEORY block
  fout << "THEORY" << std::endl;

  for (i = 1; i < fTheory.size(); ++i) {
    if (commentsTHE) {
      iter = commentsTHE->find(i);
      if (iter != commentsTHE->end()) {
        fout << std::endl;
        fout << "# " << iter->second.Data() << std::endl;
        fout << std::endl;
        commentsTHE->erase(iter);
      }
    }
    fout << fTheory[i].fLine.Data() << std::endl;
  }
  if (commentsTHE && !commentsTHE->empty()) {
    fout << std::endl;
    for(iter = commentsTHE->begin(); iter != commentsTHE->end(); ++iter) {
      fout << "# " << iter->second.Data() << std::endl;
    }
    commentsTHE->clear();
  }
  fout << std::endl;
  fout << hline.Data() << std::endl;

  // write FUNCTIONS block
  // or comment it if there is none in the data structures
  if (fFunctions.size() < 2)
    fout << "# ";
  fout << "FUNCTIONS" << std::endl;

  for (i = 1; i < fFunctions.size(); ++i) {
    if (commentsFUN) {
      iter = commentsFUN->find(i);
      if (iter != commentsFUN->end()) {
        fout << std::endl;
        fout << "# " << iter->second.Data() << std::endl;
        fout << std::endl;
        commentsFUN->erase(iter);
      }
    }
    fout << fFunctions[i].fLine.Data() << std::endl;
  }
  if (commentsFUN && !commentsFUN->empty()) {
    fout << std::endl;
    for(iter = commentsFUN->begin(); iter != commentsFUN->end(); ++iter) {
      fout << "# " << iter->second.Data() << std::endl;
    }
    commentsFUN->clear();
  }
  fout << std::endl;
  fout << hline.Data() << std::endl;

  // write GLOBAL block
  if (fGlobal.IsPresent()) {
    fout << "GLOBAL" << std::endl;

    // fittype
    if (fGlobal.GetFitType() != -1) {
      fout.width(16);
      switch (fGlobal.GetFitType()) {
      case MSR_FITTYPE_SINGLE_HISTO:
        fout << std::left << "fittype" << MSR_FITTYPE_SINGLE_HISTO << "         (single histogram fit)" << std::endl;
        break;
      case MSR_FITTYPE_SINGLE_HISTO_RRF:
        fout << std::left << "fittype" << MSR_FITTYPE_SINGLE_HISTO_RRF << "         (single histogram RRF fit)" << std::endl;
        break;
      case MSR_FITTYPE_ASYM:
        fout << std::left << "fittype" << MSR_FITTYPE_ASYM << "         (asymmetry fit)" << std::endl ;
        break;
      case MSR_FITTYPE_ASYM_RRF:
        fout << std::left << "fittype" << MSR_FITTYPE_ASYM_RRF << "         (asymmetry RRF fit)" << std::endl ;
        break;
      case MSR_FITTYPE_MU_MINUS:
        fout << std::left << "fittype" << MSR_FITTYPE_MU_MINUS << "         (mu minus fit)" << std::endl ;
        break;
      case MSR_FITTYPE_BNMR:
        fout << std::left << "fittype" << MSR_FITTYPE_BNMR << "         (beta-NMR fit)" << std::endl ;
        break;
      case MSR_FITTYPE_NON_MUSR:
        fout << std::left << "fittype" << MSR_FITTYPE_NON_MUSR << "         (non muSR fit)" << std::endl ;
        break;
      default:
        break;
      }
    }

    // RRF related stuff
    if ((fGlobal.GetRRFFreq(fGlobal.GetRRFUnit().Data()) > 0.0) && (fGlobal.GetFitType() == MSR_FITTYPE_SINGLE_HISTO_RRF)) {
      fout.width(16);
      fout << std::left << "rrf_freq ";
      fout.width(8);
      fout << std::left << fGlobal.GetRRFFreq(fGlobal.GetRRFUnit().Data());
      fout << " " << fGlobal.GetRRFUnit();
      fout << std::endl;
    }
    if ((fGlobal.GetRRFPhase() != 0.0) && (fGlobal.GetFitType() == MSR_FITTYPE_SINGLE_HISTO_RRF)) {
      fout.width(16);
      fout << "rrf_phase ";
      fout.width(8);
      fout << std::left << fGlobal.GetRRFPhase();
      fout << std::endl;
    }
    if ((fGlobal.GetRRFPacking() != -1) && (fGlobal.GetFitType() == MSR_FITTYPE_SINGLE_HISTO_RRF)) {
      fout.width(16);
      fout << "rrf_packing ";
      fout.width(8);
      fout << std::left << fGlobal.GetRRFPacking();
      fout << std::endl;
    }

    // data range
    if ((fGlobal.GetDataRange(0) != -1) || (fGlobal.GetDataRange(1) != -1) || (fGlobal.GetDataRange(2) != -1) || (fGlobal.GetDataRange(3) != -1)) {
      fout.width(16);
      fout << std::left << "data";
      for (UInt_t j=0; j<4; ++j) {
        if (fGlobal.GetDataRange(j) > 0) {
          fout.width(8);
          fout << std::left << fGlobal.GetDataRange(j);
        }
      }
      fout << std::endl;
    }

    // t0
    if (fGlobal.GetT0BinSize() > 0) {
      fout.width(16);
      fout << std::left << "t0";
      for (UInt_t j=0; j<fGlobal.GetT0BinSize(); ++j) {
        fout.width(8);
        fout.precision(1);
        fout.setf(std::ios::fixed,std::ios::floatfield);
        fout << std::left << fGlobal.GetT0Bin(j);
      }
      fout << std::endl;
    }

    // addt0
    for (UInt_t j = 0; j < fGlobal.GetAddT0BinEntries(); ++j) {
      if (fGlobal.GetAddT0BinSize(j) > 0) {
        fout.width(16);
        fout << std::left << "addt0";
        for (Int_t k=0; k<fGlobal.GetAddT0BinSize(j); ++k) {
          fout.width(8);
          fout.precision(1);
          fout.setf(std::ios::fixed,std::ios::floatfield);
          fout << std::left << fGlobal.GetAddT0Bin(j, k);
        }
        fout << std::endl;
      }
    }

    // fit range
    if ( (fGlobal.IsFitRangeInBin() && fGlobal.GetFitRangeOffset(0) != -1) ||
         (fGlobal.GetFitRange(0) != PMUSR_UNDEFINED) ) {
      fout.width(16);
      fout << std::left << "fit";
      if (fGlobal.IsFitRangeInBin()) { // fit range given in bins
        fout << "fgb";
        if (fGlobal.GetFitRangeOffset(0) > 0)
          fout << "+" << fGlobal.GetFitRangeOffset(0);
        fout << "   lgb";
        if (fGlobal.GetFitRangeOffset(1) > 0)
          fout << "-" << fGlobal.GetFitRangeOffset(1);
      } else { // fit range given in time
        for (UInt_t j=0; j<2; j++) {
          if (fGlobal.GetFitRange(j) == -1)
            break;
          UInt_t neededWidth = 7;
          UInt_t neededPrec = LastSignificant(fGlobal.GetFitRange(j));
          fout.width(neededWidth);
          fout.precision(neededPrec);
          fout << std::left << std::fixed << fGlobal.GetFitRange(j);
          if (j==0)
            fout << " ";
        }
      }
      fout << std::endl;
    }

    // packing
    if (fGlobal.GetPacking() != -1) {
      fout.width(16);
      fout << std::left << "packing";
      fout << fGlobal.GetPacking() << std::endl;
    }

    fout << std::endl << hline.Data() << std::endl;
  }

  // write RUN blocks
  for (i = 0; i < fRuns.size(); ++i) {
    if (commentsRUN) {
      iter = commentsRUN->find(i + 1);
      if (iter != commentsRUN->end()) {
        if (!i)
          fout << std::endl;
        fout << "# " << iter->second.Data() << std::endl;
        fout << std::endl;
        commentsRUN->erase(iter);
      }
    }
    fout << "RUN " << fRuns[i].GetRunName()->Data() << " ";
    pstr = fRuns[i].GetBeamline();
    if (pstr == nullptr) {
      std::cerr << std::endl << ">> PMsrHandler::WriteMsrFile: **ERROR** Couldn't obtain beamline data." << std::endl;
      assert(0);
    }
    pstr->ToUpper();
    fout << pstr->Data() << " ";
    pstr = fRuns[i].GetInstitute();
    if (pstr == nullptr) {
      std::cerr << std::endl << ">> PMsrHandler::WriteMsrFile: **ERROR** Couldn't obtain institute data." << std::endl;
      assert(0);
    }
    pstr->ToUpper();
    fout << pstr->Data() << " ";
    pstr = fRuns[i].GetFileFormat();
    if (pstr == nullptr) {
      std::cerr << std::endl << ">> PMsrHandler::WriteMsrFile: **ERROR** Couldn't obtain file format data." << std::endl;
      assert(0);
    }
    pstr->ToUpper();
    fout << pstr->Data() << "   (name beamline institute data-file-format)" << std::endl;

    // ADDRUN
    for (UInt_t j = 1; j < fRuns[i].GetRunNameSize(); ++j) {
      fout << "ADDRUN " << fRuns[i].GetRunName(j)->Data() << " ";
      pstr = fRuns[i].GetBeamline(j);
      if (pstr == nullptr) {
        std::cerr << std::endl << ">> PMsrHandler::WriteMsrFile: **ERROR** Couldn't obtain beamline data (addrun)." << std::endl;
        assert(0);
      }
      pstr->ToUpper();
      fout << pstr->Data() << " ";
      pstr = fRuns[i].GetInstitute(j);
      if (pstr == nullptr) {
        std::cerr << std::endl << ">> PMsrHandler::WriteMsrFile: **ERROR** Couldn't obtain institute data (addrun)." << std::endl;
        assert(0);
      }
      pstr->ToUpper();
      fout << pstr->Data() << " ";
      pstr = fRuns[i].GetFileFormat(j);
      if (pstr == nullptr) {
        std::cerr << std::endl << ">> PMsrHandler::WriteMsrFile: **ERROR** Couldn't obtain file format data (addrun)." << std::endl;
        assert(0);
      }
      pstr->ToUpper();
      fout << pstr->Data() << "   (name beamline institute data-file-format)" << std::endl;
    }

    // fittype
    if (fRuns[i].GetFitType() != -1) {
      fout.width(16);
      switch (fRuns[i].GetFitType()) {
      case MSR_FITTYPE_SINGLE_HISTO:
        fout << std::left << "fittype" << MSR_FITTYPE_SINGLE_HISTO << "         (single histogram fit)" << std::endl;
        break;
      case MSR_FITTYPE_SINGLE_HISTO_RRF:
        fout << std::left << "fittype" << MSR_FITTYPE_SINGLE_HISTO_RRF << "         (single histogram RRF fit)" << std::endl;
        break;
      case MSR_FITTYPE_ASYM:
        fout << std::left << "fittype" << MSR_FITTYPE_ASYM << "         (asymmetry fit)" << std::endl ;
        break;
      case MSR_FITTYPE_ASYM_RRF:
        fout << std::left << "fittype" << MSR_FITTYPE_ASYM_RRF << "         (asymmetry RRF fit)" << std::endl ;
        break;
      case MSR_FITTYPE_MU_MINUS:
        fout << std::left << "fittype" << MSR_FITTYPE_MU_MINUS << "         (mu minus fit)" << std::endl ;
        break;
      case MSR_FITTYPE_BNMR:
        fout << std::left << "fittype" << MSR_FITTYPE_BNMR << "         (beta-NMR fit)" << std::endl ;
        break;
      case MSR_FITTYPE_NON_MUSR:
        fout << std::left << "fittype" << MSR_FITTYPE_NON_MUSR << "         (non muSR fit)" << std::endl ;
        break;
      default:
        break;
      }
    }

    // alpha
    if (fRuns[i].GetAlphaParamNo() != -1) {
      fout.width(16);
      fout << std::left << "alpha";
      // check if alpha is give as a function
      if (fRuns[i].GetAlphaParamNo() >= MSR_PARAM_FUN_OFFSET)
        fout << "fun" << fRuns[i].GetAlphaParamNo()-MSR_PARAM_FUN_OFFSET;
      else
        fout << fRuns[i].GetAlphaParamNo();
      fout << std::endl;
    }

    // beta
    if (fRuns[i].GetBetaParamNo() != -1) {
      fout.width(16);
      fout << std::left << "beta";
      // check if beta is give as a function
      if (fRuns[i].GetBetaParamNo() >= MSR_PARAM_FUN_OFFSET)
        fout << "fun" << fRuns[i].GetBetaParamNo()-MSR_PARAM_FUN_OFFSET;
      else
        fout << fRuns[i].GetBetaParamNo();
      fout << std::endl;
    }

    // norm
    if (fRuns[i].GetNormParamNo() != -1) {
      fout.width(16);
      fout << std::left << "norm";
      // check if norm is give as a function
      if (fRuns[i].GetNormParamNo() >= MSR_PARAM_FUN_OFFSET)
        fout << "fun" << fRuns[i].GetNormParamNo()-MSR_PARAM_FUN_OFFSET;
      else
        fout << fRuns[i].GetNormParamNo();
      fout << std::endl;
    }

    // backgr.fit
    if (fRuns[i].GetBkgFitParamNo() != -1) {
      fout.width(16);
      fout << std::left << "backgr.fit";
      fout << fRuns[i].GetBkgFitParamNo() << std::endl;
    }

    // lifetime
    if (fRuns[i].GetLifetimeParamNo() != -1) {
      fout.width(16);
      fout << std::left << "lifetime";
      fout << fRuns[i].GetLifetimeParamNo() << std::endl;
    }

    // lifetimecorrection
    if ((fRuns[i].IsLifetimeCorrected()) && (fRuns[i].GetFitType() == MSR_FITTYPE_SINGLE_HISTO)) {
      fout << "lifetimecorrection" << std::endl;
    }

    // map
    fout << "map         ";
    for (UInt_t j=0; j<fRuns[i].GetMap()->size(); ++j) {
      fout.width(5);
      fout << std::right << fRuns[i].GetMap(j);
    }
    // if there are less maps then 10 fill with zeros
    if (fRuns[i].GetMap()->size() < 10) {
      for (UInt_t j=fRuns[i].GetMap()->size(); j<10; ++j)
        fout << "    0";
    }
    fout << std::endl;

    // forward
    if (fRuns[i].GetForwardHistoNoSize() == 0) {
      std::cerr << std::endl << ">> PMsrHandler::WriteMsrFile: **WARNING** No 'forward' data found!";
      std::cerr << std::endl << ">> Something is VERY fishy, please check your msr-file carfully." << std::endl;
    } else {
      fout.width(16);
      fout << std::left << "forward";
      for (UInt_t j=0; j<fRuns[i].GetForwardHistoNoSize(); ++j) {
        fout.width(8);
        fout << fRuns[i].GetForwardHistoNo(j);
      }
      fout << std::endl;
    }

    // backward
    if (fRuns[i].GetBackwardHistoNoSize() > 0) {
      fout.width(16);
      fout << std::left << "backward";
      for (UInt_t j=0; j<fRuns[i].GetBackwardHistoNoSize(); ++j) {
        fout.width(8);
        fout << fRuns[i].GetBackwardHistoNo(j);
      }
      fout << std::endl;
    }

    // backgr.fix
    if ((fRuns[i].GetBkgFix(0) != PMUSR_UNDEFINED) || (fRuns[i].GetBkgFix(1) != PMUSR_UNDEFINED)) {
      fout.width(15);
      fout << std::left << "backgr.fix";
      for (UInt_t j=0; j<2; ++j) {
        if (fRuns[i].GetBkgFix(j) != PMUSR_UNDEFINED) {
          fout.precision(prec);
          fout.width(12);
          fout << std::left << fRuns[i].GetBkgFix(j);
        }
      }
      fout << std::endl;
    }

    // background
    if ((fRuns[i].GetBkgRange(0) != -1) || (fRuns[i].GetBkgRange(1) != -1) || (fRuns[i].GetBkgRange(2) != -1) || (fRuns[i].GetBkgRange(3) != -1)) {
      fout.width(16);
      fout << std::left << "background";
      for (UInt_t j=0; j<4; ++j) {
        if (fRuns[i].GetBkgRange(j) > 0) {
          fout.width(8);
          fout << std::left << fRuns[i].GetBkgRange(j);
        }
      }
      fout << std::endl;
    }

    // data
    if ((fRuns[i].GetDataRange(0) != -1) || (fRuns[i].GetDataRange(1) != -1) || (fRuns[i].GetDataRange(2) != -1) || (fRuns[i].GetDataRange(3) != -1)) {
      fout.width(16);
      fout << std::left << "data";
      for (UInt_t j=0; j<4; ++j) {
        if (fRuns[i].GetDataRange(j) > 0) {
          fout.width(8);
          fout << std::left << fRuns[i].GetDataRange(j);
        }
      }
      fout << std::endl;
    }

    // t0
    if (fRuns[i].GetT0BinSize() > 0) {
      fout.width(16);
      fout << std::left << "t0";
      for (UInt_t j=0; j<fRuns[i].GetT0BinSize(); ++j) {
        fout.width(8);
        fout.precision(1);
        fout.setf(std::ios::fixed,std::ios::floatfield);
        fout << std::left << fRuns[i].GetT0Bin(j);
      }
      fout << std::endl;
    }

    // addt0
    if (fRuns[i].GetAddT0BinEntries() > 0) {
      for (UInt_t j = 0; j < fRuns[i].GetRunNameSize() - 1; ++j) {
        if (fRuns[i].GetAddT0BinSize(j) > 0) {
          fout.width(16);
          fout << std::left << "addt0";
          for (Int_t k=0; k<fRuns[i].GetAddT0BinSize(j); ++k) {
            fout.width(8);
            fout.precision(1);
            fout.setf(std::ios::fixed,std::ios::floatfield);
            fout << std::left << fRuns[i].GetAddT0Bin(j, k);
          }
          fout << std::endl;
        }
      }
    }

    // xy-data
    if (fRuns[i].GetXDataIndex() != -1) { // indices
      fout.width(16);
      fout << std::left << "xy-data";
      fout.width(8);
      fout.precision(2);
      fout << std::left << std::fixed << fRuns[i].GetXDataIndex();
      fout.width(8);
      fout.precision(2);
      fout << std::left << std::fixed << fRuns[i].GetYDataIndex();
      fout << std::endl;
    } else if (!fRuns[i].GetXDataLabel()->IsWhitespace()) { // labels
      fout.width(16);
      fout << std::left << "xy-data";
      fout.width(8);
      fout << std::left << std::fixed << fRuns[i].GetXDataLabel()->Data();
      fout << " ";
      fout.width(8);
      fout << std::left << std::fixed << fRuns[i].GetYDataLabel()->Data();
      fout << std::endl;
    }

    // fit
    if ( (fRuns[i].IsFitRangeInBin() && fRuns[i].GetFitRangeOffset(0) != -1) ||
         (fRuns[i].GetFitRange(0) != PMUSR_UNDEFINED) ) {
      fout.width(16);
      fout << std::left << "fit";
      if (fRuns[i].IsFitRangeInBin()) { // fit range given in bins
        fout << "fgb";
        if (fRuns[i].GetFitRangeOffset(0) > 0)
          fout << "+" << fRuns[i].GetFitRangeOffset(0);
        fout << "   lgb";
        if (fRuns[i].GetFitRangeOffset(1) > 0)
          fout << "-" << fRuns[i].GetFitRangeOffset(1);
      } else { // fit range given in time
        for (UInt_t j=0; j<2; j++) {
          if (fRuns[i].GetFitRange(j) == -1)
            break;
          UInt_t neededWidth = 7;
          UInt_t neededPrec = LastSignificant(fRuns[i].GetFitRange(j));
          fout.width(neededWidth);
          fout.precision(neededPrec);
          fout << std::left << std::fixed << fRuns[i].GetFitRange(j);
          if (j==0)
            fout << " ";
        }
      }
      fout << std::endl;
    }

    // packing
    if (fRuns[i].GetPacking() != -1) {
      fout.width(16);
      fout << std::left << "packing";
      fout << fRuns[i].GetPacking() << std::endl;
    }

    fout << std::endl;
  }

  if (commentsRUN && !commentsRUN->empty()) {
    for(iter = commentsRUN->begin(); iter != commentsRUN->end(); ++iter) {
      fout << "# " << iter->second.Data() << std::endl;
    }
    fout << std::endl;
    commentsRUN->clear();
  }
  fout << hline.Data() << std::endl;

  // write COMMANDS block
  fout << "COMMANDS" << std::endl;
  for (i = 0; i < fCommands.size(); ++i) {
    if (fCommands[i].fLine.BeginsWith("SET BATCH") || fCommands[i].fLine.BeginsWith("END RETURN"))
      continue;
    else
      fout << fCommands[i].fLine.Data() << std::endl;
  }
  fout << std::endl;
  fout << hline.Data() << std::endl;

  // write FOURIER block
  if (fFourier.fFourierBlockPresent) {
    fout << "FOURIER" << std::endl;

    // units
    if (fFourier.fUnits) {
      fout << "units            ";
      if (fFourier.fUnits == FOURIER_UNIT_GAUSS) {
        fout << "Gauss";
      } else if (fFourier.fUnits == FOURIER_UNIT_TESLA) {
        fout << "Tesla";
      } else if (fFourier.fUnits == FOURIER_UNIT_FREQ) {
        fout << "MHz ";
      } else if (fFourier.fUnits == FOURIER_UNIT_CYCLES) {
        fout << "Mc/s";
      }
      fout << "   # units either 'Gauss', 'Tesla', 'MHz', or 'Mc/s'";
      fout << std::endl;
    }

    // fourier_power
    if (fFourier.fFourierPower != -1) {
      fout << "fourier_power    " << fFourier.fFourierPower << std::endl;
    }

    // apodization
    if (fFourier.fApodization) {
      fout << "apodization      ";
      if (fFourier.fApodization == FOURIER_APOD_NONE) {
        fout << "NONE  ";
      } else if (fFourier.fApodization == FOURIER_APOD_WEAK) {
        fout << "WEAK  ";
      } else if (fFourier.fApodization == FOURIER_APOD_MEDIUM) {
        fout << "MEDIUM";
      } else if (fFourier.fApodization == FOURIER_APOD_STRONG) {
        fout << "STRONG";
      }
      fout << "  # NONE, WEAK, MEDIUM, STRONG";
      fout << std::endl;
    }

    // plot
    if (fFourier.fPlotTag) {
      fout << "plot             ";
      if (fFourier.fPlotTag == FOURIER_PLOT_REAL) {
        fout << "REAL ";
      } else if (fFourier.fPlotTag == FOURIER_PLOT_IMAG) {
        fout << "IMAG ";
      } else if (fFourier.fPlotTag == FOURIER_PLOT_REAL_AND_IMAG) {
        fout << "REAL_AND_IMAG";
      } else if (fFourier.fPlotTag == FOURIER_PLOT_POWER) {
        fout << "POWER";
      } else if (fFourier.fPlotTag == FOURIER_PLOT_PHASE) {
        fout << "PHASE";
      } else if (fFourier.fPlotTag == FOURIER_PLOT_PHASE_OPT_REAL) {
        fout << "PHASE_OPT_REAL";
      }
      fout << "   # REAL, IMAG, REAL_AND_IMAG, POWER, PHASE, PHASE_OPT_REAL";
      fout << std::endl;
    }

    // phase    
    if (fFourier.fPhaseParamNo.size() > 0) {
      TString phaseParamStr = BeautifyFourierPhaseParameterString();
      fout << "phase            " << phaseParamStr << std::endl;
    } else if (fFourier.fPhase.size() > 0) {
      fout << "phase            ";
      for (UInt_t i=0; i<fFourier.fPhase.size()-1; i++) {
        fout << fFourier.fPhase[i] << ", ";
      }
      fout << fFourier.fPhase[fFourier.fPhase.size()-1] << std::endl;
    }

    // range_for_phase_correction
    if ((fFourier.fRangeForPhaseCorrection[0] != -1.0) || (fFourier.fRangeForPhaseCorrection[1] != -1.0)) {
      fout << "range_for_phase_correction  " << fFourier.fRangeForPhaseCorrection[0] << "    " << fFourier.fRangeForPhaseCorrection[1] << std::endl;
    }

    // range
    if ((fFourier.fPlotRange[0] != -1.0) || (fFourier.fPlotRange[1] != -1.0)) {
      fout.setf(std::ios::fixed,std::ios::floatfield);
      UInt_t neededPrec = LastSignificant(fFourier.fPlotRange[0]);
      if (LastSignificant(fFourier.fPlotRange[1]) > neededPrec)
        neededPrec = LastSignificant(fFourier.fPlotRange[1]);
      fout.precision(neededPrec);
      fout << "range            " << fFourier.fPlotRange[0] << "    " << fFourier.fPlotRange[1] << std::endl;
    }

//     // phase_increment -- not used in msr-files at the moment (can only be set through the xml-file)
//     if (fFourier.fPhaseIncrement) {
//       fout << "phase_increment  " << fFourier.fPhaseIncrement << std::endl;
//     }

    fout << std::endl;
    fout << hline.Data() << std::endl;
  }

  // write PLOT blocks
  for (i = 0; i < fPlots.size(); ++i) {
    switch (fPlots[i].fPlotType) {
      case MSR_PLOT_SINGLE_HISTO:
        fout << "PLOT " << fPlots[i].fPlotType << "   (single histo plot)" << std::endl;
        break;
      case MSR_PLOT_SINGLE_HISTO_RRF:
        fout << "PLOT " << fPlots[i].fPlotType << "   (single histo RRF plot)" << std::endl;
        break;
      case MSR_PLOT_ASYM:
        fout << "PLOT " << fPlots[i].fPlotType << "   (asymmetry plot)" << std::endl;
        break;
      case MSR_PLOT_ASYM_RRF:
        fout << "PLOT " << fPlots[i].fPlotType << "   (asymmetry RRF plot)" << std::endl;
        break;
      case MSR_PLOT_MU_MINUS:
        fout << "PLOT " << fPlots[i].fPlotType << "   (mu minus plot)" << std::endl;
        break;
      case MSR_PLOT_BNMR:
        fout << "PLOT " << fPlots[i].fPlotType << "   (beta-NMR asymmetry plot)" << std::endl;
        break;
      case MSR_PLOT_NON_MUSR:
        fout << "PLOT " << fPlots[i].fPlotType << "   (non muSR plot)" << std::endl;
        break;
      default:
        break;
    }

    // runs
    fout << "runs     ";
    fout.precision(0);
    for (UInt_t j=0; j<fPlots[i].fRuns.size(); ++j) {
      fout.width(4);
      fout << fPlots[i].fRuns[j];
    }
    fout << std::endl;

    // range and sub_ranges
    if ((fPlots[i].fTmin.size() == 1) && (fPlots[i].fTmax.size() == 1)) {
      fout << "range    ";
      fout.precision(2);
      fout << fPlots[i].fTmin[0] << "   " << fPlots[i].fTmax[0];
    } else if ((fPlots[i].fTmin.size() > 1) && (fPlots[i].fTmax.size() > 1)) {
      fout << "sub_ranges ";
      fout.precision(2);
      for (UInt_t j=0; j < fPlots[i].fTmin.size(); ++j) {
        fout << "   " << fPlots[i].fTmin[j] << "   " << fPlots[i].fTmax[j];
      }
    }
    if (!fPlots[i].fYmin.empty() && !fPlots[i].fYmax.empty()) {
      fout << "   " << fPlots[i].fYmin[0] << "   " << fPlots[i].fYmax[0];
    }
    fout << std::endl;

    // use_fit_ranges
    if (fPlots[i].fUseFitRanges) {
      if (!fPlots[i].fYmin.empty() && !fPlots[i].fYmax.empty())
        fout << "use_fit_ranges " << fPlots[i].fYmin[0] << "  " << fPlots[i].fYmax[0] << std::endl;
      else
        fout << "use_fit_ranges" << std::endl;
    }

    // view_packing
    if (fPlots[i].fViewPacking != -1) {
      fout << "view_packing " << fPlots[i].fViewPacking << std::endl;
    }

    // logx
    if (fPlots[i].fLogX) {
      fout << "logx" << std::endl;
    }

    // logy
    if (fPlots[i].fLogY) {
      fout << "logy" << std::endl;
    }

    // lifetimecorrection
    if (fPlots[i].fLifeTimeCorrection) {
      fout << "lifetimecorrection" << std::endl;
    }
    
    // rrf_packing
    if (fPlots[i].fRRFPacking) {
      fout << "rrf_packing " << fPlots[i].fRRFPacking << std::endl;
    }

    // rrf_freq
    if (fPlots[i].fRRFFreq) {
      fout << "rrf_freq " << fPlots[i].fRRFFreq << " ";
      switch (fPlots[i].fRRFUnit) {
        case RRF_UNIT_kHz:
          fout << "kHz";
          break;
        case RRF_UNIT_MHz:
          fout << "MHz";
          break;
        case RRF_UNIT_Mcs:
          fout << "Mc/s";
          break;
        case RRF_UNIT_G:
          fout << "G";
          break;
        case RRF_UNIT_T:
          fout << "T";
          break;
        default:
          break;
      }
      fout << std::endl;
    }

    // rrf_phase
    if (fPlots[i].fRRFPhaseParamNo > 0) {
      fout << "rrf_phase  par" << fPlots[i].fRRFPhaseParamNo << std::endl;
    } else if (fPlots[i].fRRFPhase) {
      fout << "rrf_phase  " << fPlots[i].fRRFPhase << std::endl;
    }

    fout << std::endl;
  }
  if (!fPlots.empty()) {
    fout << hline.Data() << std::endl;
  }

  // write STATISTIC block
  TDatime dt;
  fout << "STATISTIC --- " << dt.AsSQLString() << std::endl;
  if (fStatistic.fValid) { // valid fit result
    if (fStatistic.fChisq) { // chisq
      str  = "  chisq = ";
      str += fStatistic.fMin;
      str += ", NDF = ";
      str += fStatistic.fNdf;
      str += ", chisq/NDF = ";
      str += fStatistic.fMin / fStatistic.fNdf;
      fout << str.Data() << std::endl;
    } else { // max. log. liklihood
      str  = "  maxLH = ";
      str += fStatistic.fMin;
      str += ", NDF = ";
      str += fStatistic.fNdf;
      str += ", maxLH/NDF = ";
      str += fStatistic.fMin / fStatistic.fNdf;
      fout << str.Data() << std::endl;
    }
  } else {
    fout << "*** FIT DID NOT CONVERGE ***" << std::endl;
  }

  // close file
  fout.close();

  str.Clear();
  pstr = nullptr;

  return PMUSR_SUCCESS;
}

//--------------------------------------------------------------------------
// SetMsrParamValue (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the fit parameter value at position idx.
 *
 * <p><b>return:</b>
 * - true if idx is within range
 * - false if idx is larger than the fit parameter vector.
 *
 * \param idx index of the fit parameter value.
 * \param value fit parameter value to be set.
 */
Bool_t PMsrHandler::SetMsrParamValue(UInt_t idx, Double_t value)
{
  if (idx >= fParam.size()) {
    std::cerr << std::endl << ">> PMsrHandler::SetMsrParamValue(): **ERROR** idx = " << idx << " is >= than the number of fit parameters " << fParam.size();
    std::cerr << std::endl;
    return false;
  }

  fParam[idx].fValue = value;

  return true;
}

//--------------------------------------------------------------------------
// SetMsrParamStep (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the fit parameter step value (initial step size for minuit2) at positon idx.
 * After a successful fit, the negative error will be writen.
 *
 * <p><b>return:</b>
 * - true if idx is within range
 * - false if idx is larger than the fit parameter vector.
 *
 * \param idx index fo the fit parameter step value
 * \param value fit parameter step value to be set.
 */
Bool_t PMsrHandler::SetMsrParamStep(UInt_t idx, Double_t value)
{
  if (idx >= fParam.size()) {
    std::cerr << std::endl << ">> PMsrHandler::SetMsrParamValue(): **ERROR** idx = " << idx << " is larger than the number of parameters " << fParam.size();
    std::cerr << std::endl;
    return false;
  }

  fParam[idx].fStep = value;

  return true;
}

//--------------------------------------------------------------------------
// SetMsrParamPosErrorPresent (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the flag whether the fit parameter positive error value is persent. This at positon idx.
 *
 * <p><b>return:</b>
 * - true if idx is within range
 * - false if idx is larger than the fit parameter vector.
 *
 * \param idx index fo the fit parameter positive error value
 * \param value fit parameter positive error value present.
 */
Bool_t PMsrHandler::SetMsrParamPosErrorPresent(UInt_t idx, Bool_t value)
{
  if (idx >= fParam.size()) {
    std::cerr << std::endl << ">> PMsrHandler::SetMsrParamPosErrorPresent(): **ERROR** idx = " << idx << " is larger than the number of parameters " << fParam.size();
    std::cerr << std::endl;
    return false;
  }

  fParam[idx].fPosErrorPresent = value;

  return true;
}

//--------------------------------------------------------------------------
// SetMsrParamPosError (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the fit parameter positive error value at positon idx.
 *
 * <p><b>return:</b>
 * - true if idx is within range
 * - false if idx is larger than the fit parameter vector.
 *
 * \param idx index fo the fit parameter positive error value
 * \param value fit parameter positive error value to be set.
 */
Bool_t PMsrHandler::SetMsrParamPosError(UInt_t idx, Double_t value)
{
  if (idx >= fParam.size()) {
    std::cerr << std::endl << ">> PMsrHandler::SetMsrParamPosError(): **ERROR** idx = " << idx << " is larger than the number of parameters " << fParam.size();
    std::cerr << std::endl;
    return false;
  }

  fParam[idx].fPosErrorPresent = true;
  fParam[idx].fPosError = value;

  return true;
}

//--------------------------------------------------------------------------
// SetMsrT0Entry (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the t0 entries for a given runNo and a given histogram index idx.
 *
 * \param runNo msr-file run number
 * \param idx msr-file histogram index
 * \param bin t0 bin value
 */
void PMsrHandler::SetMsrT0Entry(UInt_t runNo, UInt_t idx, Double_t bin)
{
  if (runNo >= fRuns.size()) { // error
    std::cerr << std::endl << ">> PMsrHandler::SetMsrT0Entry: **ERROR** runNo = " << runNo << ", is out of valid range 0.." << fRuns.size();
    std::cerr << std::endl;
    return;
  }

  if (idx >= fRuns[runNo].GetT0BinSize()) { // error
    std::cerr << std::endl << ">> PMsrHandler::SetMsrT0Entry: **WARNING** idx = " << idx << ", is out of valid range 0.." << fRuns[runNo].GetT0BinSize();
    std::cerr << std::endl << ">> Will add it anyway.";
    std::cerr << std::endl;
  }

  fRuns[runNo].SetT0Bin(bin, idx);
}

//--------------------------------------------------------------------------
// SetMsrAddT0Entry (public)
//--------------------------------------------------------------------------
/**
 * <p> Sets a t0 value for an addrun entry.
 *
 * \param runNo msr-file run number
 * \param addRunIdx msr-file addrun index, e.g. if 2 addruns are present addRunIdx can take the values 0 or 1.
 * \param histoIdx msr-file histogram index for an addrun.
 * \param bin t0 bin value.
 */
void PMsrHandler::SetMsrAddT0Entry(UInt_t runNo, UInt_t addRunIdx, UInt_t histoIdx, Double_t bin)
{
  if (runNo >= fRuns.size()) { // error
    std::cerr << std::endl << ">> PMsrHandler::SetMsrAddT0Entry: **ERROR** runNo = " << runNo << ", is out of valid range 0.." << fRuns.size();
    std::cerr << std::endl;
    return;
  }

  if (addRunIdx >= fRuns[runNo].GetAddT0BinEntries()) { // error
    std::cerr << std::endl << ">> PMsrHandler::SetMsrAddT0Entry: **WARNING** addRunIdx = " << addRunIdx << ", is out of valid range 0.." << fRuns[runNo].GetAddT0BinEntries();
    std::cerr << std::endl << ">> Will add it anyway.";
    std::cerr << std::endl;
  }

  if (static_cast<Int_t>(histoIdx) > fRuns[runNo].GetAddT0BinSize(addRunIdx)) { // error
    std::cerr << std::endl << ">> PMsrHandler::SetMsrAddT0Entry: **WARNING** histoIdx = " << histoIdx << ", is out of valid range 0.." << fRuns[runNo].GetAddT0BinSize(addRunIdx);
    std::cerr << std::endl << ">> Will add it anyway.";
    std::cerr << std::endl;
  }

  fRuns[runNo].SetAddT0Bin(bin, addRunIdx, histoIdx);
}

//--------------------------------------------------------------------------
// SetMsrDataRangeEntry (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the data range entries for a given runNo and a given histogram index idx.
 *
 * \param runNo msr-file run number
 * \param idx 0=start bin index, 1=end bin index
 * \param bin data range bin value
 */
void PMsrHandler::SetMsrDataRangeEntry(UInt_t runNo, UInt_t idx, Int_t bin)
{
  if (runNo >= fRuns.size()) { // error
    std::cerr << std::endl << ">> PMsrHandler::SetMsrDataRangeEntry: **ERROR** runNo = " << runNo << ", is out of valid range 0.." << fRuns.size();
    std::cerr << std::endl;
    return;
  }

  fRuns[runNo].SetDataRange(bin, idx);
}

//--------------------------------------------------------------------------
// SetMsrBkgRangeEntry (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the background range entries for a given runNo and a given histogram index idx.
 *
 * \param runNo msr-file run number
 * \param idx 0=start bin index, 1=end bin index
 * \param bin background range bin value
 */
void PMsrHandler::SetMsrBkgRangeEntry(UInt_t runNo, UInt_t idx, Int_t bin)
{
  if (runNo >= fRuns.size()) { // error
    std::cerr << std::endl << ">> PMsrHandler::SetMsrBkgRangeEntry: **ERROR** runNo = " << runNo << ", is out of valid range 0.." << fRuns.size();
    std::cerr << std::endl;
    return;
  }

  fRuns[runNo].SetBkgRange(bin, idx);
}

//--------------------------------------------------------------------------
// ParameterInUse (public)
//--------------------------------------------------------------------------
/**
 * <p>Needed for the following purpose: if minuit is minimizing, it varies
 * all the parameters of the parameter list (if not fixed), even if a particular
 * parameter is <b>NOT</b> used at all. This is stupid! Hence one has to check
 * if the parameter is used at all and if not, it has to be fixed.
 *
 * <p><b>return:</b>
 * - 0 if the parameter is <b>not</b> used.
 * - a value > 0 if the parameter is used.
 *
 * \param paramNo parameter number
 */
Int_t PMsrHandler::ParameterInUse(UInt_t paramNo)
{
  // check that paramNo is within acceptable range
  if (paramNo >= fParam.size())
    return -1;

  return fParamInUse[paramNo];
}

//--------------------------------------------------------------------------
// HandleFitParameterEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>The routine analyze a parameter line and, if the possible parameter list
 * is OK (what this means see below), it adds the parameter to the parameter list.
 *
 * <p>Possible cases:
 * \code
 * No Name Value Step/Neg_Error Pos_Error Boundary_Low Boundary_High
 * x  x    x     x              x         x            x              -> 7 Parameters, e.g. after a MINOS fit
 * x  x    x     x              x                                     -> 5 Parameters, e.g. after a MINOS fit
 *                                                                                     without boundaries
 * x  x    x     x                                                    -> 4 Parameters, e.g. after MIGRAD fit
 *                                                                                     without boundaries, or
 *                                                                                     when starting
 * \endcode
 *
 * <b>return:</b>
 * - true is fit parameter lines are OK.
 * - false otherwise
 *
 * \param lines is a list of lines containing the fitparameter block
 */
Bool_t PMsrHandler::HandleFitParameterEntry(PMsrLines &lines)
{
  // If msr-file is used for musrFT only, nothing needs to be done here.
  if (fFourierOnly)
    return true;

  PMsrParamStructure param;
  Bool_t    error = false;

  PMsrLines::iterator iter;

  TObjArray *tokens = nullptr;
  TObjString *ostr = nullptr;
  TString str;

  // fill param structure
  iter = lines.begin();
  while ((iter != lines.end()) && !error) {

    // init param structure
    param.fNoOfParams           = -1;
    param.fNo                   = -1;
    param.fName                 = TString("");
    param.fValue                = 0.0;
    param.fStep                 = 0.0;
    param.fPosErrorPresent      = false;
    param.fPosError             = 0.0;
    param.fLowerBoundaryPresent = false;
    param.fLowerBoundary        = 0.0;
    param.fUpperBoundaryPresent = false;
    param.fUpperBoundary        = 0.0;

    tokens = iter->fLine.Tokenize(" \t");
    if (!tokens) {
      std::cerr << std::endl << ">> PMsrHandler::HandleFitParameterEntry: **SEVERE ERROR** Couldn't tokenize Parameters in line " << iter->fLineNo;
      std::cerr << std::endl << std::endl;
      return false;
    }

    // handle various input possiblities
    if ((tokens->GetEntries() < 4) || (tokens->GetEntries() > 7) || (tokens->GetEntries() == 6)) {
      error = true;
    } else { // handle the first 4 parameter since they are always the same
      // parameter number
      ostr = dynamic_cast<TObjString*>(tokens->At(0));
      str = ostr->GetString();
      if (str.IsDigit())
        param.fNo = str.Atoi();
      else
        error = true;

      // parameter name
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      str = ostr->GetString();
      param.fName = str;

      // parameter value
      ostr = dynamic_cast<TObjString*>(tokens->At(2));
      str = ostr->GetString();
      if (str.IsFloat())
        param.fValue = static_cast<Double_t>(str.Atof());
      else
        error = true;

      // parameter value
      ostr = dynamic_cast<TObjString*>(tokens->At(3));
      str = ostr->GetString();
      if (str.IsFloat())
        param.fStep = static_cast<Double_t>(str.Atof());
      else
        error = true;

      // 4 values, i.e. No Name Value Step
      if (tokens->GetEntries() == 4) {
        param.fNoOfParams = 4;
      }

      // 5 values, i.e. No Name Value Neg_Error Pos_Error
      if (tokens->GetEntries() == 5) {
        param.fNoOfParams = 5;

        // positive error
        ostr = dynamic_cast<TObjString*>(tokens->At(4));
        str = ostr->GetString();
        if (str.IsFloat()) {
          param.fPosErrorPresent = true;
          param.fPosError = static_cast<Double_t>(str.Atof());
        } else {
          str.ToLower();
          if (!str.CompareTo("none", TString::kIgnoreCase))
            param.fPosErrorPresent = false;
          else
            error = true;
        }
      }

      // 7 values, i.e. No Name Value Neg_Error Pos_Error Lower_Boundary Upper_Boundary
      if (tokens->GetEntries() == 7) {
        param.fNoOfParams = 7;

        // positive error
        ostr = dynamic_cast<TObjString*>(tokens->At(4));
        str = ostr->GetString();
        if (str.IsFloat()) {
          param.fPosErrorPresent = true;
          param.fPosError = static_cast<Double_t>(str.Atof());
        } else {
          str.ToLower();
          if (!str.CompareTo("none", TString::kIgnoreCase))
            param.fPosErrorPresent = false;
          else
            error = true;
        }

        // lower boundary
        ostr = dynamic_cast<TObjString*>(tokens->At(5));
        str = ostr->GetString();
        // check if lower boundary is "none", i.e. upper boundary limited only
        if (!str.CompareTo("none", TString::kIgnoreCase)) { // none
          param.fLowerBoundaryPresent = false;
        } else { // assuming that the lower boundary is a number
          if (str.IsFloat()) {
            param.fLowerBoundary = static_cast<Double_t>(str.Atof());
            param.fLowerBoundaryPresent = true;
          } else {
            error = true;
          }
        }

        // upper boundary
        ostr = dynamic_cast<TObjString*>(tokens->At(6));
        str = ostr->GetString();
        // check if upper boundary is "none", i.e. lower boundary limited only
        if (!str.CompareTo("none", TString::kIgnoreCase)) { // none
          param.fUpperBoundaryPresent = false;
        } else { // assuming a number
          if (str.IsFloat()) {
            param.fUpperBoundary = static_cast<Double_t>(str.Atof());
            param.fUpperBoundaryPresent = true;
          } else {
            error = true;
          }
        }

        // check for lower-/upper-boundaries = none/none
        if (!param.fLowerBoundaryPresent && !param.fUpperBoundaryPresent)
          param.fNoOfParams = 5; // since there are no real boundaries present
      }
    }

    // check if enough elements found
    if (error) {
      std::cerr << std::endl;
      std::cerr << std::endl << ">> PMsrHandler::HandleFitParameterEntry: **ERROR** in line " << iter->fLineNo << ":";
      std::cerr << std::endl << ">> " << iter->fLine.Data();
      std::cerr << std::endl << ">> A Fit Parameter line needs to have the following form: ";
      std::cerr << std::endl;
      std::cerr << std::endl << ">> No Name Value Step/Error [Lower_Boundary Upper_Boundary]";
      std::cerr << std::endl;
      std::cerr << std::endl << ">> or";
      std::cerr << std::endl;
      std::cerr << std::endl << ">> No Name Value Step/Neg_Error Pos_Error [Lower_Boundary Upper_Boundary]";
      std::cerr << std::endl;
      std::cerr << std::endl << ">> No:             the parameter number (an Int_t)";
      std::cerr << std::endl << ">> Name:           the name of the parameter (less than 256 character)";
      std::cerr << std::endl << ">> Value:          the starting value of the parameter (a Double_t)";
      std::cerr << std::endl << ">> Step/Error,";
      std::cerr << std::endl << ">> Step/Neg_Error: the starting step value in a fit  (a Double_t), or";
      std::cerr << std::endl << ">>                 the symmetric error (MIGRAD, SIMPLEX), or";
      std::cerr << std::endl << ">>                 the negative error (MINOS)";
      std::cerr << std::endl << ">> Pos_Error:      the positive error (MINOS),  (a Double_t or \"none\")";
      std::cerr << std::endl << ">> Lower_Boundary: the lower boundary allowed for the fit parameter  (a Double_t or \"none\")";
      std::cerr << std::endl << ">> Upper_Boundary: the upper boundary allowed for the fit parameter  (a Double_t or \"none\")";
      std::cerr << std::endl;
    } else { // everything is OK, therefore add the parameter to the parameter list
      fParam.push_back(param);
    }

    // clean up
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }

    iter++;
  }

  // check if all parameters have subsequent numbers.
  for (UInt_t i=0; i<fParam.size(); i++) {
    if (fParam[i].fNo != static_cast<Int_t>(i)+1) {
      error = true;
      std::cerr << std::endl << ">> PMsrHandler::HandleFitParameterEntry: **ERROR**";
      std::cerr << std::endl << ">> Sorry, you are assuming to much from this program, it cannot";
      std::cerr << std::endl << ">> handle none subsequent numbered parameters yet or in the near future.";
      std::cerr << std::endl << ">> Found parameter " << fParam[i].fName.Data() << ", with";
      std::cerr << std::endl << ">> parameter number " << fParam[i].fNo << ", at paramter position " << i+1 << ".";
      std::cerr << std::endl << ">> This needs to be fixed first.";
      std::cerr << std::endl;
      break;
    }
  }

  return !error;
}

//--------------------------------------------------------------------------
// HandleTheoryEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>Just stores the THEORY block lines.
 *
 * <b>return:</b>
 * - true always
 *
 * \param lines is a list of lines containing the theory block
 */
Bool_t PMsrHandler::HandleTheoryEntry(PMsrLines &lines)
{
  // If msr-file is used for musrFT only, nothing needs to be done here.
  if (fFourierOnly)
    return true;

  // store the theory lines
  fTheory = lines;

  return true;
}

//--------------------------------------------------------------------------
// HandleFunctionsEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>Parses the FUNCTIONS block of the msr-file.
 *
 * <b>return:</b>
 * - true if the parsing was successful.
 * - false otherwise
 *
 * \param lines is a list of lines containing the functions block
 */
Bool_t PMsrHandler::HandleFunctionsEntry(PMsrLines &lines)
{
  // If msr-file is used for musrFT only, nothing needs to be done here.
  if (fFourierOnly)
    return true;

  // store the functions lines
  fFunctions = lines;

  // create function handler
  fFuncHandler = new PFunctionHandler(fFunctions);
  if (fFuncHandler == nullptr) {
    std::cerr << std::endl << ">> PMsrHandler::HandleFunctionsEntry: **ERROR** Couldn't invoke PFunctionHandler." << std::endl;
    return false;
  }

  // do the parsing
  if (!fFuncHandler->DoParse()) {
    return false;
  }

  // check if an empty FUNCTIONS block is present
  if ((fFuncHandler->GetNoOfFuncs() == 0) && !lines.empty()) {
    std::cerr << std::endl << ">> PMsrHandler::HandleFunctionsEntry: **WARNING** empty FUNCTIONS block found!";
    std::cerr << std::endl;
  }

  return true;
}

//--------------------------------------------------------------------------
// HandleGlobalEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>Parses the GLOBAL block of the msr-file.
 *
 * <b>return:</b>
 * - true if successful
 * - false otherwise
 *
 * \param lines is a list of lines containing the run blocks
 */
Bool_t PMsrHandler::HandleGlobalEntry(PMsrLines &lines)
{
  PMsrLines::iterator iter;
  PMsrGlobalBlock global;

  Bool_t error = false;

  TString str;
  TObjArray *tokens = nullptr;
  TObjString *ostr = nullptr;
  Int_t ival = 0;
  Double_t dval = 0.0;
  UInt_t addT0Counter = 0;

  // since this routine is called, a GLOBAL block is present
  global.SetGlobalPresent(true);

  iter = lines.begin();
  while ((iter != lines.end()) && !error) {
    // remove potential comment at the end of lines
    str = iter->fLine;
    Ssiz_t idx = str.Index("#");
    if (idx != -1)
      str.Remove(idx);

    // tokenize line
    tokens = str.Tokenize(" \t");
    if (!tokens) {
      std::cerr << std::endl << ">> PMsrHandler::HandleGlobalEntry: **SEVERE ERROR** Couldn't tokenize line " << iter->fLineNo;
      std::cerr << std::endl << std::endl;
      return false;
    }

    if (iter->fLine.BeginsWith("fittype", TString::kIgnoreCase)) { // fittype
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          Int_t fittype = str.Atoi();
          if ((fittype == MSR_FITTYPE_SINGLE_HISTO) ||
              (fittype == MSR_FITTYPE_SINGLE_HISTO_RRF) ||
              (fittype == MSR_FITTYPE_ASYM) ||
              (fittype == MSR_FITTYPE_ASYM_RRF) ||
              (fittype == MSR_FITTYPE_MU_MINUS) ||
              (fittype == MSR_FITTYPE_BNMR) ||
              (fittype == MSR_FITTYPE_NON_MUSR)) {
            global.SetFitType(fittype);
          } else {
            error = true;
          }
        } else {
          error = true;
        }
      }
    } else if (iter->fLine.BeginsWith("rrf_freq", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 3) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsFloat()) {
          dval = str.Atof();
          if (dval <= 0.0)
            error = true;
        }
        if (!error) {
          ostr = dynamic_cast<TObjString*>(tokens->At(2));
          str = ostr->GetString();
          global.SetRRFFreq(dval, str.Data());
          if (global.GetRRFFreq(str.Data()) == RRF_FREQ_UNDEF)
            error = true;
        }
      }
    } else if (iter->fLine.BeginsWith("rrf_packing", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          ival = str.Atoi();
          if (ival > 0) {
            global.SetRRFPacking(ival);
          } else {
            error = true;
          }
        } else {
          error = true;
        }
      }
    } else if (iter->fLine.BeginsWith("rrf_phase", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsFloat()) {
          dval = str.Atof();
          global.SetRRFPhase(dval);
        } else {
          error = true;
        }
      }
    } else if (iter->fLine.BeginsWith("data", TString::kIgnoreCase)) { // data
      if (tokens->GetEntries() < 3) {
        error = true;
      } else {
        for (Int_t i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsDigit()) {
            ival = str.Atoi();
            if (ival >= 0) {
              global.SetDataRange(ival, i-1);
            } else {
              error = true;
            }
          } else {
            error = true;
          }
        }
      }
    } else if (iter->fLine.BeginsWith("t0", TString::kIgnoreCase)) { // t0
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        for (Int_t i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsFloat()) {
            dval = str.Atof();
            if (dval >= 0.0)
              global.SetT0Bin(dval);
            else
              error = true;
          } else {
            error = true;
          }
        }
      }
    } else if (iter->fLine.BeginsWith("addt0", TString::kIgnoreCase)) { // addt0
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        for (Int_t i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsFloat()) {
            dval = str.Atof();
            if (dval >= 0.0)
              global.SetAddT0Bin(dval, addT0Counter, i-1);
            else
              error = true;
          } else {
            error = true;
          }
        }
      }
      addT0Counter++;
    } else if (iter->fLine.BeginsWith("fit", TString::kIgnoreCase)) { // fit range
      if (tokens->GetEntries() < 3) {
        error = true;
      } else { // fit given in time, i.e. fit <start> <end>, where <start>, <end> are given as doubles
        if (iter->fLine.Contains("fgb", TString::kIgnoreCase)) { // fit given in bins, i.e. fit fgb+n0 lgb-n1
          // check 1st entry, i.e. fgb[+n0]
          ostr = dynamic_cast<TObjString*>(tokens->At(1));
          str = ostr->GetString();
          Ssiz_t idx = str.First("+");
          TString numStr = str;
          if (idx > -1) { // '+' present hence extract n0
            numStr.Remove(0,idx+1);
            if (numStr.IsFloat()) {
              global.SetFitRangeOffset(numStr.Atoi(), 0);
            } else {
              error = true;
            }
          } else { // n0 == 0
            global.SetFitRangeOffset(0, 0);
          }
          // check 2nd entry, i.e. lgb[-n1]
          ostr = dynamic_cast<TObjString*>(tokens->At(2));
          str = ostr->GetString();
          idx = str.First("-");
          numStr = str;
          if (idx > -1) { // '-' present hence extract n1
            numStr.Remove(0,idx+1);
            if (numStr.IsFloat()) {
              global.SetFitRangeOffset(numStr.Atoi(), 1);
            } else {
              error = true;
            }
          } else { // n0 == 0
            global.SetFitRangeOffset(0, 0);
          }
          if (!error)
            global.SetFitRangeInBins(true);
        } else { // fit given in time, i.e. fit <start> <end>, where <start>, <end> are given as doubles
          for (Int_t i=1; i<3; i++) {
            ostr = dynamic_cast<TObjString*>(tokens->At(i));
            str = ostr->GetString();
            if (str.IsFloat())
              global.SetFitRange(str.Atof(), i-1);
            else
              error = true;
          }
        }
      }
    } else if (iter->fLine.BeginsWith("packing", TString::kIgnoreCase)) { // packing
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          ival = str.Atoi();
          if (ival >= 0) {
            global.SetPacking(ival);
          } else {
            error = true;
          }
        } else {
          error = true;
        }
      }
    }

    // clean up
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }

    ++iter;
  }

  if (error) {
    --iter;
    std::cerr << std::endl << ">> PMsrHandler::HandleGlobalEntry: **ERROR** in line " << iter->fLineNo << ":";
    std::cerr << std::endl << ">> '" << iter->fLine.Data() << "'";
    std::cerr << std::endl << ">> GLOBAL block syntax is too complex to print it here. Please check the manual.";
  } else { // save global
    fGlobal = global;
  }

  return !error;
}

//--------------------------------------------------------------------------
// HandleRunEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>Parses the RUN blocks of the msr-file.
 *
 * <b>return:</b>
 * - true if successful
 * - false otherwise
 *
 * \param lines is a list of lines containing the run blocks
 */
Bool_t PMsrHandler::HandleRunEntry(PMsrLines &lines)
{
  PMsrLines::iterator iter;
  PMsrRunBlock param;
  Bool_t first = true; // first run line tag
  Bool_t error = false;
  Bool_t runLinePresent = false;

  TString str, line;
  TObjArray *tokens = nullptr;
  TObjString *ostr = nullptr;

  UInt_t addT0Counter = 0;

  Int_t ival;
  Double_t dval;  

  iter = lines.begin();
  while ((iter != lines.end()) && !error) {
    // remove potential comment at the end of lines
    str = iter->fLine;
    Ssiz_t idx = str.Index("#");
    if (idx != -1)
      str.Remove(idx);

    // tokenize line
    tokens = str.Tokenize(" \t");
    if (!tokens) {
      std::cerr << std::endl << ">> PMsrHandler::HandleRunEntry: **SEVERE ERROR** Couldn't tokenize Parameters in line " << iter->fLineNo;
      std::cerr << std::endl << std::endl;
      return false;
    }

    // copy of the current line
    line = iter->fLine;
    // strip leading spaces from the begining
    line.Remove(TString::kLeading, ' ');

    // RUN line ----------------------------------------------
    if (line.BeginsWith("run", TString::kIgnoreCase)) {

      runLinePresent = true; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (!first) { // not the first run in the list
        fRuns.push_back(param);
        param.CleanUp();
      } else {
        first = false;
      }

      // get run name, beamline, institute, and file-format
      if (tokens->GetEntries() < 5) {
        error = true;
      } else {
        // run name
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        param.SetRunName(str);
        // beamline
        ostr = dynamic_cast<TObjString*>(tokens->At(2));
        str = ostr->GetString();
        param.SetBeamline(str);
        // institute
        ostr = dynamic_cast<TObjString*>(tokens->At(3));
        str = ostr->GetString();
        param.SetInstitute(str);
        // data file format
        ostr = dynamic_cast<TObjString*>(tokens->At(4));
        str = ostr->GetString();
        param.SetFileFormat(str);
      }

      addT0Counter = 0; // reset counter
    }

    // ADDRUN line ---------------------------------------------
    if (line.BeginsWith("addrun", TString::kIgnoreCase)) {

      if (!runLinePresent) {
        std::cerr << std::endl << ">> PMsrHandler::HandleRunEntry: **ERROR** Found ADDRUN without prior RUN, or";
        std::cerr << std::endl << ">>   ADDRUN lines intercepted by other stuff. All this is not allowed!";
        std::cerr << std::endl << ">>   error in line " << iter->fLineNo;
        std::cerr << std::endl;
        error = true;
        continue;
      }

      // get run name, beamline, institute, and file-format
      if (tokens->GetEntries() < 5) {
        error = true;
      } else {
        // run name
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        param.SetRunName(str);
        // beamline
        ostr = dynamic_cast<TObjString*>(tokens->At(2));
        str = ostr->GetString();
        param.SetBeamline(str);
        // institute
        ostr = dynamic_cast<TObjString*>(tokens->At(3));
        str = ostr->GetString();
        param.SetInstitute(str);
        // data file format
        ostr = dynamic_cast<TObjString*>(tokens->At(4));
        str = ostr->GetString();
        param.SetFileFormat(str);
      }
    }

    // fittype -------------------------------------------------
    if (line.BeginsWith("fittype", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          Int_t fittype = str.Atoi();
          if ((fittype == MSR_FITTYPE_SINGLE_HISTO) ||
              (fittype == MSR_FITTYPE_SINGLE_HISTO_RRF) ||
              (fittype == MSR_FITTYPE_ASYM) ||
              (fittype == MSR_FITTYPE_ASYM_RRF) ||
              (fittype == MSR_FITTYPE_MU_MINUS) ||
              (fittype == MSR_FITTYPE_BNMR) ||
              (fittype == MSR_FITTYPE_NON_MUSR)) {
            param.SetFitType(fittype);
          } else {
            error = true;
          }
        } else {
          error = true;
        }
      }
    }

    // alpha -------------------------------------------------
    if (line.BeginsWith("alpha", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          ival = str.Atoi();
          if (ival > 0)
            param.SetAlphaParamNo(ival);
          else
            error = true;
        } else if (str.Contains("fun")) {
          Int_t no;
          if (FilterNumber(str, "fun", MSR_PARAM_FUN_OFFSET, no))
            param.SetAlphaParamNo(no);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // beta -------------------------------------------------
    if (line.BeginsWith("beta", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          ival = str.Atoi();
          if (ival > 0)
            param.SetBetaParamNo(ival);
          else
            error = true;          
        } else if (str.Contains("fun")) {
          Int_t no;
          if (FilterNumber(str, "fun", MSR_PARAM_FUN_OFFSET, no))
            param.SetBetaParamNo(no);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // norm -------------------------------------------------
    if (line.BeginsWith("norm", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          param.SetNormParamNo(str.Atoi());
        } else if (str.Contains("fun")) {
          Int_t no;
          if (FilterNumber(str, "fun", MSR_PARAM_FUN_OFFSET, no))
            param.SetNormParamNo(no);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // backgr.fit --------------------------------------------
    if (line.BeginsWith("backgr.fit", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          ival = str.Atoi();
          if (ival > 0)
            param.SetBkgFitParamNo(ival);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // lifetime ------------------------------------------------
    if (line.BeginsWith("lifetime ", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          ival = str.Atoi();
          if (ival > 0)
            param.SetLifetimeParamNo(ival);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // lifetimecorrection ---------------------------------------
    if (line.BeginsWith("lifetimecorrection", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      param.SetLifetimeCorrection(true);
    }

    // map ------------------------------------------------------
    if (line.BeginsWith("map", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();
        if (str.IsDigit()) {
          ival = str.Atoi();
          if (ival >= 0)
            param.SetMap(ival);
          else
            error = true;
        } else {
          error = true;
        }
      }
      // check map entries, i.e. if the map values are within parameter bounds
      if (!fFourierOnly) {
        for (UInt_t i=0; i<param.GetMap()->size(); i++) {
          if ((param.GetMap(i) < 0) || (param.GetMap(i) > static_cast<Int_t>(fParam.size()))) {
            std::cerr << std::endl << ">> PMsrHandler::HandleRunEntry: **SEVERE ERROR** map value " << param.GetMap(i) << " in line " << iter->fLineNo << " is out of range!";
            error = true;
            break;
          }
        }
      }
    }

    // forward ------------------------------------------------
    if (line.BeginsWith("forward", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        PUIntVector group;
        str = iter->fLine;
        PStringNumberList *rl = new PStringNumberList(str.Data());
        std::string errorMsg("");
        if (rl->Parse(errorMsg, true)) {
          group = rl->GetList();
          for (UInt_t i=0; i<group.size(); i++) {
            param.SetForwardHistoNo(group[i]);
          }
        } else {
          error = true;
        }
        delete rl;
        group.clear();
      }
    }

    // backward -----------------------------------------------
    if (line.BeginsWith("backward", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        PUIntVector group;
        str = iter->fLine;
        PStringNumberList *rl = new PStringNumberList(str.Data());
        std::string errorMsg("");
        if (rl->Parse(errorMsg, true)) {
          group = rl->GetList();
          for (UInt_t i=0; i<group.size(); i++) {
            param.SetBackwardHistoNo(group[i]);
          }
        } else {
          error = true;
        }
        delete rl;
        group.clear();
      }
    }

    // backgr.fix ----------------------------------------------
    if (line.BeginsWith("backgr.fix", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        for (Int_t i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsFloat())
            param.SetBkgFix(str.Atof(), i-1);
          else
            error = true;
        }
      }
    }

    // background ---------------------------------------------
    if (line.BeginsWith("background", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if ((tokens->GetEntries() < 3) || (tokens->GetEntries() % 2 != 1)) { // odd number (>=3) of entries needed
        error = true;
      } else {
        for (Int_t i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsDigit()) {
            ival = str.Atoi();
            if (ival > 0)
              param.SetBkgRange(ival, i-1);
            else
              error = true;
          } else {
            error = true;
          }
        }
      }
    }

    // data --------------------------------------------------
    if (line.BeginsWith("data", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if ((tokens->GetEntries() < 3) || (tokens->GetEntries() % 2 != 1)) { // odd number (>=3) of entries needed
        error = true;
      } else {
        for (Int_t i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsDigit()) {
            ival = str.Atoi();
            if (ival > 0)
              param.SetDataRange(ival, i-1);
            else
              error = true;
          } else {
            error = true;
          }
        }
      }
    }

    // t0 -----------------------------------------------------
    if (line.BeginsWith("t0", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        for (Int_t i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsFloat()) {
            dval = str.Atof();
            if (dval >= 0.0)
              param.SetT0Bin(dval);
            else
              error = true;
          } else {
            error = true;
          }
        }
      }
    }

    // addt0 -----------------------------------------------------
    if (line.BeginsWith("addt0", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        for (Int_t i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsFloat()) {
            dval = str.Atof();
            if (dval >= 0.0)
              param.SetAddT0Bin(dval, addT0Counter, i-1);
            else
              error = true;
          } else {
            error = true;
          }
        }
      }

      addT0Counter++;
    }

    // fit -----------------------------------------------------
    if (line.BeginsWith("fit ", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() < 3) {
        error = true;
      } else {
        if (iter->fLine.Contains("fgb", TString::kIgnoreCase)) { // fit given in bins, i.e. fit fgb+n0 lgb-n1
          // check 1st entry, i.e. fgb[+n0]
          ostr = dynamic_cast<TObjString*>(tokens->At(1));
          str = ostr->GetString();
          Ssiz_t idx = str.First("+");
          TString numStr = str;
          if (idx > -1) { // '+' present hence extract n0
            numStr.Remove(0,idx+1);
            if (numStr.IsFloat()) {
              param.SetFitRangeOffset(numStr.Atoi(), 0);
            } else {
              error = true;
            }
          } else { // n0 == 0
            param.SetFitRangeOffset(0, 0);
          }
          // check 2nd entry, i.e. lgb[-n1]
          ostr = dynamic_cast<TObjString*>(tokens->At(2));
          str = ostr->GetString();
          idx = str.First("-");
          numStr = str;
          if (idx > -1) { // '-' present hence extract n1
            numStr.Remove(0,idx+1);
            if (numStr.IsFloat()) {
              param.SetFitRangeOffset(numStr.Atoi(), 1);
            } else {
              error = true;
            }
          } else { // n0 == 0
            param.SetFitRangeOffset(0, 0);
          }

          if (!error)
            param.SetFitRangeInBins(true);
        } else { // fit given in time, i.e. fit <start> <end>, where <start>, <end> are given as doubles
          for (Int_t i=1; i<3; i++) {
            ostr = dynamic_cast<TObjString*>(tokens->At(i));
            str = ostr->GetString();
            if (str.IsFloat())
              param.SetFitRange(str.Atof(), i-1);
            else
              error = true;
          }
        }
      }
    }

    // packing --------------------------------------------------
    if (line.BeginsWith("packing", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() != 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          ival = str.Atoi();
          if (ival > 0)
            param.SetPacking(ival);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // xy-data -----------------------------------------------
    if (line.BeginsWith("xy-data", TString::kIgnoreCase)) {

      runLinePresent = false; // this is needed to make sure that a run line is present before and ADDRUN is following

      if (tokens->GetEntries() != 3) { // xy-data x-label y-label
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) { // xy-data indices given
          param.SetXDataIndex(str.Atoi()); // x-index
          ostr = dynamic_cast<TObjString*>(tokens->At(2));
          str = ostr->GetString();
          if (str.IsDigit()) {
            ival = str.Atoi();
            if (ival > 0)
              param.SetYDataIndex(ival); // y-index
            else
              error = true;
          } else {
            error = true;
          }
        } else { // xy-data labels given
          param.SetXDataLabel(str); // x-label
          ostr = dynamic_cast<TObjString*>(tokens->At(2));
          str = ostr->GetString();
          param.SetYDataLabel(str); // y-label
        }
      }
    }

    // clean up
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }

    ++iter;
  }

  if (error) {
    --iter;
    std::cerr << std::endl << ">> PMsrHandler::HandleRunEntry: **ERROR** in line " << iter->fLineNo << ":";
    std::cerr << std::endl << ">> " << iter->fLine.Data();
    std::cerr << std::endl << ">> RUN block syntax is too complex to print it here. Please check the manual.";
  } else { // save last run found
    fRuns.push_back(param);
    param.CleanUp();
  }

  return !error;
}

//--------------------------------------------------------------------------
// FilterNumber (private)
//--------------------------------------------------------------------------
/**
 * <p>Used to filter numbers from a string of the structure strX, where
 * X is a number. The filter string is used to define the offset to X.
 * It is used to filter strings like: map1 or fun4. At the moment only
 * the filter strings 'map', 'fun', and 'par' are supported.
 *
 * <b>return:</b>
 * - true if successful
 * - false otherwise
 *
 * \param str  input string
 * \param filter filter string
 * \param offset it is used to offset to found number, e.g. strX -> no = X+offset
 * \param no filtered number
 */
Bool_t PMsrHandler::FilterNumber(TString str, const Char_t *filter, Int_t offset, Int_t &no)
{
  Int_t  found, no_found=-1;

  // copy str to an ordinary c-like string
  Char_t *cstr, filterStr[32];
  cstr = new Char_t[str.Sizeof()];
  strncpy(cstr, str.Data(), str.Sizeof());
  sprintf(filterStr, "%s%%d", filter);

  // get number if present
  found = sscanf(cstr, filterStr, &no_found);
  if (found == 1)
    if (no_found < 1000)
      no = no_found + offset;

  // clean up
  if (cstr) {
    delete [] cstr;
    cstr = nullptr;
  }

  if ((no_found < 0) || (no_found > 1000))
    return false;
  else
    return true;
}

//--------------------------------------------------------------------------
// HandleCommandsEntry (private)
//--------------------------------------------------------------------------
/**
 * <p> Just copies the COMMAND block lines.
 *
 * <b>return:</b>
 * - true
 *
 * \param lines is a list of lines containing the command block
 */
Bool_t PMsrHandler::HandleCommandsEntry(PMsrLines &lines)
{
  // If msr-file is used for musrFT only, nothing needs to be done here.
  if (fFourierOnly)
    return true;

  PMsrLines::iterator iter;

  if (lines.empty()) {
    std::cerr << std::endl << ">> PMsrHandler::HandleCommandsEntry(): **WARNING**: There is no COMMAND block! Do you really want this?";
    std::cerr << std::endl;
  }

  for (iter = lines.begin(); iter != lines.end(); ++iter) {
    if (!iter->fLine.BeginsWith("COMMANDS"))
      fCommands.push_back(*iter);
  }

  return true;
}

//--------------------------------------------------------------------------
// InitFourierParameterStructure (private)
//--------------------------------------------------------------------------
/**
 * <p>Initializes the Fourier parameter structure.
 *
 * \param fourier fourier parameters
 */
void PMsrHandler::InitFourierParameterStructure(PMsrFourierStructure &fourier)
{
  fourier.fFourierBlockPresent = false;           // fourier block present
  fourier.fUnits = FOURIER_UNIT_NOT_GIVEN;        // fourier untis, default: NOT GIVEN
  fourier.fFourierPower = -1;                     // zero padding, default: -1 = NOT GIVEN
  fourier.fDCCorrected = false;                   // dc-corrected FFT, default: false
  fourier.fApodization = FOURIER_APOD_NOT_GIVEN;  // apodization, default: NOT GIVEN
  fourier.fPlotTag = FOURIER_PLOT_NOT_GIVEN;      // initial plot tag, default: NOT GIVEN
  fourier.fPhaseRef = -1;                         // initial phase reference -1 means: use absolute phases
  fourier.fPhaseParamNo.clear();                  // initial phase parameter no vector is empty
  fourier.fPhase.clear();                         // initial phase vector is empty
  for (UInt_t i=0; i<2; i++) {
    fourier.fRangeForPhaseCorrection[i] = -1.0;   // frequency range for phase correction, default: {-1, -1} = NOT GIVEN
    fourier.fPlotRange[i] = -1.0;                 // fourier plot range, default: {-1, -1} = NOT GIVEN
  }
}

//--------------------------------------------------------------------------
// RemoveComment (private)
//--------------------------------------------------------------------------
/**
 * <p>Removes a potentially present comment from str and returns the truncated
 * string in truncStr. A comment starts with '#'
 *
 * @param str original string which might contain a comment
 * @param truncStr string from which the comment has been removed
 */
void PMsrHandler::RemoveComment(const TString &str, TString &truncStr)
{
  truncStr = str;
  Ssiz_t idx = str.First('#'); // find the index of the comment character

  // truncate string if comment is found
  if (idx > 0) {
    truncStr.Resize(idx-1);
  }
}

//--------------------------------------------------------------------------
// ParseFourierPhaseValueVector (private)
//--------------------------------------------------------------------------
/**
 * <p>examines if str has the form 'phase val0 [sep val1 ... sep valN]'.
 * If this form is found, fill in val0 ... valN to fFourier.fPhase
 * vector.
 *
 * @param fourier msr-file Fourier structure
 * @param str string to be analyzed
 * @param error flag needed to propagate a fatal error
 *
 * @return true if a phase value form is found, otherwise return false
 */
Bool_t PMsrHandler::ParseFourierPhaseValueVector(PMsrFourierStructure &fourier, const TString &str, Bool_t &error)
{
  Bool_t result = true;

  TObjArray *tok = str.Tokenize(" ,;\t");
  if (tok == nullptr) {
    std::cerr << std::endl << ">> PMsrHandler::ParseFourierPhaseValueVector: **ERROR** couldn't tokenize Fourier phase line." << std::endl << std::endl;
    return false;
  }

  // make sure there are enough tokens
  if (tok->GetEntries() < 2) {
    error = true;
    return false;
  }

  // convert all acceptable tokens
  TObjString *ostr = nullptr;
  TString sstr("");
  for (Int_t i=1; i<tok->GetEntries(); i++) {
    ostr = dynamic_cast<TObjString*>(tok->At(i));
    sstr = ostr->GetString();
    if (sstr.IsFloat()) {
      fourier.fPhase.push_back(sstr.Atof());
    } else {
      result = false;
      if (i>1) { // make sure that no 'phase val, parX' mixture is present
        std::cerr << std::endl << ">> PMsrHandler::ParseFourierPhaseValueVector: **ERROR** in Fourier phase line.";
        std::cerr << std::endl << ">>     Attempt to mix val, parX? This is currently not supported." << std::endl << std::endl;
        error = true;
      }
      break;
    }
  }

  // clean up
  if (tok) {
    delete tok;
  }

  return result;
}

//--------------------------------------------------------------------------
// ParseFourierPhaseParVector (private)
//--------------------------------------------------------------------------
/**
 * <p> examines if str has the form 'phase parX0 [sep parX1 ... sep parXN]'.
 * Also allowed is that instead of parXn only one of the parameters could have the
 * form parRn which markes a reference phase for relative phase fittings.
 * If this form is found, fill in parX0 ... parXN to fourier.fPhaseParamNo, and
 * in case a parR is present, set the fourier.fPhaseRef accordingly.
 *
 * @param fourier msr-file Fourier structure
 * @param str string to be analyzed
 * @param error flag needed to propagate a fatal error
 *
 * @return true if a phase parameter form is found, otherwise return false
 */
Bool_t PMsrHandler::ParseFourierPhaseParVector(PMsrFourierStructure &fourier, const TString &str, Bool_t &error)
{
  Bool_t result = true;
  Int_t refCount = 0;

  TObjArray *tok = str.Tokenize(" ,;\t");
  if (tok == nullptr) {
    std::cerr << std::endl << ">> PMsrHandler::ParseFourierPhaseParVector: **ERROR** couldn't tokenize Fourier phase line." << std::endl << std::endl;
    return false;
  }

  // make sure there are enough tokens
  if (tok->GetEntries() < 2) {
    error = true;
    return false;
  }

  // check that all tokens start with par
  TString sstr;
  for (Int_t i=1; i<tok->GetEntries(); i++) {
    TObjString *ostr = dynamic_cast<TObjString*>(tok->At(i));
    sstr = ostr->GetString();
    if (!sstr.BeginsWith("par")) {
      std::cerr << ">> PMsrHandler::ParseFourierPhaseParVector: **ERROR** found unhandable token '" << sstr << "'" << std::endl;
      error = true;
      result = false;
      break;
    }

    if (sstr.BeginsWith("parR")) {
      refCount++;
    }

    // rule out par(X, offset, #Param) syntax
    if (sstr.BeginsWith("par(")) {
      result = false;
      break;
    }
  }

  if (refCount > 1) {
    std::cerr << ">> PMsrHandler::ParseFourierPhaseParVector: **ERROR** found multiple parR's! Only one reference phase is accepted." << std::endl;
    result = false;
  }

  // check that token has the form parX, where X is an int
  Int_t rmNoOf = 3;
  if (result != false) {
    for (Int_t i=1; i<tok->GetEntries(); i++) {
      TObjString *ostr = dynamic_cast<TObjString*>(tok->At(i));
      sstr = ostr->GetString();
      rmNoOf = 3;
      if (sstr.BeginsWith("parR")) {
        rmNoOf++;
      }
      sstr.Remove(0, rmNoOf); // remove 'par' of 'parR' part. Rest should be an integer
      if (sstr.IsDigit()) {
        if (rmNoOf == 4) // parR
          fourier.fPhaseRef = sstr.Atoi();
        fourier.fPhaseParamNo.push_back(sstr.Atoi());
      } else {
        std::cerr << ">> PMsrHandler::ParseFourierPhaseParVector: **ERROR** found token '" << ostr->GetString() << "' which is not parX with X an integer." << std::endl;
        fourier.fPhaseParamNo.clear();
        error = true;
        break;
      }
    }
  }

  if (fourier.fPhaseParamNo.size() == tok->GetEntries()-1) { // everything as expected
    result = true;
  } else {
    result = false;
  }

  // clean up
  if (tok) {
    delete tok;
  }

  return result;
}

//--------------------------------------------------------------------------
// ParseFourierPhaseParIterVector (private)
//--------------------------------------------------------------------------
/**
 * <p> examines if str has the form 'phase par(X0, offset, #params)' or 'phase parR(X0, offset,  #params)'.
 * If this form is found, fill in parX0 ... parXN to fourier.fPhaseParamNo, and
 * in case of 'parR' also set the fourier.fPhaseRef accordingly.
 *
 * @param fourier msr-file Fourier structure
 * @param str string to be analyzed
 * @param error flag needed to propagate a fatal error
 *
 * @return true if a phase parameter iterator form is found, otherwise return false
 */
Bool_t PMsrHandler::ParseFourierPhaseParIterVector(PMsrFourierStructure &fourier, const TString &str, Bool_t &error)
{
  TString wstr = str;

  // remove 'phase' from string
  wstr.Remove(0, 5);
  wstr = wstr.Strip(TString::kLeading, ' ');

  // remove 'par(' from string if present, otherwise and error is issued
  if (!wstr.BeginsWith("par(") && !wstr.BeginsWith("parR(")) {
    std::cout << ">> PMsrHandler::ParseFourierPhaseParIterVector: **ERROR** token should start with 'par(' or 'parR(', found: '" << wstr << "' -> ERROR" << std::endl;
    error = true;
    return false;
  }
  Int_t noOf = 4; // number of characters to be removed
  Bool_t relativePhase = false; // relative phase handling wished
  if (wstr.BeginsWith("parR(")) {
    noOf += 1;
    relativePhase = true;
  }
  wstr.Remove(0, noOf);

  // remove trailing white spaces
  wstr = wstr.Strip(TString::kTrailing, ' ');

  // remove last ')'
  Ssiz_t idx=wstr.Last(')');
  wstr.Remove(idx, wstr.Length()-idx);

  // tokenize rest which should have the form 'X0, offset, #Param'
  TObjArray *tok = wstr.Tokenize(",;");
  if (tok == nullptr) {
    std::cerr << ">> PMsrHandler::ParseFourierPhaseParIterVector: **ERROR** tokenize failed." << std::endl;
    error = true;
    return false;
  }

  // check for proper number of expected elements
  if (tok->GetEntries() != 3) {
    std::cerr << ">> PMsrHandler::ParseFourierPhaseParIterVector: **ERROR** wrong syntax for the expected par(X0, offset, #param)." << std::endl;
    error = true;
    delete tok;
    return false;
  }

  Int_t x0, offset, noParam;

  // get X0
  TObjString *ostr = dynamic_cast<TObjString*>(tok->At(0));
  wstr = ostr->GetString();
  if (wstr.IsDigit()) {
    x0 = wstr.Atoi();
  } else {
    std::cerr << ">> PMsrHandler::ParseFourierPhaseParIterVector: **ERROR** X0='" << wstr << "' is not an integer." << std::endl;
    error = true;
    delete tok;
    return false;
  }

  // get offset
  ostr = dynamic_cast<TObjString*>(tok->At(1));
  wstr = ostr->GetString();
  if (wstr.IsDigit()) {
    offset = wstr.Atoi();
  } else {
    std::cerr << ">> PMsrHandler::ParseFourierPhaseParIterVector: **ERROR** offset='" << wstr << "' is not an integer." << std::endl;
    error = true;
    delete tok;
    return false;
  }

  // get noParam
  ostr = dynamic_cast<TObjString*>(tok->At(2));
  wstr = ostr->GetString();
  if (wstr.IsDigit()) {
    noParam = wstr.Atoi();
  } else {
    std::cerr << ">> PMsrHandler::ParseFourierPhaseParIterVector: **ERROR** #Param='" << wstr << "' is not an integer." << std::endl;
    error = true;
    delete tok;
    return false;
  }

  // set the reference phase parameter number for 'parR'
  if (relativePhase)
    fourier.fPhaseRef = x0;
  else
    fourier.fPhaseRef = -1;

  for (Int_t i=0; i<noParam; i++)
    fourier.fPhaseParamNo.push_back(x0 + i*offset);

  // clean up
  if (tok) {
    delete tok;
  }

  return true;
}

//--------------------------------------------------------------------------
// HandleFourierEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>Parses the Fourier block of a msr-file.
 *
 * <b>return:</b>
 * - true if successful
 * - false otherwise
 *
 * \param lines is a list of lines containing the fourier parameter block
 */
Bool_t PMsrHandler::HandleFourierEntry(PMsrLines &lines)
{
  Bool_t error = false;

  if (lines.empty()) // no fourier block present
    return true;

  PMsrFourierStructure fourier;

  InitFourierParameterStructure(fourier);

  fourier.fFourierBlockPresent = true;

  PMsrLines::iterator iter;

  TObjArray *tokens = nullptr;
  TObjString *ostr = nullptr;
  TString str, pcStr=TString("");

  Int_t ival;

  iter = lines.begin();
  while ((iter != lines.end()) && !error) {
    // tokenize line
    tokens = iter->fLine.Tokenize(" \t");
    if (!tokens) {
      std::cerr << std::endl << ">> PMsrHandler::HandleFourierEntry: **SEVERE ERROR** Couldn't tokenize Parameters in line " << iter->fLineNo;
      std::cerr << std::endl << std::endl;
      return false;
    }

    if (iter->fLine.BeginsWith("units", TString::kIgnoreCase)) { // units
      if (tokens->GetEntries() < 2) { // units are missing
        error = true;
        continue;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (!str.CompareTo("gauss", TString::kIgnoreCase)) {
          fourier.fUnits = FOURIER_UNIT_GAUSS;
        } else if (!str.CompareTo("tesla", TString::kIgnoreCase)) {
          fourier.fUnits = FOURIER_UNIT_TESLA;
        } else if (!str.CompareTo("mhz", TString::kIgnoreCase)) {
          fourier.fUnits = FOURIER_UNIT_FREQ;
        } else if (!str.CompareTo("mc/s", TString::kIgnoreCase)) {
          fourier.fUnits = FOURIER_UNIT_CYCLES;
        } else {
          error = true;
          continue;
        }
      }
    } else if (iter->fLine.BeginsWith("fourier_power", TString::kIgnoreCase)) { // fourier power (zero padding)
      if (tokens->GetEntries() < 2) { // fourier power exponent is missing
        error = true;
        continue;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          ival = str.Atoi();
          if ((ival >= 0) && (ival <= 20)) {
            fourier.fFourierPower = ival;
          } else { // fourier power out of range
            error = true;
            continue;
          }
        } else { // fourier power not a number
          error = true;
          continue;
        }
      }
    } else if (iter->fLine.BeginsWith("dc-corrected", TString::kIgnoreCase)) { // dc-corrected
      if (tokens->GetEntries() < 2) { // dc-corrected tag is missing
        error = true;
        continue;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (!str.CompareTo("true", TString::kIgnoreCase) || !str.CompareTo("1")) {
          fourier.fDCCorrected = true;
        } else if (!str.CompareTo("false", TString::kIgnoreCase) || !str.CompareTo("0")) {
          fourier.fDCCorrected = false;
        } else { // unrecognized dc-corrected tag
          error = true;
          continue;
        }
      }
    } else if (iter->fLine.BeginsWith("apodization", TString::kIgnoreCase)) { // apodization
      if (tokens->GetEntries() < 2) { // apodization tag is missing
        error = true;
        continue;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (!str.CompareTo("none", TString::kIgnoreCase)) {
          fourier.fApodization = FOURIER_APOD_NONE;
        } else if (!str.CompareTo("weak", TString::kIgnoreCase)) {
          fourier.fApodization = FOURIER_APOD_WEAK;
        } else if (!str.CompareTo("medium", TString::kIgnoreCase)) {
          fourier.fApodization = FOURIER_APOD_MEDIUM;
        } else if (!str.CompareTo("strong", TString::kIgnoreCase)) {
          fourier.fApodization = FOURIER_APOD_STRONG;
        } else { // unrecognized apodization tag
          error = true;
          continue;
        }
      }
    } else if (iter->fLine.BeginsWith("plot", TString::kIgnoreCase)) { // plot tag
      if (tokens->GetEntries() < 2) { // plot tag is missing
        error = true;
        continue;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (!str.CompareTo("real", TString::kIgnoreCase)) {
          fourier.fPlotTag = FOURIER_PLOT_REAL;
        } else if (!str.CompareTo("imag", TString::kIgnoreCase)) {
          fourier.fPlotTag = FOURIER_PLOT_IMAG;
        } else if (!str.CompareTo("real_and_imag", TString::kIgnoreCase)) {
          fourier.fPlotTag = FOURIER_PLOT_REAL_AND_IMAG;
        } else if (!str.CompareTo("power", TString::kIgnoreCase)) {
          fourier.fPlotTag = FOURIER_PLOT_POWER;
        } else if (!str.CompareTo("phase", TString::kIgnoreCase)) {
          fourier.fPlotTag = FOURIER_PLOT_PHASE;
        } else if (!str.CompareTo("phase_opt_real", TString::kIgnoreCase)) {
          fourier.fPlotTag = FOURIER_PLOT_PHASE_OPT_REAL;
        } else { // unrecognized plot tag
          error = true;
          continue;
        }
      }
    } else if (iter->fLine.BeginsWith("phase", TString::kIgnoreCase)) { // phase
      if (tokens->GetEntries() < 2) { // phase value(s)/par(s) is(are) missing
        error = true;
        continue;
      } else {
        // allowed phase parameter patterns:
        // (i)   phase val [sep val sep val ...] [# comment], val=double, sep=' ,;\t'
        // (ii)  phase parX0 [sep parX1 sep parX2 ...] [# comment], val=double, sep=' ,;\t'
        // (iii) phase par(X0 sep1 offset sep1 #param) [# comment], sep1= ',;'

        // remove potential comment
        TString wstr("");
        RemoveComment(iter->fLine, wstr);

        // check for 'phase val ...'
        Bool_t result = ParseFourierPhaseValueVector(fourier, wstr, error);
        if (error)
          continue;

        // check for 'phase parX0 ...' if not already val are found
        if (!result) {
          result = ParseFourierPhaseParVector(fourier, wstr, error);
          if (error)
            continue;
        }

        // check for 'phase par(X0, offset, #param)' if not already covered by the previous ones
        if (!result) {
          result = ParseFourierPhaseParIterVector(fourier, wstr, error);
        }

        if (!result || error) {
          continue;
        }

        // if parameter vector is given: check that all parameters are within range
        if (fourier.fPhaseParamNo.size() > 0) {
          for (UInt_t i=0; i<fourier.fPhaseParamNo.size(); i++) {
            if (fourier.fPhaseParamNo[i] > fParam.size()) {
              std::cerr << ">> PMsrHandler::HandleFourierEntry: found Fourier parameter entry par" << fourier.fPhaseParamNo[i] << " > #Param = " << fParam.size() << std::endl;
              error = true;
              --iter;
              continue;
            }
          }
        }

        // if parameter vector is given -> fill corresponding phase values
        Double_t phaseRef = 0.0;
        if (fourier.fPhaseParamNo.size() > 0) {
          // check if a relative parameter phase number is set
          if (fourier.fPhaseRef != -1) {
            phaseRef = fParam[fourier.fPhaseRef-1].fValue;
          }
          fourier.fPhase.clear();
          for (UInt_t i=0; i<fourier.fPhaseParamNo.size(); i++) {
            if (fourier.fPhaseRef == fourier.fPhaseParamNo[i]) // reference phase
              fourier.fPhase.push_back(fParam[fourier.fPhaseParamNo[i]-1].fValue);
            else
              fourier.fPhase.push_back(fParam[fourier.fPhaseParamNo[i]-1].fValue+phaseRef);
          }
        }
      }
    } else if (iter->fLine.BeginsWith("range_for_phase_correction", TString::kIgnoreCase)) {
      // keep the string. It can only be handled at the very end of the FOURIER block evaluation
      // since it needs potentially the range input and there is no guaranty this is already
      // available at this point.
      pcStr = iter->fLine;
    } else if (iter->fLine.BeginsWith("range", TString::kIgnoreCase)) { // fourier plot range
      if (tokens->GetEntries() < 3) { // plot range values are missing
        error = true;
        continue;
      } else {
        for (UInt_t i=0; i<2; i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i+1));
          str = ostr->GetString();
          if (str.IsFloat()) {
            fourier.fPlotRange[i] = str.Atof();
          } else {
            error = true;
            continue;
          }
        }
      }
    } else if (!iter->fLine.BeginsWith("fourier", TString::kIgnoreCase) && !iter->fLine.BeginsWith("#") &&
               !iter->fLine.IsWhitespace() && (iter->fLine.Length() != 0)) { // make
      error = true;
      continue;
    }

    // clean up
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }

    ++iter;
  }

  // clean up after error
  if (tokens) {
    delete tokens;
    tokens = nullptr;
  }

  // handle range_for_phase_correction if present
  if ((pcStr.Length() != 0) && !error) {
    // tokenize line
    tokens = pcStr.Tokenize(" \t");

    switch (tokens->GetEntries()) {
    case 2:
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      str = ostr->GetString();
      if (!str.CompareTo("all", TString::kIgnoreCase)) {
        fourier.fRangeForPhaseCorrection[0] = fourier.fPlotRange[0];
        fourier.fRangeForPhaseCorrection[1] = fourier.fPlotRange[1];
      } else {
        error = true;
      }
      break;
    case 3:
      for (UInt_t i=0; i<2; i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i+1));
        str = ostr->GetString();
        if (str.IsFloat()) {
          fourier.fRangeForPhaseCorrection[i] = str.Atof();
        } else {
          error = true;
        }
      }
      break;
    default:
      error = true;
      break;
    }

    // clean up
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }
  }

  if (error) {
    std::cerr << std::endl << ">> PMsrHandler::HandleFourierEntry: **ERROR** in line " << iter->fLineNo << ":";
    std::cerr << std::endl;
    std::cerr << std::endl << ">> " << iter->fLine.Data();
    std::cerr << std::endl;
    std::cerr << std::endl << ">> FOURIER block syntax, parameters in [] are optinal:";
    std::cerr << std::endl;
    std::cerr << std::endl << ">> FOURIER";
    std::cerr << std::endl << ">> [units Gauss | MHz | Mc/s]";
    std::cerr << std::endl << ">> [fourier_power n # n is a number such that zero padding up to 2^n will be used]";
    std::cerr << std::endl << ">>    n=0 means no zero padding";
    std::cerr << std::endl << ">>    0 <= n <= 20 are allowed values";
    std::cerr << std::endl << ">> [dc-corrected true | false]";
    std::cerr << std::endl << ">> [apodization none | weak | medium | strong]";
    std::cerr << std::endl << ">> [plot real | imag | real_and_imag | power | phase | phase_opt_real]";
    std::cerr << std::endl << ">> [phase valList | parList | parIterList [# comment]]";
    std::cerr << std::endl << ">>     valList : val [sep val ... sep val]. sep=' ,;\\t'";
    std::cerr << std::endl << ">>     parList : parX0 [sep parX1 ... sep parX1]";
    std::cerr << std::endl << ">>     parIterList : par(X0,offset,#param), with X0=first parameter number";
    std::cerr << std::endl << ">>                   offset=parameter offset, #param=number of phase parameters.";
    std::cerr << std::endl << ">> [range_for_phase_correction min max | all]";
    std::cerr << std::endl << ">> [range min max]";
    std::cerr << std::endl;
  } else { // save last run found
    fFourier = fourier;
  }

  return !error;
}

//--------------------------------------------------------------------------
// HandlePlotEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>Parses the PLOT block of a msr-file.
 *
 * <b>return:</b>
 * - true if successful
 * - false otherwise
 *
 * \param lines is a list of lines containing the plot block
 */
Bool_t PMsrHandler::HandlePlotEntry(PMsrLines &lines)
{
  Bool_t error = false;

  PMsrPlotStructure param;

  PMsrLines::iterator iter1;
  PMsrLines::iterator iter2;
  TObjArray *tokens = nullptr;
  TObjString *ostr = nullptr;
  TString str;

  if (lines.empty()) {
    std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry(): **WARNING**: There is no PLOT block! Do you really want this?";
    std::cerr << std::endl;
  }

  iter1 = lines.begin();
  while ((iter1 != lines.end()) && !error) {

    // initialize param structure
    param.fPlotType = -1;
    param.fLifeTimeCorrection = false;
    param.fUseFitRanges = false; // i.e. if not overwritten use the range info of the plot block
    param.fLogX = false; // i.e. if not overwritten use linear x-axis
    param.fLogY = false; // i.e. if not overwritten use linear y-axis
    param.fViewPacking = -1; // i.e. if not overwritten use the packing of the run blocks
    param.fRuns.clear();
    param.fTmin.clear();
    param.fTmax.clear();
    param.fYmin.clear();
    param.fYmax.clear();
    param.fRRFPacking = 0; // i.e. if not overwritten it will not be a valid RRF
    param.fRRFFreq = 0.0; // i.e. no RRF whished
    param.fRRFUnit = RRF_UNIT_MHz;
    param.fRRFPhaseParamNo = 0; // initial parameter no = 0 means not a parameter
    param.fRRFPhase = 0.0;

    // find next plot if any is present
    iter2 = iter1;
    ++iter2;
    for ( ; iter2 != lines.end(); ++iter2) {
      if (iter2->fLine.Contains("PLOT"))
        break;
    }

    // handle a single PLOT block
    while ((iter1 != iter2) && !error) {
      TString line = iter1->fLine;
      if (line.First('#') != -1) // remove trailing comment before proceed
        line.Resize(line.First('#'));

      if (line.Contains("PLOT")) { // handle plot header
        tokens = line.Tokenize(" \t");
        if (!tokens) {
          std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize PLOT in line " << iter1->fLineNo;
          std::cerr << std::endl << std::endl;
          return false;
        }
        if (tokens->GetEntries() < 2) { // plot type missing
          error = true;
        } else {
          ostr = dynamic_cast<TObjString*>(tokens->At(1));
          str = ostr->GetString();
          if (str.IsDigit())
            param.fPlotType = str.Atoi();
          else
            error = true;
        }
        // clean up
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
      } else if (line.Contains("lifetimecorrection", TString::kIgnoreCase)) {
        param.fLifeTimeCorrection = true;
      } else if (line.Contains("runs", TString::kIgnoreCase)) { // handle plot runs
        TComplex run;
        PStringNumberList *rl;
        std::string errorMsg;
        PUIntVector runList;
        switch (param.fPlotType) {
          case -1:
            error = true;
            break;
          case MSR_PLOT_SINGLE_HISTO: // like: runs 1 5 13
          case MSR_PLOT_SINGLE_HISTO_RRF:
          case MSR_PLOT_ASYM:
          case MSR_PLOT_BNMR:
          case MSR_PLOT_ASYM_RRF:
          case MSR_PLOT_NON_MUSR:
          case MSR_PLOT_MU_MINUS:
            rl = new PStringNumberList(line.Data());
            if (!rl->Parse(errorMsg, true)) {
              std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize PLOT in line " << iter1->fLineNo;
              std::cerr << std::endl << ">>   Error Message: " << errorMsg;
              std::cerr << std::endl << std::endl;
              return false;
            }
            runList = rl->GetList();
            for (UInt_t i=0; i<runList.size(); i++) {
              run = TComplex(runList[i], -1.0);
              param.fRuns.push_back(run);
            }
            // clean up
            runList.clear();
            delete rl;
            break;
          default:
            error = true;
            break;
        }
      } else if (line.Contains("range ", TString::kIgnoreCase)) { // handle plot range
        // remove previous entries
        param.fTmin.clear();
        param.fTmax.clear();
        param.fYmin.clear();
        param.fYmax.clear();

        tokens = line.Tokenize(" \t");
        if (!tokens) {
          std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize PLOT in line " << iter1->fLineNo;
          std::cerr << std::endl << std::endl;
          return false;
        }
        if ((tokens->GetEntries() != 3) && (tokens->GetEntries() != 5)) {
          error = true;
        } else {

          // handle t_min
          ostr = dynamic_cast<TObjString*>(tokens->At(1));
          str = ostr->GetString();
          if (str.IsFloat())
            param.fTmin.push_back(static_cast<Double_t>(str.Atof()));
          else
            error = true;

          // handle t_max
          ostr = dynamic_cast<TObjString*>(tokens->At(2));
          str = ostr->GetString();
          if (str.IsFloat())
            param.fTmax.push_back(static_cast<Double_t>(str.Atof()));
          else
            error = true;

          if (tokens->GetEntries() == 5) { // y-axis interval given as well

            // handle y_min
            ostr = dynamic_cast<TObjString*>(tokens->At(3));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fYmin.push_back(static_cast<Double_t>(str.Atof()));
            else
              error = true;

            // handle y_max
            ostr = dynamic_cast<TObjString*>(tokens->At(4));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fYmax.push_back(static_cast<Double_t>(str.Atof()));
            else
              error = true;
          }
        }
        // clean up
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
      } else if (line.Contains("sub_ranges", TString::kIgnoreCase)) {
        // remove previous entries
        param.fTmin.clear();
        param.fTmax.clear();
        param.fYmin.clear();
        param.fYmax.clear();

        tokens = line.Tokenize(" \t");
        if (!tokens) {
          std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize PLOT in line " << iter1->fLineNo;
          std::cerr << std::endl << std::endl;
          return false;
        }
        if ((tokens->GetEntries() != static_cast<Int_t>(2*param.fRuns.size() + 1)) && (tokens->GetEntries() != static_cast<Int_t>(2*param.fRuns.size() + 3))) {
          error = true;
        } else {
          // get all the times
          for (UInt_t i=0; i<param.fRuns.size(); i++) {

            // handle t_min
            ostr = dynamic_cast<TObjString*>(tokens->At(2*i+1));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fTmin.push_back(static_cast<Double_t>(str.Atof()));
            else
              error = true;

            // handle t_max
            ostr = dynamic_cast<TObjString*>(tokens->At(2*i+2));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fTmax.push_back(static_cast<Double_t>(str.Atof()));
            else
              error = true;
          }

          // get y-range if present
          if (tokens->GetEntries() == static_cast<Int_t>(2*param.fRuns.size() + 3)) {

            // handle y_min
            ostr = dynamic_cast<TObjString*>(tokens->At(2*param.fRuns.size()+1));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fYmin.push_back(static_cast<Double_t>(str.Atof()));
            else
              error = true;

            // handle y_max
            ostr = dynamic_cast<TObjString*>(tokens->At(2*param.fRuns.size()+2));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fYmax.push_back(static_cast<Double_t>(str.Atof()));
            else
              error = true;
          }
        }

        // clean up
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
      } else if (line.Contains("use_fit_ranges", TString::kIgnoreCase)) {
        param.fUseFitRanges = true;
        // check if y-ranges are given

        tokens = line.Tokenize(" \t");
        if (!tokens) {
          std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize PLOT in line " << iter1->fLineNo;
          std::cerr << std::endl << std::endl;
          return false;
        }

        if (tokens->GetEntries() == 3) { // i.e. use_fit_ranges ymin ymax
          // handle y_min
          ostr = dynamic_cast<TObjString*>(tokens->At(1));
          str = ostr->GetString();
          if (str.IsFloat())
            param.fYmin.push_back(static_cast<Double_t>(str.Atof()));
          else
            error = true;

          // handle y_max
          ostr = dynamic_cast<TObjString*>(tokens->At(2));
          str = ostr->GetString();
          if (str.IsFloat())
            param.fYmax.push_back(static_cast<Double_t>(str.Atof()));
          else
            error = true;
        }

        if ((tokens->GetEntries() != 1) && (tokens->GetEntries() != 3)) {
          std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **WARNING** use_fit_ranges with undefined additional parameters in line " << iter1->fLineNo;
          std::cerr << std::endl << ">>     Will ignore this PLOT block command line, sorry.";
          std::cerr << std::endl << ">>     Proper syntax: use_fit_ranges [ymin ymax]";
          std::cerr << std::endl << ">>     Found: '" << iter1->fLine.Data() << "'" << std::endl;
        }

        // clean up
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
      } else if (iter1->fLine.Contains("logx", TString::kIgnoreCase)) {
        param.fLogX = true;
      } else if (iter1->fLine.Contains("logy", TString::kIgnoreCase)) {
        param.fLogY = true;
      } else if (iter1->fLine.Contains("lifetimecorrection", TString::kIgnoreCase)) {
        param.fLifeTimeCorrection = true;
      } else if (iter1->fLine.Contains("view_packing", TString::kIgnoreCase)) {
        tokens = iter1->fLine.Tokenize(" \t");
        if (!tokens) {
          std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize view_packing in line " << iter1->fLineNo;
          std::cerr << std::endl << std::endl;
          return false;
        }
        if (tokens->GetEntries() != 2) {
          error = true;
        } else {
          ostr = dynamic_cast<TObjString*>(tokens->At(1));
          str = ostr->GetString();
          if (str.IsDigit()) {
            Int_t val = str.Atoi();
            if (val > 0)
              param.fViewPacking = val;
            else
              error = true;
          } else {
            error = true;
          }
        }

        // clean up
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
      } else if (iter1->fLine.Contains("rrf_freq", TString::kIgnoreCase)) {
        // expected entry: rrf_freq value unit
        // allowed units: kHz, MHz, Mc/s, G, T
        tokens = iter1->fLine.Tokenize(" \t");
        if (!tokens) {
          std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize rrf_freq in line " << iter1->fLineNo;
          std::cerr << std::endl << std::endl;
          return false;
        }
        if (tokens->GetEntries() != 3) {
          error = true;
        } else {
          // get rrf frequency
          ostr = dynamic_cast<TObjString*>(tokens->At(1));
          str = ostr->GetString();
          if (str.IsFloat()) {
            param.fRRFFreq = str.Atof();
          } else {
            error = true;
          }
          // get unit
          ostr = dynamic_cast<TObjString*>(tokens->At(2));
          str = ostr->GetString();
          if (str.Contains("kHz", TString::kIgnoreCase))
            param.fRRFUnit = RRF_UNIT_kHz;
          else if (str.Contains("MHz", TString::kIgnoreCase))
            param.fRRFUnit = RRF_UNIT_MHz;
          else if (str.Contains("Mc/s", TString::kIgnoreCase))
            param.fRRFUnit = RRF_UNIT_Mcs;
          else if (str.Contains("G", TString::kIgnoreCase))
            param.fRRFUnit = RRF_UNIT_G;
          else if (str.Contains("T", TString::kIgnoreCase))
            param.fRRFUnit = RRF_UNIT_T;
          else
            error = true;
        }
        // clean up
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
      } else if (iter1->fLine.Contains("rrf_phase", TString::kIgnoreCase)) {
        // expected entry: rrf_phase value. value given in units of degree. or
        //                 rrf_phase parX. where X is the parameter number, e.g. par3
        tokens = iter1->fLine.Tokenize(" \t");
        if (!tokens) {
          std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize rrf_phase in line " << iter1->fLineNo;
          std::cerr << std::endl << std::endl;
          return false;
        }
        if (tokens->GetEntries() != 2) {
          error = true;
        } else {
          // get rrf phase
          ostr = dynamic_cast<TObjString*>(tokens->At(1));
          str = ostr->GetString();
          if (str.IsFloat()) {
            param.fRRFPhase = str.Atof();
          } else {
            if (str.BeginsWith("par", TString::kIgnoreCase)) { // parameter value
              Int_t no = 0;
              if (FilterNumber(str, "par", 0, no)) {
                // check that the parameter is in range
                if (static_cast<Int_t>(fParam.size()) < no) {
                  error = true;
                } else {
                  // keep the parameter number in case parX was used
                  param.fRRFPhaseParamNo = no;
                  // get parameter value
                  param.fRRFPhase = fParam[no-1].fValue;
                }
              }
            } else {
              error = true;
            }
          }
        }
        // clean up
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
      } else if (iter1->fLine.Contains("rrf_packing", TString::kIgnoreCase)) {
        // expected entry: rrf_phase value. value given in units of degree
        tokens = iter1->fLine.Tokenize(" \t");
        if (!tokens) {
          std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize rrf_packing in line " << iter1->fLineNo;
          std::cerr << std::endl << std::endl;
          return false;
        }
        if (tokens->GetEntries() != 2) {
          error = true;
        } else {
          // get rrf packing
          ostr = dynamic_cast<TObjString*>(tokens->At(1));
          str = ostr->GetString();
          if (str.IsDigit()) {
            param.fRRFPacking = str.Atoi();
          } else {
            error = true;
          }
        }
        // clean up
        if (tokens) {
          delete tokens;
          tokens = nullptr;
        }
      } else {
        error = true;
      }

      ++iter1;

    }

    // analyze if the plot block is valid
    Double_t keep;
    if (!error) {
      if (param.fRuns.empty()) { // there was no run tag
        error = true;
      } else { // everything ok
        if ((param.fTmin.size() > 0) || (param.fTmax.size() > 0)) { // if range is given, check that it is ordered properly
          for (UInt_t i=0; i<param.fTmin.size(); i++) {
            if (param.fTmin[i] > param.fTmax[i]) {
              keep = param.fTmin[i];
              param.fTmin[i] = param.fTmax[i];
              param.fTmax[i] = keep;
            }
          }
        }

        if ((param.fYmin.size() > 0) || (param.fYmax.size() > 0)) { // if range is given, check that it is ordered properly
          for (UInt_t i=0; i<param.fYmin.size(); i++) {
            if (param.fYmin[i] > param.fYmax[i]) {
              keep = param.fYmin[i];
              param.fYmin[i] = param.fYmax[i];
              param.fYmax[i] = keep;
            }
          }
        }

        // check RRF entries
        if (param.fRRFFreq != 0.0) {
          if (param.fRRFPacking == 0) {
            std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry(): **ERROR** found RRF frequency but no required RRF packing.";
            std::cerr << std::endl << ">> Will ignore the RRF option.";
            std::cerr << std::endl;
            param.fRRFFreq = 0.0;
          }
        }

        // check if runs listed in the plot block indeed to exist
        for (UInt_t i=0; i<param.fRuns.size(); i++) {
          if (param.fRuns[i] > static_cast<Int_t>(fRuns.size())) {
            std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry(): **WARNING** found plot run number " << param.fRuns[i] << ".";
            std::cerr << std::endl << ">> There are only " << fRuns.size() << " runs present, will ignore this run.";
            std::cerr << std::endl;
            param.fRuns.erase(param.fRuns.begin()+i);
            i--;
          }
          if (param.fRuns[i] == 0) {
            std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry(): **WARNING** found plot run number 0.";
            std::cerr << std::endl << ">> Pot number needs to be > 0. Will ignore this entry.";
            std::cerr << std::endl;
            param.fRuns.erase(param.fRuns.begin()+i);
            i--;
          }
        }

        if (param.fRuns.size() > 0) {
          fPlots.push_back(param);
        } else {
          std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **ERROR** no valid PLOT block entries, will ignore the entire PLOT block.";
          std::cerr << std::endl;
        }
      }
    }

    if (fPlots.size() == 0) {
      error = true;
      std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **ERROR** no valid PLOT block at all present. Fix this first!";
      std::cerr << std::endl;
    }

    if (error) { // print error message
      --iter1;
      std::cerr << std::endl << ">> PMsrHandler::HandlePlotEntry: **ERROR** in line " << iter1->fLineNo << ": " << iter1->fLine.Data();
      std::cerr << std::endl << ">> A PLOT block needs to have the following structure:";
      std::cerr << std::endl;
      std::cerr << std::endl << ">> PLOT <plot_type>";
      std::cerr << std::endl << ">> runs <run_list>";
      std::cerr << std::endl << ">> [range tmin tmax [ymin ymax]]";
      std::cerr << std::endl << ">> [sub_ranges tmin1 tmax1 tmin2 tmax2 ... tminN tmaxN [ymin ymax]";
      std::cerr << std::endl << ">> [logx | logy]";
      std::cerr << std::endl << ">> [use_fit_ranges [ymin ymax]]";
      std::cerr << std::endl << ">> [view_packing n]";
      std::cerr << std::endl;
      std::cerr << std::endl << ">> where <plot_type> is: 0=single histo,";
      std::cerr << std::endl << ">>                       1=RRF single histo,";
      std::cerr << std::endl << ">>                       2=forward-backward asym,";
      std::cerr << std::endl << ">>                       3=forward-backward RRF asym,";
      std::cerr << std::endl << ">>                       4=mu minus single histo,";
      std::cerr << std::endl << ">>                       5=forward-backward beta-NMR asym,";
      std::cerr << std::endl << ">>                       8=non muSR.";
      std::cerr << std::endl << ">> <run_list> is the list of runs, e.g. runs 1 3";
      std::cerr << std::endl << ">> range is optional";
      std::cerr << std::endl << ">> sub_ranges (if present) will plot the N given runs each on its own sub-range";
      std::cerr << std::endl << ">> logx, logy (if present) will present the x-, y-axis in log-scale";
      std::cerr << std::endl << ">> use_fit_ranges (if present) will plot each run on its fit-range";
      std::cerr << std::endl << ">> view_packing n (if present) will bin all data by n (> 0) rather than the binning of the fit";
      std::cerr << std::endl;
    }

    param.fRuns.clear();

  }

  return !error;
}

//--------------------------------------------------------------------------
// HandleStatisticEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>Handles the STATISTIC block of a msr-file.
 *
 * <b>return:</b>
 * - true if successful
 * - false otherwise
 *
 * \param lines is a list of lines containing the statistic block
 */
Bool_t PMsrHandler::HandleStatisticEntry(PMsrLines &lines)
{
  // If msr-file is used for musrFT only, nothing needs to be done here.
  if (fFourierOnly)
    return true;

  if (lines.empty()) {
    std::cerr << std::endl << ">> PMsrHandler::HandleStatisticEntry: **WARNING** There is no STATISTIC block! Do you really want this?";
    std::cerr << std::endl;
    fStatistic.fValid = false;
    return true;
  }

  Char_t str[128];
  Char_t date[128];
  Char_t time[128];
  Int_t  status;
  Double_t dval;
  UInt_t ival;
  TString tstr;
  for (UInt_t i=0; i<lines.size(); i++) {
    // check if the statistic block line is illegal
    tstr = lines[i].fLine;
    tstr.Remove(TString::kLeading, ' ');
    if (tstr.Length() > 0) {
      if (!tstr.BeginsWith("#") && !tstr.BeginsWith("STATISTIC") && !tstr.BeginsWith("chisq") &&
          !tstr.BeginsWith("maxLH") && !tstr.BeginsWith("*** FIT DID NOT CONVERGE ***") &&
          !tstr.BeginsWith("expected chisq") && !tstr.BeginsWith("expected maxLH") &&
          !tstr.BeginsWith("run block")) {
        std::cerr << std::endl << ">> PMsrHandler::HandleStatisticEntry: **SYNTAX ERROR** in line " << lines[i].fLineNo;
        std::cerr << std::endl << ">> '" << lines[i].fLine.Data() << "'";
        std::cerr << std::endl << ">> not a valid STATISTIC block line";
        std::cerr << std::endl << ">> If you do not understand this, just remove the STATISTIC block, musrfit will recreate after fitting";
        std::cerr << std::endl << std::endl;
        return false;
      }
    }

    // filter date and chisq etc from strings
    // extract date and time
    if (lines[i].fLine.Contains("STATISTIC")) {
      status = sscanf(lines[i].fLine.Data(), "STATISTIC --- %s%s", date, time);
      if (status == 2) {
        fStatistic.fDate = TString(date)+TString(", ")+TString(time);
      } else {
        fStatistic.fDate = TString("????-??-??, ??:??:??");
      }
    }
    // extract chisq
    if (lines[i].fLine.Contains("chisq =")) {
      if (lines[i].fLine.Contains("expected")) { // expected chisq
        strncpy(str, lines[i].fLine.Data(), sizeof(str));
        status = sscanf(str+lines[i].fLine.Index("chisq = ")+8, "%lf", &dval);
        if (status == 1) {
          fStatistic.fMinExpected = dval;
        } else {
          fStatistic.fMinExpected = -1.0;
        }
      } else { // chisq
        fStatistic.fValid = true;
        strncpy(str, lines[i].fLine.Data(), sizeof(str));
        status = sscanf(str+lines[i].fLine.Index("chisq = ")+8, "%lf", &dval);
        if (status == 1) {
          fStatistic.fMin = dval;
        } else {
          fStatistic.fMin = -1.0;
        }
      }
    }
    // extract maxLH
    if (lines[i].fLine.Contains("maxLH =")) {
      fStatistic.fValid = true;
      strncpy(str, lines[i].fLine.Data(), sizeof(str));
      status = sscanf(str+lines[i].fLine.Index("maxLH = ")+8, "%lf", &dval);
      if (status == 1) {
        fStatistic.fMin = dval;
      } else {
        fStatistic.fMin = -1.0;
      }
    }
    // extract NDF
    if (lines[i].fLine.Contains(", NDF =")) {
      strncpy(str, lines[i].fLine.Data(), sizeof(str));
      status = sscanf(str+lines[i].fLine.Index(", NDF = ")+8, "%u", &ival);
      if (status == 1) {
        fStatistic.fNdf = ival;
      } else {
        fStatistic.fNdf = 0;
      }
    }
    // keep string
    fStatistic.fStatLines.push_back(lines[i]);
  }

  return true;
}


//--------------------------------------------------------------------------
// GetNoOfFitParameters (public)
//--------------------------------------------------------------------------
/**
 * <p>Calculate the number of fit parameters.
 *
 * \param idx run block index
 */
UInt_t PMsrHandler::GetNoOfFitParameters(UInt_t idx)
{
  UInt_t noOfFitParameters = 0;
  PUIntVector paramVector;
  PUIntVector funVector;
  PUIntVector mapVector;
  TObjArray  *tokens = nullptr;
  TObjString *ostr = nullptr;
  TString     str;
  UInt_t      k, dval;
  Int_t       status, pos;

  // check that idx is valid
  if (idx >= fRuns.size()) {
    std::cerr << std::endl << ">> PMsrHandler::GetNoOfFitParameters() **ERROR** idx=" << idx << ", out of range fRuns.size()=" << fRuns.size();
    std::cerr << std::endl;
    return 0;
  }

  // get N0 parameter, possible parameter number or function (single histo fit)
  if (fRuns[idx].GetNormParamNo() != -1) {
    if (fRuns[idx].GetNormParamNo() < MSR_PARAM_FUN_OFFSET) // parameter
      paramVector.push_back(fRuns[idx].GetNormParamNo());
    else // function
      funVector.push_back(fRuns[idx].GetNormParamNo() - MSR_PARAM_FUN_OFFSET);
  }

  // get background parameter, for the case the background is fitted (single histo fit)
  if (fRuns[idx].GetBkgFitParamNo() != -1)
    paramVector.push_back(fRuns[idx].GetBkgFitParamNo());

  // get alpha parameter if present (asymmetry fit)
  if (fRuns[idx].GetAlphaParamNo() != -1) {
    if (fRuns[idx].GetAlphaParamNo() < MSR_PARAM_FUN_OFFSET) // parameter
      paramVector.push_back(fRuns[idx].GetAlphaParamNo());
    else // function
      funVector.push_back(fRuns[idx].GetAlphaParamNo() - MSR_PARAM_FUN_OFFSET);
  }

  // get beta parameter if present (asymmetry fit)
  if (fRuns[idx].GetBetaParamNo() != -1) {
    if (fRuns[idx].GetBetaParamNo() < MSR_PARAM_FUN_OFFSET) // parameter
      paramVector.push_back(fRuns[idx].GetBetaParamNo());
    else // function
      funVector.push_back(fRuns[idx].GetBetaParamNo() - MSR_PARAM_FUN_OFFSET);
  }

  // go through the theory block and collect parameters
  // possible entries: number -> parameter, fun<number> -> function, map<number> -> maps
  for (UInt_t i=0; i<fTheory.size(); i++) {
    // remove potential comments
    str = fTheory[i].fLine;
    pos = str.Index('#');
    if (pos >= 0)
      str.Resize(pos);
    // tokenize
    tokens = str.Tokenize(" \t");
    if (!tokens) {
      mapVector.clear();
      funVector.clear();
      paramVector.clear();
      return 0;
    }

    for (Int_t j=0; j<tokens->GetEntries(); j++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(j));
      str = ostr->GetString();
      // check for parameter number
      if (str.IsDigit()) {
        dval = str.Atoi();
        paramVector.push_back(dval);
      }

      // check for map
      if (str.Contains("map")) {
        status = sscanf(str.Data(), "map%d", &dval);
        if (status == 1) {
          mapVector.push_back(dval);
        }
      }

      // check for function
      if (str.Contains("fun")) {
        status = sscanf(str.Data(), "fun%d", &dval);
        if (status == 1) {
          funVector.push_back(dval);
        }
      }
    }

    delete tokens;
    tokens = nullptr;
  }

  // go through the function block and collect parameters
  for (UInt_t i=0; i<funVector.size(); i++) {
    // find the proper function in the function block
    for (k=0; k<fFunctions.size(); k++) {
      status = sscanf(fFunctions[k].fLine.Data(), "fun%d", &dval);
      if (status == 1) {
        if (dval == funVector[i])
          break;
      }
    }

    // check if everything has been found at all
    if (k == fFunctions.size()) {
      std::cerr << std::endl << ">> PMsrHandler::GetNoOfFitParameters() **ERROR** couldn't find fun" << funVector[i];
      std::cerr << std::endl << std::endl;

      // clean up
      mapVector.clear();
      funVector.clear();
      paramVector.clear();

      return 0;
    }

    // remove potential comments
    str = fFunctions[k].fLine;
    pos = str.Index('#');
    if (pos >= 0)
      str.Resize(pos);

    // tokenize
    tokens = str.Tokenize(" \t");
    if (!tokens) {
      mapVector.clear();
      funVector.clear();
      paramVector.clear();
      return 0;
    }

    // filter out parameters and maps
    for (Int_t j=0; j<tokens->GetEntries(); j++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(j));
      str = ostr->GetString();

      // check for parameter
      if (str.BeginsWith("par")) {
        status = sscanf(str.Data(), "par%d", &dval);
        if (status == 1)
          paramVector.push_back(dval);
      }

      // check for map
      if (str.BeginsWith("map")) {
        status = sscanf(str.Data(), "map%d", &dval);
        if (status == 1)
          mapVector.push_back(dval);
      }
    }
  }

  // go through the map and collect parameters
  for (UInt_t i=0; i<mapVector.size(); i++) {
    paramVector.push_back(fRuns[idx].GetMap(mapVector[i]-1));
  }

  // eliminated multiple identical entries in paramVector
  PUIntVector param;
  param.push_back(paramVector[0]);
  for (UInt_t i=0; i<paramVector.size(); i++) {
    for (k=0; k<param.size(); k++) {
      if (param[k] == paramVector[i])
        break;
    }
    if (k == param.size())
      param.push_back(paramVector[i]);
  }

  // calculate the number of fit parameters with step != 0
  for (UInt_t i=0; i<param.size(); i++) {
    if (fParam[param[i]-1].fStep != 0.0)
      noOfFitParameters++;
  }

  // cleanup
  param.clear();
  mapVector.clear();
  funVector.clear();
  paramVector.clear();

  return noOfFitParameters;
}

//--------------------------------------------------------------------------
// FillParameterInUse (private)
//--------------------------------------------------------------------------
/**
 * <p>Fills the fParamInUse vector. An element of the vector will be 0 if the fit parameter
 * is <b>not</b> used at all, otherwise it will be > 0.
 *
 * \param theory msr-file THEROY block lines
 * \param funcs msr-file FUNCTIONS block lines
 * \param run msr-file RUN blocks lines
 */
void PMsrHandler::FillParameterInUse(PMsrLines &theory, PMsrLines &funcs, PMsrLines &run)
{
  PIntVector map;
  PIntVector fun;
  PMsrLines::iterator iter;
  TObjArray  *tokens = nullptr;
  TObjString *ostr = nullptr;
  TString    str;
  Int_t      ival, funNo;

  // create and initialize fParamInUse vector
  for (UInt_t i=0; i<fParam.size(); i++)
    fParamInUse.push_back(0);

  // go through all the theory lines ------------------------------------
  for (iter = theory.begin(); iter != theory.end(); ++iter) {
    // remove potential comments
    str = iter->fLine;
    if (str.First('#') != -1)
      str.Resize(str.First('#'));

    // everything to lower case
    str.ToLower();

    // tokenize string
    tokens = str.Tokenize(" \t");
    if (!tokens)
      continue;

    // filter param no, map no, and fun no
    for (Int_t i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString();
      if (str.IsDigit()) { // parameter number
        ival = str.Atoi();
        if ((ival > 0) && (ival < static_cast<Int_t>(fParam.size())+1)) {
          fParamInUse[ival-1]++;
        }
      } else if (str.Contains("map")) { // map
        if (FilterNumber(str, "map", MSR_PARAM_MAP_OFFSET, ival))
          map.push_back(ival-MSR_PARAM_MAP_OFFSET);
      } else if (str.Contains("fun")) { // fun
        if (FilterNumber(str, "fun", MSR_PARAM_FUN_OFFSET, ival))
          fun.push_back(ival-MSR_PARAM_FUN_OFFSET);
      }
    }

    // delete tokens
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }
  }

  // go through all the function lines: 1st time -----------------------------
  for (iter = funcs.begin(); iter != funcs.end(); ++iter) {
    // remove potential comments
    str = iter->fLine;
    if (str.First('#') != -1)
      str.Resize(str.First('#'));

    // everything to lower case
    str.ToLower();

    tokens = str.Tokenize(" /t");
    if (!tokens)
      continue;

    // filter fun number
    ostr = dynamic_cast<TObjString*>(tokens->At(0));
    str = ostr->GetString();
    if (!FilterNumber(str, "fun", MSR_PARAM_FUN_OFFSET, funNo))
      continue;
    funNo -= MSR_PARAM_FUN_OFFSET;

    // check if fun number is used, and if yes, filter parameter numbers and maps
    TString sstr;
    for (UInt_t i=0; i<fun.size(); i++) {
      if (fun[i] == funNo) { // function number found
        // filter for parX
        sstr = iter->fLine;
        Char_t sval[128];
        while (sstr.Index("par") != -1) {
          memset(sval, 0, sizeof(sval));
          sstr = &sstr[sstr.Index("par")+3]; // trunc sstr
          for (Int_t j=0; j<sstr.Sizeof(); j++) {
            if (!isdigit(sstr[j]))
              break;
            sval[j] = sstr[j];
          }
          sscanf(sval, "%d", &ival);
          fParamInUse[ival-1]++;
        }

        // filter for mapX
        sstr = iter->fLine;
        while (sstr.Index("map") != -1) {
          memset(sval, 0, sizeof(sval));
          sstr = &sstr[sstr.Index("map")+3]; // trunc sstr
          for (Int_t j=0; j<sstr.Sizeof(); j++) {
            if (!isdigit(sstr[j]))
              break;
            sval[j] = sstr[j];
          }
          sscanf(sval, "%d", &ival);
          // check if map value already in map, otherwise add it
          if (ival > 0) {
            UInt_t pos;
            for (pos=0; pos<map.size(); pos++) {
              if (ival == map[pos])
                break;
            }
            if (pos == map.size()) { // new map value
              map.push_back(ival);
            }
          }
        }
        break; // since function was found, break the loop
      }
    }

    // delete tokens
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }
  }

  // go through all the run block lines -------------------------------------
  for (iter = run.begin(); iter != run.end(); ++iter) {
    // remove potential comments
    str = iter->fLine;
    if (str.First('#') != -1)
      str.Resize(str.First('#'));

    // everything to lower case
    str.ToLower();

    // handle everything but the maps
    if (str.Contains("alpha") || str.Contains("beta") ||
        str.Contains("alpha2") || str.Contains("beta2") ||
        str.Contains("norm") || str.Contains("backgr.fit") ||
        str.Contains("lifetime ")) {
      // tokenize string
      tokens = str.Tokenize(" \t");
      if (!tokens)
        continue;
      if (tokens->GetEntries()<2)
        continue;

      ostr = dynamic_cast<TObjString*>(tokens->At(1)); // parameter number or function
      str = ostr->GetString();
      // check if parameter number
      if (str.IsDigit()) {
        ival = str.Atoi();
        fParamInUse[ival-1]++;
      }
      // check if fun
      if (str.Contains("fun")) {
        if (FilterNumber(str, "fun", MSR_PARAM_FUN_OFFSET, ival)) {
          fun.push_back(ival-MSR_PARAM_FUN_OFFSET);
        }
      }

      // delete tokens
      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
    }

    // handle the maps
    if (str.Contains("map")) {
      // tokenize string
      tokens = str.Tokenize(" \t");
      if (!tokens)
        continue;

      // get the parameter number via map
      for (UInt_t i=0; i<map.size(); i++) {
        if (map[i] == 0)
          continue;
        if (map[i] < tokens->GetEntries()) {
          ostr = dynamic_cast<TObjString*>(tokens->At(map[i]));
          str = ostr->GetString();
          if (str.IsDigit()) {
            ival = str.Atoi();
            if (ival > 0) {
              fParamInUse[ival-1]++; // this is OK since map is ranging from 1 ..
            }
          }
        }
      }

      // delete tokens
      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
    }
  }

  // go through all the function lines: 2nd time -----------------------------
  for (iter = funcs.begin(); iter != funcs.end(); ++iter) {
    // remove potential comments
    str = iter->fLine;
    if (str.First('#') != -1)
      str.Resize(str.First('#'));

    // everything to lower case
    str.ToLower();

    tokens = str.Tokenize(" /t");
    if (!tokens)
      continue;

    // filter fun number
    ostr = dynamic_cast<TObjString*>(tokens->At(0));
    str = ostr->GetString();
    if (!FilterNumber(str, "fun", MSR_PARAM_FUN_OFFSET, funNo))
      continue;
    funNo -= MSR_PARAM_FUN_OFFSET;

    // check if fun number is used, and if yes, filter parameter numbers and maps
    TString sstr;
    for (UInt_t i=0; i<fun.size(); i++) {
      if (fun[i] == funNo) { // function number found
        // filter for parX
        sstr = iter->fLine;
        Char_t sval[128];
        while (sstr.Index("par") != -1) {
          memset(sval, 0, sizeof(sval));
          sstr = &sstr[sstr.Index("par")+3]; // trunc sstr
          for (Int_t j=0; j<sstr.Sizeof(); j++) {
            if (!isdigit(sstr[j]))
              break;
            sval[j] = sstr[j];
          }
          sscanf(sval, "%d", &ival);
          fParamInUse[ival-1]++;
        }

        // filter for mapX
        sstr = iter->fLine;
        while (sstr.Index("map") != -1) {
          memset(sval, 0, sizeof(sval));
          sstr = &sstr[sstr.Index("map")+3]; // trunc sstr
          for (Int_t j=0; j<sstr.Sizeof(); j++) {
            if (!isdigit(sstr[j]))
              break;
            sval[j] = sstr[j];
          }
          sscanf(sval, "%d", &ival);
          // check if map value already in map, otherwise add it
          if (ival > 0) {
            UInt_t pos;
            for (pos=0; pos<map.size(); pos++) {
              if (ival == map[pos])
                break;
            }
            if (static_cast<UInt_t>(pos) == map.size()) { // new map value
              map.push_back(ival);
            }
          }
        }
      }
    }

    // delete tokens
    if (tokens) {
      delete tokens;
      tokens = nullptr;
    }
  }

  // go through all the run block lines 2nd time to filter remaining maps
  for (iter = run.begin(); iter != run.end(); ++iter) {
    // remove potential comments
    str = iter->fLine;
    if (str.First('#') != -1)
      str.Resize(str.First('#'));

    // everything to lower case
    str.ToLower();

    // handle the maps
    if (str.Contains("map")) {
      // tokenize string
      tokens = str.Tokenize(" \t");
      if (!tokens)
        continue;

      // get the parameter number via map
      for (UInt_t i=0; i<map.size(); i++) {
        if (map[i] == 0)
          continue;
        if (map[i] < tokens->GetEntries()) {
          ostr = dynamic_cast<TObjString*>(tokens->At(map[i]));
          str = ostr->GetString();
          if (str.IsDigit()) {
            ival = str.Atoi();
            if (ival > 0) {
              fParamInUse[ival-1]++; // this is OK since map is ranging from 1 ..
            }
          }
        }
      }

      // delete tokens
      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
    }
  }

  // if unused parameters are present, set the step value to 0.0
  for (UInt_t i=0; i<fParam.size(); i++) {
    if (!ParameterInUse(i)) {
      if (fParam[i].fStep != 0.0) {
        std::cerr << std::endl << ">> **WARNING** : Parameter No " << i+1 << " is not used at all, will fix it" << std::endl;
        fParam[i].fStep = 0.0;
      }
    }
  }

  // clean up
  map.clear();
  fun.clear();
}


//--------------------------------------------------------------------------
// CheckRunBlockIntegrity (public)
//--------------------------------------------------------------------------
/**
 * <p>Checks the consistency of each RUN block, i.e. are the necessary parameters
 * set and present, etc.
 *
 * <b>return:</b>
 * - true if the RUN blocks fulfill the minimal requirements.
 * - false otherwise
 */
Bool_t PMsrHandler::CheckRunBlockIntegrity()
{
  // go through all the present RUN blocks
  Int_t fitType = 0;
  for (UInt_t i=0; i<fRuns.size(); i++) {
    // check if fittype is defined
    fitType = fRuns[i].GetFitType();
    if (fitType == -1) { // fittype not given in the run block
      fitType = fGlobal.GetFitType();
      if (fitType == -1) {
        std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** fittype is neither defined in RUN block number " << i+1 << ", nor in the GLOBAL block." << std::endl;
        return false;
      }
    }

    // check for the different fittypes differently
    Int_t detectorGroups = 1; // number of detectors tp be grouped
    switch (fitType) {
      case PRUN_SINGLE_HISTO:
        // check of norm is present
        if ((fRuns[i].GetNormParamNo() == -1) && !fFourierOnly) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   Norm parameter number not defined. Necessary for single histogram fits." << std::endl;
          return false;
        }
        if (!fFourierOnly) { // next check NOT needed for Fourier only
          // check if norm parameter is given that it is either a valid function of a fit parameter present
          if (fRuns[i].GetNormParamNo() < MSR_PARAM_FUN_OFFSET) { // parameter number
            // check that norm parameter number is not larger than the number of parameters
            if (fRuns[i].GetNormParamNo() > static_cast<Int_t>(fParam.size())) {
              std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
              std::cerr << std::endl << ">>   Norm parameter number " << fRuns[i].GetNormParamNo() << " is larger than the number of fit parameters (" << fParam.size() << ").";
              std::cerr << std::endl << ">>   Consider to check the manual ;-)" << std::endl;
              return false;
            }
          } else { // function norm
            if (fRuns[i].GetNormParamNo()-MSR_PARAM_FUN_OFFSET > GetNoOfFuncs()) {
              std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
              std::cerr << std::endl << ">>   Norm parameter function number " << fRuns[i].GetNormParamNo()-MSR_PARAM_FUN_OFFSET << " is larger than the number of functions (" << GetNoOfFuncs() << ").";
              std::cerr << std::endl << ">>   Consider to check the manual ;-)" << std::endl;
              return false;
            }
          }
        }
        // check that there is a forward parameter number
        if (fRuns[i].GetForwardHistoNo() == -1) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   forward parameter number not defined. Necessary for single histogram fits." << std::endl;
          return false;
        }
        if ((fRuns[i].GetNormParamNo() > static_cast<Int_t>(fParam.size())) && !fFourierOnly) {
          // check if forward histogram number is a function
          if (fRuns[i].GetNormParamNo() - MSR_PARAM_FUN_OFFSET > static_cast<Int_t>(fParam.size())) {
            std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
            std::cerr << std::endl << ">>   forward histogram number " << fRuns[i].GetNormParamNo() << " is larger than the number of fit parameters (" << fParam.size() << ").";
            std::cerr << std::endl << ">>   Consider to check the manual ;-)" << std::endl;
            return false;
          }
        }
        // check fit range
        if (!fRuns[i].IsFitRangeInBin() && !fFourierOnly) { // fit range given as times in usec (RUN block)
          if ((fRuns[i].GetFitRange(0) == PMUSR_UNDEFINED) || (fRuns[i].GetFitRange(1) == PMUSR_UNDEFINED)) { // check fit range in RUN block
            if (!fGlobal.IsFitRangeInBin()) { // fit range given as times in usec (GLOBAL block)
              if ((fGlobal.GetFitRange(0) == PMUSR_UNDEFINED) || (fGlobal.GetFitRange(1) == PMUSR_UNDEFINED)) { // check fit range in GLOBAL block
                std::cerr << std::endl << "PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
                std::cerr << std::endl << "  Fit range is not defined. Necessary for single histogram fits." << std::endl;
                return false;
              }
            }
          }
        }
        // check number of T0's provided
        detectorGroups = fRuns[i].GetForwardHistoNoSize();
        if ((fRuns[i].GetT0BinSize() > detectorGroups) || (fGlobal.GetT0BinSize() > detectorGroups)) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          if (fRuns[i].GetT0BinSize() > detectorGroups)
            std::cerr << std::endl << ">>   In RUN Block " << i+1 << ": found " << fRuns[i].GetT0BinSize() << " T0 entries. Expecting max. " << detectorGroups << " entries.";
          if (fGlobal.GetT0BinSize() > 1)
            std::cerr << std::endl << ">>   In GLOBAL block: found " << fGlobal.GetT0BinSize() << " T0 entries. Expecting max. " << detectorGroups << " entries. Needs to be fixed.";
          std::cerr << std::endl << ">>   In case you added runs, please use the key word 'addt0' to add the t0's of the runs to be added." << std::endl;
          return false;
        }

        // check packing
        if ((fRuns[i].GetPacking() == -1) && (fGlobal.GetPacking() == -1)) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **WARNING** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   Packing is neither defined here, nor in the GLOBAL block, will set it to 1." << std::endl;
          fRuns[i].SetPacking(1);
        }
        break;
      case PRUN_SINGLE_HISTO_RRF:
        // check that there is a forward parameter number
        if (fRuns[i].GetForwardHistoNo() == -1) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   forward parameter number not defined. Necessary for single histogram RRF fits." << std::endl;
          return false;
        }
        if ((fRuns[i].GetNormParamNo() > static_cast<Int_t>(fParam.size())) && !fFourierOnly) {
          // check if forward histogram number is a function
          if (fRuns[i].GetNormParamNo() - MSR_PARAM_FUN_OFFSET > static_cast<Int_t>(fParam.size())) {
            std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
            std::cerr << std::endl << ">>   forward histogram number " << fRuns[i].GetNormParamNo() << " is larger than the number of fit parameters (" << fParam.size() << ").";
            std::cerr << std::endl << ">>   Consider to check the manual ;-)" << std::endl;
            return false;
          }
        }
        // check fit range
        if (!fRuns[i].IsFitRangeInBin() && !fFourierOnly) { // fit range given as times in usec (RUN block)
          if ((fRuns[i].GetFitRange(0) == PMUSR_UNDEFINED) || (fRuns[i].GetFitRange(1) == PMUSR_UNDEFINED)) { // check fit range in RUN block
            if (!fGlobal.IsFitRangeInBin()) { // fit range given as times in usec (GLOBAL block)
              if ((fGlobal.GetFitRange(0) == PMUSR_UNDEFINED) || (fGlobal.GetFitRange(1) == PMUSR_UNDEFINED)) { // check fit range in GLOBAL block
                std::cerr << std::endl << "PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
                std::cerr << std::endl << "  Fit range is not defined. Necessary for single histogram fits." << std::endl;
                return false;
              }
            }
          }
        }
        // check number of T0's provided
        detectorGroups = fRuns[i].GetForwardHistoNoSize();
        if ((fRuns[i].GetT0BinSize() > detectorGroups) || (fGlobal.GetT0BinSize() > detectorGroups)) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          if (fRuns[i].GetT0BinSize() > detectorGroups)
            std::cerr << std::endl << ">>   In RUN Block " << i+1 << ": found " << fRuns[i].GetT0BinSize() << " T0 entries. Expecting max. " << detectorGroups << " entries.";
          if (fGlobal.GetT0BinSize() > 1)
            std::cerr << std::endl << ">>   In GLOBAL block: found " << fGlobal.GetT0BinSize() << " T0 entries. Expecting max. " << detectorGroups << " entries. Needs to be fixed.";
          std::cerr << std::endl << ">>   In case you added runs, please use the key word 'addt0' to add the t0's of the runs to be added." << std::endl;
          return false;
        }
        // check that RRF frequency is given
        if (fGlobal.GetRRFUnitTag() == RRF_UNIT_UNDEF) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** no RRF frequency found in the GLOBAL block." << std::endl;
          return false;
        }
        // check that RRF packing is given
        if (fGlobal.GetRRFPacking() == -1) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** no RRF packing found in the GLOBAL block." << std::endl;
          return false;
        }
        break;
      case PRUN_ASYMMETRY:
        // check alpha
        if ((fRuns[i].GetAlphaParamNo() == -1) && !fFourierOnly) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   alpha parameter number missing which is needed for an asymmetry fit.";
          std::cerr << std::endl << ">>   Consider to check the manual ;-)" << std::endl;
          return false;
        }
        // check that there is a forward parameter number
        if (fRuns[i].GetForwardHistoNo() == -1) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   forward histogram number not defined. Necessary for asymmetry fits." << std::endl;
          return false;
        }
        // check that there is a backward parameter number
        if (fRuns[i].GetBackwardHistoNo() == -1) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   backward histogram number not defined. Necessary for asymmetry fits." << std::endl;
          return false;
        }
        // check fit range
        if (!fRuns[i].IsFitRangeInBin()) { // fit range given as times in usec
          if ((fRuns[i].GetFitRange(0) == PMUSR_UNDEFINED) || (fRuns[i].GetFitRange(1) == PMUSR_UNDEFINED)) {
            if ((fGlobal.GetFitRange(0) == PMUSR_UNDEFINED) || (fGlobal.GetFitRange(1) == PMUSR_UNDEFINED)) {
              std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
              std::cerr << std::endl << ">>   Fit range is not defined, also NOT present in the GLOBAL block. Necessary for asymmetry fits." << std::endl;
              return false;
            }
          }
        }
        // check number of T0's provided
        detectorGroups = 2*fRuns[i].GetForwardHistoNoSize();
        if (detectorGroups < 2*fRuns[i].GetBackwardHistoNoSize())
          detectorGroups = 2*fRuns[i].GetBackwardHistoNoSize();
        if ((fRuns[i].GetT0BinSize() > detectorGroups) || (fGlobal.GetT0BinSize() > detectorGroups)) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          if (fRuns[i].GetT0BinSize() > detectorGroups)
            std::cerr << std::endl << ">>   In RUN Block " << i+1 << ": found " << fRuns[i].GetT0BinSize() << " T0 entries. Expecting max. " << detectorGroups << " entries.";
          if (fGlobal.GetT0BinSize() > 1)
            std::cerr << std::endl << ">>   In GLOBAL block: found " << fGlobal.GetT0BinSize() << " T0 entries. Expecting max. " << detectorGroups << " entries. Needs to be fixed.";
          std::cerr << std::endl << ">>   In case you added runs, please use the key word 'addt0' to add the t0's of the runs to be added." << std::endl;
          return false;
        }
        // check packing
        if ((fRuns[i].GetPacking() == -1) && (fGlobal.GetPacking() == -1)) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **WARNING** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   Packing is neither defined here, nor in the GLOBAL block, will set it to 1." << std::endl;
          fRuns[i].SetPacking(1);
        }
        break;
      case PRUN_ASYMMETRY_BNMR:
        // check alpha
        // if ((fRuns[i].GetAlphaParamNo() == -1) && !fFourierOnly) {
        //   std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
        //   std::cerr << std::endl << ">>   alpha parameter number missing which is needed for an asymmetry fit.";
        //   std::cerr << std::endl << ">>   Consider to check the manual ;-)" << std::endl;
        //   return false;
        // }
        // check that there is a forward parameter number
        if (fRuns[i].GetForwardHistoNo() == -1) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   forward histogram number not defined. Necessary for asymmetry fits." << std::endl;
          return false;
        }
        // check that there is a backward parameter number
        if (fRuns[i].GetBackwardHistoNo() == -1) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   backward histogram number not defined. Necessary for asymmetry fits." << std::endl;
          return false;
        }
        // check fit range
        if (!fRuns[i].IsFitRangeInBin()) { // fit range given as times in usec
          if ((fRuns[i].GetFitRange(0) == PMUSR_UNDEFINED) || (fRuns[i].GetFitRange(1) == PMUSR_UNDEFINED)) {
            if ((fGlobal.GetFitRange(0) == PMUSR_UNDEFINED) || (fGlobal.GetFitRange(1) == PMUSR_UNDEFINED)) {
              std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
              std::cerr << std::endl << ">>   Fit range is not defined, also NOT present in the GLOBAL block. Necessary for asymmetry fits." << std::endl;
              return false;
            }
          }
        }
        // check number of T0's provided
        if ((fRuns[i].GetT0BinSize() > 2*fRuns[i].GetForwardHistoNoSize()) &&
            (fGlobal.GetT0BinSize() > 2*fRuns[i].GetForwardHistoNoSize())) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   Found " << fRuns[i].GetT0BinSize() << " T0 entries. Expecting only " << 2*fRuns[i].GetForwardHistoNoSize() << " in forward. Needs to be fixed." << std::endl;
          std::cerr << std::endl << ">>   In GLOBAL block: " << fGlobal.GetT0BinSize() << " T0 entries. Expecting only " << 2*fRuns[i].GetForwardHistoNoSize() << ". Needs to be fixed." << std::endl;
          return false;
        }
        if ((fRuns[i].GetT0BinSize() > 2*fRuns[i].GetBackwardHistoNoSize()) &&
            (fGlobal.GetT0BinSize() > 2*fRuns[i].GetBackwardHistoNoSize())) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   Found " << fRuns[i].GetT0BinSize() << " T0 entries. Expecting only " << 2*fRuns[i].GetBackwardHistoNoSize() << " in backward. Needs to be fixed." << std::endl;
          std::cerr << std::endl << ">>   In GLOBAL block: " << fGlobal.GetT0BinSize() << " T0 entries. Expecting only " << 2*fRuns[i].GetBackwardHistoNoSize() << ". Needs to be fixed." << std::endl;
          return false;
        }
        // check packing
        if ((fRuns[i].GetPacking() == -1) && (fGlobal.GetPacking() == -1)) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **WARNING** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   Packing is neither defined here, nor in the GLOBAL block, will set it to 1." << std::endl;
          fRuns[i].SetPacking(1);
        }
        break;
      case PRUN_ASYMMETRY_RRF:
        // check alpha
        if ((fRuns[i].GetAlphaParamNo() == -1) && !fFourierOnly) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   alpha parameter number missing which is needed for an asymmetry RRF fit.";
          std::cerr << std::endl << ">>   Consider to check the manual ;-)" << std::endl;
          return false;
        }
        // check that there is a forward parameter number
        if (fRuns[i].GetForwardHistoNo() == -1) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   forward histogram number not defined. Necessary for asymmetry RRF fits." << std::endl;
          return false;
        }
        // check that there is a backward parameter number
        if (fRuns[i].GetBackwardHistoNo() == -1) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   backward histogram number not defined. Necessary for asymmetry RRF fits." << std::endl;
          return false;
        }
        // check fit range
        if (!fRuns[i].IsFitRangeInBin()) { // fit range given as times in usec
          if ((fRuns[i].GetFitRange(0) == PMUSR_UNDEFINED) || (fRuns[i].GetFitRange(1) == PMUSR_UNDEFINED)) {
            if ((fGlobal.GetFitRange(0) == PMUSR_UNDEFINED) || (fGlobal.GetFitRange(1) == PMUSR_UNDEFINED)) {
              std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
              std::cerr << std::endl << ">>   Fit range is not defined, also NOT present in the GLOBAL block. Necessary for asymmetry RRF fits." << std::endl;
              return false;
            }
          }
        }
        // check number of T0's provided
        detectorGroups = 2*fRuns[i].GetForwardHistoNoSize();
        if (detectorGroups < 2*fRuns[i].GetBackwardHistoNoSize())
          detectorGroups = 2*fRuns[i].GetBackwardHistoNoSize();
        if ((fRuns[i].GetT0BinSize() > detectorGroups) || (fGlobal.GetT0BinSize() > detectorGroups)) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          if (fRuns[i].GetT0BinSize() > detectorGroups)
            std::cerr << std::endl << ">>   In RUN Block " << i+1 << ": found " << fRuns[i].GetT0BinSize() << " T0 entries. Expecting max. " << detectorGroups << " entries.";
          if (fGlobal.GetT0BinSize() > 1)
            std::cerr << std::endl << ">>   In GLOBAL block: found " << fGlobal.GetT0BinSize() << " T0 entries. Expecting max. " << detectorGroups << " entries. Needs to be fixed.";
          std::cerr << std::endl << ">>   In case you added runs, please use the key word 'addt0' to add the t0's of the runs to be added." << std::endl;
          return false;
        }
        // check that RRF frequency is given
        if (fGlobal.GetRRFUnitTag() == RRF_UNIT_UNDEF) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** no RRF frequency found in the GLOBAL block." << std::endl;
          return false;
        }
        // check that RRF packing is given
        if (fGlobal.GetRRFPacking() == -1) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** no RRF packing found in the GLOBAL block." << std::endl;
          return false;
        }
        break;
      case PRUN_MU_MINUS:
        // needs eventually to be implemented
        break;
      case PRUN_NON_MUSR:
        // check xy-data
        if ((fRuns[i].GetXDataIndex() == -1) && (fRuns[i].GetXDataLabel()->Length() == 0)) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
          std::cerr << std::endl << ">>   xy-data is missing. Necessary for non muSR fits." << std::endl;
          return false;
        }
        // check fit range
        if ((fRuns[i].GetFitRange(0) == PMUSR_UNDEFINED) || (fRuns[i].GetFitRange(1) == PMUSR_UNDEFINED)) {
          if ((fGlobal.GetFitRange(0) == PMUSR_UNDEFINED) || (fGlobal.GetFitRange(1) == PMUSR_UNDEFINED)) {
            std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** in RUN block number " << i+1;
            std::cerr << std::endl << ">>   Fit range is not defined, neither in the RUN block, nor in the GLOBAL block.";
            std::cerr << std::endl << ">>   Necessary for non muSR fits." << std::endl;
            return false;
          }
        }
        // check packing
        if (fRuns[i].GetPacking() == -1) {
          if (fGlobal.GetPacking() == -1) {
            std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **WARNING** in RUN block number " << i+1;
            std::cerr << std::endl << ">>   Packing is not defined, will set it to 1." << std::endl;
            fRuns[i].SetPacking(1);
          }
        }
        break;
      default:
        std::cerr << std::endl << ">> PMsrHandler::CheckRunBlockIntegrity(): **ERROR** fittype " << fitType << " undefined." << std::endl;
        return false;
    }

  }

  return true;
}

//--------------------------------------------------------------------------
// CheckUniquenessOfParamNames (public)
//--------------------------------------------------------------------------
/**
 * <p>Checks if all the fit parameters are unique. If not parX, parY will show
 * the first occurence of equal fit parameter names.
 *
 * <b>return:</b>
 * - true if the fit parameter names are unique.
 * - false otherwise
 *
 * \param parX index of the 1st fit parameter name for which there is a counter part.
 * \param parY index of the counter part fit parameter name.
 */
Bool_t PMsrHandler::CheckUniquenessOfParamNames(UInt_t &parX, UInt_t &parY)
{
  Bool_t unique = true;

  for (UInt_t i=0; i<fParam.size()-1; i++) {
    for (UInt_t j=i+1; j<fParam.size(); j++) {
      if (fParam[i].fName.CompareTo(fParam[j].fName) == 0) { // equal
        unique = false;
        parX = i;
        parY = j;
        break;
      }
    }
  }

  return unique;
}

//--------------------------------------------------------------------------
// CheckMaps (public)
//--------------------------------------------------------------------------
/**
 * <p>Checks if map entries found in the theory- or function-block are also
 * present in the run-block.
 *
 * <b>return:</b>
 * - true if maps or OK
 * - false otherwise
 */
Bool_t PMsrHandler::CheckMaps()
{
  Bool_t result = true;

  PIntVector mapVec;
  PIntVector mapBlock;
  PIntVector mapLineNo;

  TObjArray  *tokens = nullptr;
  TObjString *ostr = nullptr;
  TString    str;

  Int_t no;

  // check if map is present in the theory-block
  for (UInt_t i=0; i<fTheory.size(); i++) {
    if (fTheory[i].fLine.Contains("map", TString::kIgnoreCase)) {
      // map found hence filter out map number
      tokens = fTheory[i].fLine.Tokenize(" \t");
      for (Int_t j=0; j<tokens->GetEntries(); j++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(j));
        str = ostr->GetString();
        if (str.Contains("map", TString::kIgnoreCase)) {
          if (FilterNumber(str, "map", MSR_PARAM_MAP_OFFSET, no)) {
            mapVec.push_back(no);
            mapBlock.push_back(0); // 0 = theory-block
            mapLineNo.push_back(fTheory[i].fLineNo);
          }
        }
      }
      // clean up tokens
      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
    }
  }

  // check if map is present in the function-block
  for (UInt_t i=0; i<fFunctions.size(); i++) {
    if (fFunctions[i].fLine.Contains("map", TString::kIgnoreCase)) {
      // map found hence filter out map number
      tokens = fFunctions[i].fLine.Tokenize(" \t");
      for (Int_t j=0; j<tokens->GetEntries(); j++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(j));
        str = ostr->GetString();
        if (str.Contains("map", TString::kIgnoreCase)) {
          if (FilterNumber(str, "map", MSR_PARAM_MAP_OFFSET, no)) {
            mapVec.push_back(no);
            mapBlock.push_back(1); // 1 = theory-block
            mapLineNo.push_back(fTheory[i].fLineNo);
          }
        }
      }
      // clean up tokens
      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
    }
  }

  // check if present maps are found in the run-block
  Bool_t found;
  for (UInt_t i=0; i<mapVec.size(); i++) { // loop over found maps in theory- and function-block
    found = false;
    for (UInt_t j=0; j<fRuns.size(); j++) { // loop over all run-blocks
      if ((mapVec[i]-MSR_PARAM_MAP_OFFSET-1 < static_cast<Int_t>(fRuns[j].GetMap()->size())) &&
          (mapVec[i]-MSR_PARAM_MAP_OFFSET-1 >= 0)) { // map value smaller than run-block map length
        if (fRuns[j].GetMap(mapVec[i]-MSR_PARAM_MAP_OFFSET-1) != 0) { // map value in the run-block != 0
          found = true;
          break;
        }
      }
    }
    if (!found) { // map not found
      result = false;
      std::cerr << std::endl << ">> PMsrHandler::CheckMaps: **ERROR** map" << mapVec[i]-MSR_PARAM_MAP_OFFSET << " found in the ";
      if (mapBlock[i] == 0)
        std::cerr << "theory-block ";
      else
        std::cerr << "functions-block ";
      std::cerr << "in line " << mapLineNo[i] << " is not present in the run-block!";
      std::cerr << std::endl;
      if (mapVec[i]-MSR_PARAM_MAP_OFFSET == 0) {
        std::cerr << std::endl << ">> by the way: map must be > 0 ...";
        std::cerr << std::endl;
      }
    }
  }

  // clean up
  mapVec.clear();
  mapBlock.clear();
  mapLineNo.clear();

  return result;
}

//--------------------------------------------------------------------------
// CheckFuncs (public)
//--------------------------------------------------------------------------
/**
 * <p>Checks if fun entries found in the theory- and run-block are also
 * present in the functions-block.
 *
 * <b>return:</b>
 * - true if fun entries or present in the FUNCTIONS block
 * - false otherwise
 */
Bool_t PMsrHandler::CheckFuncs()
{
  Bool_t result = true;

  if (fFourierOnly)
    return result;

  PIntVector funVec;
  PIntVector funBlock;
  PIntVector funLineBlockNo;

  TObjArray  *tokens = nullptr;
  TObjString *ostr = nullptr;
  TString    str;

  Int_t no;

  // check if func is present in the theory-block
  for (UInt_t i=0; i<fTheory.size(); i++) {
    if (fTheory[i].fLine.Contains("fun", TString::kIgnoreCase)) {
      // func found hence filter out func number
      tokens = fTheory[i].fLine.Tokenize(" \t");
      for (Int_t j=0; j<tokens->GetEntries(); j++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(j));
        str = ostr->GetString();
        if (str.Contains("fun", TString::kIgnoreCase)) {
          if (FilterNumber(str, "fun", MSR_PARAM_FUN_OFFSET, no)) {
            funVec.push_back(no);
            funBlock.push_back(0); // 0 = theory-block
            funLineBlockNo.push_back(fTheory[i].fLineNo);
          }
        }
      }
      // clean up tokens
      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
    }
  }

  // check if func is present in the run-block
  for (UInt_t i=0; i<fRuns.size(); i++) {
    if (fRuns[i].GetNormParamNo() >= MSR_PARAM_FUN_OFFSET) { // function found
      funVec.push_back(fRuns[i].GetNormParamNo());
      funBlock.push_back(1); // 1 = run-block
      funLineBlockNo.push_back(i+1);
    }
  }

  // check if present funcs are found in the functions-block
  Bool_t found;
  for (UInt_t i=0; i<funVec.size(); i++) { // loop over found funcs in theory- and run-block
    found = false;
    // check if function is present in the functions-block
    for (UInt_t j=0; j<fFunctions.size(); j++) {
      if (fFunctions[j].fLine.BeginsWith("#") || fFunctions[j].fLine.IsWhitespace())
        continue;
      str = TString("fun");
      str += funVec[i] - MSR_PARAM_FUN_OFFSET;
      if (fFunctions[j].fLine.Contains(str, TString::kIgnoreCase)) {
        found = true;
        break;
      }
    }
    if (!found) { // func not found
      result = false;
      std::cerr << std::endl << ">> PMsrHandler::CheckFuncs: **ERROR** fun" << funVec[i]-MSR_PARAM_FUN_OFFSET << " found in the ";
      if (funBlock[i] == 0)
        std::cerr << "theory-block in line " << funLineBlockNo[i] << " is not present in the functions-block!";
      else
        std::cerr << "run-block No " << funLineBlockNo[i] << " (norm) is not present in the functions-block!";
      std::cerr << std::endl;
    }
  }

  // clean up
  funVec.clear();
  funBlock.clear();
  funLineBlockNo.clear();

  return result;
}

//--------------------------------------------------------------------------
// CheckHistoGrouping (public)
//--------------------------------------------------------------------------
/**
 * <p>Checks if histogram grouping makes any sense.
 *
 * <b>return:</b>
 * - true if histogram grouping seems OK
 * - false otherwise
 */
Bool_t PMsrHandler::CheckHistoGrouping()
{
  Bool_t result = true;

  for (UInt_t i=0; i<fRuns.size(); i++) {    
    // check grouping entries are not identical, e.g. forward 1 1 2
    if (fRuns[i].GetForwardHistoNoSize() > 1) {
      for (UInt_t j=0; j<fRuns[i].GetForwardHistoNoSize(); j++) {
        for (UInt_t k=j+1; k<fRuns[i].GetForwardHistoNoSize(); k++) {
          if (fRuns[i].GetForwardHistoNo(j) == fRuns[i].GetForwardHistoNo(k)) {
            std::cerr << std::endl << ">> PMsrHandler::CheckHistoGrouping: **WARNING** grouping identical histograms!!";
            std::cerr << std::endl << ">> run no " << i+1 << ", forward histo " << j+1 << " == forward histo " << k+1 << ".";
            std::cerr << std::endl << ">> this really doesn't make any sense, but you are the boss.";
            std::cerr << std::endl;
          }
        }
      }
    }

    if (fRuns[i].GetBackwardHistoNoSize() > 1) {
      for (UInt_t j=0; j<fRuns[i].GetBackwardHistoNoSize(); j++) {
        for (UInt_t k=j+1; k<fRuns[i].GetBackwardHistoNoSize(); k++) {
          if (fRuns[i].GetBackwardHistoNo(j) == fRuns[i].GetBackwardHistoNo(k)) {
            std::cerr << std::endl << ">> PMsrHandler::CheckHistoGrouping: **WARNING** grouping identical histograms!!";
            std::cerr << std::endl << ">> run no " << i+1 << ", backward histo " << j+1 << " == backward histo " << k+1 << ".";
            std::cerr << std::endl << ">> this really doesn't make any sense, but you are the boss.";
            std::cerr << std::endl;
          }
        }
      }
    }
  }

  return result;
}

//--------------------------------------------------------------------------
// CheckAddRunParameters (public)
//--------------------------------------------------------------------------
/**
 * <p>In case addrun is present check that if addt0's are given there are as many addt0's than addrun's.
 *
 * <b>return:</b>
 * - true if either no addt0 present, or # of addrun's == # of addt0's.
 * - false otherwise
 */
Bool_t PMsrHandler::CheckAddRunParameters()
{
  Bool_t result = true;

  for (UInt_t i=0; i<fRuns.size(); i++) {
    if (fRuns[i].GetRunNameSize() > 1) {
      // check concerning the addt0 tags
      if (fRuns[i].GetAddT0BinEntries() != 0) {
        if (fRuns[i].GetAddT0BinEntries() != fRuns[i].GetRunNameSize()-1) {
          std::cerr << std::endl << ">> PMsrHandler::CheckAddRunParameters: **ERROR** # of addt0 != # of addruns.";
          std::cerr << std::endl << ">> Run #" << i+1;
          std::cerr << std::endl;
          result = false;
          break;
        }
      }
    }
  }

  return result;
}

//--------------------------------------------------------------------------
// CheckMaxLikelihood (public)
//--------------------------------------------------------------------------
/**
 * <p>If log max likelihood is requested, make sure that all run blocks are of single histogram type.
 * If this is not the case, fall back to chisq, since for asymmetry/non-muSR fit, log max likelihood
 * is not defined.
 */
void PMsrHandler::CheckMaxLikelihood()
{
  if (!fStatistic.fChisq) {
    for (UInt_t i=0; i<fRuns.size(); i++) {
      if ((fRuns[i].GetFitType() != MSR_FITTYPE_SINGLE_HISTO) && (fGlobal.GetFitType() != MSR_FITTYPE_SINGLE_HISTO) &&
          (fRuns[i].GetFitType() != MSR_FITTYPE_MU_MINUS) && (fGlobal.GetFitType() != MSR_FITTYPE_MU_MINUS)) {
        std::cerr << std::endl << ">> PMsrHandler::CheckMaxLikelihood: **WARNING**: Maximum Log Likelihood Fit is only implemented";
        std::cerr << std::endl << ">>    for Single Histogram and Mu Minus Fits. Will fall back to Chi Square Fit.";
        std::cerr << std::endl << std::endl;
        fStatistic.fChisq = true;
        break;
      }
    }
  }
}

//--------------------------------------------------------------------------
// CheckRRFSettings (public)
//--------------------------------------------------------------------------
/**
 * <p>Make sure that if RRF settings are found in the GLOBAL section, the fit types
 * in the RUN blocks correspond.
 */
Bool_t PMsrHandler::CheckRRFSettings()
{
  Bool_t result = true;
  Int_t fittype = fGlobal.GetFitType();

  // first set of tests: if RRF parameters are set, check if RRF fit is chosen.
  if (fGlobal.GetRRFFreq(fGlobal.GetRRFUnit().Data()) != RRF_FREQ_UNDEF) {
    if (fittype != -1) { // check if GLOBAL fittype is set
      if ((fittype != MSR_FITTYPE_SINGLE_HISTO_RRF) &&
          (fittype != MSR_FITTYPE_ASYM_RRF)) {
        std::cerr << std::endl << ">> PMsrHandler::CheckRRFSettings: **ERROR** found GLOBAL fittype " << fittype << " and";
        std::cerr << std::endl << ">>    RRF settings in the GLOBAL section. This is NOT compatible. Fix it first.";
        result = false;
      }
    } else { // GLOBAL fittype is NOT set
      for (UInt_t i=0; i<fRuns.size(); i++) {
        fittype = fRuns[i].GetFitType();
        if ((fittype != MSR_FITTYPE_SINGLE_HISTO_RRF) &&
            (fittype != MSR_FITTYPE_ASYM_RRF)) {
          std::cerr << std::endl << ">> PMsrHandler::CheckRRFSettings: **ERROR** found RUN with fittype " << fittype << " and";
          std::cerr << std::endl << ">>    RRF settings in the GLOBAL section. This is NOT compatible. Fix it first.";
          result = false;
          break;
        }
      }
    }
  } else {
    if (fGlobal.GetRRFPacking() != -1) {
      std::cerr << std::endl << ">> PMsrHandler::CheckRRFSettings: **WARNING** found in the GLOBAL section rrf_packing, without";
      std::cerr << std::endl << ">>     rrf_freq. Doesn't make any sense. Will drop rrf_packing";
      std::cerr << std::endl << std::endl;
      fGlobal.SetRRFPacking(-1);
    }
    if (fGlobal.GetRRFPhase() != 0.0) {
      std::cerr << std::endl << ">> PMsrHandler::CheckRRFSettings: **WARNING** found in the GLOBAL section rrf_phase, without";
      std::cerr << std::endl << ">>     rrf_freq. Doesn't make any sense. Will drop rrf_phase";
      std::cerr << std::endl << std::endl;
      fGlobal.SetRRFPhase(0.0);
    }
  }

  // if not a RRF fit, done at this point
  if ((fittype != MSR_FITTYPE_SINGLE_HISTO_RRF) &&
      (fittype != MSR_FITTYPE_ASYM_RRF)) {
    return true;
  }

  // second set of tests: if RRF fit is chosen, do I find the necessary RRF parameters?
  fittype = fGlobal.GetFitType();
  if ((fittype == MSR_FITTYPE_SINGLE_HISTO_RRF) ||
      (fittype == MSR_FITTYPE_ASYM_RRF)) { // make sure RRF freq and RRF packing are set
    if (fGlobal.GetRRFFreq(fGlobal.GetRRFUnit().Data()) == RRF_FREQ_UNDEF) {
      std::cerr << std::endl << ">> PMsrHandler::CheckRRFSettings: **ERROR** RRF fit chosen, but";
      std::cerr << std::endl << ">>    no RRF frequency found in the GLOBAL section! Fix it.";
      return false;
    }
    if (fGlobal.GetRRFPacking() == -1) {
      std::cerr << std::endl << ">> PMsrHandler::CheckRRFSettings: **ERROR** RRF fit chosen, but";
      std::cerr << std::endl << ">>    no RRF packing found in the GLOBAL section! Fix it.";
      return false;
    }
  } else { // check single runs for RRF
    UInt_t rrfFitCounter = 0;
    for (UInt_t i=0; i<fRuns.size(); i++) {
      fittype = fRuns[i].GetFitType();
      if ((fittype == MSR_FITTYPE_SINGLE_HISTO_RRF) ||
          (fittype == MSR_FITTYPE_ASYM_RRF)) { // make sure RRF freq and RRF packing are set
        rrfFitCounter++;
      }
    }
    if (rrfFitCounter != fRuns.size()) {
      std::cerr << std::endl << ">> PMsrHandler::CheckRRFSettings: **ERROR** #Runs (" << fRuns.size() << ") != # RRF fits found (" << rrfFitCounter << ")";
      std::cerr << std::endl << ">>    This is currently not supported.";
      return false;
    }
    if (fGlobal.GetRRFFreq(fGlobal.GetRRFUnit().Data()) == RRF_FREQ_UNDEF) {
      std::cerr << std::endl << ">> PMsrHandler::CheckRRFSettings: **ERROR** RRF fit chosen, but";
      std::cerr << std::endl << ">>    no RRF frequency found in the GLOBAL section! Fix it.";
      return false;
    }
    if (fGlobal.GetRRFPacking() == -1) {
      std::cerr << std::endl << ">> PMsrHandler::CheckRRFSettings: **ERROR** RRF fit chosen, but";
      std::cerr << std::endl << ">>    no RRF packing found in the GLOBAL section! Fix it.";
      return false;
    }
  }

  return result;
}

//--------------------------------------------------------------------------
// GetGroupingString (public)
//--------------------------------------------------------------------------
/**
 * <p>returns the forward/backward grouping string.
 *
 * \param runNo msr-file run block number
 * \param detector tag telling which set to be used. Possible are: 'forward' and 'backward'
 * \param groupingStr compressed grouping information.
 */
void PMsrHandler::GetGroupingString(Int_t runNo, TString detector, TString &groupingStr)
{
  PIntVector grouping;

  if (!detector.CompareTo("forward", TString::kIgnoreCase)) {
    for (UInt_t i=0; i<fRuns[runNo].GetForwardHistoNoSize(); i++)
      grouping.push_back(fRuns[runNo].GetForwardHistoNo(i));
    MakeDetectorGroupingString("forward", grouping, groupingStr, false);
  } else if (!detector.CompareTo("backward", TString::kIgnoreCase)) {
    for (UInt_t i=0; i<fRuns[runNo].GetBackwardHistoNoSize(); i++)
      grouping.push_back(fRuns[runNo].GetBackwardHistoNo(i));
    MakeDetectorGroupingString("backward", grouping, groupingStr, false);
  } else {
    groupingStr = "**ERROR** unkown detector. Allow forward/backward.";
  }

  // clean up
  grouping.clear();
}

//--------------------------------------------------------------------------
// EstimateN0 (public)
//--------------------------------------------------------------------------
/**
 * <p>returns if N0 shall be estimated
 */
Bool_t PMsrHandler::EstimateN0()
{
  if (fStartupOptions == nullptr)
    return true;

  return fStartupOptions->estimateN0;
}

//--------------------------------------------------------------------------
// NeededPrecision (private)
//--------------------------------------------------------------------------
/**
 * <p>Calculates the needed precision of Double_t values for WriteMsrLogFile and WriteMsrFile of the fit range.
 * If a precision of > precLimit decimal places is needed, a warning is placed and a value of precLimit is returned.
 *
 * \param dval value for which the precision has to be estimated
 * \param precLimit precision limit
 *
 * <b>return:</b> needed precision
 */
UInt_t PMsrHandler::NeededPrecision(Double_t dval, UInt_t precLimit)
{
   UInt_t prec=0;

   if (dval == 0.0)
     return prec;

   for (UInt_t i=0; i<precLimit; i++) {
     if (static_cast<Int_t>(dval*pow(10.0,static_cast<Double_t>(i))) != 0) {
       prec = i;
       break;
     }
   }

   if (prec == precLimit) {
     std::cerr << std::endl << ">> PMsrHandler::NeededPrecision(): **WARNING** precision limit of " << precLimit << ", requested.";
   }

   return prec;
}

//--------------------------------------------------------------------------
// LastSignifiant (private)
//--------------------------------------------------------------------------
/**
 * <p>Gets the last significant digit down to precLimit.
 *
 * \param dval value for which the last signigicant digit shall be found
 * \param precLimit precision limit
 *
 * <b>return:</b> last significant digit down to precLimit
 */
UInt_t PMsrHandler::LastSignificant(Double_t dval, UInt_t precLimit)
{
  UInt_t lastSignificant = 2;
  UInt_t decimalPoint = 0;

  char str[128];

  sprintf(str, "%lf", dval);

  // find decimal point
  for (UInt_t i=0; i<strlen(str); i++) {
    if (str[i] == '.') {
      decimalPoint = i;
      break;
    }
  }

  // find last significant digit
  for (Int_t i=strlen(str)-1; i>=0; i--) {
    if (str[i] != '0') {
      if ((static_cast<UInt_t>(i)-decimalPoint) < precLimit)
        lastSignificant = static_cast<UInt_t>(i)-decimalPoint;
      else
        lastSignificant = precLimit;
      break;
    }
  }

  return lastSignificant;
}

//--------------------------------------------------------------------------
// MakeDetectorGroupingString (private)
//--------------------------------------------------------------------------
/**
 * <p>encodes the detector grouping vector.
 *
 * \param str 'forward' or 'backward'
 * \param group detector grouping vector to be encoded
 * \param result encoded detector grouping string
 * \param includeDetector if true, the detector information is included
 */
void PMsrHandler::MakeDetectorGroupingString(TString str, PIntVector &group, TString &result, Bool_t includeDetector)
{
  if (includeDetector) {
    result = str + TString("        ");
    if (str == TString("forward"))
      result += " ";
  } else {
    str = "";
  }

  if (group.size()==0)
    return;

  UInt_t i=0, j=0;
  do {
    j = i;
    if (j+1 < group.size()) {
      while (group[j]+1 == group[j+1]) {
        j++;
        if (j == group.size()-1)
          break;
      }
    }

    if (j >= i+2) {
      result += group[i];
      result += "-";
      result += group[j];
      i = j+1;
    } else {
      result += group[i];
      i++;
    }
    result += " ";
  } while (i<group.size());
}

//--------------------------------------------------------------------------
// BeautifyFourierPhaseParameterString (private)
//--------------------------------------------------------------------------
/**
 * <p>Returns the Fourier phase string if the phase is either of type
 * phase parX0 sep ... sep parXn where sep = ',' or
 * phase par(X0, offset, #param)
 *
 * @return Fourier phase parameter string if phase parameter(s) is(are) given, "??" otherwise
 */
TString PMsrHandler::BeautifyFourierPhaseParameterString()
{
  TString str("??");
  TString formatStr("par%d, par%d");

  if (fFourier.fPhaseParamNo.size() == 0)
    return str;

  Int_t phaseRef = fFourier.fPhaseRef;

  if (fFourier.fPhaseParamNo.size() == 1) {
    str = TString::Format("par%d", fFourier.fPhaseParamNo[0]);
  } else if (fFourier.fPhaseParamNo.size() == 2) {
    if (phaseRef == fFourier.fPhaseParamNo[0])
      formatStr = "parR%d, par%d";
    if (phaseRef == fFourier.fPhaseParamNo[1])
      formatStr = "par%d, parR%d";
    str = TString::Format(formatStr, fFourier.fPhaseParamNo[0], fFourier.fPhaseParamNo[1]);
  } else {
    Bool_t phaseIter = true;

    // first check if fPhaseParamNo vector can be compacted into par(X0, offset, #param) form
    Int_t offset = fFourier.fPhaseParamNo[1] - fFourier.fPhaseParamNo[0];
    for (Int_t i=2; i<fFourier.fPhaseParamNo.size(); i++) {
      if (fFourier.fPhaseParamNo[i]-fFourier.fPhaseParamNo[i-1] != offset) {
        phaseIter = false;
        break;
      }
    }

    if (phaseIter) {
      if (phaseRef != -1) {
        str = TString::Format("parR(%d, %d, %lu)", fFourier.fPhaseParamNo[0], offset, fFourier.fPhaseParamNo.size());
      } else {
        str = TString::Format("par(%d, %d, %lu)", fFourier.fPhaseParamNo[0], offset, fFourier.fPhaseParamNo.size());
      }
    } else {
      str = TString("");
      for (Int_t i=0; i<fFourier.fPhaseParamNo.size()-1; i++) {
        if (phaseRef == fFourier.fPhaseParamNo[i]) {
          str += "parR";
        } else {
          str += "par";
        }
        str += fFourier.fPhaseParamNo[i];
        str += ", ";
      }
      if (phaseRef == fFourier.fPhaseParamNo[fFourier.fPhaseParamNo.size()-1]) {
        str += "parR";
      } else {
        str += "par";
      }
      str += fFourier.fPhaseParamNo[fFourier.fPhaseParamNo.size()-1];
    }
  }

  return str;
}

//--------------------------------------------------------------------------
// CheckLegacyLifetimecorrection (private)
//--------------------------------------------------------------------------
/**
 * <p>Checks for lifetimecorrection flags in the RUN-blocks and if present,
 * transfer it to the PLOT-blocks. This is needed since originally the lifetimecorrection
 * was (miss)placed in the RUN-blocks rather than in the PLOT-blocks where it
 * most naturally would have been expected.
 */
void PMsrHandler::CheckLegacyLifetimecorrection()
{
  UInt_t idx=0;
  for (UInt_t i=0; i<fPlots.size(); i++) {
    for (UInt_t j=0; j<fPlots[i].fRuns.size(); j++) {
      idx = fPlots[i].fRuns[j]-1;
      if (fRuns[idx].IsLifetimeCorrected()) {
        fPlots[i].fLifeTimeCorrection = true;
      }
    }
  }
}

// end ---------------------------------------------------------------------
