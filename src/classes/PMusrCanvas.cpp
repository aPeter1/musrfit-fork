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

  fMultiGraphData = 0;
  fMultiGraphDiff = 0;

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
  fMultiGraphData = 0;
  fMultiGraphDiff = 0;

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
  fMultiGraphData = 0;
  fMultiGraphDiff = 0;

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
  if (fMultiGraphData) {
    delete fMultiGraphData;
    fMultiGraphData = 0;
  }
  if (fMultiGraphDiff) {
    delete fMultiGraphDiff;
    fMultiGraphDiff = 0;
  }
  if (fNonMusrData.size() > 0) {
/*
    for (unsigned int i=0; i<fNonMusrData.size(); i++)
      CleanupDataSet(fNonMusrData[i]);
*/
    fNonMusrData.clear();
  }
  if (fCurrentFourierPhaseText) {
    delete fCurrentFourierPhaseText;
    fCurrentFourierPhaseText = 0;
  }
}

//--------------------------------------------------------------------------
// SetMsrHandler (public)
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
// UpdateParamTheoryPad (public)
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
// UpdateDataTheoryPad (public)
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PMusrCanvas::UpdateDataTheoryPad()
{
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
//cout << endl << ">> runNo = " << runNo;
//cout << endl;
    if (fPlotType != runs[runNo].fFitType) {
      fValid = false;
      cout << endl << "PMusrCanvas::UpdateDataTheoryPad: **ERROR** plottype = " << fPlotType << ", fittype = " << runs[runNo].fFitType << ", however they have to correspond!";
      cout << endl;
      return;
    }
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
// UpdateInfoPad (public)
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

  // handle keys and popup menu entries
  bool relevantKey = false;
  if (x == 'q') { // quit
    Done(0);
  } else if (x == 'd') { // difference
    relevantKey = true;
    // toggle difference tag
    fDifferenceView = !fDifferenceView;
    // set the popup menu entry properly
    if (fDifferenceView) {
      fPopupMain->CheckEntry(P_MENU_ID_DIFFERENCE+P_MENU_PLOT_OFFSET*fPlotNumber);
    } else {
      fPopupMain->UnCheckEntry(P_MENU_ID_DIFFERENCE+P_MENU_PLOT_OFFSET*fPlotNumber);
    }
  } else if (x == 'u') { // unzoom to the original range
    relevantKey = true;
  } else if (x == 'f') { // Fourier
    relevantKey = true;
    if (fCurrentPlotView == PV_DATA) { // current view is data view
      // uncheck data popup entry
      fPopupMain->UnCheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
      // get default fourier tag and update fourier popup menu
      switch (fFourier.fPlotTag) {
        case FOURIER_PLOT_REAL:
          fCurrentPlotView = PV_FOURIER_REAL;
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL);
          break;
        case FOURIER_PLOT_IMAG:
          fCurrentPlotView = PV_FOURIER_IMAG;
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_IMAG);
          break;
        case FOURIER_PLOT_REAL_AND_IMAG:
          fCurrentPlotView = PV_FOURIER_REAL_AND_IMAG;
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL_AND_IMAG);
          break;
        case FOURIER_PLOT_POWER:
          fCurrentPlotView = PV_FOURIER_PWR;
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PWR);
          break;
        case FOURIER_PLOT_PHASE:
          fCurrentPlotView = PV_FOURIER_PHASE;
          fPopupFourier->CheckEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE);
          break;
        default:
          break;
      }
    } else { // current view is one of the Fourier views
      // set the current plot view to data
      fCurrentPlotView = PV_DATA;
      // uncheck all fourier popup menu items
      fPopupFourier->UnCheckEntries();
      // check the data entry
      fPopupMain->CheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
      // set the current view to data
      fCurrentPlotView = PV_DATA;
    }
  } else if (x == '+') {
    if (fCurrentPlotView != PV_DATA)
      IncrementFourierPhase();
  } else if (x == '-') {
    if (fCurrentPlotView != PV_DATA)
      DecrementFourierPhase();
  } else {
    fMainCanvas->Update();
  }

  // call the apropriate functions if necessary
  if (relevantKey) {
    if (fDifferenceView) { // difference view
      switch (fCurrentPlotView) {
        case PV_DATA:
          CleanupFourierDifference();
          HandleDifference();
          break;
        case PV_FOURIER_REAL:
        case PV_FOURIER_IMAG:
        case PV_FOURIER_REAL_AND_IMAG:
        case PV_FOURIER_PWR:
        case PV_FOURIER_PHASE:
          HandleFourierDifference();
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
          break;
        default:
          break;
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
  if (id == P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber) {
    // set appropriate plot view
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
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL) {
    // set appropriate plot view
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
    } else {
      HandleFourierDifference();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_IMAG) {
    // set appropriate plot view
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
    } else {
      HandleFourierDifference();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL_AND_IMAG) {
    // set appropriate plot view
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
    } else {
      HandleFourierDifference();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PWR) {
    // set appropriate plot view
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
    } else {
      HandleFourierDifference();
    }
  } else if (id == P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE) {
    // set appropriate plot view
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
    } else {
      HandleFourierDifference();
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
          break;
        case PV_FOURIER_REAL:
        case PV_FOURIER_IMAG:
        case PV_FOURIER_REAL_AND_IMAG:
        case PV_FOURIER_PWR:
        case PV_FOURIER_PHASE:
          HandleFourierDifference();
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
          break;
        default:
          break;
      }
    }
  } else if (id == P_MENU_ID_SAVE_DATA+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_SAVE_ASCII) {
    SaveDataAscii();
  } else if (id == P_MENU_ID_SAVE_DATA+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_SAVE_DB) {
    SaveDataDb();
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
// CreateStyle (private)
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
// InitFourier (private)
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
// InitMusrCanvas (private)
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
  fPopupMain->AddEntry("&Data", P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);
  fPopupMain->AddSeparator();

  fPopupMain->AddPopup("&Fourier", fPopupFourier);
  fPopupFourier->AddEntry("Show Real", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL);
  fPopupFourier->AddEntry("Show Imag", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_IMAG);
  fPopupFourier->AddEntry("Show Real+Imag", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_REAL_AND_IMAG);
  fPopupFourier->AddEntry("Show Power", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PWR);
  fPopupFourier->AddEntry("Show Phase", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE);
  fPopupFourier->AddSeparator();
  fPopupFourier->AddEntry("Phase +", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
  fPopupFourier->AddEntry("Phase -", P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);
  fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_PLUS);
  fPopupFourier->DisableEntry(P_MENU_ID_FOURIER+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_FOURIER_PHASE_MINUS);

  fPopupMain->AddEntry("D&ifference", P_MENU_ID_DIFFERENCE+P_MENU_PLOT_OFFSET*fPlotNumber);
  fPopupMain->AddSeparator();

  fPopupSave = new TGPopupMenu();
  fPopupSave->AddEntry("Save ascii", P_MENU_ID_SAVE_DATA+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_SAVE_ASCII);
  fPopupSave->AddEntry("Save db", P_MENU_ID_SAVE_DATA+P_MENU_PLOT_OFFSET*fPlotNumber+P_MENU_ID_SAVE_DB);

  fPopupMain->AddPopup("&Save Data", fPopupSave);
  fBar->MapSubwindows(); 
  fBar->Layout();

  fPopupMain->Connect("TGPopupMenu", "Activated(Int_t)", "PMusrCanvas", this, "HandleMenuPopup(Int_t)");

  fPopupMain->CheckEntry(P_MENU_ID_DATA+P_MENU_PLOT_OFFSET*fPlotNumber);

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
// InitDataSet (private)
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
// InitDataSet (private)
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
// CleanupDataSet (private)
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
// CleanupDataSet (private)
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
// HandleDataSet (private)
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
// HandleNonMusrDataSet (private)
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
// HandleDifference (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::HandleDifference()
{
  // check if it is necessary to calculate diff data
  if ((fPlotType != MSR_PLOT_NON_MUSR) && (fData[0].diff == 0)) {
//cout << endl << ">> calculate diff ..." << endl;
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

  // set x-axis plot range properly
  Int_t xminBin, xmaxBin;
  Double_t xmin, xmax;
  if (fPlotType != MSR_PLOT_NON_MUSR) { // muSR Data
    xminBin = fData[0].data->GetXaxis()->GetFirst(); // first bin of the zoomed range
    xmaxBin = fData[0].data->GetXaxis()->GetLast();  // last bin of the zoomed range
    xmin = fData[0].data->GetXaxis()->GetBinCenter(xminBin);
    xmax = fData[0].data->GetXaxis()->GetBinCenter(xmaxBin);
    fData[0].diff->GetXaxis()->SetRangeUser(xmin, xmax);
  } else { // non-muSR Data
    xminBin = fNonMusrData[0].data->GetXaxis()->GetFirst(); // first bin of the zoomed range
    xmaxBin = fNonMusrData[0].data->GetXaxis()->GetLast();  // last bin of the zoomed range
    xmin = fNonMusrData[0].data->GetXaxis()->GetBinCenter(xminBin);
    xmax = fNonMusrData[0].data->GetXaxis()->GetBinCenter(xmaxBin);
    fNonMusrData[0].diff->GetXaxis()->SetRangeUser(xmin, xmax);
  }

  PlotDifference();
}

//--------------------------------------------------------------------------
// HandleFourier (private)
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PMusrCanvas::HandleFourier()
{
  // check if plot type is appropriate for fourier
  if (fPlotType == MSR_PLOT_NON_MUSR)
    return;

//cout << endl << ">> in HandleFourier ..." << endl;

  // check if fourier needs to be calculated
  if (fData[0].dataFourierRe == 0) {
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
//cout << endl << ">> data scale = " << scale;
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

    // apply global phase if present
//cout << endl << ">> fFourier.fPhase = " << fFourier.fPhase;
    if (fFourier.fPhase != 0.0) {
//cout << endl << ">> apply global phase fFourier.fPhase = " << fFourier.fPhase;
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

    // find optimal Fourier phase if range is given
    if ((fFourier.fRangeForPhaseCorrection[0] != -1.0) && (fFourier.fRangeForPhaseCorrection[1] != -1.0)) {

      fCurrentFourierPhase = FindOptimalFourierPhase();

      // apply optimal Fourier phase
      double re, im;
      const double cp = TMath::Cos(fCurrentFourierPhase/180.0*TMath::Pi());
      const double sp = TMath::Sin(fCurrentFourierPhase/180.0*TMath::Pi());

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
  }

  PlotFourier();
}

//--------------------------------------------------------------------------
// HandleFourierDifference (private)
//--------------------------------------------------------------------------
/**
 * <p>
 */
void PMusrCanvas::HandleFourierDifference()
{
  // check if plot type is appropriate for fourier
  if (fPlotType == MSR_PLOT_NON_MUSR)
    return;

cout << endl << ">> in HandleFourierDifference ..." << endl;

  // check if fourier needs to be calculated
  if (fData[0].diffFourierRe == 0) {
//cout << endl << ">> will calculate Fourier diff ..." << endl;
    // check if difference has been already calcualted, if not do it
    if (fData[0].diff == 0)
      HandleDifference();
    int bin;
    bin = fData[0].diff->GetXaxis()->GetFirst();
    double startTime = fData[0].diff->GetBinCenter(bin);
    bin = fData[0].diff->GetXaxis()->GetLast();
    double endTime   = fData[0].diff->GetBinCenter(bin);
//cout << endl << ">> startTime = " << startTime << ", endTime = " << endTime << endl;
    for (unsigned int i=0; i<fData.size(); i++) {
      // calculate fourier transform of the data
      PFourier fourierData(fData[i].diff, fFourier.fUnits, startTime, endTime, fFourier.fFourierPower);
      if (!fourierData.IsValid()) {
        cout << endl << "**SEVERE ERROR** PMusrCanvas::HandleFourier: couldn't invoke PFourier to calculate the Fourier diff ..." << endl;
        return;
      }
      fourierData.Transform(fFourier.fApodization);
      double scale;
      scale = sqrt(fData[0].diff->GetBinWidth(1)/(endTime-startTime));
cout << endl << ">> data scale = " << scale;
      // get real part of the data
      fData[i].diffFourierRe = fourierData.GetRealFourier(scale);
//cout << endl << ">> i: " << i << ", fData[i].diffFourierRe = " << fData[i].diffFourierRe;
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
    }

    // apply global phase
    if (fFourier.fPhase != 0.0) {
      double re, im;
      const double cp = TMath::Cos(fFourier.fPhase/180.0*TMath::Pi());
      const double sp = TMath::Sin(fFourier.fPhase/180.0*TMath::Pi());

      fCurrentFourierPhase = fFourier.fPhase;

      for (unsigned int i=0; i<fData.size(); i++) { // loop over all data sets
        if ((fData[i].diffFourierRe != 0) && (fData[i].diffFourierIm != 0)) {
          for (int j=0; j<fData[i].diffFourierRe->GetNbinsX(); j++) { // loop over a fourier data set
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

  PlotFourierDifference();
}


//--------------------------------------------------------------------------
// FindOptimalFourierPhase (private)
//--------------------------------------------------------------------------
/**
 * <p> The idea to estimate the optimal phase is that the imaginary part of the fourier should be
 * an antisymmetric function around the resonance, hence the asymmetry defined as asymmetry = max+min,
 * where max/min is the maximum and minimum of the imaginary part, should be a minimum for the correct phase.
 */
double PMusrCanvas::FindOptimalFourierPhase()
{
cout << endl << ">> in FindOptimalFourierPhase ... ";

  // check that Fourier is really present
  if ((fData[0].dataFourierRe == 0) || (fData[0].dataFourierIm == 0))
    return 0.0;

  double minPhase, x, valIm, val_xMin, val_xMax;
  double minIm, maxIm, asymmetry;
  // get min/max of the imaginary part for phase = 0.0 as a starting point
  minPhase = 0.0;
  bool first = true;
  for (int i=0; i<fData[0].dataFourierIm->GetNbinsX(); i++) {
    x = fData[0].dataFourierIm->GetBinCenter(i);
    if ((x > fFourier.fRangeForPhaseCorrection[0]) && (x < fFourier.fRangeForPhaseCorrection[1])) {
      valIm = fData[0].dataFourierIm->GetBinContent(i);
      if (first) {
        minIm = valIm;
        maxIm = valIm;
        val_xMin = valIm;
        first = false;
      } else {
        if (valIm < minIm)
          minIm = valIm;
        if (valIm > maxIm)
          maxIm = valIm;
        val_xMax = valIm;
      }
    }
  }
  asymmetry = (maxIm+minIm)*(val_xMin-val_xMax);

  // go through all phases an check if there is a larger max-min value of the imaginary part
  double cp, sp;
  for (double phase=0.1; phase < 180.0; phase += 0.1) {
    cp = TMath::Cos(phase / 180.0 * TMath::Pi());
    sp = TMath::Sin(phase / 180.0 * TMath::Pi());
    first = true;
    for (int i=0; i<fData[0].dataFourierIm->GetNbinsX(); i++) {
      x = fData[0].dataFourierIm->GetBinCenter(i);
      if ((x > fFourier.fRangeForPhaseCorrection[0]) && (x < fFourier.fRangeForPhaseCorrection[1])) {
        valIm = -sp * fData[0].dataFourierRe->GetBinContent(i) + cp * fData[0].dataFourierIm->GetBinContent(i);
        if (first) {
          minIm = valIm;
          maxIm = valIm;
        val_xMin = valIm;
          first = false;
        } else {
          if (valIm < minIm)
            minIm = valIm;
          if (valIm > maxIm)
            maxIm = valIm;
        val_xMax = valIm;
        }
      }
    }
    if (fabs(asymmetry) > fabs((maxIm+minIm)*(val_xMin-val_xMax))) {
//cout << endl << ">> phase = " << phase << ", asymmetry = " << asymmetry << ", min/max = " << minIm << "/" << maxIm;
      minPhase = phase;
      asymmetry = (maxIm+minIm)*(val_xMin-val_xMax);
    }
  }
cout << endl << ">> optimal phase = " << minPhase << endl;

  return minPhase;
}

//--------------------------------------------------------------------------
// CleanupDifference (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::CleanupDifference()
{
  for (unsigned int i=0; i<fData.size(); i++) {
    if (fData[i].diff != 0) {
      delete fData[i].diff;
      fData[i].diff = 0;
    }
  }
}

//--------------------------------------------------------------------------
// CleanupFourier (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::CleanupFourier()
{
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
}

//--------------------------------------------------------------------------
// CleanupFourierDifference (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::CleanupFourierDifference()
{
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

//--------------------------------------------------------------------------
// CalculateDiff (private)
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
// CalculateDiff (private)
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
// FindBin (private)
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
// GetGlobalMaximum (private)
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
// GetGlobalMinimum (private)
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
// PlotData (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::PlotData()
{
  fDataTheoryPad->cd();

  // uncheck fourier menu entries
  fPopupFourier->UnCheckEntries();

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
      // check if fMultiGraphData needs to be created, and if yes add all data and theory
      if (!fMultiGraphData) {
        fMultiGraphData = new TMultiGraph();
        assert(fMultiGraphData != 0);

        // add all data to fMultiGraphData
        for (unsigned int i=0; i<fNonMusrData.size(); i++) {
          fMultiGraphData->Add(fNonMusrData[i].data, "p");
        }
        // add all the theory to fMultiGraphData
        for (unsigned int i=0; i<fNonMusrData.size(); i++) {
          fMultiGraphData->Add(fNonMusrData[i].theory, "l");
        }
      }

      fMultiGraphData->Draw("a");

      // set x-range
      Double_t xmin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin;
      Double_t xmax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmax;
      fMultiGraphData->GetXaxis()->SetRangeUser(xmin, xmax);
      // check if it is necessary to set the y-axis range
      Double_t ymin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmin;
      Double_t ymax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fYmax;
      if ((ymin != -999.0) && (ymax != -999.0)) {
        fMultiGraphData->GetYaxis()->SetRangeUser(ymin, ymax);
      } else {
        fMultiGraphData->GetYaxis()->UnZoom();
      }
      // set x-axis label
      fMultiGraphData->GetXaxis()->SetTitle(xAxisTitle.Data());
      // set y-axis label
      fMultiGraphData->GetYaxis()->SetTitle(yAxisTitle.Data());

      fMultiGraphData->Draw("a");
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
 * <p>
 *
 */
void PMusrCanvas::PlotDifference()
{
  fDataTheoryPad->cd();

  if (fPlotType < 0) // plot type not defined
    return;

  if (fPlotType != MSR_PLOT_NON_MUSR) {
//cout << endl << ">> PlotDifference(): going to plot diff spectra ... (" << fData[0].diff->GetNbinsX() << ")" << endl;
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

    // if fMultiGraphDiff is not present create it and add the diff data
    if (!fMultiGraphDiff) {
      fMultiGraphDiff = new TMultiGraph();
      assert(fMultiGraphDiff != 0);

      // add all diff data to fMultiGraphDiff
      for (unsigned int i=0; i<fNonMusrData.size(); i++) {
        fMultiGraphDiff->Add(fNonMusrData[i].diff, "p");
      }
    }

    fMultiGraphDiff->Draw("a");

    // set x-axis label
    fMultiGraphDiff->GetXaxis()->SetTitle(xAxisTitle.Data());
    // set y-axis label
    fMultiGraphDiff->GetYaxis()->SetTitle("data-theory");

    fMultiGraphDiff->Draw("a");
  }

  fDataTheoryPad->Update();

  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// PlotFourier (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::PlotFourier()
{
//cout << endl << ">> in PlotFourier() ..." << endl;

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
      fData[0].dataFourierRe->Draw("p");

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
        fData[i].dataFourierRe->Draw("psame");
      }

      // plot theories
      for (unsigned int i=0; i<fData.size(); i++) {
        fData[i].theoryFourierRe->Draw("same");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_IMAG:
      // plot first histo
      fData[0].dataFourierIm->Draw("p");

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
        fData[i].dataFourierIm->Draw("psame");
      }

      // plot theories
      for (unsigned int i=0; i<fData.size(); i++) {
        fData[i].theoryFourierIm->Draw("same");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_REAL_AND_IMAG:
      // plot first histo
      fData[0].dataFourierRe->Draw("p");

      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        min = fFourier.fPlotRange[0];
        max = fFourier.fPlotRange[1];
      } else {
        min = fData[0].dataFourierRe->GetBinLowEdge(1);
        max = fData[0].dataFourierRe->GetBinLowEdge(fData[0].dataFourierRe->GetNbinsX())+fData[0].dataFourierRe->GetBinWidth(1);
      }
      fData[0].dataFourierRe->GetXaxis()->SetRangeUser(min, max);

      // set y-range
      // first find minimum/maximum of all histos
      // real part first
      min = GetGlobalMinimum(fData[0].dataFourierRe);
      max = GetGlobalMaximum(fData[0].dataFourierRe);
      for (unsigned int i=1; i<fData.size(); i++) {
        binContent = GetGlobalMinimum(fData[i].dataFourierRe);
        if (binContent < min)
          min = binContent;
        binContent = GetGlobalMaximum(fData[i].dataFourierRe);
        if (binContent > max)
          max = binContent;
      }
      // imag part min/max
      for (unsigned int i=0; i<fData.size(); i++) {
        binContent = GetGlobalMinimum(fData[i].dataFourierIm);
        if (binContent < min)
          min = binContent;
        binContent = GetGlobalMaximum(fData[i].dataFourierIm);
        if (binContent > max)
          max = binContent;
      }
      fData[0].dataFourierRe->GetYaxis()->SetRangeUser(1.05*min, 1.05*max);

      // set x-axis title
      fData[0].dataFourierRe->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fData[0].dataFourierRe->GetYaxis()->SetTitle("Real/Imag Fourier");

      // plot all remaining data
      fData[0].dataFourierIm->Draw("psame");
      for (unsigned int i=1; i<fData.size(); i++) {
        fData[i].dataFourierRe->Draw("psame");
        fData[i].dataFourierIm->Draw("psame");
      }

      // plot theories
      for (unsigned int i=0; i<fData.size(); i++) {
        fData[i].theoryFourierRe->Draw("same");
        fData[i].theoryFourierIm->Draw("same");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_PWR:
      // plot first histo
      fData[0].dataFourierPwr->Draw("p");

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
        fData[i].dataFourierPwr->Draw("psame");
      }

      // plot theories
      for (unsigned int i=0; i<fData.size(); i++) {
        fData[i].theoryFourierPwr->Draw("same");
      }

      break;
    case PV_FOURIER_PHASE:
      // plot first histo
      fData[0].dataFourierPhase->Draw("p");

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
        fData[i].dataFourierPhase->Draw("psame");
      }

      // plot theories
      for (unsigned int i=0; i<fData.size(); i++) {
        fData[i].theoryFourierPhase->Draw("same");
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
// PlotFourierDifference (private)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrCanvas::PlotFourierDifference()
{
cout << endl << ">> in PlotFourierDifference() ..." << endl;

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
//cout << endl << ">> fData[0].diffFourierRe->GetNbinsX() = " << fData[0].diffFourierRe->GetNbinsX();
      // plot first histo
      fData[0].diffFourierRe->Draw("pl");

      // set x-range
//cout << endl << ">> fPlotRange = " << fFourier.fPlotRange[0] << ", " << fFourier.fPlotRange[1];
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        min = fFourier.fPlotRange[0];
        max = fFourier.fPlotRange[1];
      } else {
        min = fData[0].diffFourierRe->GetBinLowEdge(1);
        max = fData[0].diffFourierRe->GetBinLowEdge(fData[0].diffFourierRe->GetNbinsX())+fData[0].diffFourierRe->GetBinWidth(1);
      }
//cout << endl << ">> x-range: min, max = " << min << ", " << max;
      fData[0].diffFourierRe->GetXaxis()->SetRangeUser(min, max);

      // set y-range
      // first find minimum/maximum of all histos
      min = GetGlobalMinimum(fData[0].diffFourierRe);
      max = GetGlobalMaximum(fData[0].diffFourierRe);
//cout << endl << ">> y-range: min, max = " << min << ", " << max;
      for (unsigned int i=1; i<fData.size(); i++) {
        binContent = GetGlobalMinimum(fData[i].diffFourierRe);
        if (binContent < min)
          min = binContent;
        binContent = GetGlobalMaximum(fData[i].diffFourierRe);
        if (binContent > max)
          max = binContent;
      }
      fData[0].diffFourierRe->GetYaxis()->SetRangeUser(1.05*min, 1.05*max);
//cout << endl << "-> min, max = " << min << ", " << max;

      // set x-axis title
      fData[0].diffFourierRe->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fData[0].diffFourierRe->GetYaxis()->SetTitle("Real Fourier");

      // plot all remaining data
      for (unsigned int i=1; i<fData.size(); i++) {
        fData[i].diffFourierRe->Draw("plsame");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_IMAG:
      // plot first histo
      fData[0].diffFourierIm->Draw("pl");

      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        min = fFourier.fPlotRange[0];
        max = fFourier.fPlotRange[1];
      } else {
        min = fData[0].diffFourierIm->GetBinLowEdge(1);
        max = fData[0].diffFourierIm->GetBinLowEdge(fData[0].diffFourierIm->GetNbinsX())+fData[0].diffFourierIm->GetBinWidth(1);
      }
      fData[0].diffFourierIm->GetXaxis()->SetRangeUser(min, max);

      // set y-range
      // first find minimum/maximum of all histos
      min = GetGlobalMinimum(fData[0].diffFourierIm);
      max = GetGlobalMaximum(fData[0].diffFourierIm);
      for (unsigned int i=1; i<fData.size(); i++) {
        binContent = GetGlobalMinimum(fData[i].diffFourierIm);
        if (binContent < min)
          min = binContent;
        binContent = GetGlobalMaximum(fData[i].diffFourierIm);
        if (binContent > max)
          max = binContent;
      }
      fData[0].diffFourierIm->GetYaxis()->SetRangeUser(1.05*min, 1.05*max);

      // set x-axis title
      fData[0].diffFourierIm->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fData[0].diffFourierIm->GetYaxis()->SetTitle("Imaginary Fourier");

      // plot all remaining data
      for (unsigned int i=1; i<fData.size(); i++) {
        fData[i].diffFourierIm->Draw("plsame");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_REAL_AND_IMAG:
      // plot first histo
      fData[0].diffFourierRe->Draw("pl");

      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        min = fFourier.fPlotRange[0];
        max = fFourier.fPlotRange[1];
      } else {
        min = fData[0].diffFourierRe->GetBinLowEdge(1);
        max = fData[0].diffFourierRe->GetBinLowEdge(fData[0].diffFourierRe->GetNbinsX())+fData[0].diffFourierRe->GetBinWidth(1);
      }
      fData[0].diffFourierRe->GetXaxis()->SetRangeUser(min, max);

      // set y-range
      // first find minimum/maximum of all histos
      min = GetGlobalMinimum(fData[0].diffFourierRe);
      max = GetGlobalMaximum(fData[0].diffFourierRe);
      for (unsigned int i=1; i<fData.size(); i++) {
        binContent = GetGlobalMinimum(fData[i].diffFourierRe);
        if (binContent < min)
          min = binContent;
        binContent = GetGlobalMaximum(fData[i].diffFourierRe);
        if (binContent > max)
          max = binContent;
      }
      for (unsigned int i=0; i<fData.size(); i++) {
        binContent = GetGlobalMinimum(fData[i].diffFourierIm);
        if (binContent < min)
          min = binContent;
        binContent = GetGlobalMaximum(fData[i].diffFourierIm);
        if (binContent > max)
          max = binContent;
      }
      fData[0].diffFourierRe->GetYaxis()->SetRangeUser(1.05*min, 1.05*max);

      // set x-axis title
      fData[0].diffFourierRe->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fData[0].diffFourierRe->GetYaxis()->SetTitle("Real+Imag Fourier");

      // plot all remaining data
      fData[0].diffFourierIm->Draw("plsame");
      for (unsigned int i=1; i<fData.size(); i++) {
        fData[i].diffFourierRe->Draw("plsame");
        fData[i].diffFourierIm->Draw("plsame");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_PWR:
      // plot first histo
      fData[0].diffFourierPwr->Draw("pl");

      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        min = fFourier.fPlotRange[0];
        max = fFourier.fPlotRange[1];
      } else {
        min = fData[0].diffFourierPwr->GetBinLowEdge(1);
        max = fData[0].diffFourierPwr->GetBinLowEdge(fData[0].diffFourierPwr->GetNbinsX())+fData[0].diffFourierPwr->GetBinWidth(1);
      }
      fData[0].diffFourierPwr->GetXaxis()->SetRangeUser(min, max);

      // set y-range
      // first find minimum/maximum of all histos
      min = GetGlobalMinimum(fData[0].diffFourierPwr);
      max = GetGlobalMaximum(fData[0].diffFourierPwr);
      for (unsigned int i=1; i<fData.size(); i++) {
        binContent = GetGlobalMinimum(fData[i].diffFourierPwr);
        if (binContent < min)
          min = binContent;
        binContent = GetGlobalMaximum(fData[i].diffFourierPwr);
        if (binContent > max)
          max = binContent;
      }
      fData[0].diffFourierPwr->GetYaxis()->SetRangeUser(1.05*min, 1.05*max);

      // set x-axis title
      fData[0].diffFourierPwr->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fData[0].diffFourierPwr->GetYaxis()->SetTitle("Power Fourier");

      // plot all remaining data
      for (unsigned int i=1; i<fData.size(); i++) {
        fData[i].diffFourierPwr->Draw("plsame");
      }

      PlotFourierPhaseValue();

      break;
    case PV_FOURIER_PHASE:
      // plot first histo
      fData[0].diffFourierPhase->Draw("pl");

      // set x-range
      if ((fFourier.fPlotRange[0] != -1) && (fFourier.fPlotRange[1] != -1)) {
        min = fFourier.fPlotRange[0];
        max = fFourier.fPlotRange[1];
      } else {
        min = fData[0].diffFourierPhase->GetBinLowEdge(1);
        max = fData[0].diffFourierPhase->GetBinLowEdge(fData[0].diffFourierPhase->GetNbinsX())+fData[0].diffFourierPhase->GetBinWidth(1);
      }
      fData[0].diffFourierPhase->GetXaxis()->SetRangeUser(min, max);

      // set y-range
      // first find minimum/maximum of all histos
      min = GetGlobalMinimum(fData[0].diffFourierPhase);
      max = GetGlobalMaximum(fData[0].diffFourierPhase);
      for (unsigned int i=1; i<fData.size(); i++) {
        binContent = GetGlobalMinimum(fData[i].diffFourierPhase);
        if (binContent < min)
          min = binContent;
        binContent = GetGlobalMaximum(fData[i].diffFourierPhase);
        if (binContent > max)
          max = binContent;
      }
      fData[0].diffFourierPhase->GetYaxis()->SetRangeUser(1.05*min, 1.05*max);

      // set x-axis title
      fData[0].diffFourierPhase->GetXaxis()->SetTitle(xAxisTitle.Data());

      // set y-axis title
      fData[0].diffFourierPhase->GetYaxis()->SetTitle("Phase Fourier");

      // plot all remaining data
      for (unsigned int i=1; i<fData.size(); i++) {
        fData[i].diffFourierPhase->Draw("plsame");
      }

      PlotFourierPhaseValue();

      break;
    default:
      break;
   }

  fDataTheoryPad->Update();

  fMainCanvas->cd();
  fMainCanvas->Update();
}

//--------------------------------------------------------------------------
// PlotFourierPhaseValue (private)
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
    if ((fData[i].diffFourierRe != 0) && (fData[i].diffFourierIm != 0)) {
      for (int j=0; j<fData[i].diffFourierRe->GetNbinsX(); j++) { // loop over a fourier diff data set
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
    if ((fData[i].diffFourierRe != 0) && (fData[i].diffFourierIm != 0)) {
      for (int j=0; j<fData[i].diffFourierRe->GetNbinsX(); j++) { // loop over a fourier diff data set
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
// SaveDataAscii (private)
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
            // write header
            str = TString("% ");
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str += TString(" Field (G)");
                break;
              case FOURIER_UNIT_FREQ:
                str += TString(" Frequency (MHz)");
                break;
              case FOURIER_UNIT_CYCLES:
                str += TString(" Angular Frequency (Mc/s)");
                break;
              default:
                str += TString(" ????");
                break;
            }
            str += TString(", ");
            fout << endl << str.Data();
            for (unsigned int j=0; j<fData.size()-1; j++) {
              fout << "RealFourierData" << j << ", RealFourierTheo" << j << ", ";
            }
            fout << "RealFourierData" << fData.size()-1 << ", RealFourierTheo" << fData.size()-1;
            fout << endl;
            // get current x-range
            xminBin = fData[0].dataFourierRe->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierRe->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierRe->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierRe->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=1; i<fData[0].dataFourierRe->GetNbinsX()-1; i++) {
              xval = fData[0].dataFourierRe->GetBinCenter(i); // get x-unit
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ", ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].dataFourierRe->GetBinContent(i) << ", ";
                theoBin = fData[j].theoryFourierRe->FindBin(xval);
                fout << fData[j].theoryFourierRe->GetBinContent(theoBin) << ", ";
              }
              // write last data set
              fout << fData[fData.size()-1].dataFourierRe->GetBinContent(i) << ", ";
              theoBin = fData[fData.size()-1].theoryFourierRe->FindBin(xval);
              fout << fData[fData.size()-1].theoryFourierRe->GetBinContent(theoBin);
              fout << endl;
            }
            break;
          case PV_FOURIER_IMAG:
            // write header
            str = TString("% ");
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str += TString(" Field (G)");
                break;
              case FOURIER_UNIT_FREQ:
                str += TString(" Frequency (MHz)");
                break;
              case FOURIER_UNIT_CYCLES:
                str += TString(" Angular Frequency (Mc/s)");
                break;
              default:
                str += TString(" ????");
                break;
            }
            str += TString(", ");
            fout << endl << str.Data();
            for (unsigned int j=0; j<fData.size()-1; j++) {
              fout << "ImagFourierData" << j << ", ImagFourierTheo" << j << ", ";
            }
            fout << "ImagFourierData" << fData.size()-1 << ", ImagFourierTheo" << fData.size()-1;
            fout << endl;
            // get current x-range
            xminBin = fData[0].dataFourierIm->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierIm->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierIm->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierIm->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=1; i<fData[0].dataFourierIm->GetNbinsX()-1; i++) {
              xval = fData[0].dataFourierIm->GetBinCenter(i); // get x-unit
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ", ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].dataFourierIm->GetBinContent(i) << ", ";
                theoBin = fData[j].theoryFourierIm->FindBin(xval);
                fout << fData[j].theoryFourierIm->GetBinContent(theoBin) << ", ";
              }
              // write last data set
              fout << fData[fData.size()-1].dataFourierIm->GetBinContent(i) << ", ";
              theoBin = fData[fData.size()-1].theoryFourierIm->FindBin(xval);
              fout << fData[fData.size()-1].theoryFourierIm->GetBinContent(theoBin);
              fout << endl;
            }
            break;
          case PV_FOURIER_REAL_AND_IMAG:
            // write header
            str = TString("% ");
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str += TString(" Field (G)");
                break;
              case FOURIER_UNIT_FREQ:
                str += TString(" Frequency (MHz)");
                break;
              case FOURIER_UNIT_CYCLES:
                str += TString(" Angular Frequency (Mc/s)");
                break;
              default:
                str += TString(" ????");
                break;
            }
            str += TString(", ");
            fout << endl << str.Data();
            for (unsigned int j=0; j<fData.size()-1; j++) {
              fout << "RealFourierData" << j << ", RealFourierTheo" << j << ", ";
              fout << "ImagFourierData" << j << ", ImagFourierTheo" << j << ", ";
            }
            fout << "RealFourierData" << fData.size()-1 << ", RealFourierTheo" << fData.size()-1 << ", ";
            fout << "ImagFourierData" << fData.size()-1 << ", ImagFourierTheo" << fData.size()-1;
            fout << endl;
            // get current x-range
            xminBin = fData[0].dataFourierRe->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierRe->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierRe->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierRe->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=1; i<fData[0].dataFourierRe->GetNbinsX()-1; i++) {
              xval = fData[0].dataFourierRe->GetBinCenter(i); // get x-unit
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ", ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].dataFourierRe->GetBinContent(i) << ", ";
                theoBin = fData[j].theoryFourierRe->FindBin(xval);
                fout << fData[j].theoryFourierRe->GetBinContent(theoBin) << ", ";
                fout << fData[j].dataFourierIm->GetBinContent(i) << ", ";
                theoBin = fData[j].theoryFourierIm->FindBin(xval);
                fout << fData[j].theoryFourierIm->GetBinContent(theoBin) << ", ";
              }
              // write last data set
              fout << fData[fData.size()-1].dataFourierRe->GetBinContent(i) << ", ";
              theoBin = fData[fData.size()-1].theoryFourierRe->FindBin(xval);
              fout << fData[fData.size()-1].theoryFourierRe->GetBinContent(theoBin) << ", ";
              fout << fData[fData.size()-1].dataFourierIm->GetBinContent(i) << ", ";
              theoBin = fData[fData.size()-1].theoryFourierIm->FindBin(xval);
              fout << fData[fData.size()-1].theoryFourierIm->GetBinContent(theoBin);
              fout << endl;
            }
            break;
          case PV_FOURIER_PWR:
            // write header
            str = TString("% ");
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str += TString(" Field (G)");
                break;
              case FOURIER_UNIT_FREQ:
                str += TString(" Frequency (MHz)");
                break;
              case FOURIER_UNIT_CYCLES:
                str += TString(" Angular Frequency (Mc/s)");
                break;
              default:
                str += TString(" ????");
                break;
            }
            str += TString(", ");
            fout << endl << str.Data();
            for (unsigned int j=0; j<fData.size()-1; j++) {
              fout << "PwrFourierData" << j << ", PwrFourierTheo" << j << ", ";
            }
            fout << "PwrFourierData" << fData.size()-1 << ", PwrFourierTheo" << fData.size()-1;
            fout << endl;
            // get current x-range
            xminBin = fData[0].dataFourierPwr->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierPwr->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierPwr->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierPwr->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=1; i<fData[0].dataFourierPwr->GetNbinsX()-1; i++) {
              xval = fData[0].dataFourierPwr->GetBinCenter(i); // get x-unit
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ", ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].dataFourierPwr->GetBinContent(i) << ", ";
                theoBin = fData[j].theoryFourierPwr->FindBin(xval);
                fout << fData[j].theoryFourierPwr->GetBinContent(theoBin) << ", ";
              }
              // write last data set
              fout << fData[fData.size()-1].dataFourierPwr->GetBinContent(i) << ", ";
              theoBin = fData[fData.size()-1].theoryFourierPwr->FindBin(xval);
              fout << fData[fData.size()-1].theoryFourierPwr->GetBinContent(theoBin);
              fout << endl;
            }
            break;
          case PV_FOURIER_PHASE:
            // write header
            str = TString("% ");
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str += TString(" Field (G)");
                break;
              case FOURIER_UNIT_FREQ:
                str += TString(" Frequency (MHz)");
                break;
              case FOURIER_UNIT_CYCLES:
                str += TString(" Angular Frequency (Mc/s)");
                break;
              default:
                str += TString(" ????");
                break;
            }
            str += TString(", ");
            fout << endl << str.Data();
            for (unsigned int j=0; j<fData.size()-1; j++) {
              fout << "PhaseFourierData" << j << ", PhaseFourierTheo" << j << ", ";
            }
            fout << "PhaseFourierData" << fData.size()-1 << ", PhaseFourierTheo" << fData.size()-1;
            fout << endl;
            // get current x-range
            xminBin = fData[0].dataFourierPhase->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierPhase->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierPhase->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierPhase->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=1; i<fData[0].dataFourierPhase->GetNbinsX()-1; i++) {
              xval = fData[0].dataFourierPhase->GetBinCenter(i); // get x-unit
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ", ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].dataFourierPhase->GetBinContent(i) << ", ";
                theoBin = fData[j].theoryFourierPhase->FindBin(xval);
                fout << fData[j].theoryFourierPhase->GetBinContent(theoBin) << ", ";
              }
              // write last data set
              fout << fData[fData.size()-1].dataFourierPhase->GetBinContent(i) << ", ";
              theoBin = fData[fData.size()-1].theoryFourierPhase->FindBin(xval);
              fout << fData[fData.size()-1].theoryFourierPhase->GetBinContent(theoBin);
              fout << endl;
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

  // close file
  fout.close();

  cout << endl << ">> Data windows saved in ascii format ..." << endl;
}

//--------------------------------------------------------------------------
// SaveDataDb (private)
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
              fout << "data" << j << " " << "theo" << j << " ";
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
            // write header
            fout << "TITLE" << endl;
            fout << "  " << fMsrHandler->GetMsrTitle()->Data() << endl;
            fout << "LABELS" << endl;
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str = TString("  Field (G)");
                break;
              case FOURIER_UNIT_FREQ:
                str = TString("  Frequency (MHz)");
                break;
              case FOURIER_UNIT_CYCLES:
                str = TString("  Angular Frequency (Mc/s)");
                break;
              default:
                str = TString("  ????");
                break;
            }
            fout << str.Data() << endl;
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "  DataFourierRe" << j << endl;
              fout << "  TheoFourierRe" << j << endl;
            }
            fout << endl;
            fout << "DATA ";
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str = TString("field");
                break;
              case FOURIER_UNIT_FREQ:
                str = TString("freq");
                break;
              case FOURIER_UNIT_CYCLES:
                str = TString("cycles");
                break;
              default:
                str = TString("????");
                break;
            }
            fout << str.Data() << " ";
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "dataFourierRe" << j << " " << "theoFourierRe" << j << " ";
            }
            fout << endl;
            fout << "\\-e" << endl;

            // get current x-range
            xminBin = fData[0].dataFourierRe->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierRe->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierRe->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierRe->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=1; i<fData[0].dataFourierRe->GetNbinsX()-1; i++) {
              xval = fData[0].dataFourierRe->GetBinCenter(i); // get x-val
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ",,, ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].dataFourierRe->GetBinContent(i) << ",,, ";
                theoBin = fData[j].theoryFourierRe->FindBin(xval);
                fout << fData[j].theoryFourierRe->GetBinContent(theoBin) << ",,, ";
              }
              // write last data set
              fout << fData[fData.size()-1].dataFourierRe->GetBinContent(i) << ",,, ";
              theoBin = fData[fData.size()-1].theoryFourierRe->FindBin(xval);
              fout << fData[fData.size()-1].theoryFourierRe->GetBinContent(theoBin) << ",,, ";
              fout << endl;
            }
            break;
          case PV_FOURIER_IMAG:
            // write header
            fout << "TITLE" << endl;
            fout << "  " << fMsrHandler->GetMsrTitle()->Data() << endl;
            fout << "LABELS" << endl;
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str = TString("  Field (G)");
                break;
              case FOURIER_UNIT_FREQ:
                str = TString("  Frequency (MHz)");
                break;
              case FOURIER_UNIT_CYCLES:
                str = TString("  Angular Frequency (Mc/s)");
                break;
              default:
                str = TString("  ????");
                break;
            }
            fout << str.Data() << endl;
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "  DataFourierIm" << j << endl;
              fout << "  TheoFourierIm" << j << endl;
            }
            fout << endl;
            fout << "DATA ";
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str = TString("field");
                break;
              case FOURIER_UNIT_FREQ:
                str = TString("freq");
                break;
              case FOURIER_UNIT_CYCLES:
                str = TString("cycles");
                break;
              default:
                str = TString("????");
                break;
            }
            fout << str.Data() << " ";
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "dataFourierIm" << j << " " << "theoFourierIm" << j << " ";
            }
            fout << endl;
            fout << "\\-e" << endl;

            // get current x-range
            xminBin = fData[0].dataFourierIm->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierIm->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierIm->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierIm->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=1; i<fData[0].dataFourierIm->GetNbinsX()-1; i++) {
              xval = fData[0].dataFourierIm->GetBinCenter(i); // get x-val
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ",,, ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].dataFourierIm->GetBinContent(i) << ",,, ";
                theoBin = fData[j].theoryFourierIm->FindBin(xval);
                fout << fData[j].theoryFourierIm->GetBinContent(theoBin) << ",,, ";
              }
              // write last data set
              fout << fData[fData.size()-1].dataFourierIm->GetBinContent(i) << ",,, ";
              theoBin = fData[fData.size()-1].theoryFourierIm->FindBin(xval);
              fout << fData[fData.size()-1].theoryFourierIm->GetBinContent(theoBin) << ",,, ";
              fout << endl;
            }
            break;
          case PV_FOURIER_REAL_AND_IMAG:
            // write header
            fout << "TITLE" << endl;
            fout << "  " << fMsrHandler->GetMsrTitle()->Data() << endl;
            fout << "LABELS" << endl;
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str = TString("  Field (G)");
                break;
              case FOURIER_UNIT_FREQ:
                str = TString("  Frequency (MHz)");
                break;
              case FOURIER_UNIT_CYCLES:
                str = TString("  Angular Frequency (Mc/s)");
                break;
              default:
                str = TString("  ????");
                break;
            }
            fout << str.Data() << endl;
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "  DataFourierRe" << j << endl;
              fout << "  TheoFourierRe" << j << endl;
              fout << "  DataFourierIm" << j << endl;
              fout << "  TheoFourierIm" << j << endl;
            }
            fout << endl;
            fout << "DATA ";
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str = TString("field");
                break;
              case FOURIER_UNIT_FREQ:
                str = TString("freq");
                break;
              case FOURIER_UNIT_CYCLES:
                str = TString("cycles");
                break;
              default:
                str = TString(" ????");
                break;
            }
            fout << str.Data() << " ";
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "dataFourierRe" << j << " " << "theoFourierRe" << j << " " << "dataFourierIm" << j << " " << "theoFourierIm" << j << " ";
            }
            fout << endl;
            fout << "\\-e" << endl;

            // get current x-range
            xminBin = fData[0].dataFourierRe->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierRe->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierRe->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierRe->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=1; i<fData[0].dataFourierRe->GetNbinsX()-1; i++) {
              xval = fData[0].dataFourierRe->GetBinCenter(i); // get x-val
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ",,, ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].dataFourierRe->GetBinContent(i) << ",,, ";
                theoBin = fData[j].theoryFourierRe->FindBin(xval);
                fout << fData[j].theoryFourierRe->GetBinContent(theoBin) << ",,, ";
                fout << fData[j].dataFourierIm->GetBinContent(i) << ",,, ";
                theoBin = fData[j].theoryFourierIm->FindBin(xval);
                fout << fData[j].theoryFourierIm->GetBinContent(theoBin) << ",,, ";
              }
              // write last data set
              fout << fData[fData.size()-1].dataFourierRe->GetBinContent(i) << ",,, ";
              theoBin = fData[fData.size()-1].theoryFourierRe->FindBin(xval);
              fout << fData[fData.size()-1].theoryFourierRe->GetBinContent(theoBin) << ",,, ";
              fout << fData[fData.size()-1].dataFourierIm->GetBinContent(i) << ",,, ";
              theoBin = fData[fData.size()-1].theoryFourierIm->FindBin(xval);
              fout << fData[fData.size()-1].theoryFourierIm->GetBinContent(theoBin) << ",,, ";
              fout << endl;
            }
            break;
          case PV_FOURIER_PWR:
            // write header
            fout << "TITLE" << endl;
            fout << "  " << fMsrHandler->GetMsrTitle()->Data() << endl;
            fout << "LABELS" << endl;
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str = TString("  Field (G)");
                break;
              case FOURIER_UNIT_FREQ:
                str = TString("  Frequency (MHz)");
                break;
              case FOURIER_UNIT_CYCLES:
                str = TString("  Angular Frequency (Mc/s)");
                break;
              default:
                str = TString("  ????");
                break;
            }
            fout << str.Data() << endl;
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "  DataFourierPwr" << j << endl;
              fout << "  TheoFourierPwr" << j << endl;
            }
            fout << endl;
            fout << "DATA ";
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str = TString("field");
                break;
              case FOURIER_UNIT_FREQ:
                str = TString("freq");
                break;
              case FOURIER_UNIT_CYCLES:
                str = TString("cycles");
                break;
              default:
                str = TString("????");
                break;
            }
            fout << str.Data() << " ";
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "dataFourierPwr" << j << " " << "theoFourierPwr" << j << " ";
            }
            fout << endl;
            fout << "\\-e" << endl;

            // get current x-range
            xminBin = fData[0].dataFourierPwr->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierPwr->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierPwr->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierPwr->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=1; i<fData[0].dataFourierPwr->GetNbinsX()-1; i++) {
              xval = fData[0].dataFourierPwr->GetBinCenter(i); // get x-val
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ",,, ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].dataFourierPwr->GetBinContent(i) << ",,, ";
                theoBin = fData[j].theoryFourierPwr->FindBin(xval);
                fout << fData[j].theoryFourierPwr->GetBinContent(theoBin) << ",,, ";
              }
              // write last data set
              fout << fData[fData.size()-1].dataFourierPwr->GetBinContent(i) << ",,, ";
              theoBin = fData[fData.size()-1].theoryFourierPwr->FindBin(xval);
              fout << fData[fData.size()-1].theoryFourierPwr->GetBinContent(theoBin) << ",,, ";
              fout << endl;
            }
            break;
          case PV_FOURIER_PHASE:
            // write header
            fout << "TITLE" << endl;
            fout << "  " << fMsrHandler->GetMsrTitle()->Data() << endl;
            fout << "LABELS" << endl;
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str = TString("  Field (G)");
                break;
              case FOURIER_UNIT_FREQ:
                str = TString("  Frequency (MHz)");
                break;
              case FOURIER_UNIT_CYCLES:
                str = TString("  Angular Frequency (Mc/s)");
                break;
              default:
                str = TString("  ????");
                break;
            }
            fout << str.Data() << endl;
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "  DataFourierPhase" << j << endl;
              fout << "  TheoFourierPhase" << j << endl;
            }
            fout << endl;
            fout << "DATA ";
            switch (fFourier.fUnits) {
              case FOURIER_UNIT_FIELD:
                str = TString("field");
                break;
              case FOURIER_UNIT_FREQ:
                str = TString("freq");
                break;
              case FOURIER_UNIT_CYCLES:
                str = TString("cycles");
                break;
              default:
                str = TString("????");
                break;
            }
            fout << str.Data() << " ";
            for (unsigned int j=0; j<fData.size(); j++) {
              fout << "dataFourierPhase" << j << " " << "theoFourierPhase" << j << " ";
            }
            fout << endl;
            fout << "\\-e" << endl;

            // get current x-range
            xminBin = fData[0].dataFourierPhase->GetXaxis()->GetFirst(); // first bin of the zoomed range
            xmaxBin = fData[0].dataFourierPhase->GetXaxis()->GetLast();  // last bin of the zoomed range
            xmin = fData[0].dataFourierPhase->GetXaxis()->GetBinCenter(xminBin);
            xmax = fData[0].dataFourierPhase->GetXaxis()->GetBinCenter(xmaxBin);
            // get data
            for (int i=1; i<fData[0].dataFourierPhase->GetNbinsX()-1; i++) {
              xval = fData[0].dataFourierPhase->GetBinCenter(i); // get x-val
              if ((xval < xmin) || (xval > xmax))
                continue;
              fout << xval << ",,, ";
              for (unsigned int j=0; j<fData.size()-1; j++) {
                fout << fData[j].dataFourierPhase->GetBinContent(i) << ",,, ";
                theoBin = fData[j].theoryFourierPhase->FindBin(xval);
                fout << fData[j].theoryFourierPhase->GetBinContent(theoBin) << ",,, ";
              }
              // write last data set
              fout << fData[fData.size()-1].dataFourierPhase->GetBinContent(i) << ",,, ";
              theoBin = fData[fData.size()-1].theoryFourierPhase->FindBin(xval);
              fout << fData[fData.size()-1].theoryFourierPhase->GetBinContent(theoBin) << ",,, ";
              fout << endl;
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

  // close file
  fout.close();

  cout << endl << ">> Data windows saved in db format ..." << endl;
}
