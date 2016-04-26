/***************************************************************************

  PTheory.h

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

#ifndef _PTHEORY_H_
#define _PTHEORY_H_

#include <TSystem.h>
#include <TString.h>

#include "PMusr.h"
#include "PMsrHandler.h"
#include "PUserFcnBase.h"

// --------------------------------------------------------
// function handling tags
// --------------------------------------------------------

// function tags
#define THEORY_UNDEFINED                    -1
#define THEORY_CONST                         0
#define THEORY_ASYMMETRY                     1
#define THEORY_SIMPLE_EXP                    2
#define THEORY_GENERAL_EXP                   3
#define THEORY_SIMPLE_GAUSS                  4
#define THEORY_STATIC_GAUSS_KT               5
#define THEORY_STATIC_GAUSS_KT_LF            6
#define THEORY_DYNAMIC_GAUSS_KT_LF           7
#define THEORY_STATIC_LORENTZ_KT             8
#define THEORY_STATIC_LORENTZ_KT_LF          9
#define THEORY_DYNAMIC_LORENTZ_KT_LF        10
#define THEORY_COMBI_LGKT                   11
#define THEORY_STR_KT                       12
#define THEORY_SPIN_GLASS                   13
#define THEORY_RANDOM_ANISOTROPIC_HYPERFINE 14
#define THEORY_ABRAGAM                      15
#define THEORY_TF_COS                       16
#define THEORY_INTERNAL_FIELD               17
#define THEORY_INTERNAL_FIELD_KORNILOV      18
#define THEORY_INTERNAL_FIELD_LARKIN        19
#define THEORY_BESSEL                       20
#define THEORY_INTERNAL_BESSEL              21
#define THEORY_SKEWED_GAUSS                 22
#define THEORY_STATIC_ZF_NK                 23
#define THEORY_STATIC_TF_NK                 24
#define THEORY_DYNAMIC_ZF_NK                25
#define THEORY_DYNAMIC_TF_NK                26
#define THEORY_MU_MINUS_EXP                 27
#define THEORY_POLYNOM                      28
#define THEORY_USER_FCN                     29

// function parameter tags, i.e. how many parameters has a specific function
// if there is a comment with a (tshift), the number of parameters is increased by one
#define THEORY_PARAM_CONST                        1 // const
#define THEORY_PARAM_ASYMMETRY                    1 // asymmetry
#define THEORY_PARAM_SIMPLE_EXP                   1 // damping (tshift)
#define THEORY_PARAM_GENERAL_EXP                  2 // damping, exponents  (tshift)
#define THEORY_PARAM_SIMPLE_GAUSS                 1 // damping (tshift)
#define THEORY_PARAM_STATIC_GAUSS_KT              1 // damping (tshift)
#define THEORY_PARAM_STATIC_GAUSS_KT_LF           2 // frequency, damping (tshift)
#define THEORY_PARAM_DYNAMIC_GAUSS_KT_LF          3 // frequency, damping, hop-rate (tshift)
#define THEORY_PARAM_STATIC_LORENTZ_KT            1 // damping (tshift)
#define THEORY_PARAM_STATIC_LORENTZ_KT_LF         2 // frequency, damping (tshift)
#define THEORY_PARAM_DYNAMIC_LORENTZ_KT_LF        3 // frequency, damping, hop-rate (tshift)
#define THEORY_PARAM_COMBI_LGKT                   2 // Lorentz rate, Gauss rate (tshift)
#define THEORY_PARAM_STR_KT                       2 // rate, exponent (tshift)
#define THEORY_PARAM_SPIN_GLASS                   3 // rate, hop-rate, order parameter (tshift)
#define THEORY_PARAM_RANDOM_ANISOTROPIC_HYPERFINE 2 // frequency, rate (tshift)
#define THEORY_PARAM_ABRAGAM                      2 // rate, hop-rate (tshift)
#define THEORY_PARAM_TF_COS                       2 // phase, frequency (tshift)
#define THEORY_PARAM_INTERNAL_FIELD               5 // fraction, phase, frequency, TF damping, damping (tshift)
#define THEORY_PARAM_INTERNAL_FIELD_KORNILOV      5 // fraction, frequency, TF damping, damping, beta (tshift)
#define THEORY_PARAM_INTERNAL_FIELD_LARKIN        4 // fraction, frequency, TF damping, damping (tshift)
#define THEORY_PARAM_BESSEL                       2 // phase, frequency (tshift)
#define THEORY_PARAM_INTERNAL_BESSEL              5 // fraction, phase, frequency, TF damping, LF damping (tshift)
#define THEORY_PARAM_SKEWED_GAUSS                 4 // phase, frequency, rate minus, rate plus (tshift)
#define THEORY_PARAM_STATIC_ZF_NK                 2 // damping D0, R_b=DGbG/D0 (tshift)
#define THEORY_PARAM_STATIC_TF_NK                 4 // phase, frequency, damping D0, R_b=DGbG/D0 (tshift)
#define THEORY_PARAM_DYNAMIC_ZF_NK                3 // damping D0, R_b=DGbG/D0, nu_c (tshift)
#define THEORY_PARAM_DYNAMIC_TF_NK                5 // phase, frequency, damping D0, R_b=DGbG/D0, nu_c (tshift)
#define THEORY_PARAM_MU_MINUS_EXP                 6 // N0, tau, A, damping, phase, frequency (tshift)

// number of available user functions
#define THEORY_MAX 30

// maximal number of parameters. Needed in the contents of LF
#define THEORY_MAX_PARAM 10

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
  UInt_t fType;      ///< function tag
  UInt_t fNoOfParam; ///< number of parameters for this function
  Bool_t fTable;     ///< table flag, indicating if the function is generate from a table
  TString fName;     ///< name of the function as written into the msr-file
  TString fAbbrev;   ///< abbreviation of the function name
  TString fComment;  ///< comment added in the msr-file theory block to help the used
  TString fCommentTimeShift; ///< comment added in the msr-file theory block if there is a time shift
} PTheoDataBase;

//--------------------------------------------------------------------------------------
/**
 * <p> Holds the functions available for the user.
 */
static PTheoDataBase fgTheoDataBase[THEORY_MAX] = {

  {THEORY_CONST, THEORY_PARAM_CONST, false,
   "const", "c", "", ""},

  {THEORY_ASYMMETRY, THEORY_PARAM_ASYMMETRY, false,
   "asymmetry", "a", "", ""},

  {THEORY_SIMPLE_EXP, THEORY_PARAM_SIMPLE_EXP, false,
   "simplExpo", "se", "(rate)", "(rate tshift)"},

  {THEORY_GENERAL_EXP, THEORY_PARAM_GENERAL_EXP, false,
   "generExpo", "ge", "(rate exponent)", "(rate exponent tshift)"},

  {THEORY_SIMPLE_GAUSS, THEORY_PARAM_SIMPLE_GAUSS, false,
   "simpleGss", "sg", "(rate)", "(rate tshift)"},

  {THEORY_STATIC_GAUSS_KT, THEORY_PARAM_STATIC_GAUSS_KT, false,
   "statGssKt", "stg", "(rate)", "(rate tshift)"},

  {THEORY_STATIC_GAUSS_KT_LF, THEORY_PARAM_STATIC_GAUSS_KT_LF, true,
   "statGssKTLF", "sgktlf", "(frequency damping)", "(frequency damping tshift)"},

  {THEORY_DYNAMIC_GAUSS_KT_LF, THEORY_PARAM_DYNAMIC_GAUSS_KT_LF, true,
   "dynGssKTLF", "dgktlf", "(frequency damping hopping-rate)", "(frequency damping hopping-rate tshift)"},

  {THEORY_STATIC_LORENTZ_KT, THEORY_PARAM_STATIC_LORENTZ_KT, true,
   "statExpKT", "sekt", "(rate)", "(rate tshift)"},

  {THEORY_STATIC_LORENTZ_KT_LF, THEORY_PARAM_STATIC_LORENTZ_KT_LF, true,
   "statExpKTLF", "sektlf", "(frequency damping)", "(frequency damping tshift)"},

  {THEORY_DYNAMIC_LORENTZ_KT_LF, THEORY_PARAM_DYNAMIC_LORENTZ_KT_LF, true,
   "dynExpKTLF", "dektlf", "(frequency damping hopping-rate)", "(frequency damping hopping-rate tshift)"},

  {THEORY_COMBI_LGKT, THEORY_PARAM_COMBI_LGKT, false,
   "combiLGKT", "lgkt", "(lorentzRate gaussRate)", "(lorentzRate gaussRate tshift)"},

  {THEORY_STR_KT, THEORY_PARAM_STR_KT, false,
   "strKT", "skt", "(rate beta)", "(rate beta tshift)"},

  {THEORY_SPIN_GLASS, THEORY_PARAM_SPIN_GLASS, false,
   "spinGlass",  "spg", "(rate hopprate order)", "(rate hopprate order tshift)"},

  {THEORY_RANDOM_ANISOTROPIC_HYPERFINE, THEORY_PARAM_RANDOM_ANISOTROPIC_HYPERFINE, false,
   "rdAnisoHf", "rahf", "(frequency rate)", "(frequency rate tshift)"},

  {THEORY_ABRAGAM, THEORY_PARAM_ABRAGAM, false,
   "abragam", "ab", "(rate hopprate)", "(rate hopprate tshift)"},

  {THEORY_TF_COS, THEORY_PARAM_TF_COS, false,
   "TFieldCos", "tf", "(phase frequency)", "(phase frequency tshift)"},

  {THEORY_INTERNAL_FIELD, THEORY_PARAM_INTERNAL_FIELD, false,
   "internFld", "if", "(fraction phase frequency Trate Lrate)", "(fraction phase frequency Trate Lrate tshift)"},

  {THEORY_INTERNAL_FIELD_KORNILOV, THEORY_PARAM_INTERNAL_FIELD_KORNILOV, false,
   "internFldGK", "ifgk", "(fraction frequency Trate Lrate beta)", "(fraction frequency Trate Lrate beta tshift)"},

  {THEORY_INTERNAL_FIELD_LARKIN, THEORY_PARAM_INTERNAL_FIELD_LARKIN, false,
   "internFldLL", "ifll", "(fraction frequency Trate Lrate)", "(fraction frequency Trate Lrate tshift)"},

  {THEORY_BESSEL, THEORY_PARAM_BESSEL, false,
   "bessel", "b", "(phase frequency)", "(phase frequency tshift)"},

  {THEORY_INTERNAL_BESSEL, THEORY_PARAM_INTERNAL_BESSEL, false,
   "internBsl", "ib", "(fraction phase frequency Trate Lrate)", "(fraction phase frequency Trate Lrate tshift)"},

  {THEORY_SKEWED_GAUSS, THEORY_PARAM_SKEWED_GAUSS, false,
   "skewedGss", "skg", "(phase frequency rate_m rate_p)", "(phase frequency rate_m rate_p tshift)"},

  {THEORY_STATIC_ZF_NK, THEORY_PARAM_STATIC_ZF_NK, false,
   "staticNKZF", "snkzf", "(damping_D0 R_b)", "(damping_D0 R_b tshift)"},

  {THEORY_STATIC_TF_NK, THEORY_PARAM_STATIC_TF_NK, false,
   "staticNKTF", "snktf", "(phase frequency damping_D0 R_b)", "(phase frequency damping_D0 R_b tshift)"},

  {THEORY_DYNAMIC_ZF_NK, THEORY_PARAM_DYNAMIC_ZF_NK, false,
   "dynamicNKZF", "dnkzf", "(damping_D0 R_b nu_c)", "(damping_D0 R_b nu_c tshift)"},

  {THEORY_DYNAMIC_TF_NK, THEORY_PARAM_DYNAMIC_TF_NK, false,
   "dynamicNKTF", "dnktf", "(phase frequency damping_D0 R_b nu_c)", "(phase frequency damping_D0 R_b nu_c tshift)"},

  {THEORY_MU_MINUS_EXP, THEORY_PARAM_MU_MINUS_EXP, false,
   "muMinusExpTF", "mmsetf", "(N0 tau A lambda phase nu)", "(N0 tau A lambda phase nu tshift)"},

  {THEORY_POLYNOM, 0, false,
   "polynom", "p", "(tshift p0 p1 ... pn)", "(tshift p0 p1 ... pn)"},

  {THEORY_USER_FCN, 0, false,
   "userFcn", "u", "", ""}
};

//--------------------------------------------------------------------------------------
/**
 * <p>Class handling a theory of a msr-file.
 */
class PTheory
{
  public:
    PTheory(PMsrHandler *msrInfo, UInt_t runNo, const Bool_t hasParent = false);
    virtual ~PTheory();

    virtual Bool_t IsValid();
    virtual Double_t Func(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;

  private:
    virtual void CleanUp(PTheory *theo);
    virtual Int_t SearchDataBase(TString name);
    virtual Int_t GetUserFcnIdx(UInt_t lineNo) const;
    virtual void MakeCleanAndTidyTheoryBlock(PMsrLines* fullTheoryBlock);
    virtual void MakeCleanAndTidyPolynom(UInt_t i, PMsrLines* fullTheoryBlock);
    virtual void MakeCleanAndTidyUserFcn(UInt_t i, PMsrLines* fullTheoryBlock);

    virtual Double_t Constant(const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t Asymmetry(const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t SimpleExp(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t GeneralExp(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t SimpleGauss(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t StaticGaussKT(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t StaticGaussKTLF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t DynamicGaussKTLF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t StaticLorentzKT(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t StaticLorentzKTLF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t DynamicLorentzKTLF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t CombiLGKT(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t StrKT(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t SpinGlass(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t RandomAnisotropicHyperfine(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t Abragam(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t TFCos(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t InternalField(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t InternalFieldGK(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t InternalFieldLL(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t Bessel(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t InternalBessel(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t SkewedGauss(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t StaticNKZF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t StaticNKTF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t DynamicNKZF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t DynamicNKTF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t MuMinusExpTF(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t Polynom(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual Double_t UserFcn(register Double_t t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;

    virtual void CalculateGaussLFIntegral(const Double_t *val) const;
    virtual void CalculateLorentzLFIntegral(const Double_t *val) const;
    virtual Double_t GetLFIntegralValue(const Double_t t) const;
    virtual void CalculateDynKTLF(const Double_t *val, Int_t tag) const;
    virtual Double_t GetDynKTLFValue(const Double_t t) const;

    // variables
    Bool_t fValid; ///< flag to tell if the theory is valid
    UInt_t fType; ///< function tag
    vector<UInt_t> fParamNo; ///< holds the parameter numbers for the theory (including maps and functions, see constructor desciption)
    UInt_t fNoOfParam; ///< number of parameters for the given function
    PTheory *fAdd, *fMul; ///< pointers to the add-sub-function or the multiply-sub-function

    Int_t fUserFcnIdx; ///< index of the user function within the theory tree
    TString fUserFcnClassName; ///< name of the user function class for within root
    TString fUserFcnSharedLibName; ///< name of the shared lib to which the user function belongs
    PUserFcnBase *fUserFcn;    ///< pointer to the user function object
    mutable PDoubleVector fUserParam;  ///< vector holding the resolved user function parameters, i.e. map and function resolved.

    PMsrHandler *fMsrInfo; ///< pointer to the msr-file handler

    mutable Double_t fSamplingTime;                ///< needed for LF. Keeps the sampling time of the non-analytic integral
    mutable Double_t fPrevParam[THEORY_MAX_PARAM]; ///< needed for LF. Keeps the previous fitting parameters
    mutable PDoubleVector fLFIntegral;             ///< needed for LF. Keeps the non-analytic integral values
    mutable Double_t fDynLFdt;                     ///< needed for LF. Keeps the time step for the dynamic LF calculation, used in the integral equation approach
    mutable PDoubleVector fDynLFFuncValue;         ///< needed for LF. Keeps the dynamic LF KT function values
};

#endif //  _PTHEORY_H_
