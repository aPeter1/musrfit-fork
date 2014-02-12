/***************************************************************************

  TSkewedGss.h

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

#ifndef _TSkewedGss_H_
#define _TSkewedGss_H_

#include "PUserFcnBase.h"
#include "TPofTCalc.h"

/**
 * <p>Class implementing the musrfit user function interface for calculating muon spin depolarization functions
 * from a skewed Gaussian static field distribution
 */
class TSkewedGss : public PUserFcnBase {

public:
  // default constructor
  TSkewedGss();
  ~TSkewedGss();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  mutable vector<double> fPar; ///< parameters of the model
  TPofBCalc *fPofB; ///< static field distribution P(B)
  TPofTCalc *fPofT; ///< muon spin polarization p(t)
  mutable bool fCalcNeeded; ///< tag needed to avoid unnecessary calculations if the core parameters were unchanged
  mutable bool fFirstCall; ///< tag for checking if the function operator is called the first time
  mutable vector<double> fParForPofT; ///< parameters for the calculation of p(t)
  mutable vector<double> fParForPofB; ///< parameters for the calculation of P(B)
  string fWisdom; ///< file name of the FFTW wisdom file

  ClassDef(TSkewedGss,1)
};

#endif //_TSkewedGss_H_
