/***************************************************************************

  PMPRgeHandler.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2011 by Andreas Suter                                   *
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

#ifndef _PMPRGEHANDLER_H_
#define _PMPRGEHANDLER_H_

#include "PMagProximity.h"

class PMPRgeHandler
{
  public:
    PMPRgeHandler(const PStringVector &rgeDataPathList, const PDoubleVector &rgeDataEnergyList);
    virtual ~PMPRgeHandler();

    virtual Bool_t IsValid() { return fIsValid; }
    virtual Int_t GetRgeEnergyIndex(const Double_t energy);
    virtual Double_t GetRgeValue(const Double_t energy, const Double_t dist);
    virtual Double_t GetRgeValue(const Int_t index, const Double_t dist);

  private:
    Bool_t fIsValid;
    PMPRgeDataList fRgeDataList;

    virtual Bool_t LoadRgeData(const PStringVector &rgeDataPathList, const PDoubleVector &rgeDataEnergyList);
};

#endif // _PMPRGEHANDLER_H_
