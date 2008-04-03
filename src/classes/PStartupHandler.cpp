/***************************************************************************

  PStartupHandler.cpp

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

#include <iostream>
using namespace std;

#include <TObjArray.h>
#include <TObjString.h>
#include <TColor.h>

#include "PStartupHandler.h"

ClassImpQ(PStartupHandler)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 */
PStartupHandler::PStartupHandler()
{
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 */
PStartupHandler::~PStartupHandler()
{
  // clean up
  fDataPathList.empty();
  fMarkerList.empty();
  fColorList.empty();
}

//--------------------------------------------------------------------------
// OnStartDocument
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PStartupHandler::OnStartDocument()
{
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnEndDocument
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PStartupHandler::OnEndDocument()
{
  // check if anything was set, and if not set some default stuff

  // check if any data path is given
  if (fDataPathList.size() == 0) {
    fDataPathList.push_back(TString("/mnt/data/nemu/his"));
    fDataPathList.push_back(TString("/mnt/data/nemu/wkm"));
  }

  // check if any markers are given
  if (fMarkerList.size() == 0) {
    // to be done yet
  }

  // check if any colors are given
  if (fColorList.size() == 0) {
    // to be done yet
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
void PStartupHandler::OnStartElement(const char *str, const TList *attributes)
{
  if (!strcmp(str, "data_path")) {
    fKey = eDataPath;
  } else if (!strcmp(str, "marker")) {
    fKey = eMarker;
  } else if (!strcmp(str, "color")) {
    fKey = eColor;
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
void PStartupHandler::OnEndElement(const char *str)
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
void PStartupHandler::OnCharacters(const char *str)
{
  TObjArray  *tokens;
  TObjString *ostr;
  TString    tstr;
  Int_t      color, r, g, b;

  switch (fKey) {
    case eDataPath:
      // check that str is a valid path
      // add str to the path list
      break;
    case eMarker:
      // check that str is a number
      tstr = TString(str);
      if (tstr.IsDigit()) {
        // add converted str to the marker list
        fMarkerList.push_back(tstr.Atoi());
      } else {
        cout << endl << "PStartupHandler **WARNING** '" << str << "' is not a number, will ignore it";
        cout << endl;
      }
      break;
    case eColor:
      // check that str is a rbg code
      tstr = TString(str);
      tokens = tstr.Tokenize(",");
      // check that there any tokens
      if (!tokens) {
        cout << endl << "PStartupHandler **WARNING** '" << str << "' is not a rbg code, will ignore it";
        cout << endl;
        return;
      }
      // check there is the right number of tokens
      if (tokens->GetEntries() != 3) {
        cout << endl << "PStartupHandler **WARNING** '" << str << "' is not a rbg code, will ignore it";
        cout << endl;
        return;
      }
      // get r
      ostr = dynamic_cast<TObjString*>(tokens->At(0));
      tstr = ostr->GetString();
      if (tstr.IsDigit()) {
        r = tstr.Atoi();
      } else {
        cout << endl << "PStartupHandler **WARNING** r within the rgb code is not a number, will ignore it";
        cout << endl;
        return;
      }
      // get g
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      tstr = ostr->GetString();
      if (tstr.IsDigit()) {
        g = tstr.Atoi();
      } else {
        cout << endl << "PStartupHandler **WARNING** g within the rgb code is not a number, will ignore it";
        cout << endl;
        return;
      }
      // get b
      ostr = dynamic_cast<TObjString*>(tokens->At(2));
      tstr = ostr->GetString();
      if (tstr.IsDigit()) {
        b = tstr.Atoi();
      } else {
        cout << endl << "PStartupHandler **WARNING** b within the rgb code is not a number, will ignore it";
        cout << endl;
        return;
      }
      // clean up tokens
      if (tokens) {
        delete tokens;
        tokens = 0;
      }
      // generate the ROOT color code based on str
      color = TColor::GetColor(r,g,b);
      // add the color code to the color list
      fColorList.push_back(color);
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
void PStartupHandler::OnComment(const char *str)
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
void PStartupHandler::OnWarning(const char *str)
{
  cout << endl << "PStartupHandler **WARNING** " << str;
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
void PStartupHandler::OnError(const char *str)
{
  cout << endl << "PStartupHandler **ERROR** " << str;
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
void PStartupHandler::OnFatalError(const char *str)
{
  cout << endl << "PStartupHandler **FATAL ERROR** " << str;
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
void PStartupHandler::OnCdataBlock(const char *str, Int_t len)
{
  // nothing to be done for now
}

// end ---------------------------------------------------------------------

