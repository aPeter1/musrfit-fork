/***************************************************************************

  TTrimSPDataHandler.h

  Author: Bastian M. Wojek

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

/**
 * <p>Class used to handle a set of low energy muon implantation profiles
 */
class TTrimSPData {

public:

  TTrimSPData(const string&, map<double, string>&, bool debug = false, unsigned int highRes = 0);

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
  double DataDZ(double) const;
  void UseHighResolution(double);
  void SetOriginal() {fOrigDataNZ = fDataNZ;}
  void WeightLayers(double, const vector<double>&, const vector<double>&) const;
  double LayerFraction(double, unsigned int, const vector<double>&) const;
  double GetNofZ(double, double) const;
  void Normalize(double) const;
  bool IsNormalized(double) const;
  void ConvolveGss(double, double) const;
  double MeanRange(double) const;
  double PeakRange(double) const;

private:
  void FindEnergy(double) const;

  vector<double> fEnergy; ///< vector holding all available muon energies
  vector<double> fDZ; ///< vector holding the spatial resolution of the TRIM.SP output for all energies
  vector< vector<double> > fDataZ; ///< discrete points in real space for which n(z) has been calculated for all energies
  mutable vector< vector<double> > fDataNZ; ///< n(z) for all energies
  vector< vector<double> > fOrigDataNZ; ///< original (unmodified) implantation profiles for all energies as read in from rge-files
  mutable vector<bool> fIsNormalized; ///< tag indicating if the implantation profiles are normalized (for each energy separately)
  mutable vector<double>::const_iterator fEnergyIter; ///< iterator traversing the vector of available energies
};

#endif // _TTrimSPDataHandler_H_
