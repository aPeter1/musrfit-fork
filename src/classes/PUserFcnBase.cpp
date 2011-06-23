/***************************************************************************

  PUserFcnBase.cpp

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

#include <vector>
#include <fstream>
using namespace std;

#include "PUserFcnBase.h"

ClassImp(PUserFcnBase)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 */
PUserFcnBase::PUserFcnBase()
{
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor
 */
PUserFcnBase::~PUserFcnBase()
{
}

//--------------------------------------------------------------------------
// This function is a replacement for the ParseFile method of TSAXParser.
// It is needed because in certain environments ParseFile does not work but ParseBuffer does.
//--------------------------------------------------------------------------
/**
 * <p> Replacement for the ParseFile method of TSAXParser
 *     that can be used in user functions.
 *
 * <p><b>return:</b>
 * - 1 if file cannot be read
 * - 0 if the file has been parsed successfully
 * - parse error code otherwise
 *
 * \param saxParser pointer to a TSAXParser object
 * \param startup_path_name full path to the XML file to be read
 */
Int_t parseXmlFile(TSAXParser *saxParser, const char *startup_path_name)
{
  Int_t status;
  fstream xmlFile;
  UInt_t xmlSize = 0;
  Char_t *xmlBuffer = 0;

  xmlFile.open(startup_path_name, ios::in | ios::ate); // open file for reading and go to the end of the file
  if (xmlFile.is_open()) { // check if file has been opened successfully
    xmlSize = xmlFile.tellg(); // get the position within the stream == size of the file (since we are at the end)
    xmlFile.seekg(0, ios::beg); // go back to the beginning of the stream
    xmlBuffer = new Char_t[xmlSize]; // allocate buffer memory for the whole XML file
    xmlFile.read(xmlBuffer, xmlSize); // read in the whole XML file into the buffer
    xmlFile.close(); // close the XML file
  }
  if (!xmlBuffer) { // file has not been read into the buffer
    status = 1;
  } else {
    status = saxParser->ParseBuffer(xmlBuffer, xmlSize); // parse buffer
    delete[] xmlBuffer; // free the buffer memory
    xmlBuffer = 0;
  }

  return status;
}

// place a void pointer vector for global user function objects which might be needed
vector<void *> gGlobalUserFcn;
