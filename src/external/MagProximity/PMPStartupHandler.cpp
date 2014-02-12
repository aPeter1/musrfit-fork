/***************************************************************************

  PMPStartupHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2011 by Andreas Suter                                   *
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

#include <cmath>

#include <iostream>
#include <fstream>
using namespace std;

#include "PMPStartupHandler.h"

ClassImpQ(PMPStartupHandler)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PMPStartupHandler::PMPStartupHandler()
{
  fIsValid = true;

  fStartupFileFound = false;
  fStartupFilePath = "";

  fTrimSpDataPath = TString("");

  // get default path (for the moment only linux like)
  char startup_path_name[128];

  // check if the startup file is found in the current directory
  strcpy(startup_path_name, "./mag_proximity_startup.xml");
  if (StartupFileExists(startup_path_name)) {
    fStartupFileFound = true;
    fStartupFilePath = TString(startup_path_name);
  } else { // startup file is not found in the current directory
    cout << endl << "PMPStartupHandler(): **WARNING** Couldn't find mag_proximity_startup.xml in the current directory, will try default one." << endl;
    strncpy(startup_path_name, "/home/nemu/analysis/musrfit/src/external/MagProximity/mag_proximity_startup.xml", sizeof(startup_path_name));
    if (StartupFileExists(startup_path_name)) {
      fStartupFileFound = true;
      fStartupFilePath = TString(startup_path_name);
    }
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PMPStartupHandler::~PMPStartupHandler()
{
  fTrimSpDataPathList.clear();
  fTrimSpDataEnergyList.clear();
}

//--------------------------------------------------------------------------
// OnStartDocument
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PMPStartupHandler::OnStartDocument()
{
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnEndDocument
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PMPStartupHandler::OnEndDocument()
{
  // nothing to be done for now
}

//--------------------------------------------------------------------------
// OnStartElement
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 * \param attributes
 */
void PMPStartupHandler::OnStartElement(const char *str, const TList *attributes)
{
  if (!strcmp(str, "data_path")) {
    fKey = eDataPath;
  } else if (!strcmp(str, "energy")) {
    fKey = eEnergy;
  }
}

//--------------------------------------------------------------------------
// OnEndElement
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 */
void PMPStartupHandler::OnEndElement(const char *str)
{
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnCharacters
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 */
void PMPStartupHandler::OnCharacters(const char *str)
{
  TString tstr;

  switch (fKey) {
    case eDataPath:
      fTrimSpDataPath = str;
      break;
    case eEnergy:
      tstr = str;
      if (tstr.IsFloat()) {
        fTrimSpDataEnergyList.push_back(tstr.Atof());
        tstr = fTrimSpDataPath;
        tstr += str;
        tstr += ".rge";
        fTrimSpDataPathList.push_back(tstr);
      } else {
        cout << endl << "PMPStartupHandler::OnCharacters: **ERROR** when finding energy:";
        cout << endl << "\"" << str << "\" is not a floating point number, will ignore it and use the default value.";
        cout << endl;
      }
      break;
    default:
      break;
  }
}

//--------------------------------------------------------------------------
// OnComment
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 */
void PMPStartupHandler::OnComment(const char *str)
{
  // nothing to be done for now
}

//--------------------------------------------------------------------------
// OnWarning
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 */
void PMPStartupHandler::OnWarning(const char *str)
{
  cout << endl << "PMPStartupHandler **WARNING** " << str;
  cout << endl;
}

//--------------------------------------------------------------------------
// OnError
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 */
void PMPStartupHandler::OnError(const char *str)
{
  cout << endl << "PMPStartupHandler **ERROR** " << str;
  cout << endl;
}

//--------------------------------------------------------------------------
// OnFatalError
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 */
void PMPStartupHandler::OnFatalError(const char *str)
{
  cout << endl << "PMPStartupHandler **FATAL ERROR** " << str;
  cout << endl;
}

//--------------------------------------------------------------------------
// OnCdataBlock
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 */
void PMPStartupHandler::OnCdataBlock(const char *str, Int_t len)
{
  // nothing to be done for now
}

//--------------------------------------------------------------------------
// StartupFileExists
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PMPStartupHandler::StartupFileExists(char *fln)
{
  bool result = false;

  ifstream ifile(fln);

  if (ifile.fail()) {
    result = false;
  } else {
    result = true;
    ifile.close();
  }

  return result;
}

// -------------------------------------------------------------------------
// end
// -------------------------------------------------------------------------
