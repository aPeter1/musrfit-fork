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
#include <cassert>
#include <cmath>
#include <vector>

using namespace std;

class TBNMR : public PUserFcnBase {

public:
  // default constructor and destructor
  TBNMR(){}
  ~TBNMR(){}

  // function operator
  double operator()(double, const vector<double>&) const;

  // definition of the class for the ROOT-dictionary
  ClassDef(TBNMR,1)
};

class ExpRlx : public PUserFcnBase {

public:
  // default constructor and destructor
  ExpRlx(){}
  ~ExpRlx(){}

  // function operator
  double operator()(double, const vector<double>&) const;

  // definition of the class for the ROOT-dictionary
  ClassDef(ExpRlx,1)
};

class SExpRlx : public PUserFcnBase {

public:
  // default constructor and destructor
  SExpRlx(){}
  ~SExpRlx(){}

  // function operator
  double operator()(double, const vector<double>&) const;

  // definition of the class for the ROOT-dictionary
  ClassDef(SExpRlx,1)
};