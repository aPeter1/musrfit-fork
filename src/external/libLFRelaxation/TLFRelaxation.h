/***************************************************************************

  TLFRelaxation.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/12/04

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

#ifndef _TLFRelaxation_H_
#define _TLFRelaxation_H_

#include<vector>
#include<cstdio>
#include<cmath>

using namespace std;

//#include "gsl/gsl_math.h"
//#include "gsl/gsl_sf_exp.h"
//#include "gsl/gsl_sf_log.h"
//#include "gsl/gsl_sf_trig.h"
//#include "gsl/gsl_sf_bessel.h"


//#include "TMath.h"
#include "PUserFcnBase.h"
#include "fftw3.h"
#include "../BMWIntegrator/BMWIntegrator.h"

class TLFStatGssKT : public PUserFcnBase {

public:
  TLFStatGssKT();
  ~TLFStatGssKT();

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TIntSinGss *fIntSinGss;

  ClassDef(TLFStatGssKT,1)
};

class TLFStatLorKT : public PUserFcnBase {

public:
  TLFStatLorKT();
  ~TLFStatLorKT();

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TIntBesselJ0Exp *fIntBesselJ0Exp;

  ClassDef(TLFStatLorKT,1)
};

class TLFDynGssKT : public PUserFcnBase {

public:
  TLFDynGssKT();
  ~TLFDynGssKT();

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

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
  fftwf_plan fFFTplanFORW;
  fftwf_plan fFFTplanBACK;
  float *fFFTtime;
  fftwf_complex *fFFTfreq;
  mutable unsigned int fCounter;

  ClassDef(TLFDynGssKT,1)
};

class TLFDynLorKT : public PUserFcnBase {

public:
  TLFDynLorKT();
  ~TLFDynLorKT();

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

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
  fftwf_plan fFFTplanFORW;
  fftwf_plan fFFTplanBACK;
  float *fFFTtime;
  fftwf_complex *fFFTfreq;
  mutable unsigned int fCounter;
  mutable double fL1;
  mutable double fL2;

  ClassDef(TLFDynLorKT,1)
};

class TLFDynSG : public PUserFcnBase {

public:
  TLFDynSG();
  ~TLFDynSG();

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;
  vector<TLFDynGssKT*> fLFDynGssIntegral;

  ClassDef(TLFDynSG,1)
};

class TLFSGInterpolation : public PUserFcnBase {

public:
  TLFSGInterpolation();
  ~TLFSGInterpolation();

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TIntSGInterpolation *fIntegral;

  ClassDef(TLFSGInterpolation,1)
};


#endif //_LFRelaxation_H_
