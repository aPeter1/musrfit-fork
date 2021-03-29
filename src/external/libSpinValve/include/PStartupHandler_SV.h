/***************************************************************************

  PStartupHandler_SV.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013-2021 by Andreas Suter                              *
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

#ifndef _PSTARTUPHANDLER_SV_H_
#define _PSTARTUPHANDLER_SV_H_

#include <TObject.h>
#include <TQObject.h>
#include <TString.h>

class PStartupHandler_SV : public TObject
{
  public:
    PStartupHandler_SV();
    virtual ~PStartupHandler_SV() {}

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
    virtual UInt_t GetNoOfFields() { return fNoOfFields; }
    virtual Double_t GetRange() { return fRange; }

    virtual bool StartupFileFound() { return fStartupFileFound; }

  private:
    enum EKeyWords {eEmpty, eComment, eNoOfFields, eRange};
    EKeyWords      fKey;

    bool fIsValid;

    bool    fStartupFileFound;
    TString fStartupFilePath;

    UInt_t   fNoOfFields;
    Double_t fRange;

    bool StartupFileExists(char *fln);

  ClassDef(PStartupHandler_SV, 1)
};

#endif // _PSTARTUPHANDLER_SV_H_
