/***************************************************************************

  PAddPoissonNoise.h

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

#ifndef _PADDPOISSONNOISE_H_
#define _PADDPOISSONNOISE_H_

#include <TObject.h>
#include <TH1F.h>
#include <TRandom3.h>

class PAddPoissonNoise : public TObject
{
  public:
    PAddPoissonNoise(UInt_t seed = 0);
    virtual ~PAddPoissonNoise();

    virtual Bool_t IsValid() { return fValid; }
    virtual void SetSeed(UInt_t seed);
    virtual Double_t AddNoise(Double_t val);
    virtual void AddNoise(TH1F *histo);

  private:
    Bool_t fValid;
    TRandom3 *fRandom;

    Double_t fSquareRoot;
    Double_t fAlxm;
    Double_t fG;
    Double_t fOldMean;

    virtual Double_t PoiDev(const Double_t &mean);

  ClassDef(PAddPoissonNoise, 1)
};

#endif // _PADDPOISSONNOISE_H_
