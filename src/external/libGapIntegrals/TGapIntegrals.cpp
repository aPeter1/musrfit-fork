/***************************************************************************

  TGapIntegrals.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/09/06

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

#include "TIntegrator.h"
#include "TGapIntegrals.h"

#define PI 3.14159265358979323846
#define TWOPI 6.28318530717958647692


ClassImp(TGapSWave)
ClassImp(TGapDWave)
ClassImp(TGapAnSWave)

// s wave  gap integral

TGapSWave::TGapSWave() {
  TGapIntegral *gapint = new TGapIntegral();
  fGapIntegral = gapint;
  gapint = 0;
  delete gapint;

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
  delete gapint;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

TGapAnSWave::TGapAnSWave() {
  TAnSWaveGapIntegralDivonne *gapint = new TAnSWaveGapIntegralDivonne();
  fGapIntegral = gapint;
  gapint = 0;
  delete gapint;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
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

TGapAnSWave::~TGapAnSWave() {
  delete fGapIntegral;
  fGapIntegral = 0;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
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
    intPar.push_back(t);
    intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));

    fGapIntegral->SetParameters(intPar);
    ds = 1.0+2.0*fGapIntegral->IntegrateFunc(0.0, 2.0*(t+intPar[1]));

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
    intPar.push_back(t);
    intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));


//    double xl[] = {0.0, 0.0}; // lower bound E, phi
//    double xu[] = {2.0*(t+intPar[1]), 0.5*PI}; // upper bound E, phi

    fGapIntegral->SetParameters(intPar);
//    ds = 1.0+4.0/PI*fGapIntegral->IntegrateFunc(2, xl, xu);
    ds = 1.0+4.0/PI*fGapIntegral->IntegrateFunc();

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
    vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(t);
    intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    intPar.push_back(par[2]);


//    double xl[] = {0.0, 0.0}; // lower bound E, phi
//    double xu[] = {4.0*(t+intPar[1]), 0.5*PI}; // upper bound E, phi

    fGapIntegral->SetParameters(intPar);
//    ds = 1.0+4.0/PI*fGapIntegral->IntegrateFunc(2, xl, xu);
    ds = 1.0+4.0/PI*fGapIntegral->IntegrateFunc();

    intPar.clear();

    if (newTemp)
      fIntegralValues.push_back(ds);
    else
      fIntegralValues[vectorIndex] = ds;

    fCalcNeeded[vectorIndex] = false;
  }

  return fIntegralValues[vectorIndex];

}
