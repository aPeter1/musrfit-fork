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

/**
 * <p>Base class for any kind of theory function B(z)
 */
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
  int fSteps; ///< number of discrete points where B(z) is calculated
  double fDZ; ///< resolution in z (spacing between two neighboring discrete B(z) points)
  vector<double> fParam; ///< parameters of the B(z) function
  mutable vector<double> fZ; ///< vector holding all z-values
  mutable vector<double> fBZ; ///< vector holding all B(z)-values
};

/**
 * <p>Base class for any kind of theory function B(z) where the inverse and its derivative are given analytically
 */
class TBofZCalcInverse : public TBofZCalc {

public:

  TBofZCalcInverse() {}
  virtual ~TBofZCalcInverse() {}

  virtual vector< pair<double, double> > GetInverseAndDerivative(double) const = 0;
};

/**
 * <p>Class using the 1D London model to calculate Meissner screening in a superconducting half-space
 */
class TLondon1D_HS : public TBofZCalcInverse {

public:

  TLondon1D_HS(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

};

/**
 * <p>Class using the 1D London model to calculate Meissner screening in a thin superconducting film
 */
class TLondon1D_1L : public TBofZCalcInverse {

public:

  TLondon1D_1L(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  double fMinZ; ///< position of the minimum of B(z)
  double fMinB; ///< miniumum value of B(z)
  double fCoeff[2]; ///< array holding the results of two intermediate steps of the involved calculations

};

/**
 * <p>Class using the 1D London model to calculate Meissner screening in a thin superconducting film
 * consisting of two layers with different magnetic penetration depths
 */
class TLondon1D_2L : public TBofZCalcInverse {

public:

  TLondon1D_2L(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  int fMinTag; ///< tag specifying which layer contains the minimum value of B(z)
  double fMinZ; ///< position of the minimum of B(z)
  double fMinB; ///< miniumum value of B(z)
  double fInterfaces[3]; ///< positions of the interfaces between monotonous parts of B(z) or where the function changes
  double fCoeff[4]; ///< array holding the results of four intermediate steps of the involved calculations
};

/**
 * <p>Class calculating the Meissner screening in a conventionally proximated system
 * consisting of one metal layer and an underlying (London) superconducting half-space
 */
class TProximity1D_1LHS : public TBofZCalcInverse {

public:

  TProximity1D_1LHS(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  int fMinTag; ///< tag specifying which layer contains the minimum value of B(z)
  double fMinZ; ///< position of the minimum of B(z)
  double fMinB; ///< miniumum value of B(z)
  double fInterfaces[2]; ///< positions of the interfaces between a dead layer, the metallic region and the superconducting region
};

/**
 * <p>Class using the 1D London model to calculate Meissner screening in a thin superconducting film
 * consisting of three layers with different magnetic penetration depths
 */
class TLondon1D_3L : public TBofZCalcInverse {

public:

  TLondon1D_3L(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  int fMinTag; ///< tag specifying which layer contains the minimum value of B(z)
  double fMinZ; ///< position of the minimum of B(z)
  double fMinB; ///< miniumum value of B(z)
  double fInterfaces[4]; ///< positions of the interfaces between monotonous parts of B(z) or where the function changes
  double fCoeff[6]; ///< array holding the results of six intermediate steps of the involved calculations
};

/**
 * <p>Class using the 1D London model to calculate Meissner screening in a thin superconducting film
 * consisting of three layers with different magnetic penetration depths (where lambda is the same for the two outer layers)
 */
class TLondon1D_3LS : public TBofZCalcInverse {

public:

  TLondon1D_3LS(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  int fMinTag; ///< tag specifying which layer contains the minimum value of B(z)
  double fMinZ; ///< position of the minimum of B(z)
  double fMinB; ///< miniumum value of B(z)
  double fInterfaces[4]; ///< positions of the interfaces between monotonous parts of B(z) or where the function changes
  double fCoeff[6]; ///< array holding the results of six intermediate steps of the involved calculations
};

/**
 * <p>Class using the 1D London model to calculate Meissner screening in a thin superconducting film
 * consisting of three layers with two different magnetic penetration depths and the central layer being insulating
 */
class TLondon1D_3LwInsulator : public TBofZCalc {

public:

  TLondon1D_3LwInsulator(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;

private:
  void SetBmin();

  double fMinZ; ///< position of the minimum of B(z)
  double fMinB; ///< miniumum value of B(z)
  double fCoeff[4]; ///< array holding the results of four intermediate steps of the involved calculations
};

#endif // _BofZCalc_H_
