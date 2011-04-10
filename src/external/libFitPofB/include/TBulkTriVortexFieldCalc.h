/***************************************************************************

  TBulkTriVortexFieldCalc.h

  Author: Bastian M. Wojek

  $Id$

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

#ifndef _TBulkVortexFieldCalc_H_
#define _TBulkVortexFieldCalc_H_

#include <vector>
#include <string>
using namespace std;

#include "fftw3.h"

/**
 * <p>Base class for the calculation of the spatial field distribution B(x,y) within a 2D vortex lattice of arbitrary symmetry
 */
class TBulkVortexFieldCalc {

public:

  TBulkVortexFieldCalc() {}

  virtual ~TBulkVortexFieldCalc();

  virtual double* DataB() const {return fFFTout;}
  virtual void SetParameters(const vector<double>& par) {fParam = par; fGridExists = false;}
  virtual void CalculateGrid() const = 0;
  virtual double GetBmin() const;
  virtual double GetBmax() const;
  virtual bool GridExists() const {return fGridExists;}
  virtual void UnsetGridExists() const {fGridExists = false;}
  virtual unsigned int GetNumberOfSteps() const {return fSteps;}
  virtual bool IsTriangular() const = 0;

protected:
  vector<double> fParam; ///< parameters used to calculate B(x,y)
  unsigned int fSteps;  ///< number of steps in which the "unit cell" of the vortex lattice is devided in (in each direction)
  mutable fftw_complex *fFFTin; ///< Fourier components of the field
  mutable double *fFFTout; ///< spatial field distribution B(x,y) in a "unit cell" of the vortex lattice
  fftw_plan fFFTplan; ///< FFTW plan for the 2D-Fourier transform from Fourier space to real space
  bool fUseWisdom; ///< tag determining if FFTW wisdom is used
  string fWisdom; ///< file name of the FFTW wisdom-file
  mutable bool fGridExists; ///< tag determining if B(x,y) has been calculated for the given set of parameters
};

/**
 * <p>Class for the calculation of the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * using the London model with a Gaussian cutoff
 */
class TBulkTriVortexLondonFieldCalc : public TBulkVortexFieldCalc {

public:

  TBulkTriVortexLondonFieldCalc(const string&, const unsigned int steps = 256);
  ~TBulkTriVortexLondonFieldCalc() {}

  void CalculateGrid() const;
  bool IsTriangular() const {return true;}

};

/**
 * <p>Class for the calculation of the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * using the London model with a Gaussian cutoff for an anisotropic superconductor with the field applied along one of the principal axes 
 */
class TBulkAnisotropicTriVortexLondonFieldCalc : public TBulkVortexFieldCalc {

public:

  TBulkAnisotropicTriVortexLondonFieldCalc(const string&, const unsigned int steps = 256);
  ~TBulkAnisotropicTriVortexLondonFieldCalc() {}

  void CalculateGrid() const;
  bool IsTriangular() const {return true;}

};

/**
 * <p>Class for the calculation of the spatial field distribution B(x,y) within a 2D square vortex lattice
 * using the London model with a Gaussian cutoff
 */
class TBulkSqVortexLondonFieldCalc : public TBulkVortexFieldCalc {

public:

  TBulkSqVortexLondonFieldCalc(const string&, const unsigned int steps = 256);
  ~TBulkSqVortexLondonFieldCalc() {}

  void CalculateGrid() const;
   bool IsTriangular() const {return false;}

};

/**
 * <p>Class for the calculation of the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * using the modified London model with a Gaussian cutoff
 */
class TBulkTriVortexMLFieldCalc : public TBulkVortexFieldCalc {

public:

  TBulkTriVortexMLFieldCalc(const string&, const unsigned int steps = 256);
  ~TBulkTriVortexMLFieldCalc() {}

  void CalculateGrid() const;
  bool IsTriangular() const {return true;}

};

/**
 * <p>Class for the calculation of the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * using the modified London model with a Gaussian cutoff for an anisotropic superconductor with the field applied 
 * along one of the principal axes 
 */
class TBulkAnisotropicTriVortexMLFieldCalc : public TBulkVortexFieldCalc {

public:

  TBulkAnisotropicTriVortexMLFieldCalc(const string&, const unsigned int steps = 256);
  ~TBulkAnisotropicTriVortexMLFieldCalc() {}

  void CalculateGrid() const;
  bool IsTriangular() const {return true;}

};

/**
 * <p>Class for the calculation of the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * using the analytical Ginzburg-Landau approximation
 */
class TBulkTriVortexAGLFieldCalc : public TBulkVortexFieldCalc {

public:

  TBulkTriVortexAGLFieldCalc(const string&, const unsigned int steps = 256);
  ~TBulkTriVortexAGLFieldCalc() {}

  void CalculateGrid() const;
  bool IsTriangular() const {return true;}

};

/**
 * <p>Class for the calculation of the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * using an analytical Ginzburg-Landau approximation for tiny applied fields
 */
class TBulkTriVortexAGLIIFieldCalc : public TBulkVortexFieldCalc {

public:

  TBulkTriVortexAGLIIFieldCalc(const string&, const unsigned int steps = 256);
  ~TBulkTriVortexAGLIIFieldCalc() {}

  void CalculateGrid() const;
  bool IsTriangular() const {return true;}

};

/**
 * <p>Class for the calculation of the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * using the analytical Ginzburg-Landau approximation for an anisotropic superconductor with the field applied 
 * along one of the principal axes
 */
class TBulkAnisotropicTriVortexAGLFieldCalc : public TBulkVortexFieldCalc {

public:

  TBulkAnisotropicTriVortexAGLFieldCalc(const string&, const unsigned int steps = 256);
  ~TBulkAnisotropicTriVortexAGLFieldCalc() {}

  void CalculateGrid() const;
  bool IsTriangular() const {return true;}

};


/**
 * <p>Class for the calculation of the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * using an iterative minimization of the Ginzburg-Landau free energy after E.H. Brandt
 */
class TBulkTriVortexNGLFieldCalc : public TBulkVortexFieldCalc {

public:

  TBulkTriVortexNGLFieldCalc(const string&, const unsigned int steps = 256);
  ~TBulkTriVortexNGLFieldCalc();

  void CalculateGrid() const;

  fftw_complex* GetAkMatrix() const {return fFFTin;}
  fftw_complex* GetBkMatrix() const {return fBkMatrix;}
  double* GetOmegaMatrix() const {return fOmegaMatrix;}
  double* GetBMatrix() const {return fFFTout;}
  fftw_complex* GetOmegaDiffMatrix() const {return fOmegaDiffMatrix;}
  fftw_complex* GetQMatrix() const {return fQMatrix;}
  bool IsTriangular() const {return true;}

private:

  void CalculateGradient() const;
  void CalculateSumAk() const;
  void FillAbrikosovCoefficients() const;
  void ManipulateFourierCoefficientsA() const;
  void ManipulateFourierCoefficientsB() const;
  void ManipulateFourierCoefficientsForQx() const;
  void ManipulateFourierCoefficientsForQy() const;

  mutable double *fOmegaMatrix; ///< spatial distribution of the superconducting order parameter omega(x,y)
  mutable fftw_complex *fOmegaDiffMatrix; ///< complex matrix holding the gradient of omega(x,y): real -> d/dx, imaginary -> d/dy
  mutable fftw_complex *fBkMatrix; ///< complex matrix holding the Fourier coefficients of the field
  mutable fftw_complex *fRealSpaceMatrix; ///< complex matrix holding intermediate results of the calculations
  mutable fftw_complex *fQMatrix; ///< complex matrix holding the two components of the supervelocity
  mutable fftw_complex *fQMatrixA; ///< complex matrix for the two components of the supervelocity for Abrikosov's vortex lattice

  mutable double *fCheckAkConvergence; ///< array used for a convergence check in the calculation of the order parameter
  mutable double *fCheckBkConvergence; ///< array used for a convergence check in the calculation of the spatial field distribution

  mutable double fLatticeConstant; ///< inter-vortex distance
  mutable double fKappa; ///< Ginzburg-Landau parameter kappa=lambda/xi
  mutable double fSumAk; ///< sum of the Fourier components of the superconducting order parameter
  mutable double fSumOmegaSq; ///< average of the square of the superconducting order parameter
  mutable double fSumSum; ///< variable holding intermediate results
  fftw_plan fFFTplanBkToBandQ; ///< FFTW plan for the 2D-Fourier transform from Fourier space to real space
  fftw_plan fFFTplanOmegaToAk; ///< FFTW plan for the 2D-Fourier transform from real space to Fourier space
  fftw_plan fFFTplanOmegaToBk; ///< FFTW plan for the 2D-Fourier transform from real space to Fourier space

};

#endif // _TBulkTriVortexFieldCalc_H_
