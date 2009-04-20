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

#include <TObject.h>
#include <TString.h>
#include <TF1.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TClass.h>
#include <TMath.h>

#include <Math/SpecFuncMathMore.h>

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
  fUserFcnClassName = TString("");
  fUserFcn = 0;
  fDynLFdt = 0.0;

  static unsigned int lineNo = 1; // lineNo
  static unsigned int depth  = 0; // needed to handle '+' properly

  if (hasParent == false) { // reset static counters if root object
    lineNo = 1; // the lineNo counter and the depth counter need to be
    depth  = 0; // reset for every root object (new run).
  }

  for (unsigned int i=0; i<THEORY_MAX_PARAM; i++)
    fPrevParam[i] = 0.0;

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
  if (((unsigned int)(tokens->GetEntries()-1) < fNoOfParam) &&
      ((idx != THEORY_USER_FCN) && (idx != THEORY_POLYNOM))) {
    cout << endl << "**ERROR**: PTheory():  Theory line '" << line->fLine.Data() << "'";
    cout << endl << "  in line no " << line->fLineNo;
    cout << endl << "  expecting " << fgTheoDataBase[idx].fNoOfParam << ", but found " << tokens->GetEntries()-1; 
    fValid = false;
  }
  // keep function index
  fType = idx;
  // filter out the parameters
  int status;
  unsigned int value;
  bool ok = false;
  for (int i=1; i<tokens->GetEntries(); i++) {
    ostr = dynamic_cast<TObjString*>(tokens->At(i));
    str = ostr->GetString();

    // if userFcn, the first entry is the function name and needs to be handled specially
    if ((fType == THEORY_USER_FCN) && ((i == 1) || (i == 2))) {
//cout << endl << ">> userFcn: i=" << i << ", str=" << str.Data() << endl;
      if (i == 1) {
        fUserFcnSharedLibName = str;
      }
      if (i == 2) {
        fUserFcnClassName = str;
      }
      continue;
    }

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

  // check if user function, if so, check if it is reachable (root) and if yes invoke object
  if (!fUserFcnClassName.IsWhitespace()) {
    cout << endl << ">> user function class name: " << fUserFcnClassName.Data() << endl;
    if (!TClass::GetDict(fUserFcnClassName.Data())) {
      if (gSystem->Load(fUserFcnSharedLibName.Data()) < 0) {
        cout << endl << "**ERROR**: PTheory: user function class '" << fUserFcnClassName.Data() << "' not found.";
        cout << endl << "           Tried to load " << fUserFcnSharedLibName.Data() << " but failed.";
        cout << endl << "           See line no " << line->fLineNo;
        fValid = false;
        // clean up
        if (tokens) {
          delete tokens;
          tokens = 0;
        }
        return;
      }
    }
    // invoke user function object
    fUserFcn = 0;
    fUserFcn = (PUserFcnBase*)TClass::GetClass(fUserFcnClassName.Data())->New();
//cout << endl << ">> fUserFcn = " << fUserFcn << endl;
    if (fUserFcn == 0) {
      cout << endl << "**ERROR**: PTheory: user function object could not be invoked. See line no " << line->fLineNo;
      fValid = false;
    } else { // user function valid, hence expand the fUserParam vector to the proper size
       fUserParam.resize(fParamNo.size());
    }
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
  fUserParam.clear();

  fLFIntegral.clear();
  fDynLFFuncValue.clear();

  if (fMul) {
    delete fMul;
    fMul = 0;
  }

  if (fAdd) {
    delete fAdd;
    fAdd = 0;
  }

  if (fUserFcn) {
    delete fUserFcn;
    fUserFcn = 0;
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
double PTheory::Func(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
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
        case THEORY_STATIC_GAUSS_KT_LF:
          return StaticGaussKTLF(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_GAUSS_KT_LF:
          return DynamicGaussKTLF(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_LORENTZ_KT:
          return StaticLorentzKT(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_LORENTZ_KT_LF:
          return StaticLorentzKTLF(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_LORENTZ_KT_LF:
          return DynamicLorentzKTLF(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
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
        case THEORY_POLYNOM:
          return Polynom(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_USER_FCN:
          return UserFcn(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
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
        case THEORY_STATIC_GAUSS_KT_LF:
          return StaticGaussKTLF(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_GAUSS_KT_LF:
          return DynamicGaussKTLF(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_LORENTZ_KT:
          return StaticLorentzKT(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_LORENTZ_KT_LF:
          return StaticLorentzKTLF(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_LORENTZ_KT_LF:
          return DynamicLorentzKTLF(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
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
        case THEORY_POLYNOM:
          return Polynom(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_USER_FCN:
          return UserFcn(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
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
        case THEORY_STATIC_GAUSS_KT_LF:
          return StaticGaussKTLF(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_GAUSS_KT_LF:
          return DynamicGaussKTLF(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_LORENTZ_KT:
          return StaticLorentzKT(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_LORENTZ_KT_LF:
          return StaticLorentzKTLF(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_LORENTZ_KT_LF:
          return DynamicLorentzKTLF(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
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
        case THEORY_POLYNOM:
          return Polynom(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_USER_FCN:
          return UserFcn(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
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
        case THEORY_STATIC_GAUSS_KT_LF:
          return StaticGaussKTLF(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_GAUSS_KT_LF:
          return DynamicGaussKTLF(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_LORENTZ_KT:
          return StaticLorentzKT(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_LORENTZ_KT_LF:
          return StaticLorentzKTLF(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_LORENTZ_KT_LF:
          return DynamicLorentzKTLF(t, paramValues, funcValues);
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
        case THEORY_POLYNOM:
          return Polynom(t, paramValues, funcValues);
          break;
        case THEORY_USER_FCN:
          return UserFcn(t, paramValues, funcValues);
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
    // remove theory line comment if present, i.e. something starting with '('
    int index = str.Index("(");
    if (index > 0) // theory line comment present
      str.Resize(index);
    // tokenize line
    tokens = str.Tokenize(" \t");
    // make a handable string out of the asymmetry token
    ostr = dynamic_cast<TObjString*>(tokens->At(0));
    str = ostr->GetString();
    // check if the line is just a '+' if so nothing to be done
    if (str.Contains("+"))
      continue;
    // check if the function is a polynom
    if (!str.CompareTo("p") || str.Contains("polynom")) {
      MakeCleanAndTidyPolynom(i, fullTheoryBlock);
      continue;
    }
    // check if the function is a userFcn
    if (!str.CompareTo("u") || str.Contains("userFcn")) {
      MakeCleanAndTidyUserFcn(i, fullTheoryBlock);
      continue;
    }
    // search the theory function
    for (unsigned int j=0; j<THEORY_MAX; j++) {
      if (!str.CompareTo(fgTheoDataBase[j].fName, TString::kIgnoreCase) ||
          !str.CompareTo(fgTheoDataBase[j].fAbbrev, TString::kIgnoreCase)) {
        idx = fgTheoDataBase[j].fType;
      }
    }
    // check if theory is indeed defined. This should not be necessay at this point but ...
    if (idx == THEORY_UNDEFINED)
      return;
    // check that there enough tokens. This should not be necessay at this point but ...
    if ((unsigned int)tokens->GetEntries() < fgTheoDataBase[idx].fNoOfParam + 1)
      return;
    // make tidy string
    sprintf(substr, "%-10s", fgTheoDataBase[idx].fName.Data());
    tidy = TString(substr);
    for (unsigned int j=1; j<(unsigned int)tokens->GetEntries(); j++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(j));
      str = ostr->GetString();
      sprintf(substr, "%6s", str.Data());
      tidy += TString(substr);
    }
    if (fgTheoDataBase[idx].fComment.Length() != 0) {
      if (tidy.Length() < 35) {
        for (unsigned int k=0; k<35-(unsigned int)tidy.Length(); k++)
          tidy += TString(" ");
      } else {
        tidy += TString(" ");
      }
      if ((unsigned int)tokens->GetEntries() == fgTheoDataBase[idx].fNoOfParam + 1) // no tshift
        tidy += fgTheoDataBase[idx].fComment;
      else
        tidy += fgTheoDataBase[idx].fCommentTimeShift;
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
// MakeCleanAndTidyPolynom private
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param fullTheoryBlock
 */
void PTheory::MakeCleanAndTidyPolynom(unsigned int i, PMsrLines *fullTheoryBlock)
{
  PMsrLineStructure *line;
  TString str, tidy;
  TObjArray *tokens = 0;
  TObjString *ostr;
  char substr[256];

//cout << endl << ">> MakeCleanAndTidyPolynom: " << (*fullTheoryBlock)[i].fLine.Data();

  // init tidy
  tidy = TString("polynom ");
  // get the line to be prettyfied
  line = &(*fullTheoryBlock)[i];
  // copy line content to str in order to remove comments
  str = line->fLine.Copy();
  // tokenize line
  tokens = str.Tokenize(" \t");

  // check if comment is already present, and if yes ignore it by setting max correctly
  unsigned int max = (unsigned int)tokens->GetEntries();
  for (unsigned int j=1; j<max; j++) {
    ostr = dynamic_cast<TObjString*>(tokens->At(j));
    str = ostr->GetString();
    if (str.Contains("(")) { // comment present
      max=j;
      break;
    }
  }

  for (unsigned int j=1; j<max; j++) {
    ostr = dynamic_cast<TObjString*>(tokens->At(j));
    str = ostr->GetString();
    sprintf(substr, "%6s", str.Data());
    tidy += TString(substr);
  }

  // add comment
  tidy += " (tshift p0 p1 ... pn)";

  // write tidy string back into theory block
  (*fullTheoryBlock)[i].fLine = tidy;

  // clean up
  if (tokens) {
    delete tokens;
    tokens = 0;
  }
}

//--------------------------------------------------------------------------
// MakeCleanAndTidyUserFcn private
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param fullTheoryBlock
 */
void PTheory::MakeCleanAndTidyUserFcn(unsigned int i, PMsrLines *fullTheoryBlock)
{
  PMsrLineStructure *line;
  TString str, tidy;
  TObjArray *tokens = 0;
  TObjString *ostr;

//cout << endl << ">> MakeCleanAndTidyUserFcn: " << (*fullTheoryBlock)[i].fLine.Data();

  // init tidy
  tidy = TString("userFcn ");
  // get the line to be prettyfied
  line = &(*fullTheoryBlock)[i];
  // copy line content to str in order to remove comments
  str = line->fLine.Copy();
  // tokenize line
  tokens = str.Tokenize(" \t");

  for (unsigned int j=1; j<(unsigned int)tokens->GetEntries(); j++) {
    ostr = dynamic_cast<TObjString*>(tokens->At(j));
    str = ostr->GetString();
    tidy += TString(" ") + str;
  }

  // write tidy string back into theory block
  (*fullTheoryBlock)[i].fLine = tidy;

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
 * \param paramValues
 */
double PTheory::Asymmetry(const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: asym

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
 * \f[ = \exp\left(-\lambda t\right) \f] or
 * \f[ = \exp\left(-\lambda (t-t_{\rm shift} \right) \f]
 *
 * <p> For details concerning fParamNo and paramValues see PTheory::Asymmetry and
 * PTheory::PTheory.
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues parameter values: depolarization rate \f$\lambda\f$ 
 *          in \f$(1/\mu\mathrm{s})\f$, optionally \f$t_{\rm shift}\f$
 */
double PTheory::SimpleExp(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: lambda [tshift]

  double val[2];

  assert(fParamNo.size() <= 2);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double tt;
  if (fParamNo.size() == 1) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[1];

  return TMath::Exp(-tt*val[0]);
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::GeneralExp(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: lambda beta [tshift]

  double val[3];
  double result;

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  // check if tt*val[0] < 0 and 
  if ((tt*val[0] < 0) && (trunc(val[1])-val[1] != 0.0)) {
    result = 0.0;
  } else {
    result = TMath::Exp(-TMath::Power(tt*val[0], val[1]));
  }

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::SimpleGauss(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: sigma [tshift]

  double val[2];

  assert(fParamNo.size() <= 2);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double tt;
  if (fParamNo.size() == 1) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[1];

  return TMath::Exp(-0.5*TMath::Power(tt*val[0], 2.0));
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::StaticGaussKT(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: sigma [tshift]

  double val[2];

  assert(fParamNo.size() <= 2);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double sigma_t_2;
  if (fParamNo.size() == 1) // no tshift
    sigma_t_2 = t*t*val[0]*val[0];
  else // tshift present
    sigma_t_2 = (t-val[1])*(t-val[1])*val[0]*val[0];

  return 0.333333333333333 * (1.0 + 2.0*(1.0 - sigma_t_2)*TMath::Exp(-0.5*sigma_t_2));
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::StaticGaussKTLF(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: frequency damping [tshift]

  double val[3];
  double result;

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  // check if the parameter values have changed, and if yes recalculate the non-analytic integral
  bool newParam = false;
  for (unsigned int i=0; i<3; i++) {
    if (val[i] != fPrevParam[i]) {
      newParam = true;
      break;
    }
  }

  if (newParam) { // new parameters found
    for (unsigned int i=0; i<3; i++)
      fPrevParam[i] = val[i];
    CalculateGaussLFIntegral(val);
  }

  double tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  if (tt < 0.0) // for times < 0 return a function value of 0.0
    return 0.0;

  if (val[0] < 0.02) { // if smaller 20kHz ~ 0.27G use zero field formula
    double sigma_t_2 = tt*tt*val[1]*val[1];
    result = 0.333333333333333 * (1.0 + 2.0*(1.0 - sigma_t_2)*TMath::Exp(-0.5*sigma_t_2));
  } else {
    double delta = val[1];
    double w0    = 2.0*TMath::Pi()*val[0];

    result = 1.0 - 2.0*TMath::Power(delta/w0,2.0)*(1.0 - 
                TMath::Exp(-0.5*TMath::Power(delta*tt, 2.0))*TMath::Cos(w0*tt)) +
                GetLFIntegralValue(tt);
  }

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::DynamicGaussKTLF(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: frequency damping hopping [tshift]

  double val[4];
  double result = 0.0;
  bool useKeren = false;

  assert(fParamNo.size() <= 4);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  // check if Keren approximation can be used
  if (val[2]/val[1] > 5.0) // nu/Delta > 5.0
    useKeren = true;

  if (!useKeren) {
    // check if the parameter values have changed, and if yes recalculate the non-analytic integral
    bool newParam = false;
    for (unsigned int i=0; i<4; i++) {
      if (val[i] != fPrevParam[i]) {
        newParam = true;
        break;
      }
    }

    if (newParam) { // new parameters found
      for (unsigned int i=0; i<4; i++)
        fPrevParam[i] = val[i];
      CalculateDynKTLF(val, 0); // 0 means Gauss
    }
  }

  double tt;
  if (fParamNo.size() == 3) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[3];

  if (tt < 0.0) // for times < 0 return a function value of 0.0
    return 0.0;


  if (useKeren) { // see PRB50, 10039 (1994)
    double wL  = TWO_PI * val[0];
    double wL2 = wL*wL;
    double nu2 = val[2]*val[2];
    double Gamma_t = 2.0*val[1]*val[1]/((wL2+nu2)*(wL2+nu2))*
                       ((wL2+nu2)*val[2]*t
                        + (wL2-nu2)*(1.0 - TMath::Exp(-val[2]*t)*TMath::Cos(wL*t))
                        - 2.0*val[2]*wL*TMath::Exp(-val[2]*t)*TMath::Sin(wL*t));
    result = TMath::Exp(-Gamma_t);
  } else { // from Voltera
    result = GetDynKTLFValue(tt);
  }

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p> (see Uemura et al. PRB 31, 546 (85))
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::StaticLorentzKT(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: lambda [tshift]

  double val[2];

  assert(fParamNo.size() <= 2);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double a_t;
  if (fParamNo.size() == 1) // no tshift
    a_t = t*val[0];
  else // tshift present
    a_t = (t-val[1])*val[0];

  return 0.333333333333333 * (1.0 + 2.0*(1.0 - a_t)*TMath::Exp(-a_t));
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::StaticLorentzKTLF(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: frequency damping [tshift]

  double val[3];
  double result;

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  // check if the parameter values have changed, and if yes recalculate the non-analytic integral
  bool newParam = false;
  for (unsigned int i=0; i<3; i++) {
    if (val[i] != fPrevParam[i]) {
      newParam = true;
      break;
    }
  }

  if (newParam) { // new parameters found
    for (unsigned int i=0; i<3; i++)
      fPrevParam[i] = val[i];
    CalculateLorentzLFIntegral(val);
  }

  double tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

 if (tt < 0.0) // for times < 0 return a function value of 0.0
   return 0.0;

 if (val[0] < 0.02) { // if smaller 20kHz ~ 0.27G use zero field formula
    double at = tt*val[1];
    result = 0.333333333333333 * (1.0 + 2.0*(1.0 - at)*TMath::Exp(-at));
  } else {
    double a    = val[1];
    double at   = a*tt;
    double w0   = 2.0*TMath::Pi()*val[0];
    double a_w0 = a/w0;
    double w0t  = w0*tt;

    double j1, j0;
    if (fabs(w0t) < 0.001) { // check zero time limits of the spherical bessel functions j0(x) and j1(x)
      j0 = 1.0;
      j1 = 0.0;
    } else {
      j0 = sin(w0t)/w0t;
      j1 = (sin(w0t)-w0t*cos(w0t))/(w0t*w0t);
    }

    result = 1.0 - a_w0*j1*exp(-at) - a_w0*a_w0*(j0*exp(-at) - 1.0) - GetLFIntegralValue(tt);
  }

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::DynamicLorentzKTLF(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: frequency damping hopping [tshift]

  double val[4];
  double result = 0.0;

  assert(fParamNo.size() <= 4);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  // check if the parameter values have changed, and if yes recalculate the non-analytic integral
  bool newParam = false;
  for (unsigned int i=0; i<4; i++) {
    if (val[i] != fPrevParam[i]) {
      newParam = true;
      break;
    }
  }

  if (newParam) { // new parameters found
    for (unsigned int i=0; i<4; i++)
      fPrevParam[i] = val[i];
    CalculateDynKTLF(val, 1); // 0 means Lorentz
  }

  double tt;
  if (fParamNo.size() == 3) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[3];

  if (tt < 0.0) // for times < 0 return a function value of 0.0
    return 0.0;

  result = GetDynKTLFValue(tt);

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::CombiLGKT(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: lambdaL lambdaG [tshift]

  double val[3];

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  double lambdaL_t   = tt*val[0];
  double lambdaG_t_2 = tt*tt*val[1]*val[1];

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
double PTheory::SpinGlass(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: lambda gamma q [tshift]

  if (paramValues[fParamNo[0]] == 0.0)
    return 1.0;

  double val[4];

  assert(fParamNo.size() <= 4);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double tt;
  if (fParamNo.size() == 3) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[3];

  double lambda_2     = val[0]*val[0];
  double lambda_t_2_q = tt*tt*lambda_2*val[2];
  double rate_2       = 4.0*lambda_2*(1.0-val[2])*tt/val[1];

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
double PTheory::RandomAnisotropicHyperfine(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: nu lambda [tshift]

  double val[3];

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  double nu_t     = tt*val[0];
  double lambda_t = tt*val[1];

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
double PTheory::Abragam(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: sigma gamma [tshift]

  double val[3];

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  double gamma_t = tt*val[1];

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
double PTheory::InternalField(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: fraction phase frequency rateT rateL [tshift]

  double val[6];

  assert(fParamNo.size() <= 6);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double tt;
  if (fParamNo.size() == 5) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[5];

  return val[0]*TMath::Cos(DEG_TO_RAD*val[1]+TWO_PI*val[2]*tt)*TMath::Exp(-val[3]*tt) +
         (1-val[0])*TMath::Exp(-val[4]*tt);
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::TFCos(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: phase frequency [tshift]

  double val[3];

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  return TMath::Cos(DEG_TO_RAD*val[0]+TWO_PI*val[1]*tt);
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::Bessel(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: phase frequency [tshift]

  double val[3];

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  return TMath::BesselJ0(DEG_TO_RAD*val[0]+TWO_PI*val[1]*tt);
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::InternalBessel(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: fraction phase frequency rateT rateL [tshift]

  double val[6];

  assert(fParamNo.size() <= 6);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  double tt;
  if (fParamNo.size() == 5) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[5];

  return val[0]* TMath::BesselJ0(DEG_TO_RAD*val[1]+TWO_PI*val[2]*tt)*
                 TMath::Exp(-val[3]*tt) +
         (1.0-val[0])*TMath::Exp(-val[4]*tt);
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::SkewedGauss(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: phase frequency sigma- sigma+ [tshift]

  double val[5];
  double skg;

  assert(fParamNo.size() <= 5);

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }


  double tt;
  if (fParamNo.size() == 4) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[4];

  // val[2] = sigma-, val[3] = sigma+
  double zp = fabs(val[3])*tt/SQRT_TWO; // sigma+
  double zm = fabs(val[2])*tt/SQRT_TWO; // sigma-
  double gp = TMath::Exp(-0.5*TMath::Power(tt*val[3], 2.0)); // gauss sigma+
  double gm = TMath::Exp(-0.5*TMath::Power(tt*val[2], 2.0)); // gauss sigma-
  double wp = fabs(val[3])/(fabs(val[2])+fabs(val[3])); // sigma+ / (sigma+ + sigma-)
  double wm = 1.0-wp;
  double phase = DEG_TO_RAD*val[0];
  double freq  = TWO_PI*val[1];

  if ((zp >= 25.0) || (zm >= 25.0)) // needed to prevent crash of 1F1
    skg = 2.0e300;
  else if (fabs(val[2]) == fabs(val[3])) // sigma+ == sigma- -> Gaussian
    skg = TMath::Cos(phase+freq*tt) * gp;
  else
    skg = TMath::Cos(phase+freq*tt) * (wm*gm + wp*gp) +
          TMath::Sin(phase+freq*tt) * (wm*gm*2.0*zm/SQRT_PI*ROOT::Math::conf_hyperg(0.5,1.5,zm*zm) -
                                       wp*gp*2.0*zp/SQRT_PI*ROOT::Math::conf_hyperg(0.5,1.5,zp*zp));

  return skg;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues tshift, p0, p1, ..., pn
 */
double PTheory::Polynom(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: tshift p0 p1 p2 ...

  double result = 0.0;
  double tshift = 0.0;
  double val;
  double expo = 0.0;

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val = paramValues[fParamNo[i]];
    } else { // function
      val = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
    if (i==0) { // tshift
      tshift = val;
      continue;
    }
    result += val*pow(t-tshift, expo);
    expo++;
  }

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in \f$(\mu\mathrm{s})\f$
 * \param paramValues
 */
double PTheory::UserFcn(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
/*
static bool first = true;
if (first) {
  cout << endl << ">> UserFcn: fParamNo.size()=" << fParamNo.size() << ", fUserParam.size()=" << fUserParam.size();
  for (unsigned int i=0; i<fParamNo.size(); i++) {
    cout << endl << ">> " << i << ": " << fParamNo[i]+1;
  }
  cout << endl;
}
*/

  // check if FUNCTIONS are used
  for (unsigned int i=0; i<fUserParam.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      fUserParam[i] = paramValues[fParamNo[i]];
    } else { // function
      fUserParam[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

/*
if (first) {
  first = false;
  for (unsigned int i=0; i<fUserParam.size(); i++) {
    cout << endl << "-> " << i << ": " << fUserParam[i];
  }
  cout << endl;
  cout << endl << ">> t=" << t << ", function value=" << (*fUserFcn)(t, fUserParam);
  cout << endl;
}
*/
  return (*fUserFcn)(t, fUserParam);
}

//--------------------------------------------------------------------------
/**
 * <p>The fLFIntegral is given in steps of 1 ns, e.g. index i=100 coresponds to t=100ns
 *
 * \param val parameters needed to calculate the non-analytic integral of the static Gauss LF function.
 */
void PTheory::CalculateGaussLFIntegral(const double *val) const
{
  // val[0] = nu, val[1] = Delta

  if (val[0] == 0.0) // field == 0.0, hence nothing to be done
    return;

  const double dt=0.001; // all times in usec
  double t, ft;
  double w0 = TMath::TwoPi()*val[0];
  double Delta = val[1];
  double preFactor = 2.0*TMath::Power(Delta, 4.0) / TMath::Power(w0, 3.0);

  // clear previously allocated vector
  fLFIntegral.clear();

  // calculate integral
  t  = 0.0;
  fLFIntegral.push_back(0.0); // start value of the integral

  ft = 0.0;
  for (unsigned int i=1; i<2e4; i++) {
    t  += dt;
    ft += 0.5*dt*preFactor*(TMath::Exp(-0.5*TMath::Power(Delta * (t-dt), 2.0))*TMath::Sin(w0*(t-dt))+
                           TMath::Exp(-0.5*TMath::Power(Delta * t, 2.0))*TMath::Sin(w0*t));
    fLFIntegral.push_back(ft);
  }
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param val parameters needed to calculate the non-analytic integral of the static Lorentz LF function.
 */
void PTheory::CalculateLorentzLFIntegral(const double *val) const
{
  // val[0] = nu, val[1] = a

  if (val[0] == 0.0) // field == 0.0, hence nothing to be done
    return;

  const double dt=0.001; // all times in usec
  double t, ft;
  double w0 = TMath::TwoPi()*val[0];
  double a = val[1];
  double preFactor = a*(1+pow(a/w0,2.0));

  // clear previously allocated vector
  fLFIntegral.clear();

  // calculate integral
  t  = 0.0;
  fLFIntegral.push_back(0.0); // start value of the integral

  ft = 0.0;
  // calculate first integral bin value (needed bcause of sin(x)/x x->0)
  t  += dt;
  ft += 0.5*dt*preFactor*(1.0+sin(w0*t)/(w0*t)*exp(-a*t));
  fLFIntegral.push_back(ft);
  // calculate all the other integral bin values
  for (unsigned int i=2; i<2e4; i++) {
    t  += dt;
    ft += 0.5*dt*preFactor*(sin(w0*(t-dt))/(w0*(t-dt))*exp(-a*(t-dt))+sin(w0*t)/(w0*t)*exp(-a*t));
    fLFIntegral.push_back(ft);
  }
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in (usec)
 */
double PTheory::GetLFIntegralValue(const double t) const
{
  unsigned int idx = static_cast<unsigned int>(t/0.001); // since LF-integral is calculated in nsec

  if (idx < 0)
    return 0.0;

  if (idx > fLFIntegral.size()-1)
    return fLFIntegral[fLFIntegral.size()-1];

  // liniarly interpolate between the two relvant function bins
  double df = (fLFIntegral[idx+1]-fLFIntegral[idx])/0.001*(t-idx*0.001);

  return fLFIntegral[idx]+df;
}

//--------------------------------------------------------------------------
/**
 * <p>Number of sampling points is estimated according to w0: w0 T = 2 pi
 * t_sampling = T/16 (i.e. 16 points on 1 period)
 * N = 8/pi w0 Tmax = 16 nu0 Tmax
 *
 * \param val parameters needed to solve the voltera integral equation
 * \param tag 0=Gauss, 1=Lorentz
 */
void PTheory::CalculateDynKTLF(const double *val, int tag) const
{
  // val: 0=nu0, 1=Delta (Gauss) / a (Lorentz), 2=nu
  const double Tmax = 20.0; // 20 usec
  unsigned int N = static_cast<unsigned int>(16.0*Tmax*val[0]);

  // check if rate (Delta/a) is very high
  if (fabs(val[1]) > 0.1) {
    double tmin = 20.0;
    switch (tag) {
      case 0: // Gauss
        tmin = sqrt(3.0)/val[1];
        break;
      case 1: // Lorentz
        tmin = 2.0/val[1];
        break;
      default:
        break;
    }
    unsigned int Nrate = static_cast<unsigned int>(25.0 * Tmax / tmin);
    if (Nrate > N) {
      N = Nrate;
    }
  }

  if (N < 300) // if too view points, i.e. nu0 very small, take 300 points
    N = 300;

  // allocate memory for dyn KT LF function vector
  fDynLFFuncValue.clear(); // get rid of a possible previous vector
  fDynLFFuncValue.resize(N);

  // calculate the non-analytic integral of the static KT LF function
  switch (tag) {
    case 0: // Gauss
      CalculateGaussLFIntegral(val);
      break;
    case 1: // Lorentz
      CalculateLorentzLFIntegral(val);
      break;
    default:
      cout << endl << "**FATAL ERROR** in PTheory::CalculateDynKTLF: You should never have reached this point." << endl;
      assert(false);
      break;
  }

  // calculate the P^(0)(t) exp(-nu t) vector
  PDoubleVector p0exp(N);
  double t = 0.0;
  double dt = Tmax/N;
  fDynLFdt = dt; // keep it since it is needed in GetDynKTLFValue()
  for (unsigned int i=0; i<N; i++) {
    switch (tag) {
      case 0: // Gauss
        if (val[0] < 0.02) { // if smaller 20kHz ~ 0.27G use zero field formula
          double sigma_t_2 = t*t*val[1]*val[1];
          p0exp[i] = 0.333333333333333 * (1.0 + 2.0*(1.0 - sigma_t_2)*TMath::Exp(-0.5*sigma_t_2));
        } else {
          double delta = val[1];
          double w0    = TWO_PI*val[0];

          p0exp[i] = 1.0 - 2.0*TMath::Power(delta/w0,2.0)*(1.0 -
                    TMath::Exp(-0.5*TMath::Power(delta*t, 2.0))*TMath::Cos(w0*t)) +
                    GetLFIntegralValue(t);
        }
        break;
      case 1: // Lorentz
        if (val[0] < 0.02) { // if smaller 20kHz ~ 0.27G use zero field formula
          double at = t*val[1];
          p0exp[i] = 0.333333333333333 * (1.0 + 2.0*(1.0 - at)*TMath::Exp(-at));
        } else {
          double a    = val[1];
          double at   = a*t;
          double w0   = TWO_PI*val[0];
          double a_w0 = a/w0;
          double w0t  = w0*t;

          double j1, j0;
          if (fabs(w0t) < 0.001) { // check zero time limits of the spherical bessel functions j0(x) and j1(x)
            j0 = 1.0;
            j1 = 0.0;
          } else {
            j0 = sin(w0t)/w0t;
            j1 = (sin(w0t)-w0t*cos(w0t))/(w0t*w0t);
          }

          p0exp[i] = 1.0 - a_w0*j1*exp(-at) - a_w0*a_w0*(j0*exp(-at) - 1.0) - GetLFIntegralValue(t);
        }
        break;
      default:
        break;
    }
    p0exp[i] *= TMath::Exp(-val[2]*t);
    t += dt;
  }

  // solve the volterra equation (trapezoid integration)
  fDynLFFuncValue[0]=p0exp[0];

  double sum;
  double a;
  double preFactor = dt*val[2];
  for (unsigned int i=1; i<N; i++) {
    sum = p0exp[i];
    sum += 0.5*preFactor*p0exp[i]*fDynLFFuncValue[0];
    for (unsigned int j=1; j<i; j++) {
      sum += preFactor*p0exp[i-j]*fDynLFFuncValue[j];
    }
    a = 1.0-0.5*preFactor*p0exp[0];

    fDynLFFuncValue[i]=sum/a;
  }
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param t time in (usec)
 */
double PTheory::GetDynKTLFValue(const double t) const
{
  unsigned int idx = static_cast<unsigned int>(t/fDynLFdt);

  if (idx < 0)
    return 0.0;

  if (idx > fDynLFFuncValue.size()-1)
    return fDynLFFuncValue[fDynLFFuncValue.size()-1];

  // liniarly interpolate between the two relvant function bins
  double df = (fDynLFFuncValue[idx+1]-fDynLFFuncValue[idx])*(t-idx*fDynLFdt)/fDynLFdt;

  return fDynLFFuncValue[idx]+df;
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
