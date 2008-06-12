/***************************************************************************

  TPofTCalc.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/06/03

***************************************************************************/

#include "TPofTCalc.h"
#include "fftw3.h"
#include <cmath>
#include <iostream>
#include <cstdio>

/* USED FOR DEBUGGING -----------------------
#include <ctime>
#include <fstream>
--------------------------------------------*/

//------------------
// Constructor of the TPofTCalc class - it creates the FFT plan
// Parameters: phase, dt, dB
//------------------

TPofTCalc::TPofTCalc (const string &wisdom, const vector<double> &par) : fWisdom(wisdom) {

  fNFFT = ( int(1.0/gBar/par[1]/par[2]+1.0) % 2 ) ? int(1.0/gBar/par[1]/par[2]+2.0) : int(1.0/gBar/par[1]/par[2]+1.0);
  fTBin = 1.0/gBar/double(fNFFT-1)/par[2];

  fFFTin = (double *)malloc(sizeof(double) * fNFFT);
  fFFTout = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (fNFFT/2+1));

  cout << "Check for the FFT plan..." << endl;

  // Load wisdom from file

  int wisdomLoaded(0);

  FILE *wordsOfWisdomR;
  wordsOfWisdomR = fopen(fWisdom.c_str(), "r");
  if (wordsOfWisdomR == NULL) {
    cout << "Couldn't open wisdom file ..." << endl;
  } else { 
    wisdomLoaded = fftw_import_wisdom_from_file(wordsOfWisdomR);
    fclose(wordsOfWisdomR);
  }

  if (!wisdomLoaded) {
    cout << "No wisdom is imported..." << endl;
  }

  fFFTplan = fftw_plan_dft_r2c_1d(fNFFT, fFFTin, fFFTout, FFTW_EXHAUSTIVE);
  cout << &fFFTplan << endl;
}

//--------------
// Method that does the FFT of a given p(B)
//--------------

void TPofTCalc::DoFFT(const TPofBCalc &PofB) {

  vector<double> pB(PofB.DataPB());

/* USED FOR DEBUGGING -----------------------

  time_t seconds;
  seconds = time(NULL);

  vector<double> B(PofB.DataB());
  double Bmin(PofB.GetBmin());

  char debugfile[50];
  int n = sprintf (debugfile, "test_PB_%ld_%f.dat", seconds, Bmin);

  if (n > 0) {
    ofstream of(debugfile);

    for (unsigned int i(0); i<B.size(); i++) {
      of << B[i] << " " << pB[i] << endl;
    }
    of.close();
  }
--------------------------------------------*/

  for (unsigned int i(0); i<fNFFT; i++) {
    fFFTin[i] = pB[i];
  }
  for (unsigned int i(0); i<fNFFT/2+1; i++) {
    fFFTout[i][0] = 0.0;
    fFFTout[i][1] = 0.0;
  }

//  cout << "perform the Fourier transform..." << endl;

  fftw_execute(fFFTplan);

}

//---------------------
// Method for calculating the muon spin polarization P(t) from the Fourier transformed p(B)
// Parameters: phase, dt, dB
//---------------------

void TPofTCalc::CalcPol(const vector<double> &par) {

  double sinph(sin(par[0]*PI/180.0)), cosph(cos(par[0]*PI/180.0));

  double polTemp(0.0);
  fT.clear();
  fPT.clear();

  for (unsigned int i(0); i<fNFFT/2+1; i++){
    fT.push_back(double(i)*fTBin);
    polTemp = cosph*fFFTout[i][0]*par[2] + sinph*fFFTout[i][1]*par[2];
    fPT.push_back(polTemp);
  }
}

//---------------------
// Method for evaluating P(t) at a given t
//---------------------

double TPofTCalc::Eval(double t) const {
  for (unsigned int i(0); i<fT.size()-1; i++) {
    if (t < fT[i+1]) {
      return fPT[i]+(fPT[i+1]-fPT[i])/(fT[i+1]-fT[i])*(t-fT[i]);
    }
  }

  cout << "No data for the time " << t << " us available! Returning -999.0 ..." << endl;
  return -999.0;
}

//---------------------
// Destructor of the TPofTCalc class - it saves the FFT plan and cleans up
//---------------------

TPofTCalc::~TPofTCalc() {
  // export wisdom so it has not to be checked for the FFT-plan next time

  FILE *wordsOfWisdomW;
  wordsOfWisdomW = fopen(fWisdom.c_str(), "w");
  if (wordsOfWisdomW == NULL) {
    cout << "couldn't open file ... No wisdom is exported..." << endl;
  }

  fftw_export_wisdom_to_file(wordsOfWisdomW);

  fclose(wordsOfWisdomW);

  fftw_destroy_plan(fFFTplan);
  free(fFFTin);
  fftw_free(fFFTout);
//  fftw_cleanup();
  fT.clear();
  fPT.clear();

}

