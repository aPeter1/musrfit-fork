/***************************************************************************

  TUserLondon.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/25

***************************************************************************/

#ifndef _TUserLondon_H_
#define _TUserLondon_H_

#include "TPofTCalc.h"

class TUserLondon {

public:
  TUserLondon(const vector<unsigned int>& , const vector<double>&);
  ~TUserLondon();

  double Eval(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;
  TTrimSPData *fImpProfile;
  TPofTCalc *fPofT;
  mutable bool fCalcNeeded;
  mutable bool fLastTwoChanged;
  mutable bool fLastThreeChanged;

};

#endif //_TUserLondon_H_
