/***************************************************************************

  TBulkTriVortexFieldCalc.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2010/02/01

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010 by Bastian M. Wojek                                *
 *   based upon:                                                           *
 *   Ernst Helmut Brandt, Phys. Rev. B 71 014521 (2005)                    *
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

#include "TFilmTriVortexFieldCalc.h"

#include <cstdlib>
#include <cmath>
#include <omp.h>
#include <iostream>
using namespace std;

#include "TMath.h"

#define PI 3.14159265358979323846f
#define TWOPI 6.28318530717958647692f

const float fluxQuantum(2.067833667e7f); // in this case this is Gauss times square nm

const float sqrt3(sqrt(3.0f));
const float pi_4sqrt3(0.25f*PI/sqrt(3.0f));


float getXi(const float &hc2) { // get xi given Hc2 in Gauss
  if (hc2)
    return sqrt(fluxQuantum/(TWOPI*hc2));
  else
    return 0.0f;
}

float getHc2(const float &xi) { // get Hc2 given xi in nm
  if (xi)
    return fluxQuantum/(TWOPI*xi*xi);
  else
    return 0.0f;
}

TFilmVortexFieldCalc::~TFilmVortexFieldCalc() {
  // if a wisdom file is used export the wisdom so it has not to be checked for the FFT-plan next time
  if (fUseWisdom) {
    FILE *wordsOfWisdomW;
    wordsOfWisdomW = fopen(fWisdom.c_str(), "w");
    if (wordsOfWisdomW == NULL) {
      cout << "TFilmVortexFieldCalc::~TFilmVortexFieldCalc(): Could not open file ... No wisdom is exported..." << endl;
    } else {
      fftwf_export_wisdom_to_file(wordsOfWisdomW);
      fclose(wordsOfWisdomW);
    }
  }

  // clean up

  fftwf_destroy_plan(fFFTplan);

  delete[] fFFTin; fFFTin = 0;

  for(unsigned int i(0); i<3; ++i){
    delete[] fBout[i]; fBout[i] = 0;
  }

  fBout.clear();
  fParam.clear();
  //fftwf_cleanup();
  //fftwf_cleanup_threads();
}

float TFilmVortexFieldCalc::GetBmin() const {
  if (fGridExists) {
    double min(fBout[2][0]), curfieldSq(0.0);
    unsigned int curindex(0);
    for (unsigned int l(0); l < fStepsZ; ++l) {
      for (unsigned int j(0); j < fSteps; ++j) {
        for (unsigned int k(0); k < fSteps/2; ++k) { // check only the first quadrant of B(x,y)
          curindex = l + fStepsZ*(j*fSteps + k);
          curfieldSq = fBout[0][curindex]*fBout[0][curindex] \
                     + fBout[1][curindex]*fBout[1][curindex] \
                     + fBout[2][curindex]*fBout[2][curindex];
          if (curfieldSq < min) {
            min = curfieldSq;
          }
        }
      }
    }
    return sqrt(min);
  } else {
    CalculateGrid();
    return GetBmin();
  }
}

float TFilmVortexFieldCalc::GetBmax() const {
  if (fGridExists)
    return fBout[2][0];
  else {
    CalculateGrid();
    return GetBmax();
  }
}


TFilmTriVortexNGLFieldCalc::TFilmTriVortexNGLFieldCalc(const string& wisdom, const unsigned int steps, const unsigned int stepsZ)
 : fLatticeConstant(0.0f), fKappa(0.0f), fSumOmegaSq(0.0f), fSumSum(0.0f), fFind3dSolution(false)
{
  cout << "TFilmTriVortexNGLFieldCalc::TFilmTriVortexNGLFieldCalc... ";

  fWisdom = wisdom;
  switch (stepsZ % 2) {
    case 0:
      fStepsZ = stepsZ;
      break;
    case 1:
      fStepsZ = stepsZ + 1;
      break;
    default:
      break;
  }
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
  int init_threads(fftwf_init_threads());
  if (init_threads)
    fftwf_plan_with_nthreads(2);
#endif /* HAVE_LIBFFTW3_THREADS */

  const unsigned int stepsSqStZ(fSteps*fSteps*fStepsZ);

  float* temp;

  for (unsigned int i(0); i < 3; ++i) {
    temp = new float[stepsSqStZ]; // Bx, By, Bz
    fBout.push_back(temp);

    temp = new float[stepsSqStZ]; // (grad omega)_(x,y,z)
    fOmegaDiffMatrix.push_back(temp);
  }
  temp = 0;

  fOmegaMatrix = new float[stepsSqStZ];  // |psi|^2

  fFFTin = new fftwf_complex[stepsSqStZ]; // aK matrix
  fBkMatrix = new fftwf_complex[stepsSqStZ]; // bK matrix
  fRealSpaceMatrix = new fftwf_complex[stepsSqStZ]; // fftw output matrix

  fPkMatrix = new fftwf_complex[stepsSqStZ]; // PK matrix

  fQMatrix = new fftwf_complex[stepsSqStZ];
  fQMatrixA = new fftwf_complex[fSteps*fSteps];

  fSumAkFFTin = new fftwf_complex[fStepsZ];
  fSumAk = new fftwf_complex[fStepsZ];

  fBkS = new float[fSteps*fSteps];

  fGstorage = new float[fStepsZ];

  fCheckAkConvergence = new float[fStepsZ*fSteps];
  fCheckBkConvergence = new float[fStepsZ*fSteps];

// Load wisdom from file if it exists and should be used

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

// create the FFT plans

  if (fUseWisdom) {
    cout << "use wisdom ... ";
    // use the first plan from the base class here - it will be destroyed by the base class destructor
    fFFTplan = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fFFTin, fRealSpaceMatrix, FFTW_BACKWARD, FFTW_EXHAUSTIVE);
    fFFTplanBkToBandQ = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fBkMatrix, fBkMatrix, FFTW_BACKWARD, FFTW_EXHAUSTIVE);
    fFFTplanOmegaToAk = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fRealSpaceMatrix, fFFTin, FFTW_FORWARD, FFTW_EXHAUSTIVE);
//    fFFTplanOmegaToBk = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fBkMatrix, fBkMatrix, FFTW_FORWARD, FFTW_EXHAUSTIVE);
    fFFTplanForSumAk = fftwf_plan_dft_1d(fStepsZ, fSumAkFFTin, fSumAk, FFTW_FORWARD, FFTW_EXHAUSTIVE);
    fFFTplanForPk1 = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fPkMatrix, fPkMatrix, FFTW_FORWARD, FFTW_EXHAUSTIVE);
    fFFTplanForPk2 = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fQMatrix, fQMatrix, FFTW_BACKWARD, FFTW_EXHAUSTIVE);
  }
  else {
    cout << "do not use wisdom ... ";
    // use the first plan from the base class here - it will be destroyed by the base class destructor
    fFFTplan = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fFFTin, fRealSpaceMatrix, FFTW_BACKWARD, FFTW_ESTIMATE);
    fFFTplanBkToBandQ = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fBkMatrix, fBkMatrix, FFTW_BACKWARD, FFTW_ESTIMATE);
    fFFTplanOmegaToAk = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fRealSpaceMatrix, fFFTin, FFTW_FORWARD, FFTW_ESTIMATE);
//    fFFTplanOmegaToBk = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fBkMatrix, fBkMatrix, FFTW_FORWARD, FFTW_ESTIMATE);
    fFFTplanForSumAk = fftwf_plan_dft_1d(fStepsZ, fSumAkFFTin, fSumAk, FFTW_FORWARD, FFTW_ESTIMATE);
    fFFTplanForPk1 = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fPkMatrix, fPkMatrix, FFTW_FORWARD, FFTW_ESTIMATE);
    fFFTplanForPk2 = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fQMatrix, fQMatrix, FFTW_BACKWARD, FFTW_ESTIMATE);
  }
  cout << "done" << endl;
}

TFilmTriVortexNGLFieldCalc::~TFilmTriVortexNGLFieldCalc() {

  // clean up
  fftwf_destroy_plan(fFFTplanBkToBandQ);
  fftwf_destroy_plan(fFFTplanOmegaToAk);
//  fftwf_destroy_plan(fFFTplanOmegaToBk);
  fftwf_destroy_plan(fFFTplanForSumAk);
  fftwf_destroy_plan(fFFTplanForPk1);
  fftwf_destroy_plan(fFFTplanForPk2);

  for (unsigned int i(0); i < 3; ++i) {
    delete[] fOmegaDiffMatrix[i]; fOmegaDiffMatrix[i] = 0;
  }
  fOmegaDiffMatrix.clear();

  delete[] fOmegaMatrix; fOmegaMatrix = 0;
  delete[] fBkMatrix; fBkMatrix = 0;
  delete[] fRealSpaceMatrix; fRealSpaceMatrix = 0;
  delete[] fPkMatrix; fPkMatrix = 0;
  delete[] fQMatrix; fQMatrix = 0;
  delete[] fQMatrixA; fQMatrixA = 0;
  delete[] fSumAkFFTin; fSumAkFFTin = 0;
  delete[] fSumAk; fSumAk = 0;
  delete[] fBkS; fBkS = 0;
  delete[] fGstorage; fGstorage = 0;

  delete[] fCheckAkConvergence; fCheckAkConvergence = 0;
  delete[] fCheckBkConvergence; fCheckBkConvergence = 0;
}

void TFilmTriVortexNGLFieldCalc::CalculateGatVortexCore() const {

  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFTsq(fSteps*fSteps);
  const int NFFTsqStZ(NFFTsq*fStepsZ);
  const int NFFTz(fStepsZ);
  const int NFFTz_2(fStepsZ/2);

  float *denom = new float[NFFTz];

  int i, j, k, l, index;

  // First save a copy of the real aK-matrix in the imaginary part of the bK-matrix
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBkMatrix[l][1] = fFFTin[l][0];
  }

  // sum_K aK Kx^2 cos(Kx*x + Ky*y) cos(Kz*z)
  // First multiply the aK with Kx^2, then call FFTW

  float coeffKx(4.0f/3.0f*pow(PI/fLatticeConstant, 2.0f));;

  // k = 0

  // even rows
  for (i = 0; i < NFFT; i += 2) {
    // j = 0
    fFFTin[fStepsZ*NFFT*i][0] = 0.0f;
    // j != 0
    for (j = 2; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j*j);
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeffKx*static_cast<float>((j - NFFT)*(j - NFFT));
    }
  }

  // odd rows
  for (i = 1; i < NFFT; i += 2) {
    for (j = 0; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>((j + 1)*(j + 1));
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT));
    }
  }

  // k != 0

  if (fFind3dSolution) {
    for (k = 1; k < NFFTz; ++k) {
      // even rows
      for (i = 0; i < NFFT; i += 2) {
        // j = 0
        fFFTin[k + NFFTz*NFFT*i][0] = 0.0f;
        // j != 0
        for (j = 2; j < NFFT_2; j += 2) {
          fFFTin[k + NFFTz*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j*j);
        }
        for (j = NFFT_2; j < NFFT; j += 2) {
          fFFTin[k + NFFTz*(j + NFFT*i)][0] *= coeffKx*static_cast<float>((j - NFFT)*(j - NFFT));
        }
      }

      // odd rows
      for (i = 1; i < NFFT; i += 2) {
        for (j = 0; j < NFFT_2; j += 2) {
          fFFTin[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>((j + 1)*(j + 1));
        }
        for (j = NFFT_2; j < NFFT; j += 2) {
          fFFTin[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT));
        }
      }
    }
  } // else do nothing since the other aK are already zero since the former aK manipulation

  fftwf_execute(fFFTplan);

  // Copy the results to the gradient matrix and restore the original aK-matrix
  for (k = 0; k < NFFTz; ++k) {
    denom[k] = fRealSpaceMatrix[k][0];
    fGstorage[k] = fRealSpaceMatrix[k][0]*fRealSpaceMatrix[k][0];
  }
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fFFTin[l][0] = fBkMatrix[l][1];
  }

  // sum_K aK Kx Ky cos(Kx*x + Ky*y) cos(Kz*z)
  // First multiply the aK with Kx*Ky, then call FFTW

  const float coeffKxKy = (4.0f/sqrt3*pow(PI/fLatticeConstant, 2.0f));

  // k = 0

  // even rows
  for (i = 0; i < NFFT_2; i += 2) {
    // j = 0
    fFFTin[fStepsZ*NFFT*i][0] = 0.0f;
    // j != 0
    for (j = 2; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeffKxKy*static_cast<float>(j*i);
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeffKxKy*static_cast<float>((j - NFFT)*i);
    }
  }
  for (i = NFFT_2; i < NFFT; i += 2) {
    // j = 0
    fFFTin[fStepsZ*NFFT*i][0] = 0.0f;
    // j != 0
    for (j = 2; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeffKxKy*static_cast<float>(j*(i - NFFT));
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeffKxKy*static_cast<float>((j - NFFT)*(i - NFFT));
    }
  }

  // odd rows
  for (i = 1; i < NFFT_2; i += 2) {
    for (j = 0; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKxKy*static_cast<float>((j + 1)*i);
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKxKy*static_cast<float>((j + 1 - NFFT)*i);
    }
  }
  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    for (j = 0; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKxKy*static_cast<float>((j + 1)*(i - NFFT));
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKxKy*static_cast<float>((j + 1 - NFFT)*(i - NFFT));
    }
  }

  // k != 0

  if (fFind3dSolution) {
    for (k = 1; k < NFFTz; ++k) {
      // even rows
      for (i = 0; i < NFFT_2; i += 2) {
        // j = 0
        fFFTin[k + fStepsZ*NFFT*i][0] = 0.0f;
        // j != 0
        for (j = 2; j < NFFT_2; j += 2) {
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeffKxKy*static_cast<float>(j*i);
        }
        for (j = NFFT_2; j < NFFT; j += 2) {
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeffKxKy*static_cast<float>((j - NFFT)*i);
        }
      }
      for (i = NFFT_2; i < NFFT; i += 2) {
        // j = 0
        fFFTin[k + fStepsZ*NFFT*i][0] = 0.0f;
        // j != 0
        for (j = 2; j < NFFT_2; j += 2) {
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeffKxKy*static_cast<float>(j*(i - NFFT));
        }
        for (j = NFFT_2; j < NFFT; j += 2) {
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeffKxKy*static_cast<float>((j - NFFT)*(i - NFFT));
        }
      }

      // odd rows
      for (i = 1; i < NFFT_2; i += 2) {
        for (j = 0; j < NFFT_2; j += 2) {
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKxKy*static_cast<float>((j + 1)*i);
        }
        for (j = NFFT_2; j < NFFT; j += 2) {
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKxKy*static_cast<float>((j + 1 - NFFT)*i);
        }
      }
      for (i = NFFT_2 + 1; i < NFFT; i += 2) {
        for (j = 0; j < NFFT_2; j += 2) {
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKxKy*static_cast<float>((j + 1)*(i - NFFT));
        }
        for (j = NFFT_2; j < NFFT; j += 2) {
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKxKy*static_cast<float>((j + 1 - NFFT)*(i - NFFT));
        }
      }
    }
  } // else do nothing since the other aK are already zero since the former aK manipulation

  fftwf_execute(fFFTplan);

  // Copy the results to the gradient matrix and restore the original aK-matrix
  for (k = 0; k < NFFTz; ++k) {
    fGstorage[k] += fRealSpaceMatrix[k][0]*fRealSpaceMatrix[k][0];
  }
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fFFTin[l][0] = fBkMatrix[l][1];
  }

  // sum_K aK Kx Kz sin(Kx*x + Ky*y) sin(Kz*z)
  // First multiply the aK with Kx*Kz, then call FFTW

  const float coeffKxKz(TWOPI*PI/(sqrt3*fLatticeConstant*fThickness));

  // k = 0

  // even rows
  for (i = 0; i < NFFT_2; i += 2) {
    for (j = 0; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0f;
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0f;
    }
  }
  for (i = NFFT_2; i < NFFT; i += 2) {
    for (j = 0; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0f;
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0f;
    }
  }

  // odd rows
  for (i = 1; i < NFFT_2; i += 2) {
    for (j = 0; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
    }
  }
  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    for (j = 0; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
    }
  }

  // k != 0

  if (fFind3dSolution) {
    for (k = 1; k < NFFTz_2; ++k) {
      // even rows
      for (i = 0; i < NFFT; i += 2) {
        // j = 0
        fFFTin[k + fStepsZ*NFFT*i][0] = 0.0f;
        // j != 0
        for (j = 2; j < NFFT_2; j += 2) {
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeffKxKz*static_cast<float>(j*k);
        }
        for (j = NFFT_2; j < NFFT; j += 2) {
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeffKxKz*static_cast<float>((j - NFFT)*k);
        }
      }

      // odd rows
      for (i = 1; i < NFFT; i += 2) {
        for (j = 0; j < NFFT_2; j += 2) {
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKxKz*static_cast<float>((j + 1)*k);
        }
        for (j = NFFT_2; j < NFFT; j += 2) {
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKxKz*static_cast<float>((j + 1 - NFFT)*k);
        }
      }
    }
    for (k = NFFTz_2; k < NFFTz; ++k) {
      // even rows
      for (i = 0; i < NFFT; i += 2) {
        // j = 0
        fFFTin[k + fStepsZ*NFFT*i][0] = 0.0f;
        // j != 0
        for (j = 2; j < NFFT_2; j += 2) {
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeffKxKz*static_cast<float>(j*(k - NFFTz));
        }
        for (j = NFFT_2; j < NFFT; j += 2) {
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeffKxKz*static_cast<float>((j - NFFT)*(k - NFFTz));
        }
      }

      // odd rows
      for (i = 1; i < NFFT; i += 2) {
        for (j = 0; j < NFFT_2; j += 2) {
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKxKz*static_cast<float>((j + 1)*(k - NFFTz));
        }
        for (j = NFFT_2; j < NFFT; j += 2) {
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKxKz*static_cast<float>((j + 1 - NFFT)*(k - NFFTz));
        }
      }
    }
  } // else do nothing since the other aK are already zero since the former aK manipulation

  fftwf_execute(fFFTplan);

  // Copy the results to the gradient matrix and restore the original aK-matrix
  for (k = 0; k < NFFTz; ++k) {
    fGstorage[k] += fRealSpaceMatrix[k][1]*fRealSpaceMatrix[k][1];
  }
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fFFTin[l][0] = fBkMatrix[l][1];
  }

  // Final evaluation
  for (k = 0; k < NFFTz; ++k) {
    fGstorage[k] /= 2.0f*denom[k]*fKappa*fKappa;
  }

  delete[] denom; denom = 0;

  return;
}

void TFilmTriVortexNGLFieldCalc::CalculateGradient() const {

  // Calculate the gradient of omega stored in a vector<float*> = (dw/dx, dw/dy, dw/dz)

  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFTsq(fSteps*fSteps);
  const int NFFTsqStZ(NFFTsq*fStepsZ);
  const int NFFTsqStZ_2(NFFTsqStZ/2);
  const int NFFTz(fStepsZ);
  const int NFFTz_2(fStepsZ/2);

  int i, j, k, l, index;

  // Take the derivative of the Fourier sum of omega
  // This is going to be a bit lengthy...

  // First save a copy of the real aK-matrix in the imaginary part of the bK-matrix
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBkMatrix[l][1] = fFFTin[l][0];
  }

  // dw/dx = sum_K aK Kx sin(Kx*x + Ky*y) cos(Kz*z)
  // First multiply the aK with Kx, then call FFTW

  const float coeffKx(TWOPI/(sqrt3*fLatticeConstant));

  // k = 0

  // even rows
  for (i = 0; i < NFFT; i += 2) {
    // j = 0
    fFFTin[fStepsZ*NFFT*i][0] = 0.0f;
    // j != 0
    for (j = 2; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j);
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j - NFFT);
    }
  }

  // odd rows
  for (i = 1; i < NFFT; i += 2) {
    for (j = 0; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>(j + 1);
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>(j + 1 - NFFT);
    }
  }

  // k != 0

  if (fFind3dSolution) {
    for (k = 1; k < NFFTz_2; ++k) {
      // even rows
      for (i = 0; i < NFFT; i += 2) {
        // j = 0
        fFFTin[k + NFFTz*NFFT*i][0] = 0.0f;
        // j != 0
        for (j = 2; j < NFFT_2; j += 2) {
          fFFTin[k + NFFTz*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j);
        }
        for (j = NFFT_2; j < NFFT; j += 2) {
          fFFTin[k + NFFTz*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j - NFFT);
        }
      }

      // odd rows
      for (i = 1; i < NFFT; i += 2) {
        for (j = 0; j < NFFT_2; j += 2) {
          fFFTin[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>(j + 1);
        }
        for (j = NFFT_2; j < NFFT; j += 2) {
          fFFTin[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>(j + 1 - NFFT);
        }
      }
    }
  } // else do nothing since the other aK are already zero since the former aK manipulation

  fftwf_execute(fFFTplan);

  // Copy the results to the gradient matrix and restore the original aK-matrix
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fOmegaDiffMatrix[0][l] = fRealSpaceMatrix[l][1];
    fFFTin[l][0] = fBkMatrix[l][1];
  }

  // dw/dy = sum_K aK Ky sin(Kx*x + Ky*y) cos(Kz*z)
  // First multiply the aK with Ky, then call FFTW

  const float coeffKy(TWOPI/fLatticeConstant);
  float ky;

  // k = 0

  // even rows
  // i = 0
  for (j = 0; j < NFFT; j += 2) {
    fFFTin[NFFTz*j][0] = 0.0f;
  }
  // i != 0
  for (i = 2; i < NFFT_2; i += 2) {
    ky = coeffKy*static_cast<float>(i);
    for (j = 0; j < NFFT; j += 2) {
      fFFTin[NFFTz*(j + NFFT*i)][0] *= ky;
    }
  }
  for (i = NFFT_2; i < NFFT; i += 2) {
    ky = coeffKy*static_cast<float>(i - NFFT);
    for (j = 0; j < NFFT; j += 2) {
      fFFTin[NFFTz*(j + NFFT*i)][0] *= ky;
    }
  }

  // odd rows
  for (i = 1; i < NFFT_2; i += 2) {
    ky = coeffKy*static_cast<float>(i);
    for (j = 0; j < NFFT; j += 2) {
      fFFTin[NFFTz*(j + 1 + NFFT*i)][0] *= ky;
    }
  }
  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    ky = coeffKy*static_cast<float>(i - NFFT);
    for (j = 0; j < NFFT; j += 2) {
      fFFTin[NFFTz*(j + 1 + NFFT*i)][0] *= ky;
    }
  }

  // k != 0

  if (fFind3dSolution) {
    for (k = 1; k < NFFTz_2; ++k) {
      // even rows
      // i = 0
      for (j = 0; j < NFFT; j += 2) {
        fFFTin[k + NFFTz*j][0] = 0.0f;
      }
      // i != 0
      for (i = 2; i < NFFT_2; i += 2) {
        ky = coeffKy*static_cast<float>(i);
        for (j = 0; j < NFFT; j += 2) {
          fFFTin[k + NFFTz*(j + NFFT*i)][0] *= ky;
        }
      }
      for (i = NFFT_2; i < NFFT; i += 2) {
        ky = coeffKy*static_cast<float>(i - NFFT);
        for (j = 0; j < NFFT; j += 2) {
          fFFTin[k + NFFTz*(j + NFFT*i)][0] *= ky;
        }
      }
      // odd rows
      for (i = 1; i < NFFT_2; i += 2) {
        ky = coeffKy*static_cast<float>(i);
        for (j = 0; j < NFFT; j += 2) {
          fFFTin[k + NFFTz*(j + 1 + NFFT*i)][0] *= ky;
        }
      }
      for (i = NFFT_2 + 1; i < NFFT; i += 2) {
        ky = coeffKy*static_cast<float>(i - NFFT);
        for (j = 0; j < NFFT; j += 2) {
          fFFTin[k + NFFTz*(j + 1 + NFFT*i)][0] *= ky;
        }
      }
    }
  } // else do nothing since the other aK are already zero since the former aK manipulation

  fftwf_execute(fFFTplan);

  // Copy the results to the gradient matrix and restore the original aK-matrix
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fOmegaDiffMatrix[1][l] = fRealSpaceMatrix[l][1];
    fFFTin[l][0] = fBkMatrix[l][1];
  }

  // dw/dz = {sum_K aK Kz cos(Kx*x + Ky*y) sin(Kz*z)} - {sum_K aK Kz sin(Kz*z)}
  // First multiply the aK with Kz, then do the 1D and 3D FFTs

  const float coeffKz(TWOPI/fThickness);

  if (fFind3dSolution) {

    float kz;
    for (k = 0; k < NFFTz_2; ++k) {
      kz = coeffKz*static_cast<float>(k);
      // even rows
      for (i = 0; i < NFFT; i += 2) {
        for (j = 0; j < NFFT; j += 2) {
          fFFTin[k + NFFTz*(j + NFFT*i)][0] *= kz;
        }
      }
      // odd rows
      for (i = 1; i < NFFT; i += 2) {
        for (j = 0; j < NFFT; j += 2) {
          fFFTin[k + NFFTz*(j + 1 + NFFT*i)][0] *= kz;
        }
      }
    }
    /* Comment out the negativ k since the coefficients should be all zero'd before
    for (k = NFFTz_2; k < NFFTz; ++k) {
      kz = coeffKz*static_cast<float>(k - NFFTz);
      // even rows
      for (i = 0; i < NFFT; i += 2) {
        for (j = 0; j < NFFT; j += 2) {
          fFFTin[k + NFFTz*(j + NFFT*i)][0] *= kz;
        }
      }
      // odd rows
      for (i = 1; i < NFFT; i += 2) {
        for (j = 0; j < NFFT; j += 2) {
          fFFTin[k + NFFTz*(j + 1 + NFFT*i)][0] *= kz;
        }
      }
    }
    */

    // 1D transform - first sum up the coefficients in the other two dimensions and then call FFTW
    for (k = 0; k < NFFTz_2; ++k) {
      fSumAkFFTin[k][0] = 0.0f;
      for (index = 0; index < NFFTsq; ++index) {
        fSumAkFFTin[k][0] += fFFTin[k + NFFTz*index][0];
      }
      fSumAkFFTin[k][1] = 0.0f;
    }

    fftwf_execute(fFFTplanForSumAk);

    // 3D transform
    fftwf_execute(fFFTplan);

    // Copy the results to the gradient matrix - with the 1D-FORWARD-transform we have to _add_ fSumAk
    for (k = 0; k < NFFTz; ++k) {
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      for (index = 0; index < NFFTsq; ++index) {
        fOmegaDiffMatrix[2][k + NFFTz*index] = fRealSpaceMatrix[k + NFFTz*index][1] + fSumAk[k][1];
      }
    }

    // Restore the original aK-matrix
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsqStZ; ++l) {
      fFFTin[l][0] = fBkMatrix[l][1];
      fBkMatrix[l][1] = 0.0f;
    }
  } else {
    // For the 2D solution, dw/dz = 0
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsqStZ; ++l) {
      fOmegaDiffMatrix[2][l] = 0.0f;
      fBkMatrix[l][1] = 0.0f;
    }
  }
  /*
  // Ensure that omega and the gradient at the vortex-core positions are zero
  for (k = 0; k < NFFTz; ++k) {
    fOmegaMatrix[k] = 0.0f;
    fOmegaMatrix[k + NFFTz*(NFFT+1)*NFFT_2] = 0.0f;
    for (i = 0; i < NFFT; ++i) {
      // j = 0
      fOmegaDiffMatrix[0][k + NFFTz*NFFT*i] = 0.0f;
      // j = NFFT_2
      fOmegaDiffMatrix[0][k + NFFTz*(NFFT_2 + NFFT*i)] = 0.0f;
    }
    for (j = 0; j < NFFT; ++j) {
      // i = 0
      fOmegaDiffMatrix[1][k + NFFTz*j] = 0.0f;
      // i = NFFT_2
      fOmegaDiffMatrix[1][k + NFFTz*(j + NFFT*NFFT_2)] = 0.0f;
    }
    fOmegaDiffMatrix[2][k] = 0.0f;
    fOmegaDiffMatrix[2][k + NFFTz*(NFFT+1)*NFFT_2] = 0.0f;
  }
  for (index = 0; index < NFFTsq; ++index) {
    // k = 0
    fOmegaDiffMatrix[2][index] = 0.0f;
    // k = NFFTz_2
    fOmegaDiffMatrix[2][NFFTz_2 + index] = 0.0f;
  }
  */
  return;


/* Numerical differential quotient approximation
  const float denomYInv(static_cast<float>(NFFT)/(2.0f*fLatticeConstant));
  const float denomXInv(denomYInv/sqrt3);
  const float denomZInv(static_cast<float>(fStepsZ)/(2.0f*fThickness));

  // Ensure that omega at the vortex-core positions is zero
  for (unsigned int k(0); k < fStepsZ; ++k) {
    fOmegaMatrix[k] = 0.0f;
    fOmegaMatrix[k + fStepsZ*(NFFT+1)*NFFT_2] = 0.0f;
  }

  int i,j,k,index;

// //  Ensure that omega is NOT negative
//   #pragma omp parallel for default(shared) private(index) schedule(dynamic)
//   for (index = 0; index < NFFTsqStZ; ++index) {
//     if (fOmegaMatrix[index] < 0.0f) {
// //      cout << "Omega negative for index " << index << ", value: " << fOmegaMatrix[index] << endl;
//       fOmegaMatrix[index] = 0.0f;
//     }
//     if (fOmegaMatrix[index] > 1.0f) {
// //      cout << "Omega negative for index " << i << ", value: " << fOmegaMatrix[i] << endl;
//       fOmegaMatrix[index] = 1.0f;
//     }
//   }

  // Calculate the gradient first for Kz = 0 and for the rest only if we search the 3D solution

  // dw/dx
  k = 0;
  // j = 0, first column
  #pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = 0; i < NFFT; ++i) {
    fOmegaDiffMatrix[0][k+fStepsZ*(NFFT*i)] = (fOmegaMatrix[k+fStepsZ*(1 + NFFT*i)]-fOmegaMatrix[k+fStepsZ*(NFFT - 1 + NFFT*i)])*denomXInv;
  }
  // j = NFFT-1, last column
  #pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = 0; i < NFFT; ++i) {
    fOmegaDiffMatrix[0][k+fStepsZ*(NFFT - 1 + NFFT*i)] = (fOmegaMatrix[k+fStepsZ*(NFFT*i)]-fOmegaMatrix[k+fStepsZ*(NFFT - 2 + NFFT*i)])*denomXInv;
  }
  // the intermediate columns
  for (j = 1; j < NFFT-1; ++j) {
    #pragma omp parallel for default(shared) private(i) schedule(dynamic)
    for (i = 0; i < NFFT; ++i) {
      fOmegaDiffMatrix[0][k+fStepsZ*(j + NFFT*i)] = (fOmegaMatrix[k+fStepsZ*(j + 1 + NFFT*i)]-fOmegaMatrix[k+fStepsZ*(j - 1 + NFFT*i)])*denomXInv;
    }
  }

  // dw/dy
  for (j = 0; j < NFFT; ++j) {
    // i = 0, first row
    fOmegaDiffMatrix[1][k+fStepsZ*j] = (fOmegaMatrix[k+fStepsZ*(j + NFFT)]-fOmegaMatrix[k+fStepsZ*(j + NFFT*(NFFT - 1))])*denomYInv;
    // i = NFFT - 1, last row
    fOmegaDiffMatrix[1][k+fStepsZ*(j + NFFT*(NFFT - 1))] = (fOmegaMatrix[k+fStepsZ*j]-fOmegaMatrix[k+fStepsZ*(j + NFFT*(NFFT - 2))])*denomYInv;
    // the intermediate rows
    #pragma omp parallel for default(shared) private(i) schedule(dynamic)
    for (i = 1; i < NFFT - 1; ++i) {
      fOmegaDiffMatrix[1][k+fStepsZ*(j + NFFT*i)] = (fOmegaMatrix[k+fStepsZ*(j + NFFT*(i+1))]-fOmegaMatrix[k+fStepsZ*(j + NFFT*(i-1))])*denomYInv;
    }
  }

  if (fFind3dSolution) {
    for (k = 1; k < fStepsZ; ++k) {
//      cout << "k = " << k << endl;
      // dw/dx
      // j = 0, first column
      #pragma omp parallel for default(shared) private(i) schedule(dynamic)
      for (i = 0; i < NFFT; ++i) {
        fOmegaDiffMatrix[0][k+fStepsZ*(NFFT*i)] = (fOmegaMatrix[k+fStepsZ*(1 + NFFT*i)]-fOmegaMatrix[k+fStepsZ*(NFFT - 1 + NFFT*i)])*denomXInv;
      }
      // j = NFFT-1, last column
      #pragma omp parallel for default(shared) private(i) schedule(dynamic)
      for (i = 0; i < NFFT; ++i) {
        fOmegaDiffMatrix[0][k+fStepsZ*(NFFT - 1 + NFFT*i)] = \
                           (fOmegaMatrix[k+fStepsZ*(NFFT*i)]-fOmegaMatrix[k+fStepsZ*(NFFT - 2 + NFFT*i)])*denomXInv;
      }
      // the intermediate columns
      for (j = 1; j < NFFT-1; ++j) {
        #pragma omp parallel for default(shared) private(i) schedule(dynamic)
        for (i = 0; i < NFFT; ++i) {
          fOmegaDiffMatrix[0][k+fStepsZ*(j + NFFT*i)] = \
                             (fOmegaMatrix[k+fStepsZ*(j + 1 + NFFT*i)]-fOmegaMatrix[k+fStepsZ*(j - 1 + NFFT*i)])*denomXInv;
        }
      }

      // dw/dy
      for (j = 0; j < NFFT; ++j) {
        // i = 0, first row
        fOmegaDiffMatrix[1][k+fStepsZ*j] = (fOmegaMatrix[k+fStepsZ*(j + NFFT)]-fOmegaMatrix[k+fStepsZ*(j + NFFT*(NFFT - 1))])*denomYInv;
        // i = NFFT - 1, last row
        fOmegaDiffMatrix[1][k+fStepsZ*(j + NFFT*(NFFT - 1))] = \
                           (fOmegaMatrix[k+fStepsZ*j]-fOmegaMatrix[k+fStepsZ*(j + NFFT*(NFFT - 2))])*denomYInv;
        // the intermediate rows
        #pragma omp parallel for default(shared) private(i) schedule(dynamic)
        for (i = 1; i < NFFT - 1; ++i) {
          fOmegaDiffMatrix[1][k+fStepsZ*(j + NFFT*i)] = \
                            (fOmegaMatrix[k+fStepsZ*(j + NFFT*(i+1))]-fOmegaMatrix[k+fStepsZ*(j + NFFT*(i-1))])*denomYInv;
        }
      }
    }
  } else {
    for (k = 1; k < fStepsZ; ++k) {
      for (j = 0; j < NFFT; ++j) {
        #pragma omp parallel for default(shared) private(i, index) schedule(dynamic)
        for (i = 0; i < NFFT; ++i) {
          index = fStepsZ*(j + NFFT*i);
          fOmegaDiffMatrix[0][k + index] = fOmegaDiffMatrix[0][index]; // copy dw/dx
          fOmegaDiffMatrix[1][k + index] = fOmegaDiffMatrix[1][index]; // copy dw/dy
        }
      }
    }
  }

  // dw/dz
  if (fFind3dSolution) {
    for (j = 0; j < NFFT; ++j) {
//      #pragma omp parallel for default(shared) private(i) schedule(dynamic)
      for (i = 0; i < NFFT; ++i) {
//        cout << fOmegaMatrix[fStepsZ*(j + NFFT*i)] << " - " << fOmegaMatrix[fStepsZ - 2 + fStepsZ*(j + NFFT*i)] << " = " \
//             << (fOmegaMatrix[fStepsZ*(j + NFFT*i)] - fOmegaMatrix[fStepsZ - 2 + fStepsZ*(j + NFFT*i)]) << endl;
        // k = 0, bottom of the film (or center... who knows?!)
        fOmegaDiffMatrix[2][fStepsZ*(j + NFFT*i)] = 0.0f;
         //(fOmegaMatrix[1 + fStepsZ*(j + NFFT*i)] - fOmegaMatrix[fStepsZ - 1 + fStepsZ*(j + NFFT*i)])*denomZInv;
        // k = fStepsZ/2, center of the film (or vacuum interface... )
        fOmegaDiffMatrix[2][fStepsZ/2 + fStepsZ*(j + NFFT*i)] = 0.0f;
        // k = fStepsZ - 1
        fOmegaDiffMatrix[2][fStepsZ - 1 + fStepsZ*(j + NFFT*i)] = \
         (fOmegaMatrix[fStepsZ*(j + NFFT*i)] - fOmegaMatrix[fStepsZ - 2 + fStepsZ*(j + NFFT*i)])*denomZInv;
      }
    }

    // intermediate k
    for (k = 1; k < fStepsZ/2; ++k) {
      for (j = 0; j < NFFT; ++j) {
//        #pragma omp parallel for default(shared) private(i) schedule(dynamic)
        for (i = 0; i < NFFT; ++i) {
          fOmegaDiffMatrix[2][k+fStepsZ*(j + NFFT*i)] = \
                             (fOmegaMatrix[k + 1 + fStepsZ*(j + NFFT*i)]-fOmegaMatrix[k - 1 + fStepsZ*(j + NFFT*i)])*denomZInv;
        }
      }
    }
    for (k = fStepsZ/2 + 1; k < fStepsZ - 1; ++k) {
      for (j = 0; j < NFFT; ++j) {
        #pragma omp parallel for default(shared) private(i) schedule(dynamic)
        for (i = 0; i < NFFT; ++i) {
          fOmegaDiffMatrix[2][k+fStepsZ*(j + NFFT*i)] = \
                             (fOmegaMatrix[k + 1 + fStepsZ*(j + NFFT*i)]-fOmegaMatrix[k - 1 + fStepsZ*(j + NFFT*i)])*denomZInv;
        }
      }
    }
  } else {
    #pragma omp parallel for default(shared) private(index) schedule(dynamic)
    for (index = 0; index < NFFTsqStZ; ++index) {
      fOmegaDiffMatrix[2][index] = 0.0f;
    }
  }

  // Ensure that the derivatives at the vortex-core positions are zero
  for (k = 0; k < fStepsZ; ++k) {
    fOmegaDiffMatrix[0][0] = 0.0f;
    fOmegaDiffMatrix[0][k + fStepsZ*(NFFT+1)*NFFT_2] = 0.0f;
    fOmegaDiffMatrix[1][0] = 0.0f;
    fOmegaDiffMatrix[1][k + fStepsZ*(NFFT+1)*NFFT_2] = 0.0f;
    fOmegaDiffMatrix[2][0] = 0.0f;
    fOmegaDiffMatrix[2][k + fStepsZ*(NFFT+1)*NFFT_2] = 0.0f;
  }
  return;
  */
}

void TFilmTriVortexNGLFieldCalc::FillAbrikosovCoefficients(const float reducedField) const {
  const int NFFT(fSteps), NFFTsq(fSteps*fSteps), NFFT_2(NFFT/2), NFFTz_2(fStepsZ/2), NFFTz(fStepsZ);

  float coeff(1.0f-reducedField);
  float Gsq, sign, ii;
  int i,j,k,index;

//  k = 0;
  for (i = 0; i < NFFT_2; i += 2) {
    if (!(i % 4)) {
      sign = 1.0f;
    } else {
      sign = -1.0f;
    }
    ii = 3.0f*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      sign = -sign;
      Gsq = static_cast<float>(j*j) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = sign*coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      sign = -sign;
      Gsq = static_cast<float>((j-NFFT)*(j-NFFT)) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = sign*coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }
  }

  for (i = NFFT_2; i < NFFT; i += 2) {
    if (!(i % 4)) {
      sign = 1.0f;
    } else {
      sign = -1.0f;
    }
    ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      sign = -sign;
      Gsq = static_cast<float>(j*j) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = sign*coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      sign = -sign;
      Gsq = static_cast<float>((j-NFFT)*(j-NFFT)) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = sign*coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }
  }

  // intermediate rows
  for (i = 1; i < NFFT_2; i += 2) {
    ii = 3.0f*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      Gsq = static_cast<float>((j + 1)*(j + 1)) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      Gsq = static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }
  }

  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      Gsq = static_cast<float>((j+1)*(j+1)) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      Gsq = static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }
  }

  fFFTin[0][0] = 0.0f;

  for (k = 1; k < NFFTz; ++k) {
    #pragma omp parallel for default(shared) private(index) schedule(dynamic)
    for (index = 0; index < NFFTsq; ++index) {
      fFFTin[k + NFFTz*index][0] = 0.0f;
      fFFTin[k + NFFTz*index][1] = 0.0f;
    }
  }

  return;
}

void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsA() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2), NFFTsqStZ(fSteps*fSteps*fStepsZ), NFFTsq(fSteps*fSteps);

  // Divide EHB's coefficient no2 by two since we are considering "the full 2D reciprocal lattice", not only the half space!
  const float symCorr(0.5f);
  const float coeff1(4.0f/3.0f*pow(PI/fLatticeConstant,2.0f));
  const float coeff3(4.0f*fKappa*fKappa);
  const float coeff2(symCorr*coeff3/static_cast<float>(NFFTsqStZ));

  const float coeff4(4.0f*pow(PI/fThickness,2.0f));
  const float coeff5(2.0f*coeff2);

  int i, j, k, l, index, index2;
  float Gsq, ii, kk;

  // k = 0;
  for (i = 0; i < NFFT_2; i += 2) {
    ii = 3.0f*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      Gsq = coeff1*(static_cast<float>(j*j) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      Gsq = coeff1*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }
  }

  for (i = NFFT_2; i < NFFT; i += 2) {
    ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      Gsq = coeff1*(static_cast<float>(j*j) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      Gsq = coeff1*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }
  }

  //intermediate rows

  for (i = 1; i < NFFT_2; i += 2) {
    ii = 3.0f*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      Gsq = coeff1*(static_cast<float>((j+1)*(j+1)) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      Gsq = coeff1*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }
  }

  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      Gsq = coeff1*(static_cast<float>((j+1)*(j+1)) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      Gsq = coeff1*(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0f;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0f;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
    }
  }

  fFFTin[0][0] = 0.0f;

  // k != 0;
  if (fFind3dSolution) {
    for (k = 1; k < NFFTz_2; ++k) {
      kk = coeff4*static_cast<float>(k*k);
      for (i = 0; i < NFFT_2; i += 2) {
        ii = 3.0f*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>(j*j) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }
      }

      for (i = NFFT_2; i < NFFT; i += 2) {
        ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>(j*j) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }
      }

      //intermediate rows

      for (i = 1; i < NFFT_2; i += 2) {
        ii = 3.0f*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>((j+1)*(j+1)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }
      }

      for (i = NFFT_2 + 1; i < NFFT; i += 2) {
        ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>((j+1)*(j+1)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }
      }
      fFFTin[k][0] = 0.0f;
    }
    /* Comment out the negative Kz - setting them to zero instead to stay closer to EHB's solution
    for (k = NFFTz_2; k < fStepsZ; ++k) {
      kk = coeff4*static_cast<float>((k - NFFTz)*(k - NFFTz));
      for (i = 0; i < NFFT_2; i += 2) {
        ii = 3.0f*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>(j*j) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }
      }

      for (i = NFFT_2; i < NFFT; i += 2) {
        ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>(j*j) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }
      }

      //intermediate rows

      for (i = 1; i < NFFT_2; i += 2) {
        ii = 3.0f*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>((j+1)*(j+1)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }
      }

      for (i = NFFT_2 + 1; i < NFFT; i += 2) {
        ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>((j+1)*(j+1)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0f;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }
      }
      fFFTin[k][0] = 0.0f;
    }
    */
    for (k = NFFTz_2; k < NFFTz; ++k) {
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      for (index = 0; index < NFFTsq; ++index) {
        fFFTin[k + NFFTz*index][0] = 0.0f;
        fFFTin[k + NFFTz*index][1] = 0.0f;
      }
    }
  } // else do nothing since the other coefficients have been zero from the beginning and have not been touched

  return;
}

void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsB() const {
  const int NFFT(fSteps), NFFTsq(fSteps*fSteps), NFFT_2(fSteps/2), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);

  // Divide EHB's bK by two since we are considering "the full 2D reciprocal lattice", not only the half space!
  const float coeffKsq(4.0f/3.0f*pow(PI/fLatticeConstant,2.0f));
  const float coeffKy(TWOPI/fLatticeConstant);
  const float coeffKx(coeffKy/sqrt3);
  const float coeffPk(2.0f/static_cast<float>(fSteps*fSteps*fStepsZ));
  const float coeffBkS(2.0f/fThickness);
  const float coeffKzSq(4.0f*pow(PI/fThickness,2.0f));
  const float symCorr(0.5f);

  int i, j, k, index, index2;
  float Gsq, ii, kk, kx, ky, sign;

  // k = 0;
  for (i = 0; i < NFFT_2; i += 2) {
    ky = coeffKy*static_cast<float>(i);
    ii = 3.0f*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j);
      Gsq = coeffKsq*(static_cast<float>(j*j) + ii);
      fBkMatrix[index][0] = \
       symCorr*(coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
       1.0f*fSumSum*fBkMatrix[index][0] - coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i])/(Gsq + fSumSum);
      fBkMatrix[index][1] = 0.0f;
      fBkMatrix[index2][0] = 0.0f;
      fBkMatrix[index2][1] = 0.0f;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j - NFFT);
      Gsq = coeffKsq*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
      fBkMatrix[index][0] = \
       symCorr*(coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
       1.0f*fSumSum*fBkMatrix[index][0] - coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i])/(Gsq + fSumSum);
      fBkMatrix[index][1] = 0.0f;
      fBkMatrix[index2][0] = 0.0f;
      fBkMatrix[index2][1] = 0.0f;
    }
  }

  for (i = NFFT_2; i < NFFT; i += 2) {
    ky = coeffKy*static_cast<float>(i - NFFT);
    ii = 3.0f*static_cast<float>((i - NFFT)*(i - NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j);
      Gsq = coeffKsq*(static_cast<float>(j*j) + ii);
      fBkMatrix[index][0] = \
       symCorr*(coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
       1.0f*fSumSum*fBkMatrix[index][0] - coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i])/(Gsq + fSumSum);
      fBkMatrix[index][1] = 0.0f;
      fBkMatrix[index2][0] = 0.0f;
      fBkMatrix[index2][1] = 0.0f;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j - NFFT);
      Gsq = coeffKsq*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
      fBkMatrix[index][0] = \
       symCorr*(coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
       1.0f*fSumSum*fBkMatrix[index][0] - coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i])/(Gsq + fSumSum);
      fBkMatrix[index][1] = 0.0f;
      fBkMatrix[index2][0] = 0.0f;
      fBkMatrix[index2][1] = 0.0f;
    }
  }

  //intermediate rows

  for (i = 1; i < NFFT_2; i += 2) {
    ky = coeffKy*static_cast<float>(i);
    ii = 3.0f*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j + 1);
      Gsq = coeffKsq*(static_cast<float>((j + 1)*(j + 1)) + ii);
      fBkMatrix[index][0] = 0.0f;
      fBkMatrix[index][1] = 0.0f;
      fBkMatrix[index2][0] = \
       symCorr*(coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
       1.0f*fSumSum*fBkMatrix[index2][0] - coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i])/(Gsq + fSumSum);
      fBkMatrix[index2][1] = 0.0f;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j + 1 - NFFT);
      Gsq = coeffKsq*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
      fBkMatrix[index][0] = 0.0f;
      fBkMatrix[index][1] = 0.0f;
      fBkMatrix[index2][0] = \
       symCorr*(coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
       1.0f*fSumSum*fBkMatrix[index2][0] - coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i])/(Gsq + fSumSum);
      fBkMatrix[index2][1] = 0.0f;
    }
  }

  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    ky = coeffKy*static_cast<float>(i - NFFT);
    ii = 3.0f*static_cast<float>((i - NFFT)*(i - NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j + 1);
      Gsq = coeffKsq*(static_cast<float>((j + 1)*(j + 1)) + ii);
      fBkMatrix[index][0] = 0.0f;
      fBkMatrix[index][1] = 0.0f;
      fBkMatrix[index2][0] = \
       symCorr*(coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
       1.0f*fSumSum*fBkMatrix[index2][0] - coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i])/(Gsq + fSumSum);
      fBkMatrix[index2][1] = 0.0f;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j + 1 - NFFT);
      Gsq = coeffKsq*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
      fBkMatrix[index][0] = 0.0f;
      fBkMatrix[index][1] = 0.0f;
      fBkMatrix[index2][0] = \
       symCorr*(coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
       1.0f*fSumSum*fBkMatrix[index2][0] - coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i])/(Gsq + fSumSum);
      fBkMatrix[index2][1] = 0.0f;
    }
  }

  fBkMatrix[0][0] = 0.0f;

  // k != 0;
  if (fFind3dSolution) {
    sign = 1.f;
    for (k = 1; k < NFFTz_2; ++k) {
      sign = -sign;
      kk = coeffKzSq*static_cast<float>(k*k);
      for (i = 0; i < NFFT_2; i += 2) {
        ky = coeffKy*static_cast<float>(i);
        ii = 3.0f*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j);
          Gsq = coeffKsq*(static_cast<float>(j*j) + ii);
          fBkMatrix[index][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
           1.0f*fSumSum*fBkMatrix[index][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[index][1] = 0.0f;
          fBkMatrix[index2][0] = 0.0f;
          fBkMatrix[index2][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
          fBkMatrix[index][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
           1.0f*fSumSum*fBkMatrix[index][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[index][1] = 0.0f;
          fBkMatrix[index2][0] = 0.0f;
          fBkMatrix[index2][1] = 0.0f;
        }
      }

      for (i = NFFT_2; i < NFFT; i += 2) {
        ky = coeffKy*static_cast<float>(i - NFFT);
        ii = 3.0f*static_cast<float>((i - NFFT)*(i - NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j);
          Gsq = coeffKsq*(static_cast<float>(j*j) + ii);
          fBkMatrix[index][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
           1.0f*fSumSum*fBkMatrix[index][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[index][1] = 0.0f;
          fBkMatrix[index2][0] = 0.0f;
          fBkMatrix[index2][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
          fBkMatrix[index][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
           1.0f*fSumSum*fBkMatrix[index][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[index][1] = 0.0f;
          fBkMatrix[index2][0] = 0.0f;
          fBkMatrix[index2][1] = 0.0f;
        }
      }

      //intermediate rows

      for (i = 1; i < NFFT_2; i += 2) {
        ky = coeffKy*static_cast<float>(i);
        ii = 3.0f*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j + 1);
          Gsq = coeffKsq*(static_cast<float>((j + 1)*(j + 1)) + ii);
          fBkMatrix[index][0] = 0.0f;
          fBkMatrix[index][1] = 0.0f;
          fBkMatrix[index2][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
           1.0f*fSumSum*fBkMatrix[index2][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[index2][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j + 1 - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
          fBkMatrix[index][0] = 0.0f;
          fBkMatrix[index][1] = 0.0f;
          fBkMatrix[index2][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
           1.0f*fSumSum*fBkMatrix[index2][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[index2][1] = 0.0f;
        }
      }

      for (i = NFFT_2 + 1; i < NFFT; i += 2) {
        ky = coeffKy*static_cast<float>(i - NFFT);
        ii = 3.0f*static_cast<float>((i - NFFT)*(i - NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j + 1);
          Gsq = coeffKsq*(static_cast<float>((j + 1)*(j + 1)) + ii);
          fBkMatrix[index][0] = 0.0f;
          fBkMatrix[index][1] = 0.0f;
          fBkMatrix[index2][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
           1.0f*fSumSum*fBkMatrix[index2][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[index2][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j + 1 - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
          fBkMatrix[index][0] = 0.0f;
          fBkMatrix[index][1] = 0.0f;
          fBkMatrix[index2][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
           1.0f*fSumSum*fBkMatrix[index2][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[index2][1] = 0.0f;
        }
      }
      fBkMatrix[k][0] = 0.0f;
    }
    /* Comment out the negative Kz - setting them to zero instead to stay closer to EHB's solution
    for (k = NFFTz_2; k < NFFTz; ++k) {
      sign = +sign;
      kk = coeffKzSq*static_cast<float>((k - NFFTz)*(k - NFFTz));
      for (i = 0; i < NFFT_2; i += 2) {
        ky = coeffKy*static_cast<float>(i);
        ii = 3.0f*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          kx = coeffKx*static_cast<float>(j);
          Gsq = coeffKsq*(static_cast<float>(j*j) + ii);
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[k + fStepsZ*(j + NFFT*i)][1] - kx*fPkMatrix[k + fStepsZ*(j + NFFT*i)][1]) + \
           1.0f*fSumSum*fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          kx = coeffKx*static_cast<float>(j - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[k + fStepsZ*(j + NFFT*i)][1] - kx*fPkMatrix[k + fStepsZ*(j + NFFT*i)][1]) + \
           1.0f*fSumSum*fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }
      }

      for (i = NFFT_2; i < NFFT; i += 2) {
        ky = coeffKy*static_cast<float>(i - NFFT);
        ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          kx = coeffKx*static_cast<float>(j);
          Gsq = coeffKsq*(static_cast<float>(j*j) + ii);
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[k + fStepsZ*(j + NFFT*i)][1] - kx*fPkMatrix[k + fStepsZ*(j + NFFT*i)][1]) + \
           1.0f*fSumSum*fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          kx = coeffKx*static_cast<float>(j - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[k + fStepsZ*(j + NFFT*i)][1] - kx*fPkMatrix[k + fStepsZ*(j + NFFT*i)][1]) + \
           1.0f*fSumSum*fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }
      }

      //intermediate rows

      for (i = 1; i < NFFT_2; i += 2) {
        ky = coeffKy*static_cast<float>(i);
        ii = 3.0f*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          kx = coeffKx*static_cast<float>(j + 1);
          Gsq = coeffKsq*(static_cast<float>((j+1)*(j+1)) + ii);
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1] - kx*fPkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1]) + \
           1.0f*fSumSum*fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          kx = coeffKx*static_cast<float>(j + 1 - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1] - kx*fPkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1]) + \
           1.0f*fSumSum*fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }
      }

      for (i = NFFT_2 + 1; i < NFFT; i += 2) {
        ky = coeffKy*static_cast<float>(i - NFFT);
        ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          kx = coeffKx*static_cast<float>(j + 1);
          Gsq = coeffKsq*(static_cast<float>((j+1)*(j+1)) + ii);
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1] - kx*fPkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1]) + \
           1.0f*fSumSum*fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          kx = coeffKx*static_cast<float>(j + 1 - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][1] = 0.0f;
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] = \
           symCorr*(coeffPk*(ky*fQMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1] - kx*fPkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1]) + \
           1.0f*fSumSum*fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i])/(0.5f*(Gsq + kk) + fSumSum);
          fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0f;
        }
      }
      fBkMatrix[k][0] = 0.0f;
    }
    */
    for (k = NFFTz_2; k < NFFTz; ++k) {
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      for (index = 0; index < NFFTsq; ++index) {
        fBkMatrix[k + NFFTz*index][0] = 0.0f;
        fBkMatrix[k + NFFTz*index][1] = 0.0f;
      }
    }
  } else { // for 2D solution only
    for (k = 1; k < NFFTz; ++k) {
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      for (index = 0; index < NFFTsq; ++index) {
        fBkMatrix[k + NFFTz*index][0] = 0.0f;
        fBkMatrix[k + NFFTz*index][1] = 0.0f;
      }
    }
  }

  return;
}

void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForQx() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);
  const float coeffKy(1.5f*fLatticeConstant/PI);

  int i, j, k;
  float ii;

  for (k = 0; k < NFFTz_2; ++k) {
    // i = 0
    for (j = 0; j < NFFT; j += 2) {
      fBkMatrix[k + NFFTz*j][0] = 0.0f;
    }
    for (i = 2; i < NFFT_2; i += 2) {
      ii = 3.0f*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffKy*static_cast<float>(i)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffKy*static_cast<float>(i)/(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffKy*static_cast<float>(i-NFFT)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffKy*static_cast<float>(i-NFFT)/(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0f*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKy*static_cast<float>(i)/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKy*static_cast<float>(i)/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKy*static_cast<float>(i-NFFT)/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKy*static_cast<float>(i-NFFT)/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }
    if (!fFind3dSolution) {
      break; // then the following bK are zero anyway
    }
  }

  return;
}

void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForQy() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);
  const float coeffKx(0.5f*sqrt3*fLatticeConstant/PI);

  int i, j, k;
  float ii;

  for (k = 0; k < NFFTz_2; ++k) {
    for (i = 0; i < NFFT_2; i += 2) {
      ii = 3.0f*static_cast<float>(i*i);
      // j = 0
      fBkMatrix[k + fStepsZ*NFFT*i][0] = 0.0f;
      for (j = 2; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j-NFFT)/(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
      // j = 0
      fBkMatrix[k + fStepsZ*NFFT*i][0] = 0.0f;
      for (j = 2; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j-NFFT)/(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0f*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>(j+1)/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>(j+1-NFFT)/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>(j+1)/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>(j+1-NFFT)/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }
    if (!fFind3dSolution) {
      break; // then the following bK are zero anyway
    }
  }

  return;
}

void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForBperpX() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);
  const float coeffX(sqrt3*fLatticeConstant/fThickness);

  int i, j, k;
  float ii;

  for (k = 0; k < NFFTz_2; ++k) {
    for (i = 0; i < NFFT_2; i += 2) {
      ii = 3.0f*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        if (!i && !j)
          fBkMatrix[k][0] = 0.0f;
        else
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*k)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffX*static_cast<float>((j-NFFT)*k)/(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*k)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffX*static_cast<float>((j-NFFT)*k)/(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0f*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffX*static_cast<float>((j+1)*k)/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffX*static_cast<float>((j+1-NFFT)*k)/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffX*static_cast<float>((j+1)*k)/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffX*static_cast<float>((j+1-NFFT)*k)/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }
  }

  for (k = NFFTz_2; k < NFFTz; ++k) {
    for (i = 0; i < NFFT_2; i += 2) {
      ii = 3.0f*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        if (!i && !j)
          fBkMatrix[k][0] = 0.0f;
        else
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*(k-NFFTz))/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffX*static_cast<float>((j-NFFT)*(k-NFFTz))/(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*(k-NFFTz))/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffX*static_cast<float>((j-NFFT)*(k-NFFTz))/(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0f*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffX*static_cast<float>((j+1)*(k-NFFTz))/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffX*static_cast<float>((j+1-NFFT)*(k-NFFTz))/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffX*static_cast<float>((j+1)*(k-NFFTz))/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffX*static_cast<float>((j+1-NFFT)*(k-NFFTz))/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }
  }

  return;
}

void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForBperpY() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);
  const float coeffY(3.0f*fLatticeConstant/fThickness);

  int i, j, k;
  float ii;

  for (k = 0; k < NFFTz_2; ++k) {
    for (i = 0; i < NFFT_2; i += 2) {
      ii = 3.0f*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        if (!i && !j)
          fBkMatrix[k][0] = 0.0f;
        else
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*k)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*k)/(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffY*static_cast<float>((i-NFFT)*k)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffY*static_cast<float>((i-NFFT)*k)/(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0f*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffY*static_cast<float>(i*k)/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffY*static_cast<float>(i*k)/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffY*static_cast<float>((i-NFFT)*k)/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffY*static_cast<float>((i-NFFT)*k)/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }
  }

  for (k = NFFTz_2; k < NFFTz; ++k) {
    for (i = 0; i < NFFT_2; i += 2) {
      ii = 3.0f*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        if (!i && !j)
          fBkMatrix[k][0] = 0.0f;
        else
          fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*(k-NFFTz))/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*(k-NFFTz))/(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffY*static_cast<float>((i-NFFT)*(k-NFFTz))/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + NFFT*i)][0] *= coeffY*static_cast<float>((i-NFFT)*(k-NFFTz))/(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0f*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffY*static_cast<float>(i*(k-NFFTz))/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffY*static_cast<float>(i*(k-NFFTz))/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffY*static_cast<float>((i-NFFT)*(k-NFFTz))/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeffY*static_cast<float>((i-NFFT)*(k-NFFTz))/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }
  }

  return;
}

void TFilmTriVortexNGLFieldCalc::CalculateSumAk() const {
  const int NFFTsq(fSteps*fSteps), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);

  int k, index;

  for (k = 0; k < NFFTz_2; ++k) {
    fSumAkFFTin[k][0] = 0.0f;
    for (index = 0; index < NFFTsq; ++index) {
      fSumAkFFTin[k][0] += fFFTin[k + NFFTz*index][0];
    }
    fSumAkFFTin[k][1] = 0.0f;
  }

  fftwf_execute(fFFTplanForSumAk);

  return;
}

void TFilmTriVortexNGLFieldCalc::CalculateGrid() const {
  // SetParameters - method has to be called from the user before the calculation!!
  if (fParam.size() < 4) {
    cout << endl << "The SetParameters-method has to be called before B(x,y,z) can be calculated!" << endl;
    return;
  }
  if (!fParam[0] || !fParam[1] || !fParam[2] || !fParam[3]) {
    cout << endl << "The field, penetration depth, coherence length and layer thickness have to have finite values in order to calculate B(x,y,z)!" << endl;
    return;
  }

  float field(fabs(fParam[0])), lambda(fabs(fParam[1])), xi(fabs(fParam[2]));
  fKappa = lambda/xi;
  fThickness = fParam[3]/lambda;
  float Hc2(getHc2(xi)), Hc2_kappa(Hc2/fKappa), scaledB(field/Hc2_kappa);  // field in EHB's reduced units

  fLatticeConstant = sqrt(2.0f*TWOPI/(fKappa*scaledB*sqrt3));  // intervortex spacing in EHB's reduced units

  fC = 3.0f + (0.4f+60.f*scaledB*scaledB)*fKappa*fKappa*fLatticeConstant/fThickness; // some coefficient needed for calculating bKs

  const int NFFT(fSteps);
  const int NFFT_2(fSteps/2);
  const int NFFT_4(fSteps/4);
  const int NFFTsq(fSteps*fSteps);
  const int NFFTsq_2((fSteps/2 + 1)*fSteps);
  const int NFFTsqStZ(NFFTsq*fStepsZ);
  const int NFFTStZ(fSteps*fStepsZ);
  const int NFFTz(fStepsZ);
  const int NFFTz_2(fStepsZ/2);
  const int NFFTsqStZ_2(NFFTsq*(fStepsZ/2 + 1));

  // first check that the field is not larger than Hc2 and that we are dealing with a type II SC ...
  if ((field >= Hc2) || (lambda < xi/sqrt(2.0f))) {
    int m;
    #pragma omp parallel for default(shared) private(m) schedule(dynamic)
    for (m = 0; m < NFFTsqStZ; ++m) {
      fBout[0][m] = 0.0f;
      fBout[1][m] = 0.0f;
      fBout[2][m] = field;
    }
    // Set the flag which shows that the calculation has been done
    fGridExists = true;
    return;
  }

  int i, j, k, l, index;

  // ... now fill in the Fourier components (Abrikosov) if everything was okay above

  FillAbrikosovCoefficients(scaledB);

  // save a few coefficients for the convergence check

  for (k = 0; k < NFFTz; ++k) {
    #pragma omp parallel for default(shared) private(j,index) schedule(dynamic)
    for (j = 0; j < NFFT; ++j) {
      index = k + NFFTz*j;
      fCheckAkConvergence[index] = fFFTin[index][0];
    }
  }

  // initialize the SumAk-input with zeros
  for (k = 0; k < NFFTz; ++k) {
    fSumAkFFTin[k][0] = 0.0f;
    fSumAkFFTin[k][1] = 0.0f;
  }

  // Do the 1D-Fourier part of the omega-calculation

  CalculateSumAk();

  // Do the 3D-Fourier transform to get omega(x,y) - Abrikosov

  fftwf_execute(fFFTplan);

  for (k = 0; k < NFFTz; ++k) {
    for (j = 0; j < NFFT; ++j) {
      #pragma omp parallel for default(shared) private(i,index) schedule(dynamic)
      for (i = 0; i < NFFT; ++i) {
        index = k + NFFTz*(j + NFFT*i);
        fOmegaMatrix[index] = fSumAk[k][0] - fRealSpaceMatrix[index][0];
      }
    }
  }

  // Calculate the gradient of omega - Abrikosov

  CalculateGradient();

  // Calculate Q-Abrikosov

  float denomQAInv;
  int indexQA;

  for (i = 0; i < NFFT; ++i) {
    #pragma omp parallel for default(shared) private(j,index,indexQA,denomQAInv) schedule(dynamic)
    for (j = 0; j < NFFT; ++j) {
      indexQA = (j + NFFT*i);
      index = fStepsZ*indexQA;
      if (!fOmegaMatrix[index] || !indexQA || (indexQA == (NFFT+1)*NFFT_2)) {
        fQMatrixA[indexQA][0] = 0.0f;
        fQMatrixA[indexQA][1] = 0.0f;
      } else {
        denomQAInv = 0.5f/(fKappa*fOmegaMatrix[index]);
        fQMatrixA[indexQA][0] = fOmegaDiffMatrix[1][index]*denomQAInv;
        fQMatrixA[indexQA][1] = -fOmegaDiffMatrix[0][index]*denomQAInv;
      }
    }
  }

/* Nice but maybe not needed
  fQMatrixA[(NFFT_4 + NFFT*NFFT_4)][0] = 0.0f;
  fQMatrixA[(NFFT_4 + NFFT*3*NFFT_4)][0] = 0.0f;
  fQMatrixA[(3*NFFT_4 + NFFT*NFFT_4)][0] = 0.0f;
  fQMatrixA[(3*NFFT_4 + NFFT*3*NFFT_4)][0] = 0.0f;
  fQMatrixA[(NFFT_4 + NFFT*NFFT_4)][1] = 0.0f;
  fQMatrixA[(NFFT_4 + NFFT*3*NFFT_4)][1] = 0.0f;
  fQMatrixA[(3*NFFT_4 + NFFT*NFFT_4)][1] = 0.0f;
  fQMatrixA[(3*NFFT_4 + NFFT*3*NFFT_4)][1] = 0.0f;
*/
  // Initialize the Q-Matrix with Q-Abrikosov

  for (k = 0; k < NFFTz; ++k) {
    for (i = 0; i < NFFT; ++i) {
      #pragma omp parallel for default(shared) private(j,index,indexQA) schedule(dynamic)
      for (j = 0; j < NFFT; ++j) {
        indexQA = (j + NFFT*i);
        index = k + NFFTz*indexQA;
        fQMatrix[index][0] = fQMatrixA[indexQA][0];
        fQMatrix[index][1] = fQMatrixA[indexQA][1];
      }
    }
  }

  // initialize the bK-Matrix
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBkMatrix[l][0] = 0.0f;
//    fBkMatrix[l][1] = 0.0f;
//  already zero'd by the gradient calculation
  }

  bool akConverged(false), bkConverged(false), akInitiallyConverged(true), firstBkCalculation(true);
  float fourKappaSq(4.0f*fKappa*fKappa);

  int count (0), toll(-100);
//  while (count < 50) {
//    ++count;
  while (!akConverged || !bkConverged) {

++count;
if (count == 100)
break;
    // First iteration steps for aK
    // Keep only terms with Kz = 0

//    CalculateGatVortexCore();
//     for (k = 0; k < NFFTz; ++k) {
//       cout << "g[" << k << "] = " << fGstorage[k] << endl;
//     }

    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    for (l = 0; l < NFFTsqStZ; l++) {
      if (fOmegaMatrix[l]) {
        fRealSpaceMatrix[l][0] = fOmegaMatrix[l]*(fOmegaMatrix[l] + fQMatrix[l][0]*fQMatrix[l][0] + fQMatrix[l][1]*fQMatrix[l][1] - 2.0f) + \
         (fOmegaDiffMatrix[0][l]*fOmegaDiffMatrix[0][l] + fOmegaDiffMatrix[1][l]*fOmegaDiffMatrix[1][l] + \
          fOmegaDiffMatrix[2][l]*fOmegaDiffMatrix[2][l])/(fourKappaSq*fOmegaMatrix[l]);
      } else {
//        cout << "index where omega is zero: " << l << endl;
        fRealSpaceMatrix[l][0] = 0.0f;
      }
      fRealSpaceMatrix[l][1] = 0.0f;
    }

    // At the two vortex cores g(r) cannot be calculated as above
    // since all of this should be a smooth function anyway, I set the value of the next neighbour r
    // for the two vortex core positions in my matrix
    // If this was not enough we can get the g(0)-values by an expensive CalculateGatVortexCore()-invocation
    #pragma omp parallel for default(shared) private(k) schedule(dynamic)
    for (k = 0; k < NFFTz; ++k) {
      fRealSpaceMatrix[k][0] = fRealSpaceMatrix[k + fStepsZ*fSteps][0];//fGstorage[k];
      fRealSpaceMatrix[k + NFFTz*(NFFT+1)*NFFT_2][0] = fRealSpaceMatrix[k][0];//fGstorage[k];
    }

    fftwf_execute(fFFTplanOmegaToAk);

// if(count == toll + 2)
//  break;

    ManipulateFourierCoefficientsA();

// if(count == toll + 2)
//  break;

    // Second iteration step for aK, first recalculate omega and its gradient

    CalculateSumAk();

    // Do the Fourier transform to get omega(x,y,z)

    fftwf_execute(fFFTplan);

// if(count == toll + 2)
//  break;

    for (k = 0; k < NFFTz; ++k) {
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      for (index = 0; index < NFFTsq; ++index) {
        fOmegaMatrix[k + NFFTz*index] = fSumAk[k][0] - fRealSpaceMatrix[k + NFFTz*index][0];
      }
    }

    CalculateGradient();

// if(count == toll + 5)
//  break;

//    CalculateGatVortexCore();

    // Get the spacial averages of the second iteration step for aK

    fSumSum = 0.0f;
    fSumOmegaSq = 0.0f;
    for (k = 0; k < NFFTz; ++k) {
      for (j = 0; j < NFFT; ++j) {
        for (i = 0; i < NFFT; ++i) {
          index = k + NFFTz*(j + NFFT*i);
          fSumOmegaSq += fOmegaMatrix[index]*fOmegaMatrix[index];
          if (fOmegaMatrix[index]) {
            fSumSum += fOmegaMatrix[index]*(1.0f - (fQMatrix[index][0]*fQMatrix[index][0] + fQMatrix[index][1]*fQMatrix[index][1])) - \
            (fOmegaDiffMatrix[0][index]*fOmegaDiffMatrix[0][index] + fOmegaDiffMatrix[1][index]*fOmegaDiffMatrix[1][index] + \
              fOmegaDiffMatrix[2][index]*fOmegaDiffMatrix[2][index])/(fourKappaSq*fOmegaMatrix[index]);
          } else {
    //        fSumSum -= fGstorage[k];
            index = k + fStepsZ*(j + fSteps*(i + 1));
            if (i < NFFT - 1 && fOmegaMatrix[index]) {
              fSumSum += fOmegaMatrix[index]*(1.0f - (fQMatrix[index][0]*fQMatrix[index][0] + fQMatrix[index][1]*fQMatrix[index][1])) - \
              (fOmegaDiffMatrix[0][index]*fOmegaDiffMatrix[0][index] + \
                fOmegaDiffMatrix[1][index]*fOmegaDiffMatrix[1][index] + \
                fOmegaDiffMatrix[2][index]*fOmegaDiffMatrix[2][index])/(fourKappaSq*fOmegaMatrix[index]);
            }
          }
        }
      }
    }

    fSumSum /= fSumOmegaSq;

    // Multiply the aK with the spacial averages
    for (k = 0; k < NFFTz_2; ++k) {
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      for (index = 0; index < NFFTsq; ++index) {
        fFFTin[k + NFFTz*index][0] = fFFTin[k + NFFTz*index][0]*fSumSum;
      }
    }

    // Check if the aK iterations converged

    akConverged = true;

    for (k = 0; k < NFFTz_2; ++k) {
      for (j = 0; j < NFFT; ++j) {
        index = k + NFFTz*j;
        if (fFFTin[index][0]) {
          if (((fabs(fFFTin[index][0]) > 1.0E-5f) && (fabs(fCheckAkConvergence[index] - fFFTin[index][0])/fFFTin[index][0] > 1.0E-2f)) \
             || (fCheckAkConvergence[index]/fFFTin[index][0] < 0.0f)) {
            //cout << "old: " << fCheckAkConvergence[index] << ", new: " << fFFTin[index][0] << endl;
            akConverged = false;
            cout << "count = " << count << ", Ak index = " << index << endl;
            break;
          }
        }
      }
      if (!akConverged)
        break;
    }

    if (!akConverged) {
      for (k = 0; k < NFFTz_2; ++k) {
        #pragma omp parallel for default(shared) private(j) schedule(dynamic)
        for (j = 0; j < NFFT; ++j) {
          index = k + NFFTz*j;
          fCheckAkConvergence[index] = fFFTin[index][0];
        }
      }
    } else {
      akInitiallyConverged = true;
    }

    //  cout << "Ak Convergence: " << akConverged << endl;

    // Calculate omega again either for the bK-iteration step or again the aK-iteration

    CalculateSumAk();

   cout << "fSumAk = ";
   for (k = 0; k < NFFTz; ++k){
     cout << fSumAk[k][0] << ", ";
   }
   cout << endl;

//if(count == 10)
//break;

    // Do the Fourier transform to get omega

    fftwf_execute(fFFTplan);

    for (k = 0; k < NFFTz; ++k) {
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      for (index = 0; index < NFFTsq; ++index) {
        fOmegaMatrix[k + NFFTz*index] = fSumAk[k][0] - fRealSpaceMatrix[k + NFFTz*index][0];
      }
    }

    CalculateGradient();

//  if(count == toll + 2)
//   break;

// if (count == 50)
//   break;


    if (akInitiallyConverged) {  // if the aK iterations converged, go on with the bK calculation

// if (count == toll + 2)
//    break;
      // first calculate PK (use the Q-Matrix memory for the second part)
      #pragma omp parallel for default(shared) private(l) schedule(dynamic)
      for (l = 0; l < NFFTsqStZ; ++l) {
        fPkMatrix[l][0] = fOmegaMatrix[l]*fQMatrix[l][1];
        fQMatrix[l][0] = fOmegaMatrix[l]*fQMatrix[l][0];
        fPkMatrix[l][1] = 0.0f;
        fQMatrix[l][1] = 0.0f;
      }

// if (count == toll + 2)
//    break;

      fftwf_execute(fFFTplanForPk1);
      fftwf_execute(fFFTplanForPk2);
// if (count == toll + 2)
//    break;

      // calculate bKS
      float sign;

      for (index = 0; index < NFFTsq; ++index) {
        fBkS[index] = 0.f;
        sign = -1.f;
        for (k = 0; k < NFFTz_2; ++k) {
          sign = -sign;
          fBkS[index] += sign*fBkMatrix[k + NFFTz*index][0];
        }
      }

// if (count == toll + 2)
//    break;

      // calculate <w>
      fSumSum = 0.0f;
      for (l = 0; l < NFFTsqStZ; ++l) {
        fSumSum += fOmegaMatrix[l];
      }

      cout << "fC = " << fC << ", <w> = " << fSumSum/NFFTsqStZ << endl;

      fSumSum *= fC/static_cast<float>(NFFTsqStZ);

      // Now since we have all the ingredients for the bK, do the bK-iteration step
      ManipulateFourierCoefficientsB();

// if (count == toll + 2)
//    break;

      // Check the convergence of the bK-iterations

      if (firstBkCalculation) {
        #pragma omp parallel for default(shared) private(l) schedule(dynamic)
        for (l = 0; l < NFFTStZ; ++l) {
          fCheckBkConvergence[l] = 0.0f;
        }
        firstBkCalculation = false;
        akConverged = false;
      }

      bkConverged = true;

      for (k = 0; k < NFFTz_2; ++k) {
        for (j = 0; j < NFFT; ++j) {
          index = k + NFFTz*j;
          if (fBkMatrix[index][0]) {
            if (((fabs(fBkMatrix[index][0]) > 1.0E-5f) && \
               (fabs(fCheckBkConvergence[index] - fBkMatrix[index][0])/fBkMatrix[index][0] > 1.0E-2f)) \
             || (fCheckBkConvergence[index]/fBkMatrix[index][0] < 0.0f)) {
            // cout << "old: " << fCheckBkConvergence[index] << ", new: " << fBkMatrix[index][0] << endl;
              bkConverged = false;
              cout << "count = " << count << ", Bk index = " << index << endl;
              break;
            }
          }
        }
        if (!bkConverged)
          break;
      }

      // cout << "Bk Convergence: " << bkConverged << endl;

      if (!bkConverged) {
        for (k = 0; k < NFFTz_2; ++k) {
          #pragma omp parallel for default(shared) private(j) schedule(dynamic)
          for (j = 0; j < NFFT; ++j) {
            index = k + NFFTz*j;
            fCheckBkConvergence[index] = fBkMatrix[index][0];
          }
        }
      }

      // Go on with the calculation of Q(x,y)

      // In order to save memory I will not allocate more space for another matrix but save a copy of the bKs in the aK-Matrix
      #pragma omp parallel for default(shared) private(l) schedule(dynamic)
      for (l = 0; l < NFFTsqStZ; ++l) {
        fFFTin[l][1] = fBkMatrix[l][0];
      }

  if (count == toll + 45)
   break;

if (count == 5) {
  akConverged = true;
  bkConverged = true;
}

      if (bkConverged && akConverged) {
        if (!fFind3dSolution) {
          cout << "count = " << count << " 2D converged" << endl;
          //break;
          akConverged = false;
     //     akInitiallyConverged = false;
          bkConverged = false;
          fFind3dSolution = true;
          toll = count;
        } else {
          cout << "count = " << count << " 3D converged" << endl;
          break;
        }
      }

//  if(count == toll + 1)
// break;

      ManipulateFourierCoefficientsForQx();

//   if(count == toll + 1)
//  break;

      fftwf_execute(fFFTplanBkToBandQ);

//  if(count == toll + 1)
// break;

      for (k = 0; k < NFFTz; ++k) {
        #pragma omp parallel for default(shared) private(index) schedule(dynamic)
        for (index = 0; index < NFFTsq; ++index) {
          fQMatrix[k + NFFTz*index][0] = fQMatrixA[index][0] - fBkMatrix[k + NFFTz*index][1];
        }
      }


//  if(count == toll + 1)
// break;

/*
      for (k = 0; k < fStepsZ; ++k) {
        #pragma omp parallel for default(shared) private(j) schedule(dynamic)
        for (j = 0; j < NFFT; ++j) {
          fQMatrix[k + fStepsZ*j][0] = 0.0f;
          fQMatrix[k + fStepsZ*(j + NFFT*NFFT_2)][0] = 0.0f;
        }
        fQMatrix[k + fStepsZ*(NFFT_4 + NFFT*NFFT_4)][0] = 0.0f;
        fQMatrix[k + fStepsZ*(NFFT_4 + NFFT*3*NFFT_4)][0] = 0.0f;
        fQMatrix[k + fStepsZ*(3*NFFT_4 + NFFT*NFFT_4)][0] = 0.0f;
        fQMatrix[k + fStepsZ*(3*NFFT_4 + NFFT*3*NFFT_4)][0] = 0.0f;
      }
*/
      // Restore bKs for Qy calculation and Fourier transform to get Qy
      #pragma omp parallel for default(shared) private(l) schedule(dynamic)
      for (l = 0; l < NFFTsqStZ; ++l) {
        fBkMatrix[l][0] = fFFTin[l][1];
        fBkMatrix[l][1] = 0.0f;
      }

      ManipulateFourierCoefficientsForQy();

//  if(count == toll + 1)
//break;

      fftwf_execute(fFFTplanBkToBandQ);

//  if(count == toll + 1)
// break;

      for (k = 0; k < NFFTz; ++k) {
        #pragma omp parallel for default(shared) private(index) schedule(dynamic)
        for (index = 0; index < NFFTsq; ++index) {
          fQMatrix[k + NFFTz*index][1] = fQMatrixA[index][1] + fBkMatrix[k + NFFTz*index][1];
        }
      }

//  if(count == toll + 1)
// break;
/*
      for (k = 0; k < fStepsZ; ++k) {
        #pragma omp parallel for default(shared) private(i) schedule(dynamic)
        for (i = 0; i < NFFT; ++i) {
          fQMatrix[k + fStepsZ*(NFFT*i)][1] = 0.0f;
          fQMatrix[k + fStepsZ*(NFFT_2 + NFFT*i)][1] = 0.0f;
        }
        fQMatrix[k + fStepsZ*(NFFT_4 + NFFT*NFFT_4)][1] = 0.0f;
        fQMatrix[k + fStepsZ*(NFFT_4 + NFFT*3*NFFT_4)][1] = 0.0f;
        fQMatrix[k + fStepsZ*(3*NFFT_4 + NFFT*NFFT_4)][1] = 0.0f;
        fQMatrix[k + fStepsZ*(3*NFFT_4 + NFFT*3*NFFT_4)][1] = 0.0f;
      }
*/
      // Restore bKs for the next iteration
      #pragma omp parallel for default(shared) private(l) schedule(dynamic)
      for (l = 0; l < NFFTsqStZ; ++l) {
        fBkMatrix[l][0] = fFFTin[l][1];
        fBkMatrix[l][1] = 0.0f;
      }
    } // end if (akInitiallyConverged)
  } // end while

  // If the iterations converged, calculate the magnetic field components

  ManipulateFourierCoefficientsForBperpX();

  fftwf_execute(fFFTplanBkToBandQ);

  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBout[0][l] = fBkMatrix[l][1]*Hc2_kappa;
  }

  // Restore bKs for the By-calculation
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBkMatrix[l][0] = fFFTin[l][1];
    fBkMatrix[l][1] = 0.0f;
  }

  ManipulateFourierCoefficientsForBperpY();

  fftwf_execute(fFFTplanBkToBandQ);

  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBout[1][l] = fBkMatrix[l][1]*Hc2_kappa;
  }

  // Restore bKs for the Bz-calculation
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBkMatrix[l][0] = fFFTin[l][1];
    fBkMatrix[l][1] = 0.0f;
  }

  fftwf_execute(fFFTplanBkToBandQ);

  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBout[2][l] = (scaledB + fBkMatrix[l][0])*Hc2_kappa;
  }

  // Restore bKs for debugging
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBkMatrix[l][0] = fFFTin[l][1];
    fBkMatrix[l][1] = 0.0f;
  }

/*
  // If the iterations have converged, rescale the field from Brandt's units to Gauss

  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsq; l++) {
    fFFTout[l] *= Hc2_kappa;
  }

  // Set the flag which shows that the calculation has been done

  fGridExists = true;
  return;
*/
}
