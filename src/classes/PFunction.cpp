/***************************************************************************

  PFunction.cpp

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

#include<cmath>

#include <iostream>

#include <boost/algorithm/string/trim.hpp>  // for stripping leading whitespace in std::string

#include "PMusr.h"
#include "PFunction.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * info is an abstract syntax tree (AST) generate by the spirit parse library
 * (see http://spirit.sourceforge.net/distrib/spirit_1_8_5/libs/spirit/doc/trees.html).
 * It contains a single parsed msr-function in an ascii representation.
 * Here it takes the from
 * \verbatim
 *    assignment (root node)
 *     |_ 'FUNx'
 *     |_ '='
 *     |_ expression
 *         |_ ...
 * \endverbatim
 *
 * <p>Since it would be inefficient to evaluate this AST directly it is transferred to
 * a more efficient tree fFuncs here in the constructor.
 *
 * \param info AST parse tree holding a single parsed msr-function in an ascii representation
 */
PFunction::PFunction(tree_parse_info<> info) : fInfo(info)
{
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
 * <p>Destructor.
 */
PFunction::~PFunction()
{
  fParam.clear();
  fMap.clear();

  CleanupFuncEvalTree();
}

//--------------------------------------------------------------------------
// InitNode (protected)
//--------------------------------------------------------------------------
/**
 * <p>Initializes the node of the evaluation function tree.
 *
 * \param node to be initialized
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
 * <p>Extracts the function number of the AST tree.
 *
 * <b>return:</b> true if the function number (of FUNx, x being a number) could be extracted, otherwise false.
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
  std::string str(i->value.begin(), i->value.end());
  boost::algorithm::trim(str);

  // extract function number from string
  status = sscanf(str.c_str(), "FUN%d", &funNo);
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
 * <p>Stub to generate the function evaluation tree from the AST tree. Needed for an efficient
 * evaluation.
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
 * <p>Recursive generation of the evaluation tree.
 *
 * \param i iterator of the AST tree
 * \param node of the evaluation tree
 */
void PFunction::FillFuncEvalTree(iter_t const& i, PFuncTreeNode &node)
{
  Double_t dvalue;
  Int_t    ivalue;
  Int_t    status;
  std::string str;
  PFuncTreeNode child;

  InitNode(child);

  if (i->value.id() == PFunctionGrammar::realID) { // handle number
    str = std::string(i->value.begin(), i->value.end()); // get string
    boost::algorithm::trim(str);
    status = sscanf(str.c_str(), "%lf", &dvalue); // convert string to Double_t
    node.fID = PFunctionGrammar::realID; // keep the ID
    node.fDvalue = dvalue; // keep the value
  } else if (i->value.id() == PFunctionGrammar::constPiID) { // handle constant pi
    node.fID = PFunctionGrammar::constPiID; // keep the ID
    node.fDvalue = 3.14159265358979323846; // keep the value
  } else if (i->value.id() == PFunctionGrammar::constGammaMuID) { // handle constant gamma_mu
    node.fID = PFunctionGrammar::constGammaMuID; // keep the ID
    node.fDvalue = GAMMA_BAR_MUON; // keep the value
  } else if (i->value.id() == PFunctionGrammar::constFieldID) { // handle constant field from meta data
    node.fID = PFunctionGrammar::constFieldID; // keep the ID
  } else if (i->value.id() == PFunctionGrammar::constEnergyID) { // handle constant energy from meta data
    node.fID = PFunctionGrammar::constEnergyID; // keep the ID
  } else if (i->value.id() == PFunctionGrammar::constTempID) { // handle constant temperature from meta data
    str = std::string(i->value.begin(), i->value.end()); // get string
    boost::algorithm::trim(str);
    status = sscanf(str.c_str(), "T%d", &ivalue); // convert string to temperature index
    node.fID = PFunctionGrammar::constTempID; // keep the ID
    node.fIvalue = ivalue; // Temp idx
  } else if (i->value.id() == PFunctionGrammar::parameterID) { // handle parameter number
    str = std::string(i->value.begin(), i->value.end()); // get string
    boost::algorithm::trim(str);
    if (strstr(str.c_str(), "-")) {
      node.fSign = true;
      status = sscanf(str.c_str(), "-PAR%d", &ivalue); // convert string to parameter number
    } else {
      status = sscanf(str.c_str(), "PAR%d", &ivalue); // convert string to parameter number
    }
    node.fID = PFunctionGrammar::parameterID; // keep the ID
    node.fIvalue = ivalue; // keep the value
  } else if (i->value.id() == PFunctionGrammar::mapID) { // handle map number
    str = std::string(i->value.begin(), i->value.end()); // get string
    boost::algorithm::trim(str);
    status = sscanf(str.c_str(), "MAP%d", &ivalue); // convert string to map number
    node.fID = PFunctionGrammar::mapID; // keep the ID
    node.fIvalue = ivalue; // keep the value
  } else if (i->value.id() == PFunctionGrammar::functionID) { // handle function like cos ...
    // keep the id
    node.fID = PFunctionGrammar::functionID;
    // keep function tag
    str =  std::string(i->value.begin(), i->value.end()); // get string
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
      std::cerr << std::endl << "**PANIC ERROR**: function " << str << " doesn't exist, but you never should have reached this point!";
      std::cerr << std::endl;
      assert(0);
    }
    // add node
    node.children.push_back(child);
    // i: '(', 'expression', ')'
    FillFuncEvalTree(i->children.begin()+1, node.children[0]);
  } else if (i->value.id() == PFunctionGrammar::powerID) {
    // keep the id
    node.fID = PFunctionGrammar::powerID;
    // keep function tag
    str =  std::string(i->value.begin(), i->value.end()); // get string

    if (!strcmp(str.c_str(), "POW"))
      node.fFunctionTag = FUN_POW;
    else {
      std::cerr << std::endl << "**PANIC ERROR**: function " << str << " doesn't exist, but you never should have reached this point!";
      std::cerr << std::endl;
      assert(0);
    }
    // i: '(', 'expression', ',', expression, ')'
    // add node
    node.children.push_back(child);
    FillFuncEvalTree(i->children.begin()+1, node.children[0]); // base
    // add node
    node.children.push_back(child);
    FillFuncEvalTree(i->children.begin()+3, node.children[1]); // exponent
  } else if (i->value.id() == PFunctionGrammar::factorID) {
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
 * <p>Stub the check map and fit parameter ranges.
 *
 * \param mapSize size of the map vector
 * \param paramSize size of the parameter vector
 */
Bool_t PFunction::CheckMapAndParamRange(UInt_t mapSize, UInt_t paramSize)
{
  return FindAndCheckMapAndParamRange(fFunc, mapSize, paramSize);
}

//-------------------------------------------------------------
// FindAndCheckMapAndParamRange (protected)
//-------------------------------------------------------------
/**
 * <p>Recursive checking of map and fit parameter ranges.
 *
 * \param node of the evaluation tree
 * \param mapSize size of the map vector
 * \param paramSize size of the fit parameter vector
 */
Bool_t PFunction::FindAndCheckMapAndParamRange(PFuncTreeNode &node, UInt_t mapSize, UInt_t paramSize)
{
  if (node.fID == PFunctionGrammar::realID) {
    return true;
  } else if (node.fID == PFunctionGrammar::constPiID) {
    return true;
  } else if (node.fID == PFunctionGrammar::constGammaMuID) {
    return true;
  } else if (node.fID == PFunctionGrammar::constFieldID) {
    return true;
  } else if (node.fID == PFunctionGrammar::constEnergyID) {
    return true;
  } else if (node.fID == PFunctionGrammar::constTempID) {
    return true;
  } else if (node.fID == PFunctionGrammar::parameterID) {
    if (node.fIvalue <= static_cast<Int_t>(paramSize))
      return true;
    else
      return false;
  } else if (node.fID == PFunctionGrammar::mapID) {
    if (node.fIvalue <= static_cast<Int_t>(mapSize))
      return true;
    else
      return false;
  } else if (node.fID == PFunctionGrammar::functionID) {
    return FindAndCheckMapAndParamRange(node.children[0], mapSize, paramSize);
  } else if (node.fID == PFunctionGrammar::powerID) {
    if (FindAndCheckMapAndParamRange(node.children[0], mapSize, paramSize))
      return FindAndCheckMapAndParamRange(node.children[1], mapSize, paramSize);
    else
      return false;
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
    std::cerr << std::endl << ">> **PANIC ERROR**: PFunction::FindAndCheckMapAndParamRange: you never should have reached this point!";
    std::cerr << std::endl << ">> node.fID = " << node.fID;
    std::cerr << std::endl;
    assert(0);
  }
  return true;
}

//-------------------------------------------------------------
// Eval (public)
//-------------------------------------------------------------
/**
 * <p>Stub starting the evaluation of the evaluation tree.
 *
 * <b>return:</b> the value of the function call.
 *
 * \param param fit parameter vector
 */
Double_t PFunction::Eval(std::vector<Double_t> param, PMetaData metaData)
{
  fParam = param;
  fMetaData = metaData;

  return EvalNode(fFunc);
}

//-------------------------------------------------------------
// EvalNode (protected)
//-------------------------------------------------------------
/**
 * <p>Recursive evaluation of the evaluation tree.
 *
 * \param node of the evaluation tree
 */
Double_t PFunction::EvalNode(PFuncTreeNode &node)
{
  if (node.fID == PFunctionGrammar::realID) {
    return node.fDvalue;
  } else if (node.fID == PFunctionGrammar::constPiID) {
    return node.fDvalue;
  } else if (node.fID == PFunctionGrammar::constGammaMuID) {
    return node.fDvalue;
  } else if (node.fID == PFunctionGrammar::constFieldID) {
    return fMetaData.fField;
  } else if (node.fID == PFunctionGrammar::constEnergyID) {
    if (fMetaData.fEnergy == PMUSR_UNDEFINED) {
      std::cerr << std::endl << "**PANIC ERROR**: PFunction::EvalNode: energy meta data not available." << std::endl;
      std::cerr << std::endl;
      exit(0);
    }
    return fMetaData.fEnergy;
  } else if (node.fID == PFunctionGrammar::constTempID) {
    if (node.fIvalue >= fMetaData.fTemp.size()) {
      std::cerr << std::endl << "**PANIC ERROR**: PFunction::EvalNode: Temp idx=" << node.fIvalue << " requested which is >= #Temp(s)=" << fMetaData.fTemp.size() << " available." << std::endl;
      std::cerr << std::endl;
      exit(0);
    }
    return fMetaData.fTemp[node.fIvalue];
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
      return log(fabs(EvalNode(node.children[0])))/log(10.0);
    } else if (node.fFunctionTag == FUN_LN) {
      return log(fabs(EvalNode(node.children[0])));
    } else if (node.fFunctionTag == FUN_EXP) {
      return exp(EvalNode(node.children[0]));
    } else if (node.fFunctionTag == FUN_SQRT) {
      return sqrt(fabs(EvalNode(node.children[0])));
    } else {
      std::cerr << std::endl << "**PANIC ERROR**: PFunction::EvalNode: node.fID == PFunctionGrammar::functionID: you never should have reached this point!";
      std::cerr << std::endl;
      assert(0);
    }
  } else if (node.fID == PFunctionGrammar::powerID) {
    if (node.fFunctionTag == FUN_POW) {
      Double_t base = EvalNode(node.children[0]);
      Double_t expo = EvalNode(node.children[1]);
      // check that no complex number will result
      if (base < 0.0) { // base is negative which might be fatal
        if (expo-floor(expo) != 0.0) // exponent is not an integer number, hence take -base (positive) to avoid complex numbers, i.e. nan
          base = -base;
      }
      return pow(base, expo);
    } else {
      std::cerr << std::endl << "**PANIC ERROR**: PFunction::EvalNode: node.fID == PFunctionGrammar::powerID: you never should have reached this point!";
      std::cerr << std::endl;
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
        std::cerr << std::endl << "**PANIC ERROR**: PFunction::EvalNode: division by 0.0";
        std::cerr << std::endl << "**PANIC ERROR**: PFunction::EvalNode: requested operation: " << EvalNode(node.children[0]) << "/" << EvalNode(node.children[1]);
        std::cerr << std::endl << ">> " << fFuncString.Data() << std::endl;
        std::cerr << std::endl;
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
    std::cerr << std::endl << "**PANIC ERROR**: PFunction::EvalNode: you never should have reached this point!";
    std::cerr << std::endl;
    assert(0);
  }
  return 0.0;
}

//-------------------------------------------------------------
// CleanupFuncEvalTree (protected)
//-------------------------------------------------------------
/**
 * <p>Stub to clean up the evaluation tree.
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
 * <p>Recursive clean up of the evaluation tree.
 *
 * \param node of the evaluation tree
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
 * <p>Stub to generate the function string (clean and tidy).
 *
 * \param info AST tree
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
 * <p>Recursive generation of the function string (clean and tidy).
 *
 * \param i iterator of the AST tree
 */
void PFunction::EvalTreeForStringExpression(iter_t const& i)
{
  static Int_t termOp = 0;

  if (i->value.id() == PFunctionGrammar::realID) {
    assert(i->children.size() == 0);
    if (*i->value.begin() == '-')
      fFuncString += "(";
    fFuncString += boost::algorithm::trim_copy(std::string(i->value.begin(), i->value.end())).c_str();
    if (*i->value.begin() == '-')
      fFuncString += ")";
  } else if (i->value.id() == PFunctionGrammar::constPiID) {
    fFuncString += "Pi";
  } else if (i->value.id() == PFunctionGrammar::constGammaMuID) {
    fFuncString += "gamma_mu";
  } else if (i->value.id() == PFunctionGrammar::constFieldID) {
    fFuncString += "B";
  } else if (i->value.id() == PFunctionGrammar::constEnergyID) {
    fFuncString += "EN";
  } else if (i->value.id() == PFunctionGrammar::constTempID) {
    assert(i->children.size() == 0);
    fFuncString += boost::algorithm::trim_copy(std::string(i->value.begin(), i->value.end())).c_str();
  } else if (i->value.id() == PFunctionGrammar::funLabelID) {
    assert(i->children.size() == 0);
    //SetFuncNo(i);
    fFuncString += std::string(i->value.begin(), i->value.end()).c_str(); // funx
  } else if (i->value.id() == PFunctionGrammar::parameterID) {
    assert(i->children.size() == 0);
    fFuncString += boost::algorithm::trim_copy(std::string(i->value.begin(), i->value.end())).c_str();
  } else if (i->value.id() == PFunctionGrammar::mapID) {
    assert(i->children.size() == 0);
    fFuncString += boost::algorithm::trim_copy(std::string(i->value.begin(), i->value.end())).c_str();
  } else if (i->value.id() == PFunctionGrammar::functionID) {
    assert(i->children.size() == 3);
    fFuncString += std::string(i->value.begin(), i->value.end()).c_str(); // keep function name
    fFuncString += "(";
    // '(', expression, ')'
    EvalTreeForStringExpression(i->children.begin()+1); // the real stuff
    fFuncString += ")";
  } else if (i->value.id() == PFunctionGrammar::powerID) {
    assert(i->children.size() == 5);
    fFuncString += std::string(i->value.begin(), i->value.end()).c_str(); // keep function name
    fFuncString += "(";
    // '(', expression, ',' expression, ')'
    EvalTreeForStringExpression(i->children.begin()+1); // base expression
    fFuncString += ",";
    EvalTreeForStringExpression(i->children.begin()+3); // exponent expression
    fFuncString += ")";
  } else if (i->value.id() == PFunctionGrammar::factorID) {
    EvalTreeForStringExpression(i->children.begin());
  } else if (i->value.id() == PFunctionGrammar::termID) {
    termOp++;
    if (*i->value.begin() == '*') {
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
