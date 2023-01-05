/***************************************************************************

  PMuppStartupHandler.h

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

#ifndef _PMUPPSTARTUPHANDLER_H_
#define _PMUPPSTARTUPHANDLER_H_

#include <TObject.h>
#include <TQObject.h>
#include <TList.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>
#include <TColor.h>
#include <TSAXParser.h>

#include "mupp_plot.h"

//--------------------------------------------------------------------------
// This function is a replacement for the ParseFile method of TSAXParser.
//--------------------------------------------------------------------------
int parseXmlFile(TSAXParser*, const char*);

class PMuppStartupHandler : public TObject, public TQObject
{
public:
  PMuppStartupHandler();
  virtual ~PMuppStartupHandler();

  virtual void OnStartDocument(); // SLOT
  virtual void OnEndDocument(); // SLOT
  virtual void OnStartElement(const Char_t*, const TList*); // SLOT
  virtual void OnEndElement(const Char_t*); // SLOT
  virtual void OnCharacters(const Char_t*); // SLOT
  virtual void OnComment(const Char_t*); // SLOT
  virtual void OnWarning(const Char_t*); // SLOT
  virtual void OnError(const Char_t*); // SLOT
  virtual void OnFatalError(const Char_t*); // SLOT
  virtual void OnCdataBlock(const Char_t*, Int_t); // SLOT

  virtual Bool_t StartupFileFound() { return fStartupFileFound; } ///< true = mupp_startup.xml found
  virtual TString GetStartupFilePath() { return fStartupFilePath; } ///< returns FULLPATH/mupp_startup.xml, where FULLPATH=path were the mupp_startup.xml is found

  virtual void CheckLists();

  virtual const PIntVector GetMarkerStyleList() const { return fMarkerStyleList; }  ///< returns the marker style list
  virtual const PDoubleVector GetMarkerSizeList() const { return fMarkerSizeList; } ///< returns the marker size list
  virtual const PIntVector GetColorList() const { return fColorList; }              ///< returns the color list

private:
  enum EKeyWords {eEmpty, eMarker, eColor};
  EKeyWords       fKey; ///< xml filter key

  Bool_t               fStartupFileFound; ///< startup file found flag
  TString              fStartupFilePath;  ///< full mupp_startup.xml startup file paths
  PIntVector           fMarkerStyleList;  ///< marker style list
  PDoubleVector        fMarkerSizeList;   ///< marker size list
  PIntVector           fColorList;        ///< color list

  Bool_t StartupFileExists(Char_t *fln);

  ClassDef(PMuppStartupHandler, 1)
};

#endif // _PMUPPSTARTUPHANDLER_H_
