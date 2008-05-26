/***************************************************************************

  TTrimSPDataHandler.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/26

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
  }

  vector<double> Energy() const {return fEnergy;}
  vector<double> DataZ(double) const;
  vector<double> DataNZ(double) const;
  vector<double> OrigDataNZ(double) const;
  void WeightLayers(double, const vector<double>&, const vector<double>&) const;
  double GetNofZ(double, double) const;
  void Normalize(double);
  bool IsNormalized(double) const;

private:
  vector<double> fEnergy;
  vector< vector<double> > fDataZ;
  mutable vector< vector<double> > fDataNZ;
  vector< vector<double> > fOrigDataNZ;
  mutable vector<bool> fIsNormalized;
};

#endif // _TTrimSPDataHandler_H_
