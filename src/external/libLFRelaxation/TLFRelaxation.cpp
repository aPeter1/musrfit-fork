/***************************************************************************

  TLFRelaxation.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/12/04

***************************************************************************/

#include <cassert>

#include "TIntegrator.h"
#include "TLFRelaxation.h"

using namespace std;

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

TLFDynGssKT::TLFDynGssKT() : fCalcNeeded(true), fFirstCall(true), fWisdom("/home/l_wojek/analysis/WordsOfWisdom.dat"), fNSteps(524288), fDt(0.000040), fCounter(0) {
  // Calculate d_omega and C for given NFFT and dt
  fDw = TMath::Pi()/fNSteps/fDt;
  fC = 2.0*TMath::Log(double(fNSteps))/(double(fNSteps-1)*fDt);

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
  fFFTplanFORW = fftw_plan_dft_r2c_1d(fNSteps, fFFTtime, fFFTfreq, FFTW_EXHAUSTIVE);
  fFFTplanBACK = fftw_plan_dft_c2r_1d(fNSteps, fFFTfreq, fFFTtime, FFTW_EXHAUSTIVE);
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
  double omegaL(TMath::TwoPi()*par[0]); // Larmor frequency
  double nusq(par[2]*par[2]); // nu^2

  if(par[1]){
    if(par[2]/par[1] > 5. || omegaL > 20.0*par[1]){
      if(TMath::Abs(par[0])<0.00135538817){
        return TMath::Exp(-2.0*sigsq/nusq*(TMath::Exp(-par[2]*t)-1.0+par[2]*t)); // ZF Abragam Delta^2->2*Delta^2
      }
      double omegaLnusqp(omegaL*omegaL+nusq);
      double omegaLnusqm(omegaL*omegaL-nusq);
      return TMath::Exp(-2.0*sigsq/(omegaLnusqp*omegaLnusqp)*(omegaLnusqp*par[2]*t+omegaLnusqm*(1.0-TMath::Exp(-par[2]*t)*TMath::Cos(omegaL*t))-2.0*par[2]*omegaL*TMath::Exp(-par[2]*t)*TMath::Sin(omegaL*t))); // Keren
    }
  }

  if(fCalcNeeded){

    double tt(0.);

    if(TMath::Abs(par[0])<0.00135538817){
      for(unsigned int i(0); i<fNSteps; i++) {
        tt=double(i)*fDt;
        fFFTtime[i]=(0.33333333333333333333+0.66666666666666666667*(1.0-sigsq*tt*tt)*TMath::Exp(-0.5*sigsq*tt*tt))*TMath::Exp(-(fC+par[2])*tt)*fDt;
      }
    } else {

      double coeff1(2.0*sigsq/(omegaL*omegaL));
      double coeff2(2.0*sigsq*sigsq/(omegaL*omegaL*omegaL));

      fFFTtime[0] = 1.0*fDt;

      for(unsigned int i(1); i<fNSteps; i++) {
        tt=(double(i)-0.5)*fDt;
        fFFTtime[i]=(TMath::Sin(omegaL*tt) * TMath::Exp(-0.5*sigsq*tt*tt))*fDt;
      }

      double totoIntegrale(0.);

      for(unsigned int i(1); i<fNSteps; i++) {
        tt=double(i)*fDt;
        totoIntegrale+=fFFTtime[i];
        fFFTtime[i]=(1.0-(coeff1*(1.0-TMath::Exp(-0.5*sigsq*tt*tt)*TMath::Cos(omegaL*tt)))+(coeff2*totoIntegrale))*TMath::Exp(-(fC+par[2])*tt)*fDt;
      }
    }

    // Transform to frequency domain

    fftw_execute(fFFTplanFORW);

    // calculate F(s)

    double denom(1.0);

    for (unsigned int i(0); i<fNSteps/2+1; i++) {
      denom=(1.0-(par[2]*fFFTfreq[i][0]))*(1.0-(par[2]*fFFTfreq[i][0])) + (nusq*fFFTfreq[i][1]*fFFTfreq[i][1]);
      fFFTfreq[i][0] = (fFFTfreq[i][0]-(par[2]*fFFTfreq[i][0]*fFFTfreq[i][0])-(par[2]*fFFTfreq[i][1]*fFFTfreq[i][1]))/denom;
      fFFTfreq[i][1] = fFFTfreq[i][1]/denom;
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

// LF Dynamic Lorentz KT

TLFDynLorKT::TLFDynLorKT() : fCalcNeeded(true), fFirstCall(true), fWisdom("/home/l_wojek/analysis/WordsOfWisdom.dat"), fNSteps(524288), fDt(0.000040), fCounter(0) {
  // Calculate d_omega and C for given NFFT and dt
  fDw = TMath::Pi()/fNSteps/fDt;
  fC = 2.0*TMath::Log(double(fNSteps))/(double(fNSteps-1)*fDt);

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

  if(fCalcNeeded){

    double tt(0.);

    if(TMath::Abs(par[0])<0.00135538817){
      for(unsigned int i(0); i<fNSteps; i++) {
        tt=double(i)*fDt;
        fFFTtime[i]=(0.33333333333333333333+0.66666666666666666667*(1.0-par[1]*tt)*TMath::Exp(-par[1]*tt))*TMath::Exp(-(fC+par[2])*tt)*fDt;
      }
    } else {

      double omegaL(TMath::TwoPi()*par[0]); // Larmor frequency
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

    double denom(1.0);
    double nusq(par[2]*par[2]); // nu^2

    for (unsigned int i(0); i<fNSteps/2+1; i++) {
      denom=(1.0-(par[2]*fFFTfreq[i][0]))*(1.0-(par[2]*fFFTfreq[i][0])) + (nusq*fFFTfreq[i][1]*fFFTfreq[i][1]);
      fFFTfreq[i][0] = (fFFTfreq[i][0]-(par[2]*fFFTfreq[i][0]*fFFTfreq[i][0])-(par[2]*fFFTfreq[i][1]*fFFTfreq[i][1]))/denom;
      fFFTfreq[i][1] = fFFTfreq[i][1]/denom;
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
