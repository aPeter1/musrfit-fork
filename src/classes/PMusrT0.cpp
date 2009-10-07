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
#define DETECTOR_TAG_RIGHT    2
#define DETECTOR_TAG_LEFT     3

ClassImpQ(PMusrT0)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PMusrT0::PMusrT0()
{
  fRunNo       = -1;
  fDetectorTag = -1;

  fMainCanvas = 0;

  fHisto = 0;
  fData  = 0;
  fBkg   = 0;

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
 *
 * \param rawRunData
 * \param histoNo
 */
PMusrT0::PMusrT0(PRawRunData *rawRunData, int runNo, int histoNo, int detectorTag, int addRunNo) :
    fRunNo(runNo), fDetectorTag(detectorTag), fAddRunNo(addRunNo)
{
cout << endl << "run Name = " << rawRunData->GetRunName()->Data() << ", runNo = " << fRunNo << ", histoNo = " << histoNo << endl;

  fStatus = 0; // default is quit locally

  fAddRunOffset = 0;

  TString str = *rawRunData->GetRunName() + TString(" : ");
  str += histoNo;

  // feed necessary objects

  // feed raw data histo
  Int_t noOfBins = rawRunData->GetDataBin(histoNo-1)->size();
  Double_t start = -0.5;
  Double_t end   = noOfBins - 0.5; // -0.5 is correct since the data start at 0.0
  fHisto = new TH1F("fHisto", str.Data(), noOfBins, start, end);
  fHisto->SetMarkerStyle(21);
  fHisto->SetMarkerSize(0.5);
  fHisto->SetMarkerColor(TColor::GetColor(0,0,0)); // black

  for (unsigned int i=0; i<rawRunData->GetDataBin(histoNo-1)->size(); i++) {
    fHisto->SetBinContent(i+1, rawRunData->GetDataBin(histoNo-1)->at(i));
  }

  // generate canvas etc
  fMainCanvas = new TCanvas("fMainCanvas", str);
  fMainCanvas->SetFillColor(TColor::GetColor(255,255,255));

  fMainCanvas->Show();

  fMainCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "PMusrT0",
                       this, "HandleCmdKey(Int_t,Int_t,Int_t,TObject*)");

  // draw histos etc
  fHisto->Draw("p0 9 hist");
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
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
 * <p>
 *
 */
void PMusrT0::Done(Int_t status)
{
  Emit("Done(Int_t)", status);
}

//--------------------------------------------------------------------------
// HandleCmdKey (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>
 *
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
 * <p>
 *
 */
void PMusrT0::SetMsrHandler(PMsrHandler *msrHandler)
{
  fMsrHandler = msrHandler;

  InitDataAndBkg();
}

//--------------------------------------------------------------------------
// InitDataAndBkg
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrT0::InitDataAndBkg()
{
  // get addRun offset which depends on the fit type
  int fitType = fMsrHandler->GetMsrRunList()->at(fRunNo).fFitType;
  if (fitType == MSR_FITTYPE_SINGLE_HISTO) {
    fAddRunOffset = 2;
  } else if (fitType == MSR_FITTYPE_ASYM) {
    fAddRunOffset = 4;
  } else if (fitType == MSR_FITTYPE_ASYM_RRF) {
    fAddRunOffset = 8;
  }

  // feed data range histo
  switch (fDetectorTag) {
    case DETECTOR_TAG_FORWARD:
      fDataRange[0] = fMsrHandler->GetMsrRunList()->at(fRunNo).fDataRange[0 + fAddRunNo * fAddRunOffset];
      fDataRange[1] = fMsrHandler->GetMsrRunList()->at(fRunNo).fDataRange[1 + fAddRunNo * fAddRunOffset];
      break;
    case DETECTOR_TAG_BACKWARD:
      fDataRange[0] = fMsrHandler->GetMsrRunList()->at(fRunNo).fDataRange[2 + fAddRunNo * fAddRunOffset];
      fDataRange[1] = fMsrHandler->GetMsrRunList()->at(fRunNo).fDataRange[3 + fAddRunNo * fAddRunOffset];
      break;
    case DETECTOR_TAG_RIGHT:
      // not clear yet what to be done
      break;
    case DETECTOR_TAG_LEFT:
      // not clear yet what to be done
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

  for (int i=0; i<noOfBins; i++) {
    fData->SetBinContent(i+1, fHisto->GetBinContent(fDataRange[0]+i+1));
  }
  fData->Draw("p0 9 hist same");

  // feed background histo
  switch (fDetectorTag) {
    case DETECTOR_TAG_FORWARD:
      fBkgRange[0] = fMsrHandler->GetMsrRunList()->at(fRunNo).fBkgRange[0 + fAddRunNo * fAddRunOffset];
      fBkgRange[1] = fMsrHandler->GetMsrRunList()->at(fRunNo).fBkgRange[1 + fAddRunNo * fAddRunOffset];
      break;
    case DETECTOR_TAG_BACKWARD:
      fBkgRange[0] = fMsrHandler->GetMsrRunList()->at(fRunNo).fBkgRange[2 + fAddRunNo * fAddRunOffset];
      fBkgRange[1] = fMsrHandler->GetMsrRunList()->at(fRunNo).fBkgRange[3 + fAddRunNo * fAddRunOffset];
      break;
    case DETECTOR_TAG_RIGHT:
      // not clear yet what to be done
      break;
    case DETECTOR_TAG_LEFT:
      // not clear yet what to be done
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

  for (int i=0; i<noOfBins; i++) {
    fBkg->SetBinContent(i+1, fHisto->GetBinContent(fBkgRange[0]+i+1));
  }
  fBkg->Draw("p0 9 hist same");

  // add lines
  // t0 line
  Int_t t0Bin = 0;
  switch (fDetectorTag) {
    case DETECTOR_TAG_FORWARD:
      t0Bin = fMsrHandler->GetMsrRunList()->at(fRunNo).fT0[0 + fAddRunNo * fAddRunOffset/2];
      break;
    case DETECTOR_TAG_BACKWARD:
      t0Bin = fMsrHandler->GetMsrRunList()->at(fRunNo).fT0[1 + fAddRunNo * fAddRunOffset/2];
      break;
    case DETECTOR_TAG_RIGHT:
      // not clear yet what to be done
      break;
    case DETECTOR_TAG_LEFT:
      // not clear yet what to be done
      break;
    default:
      // not clear yet what to be done
      break;
  }
  Double_t max = fHisto->GetMaximum();
  fT0Line = new TLine((double)t0Bin, 0.0, (double)t0Bin, max);
  fT0Line->SetLineStyle(1); // solid
  fT0Line->SetLineColor(TColor::GetColor(0,255,0)); // green
  fT0Line->SetLineWidth(2);
  fT0Line->Draw();

  // data lines
  fFirstDataLine = new TLine((double)fDataRange[0], 0.0, (double)fDataRange[0], max);
  fFirstDataLine->SetLineStyle(3); // doted
  fFirstDataLine->SetLineColor(TColor::GetColor(0,0,255)); // blue
  fFirstDataLine->SetLineWidth(2);
  fFirstDataLine->Draw();

  fLastDataLine = new TLine((double)fDataRange[1], 0.0, (double)fDataRange[1], max);
  fLastDataLine->SetLineStyle(3); // doted
  fLastDataLine->SetLineColor(TColor::GetColor(0,0,255)); // blue
  fLastDataLine->SetLineWidth(2);
  fLastDataLine->Draw();

  // bkg lines
  fFirstBkgLine = new TLine((double)fBkgRange[0], 0.0, (double)fBkgRange[0], max);
  fFirstBkgLine->SetLineStyle(6); // _..._...
  fFirstBkgLine->SetLineColor(TColor::GetColor(255,0,0)); // red
  fFirstBkgLine->SetLineWidth(2);
  fFirstBkgLine->Draw();

  fLastBkgLine = new TLine((double)fBkgRange[1], 0.0, (double)fBkgRange[1], max);
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
 * <p>
 *
 */
void PMusrT0::SetT0Channel()
{
  double x=0, y=0;
  fMainCanvas->AbsPixeltoXY(fPx,fPy,x,y);

  // get binx to set t0 corresponding to fPx
  Int_t binx = fHisto->GetXaxis()->FindFixBin(x) - 1;

cout << endl << ">> PMusrT0::SetT0Channel(): binx = " << binx << endl;

  // set t0 bin in msr-Handler
  unsigned int idx = 0;
  switch(fDetectorTag) {
    case DETECTOR_TAG_FORWARD:
      idx = fAddRunNo * fAddRunOffset / 2;
      break;
    case DETECTOR_TAG_BACKWARD:
      idx = 1 + fAddRunNo * fAddRunOffset / 2;
      break;
    case DETECTOR_TAG_LEFT:
      idx = 2 + fAddRunNo * fAddRunOffset / 2;
      break;
    case DETECTOR_TAG_RIGHT:
      idx = 3 + fAddRunNo * fAddRunOffset / 2;
      break;
    default:
      break;
  }
  fMsrHandler->SetMsrT0Entry(fRunNo, idx, binx);

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
 * <p>
 *
 */
void PMusrT0::SetDataFirstChannel()
{
  double x=0, y=0;
  fMainCanvas->AbsPixeltoXY(fPx,fPy,x,y);

  // get binx to set the data first channel corresponding to fPx
  fDataRange[0] = fHisto->GetXaxis()->FindFixBin(x) - 1;

cout << endl << ">> PMusrT0::SetDataFirstChannel(): fDataRange[0] = " << fDataRange[0] << endl;

  // set the data first bin in msr-Handler
  unsigned int idx = 0;
  switch(fDetectorTag) {
    case DETECTOR_TAG_FORWARD:
      idx = fAddRunNo * fAddRunOffset;
      break;
    case DETECTOR_TAG_BACKWARD:
      idx = 2 + fAddRunNo * fAddRunOffset;
      break;
    case DETECTOR_TAG_LEFT:
      idx = 4 + fAddRunNo * fAddRunOffset;
      break;
    case DETECTOR_TAG_RIGHT:
      idx = 6 + fAddRunNo * fAddRunOffset;
      break;
    default:
      break;
  }
  fMsrHandler->SetMsrDataRangeEntry(fRunNo, idx, fDataRange[0]);

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
  for (int i=0; i<noOfBins; i++) {
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
 * <p>
 *
 */
void PMusrT0::SetDataLastChannel()
{
  double x=0, y=0;
  fMainCanvas->AbsPixeltoXY(fPx,fPy,x,y);

  // get binx to set the data last channel corresponding to fPx
  fDataRange[1] = fHisto->GetXaxis()->FindFixBin(x) - 1;

cout << endl << ">> PMusrT0::SetDataLastChannel(): fDataRange[1] = " << fDataRange[1] << endl;

  // set the data first bin in msr-Handler
  unsigned int idx = 0;
  switch(fDetectorTag) {
    case DETECTOR_TAG_FORWARD:
      idx = 1 + fAddRunNo * fAddRunOffset;
      break;
    case DETECTOR_TAG_BACKWARD:
      idx = 3 + fAddRunNo * fAddRunOffset;
      break;
    case DETECTOR_TAG_LEFT:
      idx = 5 + fAddRunNo * fAddRunOffset;
      break;
    case DETECTOR_TAG_RIGHT:
      idx = 7 + fAddRunNo * fAddRunOffset;
      break;
    default:
      break;
  }
  fMsrHandler->SetMsrDataRangeEntry(fRunNo, idx, fDataRange[1]);

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
  for (int i=0; i<noOfBins; i++) {
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
 * <p>
 *
 */
void PMusrT0::SetBkgFirstChannel()
{
  double x=0, y=0;
  fMainCanvas->AbsPixeltoXY(fPx,fPy,x,y);

  // get binx to set the background first channel corresponding to fPx
  fBkgRange[0] = fHisto->GetXaxis()->FindFixBin(x) - 1;

cout << endl << ">> PMusrT0::SetBkgFirstChannel(): fBkgRange[0] = " << fBkgRange[0] << endl;

  // set the background first bin in msr-Handler
  unsigned int idx = 0;
  switch(fDetectorTag) {
    case DETECTOR_TAG_FORWARD:
      idx = fAddRunNo * fAddRunOffset;
      break;
    case DETECTOR_TAG_BACKWARD:
      idx = 2 + fAddRunNo * fAddRunOffset;
      break;
    case DETECTOR_TAG_LEFT:
      idx = 4 + fAddRunNo * fAddRunOffset;
      break;
    case DETECTOR_TAG_RIGHT:
      idx = 6 + fAddRunNo * fAddRunOffset;
      break;
    default:
      break;
  }
  fMsrHandler->SetMsrBkgRangeEntry(fRunNo, idx, fBkgRange[0]);

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
  for (int i=0; i<noOfBins; i++) {
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
 * <p>
 *
 */
void PMusrT0::SetBkgLastChannel()
{
  double x=0, y=0;
  fMainCanvas->AbsPixeltoXY(fPx,fPy,x,y);

  // get binx to set the background last channel corresponding to fPx
  fBkgRange[1] = fHisto->GetXaxis()->FindFixBin(x) - 1;

cout << endl << ">> PMusrT0::SetBkgLastChannel(): fBkgRange[1] = " << fBkgRange[1] << endl;

  // set the background first bin in msr-Handler
  unsigned int idx = 0;
  switch(fDetectorTag) {
    case DETECTOR_TAG_FORWARD:
      idx = 1 + fAddRunNo * fAddRunOffset;
      break;
    case DETECTOR_TAG_BACKWARD:
      idx = 3 + fAddRunNo * fAddRunOffset;
      break;
    case DETECTOR_TAG_LEFT:
      idx = 5 + fAddRunNo * fAddRunOffset;
      break;
    case DETECTOR_TAG_RIGHT:
      idx = 7 + fAddRunNo * fAddRunOffset;
      break;
    default:
      break;
  }
  fMsrHandler->SetMsrBkgRangeEntry(fRunNo, idx, fBkgRange[1]);

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
  for (int i=0; i<noOfBins; i++) {
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
 * <p>
 *
 */
void PMusrT0::UnZoom()
{
  fHisto->GetXaxis()->UnZoom();

  fMainCanvas->Modified(); // needed that Update is actually working
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
