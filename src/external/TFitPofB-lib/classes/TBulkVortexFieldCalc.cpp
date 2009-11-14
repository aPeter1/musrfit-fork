/***************************************************************************

  TBulkVortexFieldCalc.cpp

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

#include "TBulkVortexFieldCalc.h"

#include <cstdlib>
#include <cmath>
#include <omp.h>
#include <iostream>
using namespace std;

#define PI 3.14159265358979323846
#define TWOPI 6.28318530717958647692

const double fluxQuantum(2.067833667e7); // in this case this is Gauss times square nm

const double sqrt3(sqrt(3.0));

//const double pi_4sqrt3(0.25*PI/sqrt3);
const double pi_4sqrt3(0.25*PI/sqrt(3.0));
//const double pi_4sqrt3(PI*sqrt(3.0));

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

  int init_threads(fftw_init_threads());
  if (init_threads)
    fftw_plan_with_nthreads(2);

  const unsigned int stepsSq(fSteps*fSteps);

  fFFTout = new double[stepsSq];

  fFFTin = new fftw_complex[stepsSq]; // Ak matrix
  fBkMatrix = new fftw_complex[stepsSq];
  fTempMatrix = new fftw_complex[stepsSq];
  fOmegaMatrix = new double[stepsSq];
//  fOmegaSqMatrix = new double[stepsSq];
//  fOmegaDDiffXMatrix = new double[stepsSq];
//  fOmegaDDiffYMatrix = new double[stepsSq];
  fOmegaDiffMatrix = new fftw_complex[stepsSq];
//  fOmegaDiffYMatrix = new double[stepsSq];
//  fGMatrix = new double[stepsSq];
  fQMatrix = new fftw_complex[stepsSq];
//  fQyMatrix = new double[stepsSq];
  fQMatrixA = new fftw_complex[stepsSq];
//  fQyMatrixA = new double[stepsSq];
//  fAbrikosovCheck = new double[stepsSq];

  fCheckAkConvergence = new double[fSteps];
  fCheckBkConvergence = new double[fSteps];

//  cout << "Check for the FFT plans..." << endl;

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
    fFFTplanAkToOmega = fftw_plan_dft_2d(fSteps, fSteps, fFFTin, fTempMatrix, FFTW_BACKWARD, FFTW_EXHAUSTIVE);
    fFFTplanBkToBandQ = fftw_plan_dft_2d(fSteps, fSteps, fBkMatrix, fBkMatrix, FFTW_BACKWARD, FFTW_EXHAUSTIVE);
    fFFTplanOmegaToAk = fftw_plan_dft_2d(fSteps, fSteps, fTempMatrix, fFFTin, FFTW_FORWARD, FFTW_EXHAUSTIVE);
    fFFTplanOmegaToBk = fftw_plan_dft_2d(fSteps, fSteps, fBkMatrix, fBkMatrix, FFTW_FORWARD, FFTW_EXHAUSTIVE);
  }
  else {
    fFFTplanAkToOmega = fftw_plan_dft_2d(fSteps, fSteps, fFFTin, fTempMatrix, FFTW_BACKWARD, FFTW_ESTIMATE);
    fFFTplanBkToBandQ = fftw_plan_dft_2d(fSteps, fSteps, fBkMatrix, fBkMatrix, FFTW_BACKWARD, FFTW_ESTIMATE);
    fFFTplanOmegaToAk = fftw_plan_dft_2d(fSteps, fSteps, fTempMatrix, fFFTin, FFTW_FORWARD, FFTW_ESTIMATE);
    fFFTplanOmegaToBk = fftw_plan_dft_2d(fSteps, fSteps, fBkMatrix, fBkMatrix, FFTW_FORWARD, FFTW_ESTIMATE);
  }
}

TBulkTriVortexNGLFieldCalc::~TBulkTriVortexNGLFieldCalc() {

  // clean up

  fftw_destroy_plan(fFFTplanAkToOmega);
  fftw_destroy_plan(fFFTplanBkToBandQ);
  fftw_destroy_plan(fFFTplanOmegaToAk);
  fftw_destroy_plan(fFFTplanOmegaToBk);
//  delete[] fAkMatrix; fAkMatrix = 0;
  delete[] fBkMatrix; fBkMatrix = 0;
  delete[] fTempMatrix; fTempMatrix = 0;
  delete[] fOmegaMatrix; fOmegaMatrix = 0;
//  delete[] fOmegaSqMatrix; fOmegaSqMatrix = 0;
  delete[] fOmegaDiffMatrix; fOmegaDiffMatrix = 0;
//  delete[] fOmegaDiffYMatrix; fOmegaDiffYMatrix = 0;
//  delete[] fOmegaDDiffXMatrix; fOmegaDiffXMatrix = 0;
//  delete[] fOmegaDDiffYMatrix; fOmegaDiffYMatrix = 0;
//  delete[] fGMatrix; fGMatrix = 0;
  delete[] fQMatrix; fQMatrix = 0;
//  delete[] fQyMatrix; fQyMatrix = 0;
  delete[] fQMatrixA; fQMatrixA = 0;
//  delete[] fQyMatrixA; fQyMatrixA = 0;
  delete[] fCheckAkConvergence; fCheckAkConvergence = 0;
  delete[] fCheckBkConvergence; fCheckBkConvergence = 0;
//  delete[] fAbrikosovCheck; fAbrikosovCheck = 0;
}

void TBulkTriVortexNGLFieldCalc::CalculateGradient() const {
  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFTsq(fSteps*fSteps);

// Derivatives of omega

  const double denomY(2.0*fLatticeConstant/static_cast<double>(NFFT));
  const double denomX(denomY*sqrt3);

  // Ensure that omega at the vortex-core positions is zero
  fOmegaMatrix[0] = 0.0;
  fOmegaMatrix[(NFFT+1)*NFFT_2] = 0.0;

  int i;

  // Ensure that omega is NOT negative
//#pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = 0; i < NFFTsq; i += 1) {
    if (fOmegaMatrix[i] < 0.0) {
      cout << "Omega negative for index " << i << ", value: " << fOmegaMatrix[i] << endl;
      fOmegaMatrix[i] = 0.0;
    }
  }

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

// // g-Matrix
// 
// #pragma omp parallel for default(shared) private(i) schedule(dynamic)
//   for (i = 0; i < NFFTsq; i++) {
//     if (!fOmegaMatrix[i]) {
//       fGMatrix[i] = 0.0;
//     } else {
//       fGMatrix[i] = (fOmegaDiffXMatrix[i]*fOmegaDiffXMatrix[i]+fOmegaDiffYMatrix[i]*fOmegaDiffYMatrix[i])/(fourKappaSq*fOmegaMatrix[i]);
//     }
//   }
// 
// // Laplacian for Abrikosov check
// 
// #pragma omp parallel for default(shared) private(i) schedule(dynamic)
//   for (i = 0; i < NFFTsq; i += 1) {
//     if (!(i % NFFT)) { // first column
//       fOmegaDDiffXMatrix[i] = (fOmegaDiffXMatrix[i+1]-fOmegaDiffXMatrix[i+NFFT-1])/denomX;
//     } else if (!((i + 1) % NFFT)) { // last column
//       fOmegaDDiffXMatrix[i] = (fOmegaDiffXMatrix[i-NFFT+1]-fOmegaDiffXMatrix[i-1])/denomX;
//     } else {
//       fOmegaDDiffXMatrix[i] = (fOmegaDiffXMatrix[i+1]-fOmegaDiffXMatrix[i-1])/denomX;
//     }
//   }
// 
//   for (i = 0; i < NFFT; i++) { // first row
//     fOmegaDDiffYMatrix[i] = (fOmegaDiffYMatrix[i+NFFT]-fOmegaDiffYMatrix[NFFTsq-NFFT+i])/denomY;
//   }
// 
//   for (i = NFFTsq - NFFT; i < NFFTsq; i++) { // last row
//     fOmegaDDiffYMatrix[i] = (fOmegaDiffYMatrix[i-NFFTsq+NFFT]-fOmegaDiffYMatrix[i-NFFT])/denomY;
//   }
// 
// #pragma omp parallel for default(shared) private(i) schedule(dynamic)
//   for (i = NFFT; i < NFFTsq - NFFT; i++) { // the rest
//     fOmegaDDiffYMatrix[i] = (fOmegaDiffYMatrix[i+NFFT]-fOmegaDiffYMatrix[i-NFFT])/denomY;
//   }

  return;
}

void TBulkTriVortexNGLFieldCalc::FillAbrikosovCoefficients() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2);

  double Gsq, sign;
  int k, l, lNFFT;

  for (l = 0; l < NFFT_2; l += 2) {
    if (!(l % 4)) {
      sign = 1.0;
    } else {
      sign = -1.0;
    }
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      sign = -sign;
      Gsq = static_cast<double>(k*k) + 3.0*static_cast<double>(l*l);
      fFFTin[lNFFT + k][0] = sign*exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT + k][1] = 0.0;
      fFFTin[lNFFT + k + 1][0] = 0.0;
      fFFTin[lNFFT + k + 1][1] = 0.0;
    }
    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      sign = -sign;
      Gsq = static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>(l*l);
      fFFTin[lNFFT + k][0] = sign*exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT + k][1] = 0.0;
      fFFTin[lNFFT + k + 1][0] = 0.0;
      fFFTin[lNFFT + k + 1][1] = 0.0;
    }

  }

  for (l = NFFT_2; l < NFFT; l += 2) {
    if (!(l % 4)) {
      sign = 1.0;
    } else {
      sign = -1.0;
    }
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      sign = -sign;
      Gsq = static_cast<double>(k*k) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
      fFFTin[lNFFT + k][0] = sign*exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT + k][1] = 0.0;
      fFFTin[lNFFT + k + 1][0] = 0.0;
      fFFTin[lNFFT + k + 1][1] = 0.0;
    }
    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      sign = -sign;
      Gsq = static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
      fFFTin[lNFFT + k][0] = sign*exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT + k][1] = 0.0;
      fFFTin[lNFFT + k + 1][0] = 0.0;
      fFFTin[lNFFT + k + 1][1] = 0.0;
    }

  }

  // intermediate rows
  for (l = 1; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      Gsq = static_cast<double>((k + 1)*(k + 1)) + 3.0*static_cast<double>(l*l);
      fFFTin[lNFFT + k][0] = 0.0;
      fFFTin[lNFFT + k][1] = 0.0;
      fFFTin[lNFFT + k + 1][0] = exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT + k + 1][1] = 0.0;
    }
    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      Gsq = static_cast<double>((k + 1-NFFT)*(k + 1-NFFT)) + 3.0*static_cast<double>(l*l);
      fFFTin[lNFFT + k][0] = 0.0;
      fFFTin[lNFFT + k][1] = 0.0;
      fFFTin[lNFFT + k + 1][0] = exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT + k + 1][1] = 0.0;
    }
  }

  for (l = NFFT_2 + 1; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      Gsq = static_cast<double>((k+1)*(k+1)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
      fFFTin[lNFFT + k][0] = 0.0;
      fFFTin[lNFFT + k][1] = 0.0;
      fFFTin[lNFFT + k + 1][0] = exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT + k + 1][1] = 0.0;
    }
    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      Gsq = static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
      fFFTin[lNFFT + k][0] = 0.0;
      fFFTin[lNFFT + k][1] = 0.0;
      fFFTin[lNFFT + k + 1][0] = exp(-pi_4sqrt3*Gsq);
      fFFTin[lNFFT + k + 1][1] = 0.0;
    }
  }

  fFFTin[0][0] = 0.0;

  return;

}

void TBulkTriVortexNGLFieldCalc::ManipulateFourierCoefficients(fftw_complex *F, const double &coeff2, const double &coeff3) const {
  const int NFFT(fSteps), NFFT_2(fSteps/2);

  const double coeff1(4.0/3.0*pow(PI/fLatticeConstant,2.0));

  int lNFFT, l, k;
  double Gsq;

  for (l = 0; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>(k*k) + 3.0*static_cast<double>(l*l));
      F[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
      F[lNFFT + k][1] = 0.0;
      F[lNFFT + k + 1][0] = 0.0;
      F[lNFFT + k + 1][1] = 0.0;
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>(l*l));
      F[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
      F[lNFFT + k][1] = 0.0;
      F[lNFFT + k + 1][0] = 0.0;
      F[lNFFT + k + 1][1] = 0.0;
    }
  }

  for (l = NFFT_2; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>(k*k) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      F[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
      F[lNFFT + k][1] = 0.0;
      F[lNFFT + k + 1][0] = 0.0;
      F[lNFFT + k + 1][1] = 0.0;
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      F[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
      F[lNFFT + k][1] = 0.0;
      F[lNFFT + k + 1][0] = 0.0;
      F[lNFFT + k + 1][1] = 0.0;
    }
  }

  //intermediate rows

  for (l = 1; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>((k+1)*(k+1)) + 3.0*static_cast<double>(l*l));
      F[lNFFT + k][0] = 0.0;
      F[lNFFT + k][1] = 0.0;
      F[lNFFT + k + 1][0] *= coeff2/(Gsq+coeff3);
      F[lNFFT + k + 1][1] = 0.0;
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>(l*l));
      F[lNFFT + k][0] = 0.0;
      F[lNFFT + k][1] = 0.0;
      F[lNFFT + k + 1][0] *= coeff2/(Gsq+coeff3);
      F[lNFFT + k + 1][1] = 0.0;
    }
  }

  for (l = NFFT_2 + 1; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>((k+1)*(k+1)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      F[lNFFT + k][0] = 0.0;
      F[lNFFT + k][1] = 0.0;
      F[lNFFT + k + 1][0] *= coeff2/(Gsq+coeff3);
      F[lNFFT + k + 1][1] = 0.0;
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      Gsq = coeff1*(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      F[lNFFT + k][0] = 0.0;
      F[lNFFT + k][1] = 0.0;
      F[lNFFT + k + 1][0] *= coeff2/(Gsq+coeff3);
      F[lNFFT + k + 1][1] = 0.0;
    }
  }

  F[0][0] = 0.0;

  return;
}


void TBulkTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForQx() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2);
  const double coeff1(1.5*fLatticeConstant/PI);

  int lNFFT, l, k;

  for (l = 0; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      if (!k && !l)
        fBkMatrix[0][0] = 0.0;
      else
        fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(l)/(static_cast<double>(k*k) + 3.0*static_cast<double>(l*l));
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(l)/(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>(l*l));
    }
  }

  for (l = NFFT_2; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>(k*k) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
    }
  }

  //intermediate rows

  for (l = 1; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(l)/(static_cast<double>((k+1)*(k+1)) + 3.0*static_cast<double>(l*l));
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(l)/(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>(l*l));
    }
  }

  for (l = NFFT_2 + 1; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>((k+1)*(k+1)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
    }
  }

  return;
}

void TBulkTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForQy() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2);
  const double coeff1(0.5*sqrt3*fLatticeConstant/PI);

  int lNFFT, l, k;

  for (l = 0; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      if (!k && !l)
        fBkMatrix[0][0] = 0.0;
      else
        fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(k)/(static_cast<double>(k*k) + 3.0*static_cast<double>(l*l));
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(k-NFFT)/(static_cast<double>((k-NFFT)*(k-NFFT))+3.0*static_cast<double>(l*l));
    }
  }

  for (l = NFFT_2; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(k)/(static_cast<double>(k*k)+3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k][0] *= coeff1*static_cast<double>(k-NFFT)/(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
    }
  }

  //intermediate rows

  for (l = 1; l < NFFT_2; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(k+1)/(static_cast<double>((k+1)*(k+1)) + 3.0*static_cast<double>(l*l));
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(k+1-NFFT)/(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>(l*l));
    }
  }

  for (l = NFFT_2 + 1; l < NFFT; l += 2) {
    lNFFT = l*NFFT;
    for (k = 0; k < NFFT_2 - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(k+1)/(static_cast<double>((k+1)*(k+1)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
    }

    for (k = NFFT_2; k < NFFT - 1; k += 2) {
      fBkMatrix[lNFFT + k + 1][0] *= coeff1*static_cast<double>(k+1-NFFT)/(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
    }
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
  double Hc2(getHc2(xi)), Hc2_kappa(Hc2/fKappa), scaledB(field/Hc2_kappa);

  cout << "4pi/S = " << 2.0*fKappa*scaledB << endl;

  fLatticeConstant = sqrt(2.0*TWOPI/(fKappa*scaledB*sqrt3));

  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFTsq(fSteps*fSteps);

  // first check that the field is not larger than Hc2 and that we are dealing with a type II SC
  if ((field >= Hc2) || (lambda < xi/sqrt(2.0))) {
  int m;
#pragma omp parallel for default(shared) private(m) schedule(dynamic)
    for (m = 0; m < NFFTsq; m++) {
      fFFTout[m] = field;
    }
    // Set the flag which shows that the calculation has been done
    fGridExists = true;
    return;
  }

  // ... now fill in the Fourier components (Abrikosov) if everything was okay above

  FillAbrikosovCoefficients();

  int l;

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFT; l++) {
    fCheckAkConvergence[l] = fFFTin[l][0];
  }

  fSumAk = 0.0;
  for (l = 1; l < NFFTsq; l++) {
      fSumAk += fFFTin[l][0];
  }

  cout << "fSumAk = " << fSumAk << endl;

  // Do the Fourier transform to get omega(x,y) - Abrikosov

  fftw_execute(fFFTplanAkToOmega);

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsq; l++) {
    fOmegaMatrix[l] = fSumAk - fTempMatrix[l][0];
  }

  double sumOmega(0.0);
  for (l = 0; l < NFFTsq; l++) {
    sumOmega += fOmegaMatrix[l];
  }
  sumOmega /= static_cast<double>(NFFTsq);

  cout << "sumOmega = " << sumOmega << endl;

  CalculateGradient();

// ///////////// Check the Abrikosov solution
// 
//   for (l = 0; l < NFFTsq; l++) {
//     if (!fOmegaMatrix[l]) {
//       fAbrikosovCheck[l] = 0.0;
//     } else {
//       fAbrikosovCheck[l] = (fOmegaDiffXMatrix[l]*fOmegaDiffXMatrix[l]+fOmegaDiffYMatrix[l]*fOmegaDiffYMatrix[l])/(fOmegaMatrix[l]*fOmegaMatrix[l]) - (fOmegaDDiffXMatrix[l] + fOmegaDDiffYMatrix[l])/fOmegaMatrix[l];
//     }
//   }

  double denomQA;

#pragma omp parallel for default(shared) private(l,denomQA) schedule(dynamic)
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

  // initial B

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsq; l++) {
    fFFTout[l] = scaledB;
  }

  bool akConverged(false), bkConverged(false), akInitiallyConverged(false), firstBkCalculation(true);
  double coeff2, coeff3;
  double fourKappaSq(4.0*fKappa*fKappa);

//  int count(0);

  while (!akConverged || !bkConverged) {// break;

//  if (count == 3) break;

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      if (fOmegaMatrix[l]){
        fTempMatrix[l][0] = fOmegaMatrix[l]*(fOmegaMatrix[l] + fQMatrix[l][0]*fQMatrix[l][0] + fQMatrix[l][1]*fQMatrix[l][1] - 2.0) + \
         (fOmegaDiffMatrix[l][0]*fOmegaDiffMatrix[l][0] + fOmegaDiffMatrix[l][1]*fOmegaDiffMatrix[l][1])/(fourKappaSq*fOmegaMatrix[l]);
      } else {
        fTempMatrix[l][0] = 0.0;
      }
      fTempMatrix[l][1] = 0.0;
    }

    // At the two vortex cores g(r) is diverging; since all of this should be a smooth function anyway, I set the value of the next neighbour r
    // for the two vortex core positions in my matrix
    fTempMatrix[0][0] = fTempMatrix[NFFT][0];
    fTempMatrix[(NFFT+1)*NFFT_2][0] = fTempMatrix[0][0];

    fftw_execute(fFFTplanOmegaToAk);

    coeff2 = fourKappaSq/static_cast<double>(NFFT*NFFT);
    coeff3 = 0.5*fourKappaSq;

    ManipulateFourierCoefficients(fFFTin, coeff2, coeff3);

  fSumAk = 0.0;
  for (l = 1; l < NFFTsq; l++) {
      fSumAk += fFFTin[l][0];
  }

    cout << "fSumAk = " << fSumAk << endl;

    // Do the Fourier transform to get omega(x,y)

    fftw_execute(fFFTplanAkToOmega);

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fOmegaMatrix[l] = fSumAk - fTempMatrix[l][0];
    }

  CalculateGradient();

  sumOmega = 0.0;
  for (l = 0; l < NFFTsq; l++) {
    sumOmega += fOmegaMatrix[l];
  }
  sumOmega /= static_cast<double>(NFFTsq);
  cout << "sumOmega = " << sumOmega << endl;

    fSumSum = 0.0;
    fSumOmegaSq = 0.0;
    for (l = 0; l < NFFTsq; l++) {
      fSumOmegaSq += fOmegaMatrix[l]*fOmegaMatrix[l];
      if(fOmegaMatrix[l]){
        fSumSum += fOmegaMatrix[l]*(1.0 - (fQMatrix[l][0]*fQMatrix[l][0] + fQMatrix[l][1]*fQMatrix[l][1])) - \
         (fOmegaDiffMatrix[l][0]*fOmegaDiffMatrix[l][0] + fOmegaDiffMatrix[l][1]*fOmegaDiffMatrix[l][1])/(fourKappaSq*fOmegaMatrix[l]);
      } else {
        if (l < NFFTsq - 1 && fOmegaMatrix[l+1]) {
          fSumSum += fOmegaMatrix[l+1]*(1.0 - (fQMatrix[l+1][0]*fQMatrix[l+1][0] + fQMatrix[l+1][1]*fQMatrix[l+1][1])) - \
         (fOmegaDiffMatrix[l+1][0]*fOmegaDiffMatrix[l+1][0] + fOmegaDiffMatrix[l+1][1]*fOmegaDiffMatrix[l+1][1])/(fourKappaSq*fOmegaMatrix[l+1]);
        }
      }
    }
    
    cout << "fSumSum = " << fSumSum << ", fSumOmegaSq = " << fSumOmegaSq << endl;

    for (l = 0; l < NFFTsq; l++) {
      fFFTin[l][0] *= fSumSum/fSumOmegaSq;
    }

    akConverged = true;

    for (l = 0; l < NFFT; l++) {
      if ((fabs(fCheckAkConvergence[l] - fFFTin[l][0])/fFFTin[l][0] > 0.025) && (fabs(fFFTin[l][0]) > 1.0E-4)) {
        cout << "old: " << fCheckAkConvergence[l] << ", new: " << fFFTin[l][0] << endl;
        akConverged = false;
        break;
      }
    }

    if (!akConverged) {
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
      for (l = 0; l < NFFT; l++) {
        fCheckAkConvergence[l] = fFFTin[l][0];
      }
    } else {
      akInitiallyConverged = true;
    }

    cout << "Ak Convergence: " << akConverged << endl;

  fSumAk = 0.0;
  for (l = 1; l < NFFTsq; l++) {
      fSumAk += fFFTin[l][0];
  }

    cout << "fSumAk = " << fSumAk << endl;

    // Do the Fourier transform to get omega(x,y)

    fftw_execute(fFFTplanAkToOmega);

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fOmegaMatrix[l] = fSumAk - fTempMatrix[l][0];
    }

    CalculateGradient();
    
  sumOmega = 0.0;
  for (l = 0; l < NFFTsq; l++) {
    sumOmega += fOmegaMatrix[l];
  }
  sumOmega /= static_cast<double>(NFFTsq);
   cout << "sumOmega = " << sumOmega << endl;

//break;
  if (akInitiallyConverged) {// break;

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fBkMatrix[l][0] = fOmegaMatrix[l]*fFFTout[l] + fSumAk*(scaledB - fFFTout[l]) - fQMatrix[l][0]*fOmegaDiffMatrix[l][1] + \
       fQMatrix[l][1]*fOmegaDiffMatrix[l][0];
      fBkMatrix[l][1] = 0.0;
    }

    // At the two vortex cores Q(r) is diverging; since all of this should be a smooth function anyway, I set the value of the next neighbour r
    // for the two vortex core positions in my matrix
    fBkMatrix[0][0] = fBkMatrix[NFFT][0];
    fBkMatrix[(NFFT+1)*NFFT_2][0] = fBkMatrix[0][0];
    
//break;
    fftw_execute(fFFTplanOmegaToBk);

//break;

    if (firstBkCalculation) {
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
      for (l = 0; l < NFFT; l++) {
        fCheckBkConvergence[l] = fBkMatrix[l][0];
      }
      firstBkCalculation = false;
      akConverged = false;
    }

    coeff2 = -2.0/static_cast<double>(NFFT*NFFT);
    coeff3 = fSumAk;

    ManipulateFourierCoefficients(fBkMatrix, coeff2, coeff3);

    bkConverged = true;

    for (l = 0; l < NFFT; l++) {
      if ((fabs(fCheckBkConvergence[l] - fBkMatrix[l][0])/fBkMatrix[l][0] > 0.025) && (fabs(fBkMatrix[l][0]) > 1.0E-4)) {
        cout << "old: " << fCheckBkConvergence[l] << ", new: " << fBkMatrix[l][0] << endl;
        bkConverged = false;
        break;
      }
    }

    cout << "Bk Convergence: " << bkConverged << endl;

    if (!bkConverged) {
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
      for (l = 0; l < NFFT; l++) {
        fCheckBkConvergence[l] = fBkMatrix[l][0];
      }
    }

// In order to save memory I will not allocate further memory for another matrix but save a copy of the bKs in the TempMatrix
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fTempMatrix[l][0] = fBkMatrix[l][0];
    }

    fftw_execute(fFFTplanBkToBandQ);

    double bb;
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      bb = scaledB + fBkMatrix[l][0];
      fFFTout[l] = (bb < 0.0 ? 0.0 : bb);
//      if (bb < 0.0) cout << "Field negative: " << bb << endl;
    }

    if (bkConverged && akConverged)
      break;

// Restore bKs for Qx calculation
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fBkMatrix[l][0] = fTempMatrix[l][0];
      fBkMatrix[l][1] = 0.0;
    }

    ManipulateFourierCoefficientsForQx();

    fftw_execute(fFFTplanBkToBandQ);

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fQMatrix[l][0] = fQMatrixA[l][0] - fBkMatrix[l][1];
    }

// Restore bKs for Qy calculation
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fBkMatrix[l][0] = fTempMatrix[l][0];
      fBkMatrix[l][1] = 0.0;
    }

    ManipulateFourierCoefficientsForQy();

    fftw_execute(fFFTplanBkToBandQ);

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fQMatrix[l][1] = fQMatrixA[l][1] + fBkMatrix[l][1];
    }
  } // end if(akInitiallyConverged)
  } // end while

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsq; l++) {
    fFFTout[l] *= Hc2_kappa;
  }

  // Set the flag which shows that the calculation has been done

  fGridExists = true;
  return;

}

