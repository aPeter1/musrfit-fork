/***************************************************************************

  PRunDataHandler.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
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

#ifndef _PRUNDATAHANDLER_H_
#define _PRUNDATAHANDLER_H_

#include <vector>
using namespace std;

#include <TString.h>

#include "PMusr.h"
#include "PMsrHandler.h"

/**
 * <p>Handler class needed to read/handle raw data files.
 */
class PRunDataHandler
{
  public:
    PRunDataHandler(PAny2ManyInfo *any2ManyInfo, const PStringVector dataPath);
    PRunDataHandler(PMsrHandler *msrInfo);
    PRunDataHandler(PMsrHandler *msrInfo, const PStringVector dataPath);
    virtual ~PRunDataHandler();

    virtual Bool_t IsAllDataAvailable() const { return fAllDataAvailable; }
    virtual PRawRunData* GetRunData(const TString &runName);

  private:
    PMsrHandler   *fMsrInfo; ///< pointer to the msr-file handler
    PAny2ManyInfo *fAny2ManyInfo; ///< pointer to the any2many data structure
    PStringVector fDataPath; ///< vector containing all the search paths where to look for data files

    Bool_t fAllDataAvailable; ///< flag indicating if all data sets could be read
    TString fRunName;       ///< current run name
    TString fRunPathName;   ///< current path file name 
    PRawRunDataList fData;  ///< keeping all the raw data

    virtual Bool_t ReadFilesMsr();
    virtual Bool_t ReadWriteFilesList();
    virtual Bool_t FileAlreadyRead(TString runName);
    virtual Bool_t FileExistsCheck(PMsrRunBlock &runInfo, const UInt_t idx);
    virtual Bool_t FileExistsCheck(const Bool_t fileName, const Int_t idx);
    virtual Bool_t ReadRootFile(UInt_t tag);
    virtual Bool_t ReadNexusFile();
    virtual Bool_t ReadWkmFile();
    virtual Bool_t ReadPsiBinFile();
    virtual Bool_t ReadMudFile();
    virtual Bool_t ReadMduAsciiFile();
    virtual Bool_t ReadAsciiFile();
    virtual Bool_t ReadDBFile();

    virtual Bool_t WriteRootFile(TString fln="");
    virtual Bool_t WriteNexusFile(TString fln="");
    virtual Bool_t WriteWkmFile(TString fln="");
    virtual Bool_t WritePsiBinFile(TString fln="");
    virtual Bool_t WriteMudFile(TString fln="");
    virtual Bool_t WriteAsciiFile(TString fln="");

    virtual Bool_t StripWhitespace(TString &str);
    virtual Bool_t IsWhitespace(const Char_t *str);
    virtual Double_t ToDouble(TString &str, Bool_t &ok);
    virtual Int_t ToInt(TString &str, Bool_t &ok);
    virtual Int_t GetDataTagIndex(TString &str, const PStringVector* fLabels);

    virtual TString GetFileName(const TString extension, Bool_t &ok);
    virtual TString FileNameFromTemplate(TString &fileNameTemplate, Int_t run, TString &year, Bool_t &ok);
    virtual TString GetMonth(Int_t month);
};

#endif //  _PRUNDATAHANDLER_H_
