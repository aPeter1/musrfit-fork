/***************************************************************************

  TVortex.h

  Author: Bastian M. Wojek

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
  virtual void SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const std::vector<double>&) const;

private:
  mutable std::vector<double> fPar; ///< parameters of the model
  TBulkTriVortexLondonFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable std::vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable std::vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable std::vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  std::string fWisdom; ///< file name of the FFTW wisdom file
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
  virtual void SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const std::vector<double>&) const;

private:
  mutable std::vector<double> fPar; ///< parameters of the model
  TBulkSqVortexLondonFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable std::vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable std::vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable std::vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  std::string fWisdom; ///< file name of the FFTW wisdom file
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
  virtual void SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const std::vector<double>&) const;

private:
  mutable std::vector<double> fPar; ///< parameters of the model
  TBulkTriVortexMLFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable std::vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable std::vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable std::vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  std::string fWisdom; ///< file name of the FFTW wisdom file
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
  virtual void SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const std::vector<double>&) const;

private:
  mutable std::vector<double> fPar; ///< parameters of the model
  TBulkTriVortexAGLFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable std::vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable std::vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable std::vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  std::string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fGridSteps; ///< number of points in x- and y-direction for which B(x,y) is calculated

  ClassDef(TBulkTriVortexAGL,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating muon-spin depolarization functions
 * originating from the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * calculated using an analytical Ginzburg-Landau approximation for very small applied fields
 */
class TBulkTriVortexAGLII : public PUserFcnBase {

public:
  TBulkTriVortexAGLII();
  ~TBulkTriVortexAGLII();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const std::vector<double>&) const;

private:
  mutable std::vector<double> fPar; ///< parameters of the model
  TBulkTriVortexAGLIIFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable std::vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable std::vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable std::vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  std::string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fGridSteps; ///< number of points in x- and y-direction for which B(x,y) is calculated

  ClassDef(TBulkTriVortexAGLII,1)
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
  virtual void SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const std::vector<double>&) const;

private:
  mutable std::vector<double> fPar; ///< parameters of the model
  TBulkTriVortexNGLFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable std::vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable std::vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable std::vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  std::string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fGridSteps; ///< number of points in x- and y-direction for which B(x,y) is calculated

  ClassDef(TBulkTriVortexNGL,1)
};

/**
 * <p>Class implementing the global part of the musrfit user function interface for calculating muon spin depolarization functions
 * originating from the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * calculated using the London model with a Gaussian cutoff for an anisotropic superconductor with the field applied along
 * one of the principal axes
 */
class TBulkAnisotropicTriVortexLondonGlobal {

public:
  // default constructor and destructor
  TBulkAnisotropicTriVortexLondonGlobal();
  ~TBulkAnisotropicTriVortexLondonGlobal();

  bool IsValid() { return fValid; }

  void Calc(const std::vector<double>&) const;

  const TPofTCalc* GetPolarizationPointer() const { return fPofT; }

private:
  mutable bool fValid; ///< tag indicating if the global part has been calculated
  mutable std::vector<double> fPar; ///< parameter vector
  TBulkAnisotropicTriVortexLondonFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable std::vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable std::vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable std::vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  std::string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fGridSteps; ///< number of points in x- and y-direction for which B(x,y) is calculated

  // definition of the class for the ROOT-dictionary
  ClassDef(TBulkAnisotropicTriVortexLondonGlobal,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating muon spin depolarization functions
 * originating from the spatial field distribution B(x,y) within a 2D "triangular vortex lattice"
 * calculated using the London model with a Gaussian cutoff for an anisotropic superconductor with the field applied along
 * one of the principal axes
 */
class TBulkAnisotropicTriVortexLondon : public PUserFcnBase {

public:
  TBulkAnisotropicTriVortexLondon();
  ~TBulkAnisotropicTriVortexLondon();

  virtual bool NeedGlobalPart() const { return true; }
  virtual void SetGlobalPart(std::vector<void *> &globalPart, unsigned int idx);
  virtual bool GlobalPartIsValid() const;

  double operator()(double, const std::vector<double>&) const;

private:
  bool fValid;
  bool fInvokedGlobal;
  int  fIdxGlobal;
  TBulkAnisotropicTriVortexLondonGlobal *fGlobalUserFcn;

  ClassDef(TBulkAnisotropicTriVortexLondon,1)
};

/**
 * <p>Class implementing the global part of the musrfit user function interface for calculating muon spin depolarization functions
 * originating from the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * calculated using the modified London model with a Gaussian cutoff for an anisotropic superconductor with the field applied along
 * one of the principal axes
 */
class TBulkAnisotropicTriVortexMLGlobal {

public:
  // default constructor and destructor
  TBulkAnisotropicTriVortexMLGlobal();
  ~TBulkAnisotropicTriVortexMLGlobal();

  bool IsValid() { return fValid; }

  void Calc(const std::vector<double>&) const;

  const TPofTCalc* GetPolarizationPointer() const { return fPofT; }

private:
  mutable bool fValid; ///< tag indicating if the global part has been calculated
  mutable std::vector<double> fPar; ///< parameter vector
  TBulkAnisotropicTriVortexMLFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable std::vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable std::vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable std::vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  std::string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fGridSteps; ///< number of points in x- and y-direction for which B(x,y) is calculated

  // definition of the class for the ROOT-dictionary
  ClassDef(TBulkAnisotropicTriVortexMLGlobal,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating muon spin depolarization functions
 * originating from the spatial field distribution B(x,y) within a 2D "triangular vortex lattice"
 * calculated using the modified London model with a Gaussian cutoff for an anisotropic superconductor with the field applied along
 * one of the principal axes
 */
class TBulkAnisotropicTriVortexML : public PUserFcnBase {

public:
  TBulkAnisotropicTriVortexML();
  ~TBulkAnisotropicTriVortexML();

  virtual bool NeedGlobalPart() const { return true; }
  virtual void SetGlobalPart(std::vector<void *> &globalPart, unsigned int idx);
  virtual bool GlobalPartIsValid() const;

  double operator()(double, const std::vector<double>&) const;

private:
  bool fValid;
  bool fInvokedGlobal;
  int  fIdxGlobal;
  TBulkAnisotropicTriVortexMLGlobal *fGlobalUserFcn;

  ClassDef(TBulkAnisotropicTriVortexML,1)
};

/**
 * <p>Class implementing the global part of the musrfit user function interface for calculating muon spin depolarization functions
 * originating from the spatial field distribution B(x,y) within a 2D triangular vortex lattice
 * calculated using the analytical Ginzburg-Landau model for an anisotropic superconductor with the field applied along
 * one of the principal axes
 */
class TBulkAnisotropicTriVortexAGLGlobal {

public:
  // default constructor and destructor
  TBulkAnisotropicTriVortexAGLGlobal();
  ~TBulkAnisotropicTriVortexAGLGlobal();

  bool IsValid() { return fValid; }

  void Calc(const std::vector<double>&) const;

  const TPofTCalc* GetPolarizationPointer() const { return fPofT; }

private:
  mutable bool fValid; ///< tag indicating if the global part has been calculated
  mutable std::vector<double> fPar; ///< parameter vector
  TBulkAnisotropicTriVortexAGLFieldCalc *fVortex; ///< spatial field distribution B(x,y) within the vortex lattice
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable std::vector<double> fParForVortex; ///< parameters for the calculation of B(x,y)
  mutable std::vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  mutable std::vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  std::string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fGridSteps; ///< number of points in x- and y-direction for which B(x,y) is calculated

  // definition of the class for the ROOT-dictionary
  ClassDef(TBulkAnisotropicTriVortexAGLGlobal,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating muon spin depolarization functions
 * originating from the spatial field distribution B(x,y) within a 2D "triangular vortex lattice"
 * calculated using the analytical Ginzburg-Landau model for an anisotropic superconductor with the field applied along
 * one of the principal axes
 */
class TBulkAnisotropicTriVortexAGL : public PUserFcnBase {

public:
  TBulkAnisotropicTriVortexAGL();
  ~TBulkAnisotropicTriVortexAGL();

  virtual bool NeedGlobalPart() const { return true; }
  virtual void SetGlobalPart(std::vector<void *> &globalPart, unsigned int idx);
  virtual bool GlobalPartIsValid() const;

  double operator()(double, const std::vector<double>&) const;

private:
  bool fValid;
  bool fInvokedGlobal;
  int  fIdxGlobal;
  TBulkAnisotropicTriVortexAGLGlobal *fGlobalUserFcn;

  ClassDef(TBulkAnisotropicTriVortexAGL,1)
};

#endif //_TVortex_H_
