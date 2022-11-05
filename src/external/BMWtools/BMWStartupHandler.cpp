 /***************************************************************************

  BMWStartupHandler.cpp

  Author: Bastian M. Wojek

  based upon:
  PStartupHandler.cpp
  by Andreas Suter

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2021 by Andreas Suter, Bastian M. Wojek            *
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

#include <cassert>

#include "BMWStartupHandler.h"

ClassImpQ(BMWStartupHandler)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor. Check if the BMW_startup.xml file is found in the local directory
 */
BMWStartupHandler::BMWStartupHandler() :
 fDebug(false), fLEM(false), fVortex(false), fLF(false), fDataPath(""), fDeltat(0.), fDeltaB(0.), fWisdomFile(""), fWisdomFileFloat(""), fNSteps(0), fGridSteps(0), fDeltatLF(0.), fNStepsLF(0)
{
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor
 */
BMWStartupHandler::~BMWStartupHandler()
{
  // clean up
  fEnergyList.clear();
  fEnergyLabelList.clear();
  fEnergies.clear();
}

//--------------------------------------------------------------------------
// OnStartDocument
//--------------------------------------------------------------------------
/**
 * <p>Called on start of the XML file reading. Initializes all necessary variables.
 */
void BMWStartupHandler::OnStartDocument()
{
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnEndDocument
//--------------------------------------------------------------------------
/**
 * <p>Called on end of XML file reading.
 */
void BMWStartupHandler::OnEndDocument()
{
  // check if anything was set, and if not set some default stuff
  CheckLists();
}

//--------------------------------------------------------------------------
// OnStartElement
//--------------------------------------------------------------------------
/**
 * <p>Called when a XML start element is found. Filters out the needed elements
 * and sets a proper key.
 *
 * \param str XML element name
 * \param attributes not used
 */
void BMWStartupHandler::OnStartElement(const char *str, const TList *attributes)
{
  if (!strcmp(str, "debug")) {
    fKey = eDebug;
  } else if (!strcmp(str, "LEM")) {
    fKey = eLEM;
  } else if (!strcmp(str, "VortexLattice")) {
    fKey = eVortex;
  } else if (!strcmp(str, "LFRelaxation")) {
    fKey = eLF;
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
  } else if (!strcmp(str, "wisdom_float")) {
    fKey = eWisdomFileFloat;
  } else if (!strcmp(str, "N_theory")) {
    fKey = eNSteps;
  } else if (!strcmp(str, "N_VortexGrid")) {
    fKey = eGridSteps;
  } else if (!strcmp(str, "delta_t_LF")) {
    fKey = eDeltatLF;
  } else if (!strcmp(str, "N_LF")) {
    fKey = eNStepsLF;
  }
}

//--------------------------------------------------------------------------
// OnEndElement
//--------------------------------------------------------------------------
/**
 * <p>Called when a XML end element is found. Resets the handler key.
 *
 * \param str not used
 */
void BMWStartupHandler::OnEndElement(const char *str)
{
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnCharacters
//--------------------------------------------------------------------------
/**
 * <p>Content of a given XML element. Filters out the data and feeds them to
 * the internal variables.
 *
 * \param str XML element string
 */
void BMWStartupHandler::OnCharacters(const char *str)
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
    case eLF:
      fLF = true;
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
    case eWisdomFileFloat:
      // set the float-wisdom file to the given name
      fWisdomFileFloat = str;
      break;
    case eNSteps:
      // convert str to int and assign it to the NSteps-member
      fNSteps = atoi(str);
      break;
    case eGridSteps:
      // convert str to int and assign it to the GridSteps-member
      fGridSteps = atoi(str);
      break;
    case eDeltatLF:
      // convert str to double and assign it to the deltatLF-member
      fDeltatLF = atof(str);
      break;
    case eNStepsLF:
      // convert str to int and assign it to the NStepsLF-member
      fNStepsLF = atoi(str);
      break;
    default:
      break;
  }
}

//--------------------------------------------------------------------------
// OnComment
//--------------------------------------------------------------------------
/**
 * <p>Called when a XML comment is found. Not used.
 *
 * \param str not used.
 */
void BMWStartupHandler::OnComment(const char *str)
{
  // nothing to be done for now
}

//--------------------------------------------------------------------------
// OnWarning
//--------------------------------------------------------------------------
/**
 * <p>Called when the XML parser emits a warning.
 *
 * \param str warning string
 */
void BMWStartupHandler::OnWarning(const char *str)
{
  std::cerr << std::endl << "BMWStartupHandler::OnWarning: BMWStartupHandler **WARNING** " << str;
  std::cerr << std::endl;
}

//--------------------------------------------------------------------------
// OnError
//--------------------------------------------------------------------------
/**
 * <p>Called when the XML parser emits an error.
 *
 * \param str error string
 */
void BMWStartupHandler::OnError(const char *str)
{
  std::cerr << std::endl << "BMWStartupHandler::OnError: BMWStartupHandler **ERROR** " << str;
  std::cerr << std::endl;
}

//--------------------------------------------------------------------------
// OnFatalError
//--------------------------------------------------------------------------
/**
 * <p>Called when the XML parser emits a fatal error.
 *
 * \param str fatal error string
 */
void BMWStartupHandler::OnFatalError(const char *str)
{
  std::cerr << std::endl << "BMWStartupHandler::OnFatalError: BMWStartupHandler **FATAL ERROR** " << str;
  std::cerr << std::endl;
}

//--------------------------------------------------------------------------
// OnCdataBlock
//--------------------------------------------------------------------------
/**
 * <p>Not used.
 *
 * \param str not used
 * \param len not used
 */
void BMWStartupHandler::OnCdataBlock(const char *str, int len)
{
  // nothing to be done for now
}

//--------------------------------------------------------------------------
// CheckList
//--------------------------------------------------------------------------
/**
 * <p>Check if the default lists are present and if not, feed them with some default settings
 *
 */
void BMWStartupHandler::CheckLists()
{
  // check if anything was set, and if not set some default stuff

  if(fLF) {
    // check if delta_t_LF is given, if not set default
    if(fDebug)
      std::cout << std::endl << "BMWStartupHandler::CheckLists: check specified LF time resolution for the Laplace transform ... " << std::endl;
    if(!fDeltatLF) {
      std::cout << "BMWStartupHandler::CheckLists: You did not specify the LF time resolution. Setting the default (0.04 ns)." << std::endl;
      fDeltatLF = 0.00004;
    } else {
      if(fDebug)
        std::cout << fDeltatLF << " us" << std::endl;
    }

    // check if N_LF is given, if not set default
    if(fDebug)
      std::cout << std::endl << "BMWStartupHandler::CheckLists: check length of the Laplace transform ..." << std::endl;
    if (!fNStepsLF) {
      std::cout << "BMWStartupHandler::CheckLists: You did not specify the length of the Laplace transform. Setting the default (524288)." << std::endl;
      fNStepsLF = 524288;
    } else {
      if(fDebug)
        std::cout << fNStepsLF << std::endl;
    }
  } else {
    // check if delta_t is given, if not set default
    if(fDebug)
      std::cout << std::endl << "BMWStartupHandler::CheckLists: check specified time resolution ... " << std::endl;
    if(!fDeltat) {
      std::cout << "BMWStartupHandler::CheckLists: You did not specify the time resolution. Setting the default (10 ns)." << std::endl;
      fDeltat = 0.01;
    } else {
      if(fDebug)
        std::cout << fDeltat << " us" << std::endl;
    }

    // check if delta_B is given, if not set default
    if(fDebug)
      std::cout << std::endl << "BMWStartupHandler::CheckLists: check specified field resolution ..." << std::endl;
    if(!fDeltaB) {
      std::cout << "BMWStartupHandler::CheckLists: You did not specify the field resolution. Setting the default (0.1 G)." << std::endl;
      fDeltaB = 0.1;
    } else {
      if(fDebug)
        std::cout << fDeltaB << " G" << std::endl;
    }
  }

  // check if any wisdom-file is specified
  if(fDebug)
    std::cout << std::endl << "BMWStartupHandler::CheckLists: check wisdom file ..." << std::endl;
  if (!fWisdomFile.size()) {
    std::cout << "BMWStartupHandler::CheckLists: You did not specify a wisdom file. No FFTW plans will be loaded or saved." << std::endl;
    fWisdomFile = "";
  } else {
    if(fDebug)
      std::cout << fWisdomFile << std::endl;
  }

  // check if any float-wisdom-file is specified
  if(fDebug)
    std::cout << std::endl << "BMWStartupHandler::CheckLists: check float-wisdom file ..." << std::endl;
  if (!fWisdomFile.size()) {
    std::cout << "BMWStartupHandler::CheckLists: You did not specify a float-wisdom file. No FFTW plans will be loaded or saved." << std::endl;
    fWisdomFileFloat = "";
  } else {
    if(fDebug)
      std::cout << fWisdomFileFloat << std::endl;
  }

  if (fLEM) {

    // check if any data path is given
    if(fDebug)
      std::cout << std::endl << "BMWStartupHandler::CheckLists: check data path ..." << std::endl;
    if (fDataPath.empty()) {
      std::cerr << "BMWStartupHandler::CheckLists: This is not going to work, you have to set a valid data path where to find the rge-files in the xml-file!" << std::endl;
      assert(!fDataPath.empty());
    } else {
      if(fDebug)
        std::cout << fDataPath << std::endl;
    }

    // check if any energies are given
    if(fDebug)
      std::cout << std::endl << "BMWStartupHandler::CheckLists: check energy list ..." << std::endl;
    if (fEnergyList.size() != fEnergyLabelList.size()) {
      std::cerr << "BMWStartupHandler::CheckLists: The number of energies and energy labels are different! Please fix it!" << std::endl \
           << "BMWStartupHandler::CheckLists: The program will be terminated now!" << std::endl;
      assert(fEnergyList.size() == fEnergyLabelList.size());
    }
    if (fEnergyList.empty()) {
      std::cerr << "BMWStartupHandler::CheckLists: Energy list empty!" << std::endl \
           << "BMWStartupHandler::CheckLists: Trying to use the standard energies: 0.0 to 35.0 keV in 0.1 keV steps" << std::endl;
      for (double x(0.0); x<= 35.0; x+=0.1) {
        fEnergyList.push_back(x);
      }
    }
    if (fEnergyLabelList.empty()) {
      std::cerr << "BMWStartupHandler::CheckLists: Energy label list empty!" << std::endl \
           << "BMWStartupHandler::CheckLists: Trying to use the specified energies as labels in the format %02.1f..." << std::endl \
           << "BMWStartupHandler::CheckLists: Most probably this will go wrong and should therefore be fixed in the xml-file!" << std::endl;
      char eChar[5];
      for(unsigned int i(0); i<fEnergyList.size(); i++) {
        snprintf(eChar, sizeof(eChar), "%02.1f", fEnergyList[i]);
        fEnergyLabelList.push_back(std::string(eChar));
      }
    }

    // fill the energies and labels in the map
    fEnergies.clear();
    for(unsigned int i(0); i < fEnergyList.size(); i++) {
      fEnergies[fEnergyList[i]] = fEnergyLabelList[i];
    }

    if(fDebug) {
      std::cout << "Energies and Labels:" << std::endl;
      for ( std::map<double, std::string>::const_iterator iter(fEnergies.begin()); iter != fEnergies.end(); ++iter )
        std::cout << iter->first << " " << iter->second << std::endl;
    }

    // check if any number of steps for the theory function is specified
    if(fDebug)
      std::cout << std::endl << "BMWStartupHandler::CheckLists: check number of steps for theory ..." << std::endl;
    if (!fNSteps) {
      std::cout << "BMWStartupHandler::CheckLists: You did not specify the number of steps for the theory. Setting the default (3000)." << std::endl;
      fNSteps = 3000;
    } else {
      if(fDebug)
        std::cout << fNSteps << std::endl;
    }
  }

  if (fVortex) {
    // check if any number of steps for the theory function is specified
    if(fDebug)
      std::cout << std::endl << "BMWStartupHandler::CheckLists: check number of steps for Vortex grid ..." << std::endl;
    if (!fGridSteps) {
      std::cout << "BMWStartupHandler::CheckLists: You did not specify the number of steps for the grid. Setting the default (256)." << std::endl;
      fGridSteps = 256;
    } else {
      if(fDebug)
        std::cout << fGridSteps << std::endl;
    }
  }
}

// -------------------------------------------------------------------------
// end
// -------------------------------------------------------------------------

