/***************************************************************************

  PVarHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2023 by Andreas Suter                              *
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

#include <iostream>

#include "PVarHandler.h"

#include "PSkipper.hpp"
#include "PErrorHandler.hpp"
#include "PStatement.hpp"
#include "PProgram.hpp"

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::PVarHandler
 */
PVarHandler::PVarHandler() :
  fColl(nullptr), fParseStr(""), fVarName(""), fIsValid(false)
{
  // nothing to be done here
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::PVarHandler
 */
PVarHandler::PVarHandler(PmuppCollection *coll, std::string parse_str, std::string var_name) :
  fColl(coll), fParseStr(parse_str), fVarName(var_name), fIsValid(false)
{
  injectPredefVariables();

  typedef std::string::const_iterator iterator_type;
  iterator_type iter = fParseStr.begin();
  iterator_type end = fParseStr.end();

  mupp::PErrorHandler<iterator_type> error_handler(iter, end);   // the error handler
  mupp::parser::PStatement<iterator_type> parser(error_handler); // the parser
  mupp::prog::PProgram prog(error_handler); // our compiler, and exec
  mupp::parser::PSkipper<iterator_type> skipper; // the skipper parser

  // perform the parsing
  bool success = phrase_parse(iter, end, parser, skipper, fAst);
  if (success && iter == end) {
    if (prog(fAst)) { // semantic analysis
      std::vector<double> data, dataErr;
      for (unsigned int i=0; i<fColl->GetRun(0).GetNoOfParam(); i++) {
        data = getData(i);
        dataErr = getDataErr(i);
        prog.add_predef_var_values(getVarName(i), data, dataErr);
      }
      if (!fVarName.empty()) {
        mupp::prog::PProgEval eval(prog.getVars()); // setup evaluation stage
        eval(fAst); // evaluate stuff

        // keep data
        bool ok;
        fVar = eval.getVar(fVarName, ok);
        if (!ok) {
          std::cerr << "**ERROR** evalution failed..." << std::endl;
          fIsValid = false;
        }
      }
    }
    fIsValid = true;
  } else {
    std::cerr << "**ERROR** parsing failed..." << std::endl;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::getValues
 * @return
 */
std::vector<double> PVarHandler::getValues()
{
  std::vector<double> data;
  if (fIsValid)
    data = fVar.GetValue();

  return data;
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::getErrors
 * @return
 */
std::vector<double> PVarHandler::getErrors()
{
  std::vector<double> data;
  if (fIsValid)
    data = fVar.GetError();

  return data;
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::injectPredefVariables
 */
void PVarHandler::injectPredefVariables()
{
  mupp::ast::statement var_stat;
  mupp::ast::variable_declaration var;

  std::string varName, errVarName;
  PmuppRun run = fColl->GetRun(0);
  for (int i=0; i<run.GetNoOfParam(); i++) {
    varName = run.GetParam(i).GetName().toLatin1().data();
    errVarName = varName + "Err";
    // inject err_name
    var.lhs.name = errVarName;
    var_stat = var;
    fAst.push_front(var_stat);
    // inject var_name
    var.lhs.name = varName;
    var_stat = var;
    fAst.push_front(var_stat);
  }
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::getVarName
 * @param idx
 * @return
 */
std::string PVarHandler::getVarName(int idx)
{
  std::string name("??");

  // make sure idx is in range
  if (idx >= fColl->GetRun(0).GetNoOfParam())
    return name;

  return fColl->GetRun(0).GetParam(idx).GetName().toLatin1().data();
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::getData
 * @param idx
 * @return
 */
std::vector<double> PVarHandler::getData(int idx)
{
  std::vector<double> data;

  // make sure idx is in range
  if (idx >= fColl->GetRun(0).GetNoOfParam())
    return data;

  double dval;
  for (int i=0; i<fColl->GetNoOfRuns(); i++) {
    dval = fColl->GetRun(i).GetParam(idx).GetValue();
    data.push_back(dval);
  }

  return data;
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::getDataErr
 * @param idx
 * @return
 */
std::vector<double> PVarHandler::getDataErr(int idx)
{
  std::vector<double> err;

  // make sure idx is in range
  if (idx >= fColl->GetRun(0).GetNoOfParam())
    return err;

  double dvalPos, dvalNeg;
  for (int i=0; i<fColl->GetNoOfRuns(); i++) {
    dvalPos = fColl->GetRun(i).GetParam(idx).GetPosErr();
    dvalNeg = fColl->GetRun(i).GetParam(idx).GetNegErr();
    dvalPos = sqrt(fabs(dvalPos*dvalNeg)); // geometric mean of pos/neg error
    err.push_back(dvalPos);
  }

  return err;
}
