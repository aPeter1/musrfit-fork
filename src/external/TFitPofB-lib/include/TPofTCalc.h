/***************************************************************************

  TPofTCalc.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/11/16

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

#ifndef _TPofTCalc_H_
#define _TPofTCalc_H_

#include "TPofBCalc.h"
#include "fftw3.h"
#include <string>

#define PI 3.14159265358979323846
#define tauMu 2.197147

class TPofTCalc {

public:

  TPofTCalc(const TPofBCalc*, const string&, const vector<double>&);
  ~TPofTCalc();

  const double* DataT() const {return fT;}
  const double* DataPT() const {return fPT;}
  void DoFFT();
  void CalcPol(const vector<double>&);
  void FakeData(const string&, const vector<double>&, const vector<double>*); 
  double Eval(double) const;

private:
  fftw_plan fFFTplan;
  double *fFFTin;
  fftw_complex *fFFTout;
  double *fT;
  double *fPT;
  double fTBin;
  int fNFFT;
  const string fWisdom;
  bool fUseWisdom;

};

#endif // _TPofTCalc_H_
