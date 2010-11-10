/***************************************************************************

  PNL_RgeHandler.h

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

#ifndef _PNL_RGEHANDLER_H_
#define _PNL_RGEHANDLER_H_

#include "PNonlocal.h"

class PNL_RgeHandler
{
  public:
    PNL_RgeHandler(const PStringVector &rgeDataPathList, const PDoubleVector &rgeDataEnergyList);
    virtual ~PNL_RgeHandler();

    virtual Bool_t IsValid() { return fIsValid; }
    virtual Int_t GetRgeEnergyIndex(const Double_t energy);
    virtual Double_t GetRgeValue(const Double_t energy, const Double_t dist);
    virtual Double_t GetRgeValue(const Int_t index, const Double_t dist);

  private:
    Bool_t fIsValid;
    PNL_RgeDataList fRgeDataList;

    virtual Bool_t LoadRgeData(const PStringVector &rgeDataPathList, const PDoubleVector &rgeDataEnergyList);
};

#endif // _PNL_RGEHANDLER_H_
