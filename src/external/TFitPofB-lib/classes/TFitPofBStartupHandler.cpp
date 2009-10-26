 /***************************************************************************

  TFitPofBStartupHandler.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/06/03

  based upon:
  $Id: PStartupHandler.cpp 3340 2008-04-30 12:27:01Z nemu $
  by Andreas Suter

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2008 by Andreas Suter, Bastian M. Wojek            *
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

#include <cstdlib>
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
TFitPofBStartupHandler::TFitPofBStartupHandler() : fDeltat(0.), fDeltaB(0.), fNSteps(0)
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
  if (!strcmp(str, "LEM")) {
    fKey = eLEM;
  } else if (!strcmp(str, "VortexLattice")) {
    fKey = eVortex;
  } else if (!strcmp(str, "data_path")) {
    fKey = eDataPath;
  } else if (!strcmp(str, "energy")) {
    fKey = eEnergy;
  } else if (!strcmp(str, "delta_t")) {
    fKey = eDeltat;
  } else if (!strcmp(str, "delta_B")) {
    fKey = eDeltaB;
  } else if (!strcmp(str, "wisdom")) {
    fKey = eWisdomFile;
  } else if (!strcmp(str, "N_theory")) {
    fKey = eNSteps;
  } else if (!strcmp(str, "N_VortexGrid")) {
    fKey = eGridSteps;
  } else if (!strcmp(str, "VortexSymmetry")) {
    fKey = eVortexSymmetry;
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
    case eLEM:
      fLEM = true;
      break;
    case eVortex:
      fVortex = true;
      break;
    case eDataPath:
      // set the data path to the given path
      fDataPath = str;
      break;
    case eEnergy:
      // add str to the energy list
      fEnergyList.push_back(str);
      break;
    case eDeltat:
      // convert str to double and assign it to the deltat-member
      fDeltat = atof(str);
      break;
    case eDeltaB:
      // convert str to double and assign it to the deltaB-member
      fDeltaB = atof(str);
      break;
    case eWisdomFile:
      // set the wisdom file to the given name
      fWisdomFile = str;
      break;
    case eNSteps:
      // convert str to int and assign it to the NSteps-member
      fNSteps = atoi(str);
      break;
    case eGridSteps:
      // convert str to int and assign it to the GridSteps-member
      fGridSteps = atoi(str);
      break;
    case eVortexSymmetry:
      // convert str to int and assign it to the VortexSymmetry-member (square = 0, triangular = 1)
      if (!strcmp(str, "square"))
        fVortexSymmetry = 2;
      else
        fVortexSymmetry = 1;
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
  cout << endl << "TFitPofBStartupHandler::OnWarning: TFitPofBStartupHandler **WARNING** " << str;
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
  cout << endl << "TFitPofBStartupHandler::OnError: TFitPofBStartupHandler **ERROR** " << str;
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
  cout << endl << "TFitPofBStartupHandler::OnFatalError: TFitPofBStartupHandler **FATAL ERROR** " << str;
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

  // check if delta_t is given, if not set default
  cout << endl << "TFitPofBStartupHandler::CheckLists: check specified time resolution ... " << endl;
  if(!fDeltat) {
    cout << "TFitPofBStartupHandler::CheckLists: You did not specify the time resolution. Setting the default (10 ns)." << endl;
    fDeltat = 0.01;
  } else {
    cout << fDeltat << " us" << endl;
  }

  // check if delta_B is given, if not set default
  cout << endl << "TFitPofBStartupHandler::CheckLists: check specified field resolution ..." << endl;
  if(!fDeltaB) {
    cout << "TFitPofBStartupHandler::CheckLists: You did not specify the field resolution. Setting the default (0.1 G)." << endl;
    fDeltaB = 0.1;
  } else {
    cout << fDeltaB << " G" << endl;
  }

  // check if any wisdom-file is specified
  cout << endl << "TFitPofBStartupHandler::CheckLists: check wisdom-file ..." << endl;
  if (!fWisdomFile.size()) {
    cout << "TFitPofBStartupHandler::CheckLists: You did not specify a wisdom file. No FFTW plans will be loaded or saved." << endl;
    fWisdomFile = "";
  } else {
    cout << fWisdomFile << endl;
  }


  if (fLEM) {

    // check if any data path is given
    cout << endl << "TFitPofBStartupHandler::CheckLists: check data path ..." << endl;
    if (!fDataPath.size()) {
      cout << "TFitPofBStartupHandler::CheckLists: This is not going to work, you have to set a valid data path where to find the rge-files in the xml-file!" << endl;
      exit(-1);
    } else {
      cout << fDataPath << endl;
    }

    // check if any energies are given
    cout << endl << "TFitPofBStartupHandler::CheckLists: check energy list ..." << endl;
    if (!fEnergyList.size()) {
      cout << "TFitPofBStartupHandler::CheckLists: Energy list empty! Setting the default list ( 0.0:0.1:32.9 keV)." << endl;
      char eChar[5];
      for(unsigned int i(0); i<33; i++) {
        for(unsigned int j(0); j<10; j++) {
          sprintf(eChar, "%02u_%u", i, j);
          fEnergyList.push_back(string(eChar));
        }
      }
    } else {
      for (unsigned int i (0); i < fEnergyList.size(); i++)
        cout << fEnergyList[i] << " ";
      cout << endl;
    }

    // check if any number of steps for the theory function is specified
    cout << endl << "TFitPofBStartupHandler::CheckLists: check number of steps for theory ..." << endl;
    if (!fNSteps) {
      cout << "TFitPofBStartupHandler::CheckLists: You did not specify the number of steps for the theory. Setting the default (3000)." << endl;
      fNSteps = 3000;
    } else {
      cout << fNSteps << endl;
    }
  }

  if (fVortex) {

    // check if any number of steps for the theory function is specified
    cout << endl << "TFitPofBStartupHandler::CheckLists: check number of steps for Vortex grid ..." << endl;
    if (!fGridSteps) {
      cout << "TFitPofBStartupHandler::CheckLists: You did not specify the number of steps for the grid. Setting the default (256)." << endl;
      fGridSteps = 256;
    } else {
      cout << fGridSteps << endl;
    }

    // check if any number of steps for the theory function is specified
    cout << endl << "TFitPofBStartupHandler::CheckLists: check symmetry of the Vortex lattice ..." << endl;
    if (!fVortexSymmetry) {
      cout << "TFitPofBStartupHandler::CheckLists: You did not specify the symmetry of the vortex lattice. Setting the default (triangular)." << endl;
      fVortexSymmetry = 1;
    } else {
      cout << (fVortexSymmetry == 2 ? "square" : "triangular") << endl;
    }
  }
}

// end ---------------------------------------------------------------------

