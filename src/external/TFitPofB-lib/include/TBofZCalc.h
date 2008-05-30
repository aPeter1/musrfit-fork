/***************************************************************************

  TBofZCalc.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/30

***************************************************************************/

#ifndef _TBofZCalc_H_
#define _TBofZCalc_H_

#include <vector>
using namespace std;

//--------------------
// Base class for any kind of theory function B(z)
// In principle only constructors for the different models have to be implemented
//--------------------

class TBofZCalc {

public:

  TBofZCalc() {}

  ~TBofZCalc() {
    fZ.clear();
    fBZ.clear();
  }

  vector<double> DataZ() const {return fZ;}
  vector<double> DataBZ() const {return fBZ;}
  double GetBmin() const;
  double GetBmax() const;
  double GetBofZ(double) const;
  double GetdZ() const {return fDZ;}

protected:
  vector<double> fZ;
  vector<double> fBZ;
  double fDZ;
};

//--------------------
// Class "for Meissner screening" in a thin superconducting film
//--------------------

class TLondon1D_1L : public TBofZCalc {

public:

  TLondon1D_1L( const vector<double>& );

};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - bilayer with two different lambdas
//--------------------

class TLondon1D_2L : public TBofZCalc {

public:

  TLondon1D_2L( const vector<double>& );

};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - tri-layer with three different lambdas
//--------------------

class TLondon1D_3L : public TBofZCalc {

public:

  TLondon1D_3L( const vector<double>& );

};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - tri-layer with two different lambdas
//--------------------

class TLondon1D_3LS : public TBofZCalc {

public:

  TLondon1D_3LS( const vector<double>& );

};

#endif // _BofZCalc_H_
