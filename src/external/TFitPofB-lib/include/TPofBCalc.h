/***************************************************************************

  TPofBCalc.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/09/04

***************************************************************************/

#ifndef _TPofBCalc_H_
#define _TPofBCalc_H_

#include "TBofZCalc.h"
#include "TTrimSPDataHandler.h"

#define gBar 0.0135538817
#define pi 3.14159265358979323846

class TPofBCalc {

public:

  TPofBCalc( const string&, const vector<double>& );
  TPofBCalc( const TBofZCalc&, const TTrimSPData&, const vector<double>&, unsigned int );
  TPofBCalc( const TBofZCalcInverse&, const TTrimSPData&, const vector<double>& );
  TPofBCalc( const vector<double>&, const vector<double>& , double dt = 0.01 );
  ~TPofBCalc() {
    fB.clear();
    fPB.clear();
  }

  vector<double> DataB() const {return fB;}
  vector<double> DataPB() const {return fPB;}
  double GetBmin() const {return fBmin;}
  double GetBmax() const {return fBmax;}
  void ConvolveGss(double);
  void AddBackground(double, double, double);

private:
  vector<double> fB;
  vector<double> fPB;
  double fBmin;
  double fBmax;
  double fDT;
  double fDB;
};

#endif // _TPofBCalc_H_
