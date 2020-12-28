/***************************************************************************

  TGapIntegrals.cpp

  Author: Bastian M. Wojek / Andreas Suter

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Bastian M. Wojek                                *
 *   bastian.wojek@psi.ch / andreas.suter@psi.ch                           *
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

#include "BMWIntegrator.h"
#include "TGapIntegrals.h"

#define PI 3.14159265358979323846
#define TWOPI 6.28318530717958647692

ClassImp(TGapSWave)
ClassImp(TGapPointPWave)
ClassImp(TGapLinePWave)
ClassImp(TGapDWave)
ClassImp(TGapCosSqDWave)
ClassImp(TGapSinSqDWave)
ClassImp(TGapAnSWave)
ClassImp(TGapNonMonDWave1)
ClassImp(TGapNonMonDWave2)
ClassImp(TGapPowerLaw)
ClassImp(TGapDirtySWave)

ClassImp(TLambdaSWave)
ClassImp(TLambdaPointPWave)
ClassImp(TLambdaLinePWave)
ClassImp(TLambdaDWave)
ClassImp(TLambdaAnSWave)
ClassImp(TLambdaNonMonDWave1)
ClassImp(TLambdaNonMonDWave2)
ClassImp(TLambdaPowerLaw)

ClassImp(TLambdaInvSWave)
ClassImp(TLambdaInvPointPWave)
ClassImp(TLambdaInvLinePWave)
ClassImp(TLambdaInvDWave)
ClassImp(TLambdaInvAnSWave)
ClassImp(TLambdaInvNonMonDWave1)
ClassImp(TLambdaInvNonMonDWave2)
ClassImp(TLambdaInvPowerLaw)

ClassImp(TFilmMagnetizationDWave)

//--------------------------------------------------------------------
/**
 * <p> s wave  gap integral
 */
TGapSWave::TGapSWave() {
  TGapIntegral *gapint = new TGapIntegral();
  fGapIntegral = gapint;
  gapint = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p> point p wave  gap integral
 */
TGapPointPWave::TGapPointPWave() {
  TPointPWaveGapIntegralCuhre *gapint = new TPointPWaveGapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p> line p wave  gap integral
 */
TGapLinePWave::TGapLinePWave() {
  TLinePWaveGapIntegralCuhre *gapint = new TLinePWaveGapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapDWave::TGapDWave() {
  TDWaveGapIntegralCuhre *gapint = new TDWaveGapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapCosSqDWave::TGapCosSqDWave() {
  TCosSqDWaveGapIntegralCuhre *gapint = new TCosSqDWaveGapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapSinSqDWave::TGapSinSqDWave() {
  TSinSqDWaveGapIntegralCuhre *gapint = new TSinSqDWaveGapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapAnSWave::TGapAnSWave() {
  TAnSWaveGapIntegralCuhre *gapint = new TAnSWaveGapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapNonMonDWave1::TGapNonMonDWave1() {
  TNonMonDWave1GapIntegralCuhre *gapint = new TNonMonDWave1GapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapNonMonDWave2::TGapNonMonDWave2() {
  TNonMonDWave2GapIntegralCuhre *gapint = new TNonMonDWave2GapIntegralCuhre();
  fGapIntegral = gapint;
  gapint = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaSWave::TLambdaSWave() {
  fLambdaInvSq = new TGapSWave();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaPointPWave::TLambdaPointPWave() {
  fLambdaInvSq = new TGapPointPWave();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaLinePWave::TLambdaLinePWave() {
  fLambdaInvSq = new TGapLinePWave();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaDWave::TLambdaDWave() {
  fLambdaInvSq = new TGapDWave();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaAnSWave::TLambdaAnSWave() {
  fLambdaInvSq = new TGapAnSWave();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaNonMonDWave1::TLambdaNonMonDWave1() {
  fLambdaInvSq = new TGapNonMonDWave1();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaNonMonDWave2::TLambdaNonMonDWave2() {
  fLambdaInvSq = new TGapNonMonDWave2();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvSWave::TLambdaInvSWave() {
  fLambdaInvSq = new TGapSWave();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvPointPWave::TLambdaInvPointPWave() {
  fLambdaInvSq = new TGapPointPWave();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvLinePWave::TLambdaInvLinePWave() {
  fLambdaInvSq = new TGapLinePWave();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvDWave::TLambdaInvDWave() {
  fLambdaInvSq = new TGapDWave();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvAnSWave::TLambdaInvAnSWave() {
  fLambdaInvSq = new TGapAnSWave();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvNonMonDWave1::TLambdaInvNonMonDWave1() {
  fLambdaInvSq = new TGapNonMonDWave1();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvNonMonDWave2::TLambdaInvNonMonDWave2() {
  fLambdaInvSq = new TGapNonMonDWave2();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapSWave::~TGapSWave() {
  delete fGapIntegral;
  fGapIntegral = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapPointPWave::~TGapPointPWave() {
  delete fGapIntegral;
  fGapIntegral = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapLinePWave::~TGapLinePWave() {
  delete fGapIntegral;
  fGapIntegral = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapDWave::~TGapDWave() {
  delete fGapIntegral;
  fGapIntegral = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapCosSqDWave::~TGapCosSqDWave() {
  delete fGapIntegral;
  fGapIntegral = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapSinSqDWave::~TGapSinSqDWave() {
  delete fGapIntegral;
  fGapIntegral = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapAnSWave::~TGapAnSWave() {
  delete fGapIntegral;
  fGapIntegral = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapNonMonDWave1::~TGapNonMonDWave1() {
  delete fGapIntegral;
  fGapIntegral = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TGapNonMonDWave2::~TGapNonMonDWave2() {
  delete fGapIntegral;
  fGapIntegral = nullptr;

  fTemp.clear();
  fTempIter = fTemp.end();
  fIntegralValues.clear();
  fCalcNeeded.clear();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaSWave::~TLambdaSWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaPointPWave::~TLambdaPointPWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaLinePWave::~TLambdaLinePWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaDWave::~TLambdaDWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaAnSWave::~TLambdaAnSWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaNonMonDWave1::~TLambdaNonMonDWave1() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaNonMonDWave2::~TLambdaNonMonDWave2() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvSWave::~TLambdaInvSWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvPointPWave::~TLambdaInvPointPWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvLinePWave::~TLambdaInvLinePWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvDWave::~TLambdaInvDWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvAnSWave::~TLambdaInvAnSWave() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvNonMonDWave1::~TLambdaInvNonMonDWave1() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TLambdaInvNonMonDWave2::~TLambdaInvNonMonDWave2() {
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
}


//--------------------------------------------------------------------
/**
 * <p>prepare the needed parameters for the integration carried out in TGapIntegral.
 * For details see also the Memo GapIntegrals.pdf, especially Eq.(7) and (9).
 */
double TGapSWave::operator()(double t, const std::vector<double> &par) const {

  // parameters: [0] Tc (K), [1] Delta0 (meV), [[2] c0 (1), [3] aG (1)]

  assert((par.size() == 2) || (par.size() == 4)); // 2 parameters: see A.~Carrington and F.~Manzano, Physica~C~\textbf{385}~(2003)~205
                                                  // 4 parameters: see R. Prozorov and R. Giannetta, Supercond. Sci. Technol. 19 (2006) R41-R67
                                                  // and Erratum Supercond. Sci. Technol. 21 (2008) 082003
                                                  // c0 in the original context is c0 = (pi kB Tc) / Delta0
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

    std::vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(0.172346648*t); // 2 kB T, kB in meV/K = 0.086173324 meV/K
    if (par.size() == 2) { // Carrington/Manzano
      intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    } else { // Prozorov/Giannetta
      intPar.push_back(par[1]*tanh(par[2]*sqrt(par[3]*(par[0]/t-1.0)))); // Delta0*tanh(c0*sqrt(aG*(Tc/T-1)))
    }

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

//--------------------------------------------------------------------
/**
 * <p>prepare the needed parameters for the integration carried out in TPointPWaveGapIntegralCuhre.
 * For details see also the Memo GapIntegrals.pdf, , especially Eq.(19) and (20).
 */
double TGapPointPWave::operator()(double t, const std::vector<double> &par) const {

  // parameters: [0] Tc (K), [1] Delta0 (meV), [[2] orientation tag, [[3] c0 (1), [4] aG (1)]]

  assert((par.size() >= 2) && (par.size() <= 5)); // 2 parameters: see A.~Carrington and F.~Manzano, Physica~C~\textbf{385}~(2003)~205
                                                  // 4 or 5 parameters: see R. Prozorov and R. Giannetta, Supercond. Sci. Technol. 19 (2006) R41-R67
                                                  // and Erratum Supercond. Sci. Technol. 21 (2008) 082003
                                                  // c0 in the original context is c0 = (pi kB Tc) / Delta0
                                                  // orientation tag: 0=aa,bb; 1=cc; 2=(sqrt[aa bb] + sqrt[aa cc] + sqrt[bb cc])/3 (default)
  if (t <= 0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  // check if orientation tag is given
  int orientation_tag(2);
  if ((par.size()==3) || (par.size()==5))
    orientation_tag = static_cast<int>(par[2]);

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
    double ds, ds1;
    std::vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(0.172346648*t); // 2 kB T, kB in meV/K = 0.086173324 meV/K
    if ((par.size() == 2) || (par.size() == 3)) { // Carrington/Manzano
      intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    } else { // Prozorov/Giannetta
      intPar.push_back(par[1]*tanh(par[2]*sqrt(par[3]*(par[0]/t-1.0)))); // Delta0*tanh(c0*sqrt(aG*(Tc/T-1)))
    }
    intPar.push_back(4.0*(t+intPar[1])); // upper limit of energy-integration: cutoff energy
    intPar.push_back(1.0); // upper limit of theta-integration

    fGapIntegral->SetParameters(intPar);
    if (orientation_tag == 0) // aa,bb
      ds = 1.0-(intPar[2]*3.0)/(2.0*intPar[0])*fGapIntegral->IntegrateFunc(0); // integral prefactor is by 2 lower [Eqs.(19,20)] since intPar[0]==2kB T!
    else if (orientation_tag == 1) // cc
      ds = 1.0-(intPar[2]*3.0)/(intPar[0])*fGapIntegral->IntegrateFunc(1); // integral prefactor is by 2 lower [Eqs.(19,20)] since intPar[0]==2kB T!
    else { // average
      ds = 1.0-(intPar[2]*3.0)/(2.0*intPar[0])*fGapIntegral->IntegrateFunc(0); // integral prefactor is by 2 lower [Eqs.(19,20)] since intPar[0]==2kB T!
      ds1 = 1.0-(intPar[2]*3.0)/(intPar[0])*fGapIntegral->IntegrateFunc(1); // integral prefactor is by 2 lower [Eqs.(19,20)] since intPar[0]==2kB T!
      ds = (ds + 2.0 * sqrt(ds*ds1))/3.0; // since aa==bb the avg looks like this
    }

    intPar.clear();

    if (newTemp)
      fIntegralValues.push_back(ds);
    else
      fIntegralValues[vectorIndex] = ds;

    fCalcNeeded[vectorIndex] = false;
  }

  return fIntegralValues[vectorIndex];
}

//--------------------------------------------------------------------
/**
 * <p>prepare the needed parameters for the integration carried out in TLinePWaveGapIntegralCuhre.
 * For details see also the Memo GapIntegrals.pdf, especially Eq.(19) and (20).
 */
double TGapLinePWave::operator()(double t, const std::vector<double> &par) const {

  // parameters: [0] Tc (K), [1] Delta0 (meV), [[2] orientation tag, [[3] c0 (1), [4] aG (1)]]

  assert((par.size() >= 2) && (par.size() <= 5)); // 2 parameters: see A.~Carrington and F.~Manzano, Physica~C~\textbf{385}~(2003)~205
                                                  // 4 or 5 parameters: see R. Prozorov and R. Giannetta, Supercond. Sci. Technol. 19 (2006) R41-R67
                                                  // and Erratum Supercond. Sci. Technol. 21 (2008) 082003
                                                  // c0 in the original context is c0 = (pi kB Tc) / Delta0
                                                  // orientation tag: 0=aa,bb; 1=cc; 2=(sqrt[aa bb] + sqrt[aa cc] + sqrt[bb cc])/3 (default)
  if (t <= 0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;


  // check if orientation tag is given
  int orientation_tag(2);
  if ((par.size()==3) || (par.size()==5))
    orientation_tag = static_cast<int>(par[2]);

  bool integralParChanged(false);  

  if (fPar.empty()) { // first time calling this routine
    fPar = par;
    integralParChanged = true;
  } else { // check if parameter have changed
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
    double ds, ds1;
    std::vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(0.172346648*t); // 2 kB T, kB in meV/K = 0.086173324 meV/K
    if ((par.size() == 2) || (par.size() == 3)) { // Carrington/Manzano
      intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    } else { // Prozorov/Giannetta
      intPar.push_back(par[1]*tanh(par[3]*sqrt(par[4]*(par[0]/t-1.0)))); // Delta0*tanh(c0*sqrt(aG*(Tc/T-1)))
    }
    intPar.push_back(4.0*(t+intPar[1])); // upper limit of energy-integration: cutoff energy
    intPar.push_back(1.0); // upper limit of z-integration

    fGapIntegral->SetParameters(intPar);
    if (orientation_tag == 0) // aa,bb
      ds = 1.0-(intPar[2]*3.0)/(2.0*intPar[0])*fGapIntegral->IntegrateFunc(0); // integral prefactor is by 2 lower [Eqs.(19,20)] since intPar[0]==2kB T!
    else if (orientation_tag == 1) // cc
      ds = 1.0-(intPar[2]*3.0)/(intPar[0])*fGapIntegral->IntegrateFunc(1); // integral prefactor is by 2 lower [Eqs.(19,20)] since intPar[0]==2kB T!
    else { // average
      ds = 1.0-(intPar[2]*3.0)/(2.0*intPar[0])*fGapIntegral->IntegrateFunc(0); // integral prefactor is by 2 lower [Eqs.(19,20)] since intPar[0]==2kB T!
      ds1 = 1.0-(intPar[2]*3.0)/(intPar[0])*fGapIntegral->IntegrateFunc(1); // integral prefactor is by 2 lower [Eqs.(19,20)] since intPar[0]==2kB T!
      ds = (ds + 2.0 * sqrt(ds*ds1))/3.0; // since aa==bb the avg looks like this
    }

    intPar.clear();

    if (newTemp)
      fIntegralValues.push_back(ds);
    else
      fIntegralValues[vectorIndex] = ds;

    fCalcNeeded[vectorIndex] = false;
  }

  return fIntegralValues[vectorIndex];
}

//--------------------------------------------------------------------
/**
 * <p>prepare the needed parameters for the integration carried out in TDWaveGapIntegralCuhre.
 * For details see also the Memo GapIntegrals.pdf, especially Eq.(7) and (10).
 */
double TGapDWave::operator()(double t, const std::vector<double> &par) const {

  // parameters: [0] Tc (K), [1] Delta0 (meV), [[2] c0 (1), [3] aG (1)]

  assert((par.size() == 2) || (par.size() == 4)); // 2 parameters: see A.~Carrington and F.~Manzano, Physica~C~\textbf{385}~(2003)~205
                                                  // 4 parameters: see R. Prozorov and R. Giannetta, Supercond. Sci. Technol. 19 (2006) R41-R67
                                                  // and Erratum Supercond. Sci. Technol. 21 (2008) 082003
                                                  // c0 in the original context is c0 = (pi kB Tc) / Delta0
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
    std::vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(0.172346648*t); // 2 kB T, kB in meV/K = 0.086173324 meV/K
    if (par.size() == 2) { // Carrington/Manzano
      intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    } else { // Prozorov/Giannetta
      intPar.push_back(par[1]*tanh(par[2]*sqrt(par[3]*(par[0]/t-1.0)))); // Delta0*tanh(c0*sqrt(aG*(Tc/T-1)))
    }
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

//--------------------------------------------------------------------
/**
 * <p>prepare the needed parameters for the integration carried out in TCosSqDWaveGapIntegralCuhre.
 * For details see also the Memo GapIntegrals.pdf, especially Eq.(7) and (??).
 */
double TGapCosSqDWave::operator()(double t, const std::vector<double> &par) const {

  // parameters: [0] Tc (K), [1] Delta0_D (meV), [2] Delta0_S (meV) [[3] c0_D (1), [4] aG_D (1), [5] c0_S (1), [6] aG_S (1)]

  assert((par.size() == 3) || (par.size() == 7)); // 3 parameters: see A.~Carrington and F.~Manzano, Physica~C~\textbf{385}~(2003)~205
                                                  // 7 parameters: see R. Prozorov and R. Giannetta, Supercond. Sci. Technol. 19 (2006) R41-R67
                                                  // and Erratum Supercond. Sci. Technol. 21 (2008) 082003
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
    std::vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(0.172346648*t); // 2 kB T, kB in meV/K = 0.086173324 meV/K
    if (par.size() == 3) { // Carrington/Manzano
      intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    } else { // Prozorov/Giannetta
      intPar.push_back(par[1]*tanh(par[3]*sqrt(par[4]*(par[0]/t-1.0)))); // Delta0_D*tanh(c0_D*sqrt(aG_D*(Tc/T-1)))
    }
    intPar.push_back(1.0*(t+intPar[1])); // upper limit of energy-integration: cutoff energy
    intPar.push_back(TMath::Pi()); // upper limit of phi-integration
    if (par.size() == 3) { // Carrington/Manzano
      intPar.push_back(par[2]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    } else { // Prozorov/Giannetta
      intPar.push_back(par[2]*tanh(par[5]*sqrt(par[6]*(par[0]/t-1.0)))); // Delta0_S*tanh(c0_S*sqrt(aG_S*(Tc/T-1)))
    }

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

//--------------------------------------------------------------------
/**
 * <p>prepare the needed parameters for the integration carried out in TSinSqDWaveGapIntegralCuhre.
 * For details see also the Memo GapIntegrals.pdf, especially Eq.(7) and (??).
 */
double TGapSinSqDWave::operator()(double t, const std::vector<double> &par) const {

  // parameters: [0] Tc (K), [1] Delta0_D (meV), [2] Delta0_S (meV) [[3] c0_D (1), [4] aG_D (1), [5] c0_S (1), [6] aG_S (1)]

  assert((par.size() == 3) || (par.size() == 7));  // 3 parameters: see A.~Carrington and F.~Manzano, Physica~C~\textbf{385}~(2003)~205
                                                   // 7 parameters: see R. Prozorov and R. Giannetta, Supercond. Sci. Technol. 19 (2006) R41-R67
                                                   // and Erratum Supercond. Sci. Technol. 21 (2008) 082003
                                                   // c0 in the original context is c0 = (pi kB Tc) / Delta0
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
    std::vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(0.172346648*t); // 2 kB T, kB in meV/K = 0.086173324 meV/K
    if (par.size() == 3) { // Carrington/Manzano
      intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    } else { // Prozorov/Giannetta
      intPar.push_back(par[1]*tanh(par[3]*sqrt(par[4]*(par[0]/t-1.0)))); // Delta0_D*tanh(c0_D*sqrt(aG_D*(Tc/T-1)))
    }
    intPar.push_back(1.0*(t+intPar[1])); // upper limit of energy-integration: cutoff energy
    intPar.push_back(TMath::Pi()); // upper limit of phi-integration
    if (par.size() == 3) { // Carrington/Manzano
      intPar.push_back(par[2]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    } else { // Prozorov/Giannetta
      intPar.push_back(par[2]*tanh(par[5]*sqrt(par[6]*(par[0]/t-1.0)))); // Delta0_S*tanh(c0_S*sqrt(aG_S*(Tc/T-1)))
    }

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

//--------------------------------------------------------------------
/**
 * <p>prepare the needed parameters for the integration carried out in TAnSWaveGapIntegralCuhre (anisotropic s-wave).
 * For details see also the Memo GapIntegrals.pdf, especially Eq.(7) and (13).
 */
double TGapAnSWave::operator()(double t, const std::vector<double> &par) const {

  // parameters: [0] Tc (K), [1] Delta0 (meV), [2] a (1), [[3] c0 (1), [4] aG (1)]

  assert((par.size() == 3) || (par.size() == 5)); // 3 parameters: see A.~Carrington and F.~Manzano, Physica~C~\textbf{385}~(2003)~205
                                                  // 5 parameters: see R. Prozorov and R. Giannetta, Supercond. Sci. Technol. 19 (2006) R41-R67
                                                  // and Erratum Supercond. Sci. Technol. 21 (2008) 082003
                                                  // c0 in the original context is c0 = (pi kB Tc) / Delta0

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
    std::vector<double> intPar; // parameters for the integral, T & Delta(T)
    intPar.push_back(0.172346648*t); // 2 kB T, kB in meV/K = 0.086173324 meV/K
    if (par.size() == 3) { // Carrington/Manzano
      intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    } else { // Prozorov/Giannetta
      intPar.push_back(par[1]*tanh(par[3]*sqrt(par[4]*(par[0]/t-1.0)))); // Delta0*tanh(c0*sqrt(aG*(Tc/T-1)))
    }
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

//--------------------------------------------------------------------
/**
 * <p>prepare the needed parameters for the integration carried out in TNonMonDWave1GapIntegralCuhre.
 * For details see also the Memo GapIntegrals.pdf, especially Eq.(7) and (11).
 */
double TGapNonMonDWave1::operator()(double t, const std::vector<double> &par) const {

  // parameters: [0] Tc (K), [1] Delta0 (meV), [2] a (1), [[3] c0 (1), [4] aG (1)]

  assert((par.size() == 3) || (par.size() == 5)); // 3 parameters: see A.~Carrington and F.~Manzano, Physica~C~\textbf{385}~(2003)~205
                                                  // 5 parameters: see R. Prozorov and R. Giannetta, Supercond. Sci. Technol. 19 (2006) R41-R67
                                                  // and Erratum Supercond. Sci. Technol. 21 (2008) 082003
                                                  // c0 in the original context is c0 = (pi kB Tc) / Delta0
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
    std::vector<double> intPar; // parameters for the integral: 2 k_B T, Delta(T), a, E_c, phi_c
    intPar.push_back(0.172346648*t); // 2 kB T, kB in meV/K = 0.086173324 meV/K
    if (par.size() == 3) { // Carrington/Manzano
      intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    } else { // Prozorov/Giannetta
      intPar.push_back(par[1]*tanh(par[3]*sqrt(par[4]*(par[0]/t-1.0)))); // Delta0*tanh(c0*sqrt(aG*(Tc/T-1)))
    }
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

//--------------------------------------------------------------------
/**
 * <p>prepare the needed parameters for the integration carried out in TNonMonDWave2GapIntegralCuhre.
 * For details see also the Memo GapIntegrals.pdf, especially Eq.(7) and (11).
 */
double TGapNonMonDWave2::operator()(double t, const std::vector<double> &par) const {

  // parameters: [0] Tc (K), [1] Delta0 (meV), [2] a (1), [[3] c0 (1), [4] aG (1)]

  assert((par.size() == 3) || (par.size() == 5)); // 3 parameters: see A.~Carrington and F.~Manzano, Physica~C~\textbf{385}~(2003)~205
                                                  // 5 parameters: see R. Prozorov and R. Giannetta, Supercond. Sci. Technol. 19 (2006) R41-R67
                                                  // and Erratum Supercond. Sci. Technol. 21 (2008) 082003
                                                  // c0 in the original context is c0 = (pi kB Tc) / Delta0
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
    std::vector<double> intPar; // parameters for the integral: 2 k_B T, Delta(T), a, E_c, phi_c
    intPar.push_back(0.172346648*t); // 2 kB T, kB in meV/K = 0.086173324 meV/K
    if (par.size() == 3) { // Carrington/Manzano
      intPar.push_back(par[1]*tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51)));
    } else { // Prozorov/Giannetta
      intPar.push_back(par[1]*tanh(par[3]*sqrt(par[4]*(par[0]/t-1.0)))); // Delta0*tanh(c0*sqrt(aG*(Tc/T-1)))
    }
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

//--------------------------------------------------------------------
/**
 * <p>Superfluid density in the ``two-fluid'' approximation.
 * For details see also the Memo GapIntegrals.pdf, especially Eq.(7) and (14).
 */
double TGapPowerLaw::operator()(double t, const std::vector<double> &par) const {

  assert(par.size() == 2); // two parameters: Tc, n

  if(t<=0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  return 1.0 - pow(t/par[0], par[1]);

}


//--------------------------------------------------------------------
/**
 * <p>Superfluid density for a dirty s-wave superconductor.
 * For details see also the Memo GapIntegrals.pdf, especially Eq.(8) and (15).
 * Here we use INTENTIONALLY the temperature dependence of the gap according
 * to A. Carrington and F. Manzano, Physica C 385 (2003) 205
 */
double TGapDirtySWave::operator()(double t, const std::vector<double> &par) const {

  // parameters: [0] Tc (K), [1] Delta0 (meV), [[2] c0 (1), [3] aG (1)]

  assert((par.size() == 2) || (par.size() == 4)); // 2 parameters: see A.~Carrington and F.~Manzano, Physica~C~\textbf{385}~(2003)~205
                                                  // 4 parameters: see R. Prozorov and R. Giannetta, Supercond. Sci. Technol. 19 (2006) R41-R67
                                                  // and Erratum Supercond. Sci. Technol. 21 (2008) 082003
  if (t<=0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  double deltaT = 0.0;
  if (par.size() == 2) { // Carrington/Manzano
    deltaT = tanh(1.82*pow(1.018*(par[0]/t-1.0),0.51));
  } else { // Prozorov/Giannetta
    deltaT = tanh(par[2]*sqrt(par[3]*(par[0]/t-1.0))); // tanh(c0*sqrt(aG*(Tc/T-1)))
  }

  return deltaT*tanh(par[1]*deltaT/(0.172346648*t)); // Delta(T)/Delta(0)*tanh(Delta(T)/2 kB T), kB in meV/K
}


//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaSWave::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 2); // two parameters: Tc, Delta0

  if (t >= par[0])
    return -1.0;

  if (t <= 0.0)
    return 1.0;

  return 1.0/sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaPointPWave::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 2); // two parameters: Tc, Delta0

  if (t >= par[0])
    return -1.0;

  if (t <= 0.0)
    return 1.0;

  return 1.0/sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaLinePWave::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 2); // two parameters: Tc, Delta0

  if (t >= par[0])
    return -1.0;

  if (t <= 0.0)
    return 1.0;

  return 1.0/sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaDWave::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 2); // two parameters: Tc, Delta0

  if (t >= par[0])
    return -1.0;

  if (t <= 0.0)
    return 1.0;

  return 1.0/sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaAnSWave::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 3); // three parameters: Tc, Delta0, a

  if (t >= par[0])
    return -1.0;

  if (t <= 0.0)
    return 1.0;

  return 1.0/sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaNonMonDWave1::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 3); // three parameters: Tc, Delta0, a

  if (t >= par[0])
    return -1.0;

  if (t <= 0.0)
    return 1.0;

  return 1.0/sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaNonMonDWave2::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 3); // three parameters: Tc, Delta0, a

  if (t >= par[0])
    return -1.0;

  if (t <= 0.0)
    return 1.0;

  return 1.0/sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaPowerLaw::operator()(double t, const std::vector<double> &par) const {

  assert(par.size() == 2); // two parameters: Tc, N

  if(t <= 0.0)
    return 1.0;
  else if (t >= par[0])
    return -1.0;

  return 1.0/sqrt(1.0 - pow(t/par[0], par[1]));
}


//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaInvSWave::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 2); // two parameters: Tc, Delta0

  if (t >= par[0])
    return 0.0;

  if (t <= 0.0)
    return 1.0;

  return sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaInvPointPWave::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 2); // two parameters: Tc, Delta0

  if (t >= par[0])
    return 0.0;

  if (t <= 0.0)
    return 1.0;

  return sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaInvLinePWave::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 2); // two parameters: Tc, Delta0

  if (t >= par[0])
    return 0.0;

  if (t <= 0.0)
    return 1.0;

  return sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaInvDWave::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 2); // two parameters: Tc, Delta0

  if (t >= par[0])
    return 0.0;

  if (t <= 0.0)
    return 1.0;

  return sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaInvAnSWave::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 3); // three parameters: Tc, Delta0, a

  if (t >= par[0])
    return 0.0;

  if (t <= 0.0)
    return 1.0;

  return sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaInvNonMonDWave1::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 3); // three parameters: Tc, Delta0, a

  if (t >= par[0])
    return 0.0;

  if (t <= 0.0)
    return 1.0;

  return sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaInvNonMonDWave2::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 3); // three parameters: Tc, Delta0, a

  if (t >= par[0])
    return 0.0;

  if (t <= 0.0)
    return 1.0;

  return sqrt((*fLambdaInvSq)(t, par));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TLambdaInvPowerLaw::operator()(double t, const std::vector<double> &par) const {

  assert(par.size() == 2); // two parameters: Tc, N

  if(t <= 0.0)
    return 1.0;
  else if (t >= par[0])
    return 0.0;

  return sqrt(1.0 - pow(t/par[0], par[1]));
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TFilmMagnetizationDWave::TFilmMagnetizationDWave()
{
  fLambdaInvSq = new TGapDWave();
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
TFilmMagnetizationDWave::~TFilmMagnetizationDWave()
{
  delete fLambdaInvSq;
  fLambdaInvSq = nullptr;
  fPar.clear();
}

//--------------------------------------------------------------------
/**
 * <p>
 */
double TFilmMagnetizationDWave::operator()(double t, const std::vector<double> &par) const
{
  assert(par.size() == 4); // four parameters: Tc, Delta0, lambda0, film-thickness

  fPar = par;

  if (t >= fPar[0])
    return 0.0;

  std::vector<double> parForGapIntegral;
  parForGapIntegral.push_back(fPar[0]);
  parForGapIntegral.push_back(fPar[1]);

  double d_2l(0.5*fPar[3]/fPar[2]*sqrt((*fLambdaInvSq)(t, parForGapIntegral)));

  parForGapIntegral.clear();

  return tanh(d_2l)/d_2l - 1.0;

}


