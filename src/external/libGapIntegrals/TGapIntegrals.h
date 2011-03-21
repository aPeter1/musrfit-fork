/***************************************************************************

  TGapIntegrals.h

  Author: Bastian M. Wojek

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

#ifndef _TGapIntegrals_H_
#define _TGapIntegrals_H_

#include<vector>
#include<cmath>

using namespace std;

#include "PUserFcnBase.h"
#include "BMWIntegrator.h"

class TGapSWave : public PUserFcnBase {

public:
  TGapSWave();
  virtual ~TGapSWave();

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
  virtual ~TGapDWave();

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
  virtual ~TGapCosSqDWave();

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
  virtual ~TGapSinSqDWave();

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
  virtual ~TGapAnSWave();

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
  virtual ~TGapNonMonDWave1();

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
  virtual ~TGapNonMonDWave2();

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
  virtual ~TGapPowerLaw() {}

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
  virtual ~TGapDirtySWave() {}

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
  virtual ~TLambdaSWave();

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
  virtual ~TLambdaDWave();

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
  virtual ~TLambdaAnSWave();

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
  virtual ~TLambdaNonMonDWave1();

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
  virtual ~TLambdaNonMonDWave2();

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
  virtual ~TLambdaPowerLaw() {}

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
  virtual ~TLambdaInvSWave();

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
  virtual ~TLambdaInvDWave();

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
  virtual ~TLambdaInvAnSWave();

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
  virtual ~TLambdaInvNonMonDWave1();

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
  virtual ~TLambdaInvNonMonDWave2();

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
  virtual ~TLambdaInvPowerLaw() {}

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
  virtual ~TFilmMagnetizationDWave();

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
