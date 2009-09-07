/***************************************************************************

  TIntegrator.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/09/07

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

#ifndef _TIntegrator_H_
#define _TIntegrator_H_

#include "Math/GSLIntegrator.h"
#include "Math/GSLMCIntegrator.h"
#include "TMath.h"

#include <vector>

using namespace std;

// 1D Integrator base class - the function to be integrated have to be implemented in a derived class

class TIntegrator {
  public:
    TIntegrator();
    virtual ~TIntegrator();
    void SetParameters(const std::vector<double> &par) const { fPar=par; }
    virtual double FuncAtX(double) const = 0;
    double IntegrateFunc(double, double);

  protected:
    mutable vector<double> fPar;

  private:
    static double FuncAtXgsl(double, void *);
    ROOT::Math::GSLIntegrator *fIntegrator;
    mutable double (*fFunc)(double, void *);
};

inline TIntegrator::TIntegrator() : fFunc(0) {
  ROOT::Math::GSLIntegrator *integrator = new ROOT::Math::GSLIntegrator(ROOT::Math::Integration::kADAPTIVE,ROOT::Math::Integration::kGAUSS31);
  fIntegrator = integrator;
  integrator = 0;
  delete integrator;
}

inline TIntegrator::~TIntegrator(){
  delete fIntegrator;
  fIntegrator=0;
  fFunc=0;
}

inline double TIntegrator::FuncAtXgsl(double x, void *obj)
{
  return ((TIntegrator*)obj)->FuncAtX(x);
}

inline double TIntegrator::IntegrateFunc(double x1, double x2)
{
  fFunc = &TIntegrator::FuncAtXgsl;
  return fIntegrator->Integral(fFunc, (this), x1, x2);
}

// Multi dimensional GSL Monte Carlo Integrations

class TMCIntegrator {
  public:
    TMCIntegrator();
    virtual ~TMCIntegrator();
    void SetParameters(const std::vector<double> &par) const { fPar=par; }
    virtual double FuncAtX(double *) const = 0;
    double IntegrateFunc(unsigned int, double *, double *);

  protected:
    mutable vector<double> fPar;

  private:
    static double FuncAtXgsl(double *, unsigned int, void *);
    ROOT::Math::GSLMCIntegrator *fMCIntegrator;
    mutable double (*fFunc)(double *, unsigned int, void *);
};

inline TMCIntegrator::TMCIntegrator() : fFunc(0) {
  ROOT::Math::GSLMCIntegrator *integrator = new ROOT::Math::GSLMCIntegrator(ROOT::Math::MCIntegration::kMISER, 1.E-6, 1.E-4, 500000);
  fMCIntegrator = integrator;
  integrator = 0;
  delete integrator;
}

inline TMCIntegrator::~TMCIntegrator(){
  delete fMCIntegrator;
  fMCIntegrator=0;
  fFunc=0;
}

inline double TMCIntegrator::FuncAtXgsl(double *x, unsigned int dim, void *obj)
{
  return ((TMCIntegrator*)obj)->FuncAtX(x);
}

inline double TMCIntegrator::IntegrateFunc(unsigned int dim, double *x1, double *x2)
{
  fFunc = &TMCIntegrator::FuncAtXgsl;
  return fMCIntegrator->Integral(fFunc, dim, x1, x2, (this));
}

// Multidimensional Integrator class for a d-wave gap integral using the Cuhre algorithm

class TDWaveGapIntegralCuhre {
  public:
    TDWaveGapIntegralCuhre() : fNDim(2) {}
    ~TDWaveGapIntegralCuhre() { fPar.clear(); }
    void SetParameters(const std::vector<double> &par) { fPar=par; }
    static void Integrand(const int*, const double[], const int*, double[]);
    double IntegrateFunc();

  protected:
    static vector<double> fPar;
    unsigned int fNDim;

};

class TAnSWaveGapIntegralCuhre {
  public:
    TAnSWaveGapIntegralCuhre() : fNDim(2) {}
    ~TAnSWaveGapIntegralCuhre() { fPar.clear(); }
    void SetParameters(const std::vector<double> &par) { fPar=par; }
    static void Integrand(const int*, const double[], const int*, double[]);
    double IntegrateFunc();

  protected:
    static vector<double> fPar;
    unsigned int fNDim;

};

class TAnSWaveGapIntegralDivonne {
  public:
    TAnSWaveGapIntegralDivonne() : fNDim(2) {}
    ~TAnSWaveGapIntegralDivonne() { fPar.clear(); }
    void SetParameters(const std::vector<double> &par) { fPar=par; }
    static void Integrand(const int*, const double[], const int*, double[]);
    double IntegrateFunc();

  protected:
    static vector<double> fPar;
    unsigned int fNDim;

};

// To be integrated: x*y dx dy

class T2DTest : public TMCIntegrator {
  public:
    T2DTest() {}
    ~T2DTest() {}
    double FuncAtX(double *) const;
};

inline double T2DTest::FuncAtX(double *x) const
{
  return x[0]*x[1];
}

// To be integrated: d wave gap integral

class TDWaveGapIntegral : public TMCIntegrator {
  public:
    TDWaveGapIntegral() {}
    ~TDWaveGapIntegral() {}
    double FuncAtX(double *) const;
};

inline double TDWaveGapIntegral::FuncAtX(double *x) const // x = {E, phi}, fPar = {T, Delta(T)}
{
  double twokt(2.0*0.08617384436*fPar[0]); // kB in meV/K
  double deltasq(TMath::Power(fPar[1]*TMath::Cos(2.0*x[1]),2.0));
  return -1.0/(2.0*twokt*TMath::CosH(TMath::Sqrt(x[0]*x[0]+deltasq)/twokt)*TMath::CosH(TMath::Sqrt(x[0]*x[0]+deltasq)/twokt));
}

// To be integrated: anisotropic s wave gap integral

class TAnSWaveGapIntegral : public TMCIntegrator {
  public:
    TAnSWaveGapIntegral() {}
    ~TAnSWaveGapIntegral() {}
    double FuncAtX(double *) const;
};

inline double TAnSWaveGapIntegral::FuncAtX(double *x) const // x = {E, phi}, fPar = {T, Delta(T), a}
{
  double twokt(2.0*0.08617384436*fPar[0]); // kB in meV/K
  double deltasq(TMath::Power(fPar[1]*(1.0+fPar[2]*TMath::Cos(4.0*x[1])),2.0));
  return -1.0/(2.0*twokt*TMath::CosH(TMath::Sqrt(x[0]*x[0]+deltasq)/twokt)*TMath::CosH(TMath::Sqrt(x[0]*x[0]+deltasq)/twokt));
}


// To be integrated: Bessel function times Exponential

class TIntBesselJ0Exp : public TIntegrator {
  public:
    TIntBesselJ0Exp() {}
    ~TIntBesselJ0Exp() {}
    double FuncAtX(double) const;
};

inline double TIntBesselJ0Exp::FuncAtX(double x) const
{
  return TMath::BesselJ0(TMath::TwoPi()*fPar[0]*x) * TMath::Exp(-fPar[1]*x);
}

// To be integrated: Sine times Gaussian

class TIntSinGss : public TIntegrator {
  public:
    TIntSinGss() {}
    ~TIntSinGss() {}
    double FuncAtX(double) const;
};

inline double TIntSinGss::FuncAtX(double x) const
{
  return TMath::Sin(TMath::TwoPi()*fPar[0]*x) * TMath::Exp(-0.5*fPar[1]*fPar[1]*x*x);
}

// To be integrated: df/dE * E / sqrt(E^2+ Delta^2)

class TGapIntegral : public TIntegrator {
  public:
    TGapIntegral() {}
    ~TGapIntegral() {}
    double FuncAtX(double) const; // variable: E

};

inline double TGapIntegral::FuncAtX(double e) const
{
  double twokt(2.0*0.08617384436*fPar[0]); // kB in meV/K
//  return -e/(2.0*twokt*TMath::CosH(e/twokt)*TMath::CosH(e/twokt)*TMath::Sqrt(e*e-fPar[1]*fPar[1]));
  return -1.0/(2.0*twokt*TMath::CosH(TMath::Sqrt(e*e+fPar[1]*fPar[1])/twokt)*TMath::CosH(TMath::Sqrt(e*e+fPar[1]*fPar[1])/twokt));
}

#endif //_TIntegrator_H_
