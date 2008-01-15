/***************************************************************************

  PTheory.h

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

#ifndef _PTHEORY_H_
#define _PTHEORY_H_

#include <vector>

#include <TString.h>

#include "PMsrHandler.h"

// --------------------------------------------------------
// function handling tags
// --------------------------------------------------------

// function tags
#define THEORY_UNDEFINED                   -1
#define THEORY_ASYMMETRY                    0
#define THEORY_SIMPLE_EXP                   1
#define THEORY_GENERAL_EXP                  2
#define THEORY_SIMPLE_GAUSS                 3
#define THEORY_STATIC_GAUSS_KT              4
#define THEORY_STATIC_KT_TABLE              5
#define THEORY_DYNAMIC_KT_TABLE             6
#define THEORY_COMBI_LGKT                   7
#define THEORY_SPIN_GLASS                   8
#define THEORY_RANDOM_ANISOTROPIC_HYPERFINE 9
#define THEORY_ABRAGAM                     10
#define THEORY_INTERNAL_FIELD              11
#define THEORY_TF_COS                      12
#define THEORY_BESSEL                      13
#define THEORY_INTERNAL_BESSEL             14
#define THEORY_USER                        15

// function parameter tags, i.e. how many parameters has a specific function
#define THEORY_PARAM_ASYMMETRY                    1 // asymetry
#define THEORY_PARAM_SIMPLE_EXP                   1 // damping
#define THEORY_PARAM_GENERAL_EXP                  2 // damping, exponents
#define THEORY_PARAM_SIMPLE_GAUSS                 1 // damping
#define THEORY_PARAM_STATIC_GAUSS_KT              1 // damping
#define THEORY_PARAM_STATIC_KT_TABLE              2 // frequency, damping
#define THEORY_PARAM_DYNAMIC_KT_TABLE             3 // frequency, damping, hop-rate
#define THEORY_PARAM_COMBI_LGKT                   2 // Lorentz rate, Gauss rate
#define THEORY_PARAM_SPIN_GLASS                   3 // rate, hop-rate, order parameter
#define THEORY_PARAM_RANDOM_ANISOTROPIC_HYPERFINE 2 // frequency, rate
#define THEORY_PARAM_ABRAGAM                      2 // rate, hop-rate
#define THEORY_PARAM_INTERNAL_FIELD               4 // phase, frequency, TF damping, damping
#define THEORY_PARAM_TF_COS                       2 // phase, frequency
#define THEORY_PARAM_BESSEL                       2 // phase, frequency
#define THEORY_PARAM_INTERNAL_BESSEL              5 // fraction, phase, frequency, TF damping, damping

// number of available user functions
#define THEORY_MAX 15

// deg -> rad factor
#define DEG_TO_RAD 0.0174532925199432955
// 2 pi
#define TWO_PI 6.28318530717958623

class PTheory;

//--------------------------------------------------------------------------------------
/**
 * <p>Structure holding the infos of a the available internally defined functions.
 */
typedef struct theo_data_base {
  unsigned int fType;      ///< function tag
  unsigned int fNoOfParam; ///< number of parameters for this function
  bool fTable;             ///< table flag, indicating if the function is generate from a table
  TString fName;           ///< name of the function as written into the msr-file
  TString fAbbrev;         ///< abbreviation of the function name
  TString fComment;        ///< comment added in the msr-file theory block to help the used
} PTheoDataBase;

//--------------------------------------------------------------------------------------
/*!
 * <p> Holds the functions available for the user.
 */
static PTheoDataBase fgTheoDataBase[THEORY_MAX] = {

        {THEORY_ASYMMETRY, THEORY_PARAM_ASYMMETRY, false,
         "asymmetry", "a", ""},

        {THEORY_SIMPLE_EXP, THEORY_PARAM_SIMPLE_EXP, false,
         "simplExpo", "se", "(rate)"},

        {THEORY_GENERAL_EXP, THEORY_PARAM_GENERAL_EXP, false,
         "generExpo", "ge", "(rate exponent)"},

        {THEORY_SIMPLE_GAUSS, THEORY_PARAM_SIMPLE_GAUSS, false,
         "simpleGss", "sg", "(rate)"},

        {THEORY_STATIC_GAUSS_KT, THEORY_PARAM_STATIC_GAUSS_KT, false,
         "statGssKt", "stg", "(rate)"},

        {THEORY_STATIC_KT_TABLE, THEORY_PARAM_STATIC_KT_TABLE, true,
         "statKTTab", "sktt", "(frequency damping table)"},

        {THEORY_DYNAMIC_KT_TABLE, THEORY_PARAM_DYNAMIC_KT_TABLE, true,
         "dynmKTTab", "dktt", "(frequency damping hopprate table)"},

        {THEORY_COMBI_LGKT, THEORY_PARAM_COMBI_LGKT, false,
         "combiLGKT", "lgkt", "(LorentzRate GaussRate)"},

        {THEORY_SPIN_GLASS, THEORY_PARAM_SPIN_GLASS, false,
         "spinGlass",  "spg", "(rate hopprate order)"},

        {THEORY_RANDOM_ANISOTROPIC_HYPERFINE, THEORY_PARAM_RANDOM_ANISOTROPIC_HYPERFINE, false,
         "rdAnisoHf", "rahf", "(frequency rate)"},

        {THEORY_ABRAGAM, THEORY_PARAM_ABRAGAM, false,
         "abragam", "ab", "(rate hopprate)"},

        {THEORY_INTERNAL_FIELD, THEORY_PARAM_INTERNAL_FIELD, false,
         "internFld", "if", "(phase frequency Trate Lrate)"},

        {THEORY_TF_COS, THEORY_PARAM_TF_COS, false,
         "TFieldCos", "tf", "(phase frequency)"},

        {THEORY_BESSEL, THEORY_PARAM_BESSEL, false,
         "bessel", "b", "(phase frequency)"},

        {THEORY_INTERNAL_BESSEL, THEORY_PARAM_INTERNAL_BESSEL, false,
         "internBsl", "ib", "(phase frequency Trate Lrate)"}};

//--------------------------------------------------------------------------------------
/**
 * <p>
 */
class PTheory
{
  public:
    PTheory(PMsrHandler *msrInfo, unsigned int runNo, const bool hasParent = false);
    virtual ~PTheory();

    virtual bool IsValid();
    virtual double Func(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;

  private:
    virtual int SearchDataBase(TString name);
    virtual void MakeCleanAndTidyTheoryBlock(PMsrLines* fullTheoryBlock);

    virtual double Asymmetry(const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double SimpleExp(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double GeneralExp(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double SimpleGauss(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double StaticGaussKT(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double StaticKTTable(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double DynamicKTTable(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double CombiLGKT(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double SpinGlass(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double RandomAnisotropicHyperfine(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double Abragam(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double InternalField(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double TFCos(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double Bessel(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;
    virtual double InternalBessel(register double t, const vector<double>& paramValues, const vector<double>& funcValues) const;

    // variables
    bool fValid;
    unsigned int fType;
    vector<unsigned int> fParamNo; ///< holds the parameter numbers for the theory (including maps and functions, see constructor desciption)
    unsigned int fNoOfParam;
//    PTable *fTable;
    PTheory *fAdd, *fMul;
//    unsigned int fTotalNoOfMsrParam;
//    TString fUserFun;
//    TString fUserFunPreParsed;

    PMsrHandler *fMsrInfo;
};

#endif //  _PTHEORY_H_
