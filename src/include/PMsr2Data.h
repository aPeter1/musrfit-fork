/***************************************************************************

  PMsr2Data.h

  Author: Bastian M. Wojek / Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Bastian M. Wojek / Andreas Suter                *
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

#ifndef _PMSR2DATA_H_
#define _PMSR2DATA_H_

#include <string>
#include <sstream>
using namespace std;

#include "PRunDataHandler.h"
#include "PStartupHandler.h"

#include <TSAXParser.h>

//-------------------------------------------------------------
/**
 * <p>
 */
class PMsr2Data
{
  public:
    PMsr2Data(const string&); // File extension
    ~PMsr2Data();

    int SetRunNumbers(unsigned int); // single run given
    int SetRunNumbers(unsigned int, unsigned int); // run list specified through first and last run number
    int SetRunNumbers(const string&); // run list file given
    int SetRunNumbers(const vector<unsigned int>&); // explicit run list specified using [ ]
    unsigned int GetPresentRun() const;

    int ParseXmlStartupFile();
    int ReadMsrFile(const string&) const;
    bool ReadRunDataFile();

    bool PrepareNewInputFile(unsigned int) const; // template
    bool PrepareGlobalInputFile(unsigned int, const string&) const; // generate msr-input file for a global fit

    int WriteOutput(const string&, bool, bool, bool global = false, unsigned int counter = 0) const;

  private:
    string fFileExtension;
    vector<unsigned int> fRunVector;
    mutable vector<unsigned int>::const_iterator fRunVectorIter;
    bool fRunListFile;
    vector<string> fIndVar;
    ifstream *fRunListFileStream;
    TSAXParser *fSaxParser;
    PStartupHandler *fStartupHandler;
    mutable PRunDataHandler *fDataHandler;
    mutable PMsrHandler *fMsrHandler;
    mutable unsigned int fNumGlobalParam;
    mutable unsigned int fNumSpecParam;
    mutable unsigned int fNumTempRunBlocks;
};


#endif // _PMSR2DATA_H_
