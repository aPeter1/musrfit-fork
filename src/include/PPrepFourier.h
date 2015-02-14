/***************************************************************************

  PPrepFourier.h

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

#ifndef _PPREPFOURIER_H_
#define _PPREPFOURIER_H_

#include <iostream>
#include <vector>
using namespace std;

#include "TH1F.h"

#include "PMusr.h"

//----------------------------------------------------------------------------
/**
 * <p>Data structure holding raw time domain uSR data together with some
 * necessary meta information.
 */
typedef struct {
  TString info;          ///< keeps all the meta information
  double timeResolution; ///< time resolution in (usec)
  int t0;                ///< keep the t0 bin
  Double_t timeRange[2]; ///< time range to be used, given in (usec).
  PDoubleVector rawData; ///< a single time domain data vector
} musrFT_data;

//----------------------------------------------------------------------------
/**
 * <p>Little helper class to prepare time-domain data for Fourier transform, without
 * theory, etc.
 */
class PPrepFourier {
  public:
    PPrepFourier();
    PPrepFourier(const Int_t *bkgRange, const Int_t packing);
    virtual ~PPrepFourier();

    void SetBkgRange(const Int_t *bkgRange);
    void SetPacking(const Int_t packing);
    void AddData(musrFT_data &data);
    void DoBkgCorrection();
    void DoPacking();
    void DoFiltering();
    void DoLifeTimeCorrection(Double_t fudge);

    TString GetInfo(const UInt_t idx);
    UInt_t GetNoOfData() { return fRawData.size(); }
    vector<TH1F*> GetData();
    TH1F *GetData(const UInt_t idx);

  private:
    vector<musrFT_data> fRawData;
    vector<PDoubleVector>fData;
    Int_t fBkgRange[2];
    Int_t fPacking;

    void InitData();
};

#endif // _PPREPFOURIER_H_

