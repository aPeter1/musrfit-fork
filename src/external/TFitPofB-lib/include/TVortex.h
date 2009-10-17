/***************************************************************************

  TVortex.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/10/17

***************************************************************************/

#ifndef _TVortex_H_
#define _TVortex_H_

#include "PUserFcnBase.h"
#include "TPofTCalc.h"

class TBulkTriVortexLondon : public PUserFcnBase {

public:
  TBulkTriVortexLondon();
  ~TBulkTriVortexLondon();

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;
  TPofTCalc *fPofT;
  mutable bool fCalcNeeded;
  mutable bool fFirstCall;
  mutable vector<double> fParForPofT;
  mutable vector<double> fParForVortex;
  mutable vector<double> fParForPofB;
  string fWisdom;
  unsigned int fGridSteps;
  unsigned int fVortexFourierComp;

  ClassDef(TBulkTriVortexLondon,1)
};

#endif //_TLondon1D_H_
