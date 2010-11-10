/***************************************************************************

  TVortex.h

  Author: Bastian M. Wojek
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

#ifndef _TVortex_H_
#define _TVortex_H_

#include "PUserFcnBase.h"
#include "TPofTCalc.h"

/**
 * <p>Class implementing the musrfit user function interface for calculating muon spin depolarization functions
 * originating from the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * calculated using the London model with a Gaussian cutoff
 */
class TBulkTriVortexLondon : public PUserFcnBase {

public:
  TBulkTriVortexLondon();
  ~TBulkTriVortexLondon();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar; ///< parameters of the model
  TBulkTriVortexLondonFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fGridSteps; ///< number of points in x- and y-direction for which B(x,y) is calculated

  ClassDef(TBulkTriVortexLondon,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating muon spin depolarization functions
 * originating from the spatial field distribution B(x,y) within a 2D square vortex lattice
 * calculated using the London model with a Gaussian cutoff
 */
class TBulkSqVortexLondon : public PUserFcnBase {

public:
  TBulkSqVortexLondon();
  ~TBulkSqVortexLondon();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar; ///< parameters of the model
  TBulkSqVortexLondonFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fGridSteps; ///< number of points in x- and y-direction for which B(x,y) is calculated

  ClassDef(TBulkSqVortexLondon,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating muon spin depolarization functions
 * originating from the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * calculated using the modified London model with a Gaussian cutoff
 */
class TBulkTriVortexML : public PUserFcnBase {

public:
  TBulkTriVortexML();
  ~TBulkTriVortexML();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar; ///< parameters of the model
  TBulkTriVortexMLFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fGridSteps; ///< number of points in x- and y-direction for which B(x,y) is calculated

  ClassDef(TBulkTriVortexML,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating muon spin depolarization functions
 * originating from the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * calculated using the analytical Ginzburg-Landau approximation
 */
class TBulkTriVortexAGL : public PUserFcnBase {

public:
  TBulkTriVortexAGL();
  ~TBulkTriVortexAGL();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar; ///< parameters of the model
  TBulkTriVortexAGLFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fGridSteps; ///< number of points in x- and y-direction for which B(x,y) is calculated

  ClassDef(TBulkTriVortexAGL,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating muon spin depolarization functions
 * originating from the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * calculated using the iterative minimization of the Ginzburg-Landau free energy after E.H. Brandt
 */
class TBulkTriVortexNGL : public PUserFcnBase {

public:
  TBulkTriVortexNGL();
  ~TBulkTriVortexNGL();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar; ///< parameters of the model
  TBulkTriVortexNGLFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fGridSteps; ///< number of points in x- and y-direction for which B(x,y) is calculated

  ClassDef(TBulkTriVortexNGL,1)
};

#endif //_TLondon1D_H_
