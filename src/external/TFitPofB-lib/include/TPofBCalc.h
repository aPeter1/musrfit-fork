/***************************************************************************

  TPofBCalc.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/24

***************************************************************************/

#ifndef _TPofBCalc_H_
#define _TPofBCalc_H_

#include "TBofZCalc.h"
#include "TTrimSPDataHandler.h"

class TPofBCalc {

public:

  TPofBCalc( const TBofZCalc&, const TTrimSPData&, const vector<double>& );
  ~TPofBCalc() {
    fB.clear();
    fPB.clear();
  }

  vector<double> DataB() const {return fB;}
  vector<double> DataPB() const {return fPB;}
  double GetBmin() const {return fBmin;}
  double GetBmax() const {return fBmax;}

private:
  vector<double> fB;
  vector<double> fPB;
  double fBmin;
  double fBmax;
  static const double gBar = 0.0135538817;

};

#endif // _TPofBCalc_H_
