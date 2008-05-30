 /***************************************************************************

  TFitPofBStartupHandler.h

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/30

  based upon:
  $Id: PStartupHandler.cpp 3340 2008-04-30 12:27:01Z nemu $
  by Andreas Suter

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter, Bastian M. Wojek                 *
 *                                                                         *
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

#include "TFitPofBStartupHandler.h"

ClassImpQ(TFitPofBStartupHandler)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>
 */
TFitPofBStartupHandler::TFitPofBStartupHandler()
{
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>
 */
TFitPofBStartupHandler::~TFitPofBStartupHandler()
{
  // clean up
  fEnergyList.clear();
}

//--------------------------------------------------------------------------
// OnStartDocument
//--------------------------------------------------------------------------
/**
 * <p>
 */
void TFitPofBStartupHandler::OnStartDocument()
{
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnEndDocument
//--------------------------------------------------------------------------
/**
 * <p>
 */
void TFitPofBStartupHandler::OnEndDocument()
{
  // check if anything was set, and if not set some default stuff
  CheckLists();
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
void TFitPofBStartupHandler::OnStartElement(const char *str, const TList *attributes)
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
void TFitPofBStartupHandler::OnEndElement(const char *str)
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
void TFitPofBStartupHandler::OnCharacters(const char *str)
{
  switch (fKey) {
    case eDataPath:
      // set the data path to the given path
      fDataPath = str;
      break;
    case eEnergy:
      // add str to the energy list
      fEnergyList.push_back(str);
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
void TFitPofBStartupHandler::OnComment(const char *str)
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
void TFitPofBStartupHandler::OnWarning(const char *str)
{
  cout << endl << "TFitPofBStartupHandler **WARNING** " << str;
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
void TFitPofBStartupHandler::OnError(const char *str)
{
  cout << endl << "TFitPofBStartupHandler **ERROR** " << str;
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
void TFitPofBStartupHandler::OnFatalError(const char *str)
{
  cout << endl << "TFitPofBStartupHandler **FATAL ERROR** " << str;
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
void TFitPofBStartupHandler::OnCdataBlock(const char *str, int len)
{
  // nothing to be done for now
}

//--------------------------------------------------------------------------
// CheckList
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void TFitPofBStartupHandler::CheckLists()
{
  // check if anything was set, and if not set some default stuff

  // check if any data path is given
  cout << endl << ">> check data path ...";
  if (!fDataPath.size()) {
    cout << endl << ">> This is not going to work, you have to set a valid data path where to find the rge-files in the xml-file!" << endl;
    exit(-1);
  }

  // check if any energies are given
  cout << endl << ">> check energy list ..." << endl;
  if (!fEnergyList.size()) {
  cout << endl << ">> Energy list empty! Setting the default list.";
    char eChar[5];
    for(unsigned int i(0); i<33; i++) {
      for(unsigned int j(0); j<10; j++) {
        sprintf(eChar, "%02u_%u", i, j);
        fEnergyList.push_back(string(eChar));
      }
    }
  }
}

// end ---------------------------------------------------------------------

