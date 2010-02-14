/***************************************************************************

  TBulkTriVortexFieldCalc.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2010/10/17

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

#ifndef _TFilmVortexFieldCalc_H_
#define _TFilmVortexFieldCalc_H_

#include <vector>
#include <string>
using namespace std;

#include "fftw3.h"

//--------------------
// Base class for any kind of vortex symmetry
//--------------------

class TFilmVortexFieldCalc {

public:

  TFilmVortexFieldCalc() {}

  virtual ~TFilmVortexFieldCalc();

  virtual vector<float*> DataB() const {return fBout;}
  virtual void SetParameters(const vector<float>& par) {fParam = par; fGridExists = false;}
  virtual void CalculateGrid() const = 0;
  virtual float GetBmin() const;
  virtual float GetBmax() const;
  virtual bool GridExists() const {return fGridExists;}
  virtual void UnsetGridExists() const {fGridExists = false;}
  virtual unsigned int GetNumberOfSteps() const {return fSteps;}

protected:
  vector<float> fParam;
  unsigned int fSteps;  // number of steps, the "unit cell" of the vortex lattice is devided in (in x- and y- direction)
  unsigned int fStepsZ; // number of layers that should be calculated in z-direction (film slices)
  mutable fftwf_complex *fFFTin; // Fourier components of omega
  mutable vector<float*> fBout; // three pointers to Bx, By, Bz; each of these arrays is in row-major order
  fftwf_plan fFFTplan;
  bool fUseWisdom;
  string fWisdom;
  mutable bool fGridExists;
};

//--------------------
// Class for triangular vortex lattice, Minimisation of the GL free energy à la Brandt (PRB 71, 014521 (2005))
//--------------------

class TFilmTriVortexNGLFieldCalc : public TFilmVortexFieldCalc {

public:

  TFilmTriVortexNGLFieldCalc(const string&, const unsigned int steps = 256, const unsigned int stepsZ = 32);
  ~TFilmTriVortexNGLFieldCalc();

  void CalculateGrid() const;

  fftwf_complex* GetAkMatrix() const {return fFFTin;}
  fftwf_complex* GetBkMatrix() const {return fBkMatrix;}
  fftwf_complex* GetRealSpaceMatrix() const {return fRealSpaceMatrix;}
  float* GetOmegaMatrix() const {return fOmegaMatrix;}
  float* GetBkSMatrix() const {return fBkS;}
  vector<float*> GetOmegaDiffMatrix() const {return fOmegaDiffMatrix;}
  fftwf_complex* GetQMatrix() const {return fQMatrix;}
  fftwf_complex* GetPMatrix() const {return fPkMatrix;}

private:

  void CalculateGradient() const;
  void CalculateSumAk() const;
  void FillAbrikosovCoefficients(const float) const;
  void ManipulateFourierCoefficientsA() const;
  void ManipulateFourierCoefficientsB() const;
  void ManipulateFourierCoefficientsForQx() const;
  void ManipulateFourierCoefficientsForQy() const;
  void ManipulateFourierCoefficientsForBperpX() const;
  void ManipulateFourierCoefficientsForBperpY() const;
  void CalculateGatVortexCore() const;

  mutable float *fOmegaMatrix;
  mutable vector<float*> fOmegaDiffMatrix;
  mutable fftwf_complex *fRealSpaceMatrix;
  mutable fftwf_complex *fBkMatrix;
  mutable fftwf_complex *fPkMatrix;
  mutable fftwf_complex *fQMatrix;
  mutable fftwf_complex *fQMatrixA;
  mutable fftwf_complex *fSumAkFFTin;
  mutable fftwf_complex *fSumAk;
  mutable float *fBkS;
  mutable float *fGstorage;

  mutable float *fCheckAkConvergence;
  mutable float *fCheckBkConvergence;

  mutable float fLatticeConstant;
  mutable float fThickness;
  mutable float fKappa;
  mutable float fSumOmegaSq;
  mutable float fSumSum;
  mutable float fC;

  mutable bool fFind3dSolution;

  fftwf_plan fFFTplanBkToBandQ;
  fftwf_plan fFFTplanOmegaToAk;
  fftwf_plan fFFTplanOmegaToBk;
  fftwf_plan fFFTplanForSumAk;
  fftwf_plan fFFTplanForPk1;
  fftwf_plan fFFTplanForPk2;
  

};

#endif // _TFilmTriVortexFieldCalc_H_
