/***************************************************************************

  PNL_PippardFitter.h

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

#ifndef _PNL_PIPPARDFITTER_H_
#define _PNL_PIPPARDFITTER_H_

// the following ifdef is needed for GCC 4.6 or higher, fftw 3.3 or higher and root 5.30.03 or lower
//#ifdef __CINT__
//typedef struct { char a[7]; } __float128; // needed since cint doesn't know it
//#endif
#include <fftw3.h>

#include "PMusr.h"
#include "PUserFcnBase.h"
#include "PNL_StartupHandler.h"
#include "PRgeHandler.h"

class PNL_PippardFitterGlobal
{
  public:
    PNL_PippardFitterGlobal();
    virtual ~PNL_PippardFitterGlobal();

    Bool_t IsValid() { return fValid; }
    virtual void SetTempExponent(const Double_t nn) { f_nn = nn; }
    virtual void CalculateField(const std::vector<Double_t> &param) const;
    virtual Int_t GetEnergyIndex(const Double_t energy) { return fRgeHandler->GetEnergyIndex(energy); }
    virtual Double_t GetMuonStoppingDensity(const Int_t energyIndex, const Double_t z) const { return fRgeHandler->Get_n(energyIndex, z); }
    virtual Double_t GetMagneticField(const Double_t z) const;    

  private:
    Bool_t fValid{true};

    PNL_StartupHandler *fStartupHandler{nullptr};
    PRgeHandler *fRgeHandler{nullptr};

    mutable std::vector<Double_t> fPreviousParam;

    Double_t f_nn;
    Double_t f_dx;           // dx = xiPT dq
    mutable Double_t f_dz;   // spatial step size

    mutable Bool_t fPlanPresent;
    mutable Int_t fFourierPoints;
    mutable fftw_plan     fPlan;
    mutable fftw_complex *fFieldq;   // (xiPT x)/(x^2 + xiPT^2 K(x,T)), x = q xiPT
    mutable fftw_complex *fFieldB;   // field calculated for specular boundary conditions
    mutable Int_t fShift; // shift needed to pick up fFieldB at the maximum for B->0

    mutable Int_t fEnergyIndex; // keeps the proper index to select n(z)


    virtual Double_t DeltaBCS(const Double_t t) const;
    virtual Double_t LambdaL_T(const Double_t lambdaL, const Double_t t) const;
    virtual Double_t XiP_T(const Double_t xi0, const Double_t meanFreePath, Double_t t) const;

  ClassDef(PNL_PippardFitterGlobal, 1)
};

class PNL_PippardFitter : public PUserFcnBase
{
  public:
    PNL_PippardFitter() {}
    virtual ~PNL_PippardFitter();

    virtual Bool_t NeedGlobalPart() const { return true; }
    virtual void SetGlobalPart(std::vector<void*> &globalPart, UInt_t idx);
    virtual Bool_t GlobalPartIsValid() const;

    virtual Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  private:
    Bool_t fValid{true};
    Bool_t fInvokedGlobal{false};
    Int_t fIdxGlobal;

    PNL_PippardFitterGlobal *fPippardFitterGlobal{nullptr};

  ClassDef(PNL_PippardFitter, 1)
};

#endif // _PNL_PIPPARDFITTER_H_
