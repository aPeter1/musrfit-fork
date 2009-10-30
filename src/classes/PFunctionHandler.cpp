/***************************************************************************

  PFunctionHandler.cpp

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

#include <string>
#include <cassert>

#include "PFunctionHandler.h"

//-------------------------------------------------------------
// Constructor
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param lines
 */
PFunctionHandler::PFunctionHandler(PMsrLines lines)
{
  fValid = true;
  fLines = lines;

//  cout << endl << "in PFunctionHandler(PMsrLines lines)";
}

//-------------------------------------------------------------
// Destructor
//-------------------------------------------------------------
/**
 * <p>
 *
 */
PFunctionHandler::~PFunctionHandler()
{
//  cout << endl << "in ~PFunctionHandler()" << endl << endl;

  fLines.clear();
  fFuncs.clear();
}

//-------------------------------------------------------------
// DoParse (public)
//-------------------------------------------------------------
/**
 * <p>
 *
 */
Bool_t PFunctionHandler::DoParse()
{
//  cout << endl << "in PFunctionHandler::DoParse() ...";

  Bool_t success = true;
  PFunctionGrammar function;
  TString line;

  // feed the function block into the parser. Start with i=1, since i=0 is FUNCTIONS
  for (UInt_t i=1; i<fLines.size(); i++) {
// cout << endl << "fLines[" << i << "] = '" << fLines[i].fLine.Data() << "'";

    // function line to upper case
    line = fLines[i].fLine;
    line.ToUpper();

    // do parsing
    tree_parse_info<> info = ast_parse(line.Data(), function, space_p);

    if (info.full) {
//      cout << endl << "parse successful ..." << endl;
      PFunction func(info);
      fFuncs.push_back(func);
    } else {
      cerr << endl << "**ERROR**: FUNCTIONS parse failed in line " << fLines[i].fLineNo << endl;
      success = false;
      break;
    }
  }

  // check that the function numbers are unique
  if (success) {
    for (UInt_t i=0; i<fFuncs.size(); i++) {
      for (UInt_t j=i+1; j<fFuncs.size(); j++) {
        if (fFuncs[i].GetFuncNo() == fFuncs[j].GetFuncNo()) {
          cerr << endl << "**ERROR**: function number " << fFuncs[i].GetFuncNo();
          cerr << " is at least twice present! Fix this first.";
          cerr << endl;
          success = false;
        }
      }
    }
  }

//   if (success) {
//     for (UInt_t i=0; i<fFuncs.size(); i++)
//       cout << endl << "func number = " << fFuncs[i].GetFuncNo();
//   }

  return success;
}

//-------------------------------------------------------------
// CheckMapAndParamRange (public)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param mapSize
 * \param paramSize
 */
Bool_t PFunctionHandler::CheckMapAndParamRange(UInt_t mapSize, UInt_t paramSize)
{
  Bool_t success = true;

  for (UInt_t i=0; i<fFuncs.size(); i++) {
    success = fFuncs[i].CheckMapAndParamRange(mapSize, paramSize);
    if (!success)
      break;
  }

  return success;
}

//-------------------------------------------------------------
// Eval (public)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param funNo
 */
Double_t PFunctionHandler::Eval(Int_t funNo, vector<Int_t> map, vector<Double_t> param)
{
  if (GetFuncIndex(funNo) == -1) {
    cerr << endl << "**ERROR**: Couldn't find FUN" << funNo << " for evaluation";
    cerr << endl;
    return 0.0;
  }

//cout << endl << "PFunctionHandler::Eval: GetFuncIndex("<<funNo<<") = " << GetFuncIndex(funNo);
//cout << endl;

  // set correct map
  fFuncs[GetFuncIndex(funNo)].SetMap(map);

  // return evaluated function
  return fFuncs[GetFuncIndex(funNo)].Eval(param);
}

//-------------------------------------------------------------
// GetFuncNo (public)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param idx
 */
Int_t PFunctionHandler::GetFuncNo(UInt_t idx)
{
  if (idx > fFuncs.size())
    return -1;

  return fFuncs[idx].GetFuncNo();
}

//-------------------------------------------------------------
// GetFuncIndex (public)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param funcNo
 */
Int_t PFunctionHandler::GetFuncIndex(Int_t funcNo)
{
  Int_t index = -1;

  for (UInt_t i=0; i<fFuncs.size(); i++) {
    if (fFuncs[i].GetFuncNo() == funcNo) {
      index = i;
      break;
    }
  }

  return index;
}

//-------------------------------------------------------------
// GetFuncString (public)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param idx
 */
TString* PFunctionHandler::GetFuncString(UInt_t idx)
{
  if (idx > fFuncs.size())
    return 0;

  return fFuncs[idx].GetFuncString();
}
