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
#include <fstream>
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
  fStartupFileFound = false;
  fStartupFilePath = "";

  // get default path (for the moment only linux like)
  char *pmusrpath;
  char *home;
  char musrpath[128];
  char startup_path_name[128];

  // check if the startup file is found in the current directory
  strcpy(startup_path_name, "./musrfit_startup.xml");
  if (StartupFileExists(startup_path_name)) {
    fStartupFileFound = true;
    fStartupFilePath = TString(startup_path_name);
  } else { // startup file is not found in the current directory
    // check if the MUSRFITPATH system variable is set
    pmusrpath = getenv("MUSRFITPATH");
    if (pmusrpath == 0) { // not set, will try default one
      home = getenv("HOME");
      sprintf(musrpath, "%s/analysis/bin", home);
      cout << endl << "**WARNING** MUSRFITPATH environment variable not set will try " << musrpath << endl;
    } else {
      strncpy(musrpath, pmusrpath, sizeof(musrpath));
    }
    sprintf(startup_path_name, "%s/musrfit_startup.xml", musrpath);
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
 * <p>
 */
PStartupHandler::~PStartupHandler()
{
  // clean up
  fDataPathList.clear();
  fMarkerList.clear();
  fColorList.clear();
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

  // init fourier default variables
  fFourierDefaults.fFourierBlockPresent = false;
  fFourierDefaults.fUnits = FOURIER_UNIT_FIELD;
  fFourierDefaults.fFourierPower = 0;
  fFourierDefaults.fApodization = FOURIER_APOD_NONE;
  fFourierDefaults.fPlotTag = FOURIER_PLOT_REAL_AND_IMAG;
  fFourierDefaults.fPhase = 0.0;
  fFourierDefaults.fRangeForPhaseCorrection[0] = -1.0;
  fFourierDefaults.fRangeForPhaseCorrection[1] = -1.0;
  fFourierDefaults.fPlotRange[0] = -1.0;
  fFourierDefaults.fPlotRange[1] = -1.0;
  fFourierDefaults.fPhaseIncrement = 1.0;
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
void PStartupHandler::OnStartElement(const char *str, const TList *attributes)
{
  if (!strcmp(str, "data_path")) {
    fKey = eDataPath;
  } else if (!strcmp(str, "marker")) {
    fKey = eMarker;
  } else if (!strcmp(str, "color")) {
    fKey = eColor;
  } else if (!strcmp(str, "units")) {
    fKey = eUnits;
  } else if (!strcmp(str, "fourier_power")) {
    fKey = eFourierPower;
  } else if (!strcmp(str, "apodization")) {
    fKey = eApodization;
  } else if (!strcmp(str, "plot")) {
    fKey = ePlot;
  } else if (!strcmp(str, "phase")) {
    fKey = ePhase;
  } else if (!strcmp(str, "phase_increment")) {
    fKey = ePhaseIncrement;
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
  Int_t      color, r, g, b, ival;

  switch (fKey) {
    case eDataPath:
      // check that str is a valid path
      // add str to the path list
      fDataPathList.push_back(str);
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
    case eUnits:
      tstr = TString(str);
      if (!tstr.CompareTo("gauss", TString::kIgnoreCase)) {
        fFourierDefaults.fUnits = FOURIER_UNIT_FIELD;
      } else if (!tstr.CompareTo("mhz", TString::kIgnoreCase)) {
        fFourierDefaults.fUnits = FOURIER_UNIT_FREQ;
      } else if (!tstr.CompareTo("mc/s", TString::kIgnoreCase)) {
        fFourierDefaults.fUnits = FOURIER_UNIT_CYCLES;
      } else {
        cout << endl << "PStartupHandler **WARNING** '" << str << "' is not a valid unit, will ignore it.";
        cout << endl;
      }
      break;
    case eFourierPower:
      tstr = TString(str);
      if (tstr.IsDigit()) {
        ival = tstr.Atoi();
        if ((ival >= 0) && (ival <= 20)) {
          fFourierDefaults.fFourierPower = ival;
        } else {
          cout << endl << "PStartupHandler **WARNING** fourier power '" << str << "' is not a valid number (0..20), will ignore it.";
          cout << endl;
        }
      } else {
        cout << endl << "PStartupHandler **WARNING** fourier power '" << str << "' is not a valid number (0..20), will ignore it.";
        cout << endl;
      }
      break;
    case eApodization:
      tstr = TString(str);
      if (!tstr.CompareTo("none", TString::kIgnoreCase)) {
        fFourierDefaults.fApodization = FOURIER_APOD_NONE;
      } else if (!tstr.CompareTo("weak", TString::kIgnoreCase)) {
        fFourierDefaults.fApodization = FOURIER_APOD_WEAK;
      } else if (!tstr.CompareTo("medium", TString::kIgnoreCase)) {
        fFourierDefaults.fApodization = FOURIER_APOD_MEDIUM;
      } else if (!tstr.CompareTo("strong", TString::kIgnoreCase)) {
        fFourierDefaults.fApodization = FOURIER_APOD_STRONG;
      } else {
        cout << endl << "PStartupHandler **WARNING** '" << str << "' is not a valid apodization, will ignore it.";
        cout << endl;
      }
      break;
    case ePlot:
      tstr = TString(str);
      if (!tstr.CompareTo("real", TString::kIgnoreCase)) {
        fFourierDefaults.fPlotTag = FOURIER_PLOT_REAL;
      } else if (!tstr.CompareTo("imag", TString::kIgnoreCase)) {
        fFourierDefaults.fPlotTag = FOURIER_PLOT_IMAG;
      } else if (!tstr.CompareTo("real_and_imag", TString::kIgnoreCase)) {
        fFourierDefaults.fPlotTag = FOURIER_PLOT_REAL_AND_IMAG;
      } else if (!tstr.CompareTo("power", TString::kIgnoreCase)) {
        fFourierDefaults.fPlotTag = FOURIER_PLOT_POWER;
      } else if (!tstr.CompareTo("phase", TString::kIgnoreCase)) {
        fFourierDefaults.fPlotTag = FOURIER_PLOT_PHASE;
      } else {
        cout << endl << "PStartupHandler **WARNING** '" << str << "' is not a valid plot option, will ignore it.";
        cout << endl;
      }
      break;
    case ePhase:
      tstr = TString(str);
      if (tstr.IsFloat()) {
        fFourierDefaults.fPhase = tstr.Atof();
      } else {
        cout << endl << "PStartupHandler **WARNING** '" << str << "' is not a valid phase, will ignore it.";
        cout << endl;
      }
      break;
    case ePhaseIncrement:
      tstr = TString(str);
      if (tstr.IsFloat()) {
        fFourierDefaults.fPhaseIncrement = tstr.Atof();
      } else {
        cout << endl << "PStartupHandler **WARNING** '" << str << "' is not a valid phase increment, will ignore it.";
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

//--------------------------------------------------------------------------
// CheckLists
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PStartupHandler::CheckLists()
{
  // check if anything was set, and if not set some default stuff

  // check if any data path is given
//cout << endl << ">> check data path list ...";  
  if (fDataPathList.size() == 0) {
//cout << endl << ">> data path list empty, will set default ones";  
    fDataPathList.push_back(TString("/mnt/data/nemu/his"));
    fDataPathList.push_back(TString("/mnt/data/nemu/wkm"));
  }

  // check if any markers are given
//cout << endl << ">> check marker list ...";  
  if (fMarkerList.size() == 0) {
//cout << endl << ">> marker list empty, will set default ones";  
    fMarkerList.push_back(24); // open circle
    fMarkerList.push_back(25); // open square
    fMarkerList.push_back(26); // open triangle
    fMarkerList.push_back(27); // open diamond
    fMarkerList.push_back(28); // open cross
    fMarkerList.push_back(29); // full star
    fMarkerList.push_back(30); // open star
    fMarkerList.push_back(20); // full circle
    fMarkerList.push_back(21); // full square
    fMarkerList.push_back(22); // full triangle
    fMarkerList.push_back(23); // full down triangle
    fMarkerList.push_back(2);  // thin cross
    fMarkerList.push_back(3);  // thin star
    fMarkerList.push_back(5);  // thin cross 45° rotated
  }

  // check if any colors are given
//cout << endl << ">> check color list ...";  
  if (fColorList.size() == 0) {
//cout << endl << ">> color list empty, will set default ones";  
    fColorList.push_back(TColor::GetColor(0, 0, 0));     // kBlack
    fColorList.push_back(TColor::GetColor(255, 0, 0));   // kRed
    fColorList.push_back(TColor::GetColor(0, 255, 0));   // kGreen
    fColorList.push_back(TColor::GetColor(0, 0, 255));   // kBlue
    fColorList.push_back(TColor::GetColor(255, 0, 255)); // kMagneta
    fColorList.push_back(TColor::GetColor(0, 255, 255)); // kCyan
    fColorList.push_back(TColor::GetColor(156, 0, 255)); // kViolette-3
    fColorList.push_back(TColor::GetColor(99, 101, 49)); // kYellow-1
    fColorList.push_back(TColor::GetColor(49, 101, 49)); // kGreen-1
    fColorList.push_back(TColor::GetColor(156, 48, 0));  // kOrange-4
  }
}

//--------------------------------------------------------------------------
// StartupFileExists
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PStartupHandler::StartupFileExists(char *fln)
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

