/***************************************************************************

  TGapIntegrals.h

  Author: Bastian M. Wojek

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

#include "PUserFcnBase.h"
#include "BMWIntegrator.h"

//--------------------------------------------------------------------
/**
 * <p>
 */
class TGapSWave : public PUserFcnBase {

public:
  TGapSWave();
  virtual ~TGapSWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapIntegral *fGapIntegral;
  mutable std::vector<double> fTemp;
  mutable std::vector<double>::const_iterator fTempIter;
  mutable std::vector<double> fIntegralValues;
  mutable std::vector<bool> fCalcNeeded;

  mutable std::vector<double> fPar;

  ClassDef(TGapSWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TGapPointPWave : public PUserFcnBase {

public:
  TGapPointPWave();
  virtual ~TGapPointPWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TPointPWaveGapIntegralCuhre *fGapIntegral;
  mutable std::vector<double> fTemp;
  mutable std::vector<double>::const_iterator fTempIter;
  mutable std::vector<double> fIntegralValues;
  mutable std::vector<bool> fCalcNeeded;

  mutable std::vector<double> fPar;

  ClassDef(TGapPointPWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TGapLinePWave : public PUserFcnBase {

public:
  TGapLinePWave();
  virtual ~TGapLinePWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TLinePWaveGapIntegralCuhre *fGapIntegral;
  mutable std::vector<double> fTemp;
  mutable std::vector<double>::const_iterator fTempIter;
  mutable std::vector<double> fIntegralValues;
  mutable std::vector<bool> fCalcNeeded;

  mutable std::vector<double> fPar;

  ClassDef(TGapLinePWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TGapDWave : public PUserFcnBase {

public:
  TGapDWave();
  virtual ~TGapDWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TDWaveGapIntegralCuhre *fGapIntegral;
  mutable std::vector<double> fTemp;
  mutable std::vector<double>::const_iterator fTempIter;
  mutable std::vector<double> fIntegralValues;
  mutable std::vector<bool> fCalcNeeded;

  mutable std::vector<double> fPar;

  ClassDef(TGapDWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TGapCosSqDWave : public PUserFcnBase {

public:
  TGapCosSqDWave();
  virtual ~TGapCosSqDWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TCosSqDWaveGapIntegralCuhre *fGapIntegral;
  mutable std::vector<double> fTemp;
  mutable std::vector<double>::const_iterator fTempIter;
  mutable std::vector<double> fIntegralValues;
  mutable std::vector<bool> fCalcNeeded;

  mutable std::vector<double> fPar;

  ClassDef(TGapCosSqDWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TGapSinSqDWave : public PUserFcnBase {

public:
  TGapSinSqDWave();
  virtual ~TGapSinSqDWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TSinSqDWaveGapIntegralCuhre *fGapIntegral;
  mutable std::vector<double> fTemp;
  mutable std::vector<double>::const_iterator fTempIter;
  mutable std::vector<double> fIntegralValues;
  mutable std::vector<bool> fCalcNeeded;

  mutable std::vector<double> fPar;

  ClassDef(TGapSinSqDWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TGapAnSWave : public PUserFcnBase {

public:
  TGapAnSWave();
  virtual ~TGapAnSWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TAnSWaveGapIntegralCuhre *fGapIntegral;
  mutable std::vector<double> fTemp;
  mutable std::vector<double>::const_iterator fTempIter;
  mutable std::vector<double> fIntegralValues;
  mutable std::vector<bool> fCalcNeeded;

  mutable std::vector<double> fPar;

  ClassDef(TGapAnSWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TGapNonMonDWave1 : public PUserFcnBase {

public:
  TGapNonMonDWave1();
  virtual ~TGapNonMonDWave1();

  double operator()(double, const std::vector<double>&) const;

private:
  TNonMonDWave1GapIntegralCuhre *fGapIntegral;
  mutable std::vector<double> fTemp;
  mutable std::vector<double>::const_iterator fTempIter;
  mutable std::vector<double> fIntegralValues;
  mutable std::vector<bool> fCalcNeeded;

  mutable std::vector<double> fPar;

  ClassDef(TGapNonMonDWave1,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TGapNonMonDWave2 : public PUserFcnBase {

public:
  TGapNonMonDWave2();
  virtual ~TGapNonMonDWave2();

  double operator()(double, const std::vector<double>&) const;

private:
  TNonMonDWave2GapIntegralCuhre *fGapIntegral;
  mutable std::vector<double> fTemp;
  mutable std::vector<double>::const_iterator fTempIter;
  mutable std::vector<double> fIntegralValues;
  mutable std::vector<bool> fCalcNeeded;

  mutable std::vector<double> fPar;

  ClassDef(TGapNonMonDWave2,1)
};


//--------------------------------------------------------------------
/**
 * <p>
 */
class TGapPowerLaw : public PUserFcnBase {

public:
  TGapPowerLaw() {}
  virtual ~TGapPowerLaw() {}

  double operator()(double, const std::vector<double>&) const;

private:

  ClassDef(TGapPowerLaw,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TGapDirtySWave : public PUserFcnBase {

public:
  TGapDirtySWave() {}
  virtual ~TGapDirtySWave() {}

  double operator()(double, const std::vector<double>&) const;

private:

  ClassDef(TGapDirtySWave,1)
};


//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaSWave : public PUserFcnBase {

public:
  TLambdaSWave();
  virtual ~TLambdaSWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapSWave *fLambdaInvSq;

  ClassDef(TLambdaSWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaPointPWave : public PUserFcnBase {

public:
  TLambdaPointPWave();
  virtual ~TLambdaPointPWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapPointPWave *fLambdaInvSq;

  ClassDef(TLambdaPointPWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaLinePWave : public PUserFcnBase {

public:
  TLambdaLinePWave();
  virtual ~TLambdaLinePWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapLinePWave *fLambdaInvSq;

  ClassDef(TLambdaLinePWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaDWave : public PUserFcnBase {

public:
  TLambdaDWave();
  virtual ~TLambdaDWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapDWave *fLambdaInvSq;

  ClassDef(TLambdaDWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaAnSWave : public PUserFcnBase {

public:
  TLambdaAnSWave();
  virtual ~TLambdaAnSWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapAnSWave *fLambdaInvSq;

  ClassDef(TLambdaAnSWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaNonMonDWave1 : public PUserFcnBase {

public:
  TLambdaNonMonDWave1();
  virtual ~TLambdaNonMonDWave1();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapNonMonDWave1 *fLambdaInvSq;

  ClassDef(TLambdaNonMonDWave1,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaNonMonDWave2 : public PUserFcnBase {

public:
  TLambdaNonMonDWave2();
  virtual ~TLambdaNonMonDWave2();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapNonMonDWave2 *fLambdaInvSq;

  ClassDef(TLambdaNonMonDWave2,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaPowerLaw : public PUserFcnBase {

public:
  TLambdaPowerLaw() {}
  virtual ~TLambdaPowerLaw() {}

  double operator()(double, const std::vector<double>&) const;

private:

  ClassDef(TLambdaPowerLaw,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaInvSWave : public PUserFcnBase {

public:
  TLambdaInvSWave();
  virtual ~TLambdaInvSWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapSWave *fLambdaInvSq;

  ClassDef(TLambdaInvSWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaInvPointPWave : public PUserFcnBase {

public:
  TLambdaInvPointPWave();
  virtual ~TLambdaInvPointPWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapPointPWave *fLambdaInvSq;

  ClassDef(TLambdaInvPointPWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaInvLinePWave : public PUserFcnBase {

public:
  TLambdaInvLinePWave();
  virtual ~TLambdaInvLinePWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapLinePWave *fLambdaInvSq;

  ClassDef(TLambdaInvLinePWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaInvDWave : public PUserFcnBase {

public:
  TLambdaInvDWave();
  virtual ~TLambdaInvDWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapDWave *fLambdaInvSq;

  ClassDef(TLambdaInvDWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaInvAnSWave : public PUserFcnBase {

public:
  TLambdaInvAnSWave();
  virtual ~TLambdaInvAnSWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapAnSWave *fLambdaInvSq;

  ClassDef(TLambdaInvAnSWave,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaInvNonMonDWave1 : public PUserFcnBase {

public:
  TLambdaInvNonMonDWave1();
  virtual ~TLambdaInvNonMonDWave1();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapNonMonDWave1 *fLambdaInvSq;

  ClassDef(TLambdaInvNonMonDWave1,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaInvNonMonDWave2 : public PUserFcnBase {

public:
  TLambdaInvNonMonDWave2();
  virtual ~TLambdaInvNonMonDWave2();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapNonMonDWave2 *fLambdaInvSq;

  ClassDef(TLambdaInvNonMonDWave2,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TLambdaInvPowerLaw : public PUserFcnBase {

public:
  TLambdaInvPowerLaw() {}
  virtual ~TLambdaInvPowerLaw() {}

  double operator()(double, const std::vector<double>&) const;

private:

  ClassDef(TLambdaInvPowerLaw,1)
};

//--------------------------------------------------------------------
/**
 * <p>
 */
class TFilmMagnetizationDWave : public PUserFcnBase {

public:
  TFilmMagnetizationDWave();
  virtual ~TFilmMagnetizationDWave();

  double operator()(double, const std::vector<double>&) const;

private:
  TGapDWave *fLambdaInvSq;

  mutable std::vector<double> fPar;

  ClassDef(TFilmMagnetizationDWave,1)
};


#endif //_TGapIntegrals_H_
