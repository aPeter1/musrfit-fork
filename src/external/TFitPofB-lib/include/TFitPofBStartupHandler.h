/***************************************************************************

  TFitPofBStartupHandler.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/06/03

  based upon:
  $Id: PStartupHandler.h 3396 2008-05-29 14:41:33Z nemu $
  by Andreas Suter

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter, Bastian M. Wojek                 *
 *                                                                         *
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

#ifndef _TFITPOFBSTARTUPHANDLER_H_
#define _TFITPOFBSTARTUPHANDLER_H_

#include <TList.h>
#include <TQObject.h>
#include <string>
#include <vector>

class TFitPofBStartupHandler : public TQObject {
  public:
    TFitPofBStartupHandler();
    virtual ~TFitPofBStartupHandler();

    virtual void OnStartDocument(); // SLOT
    virtual void OnEndDocument(); // SLOT
    virtual void OnStartElement(const char*, const TList*); // SLOT
    virtual void OnEndElement(const char*); // SLOT
    virtual void OnCharacters(const char*); // SLOT
    virtual void OnComment(const char*); // SLOT
    virtual void OnWarning(const char*); // SLOT
    virtual void OnError(const char*); // SLOT
    virtual void OnFatalError(const char*); // SLOT
    virtual void OnCdataBlock(const char*, int); // SLOT

    virtual void CheckLists();

    virtual const string GetDataPath() const { return fDataPath; }
    virtual vector<string> GetEnergyList() const { return fEnergyList; }
    virtual const double GetDeltat() const { return fDeltat; }
    virtual const double GetDeltaB() const { return fDeltaB; }
    virtual const string GetWisdomFile() const { return fWisdomFile; }

  private:
    enum EKeyWords {eEmpty, eComment, eDataPath, eEnergy, eEnergyList, eDeltat, eDeltaB, eWisdomFile};

    EKeyWords       fKey;

    string          fDataPath;
    vector<string>  fEnergyList;
    double          fDeltat;
    double          fDeltaB;
    string          fWisdomFile;

  ClassDef(TFitPofBStartupHandler, 1)
};

#endif // _TFITPOFBSTARTUPHANDLER_H_

