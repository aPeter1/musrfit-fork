/***************************************************************************

  TLFRelaxation.cpp

  Author: Bastian M. Wojek

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Bastian M. Wojek                                *
 *                                                                         *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cassert>
#include <sys/time.h>
#include <iostream>
#include <cmath>

#ifdef HAVE_GOMP
#include <omp.h>
#endif

#include <TSAXParser.h>
#include "BMWIntegrator.h"
#include "BMWStartupHandler.h"
#include "TLFRelaxation.h"

using namespace std;

#define PI 3.14159265358979323846
#define TWOPI 6.28318530717958647692
#define Nsteps 100

ClassImp(TLFStatGssKT)
ClassImp(TLFStatExpKT)
ClassImp(TLFDynGssKT)
ClassImp(TLFDynExpKT)
ClassImp(TLFDynSG)
ClassImp(TLFSGInterpolation)


//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor: Initialize variables and allocate memory for the integrator.
 */
TLFStatGssKT::TLFStatGssKT() : fCalcNeeded(true), fLastTime(0.) {
  fIntSinGss = new TIntSinGss();
  fPar.resize(2, 0.);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor: Free memory of the integrator, clean up some vectors.
 */
TLFStatGssKT::~TLFStatGssKT() {
  delete fIntSinGss;
  fIntSinGss = 0;
  fIntValues.clear();
  fPar.clear();
}

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 * <p>Method returning the function value at a given time for a given set of parameters.
 *
 * \param t time \htmlonly (&#956;s) \endhtmlonly \latexonly ($\mu\mathrm{s}$) \endlatexonly
 * \param par parameters [\htmlonly &#957;<sub>L</sub>=<i>B</i>&#947;<sub>&#956;</sub>/2&#960; (MHz), &#963; (&#956;s<sup>-1</sup>)\endhtmlonly \latexonly $\nu_{\mathrm{L}}=B\gamma_{\mu}/2\pi~(\mathrm{MHz})$, $\sigma~(\mu\mathrm{s}^{-1})$ \endlatexonly]
 */
double TLFStatGssKT::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 2); // two parameters nu=gbar*B,sigma

  if(t<0.0)
    return 1.0;

  double sigsq(par[1]*par[1]);
  double sigsqtt(sigsq*t*t);

  if(TMath::Abs(par[0])<0.00135538817){
    return (0.33333333333333333333+0.66666666666666666667*(1.0-sigsqtt)*TMath::Exp(-0.5*sigsqtt));
  }

  for (unsigned int i(0); i<2; ++i) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      fCalcNeeded = true;
    }
  }

  if (fCalcNeeded) {
    fIntValues.clear();
    fIntValues[0.] = 0.;
    fLastTime = 0.;
  } else {
    if (t > fIntValues.rbegin()->first) {
      fCalcNeeded = true;
      fLastTime = fIntValues.rbegin()->first;
    }
  }

  double omegaL(TMath::TwoPi()*par[0]);
  double coeff1(2.0*sigsq/(omegaL*omegaL));
  double coeff2(coeff1*sigsq/omegaL);

  if (fCalcNeeded) {
    fIntSinGss->SetParameters(par);
    fIntValues[t] = fIntValues[fLastTime] + fIntSinGss->IntegrateFunc(fLastTime,t);
    fCalcNeeded = false;
  }

  return (1.0-(coeff1*(1.0-TMath::Exp(-0.5*sigsqtt)*TMath::Cos(omegaL*t)))+(coeff2*fIntValues[t]));
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor: Initialize variables and allocate memory for the integrator.
 */
TLFStatExpKT::TLFStatExpKT() : fCalcNeeded(true), fLastTime(0.) {
  fIntBesselJ0Exp = new TIntBesselJ0Exp();
  fPar.resize(2, 0.);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor: Free memory of the integrator, clean up some vectors.
 */
TLFStatExpKT::~TLFStatExpKT() {
  delete fIntBesselJ0Exp;
  fIntBesselJ0Exp = 0;
  fIntValues.clear();
  fPar.clear();
}

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 * <p>Method returning the function value at a given time for a given set of parameters.
 *
 * \param t time \htmlonly (&#956;s) \endhtmlonly \latexonly ($\mu\mathrm{s}$) \endlatexonly
 * \param par parameters [\htmlonly &#957;<sub>L</sub>=<i>B</i>&#947;<sub>&#956;</sub>/2&#960; (MHz), <i>a</i> (&#956;s<sup>-1</sup>)\endhtmlonly \latexonly $\nu_{\mathrm{L}}=B\gamma_{\mu}/2\pi~(\mathrm{MHz})$, $a~(\mu\mathrm{s}^{-1})$ \endlatexonly]
 */
double TLFStatExpKT::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 2); // two parameters nu=gbar*B,rate

  if(t<0.0)
    return 1.0;

  if(TMath::Abs(par[0])<0.00135538817){
    return (0.33333333333333333333+0.66666666666666666667*(1.0-par[1]*t)*TMath::Exp(-par[1]*t));
  }

  for (unsigned int i(0); i<2; ++i) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      fCalcNeeded = true;
    }
  }

  if (fCalcNeeded) {
    fIntValues.clear();
    fIntValues[0.] = 0.;
    fLastTime = 0.;
  } else {
    if (t > fIntValues.rbegin()->first) {
      fCalcNeeded = true;
      fLastTime = fIntValues.rbegin()->first;
    }
  }

  double omegaL(TMath::TwoPi()*par[0]);
  double coeff1(par[1]/omegaL);
  double coeff2(coeff1*coeff1);
  double coeff3((1.0+coeff2)*par[1]);

  double w0t(omegaL*t);
  double j1, j0;
  if (fabs(w0t) < 1.e-5) { // check zero time limits of the spherical bessel functions j0(x) and j1(x)
    j0 = 1.0;
    j1 = 0.0;
  } else {
    j0 = TMath::Sin(w0t)/w0t;
    j1 = (TMath::Sin(w0t)-w0t*TMath::Cos(w0t))/(w0t*w0t);
  }

  if (fCalcNeeded) {
    fIntBesselJ0Exp->SetParameters(par);
    fIntValues[t] = fIntValues[fLastTime] + fIntBesselJ0Exp->IntegrateFunc(fLastTime,t);
    fCalcNeeded = false;
  }

  return (1.0-(coeff1*TMath::Exp(-par[1]*t)*j1)-(coeff2*(j0*TMath::Exp(-par[1]*t)-1.0))-coeff3*fIntValues[t]);
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 * - read XML startup file
 * - initialize variables
 * - read FFTW3 wisdom if desired
 * - allocate memory for the Laplace transform
 * - create FFTW3 plans for the Laplace transform
 */
TLFDynGssKT::TLFDynGssKT() : fCalcNeeded(true), fFirstCall(true), fCounter(0) {

#ifdef HAVE_LIBFFTW3F_THREADS
  int init_threads(fftwf_init_threads());
  if (!init_threads)
    cout << "TLFDynGssKT::TLFDynGssKT: Couldn't initialize multiple FFTW-threads ..." << endl;
  else {
#ifdef HAVE_GOMP
    fftwf_plan_with_nthreads(omp_get_num_procs());
#else
    fftwf_plan_with_nthreads(2);
#endif /* HAVE_GOMP */
  }
#endif /* HAVE_LIBFFTW3F_THREADS */

  // read startup file
  string startup_path_name("BMW_startup.xml");

  TSAXParser *saxParser = new TSAXParser();
  BMWStartupHandler *startupHandler = new BMWStartupHandler();
  saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
  int status (saxParser->ParseFile(startup_path_name.c_str()));
  // check for parse errors
  if (status) { // error
    cerr << endl << "**ERROR** reading/parsing " << startup_path_name << " failed." \
         << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
         << endl;
    assert(false);
  }

  fNSteps = startupHandler->GetNStepsLF();
  fDt = startupHandler->GetDeltat();
  fWisdom = startupHandler->GetWisdomFileFloat();

  // Calculate d_omega and C for given NFFT and dt
  fDw = PI/(fNSteps*fDt);
  fC = 2.0*TMath::Log(double(fNSteps))/(double(fNSteps-1)*fDt);

  fUseWisdom = true;
  int wisdomLoaded(0);

  FILE *wordsOfWisdomR;
  wordsOfWisdomR = fopen(fWisdom.c_str(), "r");
  if (wordsOfWisdomR == NULL) {
    fUseWisdom = false;
  } else {
    wisdomLoaded = fftwf_import_wisdom_from_file(wordsOfWisdomR);
    fclose(wordsOfWisdomR);
  }

  if (!wisdomLoaded) {
    fUseWisdom = false;
  }

  // allocating memory for the FFtransform pairs and create the FFT plans

  fFFTtime = (float *)malloc(sizeof(float) * fNSteps);
  fFFTfreq = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * (fNSteps/2+1));

  if (fUseWisdom) {
    fFFTplanFORW = fftwf_plan_dft_r2c_1d(fNSteps, fFFTtime, fFFTfreq, FFTW_EXHAUSTIVE);
    fFFTplanBACK = fftwf_plan_dft_c2r_1d(fNSteps, fFFTfreq, fFFTtime, FFTW_EXHAUSTIVE);
  } else {
    fFFTplanFORW = fftwf_plan_dft_r2c_1d(fNSteps, fFFTtime, fFFTfreq, FFTW_ESTIMATE);
    fFFTplanBACK = fftwf_plan_dft_c2r_1d(fNSteps, fFFTfreq, fFFTtime, FFTW_ESTIMATE);
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor
 * - write FFTW3 wisdom if desired
 * - free memory used for the Laplace transform
 */
TLFDynGssKT::~TLFDynGssKT() {
  // export FFTW Wisdom so it has not to be checked for the FFT-plan next time
  if (fUseWisdom) {
    FILE *wordsOfWisdomW;
    wordsOfWisdomW = fopen(fWisdom.c_str(), "w");
    if (wordsOfWisdomW == NULL) {
      cout << "TLFDynGssKT::~TLFDynGssKT(): Could not open file ... No wisdom is exported..." << endl;
    } else {
      fftwf_export_wisdom_to_file(wordsOfWisdomW);
      fclose(wordsOfWisdomW);
    }
  }
  // END of Wisdom Export

  // clean up
  fftwf_destroy_plan(fFFTplanFORW);
  fftwf_destroy_plan(fFFTplanBACK);
  free(fFFTtime);
  fftwf_free(fFFTfreq);
  cout << "TLFDynGssKT::~TLFDynGssKT(): " << fCounter << " full FFT cycles needed..." << endl;
}

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 * <p>Method returning the function value at a given time for a given set of parameters.
 *
 * \param t time \htmlonly (&#956;s) \endhtmlonly \latexonly ($\mu\mathrm{s}$) \endlatexonly
 * \param par parameters [\htmlonly &#957;<sub>L</sub>=<i>B</i>&#947;<sub>&#956;</sub>/2&#960; (MHz), &#963; (&#956;s<sup>-1</sup>), &#957; (MHz)\endhtmlonly \latexonly $\nu_{\mathrm{L}}=B\gamma_{\mu}/2\pi~(\mathrm{MHz})$, $\sigma~(\mu\mathrm{s}^{-1})$, $\nu~(\mathrm{MHz})$ \endlatexonly]
 */
double TLFDynGssKT::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 3); // three parameters nuL=gbar*B,sigma,fluct.rate nu (fourth parameter: t[us] for SG-function)

  if(t<0.0)
    return 1.0;

  if(t>20.0)
    return 0.0;

  if(fFirstCall){
    fPar = par;
    fFirstCall=false;
  }

  for (unsigned int i(0); i<3; ++i) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      fCalcNeeded=true;
    }
  }

  double sigsq(par[1]*par[1]); // sigma^2
  double omegaL(TWOPI*par[0]); // Larmor frequency
  double nusq(par[2]*par[2]); // nu^2
  double nut(par[2]*t); // nu*t
  double omegaLsq(omegaL*omegaL); // omega^2
  double omegaLnusqp(omegaLsq+nusq); //omega^2+nu^2
  double omegaLnusqm(omegaLsq-nusq); //omega^2-nu^2
  double wt(omegaL*t); // omega*t
  double enut(exp(-nut)); // exp(-nu*t)

  if(fabs(par[0])<0.00135538817){ // Zero Field
    if(!nusq){
      double sigsqtsq(sigsq*t*t);
      return 0.33333333333333333333+0.66666666666666666667*(1.0-sigsqtsq)*exp(-0.5*sigsqtsq); // ZF static Gauss KT
    }
//    return exp(-2.0*sigsq/nusq*(enut-1.0+nut)); // ZF Abragam Delta^2->2*Delta^2 (only here for TESTING - DO NOT return this value in an production environment!!!!!!)
  }

  if((par[2] >= 5.0*par[1]) || (omegaL >= 10.0*par[1])) {
    return exp(-2.0*sigsq/(omegaLnusqp*omegaLnusqp)*(omegaLnusqp*nut+omegaLnusqm*(1.0-enut*cos(wt))-2.0*par[2]*omegaL*enut*sin(wt))); // Keren
  }

  if(fCalcNeeded) {

/*    double t1,t2;
    // get start time
    struct timeval tv_start, tv_stop;
    gettimeofday(&tv_start, 0);
*/
    double tt(0.), sigsqtsq(0.);
    int i;

    if(fabs(par[0])<0.00135538817) {
      double mcplusnu(-(fC+par[2]));
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(i,tt) schedule(dynamic)
      #endif
      for(i = 0; i < static_cast<int>(fNSteps); ++i) {
        tt=static_cast<double>(i)*fDt;
        sigsqtsq=sigsq*tt*tt;
        fFFTtime[i]=(0.33333333333333333333+0.66666666666666666667*(1.0-sigsqtsq)*exp(-0.5*sigsqtsq))*exp(mcplusnu*tt)*fDt;
      }
    } else {
      double mcplusnu(-(fC+par[2]));
      double coeff1(2.0*sigsq/omegaLsq);
      double coeff2(coeff1*sigsq/omegaL);

      fFFTtime[0] = fDt;

      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(i,tt) schedule(dynamic)
      #endif
      for(i = 1; i < static_cast<int>(fNSteps); ++i) {
        tt=(static_cast<double>(i)-0.5)*fDt;
        fFFTtime[i]=(sin(omegaL*tt) * exp(-0.5*sigsq*tt*tt))*fDt;
      }

      double totoIntegrale(0.);

      for(i = 1; i < static_cast<int>(fNSteps); ++i) {
        tt=static_cast<double>(i)*fDt;
        totoIntegrale+=fFFTtime[i];
        fFFTtime[i]=(1.0-(coeff1*(1.0-exp(-0.5*sigsq*tt*tt)*cos(omegaL*tt)))+(coeff2*totoIntegrale))*exp(mcplusnu*tt)*fDt;
      }
    }
/*
    gettimeofday(&tv_stop, 0);
    t1 = (tv_stop.tv_sec - tv_start.tv_sec)*1000.0 + (tv_stop.tv_usec - tv_start.tv_usec)/1000.0;
*/
    // Transform to frequency domain

    fftwf_execute(fFFTplanFORW);

    // calculate F(s)

    double denom(1.0), imagsq(0.0), oneMINrealnu(1.0);

    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(i, imagsq, oneMINrealnu, denom) schedule(dynamic)
    #endif
    for (i = 0; i < static_cast<int>(fNSteps)/2+1; ++i) {
      imagsq=fFFTfreq[i][1]*fFFTfreq[i][1];
      oneMINrealnu=1.0-(par[2]*fFFTfreq[i][0]);
      denom=oneMINrealnu*oneMINrealnu + (nusq*imagsq);
      fFFTfreq[i][0] = (oneMINrealnu*fFFTfreq[i][0]-(par[2]*imagsq))/denom;
      fFFTfreq[i][1] /= denom;
    }

    // Transform back to time domain

    fftwf_execute(fFFTplanBACK);

//    for (unsigned int i(0); i<fNSteps; i++) {
//      fFFTtime[i]=(fDw*TMath::Exp(fC*i*fDt)/TMath::Pi()*fFFTtime[i]);
//    }
    fCalcNeeded=false;
    ++fCounter;
/*
    gettimeofday(&tv_stop, 0);
    t2 = (tv_stop.tv_sec - tv_start.tv_sec)*1000.0 + (tv_stop.tv_usec - tv_start.tv_usec)/1000.0;
    cout << "# Calculation times: " << t1 << " (ms), " << t2 << " (ms)" << endl;
*/
  }
//  return fFFTtime[int(t/fDt)];
  return fDw*exp(fC*t)/PI*fFFTtime[static_cast<int>(t/fDt)];
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor: create dynamic-Gaussian-depolarization objects for a subset of static widths and initialize the parameter vector.
 */
TLFDynSG::TLFDynSG() {
  for(unsigned int i(0); i<Nsteps; ++i)
    fLFDynGssIntegral.push_back(new TLFDynGssKT());
  fPar.resize(3, 0.0);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor: free the memory of the dynamic-Gaussian-depolarization objects and cleanup the parameter vector.
 */
TLFDynSG::~TLFDynSG() {
  for(unsigned int i(0); i<Nsteps; ++i)
    delete fLFDynGssIntegral[i];
  fLFDynGssIntegral.clear();
  fPar.clear();
}

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 * <p>Method returning the function value at a given time for a given set of parameters.
 *
 * \param t time \htmlonly (&#956;s) \endhtmlonly \latexonly ($\mu\mathrm{s}$) \endlatexonly
 * \param par parameters [\htmlonly &#957;<sub>L</sub>=<i>B</i>&#947;<sub>&#956;</sub>/2&#960; (MHz), <i>a</i> (&#956;s<sup>-1</sup>), &#957; (MHz)\endhtmlonly \latexonly $\nu_{\mathrm{L}}=B\gamma_{\mu}/2\pi~(\mathrm{MHz})$, $a~(\mu\mathrm{s}^{-1})$, $\nu~(\mathrm{MHz})$ \endlatexonly]
 */
double TLFDynSG::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 3); // three parameters nuL=gbar*B, a ,fluct.rate nu

  if(t<0.0)
    return 1.0;

  if(t>20.0)
    return 0.0;

  const double upperCutOff(50.0); // times a

  // Parameters for the integration
  for (unsigned int i(0); i<3; ++i)
    fPar[i] = par[i];

  double sigma_step(upperCutOff*par[1]/static_cast<double>(Nsteps));
  // at the moment, integrate between 0 and 50a (a "real upper limit" would be 10000a)
  double integral(0.0);
  double a_sigmaSq(0.0);
  double rho(0.0);
  double rhoNormalizer(0.0); // is [1/]sqrt(2./PI) when the integration is done between 0 and infinity

  for (unsigned int i(0); i<Nsteps; ++i) {
    fPar[1] = (static_cast<double>(i) + 0.5)*sigma_step;
    a_sigmaSq = (par[1]/(fPar[1]*fPar[1]));
    rho = a_sigmaSq*exp(-0.5*par[1]*a_sigmaSq);
    rhoNormalizer += rho;
    integral += (*(fLFDynGssIntegral[i]))(t, fPar)*rho;
  }
  integral /= rhoNormalizer;

  //cout << "value at " << t << ": " << integral << endl;

  return integral;

}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 * - read XML startup file
 * - initialize variables
 * - read FFTW3 wisdom if desired
 * - allocate memory for the Laplace transform
 * - create FFTW3 plans for the Laplace transform
 */
TLFDynExpKT::TLFDynExpKT() : fCalcNeeded(true), fFirstCall(true), fCounter(0), fL1(0.0), fL2(0.0) {

#ifdef HAVE_LIBFFTW3F_THREADS
  int init_threads(fftwf_init_threads());
  if (!init_threads)
    cout << "TLFDynExpKT::TLFDynExpKT: Couldn't initialize multiple FFTW-threads ..." << endl;
  else {
#ifdef HAVE_GOMP
    fftwf_plan_with_nthreads(omp_get_num_procs());
#else
    fftwf_plan_with_nthreads(2);
#endif /* HAVE_GOMP */
  }
#endif /* HAVE_LIBFFTW3F_THREADS */

  // read startup file
  string startup_path_name("BMW_startup.xml");

  TSAXParser *saxParser = new TSAXParser();
  BMWStartupHandler *startupHandler = new BMWStartupHandler();
  saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
  int status (saxParser->ParseFile(startup_path_name.c_str()));
  // check for parse errors
  if (status) { // error
    cerr << endl << "**ERROR** reading/parsing " << startup_path_name << " failed." \
         << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
         << endl;
    assert(false);
  }

  fNSteps = startupHandler->GetNStepsLF();
  fDt = startupHandler->GetDeltat();
  fWisdom = startupHandler->GetWisdomFileFloat();

  // Calculate d_omega and C for given NFFT and dt
  fDw = PI/(fNSteps*fDt);
  fC = 2.0*TMath::Log(double(fNSteps))/(double(fNSteps-1)*fDt);

  fUseWisdom = true;
  int wisdomLoaded(0);

  FILE *wordsOfWisdomR;
  wordsOfWisdomR = fopen(fWisdom.c_str(), "r");
  if (wordsOfWisdomR == NULL) {
    fUseWisdom = false;
  } else {
    wisdomLoaded = fftwf_import_wisdom_from_file(wordsOfWisdomR);
    fclose(wordsOfWisdomR);
  }

  if (!wisdomLoaded) {
    fUseWisdom = false;
  }

  // allocating memory for the FFtransform pairs and create the FFT plans

  fFFTtime = (float *)malloc(sizeof(float) * fNSteps);
  fFFTfreq = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * (fNSteps/2+1));

  if (fUseWisdom) {
    fFFTplanFORW = fftwf_plan_dft_r2c_1d(fNSteps, fFFTtime, fFFTfreq, FFTW_EXHAUSTIVE);
    fFFTplanBACK = fftwf_plan_dft_c2r_1d(fNSteps, fFFTfreq, fFFTtime, FFTW_EXHAUSTIVE);
  } else {
    fFFTplanFORW = fftwf_plan_dft_r2c_1d(fNSteps, fFFTtime, fFFTfreq, FFTW_ESTIMATE);
    fFFTplanBACK = fftwf_plan_dft_c2r_1d(fNSteps, fFFTfreq, fFFTtime, FFTW_ESTIMATE);
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor
 * - write FFTW3 wisdom if desired
 * - free memory used for the Laplace transform
 */
TLFDynExpKT::~TLFDynExpKT() {
  // export FFTW Wisdom so it has not to be checked for the FFT-plan next time
  if (fUseWisdom) {
    FILE *wordsOfWisdomW;
    wordsOfWisdomW = fopen(fWisdom.c_str(), "w");
    if (wordsOfWisdomW == NULL) {
      cout << "TLFDynExpKT::~TLFDynExpKT(): Could not open file ... No wisdom is exported..." << endl;
    } else {
      fftwf_export_wisdom_to_file(wordsOfWisdomW);
      fclose(wordsOfWisdomW);
    }
  }
  // END of Wisdom Export

  // clean up
  fftwf_destroy_plan(fFFTplanFORW);
  fftwf_destroy_plan(fFFTplanBACK);
  free(fFFTtime);
  fftwf_free(fFFTfreq);
  cout << "TLFDynExpKT::~TLFDynExpKT(): " << fCounter << " full FFT cyles needed..." << endl;
}

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 * <p>Method returning the function value at a given time for a given set of parameters.
 *
 * \param t time \htmlonly (&#956;s) \endhtmlonly \latexonly ($\mu\mathrm{s}$) \endlatexonly
 * \param par parameters [\htmlonly &#957;<sub>L</sub>=<i>B</i>&#947;<sub>&#956;</sub>/2&#960; (MHz), <i>a</i> (&#956;s<sup>-1</sup>), &#957; (MHz)\endhtmlonly \latexonly $\nu_{\mathrm{L}}=B\gamma_{\mu}/2\pi~(\mathrm{MHz})$, $a~(\mu\mathrm{s}^{-1})$, $\nu~(\mathrm{MHz})$ \endlatexonly]
 */
double TLFDynExpKT::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 3); // three parameters nuL=gbar*B,sigma,fluct.rate nu

  if(t<0.0)
    return 1.0;

  if(t>20.0)
    return 0.0;

  if(fFirstCall){
    fPar = par;
    fFirstCall=false;
  }

  for (unsigned int i(0); i<par.size(); i++) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      fCalcNeeded=true;
    }
  }

  double omegaL(TWOPI*par[0]); // Larmor frequency
  double a(par[1]);  // static width
  double nu(par[2]); // hopping rate

  if(fabs(par[0])<0.00135538817){ // Zero Field
    if(!par[2]){ // nu=0
      double at(par[1]*t);
      return 0.33333333333333333333+0.66666666666666666667*(1.0-at)*exp(-at); // ZF static Lorentz KT
    }
  }

  // semi-analytical approximation for {nu >= 5a} and {omegaL >= 30a}

  // check if hopping > 5 * damping, of Larmor angular frequency is > 30 * damping (BMW limit)
  if((par[2] > 5.0*par[1]) || (omegaL > 30.0*par[1])){
    if(fCalcNeeded){

      // 'c' and 'd' are parameters BMW obtained by fitting large parameter space LF-curves to the model below
      const double c[7] = {1.15331, 1.64826, -0.71763, 3.0, 0.386683, -5.01876, 2.41854};
      const double d[4] = {2.44056, 2.92063, 1.69581, 0.667277};
      double w0N[4];
      double nuN[4];
      w0N[0] = omegaL;
      nuN[0] = nu;
      for (unsigned int i=1; i<4; ++i) {
        w0N[i] = omegaL * w0N[i-1];
        nuN[i] = nu * nuN[i-1];
      }
      double denom(w0N[3]+d[0]*w0N[2]*nuN[0]+d[1]*w0N[1]*nuN[1]+d[2]*w0N[0]*nuN[2]+d[3]*nuN[3]);
      fL1 = (c[0]*w0N[2]+c[1]*w0N[1]*nuN[0]+c[2]*w0N[0]*nuN[1])/denom;
      fL2 = (c[3]*w0N[2]+c[4]*w0N[1]*nuN[0]+c[5]*w0N[0]*nuN[1]+c[6]*nuN[2])/denom;

      fCalcNeeded=false;
    }

    double w0t(omegaL*t);
    double j1, j0;
    if (fabs(w0t) < 1.e-5) { // check zero time limits of the spherical bessel functions j0(x) and j1(x)
      j0 = 1.0;
      j1 = 0.0;
    } else {
      j0 = TMath::Sin(w0t)/w0t;
      j1 = (TMath::Sin(w0t)-w0t*TMath::Cos(w0t))/(w0t*w0t);
    }

    double Gamma_t = -4.0/3.0*a*(fL1*(1.0-j0*TMath::Exp(-nu*t))+fL2*j1*TMath::Exp(-nu*t)+(1.0-fL2*omegaL/3.0-fL1*nu)*t);

    return TMath::Exp(Gamma_t);
  }

  // if no approximation can be used -> Laplace transform

  if(fCalcNeeded){

    double tt(0.);
    int i;

    if(TMath::Abs(par[0])<0.00135538817){
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(i, tt) schedule(dynamic)
      #endif
      for(i = 0; i < static_cast<int>(fNSteps); ++i) {
        tt=static_cast<double>(i)*fDt;
        fFFTtime[i]=(0.33333333333333333333+0.66666666666666666667*(1.0-par[1]*tt)*TMath::Exp(-par[1]*tt))*TMath::Exp(-(fC+par[2])*tt)*fDt;
      }
    } else {

      double coeff1(par[1]/omegaL);
      double coeff2(coeff1*coeff1);
      double coeff3((1.0+coeff2)*par[1]);

      fFFTtime[0] = 1.0*fDt;

      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(i, tt) schedule(dynamic)
      #endif
      for(i = 1; i < static_cast<int>(fNSteps); ++i) {
        tt=(double(i)-0.5)*fDt;
        fFFTtime[i]=TMath::Sin(omegaL*tt)/(omegaL*tt)*TMath::Exp(-par[1]*tt)*fDt;
      }

      double totoIntegrale(0.);
      double w0t(1.0);

      for(i = 1; i < static_cast<int>(fNSteps); ++i) {
        tt=double(i)*fDt;
        totoIntegrale+=fFFTtime[i];
        w0t = omegaL*tt;
        fFFTtime[i]=(1.0-(coeff1*TMath::Exp(-par[1]*tt)*(TMath::Sin(w0t)-w0t*TMath::Cos(w0t))/(w0t*w0t))-(coeff2*(TMath::Sin(w0t)/(w0t)*TMath::Exp(-par[1]*tt)-1.0))-coeff3*totoIntegrale)*TMath::Exp(-(fC+par[2])*tt)*fDt;
      }
    }

    // Transform to frequency domain

    fftwf_execute(fFFTplanFORW);

    // calculate F(s)

    double nusq(par[2]*par[2]); // nu^2
    double denom(1.0), imagsq(0.0), oneMINrealnu(1.0);

    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(i, imagsq, oneMINrealnu, denom) schedule(dynamic)
    #endif
    for (i = 0; i < static_cast<int>(fNSteps)/2+1; ++i) {
      imagsq=fFFTfreq[i][1]*fFFTfreq[i][1];
      oneMINrealnu=1.0-(par[2]*fFFTfreq[i][0]);
      denom=oneMINrealnu*oneMINrealnu + (nusq*imagsq);
      fFFTfreq[i][0] = (oneMINrealnu*fFFTfreq[i][0]-(par[2]*imagsq))/denom;
      fFFTfreq[i][1] /= denom;
    }

    // Transform back to time domain

    fftwf_execute(fFFTplanBACK);

//    for (unsigned int i(0); i<fNSteps; i++) {
//      fFFTtime[i]=(fDw*TMath::Exp(fC*i*fDt)/TMath::Pi()*fFFTtime[i]);
//    }
    fCalcNeeded=false;
    ++fCounter;
  }
//  return fFFTtime[int(t/fDt)];
  return fDw*TMath::Exp(fC*t)/TMath::Pi()*fFFTtime[static_cast<int>(t/fDt)];
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor: Initialize variables and allocate memory for the integrator.
 */
TLFSGInterpolation::TLFSGInterpolation() : fCalcNeeded(true), fLastTime(0.) {
  fIntegral = new TIntSGInterpolation();
  fPar.resize(4, 0.);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor: Free memory of the integrator, clean up some vectors.
 */
TLFSGInterpolation::~TLFSGInterpolation() {
  delete fIntegral;
  fIntegral = 0;
  fPar.clear();
  fIntValues.clear();
}

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 * <p>Method returning the function value at a given time for a given set of parameters.
 *
 * \param t time \htmlonly (&#956;s) \endhtmlonly \latexonly ($\mu\mathrm{s}$) \endlatexonly
 * \param par parameters [\htmlonly &#957;<sub>L</sub>=<i>B</i>&#947;<sub>&#956;</sub>/2&#960; (MHz), <i>a</i> (&#956;s<sup>-1</sup>), &#955; (&#956;s<sup>-1</sup>), &#946; (1) \endhtmlonly \latexonly $\nu_{\mathrm{L}}=B\gamma_{\mu}/2\pi~(\mathrm{MHz})$, $a~(\mu\mathrm{s}^{-1})$, $\lambda~(\mu\mathrm{s}^{-1})$, $\beta~(1)$ \endlatexonly]
 */
double TLFSGInterpolation::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 4); // four parameters nu=gbar*B [MHz], a [1/us], lambda [1/us], beta [1]

  if((t <= 0.0) || (par[3] <= 0.0))
    return 1.0;

  double expo(0.5*par[1]*par[1]*t*t/par[3]+par[2]*t);

  if (TMath::Abs(par[0]) < 0.00135538817) {
    return (0.33333333333333333333*TMath::Exp(-TMath::Power(par[2]*t,par[3])) + \
      0.66666666666666666667*(1.0-par[1]*par[1]*t*t/TMath::Power(expo,(1.0-par[3])))*TMath::Exp(-TMath::Power(expo,par[3])));
  }

  for (unsigned int i(0); i<4; ++i) {
    if( fPar[i]-par[i] ) {
      fPar[i] = par[i];
      fCalcNeeded = true;
    }
  }

  if (fCalcNeeded) {
    fIntValues.clear();
    fIntValues[0.] = 0.;
    fLastTime = 0.;
  } else {
    if (t > fIntValues.rbegin()->first) {
      fCalcNeeded = true;
      fLastTime = fIntValues.rbegin()->first;
    }
  }

  double omegaL(TMath::TwoPi()*par[0]);

  if (fCalcNeeded) {
    vector<double> intpar(par);
    intpar.push_back(t);
    fIntegral->SetParameters(intpar);
    fIntValues[t] = fIntValues[fLastTime] + fIntegral->IntegrateFunc(fLastTime,t);
    intpar.clear();
  }

  return (TMath::Exp(-TMath::Power(par[2]*t,par[3])) + 2.0*par[1]*par[1]/(omegaL*omegaL) * \
    ((TMath::Cos(omegaL*t)-TMath::Sin(omegaL*t)/(omegaL*t))*TMath::Exp(-TMath::Power(expo,par[3]))/TMath::Power(expo,(1.0-par[3])) + \
      fIntValues[t]));
}
