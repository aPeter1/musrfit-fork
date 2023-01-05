/***************************************************************************

  PMagProximity.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2023 by Andreas Suter                              *
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

#ifndef _PMAGPROXIMITYFITTER_H_
#define _PMAGPROXIMITYFITTER_H_

#include "PUserFcnBase.h"
#include "PMPStartupHandler.h"
#include "PRgeHandler.h"

class PMagProximityFitterGlobal
{
  public:
    PMagProximityFitterGlobal();
    virtual ~PMagProximityFitterGlobal();

    Bool_t IsValid() { return fValid; }
    virtual void CalculateField(const std::vector<Double_t> &param) const;
    virtual Int_t GetEnergyIndex(const Double_t energy) { return fRgeHandler->GetEnergyIndex(energy); }
    virtual Double_t GetMuonStoppingDensity(const Int_t energyIndex, const Double_t z) const { return fRgeHandler->Get_n(energyIndex, z); }
    virtual Double_t GetMagneticField(const Double_t z) const;

  private:
    Bool_t fValid;

    PMPStartupHandler *fStartupHandler;
    PRgeHandler *fRgeHandler;

    mutable std::vector<Double_t> fPreviousParam;

    mutable Int_t fEnergyIndex; // keeps the proper index to select n(z)

    mutable Double_t fDz;
    mutable std::vector<Double_t> fField;

  ClassDef(PMagProximityFitterGlobal, 1)
};

class PMagProximityFitter : public PUserFcnBase
{
  public:
    PMagProximityFitter();
    virtual ~PMagProximityFitter();

    virtual Bool_t NeedGlobalPart() const { return true; }
    virtual void SetGlobalPart(std::vector<void*> &globalPart, UInt_t idx);
    virtual Bool_t GlobalPartIsValid() const;

    virtual Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  private:
    Bool_t fValid;
    Bool_t fInvokedGlobal;
    Int_t fIdxGlobal;

    PMagProximityFitterGlobal *fMagProximityFitterGlobal;

  ClassDef(PMagProximityFitter, 1)
};

#endif // _PMAGPROXIMITYFITTER_H_
