/***************************************************************************

  BMWIntegrator.cpp

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

# include "BMWIntegrator.h"

# include "cuba.h"

std::vector<double> TDWaveGapIntegralCuhre::fPar;

double TDWaveGapIntegralCuhre::IntegrateFunc()
{
  const unsigned int NCOMP(1);
  const double EPSREL (1e-4);
  const double EPSABS (1e-6);
  const unsigned int VERBOSE (0);
  const unsigned int LAST (4);
  const unsigned int MINEVAL (0);
  const unsigned int MAXEVAL (50000);

  const unsigned int KEY (13);

  int nregions, neval, fail;
  double integral[NCOMP], error[NCOMP], prob[NCOMP];

  Cuhre(fNDim, NCOMP, Integrand,
    EPSREL, EPSABS, VERBOSE | LAST, MINEVAL, MAXEVAL,
    KEY,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

void TDWaveGapIntegralCuhre::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[]) // x = {E, phi}, fPar = {twokBT, Delta(T), Ec, phic}
{
  double deltasq(TMath::Power(fPar[1]*TMath::Cos(2.0*x[1]*fPar[3]),2.0));
  f[0] = 1.0/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[2]*fPar[2]+deltasq)/fPar[0]),2.0);
  return;
}

std::vector<double> TAnSWaveGapIntegralCuhre::fPar;

double TAnSWaveGapIntegralCuhre::IntegrateFunc()
{
  const unsigned int NCOMP(1);
  const double EPSREL (1e-4);
  const double EPSABS (1e-6);
  const unsigned int VERBOSE (0);
  const unsigned int LAST (4);
  const unsigned int MINEVAL (100);
  const unsigned int MAXEVAL (1000000);

  const unsigned int KEY (13);

  int nregions, neval, fail;
  double integral[NCOMP], error[NCOMP], prob[NCOMP];

  Cuhre(fNDim, NCOMP, Integrand,
    EPSREL, EPSABS, VERBOSE | LAST, MINEVAL, MAXEVAL,
    KEY,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

void TAnSWaveGapIntegralCuhre::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[]) // x = {E, phi}, fPar = {twokBT, Delta(T),a, Ec, phic}
{
  double deltasq(TMath::Power(fPar[1]*(1.0+fPar[2]*TMath::Cos(4.0*x[1]*fPar[4])),2.0));
  f[0] = 1.0/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[3]*fPar[3]+deltasq)/fPar[0]),2.0);
  return;
}

std::vector<double> TAnSWaveGapIntegralDivonne::fPar;

double TAnSWaveGapIntegralDivonne::IntegrateFunc()
{
  const unsigned int NCOMP(1);
  const double EPSREL (1e-4);
  const double EPSABS (1e-6);
  const unsigned int VERBOSE (0);
  const unsigned int MINEVAL (1000);
  const unsigned int MAXEVAL (1000000);
  const unsigned int KEY1 (47);
  const unsigned int KEY2 (1);
  const unsigned int KEY3 (1);
  const unsigned int MAXPASS (5);
  const double BORDER (0.);
  const double MAXCHISQ (10.);
  const double MINDEVIATION (.25);
  const unsigned int NGIVEN (0);
  const unsigned int LDXGIVEN (fNDim);
  const unsigned int NEXTRA (0);

  int nregions, neval, fail;
  double integral[NCOMP], error[NCOMP], prob[NCOMP];

  Divonne(fNDim, NCOMP, Integrand,
    EPSREL, EPSABS, VERBOSE, MINEVAL, MAXEVAL,
    KEY1, KEY2, KEY3, MAXPASS, BORDER, MAXCHISQ, MINDEVIATION,
    NGIVEN, LDXGIVEN, NULL, NEXTRA, NULL,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

void TAnSWaveGapIntegralDivonne::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[]) // x = {E, phi}, fPar = {twokBT, Delta(T),a, Ec, phic}
{
  double deltasq(TMath::Power(fPar[1]*(1.0+fPar[2]*TMath::Cos(4.0*x[1]*fPar[4])),2.0));
  f[0] = 1.0/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[3]*fPar[3]+deltasq)/fPar[0]),2.0);
  return;
}

std::vector<double> TAnSWaveGapIntegralSuave::fPar;

double TAnSWaveGapIntegralSuave::IntegrateFunc()
{
  const unsigned int NCOMP(1);
  const double EPSREL (1e-4);
  const double EPSABS (1e-6);
  const unsigned int VERBOSE (0);
  const unsigned int LAST (4);
  const unsigned int MINEVAL (1000);
  const unsigned int MAXEVAL (1000000);

  const unsigned int NNEW (1000);
  const double FLATNESS (25.);

  int nregions, neval, fail;
  double integral[NCOMP], error[NCOMP], prob[NCOMP];

  Suave(fNDim, NCOMP, Integrand,
    EPSREL, EPSABS, VERBOSE | LAST, MINEVAL, MAXEVAL,
    NNEW, FLATNESS,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

void TAnSWaveGapIntegralSuave::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[]) // x = {E, phi}, fPar = {twokBT, Delta(T),a, Ec, phic}
{
  double deltasq(TMath::Power(fPar[1]*(1.0+fPar[2]*TMath::Cos(4.0*x[1]*fPar[4])),2.0));
  f[0] = 1.0/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[3]*fPar[3]+deltasq)/fPar[0]),2.0);
  return;
}

std::vector<double> TNonMonDWave1GapIntegralCuhre::fPar;

double TNonMonDWave1GapIntegralCuhre::IntegrateFunc()
{
  const unsigned int NCOMP(1);
  const double EPSREL (1e-4);
  const double EPSABS (1e-6);
  const unsigned int VERBOSE (0);
  const unsigned int LAST (4);
  const unsigned int MINEVAL (100);
  const unsigned int MAXEVAL (1000000);

  const unsigned int KEY (13);

  int nregions, neval, fail;
  double integral[NCOMP], error[NCOMP], prob[NCOMP];

  Cuhre(fNDim, NCOMP, Integrand,
    EPSREL, EPSABS, VERBOSE | LAST, MINEVAL, MAXEVAL,
    KEY,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

void TNonMonDWave1GapIntegralCuhre::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[]) // x = {E, phi}, fPar = {twokBT, Delta(T),a, Ec, phic}
{
  double deltasq(TMath::Power(fPar[1]*(fPar[2]*TMath::Cos(2.0*x[1]*fPar[4])+(1.0-fPar[2])*TMath::Cos(6.0*x[1]*fPar[4])),2.0));
  f[0] = 1.0/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[3]*fPar[3]+deltasq)/fPar[0]),2.0);
  return;
}

std::vector<double> TNonMonDWave2GapIntegralCuhre::fPar;

double TNonMonDWave2GapIntegralCuhre::IntegrateFunc()
{
  const unsigned int NCOMP(1);
  const double EPSREL (1e-4);
  const double EPSABS (1e-6);
  const unsigned int VERBOSE (0);
  const unsigned int LAST (4);
  const unsigned int MINEVAL (100);
  const unsigned int MAXEVAL (1000000);

  const unsigned int KEY (13);

  int nregions, neval, fail;
  double integral[NCOMP], error[NCOMP], prob[NCOMP];

  Cuhre(fNDim, NCOMP, Integrand,
    EPSREL, EPSABS, VERBOSE | LAST, MINEVAL, MAXEVAL,
    KEY,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

void TNonMonDWave2GapIntegralCuhre::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[]) // x = {E, phi}, fPar = {twokBT, Delta(T),a, Ec, phic}
{
  double deltasq(4.0*fPar[2]/27.0*TMath::Power(fPar[1]*TMath::Cos(2.0*x[1]*fPar[4]), 2.0) \
    / TMath::Power(1.0 + fPar[2]*TMath::Cos(2.0*x[1]*fPar[4])*TMath::Cos(2.0*x[1]*fPar[4]), 3.0));
  f[0] = 1.0/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[3]*fPar[3]+deltasq)/fPar[0]),2.0);
  return;
}