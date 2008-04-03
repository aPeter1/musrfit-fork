/***************************************************************************

  PRunDataHandler.h

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

#ifndef _PRUNDATAHANDLER_H_
#define _PRUNDATAHANDLER_H_

#include <vector>
using namespace std;

#include <TString.h>

#include "PMusr.h"
#include "PMsrHandler.h"

class PRunDataHandler
{
  public:
    PRunDataHandler(PMsrHandler *msrInfo, const PStringVector dataPath);
    virtual ~PRunDataHandler();

    virtual bool IsAllDataAvailable() { return fAllDataAvailable; }
    virtual PRawRunData* GetRunData(TString runName);

  private:
    PMsrHandler   *fMsrInfo;
    PStringVector fDataPath;

    bool fAllDataAvailable; ///< flag indicating if all data sets could be read
    TString fRunName;       ///< current run name
    TString fRunPathName;   ///< current path file name 
    PRawRunDataList fData;  ///< keeping all the raw data

    virtual bool ReadFile();
    virtual bool FileAlreadyRead(PMsrRunStructure &runInfo);
    virtual bool FileExistsCheck(PMsrRunStructure &runInfo);
    virtual bool ReadRootFile();
    virtual bool ReadNexusFile();
    virtual bool ReadNemuFile();
    virtual bool ReadPsiBinFile();
    virtual bool ReadMudFile();
    virtual bool ReadAsciiFile();

    virtual bool StripWhitespace(TString &str);
    virtual bool IsWhitespace(const char *str);
    virtual double ToDouble(TString &str, bool &ok);
    virtual int ToInt(TString &str, bool &ok);
};

#endif //  _PRUNDATAHANDLER_H_
