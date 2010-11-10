/***************************************************************************

  TGapIntegrals.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Bastian M. Wojek                                *
 *   bastian.wojek@psi.ch                                                  *
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

#ifndef _TTwoGaps_H_
#define _TTwoGaps_H_

#include<vector>
#include<cmath>

using namespace std;

#include "PUserFcnBase.h"
#include "../BMWIntegrator/BMWIntegrator.h"

class TGapSWave : public PUserFcnBase {

public:
  TGapSWave();
  ~TGapSWave();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TGapIntegral *fGapIntegral;
  mutable vector<double> fTemp;
  mutable vector<double>::const_iterator fTempIter;
  mutable vector<double> fIntegralValues;
  mutable vector<bool> fCalcNeeded;

  mutable vector<double> fPar;

  ClassDef(TGapSWave,1)
};

class TGapDWave : public PUserFcnBase {

public:
  TGapDWave();
  ~TGapDWave();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TDWaveGapIntegralCuhre *fGapIntegral;
  mutable vector<double> fTemp;
  mutable vector<double>::const_iterator fTempIter;
  mutable vector<double> fIntegralValues;
  mutable vector<bool> fCalcNeeded;

  mutable vector<double> fPar;

  ClassDef(TGapDWave,1)
};

class TGapCosSqDWave : public PUserFcnBase {

public:
  TGapCosSqDWave();
  ~TGapCosSqDWave();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TCosSqDWaveGapIntegralCuhre *fGapIntegral;
  mutable vector<double> fTemp;
  mutable vector<double>::const_iterator fTempIter;
  mutable vector<double> fIntegralValues;
  mutable vector<bool> fCalcNeeded;

  mutable vector<double> fPar;

  ClassDef(TGapCosSqDWave,1)
};

class TGapSinSqDWave : public PUserFcnBase {

public:
  TGapSinSqDWave();
  ~TGapSinSqDWave();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TSinSqDWaveGapIntegralCuhre *fGapIntegral;
  mutable vector<double> fTemp;
  mutable vector<double>::const_iterator fTempIter;
  mutable vector<double> fIntegralValues;
  mutable vector<bool> fCalcNeeded;

  mutable vector<double> fPar;

  ClassDef(TGapSinSqDWave,1)
};


class TGapAnSWave : public PUserFcnBase {

public:
  TGapAnSWave();
  ~TGapAnSWave();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TAnSWaveGapIntegralCuhre *fGapIntegral;
  mutable vector<double> fTemp;
  mutable vector<double>::const_iterator fTempIter;
  mutable vector<double> fIntegralValues;
  mutable vector<bool> fCalcNeeded;

  mutable vector<double> fPar;

  ClassDef(TGapAnSWave,1)
};

class TGapNonMonDWave1 : public PUserFcnBase {

public:
  TGapNonMonDWave1();
  ~TGapNonMonDWave1();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TNonMonDWave1GapIntegralCuhre *fGapIntegral;
  mutable vector<double> fTemp;
  mutable vector<double>::const_iterator fTempIter;
  mutable vector<double> fIntegralValues;
  mutable vector<bool> fCalcNeeded;

  mutable vector<double> fPar;

  ClassDef(TGapNonMonDWave1,1)
};

class TGapNonMonDWave2 : public PUserFcnBase {

public:
  TGapNonMonDWave2();
  ~TGapNonMonDWave2();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TNonMonDWave2GapIntegralCuhre *fGapIntegral;
  mutable vector<double> fTemp;
  mutable vector<double>::const_iterator fTempIter;
  mutable vector<double> fIntegralValues;
  mutable vector<bool> fCalcNeeded;

  mutable vector<double> fPar;

  ClassDef(TGapNonMonDWave2,1)
};


class TGapPowerLaw : public PUserFcnBase {

public:
  TGapPowerLaw() {}
  ~TGapPowerLaw() {}

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:

  ClassDef(TGapPowerLaw,1)
};

class TGapDirtySWave : public PUserFcnBase {

public:
  TGapDirtySWave() {}
  ~TGapDirtySWave() {}

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:

  ClassDef(TGapDirtySWave,1)
};


class TLambdaSWave : public PUserFcnBase {

public:
  TLambdaSWave();
  ~TLambdaSWave();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TGapSWave *fLambdaInvSq;

  ClassDef(TLambdaSWave,1)
};

class TLambdaDWave : public PUserFcnBase {

public:
  TLambdaDWave();
  ~TLambdaDWave();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TGapDWave *fLambdaInvSq;

  ClassDef(TLambdaDWave,1)
};

class TLambdaAnSWave : public PUserFcnBase {

public:
  TLambdaAnSWave();
  ~TLambdaAnSWave();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TGapAnSWave *fLambdaInvSq;

  ClassDef(TLambdaAnSWave,1)
};

class TLambdaNonMonDWave1 : public PUserFcnBase {

public:
  TLambdaNonMonDWave1();
  ~TLambdaNonMonDWave1();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TGapNonMonDWave1 *fLambdaInvSq;

  ClassDef(TLambdaNonMonDWave1,1)
};

class TLambdaNonMonDWave2 : public PUserFcnBase {

public:
  TLambdaNonMonDWave2();
  ~TLambdaNonMonDWave2();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TGapNonMonDWave2 *fLambdaInvSq;

  ClassDef(TLambdaNonMonDWave2,1)
};


class TLambdaPowerLaw : public PUserFcnBase {

public:
  TLambdaPowerLaw() {}
  ~TLambdaPowerLaw() {}

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:

  ClassDef(TLambdaPowerLaw,1)
};

class TLambdaInvSWave : public PUserFcnBase {

public:
  TLambdaInvSWave();
  ~TLambdaInvSWave();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TGapSWave *fLambdaInvSq;

  ClassDef(TLambdaInvSWave,1)
};

class TLambdaInvDWave : public PUserFcnBase {

public:
  TLambdaInvDWave();
  ~TLambdaInvDWave();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TGapDWave *fLambdaInvSq;

  ClassDef(TLambdaInvDWave,1)
};

class TLambdaInvAnSWave : public PUserFcnBase {

public:
  TLambdaInvAnSWave();
  ~TLambdaInvAnSWave();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TGapAnSWave *fLambdaInvSq;

  ClassDef(TLambdaInvAnSWave,1)
};

class TLambdaInvNonMonDWave1 : public PUserFcnBase {

public:
  TLambdaInvNonMonDWave1();
  ~TLambdaInvNonMonDWave1();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TGapNonMonDWave1 *fLambdaInvSq;

  ClassDef(TLambdaInvNonMonDWave1,1)
};

class TLambdaInvNonMonDWave2 : public PUserFcnBase {

public:
  TLambdaInvNonMonDWave2();
  ~TLambdaInvNonMonDWave2();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TGapNonMonDWave2 *fLambdaInvSq;

  ClassDef(TLambdaInvNonMonDWave2,1)
};


class TLambdaInvPowerLaw : public PUserFcnBase {

public:
  TLambdaInvPowerLaw() {}
  ~TLambdaInvPowerLaw() {}

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:

  ClassDef(TLambdaInvPowerLaw,1)
};

class TFilmMagnetizationDWave : public PUserFcnBase {

public:
  TFilmMagnetizationDWave();
  ~TFilmMagnetizationDWave();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TGapDWave *fLambdaInvSq;

  mutable vector<double> fPar;

  ClassDef(TFilmMagnetizationDWave,1)
};


#endif //_TGapIntegrals_H_
