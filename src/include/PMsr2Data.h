/***************************************************************************

  PMsr2Data.h

  Author: Bastian M. Wojek / Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2023 by Bastian M. Wojek / Andreas Suter           *
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

// note: msr2data is on purpose implemented in a way that shows string handling can be done solely
//       using std::string, boost and related standard C++ features
//       This implies, however, occasionally strange constructs when interoperating with PMusr-classes
//       which mostly rely on ROOT's TString.

#ifndef _PMSR2DATA_H_
#define _PMSR2DATA_H_

#include <string>
#include <sstream>

#include "PRunDataHandler.h"
#include "PStartupHandler.h"

#include <TSAXParser.h>

//-------------------------------------------------------------
/**
 * <p> Class providing the necessary utilities for msr2data:
 *     generate new msr-files from a template, collect fit parameters from msr-files and write them to DB or plain ASCII files
 */
class PMsr2Data
{
  public:
    PMsr2Data(const std::string&); // File extension
    ~PMsr2Data();

    int SetRunNumbers(unsigned int); // single run given
    int SetRunNumbers(unsigned int, unsigned int); // run list specified through first and last run number
    int SetRunNumbers(const std::string&); // run list file given
    int SetRunNumbers(const std::vector<unsigned int>&); // explicit run list specified using [ ]
    unsigned int GetPresentRun() const;

    int DetermineRunNumberDigits(unsigned int, bool) const;
    int CheckRunNumbersInRange() const;

    int ParseXmlStartupFile();
    int ReadMsrFile(const std::string&) const;
    int ReadRunDataFile();

    bool PrepareNewInputFile(unsigned int, bool) const; // template
    bool PrepareGlobalInputFile(unsigned int, const std::string&, unsigned int) const; // generate msr-input file for a global fit

    int WriteOutput(const std::string&, const std::vector<unsigned int>&, bool, unsigned int, bool global = false, unsigned int counter = 0) const;

  private:
    bool PrepareNewSortedInputFile(unsigned int) const; // template
    PMsrHandler* GetSingleRunMsrFile() const;

    void WriteValue(std::fstream &outFile, const double &value, const unsigned int &width) const;
    void WriteValue(std::fstream &outFile, const double &value, const double &errValue, const unsigned int &width, const bool &db) const;
    int GetFirstSignificantDigit(const double &value) const;
    bool InParameterList(const unsigned int &paramValue, const std::vector<unsigned int>&) const;

    std::string fFileExtension;
    std::vector<unsigned int> fRunVector;
    mutable std::vector<unsigned int>::const_iterator fRunVectorIter;
    bool fRunListFile;
    std::vector<std::string> fIndVar;
    std::ifstream *fRunListFileStream;
    TSAXParser *fSaxParser;
    PStartupHandler *fStartupHandler;
    mutable PRunDataHandler *fDataHandler;
    mutable PMsrHandler *fMsrHandler;
    mutable unsigned int fNumGlobalParam;
    mutable unsigned int fNumSpecParam;
    mutable unsigned int fNumTempRunBlocks;
    mutable unsigned int fRunNumberDigits;
    mutable bool fHeaderWritten;
};


#endif // _PMSR2DATA_H_
