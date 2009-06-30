/***************************************************************************

  PNL_RgeHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Andreas Suter                                   *
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
using namespace std;

#include "PNL_RgeHandler.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PNL_RgeHandler::PNL_RgeHandler(const PStringVector &rgeDataPathList)
{
  fIsValid = false;

  fIsValid = LoadRgeData(rgeDataPathList);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PNL_RgeHandler::~PNL_RgeHandler()
{
  fRgeDataList.clear();
}

//--------------------------------------------------------------------------
// GetRgeEnergyIndex
//--------------------------------------------------------------------------
/**
 *
 */
Int_t PNL_RgeHandler::GetRgeEnergyIndex(const Double_t energy)
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
 */
Double_t PNL_RgeHandler::GetRgeValue(const Int_t index, const Double_t dist)
{
  Double_t rgeVal = 0.0;

  UInt_t distIdx = (UInt_t)(dist/(fRgeDataList[index].stoppingDistance[1]-fRgeDataList[index].stoppingDistance[0]));

  if ((distIdx >= fRgeDataList[index].stoppingDistance.size()) || (distIdx < 0)) {
    rgeVal = 0.0;
  } else {
    rgeVal = fRgeDataList[index].stoppingAmplitude[distIdx] +
             (fRgeDataList[index].stoppingAmplitude[distIdx] - fRgeDataList[index].stoppingAmplitude[distIdx+1]) *
             (fRgeDataList[index].stoppingDistance[distIdx+1]-dist)/(fRgeDataList[index].stoppingDistance[distIdx+1]-fRgeDataList[index].stoppingDistance[distIdx]);
  }

  return rgeVal;
}

//--------------------------------------------------------------------------
// GetRgeValue
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PNL_RgeHandler::GetRgeValue(const Double_t energy, const Double_t dist)
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
Bool_t PNL_RgeHandler::LoadRgeData(const PStringVector &rgeDataPathList)
{
  ifstream fin;
  PNL_RgeData data;
  Int_t idx=0;
  TString dataName;
  char line[512];
  int result;
  double dist, val;

  for (UInt_t i=0; i<rgeDataPathList.size(); i++) {
    // open rge-file for reading
    fin.open(rgeDataPathList[i].Data(), iostream::in);
    if (!fin.is_open()) {
      return false;
    }

    // extract energy from rgeDataPathList name
    dataName = rgeDataPathList[i];
    // remove rge extension
    dataName.Remove(dataName.Length()-4, 4);
    // get energy part
    dataName.Replace(0, dataName.Length()-3, "");
    if (!dataName.IsDigit()) {
      fin.close();
      return false;
    }
    data.energy = dataName.Atof()/10.0;

    // read msr-file
    idx = 0;
    while (!fin.eof()) {
      // read a line
      fin.getline(line, sizeof(line));
      idx++;

      // ignore first line
      if (idx == 1)
        continue;

      // get values
      result = sscanf(line, "%lf %lf", &dist, &val);
      // check if data are valid
      if (result != 2) {
        fin.close();
        return false;
      }

      // feed fRgeDataList
      data.stoppingDistance.push_back(dist/10.0);
      data.stoppingAmplitude.push_back(val);
    }

    // normalize stopping distribution
    Double_t norm = 0.0;
    for (UInt_t j=0; j<data.stoppingAmplitude.size(); j++)
      norm += data.stoppingAmplitude[i];
    norm *= (data.stoppingDistance[1] - data.stoppingDistance[0]);
    for (UInt_t j=0; j<data.stoppingAmplitude.size(); j++)
      data.stoppingAmplitude[i] /= norm;

    // keep data
    fRgeDataList.push_back(data);

    // clean up
    data.stoppingAmplitude.clear();
    data.stoppingDistance.clear();

    fin.close();
  }

  return true;
}
