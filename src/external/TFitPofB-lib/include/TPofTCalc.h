/***************************************************************************

  TPofTCalc.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/25

***************************************************************************/

#ifndef _TPofTCalc_H_
#define _TPofTCalc_H_

#include "TPofBCalc.h"
#include "fftw3.h"

#define PI 3.14159265358979323846

class TPofTCalc {

public:
  TPofTCalc(const vector<double>&);
  ~TPofTCalc();

  vector<double> DataT() const {return fT;}
  vector<double> DataPT() const {return fPT;}
  void DoFFT(const TPofBCalc&);
  void CalcPol(const vector<double>&);
  double Eval(double) const;

private:
  fftw_plan fFFTplan;
  fftw_complex *fFFTout;
  double *fFFTin;
  vector<double> fT;
  vector<double> fPT;
  double fTBin;
  unsigned int fNFFT;

};

#endif // _TPofTCalc_H_
