/***************************************************************************

  TTrimSPDataHandler.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/05/15

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Bastian M. Wojek                                *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _TTrimSPDataHandler_H_
#define _TTrimSPDataHandler_H_

#include <vector>
#include <string>
#include <map>
using namespace std;

class TTrimSPData {

public:

  TTrimSPData(const string&, map<double, string>&);

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
