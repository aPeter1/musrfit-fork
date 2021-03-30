/***************************************************************************

  PRgeHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2021 by Andreas Suter                              *
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

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "PRgeHandler.h"

ClassImpQ(PXmlRgeHandler)

//--------------------------------------------------------------------------
// OnStartDocument
//--------------------------------------------------------------------------
/**
 * <p>Called on start of the XML file reading. Initializes all necessary variables.
 */
void PXmlRgeHandler::OnStartDocument()
{
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnEndDocument
//--------------------------------------------------------------------------
/**
 * <p>Called on end of XML file reading.
 */
void PXmlRgeHandler::OnEndDocument()
{
  if (!fIsValid)
    return;

  // check if anything was set
  if (fTrimSpDataPath.empty()) {
    std::string err = "<data_path> content is missing!";
    fIsValid = false;
    OnError(err.c_str());
  }
  if (fTrimSpFlnPre.empty()) {
    std::string err = "<rge_fln_pre> content is missing!";
    fIsValid = false;
    OnError(err.c_str());
  }
  if (fTrimSpDataEnergyList.size()==0) {
    std::string err = "no implantation energies present!";
    fIsValid = false;
    OnError(err.c_str());
  }
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
void PXmlRgeHandler::OnStartElement(const Char_t *str, const TList *attributes)
{
  if (!strcmp(str, "trim_sp")) {
    isTrimSp=true;
  } else if (!strcmp(str, "data_path") && isTrimSp) {
    fKey = eDataPath;
  } else if (!strcmp(str, "rge_fln_pre") && isTrimSp) {
    fKey = eFlnPre;
  } else if (!strcmp(str, "energy") && isTrimSp) {
    fKey = eEnergy;
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
void PXmlRgeHandler::OnEndElement(const Char_t *str)
{
  if (!strcmp(str, "trim_sp")) {
    isTrimSp=false;
  }

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
void PXmlRgeHandler::OnCharacters(const Char_t *str)
{
  int ival;
  std::string msg(""), sstr(str);
  size_t pos;

  switch(fKey) {
    case eDataPath:
      fTrimSpDataPath=str;
      break;
    case eFlnPre:
      fTrimSpFlnPre=str;
      break;
    case eEnergy:
      try {
        ival = std::stoi(str, &pos);
      } catch(std::invalid_argument& e) {
        fIsValid = false;
        msg = "The found energy '" + sstr + "' which is not a number";
        OnError(msg.c_str());
      } catch(std::out_of_range& e) {
        fIsValid = false;
        msg = "The found energy '" + sstr + "' which is out-of-range.";
        OnError(msg.c_str());
      } catch(...) {
        fIsValid = false;
        msg = "The found energy '" + sstr + "' which generates an error.";
        OnError(msg.c_str());
      }
      if (pos != sstr.length()) {
        fIsValid = false;
        msg = "The found energy '" + sstr + "' which is not an integer";
        OnError(msg.c_str());
      }
      fTrimSpDataEnergyList.push_back(ival);
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
void PXmlRgeHandler::OnComment(const Char_t *str)
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
void PXmlRgeHandler::OnWarning(const Char_t *str)
{
  std::cerr << std::endl << "PXmlRgeHandler **WARNING** " << str;
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
void PXmlRgeHandler::OnError(const Char_t *str)
{
  std::cerr << std::endl << "PXmlRgeHandler **ERROR** " << str;
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
void PXmlRgeHandler::OnFatalError(const Char_t *str)
{
  std::cerr << std::endl << "PXmlRgeHandler **FATAL ERROR** " << str;
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
void PXmlRgeHandler::OnCdataBlock(const Char_t *str, Int_t len)
{
  // nothing to be done for now
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImp(PRgeHandler)

//--------------------------------------------------------------------------
// Ctor
//--------------------------------------------------------------------------
/**
 * @brief PRgeHandler::PRgeHandler
 * @param fln
 */
PRgeHandler::PRgeHandler(const std::string fln)
{
  // make sure there is an xml-startup file name
  if (fln.empty()) { // fln not given
    std::cerr << std::endl;
    std::cerr << "**ERROR** NO xml file name provided." << std::endl;
    std::cerr << std::endl;
    fValid=false;
    return;
  }

  // read the startup xml-file to extract the necessary rge infos.
  if (!boost::filesystem::exists(fln)) {
    std::cerr << std::endl;
    std::cerr << "**ERROR** xml file named: " << fln << " does not exist." << std::endl;
    std::cerr << std::endl;
    fValid=false;
    return;
  }

  // create the rge xml handler
  PXmlRgeHandler *xmlRge = new PXmlRgeHandler();
  if (xmlRge == nullptr) {
    std::cerr << std::endl;
    std::cerr << "**ERROR** couldn't invoke PXmlRgeHandler." << std::endl;
    std::cerr << std::endl;
    fValid=false;
    return;
  }

  // create the SAX parser
  TSAXParser *saxParser = new TSAXParser();
  if (saxParser == nullptr) {
    std::cerr << std::endl;
    std::cerr << "**ERROR** couldn't invoke TSAXParser." << std::endl;
    std::cerr << std::endl;
    fValid=false;
    return;
  }
  saxParser->SetStopOnError();

  // connect SAX parser and rge handler
  saxParser->ConnectToHandler("PXmlRgeHandler", xmlRge);
  int status = saxParser->ParseFile(fln.c_str());
  if (status != 0) {
    std::cerr << std::endl;
    std::cerr << "**ERROR** parsing the xml-file: " << fln << "." << std::endl;
    std::cerr << std::endl;
    fValid=false;
    return;
  }
  if (xmlRge->IsValid())
    fValid = true;
  else
    fValid = false;

  // read the rge-file(s) content if everything went fine so far
  std::string rgeFln;
  PRgeData dataSet;
  if (fValid) {
    const PIntVector energy = xmlRge->GetTrimSpDataVectorList();
    for (int i=0; i<energy.size(); i++) {
      // construct the file name
      rgeFln = xmlRge->GetTrimSpDataPath();
      if (rgeFln[rgeFln.size()-1] != '/')
        rgeFln += "/";
      rgeFln += xmlRge->GetTrimSpFlnPre();
      rgeFln += std::to_string(energy[i]);
      rgeFln += ".rge";
      if (!boost::filesystem::exists(rgeFln)) {
        fValid = false;
        std::cerr << std::endl;
        std::cerr << "**ERROR** rge-file: " << rgeFln << " not found." << std::endl;
        std::cerr << std::endl;
        break;
      }
      // init data set
      dataSet.energy = energy[i];
      dataSet.depth.clear();
      dataSet.amplitude.clear();
      if (!ReadRgeFile(rgeFln, dataSet)) {
        fValid = false;
        std::cerr << std::endl;
        std::cerr << "**ERROR** read error in rge-file: " << rgeFln << " not found." << std::endl;
        std::cerr << std::endl;
        break;
      }

      // get the total number of particles
      double tot=0.0;
      for (int j=0; j<dataSet.amplitude.size(); j++)
        tot += dataSet.amplitude[j];
      dataSet.noOfParticles = tot;

      // sum_j nn dzz == 1, (dzz_j = depth[j]-depth[j-1])
      dataSet.nn.resize(dataSet.amplitude.size());
      tot = 0.0;
      double zz=0.0; // "previous" zz (since depth[j]-depth[j-1] != const) it needs to be done this way.
      for (int j=0; j<dataSet.nn.size(); j++) {
        tot += dataSet.amplitude[j] * (dataSet.depth[j] - zz);
        zz = dataSet.depth[j];
      }
      for (int j=0; j<dataSet.nn.size(); j++) {
        dataSet.nn[j] = dataSet.amplitude[j] / tot;
      }

      fData.push_back(dataSet);
    }
  }

  delete saxParser;
  delete xmlRge;
}

//--------------------------------------------------------------------------
// ReadRgeFile
//--------------------------------------------------------------------------
/**
 * <p>Read the content of a rge-file.
 *
 * @param fln file name of the rge-file
 * @return true on success.
 */
bool PRgeHandler::ReadRgeFile(const std::string fln, PRgeData &data)
{
  std::ifstream fin(fln);
  if (!fin.is_open())
    return false;

  std::string line, msg;
  std::vector<std::string> tok;
  Double_t zz, nn;
  size_t pos;
  int lineNo=0;

  while (fin.good() && fValid) {
    std::getline(fin, line);
    lineNo++;
    boost::algorithm::trim(line);
    if (line.empty())
      continue;
    if (!std::isdigit(line[0]))
      continue;
    tok.clear();
    boost::algorithm::split(tok, line, boost::algorithm::is_any_of(" \t"), boost::algorithm::token_compress_on);
    if (tok.size() != 2) {
      std::cerr << std::endl;
      std::cerr << "**ERROR** in rge-file: " << fln << ", unexpected number of tokens (" << tok.size() << ")." << std::endl;
      std::cerr << std::endl;
      fin.close();
      return false;
    }
    // check distance
    try {
      zz = std::stod(tok[0], &pos);
    } catch(std::invalid_argument& e) {
      fValid = false;
      msg = "The found depth '" + tok[0] + "' which is not a number";
      std::cerr << std::endl;
      std::cerr << "**ERROR** in rge-file: " << fln << ": lineNo: " << lineNo << std::endl;
      std::cerr << "   " << msg << std::endl;
      std::cerr << std::endl;
    } catch(std::out_of_range& e) {
      fValid = false;
      msg = "The found depth '" + tok[0] + "' which is out-of-range.";
      std::cerr << std::endl;
      std::cerr << "**ERROR** in rge-file: " << fln << ": lineNo: " << lineNo << std::endl;
      std::cerr << "   " << msg << std::endl;
      std::cerr << std::endl;
    } catch(...) {
      fValid = false;
      msg = "The found depth '" + tok[0] + "' which generates an error.";
      std::cerr << std::endl;
      std::cerr << "**ERROR** in rge-file: " << fln << ": lineNo: " << lineNo << std::endl;
      std::cerr << "   " << msg << std::endl;
      std::cerr << std::endl;
    }
    if (pos != tok[0].length()) {
      fValid = false;
      msg = "The found depth '" + tok[0] + "' which is not an number.";
      std::cerr << std::endl;
      std::cerr << "**ERROR** in rge-file: " << fln << ": lineNo: " << lineNo << std::endl;
      std::cerr << "   " << msg << std::endl;
      std::cerr << std::endl;
    }
    // check number of implanted particles
    try {
      nn = std::stod(tok[1], &pos);
    } catch(std::invalid_argument& e) {
      fValid = false;
      msg = "The found #particles '" + tok[1] + "' which is not a number";
      std::cerr << std::endl;
      std::cerr << "**ERROR** in rge-file: " << fln << ": lineNo: " << lineNo << std::endl;
      std::cerr << "   " << msg << std::endl;
      std::cerr << std::endl;
    } catch(std::out_of_range& e) {
      fValid = false;
      msg = "The found #particles '" + tok[1] + "' which is out-of-range.";
      std::cerr << std::endl;
      std::cerr << "**ERROR** in rge-file: " << fln << ": lineNo: " << lineNo << std::endl;
      std::cerr << "   " << msg << std::endl;
      std::cerr << std::endl;
    } catch(...) {
      fValid = false;
      msg = "The found #particles '" + tok[1] + "' which generates an error.";
      std::cerr << std::endl;
      std::cerr << "**ERROR** in rge-file: " << fln << ": lineNo: " << lineNo << std::endl;
      std::cerr << "   " << msg << std::endl;
      std::cerr << std::endl;
    }
    if (pos != tok[1].length()) {
      fValid = false;
      msg = "The found #particles '" + tok[1] + "' which is not an integer.";
      std::cerr << std::endl;
      std::cerr << "**ERROR** in rge-file: " << fln << ": lineNo: " << lineNo << std::endl;
      std::cerr << "   " << msg << std::endl;
      std::cerr << std::endl;
    }
    data.depth.push_back(zz/10.0); // distance in nm
    data.amplitude.push_back(nn);
  }

  fin.close();

  return true;
}

//--------------------------------------------------------------------------
// GetZmax via energy
//--------------------------------------------------------------------------
/**
 * <p>Get maximal depth for a given energy.
 *
 * @param energy energy in (eV)
 * @return zMax if energy is found, -1 otherwise.
 */
Double_t PRgeHandler::GetZmax(const Double_t energy)
{
  int idx=-1;
  for (int i=0; i<fData.size(); i++) {
    if (fabs(fData[i].energy-energy) < 1.0) {
      idx = i;
      break;
    }
  }
  if (idx != -1)
    return GetZmax(idx);

  return -1.0;
}

//--------------------------------------------------------------------------
// GetZmax via index
//--------------------------------------------------------------------------
/**
 * <p>Get maximal depth for a given index.
 *
 * @param idx index for which zMax is requested.
 * @return zMax if idx is in range, -1 otherwise.
 */
Double_t PRgeHandler::GetZmax(const Int_t idx)
{
  if ((idx < 0) || (idx >= fData.size()))
    return -1.0;

  return fData[idx].depth[fData[idx].depth.size()-1];
}

//--------------------------------------------------------------------------
// Get_n via energy
//--------------------------------------------------------------------------
/**
 * <p>Get the normalized n(E,z) value.
 *
 * @param energy (eV)
 * @param z (nm)
 * @return n(E,z) if energy and z are in proper range, -1.0 otherwise.
 */
Double_t PRgeHandler::Get_n(const Double_t energy, const Double_t z)
{
  int idx=-1;
  for (int i=0; i<fData.size(); i++) {
    if (fabs(fData[i].energy-energy) < 1.0) {
      idx = i;
      break;
    }
  }
  if (idx != -1)
    return Get_n(idx, z);

  return -1.0;
}

//--------------------------------------------------------------------------
// Get_n via index
//--------------------------------------------------------------------------
/**
 * <p>Get the normalized n(idx,z) value.
 *
 * @param idx index of the rge-dataset
 * @param z (nm)
 * @return n(idx,z) if idx and z are in proper range, -1.0 otherwise.
 */
Double_t PRgeHandler::Get_n(const Int_t idx, const Double_t z)
{
  if ((idx < 0) || (idx >= fData.size()))
    return -1.0;

  if ((z < 0.0) || (z > GetZmax(idx)))
    return 0.0;

  int pos=0;
  for (int i=0; i<fData[idx].depth.size(); i++) {
    if (z <= fData[idx].depth[i]) {
      pos = i-1;
      break;
    }
  }

  Double_t nn=0.0;
  if (pos < 0) {
    nn = 0.0;
  } else { // linear interpolation
    nn = fData[idx].nn[pos] +
         (fData[idx].nn[pos+1] - fData[idx].nn[pos]) *
         (z-fData[idx].depth[pos])/(fData[idx].depth[pos+1]-fData[idx].depth[pos]);
  }

  return nn;
}

//--------------------------------------------------------------------------
// GetEnergyIndex
//--------------------------------------------------------------------------
/**
 * <p>Get the energy index by providing an energy in (eV).
 *
 * @param energy in (eV).
 * @return energy index if energy was found, -1 otherwise.
 */
Int_t PRgeHandler::GetEnergyIndex(const Double_t energy)
{
  int idx=-1;
  for (int i=0; i<fData.size(); i++) {
    if (fabs(fData[i].energy-energy) < 1.0) {
      idx = i;
      break;
    }
  }

  return idx;
}
