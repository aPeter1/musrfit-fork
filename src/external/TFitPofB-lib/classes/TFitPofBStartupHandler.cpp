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
  if (!strcmp(str, "debug")) {
    fKey = eDebug;
  } else if (!strcmp(str, "LEM")) {
    fKey = eLEM;
  } else if (!strcmp(str, "VortexLattice")) {
    fKey = eVortex;
  } else if (!strcmp(str, "data_path")) {
    fKey = eDataPath;
  } else if (!strcmp(str, "energy_label")) {
    fKey = eEnergyLabel;
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
    case eDebug:
      if (!strcmp(str, "1"))
        fDebug = true;
      else
        fDebug = false;
      break;
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
    case eEnergyLabel:
      // add str to the energy label list
      fEnergyLabelList.push_back(str);
      break;
    case eEnergy:
      // add str to the energy list
      fEnergyList.push_back(atof(str));
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
  if(fDebug)
    cout << endl << "TFitPofBStartupHandler::CheckLists: check specified time resolution ... " << endl;
  if(!fDeltat) {
    cout << "TFitPofBStartupHandler::CheckLists: You did not specify the time resolution. Setting the default (10 ns)." << endl;
    fDeltat = 0.01;
  } else {
    if(fDebug)
      cout << fDeltat << " us" << endl;
  }

  // check if delta_B is given, if not set default
  if(fDebug)
    cout << endl << "TFitPofBStartupHandler::CheckLists: check specified field resolution ..." << endl;
  if(!fDeltaB) {
    cout << "TFitPofBStartupHandler::CheckLists: You did not specify the field resolution. Setting the default (0.1 G)." << endl;
    fDeltaB = 0.1;
  } else {
    if(fDebug)
      cout << fDeltaB << " G" << endl;
  }

  // check if any wisdom-file is specified
  if(fDebug)
    cout << endl << "TFitPofBStartupHandler::CheckLists: check wisdom-file ..." << endl;
  if (!fWisdomFile.size()) {
    cout << "TFitPofBStartupHandler::CheckLists: You did not specify a wisdom file. No FFTW plans will be loaded or saved." << endl;
    fWisdomFile = "";
  } else {
    if(fDebug)
      cout << fWisdomFile << endl;
  }


  if (fLEM) {

    // check if any data path is given
    if(fDebug)
      cout << endl << "TFitPofBStartupHandler::CheckLists: check data path ..." << endl;
    if (!fDataPath.size()) {
      cout << "TFitPofBStartupHandler::CheckLists: This is not going to work, you have to set a valid data path where to find the rge-files in the xml-file!" << endl;
      exit(-1);
    } else {
      if(fDebug)
        cout << fDataPath << endl;
    }

    // check if any energies are given
    if(fDebug)
      cout << endl << "TFitPofBStartupHandler::CheckLists: check energy list ..." << endl;
    if (fEnergyList.size() != fEnergyLabelList.size()) {
      cout << "TFitPofBStartupHandler::CheckLists: The number of energies and energy labels are different! Please fix it!" << endl;
    }
    if (fEnergyList.empty()) {
      cout << "TFitPofBStartupHandler::CheckLists: Energy list empty!" << endl \
           << "TFitPofBStartupHandler::CheckLists: Trying to use the standard energies: 0.0 to 35.0 keV in 0.1 keV steps" << endl;
      for (double x(0.0); x<= 35.0; x+=0.1) {
        fEnergyList.push_back(x);
      }
    }
    if (fEnergyLabelList.empty()) {
      cout << "TFitPofBStartupHandler::CheckLists: Energy label list empty!" << endl \
           << "TFitPofBStartupHandler::CheckLists: Trying to use the specified energies as labels in the format %02.1f..." << endl \
           << "TFitPofBStartupHandler::CheckLists: Most probably this will go wrong and should therefore be fixed in the xml-file!" << endl;
      char eChar[5];
      for(unsigned int i(0); i<fEnergyList.size(); i++) {
        sprintf(eChar, "%02.1f", fEnergyList[i]);
        fEnergyLabelList.push_back(string(eChar));
      }
    }
    if(fDebug) {
      cout << "Energies: ";
      for (unsigned int i (0); i < fEnergyList.size(); i++)
        cout << fEnergyList[i] << " ";
      cout << endl;
      cout << "Energy Labels: ";
      for (unsigned int i (0); i < fEnergyLabelList.size(); i++)
        cout << fEnergyLabelList[i] << " ";
      cout << endl;
    }

    // check if any number of steps for the theory function is specified
    if(fDebug)
      cout << endl << "TFitPofBStartupHandler::CheckLists: check number of steps for theory ..." << endl;
    if (!fNSteps) {
      cout << "TFitPofBStartupHandler::CheckLists: You did not specify the number of steps for the theory. Setting the default (3000)." << endl;
      fNSteps = 3000;
    } else {
      if(fDebug)
        cout << fNSteps << endl;
    }
  }

  if (fVortex) {

    // check if any number of steps for the theory function is specified
    if(fDebug)
      cout << endl << "TFitPofBStartupHandler::CheckLists: check number of steps for Vortex grid ..." << endl;
    if (!fGridSteps) {
      cout << "TFitPofBStartupHandler::CheckLists: You did not specify the number of steps for the grid. Setting the default (256)." << endl;
      fGridSteps = 256;
    } else {
      if(fDebug)
        cout << fGridSteps << endl;
    }
  }
}

// end ---------------------------------------------------------------------

