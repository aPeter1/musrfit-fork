/***************************************************************************

  TPofBCalc.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/09/04

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

#ifndef _TPofBCalc_H_
#define _TPofBCalc_H_

#include "TBofZCalc.h"
#include "TTrimSPDataHandler.h"
#include "TBulkTriVortexFieldCalc.h"

#define gBar 0.0135538817
#define pi 3.14159265358979323846

/**
 * <p>Class used to calculate static magnetic field distributions
 */
class TPofBCalc {

public:

  // standard constructor: allocates memory for B and PB, the arrays are filled later by one of the Calculate-methods
  TPofBCalc(const vector<double>&);
  // alternative constructor: PB is not actually calculated but copied from a vector
  TPofBCalc(const vector<double>&, const vector<double>& , double dt = 0.01);
  ~TPofBCalc() {
    delete[] fB; fB = 0;
    delete[] fPB; fPB = 0;
  }

  void Calculate(const string&, const vector<double>&);
  void Calculate(const TBofZCalc*, const TTrimSPData*, const vector<double>&, unsigned int);
  void Calculate(const TBofZCalcInverse*, const TTrimSPData*, const vector<double>&);
  void Calculate(const TBulkVortexFieldCalc*, const vector<double>&);

  const double* DataB() const {return fB;}
  double* DataPB() const {return fPB;}
  double GetBmin() const {return fBmin;}
  double GetBmax() const {return fBmax;}
  unsigned int GetPBSize() const {return fPBSize;}
  void ConvolveGss(double);
  void AddBackground(double, double, double);
  double GetFirstMoment() const;
  void UnsetPBExists();

private:
  double *fB; ///< array of discrete points in the field domain
  mutable double *fPB; ///< array of discrete values of the field distribution P(B)
  double fBmin; ///< minimum field contributing to the distribution
  double fBmax; ///< maximum field contributing to the distribution
  double fDT; ///< time resolution (needed for convolutions)
  double fDB; ///< field resolution (spacing of neighboring fields for which P(B) is calculated)
  mutable bool fPBExists; ///< tag indicating if P(B) has been calculated for a set of given parameters
  unsigned int fPBSize; ///< length of the P(B) array
};

#endif // _TPofBCalc_H_
