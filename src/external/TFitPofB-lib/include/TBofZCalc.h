/***************************************************************************

  TBofZCalc.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/11/07

***************************************************************************/

#ifndef _TBofZCalc_H_
#define _TBofZCalc_H_

#include <vector>
#include <pair>
using namespace std;

//--------------------
// Base class for any kind of theory function B(z)
// In principle only constructors for the different models have to be implemented
//--------------------

class TBofZCalc {

public:

  TBofZCalc() {}

  virtual ~TBofZCalc() {
    fZ.clear();
    fBZ.clear();
  }

  virtual vector<double> DataZ() const;
  virtual vector<double> DataBZ() const;
  virtual void Calculate() = 0;
  virtual double GetBofZ(double) const = 0;
  virtual double GetBmin() const = 0;
  virtual double GetBmax() const = 0;
  double GetdZ() const {return fDZ;}

protected:
  vector<double> fZ;
  vector<double> fBZ;
  double fDZ;
  vector<double> fParam;
  unsigned int fSteps;
};

//--------------------
// Class "for Meissner screening" in a superconducting half-space
//--------------------

class TLondon1D_HS : public TBofZCalc {

public:

  TLondon1D_HS(unsigned int, const vector<double>& );

};

//--------------------
// Class "for Meissner screening" in a thin superconducting film
//--------------------

class TLondon1D_1L : public TBofZCalc {

public:

  TLondon1D_1L(unsigned int, const vector<double>& );

};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - bilayer with two different lambdas
//--------------------

class TLondon1D_2L : public TBofZCalc {

public:

  TLondon1D_2L(unsigned int, const vector<double>& );

};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - tri-layer with three different lambdas
//--------------------

class TLondon1D_3L : public TBofZCalc {

public:

  TLondon1D_3L(unsigned int, const vector<double>& );
  void Calculate();
  double GetBofZ(double) const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;
  double GetBmin() const;
  double GetBmax() const;

private:
  void SetBmin();

  int fMinTag;
  double fMinZ;
  double fMinB;
  double fCoeff[6];
  double fInterfaces[4];

};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - tri-layer with two different lambdas
//--------------------

class TLondon1D_3LS : public TBofZCalc {

public:

  TLondon1D_3LS(unsigned int, const vector<double>& );

};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - four layers with four different lambdas
//--------------------

class TLondon1D_4L : public TBofZCalc {

public:

  TLondon1D_4L(unsigned int, const vector<double>& );

};

#endif // _BofZCalc_H_
