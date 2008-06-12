/***************************************************************************

  TLondon1D.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/06/06

***************************************************************************/

#ifndef _TLondon1D_H_
#define _TLondon1D_H_

#include "PUserFcnBase.h"
#include "TPofTCalc.h"

class TLondon1D1L : public PUserFcnBase {

public:
  // default conctructor
  TLondon1D1L();
  ~TLondon1D1L();

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;
  TTrimSPData *fImpProfile;
  TPofTCalc *fPofT;
  mutable bool fCalcNeeded;
  mutable bool fFirstCall;
  mutable vector<double> fParForPofT;
  mutable vector<double> fParForBofZ;
  mutable vector<double> fParForPofB;
  string fWisdom;
  unsigned int fNSteps;

  ClassDef(TLondon1D1L,1)
};

class TLondon1D2L : public PUserFcnBase {

public:
  // default conctructor
  TLondon1D2L();
  ~TLondon1D2L();

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;
  TTrimSPData *fImpProfile;
  TPofTCalc *fPofT;
  mutable bool fCalcNeeded;
  mutable bool fFirstCall;
  mutable vector<double> fParForPofT;
  mutable vector<double> fParForBofZ;
  mutable vector<double> fParForPofB;
  string fWisdom;
  unsigned int fNSteps;
  mutable bool fLastTwoChanged;

  ClassDef(TLondon1D2L,1)
};

class TLondon1D3L : public PUserFcnBase {

public:
  // default conctructor
  TLondon1D3L();
  ~TLondon1D3L();

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;
  TTrimSPData *fImpProfile;
  TPofTCalc *fPofT;
  mutable bool fCalcNeeded;
  mutable bool fFirstCall;
  mutable vector<double> fParForPofT;
  mutable vector<double> fParForBofZ;
  mutable vector<double> fParForPofB;
  string fWisdom;
  unsigned int fNSteps;
  mutable bool fLastThreeChanged;

  ClassDef(TLondon1D3L,1)
};


class TLondon1D3LS : public PUserFcnBase {

public:
  // default conctructor
  TLondon1D3LS();
  ~TLondon1D3LS();

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;
  TTrimSPData *fImpProfile;
  TPofTCalc *fPofT;
  mutable bool fCalcNeeded;
  mutable bool fFirstCall;
  mutable vector<double> fParForPofT;
  mutable vector<double> fParForBofZ;
  mutable vector<double> fParForPofB;
  string fWisdom;
  unsigned int fNSteps;
  mutable bool fLastThreeChanged;

  ClassDef(TLondon1D3LS,1)
};

#endif //_TLondon1D_H_
