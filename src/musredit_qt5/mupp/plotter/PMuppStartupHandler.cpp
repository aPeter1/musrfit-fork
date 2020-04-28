/***************************************************************************

  PMuppStartupHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2017 by Andreas Suter                              *
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

#include <iostream>
#include <fstream>

#include "PMuppStartupHandler.h"

ClassImpQ(PMuppStartupHandler)

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
  std::fstream xmlFile;
  unsigned int xmlSize = 0;
  char *xmlBuffer = 0;

  xmlFile.open(startup_path_name, std::ios::in | std::ios::ate); // open file for reading and go to the end of the file
  if (xmlFile.is_open()) { // check if file has been opened successfully
    xmlSize = xmlFile.tellg(); // get the position within the stream == size of the file (since we are at the end)
    xmlFile.seekg(0, std::ios::beg); // go back to the beginning of the stream
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
 * @brief PMuppStartupHandler::PMuppStartupHandler
 */
PMuppStartupHandler::PMuppStartupHandler()
{
  fStartupFileFound = false;
  fStartupFilePath = "";

  // get default path (for the moment only linux like)
  Char_t *home=0;
  Char_t musrpath[128];
  Char_t startup_path_name[128];

  strncpy(musrpath, "", sizeof(musrpath));

  // check if the startup file is found in the current directory
  strcpy(startup_path_name, "./mupp_startup.xml");
  if (StartupFileExists(startup_path_name)) {
    fStartupFileFound = true;
    fStartupFilePath = TString(startup_path_name);
  }
  if (!fStartupFileFound) { // startup file not found in the current directory
    // check if the startup file is found under $HOME/.musrfit/mupp
    home = getenv("HOME");
    if (home != 0) {
      sprintf(startup_path_name, "%s/.musrfit/mupp/mupp_startup.xml", home);
      if (StartupFileExists(startup_path_name)) {
        fStartupFilePath = TString(startup_path_name);
        fStartupFileFound = true;
      }
    }
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * @brief PMuppStartupHandler::~PStartupHandler
 */
PMuppStartupHandler::~PMuppStartupHandler()
{
  fMarkerStyleList.clear();
  fMarkerSizeList.clear();
  fColorList.clear();
}

//--------------------------------------------------------------------------
// OnStartDocument
//--------------------------------------------------------------------------
/**
 * <p>Called on start of the XML file reading. Initializes all necessary variables.
 */
void PMuppStartupHandler::OnStartDocument()
{
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnEndDocument
//--------------------------------------------------------------------------
/**
 * <p>Called on end of XML file reading.
 */
void PMuppStartupHandler::OnEndDocument()
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
void PMuppStartupHandler::OnStartElement(const Char_t *str, const TList *attributes)
{
  if (!strcmp(str, "marker")) {
    fKey = eMarker;
  } else if (!strcmp(str, "color")) {
    fKey = eColor;
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
void PMuppStartupHandler::OnEndElement(const Char_t *str)
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
void PMuppStartupHandler::OnCharacters(const Char_t *str)
{
  TObjArray  *tokens;
  TObjString *ostr;
  TString tstr;
  Int_t   color, r, g, b;

  switch (fKey) {
  case eMarker:
    // check that str is <number>,<number>
    tstr = TString(str);
    tokens = tstr.Tokenize(",");
    if (tokens == 0) {
      std::cerr << std::endl << "PMuppStartupHandler **WARNING** '" << str << "' is not a marker code, will ignore it";
      std::cerr << std::endl;
      return;
    }
    if (tokens->GetEntries() != 2) {
      std::cerr << std::endl << "PMuppStartupHandler **WARNING** '" << str << "' is not a marker code, will ignore it";
      std::cerr << std::endl;
      return;
    }
    // get marker style
    ostr = dynamic_cast<TObjString*>(tokens->At(0));
    tstr = ostr->GetString();
    if (tstr.IsDigit()) {
      fMarkerStyleList.push_back(tstr.Atoi());
    } else {
      std::cerr << std::endl << "PMuppStartupHandler **WARNING** marker style '" << str << "' is not a number, will ignore it";
      std::cerr << std::endl;
    }
    // get marker size
    ostr = dynamic_cast<TObjString*>(tokens->At(1));
    tstr = ostr->GetString();
    if (tstr.IsFloat()) {
      fMarkerSizeList.push_back(tstr.Atof());
    } else {
      std::cerr << std::endl << "PMuppStartupHandler **WARNING** marker size '" << str << "' is not a float, will ignore it";
      std::cerr << std::endl;
    }
    // clean up tokens
    if (tokens) {
      delete tokens;
      tokens = 0;
    }
    break;
  case eColor:
    // check that str is a rbg code
    tstr = TString(str);
    tokens = tstr.Tokenize(",");
    // check that there any tokens
    if (tokens == 0) {
      std::cerr << std::endl << "PMuppStartupHandler **WARNING** '" << str << "' is not a rbg code, will ignore it";
      std::cerr << std::endl;
      return;
    }
    // check there is the right number of tokens
    if (tokens->GetEntries() < 3) {
      std::cerr << std::endl << "PMuppStartupHandler **WARNING** '" << str << "' is not a rbg code, will ignore it";
      std::cerr << std::endl;
      return;
    }
    // get r
    ostr = dynamic_cast<TObjString*>(tokens->At(0));
    tstr = ostr->GetString();
    if (tstr.IsDigit()) {
      r = tstr.Atoi();
    } else {
      std::cerr << std::endl << "PMuppStartupHandler **WARNING** r within the rgb code is not a number, will ignore it";
      std::cerr << std::endl;
      return;
    }
    // get g
    ostr = dynamic_cast<TObjString*>(tokens->At(1));
    tstr = ostr->GetString();
    if (tstr.IsDigit()) {
      g = tstr.Atoi();
    } else {
      std::cerr << std::endl << "PMuppStartupHandler **WARNING** g within the rgb code is not a number, will ignore it";
      std::cerr << std::endl;
      return;
    }
    // get b
    ostr = dynamic_cast<TObjString*>(tokens->At(2));
    tstr = ostr->GetString();
    if (tstr.IsDigit()) {
      b = tstr.Atoi();
    } else {
      std::cerr << std::endl << "PMuppStartupHandler **WARNING** b within the rgb code is not a number, will ignore it";
      std::cerr << std::endl;
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
 * <p>Called when a XML comment is found. Not used.
 *
 * \param str not used.
 */
void PMuppStartupHandler::OnComment(const Char_t *str)
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
void PMuppStartupHandler::OnWarning(const Char_t *str)
{
  std::cerr << std::endl << "PMuppStartupHandler **WARNING** " << str;
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
void PMuppStartupHandler::OnError(const Char_t *str)
{
  std::cerr << std::endl << "PMuppStartupHandler **ERROR** " << str;
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
void PMuppStartupHandler::OnFatalError(const Char_t *str)
{
  std::cerr << std::endl << "PMuppStartupHandler **FATAL ERROR** " << str;
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
void PMuppStartupHandler::OnCdataBlock(const Char_t *str, Int_t len)
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
void PMuppStartupHandler::CheckLists()
{
  // check if anything was set, and if not set some default stuff

  // check if any marker styles are given
  if (fMarkerStyleList.size() == 0) {
    fMarkerStyleList.push_back(24); // open circle
    fMarkerStyleList.push_back(25); // open square
    fMarkerStyleList.push_back(26); // open triangle
    fMarkerStyleList.push_back(27); // open diamond
    fMarkerStyleList.push_back(28); // open cross
    fMarkerStyleList.push_back(29); // full star
    fMarkerStyleList.push_back(30); // open star
    fMarkerStyleList.push_back(20); // full circle
    fMarkerStyleList.push_back(21); // full square
    fMarkerStyleList.push_back(22); // full triangle
    fMarkerStyleList.push_back(23); // full down triangle
    fMarkerStyleList.push_back(2);  // thin cross
    fMarkerStyleList.push_back(3);  // thin star
    fMarkerStyleList.push_back(5);  // thin cross 45° rotated
  }

  // check if any marker ssizes are given
  if (fMarkerSizeList.size() == 0) {
    fMarkerSizeList.push_back(1.3);
    fMarkerSizeList.push_back(1.3);
    fMarkerSizeList.push_back(1.5);
    fMarkerSizeList.push_back(1.5);
    fMarkerSizeList.push_back(1.5);
    fMarkerSizeList.push_back(1.5);
    fMarkerSizeList.push_back(1.3);
    fMarkerSizeList.push_back(1.3);
    fMarkerSizeList.push_back(1.5);
    fMarkerSizeList.push_back(1.5);
    fMarkerSizeList.push_back(1.5);
    fMarkerSizeList.push_back(1.5);
    fMarkerSizeList.push_back(1.5);
    fMarkerSizeList.push_back(1.5);
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
Bool_t PMuppStartupHandler::StartupFileExists(Char_t *fln)
{
  Bool_t result = false;

  std::ifstream ifile(fln);

  if (ifile.fail()) {
    result = false;
  } else {
    result = true;
    ifile.close();
  }

  return result;
}

