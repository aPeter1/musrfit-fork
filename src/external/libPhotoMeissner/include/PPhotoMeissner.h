/***************************************************************************

  PPhotoMeissner.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013-2021 by Andreas Suter                              *
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

#ifndef _PPHOTOMEISSNER_H_
#define _PPHOTOMEISSNER_H_

#include <vector>

#include "PUserFcnBase.h"
#include "PStartupHandler_PM.h"

class PPhotoMeissner : public PUserFcnBase
{
  public:
    PPhotoMeissner();
    virtual ~PPhotoMeissner();

    virtual Bool_t IsValid() { return fValid; }

    // function operator
    Double_t operator()(Double_t, const std::vector<Double_t>&) const;

  private:
    PStartupHandler_PM *fStartupHandler;

    Bool_t   fValid;      ///< flag indicating if initialization went through smoothly

    constexpr static const Double_t fDegToRad = 0.0174532925199432955;
    constexpr static const Double_t fTwoPi = 6.28318530717958623;

    Double_t InuMinus(const Double_t nu, const Double_t x) const;
    Double_t FieldFilm(const Double_t z, const std::vector<Double_t> &par) const;
    Double_t FieldHalfSpace(const Double_t z, const std::vector<Double_t> &par) const;

  // definition of the class for the ROOT dictionary
  ClassDef(PPhotoMeissner, 1)
};

#endif // _PPHOTOMEISSNER_H_
