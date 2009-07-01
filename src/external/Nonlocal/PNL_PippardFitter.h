/***************************************************************************

  PNL_PippardFitter.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Andreas Suter                                   *
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

#include <fftw3.h>

#include "PUserFcnBase.h"
#include "PNL_StartupHandler.h"
#include "PNL_RgeHandler.h"

class PNL_PippardFitter : public PUserFcnBase
{
  public:
    PNL_PippardFitter();
    virtual ~PNL_PippardFitter();

    virtual Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  private:
    PNL_StartupHandler *fStartupHandler;
    PNL_RgeHandler *fRgeHandler;

    mutable std::vector<Double_t> fPreviousParam;

    Double_t f_dx;           // dx = xiPT dq
    mutable Double_t f_dz;           // spatial step size

    mutable Bool_t fPlanPresent;
    mutable fftw_plan     fPlan;
    mutable fftw_complex *fFieldq;   // (xiPT x)/(x^2 + xiPT^2 K(x,T)), x = q xiPT
    mutable fftw_complex *fFieldB;   // field calculated for specular boundary conditions
    mutable Int_t fShift; // shift needed to pick up fFieldB at the maximum for B->0

    virtual Bool_t NewParameters(const std::vector<Double_t> &param) const;
    virtual void CalculateField(const std::vector<Double_t> &param) const;
    virtual void CalculatePolarization(const std::vector<Double_t> &param) const;

    virtual Double_t DeltaBCS(const Double_t t) const;
    virtual Double_t LambdaL_T(const Double_t lambdaL, const Double_t t) const;
    virtual Double_t XiP_T(const Double_t xi0, const Double_t meanFreePath, Double_t t) const;

  ClassDef(PNL_PippardFitter, 1)
};

#endif // _PNL_PIPPARDFITTER_H_
