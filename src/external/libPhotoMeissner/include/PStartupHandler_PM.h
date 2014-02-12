/***************************************************************************

  PStartupHandler_PM.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013 by Andreas Suter                                   *
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

#ifndef _PSTARTUPHANDLER_PM_H_
#define _PSTARTUPHANDLER_PM_H_

#include <vector>

#include <TObject.h>
#include <TQObject.h>
#include <TString.h>

//-------------------------------------------------------------
/**
 * <p> typedef to make to code more readable.
 */
typedef std::vector<TString> PStringVector;

//-------------------------------------------------------------
/**
 * <p> typedef to make to code more readable.
 */
typedef std::vector<Double_t> PDoubleVector;

//-------------------------------------------------------------
/**
 * <p>
 */
typedef struct {
  Double_t energy;
  PDoubleVector stoppingDistance;
  PDoubleVector stoppingAmplitude;
} PRgeData_PM;

//-------------------------------------------------------------
/**
 * <p>
 */
typedef std::vector<PRgeData_PM> PRgeData_PM_List;

//-------------------------------------------------------------
/**
 * <p>
 */
class PRgeHandler_PM
{
  public:
    PRgeHandler_PM(const PStringVector &rgeDataPathList, const PDoubleVector &rgeDataEnergyList);
    virtual ~PRgeHandler_PM();

    virtual Bool_t IsValid() { return fIsValid; }
    virtual Int_t GetRgeEnergyIndex(const Double_t energy);
    virtual Double_t GetRgeValue(const Double_t energy, const Double_t dist);
    virtual Double_t GetRgeValue(const Int_t index, const Double_t dist);

  private:
    Bool_t fIsValid;
    PRgeData_PM_List fRgeDataList;

    virtual Bool_t LoadRgeData(const PStringVector &rgeDataPathList, const PDoubleVector &rgeDataEnergyList);
};

//-------------------------------------------------------------
/**
 * <p>
 */
class PStartupHandler_PM : public TObject
{
  public:
    PStartupHandler_PM();
    virtual ~PStartupHandler_PM() {}

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

    virtual bool StartupFileFound() { return fStartupFileFound; }

    virtual Int_t GetRgeEnergyIndex(const Double_t energy);
    virtual Double_t GetRgeValue(const Double_t energy, const Double_t dist);
    virtual Double_t GetRgeValue(const Int_t index, const Double_t dist);

  private:
    enum EKeyWords {eEmpty, eDataPath, eEnergy};
    EKeyWords      fKey;

    bool fIsValid;

    bool    fStartupFileFound;
    TString fStartupFilePath;

    TString fRgePath;
    PStringVector fRgeFilePathList;
    PDoubleVector fRgeDataEnergyList;

    PRgeHandler_PM *fRgeHandler;

    bool StartupFileExists(char *fln);

  ClassDef(PStartupHandler_PM, 1)
};

#endif // _PSTARTUPHANDLER_PM_H_
