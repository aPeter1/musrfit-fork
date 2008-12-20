/***************************************************************************

  PStartupHandler.h

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

#ifndef _PSTARTUPHANDLER_H_
#define _PSTARTUPHANDLER_H_

#include <TObject.h>
#include <TQObject.h>
#include <TList.h>
#include <TString.h>

#include "PMusr.h"

class PStartupHandler : public TObject, public TQObject
{
  public:
    PStartupHandler();
    virtual ~PStartupHandler();

    virtual void OnStartDocument(); // SLOT
    virtual void OnEndDocument(); // SLOT
    virtual void OnStartElement(const char*, const TList*); // SLOT
    virtual void OnEndElement(const char*); // SLOT
    virtual void OnCharacters(const char*); // SLOT
    virtual void OnComment(const char*); // SLOT
    virtual void OnWarning(const char*); // SLOT
    virtual void OnError(const char*); // SLOT
    virtual void OnFatalError(const char*); // SLOT
    virtual void OnCdataBlock(const char*, Int_t); // SLOT

    virtual void CheckLists();

    virtual PMsrFourierStructure* GetFourierDefaults() { return &fFourierDefaults; }
    virtual const PStringVector GetDataPathList() const { return fDataPathList; }
    virtual const PIntVector    GetMarkerList() const { return fMarkerList; }
    virtual const PIntVector    GetColorList() const { return fColorList; }

  private:
    enum EKeyWords {eEmpty, eComment, eDataPath,
                    eFourierSettings, eUnits, eFourierPower, eApodization, ePlot, ePhase,
                    eRootSettings, eMarkerList, eMarker, 
                    eColorList, eColor};
    EKeyWords       fKey;

    PMsrFourierStructure fFourierDefaults;
    PStringVector        fDataPathList;
    PIntVector           fMarkerList;
    PIntVector           fColorList;

  ClassDef(PStartupHandler, 1)
};

#endif // _PSTARTUPHANDLER_H_

