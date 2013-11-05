/***************************************************************************

  PAddPoissonNoise.cpp

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

#include <iostream>
using namespace std;

#include <TMath.h>

#include "PAddPoissonNoise.h"

ClassImp(PAddPoissonNoise)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p> Constructor.
 *
 * \param seed for the random number generator
 */
PAddPoissonNoise::PAddPoissonNoise(UInt_t seed)
{
  fValid = true;

  fRandom = new TRandom3(seed);
  if (fRandom == 0) {
    fValid = false;
  }

  fSquareRoot = 0.0;
  fAlxm = 0.0;
  fG = 0.0;
  fOldMean = -1.0;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p> Destructor.
 *
 */
PAddPoissonNoise::~PAddPoissonNoise()
{
  if (fRandom) {
    delete fRandom;
    fRandom = 0;
  }
}

//--------------------------------------------------------------------------
// SetSeed (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the seed of the random generator.
 *
 * \param seed for the random number generator
 */
void PAddPoissonNoise::SetSeed(UInt_t seed)
{
  if (!fValid)
    return;

  fRandom->SetSeed(seed);
}

//--------------------------------------------------------------------------
// AddNoise (public)
//--------------------------------------------------------------------------
/**
 * <p>Adds Poisson noise to a single input value.
 *
 * \param val input value.
 */
Double_t PAddPoissonNoise::AddNoise(Double_t val)
{
  return PoiDev(val);
}

//--------------------------------------------------------------------------
// AddNoise (public)
//--------------------------------------------------------------------------
/**
 * <p>Adds Poisson noise to a histogram.
 *
 * \param histo histogram to which Poisson noise is added.
 */
void PAddPoissonNoise::AddNoise(TH1F *histo)
{
  if (histo == 0)
    return;

  Double_t dval;
  for (UInt_t i=1; i<(UInt_t)histo->GetNbinsX(); i++) {
    dval = histo->GetBinContent(i);
    histo->SetBinContent(i, PoiDev(dval));
  }
}

//--------------------------------------------------------------------------
// PoiDev (private)
//--------------------------------------------------------------------------
/**
 * <p>Calculates Poisson noise. Taken from Numerical Recipes: method NR::poidev, chapter 7, p. 298
 * Numerical Recipes in C++, 2nd Edition 2002.
 *
 * Returns as a floating-point number an integer value that is a random deviate drawn from a Poisson
 * distribution of mean "mean".
 *
 * \param mean value to which Poisson noise should be added.
 */
Double_t PAddPoissonNoise::PoiDev(const Double_t &mean)
{
  Double_t em, t, y;

  if (mean < 12.0) {
    if (mean != fOldMean) {
      fOldMean = mean;
      fG = exp(-mean);
    }
    em = -1.0;
    t  =  1.0;
    do {
      ++em;
      t *= fRandom->Rndm();
    } while (t > fG);
  } else {
    if (mean != fOldMean) {
      fOldMean = mean;
      fSquareRoot = sqrt(2.0*mean);
      fAlxm = log(mean);
      fG = mean*fAlxm-TMath::LnGamma(mean+1.0);
    }
    do {
      do {
        y = tan(TMath::Pi()*fRandom->Rndm());
        em = fSquareRoot*y+mean;
      } while (em < 0.0);
      em = TMath::Floor(em);
      t = 0.9*(1.0+y*y)*exp(em*fAlxm-TMath::LnGamma(em+1.0)-fG);
    } while (fRandom->Rndm() > t);
  }

  return em;
}
