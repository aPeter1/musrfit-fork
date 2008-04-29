/***************************************************************************

  PMsrHandler.cpp

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
PMsrHandler::PMsrHandler(char *fileName)
{
  // init variables
  fFileName = fileName;
  fMsrBlockCounter = 0;

  fTitle = "";

  fStatistic.fChisq = true;
  fStatistic.fMin   = -1.0;
  fStatistic.fNdf   = 0;

  fFuncHandler = 0;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 */
PMsrHandler::~PMsrHandler()
{
  fComments.clear();
  fParam.clear();
  fTheory.clear();
  fFunctions.clear();
  fRuns.clear();
  fCommands.clear();
  fPlots.clear();
  fStatistic.fStatLines.clear();
  fParamInUse.clear();
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
int PMsrHandler::ReadMsrFile()
{
  ifstream f;
  char str[256];
  TString line;
  int line_no = 0;
  int error = PMUSR_SUCCESS;

  PMsrLineStructure current;

  PMsrLines fit_parameter;
  PMsrLines theory;
  PMsrLines functions;
  PMsrLines run;
  PMsrLines commands;
  PMsrLines plot;
  PMsrLines statistic;

  // open msr-file
  f.open(fFileName.Data(), iostream::in);
  if (!f.is_open()) {
    return PMUSR_MSR_FILE_NOT_FOUND;
  }

  fMsrBlockCounter = -1; // no msr block

  // read msr-file
  while (!f.eof()) {

    // read a line
    f.getline(str, sizeof(str));
    line = str;
    line_no++;

    current.fLineNo = line_no;
    current.fLine   = line;

    if (line.BeginsWith("#") || line.IsWhitespace()) { // if the line is a comment/empty line keep it
      fComments.push_back(current);
      continue;
    }

    if (!line.IsWhitespace()) { // if not an empty line, handle it
      // check for a msr block
      if (line_no == 1) { // title
        fTitle = line;
      } else if (line.Contains("FITPARAMETER")) { // FITPARAMETER block tag
        fMsrBlockCounter = MSR_TAG_FITPARAMETER;
      } else if (line.Contains("THEORY")) {       // THEORY block tag
        fMsrBlockCounter = MSR_TAG_THEORY;
        theory.push_back(current);
      } else if (line.Contains("FUNCTIONS")) {    // FUNCTIONS block tag
        fMsrBlockCounter = MSR_TAG_FUNCTIONS;
        functions.push_back(current);
      } else if (line.Contains("RUN")) {          // RUN block tag
        fMsrBlockCounter = MSR_TAG_RUN;
        run.push_back(current);
      } else if (line.Contains("COMMANDS")) {     // COMMANDS block tag
        fMsrBlockCounter = MSR_TAG_COMMANDS;
        commands.push_back(current);
      } else if (line.Contains("PLOT")) {         // PLOT block tag
        fMsrBlockCounter = MSR_TAG_PLOT;
        plot.push_back(current);
      } else if (line.Contains("STATISTIC")) {    // STATISTIC block tag
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
    error = PMUSR_MSR_SYNTAX_ERROR;
  if (!HandleTheoryEntry(theory))
    error = PMUSR_MSR_SYNTAX_ERROR;
  if (!HandleFunctionsEntry(functions))
    error = PMUSR_MSR_SYNTAX_ERROR;
  if (!HandleRunEntry(run))
    error = PMUSR_MSR_SYNTAX_ERROR;
  if (!HandleCommandsEntry(commands))
    error = PMUSR_MSR_SYNTAX_ERROR;
  if (!HandlePlotEntry(plot))
    error = PMUSR_MSR_SYNTAX_ERROR;
  if (!HandleStatisticEntry(statistic))
    error = PMUSR_MSR_SYNTAX_ERROR;

  // fill parameter-in-use vector
  FillParameterInUse(theory, functions, run);

  // check that parameter names are unique
  unsigned int parX, parY;
  if (!CheckUniquenessOfParamNames(parX, parY)) {
    cout << endl << "PMsrHandler::ReadMsrFile: **SEVERE ERROR** parameter name " << fParam[parX].fName.Data() << " is identical for parameter no " << fParam[parX].fNo << " and " << fParam[parY].fNo << "!";
    cout << endl << "Needs to be fixed first!";
    error = PMUSR_MSR_SYNTAX_ERROR;
  }

  // clean up
  fit_parameter.clear();
  theory.clear();
  functions.clear();
  run.clear();
  commands.clear();
  plot.clear();
  statistic.clear();

// cout << endl << "# Comments: ";
// for (unsigned int i=0; i<fComments.size(); i++) {
//   cout << endl << fComments[i].fLineNo << " " << fComments[i].fLine.Data();
// }
// cout << endl;

  return error;
}

//--------------------------------------------------------------------------
// WriteMsrLogFile (public)
//--------------------------------------------------------------------------
/**
 * <p>
 */
int PMsrHandler::WriteMsrLogFile()
{
  const unsigned int prec = 6; // output precision for float/doubles

  TObjArray *tokens;
  TObjString *ostr;
  TString str;

  // construct log file name
  tokens = fFileName.Tokenize(".");
  if (!tokens)
    return PMUSR_TOKENIZE_ERROR;
  ostr = dynamic_cast<TObjString*>(tokens->At(0));
  str = ostr->GetString();
  str += ".mlog";

  // clean up
  if (tokens) {
    delete tokens;
    tokens = 0;
  }

  ofstream f;

  // open mlog-file
  f.open(str.Data(), iostream::out);
  if (!f.is_open()) {
    return PMUSR_MSR_LOG_FILE_WRITE_ERROR;
  }

  // write mlog-file
  int lineNo = 1;

  // write title
  f << fTitle.Data();
  CheckAndWriteComment(f, ++lineNo);

  // write fit parameter block
  f << endl << "FITPARAMETER";
  CheckAndWriteComment(f, ++lineNo);
  f << endl;
  for (unsigned int i=0; i<fParam.size(); i++) {
    // parameter no
    f.width(9);
    f << right << fParam[i].fNo;
    f << " ";
    // parameter name
    f.width(11);
    f << left << fParam[i].fName.Data();
    f << " ";
    // value of the parameter
    f.width(9);
    f.precision(prec);
    f << left << fParam[i].fValue;
    f << " ";
    // value of step/error/neg.error
    f.width(11);
    f.precision(prec);
    f << left << fParam[i].fStep;
    f << " ";
    f.width(11);
    f.precision(prec);
    if ((fParam[i].fNoOfParams == 5) || (fParam[i].fNoOfParams == 7)) // pos. error given
      if (fParam[i].fPosErrorPresent) // pos error is a number
        f << left << fParam[i].fPosError;
      else // pos error is a none
        f << left << "none";
    else // no pos. error
      f << left << "none";
    f << " ";
    // boundaries
    if (fParam[i].fNoOfParams > 5) {
      f.width(7);
      f.precision(prec);
      f << left << fParam[i].fLowerBoundary;
      f << " ";
      f.width(7);
      f.precision(prec);
      f << left << fParam[i].fUpperBoundary;
      f << " ";
    }
    CheckAndWriteComment(f, ++lineNo);
    // terminate parameter line if not the last line
    if (i != fParam.size()-1)
      f << endl;
  }

  // write theory block
  for (unsigned int i=0; i<fTheory.size(); i++) {
    f << endl << fTheory[i].fLine.Data();
    CheckAndWriteComment(f, ++lineNo);
  }

  // write functions block
  f << endl << "FUNCTIONS";
  CheckAndWriteComment(f, ++lineNo);
  for (int i=0; i<GetNoOfFuncs(); i++) {
    str = *fFuncHandler->GetFuncString(i);
    str.ToLower();
    f << endl << str.Data();
    CheckAndWriteComment(f, ++lineNo);
  }

  // write run block
  for (unsigned int i=0; i<fRuns.size(); i++) {
    // run header
    f << endl << "RUN " << fRuns[i].fRunName.Data() << " ";
    str = fRuns[i].fBeamline;
    str.ToUpper();
    f << str.Data() << " ";
    str = fRuns[i].fInstitute;
    str.ToUpper();
    f << str.Data() << " ";
    str = fRuns[i].fFileFormat;
    str.ToUpper();
    f << str.Data() << "   (name beamline institute data-file-format)";
    CheckAndWriteComment(f, ++lineNo);
    // fittype
    f.width(16);
    switch (fRuns[i].fFitType) {
      case MSR_FITTYPE_SINGLE_HISTO:
        f << endl << left << "fittype" << MSR_FITTYPE_SINGLE_HISTO << "         (single histogram fit)";
        break;
      case MSR_FITTYPE_ASYM:
        f << endl << left << "fittype" << MSR_FITTYPE_ASYM << "         (asymmetry fit)";
        break;
      case MSR_FITTYPE_ASYM_RRF:
        f << endl << left << "fittype" << MSR_FITTYPE_ASYM_RRF << "         (RRF asymmetry fit)";
        break;
      case MSR_FITTYPE_NO_MUSR:
        f << endl << left << "fittype" << MSR_FITTYPE_NO_MUSR << "         (non muSR fit)";
        break;
      default:
        break;
    }
    CheckAndWriteComment(f, ++lineNo);
    // rrffrequency
    if (fRuns[i].fRRFFreq != -1.0) {
      f.width(16);
      f << endl << left << "rrffrequency";
      f.precision(prec);
      f << fRuns[i].fRRFFreq;
      CheckAndWriteComment(f, ++lineNo);
    }
    // rrfpacking
    if (fRuns[i].fRRFPacking != -1) {
      f.width(16);
      f << endl << left << "rrfpacking";
      f.precision(prec);
      f << fRuns[i].fRRFPacking;
      CheckAndWriteComment(f, ++lineNo);
    }
    // alpha
    if (fRuns[i].fAlphaParamNo != -1) {
      f.width(16);
      f << endl << left << "alpha";
      f << fRuns[i].fAlphaParamNo;
      CheckAndWriteComment(f, ++lineNo);
    }
    // beta
    if (fRuns[i].fBetaParamNo != -1) {
      f.width(16);
      f << endl << left << "beta";
      f << fRuns[i].fBetaParamNo;
      CheckAndWriteComment(f, ++lineNo);
    }
    // alpha2
    if (fRuns[i].fAlpha2ParamNo != -1) {
      f.width(16);
      f << endl << left << "alpha2";
      f << fRuns[i].fAlpha2ParamNo;
      CheckAndWriteComment(f, ++lineNo);
    }
    // beta2
    if (fRuns[i].fBeta2ParamNo != -1) {
      f.width(16);
      f << endl << left << "beta2";
      f << fRuns[i].fBeta2ParamNo;
      CheckAndWriteComment(f, ++lineNo);
    }
    // norm
    if (fRuns[i].fNormParamNo != -1) {
      f.width(16);
      f << endl << left << "norm";
      // check if norm is give as a function
      if (fRuns[i].fNormParamNo >= MSR_PARAM_FUN_OFFSET)
        f << "fun" << fRuns[i].fNormParamNo-MSR_PARAM_FUN_OFFSET;
      else
        f << fRuns[i].fNormParamNo;
      CheckAndWriteComment(f, ++lineNo);
    }
    // backgr.fit
    if (fRuns[i].fBkgFitParamNo != -1) {
      f.width(16);
      f << endl << left << "backgr.fit";
      f << fRuns[i].fBkgFitParamNo;
      CheckAndWriteComment(f, ++lineNo);
    }
    // rphase
    if (fRuns[i].fPhaseParamNo != -1) {
      f.width(16);
      f << endl << left << "rphase";
      f << fRuns[i].fPhaseParamNo;
      CheckAndWriteComment(f, ++lineNo);
    }
    // lifetime
    if (fRuns[i].fLifetimeParamNo != -1) {
      f.width(16);
      f << endl << left << "lifetime";
      f << fRuns[i].fLifetimeParamNo;
      CheckAndWriteComment(f, ++lineNo);
    }
    // lifetimecorrection
    if ((fRuns[i].fLifetimeCorrection) && (fRuns[i].fFitType == MSR_FITTYPE_SINGLE_HISTO)) {
      f << endl << "lifetimecorrection";
      CheckAndWriteComment(f, ++lineNo);
    }
    // map
    f << endl << "map         ";
    for (unsigned int j=0; j<fRuns[i].fMap.size(); j++) {
      f.width(5);
      f << right << fRuns[i].fMap[j];
    }
    // if there are less maps then 10 fill with zeros
    if (fRuns[i].fMap.size() < 10) {
      for (unsigned int j=fRuns[i].fMap.size(); j<10; j++)
        f << "    0";
    }
    CheckAndWriteComment(f, ++lineNo);
    // forward
    if (fRuns[i].fForwardHistoNo != -1) {
      f.width(16);
      f << endl << left << "forward";
      f << fRuns[i].fForwardHistoNo;
      CheckAndWriteComment(f, ++lineNo);
    }
    // backward
    if (fRuns[i].fBackwardHistoNo != -1) {
      f.width(16);
      f << endl << left << "backward";
      f << fRuns[i].fBackwardHistoNo;
      CheckAndWriteComment(f, ++lineNo);
    }
    // right
    if (fRuns[i].fRightHistoNo != -1) {
      f.width(16);
      f << endl << left << "right";
      f << fRuns[i].fRightHistoNo;
      CheckAndWriteComment(f, ++lineNo);
    }
    // left
    if (fRuns[i].fLeftHistoNo != -1) {
      f.width(16);
      f << endl << left << "left";
      f << fRuns[i].fLeftHistoNo;
      CheckAndWriteComment(f, ++lineNo);
    }
    // backgr.fix
    if (!isnan(fRuns[i].fBkgFix[0])) {
      f.width(15);
      f << endl << left << "backgr.fix";
      for (unsigned int j=0; j<2; j++) {
        f.precision(prec);
        f.width(12);
        f << left << fRuns[i].fBkgFix[j];
      }
      CheckAndWriteComment(f, ++lineNo);
    }
    // background
    if (fRuns[i].fBkgRange[0] != -1) {
      f.width(16);
      f << endl << "background";
      for (unsigned int j=0; j<4; j++) {
        if (fRuns[i].fBkgRange[j] == -1)
          break;
        f.width(8);
        f << left << fRuns[i].fBkgRange[j];
      }
      CheckAndWriteComment(f, ++lineNo);
    }
    // data
    if (fRuns[i].fDataRange[0] != -1) {
      f.width(16);
      f << endl << "data";
      for (unsigned int j=0; j<4; j++) {
        if (fRuns[i].fDataRange[j] == -1)
          break;
        f.width(8);
        f << left << fRuns[i].fDataRange[j];
      }
      CheckAndWriteComment(f, ++lineNo);
    }
    // t0
    if (fRuns[i].fT0[0] != -1) {
      f.width(16);
      f << endl << "t0";
      for (unsigned int j=0; j<2; j++) {
        if (fRuns[i].fT0[j] == -1)
          break;
        f.width(8);
        f << left << fRuns[i].fT0[j];
      }
      CheckAndWriteComment(f, ++lineNo);
    }
    // fit
    if (fRuns[i].fFitRange[0] != -1) {
      f.width(16);
      f << endl << "fit";
      for (unsigned int j=0; j<2; j++) {
        if (fRuns[i].fFitRange[j] == -1)
          break;
        f.width(8);
        f.precision(2);
        f << left << fixed << fRuns[i].fFitRange[j];
      }
      CheckAndWriteComment(f, ++lineNo);
    }
    // packing
    f.width(16);
    f << endl << left << "packing";
    f << fRuns[i].fPacking;
    CheckAndWriteComment(f, ++lineNo);
  }

  // write command block
  f << endl << "COMMANDS";
  CheckAndWriteComment(f, ++lineNo);
  for (unsigned int i=0; i<fCommands.size(); i++) {
    f << endl << fCommands[i].fLine.Data();
    CheckAndWriteComment(f, ++lineNo);
  }

  // write plot block
  for (unsigned int i=0; i<fPlots.size(); i++) {
    // plot header
    switch (fPlots[i].fPlotType) {
      case MSR_PLOT_SINGLE_HISTO:
        f << endl << "PLOT " << fPlots[i].fPlotType << "   (single histo plot)";
        break;
      case MSR_PLOT_ASYM:
        f << endl << "PLOT " << fPlots[i].fPlotType << "   (asymmetry plot)";
        break;
      case MSR_PLOT_ASYM_RRF:
        f << endl << "PLOT " << fPlots[i].fPlotType << "   (rotating reference frame plot)";
        break;
      case MSR_PLOT_NO_MUSR:
        f << endl << "PLOT " << fPlots[i].fPlotType << "   (non muSR plot)";
        break;
      default:
        break;
    }
    CheckAndWriteComment(f, ++lineNo);
    // runs
    f << endl << "runs     ";
    f.precision(0);
    for (unsigned int j=0; j<fPlots[i].fRuns.size(); j++) {
      if (fPlots[i].fPlotType != MSR_PLOT_ASYM_RRF) { // all but MSR_PLOT_ASYM_RRF
        f.width(4);
        f << fPlots[i].fRuns[j].Re();
      } else { // MSR_PLOT_ASYM_RRF
        f << fPlots[i].fRuns[j].Re() << "," << fPlots[i].fRuns[j].Im() << "    ";
      }
    }
    CheckAndWriteComment(f, ++lineNo);
    // range
    f << endl << "range    ";
    f.precision(2);
    f << fPlots[i].fTmin << "   " << fPlots[i].fTmax;
    if (fPlots[i].fYmin != -999.0) {
      f << "   " << fPlots[i].fYmin << "   " << fPlots[i].fYmax;
    }
    CheckAndWriteComment(f, ++lineNo);
  }

  // write statistic block
  TDatime dt;
  f << endl << "STATISTIC --- " << dt.AsSQLString();
  CheckAndWriteComment(f, ++lineNo);
  // if fMin and fNdf are given, make new statistic block
  if ((fStatistic.fMin != -1.0) && (fStatistic.fNdf != 0)) {
    // throw away the old statistics block
    fStatistic.fStatLines.clear();
    // create the new statistics block
    PMsrLineStructure line;
    if (fStatistic.fChisq) { // chi^2
      line.fLine  = "  chisq = ";
      line.fLine += fStatistic.fMin;
      line.fLine += ", NDF = ";
      line.fLine += fStatistic.fNdf;
      line.fLine += ", chisq/NDF = ";
      line.fLine += fStatistic.fMin / fStatistic.fNdf;
    } else {
      line.fLine  = "  maxLH = ";
      line.fLine += fStatistic.fMin;
      line.fLine += ", NDF = ";
      line.fLine += fStatistic.fNdf;
      line.fLine += ", maxLH/NDF = ";
      line.fLine += fStatistic.fMin / fStatistic.fNdf;
    }
    fStatistic.fStatLines.push_back(line);
  }
  // write the statistics block
  for (unsigned int i=0; i<fStatistic.fStatLines.size(); i++) {
    f << endl << fStatistic.fStatLines[i].fLine.Data();
    CheckAndWriteComment(f, ++lineNo);
  }

  // close mlog-file
  f.close();

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
bool PMsrHandler::SetMsrParamValue(unsigned int i, double value)
{
  if (i > fParam.size()) {
    cout << endl << "PMsrHandler::SetMsrParamValue(): i = " << i << " is larger than the number of parameters " << fParam.size();
    cout << endl;
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
bool PMsrHandler::SetMsrParamStep(unsigned int i, double value)
{
  if (i > fParam.size()) {
    cout << endl << "PMsrHandler::SetMsrParamValue(): i = " << i << " is larger than the number of parameters " << fParam.size();
    cout << endl;
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
bool PMsrHandler::SetMsrParamPosErrorPresent(unsigned int i, bool value)
{
  if (i > fParam.size()) {
    cout << endl << "PMsrHandler::SetMsrParamPosErrorPresent(): i = " << i << " is larger than the number of parameters " << fParam.size();
    cout << endl;
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
bool PMsrHandler::SetMsrParamPosError(unsigned int i, double value)
{
  if (i > fParam.size()) {
    cout << endl << "PMsrHandler::SetMsrParamPosError(): i = " << i << " is larger than the number of parameters " << fParam.size();
    cout << endl;
    return false;
  }

  fParam[i].fPosErrorPresent = true;
  fParam[i].fPosError = value;

  return true;
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
int PMsrHandler::ParameterInUse(unsigned int paramNo)
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
 * x  x    x     x                        x            x              -> 6 Parameters, e.g. after a MIGRAD fit
 * x  x    x     x              x                                     -> 5 Parameters, e.g. after a MINOS fit
 *                                                                                     without boundaries
 * x  x    x     x                                                    -> 4 Parameters, e.g. after MIGRAD fit
 *                                                                                     without bounderies, or
 *                                                                                     when starting
 * \endcode
 *
 * \param lines is a list of lines containing the fitparameter block
 */
bool PMsrHandler::HandleFitParameterEntry(PMsrLines &lines)
{
  PMsrParamStructure param;
  bool    error = false;

  PMsrLines::iterator iter;

  TObjArray *tokens;
  TObjString *ostr;
  TString str;

  // init param structure
  param.fNoOfParams      = -1;
  param.fNo              = -1;
  param.fName            = TString("");
  param.fValue           = nan("NAN");
  param.fStep            = nan("NAN");
  param.fPosErrorPresent = nan("NAN");
  param.fPosError        = nan("NAN");
  param.fLowerBoundary   = nan("NAN");
  param.fUpperBoundary   = nan("NAN");

  // fill param structure
  iter = lines.begin();
  while ((iter != lines.end()) && !error) {

    tokens = iter->fLine.Tokenize(" \t");
    if (!tokens) {
      cout << endl << "SEVERE ERROR: Couldn't tokenize Parameters in line " << iter->fLineNo;
      cout << endl << endl;
      return false;
    }

    // handle various input possiblities
    if ((tokens->GetEntries() < 4) || (tokens->GetEntries() > 7)) {
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
        param.fValue = (double)str.Atof();
      else
        error = true;

      // parameter value
      ostr = dynamic_cast<TObjString*>(tokens->At(3));
      str = ostr->GetString();
      if (str.IsFloat())
        param.fStep = (double)str.Atof();
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
          param.fPosError = (double)str.Atof();
        } else {
          str.ToLower();
          if (!str.CompareTo("none"))
            param.fPosErrorPresent = false;
          else
            error = true;
        }
      }

      // 6 values, i.e. No Name Value Error Lower_Bounderay Upper_Boundary
      if (tokens->GetEntries() == 6) {
        param.fNoOfParams = 6;

        // lower boundary
        ostr = dynamic_cast<TObjString*>(tokens->At(4));
        str = ostr->GetString();
        if (str.IsFloat())
          param.fLowerBoundary = (double)str.Atof();
        else
          error = true;

        // upper boundary
        ostr = dynamic_cast<TObjString*>(tokens->At(5));
        str = ostr->GetString();
        if (str.IsFloat())
          param.fUpperBoundary = (double)str.Atof();
        else
          error = true;
      }

      // 7 values, i.e. No Name Value Neg_Error Pos_Error Lower_Bounderay Upper_Boundary
      if (tokens->GetEntries() == 7) {
        param.fNoOfParams = 7;

        // positive error
        ostr = dynamic_cast<TObjString*>(tokens->At(4));
        str = ostr->GetString();
        if (str.IsFloat()) {
          param.fPosErrorPresent = true;
          param.fPosError = (double)str.Atof();
        } else {
          str.ToLower();
          if (!str.CompareTo("none"))
            param.fPosErrorPresent = false;
          else
            error = true;
        }

        // lower boundary
        ostr = dynamic_cast<TObjString*>(tokens->At(5));
        str = ostr->GetString();
        if (str.IsFloat())
          param.fLowerBoundary = (double)str.Atof();
        else
          error = true;

        // upper boundary
        ostr = dynamic_cast<TObjString*>(tokens->At(6));
        str = ostr->GetString();
        if (str.IsFloat())
          param.fUpperBoundary = (double)str.Atof();
        else
          error = true;
      }
    }

    // check if enough elements found
    if (error) {
      cout << endl << "ERROR in line " << iter->fLineNo << ":";
      cout << endl << iter->fLine.Data();
      cout << endl << "A Fit Parameter line needs to have the following form: ";
      cout << endl;
      cout << endl << "No Name Value Step/Error [Lower_Boundary Upper_Boundary]";
      cout << endl;
      cout << endl << "or";
      cout << endl;
      cout << endl << "No Name Value Step/Neg_Error Pos_Error [Lower_Boundary Upper_Boundary]";
      cout << endl;
      cout << endl << "No:             the parameter number (an int)";
      cout << endl << "Name:           the name of the parameter (less than 256 character)";
      cout << endl << "Value:          the starting value of the parameter (a double)";
      cout << endl << "Step/Error,";
      cout << endl << "Step/Neg_Error: the starting step value in a fit  (a double), or";
      cout << endl << "                the symmetric error (MIGRAD, SIMPLEX), or";
      cout << endl << "                the negative error (MINOS)";
      cout << endl << "Pos_Error:      the positive error (MINOS),  (a double)";
      cout << endl << "Lower_Boundary: the lower boundary allowed for the fit parameter  (a double)";
      cout << endl << "Upper_Boundary: the upper boundary allowed for the fit parameter  (a double)";
      cout << endl;
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
bool PMsrHandler::HandleTheoryEntry(PMsrLines &lines)
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
bool PMsrHandler::HandleFunctionsEntry(PMsrLines &lines)
{
  // store the functions lines
  fFunctions = lines;

  // create function handler
  fFuncHandler = new PFunctionHandler(fFunctions);
  if (fFuncHandler == 0) {
    cout << endl << "**ERROR**: PMsrHandler::HandleFunctionsEntry: Couldn't invoke PFunctionHandler";
    return false;
  }

  // do the parsing
  if (!fFuncHandler->DoParse()) {
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
bool PMsrHandler::HandleRunEntry(PMsrLines &lines)
{
  PMsrLines::iterator iter;
  PMsrRunStructure param;
  bool first = true; // first run line tag
  bool error = false;

  TString str;
  TObjString *ostr;
  TObjArray *tokens;

  iter = lines.begin();
  while ((iter != lines.end()) && !error) {
    // tokenize line
    tokens = iter->fLine.Tokenize(" \t");
    if (!tokens) {
      cout << endl << "SEVERE ERROR: Couldn't tokenize Parameters in line " << iter->fLineNo;
      cout << endl << endl;
      return false;
    }

    // RUN line ----------------------------------------------
    if (iter->fLine.BeginsWith("run", TString::kIgnoreCase)) {

      if (!first) // not the first run in the list
        fRuns.push_back(param);
      else
        first = false;

      InitRunParameterStructure(param);

      // get run name, beamline, institute, and file-format
      if (tokens->GetEntries() < 5) {
        error = true;
      } else {
        // run name
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        param.fRunName = ostr->GetString();
        // beamline
        ostr = dynamic_cast<TObjString*>(tokens->At(2));
        param.fBeamline = ostr->GetString();
        // institute
        ostr = dynamic_cast<TObjString*>(tokens->At(3));
        param.fInstitute = ostr->GetString();
        // data file format
        ostr = dynamic_cast<TObjString*>(tokens->At(4));
        param.fFileFormat = ostr->GetString();
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
          int fittype = str.Atoi();
          if ((fittype == MSR_FITTYPE_SINGLE_HISTO) ||
              (fittype == MSR_FITTYPE_ASYM) ||
              (fittype == MSR_FITTYPE_ASYM_RRF) ||
              (fittype == MSR_FITTYPE_NO_MUSR)) {
            param.fFitType = fittype;
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
        if (str.IsDigit())
          param.fAlphaParamNo = str.Atoi();
        else
          error = true;
      }
    }

    // beta -------------------------------------------------
    if (iter->fLine.BeginsWith("beta", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit())
          param.fBetaParamNo = str.Atoi();
        else
          error = true;
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
          param.fNormParamNo = str.Atoi();
        } else if (str.Contains("fun")) {
          int no;
          if (FilterFunMapNumber(str, "fun", no))
            param.fNormParamNo = no;
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
        if (str.IsDigit())
          param.fBkgFitParamNo = str.Atoi();
        else
          error = true;
      }
    }

    // rphase ------------------------------------------------
    if (iter->fLine.BeginsWith("rphase", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit())
          param.fPhaseParamNo = str.Atoi();
        else
          error = true;
      }
    }

    // lifetime ------------------------------------------------
    if (iter->fLine.BeginsWith("lifetime ", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit())
          param.fLifetimeParamNo = str.Atoi();
        else
          error = true;
      }
    }

    // lifetimecorrection ---------------------------------------
    if (iter->fLine.BeginsWith("lifetimecorrection", TString::kIgnoreCase)) {
      param.fLifetimeCorrection = true;
    }

    // map ------------------------------------------------------
    if (iter->fLine.BeginsWith("map", TString::kIgnoreCase)) {
      for (int i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        str = ostr->GetString();
        if (str.IsDigit())
          // only fill map vector until a first zero is encountered
          if (str.Atoi() != 0)
            param.fMap.push_back(str.Atoi());
          else
            break;
        else
          error = true;
      }
      // check map entries, i.e. if the map values are within parameter bounds
      for (unsigned int i=0; i<param.fMap.size(); i++) {
        if ((param.fMap[i] <= 0) || (param.fMap[i] > (int) fParam.size())) {
          cout << endl << "**SEVERE ERROR** in PMsrHandler::HandleRunEntry: map value " << param.fMap[i] << " in line " << iter->fLineNo << " is out of range!";
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
        if (str.IsDigit())
          param.fForwardHistoNo = str.Atoi();
        else
          error = true;
      }
    }

    // backward -----------------------------------------------
    if (iter->fLine.BeginsWith("backward", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit())
          param.fBackwardHistoNo = str.Atoi();
        else
          error = true;
      }
    }

    // backgr.fix ----------------------------------------------
    if (iter->fLine.BeginsWith("backgr.fix", TString::kIgnoreCase)) {
      if (tokens->GetEntries() != 3) {
        error = true;
      } else {
        for (int i=1; i<3; i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsFloat())
            param.fBkgFix[i-1] = str.Atof();
          else
            error = true;
        }
      }
    }

    // background ---------------------------------------------
    if (iter->fLine.BeginsWith("background", TString::kIgnoreCase)) {
      if ((tokens->GetEntries() != 3) && (tokens->GetEntries() != 5)) {
        error = true;
      } else {
        for (int i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsDigit())
            param.fBkgRange[i-1] = str.Atoi();
          else
            error = true;
        }
      }
    }

    // data --------------------------------------------------
    if (iter->fLine.BeginsWith("data", TString::kIgnoreCase)) {
      if ((tokens->GetEntries() != 3) && (tokens->GetEntries() != 5)) {
        error = true;
      } else {
        for (int i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsDigit())
            param.fDataRange[i-1] = str.Atoi();
          else
            error = true;
        }
      }
    }

    // t0 -----------------------------------------------------
    if (iter->fLine.BeginsWith("t0", TString::kIgnoreCase)) {
      if ((tokens->GetEntries() != 2) && (tokens->GetEntries() != 3)) {
        error = true;
      } else {
        for (int i=1; i<tokens->GetEntries(); i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsDigit())
            param.fT0[i-1] = str.Atoi();
          else
            error = true;
        }
      }
    }

    // fit -----------------------------------------------------
    if (iter->fLine.BeginsWith("fit ", TString::kIgnoreCase)) {
      if (tokens->GetEntries() < 3) {
        error = true;
      } else {
        for (int i=1; i<3; i++) {
          ostr = dynamic_cast<TObjString*>(tokens->At(i));
          str = ostr->GetString();
          if (str.IsFloat())
            param.fFitRange[i-1] = str.Atof();
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
        if (str.IsDigit())
          param.fPacking = str.Atoi();
        else
          error = true;
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
          param.fRRFFreq = str.Atof();
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
        if (str.IsDigit())
          param.fRRFPacking = str.Atoi();
        else
          error = true;
      }
    }

    // alpha2 --------------------------------------------------
    if (iter->fLine.BeginsWith("alpha2", TString::kIgnoreCase)) {
      if (tokens->GetEntries() != 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit())
          param.fAlpha2ParamNo = str.Atoi();
        else
          error = true;
      }
    }

    // beta2 --------------------------------------------------
    if (iter->fLine.BeginsWith("beta2", TString::kIgnoreCase)) {
      if (tokens->GetEntries() != 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit())
          param.fBeta2ParamNo = str.Atoi();
        else
          error = true;
      }
    }

    // right --------------------------------------------------
    if (iter->fLine.BeginsWith("right", TString::kIgnoreCase)) {
      if (tokens->GetEntries() != 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit())
          param.fRightHistoNo = str.Atoi();
        else
          error = true;
      }
    }

    // left --------------------------------------------------
    if (iter->fLine.BeginsWith("left", TString::kIgnoreCase)) {
      if (tokens->GetEntries() != 2) {
        error = true;
      } else {
        ostr = dynamic_cast<TObjString*>(tokens->At(1));
        str = ostr->GetString();
        if (str.IsDigit())
          param.fLeftHistoNo = str.Atoi();
        else
          error = true;
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
    cout << endl << "ERROR in line " << iter->fLineNo << ":";
    cout << endl << iter->fLine.Data();
    cout << endl << "RUN block syntax is too complex to print it here. Please check the manual.";
  } else { // save last run found
    fRuns.push_back(param);
  }

  return !error;
}

//--------------------------------------------------------------------------
// InitRunParameterStructure (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param param
 */
void PMsrHandler::InitRunParameterStructure(PMsrRunStructure &param)
{
  param.fRunName         = "";
  param.fBeamline        = "";
  param.fInstitute       = "";
  param.fFileFormat      = "";
  param.fFitType         = -1;
  param.fAlphaParamNo    = -1;
  param.fBetaParamNo     = -1;
  param.fNormParamNo     = -1;
  param.fBkgFitParamNo   = -1;
  param.fPhaseParamNo    = -1;
  param.fLifetimeParamNo = -1;
  param.fLifetimeCorrection = false;
  param.fMap.clear(); // empty list
  param.fForwardHistoNo  = -1;
  param.fBackwardHistoNo = -1;
  for (int i=0; i<2; i++)
    param.fBkgFix[i] = nan("NAN");
  for (int i=0; i<4; i++)
    param.fBkgRange[i] = -1;
  for (int i=0; i<4; i++)
    param.fDataRange[i] = -1;
  for (int i=0; i<2; i++)
    param.fT0[i] = -1;
  for (int i=0; i<4; i++)
    param.fFitRange[i] = -1;
  param.fPacking = 1;
  param.fRRFFreq       = -1.0;
  param.fRRFPacking    = -1;
  param.fAlpha2ParamNo = -1;
  param.fBeta2ParamNo  = -1;
  param.fRightHistoNo  = -1;
  param.fLeftHistoNo   = -1;
}

//--------------------------------------------------------------------------
// FilterFunMapNumber (private)
//--------------------------------------------------------------------------
/**
 * <p>Used to filter numbers from a string of the structure strX, where
 * X is a number. The filter string is used to define the offset to X.
 * It is used to filter strings like: map1 or fun4. At the moment only
 * the filter strings 'map' and 'fun' are supported.
 *
 * \param str  input string
 * \param filter filter string
 * \param no filtered number
 */
bool PMsrHandler::FilterFunMapNumber(TString str, const char *filter, int &no)
{
  int  found, no_found=-1;

  // copy str to an ordinary c-like string
  char *cstr;
  cstr = new char[str.Sizeof()];
  strncpy(cstr, str.Data(), str.Sizeof());

  // get number if present
  if (!strcmp(filter, "fun")) {
    found = sscanf(cstr, "fun%d", &no_found);
    if (found == 1)
      if (no_found < 1000)
        no = no_found + MSR_PARAM_FUN_OFFSET;
  } else if (!strcmp(filter, "map")) {
    found = sscanf(cstr, "map%d", &no_found);
    if (found == 1)
      if (no_found < 1000)
        no = no_found + MSR_PARAM_MAP_OFFSET;
  }

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
bool PMsrHandler::HandleCommandsEntry(PMsrLines &lines)
{
  PMsrLines::iterator iter;

  if (lines.empty()) {
    cout << endl << "WARNING: There is no COMMANDS block! Do you really want this?";
    cout << endl;
  }

  for (iter = lines.begin(); iter != lines.end(); ++iter) {
    if (!iter->fLine.BeginsWith("COMMANDS"))
      fCommands.push_back(*iter);
  }

  return true;
}

//--------------------------------------------------------------------------
// HandlePlotEntry (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param lines is a list of lines containing the fitparameter block
 */
bool PMsrHandler::HandlePlotEntry(PMsrLines &lines)
{
  bool error = false;

  PMsrPlotStructure param;

  PMsrLines::iterator iter1;
  PMsrLines::iterator iter2;
  TObjArray *tokens;
  TObjArray *tokens2;
  TObjString *ostr;
  TString str;
  TString str2;

  if (lines.empty()) {
    cout << endl << "WARNING: There is no PLOT block! Do you really want this?";
    cout << endl;
  }

  iter1 = lines.begin();
  while ((iter1 != lines.end()) && !error) {

    // initialize param structure
    param.fPlotType = -1;
    param.fTmin = -999.0;
    param.fTmax = -999.0;
    param.fYmin = -999.0;
    param.fYmax = -999.0;

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
          cout << endl << "SEVERE ERROR: Couldn't tokenize PLOT in line " << iter1->fLineNo;
          cout << endl << endl;
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
      }

      if (iter1->fLine.Contains("runs")) { // handle plot runs
        TComplex run;
        switch (param.fPlotType) {
          case -1:
            error = true;
            break;
          case MSR_PLOT_SINGLE_HISTO: // like: runs 1 5 13
          case MSR_PLOT_ASYM:
          case MSR_PLOT_NO_MUSR:
            tokens = iter1->fLine.Tokenize(" \t");
            if (!tokens) {
              cout << endl << "SEVERE ERROR: Couldn't tokenize PLOT in line " << iter1->fLineNo;
              cout << endl << endl;
              return false;
            }
            if (tokens->GetEntries() < 2) { // runs missing
              error = true;
            } else {
              for (int i=1; i<tokens->GetEntries(); i++) {
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
              cout << endl << "SEVERE ERROR: Couldn't tokenize PLOT in line " << iter1->fLineNo;
              cout << endl << endl;
              return false;
            }
            if (tokens->GetEntries() < 2) { // runs missing
              error = true;
            } else {
              for (int i=1; i<tokens->GetEntries(); i++) {
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
      }

      if (iter1->fLine.Contains("range")) { // handle plot range
        tokens = iter1->fLine.Tokenize(" \t");
        if (!tokens) {
          cout << endl << "SEVERE ERROR: Couldn't tokenize PLOT in line " << iter1->fLineNo;
          cout << endl << endl;
          return false;
        }
        if ((tokens->GetEntries() != 3) && (tokens->GetEntries() != 5)) {
          error = true;
        } else {

          // handle t_min
          ostr = dynamic_cast<TObjString*>(tokens->At(1));
          str = ostr->GetString();
          if (str.IsFloat())
            param.fTmin =  (double)str.Atof();
          else
            error = true;

          // handle t_max
          ostr = dynamic_cast<TObjString*>(tokens->At(2));
          str = ostr->GetString();
          if (str.IsFloat())
            param.fTmax =  (double)str.Atof();
          else
            error = true;

          if (tokens->GetEntries() == 5) { // y-axis interval given as well

            // handle y_min
            ostr = dynamic_cast<TObjString*>(tokens->At(3));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fYmin =  (double)str.Atof();
            else
              error = true;

            // handle y_max
            ostr = dynamic_cast<TObjString*>(tokens->At(4));
            str = ostr->GetString();
            if (str.IsFloat())
              param.fYmax =  (double)str.Atof();
            else
              error = true;
          }
        }
        // clean up
        if (tokens) {
          delete tokens;
          tokens = 0;
        }
      }

      ++iter1;

    }

    // analyze if the plot block is valid
    if (!error) {
      if (param.fRuns.empty()) { // there was no run tag
        error = true;
      } else { // everything ok
        if ((param.fTmin != -999.0) || (param.fTmax != -999.0)) { // if range is given, check that it is ordered properly
          if (param.fTmin > param.fTmax) {
            double keep = param.fTmin;
            param.fTmin = param.fTmax;
            param.fTmax = keep;
          }
        }

        if ((param.fYmin != -999.0) || (param.fYmax != -999.0)) { // if range is given, check that it is ordered properly
          if (param.fYmin > param.fYmax) {
            double keep = param.fYmin;
            param.fYmin = param.fYmax;
            param.fYmax = keep;
          }
        }

        fPlots.push_back(param);
      }
    }

    if (error) { // print error message
      --iter1;
      cout << endl << "ERROR in line " << iter1->fLineNo << ": " << iter1->fLine.Data();
      cout << endl << "A PLOT block needs to have the following structure:";
      cout << endl;
      cout << endl << "PLOT <plot_type>";
      cout << endl << "runs <run_list>";
      cout << endl << "[range tmin tmax [ymin ymax]]";
      cout << endl;
      cout << endl << "where <plot_type> is: 0=single histo asym,";
      cout << endl << "                      2=forward-backward asym,"; 
      cout << endl << "                      4=RRF asym (not implemented yet),";
      cout << endl << "                      8=non muSR.";
      cout << endl << "<run_list> is the list of runs";
      cout << endl << "   for <plot_type> 0,2,8 it is a list of run numbers, e.g. runs 1 3";
      cout << endl << "   for <plot_type> 4     it is a list of 'complex' numbers, where";
      cout << endl << "                         the real part is the run number, and the";
      cout << endl << "                         imaginary one is 1=real part or 2=imag part, e.g.";
      cout << endl << "                         runs 1,1 1,2";
      cout << endl << "range is optional";
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
bool PMsrHandler::HandleStatisticEntry(PMsrLines &lines)
{
  if (lines.empty()) {
    cout << endl << "WARNING: There is no STATISTIC block! Do you really want this?";
    cout << endl;
    return false;
  }

  // check if chisq or max.log likelihood
  fStatistic.fChisq = true;
  for (unsigned int i=0; i<fCommands.size(); i++) {
    if (fCommands[i].fLine.Contains("MAX_LIKELIHOOD"))
      fStatistic.fChisq = false; // max.log likelihood
  }

  char str[128];
  char date[128];
  char time[128];
  int  status;
  double dval;
  unsigned int ival;
  for (unsigned int i=0; i<lines.size(); i++) {
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
  int        ival;

  // create and initialize fParamInUse vector
  for (unsigned int i=0; i<fParam.size(); i++)
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
    for (int i=0; i<tokens->GetEntries(); i++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(i));
      str = ostr->GetString();
      if (str.IsDigit()) { // parameter number
        ival = str.Atoi();
        if ((ival > 0) && (ival < (int)fParam.size()+1)) {
          fParamInUse[ival-1]++;
//cout << endl << ">>>> theo: param no : " << ival;
        }
      } else if (str.Contains("map")) { // map
        if (FilterFunMapNumber(str, "map", ival))
          map.push_back(ival-MSR_PARAM_MAP_OFFSET);
      } else if (str.Contains("fun")) { // fun
//cout << endl << "theo:fun: " << str.Data();
        if (FilterFunMapNumber(str, "fun", ival))
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
  for (iter = funcs.begin(); iter != funcs.end(); ++iter) {
    // remove potential comments
    str = iter->fLine;
    if (str.First('#') != -1)
      str.Resize(str.First('#'));

    // everything to lower case
    str.ToLower();

//cout << endl << ">> " << str.Data();

    tokens = str.Tokenize(" /t");
    if (!tokens)
      continue;

    // filter fun number
    ostr = dynamic_cast<TObjString*>(tokens->At(0));
    str = ostr->GetString();
    if (!FilterFunMapNumber(str, "fun", ival))
      continue;
    ival -= MSR_PARAM_FUN_OFFSET;

    // check if fun number is used, and if yes, filter parameter numbers and maps
    TString sstr;
    for (unsigned int i=0; i<fun.size(); i++) {
//cout << endl << ">> funNo: " << fun[i];
      if (fun[i] == ival) { // function number found
        // filter for parX
        sstr = iter->fLine;
        char sval[128];
        while (sstr.Index("par") != -1) {
          memset(sval, 0, sizeof(sval));
          sstr = &sstr[sstr.Index("par")+3]; // trunc sstr
//cout << endl << ">> par:sstr: " << sstr.Data();
          for (int j=0; j<sstr.Sizeof(); j++) {
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
          for (int j=0; j<sstr.Sizeof(); j++) {
            if (!isdigit(sstr[j]))
              break;
            sval[j] = sstr[j];
          }
          sscanf(sval, "%d", &ival);
//cout << endl << ">>>> mapX from func 1st, X = " << ival;
          // check if map value already in map, otherwise add it
          if (ival > 0) {
            unsigned int pos;
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
        if (FilterFunMapNumber(str, "fun", ival)) {
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
      for (unsigned int i=0; i<map.size(); i++) {
        if (map[i] < tokens->GetEntries()) {
          ostr = dynamic_cast<TObjString*>(tokens->At(map[i]));
          str = ostr->GetString();
          if (str.IsDigit()) {
            ival = str.Atoi();
            fParamInUse[ival-1]++; // this is OK since map is ranging from 1 ..
//cout << endl << ">>>> param no : " << ival << ", via map no : " << map[i];
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
    if (!FilterFunMapNumber(str, "fun", ival))
      continue;
    ival -= MSR_PARAM_FUN_OFFSET;

    // check if fun number is used, and if yes, filter parameter numbers and maps
    TString sstr;
    for (unsigned int i=0; i<fun.size(); i++) {
      if (fun[i] == ival) { // function number found
        // filter for parX
        sstr = iter->fLine;
        char sval[128];
        while (sstr.Index("par") != -1) {
          memset(sval, 0, sizeof(sval));
          sstr = &sstr[sstr.Index("par")+3]; // trunc sstr
          for (int j=0; j<sstr.Sizeof(); j++) {
            if (!isdigit(sstr[j]))
              break;
            sval[j] = sstr[j];
          }
          sscanf(sval, "%d", &ival);
//cout << endl << ">>>> parX from func 2nd, X = " << ival;
          fParamInUse[ival-1]++;
        }

        // filter for mapX
        sstr = iter->fLine;
        while (sstr.Index("map") != -1) {
          memset(sval, 0, sizeof(sval));
          sstr = &sstr[sstr.Index("map")+3]; // trunc sstr
          for (int j=0; j<sstr.Sizeof(); j++) {
            if (!isdigit(sstr[j]))
              break;
            sval[j] = sstr[j];
          }
          sscanf(sval, "%d", &ival);
//cout << endl << ">>>> mapX from func 2nd, X = " << ival;
          // check if map value already in map, otherwise add it
          if (ival > 0) {
            unsigned int pos;
            for (pos=0; pos<map.size(); pos++) {
              if (ival == map[pos])
                break;
            }
            if ((unsigned int)ival == map.size()) { // new map value
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

// cout << endl << ">> fParamInUse: ";
// for (unsigned int i=0; i<fParamInUse.size(); i++)
//   cout << endl << i+1 << ", " << fParamInUse[i];
// cout << endl;

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
bool PMsrHandler::CheckUniquenessOfParamNames(unsigned int &parX, unsigned int &parY)
{
  bool unique = true;

  for (unsigned int i=0; i<fParam.size()-1; i++) {
    for (unsigned int j=i+1; j<fParam.size(); j++) {
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
// CheckAndWriteComment
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param f
 * \param lineNo
 */
void PMsrHandler::CheckAndWriteComment(ofstream &f, int &lineNo)
{
  unsigned int i;

  // check if lineNo is present
  for (i=0; i<fComments.size(); i++) {
    if (fComments[i].fLineNo == lineNo) {
      break;
    }
  }

  if (i<fComments.size()) {
    f << endl << fComments[i].fLine.Data();
    CheckAndWriteComment(f, ++lineNo);
  }
}

// end ---------------------------------------------------------------------
