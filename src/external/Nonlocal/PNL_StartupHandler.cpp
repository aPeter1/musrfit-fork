/***************************************************************************

  PNL_StartupHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2021 by Andreas Suter                              *
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

#include <boost/filesystem.hpp>

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
  // get default path (for the moment only linux like)
  char startup_path_name[512];
  char *home_str=0;

  // check if the startup file is found in the current directory
  strcpy(startup_path_name, "./nonlocal_startup.xml");
  if (boost::filesystem::exists(startup_path_name)) {
    fStartupFileFound = true;
    fStartupFilePath = TString(startup_path_name);
  } else { // startup file is not found in the current directory
    std::cout << std::endl << "PNL_StartupHandler(): **WARNING** Couldn't find nonlocal_startup.xml in the current directory, will try default one." << std::endl;
    home_str = getenv("HOME");
    snprintf(startup_path_name, sizeof(startup_path_name), "%s/.musrfit/nonlocal_startup.xml", home_str);
    if (boost::filesystem::exists(startup_path_name)) {
      fStartupFileFound = true;
      fStartupFilePath = TString(startup_path_name);
    }
  }
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
    std::cout << std::endl << "PNL_StartupHandler::OnEndDocument: **WARNING** \"fourier_points\" not defined in nonlocal_startup.xml.";
    std::cout << std::endl << "  will set it to " << fFourierPoints << "." << std::endl;
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
        std::cout << std::endl << "PNL_StartupHandler::OnCharacters: **ERROR** when finding fourier_points:";
        std::cout << std::endl << "\"" << str << "\" is not a number, will ignore it and use the default value.";
        std::cout << std::endl;
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
  std::cout << std::endl << "PNL_StartupHandler **WARNING** " << str;
  std::cout << std::endl;
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
  std::cout << std::endl << "PNL_StartupHandler **ERROR** " << str;
  std::cout << std::endl;
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
  std::cout << std::endl << "PNL_StartupHandler **FATAL ERROR** " << str;
  std::cout << std::endl;
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

// -------------------------------------------------------------------------
// end
// -------------------------------------------------------------------------
