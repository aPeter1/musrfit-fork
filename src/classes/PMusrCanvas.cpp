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
#include <TRandom.h>
#include <TROOT.h>

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
  fDifferencePlot = false;
  fPlotNumber = -1;

  fStyle               = 0;
  fMainCanvas          = 0;
  fTitlePad            = 0;
  fDataTheoryPad       = 0;
  fParameterTheoryPad  = 0;
  fInfoPad             = 0;
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
  CreateStyle();
  InitMusrCanvas(title, wtopx, wtopy, ww, wh);
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PMusrCanvas::PMusrCanvas(const int number, const char* title,
                         Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
                         const PIntVector markerList, const PIntVector colorList) :
                         fPlotNumber(number), fMarkerList(markerList), fColorList(colorList)
{
  CreateStyle();
  InitMusrCanvas(title, wtopx, wtopy, ww, wh);
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
  fDifferencePlot = false;

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
//   cout << "px: "  << (char)fMainCanvas->GetEventX() << endl;

  TString str((Char_t)x);
  if (x == 'q') {
    Done(0);
  } else if (x == 'd') {
    HandleDifference();
  } else if (x == 'f') {
    cout << endl << ">> will show the Fourier transform, to be implemented yet." << endl;
  } else if (x == 'S') {
    cout << endl << ">> will save the shown data into a file, to be implemented yet." << endl;
  } else {
    // do all the necessary stuff **TO BE DONE**
    fMainCanvas->Update();
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
  // NonMusr axis titles
  TString xAxisTitle;
  TString yAxisTitle;

  // some checks first
  unsigned int runNo;
  PMsrPlotStructure plotInfo = fMsrHandler->GetMsrPlotList()->at(fPlotNumber);
  PMsrRunList runs = *fMsrHandler->GetMsrRunList();
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
    if (plotInfo.fPlotType != runs[runNo].fFitType) {
      fValid = false;
      cout << endl << "PMusrCanvas::UpdateDataTheoryPad: **ERROR** plottype = " << plotInfo.fPlotType << ", fittype = " << runs[runNo].fFitType << ", however they have to correspond!";
      cout << endl;
      return;
    }
    // check if NonMusr type plot and if yes get x- and y-axis title
    if (plotInfo.fPlotType == MSR_PLOT_NO_MUSR) {
      xAxisTitle = fRunList->GetXAxisTitle(runs[runNo].fRunName);
      yAxisTitle = fRunList->GetYAxisTitle(runs[runNo].fRunName);
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
      case MSR_FITTYPE_NO_MUSR:
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
  fDataTheoryPad->cd();
  if (plotInfo.fPlotType != MSR_PLOT_NO_MUSR) {
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
      switch (plotInfo.fPlotType) {
        case MSR_PLOT_SINGLE_HISTO:
          if (runList->at(0).fLifetimeCorrection) { // lifetime correction
            yAxisTitle = "asymmetry";
          } else { // no liftime correction
            yAxisTitle = "N(t) per bin";
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
  } else { // plotInfo.fPlotType == MSR_PLOT_NO_MUSR
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
  fMainCanvas->cd();
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
    if (isnan(dval)) {
      tstr += TString("??,");
    } else {
      sprintf(sval, "%0.2lf", dval);
      tstr += TString(sval) + TString("(K),");
    }
    // field if present
    tstr += TString("B=");
    dval = fRunList->GetField(runs[runNo].fRunName);
    if (isnan(dval)) {
      tstr += TString("??,");
    } else {
      sprintf(sval, "%0.2lf", dval);
      tstr += TString(sval) + TString("(G),");
    }
    // energy if present
    tstr += TString("E=");
    dval = fRunList->GetEnergy(runs[runNo].fRunName);
    if (isnan(dval)) {
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

  if (fDifferencePlot)
    fDifferencePlot = kFALSE;
  else
    fDifferencePlot = kTRUE;

  // difference plot wished hence feed difference data and plot them 
  if (fDifferencePlot) {
    // check if it is necessary to calculate diff data
    // get current x-range
  }

  // switch back to the "normal" view
  if (!fDifferencePlot) {
  }
}
