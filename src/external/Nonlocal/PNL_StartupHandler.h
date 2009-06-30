/***************************************************************************

  PNL_StartupHandler.h

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

#ifndef _PNL_STARTUPHANDLER_H_
#define _PNL_STARTUPHANDLER_H_

#include <TObject.h>
#include <TQObject.h>
#include <TString.h>

#include "PNonlocal.h"

class PNL_StartupHandler : public TObject
{
  public:
    PNL_StartupHandler();
    virtual ~PNL_StartupHandler();

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

    virtual bool IsValid() { return fIsValid; }
    virtual TString GetStartupFilePath() { return fStartupFilePath; }
    virtual const Int_t GetFourierPoints() const { return fFourierPoints; }
    virtual const PStringVector GetTrimSpDataPathList() const { return fTrimSpDataPathList; }

    virtual bool StartupFileFound() { return fStartupFileFound; }

  private:
    enum EKeyWords {eEmpty, eComment, eFourierPoints, eDataPath, eEnergy};
    EKeyWords      fKey;

    bool fIsValid;

    bool    fStartupFileFound;
    TString fStartupFilePath;

    Int_t fFourierPoints;
    TString fTrimSpDataPath;
    PStringVector fTrimSpDataPathList;

    bool StartupFileExists(char *fln);

  ClassDef(PNL_StartupHandler, 1)
};

#endif // _PNL_STARTUPHANDLER_H_
