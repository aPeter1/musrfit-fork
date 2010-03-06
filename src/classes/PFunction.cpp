/***************************************************************************

  PFunction.cpp

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

#include<cmath>

#include <iostream>
using namespace std;

#include <boost/algorithm/string/trim.hpp>  // for stripping leading whitespace in std::string

#include "PFunction.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * info is an abstract syntax tree (AST) generate by the spirit parse library
 * (see http://spirit.sourceforge.net/distrib/spirit_1_8_5/libs/spirit/doc/trees.html).
 * It contains a single parsed msr-function in an ascii representation.
 * Here it takes the from
 *    assignment (root node)
 *     |_ 'FUNx'
 *     |_ '='
 *     |_ expression
 *         |_ ...
 *
 * Since it would be inefficient to evaluate this AST directly it is transferred to
 * a more efficient tree fFuncs here in the constructor.
 *
 * \param info AST parse tree holding a single parsed msr-function in an ascii representation
 */
PFunction::PFunction(tree_parse_info<> info)
{
//  cout << endl << "in PFunction ...";

  fInfo  = info;

  // init class variables
  fValid  = true;
  fFuncNo = -1;

  // set the function number
  SetFuncNo();

  // generate function evaluation tree
  if (!GenerateFuncEvalTree()) {
    fValid = false;
  }

  EvalTreeForString(info);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 */
PFunction::~PFunction()
{
//  cout << endl << "in ~PFunction ...";
  fParam.clear();
  fMap.clear();

  CleanupFuncEvalTree();
}

//--------------------------------------------------------------------------
// InitNode (protected)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param node
 */
void PFunction::InitNode(PFuncTreeNode &node)
{
  node.fID = 0;
  node.fOperatorTag = 0;
  node.fFunctionTag = 0;
  node.fIvalue = 0;
  node.fSign = false;
  node.fDvalue = 0.0;
}

//-------------------------------------------------------------
// SetFuncNo (protected)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param i
 */
Bool_t PFunction::SetFuncNo()
{
  Int_t funNo = -1;
  Int_t status;
  Bool_t success = true;

  // get root
  iter_t i = fInfo.trees.begin(); // assignement
  i = i->children.begin(); // FUNx

  // get string from tree
  string str(i->value.begin(), i->value.end());
  boost::algorithm::trim(str);

  // extract function number from string
  status = sscanf(str.c_str(), "FUN%d", &funNo);
//cout << endl << "SetFuncNo: status = " << status << ", funNo = " << funNo;
  if (status == 1) { // found 1 Int_t
    fFuncNo = funNo;
  } else { // wrong string
    success = false;
  }

  return success;
}

//-------------------------------------------------------------
// GenerateFuncEvalTree (protected)
//-------------------------------------------------------------
/**
 * <p>
 *
 */
Bool_t PFunction::GenerateFuncEvalTree()
{
  InitNode(fFunc);
  FillFuncEvalTree(fInfo.trees.begin(), fFunc);

  return true;
}

//-------------------------------------------------------------
// FillFuncEvalTree (protected)
//-------------------------------------------------------------
/**
 * <p>
 *
 */
void PFunction::FillFuncEvalTree(iter_t const& i, PFuncTreeNode &node)
{
  Double_t dvalue;
  Int_t    ivalue;
  Int_t    status;
  string str;
  PFuncTreeNode child;

  InitNode(child);

  if (i->value.id() == PFunctionGrammar::realID) { // handle number
    str = string(i->value.begin(), i->value.end()); // get string
    boost::algorithm::trim(str);
    status = sscanf(str.c_str(), "%lf", &dvalue); // convert string to Double_t
    node.fID = PFunctionGrammar::realID; // keep the ID
    node.fDvalue = dvalue; // keep the value
// cout << endl << ">> realID: value = " << dvalue;
  } else if (i->value.id() == PFunctionGrammar::constPiID) { // handle constant pi
    node.fID = PFunctionGrammar::constPiID; // keep the ID
    node.fDvalue = 3.14159265358979323846; // keep the value
  } else if (i->value.id() == PFunctionGrammar::constGammaMuID) { // handle constant gamma_mu
    node.fID = PFunctionGrammar::constGammaMuID; // keep the ID
    node.fDvalue = 0.0135538817; // keep the value
  } else if (i->value.id() == PFunctionGrammar::parameterID) { // handle parameter number
    str = string(i->value.begin(), i->value.end()); // get string
    boost::algorithm::trim(str);
    if (strstr(str.c_str(), "-")) {
      node.fSign = true;
      status = sscanf(str.c_str(), "-PAR%d", &ivalue); // convert string to parameter number
    } else {
      status = sscanf(str.c_str(), "PAR%d", &ivalue); // convert string to parameter number
    }
    node.fID = PFunctionGrammar::parameterID; // keep the ID
    node.fIvalue = ivalue; // keep the value
// cout << endl << ">> parameterID: value = " << ivalue;
  } else if (i->value.id() == PFunctionGrammar::mapID) { // handle map number
    str = string(i->value.begin(), i->value.end()); // get string
    boost::algorithm::trim(str);
    status = sscanf(str.c_str(), "MAP%d", &ivalue); // convert string to map number
    node.fID = PFunctionGrammar::mapID; // keep the ID
    node.fIvalue = ivalue; // keep the value
// cout << endl << ">> mapID: value = " << ivalue;
  } else if (i->value.id() == PFunctionGrammar::functionID) { // handle function like cos ...
    // keep the id
    node.fID = PFunctionGrammar::functionID;
    // keep function tag
    str =  string(i->value.begin(), i->value.end()); // get string
// cout << endl << ">> functionID: value = " << str;
    if (!strcmp(str.c_str(), "COS"))
      node.fFunctionTag = FUN_COS;
    else if (!strcmp(str.c_str(), "SIN"))
      node.fFunctionTag = FUN_SIN;
    else if (!strcmp(str.c_str(), "TAN"))
      node.fFunctionTag = FUN_TAN;
    else if (!strcmp(str.c_str(), "COSH"))
      node.fFunctionTag = FUN_COSH;
    else if (!strcmp(str.c_str(), "SINH"))
      node.fFunctionTag = FUN_SINH;
    else if (!strcmp(str.c_str(), "TANH"))
      node.fFunctionTag = FUN_TANH;
    else if (!strcmp(str.c_str(), "ACOS"))
      node.fFunctionTag = FUN_ACOS;
    else if (!strcmp(str.c_str(), "ASIN"))
      node.fFunctionTag = FUN_ASIN;
    else if (!strcmp(str.c_str(), "ATAN"))
      node.fFunctionTag = FUN_ATAN;
    else if (!strcmp(str.c_str(), "ACOSH"))
      node.fFunctionTag = FUN_ACOSH;
    else if (!strcmp(str.c_str(), "ASINH"))
      node.fFunctionTag = FUN_ASINH;
    else if (!strcmp(str.c_str(), "ATANH"))
      node.fFunctionTag = FUN_ATANH;
    else if (!strcmp(str.c_str(), "LOG"))
      node.fFunctionTag = FUN_LOG;
    else if (!strcmp(str.c_str(), "LN"))
      node.fFunctionTag = FUN_LN;
    else if (!strcmp(str.c_str(), "EXP"))
      node.fFunctionTag = FUN_EXP;
    else if (!strcmp(str.c_str(), "SQRT"))
      node.fFunctionTag = FUN_SQRT;
    else {
      cerr << endl << "**PANIC ERROR**: function " << str << " doesn't exist, but you never should have reached this point!";
      cerr << endl;
      assert(0);
    }
    // add node
    node.children.push_back(child);
    // i: '(', 'expression', ')'
    FillFuncEvalTree(i->children.begin()+1, node.children[0]);
  } else if (i->value.id() == PFunctionGrammar::factorID) {
// cout << endl << ">> factorID";
    // keep the id
    node.fID = PFunctionGrammar::factorID;
    // add child lhs
    node.children.push_back(child);
    FillFuncEvalTree(i->children.begin(), node.children[0]);
  } else if (i->value.id() == PFunctionGrammar::termID) {
    // keep the id
    node.fID = PFunctionGrammar::termID;
    // keep operator tag
    if (*i->value.begin() == '*')
      node.fOperatorTag = OP_MUL;
    else
      node.fOperatorTag = OP_DIV;
/*
if (node.fOperatorTag == OP_MUL)
 cout << endl << ">> termID: value = *";
else
 cout << endl << ">> termID: value = /";
*/
    // add child lhs
    node.children.push_back(child);
    FillFuncEvalTree(i->children.begin(), node.children[0]);
    // add child rhs
    node.children.push_back(child);
    FillFuncEvalTree(i->children.begin()+1, node.children[1]);
  } else if (i->value.id() == PFunctionGrammar::expressionID) { // handle expression
    // keep the id
    node.fID = PFunctionGrammar::expressionID;
    // keep operator tag
    if (*i->value.begin() == '+')
      node.fOperatorTag = OP_ADD;
    else
      node.fOperatorTag = OP_SUB;
/*
if (node.fOperatorTag == OP_ADD)
 cout << endl << ">> expressionID: value = +";
else
 cout << endl << ">> expressionID: value = -";
*/
    // add child lhs
    node.children.push_back(child);
    FillFuncEvalTree(i->children.begin(), node.children[0]);
    // add child rhs
    node.children.push_back(child);
    FillFuncEvalTree(i->children.begin()+1, node.children[1]);
  } else if (i->value.id() == PFunctionGrammar::assignmentID) {
    // nothing to be done except to pass the next element in the ast
    // i: 'funx', '=', 'expression'
    FillFuncEvalTree(i->children.begin()+2, node);
  }
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
Bool_t PFunction::CheckMapAndParamRange(UInt_t mapSize, UInt_t paramSize)
{
  return FindAndCheckMapAndParamRange(fFunc, mapSize, paramSize);
}

//-------------------------------------------------------------
// FindAndCheckMapAndParamRange (protected)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param i
 * \param mapSize
 * \param paramSize
 */
Bool_t PFunction::FindAndCheckMapAndParamRange(PFuncTreeNode &node, UInt_t mapSize, UInt_t paramSize)
{
  if (node.fID == PFunctionGrammar::realID) {
    return true;
  } else if (node.fID == PFunctionGrammar::constPiID) {
    return true;
  } else if (node.fID == PFunctionGrammar::constGammaMuID) {
    return true;
  } else if (node.fID == PFunctionGrammar::parameterID) {
    if (node.fIvalue <= (Int_t) paramSize)
      return true;
    else
      return false;
  } else if (node.fID == PFunctionGrammar::mapID) {
    if (node.fIvalue <= (Int_t) mapSize)
      return true;
    else
      return false;
  } else if (node.fID == PFunctionGrammar::functionID) {
    return FindAndCheckMapAndParamRange(node.children[0], mapSize, paramSize);
  } else if (node.fID == PFunctionGrammar::factorID) {
    return FindAndCheckMapAndParamRange(node.children[0], mapSize, paramSize);
  } else if (node.fID == PFunctionGrammar::termID) {
    if (FindAndCheckMapAndParamRange(node.children[0], mapSize, paramSize))
      return FindAndCheckMapAndParamRange(node.children[1], mapSize, paramSize);
    else
      return false;
  } else if (node.fID == PFunctionGrammar::expressionID) {
    if (FindAndCheckMapAndParamRange(node.children[0], mapSize, paramSize))
      return FindAndCheckMapAndParamRange(node.children[1], mapSize, paramSize);
    else
      return false;
  } else {
    cerr << endl << "**PANIC ERROR**: PFunction::FindAndCheckMapAndParamRange: you never should have reached this point!";
    cerr << endl;
    assert(0);
  }
  return true;
}

//-------------------------------------------------------------
// Eval (public)
//-------------------------------------------------------------
/**
 * <p>
 *
 */
Double_t PFunction::Eval(vector<Double_t> param)
{
  fParam = param;

  return EvalNode(fFunc);
}

//-------------------------------------------------------------
// EvalNode (protected)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param node
 */
Double_t PFunction::EvalNode(PFuncTreeNode &node)
{
  if (node.fID == PFunctionGrammar::realID) {
    return node.fDvalue;
  } else if (node.fID == PFunctionGrammar::constPiID) {
    return node.fDvalue;
  } else if (node.fID == PFunctionGrammar::constGammaMuID) {
    return node.fDvalue;
  } else if (node.fID == PFunctionGrammar::parameterID) {
    Double_t dval;
    if (node.fSign)
      dval = -fParam[node.fIvalue-1];
    else
      dval = fParam[node.fIvalue-1];
    return dval;
  } else if (node.fID == PFunctionGrammar::mapID) {
    if (fMap[node.fIvalue-1] == 0) // map == 0
      return 0.0;
    else
      return fParam[fMap[node.fIvalue-1]-1];
  } else if (node.fID == PFunctionGrammar::functionID) {
    if (node.fFunctionTag == FUN_COS) {
      return cos(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_SIN) {
      return sin(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_TAN) {
      return tan(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_COSH) {
      return cosh(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_SINH) {
      return sinh(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_TANH) {
      return tanh(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_ACOS) {
      return acos(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_ASIN) {
      return asin(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_ATAN) {
      return atan(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_ACOSH) {
      return acosh(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_ASINH) {
      return asinh(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_ATANH) {
      return atanh(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_LOG) {
      return log(EvalNode(node.children[0]))/log(10.0);
    } else if (node.fFunctionTag == FUN_LN) {
      return log(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_EXP) {
      return exp(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_SQRT) {
      return sqrt(EvalNode(node.children[0]));
    } else {
      cerr << endl << "**PANIC ERROR**: PFunction::EvalNode: node.fID == PFunctionGrammar::functionID: you never should have reached this point!";
      cerr << endl;
      assert(0);
    }
  } else if (node.fID == PFunctionGrammar::factorID) {
    return EvalNode(node.children[0]);
  } else if (node.fID == PFunctionGrammar::termID) {
    if (node.fOperatorTag == OP_MUL) {
      return EvalNode(node.children[0]) * EvalNode(node.children[1]);
    } else {
      Double_t denominator = EvalNode(node.children[1]);
      if (denominator == 0.0) {
        cerr << endl << "**PANIC ERROR**: PFunction::EvalNode: division by 0.0";
        cerr << endl;
        assert(0);
      }
      return EvalNode(node.children[0]) / denominator;
    }
  } else if (node.fID == PFunctionGrammar::expressionID) {
    if (node.fOperatorTag == OP_ADD) {
      return EvalNode(node.children[0]) + EvalNode(node.children[1]);
    } else {
      return EvalNode(node.children[0]) - EvalNode(node.children[1]);
    }
  } else {
    cerr << endl << "**PANIC ERROR**: PFunction::EvalNode: you never should have reached this point!";
    cerr << endl;
    assert(0);
  }
  return 0.0;
}

//-------------------------------------------------------------
// CleanupFuncEvalTree (protected)
//-------------------------------------------------------------
/**
 * <p>
 *
 */
void PFunction::CleanupFuncEvalTree()
{
  // clean up all children
  CleanupNode(fFunc);
}

//-------------------------------------------------------------
// CleanupNode (protected)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param node
 */
void PFunction::CleanupNode(PFuncTreeNode &node)
{
  if (node.children.size() != 0) {
    for (UInt_t i=0; i<node.children.size(); i++) {
      CleanupNode(node.children[i]);
    }
    node.children.clear();
  }
}

//-------------------------------------------------------------
// EvalTreeForString (private)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param info
 */
void PFunction::EvalTreeForString(tree_parse_info<> info)
{
  fFuncString = "";
  EvalTreeForStringExpression(info.trees.begin());
}

//-------------------------------------------------------------
// EvalTreeForStringExpression (private)
//-------------------------------------------------------------
/**
 * <p>
 *
 * \param i
 */
void PFunction::EvalTreeForStringExpression(iter_t const& i)
{
  static Int_t termOp = 0;

  if (i->value.id() == PFunctionGrammar::realID) {
    assert(i->children.size() == 0);
    if (*i->value.begin() == '-')
      fFuncString += "(";
    fFuncString += string(i->value.begin(), i->value.end()).c_str();
    if (*i->value.begin() == '-')
      fFuncString += ")";
  } else if (i->value.id() == PFunctionGrammar::constPiID) {
    fFuncString += "Pi";
  } else if (i->value.id() == PFunctionGrammar::constGammaMuID) {
    fFuncString += "gamma_mu";
  } else if (i->value.id() == PFunctionGrammar::funLabelID) {
    assert(i->children.size() == 0);
    //SetFuncNo(i);
    fFuncString += string(i->value.begin(), i->value.end()).c_str(); // funx
  } else if (i->value.id() == PFunctionGrammar::parameterID) {
    assert(i->children.size() == 0);
    fFuncString += string(i->value.begin(), i->value.end()).c_str();
  } else if (i->value.id() == PFunctionGrammar::mapID) {
    assert(i->children.size() == 0);
    fFuncString += string(i->value.begin(), i->value.end()).c_str();
  } else if (i->value.id() == PFunctionGrammar::functionID) {
    assert(i->children.size() == 3);
    fFuncString += string(i->value.begin(), i->value.end()).c_str(); // keep function name
    fFuncString += "(";
    // '(', expression, ')'
    EvalTreeForStringExpression(i->children.begin()+1); // the real stuff
    fFuncString += ")";
  } else if (i->value.id() == PFunctionGrammar::factorID) {
    EvalTreeForStringExpression(i->children.begin());
  } else if (i->value.id() == PFunctionGrammar::termID) {
    termOp++;
    if (*i->value.begin() == '*') {
//cout << endl << ">> i->children.size() = " << i->children.size() << endl;
      assert(i->children.size() == 2);
      EvalTreeForStringExpression(i->children.begin());
      fFuncString += " * ";
      EvalTreeForStringExpression(i->children.begin()+1);
    } else if (*i->value.begin() == '/') {
      assert(i->children.size() == 2);
      EvalTreeForStringExpression(i->children.begin());
      fFuncString += " / ";
      EvalTreeForStringExpression(i->children.begin()+1);
    } else {
      assert(0);
    }
    termOp--;
  } else if (i->value.id() == PFunctionGrammar::expressionID) {
    if (termOp > 0)
      fFuncString += "(";
    if (*i->value.begin() == '+') {
      assert(i->children.size() == 2);
      EvalTreeForStringExpression(i->children.begin());
      fFuncString += " + ";
      EvalTreeForStringExpression(i->children.begin()+1);
    } else if (*i->value.begin() == '-') {
      assert(i->children.size() == 2);
      EvalTreeForStringExpression(i->children.begin());
      fFuncString += " - ";
      EvalTreeForStringExpression(i->children.begin()+1);
    } else {
      assert(0);
    }
    if (termOp > 0)
      fFuncString += ")";
  } else if (i->value.id() == PFunctionGrammar::assignmentID) {
    assert(i->children.size() == 3);
    EvalTreeForStringExpression(i->children.begin());
    EvalTreeForStringExpression(i->children.begin()+1); // this is the string "="
    EvalTreeForStringExpression(i->children.begin()+2); // this is the real stuff
  } else if (*i->value.begin() == '=') {
    fFuncString += " = ";
  } else {
    assert(0);
  }
}
