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
#define THEORY_ASYMMETRY                     0
#define THEORY_SIMPLE_EXP                    1
#define THEORY_GENERAL_EXP                   2
#define THEORY_SIMPLE_GAUSS                  3
#define THEORY_STATIC_GAUSS_KT               4
#define THEORY_STATIC_GAUSS_KT_LF            5
#define THEORY_DYNAMIC_GAUSS_KT_LF           6
#define THEORY_STATIC_LORENTZ_KT_LF          7
#define THEORY_DYNAMIC_LORENTZ_KT_LF         8
#define THEORY_COMBI_LGKT                    9
#define THEORY_SPIN_GLASS                   10
#define THEORY_RANDOM_ANISOTROPIC_HYPERFINE 11
#define THEORY_ABRAGAM                      12
#define THEORY_INTERNAL_FIELD               13
#define THEORY_TF_COS                       14
#define THEORY_BESSEL                       15
#define THEORY_INTERNAL_BESSEL              16
#define THEORY_SKEWED_GAUSS                 17
#define THEORY_POLYNOM                      18
#define THEORY_USER_FCN                     19

// function parameter tags, i.e. how many parameters has a specific function
// if there is a comment with a (tshift), the number of parameters is increased by one
#define THEORY_PARAM_ASYMMETRY                    1 // asymetry
#define THEORY_PARAM_SIMPLE_EXP                   1 // damping (tshift)
#define THEORY_PARAM_GENERAL_EXP                  2 // damping, exponents  (tshift)
#define THEORY_PARAM_SIMPLE_GAUSS                 1 // damping (tshift)
#define THEORY_PARAM_STATIC_GAUSS_KT              1 // damping (tshift)
#define THEORY_PARAM_STATIC_GAUSS_KT_LF           2 // frequency, damping (tshift)
#define THEORY_PARAM_DYNAMIC_GAUSS_KT_LF          3 // frequency, damping, hop-rate (tshift)
#define THEORY_PARAM_STATIC_LORENTZ_KT_LF         2 // frequency, damping (tshift)
#define THEORY_PARAM_DYNAMIC_LORENTZ_KT_LF        3 // frequency, damping, hop-rate (tshift)
#define THEORY_PARAM_COMBI_LGKT                   2 // Lorentz rate, Gauss rate (tshift)
#define THEORY_PARAM_SPIN_GLASS                   3 // rate, hop-rate, order parameter (tshift)
#define THEORY_PARAM_RANDOM_ANISOTROPIC_HYPERFINE 2 // frequency, rate (tshift)
#define THEORY_PARAM_ABRAGAM                      2 // rate, hop-rate (tshift)
#define THEORY_PARAM_INTERNAL_FIELD               4 // phase, frequency, TF damping, damping (tshift)
#define THEORY_PARAM_TF_COS                       2 // phase, frequency (tshift)
#define THEORY_PARAM_BESSEL                       2 // phase, frequency (tshift)
#define THEORY_PARAM_INTERNAL_BESSEL              5 // fraction, phase, frequency, TF damping, LF damping (tshift)
#define THEORY_PARAM_SKEWED_GAUSS                 4 // phase, frequency, rate minus, rate plus (tshift)

// number of available user functions
#define THEORY_MAX 20

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
  unsigned int fType;      ///< function tag
  unsigned int fNoOfParam; ///< number of parameters for this function
  bool fTable;             ///< table flag, indicating if the function is generate from a table
  TString fName;           ///< name of the function as written into the msr-file
  TString fAbbrev;         ///< abbreviation of the function name
  TString fComment;        ///< comment added in the msr-file theory block to help the used
  TString fCommentTimeShift; ///< comment added in the msr-file theory block if there is a time shift
} PTheoDataBase;

//--------------------------------------------------------------------------------------
/*!
 * <p> Holds the functions available for the user.
 */
static PTheoDataBase fgTheoDataBase[THEORY_MAX] = {

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
         "dynGssKTLF", "dgktt", "(frequency damping hopping-rate)", "(frequency damping hopping-rate tshift)"},

        {THEORY_STATIC_LORENTZ_KT_LF, THEORY_PARAM_STATIC_LORENTZ_KT_LF, true,
         "statExpKTLF", "sektlf", "(frequency damping)", "(frequency damping tshift)"},

        {THEORY_DYNAMIC_LORENTZ_KT_LF, THEORY_PARAM_DYNAMIC_LORENTZ_KT_LF, true,
         "dynExpKTLF", "dektt", "(frequency damping hopping-rate)", "(frequency damping hopping-rate tshift)"},

        {THEORY_COMBI_LGKT, THEORY_PARAM_COMBI_LGKT, false,
         "combiLGKT", "lgkt", "(LorentzRate GaussRate)", "(LorentzRate GaussRate tshift)"},

        {THEORY_SPIN_GLASS, THEORY_PARAM_SPIN_GLASS, false,
         "spinGlass",  "spg", "(rate hopprate order)", "(rate hopprate order tshift)"},

        {THEORY_RANDOM_ANISOTROPIC_HYPERFINE, THEORY_PARAM_RANDOM_ANISOTROPIC_HYPERFINE, false,
         "rdAnisoHf", "rahf", "(frequency rate)", "(frequency rate tshift)"},

        {THEORY_ABRAGAM, THEORY_PARAM_ABRAGAM, false,
         "abragam", "ab", "(rate hopprate)", "(rate hopprate tshift)"},

        {THEORY_INTERNAL_FIELD, THEORY_PARAM_INTERNAL_FIELD, false,
         "internFld", "if", "(phase frequency Trate Lrate)", "(phase frequency Trate Lrate tshift)"},

        {THEORY_TF_COS, THEORY_PARAM_TF_COS, false,
         "TFieldCos", "tf", "(phase frequency)", "(phase frequency tshift)"},

        {THEORY_BESSEL, THEORY_PARAM_BESSEL, false,
         "bessel", "b", "(phase frequency)", "(phase frequency tshift)"},

        {THEORY_INTERNAL_BESSEL, THEORY_PARAM_INTERNAL_BESSEL, false,
         "internBsl", "ib", "(fraction phase frequency Trate Lrate)", "(fraction phase frequency Trate Lrate tshift)"},

        {THEORY_SKEWED_GAUSS, THEORY_PARAM_SKEWED_GAUSS, false,
         "skewedGss", "skg", "(phase frequency rate_m rate_p)", "(phase frequency rate_m rate_p tshift)"},

        {THEORY_POLYNOM, 0, false,
         "polynom", "p", "(tshift p0 p1 ... pn)", "(tshift p0 p1 ... pn)"},

        {THEORY_USER_FCN, 0, false,
         "userFcn", "u", "", ""}};

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
    virtual double Func(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;

  private:
    virtual int SearchDataBase(TString name);
    virtual void MakeCleanAndTidyTheoryBlock(PMsrLines* fullTheoryBlock);
    virtual void MakeCleanAndTidyPolynom(unsigned int i, PMsrLines* fullTheoryBlock);
    virtual void MakeCleanAndTidyUserFcn(unsigned int i, PMsrLines* fullTheoryBlock);

    virtual double Asymmetry(const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double SimpleExp(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double GeneralExp(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double SimpleGauss(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double StaticGaussKT(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double StaticGaussKTLF(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double DynamicGaussKTLF(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double StaticLorentzKTLF(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double DynamicLorentzKTLF(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double CombiLGKT(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double SpinGlass(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double RandomAnisotropicHyperfine(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double Abragam(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double InternalField(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double TFCos(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double Bessel(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double InternalBessel(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double SkewedGauss(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double Polynom(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;
    virtual double UserFcn(register double t, const PDoubleVector& paramValues, const PDoubleVector& funcValues) const;

    virtual void CalculateGaussLFIntegral(const double *val) const;
    virtual void CalculateLorentzLFIntegral(const double *val) const;
    virtual double GetLFIntegralValue(const double t) const;
    virtual void CalculateDynKTLF(const double *val, int tag) const;
    virtual double GetDynKTLFValue(const double t) const;

    // variables
    bool fValid;
    unsigned int fType;
    vector<unsigned int> fParamNo; ///< holds the parameter numbers for the theory (including maps and functions, see constructor desciption)
    unsigned int fNoOfParam;
    PTheory *fAdd, *fMul;

    TString fUserFcnClassName; ///< name of the user function class for within root
    TString fUserFcnSharedLibName; ///< name of the shared lib to which the user function belongs
    PUserFcnBase *fUserFcn;    ///< pointer to the user function object
    mutable PDoubleVector fUserParam;  ///< vector holding the resolved user function parameters, i.e. map and function resolved.

    PMsrHandler *fMsrInfo;

    mutable double fPrevParam[THEORY_MAX_PARAM]; ///< needed for LF-stuff
    mutable PDoubleVector fLFIntegral;           ///< needed for LF-stuff. Keeps the non-analytic integral values
    mutable double fDynLFdt;
    mutable PDoubleVector fDynLFFuncValue;      ///< needed for LF-stuff. Keeps the dynamic LF KT function values
};

#endif //  _PTHEORY_H_
