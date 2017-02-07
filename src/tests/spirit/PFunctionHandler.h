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
#include <string>
using namespace std;

#include "PFunctionGrammar.h"
#include "PFunction.h"

class PFunctionHandler
{
  public:
    PFunctionHandler(char *fln, bool debug);
    PFunctionHandler(vector<string> lines);
    virtual ~PFunctionHandler();

    virtual bool IsValid() { return fValid; }
    virtual bool DoParse();
    virtual double Eval(int i);
    virtual unsigned int GetFuncNo(unsigned int i);
    virtual unsigned int GetNoOfFuncs() { return fFuncs.size(); }

  private:
    bool fDebug;
    bool fValid;

    string fFileName;

    vector<double> fParam;
    vector<int> fMap;
    vector<string> fLines;

    vector<PFunction> fFuncs;

    virtual bool ReadFile();
    virtual bool MapsAreValid();
    virtual int  GetFuncIndex(int funcNo);
};

// cint dictionary stuff --------------------------------------
#ifdef __CLING__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class PFunctionHandler+;

#endif // end __CLING__
//-------------------------------------------------------------

#endif // _PFUNCTIONHANDLER_H_

