/***************************************************************************

  TIntegrator.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/12/03

***************************************************************************/

#ifndef _TIntegrator_H_
#define _TIntegrator_H_

#include "Math/GSLIntegrator.h"

#include<vector>

using namespace std;

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

class TIntBesselJ0Exp : public TIntegrator {
  public:
    TIntBesselJ0Exp() {}
    ~TIntBesselJ0Exp() {}
    double FuncAtX(double) const;
};

class TIntSinGss : public TIntegrator {
  public:
    TIntSinGss() {}
    ~TIntSinGss() {}
    double FuncAtX(double) const;
};

#endif //_TIntegrator_H_
