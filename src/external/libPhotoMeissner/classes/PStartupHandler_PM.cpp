/***************************************************************************

  PStartupHandler_PM.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013 by Andreas Suter                                   *
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

#include <cmath>

#include <iostream>
#include <fstream>
using namespace std;

#include "PStartupHandler_PM.h"


//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PRgeHandler_PM::PRgeHandler_PM(const PStringVector &rgeDataPathList, const PDoubleVector &rgeDataEnergyList)
{
  fIsValid = false;

  fIsValid = LoadRgeData(rgeDataPathList, rgeDataEnergyList);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PRgeHandler_PM::~PRgeHandler_PM()
{
  fRgeDataList.clear();
}

//--------------------------------------------------------------------------
// GetRgeEnergyIndex
//--------------------------------------------------------------------------
/**
 *
 * \param energy in (keV)
 */
Int_t PRgeHandler_PM::GetRgeEnergyIndex(const Double_t energy)
{
  Int_t idx = -1;

  for (UInt_t i=0; i<fRgeDataList.size(); i++) {
    if (energy == fRgeDataList[i].energy) {
      idx = i;
      break;
    }
  }

  return idx;
}

//--------------------------------------------------------------------------
// GetRgeValue
//--------------------------------------------------------------------------
/**
 *
 * \param energy in (keV)
 * \param dist in (nm)
 */
Double_t PRgeHandler_PM::GetRgeValue(const Int_t index, const Double_t dist)
{
  Double_t rgeVal = 0.0;

  UInt_t distIdx = (UInt_t)(dist/(fRgeDataList[index].stoppingDistance[1]-fRgeDataList[index].stoppingDistance[0]));

  if ((distIdx >= fRgeDataList[index].stoppingDistance.size()) || (distIdx < 0)) {
    rgeVal = 0.0;
  } else {
    rgeVal = fRgeDataList[index].stoppingAmplitude[distIdx] +
             (fRgeDataList[index].stoppingAmplitude[distIdx+1] - fRgeDataList[index].stoppingAmplitude[distIdx]) *
             (dist-fRgeDataList[index].stoppingDistance[distIdx])/(fRgeDataList[index].stoppingDistance[distIdx+1]-fRgeDataList[index].stoppingDistance[distIdx]);
  }

  return rgeVal;
}

//--------------------------------------------------------------------------
// GetRgeValue
//--------------------------------------------------------------------------
/**
 *
 * \param energy in (keV)
 * \param dist in (nm)
 */
Double_t PRgeHandler_PM::GetRgeValue(const Double_t energy, const Double_t dist)
{
  // check if energy is present in rge data list
  Int_t idx = -1;

  for (UInt_t i=0; i<fRgeDataList.size(); i++) {
    if (energy == fRgeDataList[i].energy) {
      idx = i;
      break;
    }
  }

  // energy not found
  if (idx == -1)
    return -1.0;

  return GetRgeValue(idx, dist);
}

//--------------------------------------------------------------------------
// LoadRgeData
//--------------------------------------------------------------------------
/**
 *
 */
Bool_t PRgeHandler_PM::LoadRgeData(const PStringVector &rgeDataPathList, const PDoubleVector &rgeDataEnergyList)
{
  ifstream fin;
  PRgeData_PM data;
  Int_t idx=0;
  TString dataName, tstr;
  char line[512];
  int result;
  double dist, val;

  for (UInt_t i=0; i<rgeDataPathList.size(); i++) {
    // open rge-file for reading
    fin.open(rgeDataPathList[i].Data(), iostream::in);
    if (!fin.is_open()) {
      cerr << endl << "PRgeHandler_PM::LoadRgeData **ERROR**";
      cerr << endl << "  Could not open file " << rgeDataPathList[i].Data();
      cerr << endl;
      return false;
    }

    // keep energy (in keV)
    data.energy = rgeDataEnergyList[i]/1000.0;

    // read msr-file
    idx = 0;
    while (!fin.eof()) {
      // read a line
      fin.getline(line, sizeof(line));
      idx++;

      // ignore first line
      if (idx <= 1)
        continue;

      // ignore empty lines
      tstr = line;
      if (tstr.IsWhitespace())
        continue;


      // get values
      result = sscanf(line, "%lf %lf", &dist, &val);
      // check if data are valid
      if (result != 2) {
        fin.close();
        return false;
      }

      // feed fRgeDataList
      data.stoppingDistance.push_back(dist/10.0); // keep distancies in (nm)
      data.stoppingAmplitude.push_back(val);
    }

    // normalize stopping distribution
    Double_t norm = 0.0;
    for (UInt_t j=0; j<data.stoppingAmplitude.size(); j++)
      norm += data.stoppingAmplitude[j];
    norm *= (data.stoppingDistance[1] - data.stoppingDistance[0]);
    for (UInt_t j=0; j<data.stoppingAmplitude.size(); j++)
      data.stoppingAmplitude[j] /= norm;

    // keep data
    fRgeDataList.push_back(data);

    // clean up
    data.stoppingAmplitude.clear();
    data.stoppingDistance.clear();

    fin.close();
  }

  return true;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImpQ(PStartupHandler_PM)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PStartupHandler_PM::PStartupHandler_PM()
{
  fIsValid = true;

  fStartupFileFound = false;
  fStartupFilePath = "";

  // get default path (for the moment only linux like)
  char startup_path_name[128];

  // check if the startup file is found in the current directory
  strcpy(startup_path_name, "./photoMeissner_startup.xml");
  if (StartupFileExists(startup_path_name)) {
    fStartupFileFound = true;
    fStartupFilePath = TString(startup_path_name);
  } else { // startup file is not found in the current directory
    cout << endl << ">> PStartupHandler_PM(): **WARNING** Couldn't find photoMeissner_startup.xml in the current directory, will try default one." << endl;
    strncpy(startup_path_name, "/home/nemu/analysis/musrfit/src/external/libPhotoMeissner/test/photoMeissner_startup.xml", sizeof(startup_path_name));
    if (StartupFileExists(startup_path_name)) {
      fStartupFileFound = true;
      fStartupFilePath = TString(startup_path_name);
    }
  }

  // init RGE handler
  fRgeHandler = 0;
}

//--------------------------------------------------------------------------
// OnStartDocument
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PStartupHandler_PM::OnStartDocument()
{
  fKey = eEmpty;
}

//--------------------------------------------------------------------------
// OnEndDocument
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PStartupHandler_PM::OnEndDocument()
{
  if (!fIsValid)
    return;

  // generate the file path list
  TString str;
  for (unsigned int i=0; i<fRgeDataEnergyList.size(); i++) {
    str  = fRgePath;
    str += fRgeDataEnergyList[i];
    str += ".rge";
    fRgeFilePathList.push_back(str);
  }


  fRgeHandler = new PRgeHandler_PM(fRgeFilePathList, fRgeDataEnergyList);
  if (fRgeHandler == 0) { // severe problem
    fIsValid = false;
    cerr << endl << ">> PStartupHandler_PM::OnEndDocument(): **ERROR** couldn't invoke RGE handler." << endl << endl;
    return;
  }

  if (!fRgeHandler->IsValid()) { // severe problem
    fIsValid = false;
    cerr << endl << ">> PStartupHandler_PM::OnEndDocument(): **ERROR** RGE handler not valid." << endl << endl;
    return;
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
void PStartupHandler_PM::OnStartElement(const char *str, const TList *attributes)
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
void PStartupHandler_PM::OnEndElement(const char *str)
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
void PStartupHandler_PM::OnCharacters(const char *str)
{
  TString tstr;
  Double_t dval;

  switch (fKey) {
    case eDataPath:
      fRgePath = str;
      break;
    case eEnergy:
      tstr = str;
      if (tstr.IsFloat()) {
        dval = tstr.Atof();
        fRgeDataEnergyList.push_back(dval);
      } else {
        cerr << endl << "PStartupHandler_PM::OnCharacters: **ERROR** when finding energy:";
        cerr << endl << "\"" << str << "\" is not a floating point number, will ignore it.";
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
 * <p>
 *
 * \param str
 */
void PStartupHandler_PM::OnComment(const char *str)
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
void PStartupHandler_PM::OnWarning(const char *str)
{
  cerr << endl << "PStartupHandler_PM **WARNING** " << str;
  cerr << endl;
}

//--------------------------------------------------------------------------
// OnError
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 */
void PStartupHandler_PM::OnError(const char *str)
{
  cerr << endl << "PStartupHandler_PM **ERROR** " << str;
  cerr << endl;
}

//--------------------------------------------------------------------------
// OnFatalError
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 */
void PStartupHandler_PM::OnFatalError(const char *str)
{
  cerr << endl << "PStartupHandler_PM **FATAL ERROR** " << str;
  cerr << endl;
}

//--------------------------------------------------------------------------
// OnCdataBlock
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param str
 */
void PStartupHandler_PM::OnCdataBlock(const char *str, Int_t len)
{
  // nothing to be done for now
}

//--------------------------------------------------------------------------
// GetRgeEnergyIndex
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param energy
 */
Int_t PStartupHandler_PM::GetRgeEnergyIndex(const Double_t energy)
{
  Int_t result = -1;
  if (fIsValid)
    result = fRgeHandler->GetRgeEnergyIndex(energy);
  return result;
}

//--------------------------------------------------------------------------
// GetRgeValue
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param energy
 * \param dist
 */
Double_t PStartupHandler_PM::GetRgeValue(const Double_t energy, const Double_t dist)
{
  Double_t result = 0.0;
  if (fIsValid)
    result = fRgeHandler->GetRgeValue(energy, dist);
  return result;
}

//--------------------------------------------------------------------------
// GetRgeValue
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param index
 * \param dist
 */
Double_t PStartupHandler_PM::GetRgeValue(const Int_t index, const Double_t dist)
{
  Double_t result = 0.0;
  if (fIsValid)
    result = fRgeHandler->GetRgeValue(index, dist);
  return result;
}

//--------------------------------------------------------------------------
// StartupFileExists
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
bool PStartupHandler_PM::StartupFileExists(char *fln)
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
