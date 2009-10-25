/***************************************************************************

  TBulkVortexFieldCalc.h

  Author: Bastian M. Wojek, Alexander Maisuradze
  e-mail: bastian.wojek@psi.ch

  2009/10/17

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

#ifndef _TBulkVortexFieldCalc_H_
#define _TBulkVortexFieldCalc_H_

#include <vector>
#include <string>
using namespace std;

#include <fftw3.h>

//--------------------
// Base class for any kind of vortex symmetry
//--------------------

class TBulkVortexFieldCalc {

public:

  TBulkVortexFieldCalc() {}

  virtual ~TBulkVortexFieldCalc();

  virtual double* DataB() const {return fFFTout;}
  virtual void SetParameters(const vector<double>& par) {fParam = par; fGridExists = false;}
  virtual void CalculateGrid() const = 0;
  virtual double GetBmin() const = 0;
  virtual double GetBmax() const = 0;
  virtual bool GridExists() const {return fGridExists;}
  virtual void UnsetGridExists() const {fGridExists = false;}
  virtual unsigned int GetNumberOfSteps() const {return fSteps;}

protected:
  vector<double> fParam;
  unsigned int fSteps;  // number of steps, the "unit cell" of the vortex lattice is devided in (in x- and y- direction)
  fftw_complex *fFFTin; // Fourier components of the field
  double *fFFTout; // fields in a "unit cell" of the vortex lattice
  fftw_plan fFFTplan;
  bool fUseWisdom;
  string fWisdom;
  mutable bool fGridExists;
};

//--------------------
// Class for triangular vortex lattice, London model with Gaussian Cutoff
//--------------------

class TBulkTriVortexLondonFieldCalc : public TBulkVortexFieldCalc {

public:

  TBulkTriVortexLondonFieldCalc(const string&, const unsigned int steps = 256);
  ~TBulkTriVortexLondonFieldCalc() {}

  void CalculateGrid() const;
  double GetBmin() const;
  double GetBmax() const;

};

#endif // _TBulkVortexFieldCalc_H_
