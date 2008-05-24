/***************************************************************************

  TTrimSPDataHandler.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/24

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
  double GetNofZ(double, double) const;

private:
  vector<double> fEnergy;
  vector< vector<double> > fDataZ;
  vector< vector<double> > fDataNZ;
};

#endif // _TTrimSPDataHandler_H_
