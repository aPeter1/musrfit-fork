/***************************************************************************

  PVarHandler.h

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

#ifndef _PVARHANDLER_H_
#define _PVARHANDLER_H_

#include <string>
#include <vector>

#include <QString>

#include "Pmupp.h"
#include "PAst.hpp"
#include "PProgram.hpp"

class PVarHandler {
  public:
    PVarHandler();
    PVarHandler(PmuppCollection *coll, std::string parse_str, std::string var_name="");

    bool isValid() { return fIsValid; }
    QString getCollName() { return fColl->GetName(); }
    QString getVarName() { return QString(fVarName.c_str()); }
    std::vector<double> getValues();
    std::vector<double> getErrors();

  private:
    PmuppCollection *fColl; ///< collection needed for parsing and evaluation
    std::string fParseStr; ///< the variable input to be parsed
    std::string fVarName;  ///< variable name
    mupp::prog::PVarHandler fVar; ///< values of the evaluation

    bool fIsValid;
    mupp::ast::statement_list fAst; ///< the AST

    void injectPredefVariables();

    std::string getVarName(int idx);
    std::vector<double> getData(int idx);
    std::vector<double> getDataErr(int idx);
};

#endif //_PVARHANDLER_H_
