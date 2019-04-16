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

#include <cassert>

#include <iostream>
#include <fstream>

#include "PNL_RgeHandler.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PNL_RgeHandler::PNL_RgeHandler(const PStringVector &rgeDataPathList, const PDoubleVector &rgeDataEnergyList)
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

  // find the proper position index
  Int_t distIdx = -2;
  for (UInt_t i=0; i<fRgeDataList[index].stoppingDistance.size(); i++) {
    if (fRgeDataList[index].stoppingDistance[i] > dist) {
      distIdx = i-1;
      break;
    }
  }

  // get the proper n(z) value
  if (distIdx < 0) {
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
Bool_t PNL_RgeHandler::LoadRgeData(const PStringVector &rgeDataPathList, const PDoubleVector &rgeDataEnergyList)
{
  std::ifstream fin;
  PNL_RgeData data;
  TString tstr;
  char line[512];
  int result;
  double dist, val;

  for (UInt_t i=0; i<rgeDataPathList.size(); i++) {
    // open rge-file for reading
    fin.open(rgeDataPathList[i].Data(), std::iostream::in);
    if (!fin.is_open()) {
      std::cout << std::endl << "PNL_RgeHandler::LoadRgeData **ERROR**";
      std::cout << std::endl << "  Could not open file " << rgeDataPathList[i].Data();
      std::cout << std::endl;
      return false;
    }

    // keep energy (in keV)
    data.energy = rgeDataEnergyList[i]/1000.0;

    // read msr-file
    while (!fin.eof()) {
      // read a line
      fin.getline(line, sizeof(line));

      // ignore empty lines
      tstr = line;
      if (tstr.IsWhitespace())
        continue;

      // get values
      result = sscanf(line, "%lf %lf", &dist, &val);
      // check if data are valid, otherwise ignore it
      if (result != 2) {
        continue;
      }

      // feed fRgeDataList
      data.stoppingDistance.push_back(dist/10.0);
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
