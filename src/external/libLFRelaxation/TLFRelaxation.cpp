/***************************************************************************

  TLFRelaxation.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/12/04

***************************************************************************/

#include <cassert>
#include <sys/time.h>
#include <iostream>

#include "TIntegrator.h"
#include "TLFRelaxation.h"

using namespace std;

#define PI 3.14159265358979323846
#define TWOPI 6.28318530717958647692


ClassImp(TLFStatGssKT)
ClassImp(TLFStatLorKT)
ClassImp(TLFDynGssKT)
ClassImp(TLFDynLorKT)


// LF Static Gaussian KT

TLFStatGssKT::TLFStatGssKT() {
  TIntSinGss *intSinGss = new TIntSinGss();
  fIntSinGss = intSinGss;
  intSinGss = 0;
  delete intSinGss;
}

TLFStatGssKT::~TLFStatGssKT() {
    delete fIntSinGss;
    fIntSinGss = 0;
}

double TLFStatGssKT::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 2); // two parameters nu=gbar*B,sigma

  if(t<0.0)
    return 1.0;

  double sigsq(par[1]*par[1]);

  if(TMath::Abs(par[0])<0.00135538817){
    return (0.33333333333333333333+0.66666666666666666667*(1.0-sigsq*t*t)*TMath::Exp(-0.5*sigsq*t*t));
  }

  fIntSinGss->SetParameters(par);

  double omegaL(TMath::TwoPi()*par[0]);
  double coeff1(2.0*sigsq/(omegaL*omegaL));
  double coeff2(2.0*sigsq*sigsq/(omegaL*omegaL*omegaL));

  return (1.0-(coeff1*(1.0-TMath::Exp(-0.5*sigsq*t*t)*TMath::Cos(omegaL*t)))+(coeff2*fIntSinGss->IntegrateFunc(0.,t)));
}

// LF Static Lorentzian KT

TLFStatLorKT::TLFStatLorKT() {
  TIntBesselJ0Exp *intBesselJ0Exp = new TIntBesselJ0Exp();
  fIntBesselJ0Exp = intBesselJ0Exp;
  intBesselJ0Exp = 0;
  delete intBesselJ0Exp;
}

TLFStatLorKT::~TLFStatLorKT() {
    delete fIntBesselJ0Exp;
    fIntBesselJ0Exp = 0;
}

double TLFStatLorKT::operator()(double t, const vector<double> &par) const {

  assert(par.size() == 2); // two parameters nu=gbar*B,rate

  if(t<0.0)
    return 1.0;

  if(TMath::Abs(par[0])<0.00135538817){
    return (0.33333333333333333333+0.66666666666666666667*(1.0-par[1]*t)*TMath::Exp(-par[1]*t));
  }

  fIntBesselJ0Exp->SetParameters(par);

  double omegaL(TMath::TwoPi()*par[0]);
  double coeff1(par[1]/omegaL);
  double coeff2(coeff1*coeff1);
  double coeff3((1.0+coeff2)*par[1]);

  return (1.0-(coeff1*TMath::Exp(-par[1]*t)*TMath::BesselJ1(omegaL*t))-(coeff2*(TMath::BesselJ0(omegaL*t)*TMath::Exp(-par[1]*t)-1.0))-coeff3*fIntBesselJ0Exp->IntegrateFunc(0.,t));
}

// LF Dynamic Gaussian KT

TLFDynGssKT::TLFDynGssKT() : fCalcNeeded(true), fFirstCall(true), fWisdom("/home/l_wojek/analysis/WordsOfWisdom.dat"), fNSteps(524288), fDt(0.00004), fCounter(0) {
  // Calculate d_omega and C for given NFFT and dt
  fDw = PI/fNSteps/fDt;
  fC = 2.0*gsl_sf_log(double(fNSteps))/(double(fNSteps-1)*fDt);

  // Load FFTW Wisdom
  int wisdomLoaded(0);

  FILE *wordsOfWisdomR;
  wordsOfWisdomR = fopen(fWisdom.c_str(), "r");
  if (wordsOfWisdomR == NULL) {
    cout << "TLFDynGssKT::TLFDynGssKT: Couldn't open wisdom file ..." << endl;
  } else {
    wisdomLoaded = fftw_import_wisdom_from_file(wordsOfWisdomR);
    fclose(wordsOfWisdomR);
  }

  if (!wisdomLoaded) {
    cout << "TLFDynGssKT::TLFDynGssKT: No wisdom is imported..." << endl;
  }
  // END of WisdomLoading

  // allocating memory for the FFtransform pairs and create the FFT plans

  fFFTtime = (double *)malloc(sizeof(double) * fNSteps);
  fFFTfreq = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (fNSteps/2+1));
  fFFTplanFORW = fftw_plan_dft_r2c_1d(fNSteps, fFFTtime, fFFTfreq, FFTW_ESTIMATE);
  fFFTplanBACK = fftw_plan_dft_c2r_1d(fNSteps, fFFTfreq, fFFTtime, FFTW_ESTIMATE);
}

TLFDynGssKT::~TLFDynGssKT() {
  // export FFTW Wisdom so it has not to be checked for the FFT-plan next time
  FILE *wordsOfWisdomW;
  wordsOfWisdomW = fopen(fWisdom.c_str(), "w");
  if (wordsOfWisdomW == NULL) {
    cout << "TLFDynGssKT::~TLFDynGssKT: Could not open file ... No wisdom is exported..." << endl;
  } else {
    fftw_export_wisdom_to_file(wordsOfWisdomW);
    fclose(wordsOfWisdomW);
  }
  // END of Wisdom Export

  // clean up
  fftw_destroy_plan(fFFTplanFORW);
  fftw_destroy_plan(fFFTplanBACK);
  free(fFFTtime);
  fftw_free(fFFTfreq);
  cout << "TLFDynGssKT::~TLFDynGssKT(): " << fCounter << " full FFT cycles needed..." << endl;
}

double TLFDynGssKT::operator()(double t, const vector<double> &par) const {

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
    return exp(-2.0*sigsq/nusq*(enut-1.0+nut)); // ZF Abragam Delta^2->2*Delta^2
  }

  if((par[2] >= 5.0*par[1]) || (omegaL >= 10.0*par[1])) {
    return exp(-2.0*sigsq/(omegaLnusqp*omegaLnusqp)*(omegaLnusqp*nut+omegaLnusqm*(1.0-enut*gsl_sf_cos(wt))-2.0*par[2]*omegaL*enut*gsl_sf_sin(wt))); // Keren
  }

  if(fCalcNeeded) {

    double t1,t2;
    // get start time
    struct timeval tv_start, tv_stop;
    gettimeofday(&tv_start, 0);

    double tt(0.),sigsqtsq(0.);

    if(fabs(par[0])<0.00135538817) {
      double mcplusnu(-(fC+par[2]));
      for(unsigned int i(0); i<fNSteps; i++) {
        tt=double(i)*fDt;
        sigsqtsq=sigsq*tt*tt;
        fFFTtime[i]=(0.33333333333333333333+0.66666666666666666667*(1.0-sigsqtsq)*exp(-0.5*sigsqtsq))*exp(mcplusnu*tt)*fDt;
      }
    } else {
      double mcplusnu(-(fC+par[2]));
      double coeff1(2.0*sigsq/omegaLsq);
      double coeff2(coeff1*sigsq/omegaL);

      fFFTtime[0] = fDt;

      for(unsigned int i(1); i<fNSteps; i++) {
        tt=(double(i)-0.5)*fDt;
        fFFTtime[i]=(gsl_sf_sin(omegaL*tt) * exp(-0.5*sigsq*tt*tt))*fDt;
      }

      double totoIntegrale(0.);

      for(unsigned int i(1); i<fNSteps; i++) {
        tt=double(i)*fDt;
        totoIntegrale+=fFFTtime[i];
        fFFTtime[i]=(1.0-(coeff1*(1.0-exp(-0.5*sigsq*tt*tt)*gsl_sf_cos(omegaL*tt)))+(coeff2*totoIntegrale))*exp(mcplusnu*tt)*fDt;
      }
    }

    gettimeofday(&tv_stop, 0);
    t1 = (tv_stop.tv_sec - tv_start.tv_sec)*1000.0 + (tv_stop.tv_usec - tv_start.tv_usec)/1000.0;

    // Transform to frequency domain

    fftw_execute(fFFTplanFORW);

    // calculate F(s)

    double denom(1.0), imagsq(0.0), oneMINrealnu(1.0);

    for (unsigned int i(0); i<fNSteps/2+1; i++) {
      imagsq=fFFTfreq[i][1]*fFFTfreq[i][1];
      oneMINrealnu=1.0-(par[2]*fFFTfreq[i][0]);
      denom=oneMINrealnu*oneMINrealnu + (nusq*imagsq);
      fFFTfreq[i][0] = (oneMINrealnu*fFFTfreq[i][0]-(par[2]*imagsq))/denom;
      fFFTfreq[i][1] /= denom;
    }

    // Transform back to time domain

    fftw_execute(fFFTplanBACK);

//    for (unsigned int i(0); i<fNSteps; i++) {
//      fFFTtime[i]=(fDw*TMath::Exp(fC*i*fDt)/TMath::Pi()*fFFTtime[i]);
//    }
    fCalcNeeded=false;
    fCounter++;
    
    gettimeofday(&tv_stop, 0);
    t2 = (tv_stop.tv_sec - tv_start.tv_sec)*1000.0 + (tv_stop.tv_usec - tv_start.tv_usec)/1000.0;
    cout << "# Calculation times: " << t1 << " (ms), " << t2 << " (ms)" << endl;
    
  }
//  return fFFTtime[int(t/fDt)];
  return fDw*exp(fC*t)/PI*fFFTtime[int(t/fDt)];
}

// LF Dynamic Lorentz KT

TLFDynLorKT::TLFDynLorKT() : fCalcNeeded(true), fFirstCall(true), fWisdom("/home/l_wojek/analysis/WordsOfWisdom.dat"), fNSteps(524288), fDt(0.000040), fCounter(0), fA(1.0), fL1(0.0), fL2(0.0) {
  // Calculate d_omega and C for given NFFT and dt
  fDw = TMath::Pi()/fNSteps/fDt;
  fC = 2.0*TMath::Log(double(fNSteps))/(double(fNSteps-1)*fDt);

  // Load FFTW Wisdom
  int wisdomLoaded(0);

  FILE *wordsOfWisdomR;
  wordsOfWisdomR = fopen(fWisdom.c_str(), "r");
  if (wordsOfWisdomR == NULL) {
    cout << "TLFDynLorKT::TLFDynGssKT: Couldn't open wisdom file ..." << endl;
  } else {
    wisdomLoaded = fftw_import_wisdom_from_file(wordsOfWisdomR);
    fclose(wordsOfWisdomR);
  }

  if (!wisdomLoaded) {
    cout << "TLFDynLorKT::TLFDynGssKT: No wisdom is imported..." << endl;
  }
  // END of WisdomLoading

  // allocating memory for the FFtransform pairs and create the FFT plans

  fFFTtime = (double *)malloc(sizeof(double) * fNSteps);
  fFFTfreq = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (fNSteps/2+1));
  fFFTplanFORW = fftw_plan_dft_r2c_1d(fNSteps, fFFTtime, fFFTfreq, FFTW_EXHAUSTIVE);
  fFFTplanBACK = fftw_plan_dft_c2r_1d(fNSteps, fFFTfreq, fFFTtime, FFTW_EXHAUSTIVE);
}

TLFDynLorKT::~TLFDynLorKT() {
  // export FFTW Wisdom so it has not to be checked for the FFT-plan next time
  FILE *wordsOfWisdomW;
  wordsOfWisdomW = fopen(fWisdom.c_str(), "w");
  if (wordsOfWisdomW == NULL) {
    cout << "TLFDynGssKT::~TLFDynGssKT: Could not open file ... No wisdom is exported..." << endl;
  } else {
    fftw_export_wisdom_to_file(wordsOfWisdomW);
    fclose(wordsOfWisdomW);
  }
  // END of Wisdom Export

  // clean up
  fftw_destroy_plan(fFFTplanFORW);
  fftw_destroy_plan(fFFTplanBACK);
  free(fFFTtime);
  fftw_free(fFFTfreq);
  cout << "TLFDynLorKT::~TLFDynLorKT(): " << fCounter << " full FFT cyles needed..." << endl;
}

const double TLFDynLorKT::fX[16]={1.61849, 1.27059, 0.890315, 6.72608, 0.327258, 0.981975, 1.5964, 2.31023, 2.37689, 5.63945, 0.235734, 1.10617, 1.1664, 0.218402, 0.266562, 0.471331};

const double TLFDynLorKT::fY[20]={1.54699, 0.990321, 0.324923, 0.928399, 2.11155, 0.615106, 1.49907, 0.679423, 6.17621, 1.38907, 0.979608, 0.0593631, 0.806427, 3.33144, 1.05059, 1.29922, 0.254661, 0.284348, 0.991419, 0.375213};

double TLFDynLorKT::operator()(double t, const vector<double> &par) const {

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

  if(fabs(par[0])<0.00135538817){ // Zero Field
    if(!par[2]){ // nu=0
      double at(par[1]*t);
      return 0.33333333333333333333+0.66666666666666666667*(1.0-at)*exp(-at); // ZF static Lorentz KT
    }
  }

  // semi-analytical approximations for {nu >= 4a} and {omegaL >= 10a}

  if(par[2]){ // nu!=0
    if(par[2] >= 4.0*par[1]){ // nu >= 4a
      if(fCalcNeeded){
        double wLnu(omegaL/par[2]); // omegaL/nu
        double wLnusq(wLnu*wLnu); // (omegaL/nu)^2
        double anu(par[1]/par[2]); // a/nu

        fA=1.0+wLnu*anu*(-fX[0]/(wLnusq+fX[1])+fX[2]/(wLnusq*wLnu+fX[3])+fX[4]/(wLnusq*wLnusq+fX[5]));
        fL1=par[1]*(fX[6]/(wLnu+fX[7])+fX[8]/(wLnusq+fX[9])+fX[10]/(wLnusq*wLnusq+fX[11]));
        fL2=par[2]*(fX[12]*tanh(fX[13]*wLnu)+fX[14]*wLnu+fX[15]);

        fCalcNeeded=false;
      }
      return fA*exp(-fL1*t)+(1.0-fA)*exp(-fL2*t)*cos(omegaL*t);
    }
    if(omegaL >= 10.0*par[1]){ // omegaL >= 10a
      if(fCalcNeeded){
        double wLnu(omegaL/par[2]); // omegaL/nu
        double wLnusq(wLnu*wLnu); // (omegaL/nu)^2
        double anu(par[1]/par[2]); // a/nu

        fA=1.0-fY[0]*pow(anu,fY[1])/(wLnu+fY[2])+fY[3]*pow(anu,fY[4])/(wLnusq-fY[5])+fY[6]*pow(anu,fY[7])/(wLnusq*wLnu+fY[8]);
        fL1=par[1]*(fY[9]/(pow(wLnu,fY[10])+fY[11])-fY[12]/(pow(wLnu,fY[13])+fY[14]));
        fL2=par[2]*(fY[15]*tanh(fY[16]*wLnu)+fY[17]*pow(wLnu,fY[18])+fY[19]);

        fCalcNeeded=false;
      }
      return fA*exp(-fL1*t)+(1.0-fA)*exp(-fL2*t)*cos(omegaL*t);
    }
  }

  // if no approximation can be used -> Laplace transform

  if(fCalcNeeded){

    double tt(0.);

    if(TMath::Abs(par[0])<0.00135538817){
      for(unsigned int i(0); i<fNSteps; i++) {
        tt=double(i)*fDt;
        fFFTtime[i]=(0.33333333333333333333+0.66666666666666666667*(1.0-par[1]*tt)*TMath::Exp(-par[1]*tt))*TMath::Exp(-(fC+par[2])*tt)*fDt;
      }
    } else {

      double coeff1(par[1]/omegaL);
      double coeff2(coeff1*coeff1);
      double coeff3((1.0+coeff2)*par[1]);

      fFFTtime[0] = 1.0*fDt;

      for(unsigned int i(1); i<fNSteps; i++) {
        tt=(double(i)-0.5)*fDt;
        fFFTtime[i]=TMath::BesselJ0(omegaL*tt)*TMath::Exp(-par[1]*tt)*fDt;
      }

      double totoIntegrale(0.);

      for(unsigned int i(1); i<fNSteps; i++) {
        tt=double(i)*fDt;
        totoIntegrale+=fFFTtime[i];
        fFFTtime[i]=(1.0-(coeff1*TMath::Exp(-par[1]*tt)*TMath::BesselJ1(omegaL*tt))-(coeff2*(TMath::BesselJ0(omegaL*tt)*TMath::Exp(-par[1]*tt)-1.0))-coeff3*totoIntegrale)*TMath::Exp(-(fC+par[2])*tt)*fDt;
      }
    }

    // Transform to frequency domain

    fftw_execute(fFFTplanFORW);

    // calculate F(s)

    double nusq(par[2]*par[2]); // nu^2
    double denom(1.0), imagsq(0.0), oneMINrealnu(1.0);

    for (unsigned int i(0); i<fNSteps/2+1; i++) {
      imagsq=fFFTfreq[i][1]*fFFTfreq[i][1];
      oneMINrealnu=1.0-(par[2]*fFFTfreq[i][0]);
      denom=oneMINrealnu*oneMINrealnu + (nusq*imagsq);
      fFFTfreq[i][0] = (oneMINrealnu*fFFTfreq[i][0]-(par[2]*imagsq))/denom;
      fFFTfreq[i][1] /= denom;
    }

    // Transform back to time domain

    fftw_execute(fFFTplanBACK);

//    for (unsigned int i(0); i<fNSteps; i++) {
//      fFFTtime[i]=(fDw*TMath::Exp(fC*i*fDt)/TMath::Pi()*fFFTtime[i]);
//    }
    fCalcNeeded=false;
    fCounter++;
  }
//  return fFFTtime[int(t/fDt)];
  return fDw*TMath::Exp(fC*t)/TMath::Pi()*fFFTtime[int(t/fDt)];
}
