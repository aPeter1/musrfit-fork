/***************************************************************************

  PMsrHandler.cpp

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

#include <math.h>

#include <string>
#include <iostream>
#include <fstream>
using namespace std;

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
 * <p>
 *
 * \param fileName
 */
PMsrHandler::PMsrHandler(const Char_t *fileName) : fFileName(fileName)
{
  // init variables
  fMsrBlockCounter = 0;

  fTitle = "";

  fCopyStatisticsBlock = false;
  fStatistic.fValid = false;
  fStatistic.fChisq = true;
  fStatistic.fMin   = -1.0;
  fStatistic.fNdf   = 0;

  fFuncHandler = 0;

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
 * <p>
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
  fParamInUse.clear();

  if (fFuncHandler) {
    delete fFuncHandler;
    fFuncHandler = 0;
  }
}

//--------------------------------------------------------------------------
// ReadMsrFile (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * <p><b>return:</b>
 * - PMUSR_SUCCESS if everything is OK
 * - line number if an error in the MSR file was encountered which cannot be handled.
 *
 */
Int_t PMsrHandler::ReadMsrFile()
{
  ifstream f;
  string str;
  TString line;
  Int_t line_no = 0;
  Int_t result = PMUSR_SUCCESS;

  PMsrLineStructure current;

  PMsrLines fit_parameter;
  PMsrLines theory;
  PMsrLines functions;
  PMsrLines run;
  PMsrLines commands;
  PMsrLines fourier;
  PMsrLines plot;
  PMsrLines statistic;

  // init stuff
  InitFourierParameterStructure(fFourier);

  // open msr-file
  f.open(fFileName.Data(), iostream::in);
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

  // fill parameter-in-use vector
  if (result == PMUSR_SUCCESS)
    FillParameterInUse(theory, functions, run);

  // check that parameter names are unique
  if (result == PMUSR_SUCCESS) {
    UInt_t parX, parY;
    if (!CheckUniquenessOfParamNames(parX, parY)) {
      cerr << endl << ">> PMsrHandler::ReadMsrFile: **SEVERE ERROR** parameter name " << fParam[parX].fName.Data() << " is identical for parameter no " << fParam[parX].fNo << " and " << fParam[parY].fNo << "!";
      cerr << endl << "Needs to be fixed first!";
      cerr << endl;
      result = PMUSR_MSR_SYNTAX_ERROR;
    }
  }

  // check that if maps are present in the theory- and/or function-block,
  // that there are really present in the run block
  if (result == PMUSR_SUCCESS)
    if (!CheckMaps())
      result = PMUSR_MSR_SYNTAX_ERROR;


  // check that if functions are present in the theory- and/or run-block, that they
  // are really present in the function block
  if (result == PMUSR_SUCCESS)
    if (!CheckFuncs())
      result = PMUSR_MSR_SYNTAX_ERROR;

  // clean up
  fit_parameter.clear();
  theory.clear();
  functions.clear();
  run.clear();
  commands.clear();
  fourier.clear();
  plot.clear();
  statistic.clear();

/*
cout << endl << ">> FOURIER Block:";
cout << endl << ">>   Fourier Block Present       : " << fFourier.fFourierBlockPresent;
cout << endl << ">>   Fourier Units               : " << fFourier.fUnits;
cout << endl << ">>   Fourier Power               : " << fFourier.fFourierPower;
cout << endl << ">>   Apodization                 : " << fFourier.fApodization;
cout << endl << ">>   Plot Tag                    : " << fFourier.fPlotTag;
cout << endl << ">>   Phase                       : " << fFourier.fPhase;
cout << endl << ">>   Range for Freq. Corrections : " << fFourier.fRangeForPhaseCorrection[0] << ", " << fFourier.fRangeForPhaseCorrection[1];
cout << endl << ">>   Plot Range                  : " << fFourier.fPlotRange[0] << ", " << fFourier.fPlotRange[1];
cout << endl;
*/

  return result;
}

//--------------------------------------------------------------------------
// WriteMsrLogFile (public)
//--------------------------------------------------------------------------
/**
 * <p>
 */
Int_t PMsrHandler::WriteMsrLogFile(const Bool_t messages)
{
  const UInt_t prec = 6; // output precision for float/doubles

  Int_t tag, lineNo = 0, number;
  Int_t runNo = -1, addRunNo = 0;
  Int_t plotNo = -1;
  string line;
  TString logFileName, str, sstr, *pstr;
  TObjArray *tokens = 0;
  TObjString *ostr = 0;
  Bool_t found = false;
  Bool_t statisticBlockFound = false;
  Bool_t partialStatisticBlockFound = true;

  PBoolVector t0TagMissing; // needed for proper musrt0 handling
  for (UInt_t i=0; i<fRuns.size(); i++) {
    t0TagMissing.push_back(true);
  }
  PBoolVector backgroundTagMissing; // needed for proper musrt0 handling
  for (UInt_t i=0; i<fRuns.size(); i++) {
    backgroundTagMissing.push_back(true);
  }
  PBoolVector dataTagMissing; // needed for proper musrt0 handling
  for (UInt_t i=0; i<fRuns.size(); i++) {
    dataTagMissing.push_back(true);
  }

  ifstream fin;
  ofstream fout;

  // check msr-file for any missing tags first
  // open msr-file for reading
  fin.open(fFileName.Data(), iostream::in);
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
  fin.open(fFileName.Data(), iostream::in);
  if (!fin.is_open()) {
    return PMUSR_MSR_LOG_FILE_WRITE_ERROR;
  }

  // open mlog-file for writing
  fout.open(logFileName.Data(), iostream::out);
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
      fout << str.Data() << endl;
      continue;
    } else if (str.BeginsWith("FUNCTIONS")) {    // FUNCTIONS block tag
      tag = MSR_TAG_FUNCTIONS;
      fout << str.Data() << endl;
      continue;
    } else if (str.BeginsWith("RUN")) {          // RUN block tag
      tag = MSR_TAG_RUN;
      runNo++;
    } else if (str.BeginsWith("COMMANDS")) {     // COMMANDS block tag
      tag = MSR_TAG_COMMANDS;
      fout << str.Data() << endl;
      continue;
    } else if (str.BeginsWith("FOURIER")) {      // FOURIER block tag
      tag = MSR_TAG_FOURIER;
      fout << str.Data() << endl;
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
          fout << fTitle.Data() << endl;
        else
          fout << str.Data() << endl;
        break;
      case MSR_TAG_FITPARAMETER:
        tokens = str.Tokenize(" \t");
        if (tokens->GetEntries() == 0) { // not a parameter line
          fout << str.Data() << endl;
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
            fout << right << fParam[number].fNo;
            fout << " ";
            // parameter name
            fout.width(11);
            fout << left << fParam[number].fName.Data();
            fout << " ";
            // value of the parameter
            fout.width(9);
            fout.precision(prec);
            fout << left << fParam[number].fValue;
            fout << " ";
            // value of step/error/neg.error
            fout.width(11);
            fout.precision(prec);
            fout << left << fParam[number].fStep;
            fout << " ";
            fout.width(11);
            fout.precision(prec);
            if ((fParam[number].fNoOfParams == 5) || (fParam[number].fNoOfParams == 7)) // pos. error given
              if (fParam[number].fPosErrorPresent && (fParam[number].fStep != 0)) // pos error is a number
                fout << left << fParam[number].fPosError;
            else // pos error is a none
              fout << left << "none";
            else // no pos. error
              fout << left << "none";
            fout << " ";
            // boundaries
            if (fParam[number].fNoOfParams > 5) {
              fout.width(7);
              fout.precision(prec);
              if (fParam[number].fLowerBoundaryPresent)
                fout << left << fParam[number].fLowerBoundary;
              else
                fout << left << "none";
              fout << " ";
              fout.width(7);
              fout.precision(prec);
              if (fParam[number].fUpperBoundaryPresent)
                fout << left << fParam[number].fUpperBoundary;
              else
                fout << left << "none";
              fout << " ";
            }
            fout << endl;
          } else { // not a parameter, hence just copy it
            fout << str.Data() << endl;
          }
          // clean up tokens
          delete tokens;
        }
        break;
      case MSR_TAG_THEORY:
        found = false;
        for (UInt_t i=0; i<fTheory.size(); i++) {
          if (fTheory[i].fLineNo == lineNo) {
            fout << fTheory[i].fLine.Data() << endl;
            found = true;
          }
        }
        if (!found) {
          fout << str.Data() << endl;
        }
        break;
      case MSR_TAG_FUNCTIONS:
        sstr = str;
        sstr.Remove(TString::kLeading, ' ');
        if (str.BeginsWith("fun")) {
          if (FilterNumber(sstr, "fun", 0, number)) {
            sstr = *fFuncHandler->GetFuncString(number-1);
            sstr.ToLower();
            fout << sstr.Data() << endl;
          }
        } else {
          fout << str.Data() << endl;
        }
        break;
      case MSR_TAG_RUN:
        sstr = str;
        sstr.Remove(TString::kLeading, ' ');
        if (sstr.BeginsWith("RUN")) {
          fout << "RUN " << fRuns[runNo].GetRunName()->Data() << " ";
          pstr = fRuns[runNo].GetBeamline();
          if (pstr == 0) {
            cerr << endl << ">> PMsrHandler::WriteMsrLogFile: **ERROR** Couldn't obtain beamline data." << endl;
            assert(0);
          }
          pstr->ToUpper();
          fout << pstr->Data() << " ";
          pstr = fRuns[runNo].GetInstitute();
          if (pstr == 0) {
            cerr << endl << ">> PMsrHandler::WriteMsrLogFile: **ERROR** Couldn't obtain institute data." << endl;
            assert(0);
          }
          pstr->ToUpper();
          fout << pstr->Data() << " ";
          pstr = fRuns[runNo].GetFileFormat();
          if (pstr == 0) {
            cerr << endl << ">> PMsrHandler::WriteMsrLogFile: **ERROR** Couldn't obtain file format data." << endl;
            assert(0);
          }
          pstr->ToUpper();
          fout << pstr->Data() << "   (name beamline institute data-file-format)" << endl;
        } else if (sstr.BeginsWith("ADDRUN")) {
          addRunNo++;
          fout << "ADDRUN " << fRuns[runNo].GetRunName(addRunNo)->Data() << " ";
          pstr = fRuns[runNo].GetBeamline(addRunNo);
          if (pstr == 0) {
            cerr << endl << ">> PMsrHandler::WriteMsrLogFile: **ERROR** Couldn't obtain beamline data (addrun)." << endl;
            assert(0);
          }
          pstr->ToUpper();
          fout << pstr->Data() << " ";
          pstr = fRuns[runNo].GetInstitute(addRunNo);
          if (pstr == 0) {
            cerr << endl << ">> PMsrHandler::WriteMsrLogFile: **ERROR** Couldn't obtain institute data (addrun)." << endl;
            assert(0);
          }
          pstr->ToUpper();
          fout << pstr->Data() << " ";
          pstr = fRuns[runNo].GetFileFormat(addRunNo);
          if (pstr == 0) {
            cerr << endl << ">> PMsrHandler::WriteMsrLogFile: **ERROR** Couldn't obtain file format data (addrun)." << endl;
            assert(0);
          }
          pstr->ToUpper();
          fout << pstr->Data() << "   (name beamline institute data-file-format)" << endl;
        } else if (sstr.BeginsWith("fittype")) {
          addRunNo = 0;
          fout.width(16);
          switch (fRuns[runNo].GetFitType()) {
          case MSR_FITTYPE_SINGLE_HISTO:
            fout << left << "fittype" << MSR_FITTYPE_SINGLE_HISTO << "         (single histogram fit)" << endl;
            break;
          case MSR_FITTYPE_ASYM:
            fout << left << "fittype" << MSR_FITTYPE_ASYM << "         (asymmetry fit)" << endl ;
            break;
          case MSR_FITTYPE_ASYM_RRF:
            fout << left << "fittype" << MSR_FITTYPE_ASYM_RRF << "         (RRF asymmetry fit)" << endl ;
            break;
          case MSR_FITTYPE_NON_MUSR:
            fout << left << "fittype" << MSR_FITTYPE_NON_MUSR << "         (non muSR fit)" << endl ;
            break;
          default:
            break;
          }
        } else if (sstr.BeginsWith("rrffrequency")) {
          fout.width(16);
          fout << left << "rrffrequency";
          fout.precision(prec);
          fout << fRuns[runNo].GetRRFFreq() << endl;
        } else if (sstr.BeginsWith("rrfpacking")) {
          fout.width(16);
          fout << left << "rrfpacking";
          fout.precision(prec);
          fout << fRuns[runNo].GetRRFPacking() << endl;
        } else if (sstr.BeginsWith("alpha ")) {
          fout.width(16);
          fout << left << "alpha";
          fout << fRuns[runNo].GetAlphaParamNo() << endl;
        } else if (sstr.BeginsWith("beta ")) {
          fout.width(16);
          fout << left << "beta";
          fout << fRuns[runNo].GetBetaParamNo()  << endl;
        } else if (sstr.BeginsWith("alpha2")) {
          fout.width(16);
          fout << left << "alpha2";
          fout << fRuns[runNo].GetAlpha2ParamNo() << endl;
        } else if (sstr.BeginsWith("beta2")) {
          fout.width(16);
          fout << left << "beta2";
          fout << fRuns[runNo].GetBeta2ParamNo()  << endl;
        } else if (sstr.BeginsWith("norm")) {
          fout.width(16);
          fout << left << "norm";
          // check if norm is give as a function
          if (fRuns[runNo].GetNormParamNo() >= MSR_PARAM_FUN_OFFSET)
            fout << "fun" << fRuns[runNo].GetNormParamNo()-MSR_PARAM_FUN_OFFSET;
          else
            fout << fRuns[runNo].GetNormParamNo();
          fout << endl;
        } else if (sstr.BeginsWith("backgr.fit")) {
          fout.width(16);
          fout << left << "backgr.fit";
          fout << fRuns[runNo].GetBkgFitParamNo() << endl;
        } else if (sstr.BeginsWith("rphase")) {
          fout.width(16);
          fout << left << "rphase";
          fout << fRuns[runNo].GetPhaseParamNo() << endl;
        } else if (sstr.BeginsWith("lifetime ")) {
          fout.width(16);
          fout << left << "lifetime";
          fout << fRuns[runNo].GetLifetimeParamNo() << endl;
        } else if (sstr.BeginsWith("lifetimecorrection")) {
          if ((fRuns[runNo].IsLifetimeCorrected()) && (fRuns[runNo].GetFitType() == MSR_FITTYPE_SINGLE_HISTO)) {
            fout << "lifetimecorrection" << endl;
          }
        } else if (sstr.BeginsWith("map")) {
          fout << "map         ";
          for (UInt_t j=0; j<fRuns[runNo].GetMap()->size(); j++) {
            fout.width(5);
            fout << right << fRuns[runNo].GetMap(j);
          }
          // if there are less maps then 10 fill with zeros
          if (fRuns[runNo].GetMap()->size() < 10) {
            for (UInt_t j=fRuns[runNo].GetMap()->size(); j<10; j++)
              fout << "    0";
          }
          fout << endl;
        } else if (sstr.BeginsWith("forward")) {
          fout.width(16);
          fout << left << "forward";
          fout << fRuns[runNo].GetForwardHistoNo() << endl;
        } else if (sstr.BeginsWith("backward")) {
          fout.width(16);
          fout << left << "backward";
          fout << fRuns[runNo].GetBackwardHistoNo() << endl;
        } else if (sstr.BeginsWith("right")) {
          fout.width(16);
          fout << left << "right";
          fout << fRuns[runNo].GetRightHistoNo() << endl;
        } else if (sstr.BeginsWith("left")) {
          fout.width(16);
          fout << left << "left";
          fout << fRuns[runNo].GetLeftHistoNo() << endl;
        } else if (sstr.BeginsWith("backgr.fix")) {
          fout.width(15);
          fout << left << "backgr.fix";
          for (UInt_t j=0; j<fRuns[runNo].GetBkgFixSize(); j++) {
            fout.precision(prec);
            fout.width(12);
            fout << left << fRuns[runNo].GetBkgFix(j);
          }
          fout << endl;
        } else if (sstr.BeginsWith("background")) {
          backgroundTagMissing[runNo] = false;
          fout.width(16);
          fout << left << "background";
          for (UInt_t j=0; j<fRuns[runNo].GetBkgRangeSize(); j++) {
            fout.width(8);
            fout << left << fRuns[runNo].GetBkgRange(j)+1; // +1 since internally the data start at 0
          }
          fout << endl;
        } else if (sstr.BeginsWith("data")) {
          dataTagMissing[runNo] = false;
          fout.width(16);
          fout << left << "data";
          for (UInt_t j=0; j<fRuns[runNo].GetDataRangeSize(); j++) {
            fout.width(8);
            fout << left << fRuns[runNo].GetDataRange(j)+1; // +1 since internally the data start at 0
          }
          fout << endl;
        } else if (sstr.BeginsWith("t0")) {
          t0TagMissing[runNo] = false;
          fout.width(16);
          fout << left << "t0";
          for (UInt_t j=0; j<fRuns[runNo].GetT0Size(); j++) {
            fout.width(8);
            fout << left << fRuns[runNo].GetT0(j)+1; // +1 since internally the data start at 0
          }
          fout << endl;
        } else if (sstr.BeginsWith("xy-data")) {
          if (fRuns[runNo].GetXDataIndex() != -1) { // indices
            fout.width(16);
            fout << left << "xy-data";
            fout.width(8);
            fout.precision(2);
            fout << left << fixed << fRuns[runNo].GetXDataIndex();
            fout.width(8);
            fout.precision(2);
            fout << left << fixed << fRuns[runNo].GetYDataIndex();
            fout << endl;
          } else if (!fRuns[runNo].GetXDataLabel()->IsWhitespace()) { // labels
            fout.width(16);
            fout << left << "xy-data";
            fout.width(8);
            fout << left << fixed << fRuns[runNo].GetXDataLabel()->Data();
            fout << " ";
            fout.width(8);
            fout << left << fixed << fRuns[runNo].GetYDataLabel()->Data();
            fout << endl;
          }
        } else if (sstr.BeginsWith("fit")) {
          // check if missing t0/background/data tag are present eventhough the values are present, if so write these data values
          if (t0TagMissing[runNo]) {
            if (fRuns[runNo].GetT0Size() > 0) {
              fout.width(16);
              fout << left << "t0";
              for (UInt_t j=0; j<fRuns[runNo].GetT0Size(); j++) {
                fout.width(8);
                fout << left << fRuns[runNo].GetT0(j);
              }
              fout << endl;
            }
          }
          if (backgroundTagMissing[runNo]) {
            if (fRuns[runNo].GetBkgRangeSize() > 0) {
              fout.width(16);
              fout << left << "background";
              for (UInt_t j=0; j<fRuns[runNo].GetBkgRangeSize(); j++) {
                fout.width(8);
                fout << left << fRuns[runNo].GetBkgRange(j)+1; // +1 since internally the data start at 0
              }
              fout << endl;
            }
          }
          if (dataTagMissing[runNo]) {
            if (fRuns[runNo].GetDataRangeSize() > 0) {
              fout.width(16);
              fout << left << "data";
              for (UInt_t j=0; j<fRuns[runNo].GetDataRangeSize(); j++) {
                fout.width(8);
                fout << left << fRuns[runNo].GetDataRange(j)+1; // +1 since internally the data start at 0
              }
              fout << endl;
            }
          }
          // write fit range line
          fout.width(16);
          fout << left << "fit";
          for (UInt_t j=0; j<2; j++) {
            if (fRuns[runNo].GetFitRange(j) == -1)
              break;
            fout.width(8);
            fout.precision(2);
            fout << left << fixed << fRuns[runNo].GetFitRange(j);
          }
          fout << endl;
        } else if (sstr.BeginsWith("packing")) {
          fout.width(16);
          fout << left << "packing";
          fout << fRuns[runNo].GetPacking() << endl;
        } else {
          fout << str.Data() << endl;
        }
        break;
      case MSR_TAG_COMMANDS:
        fout << str.Data() << endl;
        break;
      case MSR_TAG_FOURIER:
        sstr = str;
        sstr.Remove(TString::kLeading, ' ');
        if (sstr.BeginsWith("units")) {
          fout << "units            ";
          if (fFourier.fUnits == FOURIER_UNIT_FIELD) {
            fout << "Gauss";
          } else if (fFourier.fUnits == FOURIER_UNIT_FREQ) {
            fout << "MHz ";
          } else if (fFourier.fUnits == FOURIER_UNIT_CYCLES) {
            fout << "Mc/s";
          }
          fout << "   # units either 'Gauss', 'MHz', or 'Mc/s'";
          fout << endl;
        } else if (sstr.BeginsWith("fourier_power")) {
          fout << "fourier_power    " << fFourier.fFourierPower << endl;
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
          fout << endl;
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
          }
          fout << "   # REAL, IMAG, REAL_AND_IMAG, POWER, PHASE";
          fout << endl;
        } else if (sstr.BeginsWith("phase")) {
          if (fFourier.fPhaseParamNo > 0) {
            fout << "phase            par" << fFourier.fPhaseParamNo << endl;
          } else {
            if (fFourier.fPhase != -999.0) {
              fout << "phase            " << fFourier.fPhase << endl;
            }
          }
        } else if (sstr.BeginsWith("range_for_phase_correction")) {
          fout << "range_for_phase_correction  " << fFourier.fRangeForPhaseCorrection[0] << "    " << fFourier.fRangeForPhaseCorrection[1] << endl;
        } else if (sstr.BeginsWith("range ")) {
          fout << "range            " << fFourier.fPlotRange[0] << "    " << fFourier.fPlotRange[1] << endl;
        } else {
          fout << str.Data() << endl;
        }
        break;
      case MSR_TAG_PLOT:
        sstr = str;
        sstr.Remove(TString::kLeading, ' ');
        if (sstr.BeginsWith("PLOT")) {
          switch (fPlots[plotNo].fPlotType) {
          case MSR_PLOT_SINGLE_HISTO:
            fout << "PLOT " << fPlots[plotNo].fPlotType << "   (single histo plot)" << endl;
            break;
          case MSR_PLOT_ASYM:
            fout << "PLOT " << fPlots[plotNo].fPlotType << "   (asymmetry plot)" << endl;
            break;
          case MSR_PLOT_ASYM_RRF:
            fout << "PLOT " << fPlots[plotNo].fPlotType << "   (rotating reference frame plot)" << endl;
            break;
          case MSR_PLOT_NON_MUSR:
            fout << "PLOT " << fPlots[plotNo].fPlotType << "   (non muSR plot)" << endl;
            break;
          default:
            break;
          }
        } else if (sstr.BeginsWith("runs")) {
          fout << "runs     ";
          fout.precision(0);
          for (UInt_t j=0; j<fPlots[plotNo].fRuns.size(); j++) {
            if (fPlots[plotNo].fPlotType != MSR_PLOT_ASYM_RRF) { // all but MSR_PLOT_ASYM_RRF
              fout.width(4);
              fout << fPlots[plotNo].fRuns[j].Re();
            } else { // MSR_PLOT_ASYM_RRF
              fout << fPlots[plotNo].fRuns[j].Re() << "," << fPlots[plotNo].fRuns[j].Im() << "    ";
            }
          }
          fout << endl;
        } else if (sstr.BeginsWith("range")) {
          fout << "range    ";
          fout.precision(2);
          fout << fPlots[plotNo].fTmin[0] << "   " << fPlots[plotNo].fTmax[0];
          if (fPlots[plotNo].fYmin.size() > 0) {
            fout << "   " << fPlots[plotNo].fYmin[0] << "   " << fPlots[plotNo].fYmax[0];
          }
          fout << endl;
        } else {
          fout << str.Data() << endl;
        }
        break;
      case MSR_TAG_STATISTIC:
        statisticBlockFound = true;
        sstr = str;
        sstr.Remove(TString::kLeading, ' ');
        if (sstr.BeginsWith("STATISTIC")) {
          TDatime dt;
          fout << "STATISTIC --- " << dt.AsSQLString() << endl;
        } else if (sstr.BeginsWith("chisq")) {
          partialStatisticBlockFound = false;
          if (fStatistic.fValid) { // valid fit result
            str  = "  chisq = ";
            str += fStatistic.fMin;
            str += ", NDF = ";
            str += fStatistic.fNdf;
            str += ", chisq/NDF = ";
            str += fStatistic.fMin / fStatistic.fNdf;
            fout << str.Data() << endl;
            if (messages)
              cout << endl << str.Data() << endl;
          } else {
           fout << "*** FIT DID NOT CONVERGE ***" << endl;
           if (messages)
             cout << endl << "*** FIT DID NOT CONVERGE ***" << endl;
          }
        } else if (sstr.BeginsWith("maxLH")) {
          partialStatisticBlockFound = false;
          if (fStatistic.fValid) { // valid fit result
            str  = "  maxLH = ";
            str += fStatistic.fMin;
            str += ", NDF = ";
            str += fStatistic.fNdf;
            str += ", maxLH/NDF = ";
            str += fStatistic.fMin / fStatistic.fNdf;
            fout << str.Data() << endl;
            if (messages)
              cout << endl << str.Data() << endl;
          } else {
           fout << "*** FIT DID NOT CONVERGE ***" << endl;
           if (messages)
             cout << endl << "*** FIT DID NOT CONVERGE ***" << endl;
          }
        } else if (sstr.BeginsWith("*** FIT DID NOT CONVERGE ***")) {
          partialStatisticBlockFound = false;
          if (fStatistic.fValid) { // valid fit result
            if (fStatistic.fChisq) { // chisq
              str  = "  chisq = ";
              str += fStatistic.fMin;
              str += ", NDF = ";
              str += fStatistic.fNdf;
              str += ", chisq/NDF = ";
              str += fStatistic.fMin / fStatistic.fNdf;
              fout << str.Data() << endl;
              if (messages)
                cout << endl << str.Data() << endl;
            } else { // max. log. liklihood
              str  = "  maxLH = ";
              str += fStatistic.fMin;
              str += ", NDF = ";
              str += fStatistic.fNdf;
              str += ", maxLH/NDF = ";
              str += fStatistic.fMin / fStatistic.fNdf;
              fout << str.Data() << endl;
              if (messages)
                cout << endl << str.Data() << endl;
            }
          } else {
           fout << "*** FIT DID NOT CONVERGE ***" << endl;
           if (messages)
             cout << endl << "*** FIT DID NOT CONVERGE ***" << endl;
          }
        } else {
          if (str.Length() > 0)
            fout << str.Data() << endl;
          else // only write endl if not eof is reached. This is preventing growing msr-files, i.e. more and more empty lines at the end of the file
            if (!fin.eof())
              fout << endl;
        }
        break;
      default:
        break;
    }
  }

  // there was no statistic block present in the msr-input-file
  if (!statisticBlockFound) {
    partialStatisticBlockFound = false;
    cerr << endl << "PMsrHandler::WriteMsrLogFile: **WARNING** no STATISTIC block present, will write a default one" << endl;
    fout << "###############################################################" << endl;
    TDatime dt;
    fout << "STATISTIC --- " << dt.AsSQLString() << endl;
    if (fStatistic.fValid) { // valid fit result
      if (fStatistic.fChisq) { // chisq
        str  = "  chisq = ";
        str += fStatistic.fMin;
        str += ", NDF = ";
        str += fStatistic.fNdf;
        str += ", chisq/NDF = ";
        str += fStatistic.fMin / fStatistic.fNdf;
        fout << str.Data() << endl;
        if (messages)
          cout << endl << str.Data() << endl;
      } else { // max. log. liklihood
        str  = "  maxLH = ";
        str += fStatistic.fMin;
        str += ", NDF = ";
        str += fStatistic.fNdf;
        str += ", maxLH/NDF = ";
        str += fStatistic.fMin / fStatistic.fNdf;
        fout << str.Data() << endl;
        if (messages)
          cout << endl << str.Data() << endl;
      }
    } else {
      fout << "*** FIT DID NOT CONVERGE ***" << endl;
      if (messages)
        cout << endl << "*** FIT DID NOT CONVERGE ***" << endl;
    }
  }

  // there was only a partial statistic block present in the msr-input-file
  if (partialStatisticBlockFound) {
    cerr << endl << "PMsrHandler::WriteMsrLogFile: **WARNING** garbage STATISTIC block present in the msr-input file.";
    cerr << endl << "** WILL ADD SOME SENSIBLE STUFF, BUT YOU HAVE TO CHECK IT SINCE I AM **NOT** REMOVING THE GARBAGE! **" << endl;
    TDatime dt;
    fout << "STATISTIC --- " << dt.AsSQLString() << endl;
    if (fStatistic.fValid) { // valid fit result
      if (fStatistic.fChisq) { // chisq
        str  = "  chisq = ";
        str += fStatistic.fMin;
        str += ", NDF = ";
        str += fStatistic.fNdf;
        str += ", chisq/NDF = ";
        str += fStatistic.fMin / fStatistic.fNdf;
        fout << str.Data() << endl;
        if (messages)
          cout << endl << str.Data() << endl;
      } else { // max. log. liklihood
        str  = "  maxLH = ";
        str += fStatistic.fMin;
        str += ", NDF = ";
        str += fStatistic.fNdf;
        str += ", maxLH/NDF = ";
        str += fStatistic.fMin / fStatistic.fNdf;
        fout << str.Data() << endl;
        if (messages)
          cout << endl << str.Data() << endl;
      }
    } else {
      fout << "*** FIT DID NOT CONVERGE ***" << endl;
      if (messages)
        cout << endl << "*** FIT DID NOT CONVERGE ***" << endl;
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
// SetMsrParamValue (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param i
 * \param value
 */
Bool_t PMsrHandler::SetMsrParamValue(UInt_t i, Double_t value)
{
  if (i > fParam.size()) {
    cerr << endl << "PMsrHandler::SetMsrParamValue(): **ERROR** i = " << i << " is larger than the number of parameters " << fParam.size();
    cerr << endl;
    return false;
  }

  fParam[i].fValue = value;

  return true;
}

//--------------------------------------------------------------------------
// SetMsrParamStep (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param i
 * \param value
 */
Bool_t PMsrHandler::SetMsrParamStep(UInt_t i, Double_t value)
{
  if (i > fParam.size()) {
    cerr << endl << "PMsrHandler::SetMsrParamValue(): **ERROR** i = " << i << " is larger than the number of parameters " << fParam.size();
    cerr << endl;
    return false;
  }

  fParam[i].fStep = value;

  return true;
}

//--------------------------------------------------------------------------
// SetMsrParamPosErrorPresent (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param i
 * \param value
 */
Bool_t PMsrHandler::SetMsrParamPosErrorPresent(UInt_t i, Bool_t value)
{
  if (i > fParam.size()) {
    cerr << endl << "PMsrHandler::SetMsrParamPosErrorPresent(): **ERROR** i = " << i << " is larger than the number of parameters " << fParam.size();
    cerr << endl;
    return false;
  }

  fParam[i].fPosErrorPresent = value;

  return true;
}

//--------------------------------------------------------------------------
// SetMsrParamPosError (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param i
 * \param value
 */
Bool_t PMsrHandler::SetMsrParamPosError(UInt_t i, Double_t value)
{
  if (i > fParam.size()) {
    cerr << endl << "PMsrHandler::SetMsrParamPosError(): **ERROR** i = " << i << " is larger than the number of parameters " << fParam.size();
    cerr << endl;
    return false;
  }

  fParam[i].fPosErrorPresent = true;
  fParam[i].fPosError = value;

  return true;
}

//--------------------------------------------------------------------------
// SetMsrT0Entry (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runNo
 * \param idx
 * \param bin
 */
void PMsrHandler::SetMsrT0Entry(UInt_t runNo, UInt_t idx, Int_t bin)
{
  if ((runNo < 0) || (runNo > fRuns.size())) { // error
    cerr << endl << "PMsrHandler::SetMsrT0Entry: **ERROR** runNo = " << runNo << ", is out of valid range 0.." << fRuns.size();
    cerr << endl;
    return;
  }

  if ((idx < 0) || (idx > fRuns[runNo].GetT0Size())) { // error
    cerr << endl << "PMsrHandler::SetMsrT0Entry: **ERROR** idx = " << idx << ", is out of valid range 0.." << fRuns[runNo].GetT0Size();
    cerr << endl;
    return;
  }

  fRuns[runNo].SetT0(bin, idx);
}

//--------------------------------------------------------------------------
// SetMsrDataRangeEntry (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runNo
 * \param idx
 * \param bin
 */
void PMsrHandler::SetMsrDataRangeEntry(UInt_t runNo, UInt_t idx, Int_t bin)
{
  if ((runNo < 0) || (runNo > fRuns.size())) { // error
    cerr << endl << "PMsrHandler::SetMsrDataRangeEntry: **ERROR** runNo = " << runNo << ", is out of valid range 0.." << fRuns.size();
    cerr << endl;
    return;
  }

  if (idx < 0) { // error
    cerr << endl << "PMsrHandler::SetMsrDataRangeEntry: **ERROR** idx = " << idx << ", is out of valid range, i.e. idx >= 0 needed.";
    cerr << endl;
    return;
  }

  fRuns[runNo].SetDataRange(bin, idx);
}

//--------------------------------------------------------------------------
// SetMsrBkgRangeEntry (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runNo
 * \param idx
 * \param bin
 */
void PMsrHandler::SetMsrBkgRangeEntry(UInt_t runNo, UInt_t idx, Int_t bin)
{
  if ((runNo < 0) || (runNo > fRuns.size())) { // error
    cerr << endl << "PMsrHandler::SetMsrBkgRangeEntry: **ERROR** runNo = " << runNo << ", is out of valid range 0.." << fRuns.size();
    cerr << endl;
    return;
  }

  if (idx < 0) { // error
    cerr << endl << "PMsrHandler::SetMsrBkgRangeEntry: idx = " << idx << ", is out of valid range, i.e. idx >= 0 needed.";
    cerr << endl;
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
 * \param paramNo
 */
Int_t PMsrHandler::ParameterInUse(UInt_t paramNo)
{
  // check that paramNo is within acceptable range
  if ((paramNo < 0) || (paramNo > fParam.size()))
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
 * \param lines is a list of lines containing the fitparameter block
 */
Bool_t PMsrHandler::HandleFitParameterEntry(PMsrLines &lines)
{
  PMsrParamStructure param;
  Bool_t    error = false;

  PMsrLines::iterator iter;

  TObjArray *tokens = 0;
  TObjString *ostr = 0;
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
      cerr << endl << ">> PMsrHandler::HandleFitParameterEntry: **SEVERE ERROR** Couldn't tokenize Parameters in line " << iter->fLineNo;
      cerr << endl << endl;
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
        param.fValue = (Double_t)str.Atof();
      else
        error = true;

      // parameter value
      ostr = dynamic_cast<TObjString*>(tokens->At(3));
      str = ostr->GetString();
      if (str.IsFloat())
        param.fStep = (Double_t)str.Atof();
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
          param.fPosError = (Double_t)str.Atof();
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
          param.fPosError = (Double_t)str.Atof();
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
            param.fLowerBoundary = (Double_t)str.Atof();
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
            param.fUpperBoundary = (Double_t)str.Atof();
            param.fUpperBoundaryPresent = true;
          } else {
            error = true;
          }
        }
      }
    }

    // check if enough elements found
    if (error) {
      cerr << endl;
      cerr << endl << ">> PMsrHandler::HandleFitParameterEntry: **ERROR** in line " << iter->fLineNo << ":";
      cerr << endl << iter->fLine.Data();
      cerr << endl << "A Fit Parameter line needs to have the following form: ";
      cerr << endl;
      cerr << endl << "No Name Value Step/Error [Lower_Boundary Upper_Boundary]";
      cerr << endl;
      cerr << endl << "or";
      cerr << endl;
      cerr << endl << "No Name Value Step/Neg_Error Pos_Error [Lower_Boundary Upper_Boundary]";
      cerr << endl;
      cerr << endl << "No:             the parameter number (an Int_t)";
      cerr << endl << "Name:           the name of the parameter (less than 256 character)";
      cerr << endl << "Value:          the starting value of the parameter (a Double_t)";
      cerr << endl << "Step/Error,";
      cerr << endl << "Step/Neg_Error: the starting step value in a fit  (a Double_t), or";
      cerr << endl << "                the symmetric error (MIGRAD, SIMPLEX), or";
      cerr << endl << "                the negative error (MINOS)";
      cerr << endl << "Pos_Error:      the positive error (MINOS),  (a Double_t or \"none\")";
      cerr << endl << "Lower_Boundary: the lower boundary allowed for the fit parameter  (a Double_t or \"none\")";
      cerr << endl << "Upper_Boundary: the upper boundary allowed for the fit parameter  (a Double_t or \"none\")";
      cerr << endl;
    } else { // everything is OK, therefore add the parameter to the parameter list
      fParam.push_back(param);
    }

    // clean up
    if (tokens) {
      delete tokens;
      tokens = 0;
    }

    iter++;
  }

  // check if all parameters have subsequent numbers.
  for (UInt_t i=0; i<fParam.size(); i++) {
    if (fParam[i].fNo != (Int_t)i+1) {
      error = true;
      cerr << endl << ">> PMsrHandler::HandleFitParameterEntry: **ERROR**";
      cerr << endl << ">> Sorry, you are assuming to much from this program, it cannot";
      cerr << endl << ">> handle none subsequent numbered parameters yet or in the near future.";
      cerr << endl << ">> Found parameter " << fParam[i].fName.Data() << ", with";
      cerr << endl << ">> parameter number " << fParam[i].fNo << ", at paramter position " << i+1 << ".";
      cerr << endl << ">> This needs to be fixed first.";
      cerr << endl;
      break;
    }
  }

  return !error;
}

//--------------------------------------------------------------------------
// HandleTheoryEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param lines is a list of lines containing the fitparameter block
 */
Bool_t PMsrHandler::HandleTheoryEntry(PMsrLines &lines)
{
  // store the theory lines
  fTheory = lines;

  return true;
}

//--------------------------------------------------------------------------
// HandleFunctionsEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param lines is a list of lines containing the fitparameter block
 */
Bool_t PMsrHandler::HandleFunctionsEntry(PMsrLines &lines)
{
  // store the functions lines
  fFunctions = lines;

  // create function handler
  fFuncHandler = new PFunctionHandler(fFunctions);
  if (fFuncHandler == 0) {
    cerr << endl << ">> PMsrHandler::HandleFunctionsEntry: **ERROR** Couldn't invoke PFunctionHandler." << endl;
    return false;
  }

  // do the parsing
  if (!fFuncHandler->DoParse()) {
    return false;
  }

  // check if an empty FUNCTIONS block is present
  if ((fFuncHandler->GetNoOfFuncs() == 0) && !lines.empty()) {
    cerr << endl << ">> PMsrHandler::HandleFunctionsEntry: **ERROR** empty FUNCTIONS block are not supported!";
    cerr << endl << ">> If you want to keep it, just comment the whole block ;-)";
    cerr << endl;
    return false;
  }

  return true;
}

//--------------------------------------------------------------------------
// HandleRunEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param lines is a list of lines containing the fitparameter block
 */
Bool_t PMsrHandler::HandleRunEntry(PMsrLines &lines)
{
  PMsrLines::iterator iter;
  PMsrRunBlock param;
  Bool_t first = true; // first run line tag
  Bool_t error = false;

  TString str;
  TObjArray *tokens = 0;
  TObjString *ostr = 0;

  Int_t dval;

  iter = lines.begin();
  while ((iter != lines.end()) && !error) {
    // tokenize line
    tokens = iter->fLine.Tokenize(" \t");
    if (!tokens) {
      cerr << endl << ">> PMsrHandler::HandleRunEntry: **SEVERE ERROR** Couldn't tokenize Parameters in line " << iter->fLineNo;
      cerr << endl << endl;
      return false;
    }

    // RUN line ----------------------------------------------
    if (iter->fLine.BeginsWith("run", TString::kIgnoreCase)) {

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
        param.AppendRunName(ostr->GetString());
        // beamline
        ostr = dynamic_cast<TObjString*>(tokens->At(2));
        param.AppendBeamline(ostr->GetString());
        // institute
        ostr = dynamic_cast<TObjString*>(tokens->At(3));
        param.AppendInstitute(ostr->GetString());
        // data file format
        ostr = dynamic_cast<TObjString*>(tokens->At(4));
        param.AppendFileFormat(ostr->GetString());
      }
    }

    // ADDRUN line ---------------------------------------------
    if (iter->fLine.BeginsWith("addrun", TString::kIgnoreCase)) {
      // get run name, beamline, institute, and file-format
      if (tokens->GetEntries() < 5) {
        error = true;
      } else {
        // run name
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        param.AppendRunName(ostr->GetString());
        // beamline
        ostr = dynamic_cast<TObjString*>(tokens->At(2));
        param.AppendBeamline(ostr->GetString());
        // institute
        ostr = dynamic_cast<TObjString*>(tokens->At(3));
        param.AppendInstitute(ostr->GetString());
        // data file format
        ostr = dynamic_cast<TObjString*>(tokens->At(4));
        param.AppendFileFormat(ostr->GetString());
      }
    }

    // fittype -------------------------------------------------
    if (iter->fLine.BeginsWith("fittype", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          Int_t fittype = str.Atoi();
          if ((fittype == MSR_FITTYPE_SINGLE_HISTO) ||
              (fittype == MSR_FITTYPE_ASYM) ||
              (fittype == MSR_FITTYPE_ASYM_RRF) ||
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
    if (iter->fLine.BeginsWith("alpha", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetAlphaParamNo(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // beta -------------------------------------------------
    if (iter->fLine.BeginsWith("beta", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetBetaParamNo(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // norm -------------------------------------------------
    if (iter->fLine.BeginsWith("norm", TString::kIgnoreCase)) {
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
    if (iter->fLine.BeginsWith("backgr.fit", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetBkgFitParamNo(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // rphase ------------------------------------------------
    if (iter->fLine.BeginsWith("rphase", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetPhaseParamNo(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // lifetime ------------------------------------------------
    if (iter->fLine.BeginsWith("lifetime ", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetLifetimeParamNo(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // lifetimecorrection ---------------------------------------
    if (iter->fLine.BeginsWith("lifetimecorrection", TString::kIgnoreCase)) {
      param.SetLifetimeCorrection(true);
    }

    // map ------------------------------------------------------
    if (iter->fLine.BeginsWith("map", TString::kIgnoreCase)) {
      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval >= 0)
            param.AppendMap(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
      // check map entries, i.e. if the map values are within parameter bounds
      for (UInt_t i=0; i<param.GetMap()->size(); i++) {
        if ((param.GetMap(i) < 0) || (param.GetMap(i) > (Int_t) fParam.size())) {
          cerr << endl << ">> PMsrHandler::HandleRunEntry: **SEVERE ERROR** map value " << param.GetMap(i) << " in line " << iter->fLineNo << " is out of range!";
          error = true;
          break;
        }
      }
    }

    // forward ------------------------------------------------
    if (iter->fLine.BeginsWith("forward", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetForwardHistoNo(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // backward -----------------------------------------------
    if (iter->fLine.BeginsWith("backward", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetBackwardHistoNo(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // backgr.fix ----------------------------------------------
    if (iter->fLine.BeginsWith("backgr.fix", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        for (Int_t i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsFloat())
            param.AppendBkgFix(str.Atof());
          else
            error = true;
        }
      }
    }

    // background ---------------------------------------------
    if (iter->fLine.BeginsWith("background", TString::kIgnoreCase)) {
      if ((tokens->GetEntries() < 3) || (tokens->GetEntries() % 2 != 1)) { // odd number (>=3) of entries needed
        error = true;
      } else {
        for (Int_t i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsDigit()) {
            dval = str.Atoi();
            if (dval > 0)
              param.AppendBkgRange(dval);
            else
              error = true;
          } else {
            error = true;
          }
        }
      }
    }

    // data --------------------------------------------------
    if (iter->fLine.BeginsWith("data", TString::kIgnoreCase)) {
      if ((tokens->GetEntries() < 3) || (tokens->GetEntries() % 2 != 1)) { // odd number (>=3) of entries needed
        error = true;
      } else {
        for (Int_t i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsDigit()) {
            dval = str.Atoi();
            if (dval > 0)
              param.AppendDataRange(dval);
            else
              error = true;
          } else {
            error = true;
          }
        }
      }
    }

    // t0 -----------------------------------------------------
    if (iter->fLine.BeginsWith("t0", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        for (Int_t i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsDigit()) {
            dval = str.Atoi();
            if (dval > 0)
              param.AppendT0(dval);
            else
              error = true;
          } else {
            error = true;
          }
        }
      }
    }

    // fit -----------------------------------------------------
    if (iter->fLine.BeginsWith("fit ", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 3) {
        error = true;
      } else {
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

    // packing --------------------------------------------------
    if (iter->fLine.BeginsWith("packing", TString::kIgnoreCase)) {
      if (tokens->GetEntries() != 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetPacking(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // rrffrequency --------------------------------------------------
    if (iter->fLine.BeginsWith("rrffrequency", TString::kIgnoreCase)) {
      if (tokens->GetEntries() != 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsFloat())
          param.SetRRFFreq(str.Atof());
        else
          error = true;
      }
    }

    // rrfpacking --------------------------------------------------
    if (iter->fLine.BeginsWith("rrfpacking", TString::kIgnoreCase)) {
      if (tokens->GetEntries() != 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetRRFPacking(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // alpha2 --------------------------------------------------
    if (iter->fLine.BeginsWith("alpha2", TString::kIgnoreCase)) {
      if (tokens->GetEntries() != 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetAlpha2ParamNo(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // beta2 --------------------------------------------------
    if (iter->fLine.BeginsWith("beta2", TString::kIgnoreCase)) {
      if (tokens->GetEntries() != 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetBeta2ParamNo(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // right --------------------------------------------------
    if (iter->fLine.BeginsWith("right", TString::kIgnoreCase)) {
      if (tokens->GetEntries() != 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetRightHistoNo(dval);
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // left --------------------------------------------------
    if (iter->fLine.BeginsWith("left", TString::kIgnoreCase)) {
      if (tokens->GetEntries() != 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit()) {
          dval = str.Atoi();
          if (dval > 0)
            param.SetLeftHistoNo(str.Atoi());
          else
            error = true;
        } else {
          error = true;
        }
      }
    }

    // xy-data -----------------------------------------------
    if (iter->fLine.BeginsWith("xy-data", TString::kIgnoreCase)) {
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
            dval = str.Atoi();
            if (dval > 0)
              param.SetYDataIndex(dval); // y-index
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
      tokens = 0;
    }

    ++iter;
  }

  if (error) {
    --iter;
    cerr << endl << ">> PMsrHandler::HandleRunEntry: **ERROR** in line " << iter->fLineNo << ":";
    cerr << endl << iter->fLine.Data();
    cerr << endl << "RUN block syntax is too complex to print it here. Please check the manual.";
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
    cstr = 0;
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
 * <p> In the command block there shall be a new command which can be used
 * to switch between chi2 and maximum_likelihood!!
 *
 * \param lines is a list of lines containing the fitparameter block
 */
Bool_t PMsrHandler::HandleCommandsEntry(PMsrLines &lines)
{
  PMsrLines::iterator iter;

  if (lines.empty()) {
    cerr << endl << "**WARNING**: There is no COMMANDS block! Do you really want this?";
    cerr << endl;
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
 * <p>
 *
 * \param fourier fourier parameters
 */
void PMsrHandler::InitFourierParameterStructure(PMsrFourierStructure &fourier)
{
  fourier.fFourierBlockPresent = false;           // fourier block present
  fourier.fUnits = FOURIER_UNIT_NOT_GIVEN;        // fourier untis, default: NOT GIVEN
  fourier.fFourierPower = -1;                     // zero padding, default: -1 = NOT GIVEN
  fourier.fApodization = FOURIER_APOD_NOT_GIVEN;  // apodization, default: NOT GIVEN
  fourier.fPlotTag = FOURIER_PLOT_NOT_GIVEN;      // initial plot tag, default: NOT GIVEN
  fourier.fPhaseParamNo = 0;                      // initial parameter no = 0 means not a parameter
  fourier.fPhase = -999.0;                        // fourier phase: -999 = NOT GIVEN
  for (UInt_t i=0; i<2; i++) {
    fourier.fRangeForPhaseCorrection[i] = -1.0;  // frequency range for phase correction, default: {-1, -1} = NOT GIVEN
    fourier.fPlotRange[i] = -1.0;                // fourier plot range, default: {-1, -1} = NOT GIVEN
  }
}

//--------------------------------------------------------------------------
// HandleFourierEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param lines is a list of lines containing the fourier parameter block
 */
Bool_t PMsrHandler::HandleFourierEntry(PMsrLines &lines)
{
//cout << endl << ">> in PMsrHandler::HandleFourierEntry ...";

  Bool_t error = false;

  if (lines.empty()) // no fourier block present
    return true;

//cout << endl << ">> in PMsrHandler::HandleFourierEntry, Fourier block present ...";

  PMsrFourierStructure fourier;

  InitFourierParameterStructure(fourier);

  fourier.fFourierBlockPresent = true;

  PMsrLines::iterator iter;

  TObjArray *tokens = 0;
  TObjString *ostr = 0;
  TString str;

  Int_t ival;

  iter = lines.begin();
  while ((iter != lines.end()) && !error) {
    // tokenize line
    tokens = iter->fLine.Tokenize(" \t");
    if (!tokens) {
      cerr << endl << ">> PMsrHandler::HandleRunEntry: **SEVERE ERROR** Couldn't tokenize Parameters in line " << iter->fLineNo;
      cerr << endl << endl;
      return false;
    }

    // units -----------------------------------------------
    if (iter->fLine.BeginsWith("units", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) { // units are missing
        error = true;
        continue;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (!str.CompareTo("gauss", TString::kIgnoreCase)) {
          fourier.fUnits = FOURIER_UNIT_FIELD;
        } else if (!str.CompareTo("mhz", TString::kIgnoreCase)) {
          fourier.fUnits = FOURIER_UNIT_FREQ;
        } else if (!str.CompareTo("mc/s", TString::kIgnoreCase)) {
          fourier.fUnits = FOURIER_UNIT_CYCLES;
        } else {
          error = true;
          continue;
        }
      }
    }

    // fourier power (zero padding) ------------------------
    if (iter->fLine.BeginsWith("fourier_power", TString::kIgnoreCase)) {
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
    }

    // apodization -----------------------------------------
    if (iter->fLine.BeginsWith("apodization", TString::kIgnoreCase)) {
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
    }

    // plot tag --------------------------------------------
    if (iter->fLine.BeginsWith("plot", TString::kIgnoreCase)) {
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
        } else { // unrecognized plot tag
          error = true;
          continue;
        }
      }
    }

    // phase -----------------------------------------------
    if (iter->fLine.BeginsWith("phase", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) { // phase value is missing
        error = true;
        continue;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.BeginsWith("par", TString::kIgnoreCase)) { // parameter value
          Int_t no = 0;
          if (FilterNumber(str, "par", 0, no)) {
            // check that the parameter is in range
            if ((Int_t)fParam.size() < no) {
              error = true;
              continue;
            }
            // keep the parameter number
            fourier.fPhaseParamNo = no;
            // get parameter value
            fourier.fPhase = fParam[no-1].fValue;
          } else {
            error = true;
            continue;
          }
        } else if (str.IsFloat()) { // phase value
          fourier.fPhase = str.Atof();
        } else {
          error = true;
          continue;
        }
      }
    }

    // range for automatic phase correction ----------------
    if (iter->fLine.BeginsWith("range_for_phase_correction", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 3) { // range values are missing
        error = true;
        continue;
      } else {
        for (UInt_t i=0; i<2; i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i+1));
          str = ostr->GetString();
          if (str.IsFloat()) {
            fourier.fRangeForPhaseCorrection[i] = str.Atof();
          } else {
            error = true;
            continue;
          }
        }
      }
    }

    // fourier plot range ----------------------------------
    if (iter->fLine.BeginsWith("range", TString::kIgnoreCase)) {
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
    }

    // clean up
    if (tokens) {
      delete tokens;
      tokens = 0;
    }


    ++iter;
  }

  if (error) {
    cerr << endl << ">> PMsrHandler::HandleFourierEntry: **ERROR** in line " << iter->fLineNo << ":";
    cerr << endl;
    cerr << endl << iter->fLine.Data();
    cerr << endl;
    cerr << endl << "FOURIER block syntax, parameters in [] are optinal:";
    cerr << endl;
    cerr << endl << "FOURIER";
    cerr << endl << "[units Gauss | MHz | Mc/s]";
    cerr << endl << "[fourier_power n # n is a number such that zero padding up to 2^n will be used]";
    cerr << endl << "   n=0 means no zero padding";
    cerr << endl << "   0 <= n <= 20 are allowed values";
    cerr << endl << "[apodization none | weak | medium | strong]";
    cerr << endl << "[plot real | imag | real_and_imag | power | phase]";
    cerr << endl << "[phase value]";
    cerr << endl << "[range_for_phase_correction min max]";
    cerr << endl << "[range min max]";
    cerr << endl;
  } else { // save last run found
    fFourier = fourier;
  }

  return !error;
}

//--------------------------------------------------------------------------
// HandlePlotEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param lines is a list of lines containing the fitparameter block
 */
Bool_t PMsrHandler::HandlePlotEntry(PMsrLines &lines)
{
  Bool_t error = false;

  PMsrPlotStructure param;

  PMsrLines::iterator iter1;
  PMsrLines::iterator iter2;
  TObjArray *tokens = 0;
  TObjArray *tokens2 = 0;
  TObjString *ostr = 0;
  TString str;
  TString str2;

  if (lines.empty()) {
    cerr << endl << "**WARNING**: There is no PLOT block! Do you really want this?";
    cerr << endl;
  }

  iter1 = lines.begin();
  while ((iter1 != lines.end()) && !error) {

    // initialize param structure
    param.fPlotType = -1;
    param.fUseFitRanges = false; // i.e. if not overwritten use the range info of the plot block
    param.fLogX = false; // i.e. if not overwritten use linear x-axis
    param.fLogY = false; // i.e. if not overwritten use linear y-axis
    param.fViewPacking = -1; // i.e. if not overwritten use the packing of the run blocks

    // find next plot if any is present
    iter2 = iter1;
    ++iter2;
    for ( ; iter2 != lines.end(); ++iter2) {
      if (iter2->fLine.Contains("PLOT"))
        break;
    }

    // handle a single PLOT block
    while ((iter1 != iter2) && !error) {

      if (iter1->fLine.Contains("PLOT")) { // handle plot header
        tokens = iter1->fLine.Tokenize(" \t");
        if (!tokens) {
          cerr << endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize PLOT in line " << iter1->fLineNo;
          cerr << endl << endl;
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
          tokens = 0;
        }
      } else if (iter1->fLine.Contains("runs", TString::kIgnoreCase)) { // handle plot runs
        TComplex run;
        switch (param.fPlotType) {
          case -1:
            error = true;
            break;
          case MSR_PLOT_SINGLE_HISTO: // like: runs 1 5 13
          case MSR_PLOT_ASYM:
          case MSR_PLOT_NON_MUSR:
            tokens = iter1->fLine.Tokenize(" \t");
            if (!tokens) {
              cerr << endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize PLOT in line " << iter1->fLineNo;
              cerr << endl << endl;
              return false;
            }
            if (tokens->GetEntries() < 2) { // runs missing
              error = true;
            } else {
              for (Int_t i=1; i<tokens->GetEntries(); i++) {
                ostr = dynamic_cast<TObjString*>(tokens->At(i));
                str = ostr->GetString();
                if (str.IsDigit()) {
                  run = TComplex(str.Atoi(),-1.0);
                  param.fRuns.push_back(run);
                } else {
                  error = true;
                }
              }
            }
            // clean up
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          case MSR_PLOT_ASYM_RRF: // like: runs 1,1 1,2
            tokens = iter1->fLine.Tokenize(" \t");
            if (!tokens) {
              cerr << endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize PLOT in line " << iter1->fLineNo;
              cerr << endl << endl;
              return false;
            }
            if (tokens->GetEntries() < 2) { // runs missing
              error = true;
            } else {
              for (Int_t i=1; i<tokens->GetEntries(); i++) {
                ostr = dynamic_cast<TObjString*>(tokens->At(i)); // something like 1,2
                str = ostr->GetString();
                tokens2 = str.Tokenize(",");
                if (!tokens2) {
                  error = true;
                } else {
                  ostr = dynamic_cast<TObjString*>(tokens2->At(0)); // real part
                  str = ostr->GetString();
                  ostr = dynamic_cast<TObjString*>(tokens2->At(1)); // imag part
                  str2 = ostr->GetString();
                  if (str.IsDigit() && str2.IsDigit()) {
                    run = TComplex(str.Atoi(),str2.Atoi());
                    param.fRuns.push_back(run);
                  } else {
                    error = true;
                  }
                }
                if (tokens2) {
                  delete tokens2;
                  tokens2 = 0;
                }
              }
            }
            // clean up
            if (tokens) {
              delete tokens;
              tokens = 0;
            }
            break;
          default:
            error = true;
            break;
        }
      } else if (iter1->fLine.Contains("range ", TString::kIgnoreCase)) { // handle plot range
        // remove previous entries
        param.fTmin.clear();
        param.fTmax.clear();
        param.fYmin.clear();
        param.fYmax.clear();

        tokens = iter1->fLine.Tokenize(" \t");
        if (!tokens) {
          cerr << endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize PLOT in line " << iter1->fLineNo;
          cerr << endl << endl;
          return false;
        }
        if ((tokens->GetEntries() != 3) && (tokens->GetEntries() != 5)) {
          error = true;
        } else {

          // handle t_min
          ostr = dynamic_cast<TObjString*>(tokens->At(1));
          str = ostr->GetString();
          if (str.IsFloat())
            param.fTmin.push_back((Double_t)str.Atof());
          else
            error = true;

          // handle t_max
          ostr = dynamic_cast<TObjString*>(tokens->At(2));
          str = ostr->GetString();
          if (str.IsFloat())
            param.fTmax.push_back((Double_t)str.Atof());
          else
            error = true;

          if (tokens->GetEntries() == 5) { // y-axis interval given as well

            // handle y_min
            ostr = dynamic_cast<TObjString*>(tokens->At(3));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fYmin.push_back((Double_t)str.Atof());
            else
              error = true;

            // handle y_max
            ostr = dynamic_cast<TObjString*>(tokens->At(4));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fYmax.push_back((Double_t)str.Atof());
            else
              error = true;
          }
        }
        // clean up
        if (tokens) {
          delete tokens;
          tokens = 0;
        }
      } else if (iter1->fLine.Contains("sub_ranges", TString::kIgnoreCase)) {
        // remove previous entries
        param.fTmin.clear();
        param.fTmax.clear();
        param.fYmin.clear();
        param.fYmax.clear();

        tokens = iter1->fLine.Tokenize(" \t");
        if (!tokens) {
          cerr << endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize PLOT in line " << iter1->fLineNo;
          cerr << endl << endl;
          return false;
        }
        if ((tokens->GetEntries() != (Int_t)(2*param.fRuns.size() + 1)) && (tokens->GetEntries() != (Int_t)(2*param.fRuns.size() + 3))) {
          error = true;
        } else {
          // get all the times
          for (UInt_t i=0; i<param.fRuns.size(); i++) {

            // handle t_min
            ostr = dynamic_cast<TObjString*>(tokens->At(2*i+1));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fTmin.push_back((Double_t)str.Atof());
            else
              error = true;

            // handle t_max
            ostr = dynamic_cast<TObjString*>(tokens->At(2*i+2));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fTmax.push_back((Double_t)str.Atof());
            else
              error = true;
          }

          // get y-range if present
          if (tokens->GetEntries() == (Int_t)(2*param.fRuns.size() + 3)) {

            // handle y_min
            ostr = dynamic_cast<TObjString*>(tokens->At(2*param.fRuns.size()+1));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fYmin.push_back((Double_t)str.Atof());
            else
              error = true;

            // handle y_max
            ostr = dynamic_cast<TObjString*>(tokens->At(2*param.fRuns.size()+2));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fYmax.push_back((Double_t)str.Atof());
            else
              error = true;
          }
        }

        // clean up
        if (tokens) {
          delete tokens;
          tokens = 0;
        }
      } else if (iter1->fLine.Contains("use_fit_ranges", TString::kIgnoreCase)) {
        param.fUseFitRanges = true;
      } else if (iter1->fLine.Contains("logx", TString::kIgnoreCase)) {
        param.fLogX = true;
      } else if (iter1->fLine.Contains("logy", TString::kIgnoreCase)) {
        param.fLogY = true;
      } else if (iter1->fLine.Contains("view_packing", TString::kIgnoreCase)) {
        tokens = iter1->fLine.Tokenize(" \t");
        if (!tokens) {
          cerr << endl << ">> PMsrHandler::HandlePlotEntry: **SEVERE ERROR** Couldn't tokenize PLOT in line " << iter1->fLineNo;
          cerr << endl << endl;
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
          tokens = 0;
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
        fPlots.push_back(param);
      }
    }

    if (error) { // print error message
      --iter1;
      cerr << endl << ">> PMsrHandler::HandlePlotEntry: **ERROR** in line " << iter1->fLineNo << ": " << iter1->fLine.Data();
      cerr << endl << "A PLOT block needs to have the following structure:";
      cerr << endl;
      cerr << endl << "PLOT <plot_type>";
      cerr << endl << "runs <run_list>";
      cerr << endl << "[range tmin tmax [ymin ymax]]";
      cerr << endl << "[sub_ranges tmin1 tmax1 tmin2 tmax2 ... tminN tmaxN [ymin ymax]";
      cerr << endl << "[logx | logy]";
      cerr << endl << "[use_fit_ranges]";
      cerr << endl << "[view_packing n]";
      cerr << endl;
      cerr << endl << "where <plot_type> is: 0=single histo asym,";
      cerr << endl << "                      2=forward-backward asym,";
      cerr << endl << "                      4=RRF asym (not implemented yet),";
      cerr << endl << "                      8=non muSR.";
      cerr << endl << "<run_list> is the list of runs";
      cerr << endl << "   for <plot_type> 0,2,8 it is a list of run numbers, e.g. runs 1 3";
      cerr << endl << "   for <plot_type> 4     it is a list of 'complex' numbers, where";
      cerr << endl << "                         the real part is the run number, and the";
      cerr << endl << "                         imaginary one is 1=real part or 2=imag part, e.g.";
      cerr << endl << "                         runs 1,1 1,2";
      cerr << endl << "range is optional";
      cerr << endl << "sub_ranges (if present) will plot the N given runs each on its own sub-range";
      cerr << endl << "logx, logy (if present) will present the x-, y-axis in log-scale";
      cerr << endl << "use_fit_ranges (if present) will plot each run on its fit-range";
      cerr << endl << "view_packing n (if present) will bin all data by n (> 0) rather than the binning of the fit";
      cerr << endl;
    }

    param.fRuns.clear();

  }

  return !error;
}

//--------------------------------------------------------------------------
// HandleStatisticEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param lines is a list of lines containing the fitparameter block
 */
Bool_t PMsrHandler::HandleStatisticEntry(PMsrLines &lines)
{
  if (lines.empty()) {
    cerr << endl << ">> PMsrHandler::HandleStatisticEntry: **WARNING** There is no STATISTIC block! Do you really want this?";
    cerr << endl;
    fStatistic.fValid = false;
    return true;
  }

  // check if chisq or max.log likelihood
  fStatistic.fChisq = true;
  for (UInt_t i=0; i<fCommands.size(); i++) {
    if (fCommands[i].fLine.Contains("MAX_LIKELIHOOD"))
      fStatistic.fChisq = false; // max.log likelihood
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
          !tstr.BeginsWith("maxLH") && !tstr.BeginsWith("*** FIT DID NOT CONVERGE ***")) {
        cerr << endl << ">> PMsrHandler::HandleStatisticEntry: **SYNTAX ERROR** in line " << lines[i].fLineNo;
        cerr << endl << ">> '" << lines[i].fLine.Data() << "'";
        cerr << endl << ">> not a valid STATISTIC block line";
        cerr << endl << ">> If you do not understand this, just remove the STATISTIC block, musrfit will recreate after fitting";
        cerr << endl << endl;
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
      fStatistic.fValid = true;
      strncpy(str, lines[i].fLine.Data(), sizeof(str));
      status = sscanf(str+lines[i].fLine.Index("chisq = ")+8, "%lf", &dval);
      if (status == 1) {
        fStatistic.fMin = dval;
      } else {
        fStatistic.fMin = -1.0;
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

// cout << endl << "Statistic:";
// cout << endl << " Date & Time: " << fStatistic.fDate.Data();
// if (fStatistic.fChisq) { // chisq
//   cout << endl << " chisq = " << fStatistic.fMin;
//   cout << endl << " NDF   = " << fStatistic.fNdf;
// } else { // maximum likelihood
//   cout << endl << " maxLH = " << fStatistic.fMin;
//   cout << endl << " NDF   = " << fStatistic.fNdf;
// }

  return true;
}

//--------------------------------------------------------------------------
// FillParameterInUse (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param theory
 * \param funcs
 * \param run
 */
void PMsrHandler::FillParameterInUse(PMsrLines &theory, PMsrLines &funcs, PMsrLines &run)
{
  PIntVector map;
  PIntVector fun;
  PMsrLines::iterator iter;
  TObjArray  *tokens = 0;
  TObjString *ostr = 0;
  TString    str;
  Int_t        ival, funNo;

  // create and initialize fParamInUse vector
  for (UInt_t i=0; i<fParam.size(); i++)
    fParamInUse.push_back(0);

  // go through all the theory lines ------------------------------------
//cout << endl << ">> theory block check ...";
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
        if ((ival > 0) && (ival < (Int_t)fParam.size()+1)) {
          fParamInUse[ival-1]++;
//cout << endl << ">>>> theo: param no : " << ival;
        }
      } else if (str.Contains("map")) { // map
        if (FilterNumber(str, "map", MSR_PARAM_MAP_OFFSET, ival))
          map.push_back(ival-MSR_PARAM_MAP_OFFSET);
      } else if (str.Contains("fun")) { // fun
//cout << endl << "theo:fun: " << str.Data();
        if (FilterNumber(str, "fun", MSR_PARAM_FUN_OFFSET, ival))
          fun.push_back(ival-MSR_PARAM_FUN_OFFSET);
      }
    }

    // delete tokens
    if (tokens) {
      delete tokens;
      tokens = 0;
    }
  }

  // go through all the function lines: 1st time -----------------------------
//cout << endl << ">> function block check (1st time) ...";
  for (iter = funcs.begin(); iter != funcs.end(); ++iter) {
    // remove potential comments
    str = iter->fLine;
    if (str.First('#') != -1)
      str.Resize(str.First('#'));

    // everything to lower case
    str.ToLower();

// cout << endl << "-----------------------";
// cout << endl << ">> " << str.Data();

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
//cout << endl << ">> fun.size() = " << fun.size();
    for (UInt_t i=0; i<fun.size(); i++) {
//cout << endl << ">> funNo: " << fun[i] << ", funNo: " << funNo;
      if (fun[i] == funNo) { // function number found
        // filter for parX
        sstr = iter->fLine;
        Char_t sval[128];
        while (sstr.Index("par") != -1) {
          memset(sval, 0, sizeof(sval));
          sstr = &sstr[sstr.Index("par")+3]; // trunc sstr
//cout << endl << ">> par:sstr: " << sstr.Data();
          for (Int_t j=0; j<sstr.Sizeof(); j++) {
            if (!isdigit(sstr[j]))
              break;
            sval[j] = sstr[j];
          }
          sscanf(sval, "%d", &ival);
//cout << endl << ">>>> parX from func 1st, X = " << ival;
          fParamInUse[ival-1]++;
        }

        // filter for mapX
        sstr = iter->fLine;
        while (sstr.Index("map") != -1) {
          memset(sval, 0, sizeof(sval));
          sstr = &sstr[sstr.Index("map")+3]; // trunc sstr
//cout << endl << ">> map:sstr: " << sstr.Data();
          for (Int_t j=0; j<sstr.Sizeof(); j++) {
            if (!isdigit(sstr[j]))
              break;
            sval[j] = sstr[j];
          }
          sscanf(sval, "%d", &ival);
//cout << endl << ">>>> mapX from func 1st, X = " << ival;
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
      tokens = 0;
    }
  }

  // go through all the run block lines -------------------------------------
//cout << endl << ">> run block check (1st time) ...";
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
        str.Contains("rphase") || str.Contains("lifetime ")) {
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
//cout << endl << ">>>> run : parameter no : " << ival;
      }
      // check if fun
      if (str.Contains("fun")) {
        if (FilterNumber(str, "fun", MSR_PARAM_FUN_OFFSET, ival)) {
          fun.push_back(ival-MSR_PARAM_FUN_OFFSET);
//cout << endl << ">>>> run : fun no : " << ival-MSR_PARAM_FUN_OFFSET;
        }
      }

      // delete tokens
      if (tokens) {
        delete tokens;
        tokens = 0;
      }
    }

    // handle the maps
    if (str.Contains("map")) {
//cout << endl << ">> " << str.Data();
      // tokenize string
      tokens = str.Tokenize(" \t");
      if (!tokens)
        continue;

      // get the parameter number via map
//cout << endl << ">> map.size() = " << map.size();
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
//cout << endl << ">>>> param no : " << ival << ", via map no : " << map[i];
            }
          }
        }
      }

      // delete tokens
      if (tokens) {
        delete tokens;
        tokens = 0;
      }
    }
  }

  // go through all the function lines: 2nd time -----------------------------
//cout << endl << ">> function block check (2nd time) ...";
  for (iter = funcs.begin(); iter != funcs.end(); ++iter) {
    // remove potential comments
    str = iter->fLine;
    if (str.First('#') != -1)
      str.Resize(str.First('#'));

    // everything to lower case
    str.ToLower();

// cout << endl << "===========================";
// cout << endl << ">> " << str.Data();

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
//cout << endl << ">> funNo: " << fun[i] << ", funNo: " << funNo;
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
//cout << endl << ">>>> parX from func 2nd, X = " << ival;
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
//cout << endl << ">>>> mapX from func 2nd, X = " << ival;
          // check if map value already in map, otherwise add it
          if (ival > 0) {
            UInt_t pos;
            for (pos=0; pos<map.size(); pos++) {
              if (ival == map[pos])
                break;
            }
            if ((UInt_t)pos == map.size()) { // new map value
              map.push_back(ival);
            }
          }
        }
      }
    }

    // delete tokens
    if (tokens) {
      delete tokens;
      tokens = 0;
    }
  }

  // go through all the run block lines 2nd time to filter remaining maps
//cout << endl << ">> run block check (2nd time) ...";
  for (iter = run.begin(); iter != run.end(); ++iter) {
    // remove potential comments
    str = iter->fLine;
    if (str.First('#') != -1)
      str.Resize(str.First('#'));

    // everything to lower case
    str.ToLower();

    // handle the maps
    if (str.Contains("map")) {
//cout << endl << ">> " << str.Data();
      // tokenize string
      tokens = str.Tokenize(" \t");
      if (!tokens)
        continue;

      // get the parameter number via map
//cout << endl << ">> map.size() = " << map.size();
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
//cout << endl << ">>>> param no : " << ival << ", via map no : " << map[i];
            }
          }
        }
      }

      // delete tokens
      if (tokens) {
        delete tokens;
        tokens = 0;
      }
    }
  }

// cout << endl << ">> fParamInUse: ";
// for (UInt_t i=0; i<fParamInUse.size(); i++)
//   cout << endl << i+1 << ", " << fParamInUse[i];
// cout << endl;

  // if unused parameters are present, set the step value to 0.0
  for (UInt_t i=0; i<fParam.size(); i++) {
    if (!ParameterInUse(i)) {
      if (fParam[i].fStep != 0.0) {
        cerr << endl << "**WARNING** : Parameter No " << i+1 << " is not used at all, will fix it" << endl;
        fParam[i].fStep = 0.0;
      }
    }
  }

  // clean up
  map.clear();
  fun.clear();
}

//--------------------------------------------------------------------------
// CheckUniquenessOfParamNames (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param parX
 * \param parY
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
// CheckMaps (private)
//--------------------------------------------------------------------------
/**
 * <p>Checks if map entries found in the theory- or function-block are also
 * present in the run-block, if yes return true otherwise return false.
 */
Bool_t PMsrHandler::CheckMaps()
{
  Bool_t result = true;

  PIntVector mapVec;
  PIntVector mapBlock;
  PIntVector mapLineNo;

  TObjArray  *tokens = 0;
  TObjString *ostr = 0;
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
        tokens = 0;
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
        tokens = 0;
      }
    }
  }

  // check if present maps are found in the run-block
  Bool_t found;
  for (UInt_t i=0; i<mapVec.size(); i++) { // loop over found maps in theory- and function-block
    found = false;
    for (UInt_t j=0; j<fRuns.size(); j++) { // loop over all run-blocks
      if ((mapVec[i]-MSR_PARAM_MAP_OFFSET-1 < (Int_t)fRuns[j].GetMap()->size()) &&
          (mapVec[i]-MSR_PARAM_MAP_OFFSET-1 >= 0)) { // map value smaller than run-block map length
        if (fRuns[j].GetMap(mapVec[i]-MSR_PARAM_MAP_OFFSET-1) != 0) { // map value in the run-block != 0
          found = true;
          break;
        }
      }
    }
    if (!found) { // map not found
      result = false;
      cerr << endl << ">> PMsrHandler::CheckMaps: **ERROR** map" << mapVec[i]-MSR_PARAM_MAP_OFFSET << " found in the ";
      if (mapBlock[i] == 0)
        cerr << "theory-block ";
      else
        cerr << "functions-block ";
      cerr << "in line " << mapLineNo[i] << " is not present in the run-block!";
      cerr << endl;
      if (mapVec[i]-MSR_PARAM_MAP_OFFSET == 0) {
        cerr << endl << ">> by the way: map must be > 0 ...";
        cerr << endl;
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
// CheckFuncs (private)
//--------------------------------------------------------------------------
/**
 * <p>Checks if fun entries found in the theory- and run-block are also
 * present in the functions-block, if yes return true otherwise return false.
 */
Bool_t PMsrHandler::CheckFuncs()
{
  Bool_t result = true;

  PIntVector funVec;
  PIntVector funBlock;
  PIntVector funLineBlockNo;

  TObjArray  *tokens = 0;
  TObjString *ostr = 0;
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
        tokens = 0;
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
      cerr << endl << ">> PMsrHandler::CheckFuncs: **ERROR** fun" << funVec[i]-MSR_PARAM_FUN_OFFSET << " found in the ";
      if (funBlock[i] == 0)
        cerr << "theory-block in line " << funLineBlockNo[i] << " is not present in the functions-block!";
      else
        cerr << "run-block No " << funLineBlockNo[i] << " (norm) is not present in the functions-block!";
      cerr << endl;
    }
  }

  // clean up
  funVec.clear();
  funBlock.clear();
  funLineBlockNo.clear();

  return result;
}

// end ---------------------------------------------------------------------
