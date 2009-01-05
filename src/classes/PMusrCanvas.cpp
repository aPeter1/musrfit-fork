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
#include <fstream>
using namespace std;

#include <TColor.h>
#include <TRandom.h>
#include <TROOT.h>
#include <TObjString.h>

#include "PMusrCanvas.h"
#include "PFourier.h"

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
  fDifferenceView  = false;
  fCurrentPlotView = PV_DATA;
  fPlotType = -1;
  fPlotNumber = -1;

  fImp   = 0;
  fBar   = 0;
  fPopupMain    = 0;
  fPopupSave    = 0;
  fPopupFourier = 0;

  fStyle               = 0;
  fMainCanvas          = 0;
  fTitlePad            = 0;
  fDataTheoryPad       = 0;
  fParameterTheoryPad  = 0;
  fInfoPad             = 0;

  InitFourier();

  fCurrentFourierPhase = fFourier.fPhaseIncrement;
  fCurrentFourierPhaseText = 0;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PMusrCanvas::PMusrCanvas(const int number, const char* title,
                         Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh) :
                         fPlotNumber(number)
{
  InitFourier();
  CreateStyle();
  InitMusrCanvas(title, wtopx, wtopy, ww, wh);

  fCurrentFourierPhase = fFourier.fPhaseIncrement;
  fCurrentFourierPhaseText = 0;
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PMusrCanvas::PMusrCanvas(const int number, const char* title,
                         Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
                         PMsrFourierStructure fourierDefault,
                         const PIntVector markerList, const PIntVector colorList) :
                         fPlotNumber(number), fFourier(fourierDefault),
                         fMarkerList(markerList), fColorList(colorList)
{
  CreateStyle();
  InitMusrCanvas(title, wtopx, wtopy, ww, wh);

  fCurrentFourierPhase = fFourier.fPhaseIncrement;
  fCurrentFourierPhaseText = 0;
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PMusrCanvas::~PMusrCanvas()
{
cout << "~PMusrCanvas() called. fMainCanvas name=" << fMainCanvas->GetName() << endl;
  // cleanup
  if (fStyle) {
    delete fStyle;
    fStyle = 0;
  }
  if (fTitlePad) {
    fTitlePad->Clear();
    delete fTitlePad;
    fTitlePad = 0;
  }
  if (fDataTheoryPad) {
    delete fDataTheoryPad;
    fDataTheoryPad = 0;
  }
  if (fParameterTheoryPad) {
    fParameterTheoryPad->Clear();
    delete fParameterTheoryPad;
    fParameterTheoryPad = 0;
  }
  if (fInfoPad) {
    fInfoPad->Clear();
    delete fInfoPad;
    fInfoPad = 0;
  }
  if (fMainCanvas) {
    delete fMainCanvas;
    fMainCanvas = 0;
  }
  if (fData.size() > 0) {
    for (unsigned int i=0; i<fData.size(); i++)
      CleanupDataSet(fData[i]);
    fData.clear();
  }
  if (fNonMusrData.size() > 0) {
    for (unsigned int i=0; i<fNonMusrData.size(); i++)
      CleanupDataSet(fNonMusrData[i]);
    fNonMusrData.clear();
  }
  if (fCurrentFourierPhaseText) {
    delete fCurrentFourierPhaseText;
    fCurrentFourierPhaseText = 0;
  }
}

//--------------------------------------------------------------------------
// InitFourier
//--------------------------------------------------------------------------
/**
 * <p>Initializes the Fourier structure.
 */
void PMusrCanvas::InitFourier()
{
  fFourier.fFourierBlockPresent = false;           // fourier block present
  fFourier.fUnits = FOURIER_UNIT_FIELD;            // fourier untis
  fFourier.fFourierPower = 0;                      // no zero padding
  fFourier.fApodization = FOURIER_APOD_NONE;       // no apodization
  fFourier.fPlotTag = FOURIER_PLOT_REAL_AND_IMAG;  // initial plot tag, plot real and imaginary part
  fFourier.fPhase = 0.0;                           // fourier phase 0°
  for (unsigned int i=0; i<2; i++) {
    fFourier.fRangeForPhaseCorrection[i] = -1.0;   // frequency range for phase correction, default: {-1, -1} = NOT GIVEN
    fFourier.fPlotRange[i] = -1.0;                 // fourier plot range, default: {-1, -1} = NOT GIVEN
  }
  fFourier.fPhaseIncrement = 1.0;                  // fourier phase increment
}

//--------------------------------------------------------------------------
// SetMsrHandler
//--------------------------------------------------------------------------
/**
 * <p>Keep the msr-handler object pointer and fill the Fourier structure if present.
 */
void PMusrCanvas::SetMsrHandler(PMsrHandler *msrHandler)
{
  fMsrHandler = msrHandler;

  // check if a fourier block is present in the msr-file, and if yes extract the given values
  if (fMsrHandler->GetMsrFourierList().fFourierBlockPresent) {
    fFourier.fFourierBlockPresent = true;
    if (fMsrHandler->GetMsrFourierList().fUnits != FOURIER_UNIT_NOT_GIVEN) {
      fFourier.fUnits = fMsrHandler->GetMsrFourierList().fUnits;
    }
    if (fMsrHandler->GetMsrFourierList().fFourierPower != -1) {
      fFourier.fFourierPower = fMsrHandler->GetMsrFourierList().fFourierPower;
    }
    if (fMsrHandler->GetMsrFourierList().fApodization != FOURIER_APOD_NOT_GIVEN) {
      fFourier.fApodization = fMsrHandler->GetMsrFourierList().fApodization;
    }
    if (fMsrHandler->GetMsrFourierList().fPlotTag != FOURIER_PLOT_NOT_GIVEN) {
      fFourier.fPlotTag = fMsrHandler->GetMsrFourierList().fPlotTag;
    }
    if (fMsrHandler->GetMsrFourierList().fPhase != -999.0) {
      fFourier.fPhase = fMsrHandler->GetMsrFourierList().fPhase;
    }
    if ((fMsrHandler->GetMsrFourierList().fRangeForPhaseCorrection[0] != -1.0) &&
        (fMsrHandler->GetMsrFourierList().fRangeForPhaseCorrection[1] != -1.0)) {
      fFourier.fRangeForPhaseCorrection[0] = fMsrHandler->GetMsrFourierList().fRangeForPhaseCorrection[0];
      fFourier.fRangeForPhaseCorrection[1] = fMsrHandler->GetMsrFourierList().fRangeForPhaseCorrection[1];
    }
    if ((fMsrHandler->GetMsrFourierList().fPlotRange[0] != -1.0) &&
        (fMsrHandler->GetMsrFourierList().fPlotRange[1] != -1.0)) {
      fFourier.fPlotRange[0] = fMsrHandler->GetMsrFourierList().fPlotRange[0];
      fFourier.fPlotRange[1] = fMsrHandler->GetMsrFourierList().fPlotRange[1];
    }
  }
}

//--------------------------------------------------------------------------
// CreateStyle
//--------------------------------------------------------------------------
/**
 * <p> Set styles for the canvas. Perhaps one could transfer them to the startup-file in the future.
 */
void PMusrCanvas::CreateStyle()
{
  fStyle = new TStyle("musrStyle", "musrStyle");
  fStyle->SetOptStat(0);  // no statistics options
  fStyle->SetOptTitle(0); // no title
  fStyle->cd();
}

//--------------------------------------------------------------------------
// InitMusrCanvas
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param title
 * \param wtopx
 * \param wtopy
 * \param ww
 * \param wh
 */
void PMusrCanvas::InitMusrCanvas(const char* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh)
{
  fValid = false;
  fDifferenceView  = false;
  fCurrentPlotView = PV_DATA;
  fPlotType = -1;

  fImp   = 0;
  fBar   = 0;
  fPopupMain    = 0;
  fPopupSave    = 0;
  fPopupFourier = 0;

  fMainCanvas          = 0;
  fTitlePad            = 0;
  fDataTheoryPad       = 0;
  fParameterTheoryPad  = 0;
  fInfoPad             = 0;

  // invoke canvas
  TString canvasName = TString("fMainCanvas");
  canvasName += fPlotNumber;
  fMainCanvas = new TCanvas(canvasName.Data(), title, wtopx, wtopy, ww, wh);
  if (fMainCanvas == 0) {
    cout << endl << "PMusrCanvas::PMusrCanvas: **PANIC ERROR**: Couldn't invoke " << canvasName.Data();
    cout << endl;
    return;
  }

  // add canvas menu
  fImp = (TRootCanvas*)fMainCanvas->GetCanvasImp(); 
  fBar = fImp->GetMenuBar(); 
  fPopupMain = fBar->AddPopup("&Musrfit");

  fPopupFourier = new TGPopupMenu();
  fPopupMain->AddPopup("&Fourier", fPopupFourier);
  fPopupFourier->AddEntry("Show Real", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL);
  fPopupFourier->AddEntry("Show Imag", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_IMAG);
  fPopupFourier->AddEntry("Show Real+Imag", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL_AND_IMAG);
  fPopupFourier->AddEntry("Show Power", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PWR);
  fPopupFourier->AddEntry("Show Phase", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE);
  fPopupFourier->AddSeparator();
  fPopupFourier->AddEntry("Phase +", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
  fPopupFourier->AddEntry("Phase -", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);

  fPopupMain->AddEntry("&Difference", P_MENU_ID_DIFFERENCE+P_MENU_PLOT_OFFSET*fPlotNumber);
  fPopupMain->AddSeparator();

  fPopupSave = new TGPopupMenu();
  fPopupSave->AddEntry("Save ascii", P_MENU_ID_SAVE_DATA+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_SAVE_ASCII);
  fPopupSave->AddEntry("Save db", P_MENU_ID_SAVE_DATA+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_SAVE_DB);

  fPopupMain->AddPopup("&Save Data", fPopupSave);
  fBar->MapSubwindows(); 
  fBar->Layout();

  fPopupMain->Connect("TGPopupMenu", "Activated(Int_t)", "PMusrCanvas", this, "HandleMenuPopup(Int_t)");

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
  fDataTheoryPad = new TPad("dataTheoryCanvas", "dataTheoryCanvas", 0.0, YINFO, XTHEO, YTITLE);
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
  fInfoPad = new TLegend(0.0, 0.0, 1.0, YINFO, "NDC");
  if (fInfoPad == 0) {
    cout << endl << "PMusrCanvas::PMusrCanvas: **PANIC ERROR**: Couldn't invoke fInfoPad";
    cout << endl;
    return;
  }
  fInfoPad->SetFillColor(TColor::GetColor(255,255,255));
  fInfoPad->SetTextAlign(12); // middle, left

  fValid = true;

  fMainCanvas->cd();
  fMainCanvas->Show();
  fMainCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "PMusrCanvas", 
                       this, "HandleCmdKey(Int_t,Int_t,Int_t,TObject*)");

// cout << "this                 " << this << endl;
// cout << "fMainCanvas          " << fMainCanvas << endl;
// cout << "fTitlePad            " << fTitlePad << endl;
// cout << "fDataTheoryPad       " << fDataTheoryPad << endl;
// cout << "fParameterTheoryPad  " << fParameterTheoryPad << endl;
// cout << "fInfoPad             " << fInfoPad << endl;
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

//   cout << ">this          " << this << endl;
//   cout << ">fMainCanvas   " << fMainCanvas << endl;
//   cout << ">selected      " << selected << endl;
// 
//cout << "x : "  << (char)x << endl;
//cout << "px: "  << (char)fMainCanvas->GetEventX() << endl;

  if (x == 'q') {
    Done(0);
  } else if (x == 'd') {
    HandleDifference();
  } else if (x == 'f') {
    if (fPlotType != MSR_PLOT_NON_MUSR) {
      HandleFourier(-1);
      cout << endl << ">> will show the Fourier transform, to be implemented yet." << endl;
    }
  } else if (x == '+') {
    IncrementFourierPhase();
  } else if (x == '-') {
    DecrementFourierPhase();
  } else {
    // do all the necessary stuff **TO BE DONE**
    fMainCanvas->Update();
  }
}

//--------------------------------------------------------------------------
// HandleMenuPopup (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::HandleMenuPopup(Int_t id)
{
  if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL) {
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(id);
    HandleFourier(FOURIER_PLOT_REAL);
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_IMAG) {
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(id);
    HandleFourier(FOURIER_PLOT_IMAG);
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL_AND_IMAG) {
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(id);
    HandleFourier(FOURIER_PLOT_REAL_AND_IMAG);
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PWR) {
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(id);
    HandleFourier(FOURIER_PLOT_POWER);
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE) {
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(id);
    HandleFourier(FOURIER_PLOT_PHASE);
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS) {
    IncrementFourierPhase();
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS) {
    DecrementFourierPhase();
  } else if (id == P_MENU_ID_DIFFERENCE+P_MENU_PLOT_OFFSET*fPlotNumber) {
    if (fPopupMain->IsEntryChecked(id))
      fPopupMain->UnCheckEntry(id);
    else
      fPopupMain->CheckEntry(id);
    HandleDifference();
  } else if (id == P_MENU_ID_SAVE_DATA+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_SAVE_ASCII) {
    SaveDataAscii();
  } else if (id == P_MENU_ID_SAVE_DATA+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_SAVE_DB) {
    SaveDataDb();
  }
}

//--------------------------------------------------------------------------
// LastCanvasClosed (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::LastCanvasClosed()
{
//  cout << endl << ">> in last canvas closed check ...";
  if (gROOT->GetListOfCanvases()->IsEmpty()) {
    Done(0);
  }
}

//--------------------------------------------------------------------------
// UpdateParamTheoryPad
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PMusrCanvas::UpdateParamTheoryPad()
{
  if (!fValid)
    return;

  TString  str;
  char     cnum[128];
  int      maxLength = 0;
  Double_t ypos;
  int      idx;

  // add parameters ------------------------------------------------------------
  PMsrParamList param = *fMsrHandler->GetMsrParamList();

  // get maximal parameter name string length
  for (unsigned int i=0; i<param.size(); i++) {
    if (param[i].fName.Length() > maxLength)
      maxLength = param[i].fName.Length();
  }
  maxLength += 2;
  // add parameters to the pad
  for (unsigned int i=0; i<param.size(); i++) {
    str = "";
    // parameter no
    str += param[i].fNo;
    if (param[i].fNo<10)
      str += "  ";
    else
      str += " ";
    // parameter name
    str += param[i].fName;
    for (int j=0; j<maxLength-param[i].fName.Length(); j++) // fill spaces
      str += " ";
    // parameter value
    if (round(param[i].fValue)-param[i].fValue==0)
      sprintf(cnum, "%.1lf", param[i].fValue);
    else
      sprintf(cnum, "%.6lf", param[i].fValue);
    str += cnum;
    for (int j=0; j<9-(int)strlen(cnum); j++) // fill spaces
      str += " ";
    str += " "; // to make sure that at least 1 space is placed
    // parameter error
    if (param[i].fPosErrorPresent) { // minos was used
      // calculate the arithmetic average of the pos. and neg. error
      double err;
      err = (param[i].fPosError - param[i].fStep) / 2.0;
      // check if the pos. and neg. error within 10%
      if ((fabs(fabs(param[i].fStep) - param[i].fPosError) < 0.1*fabs(param[i].fStep)) &&
          (fabs(fabs(param[i].fStep) - param[i].fPosError) < 0.1*param[i].fPosError)) {
        if (round(err)-err==0)
          sprintf(cnum, "%.1lf", err);
        else
          sprintf(cnum, "%.6lf", err);
      } else {
        if (round(err)-err==0)
          sprintf(cnum, "%.1lf!!", err);
        else
          sprintf(cnum, "%.6lf!!", err);
      }
      str += cnum;
    } else { // minos was not used
      if (round(param[i].fStep)-param[i].fStep==0)
        sprintf(cnum, "%.1lf", param[i].fStep);
      else
        sprintf(cnum, "%.6lf", param[i].fStep);
      str += cnum;
    }
    ypos = 0.925-i*0.025;
    fParameterTheoryPad->AddText(0.03, ypos, str.Data());
  }

  // add theory ------------------------------------------------------------
  ypos -= 0.025;
  PMsrLines theory = *fMsrHandler->GetMsrTheory();
  for (unsigned int i=1; i<theory.size(); i++) {
    // remove comment if present
    str = theory[i].fLine;
    idx = str.Index("(");
    if (idx > 0) { // comment present
      str.Resize(idx-1);
      str.Resize(str.Strip().Length());
    }
    ypos -= 0.025;
    fParameterTheoryPad->AddText(0.03, ypos, str.Data());
  }

  // add functions --------------------------------------------------------
  ypos -= 0.025;
  PMsrLines functions = *fMsrHandler->GetMsrFunctions();
  for (unsigned int i=1; i<functions.size(); i++) {
    ypos -= 0.025;
    fParameterTheoryPad->AddText(0.03, ypos, functions[i].fLine.Data());
  }

  fParameterTheoryPad->Draw();
  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// UpdateDataTheoryPad
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PMusrCanvas::UpdateDataTheoryPad()
{
/*
  // NonMusr axis titles
  TString xAxisTitle;
  TString yAxisTitle;
*/
  // some checks first
  unsigned int runNo;
  PMsrPlotStructure plotInfo = fMsrHandler->GetMsrPlotList()->at(fPlotNumber);
  PMsrRunList runs = *fMsrHandler->GetMsrRunList();

  fPlotType = plotInfo.fPlotType;
  for (unsigned int i=0; i<plotInfo.fRuns.size(); i++) {
    // first check that plot number is smaller than the maximal number of runs
    if ((int)plotInfo.fRuns[i].Re() > (int)runs.size()) {
      fValid = false;
      cout << endl << "PMusrCanvas::UpdateDataTheoryPad: **ERROR** run plot number " << (int)plotInfo.fRuns[i].Re() << " is larger than the number of runs " << runs.size();
      cout << endl;
      return;
    }
    // check that the plottype and the fittype do correspond
    runNo = (unsigned int)plotInfo.fRuns[i].Re()-1;
cout << endl << ">> runNo = " << runNo;
cout << endl;
    if (fPlotType != runs[runNo].fFitType) {
      fValid = false;
      cout << endl << "PMusrCanvas::UpdateDataTheoryPad: **ERROR** plottype = " << fPlotType << ", fittype = " << runs[runNo].fFitType << ", however they have to correspond!";
      cout << endl;
      return;
    }
/*
    // check if NonMusr type plot and if yes get x- and y-axis title
    if (fPlotType == MSR_PLOT_NON_MUSR) {
      xAxisTitle = fRunList->GetXAxisTitle(runs[runNo].fRunName);
      yAxisTitle = fRunList->GetYAxisTitle(runs[runNo].fRunName);
    }
*/
  }

  PRunData *data;
  for (unsigned int i=0; i<plotInfo.fRuns.size(); i++) {
    // get run data and create a histogram
    data = 0;
    runNo = (unsigned int)plotInfo.fRuns[i].Re()-1;
    // get data depending on the fittype
    switch (runs[runNo].fFitType) {
      case MSR_FITTYPE_SINGLE_HISTO:
        data = fRunList->GetSingleHisto(runNo, PRunListCollection::kRunNo);
        if (!data) { // something wrong
          fValid = false;
          // error message
          cout << endl << "PMusrCanvas::UpdateDataTheoryPad: **ERROR** couldn't obtain run no " << runNo << " for a single histogram plot";
          cout << endl;
          return;
        }
        // handle data
        HandleDataSet(i, runNo, data);
        break;
      case MSR_FITTYPE_ASYM:
        data = fRunList->GetAsymmetry(runNo, PRunListCollection::kRunNo);
        if (!data) { // something wrong
          fValid = false;
          // error message
          cout << endl << "PMusrCanvas::UpdateDataTheoryPad: **ERROR** couldn't obtain run no " << runNo << " for a asymmetry plot";
          cout << endl;
          return;
        }
        // handle data
        HandleDataSet(i, runNo, data);
        break;
      case MSR_FITTYPE_ASYM_RRF:
        data = fRunList->GetRRF(runNo, PRunListCollection::kRunNo);
        if (!data) { // something wrong
          fValid = false;
          // error message
          cout << endl << "PMusrCanvas::UpdateDataTheoryPad: **ERROR** couldn't obtain run no " << runNo << " for a RRF plot";
          cout << endl;
          return;
        }
        // handle data
        HandleDataSet(i, runNo, data);
        break;
      case MSR_FITTYPE_NON_MUSR:
        data = fRunList->GetNonMusr(runNo, PRunListCollection::kRunNo);
        if (!data) { // something wrong
          fValid = false;
          // error message
          cout << endl << "PMusrCanvas::UpdateDataTheoryPad: **ERROR** couldn't obtain run no " << runNo << " for a none musr data plot";
          cout << endl;
          return;
        }
        // handle data
        HandleNonMusrDataSet(i, runNo, data);
        // disable Fourier menus
        fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL);
        fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_IMAG);
        fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL_AND_IMAG);
        fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PWR);
        fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE);
        fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
        fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
        break;
      default:
        fValid = false;
        // error message
        cout << endl << "PMusrCanvas::UpdateDataTheoryPad: **ERROR** wrong plottype tag?!";
        cout << endl;
        return;
        break;
    }
  }

  // generate the histo plot
  PlotData();
}

//--------------------------------------------------------------------------
// UpdateInfoPad
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PMusrCanvas::UpdateInfoPad()
{
  if (!fValid)
    return;

  PMsrStatisticStructure statistic = *fMsrHandler->GetMsrStatistic();
  TString tstr, tsubstr;

  tstr = "musrfit: ";

  // get fit date
  tstr += statistic.fDate;
  tstr += TString(", ");

  // get chisq if not a max likelihood fit
  if (statistic.fChisq) { // chisq
    tstr += TString("chisq = ");
  } else { // max. likelihood
    tstr += TString("maxLH = ");
  }
  tstr += statistic.fMin;
  tstr += TString(" , NDF = ");
  tstr += statistic.fNdf;
  if (statistic.fChisq) { // chisq
    tstr += TString(" , chisq/NDF = ");
  } else { // max. likelihood
    tstr += TString(" , maxLH/NDF = ");
  }
  if (statistic.fNdf != 0) {
    tstr += statistic.fMin/statistic.fNdf;
  } else {
    tstr += TString("undefined");
  }

  fInfoPad->SetHeader(tstr);

  // get/set run plot info
  double dval;
  char   sval[128];
  unsigned int runNo;
  PMsrPlotStructure plotInfo = fMsrHandler->GetMsrPlotList()->at(fPlotNumber);
  PMsrRunList runs = *fMsrHandler->GetMsrRunList();
  for (unsigned int i=0; i<fData.size(); i++) {
    // run label = run_name/histo/T=0K/B=0G/E=0keV/...
    runNo = (unsigned int)plotInfo.fRuns[i].Re()-1;
    tstr  = runs[runNo].fRunName + TString(","); // run_name
    // histo info (depending on the fittype
    if (runs[runNo].fFitType == MSR_FITTYPE_SINGLE_HISTO) {
      tstr += TString("h:");
      tstr += runs[runNo].fForwardHistoNo;
      tstr += TString(",");
    } else if (runs[runNo].fFitType == MSR_FITTYPE_ASYM) {
      tstr += TString("h:");
      tstr += runs[runNo].fForwardHistoNo;
      tstr += TString("/");
      tstr += runs[runNo].fBackwardHistoNo;
      tstr += TString(",");
    }
    // temperature if present
    tstr += TString("T=");
    dval = fRunList->GetTemp(runs[runNo].fRunName);
    if (dval == -9.9e99) {
      tstr += TString("??,");
    } else {
      sprintf(sval, "%0.2lf", dval);
      tstr += TString(sval) + TString("(K),");
    }
    // field if present
    tstr += TString("B=");
    dval = fRunList->GetField(runs[runNo].fRunName);
    if (dval == -9.9e99) {
      tstr += TString("??,");
    } else {
      sprintf(sval, "%0.2lf", dval);
      tstr += TString(sval) + TString("(G),");
    }
    // energy if present
    tstr += TString("E=");
    dval = fRunList->GetEnergy(runs[runNo].fRunName);
//cout << endl << ">> dval = " << dval << " (Engery)";
    if (dval == -9.9e99) {
      tstr += TString("??,");
    } else {
      sprintf(sval, "%0.2lf", dval);
      tstr += TString(sval) + TString("(keV),");
    }
    // setup if present
    tstr += fRunList->GetSetup(runs[runNo].fRunName);
    // add entry
    fInfoPad->AddEntry(fData[i].data, tstr.Data(), "p");
  }

  fInfoPad->Draw();
  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// InitDataSet
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param dataSet
 */
void PMusrCanvas::InitDataSet(PMusrCanvasDataSet &dataSet)
{
  dataSet.data = 0;
  dataSet.dataFourierRe = 0;
  dataSet.dataFourierIm = 0;
  dataSet.dataFourierPwr = 0;
  dataSet.dataFourierPhase = 0;
  dataSet.theory = 0;
  dataSet.theoryFourierRe = 0;
  dataSet.theoryFourierIm = 0;
  dataSet.theoryFourierPwr = 0;
  dataSet.theoryFourierPhase = 0;
  dataSet.diff = 0;
  dataSet.diffFourierRe = 0;
  dataSet.diffFourierIm = 0;
  dataSet.diffFourierPwr = 0;
  dataSet.diffFourierPhase = 0;
}

//--------------------------------------------------------------------------
// InitDataSet
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param dataSet
 */
void PMusrCanvas::InitDataSet(PMusrCanvasNonMusrDataSet &dataSet)
{
  dataSet.data = 0;
  dataSet.dataFourierRe = 0;
  dataSet.dataFourierIm = 0;
  dataSet.dataFourierPwr = 0;
  dataSet.dataFourierPhase = 0;
  dataSet.theory = 0;
  dataSet.theoryFourierRe = 0;
  dataSet.theoryFourierIm = 0;
  dataSet.theoryFourierPwr = 0;
  dataSet.theoryFourierPhase = 0;
  dataSet.diff = 0;
  dataSet.diffFourierRe = 0;
  dataSet.diffFourierIm = 0;
  dataSet.diffFourierPwr = 0;
  dataSet.diffFourierPhase = 0;
}

//--------------------------------------------------------------------------
// CleanupDataSet
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param dataSet
 */
void PMusrCanvas::CleanupDataSet(PMusrCanvasDataSet &dataSet)
{
  if (dataSet.data) {
    delete dataSet.data;
    dataSet.data = 0;
  }
  if (dataSet.dataFourierRe) {
    delete dataSet.dataFourierRe;
    dataSet.dataFourierRe = 0;
  }
  if (dataSet.dataFourierIm) {
    delete dataSet.dataFourierIm;
    dataSet.dataFourierIm = 0;
  }
  if (dataSet.dataFourierPwr) {
    delete dataSet.dataFourierPwr;
    dataSet.dataFourierPwr = 0;
  }
  if (dataSet.dataFourierPhase) {
    delete dataSet.dataFourierPhase;
    dataSet.dataFourierPhase = 0;
  }
  if (dataSet.theory) {
    delete dataSet.theory;
    dataSet.theory = 0;
  }
  if (dataSet.theoryFourierRe) {
    delete dataSet.theoryFourierRe;
    dataSet.theoryFourierRe = 0;
  }
  if (dataSet.theoryFourierIm) {
    delete dataSet.theoryFourierIm;
    dataSet.theoryFourierIm = 0;
  }
  if (dataSet.theoryFourierPwr) {
    delete dataSet.theoryFourierPwr;
    dataSet.theoryFourierPwr = 0;
  }
  if (dataSet.theoryFourierPhase) {
    delete dataSet.theoryFourierPhase;
    dataSet.theoryFourierPhase = 0;
  }
  if (dataSet.diff) {
    delete dataSet.diff;
    dataSet.diff = 0;
  }
  if (dataSet.diffFourierRe) {
    delete dataSet.diffFourierRe;
    dataSet.diffFourierRe = 0;
  }
  if (dataSet.diffFourierIm) {
    delete dataSet.diffFourierIm;
    dataSet.diffFourierIm = 0;
  }
  if (dataSet.diffFourierPwr) {
    delete dataSet.diffFourierPwr;
    dataSet.diffFourierPwr = 0;
  }
  if (dataSet.diffFourierPhase) {
    delete dataSet.diffFourierPhase;
    dataSet.diffFourierPhase = 0;
  }
}

//--------------------------------------------------------------------------
// CleanupDataSet
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param dataSet
 */
void PMusrCanvas::CleanupDataSet(PMusrCanvasNonMusrDataSet &dataSet)
{
  if (dataSet.data) {
    delete dataSet.data;
    dataSet.data = 0;
  }
  if (dataSet.dataFourierRe) {
    delete dataSet.dataFourierRe;
    dataSet.dataFourierRe = 0;
  }
  if (dataSet.dataFourierIm) {
    delete dataSet.dataFourierIm;
    dataSet.dataFourierIm = 0;
  }
  if (dataSet.dataFourierPwr) {
    delete dataSet.dataFourierPwr;
    dataSet.dataFourierPwr = 0;
  }
  if (dataSet.dataFourierPhase) {
    delete dataSet.dataFourierPhase;
    dataSet.dataFourierPhase = 0;
  }
  if (dataSet.theory) {
    delete dataSet.theory;
    dataSet.theory = 0;
  }
  if (dataSet.theoryFourierRe) {
    delete dataSet.theoryFourierRe;
    dataSet.theoryFourierRe = 0;
  }
  if (dataSet.theoryFourierIm) {
    delete dataSet.theoryFourierIm;
    dataSet.theoryFourierIm = 0;
  }
  if (dataSet.theoryFourierPwr) {
    delete dataSet.theoryFourierPwr;
    dataSet.theoryFourierPwr = 0;
  }
  if (dataSet.theoryFourierPhase) {
    delete dataSet.theoryFourierPhase;
    dataSet.theoryFourierPhase = 0;
  }
  if (dataSet.diff) {
    delete dataSet.diff;
    dataSet.diff = 0;
  }
  if (dataSet.diffFourierRe) {
    delete dataSet.diffFourierRe;
    dataSet.diffFourierRe = 0;
  }
  if (dataSet.diffFourierIm) {
    delete dataSet.diffFourierIm;
    dataSet.diffFourierIm = 0;
  }
  if (dataSet.diffFourierPwr) {
    delete dataSet.diffFourierPwr;
    dataSet.diffFourierPwr = 0;
  }
  if (dataSet.diffFourierPhase) {
    delete dataSet.diffFourierPhase;
    dataSet.diffFourierPhase = 0;
  }
}

//--------------------------------------------------------------------------
// HandleDataSet
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runNo
 * \param data
 */
void PMusrCanvas::HandleDataSet(unsigned int plotNo, unsigned int runNo, PRunData *data)
{
  PMusrCanvasDataSet dataSet;
  TH1F *dataHisto;
  TH1F *theoHisto;

  TString name;
  double start;
  double end;

  InitDataSet(dataSet);

  // dataHisto -------------------------------------------------------------
  // create histo specific infos
  name  = fMsrHandler->GetMsrRunList()->at(runNo).fRunName + "_DataRunNo";
  name += (int)runNo;
  start = data->fDataTimeStart - data->fDataTimeStep/2.0;
  end   = data->fDataTimeStart + data->fValue.size()*data->fDataTimeStep + data->fDataTimeStep/2.0;

  // invoke histo
  dataHisto = new TH1F(name, name, data->fValue.size()+2, start, end);

  // fill histogram
  for (unsigned int i=0; i<data->fValue.size(); i++) {
    dataHisto->SetBinContent(i+1, data->fValue[i]);
    dataHisto->SetBinError(i+1, data->fError[i]);
  }

  // set marker and line color
  if (plotNo < fColorList.size()) {
    dataHisto->SetMarkerColor(fColorList[plotNo]);
    dataHisto->SetLineColor(fColorList[plotNo]);
  } else {
    TRandom rand(plotNo);
    Int_t color = TColor::GetColor((Int_t)rand.Integer(255), (Int_t)rand.Integer(255), (Int_t)rand.Integer(255));
    dataHisto->SetMarkerColor(color);
    dataHisto->SetLineColor(color);
  }
  // set marker size
  dataHisto->SetMarkerSize(1);
  // set marker type
  if (plotNo < fMarkerList.size()) {
    dataHisto->SetMarkerStyle(fMarkerList[plotNo]);
  } else {
    TRandom rand(plotNo);
    dataHisto->SetMarkerStyle(20+(Int_t)rand.Integer(10));
  }

  // theoHisto -------------------------------------------------------------
  // create histo specific infos
  name  = fMsrHandler->GetMsrRunList()->at(runNo).fRunName + "_TheoRunNo";
  name += (int)runNo;
  start = data->fTheoryTimeStart - data->fTheoryTimeStep/2.0;
  end   = data->fTheoryTimeStart + data->fTheory.size()*data->fTheoryTimeStep + data->fTheoryTimeStep/2.0;

//cout << endl << ">> start = " << start << ", end = " << end << endl;

  // invoke histo
  theoHisto = new TH1F(name, name, data->fTheory.size()+2, start, end);

  // fill histogram
  for (unsigned int i=0; i<data->fTheory.size(); i++) {
    theoHisto->SetBinContent(i+1, data->fTheory[i]);
  }

  // set the line color
  if (plotNo < fColorList.size()) {
    theoHisto->SetLineColor(fColorList[plotNo]);
  } else {
    TRandom rand(plotNo);
    Int_t color = TColor::GetColor((Int_t)rand.Integer(255), (Int_t)rand.Integer(255), (Int_t)rand.Integer(255));
    theoHisto->SetLineColor(color);
  }

  // fill handler list -----------------------------------------------------
  dataSet.data   = dataHisto;
  dataSet.theory = theoHisto;

  fData.push_back(dataSet);
}

//--------------------------------------------------------------------------
// HandleNonMusrDataSet
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runNo
 * \param data
 */
void PMusrCanvas::HandleNonMusrDataSet(unsigned int plotNo, unsigned int runNo, PRunData *data)
{
  PMusrCanvasNonMusrDataSet dataSet;
  TGraphErrors *dataHisto;
  TGraphErrors *theoHisto;

  InitDataSet(dataSet);

  // dataHisto -------------------------------------------------------------

  // invoke graph
  dataHisto = new TGraphErrors(data->fX.size());

  // fill graph
  for (unsigned int i=0; i<data->fX.size(); i++) {
    dataHisto->SetPoint(i, data->fX[i], data->fValue[i]);
    dataHisto->SetPointError(i, 0.0, data->fError[i]);
  }

  // set marker and line color
  if (plotNo < fColorList.size()) {
    dataHisto->SetMarkerColor(fColorList[plotNo]);
    dataHisto->SetLineColor(fColorList[plotNo]);
  } else {
    TRandom rand(plotNo);
    Int_t color = TColor::GetColor((Int_t)rand.Integer(255), (Int_t)rand.Integer(255), (Int_t)rand.Integer(255));
    dataHisto->SetMarkerColor(color);
    dataHisto->SetLineColor(color);
  }
  // set marker size
  dataHisto->SetMarkerSize(1);
  // set marker type
  if (plotNo < fMarkerList.size()) {
    dataHisto->SetMarkerStyle(fMarkerList[plotNo]);
  } else {
    TRandom rand(plotNo);
    dataHisto->SetMarkerStyle(20+(Int_t)rand.Integer(10));
  }

  // theoHisto -------------------------------------------------------------

  // invoke graph
  theoHisto = new TGraphErrors(data->fXTheory.size());

  // fill graph
  for (unsigned int i=0; i<data->fXTheory.size(); i++) {
    theoHisto->SetPoint(i, data->fXTheory[i], data->fTheory[i]);
    theoHisto->SetPointError(i, 0.0, 0.0);
  }

  // set the line color
  if (plotNo < fColorList.size()) {
    theoHisto->SetLineColor(fColorList[plotNo]);
  } else {
    TRandom rand(plotNo);
    Int_t color = TColor::GetColor((Int_t)rand.Integer(255), (Int_t)rand.Integer(255), (Int_t)rand.Integer(255));
    theoHisto->SetLineColor(color);
  }

  // fill handler list -----------------------------------------------------
  dataSet.data   = dataHisto;
  dataSet.theory = theoHisto;

  fNonMusrData.push_back(dataSet);
}

//--------------------------------------------------------------------------
// HandleDifference
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::HandleDifference()
{
  cout << endl << ">> will show the difference between the theory and the signal, to be implemented yet. fMainCanvas name = " << fMainCanvas->GetName();
  cout << endl;

  // toggle difference view flag
  fDifferenceView = !fDifferenceView;

  // difference plot wished hence feed difference data and plot them 
  if (fDifferenceView && (fCurrentPlotView == PV_DATA)) {
    // check if it is necessary to calculate diff data
    if ((fPlotType != MSR_PLOT_NON_MUSR) && (fData[0].diff == 0)) {
      TH1F *diffHisto;
      TString name;
      // loop over all histos
      for (unsigned int i=0; i<fData.size(); i++) {
        // create difference histos
        name = TString(fData[i].data->GetTitle()) + "_diff";
//cout << endl << ">> diff-name = " << name.Data() << endl;
        diffHisto = new TH1F(name, name, fData[i].data->GetNbinsX(),
                             fData[i].data->GetXaxis()->GetXmin(),
                             fData[i].data->GetXaxis()->GetXmax());

        // set marker and line color
        if (i < fColorList.size()) {
          diffHisto->SetMarkerColor(fColorList[i]);
          diffHisto->SetLineColor(fColorList[i]);
        } else {
          TRandom rand(i);
          Int_t color = TColor::GetColor((Int_t)rand.Integer(255), (Int_t)rand.Integer(255), (Int_t)rand.Integer(255));
          diffHisto->SetMarkerColor(color);
          diffHisto->SetLineColor(color);
        }
        // set marker size
        diffHisto->SetMarkerSize(1);
        // set marker type
        if (i < fMarkerList.size()) {
          diffHisto->SetMarkerStyle(fMarkerList[i]);
        } else {
          TRandom rand(i);
          diffHisto->SetMarkerStyle(20+(Int_t)rand.Integer(10));
        }

        // keep difference histo
        fData[i].diff = diffHisto;
        // calculate diff histo entry
        double value;
        for (int j=1; j<fData[i].data->GetNbinsX()-1; j++) {
          // set diff bin value
          value = CalculateDiff(fData[i].data->GetBinCenter(j),
                                fData[i].data->GetBinContent(j),
                                fData[i].theory);
          fData[i].diff->SetBinContent(j, value);
          // set error diff bin value
          value = fData[i].data->GetBinError(j);
          fData[i].diff->SetBinError(j, value);
        }
      }
    } else if ((fPlotType == MSR_PLOT_NON_MUSR) && (fNonMusrData[0].diff == 0)) {
      TGraphErrors *diffHisto;
      TString name;
      // loop over all histos
      for (unsigned int i=0; i<fNonMusrData.size(); i++) {
        // create difference histos
        diffHisto = new TGraphErrors(fNonMusrData[i].data->GetN());

        // create difference histos
        name = TString(fNonMusrData[i].data->GetTitle()) + "_diff";
//cout << endl << ">> diff-name = " << name.Data() << endl;
        diffHisto->SetNameTitle(name.Data(), name.Data());

        // set marker and line color
        if (i < fColorList.size()) {
          diffHisto->SetMarkerColor(fColorList[i]);
          diffHisto->SetLineColor(fColorList[i]);
        } else {
          TRandom rand(i);
          Int_t color = TColor::GetColor((Int_t)rand.Integer(255), (Int_t)rand.Integer(255), (Int_t)rand.Integer(255));
          diffHisto->SetMarkerColor(color);
          diffHisto->SetLineColor(color);
        }
        // set marker size
        diffHisto->SetMarkerSize(1);
        // set marker type
        if (i < fMarkerList.size()) {
          diffHisto->SetMarkerStyle(fMarkerList[i]);
        } else {
          TRandom rand(i);
          diffHisto->SetMarkerStyle(20+(Int_t)rand.Integer(10));
        }

        // keep difference histo
        fNonMusrData[i].diff = diffHisto;
        // calculate diff histo entry
        double value;
        double x, y;
        for (int j=0; j<fNonMusrData[i].data->GetN(); j++) {
          // set diff bin value
          fNonMusrData[i].data->GetPoint(j, x, y);
          value = CalculateDiff(x, y, fNonMusrData[i].theory);
          fNonMusrData[i].diff->SetPoint(j, x, value);
          // set error diff bin value
          value = fNonMusrData[i].data->GetErrorY(j);
          fNonMusrData[i].diff->SetPointError(j, 0.0, value);
        }
      }
    }
  }

  // switch back to the "normal" view
  if (!fDifferenceView) {
    // set current x-axis range
    if (fCurrentPlotView == PV_DATA)
      PlotData();
  } else {
    if (fCurrentPlotView == PV_DATA) {
      if (fPlotType != MSR_PLOT_NON_MUSR) {
        // set current x-axis range
        Int_t xminBin = fData[0].data->GetXaxis()->GetFirst(); // first bin of the zoomed range
        Int_t xmaxBin = fData[0].data->GetXaxis()->GetLast();  // last bin of the zoomed range
        Double_t xmin = fData[0].data->GetXaxis()->GetBinCenter(xminBin);
        Double_t xmax = fData[0].data->GetXaxis()->GetBinCenter(xmaxBin);
        fData[0].diff->GetXaxis()->SetRangeUser(xmin, xmax);
      }
      PlotDifference();
    }
  }
}

//--------------------------------------------------------------------------
// CalculateDiff
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param x x-value of the data
 * \param y y-value of the data
 * \param theo theory histogram
 */
double PMusrCanvas::CalculateDiff(const double x, const double y, TH1F *theo)
{
  Int_t bin = theo->FindBin(x);

  return y - theo->GetBinContent(bin);
}

//--------------------------------------------------------------------------
// CalculateDiff
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param x x-value of the data
 * \param y y-value of the data
 * \param theo theory error graph
 */
double PMusrCanvas::CalculateDiff(const double x, const double y, TGraphErrors *theo)
{
  Int_t bin = 0;
  Double_t xVal, yVal;

  bin = FindBin(x, theo);

  theo->GetPoint(bin, xVal, yVal);

//cout << endl << ">> bin=" << bin << ", x=" << xVal << " (xData=" << x << "), y=" << yVal;

  return y - yVal;
}

//--------------------------------------------------------------------------
// FindBin
//--------------------------------------------------------------------------
/**
 * <p>Analog to FindBin for histograms (TH1F) but here for TGraphErrors.
 *
 * \param x x-value of the data
 * \param graph TGraphErrors which should be seaarched
 */
Int_t PMusrCanvas::FindBin(const double x, TGraphErrors *graph)
{
  Int_t i, bin = 0;
  Double_t *xTheo = graph->GetX();

  // find proper bin of the graph
  for (i=0; i<graph->GetN(); i++) {
    if (*(xTheo+i) >= x) {
      bin = i;
      break;
    }
  }
  // in case it is the last point
  if (i == graph->GetN()) {
    bin = i;
  }

  return bin;
}

//--------------------------------------------------------------------------
// GetGlobalMaximum
//--------------------------------------------------------------------------
/**
 * <p>returns the global maximum of a histogram
 *
 * \param histo
 */
double PMusrCanvas::GetGlobalMaximum(TH1F* histo)
{
  if (histo == 0)
    return 0.0;

  double max = histo->GetBinContent(1);
  double binContent;
  for (int i=2; i < histo->GetNbinsX(); i++) {
    binContent = histo->GetBinContent(i);
    if (max < binContent)
      max = binContent;
  }

  return max;
}

//--------------------------------------------------------------------------
// GetGlobalMinimum
//--------------------------------------------------------------------------
/**
 * <p>returns the global minimum of a histogram
 *
 * \param histo
 */
double PMusrCanvas::GetGlobalMinimum(TH1F* histo)
{
  if (histo == 0)
    return 0.0;

  double min = histo->GetBinContent(1);
  double binContent;
  for (int i=2; i < histo->GetNbinsX(); i++) {
    binContent = histo->GetBinContent(i);
    if (min > binContent)
      min = binContent;
  }

  return min;
}

//--------------------------------------------------------------------------
// PlotData
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::PlotData()
{
  fDataTheoryPad->cd();

  if (fPlotType < 0) // plot type not defined
    return;

  if (fPlotType != MSR_PLOT_NON_MUSR) {
    if (fData.size() > 0) {
      fData[0].data->Draw("pe");
      // set time range
      Double_t xmin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin;
      Double_t xmax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmax;
      fData[0].data->GetXaxis()->SetRangeUser(xmin, xmax);
      // check if it is necessary to set the y-axis range
      Double_t ymin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin;
      Double_t ymax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmax;
      if ((ymin != -999.0) && (ymax != -999.0)) {
        fData[0].data->GetYaxis()->SetRangeUser(ymin, ymax);
      }
      // set x-axis label
      fData[0].data->GetXaxis()->SetTitle("time (#mus)");
      // set y-axis label
      TString yAxisTitle;
      PMsrRunList *runList = fMsrHandler->GetMsrRunList();
      switch (fPlotType) {
        case MSR_PLOT_SINGLE_HISTO:
          if (runList->at(0).fLifetimeCorrection) { // lifetime correction
            yAxisTitle = "asymmetry";
          } else { // no liftime correction
            yAxisTitle = "N(t) per nsec";
          }
          break;
        case MSR_PLOT_ASYM:
        case MSR_PLOT_ASYM_RRF:
          yAxisTitle = "asymmetry";
          break;
        default:
          yAxisTitle = "??";
          break;
      }
      fData[0].data->GetYaxis()->SetTitle(yAxisTitle.Data());
      // plot all remaining data
      for (unsigned int i=1; i<fData.size(); i++) {
        fData[i].data->Draw("pesame");
      }
      // plot all the theory
      for (unsigned int i=0; i<fData.size(); i++) {
        fData[i].theory->Draw("csame");
      }
    }
  } else { // fPlotType == MSR_PLOT_NO_MUSR
    PMsrRunList runs = *fMsrHandler->GetMsrRunList();
    PMsrPlotStructure plotInfo = fMsrHandler->GetMsrPlotList()->at(fPlotNumber);
    unsigned int runNo = (unsigned int)plotInfo.fRuns[0].Re()-1;
    TString xAxisTitle = fRunList->GetXAxisTitle(runs[runNo].fRunName);
    TString yAxisTitle = fRunList->GetYAxisTitle(runs[runNo].fRunName);

    if (fNonMusrData.size() > 0) {
      fNonMusrData[0].data->Draw("ap");
      // set x-range
      Double_t xmin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin;
      Double_t xmax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmax;
      fNonMusrData[0].data->GetXaxis()->SetRangeUser(xmin, xmax);
      // check if it is necessary to set the y-axis range
      Double_t ymin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin;
      Double_t ymax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmax;
      if ((ymin != -999.0) && (ymax != -999.0)) {
        fNonMusrData[0].data->GetYaxis()->SetRangeUser(ymin, ymax);
      }
      // set x-axis label
      fNonMusrData[0].data->GetXaxis()->SetTitle(xAxisTitle.Data());
      // set y-axis label
      fNonMusrData[0].data->GetYaxis()->SetTitle(yAxisTitle.Data());
      // plot all remaining data
      for (unsigned int i=1; i<fNonMusrData.size(); i++) {
        fNonMusrData[i].data->Draw("psame");
      }
      // plot all the theory
      for (unsigned int i=0; i<fNonMusrData.size(); i++) {
        fNonMusrData[i].theory->Draw("csame");
      }
    }
  }

  fDataTheoryPad->Update();

  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// PlotDifference
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::PlotDifference()
{
  fDataTheoryPad->cd();

  if (fPlotType < 0) // plot type not defined
    return;

  if (fPlotType != MSR_PLOT_NON_MUSR) {
cout << endl << ">> going to plot diff spectra ... (" << fData[0].diff->GetNbinsX() << ")" << endl;
    fData[0].diff->Draw("pe");
    // set x-axis label
    fData[0].diff->GetXaxis()->SetTitle("time (#mus)");
    // set y-axis label
    fData[0].diff->GetYaxis()->SetTitle("data-theory");
    // plot all remaining diff data
    for (unsigned int i=1; i<fData.size(); i++) {
      fData[i].diff->Draw("pesame");
    }
  } else { // fPlotType == MSR_PLOT_NON_MUSR
    PMsrRunList runs = *fMsrHandler->GetMsrRunList();
    PMsrPlotStructure plotInfo = fMsrHandler->GetMsrPlotList()->at(fPlotNumber);
    unsigned int runNo = (unsigned int)plotInfo.fRuns[0].Re()-1;
    TString xAxisTitle = fRunList->GetXAxisTitle(runs[runNo].fRunName);

    fNonMusrData[0].diff->Draw("ap");
    // set x-axis label
    fNonMusrData[0].diff->GetXaxis()->SetTitle(xAxisTitle.Data());
    // set y-axis label
    fNonMusrData[0].diff->GetYaxis()->SetTitle("data-theory");
    // plot all remaining diff data
    for (unsigned int i=1; i<fNonMusrData.size(); i++) {
      fNonMusrData[i].diff->Draw("psame");
    }
  }

  fDataTheoryPad->Update();

  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// PlotFourier
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::PlotFourier()
{
cout << endl << ">> in PlotFourier() ..." << endl;

  fDataTheoryPad->cd();

  if (fPlotType < 0) // plot type not defined
    return;

  if (fData.size() == 0) // no data to be plotted
    return;

  // define x-axis title
  TString xAxisTitle("");
  if (fFourier.fUnits == FOURIER_UNIT_FIELD) {
    xAxisTitle = TString("Field (G)");
  } else if (fFourier.fUnits == FOURIER_UNIT_FREQ) {
    xAxisTitle = TString("Frequency (MHz)");
  } else if (fFourier.fUnits == FOURIER_UNIT_CYCLES) {
    xAxisTitle = TString("Frequency (Mc/s)");
  } else {
    xAxisTitle = TString("??");
  }


  // plot data
  double min, max, binContent;
  switch (fCurrentPlotView) {
    case PV_FOURIER_REAL:     
//cout << endl << ">> fData[0].dataFourierRe->GetNbinsX() = " << fData[0].dataFourierRe->GetNbinsX();
      // plot first histo
      fData[0].dataFourierRe->Draw("pe");

      // set x-range
//cout << endl << ">> fPlotRange = " << fFourier.fPlotRange[0] << ", " << fFourier.fPlotRange[1];
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        min = fFourier.fPlotRange[0];
        max = fFourier.fPlotRange[1];
      } else {
        min = fData[0].dataFourierRe->GetBinLowEdge(1);
        max = fData[0].dataFourierRe->GetBinLowEdge(fData[0].dataFourierRe->GetNbinsX())+fData[0].dataFourierRe->GetBinWidth(1);
      }
//cout << endl << ">> x-range: min, max = " << min << ", " << max;
      fData[0].dataFourierRe->GetXaxis()->SetRangeUser(min, max);

      // set y-range
      // first find minimum/maximum of all histos
      min = GetGlobalMinimum(fData[0].dataFourierRe);
      max = GetGlobalMaximum(fData[0].dataFourierRe);
//cout << endl << ">> y-range: min, max = " << min << ", " << max;
      for (unsigned int i=1; i<fData.size(); i++) {
        binContent = GetGlobalMinimum(fData[i].dataFourierRe);
        if (binContent < min)
          min = binContent;
        binContent = GetGlobalMaximum(fData[i].dataFourierRe);
        if (binContent > max)
          max = binContent;
      }
      fData[0].dataFourierRe->GetYaxis()->SetRangeUser(1.05*min, 1.05*max);
//cout << endl << "-> min, max = " << min << ", " << max;

      // set x-axis title
      fData[0].dataFourierRe->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fData[0].dataFourierRe->GetYaxis()->SetTitle("Real Fourier");

      // plot all remaining data
      for (unsigned int i=1; i<fData.size(); i++) {
        fData[i].dataFourierRe->Draw("pesame");
      }

      // plot theories
      for (unsigned int i=0; i<fData.size(); i++) {
        fData[i].theoryFourierRe->Draw("esame");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_IMAG:
      // plot first histo
      fData[0].dataFourierIm->Draw("pe");

      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        min = fFourier.fPlotRange[0];
        max = fFourier.fPlotRange[1];
      } else {
        min = fData[0].dataFourierIm->GetBinLowEdge(1);
        max = fData[0].dataFourierIm->GetBinLowEdge(fData[0].dataFourierIm->GetNbinsX())+fData[0].dataFourierIm->GetBinWidth(1);
      }
      fData[0].dataFourierIm->GetXaxis()->SetRangeUser(min, max);

      // set y-range
      // first find minimum/maximum of all histos
      min = GetGlobalMinimum(fData[0].dataFourierIm);
      max = GetGlobalMaximum(fData[0].dataFourierIm);
      for (unsigned int i=1; i<fData.size(); i++) {
        binContent = GetGlobalMinimum(fData[i].dataFourierIm);
        if (binContent < min)
          min = binContent;
        binContent = GetGlobalMaximum(fData[i].dataFourierIm);
        if (binContent > max)
          max = binContent;
      }
      fData[0].dataFourierIm->GetYaxis()->SetRangeUser(1.05*min, 1.05*max);

      // set x-axis title
      fData[0].dataFourierIm->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fData[0].dataFourierIm->GetYaxis()->SetTitle("Imaginary Fourier");

      // plot all remaining data
      for (unsigned int i=1; i<fData.size(); i++) {
        fData[i].dataFourierIm->Draw("pesame");
      }

      // plot theories
      for (unsigned int i=0; i<fData.size(); i++) {
        fData[i].theoryFourierIm->Draw("esame");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_REAL_AND_IMAG:
      break;
    case PV_FOURIER_PWR:
      // plot first histo
      fData[0].dataFourierPwr->Draw("pe");

      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        min = fFourier.fPlotRange[0];
        max = fFourier.fPlotRange[1];
      } else {
        min = fData[0].dataFourierPwr->GetBinLowEdge(1);
        max = fData[0].dataFourierPwr->GetBinLowEdge(fData[0].dataFourierPwr->GetNbinsX())+fData[0].dataFourierPwr->GetBinWidth(1);
      }
      fData[0].dataFourierPwr->GetXaxis()->SetRangeUser(min, max);

      // set y-range
      // first find minimum/maximum of all histos
      min = GetGlobalMinimum(fData[0].dataFourierPwr);
      max = GetGlobalMaximum(fData[0].dataFourierPwr);
      for (unsigned int i=1; i<fData.size(); i++) {
        binContent = GetGlobalMinimum(fData[i].dataFourierPwr);
        if (binContent < min)
          min = binContent;
        binContent = GetGlobalMaximum(fData[i].dataFourierPwr);
        if (binContent > max)
          max = binContent;
      }
      fData[0].dataFourierPwr->GetYaxis()->SetRangeUser(1.05*min, 1.05*max);

      // set x-axis title
      fData[0].dataFourierPwr->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fData[0].dataFourierPwr->GetYaxis()->SetTitle("Power Fourier");

      // plot all remaining data
      for (unsigned int i=1; i<fData.size(); i++) {
        fData[i].dataFourierPwr->Draw("pesame");
      }

      // plot theories
      for (unsigned int i=0; i<fData.size(); i++) {
        fData[i].theoryFourierPwr->Draw("esame");
      }
      break;
    case PV_FOURIER_PHASE:
      // plot first histo
      fData[0].dataFourierPhase->Draw("pe");

      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        min = fFourier.fPlotRange[0];
        max = fFourier.fPlotRange[1];
      } else {
        min = fData[0].dataFourierPhase->GetBinLowEdge(1);
        max = fData[0].dataFourierPhase->GetBinLowEdge(fData[0].dataFourierPhase->GetNbinsX())+fData[0].dataFourierPhase->GetBinWidth(1);
      }
      fData[0].dataFourierPhase->GetXaxis()->SetRangeUser(min, max);

      // set y-range
      // first find minimum/maximum of all histos
      min = GetGlobalMinimum(fData[0].dataFourierPhase);
      max = GetGlobalMaximum(fData[0].dataFourierPhase);
      for (unsigned int i=1; i<fData.size(); i++) {
        binContent = GetGlobalMinimum(fData[i].dataFourierPhase);
        if (binContent < min)
          min = binContent;
        binContent = GetGlobalMaximum(fData[i].dataFourierPhase);
        if (binContent > max)
          max = binContent;
      }
      fData[0].dataFourierPhase->GetYaxis()->SetRangeUser(1.05*min, 1.05*max);

      // set x-axis title
      fData[0].dataFourierPhase->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fData[0].dataFourierPhase->GetYaxis()->SetTitle("Phase Fourier");

      // plot all remaining data
      for (unsigned int i=1; i<fData.size(); i++) {
        fData[i].dataFourierPhase->Draw("pesame");
      }

      // plot theories
      for (unsigned int i=0; i<fData.size(); i++) {
        fData[i].theoryFourierPhase->Draw("esame");
      }
      break;
    default:
      break;
  }

  fDataTheoryPad->Update();

  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// PlotFourierPhaseValue
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::PlotFourierPhaseValue()
{
  // check if phase TLatex object is present
  if (fCurrentFourierPhaseText) {
    delete fCurrentFourierPhaseText;
    fCurrentFourierPhaseText = 0;
  }

  double x, y;
  TString str;

   // plot Fourier phase
   str = TString("phase = ");
   str += fCurrentFourierPhase;   
   x = 0.7;
   y = 0.85;
   fCurrentFourierPhaseText = new TLatex();
   fCurrentFourierPhaseText->SetNDC(kTRUE);
   fCurrentFourierPhaseText->SetText(x, y, str.Data());
   fCurrentFourierPhaseText->SetTextFont(62);
   fCurrentFourierPhaseText->SetTextSize(0.03);

   fDataTheoryPad->cd();

   fCurrentFourierPhaseText->Draw();

   fDataTheoryPad->Update();
}

//--------------------------------------------------------------------------
// SaveDataAscii
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::SaveDataAscii()
{
  // generate output filename

  // in order to handle names with "." correctly this slightly odd data-filename generation
  TObjArray *tokens = fMsrHandler->GetFileName().Tokenize(".");
  TObjString *ostr;
  TString str = TString("");
  for (int i=0; i<tokens->GetEntries()-1; i++) {
    ostr = dynamic_cast<TObjString*>(tokens->At(i));
    str += ostr->GetString() + TString(".");
  }
  str += "data";

  if (tokens) {
    delete tokens;
    tokens = 0;
  }

  // open file
  ofstream fout;

  // open mlog-file
  fout.open(str.Data(), iostream::out);
  if (!fout.is_open()) {
    cout << endl << ">> PMusrCanvas::SaveDataAscii: **ERROR** couldn't open file " << str.Data() << " for writing." << endl;
    return;
  }

  // extract data
  Double_t time, xval, yval;
  Int_t xminBin;
  Int_t xmaxBin;
  Int_t theoBin;
  Double_t xmin;
  Double_t xmax;
  switch (fPlotType) {
    case MSR_PLOT_SINGLE_HISTO:
    case MSR_PLOT_ASYM:
    case MSR_PLOT_ASYM_RRF:
      if (fDifferenceView) { // difference view plot
        switch (fCurrentPlotView) {
          case PV_DATA:
            // write header
            fout << endl << "% time (us), ";
            for (unsigned int j=0; j<fData.size()-1; j++) {
              fout << "Diff" << j << ", eDiff" << j << ", ";
            }
            fout << "Diff" << fData.size()-1 << ", eDiff" << fData.size()-1;
            fout << endl;
            // get current x-range
            xminBin = fData[0].diff->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].diff->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].diff->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].diff->GetXaxis()->GetBinCenter(xmaxBin);
            // get difference data
            for (int i=1; i<fData[0].diff->GetNbinsX()-1; i++) {
              time = fData[0].diff->GetBinCenter(i); // get time
              if ((time < xmin) || (time > xmax))
                continue;
              fout << time << ", ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].diff->GetBinContent(i) << ", ";
                fout << fData[j].diff->GetBinError(i) << ", ";
              }
              // write last data set
              fout << fData[fData.size()-1].diff->GetBinContent(i) << ", ";
              fout << fData[fData.size()-1].diff->GetBinError(i);
              fout << endl;
            }
            break;
          case PV_FOURIER_REAL:
            break;
          case PV_FOURIER_IMAG:
            break;
          case PV_FOURIER_PWR:
            break;
          case PV_FOURIER_PHASE:
            break;
          default:
            break;
        }
      } else { // not a difference view plot
        switch (fCurrentPlotView) {
          case PV_DATA:
            // write header
            fout << endl << "% time (us), ";
            for (unsigned int j=0; j<fData.size()-1; j++) {
              fout << "Data" << j << ", eData" << j << ", Theo" << j << ", ";
            }
            fout << "Data" << fData.size()-1 << ", eData" << fData.size()-1 << ", Theo" << fData.size()-1;
            fout << endl;
            // get current x-range
            xminBin = fData[0].data->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].data->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].data->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].data->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=1; i<fData[0].data->GetNbinsX()-1; i++) {
              time = fData[0].data->GetBinCenter(i); // get time
              if ((time < xmin) || (time > xmax))
                continue;
              fout << time << ", ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].data->GetBinContent(i) << ", ";
                fout << fData[j].data->GetBinError(i) << ", ";
                theoBin = fData[j].theory->FindBin(time);
                fout << fData[j].theory->GetBinContent(theoBin) << ", ";
              }
              // write last data set
              fout << fData[fData.size()-1].data->GetBinContent(i) << ", ";
              fout << fData[fData.size()-1].data->GetBinError(i) << ", ";
              theoBin = fData[fData.size()-1].theory->FindBin(time);
              fout << fData[fData.size()-1].theory->GetBinContent(theoBin);
              fout << endl;
            }
            break;
          case PV_FOURIER_REAL:
            break;
          case PV_FOURIER_IMAG:
            break;
          case PV_FOURIER_PWR:
            break;
          case PV_FOURIER_PHASE:
            break;
          default:
            break;
        }
      }
      break;
    case MSR_PLOT_NON_MUSR:
      if (fDifferenceView) { // difference view plot
        switch (fCurrentPlotView) {
          case PV_DATA:
            // write header
            fout << "% " << fNonMusrData[0].diff->GetXaxis()->GetTitle() << ", ";
            for (unsigned int j=0; j<fNonMusrData.size()-1; j++) {
              fout << "Diff" << j << ", eDiff" << j << ", ";
            }
            fout << "Diff" << fNonMusrData.size()-1 << ", eDiff" << fNonMusrData.size()-1;
            fout << endl;
            // write data
            // get current x-range
            xminBin = fNonMusrData[0].diff->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fNonMusrData[0].diff->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fNonMusrData[0].diff->GetXaxis()->GetBinCenter(xminBin);
            xmax = fNonMusrData[0].diff->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=0; i<fNonMusrData[0].diff->GetN(); i++) {
              fNonMusrData[0].diff->GetPoint(i,xval,yval); // get values
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ", ";
              for (unsigned int j=0; j<fNonMusrData.size()-1; j++) {
                fNonMusrData[j].diff->GetPoint(i,xval,yval); // get values
                fout << yval << ", ";
                fout << fNonMusrData[j].diff->GetErrorY(i) << ", ";
              }
              // write last data set
              fNonMusrData[fNonMusrData.size()-1].diff->GetPoint(i,xval,yval); // get values
              fout << yval << ", ";
              fout << fNonMusrData[fNonMusrData.size()-1].diff->GetErrorY(i);
              fout << endl;
            }
            break;
          case PV_FOURIER_REAL:
            break;
          case PV_FOURIER_IMAG:
            break;
          case PV_FOURIER_PWR:
            break;
          case PV_FOURIER_PHASE:
            break;
          default:
            break;
        }
      } else { // not a difference view plot
        switch (fCurrentPlotView) {
          case PV_DATA:
            // write header
            fout << "% " << fNonMusrData[0].data->GetXaxis()->GetTitle() << ", ";
            for (unsigned int j=0; j<fNonMusrData.size()-1; j++) {
              fout << "Data" << j << ", eData" << j << ", Theo" << j << ", ";
            }
            fout << "Data" << fNonMusrData.size()-1 << ", eData" << fNonMusrData.size()-1 << ", Theo" << fNonMusrData.size()-1;
            fout << endl;
            // write data
            // get current x-range
            xminBin = fNonMusrData[0].data->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fNonMusrData[0].data->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fNonMusrData[0].data->GetXaxis()->GetBinCenter(xminBin);
            xmax = fNonMusrData[0].data->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=0; i<fNonMusrData[0].data->GetN(); i++) {
              fNonMusrData[0].data->GetPoint(i,xval,yval); // get values
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ", ";
              for (unsigned int j=0; j<fNonMusrData.size()-1; j++) {
                fNonMusrData[j].data->GetPoint(i,xval,yval); // get values
                fout << yval << ", ";
                fout << fNonMusrData[j].data->GetErrorY(i) << ", ";
                theoBin = FindBin(xval, fNonMusrData[j].theory);
                fNonMusrData[j].theory->GetPoint(theoBin,xval,yval); // get values
                fout << yval << ", ";
              }
              // write last data set
              fNonMusrData[fNonMusrData.size()-1].data->GetPoint(i,xval,yval); // get values
              fout << yval << ", ";
              fout << fNonMusrData[fNonMusrData.size()-1].data->GetErrorY(i) << ", ";
              theoBin = FindBin(xval, fNonMusrData[fNonMusrData.size()-1].theory);
              fNonMusrData[fNonMusrData.size()-1].theory->GetPoint(theoBin,xval,yval); // get values
              fout << yval;
              fout << endl;
            }
            break;
          case PV_FOURIER_REAL:
            break;
          case PV_FOURIER_IMAG:
            break;
          case PV_FOURIER_PWR:
            break;
          case PV_FOURIER_PHASE:
            break;
          default:
            break;
        }
      }
      break;
    default:
      break;
  }

  // close file
  fout.close();

  cout << endl << ">> Data windows saved in ascii format ..." << endl;
}

//--------------------------------------------------------------------------
// SaveDataDb
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::SaveDataDb()
{
  // generate output filename

  // in order to handle names with "." correctly this slightly odd data-filename generation
  TObjArray *tokens = fMsrHandler->GetFileName().Tokenize(".");
  TObjString *ostr;
  TString str = TString("");
  for (int i=0; i<tokens->GetEntries()-1; i++) {
    ostr = dynamic_cast<TObjString*>(tokens->At(i));
    str += ostr->GetString() + TString(".");
  }
  str += "db";

  if (tokens) {
    delete tokens;
    tokens = 0;
  }

  if (tokens) {
    delete tokens;
    tokens = 0;
  }

  // open file
  ofstream fout;

  // open mlog-file
  fout.open(str.Data(), iostream::out);
  if (!fout.is_open()) {
    cout << endl << ">> PMusrCanvas::SaveDataDb: **ERROR** couldn't open file " << str.Data() << " for writing." << endl;
    return;
  }

  // extract data
  Double_t time, xval, yval;
  Int_t xminBin;
  Int_t xmaxBin;
  Int_t theoBin;
  Double_t xmin;
  Double_t xmax;
  switch (fPlotType) {
    case MSR_PLOT_SINGLE_HISTO:
    case MSR_PLOT_ASYM:
    case MSR_PLOT_ASYM_RRF:
      if (fDifferenceView) { // difference view plot
        switch (fCurrentPlotView) {
          case PV_DATA:
            // write header
            fout << "TITLE" << endl;
            fout << "  " << fMsrHandler->GetMsrTitle()->Data() << endl;
            fout << "LABELS" << endl;
            fout << "  Time (us)" << endl;
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "  Diff" << j << endl;
            }
            fout << endl;
            fout << "DATA t ";
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "diff" << j << " ";
            }
            fout << endl;
            fout << "\\-e" << endl;

            // get current x-range
            xminBin = fData[0].diff->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].diff->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].diff->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].diff->GetXaxis()->GetBinCenter(xmaxBin);
            // get difference data
            for (int i=1; i<fData[0].diff->GetNbinsX()-1; i++) {
              time = fData[0].diff->GetBinCenter(i); // get time
              if ((time < xmin) || (time > xmax))
                continue;
              fout << time << ",,, ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].diff->GetBinContent(i) << ", ";
                fout << fData[j].diff->GetBinError(i) << ",, ";
              }
              // write last data set
              fout << fData[fData.size()-1].diff->GetBinContent(i) << ", ";
              fout << fData[fData.size()-1].diff->GetBinError(i) << ",,";
              fout << endl;
            }
            break;
          case PV_FOURIER_REAL:
            break;
          case PV_FOURIER_IMAG:
            break;
          case PV_FOURIER_PWR:
            break;
          case PV_FOURIER_PHASE:
            break;
          default:
            break;
        }
      } else { // not a difference view plot
        switch (fCurrentPlotView) {
          case PV_DATA:
            // write header
            fout << "TITLE" << endl;
            fout << "  " << fMsrHandler->GetMsrTitle()->Data() << endl;
            fout << "LABELS" << endl;
            fout << "  Time (us)" << endl;
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "  Data" << j << endl;
              fout << "  Theo" << j << endl;
            }
            fout << endl;
            fout << "DATA t ";
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "diff" << j << " " << "theo" << j << " ";
            }
            fout << endl;
            fout << "\\-e" << endl;

            // get current x-range
            xminBin = fData[0].data->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].data->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].data->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].data->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=1; i<fData[0].data->GetNbinsX()-1; i++) {
              time = fData[0].data->GetBinCenter(i); // get time
              if ((time < xmin) || (time > xmax))
                continue;
              fout << time << ",,, ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].data->GetBinContent(i) << ", ";
                fout << fData[j].data->GetBinError(i) << ",, ";
                theoBin = fData[j].theory->FindBin(time);
                fout << fData[j].theory->GetBinContent(theoBin) << ",,, ";
              }
              // write last data set
              fout << fData[fData.size()-1].data->GetBinContent(i) << ", ";
              fout << fData[fData.size()-1].data->GetBinError(i) << ",, ";
              theoBin = fData[fData.size()-1].theory->FindBin(time);
              fout << fData[fData.size()-1].theory->GetBinContent(theoBin) << ",,, ";
              fout << endl;
            }
            break;
          case PV_FOURIER_REAL:
            break;
          case PV_FOURIER_IMAG:
            break;
          case PV_FOURIER_PWR:
            break;
          case PV_FOURIER_PHASE:
            break;
          default:
            break;
        }
      }
      break;
    case MSR_PLOT_NON_MUSR:
      if (fDifferenceView) { // difference view plot
        switch (fCurrentPlotView) {
          case PV_DATA:
            // write header
            fout << "TITLE" << endl;
            fout << "  " << fMsrHandler->GetMsrTitle()->Data() << endl;
            fout << "LABELS" << endl;
            fout << "  Time (us)" << endl;
            for (unsigned int j=0; j<fNonMusrData.size(); j++) {
              fout << "  Diff" << j << endl;
            }
            fout << endl;
            fout << "DATA t ";
            for (unsigned int j=0; j<fNonMusrData.size(); j++) {
              fout << "diff" << j << " ";
            }
            fout << endl;
            fout << "\\-e" << endl;

            // write data
            // get current x-range
            xminBin = fNonMusrData[0].diff->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fNonMusrData[0].diff->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fNonMusrData[0].diff->GetXaxis()->GetBinCenter(xminBin);
            xmax = fNonMusrData[0].diff->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=0; i<fNonMusrData[0].diff->GetN(); i++) {
              fNonMusrData[0].diff->GetPoint(i,xval,yval); // get values
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ",,, ";
              for (unsigned int j=0; j<fNonMusrData.size()-1; j++) {
                fNonMusrData[j].diff->GetPoint(i,xval,yval); // get values
                fout << yval << ", ";
                fout << fNonMusrData[j].diff->GetErrorY(i) << ",, ";
              }
              // write last data set
              fNonMusrData[fNonMusrData.size()-1].diff->GetPoint(i,xval,yval); // get values
              fout << yval << ", ";
              fout << fNonMusrData[fNonMusrData.size()-1].diff->GetErrorY(i) << ",,";
              fout << endl;
            }
            break;
          case PV_FOURIER_REAL:
            break;
          case PV_FOURIER_IMAG:
            break;
          case PV_FOURIER_PWR:
            break;
          case PV_FOURIER_PHASE:
            break;
          default:
            break;
        }
      } else { // not a difference view plot
        switch (fCurrentPlotView) {
          case PV_DATA:
            // write header
            fout << "TITLE" << endl;
            fout << "  " << fMsrHandler->GetMsrTitle()->Data() << endl;
            fout << "LABELS" << endl;
            fout << "  Time (us)" << endl;
            for (unsigned int j=0; j<fNonMusrData.size(); j++) {
              fout << "  Data" << j << endl;
              fout << "  Theo" << j << endl;
            }
            fout << endl;
            fout << "DATA t ";
            for (unsigned int j=0; j<fNonMusrData.size(); j++) {
              fout << "diff" << j << " " << "theo" << j << " ";
            }
            fout << endl;
            fout << "\\-e" << endl;

            // write data
            // get current x-range
            xminBin = fNonMusrData[0].data->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fNonMusrData[0].data->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fNonMusrData[0].data->GetXaxis()->GetBinCenter(xminBin);
            xmax = fNonMusrData[0].data->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=0; i<fNonMusrData[0].data->GetN(); i++) {
              fNonMusrData[0].data->GetPoint(i,xval,yval); // get values
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ",,, ";
              for (unsigned int j=0; j<fNonMusrData.size()-1; j++) {
                fNonMusrData[j].data->GetPoint(i,xval,yval); // get values
                fout << yval << ", ";
                fout << fNonMusrData[j].data->GetErrorY(i) << ",, ";
                theoBin = FindBin(xval, fNonMusrData[j].theory);
                fNonMusrData[j].theory->GetPoint(theoBin,xval,yval); // get values
                fout << yval << ",,,";
              }
              // write last data set
              fNonMusrData[fNonMusrData.size()-1].data->GetPoint(i,xval,yval); // get values
              fout << yval << ", ";
              fout << fNonMusrData[fNonMusrData.size()-1].data->GetErrorY(i) << ",, ";
              theoBin = FindBin(xval, fNonMusrData[fNonMusrData.size()-1].theory);
              fNonMusrData[fNonMusrData.size()-1].theory->GetPoint(theoBin,xval,yval); // get values
              fout << yval << ",,,";
              fout << endl;
            }
            break;
          case PV_FOURIER_REAL:
            break;
          case PV_FOURIER_IMAG:
            break;
          case PV_FOURIER_PWR:
            break;
          case PV_FOURIER_PHASE:
            break;
          default:
            break;
        }
      }
      break;
    default:
      break;
  }

  // close file
  fout.close();

  cout << endl << ">> Data windows saved in db format ..." << endl;
}

//--------------------------------------------------------------------------
// HandleFourier
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param tag
 */
void PMusrCanvas::HandleFourier(int tag)
{
  // if fourier was invoked via the 'f' cmd key, take the default plot tag
  if (tag == -1) { // called via cmd key 'f'
    tag = fFourier.fPlotTag;
  }


  // if the current view is a data plot, fourier needs to be calculated
  if (fCurrentPlotView == PV_DATA) {
    if (!fDifferenceView) { // data view
      // delete fourier components
      for (unsigned int i=0; i<fData.size(); i++) {
        if (fData[i].dataFourierRe != 0) {
          delete fData[i].dataFourierRe;
          fData[i].dataFourierRe = 0;
        }
        if (fData[i].dataFourierIm != 0) {
          delete fData[i].dataFourierIm;
          fData[i].dataFourierIm = 0;
        }
        if (fData[i].dataFourierPwr != 0) {
          delete fData[i].dataFourierPwr;
          fData[i].dataFourierPwr = 0;
        }
        if (fData[i].dataFourierPhase != 0) {
          delete fData[i].dataFourierPhase;
          fData[i].dataFourierPhase = 0;
        }
        if (fData[i].theoryFourierRe != 0) {
          delete fData[i].theoryFourierRe;
          fData[i].theoryFourierRe = 0;
        }
        if (fData[i].theoryFourierIm != 0) {
          delete fData[i].theoryFourierIm;
          fData[i].theoryFourierIm = 0;
        }
        if (fData[i].theoryFourierPwr != 0) {
          delete fData[i].theoryFourierPwr;
          fData[i].theoryFourierPwr = 0;
        }
        if (fData[i].theoryFourierPhase != 0) {
          delete fData[i].theoryFourierPhase;
          fData[i].theoryFourierPhase = 0;
        }
      }

      int bin;
      bin = fData[0].data->GetXaxis()->GetFirst();
      double startTime = fData[0].data->GetBinCenter(bin);
      bin = fData[0].data->GetXaxis()->GetLast();
      double endTime   = fData[0].data->GetBinCenter(bin);
//cout << endl << ">> startTime = " << startTime << ", endTime = " << endTime << endl;
      for (unsigned int i=0; i<fData.size(); i++) {
        // calculate fourier transform of the data
        PFourier fourierData(fData[i].data, fFourier.fUnits, startTime, endTime, fFourier.fFourierPower);
        if (!fourierData.IsValid()) {
          cout << endl << "**SEVERE ERROR** PMusrCanvas::HandleFourier: couldn't invoke PFourier to calculate the Fourier data ..." << endl;
          return;
        }
        fourierData.Transform(fFourier.fApodization);
        double scale;
        scale = sqrt(fData[0].data->GetBinWidth(1)/(endTime-startTime));
cout << endl << ">> data scale = " << scale;
        // get real part of the data
        fData[i].dataFourierRe = fourierData.GetRealFourier(scale);
//cout << endl << ">> i: " << i << ", fData[i].dataFourierRe = " << fData[i].dataFourierRe;
        // get imaginary part of the data
        fData[i].dataFourierIm = fourierData.GetImaginaryFourier(scale);
        // get power part of the data
        fData[i].dataFourierPwr = fourierData.GetPowerFourier(scale);
        // get phase part of the data
        fData[i].dataFourierPhase = fourierData.GetPhaseFourier();

        // set marker and line color
        fData[i].dataFourierRe->SetMarkerColor(fData[i].data->GetMarkerColor());
        fData[i].dataFourierRe->SetLineColor(fData[i].data->GetLineColor());
        fData[i].dataFourierIm->SetMarkerColor(fData[i].data->GetMarkerColor());
        fData[i].dataFourierIm->SetLineColor(fData[i].data->GetLineColor());
        fData[i].dataFourierPwr->SetMarkerColor(fData[i].data->GetMarkerColor());
        fData[i].dataFourierPwr->SetLineColor(fData[i].data->GetLineColor());
        fData[i].dataFourierPhase->SetMarkerColor(fData[i].data->GetMarkerColor());
        fData[i].dataFourierPhase->SetLineColor(fData[i].data->GetLineColor());

        // set marker size
        fData[i].dataFourierRe->SetMarkerSize(1);
        fData[i].dataFourierIm->SetMarkerSize(1);
        fData[i].dataFourierPwr->SetMarkerSize(1);
        fData[i].dataFourierPhase->SetMarkerSize(1);
        // set marker type
        fData[i].dataFourierRe->SetMarkerStyle(fData[i].data->GetMarkerStyle());
        fData[i].dataFourierIm->SetMarkerStyle(fData[i].data->GetMarkerStyle());
        fData[i].dataFourierPwr->SetMarkerStyle(fData[i].data->GetMarkerStyle());
        fData[i].dataFourierPhase->SetMarkerStyle(fData[i].data->GetMarkerStyle());

        // calculate fourier transform of the theory
        int powerPad = (int)round(log((endTime-startTime)/fData[i].theory->GetBinWidth(1))/log(2))+3;
cout << endl << ">> powerPad = " << powerPad;
        PFourier fourierTheory(fData[i].theory, fFourier.fUnits, startTime, endTime, powerPad);
        if (!fourierTheory.IsValid()) {
          cout << endl << "**SEVERE ERROR** PMusrCanvas::HandleFourier: couldn't invoke PFourier to calculate the Fourier theory ..." << endl;
          return;
        }
        fourierTheory.Transform(fFourier.fApodization);
        scale = sqrt(fData[0].theory->GetBinWidth(1)/(endTime-startTime)*fData[0].theory->GetBinWidth(1)/fData[0].data->GetBinWidth(1));
cout << endl << ">> theory scale = " << scale << ", data.res/theory.res = " << fData[0].theory->GetBinWidth(1)/fData[0].data->GetBinWidth(1);
        // get real part of the data
        fData[i].theoryFourierRe = fourierTheory.GetRealFourier(scale);
//cout << endl << ">> i: " << i << ", fData[i].dataFourierRe = " << fData[i].dataFourierRe;
        // get imaginary part of the data
        fData[i].theoryFourierIm = fourierTheory.GetImaginaryFourier(scale);
        // get power part of the data
        fData[i].theoryFourierPwr = fourierTheory.GetPowerFourier(scale);
        // get phase part of the data
        fData[i].theoryFourierPhase = fourierTheory.GetPhaseFourier();

        // set line colors for the theory
        fData[i].theoryFourierRe->SetLineColor(fData[i].theory->GetLineColor());
        fData[i].theoryFourierIm->SetLineColor(fData[i].theory->GetLineColor());
        fData[i].theoryFourierPwr->SetLineColor(fData[i].theory->GetLineColor());
        fData[i].theoryFourierPhase->SetLineColor(fData[i].theory->GetLineColor());
      }

      // apply global phase
      if (fFourier.fPhase != 0.0) {
        double re, im;
        const double cp = TMath::Cos(fFourier.fPhase/180.0*TMath::Pi());
        const double sp = TMath::Sin(fFourier.fPhase/180.0*TMath::Pi());

        fCurrentFourierPhase = fFourier.fPhase;

        for (unsigned int i=0; i<fData.size(); i++) { // loop over all data sets
          if ((fData[i].dataFourierRe != 0) && (fData[i].dataFourierIm != 0)) {
            for (int j=0; j<fData[i].dataFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
              // calculate new fourier data set value
              re = fData[i].dataFourierRe->GetBinContent(j) * cp + fData[i].dataFourierIm->GetBinContent(j) * sp;
              im = fData[i].dataFourierIm->GetBinContent(j) * cp - fData[i].dataFourierRe->GetBinContent(j) * sp;
              // overwrite fourier data set value
              fData[i].dataFourierRe->SetBinContent(j, re);
              fData[i].dataFourierIm->SetBinContent(j, im);
            }
          }
          if ((fData[i].theoryFourierRe != 0) && (fData[i].theoryFourierIm != 0)) {
            for (int j=0; j<fData[i].theoryFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
              // calculate new fourier data set value
              re = fData[i].theoryFourierRe->GetBinContent(j) * cp + fData[i].theoryFourierIm->GetBinContent(j) * sp;
              im = fData[i].theoryFourierIm->GetBinContent(j) * cp - fData[i].theoryFourierRe->GetBinContent(j) * sp;
              // overwrite fourier data set value
              fData[i].theoryFourierRe->SetBinContent(j, re);
              fData[i].theoryFourierIm->SetBinContent(j, im);
            }
          }
        }
      }
    } else { // calculate diff fourier
      // delete fourier components
      for (unsigned int i=0; i<fData.size(); i++) {
        if (fData[i].diffFourierRe != 0) {
          delete fData[i].diffFourierRe;
          fData[i].diffFourierRe = 0;
        }
        if (fData[i].diffFourierIm != 0) {
          delete fData[i].diffFourierIm;
          fData[i].diffFourierIm = 0;
        }
        if (fData[i].diffFourierPwr != 0) {
          delete fData[i].diffFourierPwr;
          fData[i].diffFourierPwr = 0;
        }
        if (fData[i].diffFourierPhase != 0) {
          delete fData[i].diffFourierPhase;
          fData[i].diffFourierPhase = 0;
        }
      }
    }
  }

  int plotView = -1;
  switch (tag) { // called via popup menu
    case FOURIER_PLOT_REAL:
      plotView = PV_FOURIER_REAL;
      cout << endl << ">> will handle Real Part Fourier ..." << endl;
      break;
    case FOURIER_PLOT_IMAG:
      plotView = PV_FOURIER_IMAG;
      cout << endl << ">> will handle Imaginary Part Fourier ..." << endl;
      break;
    case FOURIER_PLOT_REAL_AND_IMAG:
      plotView = PV_FOURIER_REAL_AND_IMAG;
      cout << endl << ">> will handle Real+Imaginary Part Fourier ..." << endl;
      break;
    case FOURIER_PLOT_POWER:
      plotView = PV_FOURIER_PWR;
      cout << endl << ">> will handle Power Fourier ..." << endl;
      break;
    case FOURIER_PLOT_PHASE:
      plotView = PV_FOURIER_PHASE;
      cout << endl << ">> will handle Phase Fourier ..." << endl;
      break;
    default:
      break;
  }

  if (plotView == fCurrentPlotView) { // twice checked the same -> switch back to data view
    fCurrentPlotView = PV_DATA;
    // uncheck fourier menu entries
    fPopupFourier->UnCheckEntries();
    // plot data
    PlotData();
  } else { // plot fourier
    fCurrentPlotView = plotView;
    PlotFourier();
  }
}

//--------------------------------------------------------------------------
// IncrementFourierPhase (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param tag
 */
void PMusrCanvas::IncrementFourierPhase()
{
  double re, im;
  const double cp = TMath::Cos(fFourier.fPhaseIncrement/180.0*TMath::Pi());
  const double sp = TMath::Sin(fFourier.fPhaseIncrement/180.0*TMath::Pi());

  fCurrentFourierPhase += fFourier.fPhaseIncrement;
  PlotFourierPhaseValue();

  for (unsigned int i=0; i<fData.size(); i++) { // loop over all data sets
    if ((fData[i].dataFourierRe != 0) && (fData[i].dataFourierIm != 0)) {
      for (int j=0; j<fData[i].dataFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
        // calculate new fourier data set value
        re = fData[i].dataFourierRe->GetBinContent(j) * cp + fData[i].dataFourierIm->GetBinContent(j) * sp;
        im = fData[i].dataFourierIm->GetBinContent(j) * cp - fData[i].dataFourierRe->GetBinContent(j) * sp;
        // overwrite fourier data set value
        fData[i].dataFourierRe->SetBinContent(j, re);
        fData[i].dataFourierIm->SetBinContent(j, im);
      }
    }
    if ((fData[i].theoryFourierRe != 0) && (fData[i].theoryFourierIm != 0)) {
      for (int j=0; j<fData[i].theoryFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
        // calculate new fourier data set value
        re = fData[i].theoryFourierRe->GetBinContent(j) * cp + fData[i].theoryFourierIm->GetBinContent(j) * sp;
        im = fData[i].theoryFourierIm->GetBinContent(j) * cp - fData[i].theoryFourierRe->GetBinContent(j) * sp;
        // overwrite fourier data set value
        fData[i].theoryFourierRe->SetBinContent(j, re);
        fData[i].theoryFourierIm->SetBinContent(j, im);
      }
    }
  }
}

//--------------------------------------------------------------------------
// DecrementFourierPhase (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param tag
 */
void PMusrCanvas::DecrementFourierPhase()
{
  double re, im;
  const double cp = TMath::Cos(fFourier.fPhaseIncrement/180.0*TMath::Pi());
  const double sp = TMath::Sin(fFourier.fPhaseIncrement/180.0*TMath::Pi());

  fCurrentFourierPhase -= fFourier.fPhaseIncrement;
  PlotFourierPhaseValue();

  for (unsigned int i=0; i<fData.size(); i++) { // loop over all data sets
    if ((fData[i].dataFourierRe != 0) && (fData[i].dataFourierIm != 0)) {
      for (int j=0; j<fData[i].dataFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
        // calculate new fourier data set value
        re = fData[i].dataFourierRe->GetBinContent(j) * cp - fData[i].dataFourierIm->GetBinContent(j) * sp;
        im = fData[i].dataFourierIm->GetBinContent(j) * cp + fData[i].dataFourierRe->GetBinContent(j) * sp;
        // overwrite fourier data set value
        fData[i].dataFourierRe->SetBinContent(j, re);
        fData[i].dataFourierIm->SetBinContent(j, im);
      }
    }
    if ((fData[i].theoryFourierRe != 0) && (fData[i].theoryFourierIm != 0)) {
      for (int j=0; j<fData[i].theoryFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
        // calculate new fourier data set value
        re = fData[i].theoryFourierRe->GetBinContent(j) * cp - fData[i].theoryFourierIm->GetBinContent(j) * sp;
        im = fData[i].theoryFourierIm->GetBinContent(j) * cp + fData[i].theoryFourierRe->GetBinContent(j) * sp;
        // overwrite fourier data set value
        fData[i].theoryFourierRe->SetBinContent(j, re);
        fData[i].theoryFourierIm->SetBinContent(j, im);
      }
    }
  }
}
