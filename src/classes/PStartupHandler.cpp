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

#include <cstdlib>
#include <iostream>
#include <fstream>
using namespace std;

#include <TObjArray.h>
#include <TObjString.h>
#include <TColor.h>

#include "PStartupHandler.h"

ClassImpQ(PStartupHandler)

//--------------------------------------------------------------------------
// This function is a replacement for the ParseFile method of TSAXParser.
// It is needed because in certain environments ParseFile does not work but ParseBuffer does.
//--------------------------------------------------------------------------
/**
 * <p> Replacement for the ParseFile method of TSAXParser.
 *
 * <p><b>return:</b>
 * - 1 if file cannot be read
 * - 0 if the file has been parsed successfully
 * - parse error code otherwise
 *
 * \param saxParser pointer to a TSAXParser object
 * \param startup_path_name full path to the XML file to be read
 */
int parseXmlFile(TSAXParser *saxParser, const char *startup_path_name)
{
  int status;
  fstream xmlFile;
  unsigned int xmlSize = 0;
  char *xmlBuffer = 0;

  xmlFile.open(startup_path_name, ios::in | ios::ate); // open file for reading and go to the end of the file
  if (xmlFile.is_open()) { // check if file has been opened successfully
    xmlSize = xmlFile.tellg(); // get the position within the stream == size of the file (since we are at the end)
    xmlFile.seekg(0, ios::beg); // go back to the beginning of the stream
    xmlBuffer = new char[xmlSize]; // allocate buffer memory for the whole XML file
    xmlFile.read(xmlBuffer, xmlSize); // read in the whole XML file into the buffer
    xmlFile.close(); // close the XML file
  }
  if (!xmlBuffer) { // file has not been read into the buffer
    status = 1;
  } else {
    status = saxParser->ParseBuffer(xmlBuffer, xmlSize); // parse buffer
    delete[] xmlBuffer; // free the buffer memory
    xmlBuffer = 0;
  }

  return status;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor. Check if the musrfit_startup.xml file is found in some standard search paths
 */
PStartupHandler::PStartupHandler()
{
  fStartupFileFound = false;
  fStartupFilePath = "";

  // get default path (for the moment only linux like)
  Char_t *pmusrpath;
  Char_t *home;
  Char_t musrpath[128];
  Char_t startup_path_name[128];
  Bool_t pmusrpathfound = false;

  // check if the startup file is found in the current directory
  strcpy(startup_path_name, "./musrfit_startup.xml");
  if (StartupFileExists(startup_path_name)) {
    fStartupFileFound = true;
    fStartupFilePath = TString(startup_path_name);
  } else { // startup file is not found in the current directory
    // check if the MUSRFITPATH system variable is set
    pmusrpath = getenv("MUSRFITPATH");
    if (pmusrpath != 0) {
      if (strcmp(pmusrpath, "")) { // MUSRFITPATH variable set but empty
        pmusrpathfound = true;
      }
    }
    if (!pmusrpathfound) { // MUSRFITPATH not set or empty, will try default one
      home = getenv("ROOTSYS");
      sprintf(musrpath, "%s/bin", home);
      cerr << endl << "**WARNING** MUSRFITPATH environment variable not set will try " << musrpath << endl;
    } else {
      strncpy(musrpath, pmusrpath, sizeof(musrpath));
    }
    sprintf(startup_path_name, "%s/musrfit_startup.xml", musrpath);
    fStartupFilePath = TString(startup_path_name);
    if (StartupFileExists(startup_path_name)) {
      fStartupFileFound = true;
    }
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor
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
 * <p>Called on start of the XML file reading. Initializes all necessary variables.
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
 * <p>Called on end of XML file reading.
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
 * <p>Called when a XML start element is found. Filters out the needed elements
 * and sets a proper key.
 *
 * \param str XML element name
 * \param attributes not used
 */
void PStartupHandler::OnStartElement(const Char_t *str, const TList *attributes)
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
 * <p>Called when a XML end element is found. Resets the handler key.
 *
 * \param str not used
 */
void PStartupHandler::OnEndElement(const Char_t *str)
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
void PStartupHandler::OnCharacters(const Char_t *str)
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
        cerr << endl << "PStartupHandler **WARNING** '" << str << "' is not a number, will ignore it";
        cerr << endl;
      }
      break;
    case eColor:
      // check that str is a rbg code
      tstr = TString(str);
      tokens = tstr.Tokenize(",");
      // check that there any tokens
      if (!tokens) {
        cerr << endl << "PStartupHandler **WARNING** '" << str << "' is not a rbg code, will ignore it";
        cerr << endl;
        return;
      }
      // check there is the right number of tokens
      if (tokens->GetEntries() != 3) {
        cerr << endl << "PStartupHandler **WARNING** '" << str << "' is not a rbg code, will ignore it";
        cerr << endl;
        return;
      }
      // get r
      ostr = dynamic_cast<TObjString*>(tokens->At(0));
      tstr = ostr->GetString();
      if (tstr.IsDigit()) {
        r = tstr.Atoi();
      } else {
        cerr << endl << "PStartupHandler **WARNING** r within the rgb code is not a number, will ignore it";
        cerr << endl;
        return;
      }
      // get g
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      tstr = ostr->GetString();
      if (tstr.IsDigit()) {
        g = tstr.Atoi();
      } else {
        cerr << endl << "PStartupHandler **WARNING** g within the rgb code is not a number, will ignore it";
        cerr << endl;
        return;
      }
      // get b
      ostr = dynamic_cast<TObjString*>(tokens->At(2));
      tstr = ostr->GetString();
      if (tstr.IsDigit()) {
        b = tstr.Atoi();
      } else {
        cerr << endl << "PStartupHandler **WARNING** b within the rgb code is not a number, will ignore it";
        cerr << endl;
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
        cerr << endl << "PStartupHandler **WARNING** '" << str << "' is not a valid unit, will ignore it.";
        cerr << endl;
      }
      break;
    case eFourierPower:
      tstr = TString(str);
      if (tstr.IsDigit()) {
        ival = tstr.Atoi();
        if ((ival >= 0) && (ival <= 20)) {
          fFourierDefaults.fFourierPower = ival;
        } else {
          cerr << endl << "PStartupHandler **WARNING** fourier power '" << str << "' is not a valid number (0..20), will ignore it.";
          cerr << endl;
        }
      } else {
        cerr << endl << "PStartupHandler **WARNING** fourier power '" << str << "' is not a valid number (0..20), will ignore it.";
        cerr << endl;
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
        cerr << endl << "PStartupHandler **WARNING** '" << str << "' is not a valid apodization, will ignore it.";
        cerr << endl;
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
        cerr << endl << "PStartupHandler **WARNING** '" << str << "' is not a valid plot option, will ignore it.";
        cerr << endl;
      }
      break;
    case ePhase:
      tstr = TString(str);
      if (tstr.IsFloat()) {
        fFourierDefaults.fPhase = tstr.Atof();
      } else {
        cerr << endl << "PStartupHandler **WARNING** '" << str << "' is not a valid phase, will ignore it.";
        cerr << endl;
      }
      break;
    case ePhaseIncrement:
      tstr = TString(str);
      if (tstr.IsFloat()) {
        fFourierDefaults.fPhaseIncrement = tstr.Atof();
      } else {
        cerr << endl << "PStartupHandler **WARNING** '" << str << "' is not a valid phase increment, will ignore it.";
        cerr << endl;
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
 * <p>Called when a XML comment is found. Not used.
 *
 * \param str not used.
 */
void PStartupHandler::OnComment(const Char_t *str)
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
void PStartupHandler::OnWarning(const Char_t *str)
{
  cerr << endl << "PStartupHandler **WARNING** " << str;
  cerr << endl;
}

//--------------------------------------------------------------------------
// OnError
//--------------------------------------------------------------------------
/**
 * <p>Called when the XML parser emits an error.
 *
 * \param str error string
 */
void PStartupHandler::OnError(const Char_t *str)
{
  cerr << endl << "PStartupHandler **ERROR** " << str;
  cerr << endl;
}

//--------------------------------------------------------------------------
// OnFatalError
//--------------------------------------------------------------------------
/**
 * <p>Called when the XML parser emits a fatal error.
 *
 * \param str fatal error string
 */
void PStartupHandler::OnFatalError(const Char_t *str)
{
  cerr << endl << "PStartupHandler **FATAL ERROR** " << str;
  cerr << endl;
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
void PStartupHandler::OnCdataBlock(const Char_t *str, Int_t len)
{
  // nothing to be done for now
}

//--------------------------------------------------------------------------
// CheckLists
//--------------------------------------------------------------------------
/**
 * <p>Check if the default lists are present and if not, feed them with some default settings
 *
 */
void PStartupHandler::CheckLists()
{
  // check if anything was set, and if not set some default stuff

  // check if any data path is given
  if (fDataPathList.size() == 0) {
    fDataPathList.push_back(TString("/mnt/data/nemu/his"));
    fDataPathList.push_back(TString("/mnt/data/nemu/wkm"));
    fDataPathList.push_back(TString("/afs/psi.ch/project/nemu/data/his"));
    fDataPathList.push_back(TString("/afs/psi.ch/project/nemu/data/wkm"));
    fDataPathList.push_back(TString("/afs/psi.ch/project/bulkmusr/data/gps"));
    fDataPathList.push_back(TString("/afs/psi.ch/project/bulkmusr/data/dolly"));
    fDataPathList.push_back(TString("/afs/psi.ch/project/bulkmusr/data/gpd"));
    fDataPathList.push_back(TString("/afs/psi.ch/project/bulkmusr/data/ltf"));
    fDataPathList.push_back(TString("/afs/psi.ch/project/bulkmusr/data/alc"));
  }

  // check if any markers are given
  if (fMarkerList.size() == 0) {
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
  if (fColorList.size() == 0) {
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
 * <p>Checks if a file is present on the disc.
 *
 * <b>return:</b>
 * - true, if the file is present
 * - false, otherwise
 *
 * \param fln file name
 */
Bool_t PStartupHandler::StartupFileExists(Char_t *fln)
{
  Bool_t result = false;

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

