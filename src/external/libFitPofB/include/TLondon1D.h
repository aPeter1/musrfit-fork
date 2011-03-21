/***************************************************************************

  TLondon1D.h

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

#ifndef _TLondon1D_H_
#define _TLondon1D_H_

#include "PUserFcnBase.h"
#include "TPofTCalc.h"

/**
 * <p>Class implementing the musrfit user function interface for calculating low energy muon spin depolarization functions
 * using the 1D London model for Meissner screening in a superconducting half-space
 */
class TLondon1DHS : public PUserFcnBase {

public:
  // default constructor
  TLondon1DHS();
  ~TLondon1DHS();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar; ///< parameters of the model
  TTrimSPData *fImpProfile; ///< low energy muon implantation profiles
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  mutable vector<double> fParForBofZ; ///< parameters for the calculation of B(z)
  mutable vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fNSteps; ///< number of points for which B(z) is calculated

  ClassDef(TLondon1DHS,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating low energy muon spin depolarization functions
 * using the 1D London model for Meissner screening in a thin superconducting film
 */
class TLondon1D1L : public PUserFcnBase {

public:
  // default constructor
  TLondon1D1L();
  ~TLondon1D1L();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar; ///< parameters of the model
  TTrimSPData *fImpProfile; ///< low energy muon implantation profiles
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  mutable vector<double> fParForBofZ; ///< parameters for the calculation of B(z)
  mutable vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fNSteps; ///< number of points for which B(z) is calculated
  //mutable unsigned int fCallCounter;

  ClassDef(TLondon1D1L,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating low energy muon spin depolarization functions
 * using the 1D London model for Meissner screening in a thin superconducting film
 * consisting of two layers with different magnetic penetration depths
 */
class TLondon1D2L : public PUserFcnBase {

public:
  // default constructor
  TLondon1D2L();
  ~TLondon1D2L();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar; ///< parameters of the model
  TTrimSPData *fImpProfile; ///< low energy muon implantation profiles
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  mutable vector<double> fParForBofZ; ///< parameters for the calculation of B(z)
  mutable vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fNSteps; ///< number of points for which B(z) is calculated

  ClassDef(TLondon1D2L,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating low energy muon spin depolarization functions
 * using the model for Meissner screening in a conventionally proximated system
 * consisting of one metal layer and an underlying (London) superconducting half-space
 */
class TProximity1D1LHS : public PUserFcnBase {

public:
  // default constructor
  TProximity1D1LHS();
  ~TProximity1D1LHS();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar; ///< parameters of the model
  TTrimSPData *fImpProfile; ///< low energy muon implantation profiles
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  mutable vector<double> fParForBofZ; ///< parameters for the calculation of B(z)
  mutable vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fNSteps; ///< number of points for which B(z) is calculated

  ClassDef(TProximity1D1LHS,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating low energy muon spin depolarization functions
 * using the 1D London model for Meissner screening in a thin superconducting film
 * consisting of three layers with different magnetic penetration depths
 */
class TLondon1D3L : public PUserFcnBase {

public:
  // default constructor
  TLondon1D3L();
  ~TLondon1D3L();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar; ///< parameters of the model
  TTrimSPData *fImpProfile; ///< low energy muon implantation profiles
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  mutable vector<double> fParForBofZ; ///< parameters for the calculation of B(z)
  mutable vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fNSteps; ///< number of points for which B(z) is calculated

  ClassDef(TLondon1D3L,1)
};

/**
 * <p>Class implementing the musrfit user function interface for calculating low energy muon spin depolarization functions
 * using the 1D London model for Meissner screening in a thin superconducting film
 * consisting of three layers with different magnetic penetration depths (where lambda is the same for the two outer layers)
 */
class TLondon1D3LS : public PUserFcnBase {

public:
  // default constructor
  TLondon1D3LS();
  ~TLondon1D3LS();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar; ///< parameters of the model
  TTrimSPData *fImpProfile; ///< low energy muon implantation profiles
  TPofBCalc *fPofB; ///< static field distribution P(B
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  mutable vector<double> fParForBofZ; ///< parameters for the calculation of B(z)
  mutable vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  string fWisdom; ///< file name of the FFTW wisdom file
  unsigned int fNSteps; ///< number of points for which B(z) is calculated

  ClassDef(TLondon1D3LS,1)
};

// class TLondon1D4L : public PUserFcnBase {
// 
// public:
//   // default constructor
//   TLondon1D4L();
//   ~TLondon1D4L();
// 
//   double operator()(double, const vector<double>&) const;
// 
// private:
//   mutable vector<double> fPar;
//   TTrimSPData *fImpProfile;
//   TPofTCalc *fPofT;
//   mutable bool fCalcNeeded;
//   mutable bool fFirstCall;
//   mutable vector<double> fParForPofT;
//   mutable vector<double> fParForBofZ;
//   mutable vector<double> fParForPofB;
//   string fWisdom;
//   unsigned int fNSteps;
//   mutable bool fLastFourChanged;
// 
//   ClassDef(TLondon1D4L,1)
// };

#endif //_TLondon1D_H_
