/***************************************************************************

  PFitterFcn.h

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

#ifndef _PFITTERFCN_H_
#define _PFITTERFCN_H_

#include <vector>

#include "Minuit2/FCNBase.h"

#include "PRunListCollection.h"

class PFitterFcn : public ROOT::Minuit2::FCNBase
{
  public:
    PFitterFcn(PRunListCollection *runList, Bool_t useChi2);
    ~PFitterFcn();

    Double_t Up() const { return fUp; }
    Double_t operator()(const std::vector<Double_t>&) const;

    UInt_t GetTotalNoOfFittedBins() { return fRunListCollection->GetTotalNoOfBinsFitted(); }

  private:
    Double_t fUp;
    Bool_t fUseChi2;
    PRunListCollection *fRunListCollection;
};

#endif // _PFITTERFCN_H_
