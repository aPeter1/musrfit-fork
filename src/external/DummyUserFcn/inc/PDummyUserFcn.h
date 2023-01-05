/***************************************************************************

  PDummyUserFcn.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013-2023 by Andreas Suter                              *
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

#ifndef _PDUMMYUSERFNC_H_
#define _PDUMMYUSERFNC_H_

#include <vector>

#include "PUserFcnBase.h"

class PDummyUserFcnGlobal
{
  public:
    PDummyUserFcnGlobal();
    virtual ~PDummyUserFcnGlobal();

    Bool_t IsValid() { return fValid; }
    virtual void CalculatePol(const std::vector<Double_t> &param) const;
    virtual Double_t GetPolValue(const Double_t t) const;

  private:
    Bool_t fValid{true};

    mutable std::vector<Double_t> fPreviousParam;

    mutable Double_t fTimeStep{1.0e-3}; // time in (us), time step 1 (ns) here
    mutable std::vector<Double_t> fPol;

  ClassDef(PDummyUserFcnGlobal, 1)
};

class PDummyUserFcn : public PUserFcnBase
{
  public:
    PDummyUserFcn() {}
    virtual ~PDummyUserFcn();

    virtual Bool_t NeedGlobalPart() const { return true; }
    virtual void SetGlobalPart(std::vector<void*> &globalPart, UInt_t idx);
    virtual Bool_t GlobalPartIsValid() const;

    virtual Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  private:
    Bool_t fValid{true};
    Bool_t fInvokedGlobal{false};
    Int_t fIdxGlobal;

    PDummyUserFcnGlobal *fDummyUserFcnGlobal{nullptr};

  // definition of the class for the ROOT dictionary
  ClassDef(PDummyUserFcn, 1)
};

#endif // _PDUMMYUSERFNC_H_
