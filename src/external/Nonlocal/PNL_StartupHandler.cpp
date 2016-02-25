/***************************************************************************

  PNL_StartupHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Andreas Suter                                   *
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

#include "PNL_StartupHandler.h"

ClassImpQ(PNL_StartupHandler)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PNL_StartupHandler::PNL_StartupHandler()
{
  fIsValid = true;

  fStartupFileFound = false;
  fStartupFilePath = "";

  fFourierPoints = 0;
  fTrimSpDataPath = TString("");

  // get default path (for the moment only linux like)
  char startup_path_name[512];
  char *home_str;

  // check if the startup file is found in the current directory
  strcpy(startup_path_name, "./nonlocal_startup.xml");
  if (StartupFileExists(startup_path_name)) {
    fStartupFileFound = true;
    fStartupFilePath = TString(startup_path_name);
  } else { // startup file is not found in the current directory
    cout << endl << "PNL_StartupHandler(): **WARNING** Couldn't find nonlocal_startup.xml in the current directory, will try default one." << endl;
    home_str = getenv("$HOME");
    snprintf(startup_path_name, sizeof(startup_path_name), "%s/.musrfit/nonlocal_startup.xml", home_str);
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
PNL_StartupHandler::~PNL_StartupHandler()
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
void PNL_StartupHandler::OnStartDocument()
{
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnEndDocument
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PNL_StartupHandler::OnEndDocument()
{
  // check if anything was set, and if not set some default stuff
  if (fFourierPoints == 0) {
    fFourierPoints = 262144;
    cout << endl << "PNL_StartupHandler::OnEndDocument: **WARNING** \"fourier_points\" not defined in nonlocal_startup.xml.";
    cout << endl << "  will set it to " << fFourierPoints << "." << endl;
  }
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
void PNL_StartupHandler::OnStartElement(const char *str, const TList *attributes)
{
  if (!strcmp(str, "fourier_points")) {
    fKey = eFourierPoints;
  } else if (!strcmp(str, "data_path")) {
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
void PNL_StartupHandler::OnEndElement(const char *str)
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
void PNL_StartupHandler::OnCharacters(const char *str)
{
  TString tstr;

  switch (fKey) {
    case eFourierPoints:
      tstr = str;
      if (tstr.IsDigit()) {
        fFourierPoints = tstr.Atoi();
      } else {
        cout << endl << "PNL_StartupHandler::OnCharacters: **ERROR** when finding fourier_points:";
        cout << endl << "\"" << str << "\" is not a number, will ignore it and use the default value.";
        cout << endl;
      }
      break;
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
        cout << endl << "PNL_StartupHandler::OnCharacters: **ERROR** when finding energy:";
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
void PNL_StartupHandler::OnComment(const char *str)
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
void PNL_StartupHandler::OnWarning(const char *str)
{
  cout << endl << "PNL_StartupHandler **WARNING** " << str;
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
void PNL_StartupHandler::OnError(const char *str)
{
  cout << endl << "PNL_StartupHandler **ERROR** " << str;
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
void PNL_StartupHandler::OnFatalError(const char *str)
{
  cout << endl << "PNL_StartupHandler **FATAL ERROR** " << str;
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
void PNL_StartupHandler::OnCdataBlock(const char *str, Int_t len)
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
bool PNL_StartupHandler::StartupFileExists(char *fln)
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
