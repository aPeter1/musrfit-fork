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
bool PFunctionHandler::DoParse()
{
//  cout << endl << "in PFunctionHandler::DoParse() ...";

  bool success = true;
  PFunctionGrammar function;
  TString line;

  // feed the function block into the parser. Start with i=1, since i=0 is FUNCTIONS
  for (unsigned int i=1; i<fLines.size(); i++) {
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
      cout << endl << "**ERROR**: FUNCTIONS parse failed in line " << fLines[i].fLineNo << endl;
      success = false;
      break;
    }
  }

  // check that the function numbers are unique
  if (success) {
    for (unsigned int i=0; i<fFuncs.size(); i++) {
      for (unsigned int j=i+1; j<fFuncs.size(); j++) {
        if (fFuncs[i].GetFuncNo() == fFuncs[j].GetFuncNo()) {
          cout << endl << "**ERROR**: function number " << fFuncs[i].GetFuncNo();
          cout << " is at least twice present! Fix this first.";
          success = false;
        }
      }
    }
  }

//   if (success) {
//     for (unsigned int i=0; i<fFuncs.size(); i++)
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
bool PFunctionHandler::CheckMapAndParamRange(unsigned int mapSize, unsigned int paramSize)
{
  bool success = true;

  for (unsigned int i=0; i<fFuncs.size(); i++) {
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
double PFunctionHandler::Eval(int funNo, vector<int> map, vector<double> param)
{
  if (GetFuncIndex(funNo) == -1) {
    cout << endl << "**ERROR**: Couldn't find FUN" << funNo << " for evaluation";
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
int PFunctionHandler::GetFuncNo(unsigned int idx)
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
int PFunctionHandler::GetFuncIndex(int funcNo)
{
  int index = -1;

  for (unsigned int i=0; i<fFuncs.size(); i++) {
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
TString* PFunctionHandler::GetFuncString(unsigned int idx)
{
  if (idx > fFuncs.size())
    return 0;

  return fFuncs[idx].GetFuncString();
}
