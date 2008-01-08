/***************************************************************************

  PFunction.h

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

#ifndef _PFUNCTION_H_
#define _PFUNCTION_H_

#include <vector>

#include <boost/spirit/tree/ast.hpp>
using namespace boost::spirit;

#include <TString.h>

#include "PFunctionGrammar.h"

//----------------------------------------------------------------------------
#define OP_ADD 0
#define OP_SUB 1
#define OP_MUL 2
#define OP_DIV 3

#define FUN_COS   0
#define FUN_SIN   1
#define FUN_TAN   2
#define FUN_COSH  3
#define FUN_SINH  4
#define FUN_TANH  5
#define FUN_ACOS  6
#define FUN_ASIN  7
#define FUN_ATAN  8
#define FUN_ACOSH 9
#define FUN_ASINH 10
#define FUN_ATANH 11
#define FUN_LOG   12
#define FUN_LN    13
#define FUN_EXP   14

//----------------------------------------------------------------------------
typedef struct func_tree_node {
  int    fID; ///< tag showing what tree element this is
  int    fOperatorTag; ///< tag for '+', '-', '*', '/'
  int    fFunctionTag; ///< tag got "cos", "sin", ...
  int    fIvalue; ///< for parameter numbers and maps
  double fDvalue; ///< for numbers
  vector<func_tree_node> children; ///< holding sub-tree
} PFuncTreeNode;

//----------------------------------------------------------------------------
class PFunction {
  public:
    PFunction(tree_parse_info<> info);
    virtual ~PFunction();

    virtual bool IsValid() { return fValid; }
    virtual int GetFuncNo() { return fFuncNo; }
    virtual bool CheckMapAndParamRange(unsigned int mapSize, unsigned int paramSize);
    virtual double Eval(vector<double> param);
    virtual void SetMap(vector<int> map) { fMap = map; }

    virtual TString* GetFuncString() { return &fFuncString; }

  protected:
    virtual bool SetFuncNo();

    virtual bool FindAndCheckMapAndParamRange(PFuncTreeNode &node, unsigned int mapSize, unsigned int paramSize);
    virtual bool GenerateFuncEvalTree();
    virtual void FillFuncEvalTree(iter_t const& i, PFuncTreeNode &node);
    virtual double EvalNode(PFuncTreeNode &node);
    virtual void CleanupFuncEvalTree();
    virtual void CleanupNode(PFuncTreeNode &node);

  private:
    tree_parse_info<> fInfo;
    vector<double> fParam;
    vector<int> fMap;
    PFuncTreeNode fFunc;

    bool fValid; ///< flag showing if the function is valid
    int fFuncNo; ///< function number, i.e. FUNx with x the function number

    virtual void EvalTreeForString(tree_parse_info<> info);
    virtual void EvalTreeForStringExpression(iter_t const& i);
    TString fFuncString; ///< clear text representation of the function
};

#endif // _PFUNCTION_H_
