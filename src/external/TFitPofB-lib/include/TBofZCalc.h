/***************************************************************************

  TBofZCalc.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/04/25

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

#ifndef _TBofZCalc_H_
#define _TBofZCalc_H_

#include <vector>
using namespace std;

//--------------------
// Base class for any kind of theory function B(z)
//--------------------

class TBofZCalc {

public:

  TBofZCalc() {}

  virtual ~TBofZCalc() {
    fZ.clear();
    fBZ.clear();
    fParam.clear();
  }

  virtual vector<double>* DataZ() const {return &fZ;}
  virtual vector<double>* DataBZ() const {return &fBZ;}
  virtual void Calculate();
  virtual double GetBofZ(double) const = 0;
  virtual double GetBmin() const = 0;
  virtual double GetBmax() const = 0;
  double GetDZ() const {return fDZ;}

protected:
  int fSteps;
  double fDZ;
  vector<double> fParam;
  mutable vector<double> fZ;
  mutable vector<double> fBZ;
};

//--------------------
// Base class for any kind of theory function B(z) where the inverse and its derivative are given analytically
//--------------------

class TBofZCalcInverse : public TBofZCalc {

public:

  TBofZCalcInverse() {}
  virtual ~TBofZCalcInverse() {}

  virtual vector< pair<double, double> > GetInverseAndDerivative(double) const = 0;
};

//--------------------
// Class "for Meissner screening" in a superconducting half-space
//--------------------

class TLondon1D_HS : public TBofZCalcInverse {

public:

  TLondon1D_HS(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

};

//--------------------
// Class "for Meissner screening" in a thin superconducting film
//--------------------

class TLondon1D_1L : public TBofZCalcInverse {

public:

  TLondon1D_1L(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  double fMinZ;
  double fMinB;
  double fCoeff[2];

};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - bilayer with two different lambdas
//--------------------

class TLondon1D_2L : public TBofZCalcInverse {

public:

  TLondon1D_2L(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  int fMinTag;
  double fMinZ;
  double fMinB;
  double fInterfaces[3];
  double fCoeff[4];
};


//--------------------
// Class "for proximity screening" in a thin superconducting film - one layer + superconducting half space
//--------------------

class TProximity1D_1LHS : public TBofZCalcInverse {

public:

  TProximity1D_1LHS(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  int fMinTag;
  double fMinZ;
  double fMinB;
  double fInterfaces[2];
};

//--------------------
// Class "for proximity screening" in a thin superconducting film - one layer + superconducting half space
//--------------------

class TProximity1D_1LHSGss : public TBofZCalcInverse {

public:

  TProximity1D_1LHSGss(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  int fMinTag;
  double fMinZ;
  double fMinB;
  double fInterfaces[2];
};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - tri-layer with three different lambdas
//--------------------

class TLondon1D_3L : public TBofZCalcInverse {

public:

  TLondon1D_3L(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  int fMinTag;
  double fMinZ;
  double fMinB;
  double fInterfaces[4];
  double fCoeff[6];
};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - tri-layer with two different lambdas
//--------------------

class TLondon1D_3LS : public TBofZCalcInverse {

public:

  TLondon1D_3LS(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  int fMinTag;
  double fMinZ;
  double fMinB;
  double fInterfaces[4];
  double fCoeff[6];
};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - tri-layer with insulating buffer layer, two lambda
//--------------------

class TLondon1D_3LwInsulator : public TBofZCalc {

public:

  TLondon1D_3LwInsulator(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;

private:
  void SetBmin();

  double fMinZ;
  double fMinB;
  double fCoeff[4];
};

#endif // _BofZCalc_H_
