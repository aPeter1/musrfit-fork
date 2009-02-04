/***************************************************************************

  TIntegrator.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/12/24

***************************************************************************/

#ifndef _TIntegrator_H_
#define _TIntegrator_H_

#include "Math/GSLIntegrator.h"
#include "TMath.h"

#include <vector>

using namespace std;

// Integrator base class - the function to be integrated have to be implemented in a derived class

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

#endif //_TIntegrator_H_
