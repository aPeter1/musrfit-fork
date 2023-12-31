/***************************************************************************

  TPofBCalc.h

  Author: Bastian M. Wojek

  $Id$

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
#define PI 3.14159265358979323846

/**
 * <p>Class used to calculate static magnetic field distributions
 */
class TPofBCalc {

public:

  // standard constructor: allocates memory for B and PB, the arrays are filled later by one of the Calculate-methods
  TPofBCalc(const std::vector<double>&);
  // alternative constructor: PB is not actually calculated but copied from a vector
  TPofBCalc(const std::vector<double>&, const std::vector<double>& , double dt = 0.01);
  ~TPofBCalc() {
    delete[] fB; fB = nullptr;
    delete[] fPB; fPB = nullptr;
  }

  void Calculate(const std::string&, const std::vector<double>&);
  void Calculate(const TBofZCalc*, const TTrimSPData*, const std::vector<double>&, unsigned int);
  void Calculate(const TBofZCalcInverse*, const TTrimSPData*, const std::vector<double>&);
  void Calculate(const TBulkVortexFieldCalc*, const std::vector<double>&);

  const double* DataB() const {return fB;}
  double* DataPB() const {return fPB;}
  double GetBmin() const {return fBmin;}
  double GetBmax() const {return fBmax;}
  unsigned int GetPBSize() const {return fPBSize;}
  void SetPB(const std::vector<double>&) const;
  void ConvolveGss(double);
  void AddBackground(double, double, double);
  double GetFirstMoment() const;
  double GetCentralMoment(unsigned int) const;
  double GetSkewnessAlpha() const;
  void UnsetPBExists();
  void Normalize(unsigned int, unsigned int) const;

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
