/***************************************************************************

  TLondon1D.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/27

***************************************************************************/

#ifndef _TLondon1D_H_
#define _TLondon1D_H_

#include "TPofTCalc.h"

class TLondon1D {

public:
  TLondon1D() : fCalcNeeded(true) {}
  virtual ~TLondon1D();

  virtual double Eval(double, const vector<double>&) const = 0;

protected:
  mutable vector<double> fPar;
  TTrimSPData *fImpProfile;
  TPofTCalc *fPofT;
  mutable bool fCalcNeeded;
};

class TLondon1D1L : public TLondon1D {

public:
  TLondon1D1L(const vector<unsigned int>& , const vector<double>&);
  ~TLondon1D1L() {}

  double Eval(double, const vector<double>&) const;

};

class TLondon1D2L : public TLondon1D {

public:
  TLondon1D2L(const vector<unsigned int>& , const vector<double>&);
  ~TLondon1D2L() {}

  double Eval(double, const vector<double>&) const;

private:
  mutable bool fLastTwoChanged;

};

class TLondon1D3LS : public TLondon1D {

public:
  TLondon1D3LS(const vector<unsigned int>& , const vector<double>&);
  ~TLondon1D3LS() {}

  double Eval(double, const vector<double>&) const;

private:
  mutable bool fLastThreeChanged;

};

#endif //_TUserLondon1D_H_
