/***************************************************************************

  PDepthProfile.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013-2022 by Andreas Suter                              *
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

#ifndef _PDEPTHPROFILE_H_
#define _PDEPTHPROFILE_H_

#include <vector>

#include "PUserFcnBase.h"
#include "PRgeHandler.h"

class PDepthProfileGlobal
{
  public:
    PDepthProfileGlobal();
    virtual ~PDepthProfileGlobal();

    Bool_t IsValid() { return fValid; }
    virtual Int_t GetEnergyIndex(const Double_t energy) { return fRgeHandler->GetEnergyIndex(energy); }
    virtual Double_t GetMuonStoppingDensity(const Int_t energyIndex, const Double_t z) const { return fRgeHandler->Get_n(energyIndex, z); }
    virtual double GetStoppingProbability(double a, double b, Double_t energy) const;

  private:
    Bool_t fValid{true};
    

    mutable std::vector<Double_t> fPreviousParam;

    PRgeHandler *fRgeHandler{nullptr};

  ClassDef(PDepthProfileGlobal, 1)
};

class PDepthProfile : public PUserFcnBase
{
  public:
    PDepthProfile() {}
    virtual ~PDepthProfile();

    virtual Bool_t NeedGlobalPart() const { return true; }
    virtual void SetGlobalPart(std::vector<void*> &globalPart, UInt_t idx);
    virtual Bool_t GlobalPartIsValid() const;

    virtual Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  private:
    Bool_t fValid{true};
    Bool_t fInvokedGlobal{false};
    Int_t fIdxGlobal;

    mutable std::vector<Double_t> fPreviousParam;
    PDepthProfileGlobal *fDepthProfileGlobal{nullptr};

  // definition of the class for the ROOT dictionary
  ClassDef(PDepthProfile, 1)
};

#endif // _PDEPTHPROFILE_H_
