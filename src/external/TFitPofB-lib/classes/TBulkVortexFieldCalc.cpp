/***************************************************************************

  TBulkVortexFieldCalc.cpp

  Author: Bastian M. Wojek, Alexander Maisuradze
  e-mail: bastian.wojek@psi.ch

  2009/10/17

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

#include "TBulkVortexFieldCalc.h"

#include <cstdlib>
#include <cmath>
#include <omp.h>
#include <iostream>
using namespace std;

#define PI 3.14159265358979323846
#define TWOPI 6.28318530717958647692

const double fluxQuantum(2.067833667e7); // 10e14 times CGS units %% in CGS units should be 10^-7
                                         // in this case this is Gauss per square nm
const double sqrt3(sqrt(3.0));

double getXi(const double hc2) { // get xi given Hc2 in Gauss
  if (hc2)
    return sqrt(fluxQuantum/(TWOPI*hc2));
  else
    return 0.0;
}

double getHc2(const double xi) { // get Hc2 given xi in nm
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

TBulkTriVortexLondonFieldCalc::TBulkTriVortexLondonFieldCalc(const string& wisdom, const unsigned int steps) {
  fWisdom = wisdom;
  fSteps = steps;
  fParam.resize(3);
  fGridExists = false;
  if (fSteps%2)
    fSteps++;

  int init_threads(fftw_init_threads());
  if (init_threads)
    fftw_plan_with_nthreads(2);

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

// double TBulkTriVortexLondonFieldCalc::GetBatPoint(double relX, double relY) const {
// 
//   double field(fParam[0]), lambda(fParam[1]), xi(fParam[2]);
//   double xisq_2(0.5*xi*xi), lambdasq(lambda*lambda);
// 
//   double latConstTr(sqrt(fluxQuantum/field)*sqrt(sqrt(4.0/3.0)));
//   double Hc2(getHc2(xi));
// 
//   double xCoord(latConstTr*relX); // in nanometers
//   double yCoord(latConstTr*relY);
// 
//   if ((field < Hc2) && (lambda > xi/sqrt(2.0))) {
//     double KLatTr(4.0*PI/(latConstTr*sqrt3));
//     double fourierSum(1.0), fourierAdd(0.0), expo;
// 
//     // k = 0, l = 0 gives 1.0, already in fourierSum
// 
//     // l = 0, only integer k
//     for (double k (1.0); k < static_cast<double>(fNFourierComp); k+=1.0){
//       expo = 3.0*pow(KLatTr*k, 2.0);
//       fourierAdd += exp(-xisq_2*expo)/(lambdasq*expo + 1.0)*cos(sqrt3*KLatTr*k*xCoord);
//     }
// 
//     fourierSum += 2.0*fourierAdd;
//     fourierAdd = 0.0;
// 
//     // k = 0, only integer l
//     for (double l (1.0); l < static_cast<double>(fNFourierComp); l+=1.0){
//       expo = pow(KLatTr*l, 2.0);
//       fourierAdd += exp(-xisq_2*expo)/(lambdasq*expo + 1.0)*cos(KLatTr*l*yCoord);
//     }
// 
//     fourierSum += 2.0*fourierAdd;
//     fourierAdd = 0.0;
// 
//     // k != 0, l != 0 both integers
//     for (double k (1.0); k < static_cast<double>(fNFourierComp); k+=1.0){
//       for (double l (1.0); l < static_cast<double>(fNFourierComp); l+=1.0){
//         expo = KLatTr*KLatTr*(3.0*k*k + l*l);
//         fourierAdd += exp(-xisq_2*expo)/(lambdasq*expo + 1.0)*cos(sqrt3*KLatTr*k*xCoord)*cos(KLatTr*l*yCoord);
//       }
//     }
// 
//     fourierSum += 4.0*fourierAdd;
//     fourierAdd = 0.0;
// 
//     // k != 0, l != 0 both half-integral numbers
//     for (double k (0.5); k < static_cast<double>(fNFourierComp); k+=1.0){
//       for (double l (0.5); l < static_cast<double>(fNFourierComp); l+=1.0){
//         expo = KLatTr*KLatTr*(3.0*k*k + l*l);
//         fourierAdd += exp(-xisq_2*expo)/(lambdasq*expo + 1.0)*cos(sqrt3*KLatTr*k*xCoord)*cos(KLatTr*l*yCoord);
//       }
//     }
// 
//     fourierSum += 4.0*fourierAdd;
// 
// //     for(int mm = -fNFourierComp; mm <= static_cast<int>(fNFourierComp); mm++) {
// //       for(int nn = -fNFourierComp; nn <= static_cast<int>(fNFourierComp); nn++) {
// //         fourierSum += cos(KLatTr*(xCoord*mm*(0.5*sqrt(3.0)) + yCoord*mm*0.5 + yCoord*nn))*exp(-(0.5*fParam[1]*fParam[1]*KLatTr*KLatTr)*
// //          (0.75*mm*mm + (nn + 0.5*mm)*(nn + 0.5*mm)))/(1.0+(fParam[0]*KLatTr*fParam[0]*KLatTr)*(0.75*mm*mm + (nn + 0.5*mm)*(nn + 0.5*mm)));
// //       }
// //     }
// //    cout << " " << fourierSum << ", ";
//     return field*fourierSum;
//   }
//   else
//     return field;
// 
// }

void TBulkTriVortexLondonFieldCalc::CalculateGrid() const {
  // SetParameters - method has to be called from the user before the calculation!!
  double field(abs(fParam[0])), lambda(abs(fParam[1])), xi(abs(fParam[2]));
  double Hc2(getHc2(xi));

  double latConstTr(sqrt(fluxQuantum/field*sqrt(4.0/3.0)));
  double xisq_2_scaled(2.0/3.0*pow(xi*PI/latConstTr,2.0)), lambdasq_scaled(4.0/3.0*pow(lambda*PI/latConstTr,2.0));

  int NFFT(fSteps);
  int NFFT_2(fSteps/2);
  int NFFTsq(fSteps*fSteps);

   // fill the field Fourier components in the matrix

  int m;

  // ... but first check that the field is not larger than Hc2 and that we are dealing with a type II SC
  if ((field >= Hc2) || (lambda < xi/sqrt(2.0))) {
#pragma omp parallel for default(shared) private(m) schedule(dynamic)
    for (m = 0; m < NFFTsq; m++) {
      fFFTout[m] = field;
    }
    // Set the flag which shows that the calculation has been done
    fGridExists = true;
    return;
  }

  // ... now fill in the Fourier components if everything was okay above
  double Gsq;
  int k, l, lNFFT_2;

// omp causes problems with the fftw_complex*... comment it out for the moment
//#pragma omp parallel default(shared) private(l,lNFFT_2,k,Gsq)
  {
//  #pragma omp sections nowait
    {
//      #pragma omp section
//      #pragma omp parallel for default(shared) private(l,lNFFT_2,k) schedule(dynamic)
      for (l = 0; l < NFFT_2; l += 2) {
        lNFFT_2 = l*(NFFT_2 + 1);
        for (k = 0; k < NFFT_2; k += 2) {
          Gsq = 3.0*static_cast<double>(k*k) + static_cast<double>(l*l);
          fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
          fFFTin[lNFFT_2 + k][1] = 0.0;
          fFFTin[lNFFT_2 + k + 1][0] = 0.0;
          fFFTin[lNFFT_2 + k + 1][1] = 0.0;
        }
        k = NFFT_2;
        Gsq = 3.0*static_cast<double>(k*k) + static_cast<double>(l*l);
        fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
        fFFTin[lNFFT_2 + k][1] = 0.0;
      }

//      #pragma omp section
//      #pragma omp parallel for default(shared) private(l,lNFFT_2,k) schedule(dynamic)
      for (l = NFFT_2; l < NFFT; l += 2) {
        lNFFT_2 = l*(NFFT_2 + 1);
        for (k = 0; k < NFFT_2; k += 2) {
          Gsq = 3.0*static_cast<double>(k*k) + static_cast<double>((NFFT-l)*(NFFT-l));
          fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
          fFFTin[lNFFT_2 + k][1] = 0.0;
          fFFTin[lNFFT_2 + k + 1][0] = 0.0;
          fFFTin[lNFFT_2 + k + 1][1] = 0.0;
        }
        k = NFFT_2;
        Gsq = 3.0*static_cast<double>(k*k) + static_cast<double>((NFFT-l)*(NFFT-l));
        fFFTin[lNFFT_2 + k][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
        fFFTin[lNFFT_2 + k][1] = 0.0;
      }

      // intermediate rows
//      #pragma omp section
//      #pragma omp parallel for default(shared) private(l,lNFFT_2,k) schedule(dynamic)
      for (l = 1; l < NFFT_2; l += 2) {
        lNFFT_2 = l*(NFFT_2 + 1);
        for (k = 0; k < NFFT_2; k += 2) {
          Gsq = 3.0*static_cast<double>((k + 1)*(k + 1)) + static_cast<double>(l*l);
          fFFTin[lNFFT_2 + k][0] = 0.0;
          fFFTin[lNFFT_2 + k][1] = 0.0;
          fFFTin[lNFFT_2 + k + 1][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
          fFFTin[lNFFT_2 + k + 1][1] = 0.0;
        }
        k = NFFT_2;
        fFFTin[lNFFT_2 + k][0] = 0.0;
        fFFTin[lNFFT_2 + k][1] = 0.0;
      }

//      #pragma omp section
//      #pragma omp parallel for default(shared) private(l,lNFFT_2,k) schedule(dynamic)
      for (l = NFFT_2 + 1; l < NFFT; l += 2) {
        lNFFT_2 = l*(NFFT_2 + 1);
        for (k = 0; k < NFFT_2; k += 2) {
          Gsq = 3.0*static_cast<double>((k+1)*(k+1)) + static_cast<double>((NFFT-l)*(NFFT-l));
          fFFTin[lNFFT_2 + k][0] = 0.0;
          fFFTin[lNFFT_2 + k][1] = 0.0;
          fFFTin[lNFFT_2 + k + 1][0] = exp(-xisq_2_scaled*Gsq)/(1.0+lambdasq_scaled*Gsq);
          fFFTin[lNFFT_2 + k + 1][1] = 0.0;
        }
        k = NFFT_2;
        fFFTin[lNFFT_2 + k][0] = 0.0;
        fFFTin[lNFFT_2 + k][1] = 0.0;
      }
    }  /* end of sections */

  }  /* end of parallel section */

  // Do the Fourier transform to get B(x,y)

  fftw_execute(fFFTplan);

  // Multiply by the applied field
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsq; l++) {
    fFFTout[l] *= field;
  }

  // Set the flag which shows that the calculation has been done

  fGridExists = true;
  return;

}

double TBulkTriVortexLondonFieldCalc::GetBmin() const {
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

double TBulkTriVortexLondonFieldCalc::GetBmax() const {
  if (fGridExists)
    return fFFTout[0];
  else {
    CalculateGrid();
    return GetBmax();
  }
}

