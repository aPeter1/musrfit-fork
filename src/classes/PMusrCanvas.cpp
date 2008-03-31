/***************************************************************************

  PMusrCanvas.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
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
using namespace std;

#include <TColor.h>

#include "PMusrCanvas.h"

ClassImpQ(PMusrCanvas)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PMusrCanvas::PMusrCanvas()
{
  fValid = false;

  fMainCanvas          = 0;
  fTitlePad            = 0;
  fDataTheoryPad       = 0;
  fParameterTheoryPad  = 0;
  fInfoPad             = 0;
  fKeyboardHandlerText = 0;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PMusrCanvas::PMusrCanvas(const char* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh)
{
  fValid = false;

  fMainCanvas          = 0;
  fTitlePad            = 0;
  fDataTheoryPad       = 0;
  fParameterTheoryPad  = 0;
  fInfoPad             = 0;
  fKeyboardHandlerText = 0;

  // invoke canvas
  fMainCanvas = new TCanvas("fMainCanvas", title, wtopx, wtopy, ww, wh);
cout << "fMainCanvas = " << fMainCanvas << endl;
  if (fMainCanvas == 0) {
    cout << endl << "PMusrCanvas::PMusrCanvas: **PANIC ERROR**: Couldn't invoke fMainCanvas";
    cout << endl;
    return;
  }

  // divide the canvas into 4 pads
  // title pad
  fTitlePad = new TPaveText(0.0, YTITLE, 1.0, 1.0, "NDC");
  if (fTitlePad == 0) {
    cout << endl << "PMusrCanvas::PMusrCanvas: **PANIC ERROR**: Couldn't invoke fTitlePad";
    cout << endl;
    return;
  }
  fTitlePad->SetFillColor(TColor::GetColor(255,255,255));
  fTitlePad->SetTextAlign(12); // middle, left
  fTitlePad->AddText("This is a title, this is a title, this is a title, this is a title, ...");
  fTitlePad->Draw();

  // data/theory pad
  fDataTheoryPad = new TPad("dataTheoryPad", "dataTheoryPad", 0.0, YINFO, XTHEO-0.02, YTITLE);
 fDataTheoryPad = new TPad("dataTheoryPad", "dataTheoryPad", 0.0, YINFO, XTHEO, YTITLE);
  if (fDataTheoryPad == 0) {
    cout << endl << "PMusrCanvas::PMusrCanvas: **PANIC ERROR**: Couldn't invoke fDataTheoryPad";
    cout << endl;
    return;
  }
  fDataTheoryPad->SetFillColor(TColor::GetColor(255,255,255));
  fDataTheoryPad->Draw();

  // parameter/theory pad
  fParameterTheoryPad = new TPaveText(XTHEO, 0.1, 1.0, YTITLE, "NDC");
  if (fParameterTheoryPad == 0) {
    cout << endl << "PMusrCanvas::PMusrCanvas: **PANIC ERROR**: Couldn't invoke fParameterTheoryPad";
    cout << endl;
    return;
  }
  fParameterTheoryPad->SetFillColor(TColor::GetColor(255,255,255));
  fParameterTheoryPad->SetTextAlign(13); // top, left
  fParameterTheoryPad->SetTextFont(102); // courier bold, scalable so that greek parameters will be plotted properly
  fParameterTheoryPad->AddText("parameter/theory pad ...");
  fParameterTheoryPad->AddText("1 alphaLR 0.9157  0.01075");
  fParameterTheoryPad->AddText("2 alphaTB 0.8777  0.007409");
  fParameterTheoryPad->AddText("3 asyS    0.1555  0.004631");
  fParameterTheoryPad->AddText("4 lambdaS 0.06726 0.01466");
  fParameterTheoryPad->AddText("5 field   7.444   0.1995");
  fParameterTheoryPad->AddText("6 phaseLR 18.3    4.1");
  fParameterTheoryPad->AddText("7 phaseTB -83.1   2.1");
  fParameterTheoryPad->AddText(" ");
  fParameterTheoryPad->AddText("asymmetry  3");
  fParameterTheoryPad->AddText("simplExpo  4           (#lambda)");
  fParameterTheoryPad->AddText("TFieldCos  map1 fun1   (#phi #nu)");
  fParameterTheoryPad->AddText(" ");
  fParameterTheoryPad->AddText("fun1 = par5 * 0.01355");
  fParameterTheoryPad->Draw();

  // info pad
  fInfoPad = new TPaveText(0.0, 0.0, 1.0, YINFO, "NDC");
  if (fInfoPad == 0) {
    cout << endl << "PMusrCanvas::PMusrCanvas: **PANIC ERROR**: Couldn't invoke fInfoPad";
    cout << endl;
    return;
  }
  fInfoPad->SetFillColor(TColor::GetColor(255,255,255));
  fInfoPad->SetTextAlign(12); // middle, left
  fInfoPad->AddText("musrfit info pad ...");
  fInfoPad->AddText(" this is the 1st info line ...");
  fInfoPad->AddText(" this is the 2nd info line ...");
  fInfoPad->AddText(" this is the 2nd info line ...");
  fInfoPad->Draw();

  // fKeyboardHandlerText Pad init
  fDataTheoryPad->cd();
  fKeyboardHandlerText = new TPaveText(0.1, 0.1, 0.3, 0.2, "rb");
  if (fKeyboardHandlerText == 0) {
    cout << endl << "PMusrCanvas::PMusrCanvas: **PANIC ERROR**: Couldn't invoke fKeyboardHandlerText";
    cout << endl;
    return;
  }
  fKeyboardHandlerText->AddText("none");
  fKeyboardHandlerText->Draw();
  fDataTheoryPad->Modified();

  fValid = true;

  fMainCanvas->cd();
  fMainCanvas->Show();
  fMainCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "PMusrCanvas", 
                       this, "HandleCmdKey(Int_t,Int_t,Int_t,TObject*)");

cout << "this                 " << this << endl;
cout << "fMainCanvas          " << fMainCanvas << endl;
cout << "fTitlePad            " << fTitlePad << endl;
cout << "fDataTheoryPad       " << fDataTheoryPad << endl;
cout << "fParameterTheoryPad  " << fParameterTheoryPad << endl;
cout << "fInfoPad             " << fInfoPad << endl;
cout << "fKeyboardHandlerText " << fKeyboardHandlerText << endl;

}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PMusrCanvas::~PMusrCanvas()
{
  // cleanup
  if (fKeyboardHandlerText) {
    delete fKeyboardHandlerText;
    fKeyboardHandlerText = 0;
  }
  if (fTitlePad) {
    delete fTitlePad;
    fTitlePad = 0;
  }
  if (fDataTheoryPad) {
    delete fDataTheoryPad;
    fDataTheoryPad = 0;
  }
  if (fParameterTheoryPad) {
    delete fParameterTheoryPad;
    fParameterTheoryPad = 0;
  }
  if (fInfoPad) {
    delete fInfoPad;
    fInfoPad = 0;
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
void PMusrCanvas::Done(Int_t status)
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
void PMusrCanvas::HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected)
{
  if (event != kKeyPress)
     return;

  cout << ">this          " << this << endl;
  cout << ">fMainCanvas   " << fMainCanvas << endl;
  cout << ">selected      " << selected << endl;

  cout << "px: "  << (char)fMainCanvas->GetEventX() << endl;

  TString str((Char_t)x);
  if (x == 'q') {
    Done(0);
  } else {
    if (fKeyboardHandlerText) {
      fDataTheoryPad->cd();
      delete fKeyboardHandlerText;
      fKeyboardHandlerText = 0;
      fKeyboardHandlerText = new TPaveText(0.1, 0.1, 0.3, 0.2, "rb");
      fKeyboardHandlerText->AddText(str.Data());
      fKeyboardHandlerText->Draw();
      fMainCanvas->cd();
    }
    fMainCanvas->Update();
  }
}
