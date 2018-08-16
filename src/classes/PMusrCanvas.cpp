/***************************************************************************

  PMusrCanvas.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2019 by Andreas Suter                              *
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
#include <iomanip>
#include <fstream>

#include <TColor.h>
#include <TRandom.h>
#include <TROOT.h>
#include <TObjString.h>
#include <TGFileDialog.h>

#include "PMusrCanvas.h"
#include "PFourier.h"

static const char *gFiletypes[] = { "Data files", "*.dat",
                                    "All files",  "*",
                                    nullptr, nullptr };

ClassImp(PMusrCanvasPlotRange)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 */
PMusrCanvasPlotRange::PMusrCanvasPlotRange()
{
  fXRangePresent = false;
  fYRangePresent = false;

  fXmin = 0.0;
  fXmax = 0.0;
  fYmin = 0.0;
  fYmax = 0.0;
}

//--------------------------------------------------------------------------
// SetXRange (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the x-range values.
 *
 * \param xmin minimum range value
 * \param xmax maximum range value
 */
void PMusrCanvasPlotRange::SetXRange(Double_t xmin, Double_t xmax)
{
  if (xmin > xmax) {
    std::cerr << std::endl << ">> PMusrCanvasPlotRange::SetXRange(): **WARNING** xmin > xmax, will swap them." << std::endl;
    fXmin = xmax;
    fXmax = xmin;
  } else {
    fXmin = xmin;
    fXmax = xmax;
  }
  fXRangePresent = true;
}

//--------------------------------------------------------------------------
// SetYRange (public)
//--------------------------------------------------------------------------
/**
 * <p>Sets the y-range values.
 *
 * \param ymin minimum range value
 * \param ymax maximum range value
 */
void PMusrCanvasPlotRange::SetYRange(Double_t ymin, Double_t ymax)
{
  if (ymin > ymax) {
    std::cerr << std::endl << ">> PMusrCanvasPlotRange::SetYRange(): **WARNING** ymin > ymax, will swap them." << std::endl;
    fYmin = ymax;
    fYmax = ymin;
  } else {
    fYmin = ymin;
    fYmax = ymax;
  }
  fYRangePresent = true;
}


ClassImpQ(PMusrCanvas)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor
 */
PMusrCanvas::PMusrCanvas()
{
  fTimeout = 0;
  fTimeoutTimer = nullptr;

  fScaleN0AndBkg = true;
  fValid = false;
  fAveragedView     = false;
  fDifferenceView   = false;
  fCurrentPlotView  = PV_DATA;
  fPreviousPlotView = PV_DATA;
  fPlotType = -1;
  fPlotNumber = -1;

  fImp   = nullptr;
  fBar   = nullptr;
  fPopupMain    = nullptr;
  fPopupFourier = nullptr;

  fStyle            = nullptr;
  fMainCanvas       = nullptr;
  fTitlePad         = nullptr;
  fDataTheoryPad    = nullptr;
  fParameterPad     = nullptr;
  fTheoryPad        = nullptr;
  fInfoPad          = nullptr;
  fMultiGraphLegend = nullptr;

  fHistoFrame     = nullptr;

  fMultiGraphData = nullptr;
  fMultiGraphDiff = nullptr;

  InitFourier();
  InitAverage();

  fCurrentFourierPhaseText = nullptr;

  fRRFText      = nullptr;
  fRRFLatexText = nullptr;

  fXRangePresent = false;
  fYRangePresent = false;
  fXmin = 0.0;
  fXmax = 0.0;
  fYmin = 0.0;
  fYmax = 0.0;

  gStyle->SetHistMinimumZero(kTRUE); // needed to enforce proper bar option handling
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param number The plot number of the msr-file PLOT block
 * \param title Title to be displayed
 * \param wtopx top x coordinate (in pixels) to place the canvas.
 * \param wtopy top y coordinate (in pixels) to place the canvas.
 * \param ww width (in pixels) of the canvas.
 * \param wh height (in pixels) of the canvas.
 * \param batch flag: if set true, the canvas will not be displayed. This is used when just dumping of a
 *               graphical output file is wished.
 * \param fourier flag: if set true, the canvas will present the Fourier view.
 * \param avg flag: if set true, the canvas will present the averages data/Fourier view.
 */
PMusrCanvas::PMusrCanvas(const Int_t number, const Char_t* title,
                         Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
                         const Bool_t batch, const Bool_t fourier, const Bool_t avg) :
    fStartWithFourier(fourier), fStartWithAvg(avg),
    fBatchMode(batch), fPlotNumber(number)
{
  fTimeout = 0;
  fTimeoutTimer = nullptr;
  fAveragedView = false;

  fMultiGraphData = nullptr;
  fMultiGraphDiff = nullptr;

  fHistoFrame     = nullptr;

  InitFourier();
  InitAverage();
  CreateStyle();
  InitMusrCanvas(title, wtopx, wtopy, ww, wh);

  fCurrentFourierPhaseText = nullptr;

  fRRFText      = nullptr;
  fRRFLatexText = nullptr;

  fXRangePresent = false;
  fYRangePresent = false;
  fXmin = 0.0;
  fXmax = 0.0;
  fYmin = 0.0;
  fYmax = 0.0;

  gStyle->SetHistMinimumZero(kTRUE); // needed to enforce proper bar option handling
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param number The plot number of the msr-file PLOT block
 * \param title Title to be displayed
 * \param wtopx top x coordinate (in pixels) to place the canvas.
 * \param wtopy top y coordinate (in pixels) to place the canvas.
 * \param ww width (in pixels) of the canvas.
 * \param wh height (in pixels) of the canvas.
 * \param fourierDefault structure holding the pre-defined settings for a Fourier transform
 * \param markerList pre-defined list of markers
 * \param colorList pre-defined list of colors
 * \param batch flag: if set true, the canvas will not be displayed. This is used when just dumping of a
 *               graphical output file is wished.
 * \param fourier flag: if set true, the canvas will present the Fourier view.
 * \param avg flag: if set true, the canvas will present the averages data/Fourier view.
 */
PMusrCanvas::PMusrCanvas(const Int_t number, const Char_t* title,
                         Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
                         PMsrFourierStructure fourierDefault,
                         const PIntVector markerList, const PIntVector colorList,
                         const Bool_t batch, const Bool_t fourier, const Bool_t avg) :
     fStartWithFourier(fourier), fStartWithAvg(avg), fBatchMode(batch),
     fPlotNumber(number), fFourier(fourierDefault),
     fMarkerList(markerList), fColorList(colorList)
{
  fTimeout = 0;
  fTimeoutTimer = nullptr;

  fMultiGraphData = nullptr;
  fMultiGraphDiff = nullptr;

  fHistoFrame     = nullptr;

  InitAverage();
  CreateStyle();
  InitMusrCanvas(title, wtopx, wtopy, ww, wh);

  fCurrentFourierPhaseText = nullptr;

  fRRFText      = nullptr;
  fRRFLatexText = nullptr;

  fXRangePresent = false;
  fYRangePresent = false;
  fXmin = 0.0;
  fXmax = 0.0;
  fYmin = 0.0;
  fYmax = 0.0;

  gStyle->SetHistMinimumZero(kTRUE); // needed to enforce proper bar option handling
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * <p>Destructor.
 */
PMusrCanvas::~PMusrCanvas()
{
  // cleanup
  if (fTimeoutTimer) {
    delete fTimeoutTimer;
    fTimeoutTimer = nullptr;
  }
  if (fCurrentFourierPhaseText) {
    delete fCurrentFourierPhaseText;
    fCurrentFourierPhaseText = nullptr;
  }
  if (fRRFLatexText) {
    delete fRRFLatexText;
    fRRFLatexText = nullptr;
  }
  if (fRRFText) {
    delete fRRFText;
    fRRFText = nullptr;
  }
  if (fStyle) {
    delete fStyle;
    fStyle = nullptr;
  }
  if (fTitlePad) {
    fTitlePad->Clear();
    delete fTitlePad;
    fTitlePad = nullptr;
  }
  if (fData.size() > 0) {
    for (UInt_t i=0; i<fData.size(); i++)
      CleanupDataSet(fData[i]);
    fData.clear();
  }
  if (fNonMusrData.size() > 0) {
    for (UInt_t i=0; i<fNonMusrData.size(); i++)
      CleanupDataSet(fNonMusrData[i]);
    fNonMusrData.clear();
  }
  if (fMultiGraphLegend) {
    fMultiGraphLegend->Clear();
    delete fMultiGraphLegend;
    fMultiGraphLegend = nullptr;
  }
  if (fMultiGraphData) {
    delete fMultiGraphData;
    fMultiGraphData = nullptr;
  }
  if (fMultiGraphDiff) {
    delete fMultiGraphDiff;
    fMultiGraphDiff = nullptr;
  }
  if (fDataTheoryPad) {
    delete fDataTheoryPad;
    fDataTheoryPad = nullptr;
  }
  if (fParameterPad) {
    fParameterPad->Clear();
    delete fParameterPad;
    fParameterPad = nullptr;
  }
  if (fTheoryPad) {
    fTheoryPad->Clear();
    delete fTheoryPad;
    fTheoryPad = nullptr;
  }
  if (fInfoPad) {
    fInfoPad->Clear();
    delete fInfoPad;
    fInfoPad = nullptr;
  }
  if (fMainCanvas) {
    delete fMainCanvas;
    fMainCanvas = nullptr;
  }
}

//--------------------------------------------------------------------------
// SetMsrHandler (public)
//--------------------------------------------------------------------------
/**
 * <p>Keep the msr-handler object pointer and fill the Fourier structure if present.
 *
 * \param msrHandler pointer of the msr-file handler.
 */
void PMusrCanvas::SetMsrHandler(PMsrHandler *msrHandler)
{
  fMsrHandler = msrHandler;

  fScaleN0AndBkg = IsScaleN0AndBkg();

  // check if a fourier block is present in the msr-file, and if yes extract the given values
  if (fMsrHandler->GetMsrFourierList()->fFourierBlockPresent) {
    fFourier.fFourierBlockPresent = true;
    if (fMsrHandler->GetMsrFourierList()->fUnits != FOURIER_UNIT_NOT_GIVEN) {
      fFourier.fUnits = fMsrHandler->GetMsrFourierList()->fUnits;
    }
    if (fMsrHandler->GetMsrFourierList()->fFourierPower != -1) {
      fFourier.fFourierPower = fMsrHandler->GetMsrFourierList()->fFourierPower;
    }
    fFourier.fDCCorrected = fMsrHandler->GetMsrFourierList()->fDCCorrected;
    if (fMsrHandler->GetMsrFourierList()->fApodization != FOURIER_APOD_NOT_GIVEN) {
      fFourier.fApodization = fMsrHandler->GetMsrFourierList()->fApodization;
    }
    if (fMsrHandler->GetMsrFourierList()->fPlotTag != FOURIER_PLOT_NOT_GIVEN) {
      fFourier.fPlotTag = fMsrHandler->GetMsrFourierList()->fPlotTag;
    }
    fFourier.fPhase = fMsrHandler->GetMsrFourierList()->fPhase;

    if ((fMsrHandler->GetMsrFourierList()->fRangeForPhaseCorrection[0] != -1.0) &&
        (fMsrHandler->GetMsrFourierList()->fRangeForPhaseCorrection[1] != -1.0)) {
      fFourier.fRangeForPhaseCorrection[0] = fMsrHandler->GetMsrFourierList()->fRangeForPhaseCorrection[0];
      fFourier.fRangeForPhaseCorrection[1] = fMsrHandler->GetMsrFourierList()->fRangeForPhaseCorrection[1];
    }
    if ((fMsrHandler->GetMsrFourierList()->fPlotRange[0] != -1.0) &&
        (fMsrHandler->GetMsrFourierList()->fPlotRange[1] != -1.0)) {
      fFourier.fPlotRange[0] = fMsrHandler->GetMsrFourierList()->fPlotRange[0];
      fFourier.fPlotRange[1] = fMsrHandler->GetMsrFourierList()->fPlotRange[1];
    }
  }

  // check if RRF data are present
  if (((fMsrHandler->GetMsrPlotList()->at(0).fRRFPacking > 0) &&
      (fMsrHandler->GetMsrPlotList()->at(0).fRRFFreq != 0.0)) ||
      (fMsrHandler->GetMsrGlobal()->GetRRFPacking() > 0 &&
       fMsrHandler->GetMsrGlobal()->GetRRFUnit().CompareTo("??"))) {
    fRRFLatexText = new TLatex();
    fRRFLatexText->SetNDC(kTRUE);
    fRRFLatexText->SetTextFont(62);
    fRRFLatexText->SetTextSize(0.03);

    Int_t rrfUnitTag = -1;
    Double_t rrfFreq = 0.0;
    if (fMsrHandler->GetMsrPlotList()->at(0).fRRFPacking > 0) { // RRF single histo PLOT
      fRRFText = new TString("RRF: ");
      rrfUnitTag = fMsrHandler->GetMsrPlotList()->at(0).fRRFUnit;
      rrfFreq = fMsrHandler->GetMsrPlotList()->at(0).fRRFFreq;
      TString rrfFreqStr("");
      rrfFreqStr.Form("%.5g", rrfFreq);
      if (rrfUnitTag == RRF_UNIT_kHz) {
        *fRRFText += TString("#nu_{RRF} = ");
        *fRRFText += rrfFreq;
        *fRRFText += TString(" (kHz)");
      } else if (rrfUnitTag == RRF_UNIT_MHz) {
        *fRRFText += TString("#nu_{RRF} = ");
        *fRRFText += rrfFreqStr;
        *fRRFText += TString(" (MHz)");
      } else if (rrfUnitTag == RRF_UNIT_Mcs) {
        *fRRFText += TString("#omega_{RRF} = ");
        *fRRFText += rrfFreqStr;
        *fRRFText += TString(" (Mc/s)");
      } else if (rrfUnitTag == RRF_UNIT_G) {
        *fRRFText += TString("B_{RRF} = ");
        *fRRFText += rrfFreqStr;
        *fRRFText += TString(" (G)");
      } else if (rrfUnitTag == RRF_UNIT_T) {
        *fRRFText += TString("B_{RRF} = ");
        *fRRFText += rrfFreqStr;
        *fRRFText += TString(" (T)");
      }
      *fRRFText += TString(", RRF packing = ");
      *fRRFText += fMsrHandler->GetMsrPlotList()->at(0).fRRFPacking;
    } else { // RRF single histo FIT
      fRRFText = new TString("RRF: ");
      rrfUnitTag = fMsrHandler->GetMsrGlobal()->GetRRFUnitTag();
      rrfFreq = fMsrHandler->GetMsrGlobal()->GetRRFFreq(fMsrHandler->GetMsrGlobal()->GetRRFUnit().Data());
      TString rrfFreqStr("");
      rrfFreqStr.Form("%.5g", rrfFreq);
      if (rrfUnitTag == RRF_UNIT_kHz) {
        *fRRFText += TString("#nu_{RRF} = ");
        *fRRFText += rrfFreqStr;
        *fRRFText += TString(" (kHz)");
      } else if (rrfUnitTag == RRF_UNIT_MHz) {
        *fRRFText += TString("#nu_{RRF} = ");
        *fRRFText += rrfFreqStr;
        *fRRFText += TString(" (MHz)");
      } else if (rrfUnitTag == RRF_UNIT_Mcs) {
        *fRRFText += TString("#omega_{RRF} = ");
        *fRRFText += rrfFreqStr;
        *fRRFText += TString(" (Mc/s)");
      } else if (rrfUnitTag == RRF_UNIT_G) {
        *fRRFText += TString("B_{RRF} = ");
        *fRRFText += rrfFreqStr;
        *fRRFText += TString(" (G)");
      } else if (rrfUnitTag == RRF_UNIT_T) {
        *fRRFText += TString("B_{RRF} = ");
        *fRRFText += rrfFreqStr;
        *fRRFText += TString(" (T)");
      }
      *fRRFText += TString(", RRF packing = ");
      *fRRFText += fMsrHandler->GetMsrGlobal()->GetRRFPacking();
    }
  }
}

//--------------------------------------------------------------------------
// SetTimeout (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param timeout after which the done signal shall be emitted. Given in seconds
 */
void PMusrCanvas::SetTimeout(Int_t timeout)
{
  fTimeout = timeout;

  if (fTimeout <= 0)
    return;

  if (fTimeoutTimer) {
    delete fTimeoutTimer;
    fTimeoutTimer = nullptr;
  }
  fTimeoutTimer = new TTimer();

  fTimeoutTimer->Connect("Timeout()", "PMusrCanvas", this, "Done()");

  fTimeoutTimer->Start(1000*fTimeout, kTRUE);
}

//--------------------------------------------------------------------------
// UpdateParamTheoryPad (public)
//--------------------------------------------------------------------------
/**
 * <p>Feeds the pad with the fit parameter informations, and refreshes the pad.
 */
void PMusrCanvas::UpdateParamTheoryPad()
{
  if (!fValid)
    return;

  TString  str;
  Char_t     cnum[128];
  Int_t      maxLength = 0;
  Double_t ypos = 0.0, yoffset = 0.0;
  Int_t      idx = -1;

  // add parameters ------------------------------------------------------------
  PMsrParamList param = *fMsrHandler->GetMsrParamList();

  // get maximal parameter name string length
  for (UInt_t i=0; i<param.size(); i++) {
    if (param[i].fName.Length() > maxLength)
      maxLength = param[i].fName.Length();
  }
  maxLength += 2;

  // calculate yoffset based on the number of parameters
  if (param.size() > 20)
    yoffset = 1.0 / (param.size()+1);
  else
    yoffset = 0.05;

  // add parameters to the pad
  UInt_t accuracy = 6;
  Char_t accStr[32];
  for (UInt_t i=0; i<param.size(); i++) {
    str = "";
    accuracy = GetNeededAccuracy(param[i]);
    sprintf(accStr, "%%.%dlf", accuracy);
    // parameter no
    str += param[i].fNo;
    if (param[i].fNo<10)
      str += "  ";
    else
      str += " ";
    // parameter name
    str += param[i].fName;
    for (Int_t j=0; j<maxLength-param[i].fName.Length(); j++) // fill spaces
      str += " ";
    // parameter value
    if (round(param[i].fValue)-param[i].fValue==0)
      sprintf(cnum, "%.1lf", param[i].fValue);
    else
      sprintf(cnum, accStr, param[i].fValue);
    str += cnum;
    for (Int_t j=0; j<9-(Int_t)strlen(cnum); j++) // fill spaces
      str += " ";
    str += " "; // to make sure that at least 1 space is placed
    // parameter error
    if (param[i].fPosErrorPresent) { // minos was used
      // calculate the arithmetic average of the pos. and neg. error
      Double_t err;
      err = (param[i].fPosError - param[i].fStep) / 2.0;
      // check if the pos. and neg. error within 10%
      if ((fabs(fabs(param[i].fStep) - param[i].fPosError) < 0.1*fabs(param[i].fStep)) &&
          (fabs(fabs(param[i].fStep) - param[i].fPosError) < 0.1*param[i].fPosError)) {
        if (round(err)-err==0)
          sprintf(cnum, "%.1lf", err);
        else
          sprintf(cnum, accStr, err);
      } else {
        sprintf(accStr, "%%.%dlf!!", accuracy);
        if (round(err)-err==0)
          sprintf(cnum, "%.1lf!!", err);
        else
          sprintf(cnum, accStr, err);
      }
      str += cnum;
    } else { // minos was not used
      if (round(param[i].fStep)-param[i].fStep==0)
        sprintf(cnum, "%.1lf", param[i].fStep);
      else
        sprintf(cnum, accStr, param[i].fStep);
      str += cnum;
    }
    ypos = 0.98-i*yoffset;
    fParameterPad->AddText(0.03, ypos, str.Data());
  }

  // add theory ------------------------------------------------------------
  PMsrLines theory = *fMsrHandler->GetMsrTheory();
  if (theory.size() > 20)
    yoffset = 1.0/(theory.size()+1);
  else
    yoffset = 0.05;
  for (UInt_t i=1; i<theory.size(); i++) {
    // remove comment if present
    str = theory[i].fLine;
    idx = str.Index("(");
    if (idx > 0) { // comment present
      str.Resize(idx-1);
      str.Resize(str.Strip().Length());
    }
    ypos = 0.98 - i*yoffset;
    fTheoryPad->AddText(0.03, ypos, str.Data());
  }

  // add functions --------------------------------------------------------
  ypos -= 0.05;
  PMsrLines functions = *fMsrHandler->GetMsrFunctions();
  for (UInt_t i=1; i<functions.size(); i++) {
    ypos -= 0.05;
    fTheoryPad->AddText(0.03, ypos, functions[i].fLine.Data());
  }


  fParameterPad->Draw();
  fTheoryPad->Draw();
  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// UpdateDataTheoryPad (public)
//--------------------------------------------------------------------------
/**
 * <p>Feeds the pad with data/theory histograms (error graphs) and refreshes it.
 */
void PMusrCanvas::UpdateDataTheoryPad()
{
  // some checks first
  UInt_t runNo;
  PMsrPlotStructure plotInfo = fMsrHandler->GetMsrPlotList()->at(fPlotNumber);
  PMsrRunList runs = *fMsrHandler->GetMsrRunList();
  PMsrGlobalBlock *globalBlock = fMsrHandler->GetMsrGlobal();

  Int_t fitType = globalBlock->GetFitType();

  fPlotType = plotInfo.fPlotType;
  for (UInt_t i=0; i<plotInfo.fRuns.size(); i++) {
    // first check that plot number is smaller than the maximal number of runs
    if ((Int_t)plotInfo.fRuns[i] > (Int_t)runs.size()) {
      fValid = false;
      std::cerr << std::endl << ">> PMusrCanvas::UpdateDataTheoryPad(): **ERROR** run plot number " << (Int_t)plotInfo.fRuns[i] << " is larger than the number of runs " << runs.size();
      std::cerr << std::endl;
      return;
    }
    // check that the plottype and the fittype do correspond
    runNo = (UInt_t)plotInfo.fRuns[i]-1;
    if (runs[runNo].GetFitType() != -1) { // fit type found in RUN block, hence overwrite the GLOBAL block
      fitType = runs[runNo].GetFitType();
    }
    if (fitType == -1) {
      fValid = false;
      std::cerr << std::endl << ">> PMusrCanvas::UpdateDataTheoryPad(): **ERROR** plottype = " << fPlotType;
      std::cerr << ", fittype = " << runs[runNo].GetFitType() << "(RUN block)/";
      std::cerr << "fittype = " << globalBlock->GetFitType() << "(GLOBAL block). However, they have to correspond!";
      std::cerr << std::endl;
      return;
    }
  }

  PRunData *data;
  for (UInt_t i=0; i<plotInfo.fRuns.size(); i++) {
    // get run data and create a histogram
    data = nullptr;
    runNo = (UInt_t)plotInfo.fRuns[i]-1;
    // get data depending on the fittype
    if (runs[runNo].GetFitType() != -1) { // fit type found in RUN block, hence overwrite the GLOBAL block
      fitType = runs[runNo].GetFitType();
    }
    switch (fitType) {
      case MSR_FITTYPE_SINGLE_HISTO:
        data = fRunList->GetSingleHisto(runNo, PRunListCollection::kRunNo);
        if (!data) { // something wrong
          fValid = false;
          // error message
          std::cerr << std::endl << ">> PMusrCanvas::UpdateDataTheoryPad(): **ERROR** couldn't obtain run no " << runNo << " for a single histogram plot";
          std::cerr << std::endl;
          return;
        }
        // handle data
        HandleDataSet(i, runNo, data);
        break;
      case MSR_FITTYPE_SINGLE_HISTO_RRF:
        data = fRunList->GetSingleHistoRRF(runNo, PRunListCollection::kRunNo);
        if (!data) { // something wrong
          fValid = false;
          // error message
          std::cerr << std::endl << ">> PMusrCanvas::UpdateDataTheoryPad(): **ERROR** couldn't obtain run no " << runNo << " for a single histogram RRF plot";
          std::cerr << std::endl;
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
          std::cerr << std::endl << ">> PMusrCanvas::UpdateDataTheoryPad(): **ERROR** couldn't obtain run no " << runNo << " for a asymmetry plot";
          std::cerr << std::endl;
          return;
        }
        // handle data
        HandleDataSet(i, runNo, data);
        break;
      case MSR_FITTYPE_ASYM_RRF:
        data = fRunList->GetAsymmetryRRF(runNo, PRunListCollection::kRunNo);
        if (!data) { // something wrong
          fValid = false;
          // error message
          std::cerr << std::endl << ">> PMusrCanvas::UpdateDataTheoryPad(): **ERROR** couldn't obtain run no " << runNo << " for a asymmetry RRF plot";
          std::cerr << std::endl;
          return;
        }
        // handle data
        HandleDataSet(i, runNo, data);
        break;
      case MSR_FITTYPE_MU_MINUS:
        data = fRunList->GetMuMinus(runNo, PRunListCollection::kRunNo);
        if (!data) { // something wrong
          fValid = false;
          // error message
          std::cerr << std::endl << ">> PMusrCanvas::UpdateDataTheoryPad(): **ERROR** couldn't obtain run no " << runNo << " for a mu minus single histogram plot";
          std::cerr << std::endl;
          return;
        }
        // handle data
        HandleDataSet(i, runNo, data);
        break;
      case MSR_FITTYPE_BNMR:
        data = fRunList->GetAsymmetryBNMR(runNo, PRunListCollection::kRunNo);
        if (!data) { // something wrong
          fValid = false;
          // error message
          cerr << endl << ">> PMusrCanvas::UpdateDataTheoryPad(): **ERROR** couldn't obtain run no " << runNo << " for a beta-NMR asymmetry plot";
          cerr << endl;
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
          std::cerr << std::endl << ">> PMusrCanvas::UpdateDataTheoryPad(): **ERROR** couldn't obtain run no " << runNo << " for a none musr data plot";
          std::cerr << std::endl;
          return;
        }
        // handle data
        HandleNonMusrDataSet(i, runNo, data);
        if (!fBatchMode) {
          // disable Fourier menus
          fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL);
          fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_IMAG);
          fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL_AND_IMAG);
          fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PWR);
          fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE);
          fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
          fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
        }
        break;
      default:
        fValid = false;
        // error message
        std::cerr << std::endl << ">> PMusrCanvas::UpdateDataTheoryPad(): **ERROR** wrong plottype tag?!";
        std::cerr << std::endl;
        return;
        break;
    }
  }

  // generate the histo plot
  if (!fStartWithFourier || (fPlotType == MSR_PLOT_NON_MUSR)) {
    PlotData();
  } else { // show Fourier straight ahead.
    // set the menu properly
    if (!fBatchMode)
      fPopupMain->UnCheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);

    // filter proper Fourier plot tag, and set the menu tags properly
    switch (fFourier.fPlotTag) {
      case FOURIER_PLOT_REAL:
        fCurrentPlotView = PV_FOURIER_REAL;
        if (!fBatchMode) {
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL);
          fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
          fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
        }
        break;
      case FOURIER_PLOT_IMAG:
        fCurrentPlotView = PV_FOURIER_IMAG;
        if (!fBatchMode) {
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_IMAG);
          fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
          fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
        }
        break;
      case FOURIER_PLOT_REAL_AND_IMAG:
        fCurrentPlotView = PV_FOURIER_REAL_AND_IMAG;
        if (!fBatchMode) {
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL_AND_IMAG);
          fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
          fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
        }
        break;
      case FOURIER_PLOT_POWER:
        fCurrentPlotView = PV_FOURIER_PWR;
        if (!fBatchMode) {
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PWR);
        }
        break;
      case FOURIER_PLOT_PHASE:
        fCurrentPlotView = PV_FOURIER_PHASE;
        if (!fBatchMode) {
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE);
        }
        break;
      case FOURIER_PLOT_PHASE_OPT_REAL:
        fCurrentPlotView = PV_FOURIER_PHASE_OPT_REAL;
        if (!fBatchMode) {
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_OPT_REAL);
        }
        break;
      default:
        fCurrentPlotView = PV_FOURIER_PWR;
        if (!fBatchMode) {
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PWR);
        }
        break;
    }

    HandleFourier();
    PlotFourier();
  }

  // if fStartWithAvg=true, start with averaged data/Fourier representation
  // fStartWithAvg is given at the command line level
  if (fStartWithAvg) {
    HandleCmdKey(kKeyPress, (Int_t)'a', 0, 0);
  }
}

//--------------------------------------------------------------------------
// UpdateInfoPad (public)
//--------------------------------------------------------------------------
/**
 * <p>Feeds the pad with the statistics block information and the legend and refreshes it.
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
  const PDoublePairVector *ddvec;
  Char_t   sval[128];
  UInt_t runNo;
  PMsrPlotStructure plotInfo = fMsrHandler->GetMsrPlotList()->at(fPlotNumber);
  PMsrRunList runs = *fMsrHandler->GetMsrRunList();
  for (UInt_t i=0; i<fData.size(); i++) {
    // run label = run_name/histo/T=0K/B=0G/E=0keV/...
    runNo = (UInt_t)plotInfo.fRuns[i]-1;
    if (runs[runNo].GetRunNameSize() > 1)
      tstr  = "++" + *runs[runNo].GetRunName() + TString(","); // run_name
    else
      tstr  = *runs[runNo].GetRunName() + TString(","); // run_name
    // histo info (depending on the fittype
    if ((runs[runNo].GetFitType() == MSR_FITTYPE_SINGLE_HISTO) ||
        (runs[runNo].GetFitType() == MSR_FITTYPE_SINGLE_HISTO_RRF)) {
      tstr += TString("h:");
      TString grouping;
      fMsrHandler->GetGroupingString(runNo, "forward", grouping);
      tstr += grouping;
      tstr += TString(",");
    } else if ((runs[runNo].GetFitType() == MSR_FITTYPE_ASYM) ||
               (runs[runNo].GetFitType() == MSR_FITTYPE_ASYM_RRF) ||
	       (runs[runNo].GetFitType() == MSR_FITTYPE_BNMR)) {
      tstr += TString("h:");
      TString grouping;
      fMsrHandler->GetGroupingString(runNo, "forward", grouping);
      tstr += grouping;
      tstr += TString("/");
      grouping = "";
      fMsrHandler->GetGroupingString(runNo, "backward", grouping);
      tstr += grouping;
      tstr += TString(",");
    }
    // temperature if present
    ddvec = fRunList->GetTemp(*runs[runNo].GetRunName());
    if (ddvec->empty()) {
      tstr += TString("T=");
      tstr += TString("??,");
    } else if (ddvec->size() == 1){
      tstr += TString("T=");
      sprintf(sval, "%0.2lf", ddvec->at(0).first);
      tstr += TString(sval) + TString("K,");
    } else {
      for(UInt_t i(0); i<ddvec->size(); ++i){
        sprintf(sval, "T%u=", i);
        tstr += TString(sval);
        sprintf(sval, "%0.2lf", ddvec->at(i).first);
        tstr += TString(sval) + TString("K,");
      }
    }
    // field if present
    tstr += TString("B=");
    dval = fRunList->GetField(*runs[runNo].GetRunName());
    if (dval == PMUSR_UNDEFINED) {
      tstr += TString("??,");
    } else {
      if (dval < 1.0e4) { // Gauss makes sense as a unit
        sprintf(sval, "%0.2lf", dval);
        tstr += TString(sval) + TString("G,");
      } else { // Tesla makes sense as a unit
        sprintf(sval, "%0.2lf", dval/1.0e4);
        tstr += TString(sval) + TString("T,");
      }
    }
    // energy if present
    tstr += TString("E=");
    dval = fRunList->GetEnergy(*runs[runNo].GetRunName());
    if (dval == PMUSR_UNDEFINED) {
      tstr += TString("??,");
    } else {
      if (dval < 1.0e3) { // keV makes sense as a unit
        sprintf(sval, "%0.2lf", dval);
        tstr += TString(sval) + TString("keV,");
      } else { // MeV makes sense as a unit
        sprintf(sval, "%0.2lf", dval/1.0e3);
        tstr += TString(sval) + TString("MeV,");
      }
    }
    // setup if present
    tstr += fRunList->GetSetup(*runs[runNo].GetRunName());
    // add entry
    fInfoPad->AddEntry(fData[i].data, tstr.Data(), "p");
  }

  fInfoPad->Draw();
  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// Done (SIGNAL)
//--------------------------------------------------------------------------
/**
 * <p>Signal emitted that the user wants to terminate the application.
 *
 * \param status Status info
 */
void PMusrCanvas::Done(Int_t status)
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
 * - 'd' toggle between difference view and data view
 * - 'u' unzoom to the original plot range given in the msr-file.
 * - 'f' Fourier transform data. Twice 'f' will switch back to the time domain view.
 * - '+' increment the phase (real/imaginary Fourier). The phase step is defined in the musrfit_startup.xml
 * - '-' decrement the phase (real/imaginary Fourier). The phase step is defined in the musrfit_startup.xml
 *
 * \param event event type
 * \param x character key
 * \param y not used
 * \param selected not used
 */
void PMusrCanvas::HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected)
{
  if (event != kKeyPress)
     return;

  if (fBatchMode) {
    if (fStartWithAvg) { // this is needed to get the averaging in the batch mode
      HandleAverage();
      PlotAverage(true);
    }
    return;
  }

  // handle keys and popup menu entries
  enum eKeySwitch {kNotRelevant, kData, kDiffData, kFourier, kDiffFourier, kFourierDiff};
  eKeySwitch relevantKeySwitch = kNotRelevant;
  static eKeySwitch lastKeySwitch = kNotRelevant;

  if ((lastKeySwitch == kFourierDiff) && (x == 'f')) {
    std::cout << "**INFO** f-d-f doesn't make any sense, will ignore 'f' ..." << std::endl;
    return;
  }

  if ((lastKeySwitch == kDiffFourier) && (x == 'd')) {
    std::cout << "**INFO** d-f-d doesn't make any sense, will ignore 'd' ..." << std::endl;
    return;
  }

  if (x == 'q') { // quit
    Done(0);
  } else if (x == 'd') { // difference
    // update previous plot view
    fPreviousPlotView = fCurrentPlotView;
    // toggle difference tag
    fDifferenceView = !fDifferenceView;
    // set the popup menu entry properly
    if (fDifferenceView) {
      fPopupMain->CheckEntry(P_MENU_ID_DIFFERENCE+P_MENU_PLOT_OFFSET*fPlotNumber);
    } else {
      fPopupMain->UnCheckEntry(P_MENU_ID_DIFFERENCE+P_MENU_PLOT_OFFSET*fPlotNumber);
    }
    // check which relevantKeySwitch is needed
    if ((fCurrentPlotView == PV_DATA) && fDifferenceView)
      relevantKeySwitch = kDiffData;
    else if ((fCurrentPlotView == PV_DATA) && !fDifferenceView)
      relevantKeySwitch = kData;
    else if ((fCurrentPlotView != PV_DATA) && fDifferenceView)
      relevantKeySwitch = kFourierDiff;
    else if ((fCurrentPlotView != PV_DATA) && !fDifferenceView)
      relevantKeySwitch = kFourier;
  } else if (x == 'u') { // unzoom to the original range
    // update previous plot view
    fPreviousPlotView = fCurrentPlotView;
    if ((fCurrentPlotView == PV_DATA) && !fDifferenceView) {
      CleanupDifference();
      CleanupFourier();
      PlotData(true);
    } else if ((fCurrentPlotView == PV_DATA) && fDifferenceView) {
      CleanupFourierDifference();
      HandleDifference();
      PlotDifference(true);
    } else if ((fCurrentPlotView != PV_DATA) && !fDifferenceView) {
      HandleFourier();
      PlotFourier(true);
    } else if ((fCurrentPlotView != PV_DATA) && fDifferenceView) {
      HandleDifferenceFourier();
      PlotFourierDifference(true);
    }
  } else if (x == 'f') { // Fourier
    // check which relevantKeySwitch is needed
    if ((fCurrentPlotView == PV_DATA) && fDifferenceView)
      relevantKeySwitch = kDiffFourier;
    else if ((fCurrentPlotView == PV_DATA) && !fDifferenceView)
      relevantKeySwitch = kFourier;
    else if ((fCurrentPlotView != PV_DATA) && fDifferenceView)
      relevantKeySwitch = kDiffData;
    else if ((fCurrentPlotView != PV_DATA) && !fDifferenceView)
      relevantKeySwitch = kData;

    if (fCurrentPlotView == PV_DATA) { // current view is data view
      // uncheck data popup entry
      fPopupMain->UnCheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
      // get default fourier tag and update fourier popup menu
      switch (fFourier.fPlotTag) {
        case FOURIER_PLOT_REAL:
          fPreviousPlotView = fCurrentPlotView;
          fCurrentPlotView = PV_FOURIER_REAL;
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL);
          break;
        case FOURIER_PLOT_IMAG:
          fPreviousPlotView = fCurrentPlotView;
          fCurrentPlotView = PV_FOURIER_IMAG;
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_IMAG);
          break;
        case FOURIER_PLOT_REAL_AND_IMAG:
          fPreviousPlotView = fCurrentPlotView;
          fCurrentPlotView = PV_FOURIER_REAL_AND_IMAG;
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL_AND_IMAG);
          break;
        case FOURIER_PLOT_POWER:
          fPreviousPlotView = fCurrentPlotView;
          fCurrentPlotView = PV_FOURIER_PWR;
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PWR);
          break;
        case FOURIER_PLOT_PHASE:
          fPreviousPlotView = fCurrentPlotView;
          fCurrentPlotView = PV_FOURIER_PHASE;
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE);
          break;
        case FOURIER_PLOT_PHASE_OPT_REAL:
          fPreviousPlotView = fCurrentPlotView;
          fCurrentPlotView = PV_FOURIER_PHASE_OPT_REAL;
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_OPT_REAL);
          break;
        default:
          break;
      }
    } else { // current view is one of the Fourier views
      // set the current plot view to data
      fPreviousPlotView = fCurrentPlotView;
      fCurrentPlotView = PV_DATA;
      // uncheck all fourier popup menu items
      fPopupFourier->UnCheckEntries();
      // check the data entry
      fPopupMain->CheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
    }
  } else if (x == '+') {
    if (fCurrentPlotView != PV_DATA)
      IncrementFourierPhase();
  } else if (x == '-') {
    if (fCurrentPlotView != PV_DATA)
      DecrementFourierPhase();
  } else if (x == 'a') {
    if (fData.size() > 1) {
      // toggle average view flag
      fAveragedView = !fAveragedView;
      // update menu
      if (fAveragedView) {
        fPopupMain->CheckEntry(P_MENU_ID_AVERAGE+P_MENU_PLOT_OFFSET*fPlotNumber);
        HandleAverage();
        PlotAverage(true);
      } else {
        fPopupMain->UnCheckEntry(P_MENU_ID_AVERAGE+P_MENU_PLOT_OFFSET*fPlotNumber);
        CleanupAverage();
      }
      // check which relevantKeySwitch is needed
      if ((fCurrentPlotView == PV_DATA) && fDifferenceView && !fAveragedView)
        relevantKeySwitch = kDiffData;
      else if ((fCurrentPlotView == PV_DATA) && !fDifferenceView && !fAveragedView)
        relevantKeySwitch = kData;
      else if ((fCurrentPlotView != PV_DATA) && fDifferenceView && !fAveragedView)
        relevantKeySwitch = kFourierDiff;
      else if ((fCurrentPlotView != PV_DATA) && !fDifferenceView && !fAveragedView)
        relevantKeySwitch = kFourier;
    } else { // with only 1 data set, it doesn't make any sense to average!
      std::cout << "**INFO** averaging of a single data set doesn't make any sense, will ignore 'a' ..." << std::endl;
      return;
    }
  } else if (x == 'c') {
    Int_t state = fDataTheoryPad->GetCrosshair();
    if (state == 0) {
      fMainCanvas->ToggleEventStatus();
      fDataTheoryPad->SetCrosshair(2);
    } else {
      fMainCanvas->ToggleEventStatus();
      fDataTheoryPad->SetCrosshair(0);
    }
    fMainCanvas->Update();
  } else {
    fMainCanvas->Update();
  }

  lastKeySwitch = relevantKeySwitch;

  // call the apropriate functions if necessary
  switch (relevantKeySwitch) {
    case kData: // show data
      CleanupDifference();
      CleanupFourier();
      PlotData();
      break;
    case kDiffData: // show difference between data and theory
      CleanupFourierDifference();
      HandleDifference();
      PlotDifference();
      break;
    case kFourier: // show Fourier transfrom of the data
      HandleFourier();
      PlotFourier();
      break;
    case kDiffFourier: // show Fourier transform of the difference data
      HandleDifferenceFourier();
      PlotFourierDifference();
      break;
    case kFourierDiff: // show difference between the Fourier data and the Fourier theory
      CleanupFourierDifference();
      HandleFourierDifference();
      PlotFourierDifference();
      break;
    default:
      break;
  }

  // check if phase increment/decrement needs to be ghost
  if (fCurrentPlotView == PV_DATA) {
    fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
  } else {
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
  }
}

//--------------------------------------------------------------------------
// HandleMenuPopup (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>Handles the Musrfit menu.
 *
 * \param id identification key of the selected menu
 */
void PMusrCanvas::HandleMenuPopup(Int_t id)
{
  if (fBatchMode)
    return;

  static Int_t previousPlotView = PV_DATA;

  if (id == P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber) {
    // set appropriate plot view
    fPreviousPlotView = fCurrentPlotView;
    fCurrentPlotView = PV_DATA;
    // check data item
    fPopupMain->CheckEntry(id);
    // uncheck fourier popup items
    fPopupFourier->UnCheckEntries();
    // call data handling routine
    if (!fDifferenceView) {
      CleanupDifference();
      CleanupFourier();
      PlotData();
    } else {
      HandleDifference();
      PlotDifference();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL) {
    // set appropriate plot view
    fPreviousPlotView = fCurrentPlotView;
    fCurrentPlotView = PV_FOURIER_REAL;
    // uncheck data
    fPopupMain->UnCheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
    // check appropriate fourier popup item
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(id);
    // enable phase increment/decrement
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
    // handle fourier real
    if (!fDifferenceView) {
      HandleFourier();
      PlotFourier();
    } else {
      if (previousPlotView == PV_DATA)
        HandleDifferenceFourier();
      else
        HandleFourierDifference();
      PlotFourierDifference();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_IMAG) {
    // set appropriate plot view
    fPreviousPlotView = fCurrentPlotView;
    fCurrentPlotView = PV_FOURIER_IMAG;
    // uncheck data
    fPopupMain->UnCheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
    // check appropriate fourier popup item
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(id);
    // enable phase increment/decrement
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
    // handle fourier imag
    if (!fDifferenceView) {
      HandleFourier();
      PlotFourier();
    } else {
      if (previousPlotView == PV_DATA)
        HandleDifferenceFourier();
      else
        HandleFourierDifference();
      PlotFourierDifference();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL_AND_IMAG) {
    // set appropriate plot view
    fPreviousPlotView = fCurrentPlotView;
    fCurrentPlotView = PV_FOURIER_REAL_AND_IMAG;
    // uncheck data
    fPopupMain->UnCheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
    // check appropriate fourier popup item
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(id);
    // enable phase increment/decrement
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
    // handle fourier real and imag
    if (!fDifferenceView) {
      HandleFourier();
      PlotFourier();
    } else {
      if (previousPlotView == PV_DATA)
        HandleDifferenceFourier();
      else
        HandleFourierDifference();
      PlotFourierDifference();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PWR) {
    // set appropriate plot view
    fPreviousPlotView = fCurrentPlotView;
    fCurrentPlotView = PV_FOURIER_PWR;
    // uncheck data
    fPopupMain->UnCheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
    // check appropriate fourier popup item
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(id);
    // enable phase increment/decrement
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
    // handle fourier power
    if (!fDifferenceView) {
      HandleFourier();
      PlotFourier();
    } else {
      if (previousPlotView == PV_DATA)
        HandleDifferenceFourier();
      else
        HandleFourierDifference();
      PlotFourierDifference();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE) {
    // set appropriate plot view
    fPreviousPlotView = fCurrentPlotView;
    fCurrentPlotView = PV_FOURIER_PHASE;
    // uncheck data
    fPopupMain->UnCheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
    // check appropriate fourier popup item
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(id);
    // enable phase increment/decrement
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
    // handle fourier phase
    if (!fDifferenceView) {
      HandleFourier();
      PlotFourier();
    } else {
      if (previousPlotView == PV_DATA)
        HandleDifferenceFourier();
      else
        HandleFourierDifference();
      PlotFourierDifference();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_OPT_REAL) {
    // set appropriate plot view
    fPreviousPlotView = fCurrentPlotView;
    fCurrentPlotView = PV_FOURIER_PHASE_OPT_REAL;
    // make sure that phase opt. real indeed exists
    if (fData[0].dataFourierPhaseOptReal == nullptr) {
      if (fData[0].dataFourierRe == nullptr)
        HandleFourier();
      else
        CalcPhaseOptReFT();
    }
    // uncheck data
    fPopupMain->UnCheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
    // check appropriate fourier popup item
    fPopupFourier->UnCheckEntries();
    fPopupFourier->CheckEntry(id);
    // enable phase increment/decrement
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
    // handle fourier phase
    if (!fDifferenceView) {
      HandleFourier();
      PlotFourier();
    } else {
      if (previousPlotView == PV_DATA)
        HandleDifferenceFourier();
      else
        HandleFourierDifference();
      PlotFourierDifference();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS) {
    IncrementFourierPhase();
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS) {
    DecrementFourierPhase();
  } else if (id == P_MENU_ID_DIFFERENCE+P_MENU_PLOT_OFFSET*fPlotNumber) {
    // toggle difference tag
    fDifferenceView = !fDifferenceView;
    // set the popup menu entry properly
    if (fDifferenceView) {
      fPopupMain->CheckEntry(id);
    } else {
      fPopupMain->UnCheckEntry(id);
    }
    // handle data, diff, Fourier
    if (fDifferenceView) {
      switch (fCurrentPlotView) {
        case PV_DATA:
          CleanupFourierDifference();
          HandleDifference();
          PlotDifference();
          break;
        case PV_FOURIER_REAL:
        case PV_FOURIER_IMAG:
        case PV_FOURIER_REAL_AND_IMAG:
        case PV_FOURIER_PWR:
        case PV_FOURIER_PHASE:
          if (fPopupMain->IsEntryChecked(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber)) {
            CleanupFourierDifference();
            HandleDifferenceFourier();
            PlotFourierDifference();
          } else {
            CleanupFourierDifference();
            HandleFourierDifference();
            PlotFourierDifference();
          }
          break;
        default:
          break;
      }
    } else { // not a difference view
      switch (fCurrentPlotView) {
        case PV_DATA:
          CleanupDifference();
          CleanupFourier();
          PlotData();
          break;
        case PV_FOURIER_REAL:
        case PV_FOURIER_IMAG:
        case PV_FOURIER_REAL_AND_IMAG:
        case PV_FOURIER_PWR:
        case PV_FOURIER_PHASE:
          HandleFourier();
          PlotFourier();
          break;
        default:
          break;
      }
    }
  } else if (id == P_MENU_ID_AVERAGE+P_MENU_PLOT_OFFSET*fPlotNumber) {
    if (fData.size() > 1) {
      fAveragedView = !fAveragedView;
      // set the popup menu entry properly
      if (fAveragedView) {
        fPopupMain->CheckEntry(id);
        HandleAverage();
        PlotAverage();
      } else {
        fPopupMain->UnCheckEntry(id);
        CleanupAverage();
      }
    } else {
      std::cout << "**INFO** averaging of a single data set doesn't make any sense, will ignore 'a' ..." << std::endl;
      return;
    }
  } else if (id == P_MENU_ID_EXPORT_DATA+P_MENU_PLOT_OFFSET*fPlotNumber) {
    static TString dir(".");
    TGFileInfo fi;
    fi.fFileTypes = gFiletypes;
    fi.fIniDir = StrDup(dir);
    fi.fOverwrite = true;
    new TGFileDialog(0, fImp, kFDSave, &fi);
    if (fi.fFilename && strlen(fi.fFilename)) {
      ExportData(fi.fFilename);
    }
  }

  // check if phase increment/decrement needs to be ghost
  if (fCurrentPlotView == PV_DATA) {
    fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
  } else {
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->EnableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
  }

  // keep plot view setting
  previousPlotView = fCurrentPlotView;
}

//--------------------------------------------------------------------------
// LastCanvasClosed (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>Slot called when the last canvas has been closed. Will emit Done(0) which will
 * terminate the application.
 */
void PMusrCanvas::LastCanvasClosed()
{
//  std::cerr << ">> in last canvas closed check. gROOT->GetListOfCanvases()->GetEntries()=" << gROOT->GetListOfCanvases()->GetEntries() << std::endl;
  if (gROOT->GetListOfCanvases()->IsEmpty()) {
    Done(0);
  }
}

//--------------------------------------------------------------------------
// WindowClosed (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>Slot called when the canvas is closed. Seems to be necessary on some systems.
 */
void PMusrCanvas::WindowClosed()
{
//  std::cerr << ">> fMainCanvas->GetName()=" << fMainCanvas->GetName() << std::endl;
  gROOT->GetListOfCanvases()->Remove(fMainCanvas);
  LastCanvasClosed();
}

//--------------------------------------------------------------------------
// SaveGraphicsAndQuit
//--------------------------------------------------------------------------
/**
 * <p>Will save the canvas as graphics output. Needed in the batch mode of musrview.
 *
 * \param fileName file name under which the canvas shall be saved.
 * \param graphicsFormat One of the supported graphics formats.
 */
void PMusrCanvas::SaveGraphicsAndQuit(Char_t *fileName, Char_t *graphicsFormat)
{
  std::cout << std::endl << ">> SaveGraphicsAndQuit: will dump the canvas into a graphics output file (" << graphicsFormat << ") ..."<< std::endl;

  TString str(fileName);
  Int_t idx = -1;
  Int_t size = 0;
  Char_t ext[32];

  if (str.Contains(".msr")) {
    idx = str.Index(".msr");
    size = 4;
  }
  if (str.Contains(".mlog")) {
    idx = str.Index(".mlog");
    size = 5;
  }

  if (idx == -1) {
    std::cerr << std::endl << ">> PMusrCanvas::SaveGraphicsAndQuit(): **ERROR** fileName (" << fileName << ") is invalid." << std::endl;
    return;
  }

  if (fStartWithFourier)
    sprintf(ext, "_%d_F", fPlotNumber);
  else
    sprintf(ext, "_%d", fPlotNumber);
  str.Replace(idx, size, ext, strlen(ext));
  idx += strlen(ext);
  size = strlen(ext);
  sprintf(ext, ".%s", graphicsFormat);
  str.Replace(idx, size, ext, strlen(ext));

  std::cout << std::endl << ">> SaveGraphicsAndQuit: " << str.Data() << std::endl;

  fMainCanvas->SaveAs(str.Data());

  if (fPlotNumber == static_cast<Int_t>(fMsrHandler->GetMsrPlotList()->size()) - 1)
    Done(0);
}

//--------------------------------------------------------------------------
// ExportData
//--------------------------------------------------------------------------
/**
 * <p>Saves the currently seen data (data, difference, Fourier spectra, ...) in ascii column format.
 *
 * \param fileName file name to be used to save the data.
 */
void PMusrCanvas::ExportData(const Char_t *fileName)
{
  if (fileName == nullptr) { // path file name NOT provided, generate a default path file name
    std::cerr << std::endl << ">> PMusrCanvas::ExportData(): **ERROR** NO path file name provided. Will do nothing." << std::endl;
    return;
  }

  // collect relevant data
  PMusrCanvasAsciiDump dump;
  PMusrCanvasAsciiDumpVector dumpVector;

  Int_t xminBin;
  Int_t xmaxBin;
  Double_t xmin;
  Double_t xmax;
  Double_t xval, yval;

  switch (fPlotType) {
    case MSR_PLOT_SINGLE_HISTO:
    case MSR_PLOT_SINGLE_HISTO_RRF:
    case MSR_PLOT_ASYM:
    case MSR_PLOT_BNMR:
    case MSR_PLOT_ASYM_RRF:
    case MSR_PLOT_MU_MINUS:
      if (fDifferenceView) { // difference view plot
        switch (fCurrentPlotView) {
          case PV_DATA:
            // get current x-range
            xminBin = fHistoFrame->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fHistoFrame->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fHistoFrame->GetXaxis()->GetBinCenter(xminBin);
            xmax = fHistoFrame->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            if (fAveragedView) {
              GetExportDataSet(fDataAvg.diff, xmin, xmax, dumpVector);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) { // go through all the histogramms
                GetExportDataSet(fData[i].diff, xmin, xmax, dumpVector);
              }
            }
            break;
          case PV_FOURIER_REAL:
            // get current x-range
            xminBin = fData[0].diffFourierRe->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].diffFourierRe->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].diffFourierRe->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].diffFourierRe->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            if (fAveragedView) {
              GetExportDataSet(fDataAvg.diffFourierRe, xmin, xmax, dumpVector, false);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) { // go through all the histogramms
                GetExportDataSet(fData[i].diffFourierRe, xmin, xmax, dumpVector, false);
              }
            }
            break;
          case PV_FOURIER_IMAG:
            // get current x-range
            xminBin = fData[0].diffFourierIm->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].diffFourierIm->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].diffFourierIm->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].diffFourierIm->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            if (fAveragedView) {
              GetExportDataSet(fDataAvg.diffFourierIm, xmin, xmax, dumpVector, false);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) { // go through all the histogramms
                GetExportDataSet(fData[i].diffFourierIm, xmin, xmax, dumpVector, false);
              }
            }
            break;
          case PV_FOURIER_REAL_AND_IMAG:
            // get current x-range
            xminBin = fData[0].diffFourierRe->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].diffFourierRe->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].diffFourierRe->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].diffFourierRe->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            if (fAveragedView) {
              GetExportDataSet(fDataAvg.diffFourierRe, xmin, xmax, dumpVector, false);
              GetExportDataSet(fDataAvg.diffFourierIm, xmin, xmax, dumpVector, false);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) { // go through all the histogramms
                GetExportDataSet(fData[i].diffFourierRe, xmin, xmax, dumpVector, false);
                GetExportDataSet(fData[i].diffFourierIm, xmin, xmax, dumpVector, false);
              }
            }
            break;
          case PV_FOURIER_PWR:
            // get current x-range
            xminBin = fData[0].diffFourierPwr->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].diffFourierPwr->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].diffFourierPwr->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].diffFourierPwr->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            if (fAveragedView) {
              GetExportDataSet(fDataAvg.diffFourierPwr, xmin, xmax, dumpVector, false);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) {
                GetExportDataSet(fData[i].diffFourierPwr, xmin, xmax, dumpVector, false);
              }
            }
            break;
          case PV_FOURIER_PHASE:
            // get current x-range
            xminBin = fData[0].diffFourierPhase->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].diffFourierPhase->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].diffFourierPhase->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].diffFourierPhase->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            if (fAveragedView) {
              GetExportDataSet(fDataAvg.diffFourierPhase, xmin, xmax, dumpVector, false);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) {
                GetExportDataSet(fData[i].diffFourierPhase, xmin, xmax, dumpVector, false);
              }
            }
            break;
          default:
            break;
        }
      } else { // not a difference view plot
        switch (fCurrentPlotView) {
          case PV_DATA:
            // get current x-range
            xminBin = fHistoFrame->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fHistoFrame->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fHistoFrame->GetXaxis()->GetBinCenter(xminBin);
            xmax = fHistoFrame->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            if (fAveragedView) {
              GetExportDataSet(fDataAvg.data, xmin, xmax, dumpVector);
              GetExportDataSet(fDataAvg.theory, xmin, xmax, dumpVector, false);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) {
                GetExportDataSet(fData[i].data, xmin, xmax, dumpVector);
                GetExportDataSet(fData[i].theory, xmin, xmax, dumpVector, false);
              }
            }

            break;
          case PV_FOURIER_REAL:
            // get current x-range
            xminBin = fData[0].dataFourierRe->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierRe->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierRe->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierRe->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            if (fAveragedView) {
              GetExportDataSet(fDataAvg.dataFourierRe, xmin, xmax, dumpVector, false);
              GetExportDataSet(fDataAvg.theoryFourierRe, xmin, xmax, dumpVector, false);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) {
                GetExportDataSet(fData[i].dataFourierRe, xmin, xmax, dumpVector, false);
                GetExportDataSet(fData[i].theoryFourierRe, xmin, xmax, dumpVector, false);
              }
            }
            break;
          case PV_FOURIER_IMAG:
            // get current x-range
            xminBin = fData[0].dataFourierIm->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierIm->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierIm->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierIm->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            if (fAveragedView) {
              GetExportDataSet(fDataAvg.dataFourierIm, xmin, xmax, dumpVector, false);
              GetExportDataSet(fDataAvg.theoryFourierIm, xmin, xmax, dumpVector, false);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) {
                GetExportDataSet(fData[i].dataFourierIm, xmin, xmax, dumpVector, false);
                GetExportDataSet(fData[i].theoryFourierIm, xmin, xmax, dumpVector, false);
              }
            }
            break;
          case PV_FOURIER_REAL_AND_IMAG:
            // get current x-range
            xminBin = fData[0].dataFourierRe->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierRe->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierRe->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierRe->GetXaxis()->GetBinCenter(xmaxBin);

            if (fAveragedView) {
              GetExportDataSet(fDataAvg.dataFourierRe, xmin, xmax, dumpVector, false);
              GetExportDataSet(fDataAvg.theoryFourierRe, xmin, xmax, dumpVector, false);
              GetExportDataSet(fDataAvg.dataFourierIm, xmin, xmax, dumpVector, false);
              GetExportDataSet(fDataAvg.theoryFourierIm, xmin, xmax, dumpVector, false);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) {
                GetExportDataSet(fData[i].dataFourierRe, xmin, xmax, dumpVector, false);
                GetExportDataSet(fData[i].theoryFourierRe, xmin, xmax, dumpVector, false);
                GetExportDataSet(fData[i].dataFourierIm, xmin, xmax, dumpVector, false);
                GetExportDataSet(fData[i].theoryFourierIm, xmin, xmax, dumpVector, false);
              }
            }
            break;
          case PV_FOURIER_PWR:
            // get current x-range
            xminBin = fData[0].dataFourierPwr->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierPwr->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierPwr->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierPwr->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            if (fAveragedView) {
              GetExportDataSet(fDataAvg.dataFourierPwr, xmin, xmax, dumpVector, false);
              GetExportDataSet(fDataAvg.theoryFourierPwr, xmin, xmax, dumpVector, false);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) {
                GetExportDataSet(fData[i].dataFourierPwr, xmin, xmax, dumpVector, false);
                GetExportDataSet(fData[i].theoryFourierPwr, xmin, xmax, dumpVector, false);
              }
            }
            break;
          case PV_FOURIER_PHASE:
            // get current x-range
            xminBin = fData[0].dataFourierPhase->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierPhase->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierPhase->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierPhase->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            if (fAveragedView) {
              GetExportDataSet(fDataAvg.dataFourierPhase, xmin, xmax, dumpVector, false);
              GetExportDataSet(fDataAvg.theoryFourierPhase, xmin, xmax, dumpVector, false);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) {
                GetExportDataSet(fData[i].dataFourierPhase, xmin, xmax, dumpVector, false);
                GetExportDataSet(fData[i].theoryFourierPhase, xmin, xmax, dumpVector, false);
              }
            }
            break;
          case PV_FOURIER_PHASE_OPT_REAL:
            // get current x-range
            xminBin = fData[0].dataFourierPhaseOptReal->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierPhaseOptReal->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierPhaseOptReal->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierPhaseOptReal->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            if (fAveragedView) {
              GetExportDataSet(fDataAvg.dataFourierPhaseOptReal, xmin, xmax, dumpVector, false);
              GetExportDataSet(fDataAvg.theoryFourierPhaseOptReal, xmin, xmax, dumpVector, false);
            } else { // go through all the histogramms
              for (UInt_t i=0; i<fData.size(); i++) {
                GetExportDataSet(fData[i].dataFourierPhaseOptReal, xmin, xmax, dumpVector, false);
                GetExportDataSet(fData[i].theoryFourierPhaseOptReal, xmin, xmax, dumpVector, false);
              }
            }
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
            // get current x-range
            xminBin = fMultiGraphData->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fMultiGraphData->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fMultiGraphData->GetXaxis()->GetBinCenter(xminBin);
            xmax = fMultiGraphData->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            for (UInt_t i=0; i<fNonMusrData.size(); i++) { // go through all the histogramms
              // clean up dump
              dump.dataX.clear();
              dump.data.clear();
              dump.dataErr.clear();

              // go through all data bins
              for (Int_t j=0; j<fNonMusrData[i].diff->GetN(); j++) {
                // get x and y value
                fNonMusrData[i].diff->GetPoint(j,xval,yval);
                // check if time is in the current range
                if ((xval >= xmin) && (xval <= xmax)) {
                  dump.dataX.push_back(xval);
                  dump.data.push_back(yval);
                  dump.dataErr.push_back(fNonMusrData[i].diff->GetErrorY(j));
                }
              }

              // if anything found keep it
              if (dump.dataX.size() > 0)
                dumpVector.push_back(dump);
            }

            break;
          case PV_FOURIER_REAL:
            break;
          case PV_FOURIER_IMAG:
            break;
          case PV_FOURIER_REAL_AND_IMAG:
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
            // get current x-range
            xminBin = fMultiGraphData->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fMultiGraphData->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fMultiGraphData->GetXaxis()->GetBinCenter(xminBin);
            xmax = fMultiGraphData->GetXaxis()->GetBinCenter(xmaxBin);

            // fill ascii dump data
            for (UInt_t i=0; i<fNonMusrData.size(); i++) { // go through all the histogramms
              // clean up dump
              dump.dataX.clear();
              dump.data.clear();
              dump.dataErr.clear();

              // go through all data bins
              for (Int_t j=0; j<fNonMusrData[i].data->GetN(); j++) {
                // get x and y value
                fNonMusrData[i].data->GetPoint(j,xval,yval);
                // check if time is in the current range
                if ((xval >= xmin) && (xval <= xmax)) {
                  dump.dataX.push_back(xval);
                  dump.data.push_back(yval);
                  dump.dataErr.push_back(fNonMusrData[i].data->GetErrorY(j));
                }
              }

              // go through all theory bins
              for (Int_t j=0; j<fNonMusrData[i].theory->GetN(); j++) {
                // get x and y value
                fNonMusrData[i].theory->GetPoint(j,xval,yval);
                // check if time is in the current range
                if ((xval >= xmin) && (xval <= xmax)) {
                  dump.dataX.push_back(xval);
                  dump.data.push_back(yval);
                }
              }

              // if anything found keep it
              if (dump.dataX.size() > 0)
                dumpVector.push_back(dump);
            }

            break;
          case PV_FOURIER_REAL:
            break;
          case PV_FOURIER_IMAG:
            break;
          case PV_FOURIER_REAL_AND_IMAG:
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

  // open file
  std::ofstream fout;

  // open output data-file
  fout.open(fileName, std::iostream::out);
  if (!fout.is_open()) {
    std::cerr << std::endl << ">> PMusrCanvas::ExportData(): **ERROR** couldn't open file " << fileName << " for writing." << std::endl;
    return;
  }

  // find out what is the longest data/theory vector
  UInt_t maxLength = 0;
  for (UInt_t i=0; i<dumpVector.size(); i++) {
    if (maxLength < dumpVector[i].dataX.size())
      maxLength = dumpVector[i].dataX.size();
  }

  // write data to file
  if (fDifferenceView) { // difference view
    // write header
    switch (fCurrentPlotView) {
      case PV_DATA:
        if (fAveragedView) {
          fout << "% from averaged view" << std::endl;
          fout << "% x, diff, errDiff" << std::endl;
        } else {
          fout << "% ";
          for (UInt_t i=0; i<dumpVector.size()-1; i++) {
            fout << "x" << i << " , diff" << i << ", errDiff" << i << ", ";
          }
          fout << "x" << dumpVector.size()-1 << " , diff" << dumpVector.size()-1 << ", errDiff" << dumpVector.size()-1 << std::endl;
        }
        break;
      case PV_FOURIER_REAL:
        if (fAveragedView) {
          fout << "% from averaged view" << std::endl;
          fout << "% x, F_diffRe" << std::endl;
        } else {
          fout << "% ";
          for (UInt_t i=0; i<dumpVector.size()-1; i++) {
            fout << "freq" << i << ", F_diffRe" << i << ", ";
          }
          fout << "freq" << dumpVector.size()-1 << ", F_diffRe" << dumpVector.size()-1 << std::endl;
        }
        break;
      case PV_FOURIER_IMAG:
        if (fAveragedView) {
          fout << "% from averaged view" << std::endl;
          fout << "% x, F_diffIm" << std::endl;
        } else {
          fout << "% ";
          for (UInt_t i=0; i<dumpVector.size()-1; i++) {
            fout << "freq" << i << ", F_diffIm" << i << ", ";
          }
          fout << "freq" << dumpVector.size()-1 << ", F_diffIm" << dumpVector.size()-1 << std::endl;
        }
        break;
      case PV_FOURIER_REAL_AND_IMAG:
        if (fAveragedView) {
          fout << "% from averaged view" << std::endl;
          fout << "% x, F_diffRe, F_diffIm" << std::endl;
        } else {
          fout << "% ";
          for (UInt_t i=0; i<dumpVector.size()/2; i++) {
            fout << "freq" << i << ", F_diffRe" << i << ", ";
          }
          for (UInt_t i=0; i<dumpVector.size()/2-1; i++) {
            fout << "freq" << i << ", F_diffIm" << i << ", ";
          }
          fout << "freq" << dumpVector.size()/2-1 << ", F_diffIm" << dumpVector.size()/2-1 << std::endl;
        }
        break;
      case PV_FOURIER_PWR:
        if (fAveragedView) {
          fout << "% from averaged view" << std::endl;
          fout << "% x, F_diffPwr" << std::endl;
        } else {
          fout << "% ";
          for (UInt_t i=0; i<dumpVector.size()-1; i++) {
            fout << "freq" << i << ", F_diffPwr" << i << ", ";
          }
          fout << "freq" << dumpVector.size()-1 << ", F_diffPwr" << dumpVector.size()-1 << std::endl;
        }
        break;
      case PV_FOURIER_PHASE:
        if (fAveragedView) {
          fout << "% from averaged view" << std::endl;
          fout << "% x, F_diffPhase" << std::endl;
        } else {
          fout << "% ";
          for (UInt_t i=0; i<dumpVector.size()-1; i++) {
            fout << "freq" << i << ", F_diffPhase" << i << ", ";
          }
          fout << "freq" << dumpVector.size()-1 << ", F_diffPhase" << dumpVector.size()-1 << std::endl;
        }
        break;
      default:
        break;
    }

    // write difference data
    for (UInt_t i=0; i<maxLength; i++) {
      // write difference data
      for (UInt_t j=0; j<dumpVector.size()-1; j++) {
        if (i<dumpVector[j].dataX.size()) {
          fout << dumpVector[j].dataX[i] << ", ";
          fout << dumpVector[j].data[i] << ", ";
          if (dumpVector[j].dataErr.size() > 0)
            fout << dumpVector[j].dataErr[i] << ", ";
        } else {
          if (dumpVector[j].dataErr.size() > 0)
            fout << ", , , ";
          else
            fout << ", , ";
        }
      }
      // write last difference entry
      if (i<dumpVector[dumpVector.size()-1].dataX.size()) {
        fout << dumpVector[dumpVector.size()-1].dataX[i] << ", ";
        fout << dumpVector[dumpVector.size()-1].data[i] << ", ";
        if (dumpVector[dumpVector.size()-1].dataErr.size() > 0)
          fout << dumpVector[dumpVector.size()-1].dataErr[i];
      } else {
        if (dumpVector[dumpVector.size()-1].dataErr.size() > 0)
          fout << ", , ";
        else
          fout << ", ";
      }
      fout << std::endl;
    }
  } else { // no difference view
    // write header
    switch (fCurrentPlotView) {
      case PV_DATA:
        if (fAveragedView) {
          fout << "% from averaged view" << std::endl;
          fout << "% xData, data, errData, xTheory, theory" << std::endl;
        } else {
          fout << "% ";
          for (UInt_t i=0; i<dumpVector.size(); i++) {
            if (i % 2 == 0)
              fout << "xData" << i/2 << " , data" << i/2 << ", errData" << i/2 << ", ";
            else
              if (i == dumpVector.size()-1)
                fout << "xTheory" << (i-1)/2 << " , theory" << (i-1)/2 << std::endl;
              else
                fout << "xTheory" << (i-1)/2 << " , theory" << (i-1)/2 << ", ";
          }
        }
        break;
      case PV_FOURIER_REAL:
        if (fAveragedView) {
          fout << "% from averaged view" << std::endl;
          fout << "% freq, F_Re, freqTheo, F_theoRe" << std::endl;
        } else {
          fout << "% ";
          for (UInt_t i=0; i<dumpVector.size(); i++) {
            if (i % 2 == 0)
              fout << "freq" << i/2 << ", F_Re" << i/2 << ", ";
            else
              if (i == dumpVector.size()-1)
                fout << "freqTheo" << (i-1)/2 << ", F_theoRe" << (i-1)/2 << std::endl;
              else
                fout << "freqTheo" << (i-1)/2 << ", F_theoRe" << (i-1)/2 << ", ";
          }
        }
        break;
      case PV_FOURIER_IMAG:
        if (fAveragedView) {
          fout << "% from averaged view" << std::endl;
          fout << "% freq, F_Im, freqTheo, F_theoIm" << std::endl;
        } else {
          fout << "% ";
          for (UInt_t i=0; i<dumpVector.size(); i++) {
            if (i % 2 == 0)
              fout << "freq" << i/2 << ", F_Im" << i/2 << ", ";
            else
              if (i == dumpVector.size()-1)
                fout << "freqTheo" << (i-1)/2 << ", F_theoIm" << (i-1)/2 << std::endl;
              else
                fout << "freqTheo" << (i-1)/2 << ", F_theoIm" << (i-1)/2 << ", ";
          }
        }
        break;
      case PV_FOURIER_REAL_AND_IMAG:
        if (fAveragedView) {
          fout << "% from averaged view" << std::endl;
          fout << "% freq, F_Re, freqTheo, F_theoRe, freq, F_Im, freqTheo, F_theoIm" << std::endl;
        } else {
          fout << "% ";
          for (UInt_t i=0; i<dumpVector.size(); i++) {
            if (i % 4 == 0)
              fout << "freq" << i/4 << ", F_Re" << i/4 << ", ";
            else if (i % 4 == 1)
              fout << "freqTheo" << (i-1)/4 << ", F_theoRe" << (i-1)/4 << ", ";
            else if (i % 4 == 2)
              fout << "freq" << (i-2)/4 << ", F_Im" << (i-2)/4 << ", ";
            else
              if (i == dumpVector.size()-1)
                fout << "freqTheo" << (i-3)/4 << ", F_theoIm" << (i-3)/4 << std::endl;
              else
                fout << "freqTheo" << (i-3)/4 << ", F_theoIm" << (i-3)/4 << ", ";
          }
        }
        break;
      case PV_FOURIER_PWR:
        if (fAveragedView) {
          fout << "% from averaged view" << std::endl;
          fout << "% freq, F_Pwr, freqTheo, F_theoPwr" << std::endl;
        } else {
          fout << "% ";
          for (UInt_t i=0; i<dumpVector.size(); i++) {
            if (i % 2 == 0)
              fout << "freq" << i/2 << ", F_Pwr" << i/2 << ", ";
            else
              if (i == dumpVector.size()-1)
                fout << "freqTheo" << (i-1)/2 << ", F_theoPwr" << (i-1)/2 << std::endl;
              else
                fout << "freqTheo" << (i-1)/2 << ", F_theoPwr" << (i-1)/2 << ", ";
          }
        }
        break;
      case PV_FOURIER_PHASE:
        if (fAveragedView) {
          fout << "% from averaged view" << std::endl;
          fout << "% freq, F_Phase, freqTheo, F_theoPhase" << std::endl;
        } else {
          fout << "% ";
          for (UInt_t i=0; i<dumpVector.size(); i++) {
            if (i % 2 == 0)
              fout << "freq" << i/2 << ", F_Phase" << i/2 << ", ";
            else
              if (i == dumpVector.size()-1)
                fout << "freqTheo" << (i-1)/2 << ", F_theoPhase" << (i-1)/2 << std::endl;
              else
                fout << "freqTheo" << (i-1)/2 << ", F_theoPhase" << (i-1)/2 << ", ";
          }
        }
        break;
      default:
        break;
    }

    // write data and theory
    for (UInt_t i=0; i<maxLength; i++) {
      // write data/theory
      for (UInt_t j=0; j<dumpVector.size()-1; j++) {
        if (i<dumpVector[j].dataX.size()) {
          fout << std::setprecision(9) << dumpVector[j].dataX[i] << ", ";
          fout << std::setprecision(9) << dumpVector[j].data[i] << ", ";
          if (dumpVector[j].dataErr.size() > 0)
            fout << std::setprecision(9) << dumpVector[j].dataErr[i] << ", ";
        } else {
          if (dumpVector[j].dataErr.size() > 0)
            fout << " , , , ";
          else
            fout << " , , ";
        }
      }
      // write last data/theory entry
      if (i<dumpVector[dumpVector.size()-1].dataX.size()) {
        fout << std::setprecision(9) << dumpVector[dumpVector.size()-1].dataX[i] << ", ";
        fout << std::setprecision(9) << dumpVector[dumpVector.size()-1].data[i];
      } else {
        fout << " , ";
      }
      fout << std::endl;
    }
  }

  // close file
  fout.close();

  // clean up
  for (UInt_t i=0; i<dumpVector.size(); i++) {
    dumpVector[i].dataX.clear();
    dumpVector[i].data.clear();
    dumpVector[i].dataErr.clear();
  }
  dumpVector.clear();

  std::cout << std::endl << ">> Data windows saved in ascii format ..." << std::endl;
  //  if (asciiOutput) {
  //    if (fPlotNumber == static_cast<Int_t>(fMsrHandler->GetMsrPlotList()->size()) - 1)
  //      Done(0);
  //  }
}

//--------------------------------------------------------------------------
// GetExportDataSet (private)
//--------------------------------------------------------------------------
/**
 * <p> extract data for export.
 *
 * \param data
 * \param xmin
 * \param xmax
 * \param dumpData
 * \param hasError
 */
void PMusrCanvas::GetExportDataSet(const TH1F *data, const Double_t xmin, const Double_t xmax,
                                   PMusrCanvasAsciiDumpVector &dumpData, const Bool_t hasError)
{
  PMusrCanvasAsciiDump dump;
  Double_t x=0.0;

  // go through all difference data bins
  for (Int_t j=1; j<data->GetNbinsX(); j++) {
    // get time/freq
    x = data->GetBinCenter(j);
    // check if x is in the current range
    if ((x >= xmin) && (x <= xmax)) {
      dump.dataX.push_back(x);
      dump.data.push_back(data->GetBinContent(j));
      if (hasError)
        dump.dataErr.push_back(data->GetBinError(j));
    }
  }

  // if anything found keep it
  if (dump.dataX.size() > 0)
    dumpData.push_back(dump);
}

//--------------------------------------------------------------------------
// CreateStyle (private)
//--------------------------------------------------------------------------
/**
 * <p> Set styles for the canvas. Perhaps one could transfer them to the startup-file in the future.
 */
void PMusrCanvas::CreateStyle()
{
  TString musrStyle("musrStyle");
  musrStyle += fPlotNumber;
  fStyle = new TStyle(musrStyle, musrStyle);
  fStyle->SetOptStat(0);  // no statistics options
  fStyle->SetOptTitle(0); // no title
  fStyle->cd();
}

//--------------------------------------------------------------------------
// InitFourier (private)
//--------------------------------------------------------------------------
/**
 * <p>Initializes the Fourier structure.
 */
void PMusrCanvas::InitFourier()
{
  fFourier.fFourierBlockPresent = false;           // fourier block present
  fFourier.fUnits = FOURIER_UNIT_GAUSS;            // fourier untis
  fFourier.fFourierPower = 0;                      // no zero padding
  fFourier.fApodization = FOURIER_APOD_NONE;       // no apodization
  fFourier.fPlotTag = FOURIER_PLOT_REAL_AND_IMAG;  // initial plot tag, plot real and imaginary part
  fFourier.fPhaseParamNo.clear();
  fFourier.fPhase.clear();
  for (UInt_t i=0; i<2; i++) {
    fFourier.fRangeForPhaseCorrection[i] = -1.0;   // frequency range for phase correction, default: {-1, -1} = NOT GIVEN
    fFourier.fPlotRange[i] = -1.0;                 // fourier plot range, default: {-1, -1} = NOT GIVEN
  }
  fFourier.fPhaseIncrement = 1.0;                  // fourier phase increment
}

//--------------------------------------------------------------------------
// InitAverage (private)
//--------------------------------------------------------------------------
/**
 * <p>Initializes the Average structure.
 */
void PMusrCanvas::InitAverage()
{
  fDataAvg.data = nullptr;
  fDataAvg.dataFourierRe = nullptr;
  fDataAvg.dataFourierIm = nullptr;
  fDataAvg.dataFourierPwr = nullptr;
  fDataAvg.dataFourierPhase = nullptr;
  fDataAvg.dataFourierPhaseOptReal = nullptr;
  fDataAvg.theory = nullptr;
  fDataAvg.theoryFourierRe = nullptr;
  fDataAvg.theoryFourierIm = nullptr;
  fDataAvg.theoryFourierPwr = nullptr;
  fDataAvg.theoryFourierPhase = nullptr;
  fDataAvg.theoryFourierPhaseOptReal = nullptr;
  fDataAvg.diff = nullptr;
  fDataAvg.diffFourierRe = nullptr;
  fDataAvg.diffFourierIm = nullptr;
  fDataAvg.diffFourierPwr = nullptr;
  fDataAvg.diffFourierPhase = nullptr;
  fDataAvg.diffFourierPhaseOptReal = nullptr;
  fDataAvg.dataRange = nullptr;
  fDataAvg.diffFourierTag = 0;
}

//--------------------------------------------------------------------------
// InitMusrCanvas (private)
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
void PMusrCanvas::InitMusrCanvas(const Char_t* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh)
{
  fScaleN0AndBkg = true;
  fValid = false;
  fAveragedView     = false;
  fDifferenceView   = false;
  fCurrentPlotView  = PV_DATA;
  fPreviousPlotView = PV_DATA;
  fPlotType = -1;

  fImp   = nullptr;
  fBar   = nullptr;
  fPopupMain    = nullptr;
  fPopupFourier = nullptr;

  fMainCanvas          = nullptr;
  fTitlePad            = nullptr;
  fDataTheoryPad       = nullptr;
  fParameterPad        = nullptr;
  fTheoryPad           = nullptr;
  fInfoPad             = nullptr;
  fMultiGraphLegend    = nullptr;

  // invoke canvas
  TString canvasName = TString("fMainCanvas");
  canvasName += fPlotNumber;
  fMainCanvas = new TCanvas(canvasName.Data(), title, wtopx, wtopy, ww, wh);
  if (fMainCanvas == nullptr) {
    std::cerr << std::endl << ">> PMusrCanvas::PMusrCanvas(): **PANIC ERROR** Couldn't invoke " << canvasName.Data();
    std::cerr << std::endl;
    return;
  }

  fMainCanvas->Connect("Closed()", "PMusrCanvas", this, "LastCanvasClosed()");

  // add canvas menu if not in batch mode
  if (!fBatchMode) {
    fImp = (TRootCanvas*)fMainCanvas->GetCanvasImp();
    fImp->Connect("CloseWindow()", "PMusrCanvas", this, "WindowClosed()");
    fBar = fImp->GetMenuBar();
    fPopupMain = fBar->AddPopup("&Musrfit");

    fPopupFourier = new TGPopupMenu();
    fPopupMain->AddEntry("&Data", P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
    fPopupMain->AddSeparator();

    fPopupMain->AddPopup("&Fourier", fPopupFourier);
    fPopupFourier->AddEntry("Show Real", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL);
    fPopupFourier->AddEntry("Show Imag", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_IMAG);
    fPopupFourier->AddEntry("Show Real+Imag", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL_AND_IMAG);
    fPopupFourier->AddEntry("Show Power", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PWR);
    fPopupFourier->AddEntry("Show Phase", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE);
    fPopupFourier->AddEntry("Show PhaseOptReal", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_OPT_REAL);
    fPopupFourier->AddSeparator();
    fPopupFourier->AddEntry("Phase +", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->AddEntry("Phase -", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
    fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
    fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);

    fPopupMain->AddEntry("D&ifference", P_MENU_ID_DIFFERENCE+P_MENU_PLOT_OFFSET*fPlotNumber);
    fPopupMain->AddSeparator();

    fPopupMain->AddEntry("Average", P_MENU_ID_AVERAGE+P_MENU_PLOT_OFFSET*fPlotNumber);
    fPopupMain->AddSeparator();

    fPopupMain->AddEntry("Export Data", P_MENU_ID_EXPORT_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
    fBar->MapSubwindows();
    fBar->Layout();

    fPopupMain->Connect("TGPopupMenu", "Activated(Int_t)", "PMusrCanvas", this, "HandleMenuPopup(Int_t)");

    fPopupMain->CheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
  }

  // divide the canvas into 4 pads
  // title pad
  fTitlePad = new TPaveText(0.0, YTITLE, 1.0, 1.0, "NDC");
  if (fTitlePad == nullptr) {
    std::cerr << std::endl << ">> PMusrCanvas::PMusrCanvas(): **PANIC ERROR** Couldn't invoke fTitlePad";
    std::cerr << std::endl;
    return;
  }
  fTitlePad->SetFillColor(TColor::GetColor(255,255,255));
  fTitlePad->SetTextAlign(12); // middle, left
  fTitlePad->AddText(title);
  fTitlePad->Draw();

  // data/theory pad
  fDataTheoryPad = new TPad("dataTheoryPad", "dataTheoryPad", 0.0, YINFO, XTHEO, YTITLE);
  if (fDataTheoryPad == nullptr) {
    std::cerr << std::endl << ">> PMusrCanvas::PMusrCanvas(): **PANIC ERROR** Couldn't invoke fDataTheoryPad";
    std::cerr << std::endl;
    return;
  }
  fDataTheoryPad->SetFillColor(TColor::GetColor(255,255,255));
  fDataTheoryPad->Draw();

  // parameter pad
  fParameterPad = new TPaveText(XTHEO, 0.5, 1.0, YTITLE, "NDC");
  if (fParameterPad == nullptr) {
    std::cerr << std::endl << ">> PMusrCanvas::PMusrCanvas(): **PANIC ERROR** Couldn't invoke fParameterPad";
    std::cerr << std::endl;
    return;
  }
  fParameterPad->SetFillColor(TColor::GetColor(255,255,255));
  fParameterPad->SetTextAlign(13); // top, left
  fParameterPad->SetTextFont(102); // courier bold, scalable so that greek parameters will be plotted properly

  // theory pad
  fTheoryPad = new TPaveText(XTHEO, 0.1, 1.0, 0.5, "NDC");
  if (fTheoryPad == nullptr) {
    std::cerr << std::endl << ">> PMusrCanvas::PMusrCanvas(): **PANIC ERROR** Couldn't invoke fTheoryPad";
    std::cerr << std::endl;
    return;
  }
  fTheoryPad->SetFillColor(TColor::GetColor(255,255,255));
  fTheoryPad->SetTextAlign(13); // top, left
  fTheoryPad->SetTextFont(102); // courier bold, scalable so that greek parameters will be plotted properly


  // info pad
  fInfoPad = new TLegend(0.0, 0.0, 1.0, YINFO, "NDC");
  if (fInfoPad == nullptr) {
    std::cerr << std::endl << ">> PMusrCanvas::PMusrCanvas(): **PANIC ERROR** Couldn't invoke fInfoPad";
    std::cerr << std::endl;
    return;
  }
  fInfoPad->SetFillColor(TColor::GetColor(255,255,255));
  fInfoPad->SetTextAlign(12); // middle, left

  fValid = true;

  fMainCanvas->cd();

  fMainCanvas->Show();

  fMainCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "PMusrCanvas", 
                       this, "HandleCmdKey(Int_t,Int_t,Int_t,TObject*)");
}

//--------------------------------------------------------------------------
// InitDataSet (private)
//--------------------------------------------------------------------------
/**
 * <p>Initializes the data set (histogram representation).
 *
 * \param dataSet data set to be initialized
 */
void PMusrCanvas::InitDataSet(PMusrCanvasDataSet &dataSet)
{
  dataSet.data = nullptr;
  dataSet.dataFourierRe = nullptr;
  dataSet.dataFourierIm = nullptr;
  dataSet.dataFourierPwr = nullptr;
  dataSet.dataFourierPhase = nullptr;
  dataSet.dataFourierPhaseOptReal = nullptr;
  dataSet.theory = nullptr;
  dataSet.theoryFourierRe = nullptr;
  dataSet.theoryFourierIm = nullptr;
  dataSet.theoryFourierPwr = nullptr;
  dataSet.theoryFourierPhase = nullptr;
  dataSet.theoryFourierPhaseOptReal = nullptr;
  dataSet.diff = nullptr;
  dataSet.diffFourierRe = nullptr;
  dataSet.diffFourierIm = nullptr;
  dataSet.diffFourierPwr = nullptr;
  dataSet.diffFourierPhase = nullptr;
  dataSet.diffFourierPhaseOptReal = nullptr;
  dataSet.dataRange = nullptr;
}

//--------------------------------------------------------------------------
// InitDataSet (private)
//--------------------------------------------------------------------------
/**
 * <p>Initializes the data set (error graph representation).
 *
 * \param dataSet data set to be initialized
 */
void PMusrCanvas::InitDataSet(PMusrCanvasNonMusrDataSet &dataSet)
{
  dataSet.data = nullptr;
  dataSet.dataFourierRe = nullptr;
  dataSet.dataFourierIm = nullptr;
  dataSet.dataFourierPwr = nullptr;
  dataSet.dataFourierPhase = nullptr;
  dataSet.theory = nullptr;
  dataSet.theoryFourierRe = nullptr;
  dataSet.theoryFourierIm = nullptr;
  dataSet.theoryFourierPwr = nullptr;
  dataSet.theoryFourierPhase = nullptr;
  dataSet.diff = nullptr;
  dataSet.diffFourierRe = nullptr;
  dataSet.diffFourierIm = nullptr;
  dataSet.diffFourierPwr = nullptr;
  dataSet.diffFourierPhase = nullptr;
  dataSet.dataRange = nullptr;
}

//--------------------------------------------------------------------------
// CleanupDataSet (private)
//--------------------------------------------------------------------------
/**
 * <p>Cleans up a data set (histogram representation).
 *
 * \param dataSet data set to be cleaned up.
 */
void PMusrCanvas::CleanupDataSet(PMusrCanvasDataSet &dataSet)
{
  if (dataSet.data) {
    delete dataSet.data;
    dataSet.data = nullptr;
  }
  if (dataSet.dataFourierRe) {
    delete dataSet.dataFourierRe;
    dataSet.dataFourierRe = nullptr;
  }
  if (dataSet.dataFourierIm) {
    delete dataSet.dataFourierIm;
    dataSet.dataFourierIm = nullptr;
  }
  if (dataSet.dataFourierPwr) {
    delete dataSet.dataFourierPwr;
    dataSet.dataFourierPwr = nullptr;
  }
  if (dataSet.dataFourierPhase) {
    delete dataSet.dataFourierPhase;
    dataSet.dataFourierPhase = nullptr;
  }
  if (dataSet.dataFourierPhaseOptReal) {
    delete dataSet.dataFourierPhaseOptReal;
    dataSet.dataFourierPhaseOptReal = nullptr;
  }
  if (dataSet.theory) {
    delete dataSet.theory;
    dataSet.theory = nullptr;
  }
  if (dataSet.theoryFourierRe) {
    delete dataSet.theoryFourierRe;
    dataSet.theoryFourierRe = nullptr;
  }
  if (dataSet.theoryFourierIm) {
    delete dataSet.theoryFourierIm;
    dataSet.theoryFourierIm = nullptr;
  }
  if (dataSet.theoryFourierPwr) {
    delete dataSet.theoryFourierPwr;
    dataSet.theoryFourierPwr = nullptr;
  }
  if (dataSet.theoryFourierPhase) {
    delete dataSet.theoryFourierPhase;
    dataSet.theoryFourierPhase = nullptr;
  }
  if (dataSet.theoryFourierPhaseOptReal) {
    delete dataSet.theoryFourierPhaseOptReal;
    dataSet.theoryFourierPhaseOptReal = nullptr;
  }
  if (dataSet.diff) {
    delete dataSet.diff;
    dataSet.diff = nullptr;
  }
  if (dataSet.diffFourierRe) {
    delete dataSet.diffFourierRe;
    dataSet.diffFourierRe = nullptr;
  }
  if (dataSet.diffFourierIm) {
    delete dataSet.diffFourierIm;
    dataSet.diffFourierIm = nullptr;
  }
  if (dataSet.diffFourierPwr) {
    delete dataSet.diffFourierPwr;
    dataSet.diffFourierPwr = nullptr;
  }
  if (dataSet.diffFourierPhase) {
    delete dataSet.diffFourierPhase;
    dataSet.diffFourierPhase = nullptr;
  }
  if (dataSet.diffFourierPhaseOptReal) {
    delete dataSet.diffFourierPhaseOptReal;
    dataSet.diffFourierPhaseOptReal = nullptr;
  }
  if (dataSet.dataRange) {
    delete dataSet.dataRange;
    dataSet.dataRange = nullptr;
  }
}

//--------------------------------------------------------------------------
// CleanupDataSet (private)
//--------------------------------------------------------------------------
/**
 * <p>Cleans up a data set (error graph representation).
 *
 * \param dataSet data set to be cleaned up.
 */
void PMusrCanvas::CleanupDataSet(PMusrCanvasNonMusrDataSet &dataSet)
{
  if (dataSet.data) {
    delete dataSet.data;
    dataSet.data = nullptr;
  }
  if (dataSet.dataFourierRe) {
    delete dataSet.dataFourierRe;
    dataSet.dataFourierRe = nullptr;
  }
  if (dataSet.dataFourierIm) {
    delete dataSet.dataFourierIm;
    dataSet.dataFourierIm = nullptr;
  }
  if (dataSet.dataFourierPwr) {
    delete dataSet.dataFourierPwr;
    dataSet.dataFourierPwr = nullptr;
  }
  if (dataSet.dataFourierPhase) {
    delete dataSet.dataFourierPhase;
    dataSet.dataFourierPhase = nullptr;
  }
  if (dataSet.theory) {
    delete dataSet.theory;
    dataSet.theory = nullptr;
  }
  if (dataSet.theoryFourierRe) {
    delete dataSet.theoryFourierRe;
    dataSet.theoryFourierRe = nullptr;
  }
  if (dataSet.theoryFourierIm) {
    delete dataSet.theoryFourierIm;
    dataSet.theoryFourierIm = nullptr;
  }
  if (dataSet.theoryFourierPwr) {
    delete dataSet.theoryFourierPwr;
    dataSet.theoryFourierPwr = nullptr;
  }
  if (dataSet.theoryFourierPhase) {
    delete dataSet.theoryFourierPhase;
    dataSet.theoryFourierPhase = nullptr;
  }
  if (dataSet.diff) {
    delete dataSet.diff;
    dataSet.diff = nullptr;
  }
  if (dataSet.diffFourierRe) {
    delete dataSet.diffFourierRe;
    dataSet.diffFourierRe = nullptr;
  }
  if (dataSet.diffFourierIm) {
    delete dataSet.diffFourierIm;
    dataSet.diffFourierIm = nullptr;
  }
  if (dataSet.diffFourierPwr) {
    delete dataSet.diffFourierPwr;
    dataSet.diffFourierPwr = nullptr;
  }
  if (dataSet.diffFourierPhase) {
    delete dataSet.diffFourierPhase;
    dataSet.diffFourierPhase = nullptr;
  }
  if (dataSet.dataRange) {
    delete dataSet.dataRange;
    dataSet.dataRange = nullptr;
  }
}

//--------------------------------------------------------------------------
// HandleDataSet (private)
//--------------------------------------------------------------------------
/**
 * <p>Generates the necessary histograms for plotting, starting from the pre-processed data.
 *
 * \param plotNo The number of the histo within the run list (fPlotNumber is the number of the plot BLOCK)
 * \param runNo The number of the run
 * \param data pre-processed data
 */
void PMusrCanvas::HandleDataSet(UInt_t plotNo, UInt_t runNo, PRunData *data)
{
  PMusrCanvasDataSet dataSet;
  TH1F *dataHisto;
  TH1F *theoHisto;

  TString  name;
  Double_t start;
  Double_t end;
  Double_t xmin, xmax, ymin, ymax;
  Int_t    size;

  InitDataSet(dataSet);

  // create plot range object for the data set
  dataSet.dataRange = new PMusrCanvasPlotRange();

  // dataHisto -------------------------------------------------------------
  // create histo specific infos
  name  = *fMsrHandler->GetMsrRunList()->at(runNo).GetRunName() + "_DataRunNo";
  name += static_cast<Int_t>(runNo);
  name += "_";
  name += fPlotNumber;
  start = data->GetDataTimeStart() - data->GetDataTimeStep()/2.0;
  end   = start + data->GetValue()->size()*data->GetDataTimeStep();
  size  = data->GetValue()->size();
  dataSet.dataRange->SetXRange(start, end); // full possible range
  // make sure that for asymmetry the y-range is initialized reasonably
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fPlotType == MSR_PLOT_ASYM)
    dataSet.dataRange->SetYRange(-0.4, 0.4);
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fPlotType == MSR_PLOT_BNMR)
    dataSet.dataRange->SetYRange(-0.4, 0.4);
  // extract necessary range information
  if ((fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin.size() == 0) &&
      !fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fUseFitRanges) { // no range information at all
    if (fXRangePresent) {
      if (fXmin > start)
        fXmin = start;
      if (fXmax < end)
        fXmax = end;
    } else {
      fXRangePresent = true;
      fXmin = start;
      fXmax = end;
    }
    if ((fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fPlotType == MSR_PLOT_ASYM) ||
        (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fPlotType == MSR_PLOT_BNMR) ||
        (fMsrHandler->GetMsrRunList()->at(runNo).IsLifetimeCorrected())) {
      fYRangePresent = true;
      fYmin = -0.4;
      fYmax = 0.4;
    }
  }

  // check if plot range is given in the msr-file, and if yes keep the values
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin.size() == 1) {
    // keep x-range
    xmin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin[0];
    xmax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmax[0];
    dataSet.dataRange->SetXRange(xmin, xmax);
    // keep range information
    fXRangePresent = true;
    fXmin = xmin;
    fXmax = xmax;
    // check if y-range is given as well
    if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin.size() != 0) {
      ymin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin[0];
      ymax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmax[0];
      dataSet.dataRange->SetYRange(ymin, ymax);
      // keep range information
      fYRangePresent = true;
      fYmin = ymin;
      fYmax = ymax;
    }
  }

  // check if 'use_fit_ranges' plotting is whished
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fUseFitRanges) {
    start = fMsrHandler->GetMsrRunList()->at(runNo).GetFitRange(0); // needed to estimate size
    if (start == PMUSR_UNDEFINED) { // not given in the run block, try the global block entry
      start = fMsrHandler->GetMsrGlobal()->GetFitRange(0);
    }
    end   = fMsrHandler->GetMsrRunList()->at(runNo).GetFitRange(1); // needed to estimate size
    if (end == PMUSR_UNDEFINED) { // not given in the run block, try the global block entry
      end = fMsrHandler->GetMsrGlobal()->GetFitRange(1);
    }
    size  = (Int_t) ((end - start) / data->GetDataTimeStep()) + 1;
    start = data->GetDataTimeStart() +
            (Int_t)((start - data->GetDataTimeStart())/data->GetDataTimeStep()) * data->GetDataTimeStep() -
            data->GetDataTimeStep()/2.0; // closesd start value compatible with the user given
    end   = start + size * data->GetDataTimeStep(); // closesd end value compatible with the user given
    dataSet.dataRange->SetXRange(start, end);

    // make sure that for asymmetry the y-range is initialized reasonably
    if ((fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fPlotType == MSR_PLOT_ASYM) ||
        (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fPlotType == MSR_PLOT_BNMR) ||
        (fMsrHandler->GetMsrRunList()->at(runNo).IsLifetimeCorrected())) {
      dataSet.dataRange->SetYRange(-0.4, 0.4);
    }

    // keep range information
    if (fXRangePresent) {
      if (fXmin > start)
        fXmin = start;
      if (fXmax < end)
        fXmax = end;
    } else {
      fXRangePresent = true;
      fXmin = start;
      fXmax = end;
    }
    // check if y-range is given as well
    if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin.size() != 0) {
      ymin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin[0];
      ymax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmax[0];
      dataSet.dataRange->SetYRange(ymin, ymax);
      // keep range information
      fYRangePresent = true;
      fYmin = ymin;
      fYmax = ymax;
    }
  }

  // check if 'sub_ranges' plotting is whished
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin.size() > 1) {    
    start = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin[runNo]; // needed to estimate size
    end   = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmax[runNo]; // needed to estimate size
    size  = (Int_t) ((end - start) / data->GetDataTimeStep()) + 1;
    start = data->GetDataTimeStart() +
            (Int_t)((fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin[runNo] - data->GetDataTimeStart())/data->GetDataTimeStep()) * data->GetDataTimeStep() -
            data->GetDataTimeStep()/2.0; // closesd start value compatible with the user given
    end   = start + size * data->GetDataTimeStep(); // closesd end value compatible with the user given
    dataSet.dataRange->SetXRange(start, end);
    // keep range information
    if (fXRangePresent) {
      if (fXmin > start)
        fXmin = start;
      if (fXmax < end)
        fXmax = end;
    } else {
      fXRangePresent = true;
      fXmin = start;
      fXmax = end;
    }

    // check if y-range is given as well
    if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin.size() != 0) {
      ymin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin[0];
      ymax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmax[0];
      dataSet.dataRange->SetYRange(ymin, ymax);
      // keep range information
      fYRangePresent = true;
      fYmin = ymin;
      fYmax = ymax;
    }
  }

  // invoke histo
  dataHisto = new TH1F(name, name, size, start, end);

  // fill histogram
  // 1st calculate the bin-range according to the plot options
  UInt_t startBin = 0;
  UInt_t endBin   = data->GetValue()->size();

  // check if 'use_fit_range' plotting is whished
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fUseFitRanges) {
    Double_t startFitRange = fMsrHandler->GetMsrRunList()->at(runNo).GetFitRange(0);
    if (startFitRange == PMUSR_UNDEFINED) { // not given in the run block, try the global block entry
      startFitRange = fMsrHandler->GetMsrGlobal()->GetFitRange(0);
    }
    Double_t dval = (startFitRange - data->GetDataTimeStart())/data->GetDataTimeStep();
    if (dval < 0.0) { // make sure that startBin >= 0
      startBin = 0;
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found startBin data < 0 for 'use_fit_range', will set it to 0" << std::endl << std::endl;
    } else if (dval >= (Double_t)data->GetValue()->size()) { // make sure that startBin <= length of data vector
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found startBin data=" << (UInt_t)dval << " >= data vector size=" << data->GetValue()->size() << " for 'use_fit_range',";
      std::cerr << std::endl << ">> will set it to data vector size" << std::endl << std::endl;
      startBin = data->GetValue()->size();
    } else {
      startBin = (UInt_t)dval;
    }

    Double_t endFitRange = fMsrHandler->GetMsrRunList()->at(runNo).GetFitRange(1);
    if (endFitRange == PMUSR_UNDEFINED) { // not given in the run block, try the global block entry
      endFitRange = fMsrHandler->GetMsrGlobal()->GetFitRange(1);
    }
    dval = (endFitRange - data->GetDataTimeStart())/data->GetDataTimeStep();
    if (dval < 0.0) { // make sure that endBin >= 0
      endBin = 0;
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found endBin data < 0 for 'use_fit_range', will set it to 0" << std::endl << std::endl;
    } else if (dval >= (Double_t)data->GetValue()->size()) { // make sure that endBin <= length of data vector
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found endBin data=" << (UInt_t)dval << " >= data vector size=" << data->GetValue()->size() << " for 'use_fit_range',";
      std::cerr << std::endl << ">> will set it to data vector size" << std::endl << std::endl;
      endBin = data->GetValue()->size();
    } else {
      endBin   = (UInt_t)dval;
    }
  }

  // check if 'sub_ranges' plotting is whished
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin.size() > 1) {
    Double_t dval = (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin[runNo] - data->GetDataTimeStart())/data->GetDataTimeStep();
    if (dval < 0.0) { // make sure that startBin >= 0
      startBin = 0;
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found startBin data < 0 for 'sub_ranges', will set it to 0" << std::endl << std::endl;
    } else if (dval >= (Double_t)data->GetValue()->size()) { // make sure that startBin <= length of data vector
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found startBin data=" << (UInt_t)dval << " >= data vector size=" << data->GetValue()->size() << " for 'sub_ranges',";
      std::cerr << std::endl << ">> will set it to data vector size" << std::endl << std::endl;
      startBin = data->GetValue()->size();
    } else {
      startBin = (UInt_t)dval;
    }

    dval = (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmax[runNo] - data->GetDataTimeStart())/data->GetDataTimeStep();
    if (dval < 0.0) { // make sure that endBin >= 0
      endBin = 0;
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found endBin data < 0 for 'sub_ranges', will set it to 0" << std::endl << std::endl;
    } else if (dval >= (Double_t)data->GetValue()->size()) { // make sure that endtBin <= length of data vector
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found endBin data=" << (UInt_t)dval << " >= data vector size=" << data->GetValue()->size() << " for 'sub_ranges',";
      std::cerr << std::endl << ">> will set it to data vector size" << std::endl << std::endl;
      endBin = data->GetValue()->size();
    } else {
      endBin   = (UInt_t)dval;
    }
  }

  for (UInt_t i=startBin; i<endBin; i++) {
    dataHisto->SetBinContent(i-startBin+1, data->GetValue()->at(i));
    dataHisto->SetBinError(i-startBin+1, data->GetError()->at(i));
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
  name  = *fMsrHandler->GetMsrRunList()->at(runNo).GetRunName() + "_TheoRunNo";
  name += (Int_t)runNo;
  name += "_";
  name += fPlotNumber;
  start = data->GetTheoryTimeStart() - data->GetTheoryTimeStep()/2.0;
  end   = start + data->GetTheory()->size()*data->GetTheoryTimeStep();
  size  = data->GetTheory()->size();

  // check if 'use_fit_range' plotting is whished
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fUseFitRanges) {
    start = fMsrHandler->GetMsrRunList()->at(runNo).GetFitRange(0); // needed to estimate size
    if (start == PMUSR_UNDEFINED) { // not given in the run block, try the global block entry
      start = fMsrHandler->GetMsrGlobal()->GetFitRange(0);
    }
    end   = fMsrHandler->GetMsrRunList()->at(runNo).GetFitRange(1); // needed to estimate size
    if (end  == PMUSR_UNDEFINED) { // not given in the run block, try the global block entry
      end = fMsrHandler->GetMsrGlobal()->GetFitRange(1);
    }
    size  = (Int_t) ((end - start) / data->GetTheoryTimeStep()) + 1;
    start = data->GetTheoryTimeStart() +
            (Int_t)((start - data->GetTheoryTimeStart())/data->GetTheoryTimeStep()) * data->GetTheoryTimeStep() -
            data->GetTheoryTimeStep()/2.0; // closesd start value compatible with the user given
    end   = start + size * data->GetTheoryTimeStep(); // closesd end value compatible with the user given
  }

  // check if 'sub_ranges' plotting is whished
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin.size() > 1) {
    start = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin[runNo]; // needed to estimate size
    end   = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmax[runNo]; // needed to estimate size
    size  = (Int_t) ((end - start) / data->GetTheoryTimeStep()) + 1;
    start = data->GetTheoryTimeStart() +
            (Int_t)((fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin[runNo] - data->GetTheoryTimeStart())/data->GetTheoryTimeStep()) * data->GetTheoryTimeStep() -
            data->GetTheoryTimeStep()/2.0; // closesd start value compatible with the user given
    end   = start + size * data->GetTheoryTimeStep(); // closesd end value compatible with the user given
}

  // invoke histo
  theoHisto = new TH1F(name, name, size, start, end);

  // fill histogram
  startBin = 0;
  endBin   = data->GetTheory()->size();

  // check if 'use_fit_range' plotting is whished
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fUseFitRanges) {
    Double_t startFitRange = fMsrHandler->GetMsrRunList()->at(runNo).GetFitRange(0);
    if (startFitRange == PMUSR_UNDEFINED) { // not given in the run block, try the global block entry
      startFitRange = fMsrHandler->GetMsrGlobal()->GetFitRange(0);
    }
    Double_t dval = (startFitRange - data->GetDataTimeStart())/data->GetTheoryTimeStep();
    if (dval < 0.0) { // make sure that startBin >= 0
      startBin = 0;
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found startBin theory < 0 for 'use_fit_range', will set it to 0" << std::endl << std::endl;
    } else if (dval >= (Double_t)data->GetTheory()->size()) { // make sure that startBin <= length of theory vector
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found startBin theory=" << (UInt_t)dval << " >= theory vector size=" << data->GetTheory()->size() << " for 'use_fit_range',";
      std::cerr << std::endl << ">> will set it to theory vector size" << std::endl << std::endl;
      startBin = data->GetTheory()->size();
    } else {
      startBin = (UInt_t)dval;
    }

    Double_t endFitRange = fMsrHandler->GetMsrRunList()->at(runNo).GetFitRange(1);
    if (endFitRange == PMUSR_UNDEFINED) { // not given in the run block, try the global block entry
      endFitRange = fMsrHandler->GetMsrGlobal()->GetFitRange(1);
    }
    dval = (endFitRange - data->GetDataTimeStart())/data->GetTheoryTimeStep();
    if (dval < 0.0) { // make sure that endBin >= 0
      endBin = 0;
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found endBin theory < 0 for 'use_fit_range', will set it to 0" << std::endl << std::endl;
    } else if (dval >= (Double_t)data->GetTheory()->size()) { // make sure that endBin <= length of theory vector
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found endBin theory=" << (UInt_t)dval << " >= theory vector size=" << data->GetTheory()->size() << " for 'use_fit_range',";
      std::cerr << std::endl << ">> will set it to theory vector size" << std::endl << std::endl;
      endBin = data->GetTheory()->size();
    } else {
      endBin   = (UInt_t)dval;
    }
  }

  // check if 'sub_ranges' plotting is whished
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin.size() > 1) {
    startBin = (UInt_t)((fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin[runNo] -data->GetDataTimeStart())/data->GetTheoryTimeStep());
    endBin   = (UInt_t)((fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmax[runNo] -data->GetDataTimeStart())/data->GetTheoryTimeStep());

    Double_t dval = (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin[runNo] -data->GetDataTimeStart())/data->GetTheoryTimeStep();
    if (dval < 0.0) { // make sure that startBin >= 0
      startBin = 0;
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found startBin theory < 0 for 'sub_ranges', will set it to 0" << std::endl << std::endl;
    } else if (dval >= (Double_t)data->GetTheory()->size()) { // make sure that startBin <= length of theory vector
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found startBin theory=" << (UInt_t)dval << " >= theory vector size=" << data->GetTheory()->size() << " for 'sub_ranges',";
      std::cerr << std::endl << ">> will set it to theory vector size" << std::endl << std::endl;
      startBin = data->GetTheory()->size();
    } else {
      startBin = (UInt_t)dval;
    }

    dval = (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmax[runNo] -data->GetDataTimeStart())/data->GetTheoryTimeStep();
    if (dval < 0.0) { // make sure that endBin >= 0
      endBin = 0;
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found endBin theory < 0 for 'sub_ranges', will set it to 0" << std::endl << std::endl;
    } else if (dval >= (Double_t)data->GetTheory()->size()) { // make sure that endtBin <= length of theory vector
      std::cerr << std::endl << ">> PMusrCanvas::HandleDataSet(): **WARNING** found endBin theory=" << (UInt_t)dval << " >= theory vector size=" << data->GetTheory()->size() << " for 'sub_ranges',";
      std::cerr << std::endl << ">> will set it to theory vector size" << std::endl << std::endl;
      endBin = data->GetTheory()->size();
    } else {
      endBin   = (UInt_t)dval;
    }
  }

  for (UInt_t i=startBin; i<endBin; i++) {
    theoHisto->SetBinContent(i-startBin+1, data->GetTheory()->at(i));
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
  dataSet.diffFourierTag = 0; // not relevant at this point

  fData.push_back(dataSet);
}

//--------------------------------------------------------------------------
// HandleNonMusrDataSet (private)
//--------------------------------------------------------------------------
/**
 * <p>Generates the necessary error graphs for plotting, starting from the pre-processed data.
 *
 * \param plotNo The number of the histo within the run list (fPlotNumber is the number of the plot BLOCK)
 * \param runNo The number of the run
 * \param data pre-processed data
 */
void PMusrCanvas::HandleNonMusrDataSet(UInt_t plotNo, UInt_t runNo, PRunData *data)
{
  PMusrCanvasNonMusrDataSet dataSet;
  TGraphErrors *dataHisto;
  TGraphErrors *theoHisto;

  InitDataSet(dataSet);

  // create plot range object for the data set and fill it
  dataSet.dataRange = new PMusrCanvasPlotRange();

  // dataHisto -------------------------------------------------------------

  // invoke graph
  dataHisto = new TGraphErrors(data->GetX()->size());

  // fill graph
  for (UInt_t i=0; i<data->GetX()->size(); i++) {
    dataHisto->SetPoint(i, data->GetX()->at(i), data->GetValue()->at(i));
    dataHisto->SetPointError(i, 0.0, data->GetError()->at(i));
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
  theoHisto = new TGraphErrors(data->GetXTheory()->size());

  // fill graph
  for (UInt_t i=0; i<data->GetXTheory()->size(); i++) {
    theoHisto->SetPoint(i, data->GetXTheory()->at(i), data->GetTheory()->at(i));
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
  dataSet.diffFourierTag = 0; // not relevant at this point

  // check the plot range options
  Double_t xmin=0.0, xmax=0.0, ymin=0.0, ymax=0.0, x=0.0, y=0.0;

  // if no plot-range entry is present, initialize the plot range to the maximal possible given the data
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin.size() == 0) {
    dataSet.data->GetPoint(0, xmin, y); // get xmin
    dataSet.data->GetPoint(dataSet.data->GetN()-1, xmax, y); // get xmax
    dataSet.data->GetPoint(0, x, y); // init ymin/ymax
    ymin = y;
    ymax = y;
    for (Int_t i=1; i<dataSet.data->GetN(); i++) {
      dataSet.data->GetPoint(i, x, y);
      if (y < ymin)
        ymin = y;
      if (y > ymax)
        ymax = y;
    }
    Double_t dx = 0.025*(xmax-xmin);
    Double_t dy = 0.025*(ymax-ymin);
    dataSet.dataRange->SetXRange(xmin-dx, xmax+dx);
    dataSet.dataRange->SetYRange(ymin-dy, ymax+dy);
  }

  // check if plot range is given in the msr-file, and if yes keep the values
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin.size() == 1) {
    // keep x-range
    xmin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin[0];
    xmax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmax[0];
    dataSet.dataRange->SetXRange(xmin, xmax);

    // check if y-range is given as well
    if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin.size() != 0) {
      ymin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin[0];
      ymax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmax[0];
      dataSet.dataRange->SetYRange(ymin, ymax);
    }
  }

  // check if 'use_fit_range' plotting is whished
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fUseFitRanges) {
    xmin = fMsrHandler->GetMsrRunList()->at(runNo).GetFitRange(0); // needed to estimate size
    xmax = fMsrHandler->GetMsrRunList()->at(runNo).GetFitRange(1); // needed to estimate size
    dataSet.dataRange->SetXRange(xmin, xmax);

    // check if y-range is given as well
    if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin.size() != 0) {
      ymin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin[0];
      ymax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmax[0];
      dataSet.dataRange->SetYRange(ymin, ymax);
    }
  }

  // check if 'sub_ranges' plotting is whished
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin.size() > 1) {
    xmin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin[runNo]; // needed to estimate size
    xmax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmax[runNo]; // needed to estimate size
    dataSet.dataRange->SetXRange(xmin, xmax);

    // check if y-range is given as well
    if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin.size() != 0) {
      ymin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin[0];
      ymax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmax[0];
      dataSet.dataRange->SetYRange(ymin, ymax);
    }
  }

  // keep maximal range of all plot present
  fXRangePresent = true;
  fYRangePresent = true;
  if (plotNo == 0) {
    fXmin = dataSet.dataRange->GetXmin();
    fXmax = dataSet.dataRange->GetXmax();
    fYmin = dataSet.dataRange->GetYmin();
    fYmax = dataSet.dataRange->GetYmax();
  } else {
    if (fXmin > dataSet.dataRange->GetXmin())
      fXmin = dataSet.dataRange->GetXmin();
    if (fXmax < dataSet.dataRange->GetXmax())
      fXmax = dataSet.dataRange->GetXmax();
    if (fYmin > dataSet.dataRange->GetYmin())
      fYmin = dataSet.dataRange->GetYmin();
    if (fYmax < dataSet.dataRange->GetYmax())
      fYmax = dataSet.dataRange->GetYmax();
  }

  fNonMusrData.push_back(dataSet);
}

//--------------------------------------------------------------------------
// HandleDifference (private)
//--------------------------------------------------------------------------
/**
 * <p>Handles the calculation of the difference spectra (i.e. data-theory).
 * It allocates the necessary objects if they are not already present. At the
 * end it calls the plotting routine.
 */
void PMusrCanvas::HandleDifference()
{
  // check if it is necessary to calculate diff data
  if ((fPlotType != MSR_PLOT_NON_MUSR) && (fData[0].diff == nullptr)) {
    TH1F *diffHisto;
    TString name;
    // loop over all histos
    for (UInt_t i=0; i<fData.size(); i++) {
      // create difference histos
      name = TString(fData[i].data->GetTitle()) + "_diff";
      diffHisto = new TH1F(name, name, fData[i].data->GetNbinsX(),
                           fData[i].data->GetXaxis()->GetXmin(),
                           fData[i].data->GetXaxis()->GetXmax());

      // set marker and line color
      diffHisto->SetMarkerColor(fData[i].data->GetMarkerColor());
      diffHisto->SetLineColor(fData[i].data->GetLineColor());
      // set marker size
      diffHisto->SetMarkerSize(fData[i].data->GetMarkerSize());
      // set marker type
      diffHisto->SetMarkerStyle(fData[i].data->GetMarkerStyle());

      // keep difference histo
      fData[i].diff = diffHisto;
      // calculate diff histo entry
      double value;
      for (Int_t j=1; j<fData[i].data->GetNbinsX()-1; j++) {
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
  } else if ((fPlotType == MSR_PLOT_NON_MUSR) && (fNonMusrData[0].diff == nullptr)) {
    TGraphErrors *diffHisto;
    TString name;
    // loop over all histos
    for (UInt_t i=0; i<fNonMusrData.size(); i++) {
      // make sure data exists
      assert(fNonMusrData[i].data != nullptr);

      // create difference histos
      diffHisto = new TGraphErrors(fNonMusrData[i].data->GetN());

      // create difference histos
      name = TString(fNonMusrData[i].data->GetTitle()) + "_diff";
      diffHisto->SetNameTitle(name.Data(), name.Data());

      // set marker and line color
      diffHisto->SetMarkerColor(fNonMusrData[i].data->GetMarkerColor());
      diffHisto->SetLineColor(fNonMusrData[i].data->GetLineColor());
      // set marker size
      diffHisto->SetMarkerSize(fNonMusrData[i].data->GetMarkerSize());
      // set marker type
      diffHisto->SetMarkerStyle(fNonMusrData[i].data->GetMarkerStyle());

      // keep difference histo
      fNonMusrData[i].diff = diffHisto;
      // calculate diff histo entry
      double value;
      double x, y;
      for (Int_t j=0; j<fNonMusrData[i].data->GetN(); j++) {
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

//--------------------------------------------------------------------------
// HandleFourier (private)
//--------------------------------------------------------------------------
/**
 * <p>Handles the calculation of the Fourier transform.
 * It allocates the necessary objects if they are not already present. At the
 * end it calls the plotting routine.
 */
void PMusrCanvas::HandleFourier()
{
  Double_t re, im;

  // check if plot type is appropriate for fourier
  if (fPlotType == MSR_PLOT_NON_MUSR)
    return;

  // check if fourier needs to be calculated
  if (fData[0].dataFourierRe == nullptr) {
    Int_t bin;
    double startTime = fXmin;
    double endTime = fXmax;
    if (!fStartWithFourier) { // fHistoFrame present, hence get start/end from it
      bin = fHistoFrame->GetXaxis()->GetFirst();
      startTime = fHistoFrame->GetBinLowEdge(bin);
      bin = fHistoFrame->GetXaxis()->GetLast();
      endTime   = fHistoFrame->GetBinLowEdge(bin)+fHistoFrame->GetBinWidth(bin);
    }
    for (UInt_t i=0; i<fData.size(); i++) {
      // calculate fourier transform of the data
      PFourier fourierData(fData[i].data, fFourier.fUnits, startTime, endTime, fFourier.fDCCorrected, fFourier.fFourierPower);
      if (!fourierData.IsValid()) {
        std::cerr << std::endl << ">> PMusrCanvas::HandleFourier(): **SEVERE ERROR** couldn't invoke PFourier to calculate the Fourier data ..." << std::endl;
        return;
      }
      fourierData.Transform(fFourier.fApodization);
      double scale;
      scale = sqrt(fData[0].data->GetBinWidth(1)/(endTime-startTime));
      // get real part of the data
      fData[i].dataFourierRe = fourierData.GetRealFourier(scale);
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
      Int_t powerPad = (Int_t)round(log((endTime-startTime)/fData[i].theory->GetBinWidth(1))/log(2))+3;
      PFourier fourierTheory(fData[i].theory, fFourier.fUnits, startTime, endTime, fFourier.fDCCorrected, powerPad);
      if (!fourierTheory.IsValid()) {
        std::cerr << std::endl << ">> PMusrCanvas::HandleFourier(): **SEVERE ERROR** couldn't invoke PFourier to calculate the Fourier theory ..." << std::endl;
        return;
      }
      fourierTheory.Transform(fFourier.fApodization);
      scale = sqrt(fData[0].theory->GetBinWidth(1)/(endTime-startTime)*fData[0].theory->GetBinWidth(1)/fData[0].data->GetBinWidth(1));
      // get real part of the data
      fData[i].theoryFourierRe = fourierTheory.GetRealFourier(scale);
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

    // phase opt. real FT requested initially in the msr-file, hence calculate it here
    if (fCurrentPlotView == PV_FOURIER_PHASE_OPT_REAL) {
      CalcPhaseOptReFT();
    }

    // apply global phase if present
    if (fFourier.fPhase.size() != 0.0) {
      double cp;
      double sp;

      fCurrentFourierPhase = fFourier.fPhase;

      for (UInt_t i=0; i<fData.size(); i++) { // loop over all data sets
        if (fFourier.fPhase.size() == 1) {
          cp = TMath::Cos(fFourier.fPhase[0]/180.0*TMath::Pi());
          sp = TMath::Sin(fFourier.fPhase[0]/180.0*TMath::Pi());
        } else {
          cp = TMath::Cos(fFourier.fPhase[i]/180.0*TMath::Pi());
          sp = TMath::Sin(fFourier.fPhase[i]/180.0*TMath::Pi());
        }
        if ((fData[i].dataFourierRe != nullptr) && (fData[i].dataFourierIm != nullptr)) {
          for (Int_t j=0; j<fData[i].dataFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
            // calculate new fourier data set value
            re = fData[i].dataFourierRe->GetBinContent(j) * cp + fData[i].dataFourierIm->GetBinContent(j) * sp;
            im = fData[i].dataFourierIm->GetBinContent(j) * cp - fData[i].dataFourierRe->GetBinContent(j) * sp;
            // overwrite fourier data set value
            fData[i].dataFourierRe->SetBinContent(j, re);
            fData[i].dataFourierIm->SetBinContent(j, im);
          }
        }
        if ((fData[i].theoryFourierRe != nullptr) && (fData[i].theoryFourierIm != nullptr)) {
          for (Int_t j=0; j<fData[i].theoryFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
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
  }
}

//--------------------------------------------------------------------------
// HandleDifferenceFourier (private)
//--------------------------------------------------------------------------
/**
 * <p>Handles the calculation of the Fourier transform of the difference spectra (i.e. data-theory).
 * It allocates the necessary objects if they are not already present. At the
 * end it calls the plotting routine.
 */
void PMusrCanvas::HandleDifferenceFourier()
{
  // check if plot type is appropriate for fourier
  if (fPlotType == MSR_PLOT_NON_MUSR)
    return;

  // check if fourier needs to be calculated
  if (fData[0].diffFourierRe == nullptr) {
    // check if difference has been already calcualted, if not do it
    if (fData[0].diff == nullptr)
      HandleDifference();

    // get time from the current fHistoFrame
    Int_t bin;
    bin = fHistoFrame->GetXaxis()->GetFirst();
    double startTime = fHistoFrame->GetBinCenter(bin);
    bin = fHistoFrame->GetXaxis()->GetLast();
    double endTime   = fHistoFrame->GetBinCenter(bin);

    for (UInt_t i=0; i<fData.size(); i++) {
      // calculate fourier transform of the data
      PFourier fourierData(fData[i].diff, fFourier.fUnits, startTime, endTime, fFourier.fDCCorrected, fFourier.fFourierPower);
      if (!fourierData.IsValid()) {
        std::cerr << std::endl << ">> PMusrCanvas::HandleFourier(): **SEVERE ERROR** couldn't invoke PFourier to calculate the Fourier diff ..." << std::endl;
        return;
      }
      fourierData.Transform(fFourier.fApodization);
      double scale;
      scale = sqrt(fData[0].diff->GetBinWidth(1)/(endTime-startTime));
      // get real part of the data
      fData[i].diffFourierRe = fourierData.GetRealFourier(scale);
      // get imaginary part of the data
      fData[i].diffFourierIm = fourierData.GetImaginaryFourier(scale);
      // get power part of the data
      fData[i].diffFourierPwr = fourierData.GetPowerFourier(scale);
      // get phase part of the data
      fData[i].diffFourierPhase = fourierData.GetPhaseFourier();

      // set marker and line color
      fData[i].diffFourierRe->SetMarkerColor(fData[i].diff->GetMarkerColor());
      fData[i].diffFourierRe->SetLineColor(fData[i].diff->GetLineColor());
      fData[i].diffFourierIm->SetMarkerColor(fData[i].diff->GetMarkerColor());
      fData[i].diffFourierIm->SetLineColor(fData[i].diff->GetLineColor());
      fData[i].diffFourierPwr->SetMarkerColor(fData[i].diff->GetMarkerColor());
      fData[i].diffFourierPwr->SetLineColor(fData[i].diff->GetLineColor());
      fData[i].diffFourierPhase->SetMarkerColor(fData[i].diff->GetMarkerColor());
      fData[i].diffFourierPhase->SetLineColor(fData[i].diff->GetLineColor());

      // set marker size
      fData[i].diffFourierRe->SetMarkerSize(1);
      fData[i].diffFourierIm->SetMarkerSize(1);
      fData[i].diffFourierPwr->SetMarkerSize(1);
      fData[i].diffFourierPhase->SetMarkerSize(1);
      // set marker type
      fData[i].diffFourierRe->SetMarkerStyle(fData[i].diff->GetMarkerStyle());
      fData[i].diffFourierIm->SetMarkerStyle(fData[i].diff->GetMarkerStyle());
      fData[i].diffFourierPwr->SetMarkerStyle(fData[i].diff->GetMarkerStyle());
      fData[i].diffFourierPhase->SetMarkerStyle(fData[i].diff->GetMarkerStyle());

      // set diffFourierTag
      fData[i].diffFourierTag = 1; // d-f
    }

    // apply phase
    if (fFourier.fPhase.size() != 0.0) {
      double re, im;
      double cp;
      double sp;

      fCurrentFourierPhase = fFourier.fPhase;

      for (UInt_t i=0; i<fData.size(); i++) { // loop over all data sets
        if ((fData[i].diffFourierRe != nullptr) && (fData[i].diffFourierIm != nullptr)) {
          if (fFourier.fPhase.size() == 1) {
            cp = TMath::Cos(fFourier.fPhase[0]/180.0*TMath::Pi());
            sp = TMath::Sin(fFourier.fPhase[0]/180.0*TMath::Pi());
          } else {
            cp = TMath::Cos(fFourier.fPhase[i]/180.0*TMath::Pi());
            sp = TMath::Sin(fFourier.fPhase[i]/180.0*TMath::Pi());
          }
          for (Int_t j=0; j<fData[i].diffFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
            // calculate new fourier data set value
            re = fData[i].diffFourierRe->GetBinContent(j) * cp + fData[i].diffFourierIm->GetBinContent(j) * sp;
            im = fData[i].diffFourierIm->GetBinContent(j) * cp - fData[i].diffFourierRe->GetBinContent(j) * sp;
            // overwrite fourier data set value
            fData[i].diffFourierRe->SetBinContent(j, re);
            fData[i].diffFourierIm->SetBinContent(j, im);
          }
        }
      }
    }
  }
}

//--------------------------------------------------------------------------
// HandleFourierDifference (private)
//--------------------------------------------------------------------------
/**
 * <p>Handles the calculation of the difference of the Fourier spectra.
 * It allocates the necessary objects if they are not already present. At the
 * end it calls the plotting routine.
 */
void PMusrCanvas::HandleFourierDifference()
{
  // check if plot type is appropriate for fourier
  if (fPlotType == MSR_PLOT_NON_MUSR)
    return;

  // check if fourier needs to be calculated
  if (fData[0].diffFourierRe == nullptr) {
    // calculate all the Fourier differences
    Double_t dval, dvalx;
    TString name;
    Int_t theoBin;
    for (UInt_t i=0; i<fData.size(); i++) {
      // create difference histos
      // real part
      name = TString(fData[i].dataFourierRe->GetTitle()) + "_diff";
      fData[i].diffFourierRe = new TH1F(name, name, fData[i].dataFourierRe->GetNbinsX(),
                                        fData[i].dataFourierRe->GetXaxis()->GetXmin(),
                                        fData[i].dataFourierRe->GetXaxis()->GetXmax());
      // imaginary part
      name = TString(fData[i].dataFourierIm->GetTitle()) + "_diff";
      fData[i].diffFourierIm = new TH1F(name, name, fData[i].dataFourierIm->GetNbinsX(),
                                        fData[i].dataFourierIm->GetXaxis()->GetXmin(),
                                        fData[i].dataFourierIm->GetXaxis()->GetXmax());
      // power part
      name = TString(fData[i].dataFourierPwr->GetTitle()) + "_diff";
      fData[i].diffFourierPwr = new TH1F(name, name, fData[i].dataFourierPwr->GetNbinsX(),
                                         fData[i].dataFourierPwr->GetXaxis()->GetXmin(),
                                         fData[i].dataFourierPwr->GetXaxis()->GetXmax());
      // phase part
      name = TString(fData[i].dataFourierPhase->GetTitle()) + "_diff";
      fData[i].diffFourierPhase = new TH1F(name, name, fData[i].dataFourierPhase->GetNbinsX(),
                                         fData[i].dataFourierPhase->GetXaxis()->GetXmin(),
                                         fData[i].dataFourierPhase->GetXaxis()->GetXmax());

      // phase optimized real part
      name = TString(fData[i].dataFourierPhaseOptReal->GetTitle()) + "_diff";
      fData[i].diffFourierPhaseOptReal = new TH1F(name, name, fData[i].dataFourierPhaseOptReal->GetNbinsX(),
                                         fData[i].dataFourierPhaseOptReal->GetXaxis()->GetXmin(),
                                         fData[i].dataFourierPhaseOptReal->GetXaxis()->GetXmax());

      // calculate difference
      for (UInt_t j=1; j<fData[i].dataFourierRe->GetEntries(); j++) {
        dvalx = fData[i].dataFourierRe->GetXaxis()->GetBinCenter(j); // get x-axis value of bin j
        theoBin = fData[i].theoryFourierRe->FindBin(dvalx); // get the theory x-axis bin
        dval = fData[i].dataFourierRe->GetBinContent(j) - fData[i].theoryFourierRe->GetBinContent(theoBin);
        fData[i].diffFourierRe->SetBinContent(j, dval);
        dvalx = fData[i].dataFourierIm->GetXaxis()->GetBinCenter(j); // get x-axis value of bin j
        theoBin = fData[i].theoryFourierIm->FindBin(dvalx); // get the theory x-axis bin
        dval = fData[i].dataFourierIm->GetBinContent(j) - fData[i].theoryFourierIm->GetBinContent(theoBin);
        fData[i].diffFourierIm->SetBinContent(j, dval);
        dvalx = fData[i].dataFourierPwr->GetXaxis()->GetBinCenter(j); // get x-axis value of bin j
        theoBin = fData[i].theoryFourierPwr->FindBin(dvalx); // get the theory x-axis bin
        dval = fData[i].dataFourierPwr->GetBinContent(j) - fData[i].theoryFourierPwr->GetBinContent(theoBin);
        fData[i].diffFourierPwr->SetBinContent(j, dval);
        dvalx = fData[i].dataFourierPhase->GetXaxis()->GetBinCenter(j); // get x-axis value of bin j
        theoBin = fData[i].theoryFourierPhase->FindBin(dvalx); // get the theory x-axis bin
        dval = fData[i].dataFourierPhase->GetBinContent(j) - fData[i].theoryFourierPhase->GetBinContent(theoBin);
        fData[i].diffFourierPhase->SetBinContent(j, dval);
        dvalx = fData[i].dataFourierPhaseOptReal->GetXaxis()->GetBinCenter(j); // get x-axis value of bin j
        theoBin = fData[i].theoryFourierPhaseOptReal->FindBin(dvalx); // get the theory x-axis bin
        dval = fData[i].dataFourierPhaseOptReal->GetBinContent(j) - fData[i].theoryFourierPhaseOptReal->GetBinContent(theoBin);
        fData[i].diffFourierPhaseOptReal->SetBinContent(j, dval);
      }
    }

    for (UInt_t i=0; i<fData.size(); i++) {
      // set marker and line color
      fData[i].diffFourierRe->SetMarkerColor(fData[i].dataFourierRe->GetMarkerColor());
      fData[i].diffFourierRe->SetLineColor(fData[i].dataFourierRe->GetLineColor());
      fData[i].diffFourierIm->SetMarkerColor(fData[i].dataFourierIm->GetMarkerColor());
      fData[i].diffFourierIm->SetLineColor(fData[i].dataFourierIm->GetLineColor());
      fData[i].diffFourierPwr->SetMarkerColor(fData[i].dataFourierPwr->GetMarkerColor());
      fData[i].diffFourierPwr->SetLineColor(fData[i].dataFourierPwr->GetLineColor());
      fData[i].diffFourierPhase->SetMarkerColor(fData[i].dataFourierPhase->GetMarkerColor());
      fData[i].diffFourierPhase->SetLineColor(fData[i].dataFourierPhase->GetLineColor());
      fData[i].diffFourierPhaseOptReal->SetMarkerColor(fData[i].dataFourierPhaseOptReal->GetMarkerColor());
      fData[i].diffFourierPhaseOptReal->SetLineColor(fData[i].dataFourierPhaseOptReal->GetLineColor());

      // set marker size
      fData[i].diffFourierRe->SetMarkerSize(1);
      fData[i].diffFourierIm->SetMarkerSize(1);
      fData[i].diffFourierPwr->SetMarkerSize(1);
      fData[i].diffFourierPhase->SetMarkerSize(1);
      fData[i].diffFourierPhaseOptReal->SetMarkerSize(1);
      // set marker type
      fData[i].diffFourierRe->SetMarkerStyle(fData[i].dataFourierRe->GetMarkerStyle());
      fData[i].diffFourierIm->SetMarkerStyle(fData[i].dataFourierIm->GetMarkerStyle());
      fData[i].diffFourierPwr->SetMarkerStyle(fData[i].dataFourierPwr->GetMarkerStyle());
      fData[i].diffFourierPhase->SetMarkerStyle(fData[i].dataFourierPhase->GetMarkerStyle());
      fData[i].diffFourierPhaseOptReal->SetMarkerStyle(fData[i].dataFourierPhaseOptReal->GetMarkerStyle());

      // set diffFourierTag
      fData[i].diffFourierTag = 2; // f-d
    }
  }
}

//--------------------------------------------------------------------------
// HandleAverage (private)
//--------------------------------------------------------------------------
/**
 * <p>Handles the calculation of the average of the ploted data.
 * It allocates the necessary objects if they are not already present. At the
 * end it calls the plotting routine.
 */
void PMusrCanvas::HandleAverage()
{
  // check if plot type is appropriate for average
  if (fPlotType == MSR_PLOT_NON_MUSR)
    return;

  // in case there is still some average left over, cleanup first
  if (fDataAvg.data != nullptr) {
    CleanupAverage();
  }

  // create all the needed average data sets
  TString name("");
  if (fData[0].data != nullptr) {
    name = TString(fData[0].data->GetTitle()) + "_avg";
    fDataAvg.data = new TH1F(name, name, fData[0].data->GetNbinsX(),
                             fData[0].data->GetXaxis()->GetXmin(),
                             fData[0].data->GetXaxis()->GetXmax());
  }
  if (fData[0].dataFourierRe != nullptr) {
    name = TString(fData[0].dataFourierRe->GetTitle()) + "_avg";
    fDataAvg.dataFourierRe = new TH1F(name, name, fData[0].dataFourierRe->GetNbinsX(),
                                      fData[0].dataFourierRe->GetXaxis()->GetXmin(),
                                      fData[0].dataFourierRe->GetXaxis()->GetXmax());
  }
  if (fData[0].dataFourierIm != nullptr) {
    name = TString(fData[0].dataFourierIm->GetTitle()) + "_avg";
    fDataAvg.dataFourierIm = new TH1F(name, name, fData[0].dataFourierIm->GetNbinsX(),
                                      fData[0].dataFourierIm->GetXaxis()->GetXmin(),
                                      fData[0].dataFourierIm->GetXaxis()->GetXmax());
  }
  if (fData[0].dataFourierPwr != nullptr) {
    name = TString(fData[0].dataFourierPwr->GetTitle()) + "_avg";
    fDataAvg.dataFourierPwr = new TH1F(name, name, fData[0].dataFourierPwr->GetNbinsX(),
                                       fData[0].dataFourierPwr->GetXaxis()->GetXmin(),
                                       fData[0].dataFourierPwr->GetXaxis()->GetXmax());
  }
  if (fData[0].dataFourierPhase != nullptr) {
    name = TString(fData[0].dataFourierPhase->GetTitle()) + "_avg";
    fDataAvg.dataFourierPhase = new TH1F(name, name, fData[0].dataFourierPhase->GetNbinsX(),
                                         fData[0].dataFourierPhase->GetXaxis()->GetXmin(),
                                         fData[0].dataFourierPhase->GetXaxis()->GetXmax());
  }
  if (fData[0].dataFourierPhaseOptReal != nullptr) {
    name = TString(fData[0].dataFourierPhaseOptReal->GetTitle()) + "_avg";
    fDataAvg.dataFourierPhaseOptReal = new TH1F(name, name, fData[0].dataFourierPhaseOptReal->GetNbinsX(),
                                         fData[0].dataFourierPhaseOptReal->GetXaxis()->GetXmin(),
                                         fData[0].dataFourierPhaseOptReal->GetXaxis()->GetXmax());
  }
  if (fData[0].theory != nullptr) {
    name = TString(fData[0].theory->GetTitle()) + "_avg";
    fDataAvg.theory = new TH1F(name, name, fData[0].theory->GetNbinsX(),
                             fData[0].theory->GetXaxis()->GetXmin(),
                             fData[0].theory->GetXaxis()->GetXmax());
  }
  if (fData[0].theoryFourierRe != nullptr) {
    name = TString(fData[0].theoryFourierRe->GetTitle()) + "_avg";
    fDataAvg.theoryFourierRe = new TH1F(name, name, fData[0].theoryFourierRe->GetNbinsX(),
                                        fData[0].theoryFourierRe->GetXaxis()->GetXmin(),
                                        fData[0].theoryFourierRe->GetXaxis()->GetXmax());
  }
  if (fData[0].theoryFourierIm != nullptr) {
    name = TString(fData[0].theoryFourierIm->GetTitle()) + "_avg";
    fDataAvg.theoryFourierIm = new TH1F(name, name, fData[0].theoryFourierIm->GetNbinsX(),
                                        fData[0].theoryFourierIm->GetXaxis()->GetXmin(),
                                        fData[0].theoryFourierIm->GetXaxis()->GetXmax());
  }
  if (fData[0].theoryFourierPwr != nullptr) {
    name = TString(fData[0].theoryFourierPwr->GetTitle()) + "_avg";
    fDataAvg.theoryFourierPwr = new TH1F(name, name, fData[0].theoryFourierPwr->GetNbinsX(),
                                         fData[0].theoryFourierPwr->GetXaxis()->GetXmin(),
                                         fData[0].theoryFourierPwr->GetXaxis()->GetXmax());
  }
  if (fData[0].theoryFourierPhase != nullptr) {
    name = TString(fData[0].theoryFourierPhase->GetTitle()) + "_avg";
    fDataAvg.theoryFourierPhase = new TH1F(name, name, fData[0].theoryFourierPhase->GetNbinsX(),
                                           fData[0].theoryFourierPhase->GetXaxis()->GetXmin(),
                                           fData[0].theoryFourierPhase->GetXaxis()->GetXmax());
  }
  if (fData[0].theoryFourierPhaseOptReal != nullptr) {
    name = TString(fData[0].theoryFourierPhaseOptReal->GetTitle()) + "_avg";
    fDataAvg.theoryFourierPhaseOptReal = new TH1F(name, name, fData[0].theoryFourierPhaseOptReal->GetNbinsX(),
                                           fData[0].theoryFourierPhaseOptReal->GetXaxis()->GetXmin(),
                                           fData[0].theoryFourierPhaseOptReal->GetXaxis()->GetXmax());
  }
  if (fData[0].diff != nullptr) {
    name = TString(fData[0].diff->GetTitle()) + "_avg";
    fDataAvg.diff = new TH1F(name, name, fData[0].diff->GetNbinsX(),
                             fData[0].diff->GetXaxis()->GetXmin(),
                             fData[0].diff->GetXaxis()->GetXmax());
  }
  if (fData[0].diffFourierRe != nullptr) {
    name = TString(fData[0].diffFourierRe->GetTitle()) + "_avg";
    fDataAvg.diff = new TH1F(name, name, fData[0].diffFourierRe->GetNbinsX(),
                             fData[0].diffFourierRe->GetXaxis()->GetXmin(),
                             fData[0].diffFourierRe->GetXaxis()->GetXmax());
  }
  if (fData[0].diffFourierIm != nullptr) {
    name = TString(fData[0].diffFourierIm->GetTitle()) + "_avg";
    fDataAvg.diffFourierIm = new TH1F(name, name, fData[0].diffFourierIm->GetNbinsX(),
                                     fData[0].diffFourierIm->GetXaxis()->GetXmin(),
                                     fData[0].diffFourierIm->GetXaxis()->GetXmax());
  }
  if (fData[0].diffFourierPwr != nullptr) {
    name = TString(fData[0].diffFourierPwr->GetTitle()) + "_avg";
    fDataAvg.diffFourierPwr = new TH1F(name, name, fData[0].diffFourierPwr->GetNbinsX(),
                                       fData[0].diffFourierPwr->GetXaxis()->GetXmin(),
                                       fData[0].diffFourierPwr->GetXaxis()->GetXmax());
  }
  if (fData[0].diffFourierPhase != nullptr) {
    name = TString(fData[0].diffFourierPhase->GetTitle()) + "_avg";
    fDataAvg.diffFourierPhase = new TH1F(name, name, fData[0].diffFourierPhase->GetNbinsX(),
                                         fData[0].diffFourierPhase->GetXaxis()->GetXmin(),
                                         fData[0].diffFourierPhase->GetXaxis()->GetXmax());
  }
  if (fData[0].diffFourierPhaseOptReal != nullptr) {
    name = TString(fData[0].diffFourierPhaseOptReal->GetTitle()) + "_avg";
    fDataAvg.diffFourierPhaseOptReal = new TH1F(name, name, fData[0].diffFourierPhaseOptReal->GetNbinsX(),
                                         fData[0].diffFourierPhaseOptReal->GetXaxis()->GetXmin(),
                                         fData[0].diffFourierPhaseOptReal->GetXaxis()->GetXmax());
  }

  // calculate all the average data sets
  double dval;
  if (fDataAvg.data != nullptr) {
    for (Int_t i=0; i<fData[0].data->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].data, fData[0].data->GetBinCenter(i));
      }
      fDataAvg.data->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.data->SetMarkerColor(fData[0].data->GetMarkerColor());
    fDataAvg.data->SetLineColor(fData[0].data->GetLineColor());
    fDataAvg.data->SetMarkerSize(fData[0].data->GetMarkerSize());
    fDataAvg.data->SetMarkerStyle(fData[0].data->GetMarkerStyle());
  }
  if (fDataAvg.dataFourierRe != nullptr) {
    for (Int_t i=0; i<fData[0].dataFourierRe->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].dataFourierRe, fData[0].dataFourierRe->GetBinCenter(i));
      }
      fDataAvg.dataFourierRe->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.dataFourierRe->SetMarkerColor(fData[0].dataFourierRe->GetMarkerColor());
    fDataAvg.dataFourierRe->SetLineColor(fData[0].dataFourierRe->GetLineColor());
    fDataAvg.dataFourierRe->SetMarkerSize(fData[0].dataFourierRe->GetMarkerSize());
    fDataAvg.dataFourierRe->SetMarkerStyle(fData[0].dataFourierRe->GetMarkerStyle());
  }
  if (fDataAvg.dataFourierIm != nullptr) {
    for (Int_t i=0; i<fData[0].dataFourierIm->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].dataFourierIm, fData[0].dataFourierIm->GetBinCenter(i));
      }
      fDataAvg.dataFourierIm->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.dataFourierIm->SetMarkerColor(fData[0].dataFourierIm->GetMarkerColor());
    fDataAvg.dataFourierIm->SetLineColor(fData[0].dataFourierIm->GetLineColor());
    fDataAvg.dataFourierIm->SetMarkerSize(fData[0].dataFourierIm->GetMarkerSize());
    fDataAvg.dataFourierIm->SetMarkerStyle(fData[0].dataFourierIm->GetMarkerStyle());
  }
  if (fDataAvg.dataFourierPwr != nullptr) {
    for (Int_t i=0; i<fData[0].dataFourierPwr->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].dataFourierPwr, fData[0].dataFourierPwr->GetBinCenter(i));
      }
      fDataAvg.dataFourierPwr->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.dataFourierPwr->SetMarkerColor(fData[0].dataFourierPwr->GetMarkerColor());
    fDataAvg.dataFourierPwr->SetLineColor(fData[0].dataFourierPwr->GetLineColor());
    fDataAvg.dataFourierPwr->SetMarkerSize(fData[0].dataFourierPwr->GetMarkerSize());
    fDataAvg.dataFourierPwr->SetMarkerStyle(fData[0].dataFourierPwr->GetMarkerStyle());
  }
  if (fDataAvg.dataFourierPhase != nullptr) {
    for (Int_t i=0; i<fData[0].dataFourierPhase->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].dataFourierPhase, fData[0].dataFourierPhase->GetBinCenter(i));
      }
      fDataAvg.dataFourierPhase->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.dataFourierPhase->SetMarkerColor(fData[0].dataFourierPhase->GetMarkerColor());
    fDataAvg.dataFourierPhase->SetLineColor(fData[0].dataFourierPhase->GetLineColor());
    fDataAvg.dataFourierPhase->SetMarkerSize(fData[0].dataFourierPhase->GetMarkerSize());
    fDataAvg.dataFourierPhase->SetMarkerStyle(fData[0].dataFourierPhase->GetMarkerStyle());
  }
  if (fDataAvg.dataFourierPhaseOptReal != nullptr) {
    for (Int_t i=0; i<fData[0].dataFourierPhaseOptReal->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].dataFourierPhaseOptReal, fData[0].dataFourierPhaseOptReal->GetBinCenter(i));
      }
      fDataAvg.dataFourierPhaseOptReal->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.dataFourierPhaseOptReal->SetMarkerColor(fData[0].dataFourierPhaseOptReal->GetMarkerColor());
    fDataAvg.dataFourierPhaseOptReal->SetLineColor(fData[0].dataFourierPhaseOptReal->GetLineColor());
    fDataAvg.dataFourierPhaseOptReal->SetMarkerSize(fData[0].dataFourierPhaseOptReal->GetMarkerSize());
    fDataAvg.dataFourierPhaseOptReal->SetMarkerStyle(fData[0].dataFourierPhaseOptReal->GetMarkerStyle());
  }
  if (fDataAvg.theory != nullptr) {
    for (Int_t i=0; i<fData[0].theory->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].theory, fData[0].theory->GetBinCenter(i));
      }
      fDataAvg.theory->SetBinContent(i, dval/fData.size());
    }
    fDataAvg.theory->SetLineColor(fData[0].theory->GetLineColor());
  }
  if (fDataAvg.theoryFourierRe != nullptr) {
    for (Int_t i=0; i<fData[0].theoryFourierRe->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].theoryFourierRe, fData[0].theoryFourierRe->GetBinCenter(i));
      }
      fDataAvg.theoryFourierRe->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.theoryFourierRe->SetMarkerColor(fData[0].theoryFourierRe->GetMarkerColor());
    fDataAvg.theoryFourierRe->SetLineColor(fData[0].theoryFourierRe->GetLineColor());
    fDataAvg.theoryFourierRe->SetMarkerSize(fData[0].theoryFourierRe->GetMarkerSize());
    fDataAvg.theoryFourierRe->SetMarkerStyle(fData[0].theoryFourierRe->GetMarkerStyle());
  }
  if (fDataAvg.theoryFourierIm != nullptr) {
    for (Int_t i=0; i<fData[0].theoryFourierIm->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].theoryFourierIm, fData[0].theoryFourierIm->GetBinCenter(i));
      }
      fDataAvg.theoryFourierIm->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.theoryFourierIm->SetMarkerColor(fData[0].theoryFourierIm->GetMarkerColor());
    fDataAvg.theoryFourierIm->SetLineColor(fData[0].theoryFourierIm->GetLineColor());
    fDataAvg.theoryFourierIm->SetMarkerSize(fData[0].theoryFourierIm->GetMarkerSize());
    fDataAvg.theoryFourierIm->SetMarkerStyle(fData[0].theoryFourierIm->GetMarkerStyle());
  }
  if (fDataAvg.theoryFourierPwr != nullptr) {
    for (Int_t i=0; i<fData[0].theoryFourierPwr->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].theoryFourierPwr, fData[0].theoryFourierPwr->GetBinCenter(i));
      }
      fDataAvg.theoryFourierPwr->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.theoryFourierPwr->SetMarkerColor(fData[0].theoryFourierPwr->GetMarkerColor());
    fDataAvg.theoryFourierPwr->SetLineColor(fData[0].theoryFourierPwr->GetLineColor());
    fDataAvg.theoryFourierPwr->SetMarkerSize(fData[0].theoryFourierPwr->GetMarkerSize());
    fDataAvg.theoryFourierPwr->SetMarkerStyle(fData[0].theoryFourierPwr->GetMarkerStyle());
  }
  if (fDataAvg.theoryFourierPhase != nullptr) {
    for (Int_t i=0; i<fData[0].theoryFourierPhase->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].theoryFourierPhase, fData[0].theoryFourierPhase->GetBinCenter(i));
      }
      fDataAvg.theoryFourierPhase->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.theoryFourierPhase->SetMarkerColor(fData[0].theoryFourierPhase->GetMarkerColor());
    fDataAvg.theoryFourierPhase->SetLineColor(fData[0].theoryFourierPhase->GetLineColor());
    fDataAvg.theoryFourierPhase->SetMarkerSize(fData[0].theoryFourierPhase->GetMarkerSize());
    fDataAvg.theoryFourierPhase->SetMarkerStyle(fData[0].theoryFourierPhase->GetMarkerStyle());
  }
  if (fDataAvg.theoryFourierPhaseOptReal != nullptr) {
    for (Int_t i=0; i<fData[0].theoryFourierPhaseOptReal->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].theoryFourierPhaseOptReal, fData[0].theoryFourierPhaseOptReal->GetBinCenter(i));
      }
      fDataAvg.theoryFourierPhaseOptReal->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.theoryFourierPhaseOptReal->SetMarkerColor(fData[0].theoryFourierPhaseOptReal->GetMarkerColor());
    fDataAvg.theoryFourierPhaseOptReal->SetLineColor(fData[0].theoryFourierPhaseOptReal->GetLineColor());
    fDataAvg.theoryFourierPhaseOptReal->SetMarkerSize(fData[0].theoryFourierPhaseOptReal->GetMarkerSize());
    fDataAvg.theoryFourierPhaseOptReal->SetMarkerStyle(fData[0].theoryFourierPhaseOptReal->GetMarkerStyle());
  }
  if (fDataAvg.diff != nullptr) {
    for (Int_t i=0; i<fData[0].diff->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].diff, fData[0].diff->GetBinCenter(i));
      }
      fDataAvg.diff->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.diff->SetMarkerColor(fData[0].diff->GetMarkerColor());
    fDataAvg.diff->SetLineColor(fData[0].diff->GetLineColor());
    fDataAvg.diff->SetMarkerSize(fData[0].diff->GetMarkerSize());
    fDataAvg.diff->SetMarkerStyle(fData[0].diff->GetMarkerStyle());
  }
  if (fDataAvg.diffFourierRe != nullptr) {
    for (Int_t i=0; i<fData[0].diffFourierRe->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].diffFourierRe, fData[0].diffFourierRe->GetBinCenter(i));
      }
      fDataAvg.diffFourierRe->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.diffFourierRe->SetMarkerColor(fData[0].diffFourierRe->GetMarkerColor());
    fDataAvg.diffFourierRe->SetLineColor(fData[0].diffFourierRe->GetLineColor());
    fDataAvg.diffFourierRe->SetMarkerSize(fData[0].diffFourierRe->GetMarkerSize());
    fDataAvg.diffFourierRe->SetMarkerStyle(fData[0].diffFourierRe->GetMarkerStyle());
  }
  if (fDataAvg.diffFourierIm != nullptr) {
    for (Int_t i=0; i<fData[0].diffFourierIm->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].diffFourierIm, fData[0].diffFourierIm->GetBinCenter(i));
      }
      fDataAvg.diffFourierIm->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.diffFourierIm->SetMarkerColor(fData[0].diffFourierIm->GetMarkerColor());
    fDataAvg.diffFourierIm->SetLineColor(fData[0].diffFourierIm->GetLineColor());
    fDataAvg.diffFourierIm->SetMarkerSize(fData[0].diffFourierIm->GetMarkerSize());
    fDataAvg.diffFourierIm->SetMarkerStyle(fData[0].diffFourierIm->GetMarkerStyle());
  }
  if (fDataAvg.diffFourierPwr != nullptr) {
    for (Int_t i=0; i<fData[0].diffFourierPwr->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].diffFourierPwr, fData[0].diffFourierPwr->GetBinCenter(i));
      }
      fDataAvg.diffFourierPwr->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.diffFourierPwr->SetMarkerColor(fData[0].diffFourierPwr->GetMarkerColor());
    fDataAvg.diffFourierPwr->SetLineColor(fData[0].diffFourierPwr->GetLineColor());
    fDataAvg.diffFourierPwr->SetMarkerSize(fData[0].diffFourierPwr->GetMarkerSize());
    fDataAvg.diffFourierPwr->SetMarkerStyle(fData[0].diffFourierPwr->GetMarkerStyle());
  }
  if (fDataAvg.diffFourierPhase != nullptr) {
    for (Int_t i=0; i<fData[0].diffFourierPhase->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].diffFourierPhase, fData[0].diffFourierPhase->GetBinCenter(i));
      }
      fDataAvg.diffFourierPhase->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.diffFourierPhase->SetMarkerColor(fData[0].dataFourierPhase->GetMarkerColor());
    fDataAvg.diffFourierPhase->SetLineColor(fData[0].dataFourierPhase->GetLineColor());
    fDataAvg.diffFourierPhase->SetMarkerSize(fData[0].dataFourierPhase->GetMarkerSize());
    fDataAvg.diffFourierPhase->SetMarkerStyle(fData[0].dataFourierPhase->GetMarkerStyle());
  }
  if (fDataAvg.diffFourierPhaseOptReal != nullptr) {
    for (Int_t i=0; i<fData[0].diffFourierPhaseOptReal->GetNbinsX(); i++) {
      dval = 0.0;
      for (UInt_t j=0; j<fData.size(); j++) {
        dval += GetInterpolatedValue(fData[j].diffFourierPhaseOptReal, fData[0].diffFourierPhaseOptReal->GetBinCenter(i));
      }
      fDataAvg.diffFourierPhaseOptReal->SetBinContent(i, dval/fData.size());
    }
    // set marker color, line color, maker size, marker type
    fDataAvg.diffFourierPhaseOptReal->SetMarkerColor(fData[0].dataFourierPhaseOptReal->GetMarkerColor());
    fDataAvg.diffFourierPhaseOptReal->SetLineColor(fData[0].dataFourierPhaseOptReal->GetLineColor());
    fDataAvg.diffFourierPhaseOptReal->SetMarkerSize(fData[0].dataFourierPhaseOptReal->GetMarkerSize());
    fDataAvg.diffFourierPhaseOptReal->SetMarkerStyle(fData[0].dataFourierPhaseOptReal->GetMarkerStyle());
  }
}

//--------------------------------------------------------------------------
// CleanupDifference (private)
//--------------------------------------------------------------------------
/**
 * <p>Cleans up (deallocate) difference data.
 */
void PMusrCanvas::CleanupDifference()
{
  for (UInt_t i=0; i<fData.size(); i++) {
    if (fData[i].diff != nullptr) {
      delete fData[i].diff;
      fData[i].diff = nullptr;
    }
  }
}

//--------------------------------------------------------------------------
// CleanupFourier (private)
//--------------------------------------------------------------------------
/**
 * <p>Cleans up (deallocate) Fourier transform data.
 */
void PMusrCanvas::CleanupFourier()
{
  for (UInt_t i=0; i<fData.size(); i++) {
    if (fData[i].dataFourierRe != nullptr) {
      delete fData[i].dataFourierRe;
      fData[i].dataFourierRe = nullptr;
    }
    if (fData[i].dataFourierIm != nullptr) {
      delete fData[i].dataFourierIm;
      fData[i].dataFourierIm = nullptr;
    }
    if (fData[i].dataFourierPwr != nullptr) {
      delete fData[i].dataFourierPwr;
      fData[i].dataFourierPwr = nullptr;
    }
    if (fData[i].dataFourierPhase != nullptr) {
      delete fData[i].dataFourierPhase;
      fData[i].dataFourierPhase = nullptr;
    }
    if (fData[i].dataFourierPhaseOptReal != nullptr) {
      delete fData[i].dataFourierPhaseOptReal;
      fData[i].dataFourierPhaseOptReal = nullptr;
    }
    if (fData[i].theoryFourierRe != nullptr) {
      delete fData[i].theoryFourierRe;
      fData[i].theoryFourierRe = nullptr;
    }
    if (fData[i].theoryFourierIm != nullptr) {
      delete fData[i].theoryFourierIm;
      fData[i].theoryFourierIm = nullptr;
    }
    if (fData[i].theoryFourierPwr != nullptr) {
      delete fData[i].theoryFourierPwr;
      fData[i].theoryFourierPwr = nullptr;
    }
    if (fData[i].theoryFourierPhase != nullptr) {
      delete fData[i].theoryFourierPhase;
      fData[i].theoryFourierPhase = nullptr;
    }
    if (fData[i].theoryFourierPhaseOptReal != nullptr) {
      delete fData[i].theoryFourierPhaseOptReal;
      fData[i].theoryFourierPhaseOptReal = nullptr;
    }
  }
}

//--------------------------------------------------------------------------
// CleanupFourierDifference (private)
//--------------------------------------------------------------------------
/**
 * <p>Cleans up (deallocate) Fourier difference spectra.
 */
void PMusrCanvas::CleanupFourierDifference()
{
  for (UInt_t i=0; i<fData.size(); i++) {
    if (fData[i].diffFourierRe != nullptr) {
      delete fData[i].diffFourierRe;
      fData[i].diffFourierRe = nullptr;
    }
    if (fData[i].diffFourierIm != nullptr) {
      delete fData[i].diffFourierIm;
      fData[i].diffFourierIm = nullptr;
    }
    if (fData[i].diffFourierPwr != nullptr) {
      delete fData[i].diffFourierPwr;
      fData[i].diffFourierPwr = nullptr;
    }
    if (fData[i].diffFourierPhase != nullptr) {
      delete fData[i].diffFourierPhase;
      fData[i].diffFourierPhase = nullptr;
    }
    if (fData[i].diffFourierPhaseOptReal != nullptr) {
      delete fData[i].diffFourierPhaseOptReal;
      fData[i].diffFourierPhaseOptReal = nullptr;
    }
  }
}

//--------------------------------------------------------------------------
// CleanupAverage (private)
//--------------------------------------------------------------------------
/**
 * <p>Cleans up (deallocate) averaged data set.
 */
void PMusrCanvas::CleanupAverage()
{
  if (fDataAvg.data != nullptr) {
    delete fDataAvg.data;
    fDataAvg.data = nullptr;
  }
  if (fDataAvg.dataFourierRe != nullptr) {
    delete fDataAvg.dataFourierRe;
    fDataAvg.dataFourierRe = nullptr;
  }
  if (fDataAvg.dataFourierIm != nullptr) {
    delete fDataAvg.dataFourierIm;
    fDataAvg.dataFourierIm = nullptr;
  }
  if (fDataAvg.dataFourierPwr != nullptr) {
    delete fDataAvg.dataFourierPwr;
    fDataAvg.dataFourierPwr = nullptr;
  }
  if (fDataAvg.dataFourierPhase != nullptr) {
    delete fDataAvg.dataFourierPhase;
    fDataAvg.dataFourierPhase = nullptr;
  }
  if (fDataAvg.dataFourierPhaseOptReal != nullptr) {
    delete fDataAvg.dataFourierPhaseOptReal;
    fDataAvg.dataFourierPhaseOptReal = nullptr;
  }
  if (fDataAvg.theory != nullptr) {
    delete fDataAvg.theory;
    fDataAvg.theory = nullptr;
  }
  if (fDataAvg.theoryFourierRe != nullptr) {
    delete fDataAvg.theoryFourierRe;
    fDataAvg.theoryFourierRe = nullptr;
  }
  if (fDataAvg.theoryFourierIm != nullptr) {
    delete fDataAvg.theoryFourierIm;
    fDataAvg.theoryFourierIm = nullptr;
  }
  if (fDataAvg.theoryFourierPwr != nullptr) {
    delete fDataAvg.theoryFourierPwr;
    fDataAvg.theoryFourierPwr = nullptr;
  }
  if (fDataAvg.theoryFourierPhase != nullptr) {
    delete fDataAvg.theoryFourierPhase;
    fDataAvg.theoryFourierPhase = nullptr;
  }
  if (fDataAvg.theoryFourierPhaseOptReal != nullptr) {
    delete fDataAvg.theoryFourierPhaseOptReal;
    fDataAvg.theoryFourierPhaseOptReal = nullptr;
  }
  if (fDataAvg.diff != nullptr) {
    delete fDataAvg.diff;
    fDataAvg.diff = nullptr;
  }
  if (fDataAvg.diffFourierRe != nullptr) {
    delete fDataAvg.diffFourierRe;
    fDataAvg.diffFourierRe = nullptr;
  }
  if (fDataAvg.diffFourierIm != nullptr) {
    delete fDataAvg.diffFourierIm;
    fDataAvg.diffFourierIm = nullptr;
  }
  if (fDataAvg.diffFourierPwr != nullptr) {
    delete fDataAvg.diffFourierPwr;
    fDataAvg.diffFourierPwr = nullptr;
  }
  if (fDataAvg.diffFourierPhase != nullptr) {
    delete fDataAvg.diffFourierPhase;
    fDataAvg.diffFourierPhase = nullptr;
  }
  if (fDataAvg.diffFourierPhaseOptReal != nullptr) {
    delete fDataAvg.diffFourierPhaseOptReal;
    fDataAvg.diffFourierPhaseOptReal = nullptr;
  }
}

//--------------------------------------------------------------------------
// CalculateDiff (private)
//--------------------------------------------------------------------------
/**
 * @brief PMusrCanvas::CalcPhaseOptReFT
 */
void PMusrCanvas::CalcPhaseOptReFT()
{
  Double_t min = fMsrHandler->GetMsrFourierList()->fRangeForPhaseCorrection[0];
  Double_t max = fMsrHandler->GetMsrFourierList()->fRangeForPhaseCorrection[1];

  if ((min == -1.0) && (max == -1.0)) {
    if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
      min = fFourier.fPlotRange[0];
      max = fFourier.fPlotRange[1];
    } else {
      min = fData[0].dataFourierRe->GetBinLowEdge(1);
      max = fData[0].dataFourierRe->GetBinLowEdge(fData[0].dataFourierRe->GetNbinsX())+fData[0].dataFourierRe->GetBinWidth(1);
    }
  }

  PDoubleVector phaseParam;
  Char_t hName[1024];
  Double_t ph, re;

  for (UInt_t i=0; i<fData.size(); i++) {
    // handle Fourier data part
    fData[i].dataFourierPhaseOptReal = PFourier::GetPhaseOptRealFourier(fData[i].dataFourierRe, fData[i].dataFourierIm,
                                                                        phaseParam, 1.0, min, max);
    // set marker and line color
    fData[i].dataFourierPhaseOptReal->SetMarkerColor(fData[i].data->GetMarkerColor());
    fData[i].dataFourierPhaseOptReal->SetLineColor(fData[i].data->GetLineColor());
    // set marker size
    fData[i].dataFourierPhaseOptReal->SetMarkerSize(1);
    // set marker type
    fData[i].dataFourierPhaseOptReal->SetMarkerStyle(fData[i].data->GetMarkerStyle());

    // handle Fourier theory part
    // clone theory Re FT
    strcpy(hName, fData[i].theoryFourierPhase->GetName());
    strcat(hName, "_Opt_Real");
    fData[i].theoryFourierPhaseOptReal = (TH1F*) fData[i].theoryFourierRe->Clone(hName);

    // rotate the theory according to the optimized phase parameters
    // first find minBin for min of the phase correction
    Int_t minBin = fData[i].theoryFourierPhaseOptReal->GetXaxis()->FindFixBin(min);
    Int_t maxBin = fData[i].theoryFourierPhaseOptReal->GetXaxis()->FindFixBin(max);

    for (Int_t j=1; j<fData[i].theoryFourierPhaseOptReal->GetNbinsX(); j++) {
      ph = phaseParam[0] + phaseParam[1] * (Double_t)(j-minBin+1) / (Double_t)(maxBin-minBin);
      re = fData[i].theoryFourierRe->GetBinContent(j) * cos(ph) - fData[i].theoryFourierIm->GetBinContent(j) * sin(ph);
      fData[i].theoryFourierPhaseOptReal->SetBinContent(j, re);
    }
    // set line colors for the theory
    fData[i].theoryFourierPhaseOptReal->SetLineColor(fData[i].theory->GetLineColor());
  }
}

//--------------------------------------------------------------------------
// CalculateDiff (private)
//--------------------------------------------------------------------------
/**
 * <p>Calculates the difference between data and theory for histograms.
 *
 * <b>return:</b>
 * - (data - theory) value
 *
 * \param x x-value of the data
 * \param y y-value of the data
 * \param theo theory histogram
 */
double PMusrCanvas::CalculateDiff(const Double_t x, const Double_t y, TH1F *theo)
{
  Int_t bin = theo->FindBin(x);

  return y - theo->GetBinContent(bin);
}

//--------------------------------------------------------------------------
// CalculateDiff (private)
//--------------------------------------------------------------------------
/**
 * <p>Calculates the difference between data and theory for error graphs.
 *
 * <b>return:</b>
 * - (data - theory) value
 *
 * \param x x-value of the data
 * \param y y-value of the data
 * \param theo theory error graphs
 */
double PMusrCanvas::CalculateDiff(const Double_t x, const Double_t y, TGraphErrors *theo)
{
  Int_t bin = 0;
  Double_t xVal, yVal;

  bin = FindBin(x, theo);

  theo->GetPoint(bin, xVal, yVal);

  return y - yVal;
}

//--------------------------------------------------------------------------
// FindBin (private)
//--------------------------------------------------------------------------
/**
 * <p>Analog to FindBin for histograms (TH1F) but here for TGraphErrors.
 *
 * <b>return:</b>
 * - bin closest to a given x value.
 *
 * \param x x-value of the data
 * \param graph TGraphErrors which should be searched
 */
Int_t PMusrCanvas::FindBin(const Double_t x, TGraphErrors *graph)
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
Double_t PMusrCanvas::GetMaximum(TH1F* histo, Double_t xmin, Double_t xmax)
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
Double_t PMusrCanvas::GetMinimum(TH1F* histo, Double_t xmin, Double_t xmax)
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
// GetMaximum (private)
//--------------------------------------------------------------------------
/**
 * <p>returns the maximum of a TGraphErrors object in the range [xmin, xmax].
 * If xmin = xmax = -1.0, the whole histogram range is used.
 *
 * <b>return:</b>
 * - maximum, or 0.0 if the histo pointer is the null pointer.
 *
 * \param graph pointer of the histogram
 * \param xmin lower edge for the search interval.
 * \param xmax upper edge for the search interval.
 */
Double_t PMusrCanvas::GetMaximum(TGraphErrors* graph, Double_t xmin, Double_t xmax)
{
  if (graph == nullptr)
    return 0.0;

  Double_t x, y;
  if (xmin == xmax) {
    graph->GetPoint(0, x, y);
    xmin = x;
    graph->GetPoint(graph->GetN()-1, x, y);
    xmax = x;
  }

  graph->GetPoint(0, x, y);
  Double_t max = y;
  for (Int_t i=0; i<graph->GetN(); i++) {
    graph->GetPoint(i, x, y);
    if ((x >= xmin) && (x <= xmax)) {
      if (y > max)
        max = y;
    }
  }

  return max;
}

//--------------------------------------------------------------------------
// GetMinimum (private)
//--------------------------------------------------------------------------
/**
 * <p>returns the minimum of a TGraphErrors object in the range [xmin, xmax].
 * If xmin = xmax = -1.0, the whole histogram range is used.
 *
 * <b>return:</b>
 * - minimum, or 0.0 if the histo pointer is the null pointer.
 *
 * \param graph pointer of the histogram
 * \param xmin lower edge for the search interval.
 * \param xmax upper edge for the search interval.
 */
Double_t PMusrCanvas::GetMinimum(TGraphErrors* graph, Double_t xmin, Double_t xmax)
{
  if (graph == nullptr)
    return 0.0;

  Double_t x, y;
  if (xmin == xmax) {
    graph->GetPoint(0, x, y);
    xmin = x;
    graph->GetPoint(graph->GetN()-1, x, y);
    xmax = x;
  }

  graph->GetPoint(0, x, y);
  Double_t min = y;
  for (Int_t i=0; i<graph->GetN(); i++) {
    graph->GetPoint(i, x, y);
    if ((x >= xmin) && (x <= xmax)) {
      if (y < min)
        min = y;
    }
  }

  return min;
}

//--------------------------------------------------------------------------
// PlotData (private)
//--------------------------------------------------------------------------
/**
 * <p>Plots the data.
 *
 * \param unzoom if true, rescale to the original msr-file ranges
 */
void PMusrCanvas::PlotData(Bool_t unzoom)
{
  fDataTheoryPad->cd();

  if (!fBatchMode) {
    // uncheck fourier menu entries
    fPopupFourier->UnCheckEntries();
  }

  if (fPlotType < 0) // plot type not defined
    return;

  Double_t xmin, xmax;
  if (fPlotType != MSR_PLOT_NON_MUSR) {
    if (fData.size() > 0) {

      // keep the current x-axis range from the data view
      if (fHistoFrame && (fPreviousPlotView == PV_DATA)) {
        xmin = fHistoFrame->GetXaxis()->GetBinLowEdge(fHistoFrame->GetXaxis()->GetFirst());
        xmax = fHistoFrame->GetXaxis()->GetBinLowEdge(fHistoFrame->GetXaxis()->GetLast()) + fHistoFrame->GetXaxis()->GetBinWidth(fHistoFrame->GetXaxis()->GetLast());
      } else {
        xmin = fXmin;
        xmax = fXmax;
      }

      // delete old fHistoFrame if present
      if (fHistoFrame) {
        delete fHistoFrame;
        fHistoFrame = nullptr;
      }

      // get the histo frame x/y range boundaries
      Double_t dataXmin=0.0, dataXmax=0.0, dataYmin=0.0, dataYmax=0.0;
      if (unzoom) { // set the x-/y-range back to the original msr-file values
        dataXmin = fXmin;
        dataXmax = fXmax;
        if (fYRangePresent) {
          dataYmin = fYmin;
          dataYmax = fYmax;
        } else {
          dataYmin = GetMinimum(fData[0].data, dataXmin, dataXmax);
          dataYmax = GetMaximum(fData[0].data, dataXmin, dataXmax);
          for (UInt_t i=1; i<fData.size(); i++) {
            if (GetMinimum(fData[i].data, dataXmin, dataXmax) < dataYmin)
              dataYmin = GetMinimum(fData[i].data, dataXmin, dataXmax);
            if (GetMaximum(fData[i].data, dataXmin, dataXmax) > dataYmax)
              dataYmax = GetMaximum(fData[i].data, dataXmin, dataXmax);
          }
          Double_t dd = 0.05*fabs(dataYmax-dataYmin);
          dataYmin -= dd;
          dataYmax += dd;
        }
      } else { // set the x-/y-range to the previous fHistoFrame range
        dataXmin = xmin;
        dataXmax = xmax;
        if (fYRangePresent) { // explicit y-range present
          dataYmin = fYmin;
          dataYmax = fYmax;
        } else { // extract global min/max in order to have the proper y-range
          dataYmin = GetMinimum(fData[0].data, dataXmin, dataXmax);
          dataYmax = GetMaximum(fData[0].data, dataXmin, dataXmax);
          for (UInt_t i=1; i<fData.size(); i++) {
            if (GetMinimum(fData[i].data, dataXmin, dataXmax) < dataYmin)
              dataYmin = GetMinimum(fData[i].data, dataXmin, dataXmax);
            if (GetMaximum(fData[i].data, dataXmin, dataXmax) > dataYmax)
              dataYmax = GetMaximum(fData[i].data, dataXmin, dataXmax);
          }
          Double_t dd = 0.05*fabs(dataYmax-dataYmin);
          dataYmin -= dd;
          dataYmax += dd;
        }
      }
      if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fLogY) {
        dataYmin = 1.0e-4 * dataYmax;
      }

      // create histo frame in order to plot histograms possibly with different x-frames
      fHistoFrame = fDataTheoryPad->DrawFrame(dataXmin, dataYmin, dataXmax, dataYmax);

      // find the maximal number of points present in the histograms and increase the default number of points of fHistoFrame (1000) to the needed one
      UInt_t noOfPoints = 1000;
      for (UInt_t i=0; i<fData.size(); i++) {
        if (fData[i].data->GetNbinsX() > (Int_t)noOfPoints)
          noOfPoints = fData[i].data->GetNbinsX();
      }
      noOfPoints *= 2;  // make sure that there are enough points
      fHistoFrame->SetBins(noOfPoints, dataXmin, dataXmax);

      // set all histo/theory ranges properly
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].data->GetXaxis()->SetRange(fData[i].data->FindBin(dataXmin), fData[i].data->FindBin(dataXmax));
        fData[i].data->GetYaxis()->SetRangeUser(dataYmin, dataYmax);
        fData[i].theory->GetXaxis()->SetRange(fData[i].theory->FindBin(dataXmin), fData[i].theory->FindBin(dataXmax));
        fData[i].theory->GetYaxis()->SetRangeUser(dataYmin, dataYmax);
      }

      if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fLogX)
        fDataTheoryPad->SetLogx(1);
      if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fLogY)
        fDataTheoryPad->SetLogy(1);

      // set x-axis label
      PMsrRunList runs = *fMsrHandler->GetMsrRunList();
      TString setup = fRunList->GetSetup(*runs[0].GetRunName());
      if (strcmp(setup, "TRIUMF/BNQR") || strcmp(setup, "TRIUMF/BNMR")) {
	fHistoFrame->GetXaxis()->SetTitle("time (s)");
      } else {
	fHistoFrame->GetXaxis()->SetTitle("time (#mus)");
      }

      // set y-axis label
      TString yAxisTitle;
      PMsrRunList *runList = fMsrHandler->GetMsrRunList();
      switch (fPlotType) {
        case MSR_PLOT_SINGLE_HISTO:
          if (runList->at(0).IsLifetimeCorrected()) { // lifetime correction
            yAxisTitle = "Asymmetry";
          } else { // no liftime correction
            if (fScaleN0AndBkg)
              yAxisTitle = "N(t) per nsec";
            else
              yAxisTitle = "N(t) per bin";
          }
          break;
        case MSR_PLOT_SINGLE_HISTO_RRF:
        case MSR_PLOT_ASYM_RRF:
          yAxisTitle = "RRF Asymmetry";
          break;
        case MSR_PLOT_ASYM:
          yAxisTitle = "Asymmetry";
          break;
        case MSR_PLOT_BNMR:
          yAxisTitle = "Asymmetry";
          break;
        case MSR_PLOT_MU_MINUS:
          yAxisTitle = "N(t) per bin";
          break;
        default:
          yAxisTitle = "??";
          break;
      }
      fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
      fHistoFrame->GetYaxis()->SetTitle(yAxisTitle.Data());
      // plot all data
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].data->Draw("pesame");
      }
      // plot all the theory
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].theory->Draw("lsame");
      }
    }

    // check if RRF and if yes show a label
    if ((fRRFText != nullptr) && (fRRFLatexText != nullptr)) {
      fRRFLatexText->DrawLatex(0.1, 0.92, fRRFText->Data());
    }
  } else { // fPlotType == MSR_PLOT_NO_MUSR
    // keep the current x-axis range from the data view
    if (fMultiGraphDiff && (fPreviousPlotView == PV_DATA)) {
      xmin = fMultiGraphDiff->GetXaxis()->GetBinCenter(fMultiGraphDiff->GetXaxis()->GetFirst());
      xmax = fMultiGraphDiff->GetXaxis()->GetBinCenter(fMultiGraphDiff->GetXaxis()->GetLast());
    } else {
      xmin = fXmin;
      xmax = fXmax;
    }

    // tell the canvas that the selected object (the one under the mouse pointer) is not your object, before to actually delete it.
    fMainCanvas->SetSelected(fMainCanvas->GetPadSave());

    // cleanup if previous fMultiGraphData is present
    if (fMultiGraphData) {
      delete fMultiGraphData;
      fMultiGraphData = nullptr;
    }
    if (fMultiGraphDiff) {
      delete fMultiGraphDiff;
      fMultiGraphDiff = nullptr;
    }

    PMsrRunList runs = *fMsrHandler->GetMsrRunList();
    PMsrPlotStructure plotInfo = fMsrHandler->GetMsrPlotList()->at(fPlotNumber);
    UInt_t runNo = (UInt_t)plotInfo.fRuns[0]-1;
    TString xAxisTitle = fRunList->GetXAxisTitle(*runs[runNo].GetRunName(), runNo);
    TString yAxisTitle = fRunList->GetYAxisTitle(*runs[runNo].GetRunName(), runNo);

    if (fNonMusrData.size() > 0) {

      // get the histo frame x/y range boundaries
      Double_t dataXmin=0.0, dataXmax=0.0, dataYmin=0.0, dataYmax=0.0;
      if (unzoom) { // set the x-/y-range back to the original msr-file values
        dataXmin = fXmin;
        dataXmax = fXmax;
        if (fYRangePresent) {
          dataYmin = fYmin;
          dataYmax = fYmax;
        } else {
          dataYmin = GetMinimum(fNonMusrData[0].data, dataXmin, dataXmax);
          dataYmax = GetMaximum(fNonMusrData[0].data, dataXmin, dataXmax);
          for (UInt_t i=1; i<fNonMusrData.size(); i++) {
            if (GetMinimum(fNonMusrData[i].data, dataXmin, dataXmax) < dataYmin)
              dataYmin = GetMinimum(fNonMusrData[i].data, dataXmin, dataXmax);
            if (GetMaximum(fNonMusrData[i].data, dataXmin, dataXmax) > dataYmax)
              dataYmax = GetMaximum(fNonMusrData[i].data, dataXmin, dataXmax);
          }
          Double_t dd = 0.05*fabs(dataYmax-dataYmin);
          dataYmin -= dd;
          dataYmax += dd;
        }
      } else { // set the x-/y-range to the previous fHistoFrame range
        dataXmin = xmin;
        dataXmax = xmax;
        if (fYRangePresent) { // explicit y-range present
          dataYmin = fYmin;
          dataYmax = fYmax;
        } else { // extract global min/max in order to have the proper y-range
          dataYmin = GetMinimum(fNonMusrData[0].data, dataXmin, dataXmax);
          dataYmax = GetMaximum(fNonMusrData[0].data, dataXmin, dataXmax);
          for (UInt_t i=1; i<fNonMusrData.size(); i++) {
            if (GetMinimum(fNonMusrData[i].data, dataXmin, dataXmax) < dataYmin)
              dataYmin = GetMinimum(fNonMusrData[i].data, dataXmin, dataXmax);
            if (GetMaximum(fNonMusrData[i].data, dataXmin, dataXmax) > dataYmax)
              dataYmax = GetMaximum(fNonMusrData[i].data, dataXmin, dataXmax);
          }
          Double_t dd = 0.05*fabs(dataYmax-dataYmin);
          dataYmin -= dd;
          dataYmax += dd;
        }
      }
      if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fLogY) {
        if (dataYmin <= 0.0)
          dataYmin = 1.0e-4 * dataYmax;
      }

      // create fMultiGraphData, and add all data and theory
      fMultiGraphData = new TMultiGraph();
      assert(fMultiGraphData != nullptr);

      // add all data to fMultiGraphData
      for (UInt_t i=0; i<fNonMusrData.size(); i++) {
        // the next three lines are ugly but needed for the following reasons:
        // TMultiGraph is taking ownership of the TGraphErrors, hence a deep copy is needed.
        // This is not resulting in a memory leak, since the TMultiGraph object will do the cleanup
        TGraphErrors *ge = new TGraphErrors(*(fNonMusrData[i].data));
        // Data points and model curves should be fixed on the graph and not dragged around using, e.g., the mouse.
        ge->SetEditable(false);
        fMultiGraphData->Add(ge, "p");
      }
      // add all the theory to fMultiGraphData
      for (UInt_t i=0; i<fNonMusrData.size(); i++) {
        // the next three lines are ugly but needed for the following reasons:
        // TMultiGraph is taking ownership of the TGraphErrors, hence a deep copy is needed.
        // This is not resulting in a memory leak, since the TMultiGraph object will do the cleanup
        TGraphErrors *ge = new TGraphErrors(*(fNonMusrData[i].theory));
        // Data points and model curves should be fixed on the graph and not dragged around using, e.g., the mouse.
        ge->SetEditable(false);
        fMultiGraphData->Add(ge, "l");
      }

      if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fLogX)
        fDataTheoryPad->SetLogx(1);
      if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fLogY)
        fDataTheoryPad->SetLogy(1);

      fMultiGraphData->Draw("a");

      // set x/y-range
      fMultiGraphData->GetXaxis()->SetRangeUser(dataXmin, dataXmax);
      fMultiGraphData->GetYaxis()->SetRangeUser(dataYmin, dataYmax);

      // set x-, y-axis label only if there is just one data set
      if (fNonMusrData.size() == 1) {
        // set x-axis label
        fMultiGraphData->GetXaxis()->SetTitle(xAxisTitle.Data());
        // set y-axis label
        fMultiGraphData->GetYaxis()->SetTitle(yAxisTitle.Data());
      } else { // more than one data set present, hence add a legend
        if (fMultiGraphLegend) {
          delete fMultiGraphLegend;
        }
        fMultiGraphLegend = new TLegend(0.8, 0.8, 1.0, 1.0);
        assert(fMultiGraphLegend != nullptr);
        PStringVector legendLabel;
        for (UInt_t i=0; i<plotInfo.fRuns.size(); i++) {
           runNo = (UInt_t)plotInfo.fRuns[i]-1;
           xAxisTitle = fRunList->GetXAxisTitle(*runs[runNo].GetRunName(), runNo);
           yAxisTitle = fRunList->GetYAxisTitle(*runs[runNo].GetRunName(), runNo);
           legendLabel.push_back(yAxisTitle + " vs. " + xAxisTitle);
        }
        for (UInt_t i=0; i<fNonMusrData.size(); i++) {
          fMultiGraphLegend->AddEntry(fNonMusrData[i].data, legendLabel[i].Data(), "p");
        }
        legendLabel.clear();
      }

      fMultiGraphData->Draw("a");

      if (fMultiGraphLegend)
        fMultiGraphLegend->Draw();
    }

    // report canvas status events in non-musr plots
    if (!fMainCanvas->GetShowEventStatus()) {
      fMainCanvas->ToggleEventStatus();
    }
  }

  fDataTheoryPad->Update();

  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// PlotDifference (private)
//--------------------------------------------------------------------------
/**
 * <p>Plots the difference data, i.e. data-theory
 *
 * \param unzoom if true, rescale to the original msr-file ranges
 */
void PMusrCanvas::PlotDifference(Bool_t unzoom)
{
  fDataTheoryPad->cd();

  // check if log scale plotting and if yes switch back to linear
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fLogY)
    fDataTheoryPad->SetLogy(0); // switch to linear
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fLogX)
    fDataTheoryPad->SetLogx(0); // switch to linear

  if (fPlotType < 0) // plot type not defined
    return;

  Double_t xmin, xmax;
  if (fPlotType != MSR_PLOT_NON_MUSR) {
    // keep the current x-axis range from the data view
    if (fHistoFrame && (fPreviousPlotView == PV_DATA)) {
      xmin = fHistoFrame->GetXaxis()->GetBinLowEdge(fHistoFrame->GetXaxis()->GetFirst());
      xmax = fHistoFrame->GetXaxis()->GetBinLowEdge(fHistoFrame->GetXaxis()->GetLast()) + fHistoFrame->GetXaxis()->GetBinWidth(fHistoFrame->GetXaxis()->GetLast());
    } else {
      xmin = fXmin;
      xmax = fXmax;
    }

    // delete old fHistoFrame if present
    if (fHistoFrame) {
      delete fHistoFrame;
      fHistoFrame = nullptr;
    }

    Double_t dataXmin=0.0, dataXmax=0.0, dataYmin=0.0, dataYmax=0.0, dd=0.0;
    if (unzoom) {
      dataXmin = fXmin;
      dataXmax = fXmax;
      dataYmin = GetMinimum(fData[0].diff, dataXmin, dataXmax);
      dataYmax = GetMaximum(fData[0].diff, dataXmin, dataXmax);
      for (UInt_t i=1; i<fData.size(); i++) {
        if (GetMinimum(fData[i].diff, dataXmin, dataXmax) < dataYmin)
          dataYmin = GetMinimum(fData[i].diff, dataXmin, dataXmax);
        if (GetMaximum(fData[i].diff, dataXmin, dataXmax) > dataYmax)
          dataYmax = GetMaximum(fData[i].diff, dataXmin, dataXmax);
      }
      // slightly increase y-range
      dd = 0.05*fabs(dataYmax-dataYmin);
      dataYmin -= dd;
      dataYmax += dd;
    } else {
      dataXmin = xmin;
      dataXmax = xmax;
      dataYmin = GetMinimum(fData[0].diff, dataXmin, dataXmax);
      dataYmax = GetMaximum(fData[0].diff, dataXmin, dataXmax);
      for (UInt_t i=1; i<fData.size(); i++) {
        if (GetMinimum(fData[i].diff, dataXmin, dataXmax) < dataYmin)
          dataYmin = GetMinimum(fData[i].diff, dataXmin, dataXmax);
        if (GetMaximum(fData[i].diff, dataXmin, dataXmax) > dataYmax)
          dataYmax = GetMaximum(fData[i].diff, dataXmin, dataXmax);
      }
      // slightly increase y-range
      dd = 0.05*fabs(dataYmax-dataYmin);
      dataYmin -= dd;
      dataYmax += dd;
    }

    fHistoFrame = fDataTheoryPad->DrawFrame(dataXmin, dataYmin, dataXmax, dataYmax);

    // find the maximal number of points present in the histograms and increase the default number of points of fHistoFrame (1000) to the needed one
    UInt_t noOfPoints = 1000;
    for (UInt_t i=0; i<fData.size(); i++) {
      if (fData[i].diff->GetNbinsX() > (Int_t)noOfPoints)
        noOfPoints = fData[i].diff->GetNbinsX();
    }
    noOfPoints *= 2; // make sure that there are enough points
    fHistoFrame->SetBins(noOfPoints, dataXmin, dataXmax);

    // set x-axis label
    PMsrRunList runs = *fMsrHandler->GetMsrRunList();
    TString setup = fRunList->GetSetup(*runs[0].GetRunName());
    if (strcmp(setup, "TRIUMF/BNQR") || strcmp(setup, "TRIUMF/BNMR")) {
      fHistoFrame->GetXaxis()->SetTitle("time (s)");
    } else {
    fHistoFrame->GetXaxis()->SetTitle("time (#mus)");
    }
    // set y-axis label
    fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
    fHistoFrame->GetYaxis()->SetTitle("data-theory");

    // plot all diff data
    for (UInt_t i=0; i<fData.size(); i++) {
      fData[i].diff->Draw("pesame");
      // set all diff ranges properly
      if (fData[i].dataRange->IsXRangePresent())
        fData[i].diff->GetXaxis()->SetRangeUser(fData[i].dataRange->GetXmin(), fData[i].dataRange->GetXmax());
      else
        fData[i].diff->GetXaxis()->SetRange(fData[i].diff->FindBin(dataXmin), fData[i].diff->FindBin(dataXmax));

      if (fData[i].dataRange->IsYRangePresent())
        fData[i].diff->GetYaxis()->SetRangeUser(fData[i].dataRange->GetYmin(), fData[i].dataRange->GetYmax());
      else
        fData[i].diff->GetYaxis()->SetRangeUser(dataYmin, dataYmax);
    }


    // check if RRF and if yes show a label
    if ((fRRFText != nullptr) && (fRRFLatexText != nullptr)) {
      fRRFLatexText->DrawLatex(0.1, 0.92, fRRFText->Data());
    }
  } else { // fPlotType == MSR_PLOT_NON_MUSR
    // keep the current x-axis range from the data view
    if (fMultiGraphData && (fPreviousPlotView == PV_DATA)) {
      xmin = fMultiGraphData->GetXaxis()->GetBinCenter(fMultiGraphData->GetXaxis()->GetFirst());
      xmax = fMultiGraphData->GetXaxis()->GetBinCenter(fMultiGraphData->GetXaxis()->GetLast());
    } else {
      xmin = fXmin;
      xmax = fXmax;
    }

    // tell the canvas that the selected object (the one under the mouse pointer) is not your object, before to actually delete it.
    fMainCanvas->SetSelected(fMainCanvas->GetPadSave());

    // clean up previous fMultiGraphDiff
    if (fMultiGraphDiff) {
      delete fMultiGraphDiff;
      fMultiGraphDiff = nullptr;
    }
    if (fMultiGraphData) {
      delete fMultiGraphData;
      fMultiGraphData = nullptr;
    }

    PMsrRunList runs = *fMsrHandler->GetMsrRunList();
    PMsrPlotStructure plotInfo = fMsrHandler->GetMsrPlotList()->at(fPlotNumber);
    UInt_t runNo = (UInt_t)plotInfo.fRuns[0]-1;
    TString xAxisTitle = fRunList->GetXAxisTitle(*runs[runNo].GetRunName(), runNo);

    // if fMultiGraphDiff is not present create it and add the diff data
    fMultiGraphDiff = new TMultiGraph();
    assert(fMultiGraphDiff != nullptr);

    // get the histo frame x/y range boundaries
    Double_t dataXmin=0.0, dataXmax=0.0, dataYmin=0.0, dataYmax=0.0;
    if (unzoom) { // set the x-/y-range back to the original msr-file values
      dataXmin = fXmin;
      dataXmax = fXmax;
      dataYmin = GetMinimum(fNonMusrData[0].diff, dataXmin, dataXmax);
      dataYmax = GetMaximum(fNonMusrData[0].diff, dataXmin, dataXmax);
      for (UInt_t i=1; i<fNonMusrData.size(); i++) {
        if (GetMinimum(fNonMusrData[i].diff, dataXmin, dataXmax) < dataYmin)
          dataYmin = GetMinimum(fNonMusrData[i].diff, dataXmin, dataXmax);
        if (GetMaximum(fNonMusrData[i].diff, dataXmin, dataXmax) > dataYmax)
          dataYmax = GetMaximum(fNonMusrData[i].diff, dataXmin, dataXmax);
      }
      Double_t dd = 0.05*fabs(dataYmax-dataYmin);
      dataYmin -= dd;
      dataYmax += dd;
    } else { // set the x-/y-range to the previous fHistoFrame range
      dataXmin = xmin;
      dataXmax = xmax;
      dataYmin = GetMinimum(fNonMusrData[0].diff, dataXmin, dataXmax);
      dataYmax = GetMaximum(fNonMusrData[0].diff, dataXmin, dataXmax);
      for (UInt_t i=1; i<fNonMusrData.size(); i++) {
        if (GetMinimum(fNonMusrData[i].diff, dataXmin, dataXmax) < dataYmin)
          dataYmin = GetMinimum(fNonMusrData[i].diff, dataXmin, dataXmax);
        if (GetMaximum(fNonMusrData[i].diff, dataXmin, dataXmax) > dataYmax)
          dataYmax = GetMaximum(fNonMusrData[i].diff, dataXmin, dataXmax);
      }
      Double_t dd = 0.05*fabs(dataYmax-dataYmin);
      dataYmin -= dd;
      dataYmax += dd;
    }

    // add all diff data to fMultiGraphDiff
    for (UInt_t i=0; i<fNonMusrData.size(); i++) {
      // the next three lines are ugly but needed for the following reasons:
      // TMultiGraph is taking ownership of the TGraphErrors, hence a deep copy is needed.
      // This is not resulting in a memory leak, since the TMultiGraph object will do the cleaing
      TGraphErrors *ge = new TGraphErrors(*(fNonMusrData[i].diff));
      // Data points and model curves should be fixed on the graph and not dragged around using, e.g., the mouse.
      ge->SetEditable(false);
      fMultiGraphDiff->Add(ge, "p");
    }

    fMultiGraphDiff->Draw("a");

    // set x-range
    fMultiGraphDiff->GetXaxis()->SetRangeUser(dataXmin, dataXmax);
    fMultiGraphDiff->GetYaxis()->SetRangeUser(dataYmin, dataYmax);

    // set x-axis label
    fMultiGraphDiff->GetXaxis()->SetTitle(xAxisTitle.Data());
    // set y-axis label
    fMultiGraphDiff->GetYaxis()->SetTitle("data-theory");

    fMultiGraphDiff->Draw("a");

    if (fMultiGraphLegend)
      fMultiGraphLegend->Draw();
  }

  fDataTheoryPad->Update();

  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// PlotFourier (private)
//--------------------------------------------------------------------------
/**
 * <p>Plot the Fourier spectra.
 *
 * \param unzoom if true, rescale to the original Fourier range
 */
void PMusrCanvas::PlotFourier(Bool_t unzoom)
{
  fDataTheoryPad->cd();

  // check if log scale plotting and if yes switch back to linear
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fLogY)
    fDataTheoryPad->SetLogy(0); // switch to linear
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fLogX)
    fDataTheoryPad->SetLogx(0); // switch to linear

  if (fPlotType < 0) // plot type not defined
    return;

  if (fData.size() == 0) // no data to be plotted
    return;

  // define x-axis title
  TString xAxisTitle("");
  if (fFourier.fUnits == FOURIER_UNIT_GAUSS) {
    xAxisTitle = TString("Field (G)");
  } else if (fFourier.fUnits == FOURIER_UNIT_TESLA) {
    xAxisTitle = TString("Field (T)");
  } else if (fFourier.fUnits == FOURIER_UNIT_FREQ) {
    xAxisTitle = TString("Frequency (MHz)");
  } else if (fFourier.fUnits == FOURIER_UNIT_CYCLES) {
    xAxisTitle = TString("Frequency (Mc/s)");
  } else {
    xAxisTitle = TString("??");
  }

  // plot fourier data
  Double_t xmin, xmax, ymin, ymax, binContent;
  UInt_t noOfPoints = 1000;
  switch (fCurrentPlotView) {
    case PV_FOURIER_REAL:
      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        xmin = fFourier.fPlotRange[0];
        xmax = fFourier.fPlotRange[1];
      } else {
        xmin = fData[0].dataFourierRe->GetBinLowEdge(1);
        xmax = fData[0].dataFourierRe->GetBinLowEdge(fData[0].dataFourierRe->GetNbinsX())+fData[0].dataFourierRe->GetBinWidth(1);
      }

      // set y-range
      // first find minimum/maximum of all histos and theories
      ymin = GetMinimum(fData[0].dataFourierRe);
      ymax = GetMaximum(fData[0].dataFourierRe);
      binContent = GetMinimum(fData[0].theoryFourierRe);
      if (binContent < ymin)
        ymin = binContent;
      binContent = GetMaximum(fData[0].theoryFourierRe);
      if (binContent > ymax)
        ymax = binContent;
      for (UInt_t i=1; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].dataFourierRe);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].dataFourierRe);
        if (binContent > ymax)
          ymax = binContent;
        binContent = GetMinimum(fData[i].theoryFourierRe);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].theoryFourierRe);
        if (binContent > ymax)
          ymax = binContent;
      }

      // delete old fHistoFrame if present
      if (fHistoFrame) {
        delete fHistoFrame;
        fHistoFrame = nullptr;
      }

      fHistoFrame = fDataTheoryPad->DrawFrame(xmin, 1.05*ymin, xmax, 1.05*ymax);

      // find the maximal number of points present in the histograms and increase the default number of points of fHistoFrame (1000) to the needed one
      noOfPoints = 1000;
      for (UInt_t i=0; i<fData.size(); i++) {
        if (fData[i].dataFourierRe->GetNbinsX() > (Int_t)noOfPoints)
          noOfPoints = fData[i].dataFourierRe->GetNbinsX();
      }
      noOfPoints *= 2; // make sure that there are enough points
      fHistoFrame->SetBins(noOfPoints, xmin, xmax);

      // set ranges for Fourier and Fourier theory
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].dataFourierRe->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].dataFourierRe->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
        fData[i].theoryFourierRe->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].theoryFourierRe->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
      }

      // set x-axis title
      fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
      fHistoFrame->GetYaxis()->SetTitle("Real Fourier");

      // plot data
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].dataFourierRe->Draw("psame");
      }

      // plot theories
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].theoryFourierRe->Draw("same");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_IMAG:
      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        xmin = fFourier.fPlotRange[0];
        xmax = fFourier.fPlotRange[1];
      } else {
        xmin = fData[0].dataFourierIm->GetBinLowEdge(1);
        xmax = fData[0].dataFourierIm->GetBinLowEdge(fData[0].dataFourierIm->GetNbinsX())+fData[0].dataFourierIm->GetBinWidth(1);
      }

      // set y-range
      // first find minimum/maximum of all histos
      ymin = GetMinimum(fData[0].dataFourierIm);
      ymax = GetMaximum(fData[0].dataFourierIm);
      binContent = GetMinimum(fData[0].theoryFourierIm);
      if (binContent < ymin)
        ymin = binContent;
      binContent = GetMaximum(fData[0].theoryFourierIm);
      if (binContent > ymax)
        ymax = binContent;
      for (UInt_t i=1; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].dataFourierIm);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].dataFourierIm);
        if (binContent > ymax)
          ymax = binContent;
        binContent = GetMinimum(fData[i].theoryFourierIm);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].theoryFourierIm);
        if (binContent > ymax)
          ymax = binContent;
      }

      // delete old fHistoFrame if present
      if (fHistoFrame) {
        delete fHistoFrame;
        fHistoFrame = nullptr;
      }

      fHistoFrame = fDataTheoryPad->DrawFrame(xmin, 1.05*ymin, xmax, 1.05*ymax);

      // find the maximal number of points present in the histograms and increase the default number of points of fHistoFrame (1000) to the needed one
      noOfPoints = 1000;
      for (UInt_t i=0; i<fData.size(); i++) {
        if (fData[i].dataFourierIm->GetNbinsX() > (Int_t)noOfPoints)
          noOfPoints = fData[i].dataFourierIm->GetNbinsX();
      }
      noOfPoints *= 2; // make sure that there are enough points
      fHistoFrame->SetBins(noOfPoints, xmin, xmax);

      // set ranges for Fourier and Fourier theory
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].dataFourierIm->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].dataFourierIm->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
        fData[i].theoryFourierIm->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].theoryFourierIm->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
      }

      // set x-axis title
      fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
      fHistoFrame->GetYaxis()->SetTitle("Imaginary Fourier");

      // plot data
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].dataFourierIm->Draw("psame");
      }

      // plot theories
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].theoryFourierIm->Draw("same");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_REAL_AND_IMAG:
      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        xmin = fFourier.fPlotRange[0];
        xmax = fFourier.fPlotRange[1];
      } else {
        xmin = fData[0].dataFourierRe->GetBinLowEdge(1);
        xmax = fData[0].dataFourierRe->GetBinLowEdge(fData[0].dataFourierRe->GetNbinsX())+fData[0].dataFourierRe->GetBinWidth(1);
      }

      // set y-range
      // first find minimum/maximum of all histos
      // real part first
      ymin = GetMinimum(fData[0].dataFourierRe);
      ymax = GetMaximum(fData[0].dataFourierRe);
      for (UInt_t i=1; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].dataFourierRe);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].dataFourierRe);
        if (binContent > ymax)
          ymax = binContent;
      }
      // imag part min/max
      for (UInt_t i=0; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].dataFourierIm);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].dataFourierIm);
        if (binContent > ymax)
          ymax = binContent;
      }
      // theory part min/max
      for (UInt_t i=0; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].theoryFourierRe);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].theoryFourierRe);
        if (binContent > ymax)
          ymax = binContent;
        binContent = GetMinimum(fData[i].theoryFourierIm);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].theoryFourierIm);
        if (binContent > ymax)
          ymax = binContent;
      }

      // delete old fHistoFrame if present
      if (fHistoFrame) {
        delete fHistoFrame;
        fHistoFrame = nullptr;
      }

      fHistoFrame = fDataTheoryPad->DrawFrame(xmin, 1.05*ymin, xmax, 1.05*ymax);

      // find the maximal number of points present in the histograms and increase the default number of points of fHistoFrame (1000) to the needed one
      noOfPoints = 1000;
      for (UInt_t i=0; i<fData.size(); i++) {
        if (fData[i].dataFourierRe->GetNbinsX() > (Int_t)noOfPoints)
          noOfPoints = fData[i].dataFourierRe->GetNbinsX();
      }
      noOfPoints *= 2; // make sure that there are enough points
      fHistoFrame->SetBins(noOfPoints, xmin, xmax);

      // set ranges for Fourier and Fourier theory
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].dataFourierRe->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].dataFourierRe->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
        fData[i].theoryFourierRe->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].theoryFourierRe->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
        fData[i].dataFourierIm->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].dataFourierIm->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
        fData[i].theoryFourierIm->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].theoryFourierIm->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
      }

      // set x-axis title
      fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
      fHistoFrame->GetYaxis()->SetTitle("Real/Imag Fourier");

      // plot data
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].dataFourierRe->Draw("psame");
        fData[i].dataFourierIm->Draw("psame");
      }

      // plot theories
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].theoryFourierRe->Draw("same");
        fData[i].theoryFourierIm->Draw("same");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_PWR:
      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        xmin = fFourier.fPlotRange[0];
        xmax = fFourier.fPlotRange[1];
      } else {
        xmin = fData[0].dataFourierPwr->GetBinLowEdge(1);
        xmax = fData[0].dataFourierPwr->GetBinLowEdge(fData[0].dataFourierPwr->GetNbinsX())+fData[0].dataFourierPwr->GetBinWidth(1);
      }

      // set y-range
      // first find minimum/maximum of all histos and theory
      ymin = GetMinimum(fData[0].dataFourierPwr);
      ymax = GetMaximum(fData[0].dataFourierPwr);
      binContent = GetMinimum(fData[0].theoryFourierPwr);
      if (binContent < ymin)
        ymin = binContent;
      binContent = GetMaximum(fData[0].theoryFourierPwr);
      if (binContent > ymax)
        ymax = binContent;
      for (UInt_t i=1; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].dataFourierPwr);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].dataFourierPwr);
        if (binContent > ymax)
          ymax = binContent;
        binContent = GetMinimum(fData[i].theoryFourierPwr);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].theoryFourierPwr);
        if (binContent > ymax)
          ymax = binContent;
      }

      // delete old fHistoFrame if present
      if (fHistoFrame) {
        delete fHistoFrame;
        fHistoFrame = nullptr;
      }

      fHistoFrame = fDataTheoryPad->DrawFrame(xmin, 0.95*ymin, xmax, 1.05*ymax);

      // find the maximal number of points present in the histograms and increase the default number of points of fHistoFrame (1000) to the needed one
      noOfPoints = 1000;
      for (UInt_t i=0; i<fData.size(); i++) {
        if (fData[i].dataFourierPwr->GetNbinsX() > (Int_t)noOfPoints)
          noOfPoints = fData[i].dataFourierPwr->GetNbinsX();
      }
      noOfPoints *= 2; // make sure that there are enough points
      fHistoFrame->SetBins(noOfPoints, xmin, xmax);

      // set ranges for Fourier and Fourier theory
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].dataFourierPwr->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].dataFourierPwr->GetYaxis()->SetRangeUser(0.95*ymin, 1.05*ymax);
        fData[i].theoryFourierPwr->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].theoryFourierPwr->GetYaxis()->SetRangeUser(0.95*ymin, 1.05*ymax);
      }

      // set x-axis title
      fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
      fHistoFrame->GetYaxis()->SetTitle("Ampl. Fourier");

      // plot data
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].dataFourierPwr->Draw("psame");
      }

      // plot theories
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].theoryFourierPwr->Draw("same");
      }

      break;
    case PV_FOURIER_PHASE:
      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        xmin = fFourier.fPlotRange[0];
        xmax = fFourier.fPlotRange[1];
      } else {
        xmin = fData[0].dataFourierPhase->GetBinLowEdge(1);
        xmax = fData[0].dataFourierPhase->GetBinLowEdge(fData[0].dataFourierPhase->GetNbinsX())+fData[0].dataFourierPhase->GetBinWidth(1);
      }

      // set y-range
      // first find minimum/maximum of all histos
      ymin = GetMinimum(fData[0].dataFourierPhase);
      ymax = GetMaximum(fData[0].dataFourierPhase);
      binContent = GetMinimum(fData[0].theoryFourierPhase);
      if (binContent < ymin)
        ymin = binContent;
      binContent = GetMaximum(fData[0].theoryFourierPhase);
      if (binContent > ymax)
        ymax = binContent;
      for (UInt_t i=1; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].dataFourierPhase);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].dataFourierPhase);
        if (binContent > ymax)
          ymax = binContent;
        binContent = GetMinimum(fData[i].theoryFourierPhase);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].theoryFourierPhase);
        if (binContent > ymax)
          ymax = binContent;
      }

      // delete old fHistoFrame if present
      if (fHistoFrame) {
        delete fHistoFrame;
        fHistoFrame = nullptr;
      }

      fHistoFrame = fDataTheoryPad->DrawFrame(xmin, 1.05*ymin, xmax, 1.05*ymax);

      // find the maximal number of points present in the histograms and increase the default number of points of fHistoFrame (1000) to the needed one
      noOfPoints = 1000;
      for (UInt_t i=0; i<fData.size(); i++) {
        if (fData[i].dataFourierPhase->GetNbinsX() > (Int_t)noOfPoints)
          noOfPoints = fData[i].dataFourierPhase->GetNbinsX();
      }
      noOfPoints *= 2; // make sure that there are enough points
      fHistoFrame->SetBins(noOfPoints, xmin, xmax);

      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].dataFourierPhase->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].dataFourierPhase->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
        fData[i].theoryFourierPhase->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].theoryFourierPhase->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
      }

      // set x-axis title
      fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
      fHistoFrame->GetYaxis()->SetTitle("Phase Fourier");

      // plot data
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].dataFourierPhase->Draw("psame");
      }

      // plot theories
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].theoryFourierPhase->Draw("same");
      }

      break;
  case PV_FOURIER_PHASE_OPT_REAL:
    // set x-range
    if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
      xmin = fFourier.fPlotRange[0];
      xmax = fFourier.fPlotRange[1];
    } else {
      xmin = fData[0].dataFourierPhaseOptReal->GetBinLowEdge(1);
      xmax = fData[0].dataFourierPhaseOptReal->GetBinLowEdge(fData[0].dataFourierPhaseOptReal->GetNbinsX())+fData[0].dataFourierPhaseOptReal->GetBinWidth(1);
    }

    // set y-range
    // first find minimum/maximum of all histos
    ymin = GetMinimum(fData[0].dataFourierPhaseOptReal);
    ymax = GetMaximum(fData[0].dataFourierPhaseOptReal);
    binContent = GetMinimum(fData[0].theoryFourierPhaseOptReal);
    if (binContent < ymin)
      ymin = binContent;
    binContent = GetMaximum(fData[0].theoryFourierPhaseOptReal);
    if (binContent > ymax)
      ymax = binContent;
    for (UInt_t i=1; i<fData.size(); i++) {
      binContent = GetMinimum(fData[i].dataFourierPhaseOptReal);
      if (binContent < ymin)
        ymin = binContent;
      binContent = GetMaximum(fData[i].dataFourierPhaseOptReal);
      if (binContent > ymax)
        ymax = binContent;
      binContent = GetMinimum(fData[i].theoryFourierPhaseOptReal);
      if (binContent < ymin)
        ymin = binContent;
      binContent = GetMaximum(fData[i].theoryFourierPhaseOptReal);
      if (binContent > ymax)
        ymax = binContent;
    }

    // delete old fHistoFrame if present
    if (fHistoFrame) {
      delete fHistoFrame;
      fHistoFrame = nullptr;
    }

    fHistoFrame = fDataTheoryPad->DrawFrame(xmin, 1.05*ymin, xmax, 1.05*ymax);

    // find the maximal number of points present in the histograms and increase the default number of points of fHistoFrame (1000) to the needed one
    noOfPoints = 1000;
    for (UInt_t i=0; i<fData.size(); i++) {
      if (fData[i].dataFourierPhaseOptReal->GetNbinsX() > (Int_t)noOfPoints)
        noOfPoints = fData[i].dataFourierPhaseOptReal->GetNbinsX();
    }
    noOfPoints *= 2; // make sure that there are enough points
    fHistoFrame->SetBins(noOfPoints, xmin, xmax);

    for (UInt_t i=0; i<fData.size(); i++) {
      fData[i].dataFourierPhaseOptReal->GetXaxis()->SetRangeUser(xmin, xmax);
      fData[i].dataFourierPhaseOptReal->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
      fData[i].theoryFourierPhaseOptReal->GetXaxis()->SetRangeUser(xmin, xmax);
      fData[i].theoryFourierPhaseOptReal->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
    }

    // set x-axis title
    fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());

    // set y-axis title
    fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
    fHistoFrame->GetYaxis()->SetTitle("Phase Opt. Real Fourier");

    // plot data
    for (UInt_t i=0; i<fData.size(); i++) {
      fData[i].dataFourierPhaseOptReal->Draw("psame");
    }

    // plot theories
    for (UInt_t i=0; i<fData.size(); i++) {
      fData[i].theoryFourierPhaseOptReal->Draw("same");
    }

    break;
    default:
      break;
  }

  // check if RRF and if yes show a label
  if ((fRRFText != nullptr) && (fRRFLatexText != nullptr)) {
    fRRFLatexText->DrawLatex(0.1, 0.92, fRRFText->Data());
  }

  fDataTheoryPad->Update();

  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// PlotFourierDifference (private)
//--------------------------------------------------------------------------
/**
 * <p>Plot the Fourier difference, i.e. F(data)-F(theory).
 *
 * \param unzoom if true, rescale to the original Fourier range
 */
void PMusrCanvas::PlotFourierDifference(Bool_t unzoom)
{
  fDataTheoryPad->cd();

  // check if log scale plotting and if yes switch back to linear
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fLogY)
    fDataTheoryPad->SetLogy(0); // switch to linear
  if (fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fLogX)
    fDataTheoryPad->SetLogx(0); // switch to linear

  if (fPlotType < 0) // plot type not defined
    return;

  if (fData.size() == 0) // no data to be plotted
    return;

  // define x-axis title
  TString xAxisTitle("");
  if (fFourier.fUnits == FOURIER_UNIT_GAUSS) {
    xAxisTitle = TString("Field (G)");
  } else if (fFourier.fUnits == FOURIER_UNIT_TESLA) {
    xAxisTitle = TString("Field (T)");
  } else if (fFourier.fUnits == FOURIER_UNIT_FREQ) {
    xAxisTitle = TString("Frequency (MHz)");
  } else if (fFourier.fUnits == FOURIER_UNIT_CYCLES) {
    xAxisTitle = TString("Frequency (Mc/s)");
  } else {
    xAxisTitle = TString("??");
  }

  // plot data
  double xmin, xmax, ymin, ymax, binContent;
  switch (fCurrentPlotView) {
    case PV_FOURIER_REAL:
      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        xmin = fFourier.fPlotRange[0];
        xmax = fFourier.fPlotRange[1];
      } else {
        xmin = fData[0].diffFourierRe->GetBinLowEdge(1);
        xmax = fData[0].diffFourierRe->GetBinLowEdge(fData[0].diffFourierRe->GetNbinsX())+fData[0].diffFourierRe->GetBinWidth(1);
      }

      // set y-range
      // first find minimum/maximum of all histos
      ymin = GetMinimum(fData[0].diffFourierRe);
      ymax = GetMaximum(fData[0].diffFourierRe);
      for (UInt_t i=1; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].diffFourierRe);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].diffFourierRe);
        if (binContent > ymax)
          ymax = binContent;
      }

      // delete old fHistoFrame if present
      if (fHistoFrame) {
        delete fHistoFrame;
        fHistoFrame = nullptr;
      }

      fHistoFrame = fDataTheoryPad->DrawFrame(xmin, 1.05*ymin, xmax, 1.05*ymax);

      // set ranges for Fourier difference
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].diffFourierRe->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].diffFourierRe->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
      }

      // set x-axis title
      fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
      if (fData[0].diffFourierTag == 1)
        fHistoFrame->GetYaxis()->SetTitle("Real Fourier (d-f: data-theory)");
      else
        fHistoFrame->GetYaxis()->SetTitle("Real Fourier (f-d: [(F data)-(F theory)]");

      // plot data
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].diffFourierRe->Draw("plsame");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_IMAG:
      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        xmin = fFourier.fPlotRange[0];
        xmax = fFourier.fPlotRange[1];
      } else {
        xmin = fData[0].diffFourierIm->GetBinLowEdge(1);
        xmax = fData[0].diffFourierIm->GetBinLowEdge(fData[0].diffFourierIm->GetNbinsX())+fData[0].diffFourierIm->GetBinWidth(1);
      }

      // set y-range
      // first find minimum/maximum of all histos
      ymin = GetMinimum(fData[0].diffFourierIm);
      ymax = GetMaximum(fData[0].diffFourierIm);
      for (UInt_t i=1; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].diffFourierIm);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].diffFourierIm);
        if (binContent > ymax)
          ymax = binContent;
      }

      // delete old fHistoFrame if present
      if (fHistoFrame) {
        delete fHistoFrame;
        fHistoFrame = nullptr;
      }

      fHistoFrame = fDataTheoryPad->DrawFrame(xmin, 1.05*ymin, xmax, 1.05*ymax);

      // set ranges for Fourier difference
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].diffFourierIm->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].diffFourierIm->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
      }

      // set x-axis title
      fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
      if (fData[0].diffFourierTag == 1)
        fHistoFrame->GetYaxis()->SetTitle("Imaginary Fourier (d-f: data-theory)");
      else
        fHistoFrame->GetYaxis()->SetTitle("Imaginary Fourier (f-d: [(F data)-(F theory)]");

      // plot data
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].diffFourierIm->Draw("plsame");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_REAL_AND_IMAG:
      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        xmin = fFourier.fPlotRange[0];
        xmax = fFourier.fPlotRange[1];
      } else {
        xmin = fData[0].diffFourierRe->GetBinLowEdge(1);
        xmax = fData[0].diffFourierRe->GetBinLowEdge(fData[0].diffFourierRe->GetNbinsX())+fData[0].diffFourierRe->GetBinWidth(1);
      }

      // set y-range
      // first find minimum/maximum of all histos
      ymin = GetMinimum(fData[0].diffFourierRe);
      ymax = GetMaximum(fData[0].diffFourierRe);
      for (UInt_t i=1; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].diffFourierRe);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].diffFourierRe);
        if (binContent > ymax)
          ymax = binContent;
      }
      for (UInt_t i=0; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].diffFourierIm);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].diffFourierIm);
        if (binContent > ymax)
          ymax = binContent;
      }

      // delete old fHistoFrame if present
      if (fHistoFrame) {
        delete fHistoFrame;
        fHistoFrame = nullptr;
      }

      fHistoFrame = fDataTheoryPad->DrawFrame(xmin, 1.05*ymin, xmax, 1.05*ymax);

      // set ranges for Fourier difference
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].diffFourierRe->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].diffFourierRe->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
        fData[i].diffFourierIm->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].diffFourierIm->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
      }

      // set x-axis title
      fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
      if (fData[0].diffFourierTag == 1)
        fHistoFrame->GetYaxis()->SetTitle("Real+Imag Fourier (d-f: data-theory)");
      else
        fHistoFrame->GetYaxis()->SetTitle("Real+Imag Fourier (f-d: [(F data)-(F theory)]");

      // plot data
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].diffFourierRe->Draw("plsame");
        fData[i].diffFourierIm->Draw("plsame");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_PWR:
      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        xmin = fFourier.fPlotRange[0];
        xmax = fFourier.fPlotRange[1];
      } else {
        xmin = fData[0].diffFourierPwr->GetBinLowEdge(1);
        xmax = fData[0].diffFourierPwr->GetBinLowEdge(fData[0].diffFourierPwr->GetNbinsX())+fData[0].diffFourierPwr->GetBinWidth(1);
      }

      // set y-range
      // first find minimum/maximum of all histos
      ymin = GetMinimum(fData[0].diffFourierPwr);
      ymax = GetMaximum(fData[0].diffFourierPwr);
      for (UInt_t i=1; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].diffFourierPwr);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].diffFourierPwr);
        if (binContent > ymax)
          ymax = binContent;
      }

      // delete old fHistoFrame if present
      if (fHistoFrame) {
        delete fHistoFrame;
        fHistoFrame = nullptr;
      }

      fHistoFrame = fDataTheoryPad->DrawFrame(xmin, 0.95*ymin, xmax, 1.05*ymax);

      // set x-axis title
      fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set ranges for Fourier difference
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].diffFourierPwr->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].diffFourierPwr->GetYaxis()->SetRangeUser(0.95*ymin, 1.05*ymax);
      }

      // set y-axis title
      fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
      if (fData[0].diffFourierTag == 1)
        fHistoFrame->GetYaxis()->SetTitle("Ampl. Fourier (d-f: data-theory)");
      else
        fHistoFrame->GetYaxis()->SetTitle("Ampl. Fourier (f-d: [(F data)-(F theory)]");

      // plot data
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].diffFourierPwr->Draw("plsame");
      }

      break;
    case PV_FOURIER_PHASE:
      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        xmin = fFourier.fPlotRange[0];
        xmax = fFourier.fPlotRange[1];
      } else {
        xmin = fData[0].diffFourierPhase->GetBinLowEdge(1);
        xmax = fData[0].diffFourierPhase->GetBinLowEdge(fData[0].diffFourierPhase->GetNbinsX())+fData[0].diffFourierPhase->GetBinWidth(1);
      }

      // set y-range
      // first find minimum/maximum of all histos
      ymin = GetMinimum(fData[0].diffFourierPhase);
      ymax = GetMaximum(fData[0].diffFourierPhase);
      for (UInt_t i=1; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].diffFourierPhase);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].diffFourierPhase);
        if (binContent > ymax)
          ymax = binContent;
      }

      // delete old fHistoFrame if present
      if (fHistoFrame) {
        delete fHistoFrame;
        fHistoFrame = nullptr;
      }

      fHistoFrame = fDataTheoryPad->DrawFrame(xmin, 1.05*ymin, xmax, 1.05*ymax);

      // set ranges for Fourier difference
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].diffFourierPhase->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].diffFourierPhase->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
      }

      // set x-axis title
      fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
      if (fData[0].diffFourierTag == 1)
        fHistoFrame->GetYaxis()->SetTitle("Phase Fourier (d-f: data-theory)");
      else
        fHistoFrame->GetYaxis()->SetTitle("Phase Fourier [f-d: (F data)-(F theory)]");

      // plot data
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].diffFourierPhase->Draw("plsame");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_PHASE_OPT_REAL:
      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        xmin = fFourier.fPlotRange[0];
        xmax = fFourier.fPlotRange[1];
      } else {
        xmin = fData[0].diffFourierPhaseOptReal->GetBinLowEdge(1);
        xmax = fData[0].diffFourierPhaseOptReal->GetBinLowEdge(fData[0].diffFourierPhaseOptReal->GetNbinsX())+fData[0].diffFourierPhaseOptReal->GetBinWidth(1);
      }

      // set y-range
      // first find minimum/maximum of all histos
      ymin = GetMinimum(fData[0].diffFourierPhaseOptReal);
      ymax = GetMaximum(fData[0].diffFourierPhaseOptReal);
      for (UInt_t i=1; i<fData.size(); i++) {
        binContent = GetMinimum(fData[i].diffFourierPhaseOptReal);
        if (binContent < ymin)
          ymin = binContent;
        binContent = GetMaximum(fData[i].diffFourierPhaseOptReal);
        if (binContent > ymax)
          ymax = binContent;
      }

      // delete old fHistoFrame if present
      if (fHistoFrame) {
        delete fHistoFrame;
        fHistoFrame = nullptr;
      }

      fHistoFrame = fDataTheoryPad->DrawFrame(xmin, 1.05*ymin, xmax, 1.05*ymax);

      // set ranges for phase opt. real Fourier difference
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].diffFourierPhaseOptReal->GetXaxis()->SetRangeUser(xmin, xmax);
        fData[i].diffFourierPhaseOptReal->GetYaxis()->SetRangeUser(1.05*ymin, 1.05*ymax);
      }

      // set x-axis title
      fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fHistoFrame->GetYaxis()->SetTitleOffset(1.3);
      if (fData[0].diffFourierTag == 1)
        fHistoFrame->GetYaxis()->SetTitle("Real Fourier (d-f: data-theory)");
      else
        fHistoFrame->GetYaxis()->SetTitle("Real Fourier (f-d: [(F data)-(F theory)]");

      // plot data
      for (UInt_t i=0; i<fData.size(); i++) {
        fData[i].diffFourierPhaseOptReal->Draw("plsame");
      }
      break;
    default:
      break;
   }

  // check if RRF and if yes show a label
  if ((fRRFText != nullptr) && (fRRFLatexText != nullptr)) {
    fRRFLatexText->DrawLatex(0.1, 0.92, fRRFText->Data());
  }

  fDataTheoryPad->Update();

  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// PlotFourierPhaseValue (private)
//--------------------------------------------------------------------------
/**
 * <p>Writes the Fourier phase value into the data window.
 *
 * \param unzoom if true, rescale to the original Fourier range
 */
void PMusrCanvas::PlotFourierPhaseValue(Bool_t unzoom)
{
  // check if phase TLatex object is present
  if (fCurrentFourierPhaseText) {
    delete fCurrentFourierPhaseText;
    fCurrentFourierPhaseText = nullptr;
  }

  double x, y;
  TString str;

  // plot Fourier phase
  str = TString("phase = ");
  str += fCurrentFourierPhase[0];
  if (fFourier.fPhase.size() > 1) { // if more than one phase is present, do NOT plot phase info
    str = TString("");
  }
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
// PlotAverage (private)
//--------------------------------------------------------------------------
/**
 * <p>Plot the average of the given data sets.
 *
 * \param unzoom if true, rescale to the original average range.
 */
void PMusrCanvas::PlotAverage(Bool_t unzoom)
{
  fDataTheoryPad->cd();

  // define x-axis title
  TString xAxisTitle("");
  if (fCurrentPlotView == PV_DATA) {
    PMsrRunList runs = *fMsrHandler->GetMsrRunList();
    TString setup = fRunList->GetSetup(*runs[0].GetRunName());
    if (strcmp(setup, "TRIUMF/BNQR") || strcmp(setup, "TRIUMF/BNMR")) {
      xAxisTitle = TString("time (s)");
    } else {
    xAxisTitle = TString("time (#mus)");
    }
  } else { // all the Fourier
    if (fFourier.fUnits == FOURIER_UNIT_GAUSS) {
      xAxisTitle = TString("Field (G)");
    } else if (fFourier.fUnits == FOURIER_UNIT_TESLA) {
      xAxisTitle = TString("Field (T)");
    } else if (fFourier.fUnits == FOURIER_UNIT_FREQ) {
      xAxisTitle = TString("Frequency (MHz)");
    } else if (fFourier.fUnits == FOURIER_UNIT_CYCLES) {
      xAxisTitle = TString("Frequency (Mc/s)");
    } else {
      xAxisTitle = TString("??");
    }
  }
  // define y-axis title
  TString yAxisTitle("");
  if (fCurrentPlotView == PV_DATA) {
    if (!fDifferenceView) {
      PMsrRunList *runList = fMsrHandler->GetMsrRunList();
      switch (fPlotType) {
        case MSR_PLOT_SINGLE_HISTO:
          if (runList->at(0).IsLifetimeCorrected()) { // lifetime correction
            yAxisTitle = "<asymmetry>";
          } else { // no liftime correction
            if (fScaleN0AndBkg)
              yAxisTitle = "<N(t)> per nsec";
            else
              yAxisTitle = "<N(t)> per bin";
          }
          break;
        case MSR_PLOT_ASYM:
          yAxisTitle = "<asymmetry>";
          break;
        case MSR_PLOT_BNMR:
          yAxisTitle = "<asymmetry>";
          break;
        case MSR_PLOT_MU_MINUS:
          yAxisTitle = "<N(t)> per bin";
          break;
        default:
          yAxisTitle = "??";
          break;
      }
    } else { // DifferenceView
      yAxisTitle = "<data-theory>";
    }
  } else { // all the Fourier
    if (!fDifferenceView) {
      switch (fCurrentPlotView) {
        case PV_FOURIER_REAL:
          yAxisTitle = "<Real Fourier>";
          break;
        case PV_FOURIER_IMAG:
          yAxisTitle = "<Imaginary Fourier>";
          break;
        case PV_FOURIER_REAL_AND_IMAG:
          yAxisTitle = "<Real/Imag Fourier>";
          break;
        case PV_FOURIER_PWR:
          yAxisTitle = "<Ampl. Fourier>";
          break;
        case PV_FOURIER_PHASE:
          yAxisTitle = "<Phase Fourier>";
          break;
        case PV_FOURIER_PHASE_OPT_REAL:
          yAxisTitle = "<Phase Opt. Real Fourier>";
          break;
        default:
        yAxisTitle = "??";
          break;
      }
    } else { // DifferenceView
      switch (fCurrentPlotView) {
        case PV_FOURIER_REAL:
          if (fData[0].diffFourierTag == 1)
            yAxisTitle = "<Real Fourier (d-f: data-theory)>";
          else
            yAxisTitle = "<Real Fourier (f-d: [(F data)-(F theory)]>";
          break;
        case PV_FOURIER_IMAG:
        if (fData[0].diffFourierTag == 1)
          yAxisTitle = "<Imag Fourier (d-f: data-theory)>";
        else
          yAxisTitle = "<Imag Fourier (f-d: [(F data)-(F theory)]>";
        break;
          break;
        case PV_FOURIER_REAL_AND_IMAG:
        if (fData[0].diffFourierTag == 1)
          yAxisTitle = "<Real/Imag Fourier (d-f: data-theory)>";
        else
          yAxisTitle = "<Real/Imag Fourier (f-d: [(F data)-(F theory)]>";
        break;
          break;
        case PV_FOURIER_PWR:
        if (fData[0].diffFourierTag == 1)
          yAxisTitle = "<Ampl. Fourier (d-f: data-theory)>";
        else
          yAxisTitle = "<Ampl. Fourier (f-d: [(F data)-(F theory)]>";
        break;
          break;
        case PV_FOURIER_PHASE:
        if (fData[0].diffFourierTag == 1)
          yAxisTitle = "<Phase Fourier (d-f: data-theory)>";
        else
          yAxisTitle = "<Phase Fourier (f-d: [(F data)-(F theory)]>";
        break;
          break;
        default:
        yAxisTitle = "??";
          break;
      }
    }
  }

  // find proper ranges
  Double_t xmin, xmax, ymin, ymax;
  xmin = fHistoFrame->GetXaxis()->GetBinLowEdge(fHistoFrame->GetXaxis()->GetFirst());
  xmax = fHistoFrame->GetXaxis()->GetBinLowEdge(fHistoFrame->GetXaxis()->GetLast()) + fHistoFrame->GetXaxis()->GetBinWidth(fHistoFrame->GetXaxis()->GetLast());
  ymin = fHistoFrame->GetMinimum();
  ymax = fHistoFrame->GetMaximum();

  // delete old fHistoFrame if present
  if (fHistoFrame) {
    delete fHistoFrame;
    fHistoFrame = nullptr;
  }

  fHistoFrame = fDataTheoryPad->DrawFrame(xmin, ymin, xmax, ymax);

  fHistoFrame->GetXaxis()->SetTitle(xAxisTitle.Data());
  fHistoFrame->GetYaxis()->SetTitle(yAxisTitle.Data());
  fHistoFrame->GetYaxis()->SetTitleOffset(1.3);

  // find out what to be plotted
  switch (fCurrentPlotView) {
    case PV_DATA:
      if (!fDifferenceView) { // averaged data view
        fDataAvg.data->Draw("psame");
        fDataAvg.theory->Draw("same");
      } else { // averaged diff data view
        fDataAvg.diff->Draw("psame");
      }
      break;
    case PV_FOURIER_REAL:
      if (!fDifferenceView) { // averaged Fourier Real view
        fDataAvg.dataFourierRe->Draw("psame");
        fDataAvg.theoryFourierRe->Draw("same");
      } else { // averaged diff Fourier Real view
        fDataAvg.diffFourierRe->Draw("psame");
      }
      break;
    case PV_FOURIER_IMAG:
      if (!fDifferenceView) { // averaged Fourier Imag view
        fDataAvg.dataFourierIm->Draw("psame");
        fDataAvg.theoryFourierIm->Draw("same");
      } else { // averaged diff Fourier Imag view
        fDataAvg.diffFourierIm->Draw("psame");
      }
      break;
    case PV_FOURIER_REAL_AND_IMAG:
      if (!fDifferenceView) { // averaged Fourier Real&Imag view
        fDataAvg.dataFourierRe->Draw("psame");
        fDataAvg.theoryFourierRe->Draw("same");
        fDataAvg.dataFourierIm->Draw("psame");
        fDataAvg.theoryFourierIm->Draw("same");
      } else { // averaged diff Fourier Real&Imag view
        fDataAvg.diffFourierRe->Draw("psame");
        fDataAvg.diffFourierIm->Draw("psame");
      }
      break;
    case PV_FOURIER_PWR:
      if (!fDifferenceView) { // averaged Fourier Power view
        fDataAvg.dataFourierPwr->Draw("psame");
        fDataAvg.theoryFourierPwr->Draw("same");
      } else { // averaged diff Fourier Power view
        fDataAvg.diffFourierPwr->Draw("psame");
      }
      break;
    case PV_FOURIER_PHASE:
      if (!fDifferenceView) { // averaged Fourier Phase view
        fDataAvg.dataFourierPhase->Draw("psame");
        fDataAvg.theoryFourierPhase->Draw("same");
      } else { // averaged diff Fourier Phase view
        fDataAvg.diffFourierPhase->Draw("psame");
      }
      break;
    case PV_FOURIER_PHASE_OPT_REAL:
      if (!fDifferenceView) { // averaged Fourier Phase Opt Real view
        fDataAvg.dataFourierPhaseOptReal->Draw("psame");
        fDataAvg.theoryFourierPhaseOptReal->Draw("same");
      } else { // averaged diff Fourier Phase view
        fDataAvg.diffFourierPhaseOptReal->Draw("psame");
      }
      break;
    default:
      break;
  }

  // check if RRF and if yes show a label
  if ((fRRFText != nullptr) && (fRRFLatexText != nullptr)) {
    fRRFLatexText->DrawLatex(0.1, 0.92, fRRFText->Data());
  }

  fDataTheoryPad->Update();

  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// IncrementFourierPhase (private)
//--------------------------------------------------------------------------
/**
 * <p>Increments the Fourier phase and recalculate the real/imaginary part of the Fourier transform.
 */
void PMusrCanvas::IncrementFourierPhase()
{
  if (fCurrentPlotView == PV_FOURIER_PWR)
    return;

  double re, im;
  const double cp = TMath::Cos(fFourier.fPhaseIncrement/180.0*TMath::Pi());
  const double sp = TMath::Sin(fFourier.fPhaseIncrement/180.0*TMath::Pi());

  for (UInt_t i=0; i<fCurrentFourierPhase.size(); i++)
    fCurrentFourierPhase[i] += fFourier.fPhaseIncrement;
  PlotFourierPhaseValue();

  for (UInt_t i=0; i<fData.size(); i++) { // loop over all data sets
    if ((fData[i].dataFourierRe != nullptr) && (fData[i].dataFourierIm != nullptr)) {
      for (Int_t j=0; j<fData[i].dataFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
        // calculate new fourier data set value
        re = fData[i].dataFourierRe->GetBinContent(j) * cp + fData[i].dataFourierIm->GetBinContent(j) * sp;
        im = fData[i].dataFourierIm->GetBinContent(j) * cp - fData[i].dataFourierRe->GetBinContent(j) * sp;
        // overwrite fourier data set value
        fData[i].dataFourierRe->SetBinContent(j, re);
        fData[i].dataFourierIm->SetBinContent(j, im);
      }
    }
    if ((fData[i].theoryFourierRe != nullptr) && (fData[i].theoryFourierIm != nullptr)) {
      for (Int_t j=0; j<fData[i].theoryFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
        // calculate new fourier data set value
        re = fData[i].theoryFourierRe->GetBinContent(j) * cp + fData[i].theoryFourierIm->GetBinContent(j) * sp;
        im = fData[i].theoryFourierIm->GetBinContent(j) * cp - fData[i].theoryFourierRe->GetBinContent(j) * sp;
        // overwrite fourier data set value
        fData[i].theoryFourierRe->SetBinContent(j, re);
        fData[i].theoryFourierIm->SetBinContent(j, im);
      }
    }
    if ((fData[i].diffFourierRe != nullptr) && (fData[i].diffFourierIm != nullptr)) {
      for (Int_t j=0; j<fData[i].diffFourierRe->GetNbinsX(); j++) { // loop over a fourier diff data set
        // calculate new fourier diff data set value
        re = fData[i].diffFourierRe->GetBinContent(j) * cp + fData[i].diffFourierIm->GetBinContent(j) * sp;
        im = fData[i].diffFourierIm->GetBinContent(j) * cp - fData[i].diffFourierRe->GetBinContent(j) * sp;
        // overwrite fourier diff data set value
        fData[i].diffFourierRe->SetBinContent(j, re);
        fData[i].diffFourierIm->SetBinContent(j, im);
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
void PMusrCanvas::DecrementFourierPhase()
{
  if (fCurrentPlotView == PV_FOURIER_PWR)
    return;

  double re, im;
  const double cp = TMath::Cos(fFourier.fPhaseIncrement/180.0*TMath::Pi());
  const double sp = TMath::Sin(fFourier.fPhaseIncrement/180.0*TMath::Pi());

  for (UInt_t i=0; i<fCurrentFourierPhase.size(); i++)
    fCurrentFourierPhase[i] -= fFourier.fPhaseIncrement;
  PlotFourierPhaseValue();

  for (UInt_t i=0; i<fData.size(); i++) { // loop over all data sets
    if ((fData[i].dataFourierRe != nullptr) && (fData[i].dataFourierIm != nullptr)) {
      for (Int_t j=0; j<fData[i].dataFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
        // calculate new fourier data set value
        re = fData[i].dataFourierRe->GetBinContent(j) * cp - fData[i].dataFourierIm->GetBinContent(j) * sp;
        im = fData[i].dataFourierIm->GetBinContent(j) * cp + fData[i].dataFourierRe->GetBinContent(j) * sp;
        // overwrite fourier data set value
        fData[i].dataFourierRe->SetBinContent(j, re);
        fData[i].dataFourierIm->SetBinContent(j, im);
      }
    }
    if ((fData[i].theoryFourierRe != nullptr) && (fData[i].theoryFourierIm != nullptr)) {
      for (Int_t j=0; j<fData[i].theoryFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
        // calculate new fourier data set value
        re = fData[i].theoryFourierRe->GetBinContent(j) * cp - fData[i].theoryFourierIm->GetBinContent(j) * sp;
        im = fData[i].theoryFourierIm->GetBinContent(j) * cp + fData[i].theoryFourierRe->GetBinContent(j) * sp;
        // overwrite fourier data set value
        fData[i].theoryFourierRe->SetBinContent(j, re);
        fData[i].theoryFourierIm->SetBinContent(j, im);
      }
    }
    if ((fData[i].diffFourierRe != nullptr) && (fData[i].diffFourierIm != nullptr)) {
      for (Int_t j=0; j<fData[i].diffFourierRe->GetNbinsX(); j++) { // loop over a fourier diff data set
        // calculate new fourier diff data set value
        re = fData[i].diffFourierRe->GetBinContent(j) * cp - fData[i].diffFourierIm->GetBinContent(j) * sp;
        im = fData[i].diffFourierIm->GetBinContent(j) * cp + fData[i].diffFourierRe->GetBinContent(j) * sp;
        // overwrite fourier diff data set value
        fData[i].diffFourierRe->SetBinContent(j, re);
        fData[i].diffFourierIm->SetBinContent(j, im);
      }
    }
  }
}


//--------------------------------------------------------------------------
// IsScaleN0AndBkg (private)
//--------------------------------------------------------------------------
/**
 * <p>Checks if N0/Bkg normalization to 1/ns is whished. The default is yes, since most of the users want to have it that way.
 * To overwrite this, one should add the line 'SCALE_N0_BKG FALSE' to the command block of the msr-file.
 *
 * <b>return:</b>
 * - true, if scaling of N0 and Bkg to 1/ns is whished
 * - false, otherwise
 *
 * \param histoNo forward histogram number of the run
 */
Bool_t PMusrCanvas::IsScaleN0AndBkg()
{
  Bool_t willScale = true;

  PMsrLines *cmd = fMsrHandler->GetMsrCommands();
  for (UInt_t i=0; i<cmd->size(); i++) {
    if (cmd->at(i).fLine.Contains("SCALE_N0_BKG", TString::kIgnoreCase)) {
      TObjArray *tokens = nullptr;
      TObjString *ostr = nullptr;
      TString str;
      tokens = cmd->at(i).fLine.Tokenize(" \t");
      if (tokens->GetEntries() != 2) {
        std::cerr << std::endl << ">> PRunSingleHisto::IsScaleN0AndBkg(): **WARNING** Found uncorrect 'SCALE_N0_BKG' command, will ignore it.";
        std::cerr << std::endl << ">> Allowed commands: SCALE_N0_BKG TRUE | FALSE" << std::endl;
        return willScale;
      }
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      str = ostr->GetString();
      if (!str.CompareTo("FALSE", TString::kIgnoreCase)) {
        willScale = false;
      }
      // clean up
      if (tokens)
        delete tokens;
    }
  }

  return willScale;
}

//--------------------------------------------------------------------------
// GetNeededAccuracy (private)
//--------------------------------------------------------------------------
/**
 * <p>Calculates the needed accuracy of the parameter value based on the given errors.
 *
 * <b>return:</b>
 * - needed accuracy
 *
 * \param param fit parameter with its additional informations, like errors etc.
 */
UInt_t PMusrCanvas::GetNeededAccuracy(PMsrParamStructure param)
{
  const UInt_t precLimit = 6;
  UInt_t decimalPoint = 0;
  UInt_t accuracy = 6;

  if (param.fStep == 0.0) { // check if fit parameter is a constant, i.e. step==0
    char str[128];

    sprintf(str, "%lf", param.fValue);

    // find decimal point
    for (UInt_t i=0; i<strlen(str); i++) {
      if (str[i] == '.') {
        decimalPoint = i;
        break;
      }
    }

    // find last significant digit
    for (Int_t i=strlen(str)-1; i>=0; i--) {
      if (str[i] != '0') {
        if (((UInt_t)i-decimalPoint) < precLimit)
          accuracy = (UInt_t)i-decimalPoint;
        else
          accuracy = precLimit;
        break;
      }
    }

  } else if ((param.fStep != 0) && !param.fPosErrorPresent) { // check if no positive error is given step!=0 but fPosErrorPresent==false

    for (UInt_t i=0; i<precLimit; i++) {
      if ((Int_t)(param.fStep*pow(10.0,(Double_t)i)) != 0) {
        accuracy = i;
        break;
      }
    }

    if (accuracy+1 <= precLimit)
      accuracy += 1;
  } else { // positive and negative error present

    // negative error
    for (UInt_t i=0; i<precLimit; i++) {
      if ((Int_t)(param.fStep*pow(10.0,(Double_t)i)) != 0) {
        accuracy = i;
        break;
      }
    }
    // positive error
    UInt_t accuracy2 = 6;
    for (UInt_t i=0; i<precLimit; i++) {
      if ((Int_t)(param.fStep*pow(10.0,(Double_t)i)) != 0) {
        accuracy2 = i;
        break;
      }
    }

    if (accuracy2 > accuracy)
      accuracy = accuracy2;

    if (accuracy+1 <= precLimit)
      accuracy += 1;
  }

  return accuracy;
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
Double_t PMusrCanvas::GetInterpolatedValue(TH1F* histo, Double_t xVal)
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

