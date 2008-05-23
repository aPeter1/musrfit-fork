/***************************************************************************

  TPofTCalc.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/23

***************************************************************************/

#ifndef _TPofTCalc_H_
#define _TPofTCalc_H_

#include "TPofBCalc.h"
#include "fftw3.h"

class TPofTCalc {

public:
  TPofTCalc(const vector<double>&);
  ~TPofTCalc();

  vector<double> DataT() const {return fT;}
  vector<double> DataPT() const {return fPT;}
  void DoFFT(const TPofBCalc&, const vector<double>&);
  double Eval(double) const;

private:
  fftw_plan fFFTplan;
  fftw_complex *fFFTout;
  double *fFFTin;
  vector<double> fT;
  vector<double> fPT;
  double fTBin;
  unsigned int fNFFT;
  static const double PI = 3.14159265358979323846;
  static const double gBar = 0.0135538817;

};

#endif // _TPofTCalc_H_
