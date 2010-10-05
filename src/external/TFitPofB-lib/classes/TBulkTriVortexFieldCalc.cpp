/***************************************************************************

  TBulkTriVortexFieldCalc.cpp

  Author: Bastian M. Wojek, Alexander Maisuradze
  e-mail: bastian.wojek@psi.ch

  2009/10/17

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Bastian M. Wojek,  Alexander Maisuradze         *
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

#include "TBulkTriVortexFieldCalc.h"

#include <cstdlib>
#include <cmath>
#include <omp.h>
#include <iostream>
using namespace std;

#include "TMath.h"

#define PI 3.14159265358979323846
#define TWOPI 6.28318530717958647692

const double fluxQuantum(2.067833667e7); // in this case this is Gauss times square nm

const double sqrt3(sqrt(3.0));
const double pi_4sqrt3(0.25*PI/sqrt(3.0));


double getXi(const double &hc2) { // get xi given Hc2 in Gauss
  if (hc2)
    return sqrt(fluxQuantum/(TWOPI*hc2));
  else
    return 0.0;
}

double getHc2(const double &xi) { // get Hc2 given xi in nm
  if (xi)
    return fluxQuantum/(TWOPI*xi*xi);
  else
    return 0.0;
}

TBulkVortexFieldCalc::~TBulkVortexFieldCalc() {
  // if a wisdom file is used export the wisdom so it has not to be checked for the FFT-plan next time
  if (fUseWisdom) {
    FILE *wordsOfWisdomW;
    wordsOfWisdomW = fopen(fWisdom.c_str(), "w");
    if (wordsOfWisdomW == NULL) {
      cout << "TBulkVortexFieldCalc::~TBulkVortexFieldCalc(): Could not open file ... No wisdom is exported..." << endl;
    } else {
      fftw_export_wisdom_to_file(wordsOfWisdomW);
      fclose(wordsOfWisdomW);
    }
  }

  // clean up

  fftw_destroy_plan(fFFTplan);
  delete[] fFFTin; fFFTin = 0;
  delete[] fFFTout; fFFTout = 0;
  //fftw_cleanup();
  //fftw_cleanup_threads();
}

double TBulkVortexFieldCalc::GetBmin() const {
  if (fGridExists) {
    double min(fFFTout[0]);
    unsigned int minindex(0), counter(0);
    for (unsigned int j(0); j < fSteps * fSteps / 2; j++) {
      if (fFFTout[j] <= 0.0) {
        return 0.0;
      }
      if (fFFTout[j] < min) {
        min = fFFTout[j];
        minindex = j;
      }
      counter++;
      if (counter == fSteps/2) { // check only the first quadrant of B(x,y)
        counter = 0;
        j += fSteps/2;
      }
    }
    return fFFTout[minindex];
  } else {
    CalculateGrid();
    return GetBmin();
  }
}

double TBulkVortexFieldCalc::GetBmax() const {
  if (fGridExists)
    return fFFTout[0];
  else {
    CalculateGrid();
    return GetBmax();
  }
}

TBulkTriVortexLondonFieldCalc::TBulkTriVortexLondonFieldCalc(const string& wisdom, const unsigned int steps) {
  fWisdom = wisdom;
  if (steps % 2) {
    fSteps = steps + 1;
  } else {
    fSteps = steps;
  }
  fParam.resize(3);
  fGridExists = false;

#ifdef HAVE_LIBFFTW3_THREADS
  int init_threads(fftw_init_threads());
  if (init_threads) {
#ifdef HAVE_GOMP
    fftw_plan_with_nthreads(omp_get_num_procs());
#else
    fftw_plan_with_nthreads(2);
#endif /* HAVE_GOMP */
  }
#endif /* HAVE_LIBFFTW3_THREADS */

  fFFTin = new fftw_complex[(fSteps/2 + 1) * fSteps];
  fFFTout = new double[fSteps*fSteps];

//  cout << "Check for the FFT plan..." << endl;

// Load wisdom from file if it exists and should be used

  fUseWisdom = true;
  int wisdomLoaded(0);

  FILE *wordsOfWisdomR;
  wordsOfWisdomR = fopen(fWisdom.c_str(), "r");
  if (wordsOfWisdomR == NULL) {
    fUseWisdom = false;
  } else {
    wisdomLoaded = fftw_import_wisdom_from_file(wordsOfWisdomR);
    fclose(wordsOfWisdomR);
  }

  if (!wisdomLoaded) {
    fUseWisdom = false;
  }

// create the FFT plan

  if (fUseWisdom)
    fFFTplan = fftw_plan_dft_c2r_2d(fSteps, fSteps, fFFTin, fFFTout, FFTW_EXHAUSTIVE);
  else
    fFFTplan = fftw_plan_dft_c2r_2d(fSteps, fSteps, fFFTin, fFFTout, FFTW_ESTIMATE);
}

void TBulkTriVortexLondonFieldCalc::CalculateGrid() const {
  // SetParameters - method has to be called from the user before the calculation!!
  if (fParam.size() < 3) {
    cout << endl << "The SetParameters-method has to be called before B(x,y) can be calculated!" << endl;
    return;
  }
  if (!fParam[0] || !fParam[1] || !fParam[2]) {
    cout << endl << "The field, penetration depth and coherence length have to have finite values in order to calculate B(x,y)!" << endl;
    return;
  }

  double field(fabs(fParam[0])), lambda(fabs(fParam[1])), xi(fabs(fParam[2]));
  double Hc2(getHc2(xi));

  double latConstTr(sqrt(2.0*fluxQuantum/(field*sqrt3)));
  double xisq_2_scaled(2.0/3.0*pow(xi*PI/latConstTr,2.0)), lambdasq_scaled(4.0/3.0*pow(lambda*PI/latConstTr,2.0));

  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFTsq(fSteps*fSteps);

   // fill the field Fourier components in the matrix

  // ... but first check that the field is not larger than Hc2 and that we are dealing with a type II SC
  if ((field >= Hc2) || (lambda < xi/sqrt(2.0))) {
    int m;
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(m) schedule(dynamic)
    #endif
    for (m = 0; m < NFFTsq; m++) {
      fFFTout[m] = field;
    }
    // Set the flag which shows that the calculation has been done
    fGridExists = true;
    return;
  }

  // ... now fill in the Fourier components if everything was okay above
  double Gsq, ll;
  int k, l, lNFFT_2;

  for (l = 0; l < NFFT_2; l += 2) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>(k*k) + ll;
      fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    k = NFFT_2;
    Gsq = static_cast<double>(k*k) + ll;
    fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
    fFFTin[lNFFT_2 + k][1] = 0.0;
  }


  for (l = NFFT_2; l < NFFT; l += 2) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = 3.0*static_cast<double>((NFFT-l)*(NFFT-l));
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>(k*k) + ll;
      fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    k = NFFT_2;
    Gsq = static_cast<double>(k*k) + ll;
    fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
    fFFTin[lNFFT_2 + k][1] = 0.0;
  }

  // intermediate rows

  for (l = 1; l < NFFT_2; l += 2) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>((k + 1)*(k + 1)) + ll;
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    k = NFFT_2;
    fFFTin[lNFFT_2 + k][0] = 0.0;
    fFFTin[lNFFT_2 + k][1] = 0.0;
  }

  for (l = NFFT_2 + 1; l < NFFT; l += 2) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = 3.0*static_cast<double>((NFFT-l)*(NFFT-l));
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>((k+1)*(k+1)) + ll;
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    k = NFFT_2;
    fFFTin[lNFFT_2 + k][0] = 0.0;
    fFFTin[lNFFT_2 + k][1] = 0.0;
  }

  // Do the Fourier transform to get B(x,y)

  fftw_execute(fFFTplan);

  // Multiply by the applied field
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsq; l++) {
    fFFTout[l] *= field;
  }

  // Set the flag which shows that the calculation has been done

  fGridExists = true;
  return;

}

TBulkSqVortexLondonFieldCalc::TBulkSqVortexLondonFieldCalc(const string& wisdom, const unsigned int steps) {
  fWisdom = wisdom;
  if (steps % 2) {
    fSteps = steps + 1;
  } else {
    fSteps = steps;
  }
  fParam.resize(3);
  fGridExists = false;

#ifdef HAVE_LIBFFTW3_THREADS
  int init_threads(fftw_init_threads());
  if (init_threads) {
#ifdef HAVE_GOMP
    fftw_plan_with_nthreads(omp_get_num_procs());
#else
    fftw_plan_with_nthreads(2);
#endif /* HAVE_GOMP */
  }
#endif /* HAVE_LIBFFTW3_THREADS */

  fFFTin = new fftw_complex[(fSteps/2 + 1) * fSteps];
  fFFTout = new double[fSteps*fSteps];

//  cout << "Check for the FFT plan..." << endl;

// Load wisdom from file if it exists and should be used

  fUseWisdom = true;
  int wisdomLoaded(0);

  FILE *wordsOfWisdomR;
  wordsOfWisdomR = fopen(fWisdom.c_str(), "r");
  if (wordsOfWisdomR == NULL) {
    fUseWisdom = false;
  } else {
    wisdomLoaded = fftw_import_wisdom_from_file(wordsOfWisdomR);
    fclose(wordsOfWisdomR);
  }

  if (!wisdomLoaded) {
    fUseWisdom = false;
  }

// create the FFT plan

  if (fUseWisdom)
    fFFTplan = fftw_plan_dft_c2r_2d(fSteps, fSteps, fFFTin, fFFTout, FFTW_EXHAUSTIVE);
  else
    fFFTplan = fftw_plan_dft_c2r_2d(fSteps, fSteps, fFFTin, fFFTout, FFTW_ESTIMATE);
}

void TBulkSqVortexLondonFieldCalc::CalculateGrid() const {
  // SetParameters - method has to be called from the user before the calculation!!
  if (fParam.size() < 3) {
    cout << endl << "The SetParameters-method has to be called before B(x,y) can be calculated!" << endl;
    return;
  }
  if (!fParam[0] || !fParam[1] || !fParam[2]) {
    cout << endl << "The field, penetration depth and coherence length have to have finite values in order to calculate B(x,y)!" << endl;
    return;
  }

  double field(fabs(fParam[0])), lambda(fabs(fParam[1])), xi(fabs(fParam[2]));
  double Hc2(getHc2(xi));

  double latConstSq(sqrt(fluxQuantum/field));
  double xisq_2_scaled(2.0*pow(xi*PI/latConstSq,2.0)), lambdasq_scaled(4.0*pow(lambda*PI/latConstSq,2.0));

  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFTsq(fSteps*fSteps);

   // fill the field Fourier components in the matrix

  // ... but first check that the field is not larger than Hc2 and that we are dealing with a type II SC
  if ((field >= Hc2) || (lambda < xi/sqrt(2.0))) {
    int m;
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(m) schedule(dynamic)
    #endif
    for (m = 0; m < NFFTsq; m++) {
      fFFTout[m] = field;
    }
    // Set the flag which shows that the calculation has been done
    fGridExists = true;
    return;
  }

  // ... now fill in the Fourier components if everything was okay above
  double Gsq, ll;
  int k, l, lNFFT_2;

  for (l = 0; l < NFFT_2; ++l) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = static_cast<double>(l*l);
    for (k = 0; k <= NFFT_2; ++k) {
      Gsq = static_cast<double>(k*k) + ll;
      fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k][1] = 0.0;
    }
  }

  for (l = NFFT_2; l < NFFT; ++l) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = static_cast<double>((NFFT-l)*(NFFT-l));
    for (k = 0; k <= NFFT_2; ++k) {
      Gsq = static_cast<double>(k*k) + ll;
      fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k][1] = 0.0;
    }
  }

  // Do the Fourier transform to get B(x,y)

  fftw_execute(fFFTplan);

  // Multiply by the applied field
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsq; l++) {
    fFFTout[l] *= field;
  }

  // Set the flag which shows that the calculation has been done

  fGridExists = true;
  return;

}



TBulkTriVortexMLFieldCalc::TBulkTriVortexMLFieldCalc(const string& wisdom, const unsigned int steps) {
  fWisdom = wisdom;
  if (steps % 2) {
    fSteps = steps + 1;
  } else {
    fSteps = steps;
  }
  fParam.resize(3);
  fGridExists = false;

#ifdef HAVE_LIBFFTW3_THREADS
  int init_threads(fftw_init_threads());
  if (init_threads) {
#ifdef HAVE_GOMP
    fftw_plan_with_nthreads(omp_get_num_procs());
#else
    fftw_plan_with_nthreads(2);
#endif /* HAVE_GOMP */
  }
#endif /* HAVE_LIBFFTW3_THREADS */

  fFFTin = new fftw_complex[(fSteps/2 + 1) * fSteps];
  fFFTout = new double[fSteps*fSteps];

//  cout << "Check for the FFT plan..." << endl;

// Load wisdom from file if it exists and should be used

  fUseWisdom = true;
  int wisdomLoaded(0);

  FILE *wordsOfWisdomR;
  wordsOfWisdomR = fopen(fWisdom.c_str(), "r");
  if (wordsOfWisdomR == NULL) {
    fUseWisdom = false;
  } else {
    wisdomLoaded = fftw_import_wisdom_from_file(wordsOfWisdomR);
    fclose(wordsOfWisdomR);
  }

  if (!wisdomLoaded) {
    fUseWisdom = false;
  }

// create the FFT plan

  if (fUseWisdom)
    fFFTplan = fftw_plan_dft_c2r_2d(fSteps, fSteps, fFFTin, fFFTout, FFTW_EXHAUSTIVE);
  else
    fFFTplan = fftw_plan_dft_c2r_2d(fSteps, fSteps, fFFTin, fFFTout, FFTW_ESTIMATE);
}

void TBulkTriVortexMLFieldCalc::CalculateGrid() const {
  // SetParameters - method has to be called from the user before the calculation!!
  if (fParam.size() < 3) {
    cout << endl << "The SetParameters-method has to be called before B(x,y) can be calculated!" << endl;
    return;
  }
  if (!fParam[0] || !fParam[1] || !fParam[2]) {
    cout << endl << "The field, penetration depth and coherence length have to have finite values in order to calculate B(x,y)!" << endl;
    return;
  }

  double field(fabs(fParam[0])), lambda(fabs(fParam[1])), xi(fabs(fParam[2]));
  double Hc2(getHc2(xi));

  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFTsq(fSteps*fSteps);

   // fill the field Fourier components in the matrix

  // ... but first check that the field is not larger than Hc2 and that we are dealing with a type II SC
  if ((field >= Hc2) || (lambda < xi/sqrt(2.0))) {
    int m;
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(m) schedule(dynamic)
    #endif
    for (m = 0; m < NFFTsq; m++) {
      fFFTout[m] = field;
    }
    // Set the flag which shows that the calculation has been done
    fGridExists = true;
    return;
  }

  double latConstTr(sqrt(2.0*fluxQuantum/(field*sqrt3)));
  double oneMb(1.0-field/Hc2);
  double xisq_2_scaled(2.0/(3.0*oneMb)*pow(xi*PI/latConstTr,2.0)), lambdasq_scaled(4.0/(3.0*oneMb)*pow(lambda*PI/latConstTr,2.0));

  // ... now fill in the Fourier components if everything was okay above
  double Gsq, ll;
  int k, l, lNFFT_2;

  for (l = 0; l < NFFT_2; l += 2) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>(k*k) + ll;
      fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    k = NFFT_2;
    Gsq = static_cast<double>(k*k) + ll;
    fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
    fFFTin[lNFFT_2 + k][1] = 0.0;
  }


  for (l = NFFT_2; l < NFFT; l += 2) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = 3.0*static_cast<double>((NFFT-l)*(NFFT-l));
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>(k*k) + ll;
      fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    k = NFFT_2;
    Gsq = static_cast<double>(k*k) + ll;
    fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
    fFFTin[lNFFT_2 + k][1] = 0.0;
  }

  // intermediate rows

  for (l = 1; l < NFFT_2; l += 2) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>((k + 1)*(k + 1)) + ll;
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    k = NFFT_2;
    fFFTin[lNFFT_2 + k][0] = 0.0;
    fFFTin[lNFFT_2 + k][1] = 0.0;
  }

  for (l = NFFT_2 + 1; l < NFFT; l += 2) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = 3.0*static_cast<double>((NFFT-l)*(NFFT-l));
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>((k+1)*(k+1)) + ll;
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    k = NFFT_2;
    fFFTin[lNFFT_2 + k][0] = 0.0;
    fFFTin[lNFFT_2 + k][1] = 0.0;
  }

  // Do the Fourier transform to get B(x,y)

  fftw_execute(fFFTplan);

  // Multiply by the applied field
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsq; l++) {
    fFFTout[l] *= field;
  }

  // Set the flag which shows that the calculation has been done

  fGridExists = true;
  return;

}




TBulkTriVortexAGLFieldCalc::TBulkTriVortexAGLFieldCalc(const string& wisdom, const unsigned int steps) {
  fWisdom = wisdom;
  if (steps % 2) {
    fSteps = steps + 1;
  } else {
    fSteps = steps;
  }
  fParam.resize(3);
  fGridExists = false;

#ifdef HAVE_LIBFFTW3_THREADS
  int init_threads(fftw_init_threads());
  if (init_threads) {
#ifdef HAVE_GOMP
    fftw_plan_with_nthreads(omp_get_num_procs());
#else
    fftw_plan_with_nthreads(2);
#endif /* HAVE_GOMP */
  }
#endif /* HAVE_LIBFFTW3_THREADS */

  fFFTin = new fftw_complex[(fSteps/2 + 1) * fSteps];
  fFFTout = new double[fSteps*fSteps];

//  cout << "Check for the FFT plan..." << endl;

// Load wisdom from file if it exists and should be used

  fUseWisdom = true;
  int wisdomLoaded(0);

  FILE *wordsOfWisdomR;
  wordsOfWisdomR = fopen(fWisdom.c_str(), "r");
  if (wordsOfWisdomR == NULL) {
    fUseWisdom = false;
  } else {
    wisdomLoaded = fftw_import_wisdom_from_file(wordsOfWisdomR);
    fclose(wordsOfWisdomR);
  }

  if (!wisdomLoaded) {
    fUseWisdom = false;
  }

// create the FFT plan

  if (fUseWisdom)
    fFFTplan = fftw_plan_dft_c2r_2d(fSteps, fSteps, fFFTin, fFFTout, FFTW_EXHAUSTIVE);
  else
    fFFTplan = fftw_plan_dft_c2r_2d(fSteps, fSteps, fFFTin, fFFTout, FFTW_ESTIMATE);
}

void TBulkTriVortexAGLFieldCalc::CalculateGrid() const {
  // SetParameters - method has to be called from the user before the calculation!!
  if (fParam.size() < 3) {
    cout << endl << "The SetParameters-method has to be called before B(x,y) can be calculated!" << endl;
    return;
  }
  if (!fParam[0] || !fParam[1] || !fParam[2]) {
    cout << endl << "The field, penetration depth and coherence length have to have finite values in order to calculate B(x,y)!" << endl;
    return;
  }

  double field(fabs(fParam[0])), lambda(fabs(fParam[1])), xi(fabs(fParam[2]));
  double Hc2(getHc2(xi));

  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFTsq(fSteps*fSteps);

   // fill the field Fourier components in the matrix

  // ... but first check that the field is not larger than Hc2 and that we are dealing with a type II SC
  if ((field >= Hc2) || (lambda < xi/sqrt(2.0))) {
    int m;
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(m) schedule(dynamic)
    #endif
    for (m = 0; m < NFFTsq; m++) {
      fFFTout[m] = field;
    }
    // Set the flag which shows that the calculation has been done
    fGridExists = true;
    return;
  }

  double latConstTr(sqrt(2.0*fluxQuantum/(field*sqrt3)));
  double b(field/Hc2), fInf(1.0-pow(b,4.0)), xiV(xi*(sqrt(2.0)-0.75*xi/lambda)*sqrt((1.0+pow(b,4.0))*(1.0-2.0*b*pow(1.0-b,2.0))));
  double lambdasq_scaled(4.0/3.0*pow(lambda*PI/latConstTr,2.0));

  // ... now fill in the Fourier components if everything was okay above
  double Gsq, sqrtfInfSqPlusLsqGsq, ll;
  int k, l, lNFFT_2;

  for (l = 0; l < NFFT_2; l += 2) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>(k*k) + ll;
      sqrtfInfSqPlusLsqGsq = sqrt(fInf*fInf + lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k][0] = ((!k && !l) ? 1.0 : \
       fInf*TMath::BesselK1(xiV/lambda*sqrtfInfSqPlusLsqGsq)/(sqrtfInfSqPlusLsqGsq*TMath::BesselK1(xiV/lambda*fInf)));
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    k = NFFT_2;
    Gsq = static_cast<double>(k*k) + ll;
    sqrtfInfSqPlusLsqGsq = sqrt(fInf*fInf + lambdasq_scaled*Gsq);
    fFFTin[lNFFT_2 + k][0] = fInf*TMath::BesselK1(xiV/lambda*sqrtfInfSqPlusLsqGsq)/(sqrtfInfSqPlusLsqGsq*TMath::BesselK1(xiV/lambda*fInf));
    fFFTin[lNFFT_2 + k][1] = 0.0;
  }


  for (l = NFFT_2; l < NFFT; l += 2) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = 3.0*static_cast<double>((NFFT-l)*(NFFT-l));
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>(k*k) + ll;
      sqrtfInfSqPlusLsqGsq = sqrt(fInf*fInf + lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k][0] = fInf*TMath::BesselK1(xiV/lambda*sqrtfInfSqPlusLsqGsq)/(sqrtfInfSqPlusLsqGsq*TMath::BesselK1(xiV/lambda*fInf));
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    k = NFFT_2;
    Gsq = static_cast<double>(k*k) + ll;
    sqrtfInfSqPlusLsqGsq = sqrt(fInf*fInf + lambdasq_scaled*Gsq);
    fFFTin[lNFFT_2 + k][0] = fInf*TMath::BesselK1(xiV/lambda*sqrtfInfSqPlusLsqGsq)/(sqrtfInfSqPlusLsqGsq*TMath::BesselK1(xiV/lambda*fInf));
    fFFTin[lNFFT_2 + k][1] = 0.0;
  }

  // intermediate rows

  for (l = 1; l < NFFT_2; l += 2) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>((k + 1)*(k + 1)) + ll;
      sqrtfInfSqPlusLsqGsq = sqrt(fInf*fInf + lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = fInf*TMath::BesselK1(xiV/lambda*sqrtfInfSqPlusLsqGsq)/(sqrtfInfSqPlusLsqGsq*TMath::BesselK1(xiV/lambda*fInf));
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    k = NFFT_2;
    fFFTin[lNFFT_2 + k][0] = 0.0;
    fFFTin[lNFFT_2 + k][1] = 0.0;
  }

  for (l = NFFT_2 + 1; l < NFFT; l += 2) {
    lNFFT_2 = l*(NFFT_2 + 1);
    ll = 3.0*static_cast<double>((NFFT-l)*(NFFT-l));
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>((k+1)*(k+1)) + ll;
      sqrtfInfSqPlusLsqGsq = sqrt(fInf*fInf + lambdasq_scaled*Gsq);
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = fInf*TMath::BesselK1(xiV/lambda*sqrtfInfSqPlusLsqGsq)/(sqrtfInfSqPlusLsqGsq*TMath::BesselK1(xiV/lambda*fInf));
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    k = NFFT_2;
    fFFTin[lNFFT_2 + k][0] = 0.0;
    fFFTin[lNFFT_2 + k][1] = 0.0;
  }

  // Do the Fourier transform to get B(x,y)

  fftw_execute(fFFTplan);

  // Multiply by the applied field
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsq; l++) {
    fFFTout[l] *= field;
  }

  // Set the flag which shows that the calculation has been done

  fGridExists = true;
  return;

}



TBulkTriVortexNGLFieldCalc::TBulkTriVortexNGLFieldCalc(const string& wisdom, const unsigned int steps)
 : fLatticeConstant(0.0), fKappa(0.0), fSumAk(0.0), fSumOmegaSq(0.0), fSumSum(0.0)
{
  fWisdom = wisdom;
  switch (steps % 4) {
    case 0:
      fSteps = steps;
      break;
    case 1:
      fSteps = steps + 3;
      break;
    case 2:
      fSteps = steps + 2;
      break;
    case 3:
      fSteps = steps + 1;
      break;
    default:
      break;
  }

  fParam.resize(3);
  fGridExists = false;

#ifdef HAVE_LIBFFTW3_THREADS
  int init_threads(fftw_init_threads());
  if (init_threads) {
#ifdef HAVE_GOMP
    fftw_plan_with_nthreads(omp_get_num_procs());
#else
    fftw_plan_with_nthreads(2);
#endif /* HAVE_GOMP */
  }
#endif /* HAVE_LIBFFTW3_THREADS */

  const unsigned int stepsSq(fSteps*fSteps);

  fFFTout = new double[stepsSq];  // field B(x,y)
  fOmegaMatrix = new double[stepsSq];  // |psi|^2 (x,y)
  fOmegaDiffMatrix = new fftw_complex[stepsSq]; // grad omega

  fFFTin = new fftw_complex[stepsSq]; // aK matrix
  fBkMatrix = new fftw_complex[stepsSq]; // bK matrix
  fRealSpaceMatrix = new fftw_complex[stepsSq];

  fQMatrix = new fftw_complex[stepsSq];
  fQMatrixA = new fftw_complex[stepsSq];

  fCheckAkConvergence = new double[fSteps];
  fCheckBkConvergence = new double[fSteps];

// Load wisdom from file if it exists and should be used

  fUseWisdom = true;
  int wisdomLoaded(0);

  FILE *wordsOfWisdomR;
  wordsOfWisdomR = fopen(fWisdom.c_str(), "r");
  if (wordsOfWisdomR == NULL) {
    fUseWisdom = false;
  } else {
    wisdomLoaded = fftw_import_wisdom_from_file(wordsOfWisdomR);
    fclose(wordsOfWisdomR);
  }

  if (!wisdomLoaded) {
    fUseWisdom = false;
  }

// create the FFT plans

  if (fUseWisdom) {
    // use the first plan from the base class here - it will be destroyed by the base class destructor
    fFFTplan = fftw_plan_dft_2d(fSteps, fSteps, fFFTin, fRealSpaceMatrix, FFTW_BACKWARD, FFTW_EXHAUSTIVE);
    fFFTplanBkToBandQ = fftw_plan_dft_2d(fSteps, fSteps, fBkMatrix, fBkMatrix, FFTW_BACKWARD, FFTW_EXHAUSTIVE);
    fFFTplanOmegaToAk = fftw_plan_dft_2d(fSteps, fSteps, fRealSpaceMatrix, fFFTin, FFTW_FORWARD, FFTW_EXHAUSTIVE);
    fFFTplanOmegaToBk = fftw_plan_dft_2d(fSteps, fSteps, fBkMatrix, fBkMatrix, FFTW_FORWARD, FFTW_EXHAUSTIVE);
  }
  else {
    // use the first plan from the base class here - it will be destroyed by the base class destructor
    fFFTplan = fftw_plan_dft_2d(fSteps, fSteps, fFFTin, fRealSpaceMatrix, FFTW_BACKWARD, FFTW_ESTIMATE);
    fFFTplanBkToBandQ = fftw_plan_dft_2d(fSteps, fSteps, fBkMatrix, fBkMatrix, FFTW_BACKWARD, FFTW_ESTIMATE);
    fFFTplanOmegaToAk = fftw_plan_dft_2d(fSteps, fSteps, fRealSpaceMatrix, fFFTin, FFTW_FORWARD, FFTW_ESTIMATE);
    fFFTplanOmegaToBk = fftw_plan_dft_2d(fSteps, fSteps, fBkMatrix, fBkMatrix, FFTW_FORWARD, FFTW_ESTIMATE);
  }
}

TBulkTriVortexNGLFieldCalc::~TBulkTriVortexNGLFieldCalc() {

  // clean up

  fftw_destroy_plan(fFFTplanBkToBandQ);
  fftw_destroy_plan(fFFTplanOmegaToAk);
  fftw_destroy_plan(fFFTplanOmegaToBk);

  delete[] fOmegaMatrix; fOmegaMatrix = 0;
  delete[] fOmegaDiffMatrix; fOmegaDiffMatrix = 0;

  delete[] fBkMatrix; fBkMatrix = 0;
  delete[] fRealSpaceMatrix; fRealSpaceMatrix = 0;
  delete[] fQMatrix; fQMatrix = 0;
  delete[] fQMatrixA; fQMatrixA = 0;

  delete[] fCheckAkConvergence; fCheckAkConvergence = 0;
  delete[] fCheckBkConvergence; fCheckBkConvergence = 0;
}

void TBulkTriVortexNGLFieldCalc::CalculateGradient() const {

  // Calculate the gradient of omega stored in a fftw_complex array (dw/dx, dw/dy)

  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFTsq(fSteps*fSteps);

  int i, j, l, index;

  // Take the derivative of the Fourier sum of omega

  // First save a copy of the real aK-matrix in the imaginary part of the bK-matrix
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsq; ++l) {
    fBkMatrix[l][1] = fFFTin[l][0];
  }

  // dw/dx = sum_K aK Kx sin(Kx*x + Ky*y)
  // First multiply the aK with Kx, then call FFTW

  const double coeffKx(TWOPI/(sqrt3*fLatticeConstant));

  // even rows
  for (i = 0; i < NFFT; i += 2) {
    // j = 0
    fFFTin[NFFT*i][0] = 0.0;
    // j != 0
    for (j = 2; j < NFFT_2; j += 2) {
      fFFTin[(j + NFFT*i)][0] *= coeffKx*static_cast<double>(j);
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[(j + NFFT*i)][0] *= coeffKx*static_cast<double>(j - NFFT);
    }
  }

  // odd rows
  for (i = 1; i < NFFT; i += 2) {
    for (j = 0; j < NFFT_2; j += 2) {
      fFFTin[(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<double>(j + 1);
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<double>(j + 1 - NFFT);
    }
  }

  fftw_execute(fFFTplan);

  // Copy the results to the gradient matrix and restore the original aK-matrix
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsq; ++l) {
    fOmegaDiffMatrix[l][0] = fRealSpaceMatrix[l][1];
    fFFTin[l][0] = fBkMatrix[l][1];
  }

  // dw/dy = sum_K aK Ky sin(Kx*x + Ky*y)
  // First multiply the aK with Ky, then call FFTW

  const double coeffKy(TWOPI/fLatticeConstant);
  double ky;

  // even rows
  // i = 0
  for (j = 0; j < NFFT; j += 2) {
    fFFTin[j][0] = 0.0;
  }
  // i != 0
  for (i = 2; i < NFFT_2; i += 2) {
    ky = coeffKy*static_cast<double>(i);
    for (j = 0; j < NFFT; j += 2) {
      fFFTin[(j + NFFT*i)][0] *= ky;
    }
  }
  for (i = NFFT_2; i < NFFT; i += 2) {
    ky = coeffKy*static_cast<double>(i - NFFT);
    for (j = 0; j < NFFT; j += 2) {
      fFFTin[(j + NFFT*i)][0] *= ky;
    }
  }

  // odd rows
  for (i = 1; i < NFFT_2; i += 2) {
    ky = coeffKy*static_cast<double>(i);
    for (j = 0; j < NFFT; j += 2) {
      fFFTin[(j + 1 + NFFT*i)][0] *= ky;
    }
  }
  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    ky = coeffKy*static_cast<double>(i - NFFT);
    for (j = 0; j < NFFT; j += 2) {
      fFFTin[(j + 1 + NFFT*i)][0] *= ky;
    }
  }

  fftw_execute(fFFTplan);

  // Copy the results to the gradient matrix and restore the original aK-matrix
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsq; ++l) {
    fOmegaDiffMatrix[l][1] = fRealSpaceMatrix[l][1];
    fFFTin[l][0] = fBkMatrix[l][1];
    fBkMatrix[l][1] = 0.0;
  }

  // Ensure that omega at the vortex-core positions is zero
  fOmegaMatrix[0] = 0.0;
  fOmegaMatrix[(NFFT+1)*NFFT_2] = 0.0;

  // Ensure that the derivatives at the vortex-core positions are zero
  fOmegaDiffMatrix[0][0] = 0.0;
  fOmegaDiffMatrix[(NFFT+1)*NFFT_2][0] = 0.0;
  fOmegaDiffMatrix[0][1] = 0.0;
  fOmegaDiffMatrix[(NFFT+1)*NFFT_2][1] = 0.0;

/*
  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFTsq(fSteps*fSteps);

  const double denomY(2.0*fLatticeConstant/static_cast<double>(NFFT));
  const double denomX(denomY*sqrt3);

  // Ensure that omega at the vortex-core positions is zero
  fOmegaMatrix[0] = 0.0;
  fOmegaMatrix[(NFFT+1)*NFFT_2] = 0.0;

  int i;

// //  Ensure that omega is NOT negative
// // #pragma omp parallel for default(shared) private(i) schedule(dynamic)
//   for (i = 0; i < NFFTsq; i += 1) {
//     if (fOmegaMatrix[i] < 0.0) {
//       cout << "Omega negative for index " << i << ", value: " << fOmegaMatrix[i] << endl;
//       fOmegaMatrix[i] = 0.0;
//     }
//   }

  #pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = 0; i < NFFTsq; i += 1) {
    if (!(i % NFFT)) { // first column
      fOmegaDiffMatrix[i][0] = (fOmegaMatrix[i+1]-fOmegaMatrix[i+NFFT-1])/denomX;
    } else if (!((i + 1) % NFFT)) { // last column
      fOmegaDiffMatrix[i][0] = (fOmegaMatrix[i-NFFT+1]-fOmegaMatrix[i-1])/denomX;
    } else {
      fOmegaDiffMatrix[i][0] = (fOmegaMatrix[i+1]-fOmegaMatrix[i-1])/denomX;
    }
  }

  for (i = 0; i < NFFT; i++) { // first row
    fOmegaDiffMatrix[i][1] = (fOmegaMatrix[i+NFFT]-fOmegaMatrix[NFFTsq-NFFT+i])/denomY;
  }

  for (i = NFFTsq - NFFT; i < NFFTsq; i++) { // last row
    fOmegaDiffMatrix[i][1] = (fOmegaMatrix[i-NFFTsq+NFFT]-fOmegaMatrix[i-NFFT])/denomY;
  }

  #pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = NFFT; i < NFFTsq - NFFT; i++) { // the rest
    fOmegaDiffMatrix[i][1] = (fOmegaMatrix[i+NFFT]-fOmegaMatrix[i-NFFT])/denomY;
  }

  // Ensure that the derivatives at the vortex-core positions are zero
  fOmegaDiffMatrix[0][0] = 0.0;
  fOmegaDiffMatrix[(NFFT+1)*NFFT_2][0] = 0.0;
  fOmegaDiffMatrix[0][1] = 0.0;
  fOmegaDiffMatrix[(NFFT+1)*NFFT_2][1] = 0.0;
*/
  return;
}

void TBulkTriVortexNGLFieldCalc::FillAbrikosovCoefficients() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2);

  double Gsq, sign, ll;
  int k, l, lNFFT_2;

  for (l = 0; l < NFFT_2; l += 2) {
    if (!(l % 4)) {
      sign = 1.0;
    } else {
      sign = -1.0;
    }
    lNFFT_2 = l*(NFFT);
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2; k += 2) {
      sign = -sign;
      Gsq = static_cast<double>(k*k) + ll;
      fFFTin[lNFFT_2 + k][0] = sign*exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    for (k = NFFT_2; k < NFFT; k += 2) {
      sign = -sign;
      Gsq = static_cast<double>((k-NFFT)*(k-NFFT)) + ll;
      fFFTin[lNFFT_2 + k][0] = sign*exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
  }

  for (l = NFFT_2; l < NFFT; l += 2) {
    if (!(l % 4)) {
      sign = 1.0;
    } else {
      sign = -1.0;
    }
    lNFFT_2 = l*(NFFT);
    ll = 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
    for (k = 0; k < NFFT_2; k += 2) {
      sign = -sign;
      Gsq = static_cast<double>(k*k) + ll;
      fFFTin[lNFFT_2 + k][0] = sign*exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    for (k = NFFT_2; k < NFFT_2; k += 2) {
      sign = -sign;
      Gsq = static_cast<double>((k-NFFT)*(k-NFFT)) + ll;
      fFFTin[lNFFT_2 + k][0] = sign*exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
  }

  // intermediate rows
  for (l = 1; l < NFFT_2; l += 2) {
    lNFFT_2 = l*(NFFT);
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = static_cast<double>((k + 1)*(k + 1)) + ll;
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    for (k = NFFT_2; k < NFFT; k += 2) {
      Gsq = static_cast<double>((k + 1 - NFFT)*(k + 1 - NFFT)) + ll;
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
  }

  for (l = NFFT_2 + 1; l < NFFT; l += 2) {
    lNFFT_2 = l*(NFFT);
    ll = 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      Gsq = static_cast<double>((k+1)*(k+1)) + ll;
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    for (k = NFFT_2; k < NFFT; k += 2) {
      Gsq = static_cast<double>((k+1 - NFFT)*(k+1 - NFFT)) + ll;
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
  }

  fFFTin[0][0] = 0.0;

  return;
}

void TBulkTriVortexNGLFieldCalc::ManipulateFourierCoefficientsA() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTsq(fSteps*fSteps);

  // Divide Brandt's coefficient no2 by two since we are considering "the full reciprocal lattice", not only the half space!
  const double coeff1(4.0/3.0*pow(PI/fLatticeConstant,2.0));
  const double coeff3(2.0*fKappa*fKappa);
  const double coeff2(coeff3/static_cast<double>(NFFTsq));

  int lNFFT_2, l, k;
  double Gsq, ll;

  for (l = 0; l < NFFT_2; l += 2) {
    lNFFT_2 = l*(NFFT);
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = coeff1*(static_cast<double>(k*k) + ll);
      fFFTin[lNFFT_2 + k][0] *= coeff2/(Gsq+coeff3);
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    for (k = NFFT_2; k < NFFT; k += 2) {
      Gsq = coeff1*(static_cast<double>((k - NFFT)*(k - NFFT)) + ll);
      fFFTin[lNFFT_2 + k][0] *= coeff2/(Gsq+coeff3);
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
  }

  for (l = NFFT_2; l < NFFT; l += 2) {
    lNFFT_2 = l*(NFFT);
    ll = 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>(k*k) + ll);
      fFFTin[lNFFT_2 + k][0] *= coeff2/(Gsq+coeff3);
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    for (k = NFFT_2; k < NFFT; k += 2) {
      Gsq = coeff1*(static_cast<double>((k-NFFT)*(k-NFFT)) + ll);
      fFFTin[lNFFT_2 + k][0] *= coeff2/(Gsq+coeff3);
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] = 0.0;
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
  }

  //intermediate rows

  for (l = 1; l < NFFT_2; l += 2) {
    lNFFT_2 = l*(NFFT);
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = coeff1*(static_cast<double>((k+1)*(k+1)) + ll);
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] *= coeff2/(Gsq+coeff3);
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    for (k = NFFT_2; k < NFFT; k += 2) {
      Gsq = coeff1*(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + ll);
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] *= coeff2/(Gsq+coeff3);
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
  }

  for (l = NFFT_2 + 1; l < NFFT; l += 2) {
    lNFFT_2 = l*(NFFT);
    ll = 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
    for (k = 0; k < NFFT_2; k += 2) {
      Gsq = coeff1*(static_cast<double>((k+1)*(k+1)) + ll);
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] *= coeff2/(Gsq+coeff3);
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
    for (k = NFFT_2; k < NFFT; k += 2) {
      Gsq = coeff1*(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + ll);
      fFFTin[lNFFT_2 + k][0] = 0.0;
      fFFTin[lNFFT_2 + k][1] = 0.0;
      fFFTin[lNFFT_2 + k + 1][0] *= coeff2/(Gsq+coeff3);
      fFFTin[lNFFT_2 + k + 1][1] = 0.0;
    }
  }

  fFFTin[0][0] = 0.0;

  return;
}

void TBulkTriVortexNGLFieldCalc::ManipulateFourierCoefficientsB() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2);

  // Divide Brandt's coefficient no2 by two since we are considering "the full reciprocal lattice", not only the half space!
  const double coeff1(4.0/3.0*pow(PI/fLatticeConstant,2.0));
  const double coeff2(-1.0/static_cast<double>(NFFT*NFFT));
  const double coeff3(fSumAk);

  int lNFFT, l, k;
  double Gsq, ll;

  for (l = 0; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>(k*k) + ll);
      fBkMatrix[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
      fBkMatrix[lNFFT + k][1] = 0.0;
      fBkMatrix[lNFFT + k + 1][0] = 0.0;
      fBkMatrix[lNFFT + k + 1][1] = 0.0;
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>((k-NFFT)*(k-NFFT)) + ll);
      fBkMatrix[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
      fBkMatrix[lNFFT + k][1] = 0.0;
      fBkMatrix[lNFFT + k + 1][0] = 0.0;
      fBkMatrix[lNFFT + k + 1][1] = 0.0;
    }
  }

  for (l = NFFT_2; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    ll = 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>(k*k) + ll);
      fBkMatrix[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
      fBkMatrix[lNFFT + k][1] = 0.0;
      fBkMatrix[lNFFT + k + 1][0] = 0.0;
      fBkMatrix[lNFFT + k + 1][1] = 0.0;
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>((k-NFFT)*(k-NFFT)) + ll);
      fBkMatrix[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
      fBkMatrix[lNFFT + k][1] = 0.0;
      fBkMatrix[lNFFT + k + 1][0] = 0.0;
      fBkMatrix[lNFFT + k + 1][1] = 0.0;
    }
  }

  //intermediate rows

  for (l = 1; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>((k+1)*(k+1)) + ll);
      fBkMatrix[lNFFT + k][0] = 0.0;
      fBkMatrix[lNFFT + k][1] = 0.0;
      fBkMatrix[lNFFT + k + 1][0] *= coeff2/(Gsq+coeff3);
      fBkMatrix[lNFFT + k + 1][1] = 0.0;
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + ll);
      fBkMatrix[lNFFT + k][0] = 0.0;
      fBkMatrix[lNFFT + k][1] = 0.0;
      fBkMatrix[lNFFT + k + 1][0] *= coeff2/(Gsq+coeff3);
      fBkMatrix[lNFFT + k + 1][1] = 0.0;
    }
  }

  for (l = NFFT_2 + 1; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    ll = 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>((k+1)*(k+1)) + ll);
      fBkMatrix[lNFFT + k][0] = 0.0;
      fBkMatrix[lNFFT + k][1] = 0.0;
      fBkMatrix[lNFFT + k + 1][0] *= coeff2/(Gsq+coeff3);
      fBkMatrix[lNFFT + k + 1][1] = 0.0;
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + ll);
      fBkMatrix[lNFFT + k][0] = 0.0;
      fBkMatrix[lNFFT + k][1] = 0.0;
      fBkMatrix[lNFFT + k + 1][0] *= coeff2/(Gsq+coeff3);
      fBkMatrix[lNFFT + k + 1][1] = 0.0;
    }
  }

  fBkMatrix[0][0] = 0.0;

  return;
}


void TBulkTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForQx() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2);
  const double coeff1(1.5*fLatticeConstant/PI);

  int lNFFT, l, k;
  double ll;

  for (l = 0; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      if (!k && !l)
        fBkMatrix[0][0] = 0.0;
      else
        fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(l)/(static_cast<double>(k*k) + ll);
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(l)/(static_cast<double>((k-NFFT)*(k-NFFT)) + ll);
    }
  }

  for (l = NFFT_2; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    ll = 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>(k*k) + ll);
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>((k-NFFT)*(k-NFFT)) + ll);
    }
  }

  //intermediate rows

  for (l = 1; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(l)/(static_cast<double>((k+1)*(k+1)) + ll);
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(l)/(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + ll);
    }
  }

  for (l = NFFT_2 + 1; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    ll = 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>((k+1)*(k+1)) + ll);
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + ll);
    }
  }

  return;
}

void TBulkTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForQy() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2);
  const double coeff1(0.5*sqrt3*fLatticeConstant/PI);

  int lNFFT, l, k;
  double ll;

  for (l = 0; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      if (!k && !l)
        fBkMatrix[0][0] = 0.0;
      else
        fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(k)/(static_cast<double>(k*k) + ll);
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(k-NFFT)/(static_cast<double>((k-NFFT)*(k-NFFT)) + ll);
    }
  }

  for (l = NFFT_2; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    ll = 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(k)/(static_cast<double>(k*k) + ll);
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(k-NFFT)/(static_cast<double>((k-NFFT)*(k-NFFT)) + ll);
    }
  }

  //intermediate rows

  for (l = 1; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    ll = 3.0*static_cast<double>(l*l);
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(k+1)/(static_cast<double>((k+1)*(k+1)) + ll);
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(k+1-NFFT)/(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + ll);
    }
  }

  for (l = NFFT_2 + 1; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    ll = 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(k+1)/(static_cast<double>((k+1)*(k+1)) + ll);
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(k+1-NFFT)/(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + ll);
    }
  }

  return;
}

void TBulkTriVortexNGLFieldCalc::CalculateSumAk() const {
  const int NFFT_2(fSteps/2);
  const int NFFTsq_2((fSteps/2 + 1)*fSteps);
  const int NFFTsq(fSteps*fSteps);
/*
  double SumFirstColumn(0.0);

  fSumAk = 0.0;
  for (int l(1); l < NFFTsq_2; l++) {
    if (((l+1) % (NFFT_2 + 1)))
      fSumAk += fFFTin[l][0];
    if (!(l % (NFFT_2 + 1)))
      SumFirstColumn += fFFTin[l][0];
  }
  fSumAk = 2.0*fSumAk - SumFirstColumn;
*/
  fSumAk = 0.0;
  for (int l(0); l < NFFTsq; ++l) {
    fSumAk += fFFTin[l][0];
  }

  return;
}

void TBulkTriVortexNGLFieldCalc::CalculateGrid() const {
  // SetParameters - method has to be called from the user before the calculation!!
  if (fParam.size() < 3) {
    cout << endl << "The SetParameters-method has to be called before B(x,y) can be calculated!" << endl;
    return;
  }
  if (!fParam[0] || !fParam[1] || !fParam[2]) {
    cout << endl << "The field, penetration depth and coherence length have to have finite values in order to calculate B(x,y)!" << endl;
    return;
  }

  double field(fabs(fParam[0])), lambda(fabs(fParam[1])), xi(fabs(fParam[2]));
  fKappa = lambda/xi;
  double Hc2(getHc2(xi)), Hc2_kappa(Hc2/fKappa), scaledB(field/Hc2_kappa);  // field in Brandt's reduced units

  fLatticeConstant = sqrt(2.0*TWOPI/(fKappa*scaledB*sqrt3));  // intervortex spacing in Brandt's reduced units

  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFTsq(fSteps*fSteps);
  const int NFFTsq_2((fSteps/2 + 1)*fSteps);

  // first check that the field is not larger than Hc2 and that we are dealing with a type II SC ...
  if ((field >= Hc2) || (lambda < xi/sqrt(2.0))) {
    int m;
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(m) schedule(dynamic)
    #endif
    for (m = 0; m < NFFTsq; m++) {
      fFFTout[m] = field;
    }
    // Set the flag which shows that the calculation has been done
    fGridExists = true;
    return;
  }

  int l;

  // ... now fill in the Fourier components (Abrikosov) if everything was okay above

  FillAbrikosovCoefficients();

  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFT; l++) {
    fCheckAkConvergence[l] = fFFTin[l][0];
  }

  CalculateSumAk();

  // cout << "fSumAk = " << fSumAk << endl;

  // Do the Fourier transform to get omega(x,y) - Abrikosov

  fftw_execute(fFFTplan);

  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsq; l++) {
    fOmegaMatrix[l] = fSumAk - fRealSpaceMatrix[l][0];
  }

  // Calculate the gradient of omega

  CalculateGradient();

  // Calculate Q-Abrikosov

  double denomQA;

  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l,denomQA) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsq; l++) {
    if (!fOmegaMatrix[l] || !l || (l == (NFFT+1)*NFFT_2)) {
      fQMatrixA[l][0] = 0.0;
      fQMatrixA[l][1] = 0.0;
    } else {
      denomQA = 2.0*fKappa*fOmegaMatrix[l];
      fQMatrixA[l][0] = fOmegaDiffMatrix[l][1]/denomQA;
      fQMatrixA[l][1] = -fOmegaDiffMatrix[l][0]/denomQA;
    }
    fQMatrix[l][0] = fQMatrixA[l][0];
    fQMatrix[l][1] = fQMatrixA[l][1];
  }
/*
  fQMatrixA[0][0] = fQMatrixA[NFFT][0];
  fQMatrixA[(NFFT+1)*NFFT_2][0] = fQMatrixA[0][0];
  fQMatrix[0][0] = fQMatrixA[0][0];
  fQMatrix[(NFFT+1)*NFFT_2][0] = fQMatrixA[0][0];
  fQMatrixA[0][1] = fQMatrixA[NFFT][1];
  fQMatrixA[(NFFT+1)*NFFT_2][1] = fQMatrixA[0][1];
  fQMatrix[0][1] = fQMatrixA[0][1];
  fQMatrix[(NFFT+1)*NFFT_2][1] = fQMatrixA[0][1];
*/
  // initialize B(x,y) with the mean field

  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsq; l++) {
    fFFTout[l] = scaledB;
  }

  bool akConverged(false), bkConverged(false), akInitiallyConverged(false), firstBkCalculation(true);
  double fourKappaSq(4.0*fKappa*fKappa);


  while (!akConverged || !bkConverged) {

    // First iteration step for aK
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
    for (l = 0; l < NFFTsq; l++) {
      if (fOmegaMatrix[l]) {
        fRealSpaceMatrix[l][0] = fOmegaMatrix[l]*(fOmegaMatrix[l] + fQMatrix[l][0]*fQMatrix[l][0] + fQMatrix[l][1]*fQMatrix[l][1] - 2.0) + \
         (fOmegaDiffMatrix[l][0]*fOmegaDiffMatrix[l][0] + fOmegaDiffMatrix[l][1]*fOmegaDiffMatrix[l][1])/(fourKappaSq*fOmegaMatrix[l]);
      } else {
        fRealSpaceMatrix[l][0] = 0.0;
      }
      fRealSpaceMatrix[l][1] = 0.0;
    }

    // At the two vortex cores g(r) is diverging; since all of this should be a smooth function anyway, I set the value of the next neighbour r
    // for the two vortex core positions in my matrix
    fRealSpaceMatrix[0][0] = fRealSpaceMatrix[NFFT][0];
    fRealSpaceMatrix[(NFFT+1)*NFFT_2][0] = fRealSpaceMatrix[0][0];

    fftw_execute(fFFTplanOmegaToAk);

    ManipulateFourierCoefficientsA();

    // Second iteration step for aK, first recalculate omega and its gradient

    CalculateSumAk();

    // cout << "fSumAk = " << fSumAk << endl;

    // Need a copy of the aK-matrix since FFTW is manipulating the input in c2r and r2c transforms
    // Store it in the first half of the bK-matrix
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
    for (l = 0; l < NFFTsq_2; l++) {
      fBkMatrix[l][0] = fFFTin[l][0];
    }

    // Do the Fourier transform to get omega(x,y)

    fftw_execute(fFFTplan);

    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
    for (l = 0; l < NFFTsq; l++) {
      fOmegaMatrix[l] = fSumAk - fRealSpaceMatrix[l][0];
    }

    CalculateGradient();

    // Get the spacial averages of the second iteration step for aK

    fSumSum = 0.0;
    fSumOmegaSq = 0.0;
    for (l = 0; l < NFFTsq; l++) {
      fSumOmegaSq += fOmegaMatrix[l]*fOmegaMatrix[l];
      if(fOmegaMatrix[l]){
        fSumSum += fOmegaMatrix[l]*(1.0 - (fQMatrix[l][0]*fQMatrix[l][0] + fQMatrix[l][1]*fQMatrix[l][1])) - \
         (fOmegaDiffMatrix[l][0]*fOmegaDiffMatrix[l][0] + fOmegaDiffMatrix[l][1]*fOmegaDiffMatrix[l][1])/(fourKappaSq*fOmegaMatrix[l]);
      } else {
        if (l < NFFTsq - NFFT && fOmegaMatrix[l+NFFT]) {
          fSumSum += fOmegaMatrix[l+NFFT]*(1.0 - (fQMatrix[l+NFFT][0]*fQMatrix[l+NFFT][0] + fQMatrix[l+NFFT][1]*fQMatrix[l+NFFT][1])) - \
           (fOmegaDiffMatrix[l+NFFT][0]*fOmegaDiffMatrix[l+NFFT][0] + \
            fOmegaDiffMatrix[l+NFFT][1]*fOmegaDiffMatrix[l+NFFT][1])/(fourKappaSq*fOmegaMatrix[l+NFFT]);
        }
      }
    }

    // Restore the aK-matrix from the bK-space and multiply with the spacial averages
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
    for (l = 0; l < NFFTsq_2; l++) {
      fFFTin[l][0] = fBkMatrix[l][0]*fSumSum/fSumOmegaSq;
      fFFTin[l][1] = 0.0;
    }

    // Check if the aK iterations converged

    akConverged = true;

    for (l = 0; l < NFFT; l++) {
      if (fFFTin[l][0]){
        if (((fabs(fFFTin[l][0]) > 1.0E-6) && (fabs(fCheckAkConvergence[l] - fFFTin[l][0])/fFFTin[l][0] > 1.0E-6)) || \
        (fCheckAkConvergence[l]/fFFTin[l][0] < 0.0)) {
          //cout << "old: " << fCheckAkConvergence[l] << ", new: " << fFFTin[l][0] << endl;
          akConverged = false;
          //cout << "index = " << l << endl;
          break;
        }
      }
    }

    if (!akConverged) {
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
      for (l = 0; l < NFFT; l++) {
        fCheckAkConvergence[l] = fFFTin[l][0];
      }
    } else {
      akInitiallyConverged = true;
      //break;
    }

    //  cout << "Ak Convergence: " << akConverged << endl;

    // Calculate omega again either for the bK-iteration step or again the aK-iteration

    CalculateSumAk();

    // cout << "fSumAk = " << fSumAk << " count = " << count << endl;

    // Do the Fourier transform to get omega(x,y)

    fftw_execute(fFFTplan);

    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
    for (l = 0; l < NFFTsq; l++) {
      fOmegaMatrix[l] = fSumAk - fRealSpaceMatrix[l][0];
    }

    CalculateGradient();

    if (akInitiallyConverged) {  // if the aK iterations converged, go on with the bK calculation
      //cout << "converged, count=" << count << endl;
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(l) schedule(dynamic)
      #endif
      for (l = 0; l < NFFTsq; l++) {
        fBkMatrix[l][0] = fOmegaMatrix[l]*fFFTout[l] + fSumAk*(scaledB - fFFTout[l]) + \
        fQMatrix[l][1]*fOmegaDiffMatrix[l][0] - fQMatrix[l][0]*fOmegaDiffMatrix[l][1];
        fBkMatrix[l][1] = 0.0;
      }

      // At the two vortex cores Q(r) is diverging; since all of this should be a smooth function anyway, I set the value of the next neighbour r
      // for the two vortex core positions in my matrix
      fBkMatrix[0][0] = fBkMatrix[NFFT][0];
      fBkMatrix[(NFFT+1)*NFFT_2][0] = fBkMatrix[0][0];

      fftw_execute(fFFTplanOmegaToBk);

      ManipulateFourierCoefficientsB();

      // Check the convergence of the bK-iterations

      if (firstBkCalculation) {
        #ifdef HAVE_GOMP
        #pragma omp parallel for default(shared) private(l) schedule(dynamic)
        #endif
        for (l = 0; l < NFFT; l++) {
          fCheckBkConvergence[l] = 0.0;
        }
        firstBkCalculation = false;
        akConverged = false;
      }

      bkConverged = true;

      for (l = 0; l < NFFT; l++) {
        if (fBkMatrix[l][0]) {
          if (((fabs(fBkMatrix[l][0]) > 1.0E-6) && (fabs(fCheckBkConvergence[l] - fBkMatrix[l][0])/fabs(fBkMatrix[l][0]) > 1.0E-6)) || \
          (fCheckBkConvergence[l]/fBkMatrix[l][0] < 0.0)) {
            // cout << "old: " << fCheckBkConvergence[l] << ", new: " << fBkMatrix[l][0] << endl;
            bkConverged = false;
            break;
          }
        }
      }

      // cout << "Bk Convergence: " << bkConverged << endl;

      if (!bkConverged) {
        #ifdef HAVE_GOMP
        #pragma omp parallel for default(shared) private(l) schedule(dynamic)
        #endif
        for (l = 0; l < NFFT; l++) {
          fCheckBkConvergence[l] = fBkMatrix[l][0];
        }
      }

      // Go on with the calculation of B(x,y) and Q(x,y)

      // In order to save memory I will not allocate more space for another matrix but save a copy of the bKs in the aK-Matrix
      // Since aK is only half the size of bK, store every second entry in the imaginary part of aK
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(l) schedule(dynamic)
      #endif
      for (l = 0; l < NFFTsq; l+=2) {
        fFFTin[l/2][0] = fBkMatrix[l][0];
        fFFTin[l/2][1] = fBkMatrix[l+1][0];
      }

      // Fourier transform to get B(x,y)

      fftw_execute(fFFTplanBkToBandQ);

      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(l) schedule(dynamic)
      #endif
      for (l = 0; l < NFFTsq; l++) {
        fFFTout[l] = scaledB + fBkMatrix[l][0];
      }

      if (bkConverged && akConverged)
        break;

      // Restore bKs for Qx calculation and Fourier transform to get Qx
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(l) schedule(dynamic)
      #endif
      for (l = 0; l < NFFTsq; l+=2) {
        fBkMatrix[l][0] = fFFTin[l/2][0];
        fBkMatrix[l+1][0] = fFFTin[l/2][1];
        fBkMatrix[l][1] = 0.0;
        fBkMatrix[l+1][1] = 0.0;
      }

      ManipulateFourierCoefficientsForQx();

      fftw_execute(fFFTplanBkToBandQ);

      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(l) schedule(dynamic)
      #endif
      for (l = 0; l < NFFTsq; l++) {
        fQMatrix[l][0] = fQMatrixA[l][0] - fBkMatrix[l][1];
      }

      // Restore bKs for Qy calculation and Fourier transform to get Qy

      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(l) schedule(dynamic)
      #endif
      for (l = 0; l < NFFTsq; l+=2) {
        fBkMatrix[l][0] = fFFTin[l/2][0];
        fBkMatrix[l+1][0] = fFFTin[l/2][1];
        fBkMatrix[l][1] = 0.0;
        fBkMatrix[l+1][1] = 0.0;
      }

      ManipulateFourierCoefficientsForQy();

      fftw_execute(fFFTplanBkToBandQ);

      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(l) schedule(dynamic)
      #endif
      for (l = 0; l < NFFTsq; l++) {
        fQMatrix[l][1] = fQMatrixA[l][1] + fBkMatrix[l][1];
      }
    } // end if (akInitiallyConverged)
  } // end while

  // If the iterations have converged, rescale the field from Brandt's units to Gauss

  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsq; l++) {
    fFFTout[l] *= Hc2_kappa;
  }

  // Set the flag which shows that the calculation has been done

  fGridExists = true;
  return;

}
