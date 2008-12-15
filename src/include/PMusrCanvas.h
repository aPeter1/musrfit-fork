/***************************************************************************

  PMusrCanvas.h

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

#ifndef _PMUSRCANVAS_H_
#define _PMUSRCANVAS_H_

#include <TObject.h>
#include <TQObject.h>
#include <TStyle.h>
#include <TRootCanvas.h>
#include <TGMenu.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TLegend.h>
#include <TPad.h>
#include <TH1F.h>
#include <TGraphErrors.h>

#include "PMusr.h"
#ifndef __MAKECINT__
#include "PMsrHandler.h"
#include "PRunListCollection.h"
#endif // __MAKECINT__

#define YINFO  0.1
#define YTITLE 0.95
#define XTHEO  0.75

// Current Plot Views
#define PV_DATA          1
#define PV_FOURIER_REAL  2
#define PV_FOURIER_IMAG  3
#define PV_FOURIER_PWR   4
#define PV_FOURIER_PHASE 5

// Canvas menu id's
#define P_MENU_ID_FOURIER       10001
#define P_MENU_ID_DIFFERENCE    10002
#define P_MENU_ID_SAVE_DATA     10003

#define P_MENU_PLOT_OFFSET      1000

#define P_MENU_ID_FOURIER_REAL          100
#define P_MENU_ID_FOURIER_IMAG          101
#define P_MENU_ID_FOURIER_REAL_AND_IMAG 102
#define P_MENU_ID_FOURIER_PWR           103
#define P_MENU_ID_FOURIER_PHASE         104
#define P_MENU_ID_FOURIER_PHASE_PLUS    105
#define P_MENU_ID_FOURIER_PHASE_MINUS   106

#define P_MENU_ID_SAVE_ASCII    200
#define P_MENU_ID_SAVE_DB       201

//------------------------------------------------------------------------
/**
 * <p>
 */
typedef struct {
  TH1F *data;
  TH1F *dataFourierRe;
  TH1F *dataFourierIm;
  TH1F *dataFourierPwr;
  TH1F *dataFourierPhase;
  TH1F *theory;
  TH1F *theoryFourierRe;
  TH1F *theoryFourierIm;
  TH1F *theoryFourierPwr;
  TH1F *theoryFourierPhase;
  TH1F *diff;
  TH1F *diffFourierRe;
  TH1F *diffFourierIm;
  TH1F *diffFourierPwr;
  TH1F *diffFourierPhase;
} PMusrCanvasDataSet;

//------------------------------------------------------------------------
/**
 * <p>
 */
typedef vector<PMusrCanvasDataSet> PMusrCanvasDataList;

//------------------------------------------------------------------------
/**
 * <p>
 */
typedef struct {
  TGraphErrors *data;
  TGraphErrors *dataFourierRe;
  TGraphErrors *dataFourierIm;
  TGraphErrors *dataFourierPwr;
  TGraphErrors *dataFourierPhase;
  TGraphErrors *theory;
  TGraphErrors *theoryFourierRe;
  TGraphErrors *theoryFourierIm;
  TGraphErrors *theoryFourierPwr;
  TGraphErrors *theoryFourierPhase;
  TGraphErrors *diff;
  TGraphErrors *diffFourierRe;
  TGraphErrors *diffFourierIm;
  TGraphErrors *diffFourierPwr;
  TGraphErrors *diffFourierPhase;
} PMusrCanvasNonMusrDataSet;

//------------------------------------------------------------------------
/**
 * <p>
 */
typedef vector<PMusrCanvasNonMusrDataSet> PMusrCanvasNonMusrDataList;

//--------------------------------------------------------------------------
/**
 * <p>The preprocessor tag __MAKECINT__ is used to hide away from rootcint
 * the overly complex spirit header files. 
 */
class PMusrCanvas : public TObject, public TQObject
{
  public:
    PMusrCanvas();
    PMusrCanvas(const int number, const char* title, 
                Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);
    PMusrCanvas(const int number, const char* title, 
                Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
                const PIntVector markerList, const PIntVector colorList);
    virtual ~PMusrCanvas();

    virtual Bool_t IsValid() { return fValid; }

#ifndef __MAKECINT__
    virtual void SetMsrHandler(PMsrHandler *msrHandler) { fMsrHandler = msrHandler; }
    virtual void SetRunListCollection(PRunListCollection *runList) { fRunList = runList; }
#endif // __MAKECINT__

    virtual void UpdateParamTheoryPad();
    virtual void UpdateDataTheoryPad();
    virtual void UpdateInfoPad();

    virtual void Done(Int_t status=0); // *SIGNAL*
    virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT
    virtual void HandleMenuPopup(Int_t id); // SLOT
    virtual void LastCanvasClosed(); // SLOT

  private:
    Bool_t fValid;
    Bool_t fDifferenceView;  /// tag showing that the shown data, fourier, are the difference between data and theory
    Int_t  fCurrentPlotView; /// tag showing what the current plot view is: data, fourier, ...
    Int_t  fPlotType;
    Int_t  fPlotNumber;

    TStyle    *fStyle;

    // canvas menu related variables
    TRootCanvas *fImp;
    TGMenuBar   *fBar;
    TGPopupMenu *fPopupMain;
    TGPopupMenu *fPopupSave;
    TGPopupMenu *fPopupFourier;

    // canvas related variables
    TCanvas   *fMainCanvas;
    TPaveText *fTitlePad;
    TPad      *fDataTheoryPad;
    TPaveText *fParameterTheoryPad;
    TLegend   *fInfoPad;

#ifndef __MAKECINT__
    PMsrHandler        *fMsrHandler;
    PRunListCollection *fRunList;
#endif // __MAKECINT__

    PMusrCanvasDataList fData;
    PMusrCanvasNonMusrDataList fNonMusrData;

    PIntVector fMarkerList;
    PIntVector fColorList;

    virtual void CreateStyle();
    virtual void InitMusrCanvas(const char* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);
    virtual void InitDataSet(PMusrCanvasDataSet &dataSet);
    virtual void InitDataSet(PMusrCanvasNonMusrDataSet &dataSet);
    virtual void CleanupDataSet(PMusrCanvasDataSet &dataSet);
    virtual void CleanupDataSet(PMusrCanvasNonMusrDataSet &dataSet);
    virtual void HandleDataSet(unsigned int plotNo, unsigned int runNo, PRunData *data);
    virtual void HandleNonMusrDataSet(unsigned int plotNo, unsigned int runNo, PRunData *data);
    virtual void HandleDifference();

    virtual double CalculateDiff(const double x, const double y, TH1F *theo);
    virtual double CalculateDiff(const double x, const double y, TGraphErrors *theo);
    virtual Int_t  FindBin(const double x, TGraphErrors *graph);

    virtual void PlotData();
    virtual void PlotDifference();
    virtual void PlotFourier(int fourierType);

    virtual void SaveDataAscii();
    virtual void SaveDataDb();

  ClassDef(PMusrCanvas, 1)
};

#endif // _PMUSRCANVAS_H_
