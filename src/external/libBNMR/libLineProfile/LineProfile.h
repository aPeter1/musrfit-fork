/**************************************************************************/

/***************************************************************************
 *   Copyright (C) 2017 by Jonas A. Krieger                                *
 *   jonas.krieger@psi.ch                                                  *
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

#include "PUserFcnBase.h"
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <boost/math/special_functions/ellint_1.hpp>


#ifndef PI
#define PI           3.14159265358979323846  /* pi */
#endif //PI

#ifndef LINEPROFILE_H
#define LINEPROFILE_H

using namespace std;

//helper functions:
Double_t GaussianShape(Double_t, Double_t, Double_t);
Double_t LaplacianShape(Double_t, Double_t, Double_t);
Double_t LorentzianShape(Double_t, Double_t, Double_t);
Double_t IAxial(Double_t, Double_t, Double_t);
Double_t IAsym(Double_t, Double_t, Double_t,Double_t);
Double_t IAsym_low(Double_t, Double_t, Double_t, Double_t);
Double_t IAsym_high(Double_t, Double_t, Double_t, Double_t);


//
class LineGauss : public PUserFcnBase {

public:
  // function operator
  Double_t operator()(Double_t, const vector<Double_t>&) const;

  // definition of the class for the ROOT dictionary
  ClassDef(LineGauss,1)
};


class LineLaplace : public PUserFcnBase {

public:
  // function operator
  Double_t operator()(Double_t, const vector<Double_t>&) const;

  // definition of the class for the ROOT dictionary
  ClassDef(LineLaplace,1)
};


class LineLorentzian : public PUserFcnBase {

public:
  // function operator
  Double_t operator()(Double_t, const vector<Double_t>&) const;

  // definition of the class for the ROOT dictionary
  ClassDef(LineLorentzian,1)
};


class LineSkewLorentzian : public PUserFcnBase {

public:
  // function operator
  Double_t operator()(Double_t, const vector<Double_t>&) const;

  // definition of the class for the ROOT dictionary
  ClassDef(LineSkewLorentzian,1)
};



class LineSkewLorentzian2 : public PUserFcnBase {

public:
  // function operator
  Double_t operator()(Double_t, const vector<Double_t>&) const;

  // definition of the class for the ROOT dictionary
  ClassDef(LineSkewLorentzian2,1)
};



class PowderLineAxialLorGlobal {

public:
  // default constructor and destructor
  PowderLineAxialLorGlobal(){}
  virtual~PowderLineAxialLorGlobal(){}

  Bool_t IsValid() { return fValid; }

  // the following function will check if something needs to be calculated, which
  // is the case if param != fPrevParam
  void CalcNormalization(const vector<Double_t> &);

  // this routine returns the globally calculated values
  Double_t GetNormalization() const {return fNormalization;}
  UInt_t GetSteps() const {return 1e5;}

private:
  Bool_t fValid=true;
  vector<Double_t> fPrevParam;

  Double_t fNormalization;
  // definition of the class for the ROOT-dictionary
  ClassDef(PowderLineAxialLorGlobal,1)
};

class PowderLineAxialLor : public PUserFcnBase {

public:
  // default constructor and destructor
  PowderLineAxialLor();
  virtual ~PowderLineAxialLor();

  // global user-function-access functions, here with some functionality
  Bool_t NeedGlobalPart() const { return true; }
  void SetGlobalPart(vector<void*> &globalPart, UInt_t idx);
  Bool_t GlobalPartIsValid() const {return fValid;}

  // function operator
  Double_t operator()(Double_t, const vector<Double_t>&) const;

private:
  Bool_t fValid;
  Bool_t fInvokedGlobal;
  Int_t  fIdxGlobal;
  PowderLineAxialLorGlobal * fGlobalUserFcn;

  // definition of the class for the ROOT dictionary
  ClassDef(PowderLineAxialLor,1)
};


class PowderLineAxialGssGlobal {

public:
  // default constructor and destructor
  PowderLineAxialGssGlobal(){}
  virtual~PowderLineAxialGssGlobal(){}

  Bool_t IsValid() { return fValid; }

  // the following function will check if something needs to be calculated, which
  // is the case if param != fPrevParam
  void CalcNormalization(const vector<Double_t> &);

  // this routine returns the globally calculated values
  Double_t GetNormalization() const {return fNormalization;}
  UInt_t GetSteps() const {return 1e5;}

private:
  Bool_t fValid=true;
  vector<Double_t> fPrevParam;

  Double_t fNormalization;
  // definition of the class for the ROOT-dictionary
  ClassDef(PowderLineAxialGssGlobal,1)
};

class PowderLineAxialGss : public PUserFcnBase {

public:
  // default constructor and destructor
  PowderLineAxialGss();
  virtual ~PowderLineAxialGss();

  // global user-function-access functions, here with some functionality
  Bool_t NeedGlobalPart() const { return true; }
  void SetGlobalPart(vector<void*> &globalPart, UInt_t idx);
  Bool_t GlobalPartIsValid() const {return fValid;}

  // function operator
  Double_t operator()(Double_t, const vector<Double_t>&) const;

private:
  Bool_t fValid;
  Bool_t fInvokedGlobal;
  Int_t  fIdxGlobal;
  PowderLineAxialGssGlobal * fGlobalUserFcn;

  // definition of the class for the ROOT dictionary
  ClassDef(PowderLineAxialGss,1)
};



class PowderLineAsymLorGlobal {

public:
  // default constructor and destructor
  PowderLineAsymLorGlobal(){}
  virtual~PowderLineAsymLorGlobal(){}

  Bool_t IsValid() { return fValid; }

  // the following function will check if something needs to be calculated, which
  // is the case if param != fPrevParam
  void UpdateGlobalPart(const vector<Double_t> &);

  // this routine returns the globally calculated values
  Double_t GetNorm() const {return fNormalization;}
  Double_t GetCenter() const {return fOmegaCenter;}
  Double_t GetMin() const {return fOmegaMin;}
  Double_t GetMax() const {return fOmegaMax;}
  UInt_t GetSteps() const {return 1e5;}

private:
  Bool_t fValid=true;
  vector<Double_t> fPrevParam;
  Double_t fNormalization;
  Double_t fOmegaCenter;
  Double_t fOmegaMin;
  Double_t fOmegaMax;
  // definition of the class for the ROOT-dictionary
  ClassDef(PowderLineAsymLorGlobal,1)
};

class PowderLineAsymLor : public PUserFcnBase {

public:
  // default constructor and destructor
  PowderLineAsymLor();
  virtual ~PowderLineAsymLor();

  // global user-function-access functions, here with some functionality
  Bool_t NeedGlobalPart() const { return true; }
  void SetGlobalPart(vector<void*> &globalPart, UInt_t idx);
  Bool_t GlobalPartIsValid() const {return fValid;}

  // function operator
  Double_t operator()(Double_t, const vector<Double_t>&) const;

private:
  Bool_t fValid;
  Bool_t fInvokedGlobal;
  Int_t  fIdxGlobal;
  PowderLineAsymLorGlobal * fGlobalUserFcn;

  // definition of the class for the ROOT dictionary
  ClassDef(PowderLineAsymLor,1)
};


class PowderLineAsymGssGlobal {

public:
  // default constructor and destructor
  PowderLineAsymGssGlobal(){}
  virtual~PowderLineAsymGssGlobal(){}

  Bool_t IsValid() { return fValid; }

  // the following function will check if something needs to be calculated, which
  // is the case if param != fPrevParam
  void UpdateGlobalPart(const vector<Double_t> &);

  // this routine returns the globally calculated values
  Double_t GetNorm() const {return fNormalization;}
  Double_t GetCenter() const {return fOmegaCenter;}
  Double_t GetMin() const {return fOmegaMin;}
  Double_t GetMax() const {return fOmegaMax;}
  UInt_t GetSteps() const {return 1e5;}

private:
  Bool_t fValid=true;
  vector<Double_t> fPrevParam;
  Double_t fNormalization;
  Double_t fOmegaCenter;
  Double_t fOmegaMin;
  Double_t fOmegaMax;
  // definition of the class for the ROOT-dictionary
  ClassDef(PowderLineAsymGssGlobal,1)
};

class PowderLineAsymGss : public PUserFcnBase {

public:
  // default constructor and destructor
  PowderLineAsymGss();
  virtual ~PowderLineAsymGss();

  // global user-function-access functions, here with some functionality
  Bool_t NeedGlobalPart() const { return true; }
  void SetGlobalPart(vector<void*> &globalPart, UInt_t idx);
  Bool_t GlobalPartIsValid() const {return fValid;}

  // function operator
  Double_t operator()(Double_t, const vector<Double_t>&) const;

private:
  Bool_t fValid;
  Bool_t fInvokedGlobal;
  Int_t  fIdxGlobal;
  PowderLineAsymGssGlobal * fGlobalUserFcn;

  // definition of the class for the ROOT dictionary
  ClassDef(PowderLineAsymGss,1)
};




#endif //LINEPROFILE_H
