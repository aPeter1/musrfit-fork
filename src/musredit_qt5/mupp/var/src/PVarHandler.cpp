/***************************************************************************

  PVarHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2020 by Andreas Suter                              *
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

/*
#include "PSkipper.hpp"
#include "PErrorHandler.hpp"
#include "PStatement.hpp"
#include "PAstDump.hpp"
#include "PProgram.hpp"
*/

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::PVarHandler
 */
PVarHandler::PVarHandler(PmuppCollection *coll, std::string parse_str) :
  fColl(coll), fParseStr(parse_str), fIsValid(false)
{
  injectPredefVariables();
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::getValues
 * @return
 */
std::vector<double> PVarHandler::getValues()
{
  std::vector<double> result;

  return result;
}

//--------------------------------------------------------------------------
/**
 * @brief PVarHandler::getErrors
 * @return
 */
std::vector<double> PVarHandler::getErrors()
{
  std::vector<double> result;

  return result;
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
  for (int i=0; i<fColl->GetNoOfRuns(); i++) {
    varName = fColl->GetRun(i).GetName().toLatin1().data();
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
