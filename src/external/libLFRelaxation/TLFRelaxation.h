/***************************************************************************

  TLFRelaxation.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/12/04

***************************************************************************/

#ifndef _TLFRelaxation_H_
#define _TLFRelaxation_H_

#include<vector>
#include<cstdio>

using namespace std;

#include "TMath.h"
#include "PUserFcnBase.h"
#include "fftw3.h"
#include "TIntegrator.h"

class TLFStatGssKT : public PUserFcnBase {

public:
  TLFStatGssKT();
  ~TLFStatGssKT();

  double operator()(double, const vector<double>&) const;

private:
  TIntSinGss *fIntSinGss;

  ClassDef(TLFStatGssKT,1)
};

class TLFStatLorKT : public PUserFcnBase {

public:
  TLFStatLorKT();
  ~TLFStatLorKT();

  double operator()(double, const vector<double>&) const;

private:
  TIntBesselJ0Exp *fIntBesselJ0Exp;

  ClassDef(TLFStatLorKT,1)
};

class TLFDynGssKT : public PUserFcnBase {

public:
  TLFDynGssKT();
  ~TLFDynGssKT();

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;
  mutable bool fCalcNeeded;
  mutable bool fFirstCall;
  string fWisdom;
  unsigned int fNSteps;
  double fDt;
  double fDw;
  double fC;
  fftw_plan fFFTplanFORW;
  fftw_plan fFFTplanBACK;
  double *fFFTtime;
  fftw_complex *fFFTfreq;
  mutable unsigned int fCounter;

  ClassDef(TLFDynGssKT,1)
};

class TLFDynLorKT : public PUserFcnBase {

public:
  TLFDynLorKT();
  ~TLFDynLorKT();

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;
  mutable bool fCalcNeeded;
  mutable bool fFirstCall;
  string fWisdom;
  unsigned int fNSteps;
  double fDt;
  double fDw;
  double fC;
  fftw_plan fFFTplanFORW;
  fftw_plan fFFTplanBACK;
  double *fFFTtime;
  fftw_complex *fFFTfreq;
  mutable unsigned int fCounter;

  ClassDef(TLFDynLorKT,1)
};

#endif //_LFRelaxation_H_
