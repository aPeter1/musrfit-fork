/***************************************************************************

  PFunctionHandler.cpp

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

#include <string>
#include <cassert>

#include "PFunctionHandler.h"

//-------------------------------------------------------------
// Constructor
//-------------------------------------------------------------
/**
 * <p>Constructor
 *
 * \param lines msr-file FUNCTIONS block in clear text.
 */
PFunctionHandler::PFunctionHandler(PMsrLines lines) : fLines(lines)
{
  fValid = true;
}

//-------------------------------------------------------------
// Destructor
//-------------------------------------------------------------
/**
 * <p>Destructor
 */
PFunctionHandler::~PFunctionHandler()
{
  fLines.clear();
  fFuncs.clear();
  fFuncComment.clear();
}

//-------------------------------------------------------------
// DoParse (public)
//-------------------------------------------------------------
/**
 * <p>Calls the function parser.
 */
Bool_t PFunctionHandler::DoParse()
{
  Bool_t success = true;
  PFunctionGrammar function;
  TString line;

  // feed the function block into the parser. Start with i=1, since i=0 is FUNCTIONS
  for (UInt_t i=1; i<fLines.size(); i++) {

    // function line to upper case after cutting out prepended comment
    line = fLines[i].fLine;
    Ssiz_t pos = line.First('#'); // find prepended comment
    TString Comment("");
    if (pos != kNPOS) { // comment present
      for (Int_t i=pos; i<line.Length(); i++) {
        Comment += line[i];
      }
    }
    fFuncComment.push_back(Comment);
    if (pos != kNPOS) { // comment present, hence remove it from the string to be parsed
      line.Remove(pos);
      line.Remove(TString::kTrailing, ' ');
    }
    line.ToUpper();

    // do parsing
    tree_parse_info<> info = ast_parse(line.Data(), function, space_p);

    if (info.full) { // parsing successful
      PFunction func(info); // generate an evaluation function object based on the AST tree
      fFuncs.push_back(func); // feeds it to the functions vector
    } else {
      std::cerr << std::endl << "**ERROR**: FUNCTIONS parse failed in line " << fLines[i].fLineNo << std::endl;
      success = false;
      break;
    }
  }

  // check that the function numbers are unique
  if (success) {
    for (UInt_t i=0; i<fFuncs.size(); i++) {
      for (UInt_t j=i+1; j<fFuncs.size(); j++) {
        if (fFuncs[i].GetFuncNo() == fFuncs[j].GetFuncNo()) {
          std::cerr << std::endl << "**ERROR**: function number " << fFuncs[i].GetFuncNo();
          std::cerr << " is at least twice present! Fix this first.";
          std::cerr << std::endl;
          success = false;
        }
      }
    }
  }

  return success;
}

//-------------------------------------------------------------
// CheckMapAndParamRange (public)
//-------------------------------------------------------------
/**
 * <p>Check all functions if the map and fit parameters are within valid ranges, i.e. map < mapSize, param < paramSize.
 *
 * <b>return:</b> true if map and fit parameters are withing valid ranges, otherwise false.
 *
 * \param mapSize size of the map vector
 * \param paramSize size of the fit parameter vector
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
 * <p>Evaluate function number funNo for given map and param.
 *
 * <b>return:</b> value of the function for given map and param.
 *
 * \param funNo function number
 * \param map map vector
 * \param param fit parameter vector
 */
Double_t PFunctionHandler::Eval(Int_t funNo, std::vector<Int_t> map, std::vector<double> param, PMetaData metaData)
{
  if (GetFuncIndex(funNo) == -1) {
    std::cerr << std::endl << "**ERROR**: Couldn't find FUN" << funNo << " for evaluation";
    std::cerr << std::endl;
    return 0.0;
  }

  // set correct map
  fFuncs[GetFuncIndex(funNo)].SetMap(map);

  // return evaluated function
  return fFuncs[GetFuncIndex(funNo)].Eval(param, metaData);
}

//-------------------------------------------------------------
// GetFuncNo (public)
//-------------------------------------------------------------
/**
 * <p>returns the function number
 *
 * \param idx index of the function
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
 * <p>return function index for a given function number
 *
 * \param funcNo function number
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
 * <p>return the (clean and tidy) function string at index idx
 *
 * \param idx index of the function
 */
TString PFunctionHandler::GetFuncString(UInt_t idx)
{
  TString funStr("");

  if ((idx > fFuncs.size()) || (idx > fFuncComment.size()))
    return funStr;

  if (fFuncComment[idx].Length() > 0)
    funStr = *fFuncs[idx].GetFuncString() + " " + fFuncComment[idx];
  else
    funStr = *fFuncs[idx].GetFuncString();

  return funStr;
}
