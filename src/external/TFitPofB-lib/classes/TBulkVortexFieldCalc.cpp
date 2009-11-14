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

TBulkTriVortexNGLFieldCalc::TBulkTriVortexNGLFieldCalc(const string& wisdom, const unsigned int steps) : fLatticeConstant(0.0), fSumAk(0.0), fSumOmegaSq(0.0)
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

  unsigned int stepsSq(fSteps*fSteps);

  fFFTout = new double[stepsSq];

  fAkMatrix = new fftw_complex[stepsSq];
  fBkMatrix = new fftw_complex[stepsSq];
  fRealSpaceMatrix = new fftw_complex[stepsSq];
  fOmegaMatrix = new double[stepsSq];
  fOmegaSqMatrix = new double[stepsSq];
  fOmegaDDiffXMatrix = new double[stepsSq];
  fOmegaDDiffYMatrix = new double[stepsSq];
  fOmegaDiffXMatrix = new double[stepsSq];
  fOmegaDiffYMatrix = new double[stepsSq];
  fGMatrix = new double[stepsSq];
  fQxMatrix = new double[stepsSq];
  fQyMatrix = new double[stepsSq];
  fQxMatrixA = new double[stepsSq];
  fQyMatrixA = new double[stepsSq];
  fAbrikosovCheck = new double[stepsSq];

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
    fFFTplanAkToOmega = fftw_plan_dft_2d(fSteps, fSteps, fAkMatrix, fRealSpaceMatrix, FFTW_BACKWARD, FFTW_EXHAUSTIVE);
    fFFTplanBkToBandQ = fftw_plan_dft_2d(fSteps, fSteps, fBkMatrix, fRealSpaceMatrix, FFTW_BACKWARD, FFTW_EXHAUSTIVE);
    fFFTplanOmegaToAk = fftw_plan_dft_2d(fSteps, fSteps, fRealSpaceMatrix, fAkMatrix, FFTW_FORWARD, FFTW_EXHAUSTIVE);
    fFFTplanOmegaToBk = fftw_plan_dft_2d(fSteps, fSteps, fRealSpaceMatrix, fBkMatrix, FFTW_FORWARD, FFTW_EXHAUSTIVE);
  }
  else {
    fFFTplanAkToOmega = fftw_plan_dft_2d(fSteps, fSteps, fAkMatrix, fRealSpaceMatrix, FFTW_BACKWARD, FFTW_ESTIMATE);
    fFFTplanBkToBandQ = fftw_plan_dft_2d(fSteps, fSteps, fBkMatrix, fRealSpaceMatrix, FFTW_BACKWARD, FFTW_ESTIMATE);
    fFFTplanOmegaToAk = fftw_plan_dft_2d(fSteps, fSteps, fRealSpaceMatrix, fAkMatrix, FFTW_FORWARD, FFTW_ESTIMATE);
    fFFTplanOmegaToBk = fftw_plan_dft_2d(fSteps, fSteps, fRealSpaceMatrix, fBkMatrix, FFTW_FORWARD, FFTW_ESTIMATE);
  }
}

TBulkTriVortexNGLFieldCalc::~TBulkTriVortexNGLFieldCalc() {

  // clean up

  fftw_destroy_plan(fFFTplanAkToOmega);
  fftw_destroy_plan(fFFTplanBkToBandQ);
  fftw_destroy_plan(fFFTplanOmegaToAk);
  fftw_destroy_plan(fFFTplanOmegaToBk);
  delete[] fAkMatrix; fAkMatrix = 0;
  delete[] fBkMatrix; fBkMatrix = 0;
  delete[] fRealSpaceMatrix; fRealSpaceMatrix = 0;
  delete[] fOmegaMatrix; fOmegaMatrix = 0;
  delete[] fOmegaSqMatrix; fOmegaSqMatrix = 0;
  delete[] fOmegaDiffXMatrix; fOmegaDiffXMatrix = 0;
  delete[] fOmegaDiffYMatrix; fOmegaDiffYMatrix = 0;
  delete[] fOmegaDDiffXMatrix; fOmegaDiffXMatrix = 0;
  delete[] fOmegaDDiffYMatrix; fOmegaDiffYMatrix = 0;
  delete[] fGMatrix; fGMatrix = 0;
  delete[] fQxMatrix; fQxMatrix = 0;
  delete[] fQyMatrix; fQyMatrix = 0;
  delete[] fQxMatrixA; fQxMatrixA = 0;
  delete[] fQyMatrixA; fQyMatrixA = 0;
  delete[] fCheckAkConvergence; fCheckAkConvergence = 0;
  delete[] fCheckBkConvergence; fCheckBkConvergence = 0;
  delete[] fAbrikosovCheck; fAbrikosovCheck = 0;
}

void TBulkTriVortexNGLFieldCalc::CalculateIntermediateMatrices() const {
  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFTsq(fSteps*fSteps);

// Derivatives of omega

  const double denomY(2.0*fLatticeConstant/static_cast<double>(NFFT));
//  const double denomY(2.0/static_cast<double>(NFFT));
//  const double denomY(2.0);
//  const double denomX(2.0);
  const double denomX(denomY*sqrt3);
  const double kappa(fParam[1]/fParam[2]);
  const double fourKappaSq(4.0*kappa*kappa);
  
  // Ensure that omega at the vortex-core positions is zero
  fOmegaMatrix[0] = 0.0;
  fOmegaMatrix[(NFFT+1)*NFFT_2] = 0.0;

  int i;

  // Ensure that omega is NOT negative
#pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = 0; i < NFFTsq; i += 1) {
    if (fOmegaMatrix[i] < 0.0) {
      cout << "Omega negative for index " << i << endl;
      fOmegaMatrix[i] = 0.0;
    }
  }

#pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = 0; i < NFFTsq; i += 1) {
    if (!(i % NFFT)) { // first column
      fOmegaDiffXMatrix[i] = (fOmegaMatrix[i+1]-fOmegaMatrix[i+NFFT-1])/denomX;
    } else if (!((i + 1) % NFFT)) { // last column
      fOmegaDiffXMatrix[i] = (fOmegaMatrix[i-NFFT+1]-fOmegaMatrix[i-1])/denomX;
    } else {
      fOmegaDiffXMatrix[i] = (fOmegaMatrix[i+1]-fOmegaMatrix[i-1])/denomX;
    }
  }

  for (i = 0; i < NFFT; i++) { // first row
    fOmegaDiffYMatrix[i] = (fOmegaMatrix[i+NFFT]-fOmegaMatrix[NFFTsq-NFFT+i])/denomY;
  }

  for (i = NFFTsq - NFFT; i < NFFTsq; i++) { // last row
    fOmegaDiffYMatrix[i] = (fOmegaMatrix[i-NFFTsq+NFFT]-fOmegaMatrix[i-NFFT])/denomY;
  }

#pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = NFFT; i < NFFTsq - NFFT; i++) { // the rest
    fOmegaDiffYMatrix[i] = (fOmegaMatrix[i+NFFT]-fOmegaMatrix[i-NFFT])/denomY;
  }

  // Ensure that the derivatives at the vortex-core positions are zero
  fOmegaDiffXMatrix[0] = 0.0;
  fOmegaDiffXMatrix[(NFFT+1)*NFFT_2] = 0.0;
  fOmegaDiffYMatrix[0] = 0.0;
  fOmegaDiffYMatrix[(NFFT+1)*NFFT_2] = 0.0;

// g-Matrix

#pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = 0; i < NFFTsq; i++) {
    if (!fOmegaMatrix[i]) {
      fGMatrix[i] = 0.0;
    } else {
      fGMatrix[i] = (fOmegaDiffXMatrix[i]*fOmegaDiffXMatrix[i]+fOmegaDiffYMatrix[i]*fOmegaDiffYMatrix[i])/(fourKappaSq*fOmegaMatrix[i]);
    }
  }

// Laplacian for Abrikosov check

#pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = 0; i < NFFTsq; i += 1) {
    if (!(i % NFFT)) { // first column
      fOmegaDDiffXMatrix[i] = (fOmegaDiffXMatrix[i+1]-fOmegaDiffXMatrix[i+NFFT-1])/denomX;
    } else if (!((i + 1) % NFFT)) { // last column
      fOmegaDDiffXMatrix[i] = (fOmegaDiffXMatrix[i-NFFT+1]-fOmegaDiffXMatrix[i-1])/denomX;
    } else {
      fOmegaDDiffXMatrix[i] = (fOmegaDiffXMatrix[i+1]-fOmegaDiffXMatrix[i-1])/denomX;
    }
  }

  for (i = 0; i < NFFT; i++) { // first row
    fOmegaDDiffYMatrix[i] = (fOmegaDiffYMatrix[i+NFFT]-fOmegaDiffYMatrix[NFFTsq-NFFT+i])/denomY;
  }

  for (i = NFFTsq - NFFT; i < NFFTsq; i++) { // last row
    fOmegaDDiffYMatrix[i] = (fOmegaDiffYMatrix[i-NFFTsq+NFFT]-fOmegaDiffYMatrix[i-NFFT])/denomY;
  }

#pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = NFFT; i < NFFTsq - NFFT; i++) { // the rest
    fOmegaDDiffYMatrix[i] = (fOmegaDiffYMatrix[i+NFFT]-fOmegaDiffYMatrix[i-NFFT])/denomY;
  }

  return;
}

void TBulkTriVortexNGLFieldCalc::ManipulateFourierCoefficients(fftw_complex* F, const double &coeff2, const double &coeff3) const {
  const int NFFT(fSteps), NFFT_2(fSteps/2);
  int lNFFT, l, k;
  double Gsq;
  double coeff1(4.0/3.0*pow(PI/fLatticeConstant,2.0));

  for (l = 0; l < NFFT_2; l += 2) {
      lNFFT = l*NFFT;
      for (k = 0; k < NFFT_2 - 1; k += 2) {
        Gsq = coeff1*(static_cast<double>(k*k) + 3.0*static_cast<double>(l*l));
        F[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
        F[lNFFT + k][1] = 0.0;
        F[lNFFT + k + 1][0] = 0.0;
        F[lNFFT + k + 1][1] = 0.0;
      }
      if (NFFT_2 % 2) {
        Gsq = coeff1*(static_cast<double>(k*k) + 3.0*static_cast<double>(l*l));
        F[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
        F[lNFFT + k][1] = 0.0;
      }

      for (k = NFFT_2; k < NFFT - 1; k += 2) {
        Gsq = coeff1*(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>(l*l));
        F[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
        F[lNFFT + k][1] = 0.0;
        F[lNFFT + k + 1][0] = 0.0;
        F[lNFFT + k + 1][1] = 0.0;
      }
      if (NFFT_2 % 2) {
        Gsq = coeff1*(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>(l*l));
        F[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
        F[lNFFT + k][1] = 0.0;
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
      if (NFFT_2 % 2) {
        Gsq = coeff1*(static_cast<double>(k*k) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
        F[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
        F[lNFFT + k][1] = 0.0;
      }

      for (k = NFFT_2; k < NFFT - 1; k += 2) {
        Gsq = coeff1*(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
        F[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
        F[lNFFT + k][1] = 0.0;
        F[lNFFT + k + 1][0] = 0.0;
        F[lNFFT + k + 1][1] = 0.0;
      }
      if (NFFT_2 % 2) {
        Gsq = coeff1*(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
        F[lNFFT + k][0] *= coeff2/(Gsq+coeff3);
        F[lNFFT + k][1] = 0.0;
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
      if (NFFT_2 % 2) {
        F[lNFFT + k][0] = 0.0;
        F[lNFFT + k][1] = 0.0;
      }

      for (k = NFFT_2; k < NFFT - 1; k += 2) {
        Gsq = coeff1*(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>(l*l));
        F[lNFFT + k][0] = 0.0;
        F[lNFFT + k][1] = 0.0;
        F[lNFFT + k + 1][0] *= coeff2/(Gsq+coeff3);
        F[lNFFT + k + 1][1] = 0.0;
      }
      if (NFFT_2 % 2) {
        F[lNFFT + k][0] = 0.0;
        F[lNFFT + k][1] = 0.0;
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
      if (NFFT_2 % 2) {
        F[lNFFT + k][0] = 0.0;
        F[lNFFT + k][1] = 0.0;
      }

      for (k = NFFT_2; k < NFFT - 1; k += 2) {
        Gsq = coeff1*(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
        F[lNFFT + k][0] = 0.0;
        F[lNFFT + k][1] = 0.0;
        F[lNFFT + k + 1][0] *= coeff2/(Gsq+coeff3);
        F[lNFFT + k + 1][1] = 0.0;
      }
      if (NFFT_2 % 2) {
        F[lNFFT + k][0] = 0.0;
        F[lNFFT + k][1] = 0.0;
      }
    }

    F[0][0] = 0.0;

  return;
}

void TBulkTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForQx(fftw_complex* F) const {
  const int NFFT(fSteps), NFFT_2(fSteps/2);
  int lNFFT, l, k;
  double coeff1(1.5*fLatticeConstant/PI);

  for (l = 0; l < NFFT_2; l += 2) {
      lNFFT = l*NFFT;
      for (k = 0; k < NFFT_2 - 1; k += 2) {
        if (!k && !l)
          F[0][0] = 0.0;
        else
          F[lNFFT + k][0] *= coeff1*static_cast<double>(l)/(static_cast<double>(k*k) + 3.0*static_cast<double>(l*l));
      }
      if (NFFT_2 % 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(l)/(static_cast<double>(k*k) + 3.0*static_cast<double>(l*l));
      }

      for (k = NFFT_2; k < NFFT - 1; k += 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(l)/(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>(l*l));
      }
      if (NFFT_2 % 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(l)/(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>(l*l));
      }
    }

    for (l = NFFT_2; l < NFFT; l += 2) {
      lNFFT = l*NFFT;
      for (k = 0; k < NFFT_2 - 1; k += 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>(k*k) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      }
      if (NFFT_2 % 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>(k*k) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      }

      for (k = NFFT_2; k < NFFT - 1; k += 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      }
      if (NFFT_2 % 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      }
    }

    //intermediate rows

    for (l = 1; l < NFFT_2; l += 2) {
      lNFFT = l*NFFT;
      for (k = 0; k < NFFT_2 - 1; k += 2) {
        F[lNFFT + k + 1][0] *= coeff1*static_cast<double>(l)/(static_cast<double>((k+1)*(k+1)) + 3.0*static_cast<double>(l*l));
      }

      for (k = NFFT_2; k < NFFT - 1; k += 2) {
        F[lNFFT + k + 1][0] *= coeff1*static_cast<double>(l)/(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>(l*l));
      }
    }

    for (l = NFFT_2 + 1; l < NFFT; l += 2) {
      lNFFT = l*NFFT;
      for (k = 0; k < NFFT_2 - 1; k += 2) {
        F[lNFFT + k + 1][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>((k+1)*(k+1)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      }

      for (k = NFFT_2; k < NFFT - 1; k += 2) {
        F[lNFFT + k + 1][0] *= coeff1*static_cast<double>(l-NFFT)/(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      }
    }

    return;
}

void TBulkTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForQy(fftw_complex* F) const {
  const int NFFT(fSteps), NFFT_2(fSteps/2);
  int lNFFT, l, k;
  double coeff1(0.5*sqrt3*fLatticeConstant/PI);

  for (l = 0; l < NFFT_2; l += 2) {
      lNFFT = l*NFFT;
      for (k = 0; k < NFFT_2 - 1; k += 2) {
        if (!k && !l)
          F[0][0] = 0.0;
        else
          F[lNFFT + k][0] *= coeff1*static_cast<double>(k)/(static_cast<double>(k*k) + 3.0*static_cast<double>(l*l));
      }
      if (NFFT_2 % 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(k)/(static_cast<double>(k*k) + 3.0*static_cast<double>(l*l));
      }

      for (k = NFFT_2; k < NFFT - 1; k += 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(k-NFFT)/(static_cast<double>((k-NFFT)*(k-NFFT))+3.0*static_cast<double>(l*l));
      }
      if (NFFT_2 % 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(k-NFFT)/(static_cast<double>((k-NFFT)*(k-NFFT))+3.0*static_cast<double>(l*l));
      }
    }

    for (l = NFFT_2; l < NFFT; l += 2) {
      lNFFT = l*NFFT;
      for (k = 0; k < NFFT_2 - 1; k += 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(k)/(static_cast<double>(k*k)+3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      }
      if (NFFT_2 % 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(k)/(static_cast<double>(k*k)+3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      }

      for (k = NFFT_2; k < NFFT - 1; k += 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(k-NFFT)/(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      }
      if (NFFT_2 % 2) {
        F[lNFFT + k][0] *= coeff1*static_cast<double>(k-NFFT)/(static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      }
    }

    //intermediate rows

    for (l = 1; l < NFFT_2; l += 2) {
      lNFFT = l*NFFT;
      for (k = 0; k < NFFT_2 - 1; k += 2) {
        F[lNFFT + k + 1][0] *= coeff1*static_cast<double>(k+1)/(static_cast<double>((k+1)*(k+1)) + 3.0*static_cast<double>(l*l));
      }

      for (k = NFFT_2; k < NFFT - 1; k += 2) {
        F[lNFFT + k + 1][0] *= coeff1*static_cast<double>(k+1-NFFT)/(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>(l*l));
      }
    }

    for (l = NFFT_2 + 1; l < NFFT; l += 2) {
      lNFFT = l*NFFT;
      for (k = 0; k < NFFT_2 - 1; k += 2) {
        F[lNFFT + k + 1][0] *= coeff1*static_cast<double>(k+1)/(static_cast<double>((k+1)*(k+1)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
      }

      for (k = NFFT_2; k < NFFT - 1; k += 2) {
        F[lNFFT + k + 1][0] *= coeff1*static_cast<double>(k+1-NFFT)/(static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT)));
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
  double Hc2(getHc2(xi)), kappa(lambda/xi), Hc2_kappa(Hc2/kappa);
  double scaledB(field/Hc2_kappa);

  cout << "4pi/S = " << 2.0*kappa*scaledB << endl;

//  fLatticeConstant = sqrt(2.0*fluxQuantum/(scaledB*sqrt3));
  fLatticeConstant = sqrt(2.0*TWOPI/(kappa*scaledB*sqrt3));
//  fLatticeConstant = sqrt(4.0*PI/(kappa*field*sqrt3));
//  fLatticeConstant = sqrt(2.0*S/sqrt3);
//  double xisq_2_scaled(2.0/3.0*pow(xi*PI/latConstTr,2.0)), lambdasq_scaled(4.0/3.0*pow(lambda*PI/latConstTr,2.0));

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

  // ... now fill in the Fourier components (Abrikosov) if everything was okay above
  double Gsq, sign;
  int k, l, lNFFT;

// omp causes problems with the fftw_complex*... comment it out for the moment
//#pragma omp parallel default(shared) private(l,lNFFT_2,k,Gsq)
  {
//  #pragma omp sections nowait
    {
//      #pragma omp section
//      #pragma omp parallel for default(shared) private(l,lNFFT_2,k) schedule(dynamic)
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
          fAkMatrix[lNFFT + k][0] = sign*exp(-pi_4sqrt3*Gsq);
          fAkMatrix[lNFFT + k][1] = 0.0;
          fAkMatrix[lNFFT + k + 1][0] = 0.0;
          fAkMatrix[lNFFT + k + 1][1] = 0.0;
        }
        if (NFFT_2 % 2) {
          sign = -sign;
          Gsq = static_cast<double>(k*k) + 3.0*static_cast<double>(l*l);
          fAkMatrix[lNFFT + k][0] = sign*exp(-pi_4sqrt3*Gsq);
          fAkMatrix[lNFFT + k][1] = 0.0;
        }

     //   sign = -sign;

        for (k = NFFT_2; k < NFFT - 1; k += 2) {
          sign = -sign;
          Gsq = static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>(l*l);
          fAkMatrix[lNFFT + k][0] = sign*exp(-pi_4sqrt3*Gsq);
          fAkMatrix[lNFFT + k][1] = 0.0;
          fAkMatrix[lNFFT + k + 1][0] = 0.0;
          fAkMatrix[lNFFT + k + 1][1] = 0.0;
        }
        if (NFFT_2 % 2) {
          sign = -sign;
          Gsq = static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>(l*l);
          fAkMatrix[lNFFT + k][0] = sign*exp(-pi_4sqrt3*Gsq);
          fAkMatrix[lNFFT + k][1] = 0.0;
        }
      }

//      #pragma omp section
//      #pragma omp parallel for default(shared) private(l,lNFFT_2,k) schedule(dynamic)
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
          fAkMatrix[lNFFT + k][0] = sign*exp(-pi_4sqrt3*Gsq);
          fAkMatrix[lNFFT + k][1] = 0.0;
          fAkMatrix[lNFFT + k + 1][0] = 0.0;
          fAkMatrix[lNFFT + k + 1][1] = 0.0;
        }
        if (NFFT_2 % 2) {
          sign = -sign;
          Gsq = static_cast<double>(k*k) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
          fAkMatrix[lNFFT + k][0] = sign*exp(-pi_4sqrt3*Gsq);
          fAkMatrix[lNFFT + k][1] = 0.0;
        }

    //    sign = -sign;

        for (k = NFFT_2; k < NFFT - 1; k += 2) {
          sign = -sign;
          Gsq = static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
          fAkMatrix[lNFFT + k][0] = sign*exp(-pi_4sqrt3*Gsq);
          fAkMatrix[lNFFT + k][1] = 0.0;
          fAkMatrix[lNFFT + k + 1][0] = 0.0;
          fAkMatrix[lNFFT + k + 1][1] = 0.0;
        }
        if (NFFT_2 % 2) {
          sign = -sign;
          Gsq = static_cast<double>((k-NFFT)*(k-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
          fAkMatrix[lNFFT + k][0] = sign*exp(-pi_4sqrt3*Gsq);
          fAkMatrix[lNFFT + k][1] = 0.0;
        }
      }

      // intermediate rows
//      #pragma omp section
//      #pragma omp parallel for default(shared) private(l,lNFFT_2,k) schedule(dynamic)
      for (l = 1; l < NFFT_2; l += 2) {
        lNFFT = l*NFFT;
        for (k = 0; k < NFFT_2 - 1; k += 2) {
          Gsq = static_cast<double>((k + 1)*(k + 1)) + 3.0*static_cast<double>(l*l);
          fAkMatrix[lNFFT + k][0] = 0.0;
          fAkMatrix[lNFFT + k][1] = 0.0;
          fAkMatrix[lNFFT + k + 1][0] = exp(-pi_4sqrt3*Gsq);
          fAkMatrix[lNFFT + k + 1][1] = 0.0;
        }
        if (NFFT_2 % 2){
          fAkMatrix[lNFFT + k][0] = 0.0;
          fAkMatrix[lNFFT + k][1] = 0.0;
        }

        for (k = NFFT_2; k < NFFT - 1; k += 2) {    
          Gsq = static_cast<double>((k + 1-NFFT)*(k + 1-NFFT)) + 3.0*static_cast<double>(l*l);
          fAkMatrix[lNFFT + k][0] = 0.0;
          fAkMatrix[lNFFT + k][1] = 0.0;
          fAkMatrix[lNFFT + k + 1][0] = exp(-pi_4sqrt3*Gsq);
          fAkMatrix[lNFFT + k + 1][1] = 0.0;
        }
        if (NFFT_2 % 2){
          fAkMatrix[lNFFT + k][0] = 0.0;
          fAkMatrix[lNFFT + k][1] = 0.0;
        }
      }

//      #pragma omp section
//      #pragma omp parallel for default(shared) private(l,lNFFT_2,k) schedule(dynamic)
      for (l = NFFT_2 + 1; l < NFFT; l += 2) {
        lNFFT = l*NFFT;
        for (k = 0; k < NFFT_2 - 1; k += 2) {
          Gsq = static_cast<double>((k+1)*(k+1)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
          fAkMatrix[lNFFT + k][0] = 0.0;
          fAkMatrix[lNFFT + k][1] = 0.0;
          fAkMatrix[lNFFT + k + 1][0] = exp(-pi_4sqrt3*Gsq);
          fAkMatrix[lNFFT + k + 1][1] = 0.0;
        }
        if (NFFT_2 % 2){
          fAkMatrix[lNFFT + k][0] = 0.0;
          fAkMatrix[lNFFT + k][1] = 0.0;
        }

        for (k = NFFT_2; k < NFFT - 1; k += 2) {
          Gsq = static_cast<double>((k+1-NFFT)*(k+1-NFFT)) + 3.0*static_cast<double>((l-NFFT)*(l-NFFT));
          fAkMatrix[lNFFT + k][0] = 0.0;
          fAkMatrix[lNFFT + k][1] = 0.0;
          fAkMatrix[lNFFT + k + 1][0] = exp(-pi_4sqrt3*Gsq);
          fAkMatrix[lNFFT + k + 1][1] = 0.0;
        }
        if (NFFT_2 % 2){
          fAkMatrix[lNFFT + k][0] = 0.0;
          fAkMatrix[lNFFT + k][1] = 0.0;
        }
      }
    }  /* end of sections */

  }  /* end of parallel section */

  fAkMatrix[0][0] = 0.0;

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFT; l++) {
    fCheckAkConvergence[l] = fAkMatrix[l][0];
  }

  fSumAk = 0.0;
  for (l = 1; l < NFFTsq; l++) {
    fSumAk += fAkMatrix[l][0];
  }

  cout << "fSumAk = " << fSumAk << endl;

  // Do the Fourier transform to get omega(x,y) - Abrikosov

  fftw_execute(fFFTplanAkToOmega);

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsq; l++) {
    fOmegaMatrix[l] = fSumAk - fRealSpaceMatrix[l][0];
  }

  double sumOmega(0.0);
  for (l = 0; l < NFFTsq; l++) {
    sumOmega += fOmegaMatrix[l];
  }
  sumOmega /= static_cast<double>(NFFTsq);

  cout << "sumOmega = " << sumOmega << endl;

  CalculateIntermediateMatrices();

///////////// Check the Abrikosov solution

  for (l = 0; l < NFFTsq; l++) {
    if (!fOmegaMatrix[l]) {
      fAbrikosovCheck[l] = 0.0;
    } else {
      fAbrikosovCheck[l] = (fOmegaDiffXMatrix[l]*fOmegaDiffXMatrix[l]+fOmegaDiffYMatrix[l]*fOmegaDiffYMatrix[l])/(fOmegaMatrix[l]*fOmegaMatrix[l]) - (fOmegaDDiffXMatrix[l] + fOmegaDDiffYMatrix[l])/fOmegaMatrix[l];
    }
  }


  

  double denomQA;

#pragma omp parallel for default(shared) private(l,denomQA) schedule(dynamic)
  for (l = 0; l < NFFTsq; l++) {
    if (!fOmegaMatrix[l] || !l || (l == (NFFT+1)*NFFT_2)) {
      fQxMatrixA[l] = 0.0;
      fQyMatrixA[l] = 0.0;
    } else {
      denomQA = 2.0*kappa*fOmegaMatrix[l];
      fQxMatrixA[l] = fOmegaDiffYMatrix[l]/denomQA;
      fQyMatrixA[l] = -fOmegaDiffXMatrix[l]/denomQA;
    }
    fQxMatrix[l] = fQxMatrixA[l];
    fQyMatrix[l] = fQyMatrixA[l];
  }

  // initial B

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsq; l++) {
    fFFTout[l] = scaledB;
  }

  bool akConverged(false), bkConverged(false), akInitiallyConverged(false), firstBkCalculation(true);
  double coeff1, coeff2;

  int count(0);

  while (!akConverged || !bkConverged) {

//  if (count == 3) break;

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fOmegaSqMatrix[l] = fOmegaMatrix[l]*fOmegaMatrix[l];
    }

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fRealSpaceMatrix[l][0] = fOmegaMatrix[l]*fOmegaMatrix[l] - 2.0*fOmegaMatrix[l] + fOmegaMatrix[l]*(fQxMatrix[l]*fQxMatrix[l] + fQyMatrix[l]*fQyMatrix[l]) + (!fOmegaMatrix[l] ? 0.0 : (fOmegaDiffXMatrix[l]*fOmegaDiffXMatrix[l]+fOmegaDiffYMatrix[l]*fOmegaDiffYMatrix[l])/(4.0*kappa*kappa*fOmegaMatrix[l]));
      fRealSpaceMatrix[l][1] = 0.0;
    }

    fftw_execute(fFFTplanOmegaToAk);

    coeff1 = (4.0*kappa*kappa/static_cast<double>(NFFTsq));
    coeff2 = (2.0*kappa*kappa);

    ManipulateFourierCoefficients(fAkMatrix, coeff1, coeff2);
//
    fSumAk = 0.0;
    for (l = 1; l < NFFTsq; l++) {
      fSumAk += fAkMatrix[l][0];
    }

    cout << "fSumAk = " << fSumAk << endl;

    // Do the Fourier transform to get omega(x,y)

    fftw_execute(fFFTplanAkToOmega);

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fOmegaMatrix[l] = fSumAk - fRealSpaceMatrix[l][0];
    }

  CalculateIntermediateMatrices();

  sumOmega = 0.0;
  for (l = 0; l < NFFTsq; l++) {
    sumOmega += fOmegaMatrix[l];
  }
  sumOmega /= static_cast<double>(NFFTsq);
  cout << "sumOmega = " << sumOmega << endl;

//
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fOmegaSqMatrix[l] = fOmegaMatrix[l]*fOmegaMatrix[l];
    }

    fSumOmegaSq = 0.0;
    for (l = 0; l < NFFTsq; l++) {
      fSumOmegaSq += fOmegaSqMatrix[l];
    }

    double coeffSum(0.0);
    for (l = 0; l < NFFTsq; l++) {
      coeffSum += fOmegaMatrix[l] - fOmegaMatrix[l]*(fQxMatrix[l]*fQxMatrix[l] + fQyMatrix[l]*fQyMatrix[l]) - (!fOmegaMatrix[l] ? 0.0 : (fOmegaDiffXMatrix[l]*fOmegaDiffXMatrix[l]+fOmegaDiffYMatrix[l]*fOmegaDiffYMatrix[l])/(4.0*kappa*kappa*fOmegaMatrix[l]));
    }
    
    cout << "coeffSum = " << coeffSum << ", fSumOmegaSq = " << fSumOmegaSq << endl;

    for (l = 0; l < NFFTsq; l++) {
      fAkMatrix[l][0] *= coeffSum/fSumOmegaSq;
    }

    akConverged = true;

    for (l = 0; l < NFFT; l++) {
      if ((fabs(fCheckAkConvergence[l] - fAkMatrix[l][0])/fAkMatrix[l][0] > 0.025) && (fabs(fAkMatrix[l][0]) > 1.0E-4)) {
        cout << "old: " << fCheckAkConvergence[l] << ", new: " << fAkMatrix[l][0] << endl;
        akConverged = false;
        break;
      }
    }

    if (!akConverged) {
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
      for (l = 0; l < NFFT; l++) {
        fCheckAkConvergence[l] = fAkMatrix[l][0];
      }
    } else {
      akInitiallyConverged = true;
    }

    cout << "Ak Convergence: " << akConverged << endl;

    fSumAk = 0.0;
    for (l = 1; l < NFFTsq; l++) {
      fSumAk += fAkMatrix[l][0];
    }

    cout << "fSumAk = " << fSumAk << endl;

    // Do the Fourier transform to get omega(x,y)

    fftw_execute(fFFTplanAkToOmega);

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fOmegaMatrix[l] = fSumAk - fRealSpaceMatrix[l][0];
    }

    CalculateIntermediateMatrices();
    
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
      fRealSpaceMatrix[l][0] = fOmegaMatrix[l]*fFFTout[l] + fSumAk*(scaledB - fFFTout[l]) - fQxMatrix[l]*fOmegaDiffYMatrix[l] + fQyMatrix[l]*fOmegaDiffXMatrix[l];
      fRealSpaceMatrix[l][1] = 0.0;
    }

    fftw_execute(fFFTplanOmegaToBk);

    if (firstBkCalculation) {
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
      for (l = 0; l < NFFT; l++) {
        fCheckBkConvergence[l] = fBkMatrix[l][0];
      }
      firstBkCalculation = false;
      akConverged = false;
    }

    coeff1 = -2.0/static_cast<double>(NFFTsq);

    ManipulateFourierCoefficients(fBkMatrix, coeff1, fSumAk);

    fftw_execute(fFFTplanBkToBandQ);

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fFFTout[l] = scaledB + fRealSpaceMatrix[l][0];
    }

  // Ensure that the field is NOT negative
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsq; l++) {
    if (fFFTout[l] < 0.0)
      fFFTout[l] = 0.0;
  }
//  break;

    

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
    } else if (count == 1) {
      break;
    } else {
      bkConverged = false;
      count++;
    }

// I need a copy of Bk... store it to Ak since already tooooooooooooooooooo much memory is allocated
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fAkMatrix[l][0] = fBkMatrix[l][0];
      fAkMatrix[l][1] = fBkMatrix[l][1];
    }

    ManipulateFourierCoefficientsForQx(fBkMatrix);

    fftw_execute(fFFTplanBkToBandQ);

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fQxMatrix[l] = fQxMatrixA[l] - fRealSpaceMatrix[l][1];
    }

// Restore Bk... 
#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fBkMatrix[l][0] = fAkMatrix[l][0];
      fBkMatrix[l][1] = fAkMatrix[l][1];
    }

    ManipulateFourierCoefficientsForQy(fBkMatrix);

    fftw_execute(fFFTplanBkToBandQ);

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsq; l++) {
      fQyMatrix[l] = fQyMatrixA[l] + fRealSpaceMatrix[l][1];
    }
  }
  }

#pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsq; l++) {
    fFFTout[l] *= Hc2_kappa;
  }

  // Set the flag which shows that the calculation has been done

  fGridExists = true;
  return;

}

