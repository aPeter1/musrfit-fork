/***************************************************************************

  PStartupHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2023 by Andreas Suter                              *
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

#include <sys/types.h>
#include <sys/stat.h>

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <TObjArray.h>
#include <TObjString.h>
#include <TColor.h>
#include <TList.h>
#include <TXMLAttr.h>

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
  std::fstream xmlFile;
  unsigned int xmlSize = 0;
  char *xmlBuffer = nullptr;

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
    xmlBuffer = nullptr;
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
  Char_t *pmusrpath=nullptr;
  Char_t *home=nullptr;
  Char_t musrpath[128];
  Char_t startup_path_name[128];

  strncpy(musrpath, "", sizeof(musrpath));

  // check if the startup file is found in the current directory
  strcpy(startup_path_name, "./musrfit_startup.xml");
  if (StartupFileExists(startup_path_name)) {
    fStartupFileFound = true;
    fStartupFilePath = TString(startup_path_name);
  }
  if (!fStartupFileFound) { // startup file not found in the current directory
    // check if the startup file is found under $HOME/.musrfit
    home = getenv("HOME");
    if (home != nullptr) {
      snprintf(startup_path_name, sizeof(startup_path_name), "%s/.musrfit/musrfit_startup.xml", home);
      if (StartupFileExists(startup_path_name)) {
        fStartupFilePath = TString(startup_path_name);
        fStartupFileFound = true;
      }
    }
  }
  if (!fStartupFileFound) { // startup file not found in $HOME/.musrfit
    // check if the MUSRFITPATH system variable is set
    pmusrpath = getenv("MUSRFITPATH");
    if (pmusrpath != nullptr) {
      snprintf(startup_path_name, sizeof(startup_path_name), "%s/musrfit_startup.xml", pmusrpath);
      if (StartupFileExists(startup_path_name)) {
        fStartupFilePath = TString(startup_path_name);
        fStartupFileFound = true;
      }
    }
  }
  if (!fStartupFileFound) { // MUSRFITPATH not set or empty, will try $ROOTSYS/bin
    home = getenv("ROOTSYS");
    if (home != nullptr) {
      snprintf(musrpath, sizeof(musrpath), "%s/bin", home);
      std::cerr << std::endl << "**WARNING** MUSRFITPATH environment variable not set will try " << musrpath << std::endl;
      snprintf(startup_path_name, sizeof(startup_path_name), "%s/musrfit_startup.xml", musrpath);
      if (StartupFileExists(startup_path_name)) {
        fStartupFilePath = TString(startup_path_name);
        fStartupFileFound = true;
      }
    }
  }

  // if musrfit_startup.xml is still not found, will create a default one
  if (!fStartupFileFound) {
    std::cout << std::endl << "**INFO** no musrfit_startup.xml file found, will write a default one." << std::endl;
    if (!WriteDefaultStartupFile()) {
      std::cerr << std::endl << "**ERROR** couldn't write default musrfit_startup.xml." << std::endl;
    } else {
      home = getenv("HOME");
      if (home != nullptr) {
        snprintf(startup_path_name, sizeof(startup_path_name), "%s/.musrfit/musrfit_startup.xml", home);
        if (StartupFileExists(startup_path_name)) {
          fStartupFilePath = TString(startup_path_name);
          fStartupFileFound = true;
        }
      }
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
  fRunNameTemplate.clear();
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
  fFourierDefaults.fUnits = FOURIER_UNIT_GAUSS;
  fFourierDefaults.fFourierPower = 0;
  fFourierDefaults.fApodization = FOURIER_APOD_NONE;
  fFourierDefaults.fPlotTag = FOURIER_PLOT_REAL_AND_IMAG;
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
  } else if (!strcmp(str, "run_name_template")) {
    fKey = eRunNameTemplate;
    TXMLAttr *attr;
    TIter next(attributes);
    while ((attr = (TXMLAttr*) next())) {
      if (!strcmp(attr->GetName(), "inst")) {
        fCurrentInstrumentName = attr->GetValue();
      }
    }
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

  PRunNameTemplate tmpl;
  switch (fKey) {
    case eDataPath:
      // check that str is a valid path
      // add str to the path list
      fDataPathList.push_back(str);
      break;
    case eRunNameTemplate:
      tmpl.instrument = fCurrentInstrumentName;
      tmpl.runNameTemplate = str;
      fRunNameTemplate.push_back(tmpl);
      fCurrentInstrumentName="???";
      break;
    case eMarker:
      // check that str is a number
      tstr = TString(str);
      if (tstr.IsDigit()) {
        // add converted str to the marker list
        fMarkerList.push_back(tstr.Atoi());
      } else {
        std::cerr << std::endl << "PStartupHandler **WARNING** '" << str << "' is not a number, will ignore it";
        std::cerr << std::endl;
      }
      break;
    case eColor:
      // check that str is a rbg code
      tstr = TString(str);
      tokens = tstr.Tokenize(",");
      // check that there any tokens
      if (!tokens) {
        std::cerr << std::endl << "PStartupHandler **WARNING** '" << str << "' is not a rbg code, will ignore it";
        std::cerr << std::endl;
        return;
      }
      // check there is the right number of tokens
      if (tokens->GetEntries() != 3) {
        std::cerr << std::endl << "PStartupHandler **WARNING** '" << str << "' is not a rbg code, will ignore it";
        std::cerr << std::endl;
        return;
      }
      // get r
      ostr = dynamic_cast<TObjString*>(tokens->At(0));
      tstr = ostr->GetString();
      if (tstr.IsDigit()) {
        r = tstr.Atoi();
      } else {
        std::cerr << std::endl << "PStartupHandler **WARNING** r within the rgb code is not a number, will ignore it";
        std::cerr << std::endl;
        return;
      }
      // get g
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      tstr = ostr->GetString();
      if (tstr.IsDigit()) {
        g = tstr.Atoi();
      } else {
        std::cerr << std::endl << "PStartupHandler **WARNING** g within the rgb code is not a number, will ignore it";
        std::cerr << std::endl;
        return;
      }
      // get b
      ostr = dynamic_cast<TObjString*>(tokens->At(2));
      tstr = ostr->GetString();
      if (tstr.IsDigit()) {
        b = tstr.Atoi();
      } else {
        std::cerr << std::endl << "PStartupHandler **WARNING** b within the rgb code is not a number, will ignore it";
        std::cerr << std::endl;
        return;
      }
      // clean up tokens
      if (tokens) {
        delete tokens;
        tokens = nullptr;
      }
      // generate the ROOT color code based on str
      color = TColor::GetColor(r,g,b);
      // add the color code to the color list
      fColorList.push_back(color);
      break;
    case eUnits:
      tstr = TString(str);
      if (!tstr.CompareTo("gauss", TString::kIgnoreCase)) {
        fFourierDefaults.fUnits = FOURIER_UNIT_GAUSS;
      } else if (!tstr.CompareTo("tesla", TString::kIgnoreCase)) {
        fFourierDefaults.fUnits = FOURIER_UNIT_TESLA;
      } else if (!tstr.CompareTo("mhz", TString::kIgnoreCase)) {
        fFourierDefaults.fUnits = FOURIER_UNIT_FREQ;
      } else if (!tstr.CompareTo("mc/s", TString::kIgnoreCase)) {
        fFourierDefaults.fUnits = FOURIER_UNIT_CYCLES;
      } else {
        std::cerr << std::endl << "PStartupHandler **WARNING** '" << str << "' is not a valid unit, will ignore it.";
        std::cerr << std::endl;
      }
      break;
    case eFourierPower:
      tstr = TString(str);
      if (tstr.IsDigit()) {
        ival = tstr.Atoi();
        if ((ival >= 0) && (ival <= 20)) {
          fFourierDefaults.fFourierPower = ival;
        } else {
          std::cerr << std::endl << "PStartupHandler **WARNING** fourier power '" << str << "' is not a valid number (0..20), will ignore it.";
          std::cerr << std::endl;
        }
      } else {
        std::cerr << std::endl << "PStartupHandler **WARNING** fourier power '" << str << "' is not a valid number (0..20), will ignore it.";
        std::cerr << std::endl;
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
        std::cerr << std::endl << "PStartupHandler **WARNING** '" << str << "' is not a valid apodization, will ignore it.";
        std::cerr << std::endl;
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
        std::cerr << std::endl << "PStartupHandler **WARNING** '" << str << "' is not a valid plot option, will ignore it.";
        std::cerr << std::endl;
      }
      break;
    case ePhase:
      tstr = TString(str);
      if (tstr.IsFloat()) {
        fFourierDefaults.fPhase.push_back(tstr.Atof());
      } else {
        std::cerr << std::endl << "PStartupHandler **WARNING** '" << str << "' is not a valid phase, will ignore it.";
        std::cerr << std::endl;
      }
      break;
    case ePhaseIncrement:
      tstr = TString(str);
      if (tstr.IsFloat()) {
        fFourierDefaults.fPhaseIncrement = tstr.Atof();
      } else {
        std::cerr << std::endl << "PStartupHandler **WARNING** '" << str << "' is not a valid phase increment, will ignore it.";
        std::cerr << std::endl;
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
  std::cerr << std::endl << "PStartupHandler **WARNING** " << str;
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
void PStartupHandler::OnError(const Char_t *str)
{
  std::cerr << std::endl << "PStartupHandler **ERROR** " << str;
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
void PStartupHandler::OnFatalError(const Char_t *str)
{
  std::cerr << std::endl << "PStartupHandler **FATAL ERROR** " << str;
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

  std::ifstream ifile(fln);

  if (ifile.fail()) {
    result = false;
  } else {
    result = true;
    ifile.close();
  }

  return result;
}

//--------------------------------------------------------------------------
// WriteDefaultStartupFile
//--------------------------------------------------------------------------
Bool_t PStartupHandler::WriteDefaultStartupFile()
{
  // get home
  Char_t startup_path_name[256];
  Char_t *home = nullptr;
  home = getenv("HOME");
  if (home == nullptr) {
    std::cerr << std::endl << "**ERROR** couldn't obtain $HOME." << std::endl;
    return false;
  }

  // first check that $HOME/.musrfit exists and if NOT create it
  struct stat info;

  snprintf(startup_path_name, sizeof(startup_path_name), "%s/.musrfit", home);
  if (!stat(startup_path_name, &info)) {
    if (!(info.st_mode & S_IFDIR))
      return false;
  } else {
    if (mkdir(startup_path_name, 0777)) {
      std::cerr << std::endl << "**ERROR** couldn't create '" << startup_path_name << "'" << std::endl;
      return false;
    }
  }

  // set path-name for musrfit_startup.xml
  snprintf(startup_path_name, sizeof(startup_path_name), "%s/.musrfit/musrfit_startup.xml", home);

  std::ofstream fout(startup_path_name, std::ofstream::out);
  if (!fout.is_open()) {
    std::cerr << std::endl << "**ERROR** couldn't open '" << startup_path_name << "' for writing." << std::endl;
    return false;
  }

  // write default musrfit_startup.xml
  fout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  fout << "<musrfit xmlns=\"http://lmu.web.psi.ch/musrfit/user/MUSR/WebHome.html\">" << std::endl;
  fout << "    <comment>" << std::endl;
  fout << "        Defines default settings for the musrfit package" << std::endl;
  fout << "    </comment>" << std::endl;
  fout << "    <data_path>/afs/psi.ch/project/nemu/data/his</data_path>" << std::endl;
  fout << "    <data_path>/afs/psi.ch/project/nemu/data/wkm</data_path>" << std::endl;
  fout << "    <data_path>/afs/psi.ch/project/bulkmusr/data/gps</data_path>" << std::endl;
  fout << "    <data_path>/afs/psi.ch/project/bulkmusr/data/dolly</data_path>" << std::endl;
  fout << "    <data_path>/afs/psi.ch/project/bulkmusr/data/gpd</data_path>" << std::endl;
  fout << "    <data_path>/afs/psi.ch/project/bulkmusr/data/ltf</data_path>" << std::endl;
  fout << "    <data_path>/afs/psi.ch/project/bulkmusr/data/alc</data_path>" << std::endl;
  fout << "    <data_path>/afs/psi.ch/project/bulkmusr/data/hifi</data_path>" << std::endl;
  fout << "    <data_path>/afs/psi.ch/project/bulkmusr/data/lem</data_path>" << std::endl;
  fout << "    <!-- Dolly/PSI -->" << std::endl;
  fout << "    <run_name_template inst=\"dolly\">d%yyyy%/pie1/deltat_flc_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"dolly\">d%yyyy%/pie3/deltat_flc_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"dolly\">d%yyyy%/deltat_flc_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"dolly\">d%yyyy%/deltat_pta_dolly_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"dolly\">d%yyyy%/pta/deltat_pta_dolly_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"dolly\">d%yyyy%/pta/deltat_pta_dolly_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"dolly\">d%yyyy%/tdc/deltat_tdc_dolly_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"dolly\">d%yyyy%/tdc/root/deltat_tdc_dolly_%rrrr%.root</run_name_template>" << std::endl;
  fout << "    <!-- Flame/PSI -->" << std::endl;
  fout << "    <run_name_template inst=\"flame\">d%yyyy%/tdc/root/deltat_tdc_flame_%yyyy%_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <!-- GPD/PSI -->" << std::endl;
  fout << "    <run_name_template inst=\"gpd\">d%yyyy%/deltat_mue1_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gpd\">d%yyyy%/deltat_fq_si_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gpd\">d%yyyy%/deltat_strobo_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gpd\">d%yyyy%/deltat_hp_ni_ht_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gpd\">d%yyyy%/deltat_hp_ni_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gpd\">d%yyyy%/deltat_ccr2_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gpd\">d%yyyy%/deltat_gpd_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gpd\">d%yyyy%/deltat_janis_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gpd\">d%yyyy%/deltat_janis_gpd_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gpd\">d%yyyy%/deltat_pta_gpd_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gpd\">d%yyyy%/tdc/deltat_tdc_gpd_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gpd\">d%yyyy%/tdc/root/deltat_tdc_gpd_%rrrr%.root</run_name_template>" << std::endl;
  fout << "    <!-- GPS/PSI -->" << std::endl;
  fout << "    <run_name_template inst=\"gps\">d%yyyy%/deltat_ccr_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gps\">d%yyyy%/deltat_he3_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gps\">d%yyyy%/deltat_stutt_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gps\">d%yyyy%/deltat_ltf_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gps\">d%yyyy%/deltat_flc_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gps\">d%yyyy%/deltat_flc2_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gps\">d%yyyy%/deltat_oven_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gps\">d%yyyy%/deltat_oven2_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gps\">d%yyyy%/deltat_pta_gps_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gps\">d%yyyy%/tdc/deltat_tdc_gps_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"gps\">d%yyyy%/tdc/root/deltat_tdc_gps_%yyyy%_%rrrr%.root</run_name_template>" << std::endl;
  fout << "    <!-- HAL-9500/PSI == HIFI/PSI -->" << std::endl;
  fout << "    <run_name_template inst=\"hifi\">d%yyyy%/tdc/deltat_hifi_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"hifi\">d%yyyy%/tdc/tdc_hifi_%yyyy%_%rrrrr%.mdu</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"hifi\">d%yyyy%/tdc/root/deltat_tdc_hifi_%yyyy%_%rrrr%.mdu</run_name_template>" << std::endl;
  fout << "    <!-- LTF/PSI -->" << std::endl;
  fout << "    <run_name_template inst=\"ltf\">d%yyyy%/deltat_ltf_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"ltf\">d%yyyy%/deltat_pta_ltf_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"ltf\">d%yyyy%/pta/deltat_pta_ltf_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"ltf\">d%yyyy%/tdc/deltat_tdc_ltf_%rrrr%.bin</run_name_template>" << std::endl;
  fout << "    <!-- LEM/PSI -->" << std::endl;
  fout << "    <run_name_template inst=\"lem\">%yyyy%/lem%yy%_his_%rrrr%.root</run_name_template>" << std::endl;
  fout << "    <run_name_template inst=\"lem\">d%yyyy%/tdc/lem%yy%_his_%rrrr%.root</run_name_template>" << std::endl;
  fout << "    <fourier_settings>" << std::endl;
  fout << "        <units>Gauss</units>" << std::endl;
  fout << "        <fourier_power>0</fourier_power>" << std::endl;
  fout << "        <apodization>none</apodization>" << std::endl;
  fout << "        <plot>real_and_imag</plot>" << std::endl;
  fout << "        <phase>0.0</phase>" << std::endl;
  fout << "        <phase_increment>1.0</phase_increment>" << std::endl;
  fout << "    </fourier_settings>" << std::endl;
  fout << "    <root_settings>" << std::endl;
  fout << "        <marker_list>" << std::endl;
  fout << "            <!-- Root marker numbers -->" << std::endl;
  fout << "            <marker>24</marker> <!-- open circle -->" << std::endl;
  fout << "            <marker>25</marker> <!-- open square -->" << std::endl;
  fout << "            <marker>26</marker> <!-- open triangle -->" << std::endl;
  fout << "            <marker>27</marker> <!-- open diamond -->" << std::endl;
  fout << "            <marker>28</marker> <!-- open cross -->" << std::endl;
  fout << "            <marker>29</marker> <!-- full star -->" << std::endl;
  fout << "            <marker>30</marker> <!-- open star -->" << std::endl;
  fout << "            <marker>20</marker> <!-- full circle -->" << std::endl;
  fout << "            <marker>21</marker> <!-- full square -->" << std::endl;
  fout << "            <marker>22</marker> <!-- full triangle -->" << std::endl;
  fout << "            <marker>23</marker> <!-- full triangle down -->" << std::endl;
  fout << "            <marker>2</marker>  <!-- thin cross -->" << std::endl;
  fout << "            <marker>3</marker>  <!-- thin star -->" << std::endl;
  fout << "            <marker>5</marker>  <!-- thin x -->" << std::endl;
  fout << "        </marker_list>" << std::endl;
  fout << "        <color_list>" << std::endl;
  fout << "            <!-- Color as RGB coded string -->" << std::endl;
  fout << "            <color>0,0,0</color>      <!-- kBlack -->" << std::endl;
  fout << "            <color>255,0,0</color>    <!-- kRed -->" << std::endl;
  fout << "            <color>0,255,0</color>    <!-- kGreen -->" << std::endl;
  fout << "            <color>0,0,255</color>    <!-- kBlue -->" << std::endl;
  fout << "            <color>255,0,255</color>  <!-- kMagenta -->" << std::endl;
  fout << "            <color>0,255,255</color>  <!-- kCyan -->" << std::endl;
  fout << "            <color>153,0,255</color>  <!-- kViolet-3 -->" << std::endl;
  fout << "            <color>102,102,51</color> <!-- kYellow-1 -->" << std::endl;
  fout << "            <color>51,102,51</color>  <!-- kGreen-1 -->" << std::endl;
  fout << "            <color>153,0,0</color>    <!-- kRed+2 -->" << std::endl;
  fout << "        </color_list>" << std::endl;
  fout << "    </root_settings>" << std::endl;
  fout << "</musrfit>" << std::endl;

  fout.close();

  return true;
}

// -------------------------------------------------------------------------
// end
// -------------------------------------------------------------------------

