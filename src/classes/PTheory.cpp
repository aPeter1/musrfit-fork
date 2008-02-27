/***************************************************************************

  PTheory.cpp

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

#include <iostream>
using namespace std;

#include <TString.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TMath.h>

#include "PMsrHandler.h"
#include "PTheory.h"

#define SQRT_TWO 1.41421356237
#define SQRT_PI  1.77245385091

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p> The theory block his parsed and mapped to a tree. Every line (except the '+')
 * is a theory object by itself, i.e. the whole theory is build up recursivly.
 * Example:
 *   Theory block:
 *     a 1
 *     tf 2 3
 *     se 4
 *     +
 *     a 5
 *     tf 6 7
 *     se 8
 *     +
 *     a 9
 *     tf 10 11
 *
 * This is mapped into the following binary tree:
 *
 *                a 1
 *              +/   \*
 *             a 5   tf 2 3
 *           + /  \*    \ *
 *           a 9  a 5   se 4
 *             \*   \*
 *        tf 10 11  tf 6 7
 *                   \*
 *                   se 8
 *
 * \param msrInfo
 * \param runNo
 * \param parent needed in order to know if PTheory is the root object or a child
 *               false (default) -> this is the root object
 *               true -> this is part of an already existing object tree
 */
PTheory::PTheory(PMsrHandler *msrInfo, unsigned int runNo, const bool hasParent)
{
  // init stuff
  fValid = true;
  fAdd = 0;
  fMul = 0;

  static unsigned int lineNo = 1; // lineNo
  static unsigned int depth  = 0; // needed to handle '+' properly

  if (hasParent == false) { // reset static counters if root object
    lineNo = 1; // the lineNo counter and the depth counter need to be
    depth  = 0; // reset for every root object (new run).
  }

  // get the input to be analyzed from the msr handler
  PMsrLines *fullTheoryBlock = msrInfo->GetMsrTheory();
  if (lineNo > fullTheoryBlock->size()-1) {
    return;
  }
  // get the line to be parsed
  PMsrLineStructure *line = &(*fullTheoryBlock)[lineNo];

  // copy line content to str in order to remove comments
  TString str = line->fLine.Copy();

  // remove theory line comment if present, i.e. something starting with '('
  int index = str.Index("(");
  if (index > 0) // theory line comment present
    str.Resize(index);

  // remove msr-file comment if present, i.e. something starting with '#'
  index = str.Index("#");
  if (index > 0) // theory line comment present
    str.Resize(index);

  // tokenize line
  TObjArray *tokens;
  TObjString *ostr;

  tokens = str.Tokenize(" \t");
  if (!tokens) {
    cout << endl << "**SEVERE ERROR**: PTheory(): Couldn't tokenize theory block line " << line->fLineNo << ".";
    cout << endl << "  line content: " << line->fLine.Data();
    cout << endl;
    exit(0);
  }
  ostr = dynamic_cast<TObjString*>(tokens->At(0));
  str = ostr->GetString();

  // search the theory function
  unsigned int idx = SearchDataBase(str);

  // function found is not defined
  if (idx == (unsigned int) THEORY_UNDEFINED) {
    cout << endl << "**ERROR**: PTheory(): Theory line '" << line->fLine.Data() << "'";
    cout << endl << "  in line no " << line->fLineNo << " is undefined!";
    fValid = false;
    return;
  }

  // line is a valid function, hence analyze parameters
  if ((unsigned int)(tokens->GetEntries()-1) != fNoOfParam) {
    cout << endl << "**ERROR**: PTheory():  Theory line '" << line->fLine.Data() << "'";
    cout << "  in line no " << line->fLineNo;
    cout << "  expecting " << fgTheoDataBase[idx].fNoOfParam << ", but found " << tokens->GetEntries()-1; 
    fValid = false;
  }
  // keep function index
  fType = idx;
  // filter out the parameters
  int status;
  unsigned int value;
  bool ok = false;;
  for (int i=1; i<tokens->GetEntries(); i++) {
    ostr = dynamic_cast<TObjString*>(tokens->At(i));
    str = ostr->GetString();

    // check if str is map
    if (str.Contains("map")) {
      status = sscanf(str.Data(), "map%u", &value);
      if (status == 1) { // everthing ok
        ok = true;
        // get parameter from map
        PIntVector maps = (*msrInfo->GetMsrRunList())[runNo].fMap;
        if ((value <= maps.size()) && (value > 0)) { // everything fine
          fParamNo.push_back(maps[value-1]-1);
        } else { // map index out of range
          cout << endl << "**ERROR**: PTheory: map index " << value << " out of range! See line no " << line->fLineNo;
          fValid = false;
        }
      } else { // something wrong
        cout << endl << "**ERROR**: PTheory: map '" << str.Data() << "' not allowed. See line no " << line->fLineNo;
        fValid = false;
      }
    } else if (str.Contains("fun")) { // check if str is fun
      status = sscanf(str.Data(), "fun%u", &value);
      if (status == 1) { // everthing ok
        ok = true;
        // handle function, i.e. get, from the function number x (FUNx), the function index, 
        // add function offset and fill "parameter vector" 
        fParamNo.push_back(msrInfo->GetFuncIndex(value)+MSR_PARAM_FUN_OFFSET);
      } else { // something wrong
        fValid = false;
      }
    } else { // check if str is param no
      status = sscanf(str.Data(), "%u", &value);
      if (status == 1) { // everthing ok
        ok = true;
        fParamNo.push_back(value-1);
      }
      // check if one of the valid entries was found
      if (!ok) {
        cout << endl << "**ERROR**: PTheory: '" << str.Data() << "' not allowed. See line no " << line->fLineNo;
        fValid = false;
      }
    }
  }

  // call the next line (only if valid so far and not the last line)
  // handle '*'
  if (fValid && (lineNo < fullTheoryBlock->size()-1)) {
    line = &(*fullTheoryBlock)[lineNo+1];
    if (!line->fLine.Contains("+")) { // make sure next line is not a '+'
      depth++;
      lineNo++;
      fMul = new PTheory(msrInfo, runNo, true);
      depth--;
    }
  }
  // call the next line (only if valid so far and not the last line)
  // handle '+'
  if (fValid && (lineNo < fullTheoryBlock->size()-1)) {
    line = &(*fullTheoryBlock)[lineNo+1];
    if ((depth == 0) && line->fLine.Contains("+")) {
      lineNo += 2; // go to the next theory function line
      fAdd = new PTheory(msrInfo, runNo, true);
    }
  }

  // make clean and tidy theory block for the msr-file
  if (fValid && !hasParent) { // parent theory object
    MakeCleanAndTidyTheoryBlock(fullTheoryBlock);
  }

  // clean up
  if (tokens) {
    delete tokens;
    tokens = 0;
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 */
PTheory::~PTheory()
{
//cout << endl << "PTheory::~PTheory() ..." << endl;

  fParamNo.clear();

  if (fMul) {
    delete fMul;
    fMul = 0;
  }

  if (fAdd) {
    delete fAdd;
    fAdd = 0;
  }
}

//--------------------------------------------------------------------------
// IsValid
//--------------------------------------------------------------------------
/**
 * <p>Checks if the theory tree is valid. Needs to be implemented!!
 *
 */
bool PTheory::IsValid()
{

  if (fMul) {
    if (fAdd) {
      return (fValid && fMul->IsValid() && fAdd->IsValid());
    } else {
      return (fValid && fMul->IsValid());
    }
  } else {
    if (fAdd) {
      return (fValid && fAdd->IsValid());
    } else {
      return fValid;
    }
  }

  return false;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t
 * \param paramValues
 */
double PTheory::Func(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{

  if (fMul) {
    if (fAdd) { // fMul != 0 && fAdd != 0
      switch (fType) {
        case THEORY_ASYMMETRY:
          return Asymmetry(paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_SIMPLE_EXP:
          return SimpleExp(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_GENERAL_EXP:
          return GeneralExp(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_SIMPLE_GAUSS:
          return SimpleGauss(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_GAUSS_KT:
          return StaticGaussKT(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_KT_TABLE:
          return StaticKTTable(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_KT_TABLE:
          return DynamicKTTable(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_COMBI_LGKT:
          return CombiLGKT(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_SPIN_GLASS:
          return SpinGlass(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_RANDOM_ANISOTROPIC_HYPERFINE:
          return RandomAnisotropicHyperfine(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_ABRAGAM:
          return Abragam(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) + 
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD:
          return InternalField(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_TF_COS:
          return TFCos(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) + 
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_BESSEL:
          return Bessel(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) + 
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_BESSEL:
          return InternalBessel(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_SKEWED_GAUSS:
          return SkewedGauss(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        default:
          cout << endl << "**PANIC ERROR**: PTheory::Func: You never should have reached this line?!?! (" << fType << ")";
          cout << endl;
          exit(0);
      }
    } else { // fMul !=0 && fAdd == 0
      switch (fType) {
        case THEORY_ASYMMETRY:
          return Asymmetry(paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_SIMPLE_EXP:
          return SimpleExp(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_GENERAL_EXP:
          return GeneralExp(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_SIMPLE_GAUSS:
          return SimpleGauss(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_GAUSS_KT:
          return StaticGaussKT(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_KT_TABLE:
          return StaticKTTable(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_KT_TABLE:
          return DynamicKTTable(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_COMBI_LGKT:
          return CombiLGKT(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_SPIN_GLASS:
          return SpinGlass(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_RANDOM_ANISOTROPIC_HYPERFINE:
          return RandomAnisotropicHyperfine(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_ABRAGAM:
          return Abragam(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD:
          return InternalField(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_TF_COS:
          return TFCos(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_BESSEL:
          return Bessel(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_BESSEL:
          return InternalBessel(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_SKEWED_GAUSS:
          return SkewedGauss(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        default:
          cout << endl << "**PANIC ERROR**: PTheory::Func: You never should have reached this line?!?! (" << fType << ")";
          cout << endl;
          exit(0);
      }
    }
  } else { // fMul == 0 && fAdd != 0
    if (fAdd) {
      switch (fType) {
        case THEORY_ASYMMETRY:
          return Asymmetry(paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_SIMPLE_EXP:
          return SimpleExp(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_GENERAL_EXP:
          return GeneralExp(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_SIMPLE_GAUSS:
          return SimpleGauss(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_GAUSS_KT:
          return StaticGaussKT(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_KT_TABLE:
          return StaticKTTable(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_KT_TABLE:
          return DynamicKTTable(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_COMBI_LGKT:
          return CombiLGKT(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_SPIN_GLASS:
          return SpinGlass(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_RANDOM_ANISOTROPIC_HYPERFINE:
          return RandomAnisotropicHyperfine(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_ABRAGAM:
          return Abragam(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD:
          return InternalField(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_TF_COS:
          return TFCos(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_BESSEL:
          return Bessel(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_BESSEL:
          return InternalBessel(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_SKEWED_GAUSS:
          return SkewedGauss(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        default:
          cout << endl << "**PANIC ERROR**: PTheory::Func: You never should have reached this line?!?! (" << fType << ")";
          cout << endl;
          exit(0);
      }
    } else { // fMul == 0 && fAdd == 0
      switch (fType) {
        case THEORY_ASYMMETRY:
          return Asymmetry(paramValues, funcValues);
          break;
        case THEORY_SIMPLE_EXP:
          return SimpleExp(t, paramValues, funcValues);
          break;
        case THEORY_GENERAL_EXP:
          return GeneralExp(t, paramValues, funcValues);
          break;
        case THEORY_SIMPLE_GAUSS:
          return SimpleGauss(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_GAUSS_KT:
          return StaticGaussKT(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_KT_TABLE:
          return StaticKTTable(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_KT_TABLE:
          return DynamicKTTable(t, paramValues, funcValues);
          break;
        case THEORY_COMBI_LGKT:
          return CombiLGKT(t, paramValues, funcValues);
          break;
        case THEORY_SPIN_GLASS:
          return SpinGlass(t, paramValues, funcValues);
          break;
        case THEORY_RANDOM_ANISOTROPIC_HYPERFINE:
          return RandomAnisotropicHyperfine(t, paramValues, funcValues);
          break;
        case THEORY_ABRAGAM:
          return Abragam(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD:
          return InternalField(t, paramValues, funcValues);
          break;
        case THEORY_TF_COS:
          return TFCos(t, paramValues, funcValues);
          break;
        case THEORY_BESSEL:
          return Bessel(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_BESSEL:
          return InternalBessel(t, paramValues, funcValues);
          break;
        case THEORY_SKEWED_GAUSS:
          return SkewedGauss(t, paramValues, funcValues);
          break;
        default:
          cout << endl << "**PANIC ERROR**: PTheory::Func: You never should have reached this line?!?! (" << fType << ")";
          cout << endl;
          exit(0);
      }
    }
  }

  return 0.0;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param name
 */
int PTheory::SearchDataBase(TString name)
{
  int idx = THEORY_UNDEFINED;

  for (unsigned int i=0; i<THEORY_MAX; i++) {
    if (!name.CompareTo(fgTheoDataBase[i].fName, TString::kIgnoreCase) ||
        !name.CompareTo(fgTheoDataBase[i].fAbbrev, TString::kIgnoreCase)) {
      idx = fgTheoDataBase[i].fType;
      fType = idx;
      fNoOfParam = fgTheoDataBase[i].fNoOfParam;
    }
  }

  return idx;
}

//--------------------------------------------------------------------------
// MakeCleanAndTidyTheoryBlock private
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param fullTheoryBlock
 */
void PTheory::MakeCleanAndTidyTheoryBlock(PMsrLines *fullTheoryBlock)
{
  PMsrLineStructure *line;
  TString str, tidy;
  char substr[256];
  TObjArray *tokens = 0;
  TObjString *ostr;
  int idx = THEORY_UNDEFINED;

  for (unsigned int i=1; i<fullTheoryBlock->size(); i++) {
    // get the line to be prettyfied
    line = &(*fullTheoryBlock)[i];
    // copy line content to str in order to remove comments
    str = line->fLine.Copy();
    // tokenize line
    tokens = str.Tokenize(" \t");
    // make a handable string out of the asymmetry token
    ostr = dynamic_cast<TObjString*>(tokens->At(0));
    str = ostr->GetString();
    // check if the line is just a '+'; if so nothing to be done
    if (str.Contains("+"))
      continue;
    // search the theory function
    for (unsigned int j=0; j<THEORY_MAX; j++) {
      if (!str.CompareTo(fgTheoDataBase[j].fName, TString::kIgnoreCase) ||
          !str.CompareTo(fgTheoDataBase[j].fAbbrev, TString::kIgnoreCase)) {
        idx = fgTheoDataBase[j].fType;
      }
    }
    // check if theory is indeed defined. This should not be necessay at this point but ...
    if (idx == THEORY_UNDEFINED)
      continue;
    // check that there enough tokens. This should not be necessay at this point but ...
    if ((unsigned int)tokens->GetEntries() < fgTheoDataBase[idx].fNoOfParam + 1)
      continue;
    // make tidy string
    sprintf(substr, "%-10s", fgTheoDataBase[idx].fName.Data());
    tidy = TString(substr);
    for (unsigned j=1; j<fgTheoDataBase[idx].fNoOfParam+1; j++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(j));
      str = ostr->GetString();
      sprintf(substr, "%6s", str.Data());
      tidy += TString(substr);
    }
    if (fgTheoDataBase[idx].fComment.Length() != 0) {
      unsigned int size = tidy.Length();
      for (unsigned int k=0; k<35-size; k++)
        tidy += TString(" ");
      tidy += fgTheoDataBase[idx].fComment;
    }
    // write tidy string back into theory block
    (*fullTheoryBlock)[i].fLine = tidy;
  }

  // clean up
  if (tokens) {
    delete tokens;
    tokens = 0;
  }
}

//--------------------------------------------------------------------------
/**
 * <p> Asymmetry
 * \f[ = A \f]
 *
 * <p> Every theory-function handles its parameters the following way: It holds
 * an array fParamNo which is holding the parameter/map/function number. The numbering
 * is as such: par1, par2, ..., parX1, internalPar1, internalParX2, map1, ..., mapX3, 
 * fun1, ..., funX4, where X1 is the number of parameters, X2 the number of internal 
 * parameters, i.e. parameters listed in the runs, X3 the number of maps, and X4 the 
 * number of used functions.
 *
 * \param paramValues is an array of parameter values [par1, par2, ..., parX1, 
 *                    internalPar1, ..., internalParX2, map1, ..., mapX3, fun1, ..., funX4]
 */
double PTheory::Asymmetry(const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double asym;

  // check if FUNCTIONS are used
  if (fParamNo[0] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
    asym = paramValues[fParamNo[0]];
  } else {
    asym = funcValues[fParamNo[0]-MSR_PARAM_FUN_OFFSET];
  }

  return asym;
}

//--------------------------------------------------------------------------
/**
 * <p> Simple exponential
 * \f[ = \exp\left(-\lambda t\right) \f].
 *
 * <p> For details concerning fParamNo and paramValues see PTheory::Asymmetry and
 * PTheory::PTheory.
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues parameter values, here only one, namely the depolarization 
 * rate \f$\lambda\f$ in \f$(1/\mu\mathrm{s})\f$
 */
double PTheory::SimpleExp(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double lambda;

  // check if FUNCTIONS are used
  if (fParamNo[0] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
    lambda = paramValues[fParamNo[0]];
  } else { // function
    lambda = funcValues[fParamNo[0]-MSR_PARAM_FUN_OFFSET];
  }

  return TMath::Exp(-t*lambda);
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::GeneralExp(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double val[2];

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<2; i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  return TMath::Exp(-TMath::Power(t*val[0], val[1]));
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::SimpleGauss(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double sigma;

  // check if FUNCTIONS are used
  if (fParamNo[0] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
    sigma = paramValues[fParamNo[0]];
  } else { // function
    sigma = funcValues[fParamNo[0]-MSR_PARAM_FUN_OFFSET];
  }

  return TMath::Exp(-0.5*TMath::Power(t*sigma, 2.0));
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::StaticGaussKT(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double sigma;

  // check if FUNCTIONS are used
  if (fParamNo[0] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
    sigma = paramValues[fParamNo[0]];
  } else { // function
    sigma = funcValues[fParamNo[0]-MSR_PARAM_FUN_OFFSET];
  }

  double sigma_t_2 = t*t*sigma*sigma;

  return 0.333333333333333 * (1.0 + 2.0*(1.0 - sigma_t_2)*TMath::Exp(-0.5*sigma_t_2));
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::StaticKTTable(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  return 0.0;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::DynamicKTTable(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  return 0.0;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::CombiLGKT(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double val[2];

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<2; i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double lambdaL_t   = t*val[0];
  double lambdaG_t_2 = t*t*val[1]*val[1];

  return 0.333333333333333 * 
         (1.0 + 2.0*(1.0-lambdaL_t-lambdaG_t_2)*TMath::Exp(-(lambdaL_t+0.5*lambdaG_t_2)));
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::SpinGlass(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  if (paramValues[fParamNo[0]] == 0.0)
    return 1.0;

  double val[3];

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<3; i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double lambda_2     = val[0]*val[0];
  double lambda_t_2_q = t*t*lambda_2*val[2];
  double rate_2       = 4.0*lambda_2*(1.0-val[2])*t/val[1];

  double rateL = TMath::Sqrt(rate_2);
  double rateT = TMath::Sqrt(rate_2+lambda_t_2_q);

  return 0.333333333333333*(TMath::Exp(-rateL) + 2.0*(1.0-lambda_t_2_q/rateT)*TMath::Exp(-rateT));
}

//--------------------------------------------------------------------------
/**
 * <p>Where does this come from ??? please give a reference
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::RandomAnisotropicHyperfine(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double val[2];

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<2; i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double nu_t     = t*val[0];
  double lambda_t = t*val[1];

  return 0.166666666666667*(1.0-0.5*nu_t)*TMath::Exp(-0.5*nu_t) +
         0.333333333333333*(1.0-0.25*nu_t)*TMath::Exp(-0.25*(nu_t+2.44949*lambda_t));
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::Abragam(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double val[2];

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<2; i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double gamma_t = t*val[1];

  return TMath::Exp(-TMath::Power(val[0]/val[1],2.0)*
                    (TMath::Exp(-gamma_t)-1.0-gamma_t));
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::InternalField(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double val[4];

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<4; i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  return 0.666666666666667*
                 TMath::Cos(DEG_TO_RAD*val[0])+TWO_PI*val[1]*
                 TMath::Exp(-val[2]*t) +
         0.333333333333333*TMath::Exp(-val[3]*t);
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::TFCos(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double val[2];

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<2; i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  return TMath::Cos(DEG_TO_RAD*val[0]+TWO_PI*val[1]*t);
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::Bessel(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double val[2];

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<2; i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  return TMath::BesselJ0(DEG_TO_RAD*val[0]+TWO_PI*val[1]*t);
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::InternalBessel(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double val[4];

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<4; i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  return 0.666666666666667*
                 TMath::BesselJ0(DEG_TO_RAD*val[0]+TWO_PI*val[1])*
                 TMath::Exp(-val[2]*t) +
         0.333333333333333*TMath::Exp(-val[3]*t);
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::SkewedGauss(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const
{
  double val[4];
  double skg;

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<4; i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }


  // val[2] = sigma-, val[3] = sigma+
  double z1 = val[3]*t/SQRT_TWO; // sigma+
  double z2 = val[2]*t/SQRT_TWO; // sigma-
  double g1 = TMath::Exp(-0.5*TMath::Power(t*val[3], 2.0)); // gauss sigma+
  double g2 = TMath::Exp(-0.5*TMath::Power(t*val[2], 2.0)); // gauss sigma-

  if ((z1 >= 25.0) || (z2 >= 25.0)) // needed to prevent crash of 1F1
    skg = 2.0e300;
  else
    skg = 0.5*TMath::Cos(DEG_TO_RAD*val[0]+TWO_PI*val[1]*t) * ( g1 + g2 ) -
          0.5*TMath::Sin(DEG_TO_RAD*val[0]+TWO_PI*val[1]*t) *
          (
          g1*2.0*z1/SQRT_PI*gsl_sf_hyperg_1F1(0.5,1.5,z1*z1) -
          g2*2.0*z2/SQRT_PI*gsl_sf_hyperg_1F1(0.5,1.5,z2*z2)
          );

  return skg;
}
