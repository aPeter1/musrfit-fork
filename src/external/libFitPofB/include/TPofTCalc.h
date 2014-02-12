/***************************************************************************

  TPofTCalc.h

  Author: Bastian M. Wojek

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

// the following ifdef is needed for GCC 4.6 or higher, fftw 3.3 or higher and root 5.30.03 or lower
#ifdef __CINT__
typedef struct { char a[7]; } __float128; // needed since cint doesn't know it
#endif
#include "fftw3.h"
#include <string>

#define PI 3.14159265358979323846
#define tauMu 2.197147

/**
 * <p>Class used to calculate a muon spin depolarization function from a static magnetic field distribution
 */
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
  fftw_plan fFFTplan; ///< FFTW plan for the 1D Fourier transform from field domain to time domain 
  double *fFFTin; ///< input array of the Fourier transform in the field domain 
  fftw_complex *fFFTout; ///< output array of the Fourier transform in the time domain
  double *fT; ///< array containing the discrete times for which the polarization p(t) is calculated
  double *fPT; ///< array containing the discrete values of the polarization p(t)
  double fTBin; ///< time resolution
  int fNFFT; ///< length of the discrete 1D Fourier transform
  const string fWisdom; ///< file name of the FFTW wisdom file
  bool fUseWisdom; ///< tag determining if a FFTW wisdom file is used

};

#endif // _TPofTCalc_H_
