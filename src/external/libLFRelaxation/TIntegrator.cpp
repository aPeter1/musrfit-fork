/***************************************************************************

  TIntegrator.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/12/03

***************************************************************************/

#include "TIntegrator.h"
#include "TMath.h"

using namespace std;

TIntegrator::TIntegrator() : fFunc(0) {
  ROOT::Math::GSLIntegrator *integrator = new ROOT::Math::GSLIntegrator(ROOT::Math::Integration::kADAPTIVE,ROOT::Math::Integration::kGAUSS51);
  fIntegrator = integrator;
  integrator = 0;
  delete integrator;
}

TIntegrator::~TIntegrator(){
  delete fIntegrator;
  fIntegrator=0;
  fFunc=0;
}

inline double TIntegrator::FuncAtXgsl(double x, void *obj)
{
  return ((TIntegrator*)obj)->FuncAtX(x);
}

double TIntegrator::IntegrateFunc(double x1, double x2)
{
  fFunc = &TIntegrator::FuncAtXgsl;
  return fIntegrator->Integral(fFunc, (this), x1, x2);
}


inline double TIntBesselJ0Exp::FuncAtX(double x) const
{
  return TMath::BesselJ0(TMath::TwoPi()*fPar[0]*x) * TMath::Exp(-fPar[1]*x);
}


inline double TIntSinGss::FuncAtX(double x) const
{
  return TMath::Sin(TMath::TwoPi()*fPar[0]*x) * TMath::Exp(-0.5*fPar[1]*fPar[1]*x*x);
}

