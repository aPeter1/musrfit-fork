/***************************************************************************

  PMusrT0.h

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

#ifndef _PMUSRT0_H_
#define _PMUSRT0_H_

#include <TObject.h>
#include <TQObject.h>
#include <TStyle.h>
#include <TRootCanvas.h>
#include <TGMenu.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TLine.h>
#include <TLatex.h>

#include "PMusr.h"
#ifndef __MAKECINT__
#include "PMsrHandler.h"
#endif // __MAKECINT__

#define PMUSRT0_FORWARD  0
#define PMUSRT0_BACKWARD 1

#define PMUSRT0_GET_T0                    0
#define PMUSRT0_GET_DATA_AND_BKG_RANGE    1
#define PMUSRT0_GET_T0_DATA_AND_BKG_RANGE 2

/*
#define PMUSRT0_NOADDRUN_NOGROUPING 0
#define PMUSRT0_ADDRUN_NOGROUPING   1
#define PMUSRT0_NOADDRUN_GROUPING   2
#define PMUSRT0_ADDRUN_GROUPING     3
*/

class PMusrT0Data {
  public:
    PMusrT0Data();
    virtual ~PMusrT0Data();


    virtual void InitData();

    virtual Bool_t IsSingleHisto() { return fSingleHisto; }
    virtual UInt_t GetRawRunDataSize() { return fRawRunData.size(); }
    virtual PRawRunData* GetRawRunData(Int_t idx);
    virtual Int_t GetRunNo() { return fRunNo; }
    virtual Int_t GetAddRunIdx() { return fAddRunIdx; }
    virtual Int_t GetHistoNoIdx() { return fHistoNoIdx; }
    virtual UInt_t GetHistoNoSize() { return fHistoNo.size(); }
    virtual Int_t GetHistoNo(UInt_t idx);
    virtual Int_t GetDetectorTag() { return fDetectorTag; }
    virtual Int_t GetCmdTag() { return fCmdTag; }
    virtual UInt_t GetT0Size() { return fT0.size(); }
    virtual Int_t GetT0(UInt_t idx);
    virtual UInt_t GetAddT0Entries() { return fAddT0.size(); }
    virtual UInt_t GetAddT0Size(UInt_t idx);
    virtual Int_t GetAddT0(UInt_t addRunIdx, UInt_t idx);

    virtual void SetSingleHisto(const Bool_t flag) { fSingleHisto = flag; }
    virtual void SetRawRunData(const vector<PRawRunData*> rawRunData) { fRawRunData = rawRunData; }
    virtual void SetRunNo(const UInt_t runNo) { fRunNo = runNo; }
    virtual void SetAddRunIdx(const UInt_t addRunIdx) { fAddRunIdx = addRunIdx; }
    virtual void SetHistoNoIdx(const UInt_t histoNoIdx) { fHistoNoIdx = histoNoIdx; }
    virtual void SetHistoNo(const PIntVector histoNo) { fHistoNo = histoNo; }
    virtual void SetDetectorTag(const UInt_t detectorTag) { fDetectorTag = detectorTag; }
    virtual void SetCmdTag(const UInt_t cmdTag) { fCmdTag = cmdTag; }
    virtual void SetT0(UInt_t val, UInt_t idx);
    virtual void SetAddT0(UInt_t val, UInt_t addRunIdx, UInt_t idx);

  private:
    Bool_t fSingleHisto;              ///< true if single histo fit, false for asymmetry fit
    vector<PRawRunData*> fRawRunData; ///< holds the raw data of the needed runs, idx=0 the run, idx>0 the addruns
    Int_t fRunNo;                     ///< msr-file run number
    Int_t fAddRunIdx;                 ///< msr-file addrun index
    Int_t fHistoNoIdx;                ///< msr-file histo number index
    PIntVector fHistoNo;              ///< msr-file histo numbers
    Int_t fDetectorTag;               ///< detector tag. forward=0,backward=1
    Int_t fCmdTag;                    ///< command tag. 0=get t0, 1=get data-/bkg-range, 2=get t0, and data-/bkg-range
    PIntVector fT0;                   ///< holding the t0's of the run
    vector<PIntVector> fAddT0;        ///< holding the t0's of the addruns
};


//--------------------------------------------------------------------------
/**
 * <p>The preprocessor tag __MAKECINT__ is used to hide away from rootcint
 * the overly complex spirit header files.
 */
class PMusrT0 : public TObject, public TQObject
{
  public:
    PMusrT0();
    PMusrT0(PMusrT0Data &data);

    virtual ~PMusrT0();

    virtual Bool_t IsValid() { return fValid; }

    virtual void Done(Int_t status=0); // *SIGNAL*
    virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT

#ifndef __MAKECINT__
    virtual void SetMsrHandler(PMsrHandler *msrHandler);
#endif // __MAKECINT__

    virtual void InitT0();
    virtual void InitDataAndBkg();
    virtual Int_t GetStatus() { return fStatus; }

  private:
#ifndef __MAKECINT__
    PMsrHandler *fMsrHandler;
#endif // __MAKECINT__

    Bool_t fValid;

    Int_t fStatus;

    PMusrT0Data fMusrT0Data;

    Bool_t fDataAndBkgEnabled;
    Bool_t fT0Enabled;
    Int_t  fT0Estimated;

    // canvas related variables
    TCanvas   *fMainCanvas;

    TH1F *fHisto;
    TH1F *fData;
    TH1F *fBkg;

    TLatex *fToDoInfo;

    TLine *fT0Line;
    TLine *fFirstBkgLine;
    TLine *fLastBkgLine;
    TLine *fFirstDataLine;
    TLine *fLastDataLine;

    Int_t fPx;
    Int_t fPy;

    Int_t fDataRange[2];
    Int_t fBkgRange[2];

    void SetT0Channel();
    void SetEstimatedT0Channel();
    void SetDataFirstChannel();
    void SetDataLastChannel();
    void SetBkgFirstChannel();
    void SetBkgLastChannel();
    void UnZoom();
    void ZoomT0();

  ClassDef(PMusrT0, 1)
};

#endif // _PMUSRT0_H_
