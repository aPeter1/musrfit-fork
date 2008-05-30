/***************************************************************************

  TLondon1D.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/30

***************************************************************************/

#ifndef _TLondon1D_H_
#define _TLondon1D_H_

#include "TObject.h"
#include "TPofTCalc.h"

class TLondon1D : public TObject {

public:
  // default conctructor only for the ROOT dictionary - DO NOT USE IT OTHERWISE!
  TLondon1D() : fCalcNeeded(true) {}
  virtual ~TLondon1D();

  virtual double Eval(double, const vector<double>&) const = 0;

protected:
  mutable vector<double> fPar;
  TTrimSPData *fImpProfile;
  TPofTCalc *fPofT;
  mutable bool fCalcNeeded;

  ClassDef(TLondon1D,1)
};

class TLondon1D1L : public TLondon1D {

public:
  // default conctructor only for the ROOT dictionary - DO NOT USE IT OTHERWISE!
  TLondon1D1L() {}
  TLondon1D1L(const vector<unsigned int>& , const vector<double>&);
  ~TLondon1D1L() {}

  double Eval(double, const vector<double>&) const;

  ClassDef(TLondon1D1L,1)
};

class TLondon1D2L : public TLondon1D {

public:
  // default conctructor only for the ROOT dictionary - DO NOT USE IT OTHERWISE!
  TLondon1D2L() : fLastTwoChanged(true) {}
  TLondon1D2L(const vector<unsigned int>& , const vector<double>&);
  ~TLondon1D2L() {}

  double Eval(double, const vector<double>&) const;

private:
  mutable bool fLastTwoChanged;

  ClassDef(TLondon1D2L,1)
};

class TLondon1D3L : public TLondon1D {

public:
  // default conctructor only for the ROOT dictionary - DO NOT USE IT OTHERWISE!
  TLondon1D3L() : fLastThreeChanged(true) {}
  TLondon1D3L(const vector<unsigned int>& , const vector<double>&);
  ~TLondon1D3L() {}

  double Eval(double, const vector<double>&) const;

private:
  mutable bool fLastThreeChanged;

  ClassDef(TLondon1D3L,1)
};


class TLondon1D3LS : public TLondon1D {

public:
  // default conctructor only for the ROOT dictionary - DO NOT USE IT OTHERWISE!
  TLondon1D3LS() : fLastThreeChanged(true) {}
  TLondon1D3LS(const vector<unsigned int>& , const vector<double>&);
  ~TLondon1D3LS() {}

  double Eval(double, const vector<double>&) const;

private:
  mutable bool fLastThreeChanged;

  ClassDef(TLondon1D3LS,1)
};

#endif //_TLondon1D_H_
