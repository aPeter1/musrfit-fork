/***************************************************************************

  PVarHandler.h

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

#ifndef _PVARHANDLER_H_
#define _PVARHANDLER_H_

#include <string>
#include <vector>

#include <QString>

#include <PAst.hpp>

class PVarHandler {
  public:
    PVarHandler();

    void setInput(QString &str) { fInput = str.toLatin1().data(); }
    bool parse();
    bool semcheck();
    std::vector<double> getValues();
    std::vector<double> getErrors();

  private:
    std::string fInput; ///< the variable input to be parsed
    mupp::ast::statement_list fAst; ///< the AST
};

#endif //_PVARHANDLER_H_
