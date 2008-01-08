/***************************************************************************

  PFunctionHandler.h

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

#ifndef _PFUNCTIONHANDLER_H_
#define _PFUNCTIONHANDLER_H_

#include <iostream>
#include <vector>
using namespace std;

#include <TString.h>

#include "PMusr.h"
#include "PFunctionGrammar.h"
#include "PFunction.h"

class PFunctionHandler
{
  public:
    PFunctionHandler(PMsrLines lines);
    virtual ~PFunctionHandler();

    virtual bool IsValid() { return fValid; }
    virtual bool DoParse();
    virtual bool CheckMapAndParamRange(unsigned int mapSize, unsigned int paramSize);
    virtual double Eval(int funNo, vector<int> map, vector<double> param);
    virtual int GetFuncNo(unsigned int idx);
    virtual int GetFuncIndex(int funcNo);
    virtual unsigned int GetNoOfFuncs() { return fFuncs.size(); }
    virtual TString* GetFuncString(unsigned int idx);

  private:
    bool fValid;

    PMsrLines fLines;
    vector<PFunction> fFuncs;
};

#endif // _PFUNCTIONHANDLER_H_

