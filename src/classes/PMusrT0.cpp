/***************************************************************************

  PMusrT0.cpp

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

#include <TColor.h>
#include <TRandom.h>
#include <TROOT.h>
#include <TObjString.h>

#include "PMusrT0.h"

#define DETECTOR_TAG_FORWARD  0
#define DETECTOR_TAG_BACKWARD 1

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 */
PMusrT0Data::PMusrT0Data()
{
  InitData();
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor.
 */
PMusrT0Data::~PMusrT0Data()
{
  fRawRunData.clear();
  fHistoNo.clear();
  fT0.clear();
  for (UInt_t i=0; i<fAddT0.size(); i++)
    fAddT0[i].clear();
  fAddT0.clear();
}

//--------------------------------------------------------------------------
// InitData
//--------------------------------------------------------------------------
/**
 * <p>Initialize the necessary variables.
 */
void PMusrT0Data::InitData()
{
  fSingleHisto = true;
  fRunNo       = -1;
  fAddRunIdx   = -1;
  fHistoNoIdx  = -1;
  fDetectorTag = -1;
  fCmdTag      = -1;
  fRawRunData.clear();
  fHistoNo.clear();
  fT0.clear();
  for (UInt_t i=0; i<fAddT0.size(); i++)
    fAddT0[i].clear();
  fAddT0.clear();
}

//--------------------------------------------------------------------------
// GetRawRunData
//--------------------------------------------------------------------------
/**
 * <p>Returns the raw run data set with index idx.
 *
 * <b>return:</b>
 * - raw run data set
 * - 0 pointer, if idx is out of range
 *
 * \param idx index of the raw run data
 */
PRawRunData* PMusrT0Data::GetRawRunData(Int_t idx)
{
  if ((idx < 0) || (idx >= static_cast<Int_t>(fRawRunData.size())))
    return 0;

  return fRawRunData[idx];
}

//--------------------------------------------------------------------------
// GetHistoNo
//--------------------------------------------------------------------------
/**
 * <p>Get histogram number of a run.
 *
 * <b>return:</b>
 * - histogram number
 * - -1 if index is out of range
 *
 * \param idx index of the run (msr-file).
 */
Int_t PMusrT0Data::GetHistoNo(UInt_t idx)
{
  if (idx >= fHistoNo.size())
    return -1;

  return fHistoNo[idx];
}

//--------------------------------------------------------------------------
// GetT0
//--------------------------------------------------------------------------
/**
 * <p>Get t0 (in bin) of a run.
 *
 * <b>return:</b>
 * - t0 bin
 * - -1 if index is out of range
 *
 * \param idx index of the run (msr-file).
 */
Int_t PMusrT0Data::GetT0(UInt_t idx)
{
  if (idx >= fT0.size())
    return -1;

  return fT0[idx];
}

//--------------------------------------------------------------------------
// GetAddT0Size
//--------------------------------------------------------------------------
/**
 * <p>Get addt0 size of a run, i.e. the number of addt0's for a given msr-file run.
 *
 * <b>return:</b>
 * - number of addt0's
 * - -1 if index is out of range
 *
 * \param idx index of the run (msr-file).
 */
UInt_t PMusrT0Data::GetAddT0Size(UInt_t idx)
{
  if (idx >= fAddT0.size())
    return 0;

  return fAddT0[idx].size();
}

//--------------------------------------------------------------------------
// GetAddT0
//--------------------------------------------------------------------------
/**
 * <p>Get addt0 (in bin) of a run.
 *
 * <b>return:</b>
 * - addt0 bin
 * - -1 if index is out of range
 *
 * \param addRunIdx index of the addrun
 * \param idx index of the run (msr-file).
 */
Int_t PMusrT0Data::GetAddT0(UInt_t addRunIdx, UInt_t idx)
{
  if (addRunIdx >= fAddT0.size())
    return -1;

  if (idx >= fAddT0[addRunIdx].size())
    return -1;

  return fAddT0[addRunIdx][idx];
}

//--------------------------------------------------------------------------
// SetT0
//--------------------------------------------------------------------------
/**
 * <p>Set t0 value.
 *
 * \param val t0 value to be set
 * \param idx index at which t0 shall be set.
 */
void PMusrT0Data::SetT0(UInt_t val, UInt_t idx)
{
  if (idx >= fT0.size())
    fT0.resize(idx+1);

  fT0[idx] = val;
}

//--------------------------------------------------------------------------
// SetAddT0
//--------------------------------------------------------------------------
/**
 * <p>Set addt0 value.
 *
 * \param val t0 value to be set
 * \param addRunIdx addt0 index (for each addrun, there has to be an addt0)
 * \param idx index at which t0 shall be set.
 */
void PMusrT0Data::SetAddT0(UInt_t val, UInt_t addRunIdx, UInt_t idx)
{
  if (addRunIdx >= fAddT0.size())
    fAddT0.resize(addRunIdx+1);

  if (idx >= fAddT0[addRunIdx].size())
    fAddT0[addRunIdx].resize(idx+1);

  fAddT0[addRunIdx][idx] = val;
}

//--------------------------------------------------------------------------

ClassImpQ(PMusrT0)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 */
PMusrT0::PMusrT0()
{
  fValid = false;

  fStatus = 0; // default is quit locally

  fMainCanvas = 0;

  fHisto = 0;
  fData  = 0;
  fBkg   = 0;

  fToDoInfo = 0;

  fDataAndBkgEnabled = false;
  fT0Enabled         = false;

  fDataRange[0] = 0;
  fDataRange[1] = 0;
  fBkgRange[0]  = 0;
  fBkgRange[1]  = 0;

  fT0Line        = 0;
  fFirstBkgLine  = 0;
  fLastBkgLine   = 0;
  fFirstDataLine = 0;
  fLastDataLine  = 0;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 *
 * \param data raw run data set
 */
PMusrT0::PMusrT0(PMusrT0Data &data) : fMusrT0Data(data)
{
  fValid = true;

  fStatus = 0; // default is quit locally

  fMainCanvas = 0;

  fHisto = 0;
  fData  = 0;
  fBkg   = 0;

  fToDoInfo = 0;

  fDataAndBkgEnabled = false;
  fT0Enabled         = false;

  fDataRange[0] = 0;
  fDataRange[1] = 0;
  fBkgRange[0]  = 0;
  fBkgRange[1]  = 0;

  fT0Line        = 0;
  fFirstBkgLine  = 0;
  fLastBkgLine   = 0;
  fFirstDataLine = 0;
  fLastDataLine  = 0;

  // feed necessary objects
  TString str;

  if ((fMusrT0Data.GetCmdTag() == PMUSRT0_GET_T0) || (fMusrT0Data.GetCmdTag() == PMUSRT0_GET_T0_DATA_AND_BKG_RANGE)) {
    str = *fMusrT0Data.GetRawRunData(fMusrT0Data.GetAddRunIdx())->GetRunName() + TString(" : ");
    str += fMusrT0Data.GetHistoNo(fMusrT0Data.GetHistoNoIdx());
    if (fMusrT0Data.GetDetectorTag() == PMUSRT0_FORWARD)
      str += " (f)";
    else
      str += " (b)";
    str += ", msr runNo = ";
    str += fMusrT0Data.GetRunNo()+1;


    // feed raw data histo
    PRawRunData *rawRunData = fMusrT0Data.GetRawRunData(fMusrT0Data.GetAddRunIdx());
    if (rawRunData == 0) {
      fValid = false;
      return;
    }
    Int_t histoNo = fMusrT0Data.GetHistoNo(fMusrT0Data.GetHistoNoIdx())-1;
    Int_t noOfBins = rawRunData->GetDataBin(histoNo)->size();
    Double_t start = -0.5;
    Double_t end   = noOfBins - 0.5; // -0.5 is correct since the data start at 0.0
    fHisto = new TH1F("fHisto", str.Data(), noOfBins, start, end);
    fHisto->SetMarkerStyle(21);
    fHisto->SetMarkerSize(0.5);
    fHisto->SetMarkerColor(TColor::GetColor(0,0,0)); // black

    Double_t maxVal = 0.0;
    fT0Estimated = 0;
    for (UInt_t i=0; i<rawRunData->GetDataBin(histoNo)->size(); i++) {
      fHisto->SetBinContent(i+1, rawRunData->GetDataBin(histoNo)->at(i));
      if (rawRunData->GetDataBin(histoNo)->at(i) > maxVal) {
        maxVal = rawRunData->GetDataBin(histoNo)->at(i);
        fT0Estimated = i;
      }
    }
  } else {
    str = *fMusrT0Data.GetRawRunData(0)->GetRunName() + TString(" : ");
    if (fMusrT0Data.GetDetectorTag() == PMUSRT0_FORWARD)
      str += " forward grouped and runs added";
    else
      str += " backward grouped and runs added";
    str += ", msr runNo = ";
    str += fMusrT0Data.GetRunNo()+1;

    // feed raw data histo
    PRawRunData *rawRunData = fMusrT0Data.GetRawRunData(0);
    if (rawRunData == 0) {
      fValid = false;
      return;
    }

    // get run and first histo of grouping and feed it into fHisto
    Int_t histoNo = fMusrT0Data.GetHistoNo(0)-1;
    Int_t noOfBins = rawRunData->GetDataBin(histoNo)->size();
    Double_t start = -0.5;
    Double_t end   = noOfBins - 0.5; // -0.5 is correct since the data start at 0.0
    fHisto = new TH1F("fHisto", str.Data(), noOfBins, start, end);
    fHisto->SetMarkerStyle(21);
    fHisto->SetMarkerSize(0.5);
    fHisto->SetMarkerColor(TColor::GetColor(0,0,0)); // black

    // sum up all necessary histograms by taking care of the proper t0's
    Int_t t00;
    Int_t t0;
    Double_t dval;

    // check if asymmetry fit
    UInt_t factor=1;
    if (!fMusrT0Data.IsSingleHisto())
      factor=2;

    if (fMusrT0Data.GetDetectorTag() == PMUSRT0_FORWARD)
      t00 = fMusrT0Data.GetT0(0);
    else
      t00 = fMusrT0Data.GetT0(1);

    // check if there are addruns and grouping
    if ((fMusrT0Data.GetRawRunDataSize() > 1) && (fMusrT0Data.GetHistoNoSize() > 1)) { // addruns and grouping present

      for (Int_t i=0; i<noOfBins; i++) { // loop over all entries in fHisto

        dval = 0;

        for (UInt_t k=0; k<fMusrT0Data.GetHistoNoSize(); k++) { // loop over all histograms (grouping)

          histoNo = fMusrT0Data.GetHistoNo(k)-1;

          // get t0
          if (fMusrT0Data.GetDetectorTag() == PMUSRT0_FORWARD)
            t0 = fMusrT0Data.GetT0(factor*k);
          else
            t0 = fMusrT0Data.GetT0(factor*k+1);

          // get proper rawRunData
          rawRunData = fMusrT0Data.GetRawRunData(0);

          // get bin value from run/grouping
          if ((i+t0-t00 > 0) && (i+t0-t00 < static_cast<Int_t>(rawRunData->GetDataBin(histoNo)->size())))
            dval += rawRunData->GetDataBin(histoNo)->at(i+t0-t00);

          for (UInt_t j=1; j<fMusrT0Data.GetRawRunDataSize(); j++) { // loop over all potential addruns

            rawRunData = fMusrT0Data.GetRawRunData(j);

            // get t0
            if (fMusrT0Data.GetDetectorTag() == PMUSRT0_FORWARD)
              t0 = fMusrT0Data.GetAddT0(j-1, factor*k);
            else
              t0 = fMusrT0Data.GetAddT0(j-1, factor*k+1);

            // get bin value from addrun/grouping
            if ((i+t0-t00 > 0) && (i+t0-t00 < static_cast<Int_t>(rawRunData->GetDataBin(histoNo)->size())))
              dval += rawRunData->GetDataBin(histoNo)->at(i+t0-t00);
          }
        }
        // set bin value of fHisto
        fHisto->SetBinContent(i+1, dval);
      }

    } else if (fMusrT0Data.GetRawRunDataSize() > 1) { // only addruns present

      for (Int_t i=0; i<noOfBins; i++) { // loop over all entries in fHisto

        dval = 0;

        histoNo = fMusrT0Data.GetHistoNo(0)-1;

        // get t0
        if (fMusrT0Data.GetDetectorTag() == PMUSRT0_FORWARD)
          t0 = fMusrT0Data.GetT0(0);
        else
          t0 = fMusrT0Data.GetT0(1);

        // get proper rawRunData
        rawRunData = fMusrT0Data.GetRawRunData(0);

        // get bin value from run/grouping
        if ((i+t0-t00 > 0) && (i+t0-t00 < static_cast<Int_t>(rawRunData->GetDataBin(histoNo)->size())))
          dval += rawRunData->GetDataBin(histoNo)->at(i+t0-t00);

        for (UInt_t j=1; j<fMusrT0Data.GetRawRunDataSize(); j++) { // loop over all potential addruns

          // get proper rawRunData
          rawRunData = fMusrT0Data.GetRawRunData(j);

          // get t0
          if (fMusrT0Data.GetDetectorTag() == PMUSRT0_FORWARD)
            t0 = fMusrT0Data.GetAddT0(j-1, 0);
          else
            t0 = fMusrT0Data.GetAddT0(j-1, 1);

          // get bin value from addrun/grouping
          if ((i+t0-t00 > 0) && (i+t0-t00 < static_cast<Int_t>(rawRunData->GetDataBin(histoNo)->size())))
            dval += rawRunData->GetDataBin(histoNo)->at(i+t0-t00);
        }
        // set bin value of fHisto
        fHisto->SetBinContent(i+1, dval);
      }

    } else if (fMusrT0Data.GetHistoNoSize() > 1) { // only grouping persent

      for (Int_t i=0; i<noOfBins; i++) { // loop over all entries in fHisto

        dval = 0;

        // get proper rawRunData
        rawRunData = fMusrT0Data.GetRawRunData(0);

        for (UInt_t k=0; k<fMusrT0Data.GetHistoNoSize(); k++) { // loop over all histograms (grouping)

          histoNo = fMusrT0Data.GetHistoNo(k)-1;

          // get t0
          if (fMusrT0Data.GetDetectorTag() == PMUSRT0_FORWARD)
            t0 = fMusrT0Data.GetT0(factor*k);
          else
            t0 = fMusrT0Data.GetT0(factor*k+1);

          // get bin value from run/grouping
          if ((i+t0-t00 > 0) && (i+t0-t00 < static_cast<Int_t>(rawRunData->GetDataBin(histoNo)->size())))
            dval += rawRunData->GetDataBin(histoNo)->at(i+t0-t00);

        }
        // set bin value of fHisto
        fHisto->SetBinContent(i+1, dval);
      }

    }
  }

  // generate canvas etc
  fMainCanvas = new TCanvas("fMainCanvas", str);
  fMainCanvas->SetFillColor(TColor::GetColor(255,255,255));


  fMainCanvas->Show();

  fMainCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "PMusrT0",
                       this, "HandleCmdKey(Int_t,Int_t,Int_t,TObject*)");

  if (fMusrT0Data.GetCmdTag() != PMUSRT0_GET_T0)
    fDataAndBkgEnabled = true;
  else
    fDataAndBkgEnabled = false;

  if (fMusrT0Data.GetCmdTag() != PMUSRT0_GET_DATA_AND_BKG_RANGE)
    fT0Enabled = true;
  else
    fT0Enabled = false;

  // do not show root statistics block
  fHisto->SetStats(kFALSE);

  // draw histos etc
  fHisto->Draw("p0 9 hist");

  if (fMusrT0Data.GetCmdTag() == PMUSRT0_GET_T0) {
    str = "please set t0 bin only.";
    fToDoInfo = new TLatex();
    fToDoInfo->SetTextFont(51);
    fToDoInfo->SetTextSize(0.030);
    fToDoInfo->SetLineWidth(2);
    fToDoInfo->SetNDC(kTRUE);
    fToDoInfo->DrawLatex(0.1, 0.91, str.Data());
  }
  if (fMusrT0Data.GetCmdTag() == PMUSRT0_GET_DATA_AND_BKG_RANGE) {
    str = "please set data and bkg range.";
    fToDoInfo = new TLatex();
    fToDoInfo->SetTextFont(51);
    fToDoInfo->SetTextSize(0.030);
    fToDoInfo->SetLineWidth(2);
    fToDoInfo->SetNDC(kTRUE);
    fToDoInfo->DrawLatex(0.1, 0.91, str.Data());
  }
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor.
 */
PMusrT0::~PMusrT0()
{
  if (fHisto) {
    delete fHisto;
    fHisto = 0;
  }
  if (fData) {
    delete fData;
    fData = 0;
  }
  if (fBkg) {
    delete fBkg;
    fBkg = 0;
  }
  if (fToDoInfo) {
    delete fToDoInfo;
    fToDoInfo = 0;
  }
  if (fT0Line) {
    delete fT0Line;
    fT0Line = 0;
  }
  if (fFirstBkgLine) {
    delete fFirstBkgLine;
    fFirstBkgLine = 0;
  }
  if (fLastBkgLine) {
    delete fLastBkgLine;
    fLastBkgLine = 0;
  }
  if (fFirstDataLine) {
    delete fFirstDataLine;
    fFirstDataLine = 0;
  }
  if (fLastDataLine) {
    delete fLastDataLine;
    fLastDataLine = 0;
  }
  if (fMainCanvas) {
    delete fMainCanvas;
    fMainCanvas = 0;
  }
}

//--------------------------------------------------------------------------
// Done (SIGNAL)
//--------------------------------------------------------------------------
/**
 * <p>Signal emitted if the user wants to terminate the application.
 */
void PMusrT0::Done(Int_t status)
{
  Emit("Done(Int_t)", status);
}

//--------------------------------------------------------------------------
// HandleCmdKey (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>Filters keyboard and mouse events, and if they are a command key (see below) carries out the
 * necessary actions.
 * <p>Currently implemented command keys:
 * - 'q' close the currently shown canvas
 * - 'Q' quite the application
 * - 'z' zoom to the region aroung t0
 * - 'T' set t0 channel to the estimated t0
 * - 't' set t0 channel to the current cursor position
 * - 'b' set first background channel
 * - 'B' set last background channel
 * - 'd' set first good bin channel
 * - 'D' set last good bin channel
 *
 * \param event event type
 * \param x keyboard event: character key; mouse event: x-position
 * \param y mouse event: y-position
 * \param selected not used
 */
void PMusrT0::HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected)
{
  if (event != kKeyPress) {
    fPx = x;
    fPy = y;
    return;
  }

  // handle keys and popup menu entries
  if (x == 'q') { // quit
    fStatus = 0; // will quit locally
    Done(0);
  } else if (x == 'Q') { // terminate musrt0
    fStatus = 1; // will quit globally
    Done(0);
  } else if (x == 'u') { // unzoom to the original range
    UnZoom();
  } else if (x == 'z') { // zoom to the region around t0, and the estimated t0
    ZoomT0();
  } else if (x == 'T') { // set estimated t0 channel
    SetEstimatedT0Channel();
  } else if (x == 't') { // set t0 channel
    SetT0Channel();
  } else if (x == 'b') { // set first background channel
    SetBkgFirstChannel();
  } else if (x == 'B') { // set last background channel
    SetBkgLastChannel();
  } else if (x == 'd') { // set first data channel
    SetDataFirstChannel();
  } else if (x == 'D') { // set last data channel
    SetDataLastChannel();
  }
}

//--------------------------------------------------------------------------
// SetMsrHandler
//--------------------------------------------------------------------------
/**
 * <p>Set the msr-file handler
 *
 * \param msrHandler msr-file handler pointer
 */
void PMusrT0::SetMsrHandler(PMsrHandler *msrHandler)
{
  fMsrHandler = msrHandler;
}

//--------------------------------------------------------------------------
// InitT0
//--------------------------------------------------------------------------
/**
 * <p>Initialize t0 data.
 */
void PMusrT0::InitT0()
{
  // t0 line
  Int_t t0Bin = 0;
  Int_t histoIdx  = fMusrT0Data.GetHistoNoIdx();
  Int_t addRunIdx = fMusrT0Data.GetAddRunIdx();
  UInt_t factor=1;
  if (!fMusrT0Data.IsSingleHisto())
    factor=2;
  switch (fMusrT0Data.GetDetectorTag() ) {
    case PMUSRT0_FORWARD:
      if (addRunIdx == 0)
        t0Bin = fMsrHandler->GetMsrRunList()->at(fMusrT0Data.GetRunNo()).GetT0(factor*histoIdx);
      else if (addRunIdx > 0)
        t0Bin = fMsrHandler->GetMsrRunList()->at(fMusrT0Data.GetRunNo()).GetAddT0(addRunIdx-1, factor*histoIdx);
      break;
    case PMUSRT0_BACKWARD:
      if (addRunIdx == 0)
        t0Bin = fMsrHandler->GetMsrRunList()->at(fMusrT0Data.GetRunNo()).GetT0(factor*histoIdx+1);
      else if (addRunIdx > 0)
        t0Bin = fMsrHandler->GetMsrRunList()->at(fMusrT0Data.GetRunNo()).GetAddT0(addRunIdx-1, factor*histoIdx+1);
      break;
    default:
      // not clear yet what to be done
      break;
  }
  Double_t max = fHisto->GetMaximum();

  fT0Line = new TLine((Double_t)t0Bin, 0.0, (Double_t)t0Bin, max);
  fT0Line->SetLineStyle(1); // solid
  fT0Line->SetLineColor(TColor::GetColor(0,255,0)); // green
  fT0Line->SetLineWidth(2);
  fT0Line->Draw();
}

//--------------------------------------------------------------------------
// InitDataAndBkg
//--------------------------------------------------------------------------
/**
 * <p>Initialize data and background ranges.
 */
void PMusrT0::InitDataAndBkg()
{
  // feed data range histo
  switch (fMusrT0Data.GetDetectorTag()) {
    case PMUSRT0_FORWARD:
      fDataRange[0] = fMsrHandler->GetMsrRunList()->at(fMusrT0Data.GetRunNo()).GetDataRange(0);
      fDataRange[1] = fMsrHandler->GetMsrRunList()->at(fMusrT0Data.GetRunNo()).GetDataRange(1);
      break;
    case PMUSRT0_BACKWARD:
      fDataRange[0] = fMsrHandler->GetMsrRunList()->at(fMusrT0Data.GetRunNo()).GetDataRange(2);
      fDataRange[1] = fMsrHandler->GetMsrRunList()->at(fMusrT0Data.GetRunNo()).GetDataRange(3);
      break;
    default:
      // not clear yet what to be done
      break;
  }

  Int_t noOfBins = fDataRange[1]-fDataRange[0]+1;
  Double_t start = fDataRange[0] - 0.5;
  Double_t end   = fDataRange[1] + 0.5;
  fData = new TH1F("fData", "fData", noOfBins, start, end);
  fData->SetMarkerStyle(21);
  fData->SetMarkerSize(0.5);
  fData->SetMarkerColor(TColor::GetColor(0,0,255)); // blue

  for (Int_t i=0; i<noOfBins; i++) {
    fData->SetBinContent(i+1, fHisto->GetBinContent(fDataRange[0]+i+1));
  }
  fData->Draw("p0 9 hist same");

  // feed background histo
  switch (fMusrT0Data.GetDetectorTag()) {
    case PMUSRT0_FORWARD:
      fBkgRange[0] = fMsrHandler->GetMsrRunList()->at(fMusrT0Data.GetRunNo()).GetBkgRange(0);
      fBkgRange[1] = fMsrHandler->GetMsrRunList()->at(fMusrT0Data.GetRunNo()).GetBkgRange(1);
      break;
    case PMUSRT0_BACKWARD:
      fBkgRange[0] = fMsrHandler->GetMsrRunList()->at(fMusrT0Data.GetRunNo()).GetBkgRange(2);
      fBkgRange[1] = fMsrHandler->GetMsrRunList()->at(fMusrT0Data.GetRunNo()).GetBkgRange(3);
      break;
    default:
      // not clear yet what to be done
      break;
  }

  noOfBins = fBkgRange[1]-fBkgRange[0]+1;
  start = fBkgRange[0] - 0.5;
  end   = fBkgRange[1] + 0.5;
  fBkg = new TH1F("fBkg", "fBkg", noOfBins, start, end);
  fBkg->SetMarkerStyle(21);
  fBkg->SetMarkerSize(0.5);
  fBkg->SetMarkerColor(TColor::GetColor(255,0,0)); // red

  for (Int_t i=0; i<noOfBins; i++) {
    fBkg->SetBinContent(i+1, fHisto->GetBinContent(fBkgRange[0]+i+1));
  }
  fBkg->Draw("p0 9 hist same");

  // add lines
  Double_t max = fHisto->GetMaximum();

  // data lines
  fFirstDataLine = new TLine((Double_t)fDataRange[0], 0.0, (Double_t)fDataRange[0], max);
  fFirstDataLine->SetLineStyle(3); // doted
  fFirstDataLine->SetLineColor(TColor::GetColor(0,0,255)); // blue
  fFirstDataLine->SetLineWidth(2);
  fFirstDataLine->Draw();

  fLastDataLine = new TLine((Double_t)fDataRange[1], 0.0, (Double_t)fDataRange[1], max);
  fLastDataLine->SetLineStyle(3); // doted
  fLastDataLine->SetLineColor(TColor::GetColor(0,0,255)); // blue
  fLastDataLine->SetLineWidth(2);
  fLastDataLine->Draw();

  // bkg lines
  fFirstBkgLine = new TLine((Double_t)fBkgRange[0], 0.0, (Double_t)fBkgRange[0], max);
  fFirstBkgLine->SetLineStyle(6); // _..._...
  fFirstBkgLine->SetLineColor(TColor::GetColor(255,0,0)); // red
  fFirstBkgLine->SetLineWidth(2);
  fFirstBkgLine->Draw();

  fLastBkgLine = new TLine((Double_t)fBkgRange[1], 0.0, (Double_t)fBkgRange[1], max);
  fLastBkgLine->SetLineStyle(6); // _..._...
  fLastBkgLine->SetLineColor(TColor::GetColor(255,0,0)); // red
  fLastBkgLine->SetLineWidth(2);
  fLastBkgLine->Draw();

  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// SetT0Channel
//--------------------------------------------------------------------------
/**
 * <p>Set the t0 channel to the current cursor position and keep the x-position as new t0 bin.
 */
void PMusrT0::SetT0Channel()
{
  if (!fT0Enabled)
    return;

  Double_t x=0, y=0;
  fMainCanvas->AbsPixeltoXY(fPx,fPy,x,y);

  // get binx to set t0 corresponding to fPx
  Int_t binx = fHisto->GetXaxis()->FindFixBin(x) - 1;

  cout << endl << ">> PMusrT0::SetT0Channel(): t0 = " << binx << endl;

  // set t0 bin in msr-Handler
  UInt_t idx;
  if (fMusrT0Data.IsSingleHisto()) {
    idx = fMusrT0Data.GetHistoNoIdx();
    if (fMusrT0Data.GetDetectorTag() == PMUSRT0_BACKWARD)
      idx += 1;
  } else {
    idx = 2*fMusrT0Data.GetHistoNoIdx();
    if (fMusrT0Data.GetDetectorTag() == PMUSRT0_BACKWARD)
      idx += 1;
  }

  if (fMusrT0Data.GetAddRunIdx() == 0)
    fMsrHandler->SetMsrT0Entry(fMusrT0Data.GetRunNo(), idx, binx);
  else if (fMusrT0Data.GetAddRunIdx() > 0)
    fMsrHandler->SetMsrAddT0Entry(fMusrT0Data.GetRunNo(), fMusrT0Data.GetAddRunIdx()-1, idx, binx);

  // shift line to the proper position
  fT0Line->SetX1(x);
  fT0Line->SetX2(x);

  fMainCanvas->Modified(); // needed that Update is actually working
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// SetEstimatedT0Channel
//--------------------------------------------------------------------------
/**
 * <p>Set the estimated t0 channel to the current cursor position and keep the x-position as new t0 bin.
 */
void PMusrT0::SetEstimatedT0Channel()
{
  if (!fT0Enabled)
    return;

  // set t0 bin in msr-Handler
  UInt_t idx;
  if (fMusrT0Data.IsSingleHisto()) {
    idx = fMusrT0Data.GetHistoNoIdx();
    if (fMusrT0Data.GetDetectorTag() == PMUSRT0_BACKWARD)
      idx += 1;
  } else {
    idx = 2*fMusrT0Data.GetHistoNoIdx();
    if (fMusrT0Data.GetDetectorTag() == PMUSRT0_BACKWARD)
      idx += 1;
  }

  if (fMusrT0Data.GetAddRunIdx() == 0)
    fMsrHandler->SetMsrT0Entry(fMusrT0Data.GetRunNo(), idx, fT0Estimated);
  else if (fMusrT0Data.GetAddRunIdx() > 0)
    fMsrHandler->SetMsrAddT0Entry(fMusrT0Data.GetRunNo(), fMusrT0Data.GetAddRunIdx()-1, idx, fT0Estimated);

  Double_t x = fHisto->GetXaxis()->GetBinCenter(fT0Estimated)+1.0; // +1.0 needed since the first bin == 1 not 0.

  cout << endl << ">> PMusrT0::SetEstimatedT0Channel(): estimated t0 = " << fT0Estimated << endl;

  // shift line to the proper position
  fT0Line->SetX1(x);
  fT0Line->SetX2(x);

  fMainCanvas->Modified(); // needed that Update is actually working
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// SetDataFirstChannel
//--------------------------------------------------------------------------
/**
 * <p>Set the first good data channel to the current cursor position and keep the x-position as new first good data bin.
 */
void PMusrT0::SetDataFirstChannel()
{
  if (!fDataAndBkgEnabled)
    return;

  Double_t x=0, y=0;
  fMainCanvas->AbsPixeltoXY(fPx,fPy,x,y);

  // get binx to set the data first channel corresponding to fPx
  fDataRange[0] = fHisto->GetXaxis()->FindFixBin(x) - 1;

  cout << endl << ">> PMusrT0::SetDataFirstChannel(): fDataRange[0] = " << fDataRange[0] << endl;

  // set the data first bin in msr-Handler
  UInt_t idx = 0;
  if (fMusrT0Data.GetDetectorTag() == PMUSRT0_BACKWARD)
    idx = 2;
  fMsrHandler->SetMsrDataRangeEntry(fMusrT0Data.GetRunNo(), idx, fDataRange[0]);

  // shift line to the proper position
  fFirstDataLine->SetX1(x);
  fFirstDataLine->SetX2(x);

  // recreate data histo
  delete fData;
  fData = 0;

  // refill data histo
  Int_t noOfBins = fDataRange[1]-fDataRange[0]+1;
  Double_t start = fDataRange[0] - 0.5;
  Double_t end   = fDataRange[1] + 0.5;
  fData = new TH1F("fData", "fData", noOfBins, start, end);
  fData->SetMarkerStyle(21);
  fData->SetMarkerSize(0.5);
  fData->SetMarkerColor(TColor::GetColor(0,0,255)); // blue
  for (Int_t i=0; i<noOfBins; i++) {
    fData->SetBinContent(i+1, fHisto->GetBinContent(fDataRange[0]+i+1));
  }
  fData->Draw("p0 9 hist same");

  fMainCanvas->Modified(); // needed that Update is actually working
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// SetDataLastChannel
//--------------------------------------------------------------------------
/**
 * <p>Set the last good data channel to the current cursor position and keep the x-position as new last good data bin.
 */
void PMusrT0::SetDataLastChannel()
{
  if (!fDataAndBkgEnabled)
    return;

  Double_t x=0, y=0;
  fMainCanvas->AbsPixeltoXY(fPx,fPy,x,y);

  // get binx to set the data last channel corresponding to fPx
  fDataRange[1] = fHisto->GetXaxis()->FindFixBin(x) - 1;

  cout << endl << ">> PMusrT0::SetDataLastChannel(): fDataRange[1] = " << fDataRange[1] << endl;

  // set the data first bin in msr-Handler
  UInt_t idx = 1;
  if (fMusrT0Data.GetDetectorTag() == PMUSRT0_BACKWARD)
    idx = 3;
  fMsrHandler->SetMsrDataRangeEntry(fMusrT0Data.GetRunNo(), idx, fDataRange[1]);

  // shift line to the proper position
  fLastDataLine->SetX1(x);
  fLastDataLine->SetX2(x);

  // recreate data histo
  delete fData;
  fData = 0;

  // refill data histo
  Int_t noOfBins = fDataRange[1]-fDataRange[0]+1;
  Double_t start = fDataRange[0] - 0.5;
  Double_t end   = fDataRange[1] + 0.5;
  fData = new TH1F("fData", "fData", noOfBins, start, end);
  fData->SetMarkerStyle(21);
  fData->SetMarkerSize(0.5);
  fData->SetMarkerColor(TColor::GetColor(0,0,255)); // blue
  for (Int_t i=0; i<noOfBins; i++) {
    fData->SetBinContent(i+1, fHisto->GetBinContent(fDataRange[0]+i+1));
  }
  fData->Draw("p0 9 hist same");

  fMainCanvas->Modified(); // needed that Update is actually working
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// SetBkgFirstChannel
//--------------------------------------------------------------------------
/**
 * <p>Set the first background channel to the current cursor position and keep the x-position as new first background bin.
 */
void PMusrT0::SetBkgFirstChannel()
{
  if (!fDataAndBkgEnabled)
    return;

  Double_t x=0, y=0;
  fMainCanvas->AbsPixeltoXY(fPx,fPy,x,y);

  // get binx to set the background first channel corresponding to fPx
  fBkgRange[0] = fHisto->GetXaxis()->FindFixBin(x) - 1;

  cout << endl << ">> PMusrT0::SetBkgFirstChannel(): fBkgRange[0] = " << fBkgRange[0] << endl;

  // set the background first bin in msr-Handler
  UInt_t idx = 0;
  if (fMusrT0Data.GetDetectorTag() == PMUSRT0_BACKWARD)
    idx = 2;
  fMsrHandler->SetMsrBkgRangeEntry(fMusrT0Data.GetRunNo(), idx, fBkgRange[0]);

  // shift line to the proper position
  fFirstBkgLine->SetX1(x);
  fFirstBkgLine->SetX2(x);

  // recreate data histo
  delete fBkg;
  fBkg = 0;

  // refill data histo
  Int_t noOfBins = fBkgRange[1]-fBkgRange[0]+1;
  Double_t start = fBkgRange[0] - 0.5;
  Double_t end   = fBkgRange[1] + 0.5;
  fBkg = new TH1F("fBkg", "fBkg", noOfBins, start, end);
  fBkg->SetMarkerStyle(21);
  fBkg->SetMarkerSize(0.5);
  fBkg->SetMarkerColor(TColor::GetColor(255,0,0)); // red
  for (Int_t i=0; i<noOfBins; i++) {
    fBkg->SetBinContent(i+1, fHisto->GetBinContent(fBkgRange[0]+i+1));
  }
  fBkg->Draw("p0 9 hist same");

  fMainCanvas->Modified(); // needed that Update is actually working
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// SetBkgLastChannel
//--------------------------------------------------------------------------
/**
 * <p>Set the last background channel to the current cursor position and keep the x-position as new last background bin.
 */
void PMusrT0::SetBkgLastChannel()
{
  if (!fDataAndBkgEnabled)
    return;

  Double_t x=0, y=0;
  fMainCanvas->AbsPixeltoXY(fPx,fPy,x,y);

  // get binx to set the background last channel corresponding to fPx
  fBkgRange[1] = fHisto->GetXaxis()->FindFixBin(x) - 1;

  cout << endl << ">> PMusrT0::SetBkgLastChannel(): fBkgRange[1] = " << fBkgRange[1] << endl;

  // set the background first bin in msr-Handler
  UInt_t idx = 1;
  if (fMusrT0Data.GetDetectorTag() == PMUSRT0_BACKWARD)
    idx = 3;
  fMsrHandler->SetMsrBkgRangeEntry(fMusrT0Data.GetRunNo(), idx, fBkgRange[1]);

  // shift line to the proper position
  fLastBkgLine->SetX1(x);
  fLastBkgLine->SetX2(x);

  // recreate data histo
  delete fBkg;
  fBkg = 0;

  // refill data histo
  Int_t noOfBins = fBkgRange[1]-fBkgRange[0]+1;
  Double_t start = fBkgRange[0] - 0.5;
  Double_t end   = fBkgRange[1] + 0.5;
  fBkg = new TH1F("fBkg", "fBkg", noOfBins, start, end);
  fBkg->SetMarkerStyle(21);
  fBkg->SetMarkerSize(0.5);
  fBkg->SetMarkerColor(TColor::GetColor(255,0,0)); // red
  for (Int_t i=0; i<noOfBins; i++) {
    fBkg->SetBinContent(i+1, fHisto->GetBinContent(fBkgRange[0]+i+1));
  }
  fBkg->Draw("p0 9 hist same");

  fMainCanvas->Modified(); // needed that Update is actually working
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// UnZoom
//--------------------------------------------------------------------------
/**
 * <p>Unzoom the current histogram
 */
void PMusrT0::UnZoom()
{
  fHisto->GetXaxis()->UnZoom();
  fHisto->GetYaxis()->UnZoom();

  fMainCanvas->Modified(); // needed that Update is actually working
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// ZoomT0
//--------------------------------------------------------------------------
/**
 * <p>Zoom into the histogram region of the t0, and/or estimated t0 range.
 */
void PMusrT0::ZoomT0()
{
  if (!fT0Enabled)
    return;

  const Int_t range = 75;

  // get current t0 position
  Double_t t0x = fT0Line->GetX1();
  Int_t t0 = fHisto->GetXaxis()->FindBin(t0x)-1;

  Int_t min = t0 - range;
  Int_t max = t0 + range;

  // check if t0 is defined at all
  if (t0 <= 0) {
    min = fT0Estimated - range;
    max = fT0Estimated + range;
  }

  if (fT0Estimated < min) {
    min = fT0Estimated - range;
  }
  if (fT0Estimated > max) {
    max = fT0Estimated + range;
  }

  fHisto->GetXaxis()->SetRangeUser(min, max);

  fMainCanvas->Modified(); // needed that Update is actually working
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
