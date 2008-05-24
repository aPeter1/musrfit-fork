/***************************************************************************

  TFitPofB.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/24

***************************************************************************/

#ifndef _TFitPofB_H_
#define _TFitPofB_H_

#include "TPofTCalc.h"

class TFitPofB {

public:
  TFitPofB(const vector<unsigned int>& , const vector<double>&);
  ~TFitPofB();

  double Eval(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;
  TTrimSPData *fImpProfile;
  TPofTCalc *fPofT;
  mutable bool fCalcNeeded;
  
};

#endif //_TFitPofB_H_
