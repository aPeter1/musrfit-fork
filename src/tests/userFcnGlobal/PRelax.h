/***************************************************************************

  PRelax.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
 *   andreas.suter@psi.c                                                   *
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

#ifndef _PRELAX_H_
#define _PRELAX_H_

#include <vector>
#include <iostream>

#include "PUserFcnBase.h"

class PExpGlobal;
class PGaussGlobal;

//-----------------------------------------------------------------------------------
/**
 * <p>User function example class. Global exponential function.
 */
class PExp : public PUserFcnBase
{
  public:
    PExp();
    virtual ~PExp();

    virtual Bool_t NeedGlobalPart() const { return true; }
    virtual void SetGlobalPart(vector<void*> &globalPart, UInt_t idx);
    virtual Bool_t GlobalPartIsValid() const;

    Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  private:
    Bool_t fValid;
    Bool_t fInvokedGlobal;
    Int_t fIdxGlobal;
    PExpGlobal *fGlobalUserFcn;

  ClassDef(PExp, 1)
};

//-----------------------------------------------------------------------------------
/**
 * <p>User function example class (global part). Global exponential function.
 */
class PExpGlobal
{
  public:
    PExpGlobal();
    virtual ~PExpGlobal();

    Bool_t IsValid() { return fValid; }
    void Calc(const std::vector<Double_t> &param);

  private:
    Bool_t fValid;
    std::vector<Double_t> fPrevParam;
    Double_t fRandom;

  ClassDef(PExpGlobal, 1)
};

//-----------------------------------------------------------------------------------
/**
 * <p>User function example class. Global Gaussian function.
 */
class PGauss : public PUserFcnBase
{
  public:
    PGauss();
    virtual ~PGauss();

    virtual Bool_t NeedGlobalPart() const { return true; }
    virtual void SetGlobalPart(vector<void*> &globalPart, UInt_t idx);
    virtual Bool_t GlobalPartIsValid() const;

    Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  private:
    Bool_t fValid;
    Bool_t fInvokedGlobal;
    Int_t fIdxGlobal;
    PGaussGlobal *fGlobalUserFcn;

  ClassDef(PGauss, 1)
};

//-----------------------------------------------------------------------------------
/**
 * <p>User function example class (global part). Global Gaussian function.
 */
class PGaussGlobal
{
  public:
    PGaussGlobal();
    virtual ~PGaussGlobal();

    Bool_t IsValid() { return fValid; }
    void Calc(const std::vector<Double_t> &param);

  private:
    Bool_t fValid;
    std::vector<Double_t> fPrevParam;
    Double_t fRandom;

  ClassDef(PGaussGlobal, 1)
};

#endif // _PRELAX_H_
