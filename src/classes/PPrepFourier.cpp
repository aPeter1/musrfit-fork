/***************************************************************************

  PPrepFourier.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2016 by Andreas Suter                              *
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
PPrepFourier::PPrepFourier(const Int_t packing, const Int_t *bkgRange, PDoubleVector bkg) :
  fPacking(packing)
{
  SetBkgRange(bkgRange);
  SetBkg(bkg);
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
 * <p>set the background range.
 *
 * \param bkgRange array with background range
 */
void PPrepFourier::SetBkgRange(const Int_t *bkgRange)
{
  Int_t err=0;
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
// SetBkgRange
//--------------------------------------------------------------------------
/**
 * <p>set the background values for all the histos.
 *
 * \param bkg vector
 */
void PPrepFourier::SetBkg(PDoubleVector bkg)
{
  for (UInt_t i=0; i<bkg.size(); i++)
    fBkg.push_back(bkg[i]);
}

//--------------------------------------------------------------------------
// SetPacking
//--------------------------------------------------------------------------
/**
 * <p>set the packing for the histograms.
 *
 * \param packing number to be used.
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
 * <p>add a data-set (time domain data + meta information) to the internal
 * data vector.
 *
 * \param data set to be added
 */
void PPrepFourier::AddData(musrFT_data &data)
{
  fRawData.push_back(data);
}

//--------------------------------------------------------------------------
// DoBkgCorrection
//--------------------------------------------------------------------------
/**
 * <p>Correct the internal data sets according to a background interval given.
 */
void PPrepFourier::DoBkgCorrection()
{
  // make sure fData are already present, and if not create the necessary data sets
  if (fData.size() != fRawData.size()) {
    InitData();
  }

  // if no bkg-range is given, nothing needs to be done
  if ((fBkgRange[0] == -1) && (fBkgRange[1] == -1) && (fBkg.size() == 0)) {
    return;
  }

  if ((fBkgRange[0] != -1) && (fBkgRange[1] != -1)) { // background range is given
    // make sure that the bkg range is ok
    for (UInt_t i=0; i<fRawData.size(); i++) {
      if ((fBkgRange[0] >= (Int_t)fRawData[i].rawData.size()) || (fBkgRange[1] >= (Int_t)fRawData[i].rawData.size())) {
        cerr << endl << "PPrepFourier::DoBkgCorrection() **ERROR** bkg-range out of data-range!";
        return;
      }
    }

    Double_t bkg=0.0;
    for (UInt_t i=0; i<fRawData.size(); i++) {
      // calculate the bkg for the given range
      for (Int_t j=fBkgRange[0]; j<=fBkgRange[1]; j++) {
        bkg += fRawData[i].rawData[j];
      }
      bkg /= (fBkgRange[1]-fBkgRange[0]+1);
      cout << "info> background " << i << ": " << bkg << endl;

      // correct data
      for (UInt_t j=0; j<fData[i].size(); j++)
        fData[i][j] -= bkg;
    }
  } else { // there might be an explicit background list
    // check if there is a background list
    if (fBkg.size() == 0)
      return;

    // check if there are as many background values than data values
    if (fBkg.size() != fData.size()) {
      cerr << endl << "PPrepFourier::DoBkgCorrection() **ERROR** #bkg values != #histos. Will do nothing here." << endl;
      return;
    }

    for (UInt_t i=0; i<fData.size(); i++)
      for (UInt_t j=0; j<fData[i].size(); j++)
        fData[i][j] -= fBkg[i];
  }
}

//--------------------------------------------------------------------------
// DoPacking
//--------------------------------------------------------------------------
/**
 * <p>Rebin (pack) the internal data.
 */
void PPrepFourier::DoPacking()
{
  // make sure fData are already present, and if not create the necessary data sets
  if (fData.size() != fRawData.size()) {
    InitData();
  }

  if (fPacking == 1) // nothing to be done
    return;

  PDoubleVector tmpData;
  Double_t dval = 0.0;
  for (UInt_t i=0; i<fData.size(); i++) {
    tmpData.clear();
    dval = 0.0;
    for (UInt_t j=0; j<fData[i].size(); j++) {
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
  }
}

//--------------------------------------------------------------------------
// DoLifeTimeCorrection
//--------------------------------------------------------------------------
/**
 * <p>Try to do a muon life time correction. The idea is to estimate N0 without
 * any theory. This will be OK for high fields (> couple kGauss) but not so good
 * for low fields.
 *
 * \param fudge rescaling factor for the estimated N0. Should be around 1
 */
void PPrepFourier::DoLifeTimeCorrection(Double_t fudge)
{
  // make sure fData are already present, and if not create the necessary data sets
  if (fData.size() != fRawData.size()) {
    InitData();
  }

  // calc exp(+t/tau)*N(t), where N(t) is already background corrected
  Double_t scale;
  for (UInt_t i=0; i<fData.size(); i++) {
    scale = fRawData[i].timeResolution / PMUON_LIFETIME;
    for (UInt_t j=0; j<fData[i].size(); j++) {
      fData[i][j] *= exp(j*scale);
    }
  }

  // calc N0
  Double_t dval;
  Double_t N0;
  for (UInt_t i=0; i<fData.size(); i++) {
    dval = 0.0;
    for (UInt_t j=0; j<fData[i].size(); j++) {
      dval += fData[i][j];
    }
    N0 = dval/fData[i].size();
    N0 *= fudge;
    for (UInt_t j=0; j<fData[i].size(); j++) {
      fData[i][j] -= N0;
      fData[i][j] /= N0;
    }
  }
}

//--------------------------------------------------------------------------
// GetInfo
//--------------------------------------------------------------------------
/**
 * <p>Returns the meta information of a data set.
 *
 * \param idx index of the object
 */
TString PPrepFourier::GetInfo(const UInt_t idx)
{
  TString info("");

  if (idx < fRawData.size())
    info = fRawData[idx].info;

  return info;
}

//--------------------------------------------------------------------------
// GetDataSetTag
//--------------------------------------------------------------------------
/**
 * <p>Returns the data set tag of the object
 *
 * \param idx index of the object
 */
Int_t PPrepFourier::GetDataSetTag(const UInt_t idx)
{
  Int_t result = -1;

  if (idx < fRawData.size())
    result = fRawData[idx].dataSetTag;

  return result;
}

//--------------------------------------------------------------------------
// GetData
//--------------------------------------------------------------------------
/**
 * <p>Creates the requested TH1F objects and returns them. The ownership is with
 * the caller.
 */
vector<TH1F*> PPrepFourier::GetData()
{
  vector<TH1F*> data;
  data.resize(fData.size());

  // if not data are present, just return an empty vector
  if (fData.size() == 0)
    return data;

  TString name("");
  Double_t dt=0.0;
  Double_t start=0.0;
  Double_t end=0.0;
  UInt_t size;
  UInt_t startIdx;
  UInt_t endIdx;

  for (UInt_t i=0; i<fData.size(); i++) {
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
    for (UInt_t j=startIdx; j<endIdx; j++)
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
 * \param idx index of the requested histogram
 */
TH1F *PPrepFourier::GetData(const UInt_t idx)
{
  if (fData.size() == 0) // no data present
    return 0;

  if (idx > fData.size()) // requested index out of range
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
  for (UInt_t i=startIdx; i<endIdx; i++)
    data->SetBinContent(i-startIdx+1, fData[idx][i]);

  return data;
}

//--------------------------------------------------------------------------
// InitData (private)
//--------------------------------------------------------------------------
/**
 * <p>Copy raw-data to internal data from t0 to the size of raw-data.
 */
void PPrepFourier::InitData()
{
  fData.resize(fRawData.size());
  UInt_t t0;
  for (UInt_t i=0; i<fRawData.size(); i++) {
    if (fRawData[i].t0 >= 0)
      t0 = fRawData[i].t0;
    else
      t0 = 0;
    for (UInt_t j=t0; j<fRawData[i].rawData.size(); j++) {
      fData[i].push_back(fRawData[i].rawData[j]);
    }
  }
}
