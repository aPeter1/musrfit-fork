/***************************************************************************

  PMuppCanvas.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2021 by Andreas Suter                              *
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

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <TColor.h>
#include <TROOT.h>
#include <TGFileDialog.h>
#include <TGMsgBox.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TRandom.h>
#include <TAxis.h>

#include "PMuppCanvas.h"

static const char *gFiletypes[] = { "Data files", "*.dat",
                                    "All files",  "*",
                                    0,            0 };

ClassImpQ(PMuppCanvas)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * @brief PMuppCanvas::PMuppCanvas
 */
PMuppCanvas::PMuppCanvas()
{
  fFtokName = TString("");
  fCheckMsgQueue = 0;

  fStyle = 0;
  fImp = 0;
  fBar = 0;
  fPopupMain = 0;

  fMainCanvas = 0;
  fMultiGraph = 0;

  gStyle->SetHistMinimumZero(kTRUE); // needed to enforce proper bar option handling
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 * @brief PMuppCanvas::PMuppCanvas
 * @param title
 * @param wtopx
 * @param wtopy
 * @param ww
 * @param wh
 * @param markerSytleList
 * @param markerSizeList
 * @param colorList
 */
PMuppCanvas::PMuppCanvas(const Char_t *title, Int_t wtopx, Int_t wtopy,
                         Int_t ww, Int_t wh, const PIntVector markerSytleList,
                         const PDoubleVector markerSizeList, const PIntVector colorList,
                         const int mupp_instance) :
  fMuppInstance(mupp_instance),
  fMarkerStyleList(markerSytleList),
  fMarkerSizeList(markerSizeList),
  fColorList(colorList)
{
  fValid = true;

  fFtokName = TString("");
  fCheckMsgQueue = 0;

  // install IPC message queue timer
  fCheckMsgQueue = new TTimer();
  if (fCheckMsgQueue == 0) {
    fValid = false;
    std::cerr << "**ERROR** couldn't start IPC message queue timer..." << std::endl;
    return;
  }
  fCheckMsgQueue->Connect("Timeout()", "PMuppCanvas", this, "CheckIPCMsgQueue()");
  fCheckMsgQueue->Start(200, kFALSE);

  CreateStyle();
  InitMuppCanvas(title, wtopx, wtopy, ww, wh);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 * @brief PMuppCanvas::~PMuppCanvas
 */
PMuppCanvas::~PMuppCanvas()
{
  if (fMainCanvas) {
    delete fMainCanvas;
  }
  if (fMultiGraph) {
    delete fMultiGraph;
  }
  if (fStyle) {
    delete fStyle;
  }
}

//--------------------------------------------------------------------------
// Done (SIGNAL)
//--------------------------------------------------------------------------
/**
 * <p>Signal emitted that the user wants to terminate the application.
 *
 * \param status Status info
 */
void PMuppCanvas::Done(Int_t status)
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
 * - 'q' quit mupp_plot
 *
 * \param event event type
 * \param x character key
 * \param y not used
 * \param selected not used
 */
void PMuppCanvas::HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected)
{
  if (event != kKeyPress)
     return;

  // handle keys and popup menu entries
  if (x == 'q') { // quit
    Done(0);
  } else if (x == 'b') { // about
    new TGMsgBox(gClient->GetRoot(), 0, "About mupp", "created by Andreas Suter\nPSI/NUM/LMU/LEM\n2018", kMBIconAsterisk);
  }
}

//--------------------------------------------------------------------------
// HandleMenuPopup (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>Handles the mupp menu.
 *
 * \param id identification key of the selected menu
 */
void PMuppCanvas::HandleMenuPopup(Int_t id)
{
  if (id == P_MENU_ID_EXPORT) {
    ExportData();
  } else if (id == P_MENU_ID_ABOUT) {
    new TGMsgBox(gClient->GetRoot(), 0, "About mupp", "created by Andreas Suter\nPSI/NUM/LMU/LEM\n2017", kMBIconAsterisk);
  }
}

//--------------------------------------------------------------------------
// LastCanvasClosed (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>Slot called when the last canvas has been closed. Will emit Done(0) which will
 * terminate the application.
 */
void PMuppCanvas::LastCanvasClosed()
{
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
void PMuppCanvas::WindowClosed()
{
  gROOT->GetListOfCanvases()->Remove(fMainCanvas);
  LastCanvasClosed();
}

//--------------------------------------------------------------------------
// CreateStyle (private)
//--------------------------------------------------------------------------
/**
 * <p> Set styles for the canvas. Perhaps one could transfer them to the startup-file in the future.
 */
void PMuppCanvas::CreateStyle()
{
  TString muppStyle("muppStyle");
  fStyle = new TStyle(muppStyle, muppStyle);
  fStyle->SetOptStat(0);  // no statistics options
  fStyle->SetOptTitle(0); // no title
  // set margins
  fStyle->SetPadLeftMargin(0.15);
  fStyle->SetPadRightMargin(0.05);
  fStyle->SetPadBottomMargin(0.12);
  fStyle->SetPadTopMargin(0.05);

  fStyle->cd();
}

//--------------------------------------------------------------------------
/**
 * @brief PMuppCanvas::InitMuppCanvas
 * @param title
 * @param wtopx
 * @param wtopy
 * @param ww
 * @param wh
 */
void PMuppCanvas::InitMuppCanvas(const Char_t *title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh)
{
  fValid = false;

  // invoke canvas
  TString canvasName = TString("fMuppCanvas");
  fMainCanvas = new TCanvas(canvasName.Data(), title, wtopx, wtopy, ww, wh);
  if (fMainCanvas == 0) {
    std::cerr << std::endl << ">> PMuppCanvas::InitMuppCanvas(): **PANIC ERROR** Couldn't invoke " << canvasName.Data();
    std::cerr << std::endl;
    return;
  }

  fMainCanvas->SetFillColor(0);

  fMultiGraph = 0;

  fImp = (TRootCanvas*)fMainCanvas->GetCanvasImp();
  fImp->Connect("CloseWindow()", "PMuppCanvas", this, "WindowClosed()");
  fBar = fImp->GetMenuBar();
  fPopupMain = fBar->AddPopup("&Mupp");

  fPopupMain->AddEntry("Export &Data", P_MENU_ID_EXPORT);
  fPopupMain->AddSeparator();
  fPopupMain->AddEntry("A&bout", P_MENU_ID_ABOUT);

  fBar->MapSubwindows();
  fBar->Layout();

  fPopupMain->Connect("TGPopupMenu", "Activated(Int_t)", "PMuppCanvas", this, "HandleMenuPopup(Int_t)");

  fValid = true;

  fMainCanvas->cd();
  fMainCanvas->Show();
  fMainCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "PMuppCanvas",
                       this, "HandleCmdKey(Int_t,Int_t,Int_t,TObject*)");
}

//--------------------------------------------------------------------------
/**
 * @brief PMuppCanvas::CheckIPCMsgQueue
 */
void PMuppCanvas::CheckIPCMsgQueue()
{
  int msqid, flags, type, key, status;
  ssize_t msgLen;
  struct mbuf msg;
  char str[1024];

  // get msqid
  if (fFtokName.IsWhitespace()) {
    strncpy(str, getenv("HOME"), sizeof(str)-1);
    if (strlen(str) == 0) {
      std::cerr << std::endl;
      std::cerr << "**ERROR** couldn't get value of the environment variable HOME." << std::endl << std::endl;
      return;
    }
    memset(str, '\0', sizeof(str));
    snprintf(str, sizeof(str), "%s/.musrfit/mupp/_mupp_ftok_%d.dat", getenv("HOME"), fMuppInstance);
    std::ifstream fin(str, std::ifstream::in);
    if (!fin.is_open()) {
      std::cerr << std::endl;
      std::cerr << "**ERROR** couldn't open " << str << std::endl;
      return;
    }
    fin.getline(str, 1024);
    fin.close();
    fFtokName = str;
  }

  key = ftok(fFtokName.Data(), fMuppInstance);
  flags = IPC_CREAT;
  msqid = msgget(key, flags | S_IRUSR | S_IWUSR);
  if (msqid == -1) {
    std::cerr << std::endl;
    std::cerr << "**ERROR** couldn't get msqid." << std::endl << std::endl;
    return;
  }

  // get message
  flags = IPC_NOWAIT;
  type = 1;
  msgLen = msgrcv(msqid, &msg, PMUPP_MAX_MTEXT, type, flags);

  if (msgLen > 0) {
    status = ReadPlotData(msg.mtext);
    if (status != 0) {
      std::cerr << "**ERROR** reading " << msg.mtext << std::endl;
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief PMuppCanvas::ReadPlotData
 * @param fln
 * @return
 */
int PMuppCanvas::ReadPlotData(const Char_t *fln)
{
  std::ifstream fin(fln, std::ifstream::in);
  char line[1024];

  if (!fin.is_open()) {
    std::cerr << "**ERROR** Couldn't open " << fln << std::endl;
    return -1;
  }

  TString str;
  TObjArray *tok;
  TObjString *ostr;
  Ssiz_t idx;
  Int_t noOfDataTokens=0;
  PDataCollection data;
  PDataPoint dataPoint;
  fPlotData.clear();
  while (fin.good()) {
    fin.getline(line, 1024);
    if (line[0] == '%') {
      if (strstr(line, "% collName")) {
        noOfDataTokens = 0;
        InitDataCollection(data); // make sure that one starts with a fresh collection
        str = line;
        idx = str.First("=");
        str = str.Remove(0,idx+2);
        data.name = str;
      } else if (strstr(line, "% end")) {
        fPlotData.push_back(data);
      }
    } else if (strlen(line) == 0) { // ignore empty lines
      continue;
    } else if (strstr(line, "xLabel:")) { // header information
      str = line;
      tok = str.Tokenize(":,");
      if (tok == 0) {
        std::cerr << "**ERROR** Couldn't tokenize the label line." << std::endl;
        fin.close();
        return -2;
      }
      if (tok->GetEntries() < 4) {
        std::cerr << "**ERROR** label line doesn't contain sufficient information." << std::endl;
        fin.close();
        return -3;
      }
      ostr = dynamic_cast<TObjString*>(tok->At(1));
      data.xLabel = ostr->GetString();
      noOfDataTokens++;
      for (Int_t i=3; i<tok->GetEntries(); i+=2) {
        ostr = dynamic_cast<TObjString*>(tok->At(i));
        data.yLabel.push_back(ostr->GetString());
        noOfDataTokens++;
      }

      // clean up
      delete tok;
      tok = 0;
    } else { // data lines
      str = line;
      tok = str.Tokenize(", ");
      if (tok == 0) {
        std::cerr << "**ERROR** Couldn't tokenize data line." << std::endl;
        fin.close();
        return -4;
      }
      if (tok->GetEntries() != (noOfDataTokens-1)*3+1) {
        std::cerr << "**ERROR** number of token in data line (" << tok->GetEntries() << ") is inconsistent with no of labels (" << noOfDataTokens << ")." << std::endl;
        fin.close();
        return -4;
      }

      // resize y-value vector properly
      data.yValue.resize(noOfDataTokens-1);

      // raw data point
      // x-value
      ostr = dynamic_cast<TObjString*>(tok->At(0));
      str = ostr->GetString();
      if (!str.IsFloat()) {
        std::cerr << "**ERROR** token found in data line is not a number (" << str << ")." << std::endl;
        fin.close();
        return -5;
      }
      data.xValue.push_back(str.Atof());

      // y-value(s)
      Int_t idx=0;
      for (Int_t i=1; i<tok->GetEntries(); i+=3) {
        // y-value
        ostr = dynamic_cast<TObjString*>(tok->At(i));
        str = ostr->GetString();
        if (!str.IsFloat()) {
          std::cerr << "**ERROR** token found in data line is not a number (" << str << ")." << std::endl;
          fin.close();
          return -5;
        }
        dataPoint.y = str.Atof();

        // pos y error-value
        ostr = dynamic_cast<TObjString*>(tok->At(i+1));
        str = ostr->GetString();
        if (!str.IsFloat()) {
          std::cerr << "**ERROR** token found in data line is not a number (" << str << ")." << std::endl;
          fin.close();
          return -5;
        }
        dataPoint.eYpos = str.Atof();

        // neg y error-value
        ostr = dynamic_cast<TObjString*>(tok->At(i+2));
        str = ostr->GetString();
        if (!str.IsFloat()) {
          std::cerr << "**ERROR** token found in data line is not a number (" << str << ")." << std::endl;
          fin.close();
          return -5;
        }
        dataPoint.eYneg = str.Atof();

        // push dataPoint back into data structure
        data.yValue[idx].push_back(dataPoint);
        idx++;
      }

      // clean up
      delete tok;
      tok = 0;
    }
  }

  fin.close();

  // check if data structure makes sense
  for (UInt_t i=0; i<fPlotData.size(); i++) {
    std::cerr << "debug> collection " << i << std::endl;
    std::cerr << "debug> name    : " << fPlotData[i].name << std::endl;
    std::cerr << "debug> x-label : " << fPlotData[i].xLabel << std::endl;
    std::cerr << "debug> x-vales : " << std::endl;
    std::cerr << "debug> ";
    for (UInt_t j=0; j<fPlotData[i].xValue.size(); j++) {
      std::cerr << fPlotData[i].xValue[j] << ", ";
    }
    std::cerr << std::endl;
    for (UInt_t j=0; j<fPlotData[i].yValue.size(); j++) {
      std::cerr << "debug> y-label : " << fPlotData[i].yLabel[j] << std::endl;
      std::cerr << "debug> y-values " << j << ": " << std::endl;
      for (UInt_t k=0; k<fPlotData[i].yValue[j].size(); k++) {
        std::cerr << "debug> " << k << ": " << fPlotData[i].yValue[j][k].y << "(" << fPlotData[i].yValue[j][k].eYneg << "/" << fPlotData[i].yValue[j][k].eYpos << ")" << std::endl;
      }
    }
    std::cerr << std::endl;
  }

  // feed graph objects
  UpdateGraphs();

  return 0;
}

//--------------------------------------------------------------------------
/**
 * @brief PMuppCanvas::InitDataCollection
 * @param coll
 */
void PMuppCanvas::InitDataCollection(PDataCollection &coll)
{
  coll.name = TString("");
  coll.xLabel = TString("");
  coll.yLabel.clear();
  coll.xValue.clear();
  coll.yValue.clear();
}

//--------------------------------------------------------------------------
/**
 * @brief PMuppCanvas::UpdateGraphs
 */
void PMuppCanvas::UpdateGraphs()
{
  // first: get rid of all previous plots
  for (UInt_t i=0; i<fGraphE.size(); i++) {
    delete fGraphE[i];
  }
  fGraphE.clear();

  if (fMultiGraph) {
    delete fMultiGraph;
    fMultiGraph = 0;
  }

  // second: create all the necessary graphs
  TGraphAsymmErrors *gg = 0;
  UInt_t idxS = 0, idxC = 0;
  Int_t color;
  TString str;
  if (fMultiGraph == 0) { // first time called
    for (UInt_t i=0; i<fPlotData.size(); i++) { // loop over all collections
      for (UInt_t j=0; j<fPlotData[i].yValue.size(); j++) { // loop over all graph's within the collection
        gg = new TGraphAsymmErrors(fPlotData[i].xValue.size());
        for (UInt_t k=0; k<fPlotData[i].yValue[j].size(); k++) {
          gg->SetPoint(k, fPlotData[i].xValue[k], fPlotData[i].yValue[j][k].y);
          gg->SetPointError(k, 0.0, 0.0, fabs(fPlotData[i].yValue[j][k].eYneg), fPlotData[i].yValue[j][k].eYpos);
          // set marker style and size
          if (idxS < fMarkerStyleList.size()) {
            gg->SetMarkerStyle(fMarkerStyleList[idxS]);
            gg->SetMarkerSize(fMarkerSizeList[idxS]);
          } else { // random choice of the marker
            // still missing as35
          }
          // set color
          if (idxC < fColorList.size()) {
            gg->SetMarkerColor(fColorList[idxC]);
            gg->SetLineColor(fColorList[idxC]);
            gg->SetFillColor(kWhite);
          } else { // random choise of the color
            TRandom rand(i+j+k);
            color = TColor::GetColor((Int_t)rand.Integer(255), (Int_t)rand.Integer(255), (Int_t)rand.Integer(255));
            gg->SetMarkerColor(color);
            gg->SetLineColor(color);
            gg->SetFillColor(kWhite);
          }
        }
        idxS++;
        idxC++;

        // set name
        str = fPlotData[i].yLabel[j];
        if (fPlotData[i].yValue.size() > 1) {
          str += "-";
          str += j;
        }
        gg->SetName(str);
        gg->SetTitle(str);

        // set x-axis title
        gg->GetXaxis()->SetTitle(fPlotData[i].xLabel);
        // set y-axis title
        gg->GetYaxis()->SetTitle(fPlotData[i].yLabel[j]);

        fGraphE.push_back(gg);
      }
    }

    fMultiGraph = new TMultiGraph();
    if (fMultiGraph == 0) {
      std::cerr << "**ERROR** couldn't create necessary TMultiGraph object." << std::endl;
      return;
    }

    if (fGraphE.size() == 0) {
      std::cerr << "**ERROR** NO graphs present! This should never happen." << std::endl;
      return;
    }

    for (UInt_t i=0; i<fGraphE.size(); i++) {
      fMultiGraph->Add(fGraphE[i], "p");
    }

    fMultiGraph->Draw("a");

    // set x-axis limits. This is needed that the graphs x-axis starts at 0.0
    Double_t xmin=fMultiGraph->GetXaxis()->GetXmin();
    Double_t xmax=fMultiGraph->GetXaxis()->GetXmax();
    if (xmin > 0.0) {
      fMultiGraph->GetXaxis()->SetLimits(0.0, xmax);
    }

    // set fMultiGraph x/y-axis title
    if (fGraphE.size() == 1) { // only one data set
      fMultiGraph->GetXaxis()->SetTitle(fGraphE[0]->GetXaxis()->GetTitle());
      fMultiGraph->GetXaxis()->SetTitleSize(0.05);
      fMultiGraph->GetYaxis()->SetTitle(fGraphE[0]->GetYaxis()->GetTitle());
      fMultiGraph->GetYaxis()->SetTitleOffset(1.25);
      fMultiGraph->GetYaxis()->SetTitleSize(0.05);
      fMultiGraph->GetYaxis()->SetLabelOffset(0.01);
      fMultiGraph->GetYaxis()->SetDecimals();
    } else { // multiple data sets
      fMultiGraph->GetXaxis()->SetTitle(fGraphE[0]->GetXaxis()->GetTitle()); // need a better solution, as35
      fMultiGraph->GetXaxis()->SetTitleSize(0.05);
      fMultiGraph->GetYaxis()->SetTitle(fGraphE[0]->GetYaxis()->GetTitle()); // need a better solution, as35
      fMultiGraph->GetYaxis()->SetTitleOffset(1.25);
      fMultiGraph->GetYaxis()->SetTitleSize(0.05);
      fMultiGraph->GetYaxis()->SetLabelOffset(0.01);
      fMultiGraph->GetYaxis()->SetDecimals();
    }
    gPad->Modified();

    fMainCanvas->cd();
    fMainCanvas->Update();
  } else { // update of the graphs
    // still missing, as35
  }
}

//--------------------------------------------------------------------------
/**
 * @brief PMuppCanvas::ExportData
 */
void PMuppCanvas::ExportData()
{
  static TString dir(".");
  TGFileInfo fi;
  fi.fFileTypes = gFiletypes;
  fi.fIniDir = StrDup(dir);
  fi.fOverwrite = true;
  new TGFileDialog(0, fImp, kFDSave, &fi);
  if (fi.fFilename && strlen(fi.fFilename)) {
    std::ofstream fout(fi.fFilename, std::ios_base::out);
    // write header
    fout << "% ";
    for (int i=0; i<fPlotData.size(); i++) {
      fout << fPlotData[i].xLabel.Data() << ", ";
      for (int j=0; j<fPlotData[i].yLabel.size(); j++) {
        if ((i == fPlotData.size()-1) && (j == fPlotData[i].yLabel.size()-1))
          fout << fPlotData[i].yLabel[j].Data() << ", " << fPlotData[i].yLabel[j].Data() << "ErrPos, " << fPlotData[i].yLabel[j].Data() << "ErrNeg";
        else
          fout << fPlotData[i].yLabel[j].Data() << ", " << fPlotData[i].yLabel[j].Data() << "ErrPos, " << fPlotData[i].yLabel[j].Data() << "ErrNeg, ";
      }
    }
    fout << std::endl;

    // search the longest data set
    Int_t maxLength=0;
    for (int i=0; i<fPlotData.size(); i++) {
      if (maxLength < fPlotData[i].xValue.size())
        maxLength = fPlotData[i].xValue.size();
    }
    // write data
    for (int i=0; i<maxLength; i++) { // maximal data set length
      for (int j=0; j<fPlotData.size(); j++) { // number of x-data sets
        // write x-value
        if (i < fPlotData[j].xValue.size()) // make sure that the entry exists
          fout << fPlotData[j].xValue[i] << ", ";
        else
          fout << " , ";
        // write y-value and y-value error
        for (int k=0; k<fPlotData[j].yValue.size(); k++) { // number of y-data sets
          if ((j == fPlotData.size()-1) && (k == fPlotData[j].yValue.size()-1))
            if (i < fPlotData[j].yValue[k].size())
              fout << fPlotData[j].yValue[k][i].y << ", " << fPlotData[j].yValue[k][i].eYpos << ", " << fPlotData[j].yValue[k][i].eYneg;
            else
              fout << ", , , ";
          else
            if (i < fPlotData[j].yValue[k].size())
              fout << fPlotData[j].yValue[k][i].y << ", " << fPlotData[j].yValue[k][i].eYpos << ", " << fPlotData[j].yValue[k][i].eYneg << ", ";
            else
              fout << ", , , ";
        }
      }
      fout << std::endl;
    }

    fout.close();
  }
}
