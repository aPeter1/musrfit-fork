/***************************************************************************

  TBNMR.h

  Author: Zaher Salman
  e-mail: zaher.salman@psi.ch

  2010/09/02

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010 by Zaher Salman                                *
 *   zaher.salman@psi.ch                                                  *
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
#include "TF1.h"
#include "Math/WrappedTF1.h"
#include "Math/GaussIntegrator.h"
#include <cassert>
#include <cmath>
#include <vector>

#ifndef LIBBNMRH
#define LIBBNMRH

#define tau_Li 1.210     // In seconds 
#define PI 3.14159265358979323846
#define TWOPI 6.28318530717958647692

using namespace std;


class ExpRlx : public PUserFcnBase {

public:
  // default constructor and destructor
  ExpRlx(){}
  ~ExpRlx(){}

  // function operator
  double operator()(double, const std::vector<double>&) const;

  // definition of the class for the ROOT-dictionary
  ClassDef(ExpRlx,1)
};

class SExpRlx : public PUserFcnBase {

public:
  // default constructor and destructor
  SExpRlx(){}
  ~SExpRlx(){}

  // function operator
  double operator()(double, const std::vector<double>&) const;
private:
  static TF1 sexp1;
  static TF1 sexp2;

  // definition of the class for the ROOT-dictionary
  ClassDef(SExpRlx,1)
};


#endif //LIBBNMRH
