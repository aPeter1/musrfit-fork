/***************************************************************************

  PPrepFourier.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2015 by Andreas Suter                              *
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

#include "PPrepFourier.h"

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 */
PPrepFourier::PPrepFourier()
{
  fBkgRange[0] = -1;
  fBkgRange[1] = -1;
  fPacking = 1;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 */
PPrepFourier::PPrepFourier(const Int_t *bkgRange, const Int_t packing) :
  fPacking(packing)
{
  SetBkgRange(bkgRange);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor.
 */
PPrepFourier::~PPrepFourier()
{
  fRawData.clear();
  fData.clear();
}

//--------------------------------------------------------------------------
// SetBkgRange
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param bkgRange
 */
void PPrepFourier::SetBkgRange(const Int_t *bkgRange)
{
  cout << endl << "debug> bkgRange: " << bkgRange[0] << ", " << bkgRange[1] << endl;

  int err=0;
  if (bkgRange[0] >= -1) {
    fBkgRange[0] = bkgRange[0];
  } else {
    err = 1;
  }
  if (bkgRange[1] >= -1) {
    fBkgRange[1] = bkgRange[1];
  } else {
    if (err == 0)
      err = 2;
    else
      err = 3;
  }

  TString errMsg("");
  switch (err) {
    case 1:
    errMsg = TString::Format("start bkg range < 0 (given: %d), will ignore it.", bkgRange[0]);
      break;
    case 2:
      errMsg = TString::Format("end bkg range < 0 (given: %d), will ignore it.", bkgRange[1]);
      break;
    case 3:
      errMsg = TString::Format("start/end bkg range < 0 (given: %d/%d), will ignore it.", bkgRange[0], bkgRange[1]);
      break;
    default:
      errMsg = TString("??");
      break;
  }

  if (err != 0) {
    cerr << endl << ">> PPrepFourier::SetBkgRange: **WARNING** " << errMsg << endl;
  }
}

//--------------------------------------------------------------------------
// SetPacking
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param packing
 */
void PPrepFourier::SetPacking(const Int_t packing)
{
  if (packing > 0) {
    fPacking = packing;
  } else {
    cerr << endl << ">> PPrepFourier::SetPacking: **WARNING** found packing=" << packing << " < 0, will ignore it." << endl;
  }
}

//--------------------------------------------------------------------------
// AddData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param data
 */
void PPrepFourier::AddData(musrFT_data &data)
{
  fRawData.push_back(data);
}

//--------------------------------------------------------------------------
// DoBkgCorrection
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PPrepFourier::DoBkgCorrection()
{
  cout << endl << "debug> DoBkgCorrection ...";

  // if no bkg-range is given, nothing needs to be done
  if ((fBkgRange[0] == -1) && (fBkgRange[1] == -1)) {
    cout << endl << "debug> no background range given ...";
    return;
  }

  // make sure that the bkg range is ok
  for (unsigned int i=0; i<fRawData.size(); i++) {
    if ((fBkgRange[0] >= fRawData[i].rawData.size()) || (fBkgRange[1] >= fRawData[i].rawData.size())) {
      cerr << endl << "PPrepFourier::DoBkgCorrection() **ERROR** bkg-range out of data-range!";
      return;
    }
  }

  // make sure fData are already present, and if not create the necessary data sets
  if (fData.size() != fRawData.size()) {
    InitData();
  }

  Double_t bkg=0.0;
  for (unsigned int i=0; i<fRawData.size(); i++) {
    // calculate the bkg for the given range
    for (int j=fBkgRange[0]; j<=fBkgRange[1]; j++) {
      bkg += fRawData[i].rawData[j];
    }
    bkg /= (fBkgRange[1]-fBkgRange[0]+1);
    cout << endl << "debug> histo: " << i << ", bkg=" << bkg;

    // correct data
    for (unsigned int j=0; j<fData[i].size(); j++)
      fData[i][j] -= bkg;
  }
}

//--------------------------------------------------------------------------
// DoPacking
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PPrepFourier::DoPacking()
{
  cout << endl << "debug> DoPacking ...";

  if (fPacking == 1) // nothing to be done
    return;

  PDoubleVector tmpData;
  Double_t dval = 0.0;
  for (unsigned int i=0; i<fData.size(); i++) {
    tmpData.clear();
    dval = 0.0;
    for (unsigned int j=0; j<fData[i].size(); j++) {
      if ((j % fPacking == 0) && (j != 0)) {
        tmpData.push_back(dval);
        dval = 0.0;
      } else {
        dval += fData[i][j];
      }
    }
    // change the original data set with the packed one
    fData[i].clear();
    fData[i] = tmpData;

    cout << endl << "debug> histo " << i+1 << ": packed data: ";
    for (unsigned int j=0; j<15; j++)
      cout << fData[i][j] << ", ";
  }
}

//--------------------------------------------------------------------------
// DoFiltering
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PPrepFourier::DoFiltering()
{
  cout << endl << "debug> DoFiltering not yet implemented ...";
}

//--------------------------------------------------------------------------
// GetInfo
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param idx
 */
TString PPrepFourier::GetInfo(const UInt_t idx)
{
  TString info("");

  if (idx < fRawData.size())
    info = fRawData[idx].info;

  return info;
}

//--------------------------------------------------------------------------
// GetData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
vector<TH1F*> PPrepFourier::GetData()
{
  vector<TH1F*> data;
  data.resize(fData.size());

  TString name("");
  Double_t dt=0.0;
  Double_t start=0.0;
  Double_t end=0.0;
  UInt_t size;
  UInt_t startIdx;
  UInt_t endIdx;

  for (unsigned int i=0; i<fData.size(); i++) {
    name = TString::Format("histo%2d", i);
    dt = fRawData[i].timeResolution*fPacking;
    start = fRawData[i].timeRange[0];
    end = fRawData[i].timeRange[1];

    // init size and start/end indices
    size = fData[i].size();
    startIdx = 1;
    endIdx = size;

    // time range given, hence calculate the proper size
    if (start != -1.0) {
      size = (UInt_t)((end-start)/dt);
      if (start >= 0.0) {
        startIdx = (UInt_t)(start/dt)+1;
        endIdx = (UInt_t)(end/dt)+1;
      } else {
        cerr << endl << ">> PPrepFourier::GetData **WARNING** found start time < 0.0, will set it to 0.0" << endl;
        endIdx = static_cast<UInt_t>(end/dt)+1;
      }
    }

    if (start == -1.0) { // no time range given, hence start == 0.0 - dt/2
      start = -dt/2.0;
    } else { // time range given
      start -= dt/2.0;
    }

    if (end == -1.0) { // no time range given, hence end == (fData[idx].size()-1)*dt + dt/2
      end = (fData[i].size()-1)*dt+dt/2.0;
    } else { // time range given
      end += dt/2.0;
    }

    data[i] = new TH1F(name.Data(), fRawData[i].info.Data(), size, start, end);
    for (unsigned int j=startIdx; j<endIdx; j++)
      data[i]->SetBinContent(j-startIdx+1, fData[i][j]);
  }

  return data;
}

//--------------------------------------------------------------------------
// GetData
//--------------------------------------------------------------------------
/**
 * <p>Creates the requested TH1F object and returns it. The ownership is with
 * the caller.
 *
 * \param idx
 */
TH1F *PPrepFourier::GetData(const UInt_t idx)
{
  if (idx > fData.size())
    return 0;

  TString name = TString::Format("histo%2d", idx);
  Double_t dt = fRawData[idx].timeResolution*fPacking;
  Double_t start = fRawData[idx].timeRange[0];
  Double_t end = fRawData[idx].timeRange[1];
  UInt_t size = fData[idx].size();
  UInt_t startIdx = 1;
  UInt_t endIdx = size;

  // time range given, hence calculate the proper size
  if (start != -1.0) {
    size = (UInt_t)((end-start)/dt);
    if (start >= 0.0) {
      startIdx = (UInt_t)(start/dt)+1;
      endIdx = (UInt_t)(end/dt)+1;
    } else {
      cerr << endl << ">> PPrepFourier::GetData **WARNING** found start time < 0.0, will set it to 0.0" << endl;
      endIdx = (UInt_t)(end/dt)+1;
    }
  }

  if (start == -1.0) { // no time range given, hence start == 0.0 - dt/2
    start = -dt/2.0;
  } else { // time range given
    start -= dt/2.0;
  }

  if (end == -1.0) { // no time range given, hence end == (fData[idx].size()-1)*dt + dt/2
    end = (fData[idx].size()-1)*dt+dt/2.0;
  } else { // time range given
    end += dt/2.0;
  }

  TH1F *data = new TH1F(name.Data(), fRawData[idx].info.Data(), size, start, end);
  for (unsigned int i=startIdx; i<endIdx; i++)
    data->SetBinContent(i-startIdx+1, fData[idx][i]);

  return data;
}

//--------------------------------------------------------------------------
// InitData
//--------------------------------------------------------------------------
/**
 * <p>Copy raw-data to internal data from t0 to the size of raw-data.
 */
void PPrepFourier::InitData()
{
  fData.resize(fRawData.size());
  for (unsigned int i=0; i<fRawData.size(); i++) {
    for (unsigned int j=fRawData[i].t0; j<fRawData[i].rawData.size(); j++) {
      fData[i].push_back(fRawData[i].rawData[j]);
    }
  }
}
