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
  fPlotNumber = -1;

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
PMusrCanvas::PMusrCanvas(const int number, const char* title,
                         Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh) :
                         fPlotNumber(number)
{
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
cout << "~PMusrCanvas() called" << endl;
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
  if (fData.size() > 0) {
    for (unsigned int i=0; i<fData.size(); i++)
      CleanupDataSet(fData[i]);
    fData.clear();
  }
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

  fMainCanvas          = 0;
  fTitlePad            = 0;
  fDataTheoryPad       = 0;
  fParameterTheoryPad  = 0;
  fInfoPad             = 0;
  fKeyboardHandlerText = 0;

  // invoke canvas
  TString canvasName = TString("fMainCanvas");
  canvasName += fPlotNumber;
  fMainCanvas = new TCanvas(canvasName.Data(), title, wtopx, wtopy, ww, wh);
cout << canvasName.Data() << " = " << fMainCanvas << endl;
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

// cout << "this                 " << this << endl;
// cout << "fMainCanvas          " << fMainCanvas << endl;
// cout << "fTitlePad            " << fTitlePad << endl;
// cout << "fDataTheoryPad       " << fDataTheoryPad << endl;
// cout << "fParameterTheoryPad  " << fParameterTheoryPad << endl;
// cout << "fInfoPad             " << fInfoPad << endl;
// cout << "fKeyboardHandlerText " << fKeyboardHandlerText << endl;
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
      if (round(param[i].fStep)-param[i].fStep==0)
        sprintf(cnum, "%.1lf", param[i].fStep);
      else
        sprintf(cnum, "%.6lf", param[i].fStep);
      str += cnum;
      str += "/";
      if (round(param[i].fPosError)-param[i].fPosError==0)
        sprintf(cnum, "%.1lf", param[i].fPosError);
      else
        sprintf(cnum, "%.6lf", param[i].fPosError);
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
        HandleSingleHistoDataSet(runNo, data);
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
        HandleAsymmetryDataSet(runNo, data);
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
        HandleRRFDataSet(runNo, data);
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
        HandleNoneMusrDataSet(runNo, data);
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
  if (fData.size() > 0) {
    fData[0].data->Draw("pe1");
    // set time range
    Double_t xmin = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmin;
    Double_t xmax = fMsrHandler->GetMsrPlotList()->at(fPlotNumber).fTmax;
    fData[0].data->GetXaxis()->SetRangeUser(xmin, xmax);
    // check if it is necessary to set the y-axis range
    // plot all remaining data
    for (unsigned int i=1; i<fData.size(); i++) {
      fData[i].data->Draw("pe1same");
    }
    // plot all the theory
    for (unsigned int i=0; i<fData.size(); i++) {
      fData[i].theory->Draw("csame");
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

  // get run plot info

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
// HandleSingleHistoDataSet
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runNo
 * \param data
 */
void PMusrCanvas::HandleSingleHistoDataSet(unsigned int runNo, PRunData *data)
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
  start = data->fDataTimeStart;
  end   = data->fDataTimeStart + (data->fValue.size()-1)*data->fDataTimeStep;

  // invoke histo
  dataHisto = new TH1F(name, name, data->fValue.size(), start, end);

  // check if lifetimecorrection is set
  if (fMsrHandler->GetMsrRunList()->at(runNo).fLifetimeCorrection) {
    // get background
    // get lifetime
    // fill histogram
  } else { // no lifetimecorrection
    // fill histogram
    int pack = fMsrHandler->GetMsrRunList()->at(runNo).fPacking;
    for (unsigned int i=0; i<data->fValue.size(); i++) {
      dataHisto->SetBinContent(i, pack*data->fValue[i]);
      dataHisto->SetBinError(i, TMath::Sqrt(pack)*data->fError[i]);
    }
  }

  // set marker and line color
  if (runNo < fColorList.size()) {
    dataHisto->SetMarkerColor(fColorList[runNo]);
    dataHisto->SetLineColor(fColorList[runNo]);
  } else {
    TRandom rand(runNo);
    Int_t color = TColor::GetColor((Int_t)rand.Integer(255), (Int_t)rand.Integer(255), (Int_t)rand.Integer(255));
    dataHisto->SetMarkerColor(color);
    dataHisto->SetLineColor(color);
  }
  // set marker size
  dataHisto->SetMarkerSize(1);
  // set marker type
  if (runNo < fMarkerList.size()) {
    dataHisto->SetMarkerStyle(fMarkerList[runNo]);
  } else {
    TRandom rand(runNo);
    dataHisto->SetMarkerStyle(20+(Int_t)rand.Integer(10));
  }

  // theoHisto -------------------------------------------------------------
  // create histo specific infos
  name  = fMsrHandler->GetMsrRunList()->at(runNo).fRunName + "_TheoRunNo";
  name += (int)runNo;
  start = data->fTheoryTimeStart;
  end   = data->fTheoryTimeStart + (data->fTheory.size()-1)*data->fTheoryTimeStep;

  // invoke histo
  theoHisto = new TH1F(name, name, data->fTheory.size(), start, end);

  // check if lifetimecorrection is set
  if (fMsrHandler->GetMsrRunList()->at(runNo).fLifetimeCorrection) {
    // get background
    // get lifetime
    // fill histogram
  } else { // no lifetimecorrection
    // fill histogram
    int pack = fMsrHandler->GetMsrRunList()->at(runNo).fPacking;
    for (unsigned int i=0; i<data->fValue.size(); i++) {
      theoHisto->SetBinContent(i, pack*data->fTheory[i]);
    }
  }

  // set the line color
  if (runNo < fColorList.size()) {
    theoHisto->SetLineColor(fColorList[runNo]);
  } else {
    TRandom rand(runNo);
    Int_t color = TColor::GetColor((Int_t)rand.Integer(255), (Int_t)rand.Integer(255), (Int_t)rand.Integer(255));
    theoHisto->SetLineColor(color);
  }

  // fill handler list -----------------------------------------------------
  dataSet.data   = dataHisto;
  dataSet.theory = theoHisto;

  fData.push_back(dataSet);
}

//--------------------------------------------------------------------------
// HandleAsymmetryDataSet
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runNo
 * \param data
 */
void PMusrCanvas::HandleAsymmetryDataSet(unsigned int runNo, PRunData *data)
{
}

//--------------------------------------------------------------------------
// HandleRRFDataSet
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runNo
 * \param data
 */
void PMusrCanvas::HandleRRFDataSet(unsigned int runNo, PRunData *data)
{
}

//--------------------------------------------------------------------------
// HandleNoneMusrDataSet
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param runNo
 * \param data
 */
void PMusrCanvas::HandleNoneMusrDataSet(unsigned int runNo, PRunData *data)
{
}

