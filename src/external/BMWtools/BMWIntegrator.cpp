/***************************************************************************

  BMWIntegrator.cpp

  Author: Bastian M. Wojek

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

#include "BMWIntegrator.h"

#include "cuba.h"

#define USERDATA NULL
#define SEED 0
#define STATEFILE NULL

std::vector<double> TDWaveGapIntegralCuhre::fPar;

/**
 * <p>Integrate the function using the Cuhre interface
 *
 * <p><b>return:</b>
 * - value of the integral
 */
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

  Cuhre(fNDim, NCOMP, Integrand, USERDATA,
    EPSREL, EPSABS, VERBOSE | LAST, MINEVAL, MAXEVAL,
    KEY, STATEFILE,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

/**
 * <p>Calculate the function value for the use with Cuhre---actual implementation of the function
 *
 * <p><b>return:</b>
 * - 0
 *
 * \param ndim number of dimensions of the integral (2 here)
 * \param x point where the function should be evaluated
 * \param ncomp number of components of the integrand (1 here)
 * \param f function value
 * \param userdata additional user parameters (required by the interface, NULL here)
 */
int TDWaveGapIntegralCuhre::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[], void *userdata) // x = {E, phi}, fPar = {twokBT, Delta(T), Ec, phic}
{
  double deltasq(TMath::Power(fPar[1]*TMath::Cos(2.0*x[1]*fPar[3]),2.0));
  f[0] = 1.0/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[2]*fPar[2]+deltasq)/fPar[0]),2.0);
  return 0;
}

std::vector<double> TCosSqDWaveGapIntegralCuhre::fPar;

/**
 * <p>Integrate the function using the Cuhre interface
 *
 * <p><b>return:</b>
 * - value of the integral
 */
double TCosSqDWaveGapIntegralCuhre::IntegrateFunc()
{
  const unsigned int NCOMP(1);
  const double EPSREL (1e-8);
  const double EPSABS (1e-6);
  const unsigned int VERBOSE (0);
  const unsigned int LAST (4);
  const unsigned int MINEVAL (0);
  const unsigned int MAXEVAL (500000);

  const unsigned int KEY (13);

  int nregions, neval, fail;
  double integral[NCOMP], error[NCOMP], prob[NCOMP];

  Cuhre(fNDim, NCOMP, Integrand, USERDATA,
    EPSREL, EPSABS, VERBOSE | LAST, MINEVAL, MAXEVAL,
    KEY, STATEFILE,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

/**
 * <p>Calculate the function value for the use with Cuhre---actual implementation of the function
 *
 * <p><b>return:</b>
 * - 0
 *
 * \param ndim number of dimensions of the integral (2 here)
 * \param x point where the function should be evaluated
 * \param ncomp number of components of the integrand (1 here)
 * \param f function value
 * \param userdata additional user parameters (required by the interface, NULL here)
 */
int TCosSqDWaveGapIntegralCuhre::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[], void *userdata) // x = {E, phi}, fPar = {twokBT, DeltaD(T), Ec, phic, DeltaS(T)}
{
  double deltasq(TMath::Power(fPar[1]*TMath::Cos(2.0*x[1]*fPar[3]) + fPar[4], 2.0));
  f[0] = TMath::Power(TMath::Cos(x[1]*fPar[3])/TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[2]*fPar[2]+deltasq)/fPar[0]),2.0);
  return 0;
}

std::vector<double> TSinSqDWaveGapIntegralCuhre::fPar;

/**
 * <p>Integrate the function using the Cuhre interface
 *
 * <p><b>return:</b>
 * - value of the integral
 */
double TSinSqDWaveGapIntegralCuhre::IntegrateFunc()
{
  const unsigned int NCOMP(1);
  const double EPSREL (1e-8);
  const double EPSABS (1e-10);
  const unsigned int VERBOSE (0);
  const unsigned int LAST (4);
  const unsigned int MINEVAL (0);
  const unsigned int MAXEVAL (500000);

  const unsigned int KEY (13);

  int nregions, neval, fail;
  double integral[NCOMP], error[NCOMP], prob[NCOMP];

  Cuhre(fNDim, NCOMP, Integrand, USERDATA,
    EPSREL, EPSABS, VERBOSE | LAST, MINEVAL, MAXEVAL,
    KEY, STATEFILE,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

/**
 * <p>Calculate the function value for the use with Cuhre---actual implementation of the function
 *
 * <p><b>return:</b>
 * - 0
 *
 * \param ndim number of dimensions of the integral (2 here)
 * \param x point where the function should be evaluated
 * \param ncomp number of components of the integrand (1 here)
 * \param f function value
 * \param userdata additional user parameters (required by the interface, NULL here)
 */
int TSinSqDWaveGapIntegralCuhre::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[], void *userdata) // x = {E, phi}, fPar = {twokBT, DeltaD(T), Ec, phic, DeltaS(T)}
{
  double deltasq(TMath::Power(fPar[1]*TMath::Cos(2.0*x[1]*fPar[3]) + fPar[4], 2.0));
  f[0] = TMath::Power(TMath::Sin(x[1]*fPar[3]),2.0)/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[2]*fPar[2]+deltasq)/fPar[0]),2.0);
  return 0;
}

std::vector<double> TAnSWaveGapIntegralCuhre::fPar;

/**
 * <p>Integrate the function using the Cuhre interface
 *
 * <p><b>return:</b>
 * - value of the integral
 */
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

  Cuhre(fNDim, NCOMP, Integrand, USERDATA,
    EPSREL, EPSABS, VERBOSE | LAST, MINEVAL, MAXEVAL,
    KEY, STATEFILE,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

/**
 * <p>Calculate the function value for the use with Cuhre---actual implementation of the function
 *
 * <p><b>return:</b>
 * - 0
 *
 * \param ndim number of dimensions of the integral (2 here)
 * \param x point where the function should be evaluated
 * \param ncomp number of components of the integrand (1 here)
 * \param f function value
 * \param userdata additional user parameters (required by the interface, NULL here)
 */
int TAnSWaveGapIntegralCuhre::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[], void *userdata) // x = {E, phi}, fPar = {twokBT, Delta(T),a, Ec, phic}
{
  double deltasq(TMath::Power(fPar[1]*(1.0+fPar[2]*TMath::Cos(4.0*x[1]*fPar[4])),2.0));
  f[0] = 1.0/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[3]*fPar[3]+deltasq)/fPar[0]),2.0);
  return 0;
}

std::vector<double> TAnSWaveGapIntegralDivonne::fPar;

/**
 * <p>Integrate the function using the Divonne interface
 *
 * <p><b>return:</b>
 * - value of the integral
 */
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

  Divonne(fNDim, NCOMP, Integrand, USERDATA,
    EPSREL, EPSABS, VERBOSE, SEED, MINEVAL, MAXEVAL,
    KEY1, KEY2, KEY3, MAXPASS, BORDER, MAXCHISQ, MINDEVIATION,
    NGIVEN, LDXGIVEN, NULL, NEXTRA, NULL, STATEFILE,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

/**
 * <p>Calculate the function value for the use with Divonne---actual implementation of the function
 *
 * <p><b>return:</b>
 * - 0
 *
 * \param ndim number of dimensions of the integral (2 here)
 * \param x point where the function should be evaluated
 * \param ncomp number of components of the integrand (1 here)
 * \param f function value
 * \param userdata additional user parameters (required by the interface, NULL here)
 */
int TAnSWaveGapIntegralDivonne::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[], void *userdata) // x = {E, phi}, fPar = {twokBT, Delta(T),a, Ec, phic}
{
  double deltasq(TMath::Power(fPar[1]*(1.0+fPar[2]*TMath::Cos(4.0*x[1]*fPar[4])),2.0));
  f[0] = 1.0/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[3]*fPar[3]+deltasq)/fPar[0]),2.0);
  return 0;
}

std::vector<double> TAnSWaveGapIntegralSuave::fPar;

/**
 * <p>Integrate the function using the Suave interface
 *
 * <p><b>return:</b>
 * - value of the integral
 */
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

  Suave(fNDim, NCOMP, Integrand, USERDATA,
    EPSREL, EPSABS, VERBOSE | LAST, SEED, MINEVAL, MAXEVAL,
    NNEW, FLATNESS, STATEFILE,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

/**
 * <p>Calculate the function value for the use with Suave---actual implementation of the function
 *
 * <p><b>return:</b>
 * - 0
 *
 * \param ndim number of dimensions of the integral (2 here)
 * \param x point where the function should be evaluated
 * \param ncomp number of components of the integrand (1 here)
 * \param f function value
 * \param userdata additional user parameters (required by the interface, NULL here)
 */
int TAnSWaveGapIntegralSuave::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[], void *userdata) // x = {E, phi}, fPar = {twokBT, Delta(T),a, Ec, phic}
{
  double deltasq(TMath::Power(fPar[1]*(1.0+fPar[2]*TMath::Cos(4.0*x[1]*fPar[4])),2.0));
  f[0] = 1.0/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[3]*fPar[3]+deltasq)/fPar[0]),2.0);
  return 0;
}

std::vector<double> TNonMonDWave1GapIntegralCuhre::fPar;

/**
 * <p>Integrate the function using the Cuhre interface
 *
 * <p><b>return:</b>
 * - value of the integral
 */
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

  Cuhre(fNDim, NCOMP, Integrand, USERDATA,
    EPSREL, EPSABS, VERBOSE | LAST, MINEVAL, MAXEVAL,
    KEY, STATEFILE,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

/**
 * <p>Calculate the function value for the use with Cuhre---actual implementation of the function
 *
 * <p><b>return:</b>
 * - 0
 *
 * \param ndim number of dimensions of the integral (2 here)
 * \param x point where the function should be evaluated
 * \param ncomp number of components of the integrand (1 here)
 * \param f function value
 * \param userdata additional user parameters (required by the interface, NULL here)
 */
int TNonMonDWave1GapIntegralCuhre::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[], void *userdata) // x = {E, phi}, fPar = {twokBT, Delta(T),a, Ec, phic}
{
  double deltasq(TMath::Power(fPar[1]*(fPar[2]*TMath::Cos(2.0*x[1]*fPar[4])+(1.0-fPar[2])*TMath::Cos(6.0*x[1]*fPar[4])),2.0));
  f[0] = 1.0/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[3]*fPar[3]+deltasq)/fPar[0]),2.0);
  return 0;
}

std::vector<double> TNonMonDWave2GapIntegralCuhre::fPar;

/**
 * <p>Integrate the function using the Cuhre interface
 *
 * <p><b>return:</b>
 * - value of the integral
 */
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

  Cuhre(fNDim, NCOMP, Integrand, USERDATA,
    EPSREL, EPSABS, VERBOSE | LAST, MINEVAL, MAXEVAL,
    KEY, STATEFILE,
    &nregions, &neval, &fail, integral, error, prob);

  return integral[0];
}

/**
 * <p>Calculate the function value for the use with Cuhre---actual implementation of the function
 *
 * <p><b>return:</b>
 * - 0
 *
 * \param ndim number of dimensions of the integral (2 here)
 * \param x point where the function should be evaluated
 * \param ncomp number of components of the integrand (1 here)
 * \param f function value
 * \param userdata additional user parameters (required by the interface, NULL here)
 */
int TNonMonDWave2GapIntegralCuhre::Integrand(const int *ndim, const double x[],
                      const int *ncomp, double f[], void *userdata) // x = {E, phi}, fPar = {twokBT, Delta(T),a, Ec, phic}
{
  double deltasq(4.0*fPar[2]/27.0*TMath::Power(fPar[1]*TMath::Cos(2.0*x[1]*fPar[4]), 2.0) \
    / TMath::Power(1.0 + fPar[2]*TMath::Cos(2.0*x[1]*fPar[4])*TMath::Cos(2.0*x[1]*fPar[4]), 3.0));
  f[0] = 1.0/TMath::Power(TMath::CosH(TMath::Sqrt(x[0]*x[0]*fPar[3]*fPar[3]+deltasq)/fPar[0]),2.0);
  return 0;
}
