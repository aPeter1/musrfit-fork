/***************************************************************************

  TGapIntegrals.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/09/06

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
#include "TIntegrator.h"

class TGapSWave : public PUserFcnBase {

public:
  TGapSWave();
  ~TGapSWave();

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

class TGapAnSWave : public PUserFcnBase {

public:
  TGapAnSWave();
  ~TGapAnSWave();

  double operator()(double, const vector<double>&) const;

private:
  TAnSWaveGapIntegralSuave *fGapIntegral;
  mutable vector<double> fTemp;
  mutable vector<double>::const_iterator fTempIter;
  mutable vector<double> fIntegralValues;
  mutable vector<bool> fCalcNeeded;

  mutable vector<double> fPar;

  ClassDef(TGapAnSWave,1)
};

#endif //_TGapIntegrals_H_
