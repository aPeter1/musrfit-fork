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
PMusrT0::PMusrT0(PRawRunData *rawRunData, int runNo, int histoNo, int detectorTag, int addRunNo)
{
  cout << endl << "run Name = " << rawRunData->fRunName.Data() << ", histoNo = " << histoNo << endl;

  fStatus = 0; // default is quit locally

  fRunNo       = runNo;
  fDetectorTag = detectorTag;
  fAddRunNo    = addRunNo;

  TString str = rawRunData->fRunName + TString(" : ");
  str += histoNo;

  // feed necessary objects

  // feed raw data histo
  Int_t noOfBins = rawRunData->fDataBin[histoNo-1].size();
  Double_t start = -0.5;
  Double_t end   = noOfBins + 0.5;
  fHisto = new TH1F("fHisto", str.Data(), noOfBins, start, end);
  fHisto->SetMarkerStyle(21);
  fHisto->SetMarkerSize(0.5);
  fHisto->SetMarkerColor(TColor::GetColor(0,0,0)); // black

  for (unsigned int i=0; i<rawRunData->fDataBin[histoNo-1].size(); i++) {
    fHisto->SetBinContent(i+1, rawRunData->fDataBin[histoNo-1][i]);
  }

  // generate canvas etc
  fMainCanvas = new TCanvas("fMainCanvas", str);
  fMainCanvas->SetFillColor(TColor::GetColor(255,255,255));

  // add canvas menu
  fImp = (TRootCanvas*)fMainCanvas->GetCanvasImp();
  fBar = fImp->GetMenuBar();
  fPopupMain = fBar->AddPopup("&MusrT0");

  fPopupMain->AddEntry("&T0", P_MENU_ID_T0);
  fPopupMain->AddSeparator();
  fPopupMain->AddEntry("First Bkg Channel", P_MENU_ID_FIRST_BKG_CHANNEL);
  fPopupMain->AddEntry("Last  Bkg Channel", P_MENU_ID_LAST_BKG_CHANNEL);
  fPopupMain->AddSeparator();
  fPopupMain->AddEntry("First Data Channel", P_MENU_ID_FIRST_DATA_CHANNEL);
  fPopupMain->AddEntry("Last  Data Channel", P_MENU_ID_LAST_DATA_CHANNEL);
  fPopupMain->AddSeparator();
  fPopupMain->AddEntry("UnZoom", P_MENU_ID_UNZOOM);


  fBar->MapSubwindows();
  fBar->Layout();
  fPopupMain->Connect("TGPopupMenu", "Activated(Int_t)", "PMusrT0", this, "HandleMenuPopup(Int_t)");

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
  if (event != kKeyPress)
     return;

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
    cout << endl << "will set t0 channel ..." << endl;
  } else if (x == 'b') { // set first background channel
    cout << endl << "will set first background channel ..." << endl;
  } else if (x == 'B') { // set last background channel
    cout << endl << "will set last background channel ..." << endl;
  } else if (x == 'd') { // set first data channel
    cout << endl << "will set first data channel ..." << endl;
  } else if (x == 'D') { // set last data channel
    cout << endl << "will set last data channel ..." << endl;
  }
}

//--------------------------------------------------------------------------
// HandleMenuPopup (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrT0::HandleMenuPopup(Int_t id)
{
  switch (id) {
    case P_MENU_ID_T0:
      break;
    case P_MENU_ID_FIRST_BKG_CHANNEL:
      break;
    case P_MENU_ID_LAST_BKG_CHANNEL:
      break;
    case P_MENU_ID_FIRST_DATA_CHANNEL:
      break;
    case P_MENU_ID_LAST_DATA_CHANNEL:
      break;
    case P_MENU_ID_UNZOOM:
      UnZoom();
      break;
    default:
      break;
  }
}

//--------------------------------------------------------------------------
// HandleMenuPopup (SLOT)
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
// HandleMenuPopup (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrT0::InitDataAndBkg()
{
  // get addRun offset which depends on the fit type
  int addRunOffset = 0;
  int fitType = fMsrHandler->GetMsrRunList()->at(fRunNo).fFitType;
  if (fitType == MSR_FITTYPE_SINGLE_HISTO) {
    addRunOffset = 2;
  } else if (fitType == MSR_FITTYPE_ASYM) {
    addRunOffset = 4;
  } else if (fitType == MSR_FITTYPE_ASYM_RRF) {
    addRunOffset = 8;
  }

  // feed data range histo
  int dataRange[2];
  switch (fDetectorTag) {
    case DETECTOR_TAG_FORWARD:
      dataRange[0] = fMsrHandler->GetMsrRunList()->at(fRunNo).fDataRange[0 + fAddRunNo * addRunOffset];
      dataRange[1] = fMsrHandler->GetMsrRunList()->at(fRunNo).fDataRange[1 + fAddRunNo * addRunOffset];
      break;
    case DETECTOR_TAG_BACKWARD:
      dataRange[0] = fMsrHandler->GetMsrRunList()->at(fRunNo).fDataRange[2 + fAddRunNo * addRunOffset];
      dataRange[1] = fMsrHandler->GetMsrRunList()->at(fRunNo).fDataRange[3 + fAddRunNo * addRunOffset];
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

  Int_t noOfBins = dataRange[1]-dataRange[0]+1;
  Double_t start = dataRange[0] - 0.5;
  Double_t end   = dataRange[1] + 0.5;
  fData = new TH1F("fData", "fData", noOfBins, start, end);
  fData->SetMarkerStyle(21);
  fData->SetMarkerSize(0.5);
  fData->SetMarkerColor(TColor::GetColor(0,0,255)); // blue

  for (int i=0; i<noOfBins; i++) {
    fData->SetBinContent(i+1, fHisto->GetBinContent(dataRange[0]+i+1));
  }
  fData->Draw("p0 9 hist same");

  // feed background histo
  int bkgRange[2];
  switch (fDetectorTag) {
    case DETECTOR_TAG_FORWARD:
      bkgRange[0] = fMsrHandler->GetMsrRunList()->at(fRunNo).fBkgRange[0 + fAddRunNo * addRunOffset];
      bkgRange[1] = fMsrHandler->GetMsrRunList()->at(fRunNo).fBkgRange[1 + fAddRunNo * addRunOffset];
      break;
    case DETECTOR_TAG_BACKWARD:
      bkgRange[0] = fMsrHandler->GetMsrRunList()->at(fRunNo).fBkgRange[2 + fAddRunNo * addRunOffset];
      bkgRange[1] = fMsrHandler->GetMsrRunList()->at(fRunNo).fBkgRange[3 + fAddRunNo * addRunOffset];
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

  noOfBins = bkgRange[1]-bkgRange[0]+1;
  start = bkgRange[0] - 0.5;
  end   = bkgRange[1] + 0.5;
  fBkg = new TH1F("fBkg", "fBkg", noOfBins, start, end);
  fBkg->SetMarkerStyle(21);
  fBkg->SetMarkerSize(0.5);
  fBkg->SetMarkerColor(TColor::GetColor(255,0,0)); // red

  for (int i=0; i<noOfBins; i++) {
    fBkg->SetBinContent(i+1, fHisto->GetBinContent(bkgRange[0]+i+1));
  }
  fBkg->Draw("p0 9 hist same");

  // add lines
  // t0 line
  int t0Bin;
  switch (fDetectorTag) {
    case DETECTOR_TAG_FORWARD:
      t0Bin = fMsrHandler->GetMsrRunList()->at(fRunNo).fT0[0 + fAddRunNo * addRunOffset/2];
      break;
    case DETECTOR_TAG_BACKWARD:
      t0Bin = fMsrHandler->GetMsrRunList()->at(fRunNo).fT0[1 + fAddRunNo * addRunOffset/2];
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
  fFirstDataLine = new TLine((double)dataRange[0], 0.0, (double)dataRange[0], max);
  fFirstDataLine->SetLineStyle(3); // doted
  fFirstDataLine->SetLineColor(TColor::GetColor(0,0,255)); // blue
  fFirstDataLine->SetLineWidth(2);
  fFirstDataLine->Draw();

  fLastDataLine = new TLine((double)dataRange[1], 0.0, (double)dataRange[1], max);
  fLastDataLine->SetLineStyle(3); // doted
  fLastDataLine->SetLineColor(TColor::GetColor(0,0,255)); // blue
  fLastDataLine->SetLineWidth(2);
  fLastDataLine->Draw();

  // bkg lines
  fFirstBkgLine = new TLine((double)bkgRange[0], 0.0, (double)bkgRange[0], max);
  fFirstBkgLine->SetLineStyle(6); // _..._...
  fFirstBkgLine->SetLineColor(TColor::GetColor(255,0,0)); // red
  fFirstBkgLine->SetLineWidth(2);
  fFirstBkgLine->Draw();

  fLastBkgLine = new TLine((double)bkgRange[1], 0.0, (double)bkgRange[1], max);
  fLastBkgLine->SetLineStyle(6); // _..._...
  fLastBkgLine->SetLineColor(TColor::GetColor(255,0,0)); // red
  fLastBkgLine->SetLineWidth(2);
  fLastBkgLine->Draw();

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
  cout << endl << ">> in UnZoom ..." << endl;

  fHisto->GetXaxis()->UnZoom();

  fMainCanvas->Modified(); // needed that Update is actually working
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
