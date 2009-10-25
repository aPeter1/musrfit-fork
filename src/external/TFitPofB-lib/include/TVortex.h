/***************************************************************************

  TVortex.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/10/17

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

#ifndef _TVortex_H_
#define _TVortex_H_

#include "PUserFcnBase.h"
#include "TPofTCalc.h"

class TBulkTriVortexLondon : public PUserFcnBase {

public:
  TBulkTriVortexLondon();
  ~TBulkTriVortexLondon();

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar;
  TBulkTriVortexLondonFieldCalc *fVortex;
  TPofBCalc *fPofB;
  TPofTCalc *fPofT;
  mutable bool fCalcNeeded;
  mutable bool fFirstCall;
  mutable vector<double> fParForVortex;
  mutable vector<double> fParForPofB;
  mutable vector<double> fParForPofT;
  string fWisdom;
  unsigned int fGridSteps;

  ClassDef(TBulkTriVortexLondon,1)
};

#endif //_TLondon1D_H_
