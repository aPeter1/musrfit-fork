/***************************************************************************

  TFilmTriVortexFieldCalc.cpp

  Author: Bastian M. Wojek

  $Id$

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "TFilmTriVortexFieldCalc.h"

#include <cstdlib>
#include <cmath>

#ifdef HAVE_GOMP
#include <omp.h>
#endif

#include <iostream>
using namespace std;

#include "TMath.h"

#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067f
#define TWOPI (2.0f*3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067f)

const float fluxQuantum(2.067833667e7f); // in this case this is Gauss times square nm

const float sqrt3(sqrt(3.0f));
const float pi_4sqrt3(0.25f*PI/sqrt(3.0f));


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
    float min(fBout[2][0]), curfieldSq(0.0);
    unsigned int curindex(0);
    for (unsigned int k(0); k < fStepsZ; ++k) {
      for (unsigned int i(0); i < fSteps/2; ++i) {
        for (unsigned int j(0); j < fSteps/2; ++j) { // check only the first quadrant of B(x,y)
          curindex = k + fStepsZ*(j + fSteps*i);
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
 : fLatticeConstant(0.0), fKappa(0.0), fSumOmegaSq(0.0), fSumSum(0.0), fFind3dSolution(false)
{
//  cout << "TFilmTriVortexNGLFieldCalc::TFilmTriVortexNGLFieldCalc... ";

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
  if (init_threads) {
#ifdef HAVE_GOMP
    fftwf_plan_with_nthreads(omp_get_num_procs());
#else
    fftwf_plan_with_nthreads(2);
#endif /* HAVE_GOMP */
  }
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

  fBkS = new fftwf_complex[fSteps*fSteps];

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
//    cout << "use wisdom ... ";
    // use the first plan from the base class here - it will be destroyed by the base class destructor
    fFFTplan = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fFFTin, fRealSpaceMatrix, FFTW_BACKWARD, FFTW_EXHAUSTIVE);
    fFFTplanBkToBandQ = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fBkMatrix, fBkMatrix, FFTW_BACKWARD, FFTW_EXHAUSTIVE);
    fFFTplanOmegaToAk = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fRealSpaceMatrix, fFFTin, FFTW_FORWARD, FFTW_EXHAUSTIVE);
    fFFTplanForSumAk = fftwf_plan_dft_1d(fStepsZ, fSumAkFFTin, fSumAk, FFTW_FORWARD, FFTW_EXHAUSTIVE);
    fFFTplanForPk1 = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fPkMatrix, fPkMatrix, FFTW_FORWARD, FFTW_EXHAUSTIVE);
    fFFTplanForPk2 = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fQMatrix, fQMatrix, FFTW_BACKWARD, FFTW_EXHAUSTIVE);
    fFFTplanForBatSurf = fftwf_plan_dft_2d(fSteps, fSteps, fBkS, fBkS, FFTW_FORWARD, FFTW_EXHAUSTIVE);
  }
  else {
//    cout << "do not use wisdom ... ";
    // use the first plan from the base class here - it will be destroyed by the base class destructor
    fFFTplan = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fFFTin, fRealSpaceMatrix, FFTW_BACKWARD, FFTW_ESTIMATE);
    fFFTplanBkToBandQ = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fBkMatrix, fBkMatrix, FFTW_BACKWARD, FFTW_ESTIMATE);
    fFFTplanOmegaToAk = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fRealSpaceMatrix, fFFTin, FFTW_FORWARD, FFTW_ESTIMATE);
    fFFTplanForSumAk = fftwf_plan_dft_1d(fStepsZ, fSumAkFFTin, fSumAk, FFTW_FORWARD, FFTW_ESTIMATE);
    fFFTplanForPk1 = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fPkMatrix, fPkMatrix, FFTW_FORWARD, FFTW_ESTIMATE);
    fFFTplanForPk2 = fftwf_plan_dft_3d(fSteps, fSteps, fStepsZ, fQMatrix, fQMatrix, FFTW_BACKWARD, FFTW_ESTIMATE);
    fFFTplanForBatSurf = fftwf_plan_dft_2d(fSteps, fSteps, fBkS, fBkS, FFTW_FORWARD, FFTW_ESTIMATE);
  }
//  cout << "done" << endl;
}

TFilmTriVortexNGLFieldCalc::~TFilmTriVortexNGLFieldCalc() {

  // clean up
  fftwf_destroy_plan(fFFTplanBkToBandQ);
  fftwf_destroy_plan(fFFTplanOmegaToAk);
  fftwf_destroy_plan(fFFTplanForSumAk);
  fftwf_destroy_plan(fFFTplanForPk1);
  fftwf_destroy_plan(fFFTplanForPk2);
  fftwf_destroy_plan(fFFTplanForBatSurf);

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
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBkMatrix[l][1] = fFFTin[l][0];
  }

  // sum_K aK Kx^2 cos(Kx*x + Ky*y) cos(Kz*z)
  // First multiply the aK with Kx^2, then call FFTW

  float coeffKx(4.0/3.0*pow(PI/fLatticeConstant, 2.0f));;

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
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
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
    fFFTin[fStepsZ*NFFT*i][0] = 0.0;
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
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
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
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0;
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0;
    }
  }
  for (i = NFFT_2; i < NFFT; i += 2) {
    for (j = 0; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0;
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0;
    }
  }

  // odd rows
  for (i = 1; i < NFFT_2; i += 2) {
    for (j = 0; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
    }
  }
  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    for (j = 0; j < NFFT_2; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
    }
    for (j = NFFT_2; j < NFFT; j += 2) {
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
    }
  }

  // k != 0

  if (fFind3dSolution) {
    for (k = 1; k < NFFTz_2; ++k) {
      // even rows
      for (i = 0; i < NFFT; i += 2) {
        // j = 0
        fFFTin[k + fStepsZ*NFFT*i][0] = 0.0;
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
        fFFTin[k + fStepsZ*NFFT*i][0] = 0.0;
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
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsqStZ; ++l) {
    fFFTin[l][0] = fBkMatrix[l][1];
  }

  // Final evaluation
  for (k = 0; k < NFFTz; ++k) {
    fGstorage[k] /= 2.0*denom[k]*fKappa*fKappa;
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
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
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
    fFFTin[fStepsZ*NFFT*i][0] = 0.0;
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
    for (k = 1; k < NFFTz; ++k) {
      // even rows
      for (i = 0; i < NFFT; i += 2) {
        // j = 0
        fFFTin[k + NFFTz*NFFT*i][0] = 0.0;
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
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
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
    fFFTin[NFFTz*j][0] = 0.0;
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
    for (k = 1; k < NFFTz; ++k) {
      // even rows
      // i = 0
      for (j = 0; j < NFFT; j += 2) {
        fFFTin[k + NFFTz*j][0] = 0.0;
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
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
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

    // 1D transform - first sum up the coefficients in the other two dimensions and then call FFTW
    for (k = 0; k < NFFTz; ++k) {
      fSumAkFFTin[k][0] = 0.0;
      for (index = 0; index < NFFTsq; ++index) {
        fSumAkFFTin[k][0] += fFFTin[k + NFFTz*index][0];
      }
      fSumAkFFTin[k][1] = 0.0;
    }

    fftwf_execute(fFFTplanForSumAk);

    // 3D transform
    fftwf_execute(fFFTplan);

    // Copy the results to the gradient matrix - with the 1D-FORWARD-transform we have to _add_ fSumAk
    for (k = 0; k < NFFTz; ++k) {
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      #endif
      for (index = 0; index < NFFTsq; ++index) {
        fOmegaDiffMatrix[2][k + NFFTz*index] = fRealSpaceMatrix[k + NFFTz*index][1] + fSumAk[k][1];
      }
    }

    // Restore the original aK-matrix
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
    for (l = 0; l < NFFTsqStZ; ++l) {
      fFFTin[l][0] = fBkMatrix[l][1];
      fBkMatrix[l][1] = 0.0;
    }
  } else {
    // For the 2D solution, dw/dz = 0
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
    for (l = 0; l < NFFTsqStZ; ++l) {
      fOmegaDiffMatrix[2][l] = 0.0;
      fBkMatrix[l][1] = 0.0;
    }
  }
/* If the numerics is fine, this part is not needed */
  // Ensure that omega and the gradient at the vortex-core positions are zero
  for (k = 0; k < NFFTz; ++k) {
    fOmegaMatrix[k] = 0.0;
    fOmegaMatrix[k + NFFTz*(NFFT+1)*NFFT_2] = 0.0;
    fOmegaDiffMatrix[0][k] = 0.0;
    fOmegaDiffMatrix[0][k + NFFTz*(NFFT+1)*NFFT_2] = 0.0;
    fOmegaDiffMatrix[1][k] = 0.0;
    fOmegaDiffMatrix[1][k + NFFTz*(NFFT+1)*NFFT_2] = 0.0;
    fOmegaDiffMatrix[2][k] = 0.0;
    fOmegaDiffMatrix[2][k + NFFTz*(NFFT+1)*NFFT_2] = 0.0;
  }/*
    for (i = 0; i < NFFT; ++i) {
      // j = 0
      fOmegaDiffMatrix[0][k + NFFTz*NFFT*i] = 0.0;
      // j = NFFT_2
      fOmegaDiffMatrix[0][k + NFFTz*(NFFT_2 + NFFT*i)] = 0.0;
    }
    for (j = 0; j < NFFT; ++j) {
      // i = 0
      fOmegaDiffMatrix[1][k + NFFTz*j] = 0.0;
      // i = NFFT_2
      fOmegaDiffMatrix[1][k + NFFTz*(j + NFFT*NFFT_2)] = 0.0;
    }
    fOmegaDiffMatrix[2][k] = 0.0;
    fOmegaDiffMatrix[2][k + NFFTz*(NFFT+1)*NFFT_2] = 0.0;
  }
  for (index = 0; index < NFFTsq; ++index) {
    // k = 0
    fOmegaDiffMatrix[2][index] = 0.0;
    // k = NFFTz_2
    fOmegaDiffMatrix[2][NFFTz_2 + index] = 0.0;
  }
*/
  return;
}

void TFilmTriVortexNGLFieldCalc::FillAbrikosovCoefficients(const float reducedField) const {
  const int NFFT(fSteps), NFFTsq(fSteps*fSteps), NFFT_2(NFFT/2), NFFTz_2(fStepsZ/2), NFFTz(fStepsZ);

  float coeff(1.0-reducedField);
  float Gsq, sign, ii;
  int i,j,k,index;

//  k = 0;
  for (i = 0; i < NFFT_2; i += 2) {
    if (!(i % 4)) {
      sign = 1.0;
    } else {
      sign = -1.0;
    }
    ii = 3.0*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      sign = -sign;
      Gsq = static_cast<float>(j*j) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = sign*coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      sign = -sign;
      Gsq = static_cast<float>((j-NFFT)*(j-NFFT)) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = sign*coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }
  }

  for (i = NFFT_2; i < NFFT; i += 2) {
    if (!(i % 4)) {
      sign = 1.0;
    } else {
      sign = -1.0;
    }
    ii = 3.0*static_cast<float>((i-NFFT)*(i-NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      sign = -sign;
      Gsq = static_cast<float>(j*j) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = sign*coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      sign = -sign;
      Gsq = static_cast<float>((j-NFFT)*(j-NFFT)) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = sign*coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }
  }

  // intermediate rows
  for (i = 1; i < NFFT_2; i += 2) {
    ii = 3.0*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      Gsq = static_cast<float>((j + 1)*(j + 1)) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      Gsq = static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }
  }

  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    ii = 3.0*static_cast<float>((i-NFFT)*(i-NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      Gsq = static_cast<float>((j+1)*(j+1)) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      Gsq = static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii;
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = coeff*exp(-pi_4sqrt3*Gsq);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }
  }

  fFFTin[0][0] = 0.0;

  for (k = 1; k < NFFTz; ++k) {
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(index) schedule(dynamic)
    #endif
    for (index = 0; index < NFFTsq; ++index) {
      fFFTin[k + NFFTz*index][0] = 0.0;
      fFFTin[k + NFFTz*index][1] = 0.0;
    }
  }

  return;
}

void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsA() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2), NFFTsqStZ(fSteps*fSteps*fStepsZ), NFFTsq(fSteps*fSteps);

  // Divide EHB's coefficient no2 by two since we are considering "the full 3D reciprocal lattice", not only the half space!
  // Additionally treat all K the same (no difference between Kperp and K with Kz != 0)
  const float symCorr(1.0f);
  const float coeff1(4.0f/3.0f*pow(PI/fLatticeConstant,2.0f));
  const float coeff3(2.0f*fKappa*fKappa);
  const float coeff2(symCorr*coeff3/static_cast<float>(NFFTsqStZ));

  const float coeff4(4.0f*pow(PI/fThickness,2.0f));
  const float coeff5(1.0f*coeff2);

  int i, j, k, l, index, index2;
  float Gsq, ii, kk;

  // k = 0;
  for (i = 0; i < NFFT_2; i += 2) {
    ii = 3.0f*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      Gsq = coeff1*(static_cast<float>(j*j) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      Gsq = coeff1*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }
  }

  for (i = NFFT_2; i < NFFT; i += 2) {
    ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      Gsq = coeff1*(static_cast<float>(j*j) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      Gsq = coeff1*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }
  }

  //intermediate rows

  for (i = 1; i < NFFT_2; i += 2) {
    ii = 3.0f*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      Gsq = coeff1*(static_cast<float>((j+1)*(j+1)) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      Gsq = coeff1*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }
  }

  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      Gsq = coeff1*(static_cast<float>((j+1)*(j+1)) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      Gsq = coeff1*(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      fFFTin[fStepsZ*(j + NFFT*i)][0] = 0.0;
      fFFTin[fStepsZ*(j + NFFT*i)][1] = 0.0;
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][0] *= coeff2/(Gsq+coeff3);
      fFFTin[fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
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
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }
      }

      for (i = NFFT_2; i < NFFT; i += 2) {
        ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>(j*j) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }
      }

      //intermediate rows

      for (i = 1; i < NFFT_2; i += 2) {
        ii = 3.0f*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>((j+1)*(j+1)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }
      }

      for (i = NFFT_2 + 1; i < NFFT; i += 2) {
        ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>((j+1)*(j+1)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }
      }
      fFFTin[k][0] = 0.0f;
    }

    for (k = NFFTz_2; k < NFFTz; ++k) {
      kk = coeff4*static_cast<float>((k - NFFTz)*(k - NFFTz));
      for (i = 0; i < NFFT_2; i += 2) {
        ii = 3.0f*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>(j*j) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }
      }

      for (i = NFFT_2; i < NFFT; i += 2) {
        ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>(j*j) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }
      }

      //intermediate rows

      for (i = 1; i < NFFT_2; i += 2) {
        ii = 3.0f*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>((j+1)*(j+1)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }
      }

      for (i = NFFT_2 + 1; i < NFFT; i += 2) {
        ii = 3.0f*static_cast<float>((i-NFFT)*(i-NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          Gsq = coeff1*(static_cast<float>((j+1)*(j+1)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          Gsq = coeff1*(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii) + kk;
          fFFTin[k + fStepsZ*(j + NFFT*i)][0] = 0.0;
          fFFTin[k + fStepsZ*(j + NFFT*i)][1] = 0.0;
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][0] *= coeff5/(Gsq+coeff3);
          fFFTin[k + fStepsZ*(j + 1 + NFFT*i)][1] = 0.0;
        }
      }
      fFFTin[k][0] = 0.0f;
    }
/*
    for (k = NFFTz_2; k < NFFTz; ++k) {
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      for (index = 0; index < NFFTsq; ++index) {
        fFFTin[k + NFFTz*index][0] = 0.0;
        fFFTin[k + NFFTz*index][1] = 0.0;
      }
    }
*/
  } // else do nothing since the other coefficients have been zero from the beginning and have not been touched

  return;
}

void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsB() const {
  const int NFFT(fSteps), NFFTsq(fSteps*fSteps), NFFT_2(fSteps/2), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);

  // Divide EHB's PK by two since we are considering "the full 3D reciprocal lattice", not only the half space!
  // Additionally treat all K the same (no difference between Kperp and K with Kz != 0)
  const float coeffKsq(4.0f/3.0f*pow(PI/fLatticeConstant,2.0f));
  const float coeffKy(TWOPI/fLatticeConstant);
  const float coeffKx(coeffKy/sqrt3);
  const float coeffPk(1.0f/static_cast<float>(fSteps*fSteps*fStepsZ));
  const float coeffBkS(2.0f/fThickness);
  const float coeffKzSq(4.0f*pow(PI/fThickness,2.0f));

  int i, j, k, index, index2;
  float Gsq, ii, kk, kx, ky, sign;

  // k = 0;
  for (i = 0; i < NFFT_2; i += 2) {
    ky = coeffKy*static_cast<float>(i);
    ii = 3.0*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j);
      Gsq = coeffKsq*(static_cast<float>(j*j) + ii);
      fBkMatrix[index][0] = \
       (coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
       1.0f*fSumSum*fBkMatrix[index][0] - coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i][0])/(Gsq + 1.0f*fSumSum);
      fBkMatrix[index][1] = 0.0;
      fBkMatrix[index2][0] = 0.0;
      fBkMatrix[index2][1] = 0.0;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j - NFFT);
      Gsq = coeffKsq*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
      fBkMatrix[index][0] = \
       (coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
       1.0f*fSumSum*fBkMatrix[index][0] - coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i][0])/(Gsq + 1.0f*fSumSum);
      fBkMatrix[index][1] = 0.0;
      fBkMatrix[index2][0] = 0.0;
      fBkMatrix[index2][1] = 0.0;
    }
  }

  for (i = NFFT_2; i < NFFT; i += 2) {
    ky = coeffKy*static_cast<float>(i - NFFT);
    ii = 3.0*static_cast<float>((i - NFFT)*(i - NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j);
      Gsq = coeffKsq*(static_cast<float>(j*j) + ii);
      fBkMatrix[index][0] = \
       (coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
       1.0f*fSumSum*fBkMatrix[index][0] - coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i][0])/(Gsq + 1.0f*fSumSum);
      fBkMatrix[index][1] = 0.0;
      fBkMatrix[index2][0] = 0.0;
      fBkMatrix[index2][1] = 0.0;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j - NFFT);
      Gsq = coeffKsq*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
      fBkMatrix[index][0] = \
       (coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
       1.0f*fSumSum*fBkMatrix[index][0] - coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i][0])/(Gsq + 1.0f*fSumSum);
      fBkMatrix[index][1] = 0.0;
      fBkMatrix[index2][0] = 0.0;
      fBkMatrix[index2][1] = 0.0;
    }
  }

  //intermediate rows

  for (i = 1; i < NFFT_2; i += 2) {
    ky = coeffKy*static_cast<float>(i);
    ii = 3.0*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j + 1);
      Gsq = coeffKsq*(static_cast<float>((j + 1)*(j + 1)) + ii);
      fBkMatrix[index][0] = 0.0;
      fBkMatrix[index][1] = 0.0;
      fBkMatrix[index2][0] = \
       (coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
       1.0f*fSumSum*fBkMatrix[index2][0] - coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i][0])/(Gsq + 1.0f*fSumSum);
      fBkMatrix[index2][1] = 0.0;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j + 1 - NFFT);
      Gsq = coeffKsq*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
      fBkMatrix[index][0] = 0.0;
      fBkMatrix[index][1] = 0.0;
      fBkMatrix[index2][0] = \
       (coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
       1.0f*fSumSum*fBkMatrix[index2][0] - coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i][0])/(Gsq + 1.0f*fSumSum);
      fBkMatrix[index2][1] = 0.0;
    }
  }

  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    ky = coeffKy*static_cast<float>(i - NFFT);
    ii = 3.0*static_cast<float>((i - NFFT)*(i - NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j + 1);
      Gsq = coeffKsq*(static_cast<float>((j + 1)*(j + 1)) + ii);
      fBkMatrix[index][0] = 0.0;
      fBkMatrix[index][1] = 0.0;
      fBkMatrix[index2][0] = \
       (coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
       1.0f*fSumSum*fBkMatrix[index2][0] - coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i][0])/(Gsq + 1.0f*fSumSum);
      fBkMatrix[index2][1] = 0.0;
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      index = NFFTz*(j + NFFT*i);
      index2 = index + NFFTz;
      kx = coeffKx*static_cast<float>(j + 1 - NFFT);
      Gsq = coeffKsq*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
      fBkMatrix[index][0] = 0.0;
      fBkMatrix[index][1] = 0.0;
      fBkMatrix[index2][0] = \
       (coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
       1.0f*fSumSum*fBkMatrix[index2][0] - coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i][0])/(Gsq + 1.0f*fSumSum);
      fBkMatrix[index2][1] = 0.0;
    }
  }

  fBkMatrix[0][0] = 0.0;

  // k != 0;
  if (fFind3dSolution) {
    sign = 1.f;
    for (k = 1; k < NFFTz_2; ++k) {
      sign = -sign;
      kk = coeffKzSq*static_cast<float>(k*k);
      for (i = 0; i < NFFT_2; i += 2) {
        ky = coeffKy*static_cast<float>(i);
        ii = 3.0*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j);
          Gsq = coeffKsq*(static_cast<float>(j*j) + ii);
          fBkMatrix[index][0] = \
           (coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
           1.0f*fSumSum*fBkMatrix[index][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = 0.0;
          fBkMatrix[index2][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
          fBkMatrix[index][0] = \
           (coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
           1.0f*fSumSum*fBkMatrix[index][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = 0.0;
          fBkMatrix[index2][1] = 0.0;
        }
      }

      for (i = NFFT_2; i < NFFT; i += 2) {
        ky = coeffKy*static_cast<float>(i - NFFT);
        ii = 3.0*static_cast<float>((i - NFFT)*(i - NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j);
          Gsq = coeffKsq*(static_cast<float>(j*j) + ii);
          fBkMatrix[index][0] = \
           (coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
           1.0f*fSumSum*fBkMatrix[index][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = 0.0;
          fBkMatrix[index2][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
          fBkMatrix[index][0] = \
           (coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
           1.0f*fSumSum*fBkMatrix[index][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = 0.0;
          fBkMatrix[index2][1] = 0.0;
        }
      }

      //intermediate rows

      for (i = 1; i < NFFT_2; i += 2) {
        ky = coeffKy*static_cast<float>(i);
        ii = 3.0*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j + 1);
          Gsq = coeffKsq*(static_cast<float>((j + 1)*(j + 1)) + ii);
          fBkMatrix[index][0] = 0.0;
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = \
           (coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
           1.0f*fSumSum*fBkMatrix[index2][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index2][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j + 1 - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
          fBkMatrix[index][0] = 0.0;
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = \
           (coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
           1.0f*fSumSum*fBkMatrix[index2][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index2][1] = 0.0;
        }
      }

      for (i = NFFT_2 + 1; i < NFFT; i += 2) {
        ky = coeffKy*static_cast<float>(i - NFFT);
        ii = 3.0*static_cast<float>((i - NFFT)*(i - NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j + 1);
          Gsq = coeffKsq*(static_cast<float>((j + 1)*(j + 1)) + ii);
          fBkMatrix[index][0] = 0.0;
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = \
           (coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
           1.0f*fSumSum*fBkMatrix[index2][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index2][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j + 1 - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
          fBkMatrix[index][0] = 0.0;
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = \
           (coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
           1.0f*fSumSum*fBkMatrix[index2][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index2][1] = 0.0;
        }
      }
      fBkMatrix[k][0] = 0.0;
    }

    for (k = NFFTz_2; k < NFFTz; ++k) {
      sign = -sign;
      kk = coeffKzSq*static_cast<float>((k - NFFTz)*(k - NFFTz));
      for (i = 0; i < NFFT_2; i += 2) {
        ky = coeffKy*static_cast<float>(i);
        ii = 3.0*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j);
          Gsq = coeffKsq*(static_cast<float>(j*j) + ii);
          fBkMatrix[index][0] = \
           (coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
           1.0f*fSumSum*fBkMatrix[index][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = 0.0;
          fBkMatrix[index2][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
          fBkMatrix[index][0] = \
           (coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
           1.0f*fSumSum*fBkMatrix[index][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = 0.0;
          fBkMatrix[index2][1] = 0.0;
        }
      }

      for (i = NFFT_2; i < NFFT; i += 2) {
        ky = coeffKy*static_cast<float>(i - NFFT);
        ii = 3.0*static_cast<float>((i-NFFT)*(i-NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j);
          Gsq = coeffKsq*(static_cast<float>(j*j) + ii);
          fBkMatrix[index][0] = \
           (coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
           1.0f*fSumSum*fBkMatrix[index][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = 0.0;
          fBkMatrix[index2][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
          fBkMatrix[index][0] = \
           (coeffPk*(ky*fQMatrix[index][1] + kx*fPkMatrix[index][1]) + \
           1.0f*fSumSum*fBkMatrix[index][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = 0.0;
          fBkMatrix[index2][1] = 0.0;
        }
      }

      //intermediate rows

      for (i = 1; i < NFFT_2; i += 2) {
        ky = coeffKy*static_cast<float>(i);
        ii = 3.0*static_cast<float>(i*i);
        for (j = 0; j < NFFT_2; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j + 1);
          Gsq = coeffKsq*(static_cast<float>((j+1)*(j+1)) + ii);
          fBkMatrix[index][0] = 0.0;
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = \
           (coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
           1.0f*fSumSum*fBkMatrix[index2][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index2][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j + 1 - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
          fBkMatrix[index][0] = 0.0;
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = \
           (coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
           1.0f*fSumSum*fBkMatrix[index2][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index2][1] = 0.0;
        }
      }

      for (i = NFFT_2 + 1; i < NFFT; i += 2) {
        ky = coeffKy*static_cast<float>(i - NFFT);
        ii = 3.0*static_cast<float>((i-NFFT)*(i-NFFT));
        for (j = 0; j < NFFT_2; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j + 1);
          Gsq = coeffKsq*(static_cast<float>((j + 1)*(j + 1)) + ii);
          fBkMatrix[index][0] = 0.0;
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = \
           (coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
           1.0f*fSumSum*fBkMatrix[index2][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index2][1] = 0.0;
        }

        for (j = NFFT_2; j < NFFT; j += 2) {
          index = k + NFFTz*(j + NFFT*i);
          index2 = index + NFFTz;
          kx = coeffKx*static_cast<float>(j + 1 - NFFT);
          Gsq = coeffKsq*(static_cast<float>((j + 1 - NFFT)*(j + 1 - NFFT)) + ii);
          fBkMatrix[index][0] = 0.0;
          fBkMatrix[index][1] = 0.0;
          fBkMatrix[index2][0] = \
           (coeffPk*(ky*fQMatrix[index2][1] + kx*fPkMatrix[index2][1]) + \
           1.0f*fSumSum*fBkMatrix[index2][0] - sign*coeffBkS*sqrt(Gsq)*fBkS[j + 1 + NFFT*i][0])/(1.0f*(Gsq + kk) + fSumSum);
          fBkMatrix[index2][1] = 0.0;
        }
      }
      fBkMatrix[k][0] = 0.0;
    }
/*
    for (k = NFFTz_2; k < NFFTz; ++k) {
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      for (index = 0; index < NFFTsq; ++index) {
        fBkMatrix[k + NFFTz*index][0] = 0.0;
        fBkMatrix[k + NFFTz*index][1] = 0.0;
      }
    }
*/
  } else { // for 2D solution only
    for (k = 1; k < NFFTz; ++k) {
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      #endif
      for (index = 0; index < NFFTsq; ++index) {
        fBkMatrix[k + NFFTz*index][0] = 0.0;
        fBkMatrix[k + NFFTz*index][1] = 0.0;
      }
    }
  }

  return;
}

void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForQx() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);
  const float coeffKy(1.5*fLatticeConstant/PI);

  int i, j, k;
  float ii;

  for (k = 0; k < NFFTz; ++k) {
    // i = 0
    for (j = 0; j < NFFT; j += 2) {
      fBkMatrix[k + NFFTz*j][0] = 0.0;
    }
    for (i = 2; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffKy*static_cast<float>(i)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffKy*static_cast<float>(i)/(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ii = 3.0*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffKy*static_cast<float>(i-NFFT)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffKy*static_cast<float>(i-NFFT)/(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKy*static_cast<float>(i)/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKy*static_cast<float>(i)/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ii = 3.0*static_cast<float>((i-NFFT)*(i-NFFT));
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
  const float coeffKx(0.5*sqrt3*fLatticeConstant/PI);

  int i, j, k;
  float ii;

  for (k = 0; k < NFFTz; ++k) {
    for (i = 0; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      // j = 0
      fBkMatrix[k + NFFTz*NFFT*i][0] = 0.0;
      for (j = 2; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j-NFFT)/(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ii = 3.0*static_cast<float>((i-NFFT)*(i-NFFT));
      // j = 0
      fBkMatrix[k + NFFTz*NFFT*i][0] = 0.0;
      for (j = 2; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffKx*static_cast<float>(j-NFFT)/(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>(j+1)/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>(j+1-NFFT)/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ii = 3.0*static_cast<float>((i-NFFT)*(i-NFFT));
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>(j+1)/(static_cast<float>((j+1)*(j+1)) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        fBkMatrix[k + NFFTz*(j + 1 + NFFT*i)][0] *= coeffKx*static_cast<float>(j+1-NFFT)/(static_cast<float>((j+1-NFFT)*(j+1-NFFT)) + ii);
      }
    }
    if (!fFind3dSolution) {
      break; // then the following bK are zero anyway
    }
  }

  return;
}


void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForBperpXatSurface() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);

  int i, j, k;
  float ii;

  for (i = 0; i < NFFT_2; i += 2) {
    ii = 3.0f*static_cast<float>(i*i);
    // j = 0
    fBkS[NFFT*i][0] = 0.0f;
    // j != 0
    for (j = 2; j < NFFT_2; j += 2) {
      fBkS[j + NFFT*i][0] *= static_cast<float>(j)/sqrt(static_cast<float>(j*j) + ii);
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      fBkS[j + NFFT*i][0] *= static_cast<float>(j-NFFT)/sqrt(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
    }
  }

  for (i = NFFT_2; i < NFFT; i += 2) {
    ii = 3.0*static_cast<float>((i-NFFT)*(i-NFFT));
    // j = 0
    fBkS[NFFT*i][0] = 0.0f;
    // j != 0
    for (j = 2; j < NFFT_2; j += 2) {
      fBkS[j + NFFT*i][0] *= static_cast<float>(j)/sqrt(static_cast<float>(j*j) + ii);
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      fBkS[j + NFFT*i][0] *= static_cast<float>(j-NFFT)/sqrt(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
    }
  }

  //intermediate rows

  for (i = 1; i < NFFT_2; i += 2) {
    ii = 3.0f*static_cast<float>(i*i);
    for (j = 1; j < NFFT_2; j += 2) {
      fBkS[j + NFFT*i][0] *= static_cast<float>(j)/sqrt(static_cast<float>(j*j) + ii);
    }

    for (j = NFFT_2 + 1; j < NFFT; j += 2) {
      fBkS[j + NFFT*i][0] *= static_cast<float>(j-NFFT)/sqrt(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
    }
  }

  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    ii = 3.0*static_cast<float>((i-NFFT)*(i-NFFT));
    for (j = 1; j < NFFT_2; j += 2) {
      fBkS[j + NFFT*i][0] *= static_cast<float>(j)/sqrt(static_cast<float>(j*j) + ii);
    }

    for (j = NFFT_2 + 1; j < NFFT; j += 2) {
      fBkS[j + NFFT*i][0] *= static_cast<float>(j-NFFT)/sqrt(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
    }
  }

  return;
}

void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForBperpYatSurface() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);

  int i, j, k;
  float ii;

  // i = 0
  for (j = 0; j < NFFT; j += 2) {
    fBkS[j][0] = 0.0f;
  }
  for (i = 2; i < NFFT_2; i += 2) {
    ii = 3.0*static_cast<float>(i*i);
    for (j = 0; j < NFFT_2; j += 2) {
      fBkS[j + NFFT*i][0] *= sqrt3*static_cast<float>(i)/sqrt(static_cast<float>(j*j) + ii);
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      fBkS[j + NFFT*i][0] *= sqrt3*static_cast<float>(i)/sqrt(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
    }
  }

  for (i = NFFT_2; i < NFFT; i += 2) {
    ii = 3.0*static_cast<float>((i-NFFT)*(i-NFFT));
    for (j = 0; j < NFFT_2; j += 2) {
      fBkS[j + NFFT*i][0] *= sqrt3*static_cast<float>(i-NFFT)/sqrt(static_cast<float>(j*j) + ii);
    }

    for (j = NFFT_2; j < NFFT; j += 2) {
      fBkS[j + NFFT*i][0] *= sqrt3*static_cast<float>(i-NFFT)/sqrt(static_cast<float>((j - NFFT)*(j - NFFT)) + ii);
    }
  }

  //intermediate rows

  for (i = 1; i < NFFT_2; i += 2) {
    ii = 3.0*static_cast<float>(i*i);
    for (j = 1; j < NFFT_2; j += 2) {
      fBkS[j + NFFT*i][0] *= sqrt3*static_cast<float>(i)/sqrt(static_cast<float>(j*j) + ii);
    }

    for (j = NFFT_2 + 1; j < NFFT; j += 2) {
      fBkS[j + NFFT*i][0] *= sqrt3*static_cast<float>(i)/sqrt(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
    }
  }

  for (i = NFFT_2 + 1; i < NFFT; i += 2) {
    ii = 3.0*static_cast<float>((i-NFFT)*(i-NFFT));
    for (j = 1; j < NFFT_2; j += 2) {
      fBkS[j + NFFT*i][0] *= sqrt3*static_cast<float>(i-NFFT)/sqrt(static_cast<float>(j*j) + ii);
    }

    for (j = NFFT_2 + 1; j < NFFT; j += 2) {
      fBkS[j + NFFT*i][0] *= sqrt3*static_cast<float>(i-NFFT)/sqrt(static_cast<float>((j-NFFT)*(j-NFFT)) + ii);
    }
  }

  return;
}


void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForBperpXFirst() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTsq(fSteps*fSteps), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);
  const float coeffX(sqrt3*fLatticeConstant/fThickness);

  int i, j, k, kx, ky, kz, index;
  float ii;

  // k = 0
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(index) schedule(dynamic)
  #endif
  for (index = 0; index < NFFTsq; ++index) {
    fBkMatrix[NFFTz*index][0] = 0.0f;
  }

  for (k = 1; k < NFFTz_2; ++k) {
    for (i = 0; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      // j = 0
      fBkMatrix[k + NFFTz*NFFT*i][0] = 0.0f;
      // j != 0
      for (j = 2; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*k)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*k)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      // j = 0
      fBkMatrix[k + NFFTz*NFFT*i][0] = 0.0f;
      // j != 0
      for (j = 2; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*k)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*k)/(static_cast<float>(kx*kx) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*k)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*k)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*k)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*k)/(static_cast<float>(kx*kx) + ii);
      }
    }
  }

  for (k = NFFTz_2; k < NFFTz; ++k) {
    kz = k - NFFTz;
    for (i = 0; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      // j = 0
      fBkMatrix[k + NFFTz*NFFT*i][0] = 0.0f;
      // j != 0
      for (j = 2; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      // j = 0
      fBkMatrix[k + NFFTz*NFFT*i][0] = 0.0f;
      // j != 0
      for (j = 2; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }
  }
/*
  for (k = NFFTz_2; k < NFFTz; ++k) {
    for (index = 0; index < NFFTsq; ++index) {
      fBkMatrix[k + NFFTz*index][0] = 0.0f;
    }
  }
*/
  return;
}

void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForBperpXSecond() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTsq(fSteps*fSteps), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);
  const float coeffX(sqrt3*fLatticeConstant/fThickness);

  int i, j, k, kx, ky, kz, index;
  float ii;

  // k = 0
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(index) schedule(dynamic)
  #endif
  for (index = 0; index < NFFTsq; ++index) {
    fBkMatrix[NFFTz*index][0] = 0.0f;
  }

  for (k = 1; k < NFFTz_2; ++k) {
    kz = -k;
    for (i = 0; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      // j = 0
      fBkMatrix[k + NFFTz*NFFT*i][0] = 0.0f;
      // j != 0
      for (j = 2; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      // j = 0
      fBkMatrix[k + NFFTz*NFFT*i][0] = 0.0f;
      // j != 0
      for (j = 2; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }
  }

  for (k = NFFTz_2; k < NFFTz; ++k) {
    kz = NFFTz - k;
    for (i = 0; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      // j = 0
      fBkMatrix[k + NFFTz*NFFT*i][0] = 0.0f;
      // j != 0
      for (j = 2; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      // j = 0
      fBkMatrix[k + NFFTz*NFFT*i][0] = 0.0f;
      // j != 0
      for (j = 2; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(j*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffX*static_cast<float>(kx*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }
  }

  return;
}


void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForBperpYFirst() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTsq(fSteps*fSteps), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);
  const float coeffY(3.0f*fLatticeConstant/fThickness);

  int i, j, k, kx, ky, kz, index;
  float ii;

  // k = 0
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(index) schedule(dynamic)
  #endif
  for (index = 0; index < NFFTsq; ++index) {
    fBkMatrix[NFFTz*index][0] = 0.0f;
  }

  for (k = 1; k < NFFTz_2; ++k) {
    // i = 0
    for (j = 0; j < NFFT; j += 2) {
      fBkMatrix[k + NFFTz*j][0] = 0.0f;
    }
    // i != 0
    for (i = 2; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*k)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*k)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*k)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*k)/(static_cast<float>(kx*kx) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*k)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*k)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*k)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*k)/(static_cast<float>(kx*kx) + ii);
      }
    }
  }

  for (k = NFFTz_2; k < NFFTz; ++k) {
    kz = k - NFFTz;
    // i = 0
    for (j = 0; j < NFFT; j += 2) {
      fBkMatrix[k + NFFTz*j][0] = 0.0f;
    }
    // i != 0
    for (i = 2; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0f*static_cast<float>(ky*ky);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }
  }

  return;
}

void TFilmTriVortexNGLFieldCalc::ManipulateFourierCoefficientsForBperpYSecond() const {
  const int NFFT(fSteps), NFFT_2(fSteps/2), NFFTsq(fSteps*fSteps), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);
  const float coeffY(3.0f*fLatticeConstant/fThickness);

  int i, j, k, kx, ky, kz, index;
  float ii;

  // k = 0
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(index) schedule(dynamic)
  #endif
  for (index = 0; index < NFFTsq; ++index) {
    fBkMatrix[NFFTz*index][0] = 0.0f;
  }

  for (k = 1; k < NFFTz_2; ++k) {
    kz = -k;
    // i = 0
    for (j = 0; j < NFFT; j += 2) {
      fBkMatrix[k + NFFTz*j][0] = 0.0f;
    }
    // i != 0
    for (i = 2; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }
  }

  for (k = NFFTz_2; k < NFFTz; ++k) {
    kz = NFFTz - k;
    // i = 0
    for (j = 0; j < NFFT; j += 2) {
      fBkMatrix[k + NFFTz*j][0] = 0.0f;
    }
    // i != 0
    for (i = 2; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0*static_cast<float>(ky*ky);
      for (j = 0; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    //intermediate rows

    for (i = 1; i < NFFT_2; i += 2) {
      ii = 3.0*static_cast<float>(i*i);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(i*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }

    for (i = NFFT_2 + 1; i < NFFT; i += 2) {
      ky = i - NFFT;
      ii = 3.0f*static_cast<float>(ky*ky);
      for (j = 1; j < NFFT_2; j += 2) {
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*kz)/(static_cast<float>(j*j) + ii);
      }

      for (j = NFFT_2 + 1; j < NFFT; j += 2) {
        kx = j - NFFT;
        fBkMatrix[k + NFFTz*(j + NFFT*i)][0] *= coeffY*static_cast<float>(ky*kz)/(static_cast<float>(kx*kx) + ii);
      }
    }
  }

  return;
}

void TFilmTriVortexNGLFieldCalc::CalculateSumAk() const {
  const int NFFTsq(fSteps*fSteps), NFFTz(fStepsZ), NFFTz_2(fStepsZ/2);

  int k, index;

  for (k = 0; k < NFFTz; ++k) {
    fSumAkFFTin[k][0] = 0.0;
    for (index = 0; index < NFFTsq; ++index) {
      fSumAkFFTin[k][0] += fFFTin[k + NFFTz*index][0];
    }
    fSumAkFFTin[k][1] = 0.0;
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
  float Hc2(fluxQuantum/(TWOPI*xi*xi)), Hc2_kappa(Hc2/fKappa), scaledB(field/Hc2_kappa);  // field in EHB's reduced units

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
  if ((field >= Hc2) || (lambda < xi/sqrt(2.0))) {
    int m;
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(m) schedule(dynamic)
    #endif
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

  FillAbrikosovCoefficients(0.0);

  // save a few coefficients for the convergence check

  for (k = 0; k < NFFTz; ++k) {
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(j,index) schedule(dynamic)
    #endif
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
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(i,index) schedule(dynamic)
      #endif
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

  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(index,denomQAInv) schedule(dynamic)
  #endif
  for (index = 0; index < NFFTsq; ++index) {
    if (!fOmegaMatrix[NFFTz*index] || !index || (index == (NFFT+1)*NFFT_2)) {
      fQMatrixA[index][0] = 0.0;
      fQMatrixA[index][1] = 0.0;
    } else {
      denomQAInv = 0.5/(fKappa*fOmegaMatrix[NFFTz*index]);
      fQMatrixA[index][0] = fOmegaDiffMatrix[1][NFFTz*index]*denomQAInv;
      fQMatrixA[index][1] = -fOmegaDiffMatrix[0][NFFTz*index]*denomQAInv;
    }
  }

/* Nice but maybe not needed
  fQMatrixA[(NFFT_4 + NFFT*NFFT_4)][0] = 0.0;
  fQMatrixA[(NFFT_4 + NFFT*3*NFFT_4)][0] = 0.0;
  fQMatrixA[(3*NFFT_4 + NFFT*NFFT_4)][0] = 0.0;
  fQMatrixA[(3*NFFT_4 + NFFT*3*NFFT_4)][0] = 0.0;
  fQMatrixA[(NFFT_4 + NFFT*NFFT_4)][1] = 0.0;
  fQMatrixA[(NFFT_4 + NFFT*3*NFFT_4)][1] = 0.0;
  fQMatrixA[(3*NFFT_4 + NFFT*NFFT_4)][1] = 0.0;
  fQMatrixA[(3*NFFT_4 + NFFT*3*NFFT_4)][1] = 0.0;
*/
  // Initialize the Q-Matrix with Q-Abrikosov

  for (k = 0; k < NFFTz; ++k) {
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(index) schedule(dynamic)
    #endif
    for (index = 0; index < NFFTsq; ++index) {
      fQMatrix[k + NFFTz*index][0] = fQMatrixA[index][0];
      fQMatrix[k + NFFTz*index][1] = fQMatrixA[index][1];
    }
  }

  // initialize the bK-Matrix
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBkMatrix[l][0] = 0.0;
//    fBkMatrix[l][1] = 0.0;
//  already zero'd by the gradient calculation
  }

  bool akConverged(false), bkConverged(false), firstBkCalculation(true);
  float fourKappaSq(4.0*fKappa*fKappa), meanAk(0.0f);

  int count(0);

  while (!akConverged || !bkConverged) {

    ++count;

    // First iteration steps for aK
    // Keep only terms with Kz = 0

//       CalculateGatVortexCore();
//       for (k = 0; k < NFFTz; ++k) {
//         cout << "g[" << k << "] = " << fGstorage[k] << endl;
//       }

    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
    for (l = 0; l < NFFTsqStZ; l++) {
      if (fOmegaMatrix[l]) {
        fRealSpaceMatrix[l][0] = fOmegaMatrix[l]*(fOmegaMatrix[l] + fQMatrix[l][0]*fQMatrix[l][0] + fQMatrix[l][1]*fQMatrix[l][1] - 2.0) + \
         (fOmegaDiffMatrix[0][l]*fOmegaDiffMatrix[0][l] + fOmegaDiffMatrix[1][l]*fOmegaDiffMatrix[1][l] + \
          fOmegaDiffMatrix[2][l]*fOmegaDiffMatrix[2][l])/(fourKappaSq*fOmegaMatrix[l]);
      } else {
//        cout << "index where omega is zero: " << l << endl;
        fRealSpaceMatrix[l][0] = 0.0;
      }
      fRealSpaceMatrix[l][1] = 0.0;
    }

    // At the two vortex cores g(r) cannot be calculated as above
    // since all of this should be a smooth function anyway, I set the value of the next neighbour r
    // for the two vortex core positions in my matrix
    // If this was not enough we can get the g(0)-values by an expensive CalculateGatVortexCore()-invocation (not working at the moment)
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(k) schedule(dynamic)
    #endif
    for (k = 0; k < NFFTz; ++k) {
      fRealSpaceMatrix[k][0] = fRealSpaceMatrix[k + fStepsZ*fSteps][0];//fGstorage[k];
      fRealSpaceMatrix[k + NFFTz*(NFFT+1)*NFFT_2][0] = fRealSpaceMatrix[k][0];//fGstorage[k];
    }

    fftwf_execute(fFFTplanOmegaToAk);

    ManipulateFourierCoefficientsA();


    // Second iteration step for aK, first recalculate omega and its gradient

    CalculateSumAk();

    // Do the Fourier transform to get omega(x,y,z)

    fftwf_execute(fFFTplan);

    for (k = 0; k < NFFTz; ++k) {
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      #endif
      for (index = 0; index < NFFTsq; ++index) {
        fOmegaMatrix[k + NFFTz*index] = fSumAk[k][0] - fRealSpaceMatrix[k + NFFTz*index][0];
      }
    }

    CalculateGradient();


    //CalculateGatVortexCore();

    // Get the spacial averages of the second iteration step for aK

    fSumSum = 0.0f;
    fSumOmegaSq = 0.0f;
    for (k = 0; k < NFFTz; ++k) {
      for (j = 0; j < NFFT; ++j) {
        for (i = 0; i < NFFT; ++i) {
          index = k + NFFTz*(j + NFFT*i);
          fSumOmegaSq += fOmegaMatrix[index]*fOmegaMatrix[index];
          if (fOmegaMatrix[index]) {// && (index != k) && (index != k + NFFTz*(NFFT+1)*NFFT_2)) {
            fSumSum += fOmegaMatrix[index]*(1.0f - (fQMatrix[index][0]*fQMatrix[index][0] + fQMatrix[index][1]*fQMatrix[index][1])) - \
            (fOmegaDiffMatrix[0][index]*fOmegaDiffMatrix[0][index] + fOmegaDiffMatrix[1][index]*fOmegaDiffMatrix[1][index] + \
              fOmegaDiffMatrix[2][index]*fOmegaDiffMatrix[2][index])/(fourKappaSq*fOmegaMatrix[index]);
          } else {
//            cout << "! fOmegaMatrix at index " << index << endl;
           // fSumSum -= fGstorage[k];
           index = k + fStepsZ*(j + fSteps*(i + 1));
            if (i < NFFT - 1 && fOmegaMatrix[index]) {
              fSumSum += fOmegaMatrix[index]*(1.0 - (fQMatrix[index][0]*fQMatrix[index][0] + fQMatrix[index][1]*fQMatrix[index][1])) - \
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
    for (k = 0; k < NFFTz; ++k) {
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      #endif
      for (index = 0; index < NFFTsq; ++index) {
        fFFTin[k + NFFTz*index][0] = fFFTin[k + NFFTz*index][0]*fSumSum;
      }
    }

    // Check if the aK iterations converged

    akConverged = true;

    for (k = 0; k < NFFTz; ++k) {
      for (j = 0; j < NFFT; ++j) {
        index = k + NFFTz*j;
        if (fFFTin[index][0]) {
          if (((fabs(fFFTin[index][0]) > 1.0E-5f) && (fabs(fCheckAkConvergence[index] - fFFTin[index][0])/fFFTin[index][0] > 5.0E-3f)) \
             || ((fabs(fFFTin[index][0]) > 1.0E-10f) && (fCheckAkConvergence[index]/fFFTin[index][0] < 0.0))) {
            //cout << "old: " << fCheckAkConvergence[index] << ", new: " << fFFTin[index][0] << endl;
            akConverged = false;
            //cout << "count = " << count << ", Ak index = " << index << endl;
            break;
          }
        }
      }
      if (!akConverged)
        break;
    }

    if (!akConverged) {
      for (k = 0; k < NFFTz; ++k) {
        #ifdef HAVE_GOMP
        #pragma omp parallel for default(shared) private(j, index) schedule(dynamic)
        #endif
        for (j = 0; j < NFFT; ++j) {
          index = k + NFFTz*j;
          fCheckAkConvergence[index] = fFFTin[index][0];
        }
      }
    }

    //  cout << "Ak Convergence: " << akConverged << endl;

    // Calculate omega again either for the bK-iteration step or again the aK-iteration

    CalculateSumAk();

//    cout << "fSumAk = ";
//    for (k = 0; k < NFFTz; ++k){
//      cout << fSumAk[k][0] << ", ";
//    }
//    cout << endl;

    meanAk = 0.0f;
    for (k = 0; k < NFFTz; ++k) {
      meanAk += fSumAk[k][0];
    }
    meanAk /= static_cast<float>(NFFTz);

    // Do the Fourier transform to get omega

    fftwf_execute(fFFTplan);

    for (k = 0; k < NFFTz; ++k) {
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      #endif
      for (index = 0; index < NFFTsq; ++index) {
        fOmegaMatrix[k + NFFTz*index] = fSumAk[k][0] - fRealSpaceMatrix[k + NFFTz*index][0];
      }
    }

    CalculateGradient();

    // first calculate PK (use the Q-Matrix memory for the second part)
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
    for (l = 0; l < NFFTsqStZ; ++l) {
      fPkMatrix[l][0] = fOmegaMatrix[l]*fQMatrix[l][1];
      fQMatrix[l][0] = fOmegaMatrix[l]*fQMatrix[l][0];
      fPkMatrix[l][1] = 0.0;
      fQMatrix[l][1] = 0.0;
    }

    fftwf_execute(fFFTplanForPk1);
    fftwf_execute(fFFTplanForPk2);

    // calculate bKS
    float sign;

    for (index = 0; index < NFFTsq; ++index) {
      fBkS[index][0] = 0.f;
      sign = -1.f;
      for (k = 0; k < NFFTz; ++k) {
        sign = -sign;
        fBkS[index][0] += sign*fBkMatrix[k + NFFTz*index][0];
      }
      fBkS[index][1] = 0.f;
    }

    //  cout << "fC = " << fC << ", meanAk = " << meanAk << endl;

    fSumSum = fC*meanAk;

    // Now since we have all the ingredients for the bK, do the bK-iteration step
    ManipulateFourierCoefficientsB();

    // Check the convergence of the bK-iterations

    if (firstBkCalculation) {
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(l) schedule(dynamic)
      #endif
      for (l = 0; l < NFFTStZ; ++l) {
        fCheckBkConvergence[l] = 0.0f;
      }
      firstBkCalculation = false;
      akConverged = false;
    }

    bkConverged = true;

    for (k = 0; k < NFFTz; ++k) {
      for (j = 0; j < NFFT; ++j) {
        index = k + NFFTz*j;
        if (fBkMatrix[index][0]) {
          if (((fabs(fBkMatrix[index][0]) > 1.0E-5f) && \
              (fabs(fCheckBkConvergence[index] - fBkMatrix[index][0])/fBkMatrix[index][0] > 5.0E-3f)) \
            || ((fabs(fBkMatrix[index][0]) > 1.0E-10f) && (fCheckBkConvergence[index]/fBkMatrix[index][0] < 0.0))) {
            //cout << "old: " << fCheckBkConvergence[index] << ", new: " << fBkMatrix[index][0] << endl;
            bkConverged = false;
            //cout << "count = " << count << ", Bk index = " << index << endl;
            break;
          }
        }
      }
      if (!bkConverged)
        break;
    }

    // cout << "Bk Convergence: " << bkConverged << endl;

    if (!bkConverged) {
      for (k = 0; k < NFFTz; ++k) {
        #ifdef HAVE_GOMP
        #pragma omp parallel for default(shared) private(j) schedule(dynamic)
        #endif
        for (j = 0; j < NFFT; ++j) {
          index = k + NFFTz*j;
          fCheckBkConvergence[index] = fBkMatrix[index][0];
        }
      }
    }

    // In order to save memory I will not allocate more space for another matrix but save a copy of the bKs in the aK-Matrix
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
    for (l = 0; l < NFFTsqStZ; ++l) {
      fFFTin[l][1] = fBkMatrix[l][0];
    }

    if (count == 50) {
      cout << "3D iterations aborted after " << count << " steps" << endl;
      break;
    }

    if (count == 5) { // do five 2D iterations and go on with the 3D iterations afterwards
      akConverged = true;
      bkConverged = true;
    }

    if (bkConverged && akConverged) {
      if (!fFind3dSolution) {
        //cout << "count = " << count << " 2D converged" << endl;
        //cout << "2D iterations converged after " << count << " steps" << endl;
        //break;
        akConverged = false;
        bkConverged = false;
        fFind3dSolution = true;
      } else {
        cout << "3D iterations converged after " << count << " steps" << endl;
        break;
      }
    }

    // Go on with the calculation of Q(x,y)

    ManipulateFourierCoefficientsForQx();

    fftwf_execute(fFFTplanBkToBandQ);

    for (k = 0; k < NFFTz; ++k) {
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      #endif
      for (index = 0; index < NFFTsq; ++index) {
        fQMatrix[k + NFFTz*index][0] = fQMatrixA[index][0] - fBkMatrix[k + NFFTz*index][1];
      }
    }

    // Restore bKs for Qy calculation and Fourier transform to get Qy
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
    for (l = 0; l < NFFTsqStZ; ++l) {
      fBkMatrix[l][0] = fFFTin[l][1];
      fBkMatrix[l][1] = 0.0;
    }

    ManipulateFourierCoefficientsForQy();

    fftwf_execute(fFFTplanBkToBandQ);

    for (k = 0; k < NFFTz; ++k) {
      #ifdef HAVE_GOMP
      #pragma omp parallel for default(shared) private(index) schedule(dynamic)
      #endif
      for (index = 0; index < NFFTsq; ++index) {
        fQMatrix[k + NFFTz*index][1] = fQMatrixA[index][1] + fBkMatrix[k + NFFTz*index][1];
      }
    }

    // Restore bKs for the next iteration
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(l) schedule(dynamic)
    #endif
    for (l = 0; l < NFFTsqStZ; ++l) {
      fBkMatrix[l][0] = fFFTin[l][1];
      fBkMatrix[l][1] = 0.0;
    }
  } // end while

  // If the iterations have finished, calculate the magnetic field components

  ManipulateFourierCoefficientsForBperpXFirst();

  fftwf_execute(fFFTplanBkToBandQ);

  // Fill in the B-Matrix and restore the bKs for the second part of the Bx-calculation
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBout[0][l] = fBkMatrix[l][0];
    fBkMatrix[l][0] = fFFTin[l][1];
    fBkMatrix[l][1] = 0.0f;
  }

  ManipulateFourierCoefficientsForBperpXSecond();

  fftwf_execute(fFFTplanBkToBandQ);

  // Fill in the B-Matrix and restore the bKs for the By-calculation

  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBout[0][l] = 0.5f*(fBkMatrix[l][0] - fBout[0][l])*Hc2_kappa;
    fBkMatrix[l][0] = fFFTin[l][1];
    fBkMatrix[l][1] = 0.0f;
  }

  ManipulateFourierCoefficientsForBperpYFirst();

  fftwf_execute(fFFTplanBkToBandQ);

  // Fill in the B-Matrix and restore the bKs for the second part of the By-calculation
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBout[1][l] = fBkMatrix[l][0];
    fBkMatrix[l][0] = fFFTin[l][1];
    fBkMatrix[l][1] = 0.0f;
  }

  ManipulateFourierCoefficientsForBperpYSecond();

  fftwf_execute(fFFTplanBkToBandQ);

  // Fill in the B-Matrix and restore the bKs for the second part of the By-calculation
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBout[1][l] = 0.5f*(fBkMatrix[l][0] - fBout[1][l])*Hc2_kappa;
    fBkMatrix[l][0] = fFFTin[l][1];
    fBkMatrix[l][1] = 0.0f;
  }

  fftwf_execute(fFFTplanBkToBandQ);

  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  #endif
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBout[2][l] = (scaledB + fBkMatrix[l][0])*Hc2_kappa;
  }

  // Since the surface is not included in the Bx and By-calculation above, we do another step

  // Save a copy of the BkS - where does not matter since this is the very end of the calculation...
  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(index) schedule(dynamic)
  #endif
  for (index = 0; index < NFFTsq; ++index) {
    fFFTin[index][1] = fBkS[index][0];
  }

  ManipulateFourierCoefficientsForBperpXatSurface();

  fftwf_execute(fFFTplanForBatSurf);

  // Write the surface fields to the field-Matrix and restore the BkS for the By-calculation

  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(index) schedule(dynamic)
  #endif
  for (index = 0; index < NFFTsq; ++index) {
    fBout[0][NFFTz/2 + NFFTz*index] = fBkS[index][1]*Hc2_kappa;
    fBkS[index][0] = fFFTin[index][1];
    fBkS[index][1] = 0.0f;
  }

  ManipulateFourierCoefficientsForBperpYatSurface();

  fftwf_execute(fFFTplanForBatSurf);

  // Write the surface fields to the field-Matrix

  #ifdef HAVE_GOMP
  #pragma omp parallel for default(shared) private(index) schedule(dynamic)
  #endif
  for (index = 0; index < NFFTsq; ++index) {
    fBout[1][NFFTz/2 + NFFTz*index] = fBkS[index][1]*Hc2_kappa;
  }

/*
  float normalizer(1.f/fBout[2][0]);
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBout[2][l] *= normalizer;
  }

  // Restore bKs for debugging
  #pragma omp parallel for default(shared) private(l) schedule(dynamic)
  for (l = 0; l < NFFTsqStZ; ++l) {
    fBkMatrix[l][0] = fFFTin[l][1];
    fBkMatrix[l][1] = 0.0;
  }
*/

  // Set the flag which shows that the calculation has been done

  fGridExists = true;
  return;

}
