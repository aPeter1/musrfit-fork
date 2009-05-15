/***************************************************************************

  TTrimSPDataHandler.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/05/15

***************************************************************************/

#ifndef _TTrimSPDataHandler_H_
#define _TTrimSPDataHandler_H_

#include <vector>
#include <string>
using namespace std;

class TTrimSPData {

public:

  TTrimSPData(const string&, vector<string>&);

  ~TTrimSPData() {
    fDataZ.clear();
    fDataNZ.clear();
    fOrigDataNZ.clear();
    fEnergy.clear();
    fDZ.clear();
    fIsNormalized.clear();
  }

  vector<double> Energy() const {return fEnergy;}
  vector<double> DataZ(double) const;
  vector<double> DataNZ(double) const;
  vector<double> OrigDataNZ(double) const;
  void UseHighResolution(double);
  void WeightLayers(double, const vector<double>&, const vector<double>&) const;
  double LayerFraction(double, unsigned int, const vector<double>&) const;
  double GetNofZ(double, double) const;
  void Normalize(double) const;
  bool IsNormalized(double) const;
  void ConvolveGss(double, double) const;
  double MeanRange(double) const;
  double PeakRange(double) const;

private:
  vector<double> fEnergy;
  vector<double> fDZ;
  vector< vector<double> > fDataZ;
  mutable vector< vector<double> > fDataNZ;
  vector< vector<double> > fOrigDataNZ;
  mutable vector<bool> fIsNormalized;
  mutable vector<double>::const_iterator fEnergyIter;
};

#endif // _TTrimSPDataHandler_H_
