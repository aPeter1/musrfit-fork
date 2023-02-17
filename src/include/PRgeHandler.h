/***************************************************************************

  PRgeHandler.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2023 by Andreas Suter                              *
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

#ifndef _PRGEHANDLER_H_
#define _PRGEHANDLER_H_

#include <string>
#include <vector>

#include <TObject.h>
#include <TQObject.h>
#include <TSAXParser.h>

#include "PMusr.h"

//-----------------------------------------------------------------------------
/**
 * <p>Keep a single rge table from TrimSP for a given energy.
 */
struct PRgeData {
  Double_t energy;
  PDoubleVector depth;
  PDoubleVector amplitude;
  PDoubleVector nn; // normalized int n(z) dz = 1 amplitudes
  Double_t noOfParticles;
};

//-----------------------------------------------------------------------------
/**
 * <p>Keep all rge tables from TrimSP.
 */
typedef std::vector<PRgeData> PRgeDataList;

//-----------------------------------------------------------------------------
/**
 * <p>Reads the xml-startup file in order to extract all necessary information
 * about the RGE files (TrimSP) needed.
 */
class PXmlRgeHandler : public TObject, public TQObject
{
  public:
    PXmlRgeHandler() {}
    virtual ~PXmlRgeHandler() {}

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
    virtual std::string GetTrimSpDataPath() { return fTrimSpDataPath; }
    virtual std::string GetTrimSpFlnPre() { return fTrimSpFlnPre; }
    virtual const PIntVector GetTrimSpDataVectorList() const { return fTrimSpDataEnergyList; }

  private:
    enum EKeyWords {eEmpty, eDataPath, eFlnPre, eEnergy};
    EKeyWords      fKey;

    bool isTrimSp{false};
    bool fIsValid{true};

    std::string fTrimSpDataPath{""}; //< where to find the rge-files
    std::string fTrimSpFlnPre{""};   //< keeps the preface of the rge file name, e.g. LCCO_E
    PIntVector  fTrimSpDataEnergyList; //< TrimSP implantation energy list (in eV)

  ClassDef(PXmlRgeHandler, 1)
};

//-----------------------------------------------------------------------------
/**
 * @brief The PRegHandler class
 */
class PRgeHandler : public TObject
{
  public:
    PRgeHandler(std::string fln="");
    virtual ~PRgeHandler() {}

    virtual bool IsValid() { return fValid; }
    virtual UInt_t GetNoOfRgeDataSets() { return (UInt_t)fData.size(); }
    virtual PRgeDataList GetRgeData() { return fData; }
    virtual Double_t GetZmax(const Double_t energy);
    virtual Double_t GetZmax(const Int_t idx);
    virtual Double_t Get_n(const Double_t energy, const Double_t z);
    virtual Double_t Get_n(const Int_t idx, const Double_t z);
    virtual Int_t GetEnergyIndex(const Double_t energy);

  private:
    bool fValid{false};
    PRgeDataList fData;

    virtual bool ReadRgeFile(const std::string fln, PRgeData &data);

  ClassDef(PRgeHandler, 1)
};

#endif // _PRGEHANDLER_H_
