/***************************************************************************

  PStartupHandler.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2012 by Andreas Suter                              *
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

#ifndef _PSTARTUPHANDLER_H_
#define _PSTARTUPHANDLER_H_

#include <TObject.h>
#include <TQObject.h>
#include <TList.h>
#include <TString.h>
#include <TSAXParser.h>

#include "PMusr.h"

//--------------------------------------------------------------------------
// This function is a replacement for the ParseFile method of TSAXParser.
//--------------------------------------------------------------------------
int parseXmlFile(TSAXParser*, const char*);

/**
 * <p>Handles the XML musrfit startup file (musrfit_startup.xml) in which default settings
 * are stored:
 * - list of search paths for the data files
 * - Fourier transform default settings
 * - marker list for plotting
 * - color list for plotting
 */
class PStartupHandler : public TObject, public TQObject
{
  public:
    PStartupHandler();
    virtual ~PStartupHandler();

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

    virtual Bool_t StartupFileFound() { return fStartupFileFound; } ///< true = musrfit_startup.xml found
    virtual TString GetStartupFilePath() { return fStartupFilePath; } ///< returns FULLPATH/musrfit_startup.xml, where FULLPATH=path were the musrfit_startup.xml is found

    virtual void CheckLists();

    virtual const Bool_t         GetWritePerRunBlockChisq() { return fWritePerRunBlockChisq; } ///< returns the write_per_run_block_chisq flag
    virtual PMsrFourierStructure GetFourierDefaults() { return fFourierDefaults; } ///< returns the Fourier defaults
    virtual const PStringVector  GetDataPathList() const { return fDataPathList; } ///< returns the search data path list
    virtual const PIntVector     GetMarkerList() const { return fMarkerList; }     ///< returns the marker list
    virtual const PIntVector     GetColorList() const { return fColorList; }       ///< returns the color list

  private:
    enum EKeyWords {eEmpty, eComment, eDataPath, eWritePerRunBlockChisq,
                    eFourierSettings, eUnits, eFourierPower, eApodization, ePlot, ePhase, ePhaseIncrement,
                    eRootSettings, eMarkerList, eMarker, 
                    eColorList, eColor};
    EKeyWords       fKey; ///< xml filter key

    Bool_t               fStartupFileFound; ///< startup file found flag
    TString              fStartupFilePath;  ///< full musrfit_startup.xml startup file paths
    Bool_t               fWritePerRunBlockChisq; ///< flag showing if per run block chisq and the expected chisq shall be written to the msr-file
    PMsrFourierStructure fFourierDefaults;  ///< Fourier defaults
    PStringVector        fDataPathList;     ///< search data path list
    PIntVector           fMarkerList;       ///< marker list
    PIntVector           fColorList;        ///< color list

    Bool_t StartupFileExists(Char_t *fln);

  ClassDef(PStartupHandler, 1)
};

#endif // _PSTARTUPHANDLER_H_

