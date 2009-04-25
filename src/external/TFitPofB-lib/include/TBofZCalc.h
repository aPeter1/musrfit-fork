/***************************************************************************

  TBofZCalc.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/04/25

***************************************************************************/

#ifndef _TBofZCalc_H_
#define _TBofZCalc_H_

#include <vector>
using namespace std;

//--------------------
// Base class for any kind of theory function B(z)
//--------------------

class TBofZCalc {

public:

  TBofZCalc() {}

  virtual ~TBofZCalc() {
    fZ.clear();
    fBZ.clear();
    fParam.clear();
  }

  virtual vector<double> DataZ() const {return fZ;}
  virtual vector<double> DataBZ() const {return fBZ;}
  virtual void Calculate();
  virtual double GetBofZ(double) const = 0;
  virtual double GetBmin() const = 0;
  virtual double GetBmax() const = 0;
  double GetDZ() const {return fDZ;}

protected:
  int fSteps;
  double fDZ;
  vector<double> fParam;
  vector<double> fZ;
  vector<double> fBZ;
};

//--------------------
// Base class for any kind of theory function B(z) where the inverse and its derivative are given analytically
//--------------------

class TBofZCalcInverse : public TBofZCalc {

public:

  TBofZCalcInverse() {}
  virtual ~TBofZCalcInverse() {}

  virtual vector< pair<double, double> > GetInverseAndDerivative(double) const = 0;
};

//--------------------
// Class "for Meissner screening" in a superconducting half-space
//--------------------

class TLondon1D_HS : public TBofZCalcInverse {

public:

  TLondon1D_HS(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

};

//--------------------
// Class "for Meissner screening" in a thin superconducting film
//--------------------

class TLondon1D_1L : public TBofZCalcInverse {

public:

  TLondon1D_1L(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  double fMinZ;
  double fMinB;
  double fCoeff[2];

};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - bilayer with two different lambdas
//--------------------

class TLondon1D_2L : public TBofZCalcInverse {

public:

  TLondon1D_2L(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  int fMinTag;
  double fMinZ;
  double fMinB;
  double fInterfaces[3];
  double fCoeff[4];
};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - tri-layer with three different lambdas
//--------------------

class TLondon1D_3L : public TBofZCalcInverse {

public:

  TLondon1D_3L(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  int fMinTag;
  double fMinZ;
  double fMinB;
  double fInterfaces[4];
  double fCoeff[6];
};

//--------------------
// Class "for Meissner screening" in a thin superconducting film - tri-layer with two different lambdas
//--------------------

class TLondon1D_3LS : public TBofZCalcInverse {

public:

  TLondon1D_3LS(const vector<double>&, unsigned int steps = 3000);
  double GetBofZ(double) const;
  double GetBmin() const;
  double GetBmax() const;
  vector< pair<double, double> > GetInverseAndDerivative(double) const;

private:
  void SetBmin();

  int fMinTag;
  double fMinZ;
  double fMinB;
  double fInterfaces[4];
  double fCoeff[6];
};

// //--------------------
// // Class "for Meissner screening" in a thin superconducting film - four layers with four different lambdas
// //--------------------
// 
// class TLondon1D_4L : public TBofZCalc {
// 
// public:
// 
//   TLondon1D_4L(unsigned int, const vector<double>& );
// 
// };

#endif // _BofZCalc_H_
