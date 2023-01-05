/***************************************************************************

  PFourierCanvas.cpp

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

#include <iostream>
#include <fstream>

#include <TColor.h>
#include <TRandom.h>
#include <TROOT.h>
#include <TDatime.h>
#include <TMath.h>
#include <TGFileDialog.h>

#include "PFourierCanvas.h"

#define YINFO  0.2
#define YTITLE 0.95

static const Char_t *gFiletypes[] = { "Data files", "*.dat",
                                      "All files",  "*",
                                      nullptr, nullptr };

ClassImpQ(PFourierCanvas)

//---------------------------------------------------------------------------
/**
 * <p>Constructor
 */
PFourierCanvas::PFourierCanvas()
{
  fTimeout = 0;
  fTimeoutTimer = nullptr;

  fBatchMode = false;
  fValid = false;
  fAveragedView = false;
  fAveragedViewPerDataSet = false;
  fCurrentPlotView = FOURIER_PLOT_NOT_GIVEN;
  fInitialXRange[0] = 0.0;
  fInitialXRange[1] = 0.0;

  fTitle = TString("");
  fXaxisTitle = TString("");

  fCurrentFourierPhase = 0.0;
  fCurrentFourierPhaseText = nullptr;

  fStyle = nullptr;
  fImp = nullptr;
  fBar = nullptr;
  fPopupMain = nullptr;
  fPopupFourier = nullptr;

  fMainCanvas = nullptr;
  fTitlePad = nullptr;
  fFourierPad = nullptr;
  fInfoPad = nullptr;
}

//---------------------------------------------------------------------------
/**
 * <p>Constructor
 *
 * \param fourier
 * \param title
 * \param showAverage
 * \param fourierPlotOpt
 * \param fourierXrange
 * \param phase
 * \param wtopx
 * \param wtopy
 * \param ww
 * \param wh
 * \param batch
 */
PFourierCanvas::PFourierCanvas(std::vector<PFourier*> &fourier, PIntVector dataSetTag, const Char_t* title,
                               const Bool_t showAverage, const Bool_t showAveragePerDataSet,
                               const Int_t fourierPlotOpt, Double_t fourierXrange[], Double_t phase,
                               Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
                               const Bool_t batch) :
  fBatchMode(batch), fAveragedView(showAverage), fAveragedViewPerDataSet(showAveragePerDataSet),
  fDataSetTag(dataSetTag), fCurrentPlotView(fourierPlotOpt), fTitle(title), fFourier(fourier),
  fCurrentFourierPhase(phase)
{
  fInitialXRange[0] = fourierXrange[0];
  fInitialXRange[1] = fourierXrange[1];

  fTimeout = 0;
  fTimeoutTimer = nullptr;

  fValid = false;

  fCurrentFourierPhaseText = nullptr;

  // generate fMarkerList and fColorList, since they are not provided
  TRandom rand;
  Int_t style, color;
  for (UInt_t i=0; i<fourier.size(); i++) {
    rand.SetSeed(i);
    style = 20+static_cast<Int_t>(rand.Integer(10));
    fMarkerList.push_back(style);
    color = TColor::GetColor(static_cast<Int_t>(rand.Integer(255)), static_cast<Int_t>(rand.Integer(255)), static_cast<Int_t>(rand.Integer(255)));
    fColorList.push_back(color);
  }

  CreateXaxisTitle();
  CreateStyle();
  InitFourierDataSets();
  InitFourierCanvas(fTitle, wtopx, wtopy, ww, wh);

  gStyle->SetHistMinimumZero(kTRUE); // needed to enforce proper bar option handling
}

//---------------------------------------------------------------------------
/**
 * <p>Constructor
 *
 * \param fourier
 * \param title
 * \param showAverage
 * \param fourierPlotOpt
 * \param fourierXrange
 * \param phase
 * \param wtopx
 * \param wtopy
 * \param ww
 * \param wh
 * \param markerList
 * \param colorList
 * \param batch
 */
PFourierCanvas::PFourierCanvas(std::vector<PFourier*> &fourier, PIntVector dataSetTag, const Char_t* title,
                               const Bool_t showAverage, const Bool_t showAveragePerDataSet,
                               const Int_t fourierPlotOpt, Double_t fourierXrange[], Double_t phase,
                               Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
                               const PIntVector markerList, const PIntVector colorList, const Bool_t batch) :
  fBatchMode(batch), fAveragedView(showAverage), fAveragedViewPerDataSet(showAveragePerDataSet),
  fDataSetTag(dataSetTag), fCurrentPlotView(fourierPlotOpt), fTitle(title), fFourier(fourier),
  fCurrentFourierPhase(phase), fMarkerList(markerList), fColorList(colorList)
{
  fInitialXRange[0] = fourierXrange[0];
  fInitialXRange[1] = fourierXrange[1];

  fTimeout = 0;
  fTimeoutTimer = nullptr;

  fValid = false;

  fCurrentFourierPhaseText = nullptr;

  // generate fMarkerList and fColorList, since they are not provided
  TRandom rand;
  Int_t style, color;
  for (UInt_t i=static_cast<UInt_t>(fMarkerList.size()); i<fourier.size(); i++) {
    rand.SetSeed(i);
    style = 20+static_cast<Int_t>(rand.Integer(10));
    fMarkerList.push_back(style);
  }
  for (UInt_t i=static_cast<UInt_t>(fColorList.size()); i<fourier.size(); i++) {
    rand.SetSeed(i);
    color = TColor::GetColor(static_cast<Int_t>(rand.Integer(255)), static_cast<Int_t>(rand.Integer(255)), static_cast<Int_t>(rand.Integer(255)));
    fColorList.push_back(color);
  }

  CreateXaxisTitle();
  CreateStyle();
  InitFourierDataSets();
  InitFourierCanvas(fTitle, wtopx, wtopy, ww, wh);

  gStyle->SetHistMinimumZero(kTRUE); // needed to enforce proper bar option handling
}

//---------------------------------------------------------------------------
/**
 * <p>Destructor
 */
PFourierCanvas::~PFourierCanvas()
{
  if (fTimeoutTimer)
    delete fTimeoutTimer;

  if (fCurrentFourierPhaseText)
    delete fCurrentFourierPhaseText;

/*
  if (fStyle) {
    delete fStyle;
    fStyle = 0;
  }
*/
  if (fTitlePad) {
    fTitlePad->Clear();
    delete fTitlePad;
    fTitlePad = nullptr;
  }

  if (fFourierHistos.size() > 0) {
    for (UInt_t i=0; i<fFourierHistos.size(); i++) {
      delete fFourierHistos[i].dataFourierRe;
      delete fFourierHistos[i].dataFourierIm;
      delete fFourierHistos[i].dataFourierPwr;
      delete fFourierHistos[i].dataFourierPhase;
      delete fFourierHistos[i].dataFourierPhaseOptReal;
    }
    fFourierHistos.clear();
  }

  CleanupAverage();

/*
  if (fFourierPad) {
    fFourierPad->Clear();
    delete fFourierPad;
    fFourierPad = 0;
  }
*/
  if (fInfoPad) {
    fInfoPad->Clear();
    delete fInfoPad;
    fInfoPad = nullptr;
  }

  if (fLegAvgPerDataSet) {
    fLegAvgPerDataSet->Clear();
    delete fLegAvgPerDataSet;
    fLegAvgPerDataSet = nullptr;
  }

/*
  if (fMainCanvas) {
    delete fMainCanvas;
    fMainCanvas = nullptr;
  }
*/
}

//--------------------------------------------------------------------------
// Done (SIGNAL)
//--------------------------------------------------------------------------
/**
 * <p>Signal emitted that the user wants to terminate the application.
 *
 * \param status Status info
 */
void PFourierCanvas::Done(Int_t status)
{
  Emit("Done(Int_t)", status);
}

//--------------------------------------------------------------------------
// HandleCmdKey (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>Filters keyboard events, and if they are a command key (see below) carries out the
 * necessary actions.
 * <p>Currently implemented command keys:
 * - 'q' quit musrview
 * - 'u' unzoom to the original plot range given in the msr-file.
 * - 'a' toggle between average view and single Fourier histo view.
 * - '+' increment the phase (real/imaginary Fourier). The phase step is defined in the musrfit_startup.xml
 * - '-' decrement the phase (real/imaginary Fourier). The phase step is defined in the musrfit_startup.xml
 *
 * \param event event type
 * \param x character key
 * \param y not used
 * \param selected not used
 */
void PFourierCanvas::HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected)
{
  if (event != kKeyPress)
     return;

  if (fBatchMode)
    return;

  if (x == 'q') { // quit
    Done(0);
  } else if (x == 'u') { // unzoom
    if (fAveragedView || fAveragedViewPerDataSet)
      PlotAverage();
    else
      PlotFourier();
  } else if (x == 'a') { // toggle between average view and single histo view
    // toggle average view flag
    fAveragedView = !fAveragedView;
    // unset average per data set view flag
    fAveragedViewPerDataSet = false;
    // update menu
    if (fAveragedView) {
      fPopupMain->CheckEntry(P_MENU_ID_AVERAGE);
      fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE_PER_DATA_SET);
      HandleAverage();
      PlotAverage();
    } else {
      fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE);
      fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE_PER_DATA_SET);
      CleanupAverage();
      PlotFourier();
    }
  } else if (x == 'd') { // toggle between average view per data set and single histo view
    // toggle average per data set view flag
    fAveragedViewPerDataSet = !fAveragedViewPerDataSet;
    // unset average view flag
    fAveragedView = false;
    // update menu
    if (fAveragedViewPerDataSet) {
      fPopupMain->CheckEntry(P_MENU_ID_AVERAGE_PER_DATA_SET);
      fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE);
      HandleAverage();
      PlotAverage();
    } else {
      fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE_PER_DATA_SET);
      fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE);
      CleanupAverage();
      PlotFourier();
    }
  } else if (x == 'c') {
    Int_t state = fFourierPad->GetCrosshair();
    if (state == 0) {
      fMainCanvas->ToggleEventStatus();
      fFourierPad->SetCrosshair(2);
    } else {
      fMainCanvas->ToggleEventStatus();
      fFourierPad->SetCrosshair(0);
    }
    fMainCanvas->Update();
  } else if (x == '+') { // increment phase (Fourier real/imag)
    IncrementFourierPhase();
  } else if (x == '-') { // decrement phase (Fourier real/imag)
    DecrementFourierPhase();
  }
}

//--------------------------------------------------------------------------
// HandleMenuPopup (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>Handles the MusrFT menu.
 *
 * \param id identification key of the selected menu
 */
void PFourierCanvas::HandleMenuPopup(Int_t id)
{
  if (fBatchMode)
    return;

  if (id == P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_REAL) {
    // uncheck fourier popup items
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_REAL);
    fCurrentPlotView = FOURIER_PLOT_REAL;
    if (!fAveragedView) {
      PlotFourier();
    } else {
      HandleAverage();
      PlotAverage();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_IMAG) {
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_IMAG);
    fCurrentPlotView = FOURIER_PLOT_IMAG;
    if (!fAveragedView) {
      PlotFourier();
    } else {
      HandleAverage();
      PlotAverage();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_REAL_AND_IMAG) {
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_REAL_AND_IMAG);
    fCurrentPlotView = P_MENU_ID_FOURIER_REAL_AND_IMAG;
    if (!fAveragedView) {
      PlotFourier();
    } else {
      HandleAverage();
      PlotAverage();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PWR) {
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PWR);
    fCurrentPlotView = FOURIER_PLOT_POWER;
    if (!fAveragedView) {
      PlotFourier();
    } else {
      HandleAverage();
      PlotAverage();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE) {
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE);
    fCurrentPlotView = FOURIER_PLOT_PHASE;
    if (!fAveragedView) {
      PlotFourier();
    } else {
      HandleAverage();
      PlotAverage();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE_OPT_REAL) {
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE_OPT_REAL);
    fCurrentPlotView = FOURIER_PLOT_PHASE_OPT_REAL;
    if (!fAveragedView) {
      PlotFourier();
    } else {
      HandleAverage();
      PlotAverage();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE_PLUS) {
    IncrementFourierPhase();
  } else if (id == P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE_MINUS) {
    DecrementFourierPhase();
  } else if (id == P_MENU_ID_AVERAGE) {
    // toggle average view flag
    fAveragedView = !fAveragedView;
    // unset average per data set view flag
    fAveragedViewPerDataSet = false;
    // update menu
    if (fAveragedView) {
      fPopupMain->CheckEntry(P_MENU_ID_AVERAGE);
      fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE_PER_DATA_SET);
      HandleAverage();
      PlotAverage();
    } else {
      fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE);
      fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE_PER_DATA_SET);
      CleanupAverage();
      PlotFourier();
    }
  } else if (id == P_MENU_ID_AVERAGE_PER_DATA_SET) {
    // toggle average per data set view flag
    fAveragedViewPerDataSet = !fAveragedViewPerDataSet;
    // unset average view flag
    fAveragedView = false;
    // update menu
    if (fAveragedViewPerDataSet) {
      fPopupMain->CheckEntry(P_MENU_ID_AVERAGE_PER_DATA_SET);
      fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE);
      HandleAverage();
      PlotAverage();
    } else {
      fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE_PER_DATA_SET);
      fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE);
      CleanupAverage();
      PlotFourier();
    }
  } else if (id == P_MENU_ID_EXPORT_DATA) {
    static TString dir(".");
    TGFileInfo fi;
    fi.fFileTypes = gFiletypes;
    fi.fIniDir = StrDup(dir);
    fi.fOverwrite = true;
    new TGFileDialog(nullptr, fImp, kFDSave, &fi);
    if (fi.fFilename && strlen(fi.fFilename)) {
      ExportData(fi.fFilename);
    }
  }
}

//--------------------------------------------------------------------------
// LastCanvasClosed (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>Slot called when the last canvas has been closed. Will emit Done(0) which will
 * terminate the application.
 */
void PFourierCanvas::LastCanvasClosed()
{
  if (gROOT->GetListOfCanvases()->IsEmpty()) {
    Done(0);
  }
}

//--------------------------------------------------------------------------
// UpdateFourierPad (public)
//--------------------------------------------------------------------------
/**
 * <p>Feeds the pad with the Fourier data sets and refreshes it.
 */
void PFourierCanvas::UpdateFourierPad()
{
  if (!fValid)
    return;

  if (fAveragedView || fAveragedViewPerDataSet)
    PlotAverage();
  else
    PlotFourier();

  fFourierPad->Draw();
  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// UpdateInfoPad (public)
//--------------------------------------------------------------------------
/**
 * <p>Feeds the pad with the legend and refreshes it.
 */
void PFourierCanvas::UpdateInfoPad()
{
  if (!fValid)
    return;

  // write header line
  TDatime dt;
  Char_t dtStr[128];
  strncpy(dtStr, dt.AsSQLString(), sizeof(dtStr));
  TString str("musrFT: ");
  str += dtStr;
  fInfoPad->SetHeader(str);

  Char_t title[1024];
  for (UInt_t i=0; i<fFourier.size(); i++) {
    strncpy(title, fFourier[i]->GetDataTitle(), sizeof(title));
    // add entry
    fInfoPad->AddEntry(fFourierHistos[i].dataFourierRe, title, "p");
  }

  fInfoPad->Draw();
  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// SetTimeout (public)
//--------------------------------------------------------------------------
/**
 * <p>sets the timeout after which the program shall terminate.
 *
 * \param timeout after which the done signal shall be emitted. Given in seconds
 */
void PFourierCanvas::SetTimeout(Int_t timeout)
{
  fTimeout = timeout;

  if (fTimeout <= 0)
    return;

  if (fTimeoutTimer) {
    delete fTimeoutTimer;
    fTimeoutTimer = nullptr;
  }
  fTimeoutTimer = new TTimer();

  fTimeoutTimer->Connect("Timeout()", "PFourierCanvas", this, "Done()");

  fTimeoutTimer->Start(1000*fTimeout, kTRUE);
}

//--------------------------------------------------------------------------
// SaveGraphicsAndQuit
//--------------------------------------------------------------------------
/**
 * <p>Will save the canvas as graphics output. Needed in the batch mode of musrview.
 *
 * \param fileName file name under which the canvas shall be saved.
 */
void PFourierCanvas::SaveGraphicsAndQuit(const Char_t *fileName)
{
  std::cout << std::endl << ">> SaveGraphicsAndQuit: will dump the canvas into a graphics output file: " << fileName << " ..." << std::endl;

  fMainCanvas->SaveAs(fileName);

  Done(0);
}

//--------------------------------------------------------------------------
// ExportData
//--------------------------------------------------------------------------
/**
 * <p>Exports the currently displayed Fourier data set in ascii column format.
 */
void PFourierCanvas::ExportData(const Char_t *pathFileName)
{
  TString pfn("");

  if (pathFileName) { // path file name provided
    pfn = TString(pathFileName);
  } else { // path file name NOT provided, generate a default path file name
    std::cerr << std::endl << ">> PFourierCanvas::ExportData **ERROR** NO path file name provided. Will do nothing." << std::endl;
    return;
  }

  TString xAxis(""), yAxis("");
  Int_t xMinBin, xMaxBin;
  if (fAveragedView) {
    switch (fCurrentPlotView) {
      case FOURIER_PLOT_REAL:
        xAxis = fFourierHistos[0].dataFourierRe->GetXaxis()->GetTitle();
        yAxis = TString("<Real>");
        xMinBin = fFourierHistos[0].dataFourierRe->GetXaxis()->GetFirst();
        xMaxBin = fFourierHistos[0].dataFourierRe->GetXaxis()->GetLast();
        break;
      case FOURIER_PLOT_IMAG:
        xAxis = fFourierHistos[0].dataFourierIm->GetXaxis()->GetTitle();
        yAxis = TString("<Imag>");
        xMinBin = fFourierHistos[0].dataFourierIm->GetXaxis()->GetFirst();
        xMaxBin = fFourierHistos[0].dataFourierIm->GetXaxis()->GetLast();
        break;
      case FOURIER_PLOT_POWER:
        xAxis = fFourierHistos[0].dataFourierPwr->GetXaxis()->GetTitle();
        yAxis = TString("<Power>");
        xMinBin = fFourierHistos[0].dataFourierPwr->GetXaxis()->GetFirst();
        xMaxBin = fFourierHistos[0].dataFourierPwr->GetXaxis()->GetLast();
        break;
      case FOURIER_PLOT_PHASE:
        xAxis = fFourierHistos[0].dataFourierPhase->GetXaxis()->GetTitle();
        yAxis = TString("<Phase>");
        xMinBin = fFourierHistos[0].dataFourierPhase->GetXaxis()->GetFirst();
        xMaxBin = fFourierHistos[0].dataFourierPhase->GetXaxis()->GetLast();
        break;        
      case FOURIER_PLOT_PHASE_OPT_REAL:
        xAxis = fFourierHistos[0].dataFourierPhaseOptReal->GetXaxis()->GetTitle();
        yAxis = TString("<Phase>");
        xMinBin = fFourierHistos[0].dataFourierPhaseOptReal->GetXaxis()->GetFirst();
        xMaxBin = fFourierHistos[0].dataFourierPhaseOptReal->GetXaxis()->GetLast();
        break;
      default:
        xAxis = TString("??");
        yAxis = TString("??");
        xMinBin = 0;
        xMaxBin = 0;
        break;
    }
  } else {
    switch (fCurrentPlotView) {
      case FOURIER_PLOT_REAL:
        xAxis = fFourierHistos[0].dataFourierRe->GetXaxis()->GetTitle();
        yAxis = TString("Real");
        xMinBin = fFourierHistos[0].dataFourierRe->GetXaxis()->GetFirst();
        xMaxBin = fFourierHistos[0].dataFourierRe->GetXaxis()->GetLast();
        break;
      case FOURIER_PLOT_IMAG:
        xAxis = fFourierHistos[0].dataFourierIm->GetXaxis()->GetTitle();
        yAxis = TString("Imag");
        xMinBin = fFourierHistos[0].dataFourierIm->GetXaxis()->GetFirst();
        xMaxBin = fFourierHistos[0].dataFourierIm->GetXaxis()->GetLast();
        break;
      case FOURIER_PLOT_REAL_AND_IMAG:
        xAxis = fFourierHistos[0].dataFourierRe->GetXaxis()->GetTitle();
        yAxis = TString("Real+Imag");
        xMinBin = fFourierHistos[0].dataFourierRe->GetXaxis()->GetFirst();
        xMaxBin = fFourierHistos[0].dataFourierRe->GetXaxis()->GetLast();
        break;
      case FOURIER_PLOT_POWER:
        xAxis = fFourierHistos[0].dataFourierPwr->GetXaxis()->GetTitle();
        yAxis = TString("Power");
        xMinBin = fFourierHistos[0].dataFourierPwr->GetXaxis()->GetFirst();
        xMaxBin = fFourierHistos[0].dataFourierPwr->GetXaxis()->GetLast();
        break;
      case FOURIER_PLOT_PHASE:
        xAxis = fFourierHistos[0].dataFourierPhase->GetXaxis()->GetTitle();
        yAxis = TString("Phase");
        xMinBin = fFourierHistos[0].dataFourierPhase->GetXaxis()->GetFirst();
        xMaxBin = fFourierHistos[0].dataFourierPhase->GetXaxis()->GetLast();
        break;
      case FOURIER_PLOT_PHASE_OPT_REAL:
        xAxis = fFourierHistos[0].dataFourierPhaseOptReal->GetXaxis()->GetTitle();
        yAxis = TString("Phase");
        xMinBin = fFourierHistos[0].dataFourierPhaseOptReal->GetXaxis()->GetFirst();
        xMaxBin = fFourierHistos[0].dataFourierPhaseOptReal->GetXaxis()->GetLast();
        break;
      default:
        xAxis = TString("??");
        yAxis = TString("??");
        xMinBin = 0;
        xMaxBin = 0;
        break;
    }
  }

  // write data to file
  std::ofstream fout(pfn.Data(), std::ofstream::out);

  if (fAveragedView) {
    // write header
    fout << "% " << pfn << std::endl;
    fout << "% averaged data of:" << std::endl;
    for (UInt_t i=0; i<fFourierHistos.size(); i++) {
      fout << "%   " << fFourierHistos[i].dataFourierRe->GetTitle() << std::endl;
    }
    fout << "%------------" << std::endl;
    fout << "% " << xAxis << ", " << yAxis << std::endl;
    for (Int_t i=xMinBin; i<xMaxBin; i++) {
      switch (fCurrentPlotView) {
        case FOURIER_PLOT_REAL:
          fout << fFourierAverage[0].dataFourierRe->GetBinCenter(i) << ", " << fFourierAverage[0].dataFourierRe->GetBinContent(i) << std::endl;
          break;
        case FOURIER_PLOT_IMAG:
          fout << fFourierAverage[0].dataFourierIm->GetBinCenter(i) << ", " << fFourierAverage[0].dataFourierIm->GetBinContent(i) << std::endl;
          break;
        case FOURIER_PLOT_POWER:
          fout << fFourierAverage[0].dataFourierPwr->GetBinCenter(i) << ", " << fFourierAverage[0].dataFourierPwr->GetBinContent(i) << std::endl;
          break;
        case FOURIER_PLOT_PHASE:
          fout << fFourierAverage[0].dataFourierPhase->GetBinCenter(i) << ", " << fFourierAverage[0].dataFourierPhase->GetBinContent(i) << std::endl;
          break;
        case FOURIER_PLOT_PHASE_OPT_REAL:
          fout << fFourierAverage[0].dataFourierPhaseOptReal->GetBinCenter(i) << ", " << fFourierAverage[0].dataFourierPhaseOptReal->GetBinContent(i) << std::endl;
          break;
        default:
          break;
      }
    }
  } else {
    // write header
    fout << "% " << pfn << std::endl;
    fout << "% data of:" << std::endl;
    for (UInt_t i=0; i<fFourierHistos.size(); i++) {
      fout << "%   " << fFourierHistos[i].dataFourierRe->GetTitle() << std::endl;
    }
    fout << "%------------" << std::endl;
    fout << "% ";
    for (UInt_t i=0; i<fFourierHistos.size()-1; i++) {
      fout << xAxis << i << ", " << yAxis << i << ", ";
    }
    fout << xAxis << fFourierHistos.size()-1 << ", " << yAxis << fFourierHistos.size()-1 << std::endl;

    // write data
    for (Int_t i=xMinBin; i<xMaxBin; i++) {
      for (UInt_t j=0; j<fFourierHistos.size()-1; j++) {
        switch (fCurrentPlotView) {
          case FOURIER_PLOT_REAL:
            fout << fFourierHistos[j].dataFourierRe->GetBinCenter(i) << ", " << fFourierHistos[j].dataFourierRe->GetBinContent(i) << ", ";
            break;
          case FOURIER_PLOT_IMAG:
            fout << fFourierHistos[j].dataFourierIm->GetBinCenter(i) << ", " << fFourierHistos[j].dataFourierIm->GetBinContent(i) << ", ";
            break;
          case FOURIER_PLOT_REAL_AND_IMAG:
            break;
          case FOURIER_PLOT_POWER:
            fout << fFourierHistos[j].dataFourierPwr->GetBinCenter(i) << ", " << fFourierHistos[j].dataFourierPwr->GetBinContent(i) << ", ";
            break;
          case FOURIER_PLOT_PHASE:
            fout << fFourierHistos[j].dataFourierPhase->GetBinCenter(i) << ", " << fFourierHistos[j].dataFourierPhase->GetBinContent(i) << ", ";
            break;
          case FOURIER_PLOT_PHASE_OPT_REAL:
            fout << fFourierHistos[j].dataFourierPhaseOptReal->GetBinCenter(i) << ", " << fFourierHistos[j].dataFourierPhaseOptReal->GetBinContent(i) << ", ";
            break;
          default:
            break;
        }
      }
      switch (fCurrentPlotView) {
        case FOURIER_PLOT_REAL:
          fout << fFourierHistos[fFourierHistos.size()-1].dataFourierRe->GetBinCenter(i) << ", " << fFourierHistos[fFourierHistos.size()-1].dataFourierRe->GetBinContent(i) << std::endl;
          break;
        case FOURIER_PLOT_IMAG:
          fout << fFourierHistos[fFourierHistos.size()-1].dataFourierIm->GetBinCenter(i) << ", " << fFourierHistos[fFourierHistos.size()-1].dataFourierIm->GetBinContent(i) << std::endl;
          break;
        case FOURIER_PLOT_REAL_AND_IMAG:
          break;
        case FOURIER_PLOT_POWER:
          fout << fFourierHistos[fFourierHistos.size()-1].dataFourierPwr->GetBinCenter(i) << ", " << fFourierHistos[fFourierHistos.size()-1].dataFourierPwr->GetBinContent(i) << std::endl;
          break;
        case FOURIER_PLOT_PHASE:
          fout << fFourierHistos[fFourierHistos.size()-1].dataFourierPhase->GetBinCenter(i) << ", " << fFourierHistos[fFourierHistos.size()-1].dataFourierPhase->GetBinContent(i) << std::endl;
          break;
        case FOURIER_PLOT_PHASE_OPT_REAL:
          fout << fFourierHistos[fFourierHistos.size()-1].dataFourierPhaseOptReal->GetBinCenter(i) << ", " << fFourierHistos[fFourierHistos.size()-1].dataFourierPhaseOptReal->GetBinContent(i) << std::endl;
          break;
        default:
          break;
      }
    }
  }

  fout.close();
}

//--------------------------------------------------------------------------
// CreateXaxisTitle (private)
//--------------------------------------------------------------------------
/**
 * <p>Creates the x-axis title based on the Fourier units used.
 */
void PFourierCanvas::CreateXaxisTitle()
{
  switch (fFourier[0]->GetUnitTag()) {
    case FOURIER_UNIT_GAUSS:
      fXaxisTitle = TString("Field (Gauss)");
      break;
    case FOURIER_UNIT_TESLA:
      fXaxisTitle = TString("Field (Tesla)");
      break;
    case FOURIER_UNIT_FREQ:
      fXaxisTitle = TString("Frequency (MHz)");
      break;
    case FOURIER_UNIT_CYCLES:
      fXaxisTitle = TString("Angular Frequency (Mc/s)");
      break;
    default:
      fXaxisTitle = TString("??");
      break;
  }
}

//--------------------------------------------------------------------------
// CreateStyle (private)
//--------------------------------------------------------------------------
/**
 * <p> Set styles for the canvas. Perhaps one could transfer them to the startup-file in the future.
 */
void PFourierCanvas::CreateStyle()
{
  TString musrFTStyle("musrFTStyle");
  fStyle = new TStyle(musrFTStyle, musrFTStyle);
  fStyle->SetOptStat(0);  // no statistics options
  fStyle->SetOptTitle(0); // no title
  fStyle->cd();
}

//--------------------------------------------------------------------------
// InitFourierDataSets (private)
//--------------------------------------------------------------------------
/**
 * <p>Initialize the Fourier data sets, i.e. get the TH1F objects, set markers,
 * set colors, ...
 */
void PFourierCanvas::InitFourierDataSets()
{
  // get all the Fourier histos
  fFourierHistos.resize(fFourier.size());
  for (UInt_t i=0; i<fFourierHistos.size(); i++) {
    fFourierHistos[i].dataFourierRe = fFourier[i]->GetRealFourier();
    fFourierHistos[i].dataFourierIm = fFourier[i]->GetImaginaryFourier();
    fFourierHistos[i].dataFourierPwr = fFourier[i]->GetPowerFourier();
    fFourierHistos[i].dataFourierPhase = fFourier[i]->GetPhaseFourier();
    if (fCurrentPlotView == FOURIER_PLOT_PHASE_OPT_REAL) {
      fFourierHistos[i].dataFourierPhaseOptReal = fFourier[i]->GetPhaseOptRealFourier(fFourierHistos[i].dataFourierRe,
                      fFourierHistos[i].dataFourierIm, fFourierHistos[i].optPhase, 1.0, fInitialXRange[0], fInitialXRange[1]);
    }
  }

  // rescale histo to abs(maximum) == 1
  Double_t max = 0.0, dval = 0.0;
  Int_t start = fFourierHistos[0].dataFourierRe->FindBin(fInitialXRange[0]);
  Int_t end   = fFourierHistos[0].dataFourierRe->FindBin(fInitialXRange[1]);
  // real
  for (UInt_t i=0; i<fFourierHistos.size(); i++) {
    for (Int_t j=start; j<=end; j++) {
      dval = fFourierHistos[i].dataFourierRe->GetBinContent(j);
      if (fabs(dval) > max)
        max = dval;
    }
  }
  for (UInt_t i=0; i<fFourierHistos.size(); i++) {
    for (Int_t j=1; j<fFourierHistos[i].dataFourierRe->GetNbinsX(); j++) {
      fFourierHistos[i].dataFourierRe->SetBinContent(j, fFourierHistos[i].dataFourierRe->GetBinContent(j)/fabs(max));
    }
  }

  // imaginary
  for (UInt_t i=0; i<fFourierHistos.size(); i++) {
    for (Int_t j=start; j<=end; j++) {
      dval = fFourierHistos[i].dataFourierIm->GetBinContent(j);
      if (fabs(dval) > max)
        max = dval;
    }
  }
  for (UInt_t i=0; i<fFourierHistos.size(); i++) {
    for (Int_t j=1; j<fFourierHistos[i].dataFourierIm->GetNbinsX(); j++) {
      fFourierHistos[i].dataFourierIm->SetBinContent(j, fFourierHistos[i].dataFourierIm->GetBinContent(j)/fabs(max));
    }
  }

  // power
  max = 0.0;
  for (UInt_t i=0; i<fFourierHistos.size(); i++) {
    for (Int_t j=start; j<=end; j++) {
      dval = fFourierHistos[i].dataFourierPwr->GetBinContent(j);
      if (fabs(dval) > max)
        max = dval;
    }
  }
  for (UInt_t i=0; i<fFourierHistos.size(); i++) {
    for (Int_t j=1; j<fFourierHistos[i].dataFourierPwr->GetNbinsX(); j++) {
      fFourierHistos[i].dataFourierPwr->SetBinContent(j, fFourierHistos[i].dataFourierPwr->GetBinContent(j)/fabs(max));
    }
  }

  // phase
  max = 0.0;
  for (UInt_t i=0; i<fFourierHistos.size(); i++) {
    for (Int_t j=start; j<=end; j++) {
      dval = fFourierHistos[i].dataFourierPhase->GetBinContent(j);
      if (fabs(dval) > max)
        max = dval;
    }
  }
  for (UInt_t i=0; i<fFourierHistos.size(); i++) {
    for (Int_t j=1; j<fFourierHistos[i].dataFourierPhase->GetNbinsX(); j++) {
      fFourierHistos[i].dataFourierPhase->SetBinContent(j, fFourierHistos[i].dataFourierPhase->GetBinContent(j)/fabs(max));
    }
  }

  if (fCurrentPlotView == FOURIER_PLOT_PHASE_OPT_REAL) {
    // phase opt real
    for (UInt_t i=0; i<fFourierHistos.size(); i++) {
      for (Int_t j=start; j<=end; j++) {
        dval = fFourierHistos[i].dataFourierPhaseOptReal->GetBinContent(j);
        if (fabs(dval) > max)
          max = dval;
      }
    }
    for (UInt_t i=0; i<fFourierHistos.size(); i++) {
      for (Int_t j=1; j<fFourierHistos[i].dataFourierPhaseOptReal->GetNbinsX(); j++) {
        fFourierHistos[i].dataFourierPhaseOptReal->SetBinContent(j, fFourierHistos[i].dataFourierPhaseOptReal->GetBinContent(j)/fabs(max));
      }
    }
  }

  // set the marker and line color
  for (UInt_t i=0; i<fFourierHistos.size(); i++) {
    fFourierHistos[i].dataFourierRe->SetMarkerColor(fColorList[i]);
    fFourierHistos[i].dataFourierRe->SetLineColor(fColorList[i]);
    fFourierHistos[i].dataFourierIm->SetMarkerColor(fColorList[i]);
    fFourierHistos[i].dataFourierIm->SetLineColor(fColorList[i]);
    fFourierHistos[i].dataFourierPwr->SetMarkerColor(fColorList[i]);
    fFourierHistos[i].dataFourierPwr->SetLineColor(fColorList[i]);
    fFourierHistos[i].dataFourierPhase->SetMarkerColor(fColorList[i]);
    fFourierHistos[i].dataFourierPhase->SetLineColor(fColorList[i]);
    if (fCurrentPlotView == FOURIER_PLOT_PHASE_OPT_REAL) {
      fFourierHistos[i].dataFourierPhaseOptReal->SetMarkerColor(fColorList[i]);
      fFourierHistos[i].dataFourierPhaseOptReal->SetLineColor(fColorList[i]);
    }
  }

  // set the marker symbol and size
  for (UInt_t i=0; i<fFourierHistos.size(); i++) {
    fFourierHistos[i].dataFourierRe->SetMarkerStyle(fMarkerList[i]);
    fFourierHistos[i].dataFourierRe->SetMarkerSize(0.7);
    fFourierHistos[i].dataFourierIm->SetMarkerStyle(fMarkerList[i]);
    fFourierHistos[i].dataFourierIm->SetMarkerSize(0.7);
    fFourierHistos[i].dataFourierPwr->SetMarkerStyle(fMarkerList[i]);
    fFourierHistos[i].dataFourierPwr->SetMarkerSize(0.7);
    fFourierHistos[i].dataFourierPhase->SetMarkerStyle(fMarkerList[i]);
    fFourierHistos[i].dataFourierPhase->SetMarkerSize(0.7);
    if (fCurrentPlotView == FOURIER_PLOT_PHASE_OPT_REAL) {
      fFourierHistos[i].dataFourierPhaseOptReal->SetMarkerStyle(fMarkerList[i]);
      fFourierHistos[i].dataFourierPhaseOptReal->SetMarkerSize(0.7);
    }
  }

  // initialize average histos
  CleanupAverage();
}

//--------------------------------------------------------------------------
// InitFourierCanvas (private)
//--------------------------------------------------------------------------
/**
 * <p>Initialize the class, and sets up the necessary objects.
 *
 * \param title Title to be displayed
 * \param wtopx top x coordinate (in pixels) to place the canvas.
 * \param wtopy top y coordinate (in pixels) to place the canvas.
 * \param ww width (in pixels) of the canvas.
 * \param wh height (in pixels) of the canvas.
 */
void PFourierCanvas::InitFourierCanvas(const Char_t* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh)
{
  fImp   = nullptr;
  fBar   = nullptr;
  fPopupMain    = nullptr;
  fPopupFourier = nullptr;

  fMainCanvas  = nullptr;
  fTitlePad    = nullptr;
  fFourierPad  = nullptr;
  fInfoPad     = nullptr;

  // invoke canvas
  TString canvasName = TString("fMainCanvas");
  fMainCanvas = new TCanvas(canvasName.Data(), title, wtopx, wtopy, ww, wh);
  if (fMainCanvas == nullptr) {
    std::cerr << std::endl << "PFourierCanvas::PFourierCanvas: **PANIC ERROR**: Couldn't invoke " << canvasName.Data();
    std::cerr << std::endl;
    return;
  }

  // add canvas menu if not in batch mode
  if (!fBatchMode) {
    fImp = static_cast<TRootCanvas*>(fMainCanvas->GetCanvasImp());
    fBar = fImp->GetMenuBar();
    fPopupMain = fBar->AddPopup("MusrFT");

    fPopupFourier = new TGPopupMenu();

    fPopupMain->AddPopup("&Fourier", fPopupFourier);
    fPopupFourier->AddEntry("Show Real", P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_REAL);
    fPopupFourier->AddEntry("Show Imag", P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_IMAG);
    fPopupFourier->AddEntry("Show Real+Imag", P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_REAL_AND_IMAG);
    fPopupFourier->AddEntry("Show Power", P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PWR);
    fPopupFourier->AddEntry("Show Phase", P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE);
    fPopupFourier->AddEntry("Show Phase Opt Fourier", P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE_OPT_REAL);
    fPopupFourier->AddSeparator();
    fPopupFourier->AddEntry("Phase +", P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->AddEntry("Phase -", P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE_MINUS);
    fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE_MINUS);

    switch (fCurrentPlotView) {
      case FOURIER_PLOT_REAL:
        fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_REAL);
        break;
      case FOURIER_PLOT_IMAG:
        fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_IMAG);
        break;
      case FOURIER_PLOT_REAL_AND_IMAG:
        fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_REAL_AND_IMAG);
        break;
      case FOURIER_PLOT_POWER:
        fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PWR);
        break;
      case FOURIER_PLOT_PHASE:
        fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE);
        break;
      case FOURIER_PLOT_PHASE_OPT_REAL:
        fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_ID_FOURIER_PHASE_OPT_REAL);
        break;
      default:
        break;
    }

    fPopupMain->AddEntry("Average", P_MENU_ID_AVERAGE);
    if (fAveragedView)
      fPopupMain->CheckEntry(P_MENU_ID_AVERAGE);
    fPopupMain->AddEntry("Average per Data Set", P_MENU_ID_AVERAGE_PER_DATA_SET);
    if (fAveragedViewPerDataSet)
      fPopupMain->CheckEntry(P_MENU_ID_AVERAGE_PER_DATA_SET);
    fPopupMain->AddSeparator();

    fPopupMain->AddEntry("Export Data", P_MENU_ID_EXPORT_DATA);
    fBar->MapSubwindows();
    fBar->Layout();

    fPopupMain->Connect("TGPopupMenu", "Activated(Int_t)", "PFourierCanvas", this, "HandleMenuPopup(Int_t)");
  }

  // divide the canvas into sub pads
  // title pad
  fTitlePad = new TPaveText(0.0, YTITLE, 1.0, 1.0, "NDC");
  if (fTitlePad == nullptr) {
    std::cerr << std::endl << "PFourierCanvas::PFourierCanvas: **PANIC ERROR**: Couldn't invoke fTitlePad";
    std::cerr << std::endl;
    return;
  }
  fTitlePad->SetFillColor(TColor::GetColor(255,255,255));
  fTitlePad->SetTextAlign(12); // middle, left
  fTitlePad->AddText(title);
  fTitlePad->Draw();

  // fourier pad
  fFourierPad = new TPad("fFourierPad", "fFourierPad", 0.0, YINFO, 1.0, YTITLE);
  if (fFourierPad == nullptr) {
    std::cerr << std::endl << "PFourierCanvas::PFourierCanvas: **PANIC ERROR**: Couldn't invoke fFourierPad";
    std::cerr << std::endl;
    return;
  }
  fFourierPad->SetFillColor(TColor::GetColor(255,255,255));
  fFourierPad->Draw();

  // info pad
  fInfoPad = new TLegend(0.0, 0.0, 1.0, YINFO, "NDC");
  if (fInfoPad == nullptr) {
    std::cerr << std::endl << "PFourierCanvas::PFourierCanvas: **PANIC ERROR**: Couldn't invoke fInfoPad";
    std::cerr << std::endl;
    return;
  }
  fInfoPad->SetFillColor(TColor::GetColor(255,255,255));
  fInfoPad->SetTextAlign(12); // middle, left

  fLegAvgPerDataSet = nullptr;

  fValid = true;

  if ((fFourier.size() != fDataSetTag.size()) && fAveragedViewPerDataSet) {
    fValid = false;
    std::cerr << std::endl << "PFourierCanvas::PFourierCanvas: **PANIC ERROR**: # Fourier != # Data Set Tags." << std::endl;
  }

  fMainCanvas->cd();

  fMainCanvas->Show();

  fMainCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "PFourierCanvas",
                       this, "HandleCmdKey(Int_t,Int_t,Int_t,TObject*)");
}

//--------------------------------------------------------------------------
// CleanupAverage (private)
//--------------------------------------------------------------------------
/**
 * <p>Cleanup average Fourier data sets.
 */
void PFourierCanvas::CleanupAverage()
{
  for (UInt_t i=0; i<fFourierAverage.size(); i++) {
    if (fFourierAverage[i].dataFourierRe != nullptr) {
      delete fFourierAverage[i].dataFourierRe;
      fFourierAverage[i].dataFourierRe = nullptr;
    }
    if (fFourierAverage[i].dataFourierIm) {
      delete fFourierAverage[i].dataFourierIm;
      fFourierAverage[i].dataFourierIm = nullptr;
    }
    if (fFourierAverage[i].dataFourierPwr) {
      delete fFourierAverage[i].dataFourierPwr;
      fFourierAverage[i].dataFourierPwr = nullptr;
    }
    if (fFourierAverage[i].dataFourierPhase) {
      delete fFourierAverage[i].dataFourierPhase;
      fFourierAverage[i].dataFourierPhase = nullptr;
    }
    if (fFourierAverage[i].dataFourierPhaseOptReal) {
      delete fFourierAverage[i].dataFourierPhaseOptReal;
      fFourierAverage[i].dataFourierPhaseOptReal = nullptr;
    }
  }
  fFourierAverage.clear();
}

//--------------------------------------------------------------------------
// HandleAverage (private)
//--------------------------------------------------------------------------
/**
 * <p>Average Fourier data sets.
 */
void PFourierCanvas::HandleAverage()
{
  if (fFourierHistos.size() == 0)
    return;

  CleanupAverage();

  TString name("");
  Double_t dval=0.0;

  Bool_t phaseOptRealPresent = false;
  if (fFourierHistos[0].dataFourierPhaseOptReal != nullptr)
    phaseOptRealPresent = true;

  // check if ALL data shall be averaged
  if (fAveragedView) {
    fFourierAverage.resize(1);

    // create average histograms
    name = TString(fFourierHistos[0].dataFourierRe->GetTitle()) + "_avg";
    fFourierAverage[0].dataFourierRe = new TH1F(name, name, fFourierHistos[0].dataFourierRe->GetNbinsX(),
                                                fFourierHistos[0].dataFourierRe->GetXaxis()->GetXmin(),
                                                fFourierHistos[0].dataFourierRe->GetXaxis()->GetXmax());

    name = TString(fFourierHistos[0].dataFourierIm->GetTitle()) + "_avg";
    fFourierAverage[0].dataFourierIm = new TH1F(name, name, fFourierHistos[0].dataFourierIm->GetNbinsX(),
                                                fFourierHistos[0].dataFourierIm->GetXaxis()->GetXmin(),
                                                fFourierHistos[0].dataFourierIm->GetXaxis()->GetXmax());

    name = TString(fFourierHistos[0].dataFourierPwr->GetTitle()) + "_avg";
    fFourierAverage[0].dataFourierPwr = new TH1F(name, name, fFourierHistos[0].dataFourierPwr->GetNbinsX(),
                                                 fFourierHistos[0].dataFourierPwr->GetXaxis()->GetXmin(),
                                                 fFourierHistos[0].dataFourierPwr->GetXaxis()->GetXmax());

    name = TString(fFourierHistos[0].dataFourierPhase->GetTitle()) + "_avg";
    fFourierAverage[0].dataFourierPhase = new TH1F(name, name, fFourierHistos[0].dataFourierPhase->GetNbinsX(),
                                                   fFourierHistos[0].dataFourierPhase->GetXaxis()->GetXmin(),
                                                   fFourierHistos[0].dataFourierPhase->GetXaxis()->GetXmax());

    if (phaseOptRealPresent) {
      name = TString(fFourierHistos[0].dataFourierPhaseOptReal->GetTitle()) + "_avg";
      fFourierAverage[0].dataFourierPhaseOptReal = new TH1F(name, name, fFourierHistos[0].dataFourierPhaseOptReal->GetNbinsX(),
                                                            fFourierHistos[0].dataFourierPhaseOptReal->GetXaxis()->GetXmin(),
                                                            fFourierHistos[0].dataFourierPhaseOptReal->GetXaxis()->GetXmax());
    }

    // real average
    for (Int_t j=0; j<fFourierHistos[0].dataFourierRe->GetNbinsX(); j++) {
      dval = 0.0;
      for (UInt_t i=0; i<fFourierHistos.size(); i++) {
        if (j < fFourierHistos[i].dataFourierRe->GetNbinsX())
          dval += GetInterpolatedValue(fFourierHistos[i].dataFourierRe, fFourierHistos[0].dataFourierRe->GetBinCenter(j));
      }
      fFourierAverage[0].dataFourierRe->SetBinContent(j, dval/fFourierHistos.size());
    }
    // set marker color, line color, maker size, marker type
    fFourierAverage[0].dataFourierRe->SetMarkerColor(kBlack);
    fFourierAverage[0].dataFourierRe->SetLineColor(kBlack);
    fFourierAverage[0].dataFourierRe->SetMarkerSize(0.8);
    fFourierAverage[0].dataFourierRe->SetMarkerStyle(22); // closed up triangle

    // imaginary average
    for (Int_t j=0; j<fFourierHistos[0].dataFourierIm->GetNbinsX(); j++) {
      dval = 0.0;
      for (UInt_t i=0; i<fFourierHistos.size(); i++) {
        if (j < fFourierHistos[i].dataFourierIm->GetNbinsX())
          dval += GetInterpolatedValue(fFourierHistos[i].dataFourierIm, fFourierHistos[0].dataFourierIm->GetBinCenter(j));
      }
      fFourierAverage[0].dataFourierIm->SetBinContent(j, dval/fFourierHistos.size());
    }
    // set marker color, line color, maker size, marker type
    fFourierAverage[0].dataFourierIm->SetMarkerColor(kBlack);
    fFourierAverage[0].dataFourierIm->SetLineColor(kBlack);
    fFourierAverage[0].dataFourierIm->SetMarkerSize(0.8);
    fFourierAverage[0].dataFourierIm->SetMarkerStyle(22); // closed up triangle

    // power average
    for (Int_t j=0; j<fFourierHistos[0].dataFourierPwr->GetNbinsX(); j++) {
      dval = 0.0;
      for (UInt_t i=0; i<fFourierHistos.size(); i++) {
        if (j < fFourierHistos[i].dataFourierPwr->GetNbinsX())
          dval += GetInterpolatedValue(fFourierHistos[i].dataFourierPwr, fFourierHistos[0].dataFourierPwr->GetBinCenter(j));
      }
      fFourierAverage[0].dataFourierPwr->SetBinContent(j, dval/fFourierHistos.size());
    }
    // set marker color, line color, maker size, marker type
    fFourierAverage[0].dataFourierPwr->SetMarkerColor(kBlack);
    fFourierAverage[0].dataFourierPwr->SetLineColor(kBlack);
    fFourierAverage[0].dataFourierPwr->SetMarkerSize(0.8);
    fFourierAverage[0].dataFourierPwr->SetMarkerStyle(22); // closed up triangle

    // phase average
    for (Int_t j=0; j<fFourierHistos[0].dataFourierPhase->GetNbinsX(); j++) {
      dval = 0.0;
      for (UInt_t i=0; i<fFourierHistos.size(); i++) {
        if (j < fFourierHistos[i].dataFourierPhase->GetNbinsX())
          dval += GetInterpolatedValue(fFourierHistos[i].dataFourierPhase, fFourierHistos[0].dataFourierPhase->GetBinCenter(j));
      }
      fFourierAverage[0].dataFourierPhase->SetBinContent(j, dval/fFourierHistos.size());
    }
    // set marker color, line color, maker size, marker type
    fFourierAverage[0].dataFourierPhase->SetMarkerColor(kBlack);
    fFourierAverage[0].dataFourierPhase->SetLineColor(kBlack);
    fFourierAverage[0].dataFourierPhase->SetMarkerSize(0.8);
    fFourierAverage[0].dataFourierPhase->SetMarkerStyle(22);

    if (phaseOptRealPresent) {
      // phase optimised real average
      for (Int_t j=0; j<fFourierHistos[0].dataFourierPhaseOptReal->GetNbinsX(); j++) {
        dval = 0.0;
        for (UInt_t i=0; i<fFourierHistos.size(); i++) {
          if (j < fFourierHistos[i].dataFourierPhaseOptReal->GetNbinsX())
            dval += GetInterpolatedValue(fFourierHistos[i].dataFourierPhaseOptReal, fFourierHistos[0].dataFourierPhaseOptReal->GetBinCenter(j));
        }
        fFourierAverage[0].dataFourierPhaseOptReal->SetBinContent(j, dval/fFourierHistos.size());
      }
      // set marker color, line color, maker size, marker type
      fFourierAverage[0].dataFourierPhaseOptReal->SetMarkerColor(kBlack);
      fFourierAverage[0].dataFourierPhaseOptReal->SetLineColor(kBlack);
      fFourierAverage[0].dataFourierPhaseOptReal->SetMarkerSize(0.8);
      fFourierAverage[0].dataFourierPhaseOptReal->SetMarkerStyle(22);
    }
  }

  // check if per data set shall be averaged
  if (fAveragedViewPerDataSet) {
    // check what size is required
    UInt_t count = 1;
    Int_t currentTag = fDataSetTag[0];
    for (UInt_t i=1; i<fDataSetTag.size(); i++) {
      if (fDataSetTag[i] != currentTag) {
        currentTag = fDataSetTag[i];
        count++;
      }
    }
    fFourierAverage.resize(count);

    Int_t start=0, end=0;
    for (UInt_t i=0; i<fFourierAverage.size(); i++) {
      // get data set range
      count = 0;
      currentTag = fDataSetTag[0];
      start = -1, end = -1;
      for (UInt_t j=0; j<fDataSetTag.size(); j++) {
        if ((count == i) && (start == -1)) {
          if (j > 0)
            start = j-1;
          else
            start = 0;
        }
        if (currentTag != fDataSetTag[j]) {
          if (count == i) {
            end = j-1;
            break;
          }
          count++;
          currentTag = fDataSetTag[j];
        }
      }
      if (start == -1)
        start = fDataSetTag.size()-1;
      if (end == -1)
        end = fDataSetTag.size()-1;

      // create average histograms
      name  = TString(fFourierHistos[start].dataFourierRe->GetTitle()) + "_avg";
      fFourierAverage[i].dataFourierRe = new TH1F(name, name, fFourierHistos[0].dataFourierRe->GetNbinsX(),
                                                  fFourierHistos[0].dataFourierRe->GetXaxis()->GetXmin(),
                                                  fFourierHistos[0].dataFourierRe->GetXaxis()->GetXmax());

      name  = TString(fFourierHistos[start].dataFourierIm->GetTitle()) + "_avg";
      fFourierAverage[i].dataFourierIm = new TH1F(name, name, fFourierHistos[0].dataFourierIm->GetNbinsX(),
                                                  fFourierHistos[0].dataFourierIm->GetXaxis()->GetXmin(),
                                                  fFourierHistos[0].dataFourierIm->GetXaxis()->GetXmax());

      name  = TString(fFourierHistos[start].dataFourierPwr->GetTitle()) + "_avg";
      fFourierAverage[i].dataFourierPwr = new TH1F(name, name, fFourierHistos[0].dataFourierPwr->GetNbinsX(),
                                                   fFourierHistos[0].dataFourierPwr->GetXaxis()->GetXmin(),
                                                   fFourierHistos[0].dataFourierPwr->GetXaxis()->GetXmax());

      name  = TString(fFourierHistos[start].dataFourierPhase->GetTitle()) + "_avg";
      fFourierAverage[i].dataFourierPhase = new TH1F(name, name, fFourierHistos[0].dataFourierPhase->GetNbinsX(),
                                                     fFourierHistos[0].dataFourierPhase->GetXaxis()->GetXmin(),
                                                     fFourierHistos[0].dataFourierPhase->GetXaxis()->GetXmax());

      if (phaseOptRealPresent) {
        name  = TString(fFourierHistos[start].dataFourierPhaseOptReal->GetTitle()) + "_avg";
        fFourierAverage[i].dataFourierPhaseOptReal = new TH1F(name, name, fFourierHistos[0].dataFourierPhaseOptReal->GetNbinsX(),
                                                              fFourierHistos[0].dataFourierPhaseOptReal->GetXaxis()->GetXmin(),
                                                              fFourierHistos[0].dataFourierPhaseOptReal->GetXaxis()->GetXmax());
      }

      // real average
      for (Int_t j=0; j<fFourierHistos[0].dataFourierRe->GetNbinsX(); j++) {
        dval = 0.0;
        for (Int_t k=start; k<=end; k++) {
          if (j < fFourierHistos[k].dataFourierRe->GetNbinsX())
            dval += GetInterpolatedValue(fFourierHistos[k].dataFourierRe, fFourierHistos[0].dataFourierRe->GetBinCenter(j));
        }
        fFourierAverage[i].dataFourierRe->SetBinContent(j, dval/(end-start+1));
      }
      // set marker color, line color, maker size, marker type
      fFourierAverage[i].dataFourierRe->SetMarkerColor(fColorList[i]);
      fFourierAverage[i].dataFourierRe->SetLineColor(fColorList[i]);
      fFourierAverage[i].dataFourierRe->SetMarkerSize(0.8);
      fFourierAverage[i].dataFourierRe->SetMarkerStyle(22); // closed up triangle

      // imaginary average
      for (Int_t j=0; j<fFourierHistos[0].dataFourierIm->GetNbinsX(); j++) {
        dval = 0.0;
        for (Int_t k=start; k<=end; k++) {
          if (j < fFourierHistos[k].dataFourierIm->GetNbinsX())
            dval += GetInterpolatedValue(fFourierHistos[k].dataFourierIm, fFourierHistos[0].dataFourierIm->GetBinCenter(j));
        }
        fFourierAverage[i].dataFourierIm->SetBinContent(j, dval/(end-start+1));
      }
      // set marker color, line color, maker size, marker type
      fFourierAverage[i].dataFourierIm->SetMarkerColor(fColorList[i]);
      fFourierAverage[i].dataFourierIm->SetLineColor(fColorList[i]);
      fFourierAverage[i].dataFourierIm->SetMarkerSize(0.8);
      fFourierAverage[i].dataFourierIm->SetMarkerStyle(22); // closed up triangle

      // power average
      for (Int_t j=0; j<fFourierHistos[0].dataFourierPwr->GetNbinsX(); j++) {
        dval = 0.0;
        for (Int_t k=start; k<=end; k++) {
          if (j < fFourierHistos[k].dataFourierPwr->GetNbinsX())
            dval += GetInterpolatedValue(fFourierHistos[k].dataFourierPwr, fFourierHistos[0].dataFourierPwr->GetBinCenter(j));
        }
        fFourierAverage[i].dataFourierPwr->SetBinContent(j, dval/(end-start+1));
      }
      // set marker color, line color, maker size, marker type
      fFourierAverage[i].dataFourierPwr->SetMarkerColor(fColorList[i]);
      fFourierAverage[i].dataFourierPwr->SetLineColor(fColorList[i]);
      fFourierAverage[i].dataFourierPwr->SetMarkerSize(0.8);
      fFourierAverage[i].dataFourierPwr->SetMarkerStyle(22); // closed up triangle

      // phase average
      for (Int_t j=0; j<fFourierHistos[0].dataFourierPhase->GetNbinsX(); j++) {
        dval = 0.0;
        for (Int_t k=start; k<=end; k++) {
          if (j < fFourierHistos[k].dataFourierPhase->GetNbinsX())
            dval += GetInterpolatedValue(fFourierHistos[k].dataFourierPhase, fFourierHistos[0].dataFourierPhase->GetBinCenter(j));
        }
        fFourierAverage[i].dataFourierPhase->SetBinContent(j, dval/(end-start+1));
      }
      // set marker color, line color, maker size, marker type
      fFourierAverage[i].dataFourierPhase->SetMarkerColor(fColorList[i]);
      fFourierAverage[i].dataFourierPhase->SetLineColor(fColorList[i]);
      fFourierAverage[i].dataFourierPhase->SetMarkerSize(0.8);
      fFourierAverage[i].dataFourierPhase->SetMarkerStyle(22); // closed up triangle

      if (phaseOptRealPresent) {
        // phase optimised real average
        for (Int_t j=0; j<fFourierHistos[0].dataFourierPhaseOptReal->GetNbinsX(); j++) {
          dval = 0.0;
          for (Int_t k=start; k<=end; k++) {
            if (j < fFourierHistos[k].dataFourierPhaseOptReal->GetNbinsX())
              dval += GetInterpolatedValue(fFourierHistos[k].dataFourierPhaseOptReal, fFourierHistos[0].dataFourierPhaseOptReal->GetBinCenter(j));
          }
          fFourierAverage[i].dataFourierPhaseOptReal->SetBinContent(j, dval/(end-start+1));
        }
        // set marker color, line color, maker size, marker type
        fFourierAverage[i].dataFourierPhaseOptReal->SetMarkerColor(fColorList[i]);
        fFourierAverage[i].dataFourierPhaseOptReal->SetLineColor(fColorList[i]);
        fFourierAverage[i].dataFourierPhaseOptReal->SetMarkerSize(0.8);
        fFourierAverage[i].dataFourierPhaseOptReal->SetMarkerStyle(22); // closed up triangle
      }
    }
  }
}

//--------------------------------------------------------------------------
// CalcPhaseOptReal (private)
//--------------------------------------------------------------------------
/**
 * <p>calculate the phase opt. real FT
 */
void PFourierCanvas::CalcPhaseOptReal()
{
  Int_t start = fFourierHistos[0].dataFourierRe->FindFixBin(fInitialXRange[0]);
  Int_t end   = fFourierHistos[0].dataFourierRe->FindFixBin(fInitialXRange[1]);

  Double_t dval=0.0, max=0.0;
  for (UInt_t i=0; i<fFourierHistos.size(); i++) {
    fFourierHistos[i].dataFourierPhaseOptReal = fFourier[i]->GetPhaseOptRealFourier(fFourierHistos[i].dataFourierRe,
                                                                                    fFourierHistos[i].dataFourierIm, fFourierHistos[i].optPhase, 1.0, fInitialXRange[0], fInitialXRange[1]);
    // normalize it
    max = 0.0;
    for (Int_t j=start; j<=end; j++) {
      dval = fFourierHistos[i].dataFourierPhaseOptReal->GetBinContent(j);
      if (fabs(dval) > max)
        max = dval;
    }
    for (Int_t j=1; j<fFourierHistos[i].dataFourierPhaseOptReal->GetNbinsX(); j++) {
      fFourierHistos[i].dataFourierPhaseOptReal->SetBinContent(j, fFourierHistos[i].dataFourierPhaseOptReal->GetBinContent(j)/fabs(max));
    }
    // set the marker and line color
    fFourierHistos[i].dataFourierPhaseOptReal->SetMarkerColor(fColorList[i]);
    fFourierHistos[i].dataFourierPhaseOptReal->SetLineColor(fColorList[i]);

    // set the marker symbol and size
    fFourierHistos[i].dataFourierPhaseOptReal->SetMarkerStyle(fMarkerList[i]);
    fFourierHistos[i].dataFourierPhaseOptReal->SetMarkerSize(0.7);
  }
}

//--------------------------------------------------------------------------
// PlotFourier (private)
//--------------------------------------------------------------------------
/**
 * <p>Plot the Fourier spectra.
 */
void PFourierCanvas::PlotFourier()
{
  // check if phase opt real Fourier spectra already exists if requested,
  // and if not calculate them first
  if (fCurrentPlotView == FOURIER_PLOT_PHASE_OPT_REAL) {
    if (fFourierHistos[0].dataFourierPhaseOptReal == nullptr) { // not yet calculated
      CalcPhaseOptReal();
    }
  }

  fFourierPad->cd();

  Double_t xmin=0, xmax=0;
  xmin = fInitialXRange[0];
  xmax = fInitialXRange[1];

  Double_t ymin=0, ymax=0;
  switch (fCurrentPlotView) {
  case FOURIER_PLOT_REAL:
    fFourierHistos[0].dataFourierRe->GetXaxis()->SetRangeUser(xmin, xmax);
    ymin = GetMinimum(fFourierHistos[0].dataFourierRe, xmin, xmax);
    ymax = GetMaximum(fFourierHistos[0].dataFourierRe, xmin, xmax);
    for (UInt_t i=1; i<fFourierHistos.size(); i++) {
      if (GetMaximum(fFourierHistos[i].dataFourierRe, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierHistos[i].dataFourierRe, xmin, xmax);
      if (GetMinimum(fFourierHistos[i].dataFourierRe, xmin, xmax) < ymin)
        ymin = GetMinimum(fFourierHistos[i].dataFourierRe, xmin, xmax);
    }
    fFourierHistos[0].dataFourierRe->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
    fFourierHistos[0].dataFourierRe->GetYaxis()->SetTitle("Real");
    fFourierHistos[0].dataFourierRe->GetYaxis()->SetTitleOffset(1.3);
    fFourierHistos[0].dataFourierRe->GetYaxis()->SetDecimals(kTRUE);
    fFourierHistos[0].dataFourierRe->GetXaxis()->SetTitle(fXaxisTitle.Data());
    fFourierHistos[0].dataFourierRe->Draw("p");
    for (UInt_t i=1; i<fFourierHistos.size(); i++) {
      fFourierHistos[i].dataFourierRe->Draw("psame");
    }
    break;
  case FOURIER_PLOT_IMAG:
    fFourierHistos[0].dataFourierIm->GetXaxis()->SetRangeUser(xmin, xmax);
    ymin = GetMinimum(fFourierHistos[0].dataFourierIm, xmin, xmax);
    ymax = GetMaximum(fFourierHistos[0].dataFourierIm, xmin, xmax);
    for (UInt_t i=1; i<fFourierHistos.size(); i++) {
      if (GetMaximum(fFourierHistos[i].dataFourierIm, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierHistos[i].dataFourierIm, xmin, xmax);
      if (GetMinimum(fFourierHistos[i].dataFourierIm, xmin, xmax) < ymin)
        ymin = GetMinimum(fFourierHistos[i].dataFourierIm, xmin, xmax);
    }
    fFourierHistos[0].dataFourierIm->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
    fFourierHistos[0].dataFourierIm->GetYaxis()->SetTitleOffset(1.3);
    fFourierHistos[0].dataFourierIm->GetYaxis()->SetDecimals(kTRUE);
    fFourierHistos[0].dataFourierIm->GetYaxis()->SetTitle("Imag");
    fFourierHistos[0].dataFourierIm->GetXaxis()->SetTitle(fXaxisTitle.Data());
    fFourierHistos[0].dataFourierIm->Draw("p");
    for (UInt_t i=1; i<fFourierHistos.size(); i++) {
      fFourierHistos[i].dataFourierIm->Draw("psame");
    }
    break;
  case FOURIER_PLOT_REAL_AND_IMAG:
    fFourierHistos[0].dataFourierRe->GetXaxis()->SetRangeUser(xmin, xmax);
    ymin = GetMinimum(fFourierHistos[0].dataFourierRe, xmin, xmax);
    ymax = GetMaximum(fFourierHistos[0].dataFourierRe, xmin, xmax);
    for (UInt_t i=1; i<fFourierHistos.size(); i++) {
      if (GetMaximum(fFourierHistos[i].dataFourierRe, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierHistos[i].dataFourierRe, xmin, xmax);
      if (GetMaximum(fFourierHistos[i].dataFourierIm, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierHistos[i].dataFourierIm, xmin, xmax);
      if (GetMinimum(fFourierHistos[i].dataFourierRe, xmin, xmax) < ymin)
        ymin = GetMinimum(fFourierHistos[i].dataFourierRe, xmin, xmax);
      if (GetMinimum(fFourierHistos[i].dataFourierIm, xmin, xmax) < ymin)
        ymin = GetMinimum(fFourierHistos[i].dataFourierIm, xmin, xmax);
    }
    fFourierHistos[0].dataFourierRe->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
    fFourierHistos[0].dataFourierRe->GetYaxis()->SetTitleOffset(1.3);
    fFourierHistos[0].dataFourierRe->GetYaxis()->SetDecimals(kTRUE);
    fFourierHistos[0].dataFourierRe->GetYaxis()->SetTitle("Real");
    fFourierHistos[0].dataFourierRe->GetXaxis()->SetTitle(fXaxisTitle.Data());
    fFourierHistos[0].dataFourierRe->Draw("p");
    for (UInt_t i=1; i<fFourierHistos.size(); i++) {
      fFourierHistos[i].dataFourierRe->Draw("psame");
    }
    for (UInt_t i=0; i<fFourierHistos.size(); i++) {
      fFourierHistos[i].dataFourierIm->Draw("psame");
    }
    break;
  case FOURIER_PLOT_POWER:
    // get maximum of Fourier data in the range
    fFourierHistos[0].dataFourierPwr->GetXaxis()->SetRangeUser(xmin, xmax);
    ymin = 0.0;
    ymax = GetMaximum(fFourierHistos[0].dataFourierPwr, xmin, xmax);
    for (UInt_t i=1; i<fFourierHistos.size(); i++) {
      if (GetMaximum(fFourierHistos[i].dataFourierPwr, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierHistos[i].dataFourierPwr, xmin, xmax);
    }
    fFourierHistos[0].dataFourierPwr->GetYaxis()->SetRangeUser(ymin, 1.03*ymax);
    fFourierHistos[0].dataFourierPwr->GetYaxis()->SetTitle("Power");
    fFourierHistos[0].dataFourierPwr->GetYaxis()->SetTitleOffset(1.3);
    fFourierHistos[0].dataFourierPwr->GetYaxis()->SetDecimals(kTRUE);
    fFourierHistos[0].dataFourierPwr->GetXaxis()->SetTitle(fXaxisTitle.Data());
    fFourierHistos[0].dataFourierPwr->Draw("p");
    for (UInt_t i=1; i<fFourierHistos.size(); i++) {
      fFourierHistos[i].dataFourierPwr->Draw("psame");
    }
    break;
  case FOURIER_PLOT_PHASE:
    fFourierHistos[0].dataFourierPhase->GetXaxis()->SetRangeUser(xmin, xmax);
    ymin = GetMinimum(fFourierHistos[0].dataFourierPhase, xmin, xmax);
    ymax = GetMaximum(fFourierHistos[0].dataFourierPhase, xmin, xmax);
    for (UInt_t i=1; i<fFourierHistos.size(); i++) {
      if (GetMaximum(fFourierHistos[i].dataFourierPhase, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierHistos[i].dataFourierPhase, xmin, xmax);
      if (GetMinimum(fFourierHistos[i].dataFourierPhase, xmin, xmax) < ymin)
        ymin = GetMinimum(fFourierHistos[i].dataFourierPhase, xmin, xmax);
    }
    fFourierHistos[0].dataFourierPhase->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
    fFourierHistos[0].dataFourierPhase->GetYaxis()->SetTitleOffset(1.3);
    fFourierHistos[0].dataFourierPhase->GetYaxis()->SetDecimals(kTRUE);
    fFourierHistos[0].dataFourierPhase->GetYaxis()->SetTitle("Phase");
    fFourierHistos[0].dataFourierPhase->GetXaxis()->SetTitle(fXaxisTitle.Data());
    fFourierHistos[0].dataFourierPhase->Draw("p");
    for (UInt_t i=1; i<fFourierHistos.size(); i++) {
      fFourierHistos[i].dataFourierPhase->Draw("psame");
    }
    break;
  case FOURIER_PLOT_PHASE_OPT_REAL:
    fFourierHistos[0].dataFourierPhaseOptReal->GetXaxis()->SetRangeUser(xmin, xmax);
    ymin = GetMinimum(fFourierHistos[0].dataFourierPhaseOptReal, xmin, xmax);
    ymax = GetMaximum(fFourierHistos[0].dataFourierPhaseOptReal, xmin, xmax);
    for (UInt_t i=1; i<fFourierHistos.size(); i++) {
      if (GetMaximum(fFourierHistos[i].dataFourierPhaseOptReal, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierHistos[i].dataFourierPhaseOptReal, xmin, xmax);
      if (GetMinimum(fFourierHistos[i].dataFourierPhaseOptReal, xmin, xmax) < ymin)
        ymin = GetMinimum(fFourierHistos[i].dataFourierPhaseOptReal, xmin, xmax);
    }
    fFourierHistos[0].dataFourierPhaseOptReal->GetYaxis()->SetRangeUser(ymin, 1.05*ymax);
    fFourierHistos[0].dataFourierPhaseOptReal->GetYaxis()->SetTitleOffset(1.3);
    fFourierHistos[0].dataFourierPhaseOptReal->GetYaxis()->SetDecimals(kTRUE);
    fFourierHistos[0].dataFourierPhaseOptReal->GetYaxis()->SetTitle("Phase Opt. Real");
    fFourierHistos[0].dataFourierPhaseOptReal->GetXaxis()->SetTitle(fXaxisTitle.Data());
    fFourierHistos[0].dataFourierPhaseOptReal->Draw("p");
    for (UInt_t i=1; i<fFourierHistos.size(); i++) {
      fFourierHistos[i].dataFourierPhaseOptReal->Draw("psame");
    }
    break;
  default:
    break;
  }

  fFourierPad->Update();

  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// PlotFourierPhaseValue (private)
//--------------------------------------------------------------------------
/**
 * <p>Writes the Fourier phase value into the data window.
 */
void PFourierCanvas::PlotFourierPhaseValue()
{
  // check if phase TLatex object is present
  if (fCurrentFourierPhaseText) {
    delete fCurrentFourierPhaseText;
    fCurrentFourierPhaseText = nullptr;
  }

  Double_t x, y;
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

  fFourierPad->cd();

  fCurrentFourierPhaseText->Draw();

  fFourierPad->Update();
}

//--------------------------------------------------------------------------
// PlotAverage (private)
//--------------------------------------------------------------------------
/**
 * <p>Plot the average of the given Fourier data sets.
 */
void PFourierCanvas::PlotAverage()
{
  fFourierPad->cd();

  if (fFourierAverage.size() == 0) { // ups, HandleAverage never called!
    HandleAverage();
  }

  Double_t xmin=0.0, xmax=0.0;
  xmin = fInitialXRange[0];
  xmax = fInitialXRange[1];
  Double_t ymin=0.0, ymax=0.0;

  if (fLegAvgPerDataSet) {
    fLegAvgPerDataSet->Clear();
    delete fLegAvgPerDataSet;
    fLegAvgPerDataSet = nullptr;
  }

  switch (fCurrentPlotView) {
  case FOURIER_PLOT_REAL:
    fFourierAverage[0].dataFourierRe->GetXaxis()->SetRangeUser(xmin, xmax);
    ymin = GetMinimum(fFourierAverage[0].dataFourierRe, xmin, xmax);
    ymax = GetMaximum(fFourierAverage[0].dataFourierRe, xmin, xmax);
    for (UInt_t i=1; i<fFourierAverage.size(); i++) {
      if (GetMaximum(fFourierAverage[i].dataFourierRe, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierAverage[i].dataFourierRe, xmin, xmax);
      if (GetMinimum(fFourierAverage[i].dataFourierRe, xmin, xmax) < ymin)
        ymin = GetMinimum(fFourierAverage[i].dataFourierRe, xmin, xmax);
    }
    fFourierAverage[0].dataFourierRe->GetYaxis()->SetRangeUser(1.03*ymin, 1.03*ymax);
    fFourierAverage[0].dataFourierRe->GetYaxis()->SetTitleOffset(1.3);
    fFourierAverage[0].dataFourierRe->GetYaxis()->SetDecimals(kTRUE);
    fFourierAverage[0].dataFourierRe->GetYaxis()->SetTitle("<Real>");
    fFourierAverage[0].dataFourierRe->GetXaxis()->SetTitle(fXaxisTitle.Data());
    fFourierAverage[0].dataFourierRe->GetXaxis()->SetRangeUser(xmin, xmax);
    fFourierAverage[0].dataFourierRe->Draw("p");
    break;
  case FOURIER_PLOT_IMAG:
    fFourierAverage[0].dataFourierIm->GetXaxis()->SetRangeUser(xmin, xmax);
    ymin = GetMinimum(fFourierAverage[0].dataFourierIm, xmin, xmax);
    ymax = GetMaximum(fFourierAverage[0].dataFourierIm, xmin, xmax);
    for (UInt_t i=1; i<fFourierAverage.size(); i++) {
      if (GetMaximum(fFourierAverage[i].dataFourierIm, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierAverage[i].dataFourierIm, xmin, xmax);
      if (GetMinimum(fFourierAverage[i].dataFourierIm, xmin, xmax) < ymin)
        ymin = GetMinimum(fFourierAverage[i].dataFourierIm, xmin, xmax);
    }
    fFourierAverage[0].dataFourierIm->GetYaxis()->SetRangeUser(1.03*ymin, 1.03*ymax);
    fFourierAverage[0].dataFourierIm->GetYaxis()->SetTitleOffset(1.3);
    fFourierAverage[0].dataFourierIm->GetYaxis()->SetDecimals(kTRUE);
    fFourierAverage[0].dataFourierIm->GetYaxis()->SetTitle("<Imag>");
    fFourierAverage[0].dataFourierIm->GetXaxis()->SetTitle(fXaxisTitle.Data());
    fFourierAverage[0].dataFourierIm->GetXaxis()->SetRangeUser(xmin, xmax);
    fFourierAverage[0].dataFourierIm->Draw("p");
    break;
  case FOURIER_PLOT_REAL_AND_IMAG:
    fFourierAverage[0].dataFourierRe->GetXaxis()->SetRangeUser(xmin, xmax);
    ymin = GetMinimum(fFourierAverage[0].dataFourierRe, xmin, xmax);
    ymax = GetMaximum(fFourierAverage[0].dataFourierRe, xmin, xmax);
    for (UInt_t i=1; i<fFourierAverage.size(); i++) {
      if (GetMaximum(fFourierAverage[i].dataFourierRe, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierAverage[i].dataFourierRe, xmin, xmax);
      if (GetMinimum(fFourierAverage[i].dataFourierRe, xmin, xmax) < ymin)
        ymin = GetMinimum(fFourierAverage[i].dataFourierRe, xmin, xmax);
    }
    fFourierAverage[0].dataFourierRe->GetYaxis()->SetRangeUser(1.03*ymin, 1.03*ymax);
    fFourierAverage[0].dataFourierRe->GetYaxis()->SetTitleOffset(1.3);
    fFourierAverage[0].dataFourierRe->GetYaxis()->SetDecimals(kTRUE);
    fFourierAverage[0].dataFourierRe->GetYaxis()->SetTitle("<Real+Imag>");
    fFourierAverage[0].dataFourierRe->GetXaxis()->SetTitle(fXaxisTitle.Data());
    fFourierAverage[0].dataFourierRe->GetXaxis()->SetRangeUser(xmin, xmax);
    fFourierAverage[0].dataFourierRe->Draw("p");
    fFourierAverage[0].dataFourierIm->Draw("psame");
    break;
  case FOURIER_PLOT_POWER:
    // get maximum of Fourier data in the range
    fFourierAverage[0].dataFourierPwr->GetXaxis()->SetRangeUser(xmin, xmax);
    ymin = 0.0;
    ymax = GetMaximum(fFourierAverage[0].dataFourierPwr, xmin, xmax);
    for (UInt_t i=1; i<fFourierAverage.size(); i++) {
      if (GetMaximum(fFourierAverage[i].dataFourierPwr, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierAverage[i].dataFourierPwr, xmin, xmax);
    }
    fFourierAverage[0].dataFourierPwr->GetYaxis()->SetRangeUser(ymin, 1.03*ymax);
    fFourierAverage[0].dataFourierPwr->GetYaxis()->SetTitleOffset(1.3);
    fFourierAverage[0].dataFourierPwr->GetYaxis()->SetDecimals(kTRUE);
    fFourierAverage[0].dataFourierPwr->GetYaxis()->SetTitle("<Power>");
    fFourierAverage[0].dataFourierPwr->GetXaxis()->SetTitle(fXaxisTitle.Data());
    fFourierAverage[0].dataFourierPwr->Draw("p");
    break;
  case FOURIER_PLOT_PHASE:
    fFourierAverage[0].dataFourierPhase->GetXaxis()->SetRangeUser(xmin, xmax);
    ymin = GetMinimum(fFourierAverage[0].dataFourierPhase, xmin, xmax);
    ymax = GetMaximum(fFourierAverage[0].dataFourierPhase, xmin, xmax);
    for (UInt_t i=1; i<fFourierAverage.size(); i++) {
      if (GetMaximum(fFourierAverage[i].dataFourierPhase, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierAverage[i].dataFourierPhase, xmin, xmax);
      if (GetMinimum(fFourierAverage[i].dataFourierPhase, xmin, xmax) < ymin)
        ymin = GetMinimum(fFourierAverage[i].dataFourierPhase, xmin, xmax);
    }
    fFourierAverage[0].dataFourierPhase->GetYaxis()->SetRangeUser(1.03*ymin, 1.03*ymax);
    fFourierAverage[0].dataFourierPhase->GetYaxis()->SetTitleOffset(1.3);
    fFourierAverage[0].dataFourierPhase->GetYaxis()->SetDecimals(kTRUE);
    fFourierAverage[0].dataFourierPhase->GetYaxis()->SetTitle("<Phase>");
    fFourierAverage[0].dataFourierPhase->GetXaxis()->SetTitle(fXaxisTitle.Data());
    fFourierAverage[0].dataFourierPhase->GetXaxis()->SetRangeUser(xmin, xmax);
    fFourierAverage[0].dataFourierPhase->Draw("p");
    break;
  case FOURIER_PLOT_PHASE_OPT_REAL:
    if (fFourierHistos[0].dataFourierPhaseOptReal == nullptr) {
      std::cout << "debug> need to calculate phase opt. average first ..." << std::endl;
      CalcPhaseOptReal();
      HandleAverage();
    }
    fFourierAverage[0].dataFourierPhaseOptReal->GetXaxis()->SetRangeUser(xmin, xmax);
    ymin = GetMinimum(fFourierAverage[0].dataFourierPhaseOptReal, xmin, xmax);
    ymax = GetMaximum(fFourierAverage[0].dataFourierPhaseOptReal, xmin, xmax);
    for (UInt_t i=1; i<fFourierAverage.size(); i++) {
      if (GetMaximum(fFourierAverage[i].dataFourierPhaseOptReal, xmin, xmax) > ymax)
        ymax = GetMaximum(fFourierAverage[i].dataFourierPhaseOptReal, xmin, xmax);
      if (GetMinimum(fFourierAverage[i].dataFourierPhaseOptReal, xmin, xmax) < ymin)
        ymin = GetMinimum(fFourierAverage[i].dataFourierPhaseOptReal, xmin, xmax);
    }
    fFourierAverage[0].dataFourierPhaseOptReal->GetYaxis()->SetRangeUser(ymin, 1.03*ymax);
    fFourierAverage[0].dataFourierPhaseOptReal->GetYaxis()->SetTitleOffset(1.3);
    fFourierAverage[0].dataFourierPhaseOptReal->GetYaxis()->SetDecimals(kTRUE);
    fFourierAverage[0].dataFourierPhaseOptReal->GetYaxis()->SetTitle("<Phase Opt. Real>");
    fFourierAverage[0].dataFourierPhaseOptReal->GetXaxis()->SetTitle(fXaxisTitle.Data());
    fFourierAverage[0].dataFourierPhaseOptReal->GetXaxis()->SetRangeUser(xmin, xmax);
    fFourierAverage[0].dataFourierPhaseOptReal->Draw("p");
    break;
  default:
    break;
  }

  if (fAveragedViewPerDataSet) { // plot all the rest
    fLegAvgPerDataSet = new TLegend(0.55, 0.4, 0.85, 0.6);
    TString str = GetDataSetName(fFourierAverage[0].dataFourierPwr->GetTitle());
    TString label = TString::Format("<%s>", str.Data());
    fLegAvgPerDataSet->AddEntry(fFourierAverage[0].dataFourierPwr, label.Data());
    for (UInt_t i=1; i<fFourierAverage.size(); i++) {
      switch (fCurrentPlotView) {
      case FOURIER_PLOT_REAL:
        fFourierAverage[i].dataFourierRe->Draw("psame");
        break;
      case FOURIER_PLOT_IMAG:
        fFourierAverage[i].dataFourierIm->Draw("psame");
        break;
      case FOURIER_PLOT_REAL_AND_IMAG:
        fFourierAverage[i].dataFourierRe->Draw("psame");
        fFourierAverage[i].dataFourierIm->Draw("psame");
        break;
      case FOURIER_PLOT_POWER:
        fFourierAverage[i].dataFourierPwr->Draw("psame");
        break;
      case FOURIER_PLOT_PHASE:
        fFourierAverage[i].dataFourierPhase->Draw("psame");
        break;
      case FOURIER_PLOT_PHASE_OPT_REAL:
        fFourierAverage[i].dataFourierPhaseOptReal->Draw("psame");
        break;
      default:
        break;
      }
      // add legend ?!?!
      str = GetDataSetName(fFourierAverage[i].dataFourierPwr->GetTitle());
      label = TString::Format("<%s>", str.Data());
      fLegAvgPerDataSet->AddEntry(fFourierAverage[i].dataFourierPwr, label.Data());
    }
    fLegAvgPerDataSet->Draw();
  }

  fFourierPad->Update();

  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// IncrementFourierPhase (private)
//--------------------------------------------------------------------------
/**
 * <p>Increments the Fourier phase and recalculate the real/imaginary part of the Fourier transform.
 */
void PFourierCanvas::IncrementFourierPhase()
{
  if ((fCurrentPlotView == FOURIER_PLOT_POWER) || (fCurrentPlotView == FOURIER_PLOT_PHASE))
    return;

  Double_t re, im;
  Double_t inc = 1.0;
  const Double_t cp = TMath::Cos(inc/180.0*TMath::Pi());
  const Double_t sp = TMath::Sin(inc/180.0*TMath::Pi());

  fCurrentFourierPhase += inc;
  PlotFourierPhaseValue();

  for (UInt_t i=0; i<fFourierHistos.size(); i++) { // loop over all data sets
    if ((fFourierHistos[i].dataFourierRe != nullptr) && (fFourierHistos[i].dataFourierIm != nullptr)) {
      for (Int_t j=0; j<fFourierHistos[i].dataFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
        // calculate new fourier data set value
        re = fFourierHistos[i].dataFourierRe->GetBinContent(j) * cp + fFourierHistos[i].dataFourierIm->GetBinContent(j) * sp;
        im = fFourierHistos[i].dataFourierIm->GetBinContent(j) * cp - fFourierHistos[i].dataFourierRe->GetBinContent(j) * sp;
        // overwrite fourier data set value
        fFourierHistos[i].dataFourierRe->SetBinContent(j, re);
        fFourierHistos[i].dataFourierIm->SetBinContent(j, im);
      }
    }
  }
}

//--------------------------------------------------------------------------
// DecrementFourierPhase (private)
//--------------------------------------------------------------------------
/**
 * <p>Decrements the Fourier phase and recalculate the real/imaginary part of the Fourier transform.
 */
void PFourierCanvas::DecrementFourierPhase()
{
  if ((fCurrentPlotView == FOURIER_PLOT_POWER) || (fCurrentPlotView == FOURIER_PLOT_PHASE))
    return;

  Double_t re, im;
  Double_t inc = 1.0;
  const Double_t cp = TMath::Cos(inc/180.0*TMath::Pi());
  const Double_t sp = TMath::Sin(inc/180.0*TMath::Pi());

  fCurrentFourierPhase -= inc;
  PlotFourierPhaseValue();

  for (UInt_t i=0; i<fFourierHistos.size(); i++) { // loop over all data sets
    if ((fFourierHistos[i].dataFourierRe != nullptr) && (fFourierHistos[i].dataFourierIm != nullptr)) {
      for (Int_t j=0; j<fFourierHistos[i].dataFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
        // calculate new fourier data set value
        re = fFourierHistos[i].dataFourierRe->GetBinContent(j) * cp - fFourierHistos[i].dataFourierIm->GetBinContent(j) * sp;
        im = fFourierHistos[i].dataFourierIm->GetBinContent(j) * cp + fFourierHistos[i].dataFourierRe->GetBinContent(j) * sp;
        // overwrite fourier data set value
        fFourierHistos[i].dataFourierRe->SetBinContent(j, re);
        fFourierHistos[i].dataFourierIm->SetBinContent(j, im);
      }
    }
  }
}

//--------------------------------------------------------------------------
// GetMaximum (private)
//--------------------------------------------------------------------------
/**
 * <p>returns the maximum of a histogram in the range [xmin, xmax].
 * If xmin = xmax = -1.0, the whole histogram range is used.
 *
 * <b>return:</b>
 * - maximum, or 0.0 if the histo pointer is the null pointer.
 *
 * \param histo pointer of the histogram
 * \param xmin lower edge for the search interval.
 * \param xmax upper edge for the search interval.
 */
Double_t PFourierCanvas::GetMaximum(TH1F* histo, Double_t xmin, Double_t xmax)
{
  if (histo == nullptr)
    return 0.0;

  Int_t start=0, end=0;
  if (xmin == xmax) {
    start = 1;
    end = histo->GetNbinsX();
  } else {
    start = histo->FindBin(xmin);
    if ((start==0) || (start==histo->GetNbinsX()+1)) // underflow/overflow
      start = 1;
    end = histo->FindBin(xmax);
    if ((end==0) || (end==histo->GetNbinsX()+1)) // underflow/overflow
      end = histo->GetNbinsX();
  }

  Double_t max = histo->GetBinContent(start);
  Double_t binContent;
  for (Int_t i=start; i<end; i++) {
    binContent = histo->GetBinContent(i);
    if (max < binContent)
      max = binContent;
  }

  return max;
}

//--------------------------------------------------------------------------
// GetMinimum (private)
//--------------------------------------------------------------------------
/**
 * <p>returns the minimum of a histogram in the range [xmin, xmax].
 * If xmin = xmax = -1.0, the whole histogram range is used.
 *
 * <b>return:</b>
 * - minimum, or 0.0 if the histo pointer is the null pointer.
 *
 * \param histo pointer of the histogram
 * \param xmin lower edge for the search interval.
 * \param xmax upper edge for the search interval.
 */
Double_t PFourierCanvas::GetMinimum(TH1F* histo, Double_t xmin, Double_t xmax)
{
  if (histo == nullptr)
    return 0.0;

  Int_t start=0, end=0;
  if (xmin == xmax) {
    start = 1;
    end = histo->GetNbinsX();
  } else {
    start = histo->FindBin(xmin);
    if ((start==0) || (start==histo->GetNbinsX()+1)) // underflow/overflow
      start = 1;
    end = histo->FindBin(xmax);
    if ((end==0) || (end==histo->GetNbinsX()+1)) // underflow/overflow
      end = histo->GetNbinsX();
  }

  Double_t min = histo->GetBinContent(start);
  Double_t binContent;
  for (Int_t i=start; i<end; i++) {
    binContent = histo->GetBinContent(i);
    if (min > binContent)
      min = binContent;
  }

  return min;
}

//--------------------------------------------------------------------------
// GetInterpolatedValue (private)
//--------------------------------------------------------------------------
/**
 * <p>search for xVal in histo. If xVal is not found exactly, interpolate and
 * return the interpolated y-value.
 *
 * <b>return:</b>
 * - interpolated value if xVal is within histo range, 0 otherwise.
 *
 * \param histo pointer of the histogram
 * \param xVal x-value to be looked for
 */
Double_t PFourierCanvas::GetInterpolatedValue(TH1F* histo, Double_t xVal)
{
  if (histo == nullptr)
    return 0.0;

  Int_t idx = histo->FindBin(xVal);

  // make sure idx is within range
  if ((idx < 1) || (idx > histo->GetNbinsX()))
    return 0.0;

  // make sure the lower bound idx is found. This is needed since
  // FindBin rounds towards the closer idx.
  if (histo->GetBinCenter(idx) > xVal)
    --idx;

  Double_t x0, x1, y0, y1;
  x0 = histo->GetBinCenter(idx);
  x1 = histo->GetBinCenter(idx+1);
  y0 = histo->GetBinContent(idx);
  y1 = histo->GetBinContent(idx+1);

  return (y1-y0)*(xVal-x0)/(x1-x0)+y0;
}

//--------------------------------------------------------------------------
// GetDataSetName (private)
//--------------------------------------------------------------------------
/**
 * <p>filters out the data set name from the title
 *
 * <b>return:</b>
 * - data set name
 *
 * \param title title of the Fourier histogram
 */
TString PFourierCanvas::GetDataSetName(TString title)
{
  TString str(title);

  Ssiz_t idx = str.First(':');
  str.Remove(0, idx+1);
  idx = str.First(',');
  str.Remove(idx);

  return str;
}
