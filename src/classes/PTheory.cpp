/***************************************************************************

  PTheory.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2016 by Andreas Suter                              *
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

#include <iostream>
#include <vector>
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

extern vector<void*> gGlobalUserFcn;

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p> Constructor.
 *
 * <p> The theory block his parsed and mapped to a tree. Every line (except the '+')
 * is a theory object by itself, i.e. the whole theory is build up recursivly.
 * Example:
 * \verbatim
    Theory block:
      a 1
      tf 2 3
      se 4
      +
      a 5
      tf 6 7
      se 8
      +
      a 9
      tf 10 11
 \endverbatim
 *
 * <p> This is mapped into the following binary tree:
 * \verbatim
                 a 1
               +/   \*
              a 5   tf 2 3
            + /  \*    \ *
            a 9  a 5   se 4
              \*   \*
         tf 10 11  tf 6 7
                    \*
                    se 8
 \endverbatim
 *
 * \param msrInfo msr-file handler
 * \param runNo msr-file run number
 * \param hasParent flag needed in order to know if PTheory is the root object or a child
 *               false (default) means this is the root object
 *               true means this is part of an already existing object tree
 */
PTheory::PTheory(PMsrHandler *msrInfo, UInt_t runNo, const Bool_t hasParent) : fMsrInfo(msrInfo)
{
  // init stuff
  fValid = true;
  fAdd = 0;
  fMul = 0;
  fUserFcnClassName = TString("");
  fUserFcn = 0;
  fDynLFdt = 0.0;
  fSamplingTime = 0.001; // default = 1ns (units in us)

  static UInt_t lineNo = 1; // lineNo
  static UInt_t depth  = 0; // needed to handle '+' properly

  if (hasParent == false) { // reset static counters if root object
    lineNo = 1; // the lineNo counter and the depth counter need to be
    depth  = 0; // reset for every root object (new run).
  }

  for (UInt_t i=0; i<THEORY_MAX_PARAM; i++)
    fPrevParam[i] = 0.0;

  // keep the number of user functions found up to this point
  fUserFcnIdx = GetUserFcnIdx(lineNo);

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
  Int_t index = str.Index("(");
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
    cerr << endl << ">> PTheory::PTheory: **SEVERE ERROR** Couldn't tokenize theory block line " << line->fLineNo << ".";
    cerr << endl << ">>  line content: " << line->fLine.Data();
    cerr << endl;
    exit(0);
  }
  ostr = dynamic_cast<TObjString*>(tokens->At(0));
  str = ostr->GetString();

  // search the theory function
  UInt_t idx = SearchDataBase(str);

  // function found is not defined
  if (idx == (UInt_t) THEORY_UNDEFINED) {
    cerr << endl << ">> PTheory::PTheory: **ERROR** Theory line '" << line->fLine.Data() << "'";
    cerr << endl << ">>  in line no " << line->fLineNo << " is undefined!";
    cerr << endl;
    fValid = false;
    return;
  }

  // line is a valid function, hence analyze parameters
  if (((UInt_t)(tokens->GetEntries()-1) < fNoOfParam) &&
      ((idx != THEORY_USER_FCN) && (idx != THEORY_POLYNOM))) {
    cerr << endl << ">> PTheory::PTheory: **ERROR** Theory line '" << line->fLine.Data() << "'";
    cerr << endl << ">>  in line no " << line->fLineNo;
    cerr << endl << ">>  expecting " << fgTheoDataBase[idx].fNoOfParam << ", but found " << tokens->GetEntries()-1;
    cerr << endl;
    fValid = false;
  }
  // keep function index
  fType = idx;
  // filter out the parameters
  Int_t status;
  UInt_t value;
  Bool_t ok = false;
  for (Int_t i=1; i<tokens->GetEntries(); i++) {
    ostr = dynamic_cast<TObjString*>(tokens->At(i));
    str = ostr->GetString();

    // if userFcn, the first entry is the function name and needs to be handled specially
    if ((fType == THEORY_USER_FCN) && ((i == 1) || (i == 2))) {
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
        PIntVector maps = *(*msrInfo->GetMsrRunList())[runNo].GetMap();
        if ((value <= maps.size()) && (value > 0)) { // everything fine
          fParamNo.push_back(maps[value-1]-1);
        } else { // map index out of range
          cerr << endl << ">> PTheory::PTheory: **ERROR** map index " << value << " out of range! See line no " << line->fLineNo;
          cerr << endl;
          fValid = false;
        }
      } else { // something wrong
        cerr << endl << ">> PTheory::PTheory: **ERROR**: map '" << str.Data() << "' not allowed. See line no " << line->fLineNo;
        cerr << endl;
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
        cerr << endl << ">> PTheory::PTheory: **ERROR** '" << str.Data() << "' not allowed. See line no " << line->fLineNo;
        cerr << endl;
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
        cerr << endl << ">> PTheory::PTheory: **ERROR** user function class '" << fUserFcnClassName.Data() << "' not found.";
        cerr << endl << ">>           Tried to load " << fUserFcnSharedLibName.Data() << " but failed.";
        cerr << endl << ">>           See line no " << line->fLineNo;
        cerr << endl;
        fValid = false;
        // clean up
        if (tokens) {
          delete tokens;
          tokens = 0;
        }
        return;
      } else if (!TClass::GetDict(fUserFcnClassName.Data())) {
        cerr << endl << ">> PTheory::PTheory: **ERROR** user function class '" << fUserFcnClassName.Data() << "' not found.";
        cerr << endl << ">>           " << fUserFcnSharedLibName.Data() << " loaded successfully, but no dictionary present.";
        cerr << endl << ">>           See line no " << line->fLineNo;
        cerr << endl;
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
    if (fUserFcn == 0) {
      cerr << endl << ">> PTheory::PTheory: **ERROR** user function object could not be invoked. See line no " << line->fLineNo;
      cerr << endl;
      fValid = false;
    } else { // user function valid, hence expand the fUserParam vector to the proper size
      fUserParam.resize(fParamNo.size());
    }

    // check if the global part of the user function is needed
    if (fUserFcn->NeedGlobalPart()) {
      fUserFcn->SetGlobalPart(gGlobalUserFcn, fUserFcnIdx); // invoke or retrieve global user function object
      if (!fUserFcn->GlobalPartIsValid()) {
        cerr << endl << ">> PTheory::PTheory: **ERROR** global user function object could not be invoked/retrived. See line no " << line->fLineNo;
        cerr << endl;
        fValid = false;
      }
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
 * <p>Destructor
 */
PTheory::~PTheory()
{
  fParamNo.clear();
  fUserParam.clear();

  fLFIntegral.clear();
  fDynLFFuncValue.clear();

  // recursive clean up
  CleanUp(this);

  if (fUserFcn) {
    delete fUserFcn;
    fUserFcn = 0;
  }

  gGlobalUserFcn.clear();
}

//--------------------------------------------------------------------------
// IsValid
//--------------------------------------------------------------------------
/**
 * <p>Checks if the theory tree is valid.
 *
 * <b>return:</b>
 * - true if valid
 * - false otherwise
 */
Bool_t PTheory::IsValid()
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
 * <p>Evaluates the theory tree.
 *
 * <b>return:</b>
 * - theory value for the given sets of parameters
 *
 * \param t time for single histogram, asymmetry, and mu-minus fits, x-axis value non-muSR fits
 * \param paramValues vector with the parameters
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::Func(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  if (fMul) {
    if (fAdd) { // fMul != 0 && fAdd != 0
      switch (fType) {
        case THEORY_CONST:
          return Constant(paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
              fAdd->Func(t, paramValues, funcValues);
          break;
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
        case THEORY_STR_KT:
          return StrKT(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
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
        case THEORY_TF_COS:
          return TFCos(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD:
          return InternalField(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD_KORNILOV:
          return InternalFieldGK(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
              fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD_LARKIN:
          return InternalFieldLL(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
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
        case THEORY_STATIC_ZF_NK:
          return StaticNKZF (t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_TF_NK:
          return StaticNKTF (t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_ZF_NK:
          return DynamicNKZF (t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
                 fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_TF_NK:
          return DynamicNKTF (t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues) +
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
          cerr << endl << ">> PTheory::Func: **PANIC ERROR** You never should have reached this line?!?! (" << fType << ")";
          cerr << endl;
          exit(0);
      }
    } else { // fMul !=0 && fAdd == 0
      switch (fType) {
        case THEORY_CONST:
          return Constant(paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
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
        case THEORY_STR_KT:
          return StrKT(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
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
        case THEORY_TF_COS:
          return TFCos(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD:
          return InternalField(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD_KORNILOV:
          return InternalFieldGK(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD_LARKIN:
          return InternalFieldLL(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
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
        case THEORY_STATIC_ZF_NK:
          return StaticNKZF (t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_TF_NK:
          return StaticNKTF (t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_ZF_NK:
          return DynamicNKZF (t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_TF_NK:
          return DynamicNKTF (t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_POLYNOM:
          return Polynom(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        case THEORY_USER_FCN:
          return UserFcn(t, paramValues, funcValues) * fMul->Func(t, paramValues, funcValues);
          break;
        default:
          cerr << endl << ">> PTheory::Func: **PANIC ERROR** You never should have reached this line?!?! (" << fType << ")";
          cerr << endl;
          exit(0);
      }
    }
  } else { // fMul == 0 && fAdd != 0
    if (fAdd) {
      switch (fType) {
        case THEORY_CONST:
          return Constant(paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
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
        case THEORY_STR_KT:
          return StrKT(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
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
        case THEORY_TF_COS:
          return TFCos(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD:
          return InternalField(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD_KORNILOV:
          return InternalFieldGK(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD_LARKIN:
          return InternalFieldLL(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
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
        case THEORY_STATIC_ZF_NK:
          return StaticNKZF (t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_TF_NK:
          return StaticNKTF (t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_ZF_NK:
          return DynamicNKZF (t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_TF_NK:
          return DynamicNKTF (t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_POLYNOM:
          return Polynom(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        case THEORY_USER_FCN:
          return UserFcn(t, paramValues, funcValues) + fAdd->Func(t, paramValues, funcValues);
          break;
        default:
          cerr << endl << ">> PTheory::Func: **PANIC ERROR** You never should have reached this line?!?! (" << fType << ")";
          cerr << endl;
          exit(0);
      }
    } else { // fMul == 0 && fAdd == 0
      switch (fType) {
        case THEORY_CONST:
          return Constant(paramValues, funcValues);
          break;
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
        case THEORY_STR_KT:
          return StrKT(t, paramValues, funcValues);
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
        case THEORY_TF_COS:
          return TFCos(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD:
          return InternalField(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD_KORNILOV:
          return InternalFieldGK(t, paramValues, funcValues);
          break;
        case THEORY_INTERNAL_FIELD_LARKIN:
          return InternalFieldLL(t, paramValues, funcValues);
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
        case THEORY_STATIC_ZF_NK:
          return StaticNKZF(t, paramValues, funcValues);
          break;
        case THEORY_STATIC_TF_NK:
          return StaticNKTF(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_ZF_NK:
          return DynamicNKZF(t, paramValues, funcValues);
          break;
        case THEORY_DYNAMIC_TF_NK:
          return DynamicNKTF(t, paramValues, funcValues);
          break;
        case THEORY_POLYNOM:
          return Polynom(t, paramValues, funcValues);
          break;
        case THEORY_USER_FCN:
          return UserFcn(t, paramValues, funcValues);
          break;
        default:
          cerr << endl << ">> PTheory::Func: **PANIC ERROR** You never should have reached this line?!?! (" << fType << ")";
          cerr << endl;
          exit(0);
      }
    }
  }

  return 0.0;
}

//--------------------------------------------------------------------------
/**
 * <p> Recursively clean up theory
 *
 * If data were a pointer to some data on the heap,
 * here, we would call delete on it. If it were a "composed" object,
 * its destructor would get called automatically after our own
 * destructor, so we would not have to worry about it.
 *
 * So all we have to clean up is the left and right subchild.
 * It turns out that we don't have to check for null pointers;
 * C++ automatically ignores a call to delete on a NULL pointer
 * (according to the man page, the same is true with malloc() in C)
 *
 * the COOLEST part is that if right is a non-null pointer,
 * the destructor gets called recursively!
 *
 * \param theo pointer to the theory object
 */
void PTheory::CleanUp(PTheory *theo)
{
  if (theo->fMul) { // '*' present
     delete theo->fMul;
     theo->fMul = 0;
  }

  if (theo->fAdd) {
     delete theo->fAdd;
     theo->fAdd = 0;
  }
}

//--------------------------------------------------------------------------
/**
 * <p>Searches the internal look up table for the theory name, and if found
 * set some internal variables to proper values.
 *
 * <b>return:</b>
 * - index of the theory function
 *
 * \param name theory name
 */
Int_t PTheory::SearchDataBase(TString name)
{
  Int_t idx = THEORY_UNDEFINED;

  for (UInt_t i=0; i<THEORY_MAX; i++) {
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
// GetUserFcnIdx (private)
//--------------------------------------------------------------------------
/**
 * <p>Counts the number of user functions in the theory block up to lineNo.
 *
 * <b>return:</b> to number of user functions found up to lineNo
 *
 * \param lineNo current line number in the theory block
 */
Int_t PTheory::GetUserFcnIdx(UInt_t lineNo) const
{
  Int_t userFcnIdx = -1;

  // retrieve the theory block from the msr-file handler
  PMsrLines *fullTheoryBlock = fMsrInfo->GetMsrTheory();

  // make sure that lineNo is within proper bounds
  if (lineNo > fullTheoryBlock->size())
    return userFcnIdx;

  // count the number of user function present up to the lineNo
  for (UInt_t i=1; i<=lineNo; i++) {
    if (fullTheoryBlock->at(i).fLine.Contains("userFcn", TString::kIgnoreCase))
      userFcnIdx++;
  }

  return userFcnIdx;
}

//--------------------------------------------------------------------------
// MakeCleanAndTidyTheoryBlock private
//--------------------------------------------------------------------------
/**
 * <p>Takes the theory block form the msr-file, and makes a nicely formated
 * theory block.
 *
 * \param fullTheoryBlock msr-file text lines of the theory block
 */
void PTheory::MakeCleanAndTidyTheoryBlock(PMsrLines *fullTheoryBlock)
{
  PMsrLineStructure *line;
  TString str, tidy;
  Char_t substr[256];
  TObjArray *tokens = 0;
  TObjString *ostr = 0;
  Int_t idx = THEORY_UNDEFINED;

  for (UInt_t i=1; i<fullTheoryBlock->size(); i++) {
    // get the line to be prettyfied
    line = &(*fullTheoryBlock)[i];
    // copy line content to str in order to remove comments
    str = line->fLine.Copy();
    // remove theory line comment if present, i.e. something starting with '('
    Int_t index = str.Index("(");
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
    for (UInt_t j=0; j<THEORY_MAX; j++) {
      if (!str.CompareTo(fgTheoDataBase[j].fName, TString::kIgnoreCase) ||
          !str.CompareTo(fgTheoDataBase[j].fAbbrev, TString::kIgnoreCase)) {
        idx = fgTheoDataBase[j].fType;
      }
    }
    // check if theory is indeed defined. This should not be necessay at this point but ...
    if (idx == THEORY_UNDEFINED)
      return;
    // check that there enough tokens. This should not be necessay at this point but ...
    if ((UInt_t)tokens->GetEntries() < fgTheoDataBase[idx].fNoOfParam + 1)
      return;
    // make tidy string
    sprintf(substr, "%-10s", fgTheoDataBase[idx].fName.Data());
    tidy = TString(substr);
    for (UInt_t j=1; j<(UInt_t)tokens->GetEntries(); j++) {
      ostr = dynamic_cast<TObjString*>(tokens->At(j));
      str = ostr->GetString();
      sprintf(substr, "%6s", str.Data());
      tidy += TString(substr);
    }
    if (fgTheoDataBase[idx].fComment.Length() != 0) {
      if (tidy.Length() < 35) {
        for (UInt_t k=0; k<35-(UInt_t)tidy.Length(); k++)
          tidy += TString(" ");
      } else {
        tidy += TString(" ");
      }
      if ((UInt_t)tokens->GetEntries() == fgTheoDataBase[idx].fNoOfParam + 1) // no tshift
        tidy += fgTheoDataBase[idx].fComment;
      else
        tidy += fgTheoDataBase[idx].fCommentTimeShift;
    }
    // write tidy string back into theory block
    (*fullTheoryBlock)[i].fLine = tidy;

    // clean up
    if (tokens) {
      delete tokens;
      tokens = 0;
    }
  }

}

//--------------------------------------------------------------------------
// MakeCleanAndTidyPolynom private
//--------------------------------------------------------------------------
/**
 * <p>Prettifies a polynom theory line.
 *
 * \param i line index of the theory polynom line to be prettified
 * \param fullTheoryBlock msr-file text lines of the theory block
 */
void PTheory::MakeCleanAndTidyPolynom(UInt_t i, PMsrLines *fullTheoryBlock)
{
  PMsrLineStructure *line;
  TString str, tidy;
  TObjArray *tokens = 0;
  TObjString *ostr;
  Char_t substr[256];

  // init tidy
  tidy = TString("polynom ");
  // get the line to be prettyfied
  line = &(*fullTheoryBlock)[i];
  // copy line content to str in order to remove comments
  str = line->fLine.Copy();
  // tokenize line
  tokens = str.Tokenize(" \t");

  // check if comment is already present, and if yes ignore it by setting max correctly
  UInt_t max = (UInt_t)tokens->GetEntries();
  for (UInt_t j=1; j<max; j++) {
    ostr = dynamic_cast<TObjString*>(tokens->At(j));
    str = ostr->GetString();
    if (str.Contains("(")) { // comment present
      max=j;
      break;
    }
  }

  for (UInt_t j=1; j<max; j++) {
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
 * <p>Prettifies a user function theory line.
 *
 * \param i line index of the user function line to be prettified
 * \param fullTheoryBlock msr-file text lines of the theory block
 */
void PTheory::MakeCleanAndTidyUserFcn(UInt_t i, PMsrLines *fullTheoryBlock)
{
  PMsrLineStructure *line;
  TString str, tidy;
  TObjArray *tokens = 0;
  TObjString *ostr;

  // init tidy
  tidy = TString("userFcn ");
  // get the line to be prettyfied
  line = &(*fullTheoryBlock)[i];
  // copy line content to str in order to remove comments
  str = line->fLine.Copy();
  // tokenize line
  tokens = str.Tokenize(" \t");

  for (UInt_t j=1; j<(UInt_t)tokens->GetEntries(); j++) {
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
 * <p> theory function: Const
 * \f[ = const \f]
 *
 * <b>meaning of paramValues:</b> const
 *
 * <b>return:</b> function value
 *
 * \param paramValues vector with the parameters
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::Constant(const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: const

  Double_t constant;

  // check if FUNCTIONS are used
  if (fParamNo[0] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
    constant = paramValues[fParamNo[0]];
  } else {
    constant = funcValues[fParamNo[0]-MSR_PARAM_FUN_OFFSET];
  }

  return constant;
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: Asymmetry
 * \f[ = A \f]
 *
 * <b>meaning of paramValues:</b> \f$A\f$
 *
 * <b>return:</b> function value
 *
 * \param paramValues vector with the parameters
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::Asymmetry(const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: asym

  Double_t asym;

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
 * <p> theory function: simple exponential
 * \f[ = \exp\left(-\lambda t\right) \f] or
 * \f[ = \exp\left(-\lambda [t-t_{\rm shift}] \right) \f]
 *
 * <b>meaning of paramValues:</b> \f$\lambda\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::SimpleExp(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: lambda [tshift]

  Double_t val[2];

  assert(fParamNo.size() <= 2);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 1) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[1];

  return TMath::Exp(-tt*val[0]);
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: generalized exponential
 * \f[ = \exp\left(-[\lambda t]^\beta\right) \f] or
 * \f[ = \exp\left(-[\lambda (t-t_{\rm shift})]^\beta\right) \f]
 *
 * <b>meaning of paramValues:</b> \f$\lambda\f$, \f$\beta\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::GeneralExp(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: lambda beta [tshift]

  Double_t val[3];
  Double_t result;

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
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
 * <p> theory function: simple Gaussian
 * \f[ = \exp\left(-1/2 [\sigma t]^2\right) \f] or
 * \f[ = \exp\left(-1/2 [\sigma (t-t_{\rm shift})]^2\right) \f]
 *
 * <b>meaning of paramValues:</b> \f$\sigma\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::SimpleGauss(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: sigma [tshift]

  Double_t val[2];

  assert(fParamNo.size() <= 2);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 1) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[1];

  return TMath::Exp(-0.5*TMath::Power(tt*val[0], 2.0));
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: static Gaussian Kubo-Toyabe in zero applied field
 * \f[ = \frac{1}{3} + \frac{2}{3} \left[1-(\sigma t)^2\right] \exp\left[-1/2 (\sigma t)^2\right] \f] or
 * \f[ = \frac{1}{3} + \frac{2}{3} \left[1-(\sigma \{t-t_{\rm shift}\})^2\right] \exp\left[-1/2 (\sigma \{t-t_{\rm shift}\})^2\right] \f]
 *
 * <b>meaning of paramValues:</b> \f$\sigma\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::StaticGaussKT(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: sigma [tshift]

  Double_t val[2];

  assert(fParamNo.size() <= 2);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t sigma_t_2;
  if (fParamNo.size() == 1) // no tshift
    sigma_t_2 = t*t*val[0]*val[0];
  else // tshift present
    sigma_t_2 = (t-val[1])*(t-val[1])*val[0]*val[0];

  return 0.333333333333333 * (1.0 + 2.0*(1.0 - sigma_t_2)*TMath::Exp(-0.5*sigma_t_2));
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: static Gaussian Kubo-Toyabe in longitudinal applied field
 *
 * \f[ = 1-\frac{2\sigma^2}{(2\pi\nu)^2}\left[1-\exp\left(-1/2 \{\sigma t\}^2\right)\cos(2\pi\nu t)\right] +
       \frac{2\sigma^4}{(2\pi\nu)^3}\int^t_0 \exp\left(-1/2 \{\sigma \tau\}^2\right)\sin(2\pi\nu\tau)\mathrm{d}\tau \f]
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$\sigma\f$, \f$\nu\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::StaticGaussKTLF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{

  // expected parameters: frequency damping [tshift]

  Double_t val[3];
  Double_t result;

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  // check if all parameters == 0
  if ((val[0] == 0.0) && (val[1] == 0.0))
    return 1.0;

  // check if the parameter values have changed, and if yes recalculate the non-analytic integral
  // check only the first two parameters since the tshift is irrelevant for the LF-integral calculation!!
  Bool_t newParam = false;
  for (UInt_t i=0; i<2; i++) {
    if (val[i] != fPrevParam[i]) {
      newParam = true;
      break;
    }
  }

  if (newParam) { // new parameters found
    {
      for (UInt_t i=0; i<2; i++)
        fPrevParam[i] = val[i];
      CalculateGaussLFIntegral(val);
    }
  }

  Double_t tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  if (tt < 0.0) // for times < 0 return a function value of 1.0
    return 1.0;

  Double_t sigma_t_2 = 0.0;
  if (val[0] < 0.02) { // if smaller 20kHz ~ 0.27G use the ZF formula
    sigma_t_2 = tt*tt*val[1]*val[1];
    result = 0.333333333333333 * (1.0 + 2.0*(1.0 - sigma_t_2)*TMath::Exp(-0.5*sigma_t_2));
  } else if (val[1]/val[0] > 79.5775) { // check if Delta/w0 > 500.0, in which case the ZF formula is used
    sigma_t_2 = tt*tt*val[1]*val[1];
    result = 0.333333333333333 * (1.0 + 2.0*(1.0 - sigma_t_2)*TMath::Exp(-0.5*sigma_t_2));
  } else {
    Double_t delta = val[1];
    Double_t w0    = 2.0*TMath::Pi()*val[0];

    result = 1.0 - 2.0*TMath::Power(delta/w0,2.0)*(1.0 - 
                TMath::Exp(-0.5*TMath::Power(delta*tt, 2.0))*TMath::Cos(w0*tt)) +
                GetLFIntegralValue(tt);
  }

  return result;

}

//--------------------------------------------------------------------------
/**
 * <p> theory function: dynamic Gaussian Kubo-Toyabe in longitudinal applied field
 *
 * \f[ = \frac{1}{2\pi \imath}\int_{\gamma-\imath\infty}^{\gamma+\imath\infty}
 *       \frac{f_{\mathrm{G}}(s+\Gamma)}{1-\Gamma f_{\mathrm{G}}(s+\Gamma)} \exp(s t) \mathrm{d}s,
 *       \mathrm{~where~}\,f_{\mathrm{G}}(s)\equiv \int_0^{\infty}G_{\mathrm{G,LF}}(t)\exp(-s t) \mathrm{d}t\f]
 * or the time shifted version of it. \f$G_{\mathrm{G,LF}}(t)\f$ is the static Gaussian Kubo-Toyabe in longitudinal applied field.
 *
 * <p>The current implementation is not realized via the above formulas, but ...
 *
 * <b>meaning of paramValues:</b> \f$\sigma\f$, \f$\nu\f$, \f$\Gamma\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::DynamicGaussKTLF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: frequency damping hopping [tshift]

  Double_t val[4];
  Double_t result = 0.0;
  Bool_t useKeren = false;

  assert(fParamNo.size() <= 4);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  // check if all parameters == 0
  if ((val[0] == 0.0) && (val[1] == 0.0) && (val[2] == 0.0))
    return 1.0;

  // make sure that damping and hopping are positive definite
  if (val[1] < 0.0)
    val[1] = -val[1];
  if (val[2] < 0.0)
    val[2] = -val[2];

  // check that Delta != 0, if not (i.e. stupid parameter) return 1, which is the correct limit
  if (fabs(val[1]) < 1.0e-6) {
    return 1.0;
  }

  // check if Keren approximation can be used
  if (val[2]/val[1] > 5.0) // nu/Delta > 5.0
    useKeren = true;

  if (!useKeren) {
    // check if the parameter values have changed, and if yes recalculate the non-analytic integral
    // check only the first three parameters since the tshift is irrelevant for the LF-integral calculation!!
    Bool_t newParam = false;
    for (UInt_t i=0; i<3; i++) {
      if (val[i] != fPrevParam[i]) {
        newParam = true;
        break;
      }
    }

    if (newParam) { // new parameters found
      for (UInt_t i=0; i<3; i++)
        fPrevParam[i] = val[i];
      CalculateDynKTLF(val, 0); // 0 means Gauss
    }
  }

  Double_t tt;
  if (fParamNo.size() == 3) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[3];

  if (tt < 0.0) // for times < 0 return a function value of 0.0
    return 0.0;


  if (useKeren) { // see PRB50, 10039 (1994)
    Double_t wL  = TWO_PI * val[0];
    Double_t wL2 = wL*wL;
    Double_t nu2 = val[2]*val[2];
    Double_t Gamma_t = 2.0*val[1]*val[1]/((wL2+nu2)*(wL2+nu2))*
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
 * <p> theory function: static Lorentzain Kubo-Toyabe in zero applied field (see Uemura et al. PRB 31, 546 (85)).
 *
 * \f[ = 1/3 + 2/3 [1 - \lambda t] \exp(-\lambda t) \f]
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$\lambda\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::StaticLorentzKT(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: lambda [tshift]

  Double_t val[2];

  assert(fParamNo.size() <= 2);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t a_t;
  if (fParamNo.size() == 1) // no tshift
    a_t = t*val[0];
  else // tshift present
    a_t = (t-val[1])*val[0];

  return 0.333333333333333 * (1.0 + 2.0*(1.0 - a_t)*TMath::Exp(-a_t));
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: static Lorentzain Kubo-Toyabe in longitudinal applied field (see Uemura et al. PRB 31, 546 (85)).
 *
 * \f[ = 1-\frac{a}{2\pi\nu}j_1(2\pi\nu t)\exp\left(-at\right)-
 *       \left(\frac{a}{2\pi\nu}\right)^2 \left[j_0(2\pi\nu t)\exp\left(-at\right)-1\right]-
 *       a\left[1+\left(\frac{a}{2\pi\nu}\right)^2\right]\int^t_0 \exp\left(-a\tau\right)j_0(2\pi\nu\tau)\mathrm{d}\tau) \f]
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$a\f$, \f$\nu\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::StaticLorentzKTLF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: frequency damping [tshift]

  Double_t val[3];
  Double_t result;

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  // check if all parameters == 0
  if ((val[0] == 0.0) && (val[1] == 0.0))
    return 1.0;

  // check if the parameter values have changed, and if yes recalculate the non-analytic integral
  // check only the first two parameters since the tshift is irrelevant for the LF-integral calculation!!
  Bool_t newParam = false;
  for (UInt_t i=0; i<2; i++) {
    if (val[i] != fPrevParam[i]) {
      newParam = true;
      break;
    }
  }

  if (newParam) { // new parameters found
    for (UInt_t i=0; i<2; i++)
      fPrevParam[i] = val[i];
    CalculateLorentzLFIntegral(val);
  }

  Double_t tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  if (tt < 0.0) // for times < 0 return a function value of 1.0
    return 1.0;

 if (val[0] < 0.02) { // if smaller 20kHz ~ 0.27G use the ZF formula
    Double_t at = tt*val[1];
    result = 0.333333333333333 * (1.0 + 2.0*(1.0 - at)*TMath::Exp(-at));
  } else if (val[1]/val[0] > 159.1549) { // check if a/w0 > 1000.0, in which case the ZF formula is used
    Double_t at = tt*val[1];
    result = 0.333333333333333 * (1.0 + 2.0*(1.0 - at)*TMath::Exp(-at));
  } else {
    Double_t a    = val[1];
    Double_t at   = a*tt;
    Double_t w0   = 2.0*TMath::Pi()*val[0];
    Double_t a_w0 = a/w0;
    Double_t w0t  = w0*tt;

    Double_t j1, j0;
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
 * <p> theory function: dynamic Lorentzain Kubo-Toyabe in longitudinal applied field
 * (see R. S. Hayano et al., Phys. Rev. B 20 (1979) 850; P. Dalmas de Reotier and A. Yaouanc,
 * J. Phys.: Condens. Matter 4 (1992) 4533; A. Keren, Phys. Rev. B 50 (1994) 10039).
 *
 * \f[ =  \frac{1}{2\pi \imath}\int_{\gamma-\imath\infty}^{\gamma+\imath\infty}
 * \frac{f_{\mathrm{L}}(s+\Gamma)}{1-\Gamma f_{\mathrm{L}}(s+\Gamma)} \exp(s t) \mathrm{d}s,
 * \mathrm{~where~}\,f_{\mathrm{L}}(s)\equiv \int_0^{\infty}G_{\mathrm{L,LF}}(t)\exp(-s t) \mathrm{d}t\f]
 * or the time shifted version of it. \f$G_{\mathrm{L,LF}}(t)\f$ is the static Lorentzain Kubo-Toyabe function in longitudinal field
 *
 * <p>The current implementation is not realized via the above formulas, but ...
 *
 * <b>meaning of paramValues:</b> \f$a\f$, \f$\nu\f$, \f$\Gamma\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::DynamicLorentzKTLF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: frequency damping hopping [tshift]

  Double_t val[4];
  Double_t result = 0.0;

  assert(fParamNo.size() <= 4);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  // check if all parameters == 0
  if ((val[0] == 0.0) && (val[1] == 0.0) && (val[2] == 0.0))
    return 1.0;

  // make sure that damping and hopping are positive definite
  if (val[1] < 0.0)
    val[1] = -val[1];
  if (val[2] < 0.0)
    val[2] = -val[2];


  Double_t tt;
  if (fParamNo.size() == 3) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[3];

  if (tt < 0.0) // for times < 0 return a function value of 1.0
    return 1.0;

  // check if hopping > 5 * damping, of Larmor angular frequency is > 30 * damping (BMW limit)
  Double_t w0 = 2.0*TMath::Pi()*val[0];
  Double_t a  = val[1];
  Double_t nu = val[2];
  if ((nu > 5.0 * a) || (w0 >= 30.0 * a)) {
    // 'c' and 'd' are parameters BMW obtained by fitting large parameter space LF-curves to the model below
    const Double_t c[7] = {1.15331, 1.64826, -0.71763, 3.0, 0.386683, -5.01876, 2.41854};
    const Double_t d[4] = {2.44056, 2.92063, 1.69581, 0.667277};
    Double_t w0N[4];
    Double_t nuN[4];
    w0N[0] = w0;
    nuN[0] = nu;
    for (UInt_t i=1; i<4; i++) {
      w0N[i] = w0 * w0N[i-1];
      nuN[i] = nu * nuN[i-1];
    }
    Double_t denom = w0N[3]+d[0]*w0N[2]*nuN[0]+d[1]*w0N[1]*nuN[1]+d[2]*w0N[0]*nuN[2]+d[3]*nuN[3];
    Double_t b1 = (c[0]*w0N[2]+c[1]*w0N[1]*nuN[0]+c[2]*w0N[0]*nuN[1])/denom;
    Double_t b2 = (c[3]*w0N[2]+c[4]*w0N[1]*nuN[0]+c[5]*w0N[0]*nuN[1]+c[6]*nuN[2])/denom;

    Double_t w0t = w0*tt;
    Double_t j1, j0;
    if (fabs(w0t) < 0.001) { // check zero time limits of the spherical bessel functions j0(x) and j1(x)
      j0 = 1.0;
      j1 = 0.0;
    } else {
      j0 = sin(w0t)/w0t;
      j1 = (sin(w0t)-w0t*cos(w0t))/(w0t*w0t);
    }

    Double_t Gamma_t = -4.0/3.0*a*(b1*(1.0-j0*TMath::Exp(-nu*tt))+b2*j1*TMath::Exp(-nu*tt)+(1.0-b2*w0/3.0-b1*nu)*tt);

    return TMath::Exp(Gamma_t);
  }

  // check if the parameter values have changed, and if yes recalculate the non-analytic integral
  // check only the first three parameters since the tshift is irrelevant for the LF-integral calculation!!
  Bool_t newParam = false;
  for (UInt_t i=0; i<3; i++) {
    if (val[i] != fPrevParam[i]) {
      newParam = true;
      break;
    }
  }

  if (newParam) { // new parameters found
    for (UInt_t i=0; i<3; i++)
      fPrevParam[i] = val[i];
    CalculateDynKTLF(val, 1); // 1 means Lorentz
  }

  result = GetDynKTLFValue(tt);

  return result;

}

//--------------------------------------------------------------------------
/**
 * <p> theory function: dynamic Lorentzain Kubo-Toyabe in longitudinal applied field
 *
 * \f[ =  1/3 + 2/3 \left(1-(\sigma t)^2-\lambda t\right) \exp\left(-1/2(\sigma t)^2-\lambda t\right)\f]
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$\lambda\f$, \f$\sigma\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::CombiLGKT(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: lambdaL lambdaG [tshift]

  Double_t val[3];

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  Double_t lambdaL_t   = tt*val[0];
  Double_t lambdaG_t_2 = tt*tt*val[1]*val[1];

  return 0.333333333333333 * 
         (1.0 + 2.0*(1.0-lambdaL_t-lambdaG_t_2)*TMath::Exp(-(lambdaL_t+0.5*lambdaG_t_2)));
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: stretched Kubo-Toyabe in zero field.
 * Ref: Crook M. R. and Cywinski R., J. Phys. Condens. Matter, 9 (1997) 1149.
 *
 * \f[ =  1/3 + 2/3 \left(1-(\sigma t)^\beta \right) \exp\left(-(\sigma t)^\beta / \beta\right)\f]
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$\sigma\f$, \f$\beta\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::StrKT(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: sigma beta [tshift]

  Double_t val[3];

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  // check for beta too small (beta < 0.1) in which case numerical problems could arise and the function is anyhow
  // almost identical to a constant of 1/3.
  if (val[1] < 0.1)
    return 0.333333333333333;

  Double_t tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  Double_t sigma_t   = TMath::Power(tt*val[0],val[1]);

  return 0.333333333333333 * 
    (1.0 + 2.0*(1.0-sigma_t)*TMath::Exp(-sigma_t/val[1]));
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: spin glass function
 *
 * \f[ = \frac{1}{3}\exp\left(-\sqrt{\frac{4\lambda^2(1-q)t}{\gamma}}\right)+\frac{2}{3}\left(1-\frac{q\lambda^2t^2}{\sqrt{\frac{4\lambda^2(1-q)t}{\gamma}+q\lambda^2t^2}}\right)\exp\left(-\sqrt{\frac{4\lambda^2(1-q)t}{\gamma}+q\lambda^2t^2}\right)\f]
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$\lambda\f$, \f$\gamma\f$, \f$q\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::SpinGlass(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: lambda gamma q [tshift]

  if (paramValues[fParamNo[0]] == 0.0)
    return 1.0;

  Double_t val[4];

  assert(fParamNo.size() <= 4);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 3) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[3];

  Double_t lambda_2     = val[0]*val[0];
  Double_t lambda_t_2_q = tt*tt*lambda_2*val[2];
  Double_t rate_2       = 4.0*lambda_2*(1.0-val[2])*tt/val[1];

  Double_t rateL = TMath::Sqrt(fabs(rate_2));
  Double_t rateT = TMath::Sqrt(fabs(rate_2)+lambda_t_2_q);

  return 0.333333333333333*(TMath::Exp(-rateL) + 2.0*(1.0-lambda_t_2_q/rateT)*TMath::Exp(-rateT));
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: random anisotropic hyperfine function (see  R. E. Turner and D. R. Harshman, Phys. Rev. B 34 (1986) 4467)
 *
 * \f[ = \frac{1}{6}\left(1-\frac{\nu t}{2}\right)\exp\left(-\frac{\nu t}{2}\right)+\frac{1}{3}\left(1-\frac{\nu t}{4}\right)\exp\left(-\frac{\nu t + 2.44949\lambda t}{4}\right)\f]
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$\nu\f$, \f$\lambda\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::RandomAnisotropicHyperfine(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: nu lambda [tshift]

  Double_t val[3];

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  Double_t nu_t     = tt*val[0];
  Double_t lambda_t = tt*val[1];

  return 0.166666666666667*(1.0-0.5*nu_t)*TMath::Exp(-0.5*nu_t) +
         0.333333333333333*(1.0-0.25*nu_t)*TMath::Exp(-0.25*(nu_t+2.44949*lambda_t));
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: Abragam function
 *
 * \f[ = \exp\left[-\frac{\sigma^2}{\gamma^2}\left(e^{-\gamma t}-1+\gamma t\right)\right] \f]
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$\sigma\f$, \f$\gamma\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::Abragam(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: sigma gamma [tshift]

  Double_t val[3];

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  Double_t gamma_t = tt*val[1];

  return TMath::Exp(-TMath::Power(val[0]/val[1],2.0)*
                    (TMath::Exp(-gamma_t)-1.0+gamma_t));
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: cosine including phase
 *
 * \f[ = \cos(2\pi\nu t + \varphi) \f]
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$\nu\f$, \f$\varphi\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::TFCos(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: phase frequency [tshift]

  Double_t val[3];

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  return TMath::Cos(DEG_TO_RAD*val[0]+TWO_PI*val[1]*tt);
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: internal field function
 *
 * \f[ = \alpha\,\cos\left(2\pi\nu t+\frac{\pi\varphi}{180}\right)\exp\left(-\lambda_{\mathrm{T}}t\right)+(1-\alpha)\,\exp\left(-\lambda_{\mathrm{L}}t\right)\f]
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$\alpha\f$, \f$\varphi\f$, \f$\nu\f$, \f$\lambda_{\rm T}\f$, \f$\lambda_{\rm L}\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::InternalField(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: fraction phase frequency rateT rateL [tshift]

  Double_t val[6];

  assert(fParamNo.size() <= 6);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 5) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[5];

  return val[0]*TMath::Cos(DEG_TO_RAD*val[1]+TWO_PI*val[2]*tt)*TMath::Exp(-val[3]*tt) +
         (1-val[0])*TMath::Exp(-val[4]*tt);
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: internal field function Gauss-Kornilov (see Physics Letters A <b>153</b>, 364 (1991)).
 *
 * \f[ = \alpha\,\left[\cos(2\pi\nu t)-\frac{\sigma^2 t}{2\pi\nu}\sin(2\pi\nu t)\right]\exp(-[\sigma t]^2/2)+(1-\alpha)\,\exp(-(\lambda t)^\beta)\f]
 * or the time shifted version of it. For the powder averaged case, \f$\alpha=2/3\f$.
 *
 * <b>meaning of paramValues:</b> \f$\alpha\f$, \f$\nu\f$, \f$\sigma\f$, \f$\lambda\f$, \f$\beta\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::InternalFieldGK(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: [0]:fraction [1]:frequency [2]:sigma [3]:lambda [4]:beta [[5]:tshift]

  Double_t val[6];

  assert(fParamNo.size() <= 6);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 5) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[5];

  Double_t result = 0.0;
  Double_t nu_t = val[1]*tt;
  Double_t rateLF = TMath::Power(val[3]*tt, val[4]);
  Double_t rate2 = val[2]*val[2]*tt*tt; // (sigma t)^2

  if (val[1] < 0.01) { // internal field frequency is approaching zero
    result = (1.0-val[0])*TMath::Exp(-rateLF) + val[0]*(1.0-rate2)*TMath::Exp(-0.5*rate2);
  } else {
    result = (1.0-val[0])*TMath::Exp(-rateLF) + val[0]*(TMath::Cos(nu_t)-val[2]*val[2]*tt/val[1]*TMath::Sin(nu_t))*TMath::Exp(-0.5*rate2);
  }

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: internal field function Lorentz-Larkin (see Physica B: Condensed Matter <b>289-290</b>, 153 (2000)).
 *
 * \f[ = \alpha\,\left[\cos(2\pi\nu t)-\frac{a}{2\pi\nu}\sin(2\pi\nu t)\right]\exp(-a t)+(1-\alpha)\,\exp(-(\lambda t)^\beta)\f]
 * or the time shifted version of it. For the powder averaged case, \f$\alpha=2/3\f$.
 *
 * <b>meaning of paramValues:</b> \f$\alpha\f$, \f$\nu\f$, \f$a\f$, \f$\lambda\f$, \f$\beta\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::InternalFieldLL(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: [0]:fraction [1]:frequency [2]:a [3]:lambda [4]:beta [[5]:tshift]

  Double_t val[6];

  assert(fParamNo.size() <= 6);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 5) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[5];

  Double_t result = 0.0;
  Double_t nu_t = val[1]*tt;
  Double_t rateLF = TMath::Power(val[3]*tt, val[4]);
  Double_t a_t = val[2]*tt; // a t

  if (val[1] < 0.01) { // internal field frequency is approaching zero
    result = (1.0-val[0])*TMath::Exp(-rateLF) + val[0]*(1.0-a_t)*TMath::Exp(-a_t);
  } else {
    result = (1.0-val[0])*TMath::Exp(-rateLF) + val[0]*(TMath::Cos(nu_t)-val[3]/val[1]*TMath::Sin(nu_t))*TMath::Exp(-a_t);
  }

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: spherical bessel function including phase
 *
 * \f[ = j_0(2\pi\nu t + \varphi) \f]
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$\nu\f$, \f$\varphi\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::Bessel(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: phase frequency [tshift]

  Double_t val[3];

  assert(fParamNo.size() <= 3);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  return TMath::BesselJ0(DEG_TO_RAD*val[0]+TWO_PI*val[1]*tt);
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: internal field bessel function
 *
 * \f[ = \alpha\,j_0\left(2\pi\nu t+\frac{\pi\varphi}{180}\right)\exp\left(-\lambda_{\mathrm{T}}t\right)+(1-\alpha)\,\exp\left(-\lambda_{\mathrm{L}}t\right)\f]
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$\alpha\f$, \f$\varphi\f$, \f$\nu\f$, \f$\lambda_{\rm T}\f$, \f$\lambda_{\rm L}\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::InternalBessel(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: fraction phase frequency rateT rateL [tshift]

  Double_t val[6];

  assert(fParamNo.size() <= 6);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
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
 * <p> theory function: skewed Gaussian function
 *
 * \f{eqnarray*}{ &=& \frac{\sigma_{-}}{\sigma_{+}+\sigma_{-}}\exp\left[-\frac{\sigma_{-}^2t^2}{2}\right]
 * \left\lbrace\cos\left(2\pi\nu t+\frac{\pi\varphi}{180}\right)+
 * \sin\left(2\pi\nu t+\frac{\pi\varphi}{180}\right)\mathrm{Erfi}\left(\frac{\sigma_{-}t}{\sqrt{2}}\right)\right\rbrace+\\
 * & & \frac{\sigma_{+}}{\sigma_{+}+\sigma_{-}}
 * \exp\left[-\frac{\sigma_{+}^2t^2}{2}\right]\left\lbrace\cos\left(2\pi\nu t+\frac{\pi\varphi}{180}\right)-
 * \sin\left(2\pi\nu t+\frac{\pi\varphi}{180}\right)\mathrm{Erfi}\left(\frac{\sigma_{+}t}{\sqrt{2}}\right)\right\rbrace
 * \f}
 * or the time shifted version of it.
 *
 * <b>meaning of paramValues:</b> \f$\varphi\f$, \f$\nu\f$, \f$\sigma_{-}\f$, \f$\sigma_{+}\f$ [, \f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::SkewedGauss(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: phase frequency sigma- sigma+ [tshift]

  Double_t val[5];
  Double_t skg;

  assert(fParamNo.size() <= 5);

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }


  Double_t tt;
  if (fParamNo.size() == 4) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[4];

  // val[2] = sigma-, val[3] = sigma+
  Double_t zp = fabs(val[3])*tt/SQRT_TWO; // sigma+
  Double_t zm = fabs(val[2])*tt/SQRT_TWO; // sigma-
  Double_t gp = TMath::Exp(-0.5*TMath::Power(tt*val[3], 2.0)); // gauss sigma+
  Double_t gm = TMath::Exp(-0.5*TMath::Power(tt*val[2], 2.0)); // gauss sigma-
  Double_t wp = fabs(val[3])/(fabs(val[2])+fabs(val[3])); // sigma+ / (sigma+ + sigma-)
  Double_t wm = 1.0-wp;
  Double_t phase = DEG_TO_RAD*val[0];
  Double_t freq  = TWO_PI*val[1];

  if ((zp >= 25.0) || (zm >= 25.0)) // needed to prevent crash of 1F1
    skg = 2.0e6;
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
 * <p> theory function: staticNKZF (see D.R. Noakes and G.M. Kalvius Phys. Rev. B 56, 2352 (1997) and
 * A. Yaouanc and P. Dalmas de Reotiers, "Muon Spin Rotation, Relaxation, and Resonance" Oxford, Section 6.4.1.3)
 * However, I have rewritten it using the identity \f$\Delta_{\rm eff}^2 = (1+R_b^2) \Delta_0^2\f$, which allows
 * to massively simplify the formula which now only involves \f$R_b\f$ and \f$\Delta_0\f$. Here \f$\Delta_0\f$
 * Will be given in units of \f$1/\mu\mathrm{sec}\f$.
 *
 * \f[ = \frac{1}{3} + \frac{2}{3}\,\frac{1}{\left(1+(R_b \Delta_0 t)^2\right)^{3/2}}\,
 *    \left(1 - \frac{(\Delta_0 t)^2}{\left(1+(R_b \Delta_0 t)^2\right)}\right)\,
 *    \exp\left[\frac{(\Delta_0 t)^2}{2\left(1+(R_b \Delta_0 t)^2\right)}\right] \f]
 *
 * <b>meaning of paramValues:</b> \f$\Delta_0\f$, \f$R_{\rm b} = \Delta_{\rm GbG}/\Delta_0\f$ [,\f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::StaticNKZF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected paramters: damping_D0 [0], R_b tshift [1]

  Double_t val[3];
  Double_t result = 1.0;

  assert(fParamNo.size() <= 3);

  if (t < 0.0)
    return result;

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 2) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[2];

  Double_t D2_t2 = val[0]*val[0]*tt*tt;
  Double_t denom = 1.0+val[1]*val[1]*D2_t2;

  result = 0.333333333333333 + 0.666666666666666667 * TMath::Power(1.0/denom, 1.5) * (1.0 - (D2_t2/denom)) * exp(-0.5*D2_t2/denom);

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: staticNKTF (see D.R. Noakes and G.M. Kalvius Phys. Rev. B 56, 2352 (1997) and
 * A. Yaouanc and P. Dalmas de Reotiers, "Muon Spin Rotation, Relaxation, and Resonance" Oxford, Section 6.4.1.3)
 * However, I have rewritten it using the identity \f$\Delta_{\rm eff}^2 = (1+R_b^2) \Delta_0^2\f$, which allows
 * to massively simplify the formula which now only involves \f$R_b\f$ and \f$\Delta_0\f$. Here \f$\Delta_0\f$
 * Will be given in units of \f$1/\mu\mathrm{sec}\f$.
 *
 * \f[ = \frac{1}{\sqrt{1+(R_b \gamma\Delta_0 t)^2}}\,
 *     \exp\left[-\frac{(\gamma\Delta_0 t)^2}{2(1+(R_b \gamma\Delta_0 t)^2)}\right]\,
 *     \cos(\gamma B_{\rm ext} t + \varphi) \f]
 *
 * <b>meaning of paramValues:</b> \f$\varphi\f$, \f$\nu = \gamma B_{\rm ext}\f$, \f$\Delta_0\f$, \f$R_{\rm b} = \Delta_{\rm GbG}/\Delta_0\f$ [,\f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::StaticNKTF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected paramters: phase [0], frequency [1], damping_D0 [2], R_b [3],  [tshift [4]]

  Double_t val[5];
  Double_t result = 1.0;

  assert(fParamNo.size() <= 5);

  if (t < 0.0)
    return result;

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 4) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[4];

  Double_t D2_t2 = val[2]*val[2]*tt*tt;
  Double_t denom = 1.0+val[3]*val[3]*D2_t2;

  result = sqrt(1.0/denom)*exp(-0.5*D2_t2/denom)*TMath::Cos(DEG_TO_RAD*val[0]+TWO_PI*val[1]*tt);

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: dynamicNKZF (see D.R. Noakes and G.M. Kalvius Phys. Rev. B 56, 2352 (1997) and
 * A. Yaouanc and P. Dalmas de Reotiers, "Muon Spin Rotation, Relaxation, and Resonance" Oxford, Section 6.4.1.3)
 * However, I have rewritten it using the identity \f$\Delta_{\rm eff}^2 = (1+R_b^2) \Delta_0^2\f$, which allows
 * to massively simplify the formula which now only involves \f$R_b\f$ and \f$\Delta_0\f$. Here \f$\Delta_0\f$
 * Will be given in units of \f$1/\mu\mathrm{sec}\f$.
 *
 * \f{eqnarray*}
 * \Theta(t) &=& \frac{\exp(-\nu_c t) - 1 - \nu_c t}{\nu_c^2} \\
 * P_{Z}^{\rm dyn}(t) &=& \sqrt{\frac{1}{1+4 R_b^2 \Delta_0^2 \Theta(t)}}\,\exp\left[-\frac{2 \Delta_0^2 \Theta(t)}{1+4 R_b^2 \Delta_0^2 \Theta(t)}\right]
 * \f}
 *
 * <b>meaning of paramValues:</b> \f$\Delta_0\f$, \f$R_{\rm b} = \Delta_{\rm GbG}/\Delta_0\f$, \f$\nu_c\f$ [,\f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::DynamicNKZF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected paramters: damping_D0 [0], R_b [1], nu_c [2], [tshift [3]]

  Double_t val[4];
  Double_t result = 1.0;

  assert(fParamNo.size() <= 4);

  if (t < 0.0)
    return result;

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 3) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[3];

  Double_t theta;
  if (val[2] < 1.0e-6) { // nu_c -> 0
    theta = 0.5*tt*tt;
  } else {
    theta = (exp(-val[2]*tt) - 1.0 + val[2]*tt)/(val[2]*val[2]);
  }
  Double_t denom = 1.0 + 4.0*val[0]*val[0]*val[1]*val[1]*theta;

  result = sqrt(1.0/denom)*exp(-2.0*val[0]*val[0]*theta/denom);

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: dynamicNKTF (see D.R. Noakes and G.M. Kalvius Phys. Rev. B 56, 2352 (1997) and
 * A. Yaouanc and P. Dalmas de Reotiers, "Muon Spin Rotation, Relaxation, and Resonance" Oxford, Section 6.4.1.3)
 * However, I have rewritten it using the identity \f$\Delta_{\rm eff}^2 = (1+R_b^2) \Delta_0^2\f$, which allows
 * to massively simplify the formula which now only involves \f$R_b\f$ and \f$\Delta_0\f$. Here \f$\Delta_0\f$
 * Will be given in units of \f$1/\mu\mathrm{sec}\f$.
 *
 * \f{eqnarray*}
 * \Theta(t) &=& \frac{\exp(-\nu_c t) - 1 - \nu_c t}{\nu_c^2} \\
 * P_{X}^{\rm dyn}(t) &=& \sqrt{\frac{1}{1+2 R_b^2 \Delta_0^2 \Theta(t)}}\,\exp\left[-\frac{\Delta_0^2 \Theta(t)}{1+2 R_b^2 \Delta_0^2 \Theta(t)}\right]\,\cos(\gamma B_{\rm ext} t + \varphi)
 * \f}
 *
 * <b>meaning of paramValues:</b> \f$\varphi\f$, \f$\nu = \gamma B_{\rm ext}\f$, \f$\Delta_0\f$, \f$R_{\rm b} = \Delta_{\rm GbG}/\Delta_0\f$, \f$\nu_c\f$ [,\f$t_{\rm shift}\f$]
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::DynamicNKTF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected paramters: phase [0], frequency [1], damping_D0 [2], R_b [3], nu_c [4], [tshift [5]]

  Double_t val[6];
  Double_t result = 1.0;

  assert(fParamNo.size() <= 6);

  if (t < 0.0)
    return result;

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      val[i] = paramValues[fParamNo[i]];
    } else { // function
      val[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  Double_t tt;
  if (fParamNo.size() == 5) // no tshift
    tt = t;
  else // tshift present
    tt = t-val[5];

  Double_t theta;
  if (val[4] < 1.0e-6) { // nu_c -> 0
    theta = 0.5*tt*tt;
  } else {
    theta = (exp(-val[4]*tt) - 1.0 + val[4]*tt)/(val[4]*val[4]);
  }
  Double_t denom = 1.0 + 2.0*val[2]*val[2]*val[3]*val[3]*theta;

  result = sqrt(1.0/denom)*exp(-val[2]*val[2]*theta/denom)*TMath::Cos(DEG_TO_RAD*val[0]+TWO_PI*val[1]*tt);

  return result;
}

//--------------------------------------------------------------------------
/**
 * <p> theory function: polynom
 *
 * \f[ = \sum_{k=0}^n a_k (t-t_{\rm shift})^k \f]
 *
 * <b>meaning of paramValues:</b> \f$t_{\rm shift}\f$, \f$a_0\f$, \f$a_1\f$, \f$\ldots\f$, \f$a_n\f$
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::Polynom(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // expected parameters: tshift p0 p1 p2 ...

  Double_t result = 0.0;
  Double_t tshift = 0.0;
  Double_t val;
  Double_t expo = 0.0;

  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fParamNo.size(); i++) {
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
 * <p> theory function: user function
 *
 * <b>return:</b> function value
 *
 * \param t time in \f$(\mu\mathrm{s})\f$, or x-axis value for non-muSR fit
 * \param paramValues parameter values
 * \param funcValues vector with the functions (i.e. functions of the parameters)
 */
Double_t PTheory::UserFcn(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const
{
  // check if FUNCTIONS are used
  for (UInt_t i=0; i<fUserParam.size(); i++) {
    if (fParamNo[i] < MSR_PARAM_FUN_OFFSET) { // parameter or resolved map
      fUserParam[i] = paramValues[fParamNo[i]];
    } else { // function
      fUserParam[i] = funcValues[fParamNo[i]-MSR_PARAM_FUN_OFFSET];
    }
  }

  return (*fUserFcn)(t, fUserParam);
}

//--------------------------------------------------------------------------
/**
 * <p>Calculates the non-analytic integral of the static Gaussian Kubo-Toyabe in longitudinal field, i.e.
 * \f[ G_{\rm G,LF}(t) = \int^t_0 \exp\left[-1/2 (\sigma\tau)^2\right]\sin(2\pi\nu\tau)\mathrm{d}\tau \f]
 * and stores \f$G_{\rm G,LF}(t)\f$ in a vector, fLFIntegral, from \f$t=0\f$ up to 20us, if needed
 *
 * The fLFIntegral is given in steps of fSamplingTime (default = 1 ns), e.g. for fSamplingTime = 0.001,
 * index i=100 coresponds to t=100ns
 *
 * <b>meaning of val:</b> val[0]=\f$\nu\f$, val[1]=\f$\sigma\f$
 *
 * \param val parameters needed to calculate the non-analytic integral of the static Gauss LF function.
 */
void PTheory::CalculateGaussLFIntegral(const Double_t *val) const
{
  // val[0] = nu (field), val[1] = Delta

  if (val[0] == 0.0) { // field == 0.0, hence nothing to be done
    return;
  } else if (val[1]/val[0] > 79.5775) { // check if a/w0 > 500.0, in which case the ZF formula is used and here nothing has to be done
    return;
  }


  Double_t dt=0.001; // all times in usec
  Double_t t, ft;
  Double_t w0 = TMath::TwoPi()*val[0];
  Double_t Delta = val[1];
  Double_t preFactor = 2.0*TMath::Power(Delta, 4.0) / TMath::Power(w0, 3.0);

  // check if the time resolution needs to be increased
  const Int_t samplingPerPeriod = 20;
  const Int_t samplingOnExp = 3000;
  if ((Delta <= w0) && (1.0/val[0] < 20.0)) { // makes sure that the frequency sampling is fine enough
    if (1.0/val[0]/samplingPerPeriod < 0.001) {
      dt = 1.0/val[0]/samplingPerPeriod;
    }
  } else if ((Delta > w0) && (Delta <= 10.0)) {
    if (Delta/w0 > 10.0) {
      dt = 0.00005;
    }
  } else if ((Delta > w0) && (Delta > 10.0)) { // makes sure there is a fine enough sampling for large Delta's
    if (1.0/Delta/samplingOnExp < 0.001) {
      dt = 1.0/Delta/samplingOnExp;
    }
  }

  // keep sampling time
  fSamplingTime = dt;

  // clear previously allocated vector
  fLFIntegral.clear();

  // calculate integral
  t  = 0.0;
  fLFIntegral.push_back(0.0); // start value of the integral

  ft = 0.0;
  Double_t step = 0.0;
  do {
    t  += dt;
    step = 0.5*dt*preFactor*(exp(-0.5*pow(Delta * (t-dt), 2.0))*sin(w0*(t-dt))+
                             exp(-0.5*pow(Delta * t, 2.0))*sin(w0*t));
    ft += step;
    fLFIntegral.push_back(ft);
  } while ((t <= 20.0) && (fabs(step) > 1.0e-10));
}

//--------------------------------------------------------------------------
/**
 * <p>Calculates the non-analytic integral of the static Lorentzian Kubo-Toyabe in longitudinal field, i.e.
 * \f[ G_{\rm L,LF}(t) = \int^t_0 \exp\left[-a \tau \right] j_0(2\pi\nu\tau)\mathrm{d}\tau \f]
 * and stores \f$G_{\rm L,LF}(t)\f$ in a vector, fLFIntegral, from \f$t=0\f$ up to 20 us, if needed.
 *
 * The fLFIntegral is given in steps of fSamplingTime (default = 1 ns), e.g. for fSamplingTime = 0.001,
 * index i=100 coresponds to t=100ns
 *
 * <b>meaning of val:</b> val[0]=\f$\nu\f$, val[1]=\f$a\f$
 *
 * \param val parameters needed to calculate the non-analytic integral of the static Lorentz LF function.
 */
void PTheory::CalculateLorentzLFIntegral(const Double_t *val) const
{
  // val[0] = nu, val[1] = a

  // a few checks if the integral actually needs to be calculated
  if (val[0] < 0.02) { // if smaller 20kHz ~ 0.27G use the ZF formula and here nothing has to be done
    return;
  } else if (val[1]/val[0] > 159.1549) { // check if a/w0 > 1000.0, in which case the ZF formula is used and here nothing has to be done
    return;
  }

  Double_t dt=0.001; // all times in usec
  Double_t t, ft;
  Double_t w0 = TMath::TwoPi()*val[0];
  Double_t a = val[1];
  Double_t preFactor = a*(1+pow(a/w0,2.0));

  // check if the time resolution needs to be increased
  const Int_t samplingPerPeriod = 20;
  const Int_t samplingOnExp = 3000;
  if ((a <= w0) && (1.0/val[0] < 20.0)) { // makes sure that the frequency sampling is fine enough
    if (1.0/val[0]/samplingPerPeriod < 0.001) {
      dt = 1.0/val[0]/samplingPerPeriod;
    }
  } else if ((a > w0) && (a <= 10.0)) {
    if (a/w0 > 10.0) {
      dt = 0.00005;
    }
  } else if ((a > w0) && (a > 10.0)) { // makes sure there is a fine enough sampling for large a's
    if (1.0/a/samplingOnExp < 0.001) {
      dt = 1.0/a/samplingOnExp;
    }
  }

  // keep sampling time
  fSamplingTime = dt;

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
  Double_t step = 0.0;
  do {
    t  += dt;
    step = 0.5*dt*preFactor*(sin(w0*(t-dt))/(w0*(t-dt))*exp(-a*(t-dt))+sin(w0*t)/(w0*t)*exp(-a*t));
    ft += step;
    fLFIntegral.push_back(ft);
  } while ((t <= 20.0) && (fabs(step) > 1.0e-10));
}


//--------------------------------------------------------------------------
/**
 * <p>Gets value of the non-analytic static LF integral.
 *
 * <b>return:</b> value of the non-analytic static LF integral
 *
 * \param t time in (usec)
 */
Double_t PTheory::GetLFIntegralValue(const Double_t t) const
{
  if (t < 0.0)
    return 0.0;

  UInt_t idx = static_cast<UInt_t>(t/fSamplingTime);

  if (idx + 2 > fLFIntegral.size())
    return fLFIntegral.back();

  // linearly interpolate between the two relvant function bins
  Double_t df = (fLFIntegral[idx+1]-fLFIntegral[idx])*(t/fSamplingTime-static_cast<Double_t>(idx));

  return fLFIntegral[idx]+df;
}

//--------------------------------------------------------------------------
/**
 * <p><b>Docu of this function still missing!!</b>
 *
 * <p>Number of sampling points is estimated according to w0: w0 T = 2 pi
 * t_sampling = T/16 (i.e. 16 points on 1 period)
 * N = 8/pi w0 Tmax = 16 nu0 Tmax
 *
 * \param val parameters needed to solve the voltera integral equation
 * \param tag 0=Gauss, 1=Lorentz
 */
void PTheory::CalculateDynKTLF(const Double_t *val, Int_t tag) const
{
  // val: 0=nu0, 1=Delta (Gauss) / a (Lorentz), 2=nu
  const Double_t Tmax = 20.0; // 20 usec
  UInt_t N = static_cast<UInt_t>(16.0*Tmax*val[0]);

  // check if rate (Delta or a) is very high
  if (fabs(val[1]) > 0.1) {
    Double_t tmin = 20.0;
    switch (tag) {
      case 0: // Gauss
        tmin = fabs(sqrt(3.0)/val[1]);
        break;
      case 1: // Lorentz
        tmin = fabs(2.0/val[1]);
        break;
      default:
        break;
    }
    UInt_t Nrate = static_cast<UInt_t>(25.0 * Tmax / tmin);
    if (Nrate > N) {
      N = Nrate;
    }
  }

  if (N < 300) // if too few points, i.e. nu0 very small, take 300 points
    N = 300;

  if (N>1e6) // make sure that N is not too large to prevent memory overflow
    N = 1e6;

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
      cerr << endl << ">> PTheory::CalculateDynKTLF: **FATAL ERROR** You should never have reached this point." << endl;
      assert(false);
      break;
  }

  // calculate the P^(0)(t) exp(-nu t) vector
  PDoubleVector p0exp(N);
  Double_t t = 0.0;
  Double_t dt = Tmax/N;
  fDynLFdt = dt; // keep it since it is needed in GetDynKTLFValue()
  for (UInt_t i=0; i<N; i++) {
    switch (tag) {
      case 0: // Gauss
        if (val[0] < 0.02) { // if smaller 20kHz ~ 0.27G use zero field formula
          Double_t sigma_t_2 = t*t*val[1]*val[1];
          p0exp[i] = 0.333333333333333 * (1.0 + 2.0*(1.0 - sigma_t_2)*TMath::Exp(-0.5*sigma_t_2));
        } else if (val[1]/val[0] > 79.5775) { // check if Delta/w0 > 500.0, in which case the ZF formula is used
          Double_t sigma_t_2 = t*t*val[1]*val[1];
          p0exp[i] = 0.333333333333333 * (1.0 + 2.0*(1.0 - sigma_t_2)*TMath::Exp(-0.5*sigma_t_2));
        } else {
          Double_t delta = val[1];
          Double_t w0    = TWO_PI*val[0];

          p0exp[i] = 1.0 - 2.0*TMath::Power(delta/w0,2.0)*(1.0 -
                    TMath::Exp(-0.5*TMath::Power(delta*t, 2.0))*TMath::Cos(w0*t)) +
                    GetLFIntegralValue(t);
        }
        break;
      case 1: // Lorentz
        if (val[0] < 0.02) { // if smaller 20kHz ~ 0.27G use zero field formula
          Double_t at = t*val[1];
          p0exp[i] = 0.333333333333333 * (1.0 + 2.0*(1.0 - at)*TMath::Exp(-at));
        } else if (val[1]/val[0] > 159.1549) { // check if a/w0 > 1000.0, in which case the ZF formula is used
          Double_t at = t*val[1];
          p0exp[i] = 0.333333333333333 * (1.0 + 2.0*(1.0 - at)*TMath::Exp(-at));
        } else {
          Double_t a    = val[1];
          Double_t at   = a*t;
          Double_t w0   = TWO_PI*val[0];
          Double_t a_w0 = a/w0;
          Double_t w0t  = w0*t;

          Double_t j1, j0;
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

  Double_t sum;
  Double_t a;
  Double_t preFactor = dt*val[2];
  for (UInt_t i=1; i<N; i++) {
    sum = p0exp[i];
    sum += 0.5*preFactor*p0exp[i]*fDynLFFuncValue[0];
    for (UInt_t j=1; j<i; j++) {
      sum += preFactor*p0exp[i-j]*fDynLFFuncValue[j];
    }
    a = 1.0-0.5*preFactor*p0exp[0];

    fDynLFFuncValue[i]=sum/a;
  }

  // clean up
  p0exp.clear();
}

//--------------------------------------------------------------------------
/**
 * <p>Gets value of the dynamic Kubo-Toyabe LF function.
 *
 * <b>return:</b> function value
 *
 * \param t time in (usec)
 */
Double_t PTheory::GetDynKTLFValue(const Double_t t) const
{
  if (t < 0.0)
    return 0.0;

  UInt_t idx = static_cast<UInt_t>(t/fDynLFdt);

  if (idx + 2 > fDynLFFuncValue.size())
    return fDynLFFuncValue.back();

  // linearly interpolate between the two relvant function bins
  Double_t df = (fDynLFFuncValue[idx+1]-fDynLFFuncValue[idx])*(t/fDynLFdt-static_cast<Double_t>(idx));

  return fDynLFFuncValue[idx]+df;
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
