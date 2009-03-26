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

ClassImpQ(PMusrT0)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PMusrT0::PMusrT0()
{
  fMainCanvas = 0;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 * \param rawRunData
 * \param histoNo
 */
PMusrT0::PMusrT0(PRawRunData *rawRunData, unsigned int histoNo)
{
  cout << endl << "run Name = " << rawRunData->fRunName.Data() << ", histoNo = " << histoNo << endl;

  TString str = rawRunData->fRunName + TString(" : ");
  str += histoNo;

  // feed necessary objects

  // feed raw data histo
  Int_t noOfBins = rawRunData->fDataBin[histoNo].size();
  Double_t start = -0.5;
  Double_t end   = noOfBins + 0.5;
  fHisto = new TH1F("fHisto", str.Data(), noOfBins, start, end);
  fHisto->SetMarkerStyle(21);
  fHisto->SetMarkerSize(1);
  fHisto->SetMarkerColor(TColor::GetColor(0,0,0)); // black

  for (unsigned int i=0; i<rawRunData->fDataBin[histoNo].size(); i++) {
    fHisto->SetBinContent(i+1, rawRunData->fDataBin[histoNo][i]);
  }

  // generate canvas etc
  fMainCanvas = new TCanvas("fMainCanvas", str);

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

  fMainCanvas->cd();
  fMainCanvas->Show();

  fMainCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "PMusrT0",
                       this, "HandleCmdKey(Int_t,Int_t,Int_t,TObject*)");

  // draw histos etc
  fHisto->Draw("p");
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PMusrT0::~PMusrT0()
{
  if (fMainCanvas) {
    delete fMainCanvas;
    fMainCanvas = 0;
  }
  if (fHisto) {
    delete fHisto;
    fHisto = 0;
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

//   cout << ">this          " << this << endl;
//   cout << ">fMainCanvas   " << fMainCanvas << endl;
//   cout << ">selected      " << selected << endl;
//
//cout << "x : "  << (char)x << endl;
//cout << "px: "  << (char)fMainCanvas->GetEventX() << endl;

  // handle keys and popup menu entries
  if (x == 'q') { // quit
    Done(0);
  } else if (x == 'u') { // unzoom to the original range
    cout << endl << "will unzoom ..." << endl;
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
  // feed data range histo

  // feed background histo

  // add lines
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
