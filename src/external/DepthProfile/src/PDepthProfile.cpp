/***************************************************************************

  PDepthProfile.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2022 by Andreas Suter                              *
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

#include <cassert>
#include <cmath>

#include <iostream>

#include <TSAXParser.h>
#include <TMath.h>

#include "PDepthProfile.h"

ClassImp(PDepthProfileGlobal)

//--------------------------------------------------------------------------
// Constructor (PDepthProfileGlobal)
//--------------------------------------------------------------------------
/**
 * <p>Constructor. Reads the necessary rge-files based on the depth_profile_startup.xml
 */
PDepthProfileGlobal::PDepthProfileGlobal()
{
  // load all the TRIM.SP rge-files
  fRgeHandler = new PRgeHandler("./depth_profile_startup.xml");
  if (!fRgeHandler->IsValid()) {
    std::cout << std::endl << ">> PDepthProfileGlobal::PDepthProfileGlobal **PANIC ERROR**";
    std::cout << std::endl << ">>  rge data handler too unhappy. Will terminate unfriendly, sorry.";
    std::cout << std::endl;
    fValid = false;
  }
}

//--------------------------------------------------------------------------
// Destructor (PDepthProfileGlobal)
//--------------------------------------------------------------------------
/**
 * <p>Clean up the rge-handler.
 */
PDepthProfileGlobal::~PDepthProfileGlobal()
{
  if (fRgeHandler) {
    delete fRgeHandler;
    fRgeHandler = nullptr;
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImp(PDepthProfile)

//--------------------------------------------------------------------------
// Destructor (PDepthProfile)
//--------------------------------------------------------------------------
/**
 * <p>Clean up the global part.
 */
PDepthProfile::~PDepthProfile()
{
  if ((fDepthProfileGlobal != 0) && fInvokedGlobal) {
    delete fDepthProfileGlobal;
    fDepthProfileGlobal = nullptr;
  }
}

//--------------------------------------------------------------------------
// SetGlobalPart (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 *
 * \param globalPart
 * \param idx
 */
void PDepthProfile::SetGlobalPart(std::vector<void*> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) {
    fDepthProfileGlobal = new PDepthProfileGlobal();
    if (fDepthProfileGlobal == nullptr) {
      fValid = false;
      std::cerr << std::endl << ">> PDepthProfile::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << std::endl;
    } else if (!fDepthProfileGlobal->IsValid()) {
      fValid = false;
      std::cerr << std::endl << ">> PDepthProfile::SetGlobalPart(): **ERROR** initialization of global user function object failed, sorry ..." << std::endl;
    } else {
      fValid = true;
      fInvokedGlobal = true;
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<PDepthProfileGlobal*>(fDepthProfileGlobal);
    }
  } else {
    fValid = true;
    fDepthProfileGlobal = (PDepthProfileGlobal*)globalPart[fIdxGlobal];
  }
}

//--------------------------------------------------------------------------
// GlobalPartIsValid (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 */
Bool_t PDepthProfile::GlobalPartIsValid() const
{
  return (fValid && fDepthProfileGlobal->IsValid());
}

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 * <p>Yet to be implemented.
 */
Double_t PDepthProfile::operator()(Double_t t, const std::vector<Double_t> &param) const
{
  return 0.0;
}

