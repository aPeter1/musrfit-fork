/***************************************************************************

  PStartupHandler_SV.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013 by Andreas Suter                                   *
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

#include "PStartupHandler_SV.h"

ClassImpQ(PStartupHandler_SV)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PStartupHandler_SV::PStartupHandler_SV()
{
  fIsValid = true;

  fStartupFileFound = false;
  fStartupFilePath = "";

  // get default path (for the moment only linux like)
  char startup_path_name[128];

  // check if the startup file is found in the current directory
  strcpy(startup_path_name, "./spinValve_startup.xml");
  if (StartupFileExists(startup_path_name)) {
    fStartupFileFound = true;
    fStartupFilePath = TString(startup_path_name);
  } else { // startup file is not found in the current directory
    cout << endl << ">> PStartupHandler_SV(): **WARNING** Couldn't find skewedLorentzian_startup.xml in the current directory, will try default one." << endl;
    strncpy(startup_path_name, "/home/nemu/analysis/musrfit/src/external/libSpinValve/test/spinValve_startup.xml", sizeof(startup_path_name));
    if (StartupFileExists(startup_path_name)) {
      fStartupFileFound = true;
      fStartupFilePath = TString(startup_path_name);
    }
  }

  // init rest
  fNoOfFields = 0;
  fRange = 0.0;
}

//--------------------------------------------------------------------------
// OnStartDocument
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PStartupHandler_SV::OnStartDocument()
{
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnEndDocument
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PStartupHandler_SV::OnEndDocument()
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
void PStartupHandler_SV::OnStartElement(const char *str, const TList *attributes)
{
  if (!strcmp(str, "number_of_fields")) {
    fKey = eNoOfFields;
  } else if (!strcmp(str, "range")) {
    fKey = eRange;
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
void PStartupHandler_SV::OnEndElement(const char *str)
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
void PStartupHandler_SV::OnCharacters(const char *str)
{
  TString tstr;

  switch (fKey) {
    case eNoOfFields:
      tstr = str;
      if (tstr.IsDigit()) {
      fNoOfFields = tstr.Atoi();
      } else {
        cout << endl << "PStartupHandler_SV::OnCharacters: **ERROR** when finding number_of_fields:";
        cout << endl << "\"" << str << "\" is not a number, will ignore it and use the default value.";
        cout << endl;
      }
      break;
    case eRange:
      tstr = str;
      if (tstr.IsFloat()) {
        fRange = tstr.Atof();
      } else {
        cout << endl << "PStartupHandler_SV::OnCharacters: **ERROR** when finding range:";
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
void PStartupHandler_SV::OnComment(const char *str)
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
void PStartupHandler_SV::OnWarning(const char *str)
{
  cout << endl << "PStartupHandler_SV **WARNING** " << str;
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
void PStartupHandler_SV::OnError(const char *str)
{
  cout << endl << "PStartupHandler_SV **ERROR** " << str;
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
void PStartupHandler_SV::OnFatalError(const char *str)
{
  cout << endl << "PStartupHandler_SV **FATAL ERROR** " << str;
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
void PStartupHandler_SV::OnCdataBlock(const char *str, Int_t len)
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
bool PStartupHandler_SV::StartupFileExists(char *fln)
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
