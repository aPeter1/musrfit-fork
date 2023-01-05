/***************************************************************************

  PFunctionHandler.h

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

#ifndef _PFUNCTIONHANDLER_H_
#define _PFUNCTIONHANDLER_H_

#include <iostream>
#include <vector>

#include <TString.h>

#include "PMusr.h"
#include "PFunctionGrammar.h"
#include "PFunction.h"

/**
 * <p>Handler of <em>all</em> functions in a msr-file FUNCTIONS block.
 */
class PFunctionHandler
{
  public:
    PFunctionHandler(PMsrLines lines);
    virtual ~PFunctionHandler();

    virtual Bool_t IsValid() { return fValid; }
    virtual Bool_t DoParse();
    virtual Bool_t CheckMapAndParamRange(UInt_t mapSize, UInt_t paramSize);
    virtual double Eval(Int_t funNo, std::vector<Int_t> map, std::vector<double> param, PMetaData metaData);
    virtual Int_t GetFuncNo(UInt_t idx);
    virtual Int_t GetFuncIndex(Int_t funcNo);
    virtual UInt_t GetNoOfFuncs() { return fFuncs.size(); }
    virtual TString GetFuncString(UInt_t idx);

  private:
    Bool_t fValid; ///< true = function handler has valid functions

    PMsrLines fLines; ///< stores the msr-file FUNCTIONS block as clear text.
    std::vector<PFunction> fFuncs; ///< vector of all evaluatable functions
    std::vector<TString> fFuncComment; ///< vector of prepended function comments
};

#endif // _PFUNCTIONHANDLER_H_

