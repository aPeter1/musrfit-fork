/***************************************************************************

  TIntegrator.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/12/24

***************************************************************************/

#include "TIntegrator.h"

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



