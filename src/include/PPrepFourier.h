/***************************************************************************

  PPrepFourier.h

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

#ifndef _PPREPFOURIER_H_
#define _PPREPFOURIER_H_

#include <iostream>
#include <vector>

#include <TH1F.h>
#include <TMath.h>

#include "PMusr.h"

//----------------------------------------------------------------------------
/**
 * <p>Data structure holding raw time domain uSR data together with some
 * necessary meta information.
 */
struct musrFT_data {
  Int_t dataSetTag;        ///< tag to label the data set. Needed for average-per-data-set
  TString info;            ///< keeps all the meta information
  Double_t timeResolution; ///< time resolution in (usec)
  Int_t t0;                ///< keep the t0 bin
  Double_t timeRange[2];   ///< time range to be used, given in (usec).
  PDoubleVector rawData;   ///< a single time domain data vector
};

//----------------------------------------------------------------------------
/**
 * <p>Little helper class to prepare time-domain data for Fourier transform, without
 * theory, etc.
 */
class PPrepFourier {
  public:
    PPrepFourier();
    PPrepFourier(const Int_t packing, const Int_t *bkgRange, PDoubleVector bkg);
    virtual ~PPrepFourier();

    virtual void SetBkgRange(const Int_t *bkgRange);
    virtual void SetBkg(PDoubleVector bkg);
    virtual void SetPacking(const Int_t packing);
    virtual void AddData(musrFT_data &data);
    virtual void DoBkgCorrection();
    virtual void DoPacking();
    virtual void DoLifeTimeCorrection(Double_t fudge);

    TString GetInfo(const UInt_t idx);
    Int_t GetDataSetTag(const UInt_t idx);
    UInt_t GetNoOfData() { return fRawData.size(); }
    std::vector<TH1F*> GetData();
    TH1F *GetData(const UInt_t idx);

  private:
    std::vector<musrFT_data> fRawData;
    std::vector<PDoubleVector>fData;
    Int_t fBkgRange[2];
    PDoubleVector fBkg;
    Int_t fPacking;

    virtual void InitData();
};

#endif // _PPREPFOURIER_H_

