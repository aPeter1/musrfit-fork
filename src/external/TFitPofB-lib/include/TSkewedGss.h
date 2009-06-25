/***************************************************************************

  TSkewedGss.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/01/24

***************************************************************************/

#ifndef _TSkewedGss_H_
#define _TSkewedGss_H_

#include "PUserFcnBase.h"
#include "TPofTCalc.h"

class TSkewedGss : public PUserFcnBase {

public:
  // default constructor
  TSkewedGss();
  ~TSkewedGss();

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;
  TPofTCalc *fPofT;
  mutable bool fCalcNeeded;
  mutable bool fFirstCall;
  mutable vector<double> fParForPofT;
  mutable vector<double> fParForPofB;
  string fWisdom;

  ClassDef(TSkewedGss,1)
};

#endif //_TSkewedGss_H_
