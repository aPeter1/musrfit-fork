/***************************************************************************

  TBulkVortexFieldCalc.h

  Author: Bastian M. Wojek, Alexander Maisuradze
  e-mail: bastian.wojek@psi.ch

  2009/10/17

***************************************************************************/

#ifndef _TBulkVortexFieldCalc_H_
#define _TBulkVortexFieldCalc_H_

#include <vector>
using namespace std;

//--------------------
// Base class for any kind of vortex symmetry
//--------------------

class TBulkVortexFieldCalc {

public:

  TBulkVortexFieldCalc() {}

  virtual ~TBulkVortexFieldCalc() {
    for(unsigned int i(0); i < fB.size(); i++)
      fB[i].clear();
    fB.clear();
    fParam.clear();
  }

  virtual vector< vector<double> > DataB() const {return fB;}
  virtual void CalculateGrid() const = 0;
  virtual double GetBatPoint(double, double) const = 0;
  virtual double GetBmin() const = 0;
  virtual double GetBmax() const = 0;

protected:
  unsigned int fSteps;  // number of steps, the "unit cell" of the vortex lattice is devided in (in x- and y- direction)
  vector<double> fParam; // lambda, xi, appl.field
  mutable vector< vector<double> > fB; // fields in a "unit cell" of the vortex lattice
};

//--------------------
// Class for triangular vortex lattice, London model
//--------------------

class TBulkTriVortexLondonFieldCalc : public TBulkVortexFieldCalc {

public:

  TBulkTriVortexLondonFieldCalc(const vector<double>&, const unsigned int steps = 200, const unsigned int Ncomp = 17);
  ~TBulkTriVortexLondonFieldCalc() {}

  void CalculateGrid() const;
  double GetBatPoint(double, double) const;
  double GetBmin() const;
  double GetBmax() const;

private:
  unsigned int fNFourierComp;
};

#endif // _TBulkVortexFieldCalc_H_
