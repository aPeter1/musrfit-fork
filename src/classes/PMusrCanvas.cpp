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
  fTitlePad->AddText(title);
  fTitlePad->Draw();

  // data/theory pad
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

//--------------------------------------------------------------------------
// SetParameterList
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param paramList
 */
void PMusrCanvas::SetParameterList(PMsrParamList &paramList)
{
  fParamList = paramList;
}

//--------------------------------------------------------------------------
// SetTheoryList
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param theoryList
 */
void PMusrCanvas::SetTheoryList(PMsrLines &theoryList)
{
  fTheoryList = theoryList;
}

//--------------------------------------------------------------------------
// SetFunctionList
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param functionList
 */
void PMusrCanvas::SetFunctionList(PMsrLines &functionList)
{
  fFunctionList = functionList;
}

//--------------------------------------------------------------------------
// UpdateParamTheoryPad
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PMusrCanvas::UpdateParamTheoryPad()
{
  TString  str;
  char     cnum[128];
  int      maxLength = 0;
  Double_t ypos;
  int      idx;

  // add parameters ------------------------------------------------------------
  // get maximal parameter name string length
  for (unsigned int i=0; i<fParamList.size(); i++) {
    if (fParamList[i].fName.Length() > maxLength)
      maxLength = fParamList[i].fName.Length();
  }
  maxLength += 2;
  // add parameters to the pad
  for (unsigned int i=0; i<fParamList.size(); i++) {
    str = "";
    // parameter no
    str += fParamList[i].fNo;
    if (fParamList[i].fNo<10)
      str += "  ";
    else
      str += " ";
    // parameter name
    str += fParamList[i].fName;
    for (int j=0; j<maxLength-fParamList[i].fName.Length(); j++) // fill spaces
      str += " ";
    // parameter value
    if (round(fParamList[i].fValue)-fParamList[i].fValue==0)
      sprintf(cnum, "%.1lf", fParamList[i].fValue);
    else
      sprintf(cnum, "%.6lf", fParamList[i].fValue);
    str += cnum;
    for (int j=0; j<9-(int)strlen(cnum); j++) // fill spaces
      str += " ";
    str += " "; // to make sure that at least 1 space is placed
    // parameter error
    if (fParamList[i].fPosErrorPresent) { // minos was used
      if (round(fParamList[i].fStep)-fParamList[i].fStep==0)
        sprintf(cnum, "%.1lf", fParamList[i].fStep);
      else
        sprintf(cnum, "%.6lf", fParamList[i].fStep);
      str += cnum;
      str += "/";
      if (round(fParamList[i].fPosError)-fParamList[i].fPosError==0)
        sprintf(cnum, "%.1lf", fParamList[i].fPosError);
      else
        sprintf(cnum, "%.6lf", fParamList[i].fPosError);
      str += cnum;
    } else { // minos was not used
      if (round(fParamList[i].fStep)-fParamList[i].fStep==0)
        sprintf(cnum, "%.1lf", fParamList[i].fStep);
      else
        sprintf(cnum, "%.6lf", fParamList[i].fStep);
      str += cnum;
    }
    ypos = 0.925-i*0.025;
    fParameterTheoryPad->AddText(0.03, ypos, str.Data());
  }

  // add theory ------------------------------------------------------------
  ypos -= 0.025;
  for (unsigned int i=1; i<fTheoryList.size(); i++) {
    // remove comment if present
    str = fTheoryList[i].fLine;
    idx = str.Index("(");
    if (idx > 0) { // comment present
      str.Resize(idx-1);
      str.Resize(str.Strip().Length());
    }
    ypos -= 0.025;
    fParameterTheoryPad->AddText(0.03, ypos, str.Data());
  }

  fParameterTheoryPad->Draw();
  fMainCanvas->cd();
  fMainCanvas->Update();
}
