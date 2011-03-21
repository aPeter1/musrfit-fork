/***************************************************************************

  BMWIntegrator.h

  Author: Bastian M. Wojek

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Bastian M. Wojek                                *
 *   bastian.wojek@psi.ch                                                  *
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

#ifndef _BMWIntegrator_H_
#define _BMWIntegrator_H_

#include "Math/GSLIntegrator.h"
#include "Math/GSLMCIntegrator.h"
#include "TMath.h"

#include <cmath>
#include <vector>

using namespace std;

/**
 * <p>Base class for 1D integrations using the GNU Scientific Library integrator.
 *    The function which should be integrated has to be implemented in a derived class.
 *    Note: The purpose of this is to offer an easy-to-use interface---not the most efficient integration routine.
 */
class TIntegrator {
  public:
    TIntegrator();
    virtual ~TIntegrator();
    void SetParameters(const std::vector<double> &par) const { fPar=par; }
    virtual double FuncAtX(double) const = 0;
    double IntegrateFunc(double, double);

  protected:
    mutable vector<double> fPar; ///< parameters of the integrand

  private:
    static double FuncAtXgsl(double, void *);
    ROOT::Math::GSLIntegrator *fIntegrator; ///< pointer to the GSL integrator
    mutable double (*fFunc)(double, void *); ///< pointer to the integrand function
};

/**
 * <p>Constructor of the base class for 1D integrations
 *    Allocation of memory for an integration using the adaptive 31 point Gauss-Kronrod rule
 */
inline TIntegrator::TIntegrator() : fFunc(0) {
  fIntegrator = new ROOT::Math::GSLIntegrator(ROOT::Math::Integration::kADAPTIVE,ROOT::Math::Integration::kGAUSS31);
}

/**
 * <p>Destructor of the base class for 1D integrations
 *    Clean up.
 */
inline TIntegrator::~TIntegrator(){
  fPar.clear();
  delete fIntegrator;
  fIntegrator=0;
  fFunc=0;
}

/**
 * <p>Method for passing the integrand function value to the integrator.
 *
 * <p><b>return:</b>
 * - function value of the integrand
 *
 * \param x point at which the function value is calculated
 * \param obj pointer to the integrator
 */
inline double TIntegrator::FuncAtXgsl(double x, void *obj)
{
  return ((TIntegrator*)obj)->FuncAtX(x);
}

/**
 * <p>Calculate the integral of the function between the given boundaries
 *
 * <p><b>return:</b>
 * - value of the integral
 *
 * \param x1 lower boundary
 * \param x2 upper boundary
 */
inline double TIntegrator::IntegrateFunc(double x1, double x2)
{
  fFunc = &TIntegrator::FuncAtXgsl;
  return fIntegrator->Integral(fFunc, (this), x1, x2);
}

/**
 * <p>Base class for multidimensional Monte-Carlo integrations using the GNU Scientific Library integrator.
 *    The function which should be integrated has to be implemented in a derived class.
 *    Note: The purpose of this is to offer an easy-to-use interface---not the most efficient integration routine.
 */
class TMCIntegrator {
  public:
    TMCIntegrator();
    virtual ~TMCIntegrator();
    void SetParameters(const std::vector<double> &par) const { fPar=par; }
    virtual double FuncAtX(double *) const = 0;
    double IntegrateFunc(size_t, double *, double *);

  protected:
    mutable vector<double> fPar; ///< parameters of the integrand

  private:
    static double FuncAtXgsl(double *, size_t, void *);
    ROOT::Math::GSLMCIntegrator *fMCIntegrator; ///< pointer to the GSL integrator
    mutable double (*fFunc)(double *, size_t, void *); ///< pointer to the integrand function
};

/**
 * <p>Constructor of the base class for multidimensional Monte-Carlo integrations
 *    Allocation of memory for an integration using the MISER algorithm of Press and Farrar
 */
inline TMCIntegrator::TMCIntegrator() : fFunc(0) {
  fMCIntegrator = new ROOT::Math::GSLMCIntegrator(ROOT::Math::MCIntegration::kMISER, 1.E-6, 1.E-4, 500000);
}

/**
 * <p>Destructor of the base class for 1D integrations
 *    Clean up.
 */
inline TMCIntegrator::~TMCIntegrator(){
  fPar.clear();
  delete fMCIntegrator;
  fMCIntegrator=0;
  fFunc=0;
}

/**
 * <p>Method for passing the integrand function value to the integrator.
 *
 * <p><b>return:</b>
 * - function value of the integrand
 *
 * \param x point at which the function value is calculated
 * \param dim number of dimensions
 * \param obj pointer to the integrator
 */
inline double TMCIntegrator::FuncAtXgsl(double *x, size_t dim, void *obj)
{
  return ((TMCIntegrator*)obj)->FuncAtX(x);
}

/**
 * <p>Calculate the integral of the function between the given boundaries
 *
 * <p><b>return:</b>
 * - value of the integral
 *
 * \param dim number of dimensions
 * \param x1 lower boundary array
 * \param x2 upper boundary array
 */
inline double TMCIntegrator::IntegrateFunc(size_t dim, double *x1, double *x2)
{
  fFunc = &TMCIntegrator::FuncAtXgsl;
  return fMCIntegrator->Integral(fFunc, dim, x1, x2, (this));
}

/**
 * <p>Two-dimensional integrator class for the efficient calculation of the superfluid density within the semi-classical model
 *    assuming a cylindrical Fermi surface and a d_{x^2-y^2} symmetry of the superconducting order parameter.
 *    The integration uses the Cuhre algorithm of the Cuba library.
 */
class TDWaveGapIntegralCuhre {
  public:
    TDWaveGapIntegralCuhre() : fNDim(2) {}
    ~TDWaveGapIntegralCuhre() { fPar.clear(); }
    void SetParameters(const std::vector<double> &par) { fPar=par; }
    static int Integrand(const int*, const double[], const int*, double[], void*);
    double IntegrateFunc();

  protected:
    static vector<double> fPar; ///< parameters of the integrand
    unsigned int fNDim; ///< dimension of the integral
};

/**
 * <p>Two-dimensional integrator class for the efficient calculation of the superfluid density along the a-axis
 *    within the semi-classical model assuming a cylindrical Fermi surface and a mixed d_{x^2-y^2} + s symmetry of the
 *    superconducting order parameter (effectively: d_{x^2-y^2} with shifted nodes and a-b-anisotropy).
 *    The integration uses the Cuhre algorithm of the Cuba library.
 */
class TCosSqDWaveGapIntegralCuhre {
  public:
    TCosSqDWaveGapIntegralCuhre() : fNDim(2) {}
    ~TCosSqDWaveGapIntegralCuhre() { fPar.clear(); }
    void SetParameters(const std::vector<double> &par) { fPar=par; }
    static int Integrand(const int*, const double[], const int*, double[], void*);
    double IntegrateFunc();

  protected:
    static vector<double> fPar; ///< parameters of the integrand
    unsigned int fNDim; ///< dimension of the integral
};

/**
 * <p>Two-dimensional integrator class for the efficient calculation of the superfluid density along the b-axis
 *    within the semi-classical model assuming a cylindrical Fermi surface and a mixed d_{x^2-y^2} + s symmetry of the
 *    superconducting order parameter (effectively: d_{x^2-y^2} with shifted nodes and a-b-anisotropy).
 *    The integration uses the Cuhre algorithm of the Cuba library.
 */
class TSinSqDWaveGapIntegralCuhre {
  public:
    TSinSqDWaveGapIntegralCuhre() : fNDim(2) {}
    ~TSinSqDWaveGapIntegralCuhre() { fPar.clear(); }
    void SetParameters(const std::vector<double> &par) { fPar=par; }
    static int Integrand(const int*, const double[], const int*, double[], void*);
    double IntegrateFunc();

  protected:
    static vector<double> fPar; ///< parameters of the integrand
    unsigned int fNDim; ///< dimension of the integral
};

/**
 * <p>Two-dimensional integrator class for the efficient calculation of the superfluid density within the semi-classical model
 *    assuming a cylindrical Fermi surface and an "anisotropic s-wave" symmetry of the superconducting order parameter.
 *    The integration uses the Cuhre algorithm of the Cuba library.
 */
class TAnSWaveGapIntegralCuhre {
  public:
    TAnSWaveGapIntegralCuhre() : fNDim(2) {}
    ~TAnSWaveGapIntegralCuhre() { fPar.clear(); }
    void SetParameters(const std::vector<double> &par) { fPar=par; }
    static int Integrand(const int*, const double[], const int*, double[], void*);
    double IntegrateFunc();

  protected:
    static vector<double> fPar; ///< parameters of the integrand
    unsigned int fNDim; ///< dimension of the integral
};

/**
 * <p>Two-dimensional integrator class for the efficient calculation of the superfluid density within the semi-classical model
 *    assuming a cylindrical Fermi surface and an "anisotropic s-wave" symmetry of the superconducting order parameter.
 *    The integration uses the Divonne algorithm of the Cuba library.
 */
class TAnSWaveGapIntegralDivonne {
  public:
    TAnSWaveGapIntegralDivonne() : fNDim(2) {}
    ~TAnSWaveGapIntegralDivonne() { fPar.clear(); }
    void SetParameters(const std::vector<double> &par) { fPar=par; }
    static int Integrand(const int*, const double[], const int*, double[], void*);
    double IntegrateFunc();

  protected:
    static vector<double> fPar; ///< parameters of the integrand
    unsigned int fNDim; ///< dimension of the integral
};

/**
 * <p>Two-dimensional integrator class for the efficient calculation of the superfluid density within the semi-classical model
 *    assuming a cylindrical Fermi surface and an "anisotropic s-wave" symmetry of the superconducting order parameter.
 *    The integration uses the Suave algorithm of the Cuba library.
 */
class TAnSWaveGapIntegralSuave {
  public:
    TAnSWaveGapIntegralSuave() : fNDim(2) {}
    ~TAnSWaveGapIntegralSuave() { fPar.clear(); }
    void SetParameters(const std::vector<double> &par) { fPar=par; }
    static int Integrand(const int*, const double[], const int*, double[], void*);
    double IntegrateFunc();

  protected:
    static vector<double> fPar; ///< parameters of the integrand
    unsigned int fNDim; ///< dimension of the integral
};

/**
 * <p>Two-dimensional integrator class for the efficient calculation of the superfluid density within the semi-classical model
 *    assuming a cylindrical Fermi surface and an "non-monotonic d-wave" symmetry of the superconducting order parameter.
 *    The integration uses the Cuhre algorithm of the Cuba library.
 */
class TNonMonDWave1GapIntegralCuhre {
  public:
    TNonMonDWave1GapIntegralCuhre() : fNDim(2) {}
    ~TNonMonDWave1GapIntegralCuhre() { fPar.clear(); }
    void SetParameters(const std::vector<double> &par) { fPar=par; }
    static int Integrand(const int*, const double[], const int*, double[], void*);
    double IntegrateFunc();

  protected:
    static vector<double> fPar; ///< parameters of the integrand
    unsigned int fNDim; ///< dimension of the integral
};

/**
 * <p>Two-dimensional integrator class for the efficient calculation of the superfluid density within the semi-classical model
 *    assuming a cylindrical Fermi surface and an "non-monotonic d-wave" symmetry of the superconducting order parameter.
 *    The integration uses the Cuhre algorithm of the Cuba library.
 */
class TNonMonDWave2GapIntegralCuhre {
  public:
    TNonMonDWave2GapIntegralCuhre() : fNDim(2) {}
    ~TNonMonDWave2GapIntegralCuhre() { fPar.clear(); }
    void SetParameters(const std::vector<double> &par) { fPar=par; }
    static int Integrand(const int*, const double[], const int*, double[], void*);
    double IntegrateFunc();

  protected:
    static vector<double> fPar; ///< parameters of the integrand
    unsigned int fNDim; ///< dimension of the integral
};

/**
 * <p>Test class for the 2D MC integration
 *    Integral: x*y dx dy
 */
class T2DTest : public TMCIntegrator {
  public:
    T2DTest() {}
    ~T2DTest() {}
    double FuncAtX(double *) const;
};

/**
 * <p>Calculate the function value---actual implementation of the function x*y
 *
 * <p><b>return:</b>
 * - function value
 *
 * \param x point where the function should be evaluated
 */
inline double T2DTest::FuncAtX(double *x) const
{
  return x[0]*x[1];
}

/**
 * <p>Class for the 2D Monte-Carlo integration for the calculation of the superfluid density within the semi-classical model
 *    assuming a cylindrical Fermi surface and a d_{x^2-y^2} symmetry of the superconducting order parameter.
 *    The integration uses the GSL integration routines.
 */
class TDWaveGapIntegral : public TMCIntegrator {
  public:
    TDWaveGapIntegral() {}
    ~TDWaveGapIntegral() {}
    double FuncAtX(double *) const;
};

/**
 * <p>Calculate the function value---actual implementation of the function
 *
 * <p><b>return:</b>
 * - function value
 *
 * \param x point where the function should be evaluated
 */
inline double TDWaveGapIntegral::FuncAtX(double *x) const // x = {E, phi}, fPar = {T, Delta(T)}
{
  double twokt(2.0*0.08617384436*fPar[0]); // kB in meV/K
  double deltasq(TMath::Power(fPar[1]*TMath::Cos(2.0*x[1]),2.0));
  return -1.0/(2.0*twokt*TMath::CosH(TMath::Sqrt(x[0]*x[0]+deltasq)/twokt)*TMath::CosH(TMath::Sqrt(x[0]*x[0]+deltasq)/twokt));
}

/**
 * <p>Class for the 2D Monte-Carlo integration for the calculation of the superfluid density within the semi-classical model
 *    assuming a cylindrical Fermi surface and an "anisotropic s-wave" symmetry of the superconducting order parameter.
 *    The integration uses the GSL integration routines.
 */
class TAnSWaveGapIntegral : public TMCIntegrator {
  public:
    TAnSWaveGapIntegral() {}
    ~TAnSWaveGapIntegral() {}
    double FuncAtX(double *) const;
};

/**
 * <p>Calculate the function value---actual implementation of the function
 *
 * <p><b>return:</b>
 * - function value
 *
 * \param x point where the function should be evaluated
 */
inline double TAnSWaveGapIntegral::FuncAtX(double *x) const // x = {E, phi}, fPar = {T, Delta(T), a}
{
  double twokt(2.0*0.08617384436*fPar[0]); // kB in meV/K
  double deltasq(TMath::Power(fPar[1]*(1.0+fPar[2]*TMath::Cos(4.0*x[1])),2.0));
  return -1.0/(2.0*twokt*TMath::CosH(TMath::Sqrt(x[0]*x[0]+deltasq)/twokt)*TMath::CosH(TMath::Sqrt(x[0]*x[0]+deltasq)/twokt));
}

/**
 * <p>Class for the 1D integration of j0(a*x)*exp(-b*x)
 *    The integration uses the GSL integration routines.
 */
class TIntBesselJ0Exp : public TIntegrator {
  public:
    TIntBesselJ0Exp() {}
    ~TIntBesselJ0Exp() {}
    double FuncAtX(double) const;
};

/**
 * <p>Calculate the function value---actual implementation of the function j0(a*x)*exp(-b*x)
 *
 * <p><b>return:</b>
 * - function value
 *
 * \param x point where the function should be evaluated
 */
inline double TIntBesselJ0Exp::FuncAtX(double x) const
{
  double w0t(TMath::TwoPi()*fPar[0]*x);
  double j0;
  if (fabs(w0t) < 0.001) { // check zero time limits of the spherical bessel functions j0(x) and j1(x)
    j0 = 1.0;
  } else {
    j0 = TMath::Sin(w0t)/w0t;
  }
  return j0 * TMath::Exp(-fPar[1]*x);
}

/**
 * <p>Class for the 1D integration of sin(a*x)*exp(-b*x*x)
 *    The integration uses the GSL integration routines.
 */
class TIntSinGss : public TIntegrator {
  public:
    TIntSinGss() {}
    ~TIntSinGss() {}
    double FuncAtX(double) const;
};

/**
 * <p>Calculate the function value---actual implementation of the function sin(a*x)*exp(-b*x*x)
 *
 * <p><b>return:</b>
 * - function value
 *
 * \param x point where the function should be evaluated
 */
inline double TIntSinGss::FuncAtX(double x) const
{
  return TMath::Sin(TMath::TwoPi()*fPar[0]*x) * TMath::Exp(-0.5*fPar[1]*fPar[1]*x*x);
}

/**
 * <p>Class for the 1D integration of the "DeRenzi Spin Glass Interpolation Integrand"
 *    See Eq. (5) of R. De Renzi and S. Fanesi, Physica B 289-290, 209-212 (2000).
 *    doi:10.1016/S0921-4526(00)00368-9
 *    The integration uses the GSL integration routines.
 */
class TIntSGInterpolation : public TIntegrator {
  public:
    TIntSGInterpolation() {}
    ~TIntSGInterpolation() {}
    double FuncAtX(double) const;
};

/**
 * <p>Calculate the function value---actual implementation of the function
 *
 * <p><b>return:</b>
 * - function value
 *
 * \param x point where the function should be evaluated
 */
inline double TIntSGInterpolation::FuncAtX(double x) const
{
  // Parameters: nu_L [MHz], a [1/us], lambda [1/us], beta [1], t [us]
  double wt(TMath::TwoPi()*fPar[0]*x);
  double expo(0.5*fPar[1]*fPar[1]*x*x/fPar[3]+fPar[2]*fPar[4]);
  return (wt*TMath::Cos(wt)-TMath::Sin(wt))/(wt*wt)*TMath::Exp(-TMath::Power(expo,fPar[3]))/TMath::Power(expo,(1.0-fPar[3]));
}

/**
 * <p>Class for the 1D integration for the calculation of the superfluid density within the semi-classical model
 *    assuming a cylindrical Fermi surface and an isotropic s-wave symmetry of the superconducting order parameter.
 *    The integration uses the GSL integration routines.
 */
class TGapIntegral : public TIntegrator {
  public:
    TGapIntegral() {}
    ~TGapIntegral() {}
    double FuncAtX(double) const; // variable: E
};

/**
 * <p>Calculate the function value---actual implementation of the function df/dE * E / sqrt(E^2 - Delta^2)
 *
 * <p><b>return:</b>
 * - function value
 *
 * \param x point where the function should be evaluated
 */
inline double TGapIntegral::FuncAtX(double e) const
{
  return 1.0/(TMath::Power(TMath::CosH(TMath::Sqrt(e*e+fPar[1]*fPar[1])/fPar[0]),2.0));
}

#endif //_TIntegrator_H_
