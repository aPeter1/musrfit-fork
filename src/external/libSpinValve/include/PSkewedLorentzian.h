/***************************************************************************

  PSkewedLorentzian.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013 by Andreas Suter                                   *
 *   andreas.suter@psi.ch                                                  *
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

#ifndef _PSKEWEDLORENTZIAN_H_
#define _PSKEWEDLORENTZIAN_H_

#include <vector>
using namespace std;

#include "PUserFcnBase.h"
#include "PStartupHandler_SV.h"

class PSkewedLorentzian : public PUserFcnBase
{
  public:
    PSkewedLorentzian();
    virtual ~PSkewedLorentzian();

    virtual Bool_t IsValid() { return fValid; }

    // global user-function-access functions, here without any functionality
    virtual Bool_t NeedGlobalPart() const { return false; }
    void SetGlobalPart(vector<void*> &globalPart, UInt_t idx) { }
    Bool_t GlobalPartIsValid() const { return true; }

    // function operator
    Double_t operator()(Double_t, const vector<Double_t>&) const;

  private:
    PStartupHandler_SV *fStartupHandler;

    Bool_t   fValid;      ///< flag indicating if initialization went through smoothly
    UInt_t   fNoOfFields; ///< number of sampling points in field around the Lorentzian peak
    Double_t fRange;      ///< range in which the sampling points are placed, given in units of \beta(1\pm\Delta)

    static const Double_t fDegToRad = 0.0174532925199432955;
    static const Double_t fTwoPi = 6.28318530717958623;

  // definition of the class for the ROOT dictionary
  ClassDef(PSkewedLorentzian, 1)
};

#endif // _PSKEWEDLORENTZIAN_H_
