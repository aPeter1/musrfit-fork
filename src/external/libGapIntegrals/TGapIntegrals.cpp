/***************************************************************************

  TGapIntegrals.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

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

#include <cassert>
#include <iostream>

using namespace std;

#include "BMWIntegrator.h"
#include "TGapIntegrals.h"

#define PI 3.14159265358979323846
#define TWOPI 6.28318530717958647692


ClassImp(TGapSWave)
ClassImp(TGapDWave)
ClassImp(TGapCosSqDWave)
ClassImp(TGapSinSqDWave)
ClassImp(TGapAnSWave)
ClassImp(TGapNonMonDWave1)
ClassImp(TGapNonMonDWave2)
ClassImp(TGapPowerLaw)
ClassImp(TGapDirtySWave)

ClassImp(TLambdaSWave)
ClassImp(TLambdaDWave)
ClassImp(TLambdaAnSWave)
ClassImp(TLambdaNonMonDWave1)
ClassImp(TLambdaNonMonDWave2)
ClassImp(TLambdaPowerLaw)

ClassImp(TLambdaInvSWave)
ClassImp(TLambdaInvDWave)
ClassImp(TLambdaInvAnSWave)
ClassImp(TLambdaInvNonMonDWave1)
ClassImp(TLambdaInvNonMonDWave2)
ClassImp(TLambdaInvPowerLaw)

ClassImp(TFilmMagnetizationDWave)

// s wave  gap integral

TGapSWave::TGapSWave() {
  TGapIntegral *gapint = new TGapIntegral();
  fGapIntegral = gapint;
  gapint = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapDWave::TGapDWave() {
  TDWaveGapIntegralCuhre *gapint = new TDWaveGapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapCosSqDWave::TGapCosSqDWave() {
  TCosSqDWaveGapIntegralCuhre *gapint = new TCosSqDWaveGapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapSinSqDWave::TGapSinSqDWave() {
  TSinSqDWaveGapIntegralCuhre *gapint = new TSinSqDWaveGapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapAnSWave::TGapAnSWave() {
  TAnSWaveGapIntegralCuhre *gapint = new TAnSWaveGapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapNonMonDWave1::TGapNonMonDWave1() {
  TNonMonDWave1GapIntegralCuhre *gapint = new TNonMonDWave1GapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapNonMonDWave2::TGapNonMonDWave2() {
  TNonMonDWave2GapIntegralCuhre *gapint = new TNonMonDWave2GapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TLambdaSWave::TLambdaSWave() {
  fLambdaInvSq = new TGapSWave();
}

TLambdaDWave::TLambdaDWave() {
  fLambdaInvSq = new TGapDWave();
}

TLambdaAnSWave::TLambdaAnSWave() {
  fLambdaInvSq = new TGapAnSWave();
}

TLambdaNonMonDWave1::TLambdaNonMonDWave1() {
  fLambdaInvSq = new TGapNonMonDWave1();
}

TLambdaNonMonDWave2::TLambdaNonMonDWave2() {
  fLambdaInvSq = new TGapNonMonDWave2();
}

TLambdaInvSWave::TLambdaInvSWave() {
  fLambdaInvSq = new TGapSWave();
}

TLambdaInvDWave::TLambdaInvDWave() {
  fLambdaInvSq = new TGapDWave();
}

TLambdaInvAnSWave::TLambdaInvAnSWave() {
  fLambdaInvSq = new TGapAnSWave();
}

TLambdaInvNonMonDWave1::TLambdaInvNonMonDWave1() {
  fLambdaInvSq = new TGapNonMonDWave1();
}

TLambdaInvNonMonDWave2::TLambdaInvNonMonDWave2() {
  fLambdaInvSq = new TGapNonMonDWave2();
}

TGapSWave::~TGapSWave() {
  delete fGapIntegral;
  fGapIntegral = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapDWave::~TGapDWave() {
  delete fGapIntegral;
  fGapIntegral = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapCosSqDWave::~TGapCosSqDWave() {
  delete fGapIntegral;
  fGapIntegral = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapSinSqDWave::~TGapSinSqDWave() {
  delete fGapIntegral;
  fGapIntegral = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapAnSWave::~TGapAnSWave() {
  delete fGapIntegral;
  fGapIntegral = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapNonMonDWave1::~TGapNonMonDWave1() {
  delete fGapIntegral;
  fGapIntegral = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapNonMonDWave2::~TGapNonMonDWave2() {
  delete fGapIntegral;
  fGapIntegral = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TLambdaSWave::~TLambdaSWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = 0;
}

TLambdaDWave::~TLambdaDWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = 0;
}

TLambdaAnSWave::~TLambdaAnSWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = 0;
}

TLambdaNonMonDWave1::~TLambdaNonMonDWave1() {
  delete fLambdaInvSq;
  fLambdaInvSq = 0;
}

TLambdaNonMonDWave2::~TLambdaNonMonDWave2() {
  delete fLambdaInvSq;
  fLambdaInvSq = 0;
}

TLambdaInvSWave::~TLambdaInvSWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = 0;
}

TLambdaInvDWave::~TLambdaInvDWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = 0;
}

TLambdaInvAnSWave::~TLambdaInvAnSWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = 0;
}

TLambdaInvNonMonDWave1::~TLambdaInvNonMonDWave1() {
  delete fLambdaInvSq;
  fLambdaInvSq = 0;
}

TLambdaInvNonMonDWave2::~TLambdaInvNonMonDWave2() {
  delete fLambdaInvSq;
  fLambdaInvSq = 0;
}


double TGapSWave::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 2); // two parameters: Tc, Delta(0)

  if (t<=0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  bool integralParChanged(false);

  if (fPar.empty()) { // first time calling this routine
    fPar = par;
    integralParChanged = true;
  } else { // check if Tc or Delta0 have changed
    for (unsigned int i(0); i<par.size(); i++) {
      if (par[i] != fPar[i]) {
        fPar[i] = par[i];
        integralParChanged = true;
      }
    }
  }

  bool newTemp(false);
  unsigned int vectorIndex;

  if (integralParChanged) {
    fCalcNeeded.clear();
    fCalcNeeded.resize(fTemp.size(), true);
  }

  fTempIter = find(fTemp.begin(), fTemp.end(), t);
  if(fTempIter == fTemp.end()) {
    fTemp.push_back(t);
    vectorIndex = fTemp.size() - 1;
    fCalcNeeded.push_back(true);
    newTemp = true;
  } else {
    vectorIndex = fTempIter - fTemp.begin();
  }

  if (fCalcNeeded[vectorIndex]) {

    double ds;

    vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(2.0*0.08617384436*t); // 2 kB T, kB in meV/K
    intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));

    fGapIntegral->SetParameters(intPar);
    ds = 1.0-1.0/intPar[0]*fGapIntegral->IntegrateFunc(0.0, 2.0*(t+intPar[1]));

    intPar.clear();

    if (newTemp)
      fIntegralValues.push_back(ds);
    else
      fIntegralValues[vectorIndex] = ds;

    fCalcNeeded[vectorIndex] = false;
  }

  return fIntegralValues[vectorIndex];

}

double TGapDWave::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 2); // two parameters: Tc, Delta(0)

  if (t<=0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  bool integralParChanged(false);

  if (fPar.empty()) { // first time calling this routine
    fPar = par;
    integralParChanged = true;
  } else { // check if Tc or Delta0 have changed
    for (unsigned int i(0); i<par.size(); i++) {
      if (par[i] != fPar[i]) {
        fPar[i] = par[i];
        integralParChanged = true;
      }
    }
  }

  bool newTemp(false);
  unsigned int vectorIndex;

  if (integralParChanged) {
    fCalcNeeded.clear();
    fCalcNeeded.resize(fTemp.size(), true);
  }

  fTempIter = find(fTemp.begin(), fTemp.end(), t);
  if(fTempIter == fTemp.end()) {
    fTemp.push_back(t);
    vectorIndex = fTemp.size() - 1;
    fCalcNeeded.push_back(true);
    newTemp = true;
  } else {
    vectorIndex = fTempIter - fTemp.begin();
  }

  if (fCalcNeeded[vectorIndex]) {

    double ds;
    vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(2.0*0.08617384436*t); // 2 kB T, kB in meV/K
    //intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    intPar.push_back(par[1]*tanh(TMath::Pi()*0.08617384436*par[0]/par[1]*sqrt(4.0/3.0*(par[0]/t-1.0))));
    intPar.push_back(4.0*(t+intPar[1])); // upper limit of energy-integration: cutoff energy
    intPar.push_back(TMath::PiOver2()); // upper limit of phi-integration

//    double xl[] = {0.0, 0.0}; // lower bound E, phi
//    double xu[] = {4.0*(t+intPar[1]), 0.5*PI}; // upper bound E, phi

    fGapIntegral->SetParameters(intPar);
//    ds = 1.0+4.0/PI*fGapIntegral->IntegrateFunc(2, xl, xu);
    ds = 1.0-intPar[2]/intPar[0]*fGapIntegral->IntegrateFunc();

    intPar.clear();

    if (newTemp)
      fIntegralValues.push_back(ds);
    else
      fIntegralValues[vectorIndex] = ds;

    fCalcNeeded[vectorIndex] = false;
  }

  return fIntegralValues[vectorIndex];

}

double TGapCosSqDWave::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 3); // three parameters: Tc, DeltaD(0), DeltaS(0)

  if (t<=0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  bool integralParChanged(false);

  if (fPar.empty()) { // first time calling this routine
    fPar = par;
    integralParChanged = true;
  } else { // check if Tc or Delta0 have changed
    for (unsigned int i(0); i<par.size(); i++) {
      if (par[i] != fPar[i]) {
        fPar[i] = par[i];
        integralParChanged = true;
      }
    }
  }

  bool newTemp(false);
  unsigned int vectorIndex;

  if (integralParChanged) {
    fCalcNeeded.clear();
    fCalcNeeded.resize(fTemp.size(), true);
  }

  fTempIter = find(fTemp.begin(), fTemp.end(), t);
  if(fTempIter == fTemp.end()) {
    fTemp.push_back(t);
    vectorIndex = fTemp.size() - 1;
    fCalcNeeded.push_back(true);
    newTemp = true;
  } else {
    vectorIndex = fTempIter - fTemp.begin();
  }

  if (fCalcNeeded[vectorIndex]) {

    double ds;
    vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(2.0*0.08617384436*t); // 2 kB T, kB in meV/K
//    intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    intPar.push_back(par[1]*tanh(TMath::Pi()*0.08617384436*par[0]/par[1]*sqrt(4.0/3.0*(par[0]/t-1.0)))); // DeltaD(T)
    intPar.push_back(1.0*(t+intPar[1])); // upper limit of energy-integration: cutoff energy
    intPar.push_back(TMath::Pi()); // upper limit of phi-integration
    intPar.push_back(par[2]*tanh(TMath::Pi()*0.08617384436*par[0]/par[2]*sqrt((par[0]/t-1.0)))); // DeltaS(T)

//    double xl[] = {0.0, 0.0}; // lower bound E, phi
//    double xu[] = {4.0*(t+intPar[1]), 0.5*PI}; // upper bound E, phi

    fGapIntegral->SetParameters(intPar);
//    ds = 1.0+4.0/PI*fGapIntegral->IntegrateFunc(2, xl, xu);
    ds = 1.0-2.0*intPar[2]/intPar[0]*fGapIntegral->IntegrateFunc();

    intPar.clear();

    if (newTemp)
      fIntegralValues.push_back(ds);
    else
      fIntegralValues[vectorIndex] = ds;

    fCalcNeeded[vectorIndex] = false;
  }

  return fIntegralValues[vectorIndex];

}

double TGapSinSqDWave::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 3); // two parameters: Tc, Delta(0), s-weight

  if (t<=0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  bool integralParChanged(false);

  if (fPar.empty()) { // first time calling this routine
    fPar = par;
    integralParChanged = true;
  } else { // check if Tc or Delta0 have changed
    for (unsigned int i(0); i<par.size(); i++) {
      if (par[i] != fPar[i]) {
        fPar[i] = par[i];
        integralParChanged = true;
      }
    }
  }

  bool newTemp(false);
  unsigned int vectorIndex;

  if (integralParChanged) {
    fCalcNeeded.clear();
    fCalcNeeded.resize(fTemp.size(), true);
  }

  fTempIter = find(fTemp.begin(), fTemp.end(), t);
  if(fTempIter == fTemp.end()) {
    fTemp.push_back(t);
    vectorIndex = fTemp.size() - 1;
    fCalcNeeded.push_back(true);
    newTemp = true;
  } else {
    vectorIndex = fTempIter - fTemp.begin();
  }

  if (fCalcNeeded[vectorIndex]) {

    double ds;
    vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(2.0*0.08617384436*t); // 2 kB T, kB in meV/K
//    intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    intPar.push_back(par[1]*tanh(TMath::Pi()*0.08617384436*par[0]/par[1]*sqrt(4.0/3.0*(par[0]/t-1.0))));
    intPar.push_back(1.0*(t+intPar[1])); // upper limit of energy-integration: cutoff energy
    intPar.push_back(TMath::Pi()); // upper limit of phi-integration
    intPar.push_back(par[2]*tanh(TMath::Pi()*0.08617384436*par[0]/par[2]*sqrt((par[0]/t-1.0)))); // DeltaS(T)

//    double xl[] = {0.0, 0.0}; // lower bound E, phi
//    double xu[] = {4.0*(t+intPar[1]), 0.5*PI}; // upper bound E, phi

    fGapIntegral->SetParameters(intPar);
//    ds = 1.0+4.0/PI*fGapIntegral->IntegrateFunc(2, xl, xu);
    ds = 1.0-2.0*intPar[2]/intPar[0]*fGapIntegral->IntegrateFunc();

    intPar.clear();

    if (newTemp)
      fIntegralValues.push_back(ds);
    else
      fIntegralValues[vectorIndex] = ds;

    fCalcNeeded[vectorIndex] = false;
  }

  return fIntegralValues[vectorIndex];

}

double TGapAnSWave::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 3); // three parameters: Tc, Delta(0), a

  if (t<=0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  bool integralParChanged(false);

  if (fPar.empty()) { // first time calling this routine
    fPar = par;
    integralParChanged = true;
  } else { // check if Tc or Delta0 have changed
    for (unsigned int i(0); i<par.size(); i++) {
      if (par[i] != fPar[i]) {
        fPar[i] = par[i];
        integralParChanged = true;
      }
    }
  }

  bool newTemp(false);
  unsigned int vectorIndex;

  if (integralParChanged) {
    fCalcNeeded.clear();
    fCalcNeeded.resize(fTemp.size(), true);
  }

  fTempIter = find(fTemp.begin(), fTemp.end(), t);
  if(fTempIter == fTemp.end()) {
    fTemp.push_back(t);
    vectorIndex = fTemp.size() - 1;
    fCalcNeeded.push_back(true);
    newTemp = true;
  } else {
    vectorIndex = fTempIter - fTemp.begin();
  }

  if (fCalcNeeded[vectorIndex]) {

    double ds;
    vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(2.0*0.08617384436*t); // 2 kB T, kB in meV/K
    intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    intPar.push_back(par[2]);
    intPar.push_back(4.0*(t+(1.0+par[2])*intPar[1])); // upper limit of energy-integration: cutoff energy
    intPar.push_back(TMath::PiOver2()); // upper limit of phi-integration

//    double xl[] = {0.0, 0.0}; // lower bound E, phi
//    double xu[] = {4.0*(t+intPar[1]), 0.5*PI}; // upper bound E, phi

    fGapIntegral->SetParameters(intPar);
//    ds = 1.0+4.0/PI*fGapIntegral->IntegrateFunc(2, xl, xu);
    ds = 1.0-intPar[3]/intPar[0]*fGapIntegral->IntegrateFunc();

    intPar.clear();

    if (newTemp)
      fIntegralValues.push_back(ds);
    else
      fIntegralValues[vectorIndex] = ds;

    fCalcNeeded[vectorIndex] = false;
  }

  return fIntegralValues[vectorIndex];

}

double TGapNonMonDWave1::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 3); // two parameters: Tc, Delta(0), a

  if (t<=0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  bool integralParChanged(false);

  if (fPar.empty()) { // first time calling this routine
    fPar = par;
    integralParChanged = true;
  } else { // check if Tc or Delta0 have changed
    for (unsigned int i(0); i<par.size(); i++) {
      if (par[i] != fPar[i]) {
        fPar[i] = par[i];
        integralParChanged = true;
      }
    }
  }

  bool newTemp(false);
  unsigned int vectorIndex;

  if (integralParChanged) {
    fCalcNeeded.clear();
    fCalcNeeded.resize(fTemp.size(), true);
  }

  fTempIter = find(fTemp.begin(), fTemp.end(), t);
  if(fTempIter == fTemp.end()) {
    fTemp.push_back(t);
    vectorIndex = fTemp.size() - 1;
    fCalcNeeded.push_back(true);
    newTemp = true;
  } else {
    vectorIndex = fTempIter - fTemp.begin();
  }

  if (fCalcNeeded[vectorIndex]) {

    double ds;
    vector<double> intPar; // parameters for the integral: 2 k_B T, Delta(T), a, E_c, phi_c
    intPar.push_back(2.0*0.08617384436*t); // 2 kB T, kB in meV/K
    intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    intPar.push_back(par[2]);
    intPar.push_back(4.0*(t+intPar[1])); // upper limit of energy-integration: cutoff energy
    intPar.push_back(TMath::PiOver2()); // upper limit of phi-integration

    fGapIntegral->SetParameters(intPar);

    ds = 1.0-intPar[3]/intPar[0]*fGapIntegral->IntegrateFunc();

    intPar.clear();

    if (newTemp)
      fIntegralValues.push_back(ds);
    else
      fIntegralValues[vectorIndex] = ds;

    fCalcNeeded[vectorIndex] = false;
  }

  return fIntegralValues[vectorIndex];

}

double TGapNonMonDWave2::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 3); // two parameters: Tc, Delta(0), a

  if (t<=0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  bool integralParChanged(false);

  if (fPar.empty()) { // first time calling this routine
    fPar = par;
    integralParChanged = true;
  } else { // check if Tc or Delta0 have changed
    for (unsigned int i(0); i<par.size(); i++) {
      if (par[i] != fPar[i]) {
        fPar[i] = par[i];
        integralParChanged = true;
      }
    }
  }

  bool newTemp(false);
  unsigned int vectorIndex;

  if (integralParChanged) {
    fCalcNeeded.clear();
    fCalcNeeded.resize(fTemp.size(), true);
  }

  fTempIter = find(fTemp.begin(), fTemp.end(), t);
  if(fTempIter == fTemp.end()) {
    fTemp.push_back(t);
    vectorIndex = fTemp.size() - 1;
    fCalcNeeded.push_back(true);
    newTemp = true;
  } else {
    vectorIndex = fTempIter - fTemp.begin();
  }

  if (fCalcNeeded[vectorIndex]) {

    double ds;
    vector<double> intPar; // parameters for the integral: 2 k_B T, Delta(T), a, E_c, phi_c
    intPar.push_back(2.0*0.08617384436*t); // 2 kB T, kB in meV/K
    intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    intPar.push_back(par[2]);
    intPar.push_back(4.0*(t+intPar[1])); // upper limit of energy-integration: cutoff energy
    intPar.push_back(TMath::PiOver2()); // upper limit of phi-integration

    fGapIntegral->SetParameters(intPar);

    ds = 1.0-intPar[3]/intPar[0]*fGapIntegral->IntegrateFunc();

    intPar.clear();

    if (newTemp)
      fIntegralValues.push_back(ds);
    else
      fIntegralValues[vectorIndex] = ds;

    fCalcNeeded[vectorIndex] = false;
  }

  return fIntegralValues[vectorIndex];

}

double TGapPowerLaw::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 2); // two parameters: Tc, N

  if(t<=0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  return 1.0 - pow(t/par[0], par[1]);

}


double TGapDirtySWave::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 2); // two parameters: Tc, Delta(0)

  if (t<=0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  double deltaT(tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));

  return deltaT*tanh(par[1]*deltaT/(2.0*0.08617384436*t)); // Delta(T)/Delta(0)*tanh(Delta(T)/2 kB T), kB in meV/K

}


double TLambdaSWave::operator()(double t, const vector<double> &par) const
{
  assert(par.size() == 2); // two parameters: Tc, Delta0

  if (t >= par[0])
    return -1.0;

  if (t <= 0.0)
    return 1.0;

  return 1.0/sqrt((*fLambdaInvSq)(t, par));

}

double TLambdaDWave::operator()(double t, const vector<double> &par) const
{
  assert(par.size() == 2); // two parameters: Tc, Delta0

  if (t >= par[0])
    return -1.0;

  if (t <= 0.0)
    return 1.0;

  return 1.0/sqrt((*fLambdaInvSq)(t, par));

}

double TLambdaAnSWave::operator()(double t, const vector<double> &par) const
{
  assert(par.size() == 3); // three parameters: Tc, Delta0, a

  if (t >= par[0])
    return -1.0;

  if (t <= 0.0)
    return 1.0;

  return 1.0/sqrt((*fLambdaInvSq)(t, par));

}

double TLambdaNonMonDWave1::operator()(double t, const vector<double> &par) const
{
  assert(par.size() == 3); // three parameters: Tc, Delta0, a

  if (t >= par[0])
    return -1.0;

  if (t <= 0.0)
    return 1.0;

  return 1.0/sqrt((*fLambdaInvSq)(t, par));

}

double TLambdaNonMonDWave2::operator()(double t, const vector<double> &par) const
{
  assert(par.size() == 3); // three parameters: Tc, Delta0, a

  if (t >= par[0])
    return -1.0;

  if (t <= 0.0)
    return 1.0;

  return 1.0/sqrt((*fLambdaInvSq)(t, par));

}

double TLambdaPowerLaw::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 2); // two parameters: Tc, N

  if(t <= 0.0)
    return 1.0;
  else if (t >= par[0])
    return -1.0;

  return 1.0/sqrt(1.0 - pow(t/par[0], par[1]));

}


double TLambdaInvSWave::operator()(double t, const vector<double> &par) const
{
  assert(par.size() == 2); // two parameters: Tc, Delta0

  if (t >= par[0])
    return 0.0;

  if (t <= 0.0)
    return 1.0;

  return sqrt((*fLambdaInvSq)(t, par));

}

double TLambdaInvDWave::operator()(double t, const vector<double> &par) const
{
  assert(par.size() == 2); // two parameters: Tc, Delta0

  if (t >= par[0])
    return 0.0;

  if (t <= 0.0)
    return 1.0;

  return sqrt((*fLambdaInvSq)(t, par));

}

double TLambdaInvAnSWave::operator()(double t, const vector<double> &par) const
{
  assert(par.size() == 3); // three parameters: Tc, Delta0, a

  if (t >= par[0])
    return 0.0;

  if (t <= 0.0)
    return 1.0;

  return sqrt((*fLambdaInvSq)(t, par));

}

double TLambdaInvNonMonDWave1::operator()(double t, const vector<double> &par) const
{
  assert(par.size() == 3); // three parameters: Tc, Delta0, a

  if (t >= par[0])
    return 0.0;

  if (t <= 0.0)
    return 1.0;

  return sqrt((*fLambdaInvSq)(t, par));

}

double TLambdaInvNonMonDWave2::operator()(double t, const vector<double> &par) const
{
  assert(par.size() == 3); // three parameters: Tc, Delta0, a

  if (t >= par[0])
    return 0.0;

  if (t <= 0.0)
    return 1.0;

  return sqrt((*fLambdaInvSq)(t, par));

}

double TLambdaInvPowerLaw::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 2); // two parameters: Tc, N

  if(t <= 0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  return sqrt(1.0 - pow(t/par[0], par[1]));

}

TFilmMagnetizationDWave::TFilmMagnetizationDWave()
{
  fLambdaInvSq = new TGapDWave();
  fPar.clear();
}

TFilmMagnetizationDWave::~TFilmMagnetizationDWave()
{
  delete fLambdaInvSq;
  fLambdaInvSq = 0;
  fPar.clear();
}

double TFilmMagnetizationDWave::operator()(double t, const vector<double> &par) const
{
  assert(par.size() == 4); // four parameters: Tc, Delta0, lambda0, film-thickness

  fPar = par;

  if (t >= fPar[0])
    return 0.0;

  vector<double> parForGapIntegral;
  parForGapIntegral.push_back(fPar[0]);
  parForGapIntegral.push_back(fPar[1]);

  double d_2l(0.5*fPar[3]/fPar[2]*sqrt((*fLambdaInvSq)(t, parForGapIntegral)));

  parForGapIntegral.clear();

  return tanh(d_2l)/d_2l - 1.0;

}


