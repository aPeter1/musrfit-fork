/***************************************************************************
 *   Copyright (C) 2007-2015 by Andreas Suter                              *
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

#ifndef _PFOURIERCANVAS_H_
#define _PFOURIERCANVAS_H_

#include <TObject.h>
#include <TQObject.h>
#include <TTimer.h>
#include <TStyle.h>
#include <TRootCanvas.h>
#include <TGMenu.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TLegend.h>
#include <TPad.h>
#include <TH1F.h>
#include <TLatex.h>

#include "PMusr.h"
#include "PFourier.h"

// Canvas menu id's
#define P_MENU_ID_FOURIER       10001
#define P_MENU_ID_AVERAGE       10002
#define P_MENU_ID_SAVE_DATA     10003

#define P_MENU_ID_FOURIER_REAL          100
#define P_MENU_ID_FOURIER_IMAG          101
#define P_MENU_ID_FOURIER_REAL_AND_IMAG 102
#define P_MENU_ID_FOURIER_PWR           103
#define P_MENU_ID_FOURIER_PHASE         104
#define P_MENU_ID_FOURIER_PHASE_PLUS    105
#define P_MENU_ID_FOURIER_PHASE_MINUS   106

#define P_MENU_ID_SAVE_ASCII    200

//------------------------------------------------------------------------
/**
 * <p>Structure holding all necessary Fourier histograms.
 */
typedef struct {
  TH1F *dataFourierRe;       ///< real part of the Fourier transform of the data histogram
  TH1F *dataFourierIm;       ///< imaginary part of the Fourier transform of the data histogram
  TH1F *dataFourierPwr;      ///< power spectrum of the Fourier transform of the data histogram
  TH1F *dataFourierPhase;    ///< phase spectrum of the Fourier transform of the data histogram
} PFourierCanvasDataSet;

//------------------------------------------------------------------------
/**
 * <p>typedef to make to code more readable: list of histogram data sets.
 */
typedef vector<PFourierCanvasDataSet> PFourierCanvasDataList;

//--------------------------------------------------------------------------
/**
 * <p>
 */
class PFourierCanvas : public TObject, public TQObject
{
  public:
    PFourierCanvas();
    PFourierCanvas(vector<PFourier*> &fourier, const Char_t* title, const Bool_t showAverage,
                   const Int_t fourierPlotOpt, Double_t fourierXrange[2], Double_t phase,
                   Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh, const Bool_t batch);
    PFourierCanvas(vector<PFourier*> &fourier, const Char_t* title, const Bool_t showAverage,
                   const Int_t fourierPlotOpt, Double_t fourierXrange[2], Double_t phase,
                   Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
                   const PIntVector markerList, const PIntVector colorList, const Bool_t batch);
    virtual ~PFourierCanvas();

    virtual void Done(Int_t status=0); // *SIGNAL*
    virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT
    virtual void HandleMenuPopup(Int_t id); // SLOT
    virtual void LastCanvasClosed(); // SLOT

    virtual void UpdateFourierPad();
    virtual void UpdateInfoPad();

    virtual Bool_t IsValid() { return fValid; }

    virtual void SetTimeout(Int_t ival);

    virtual void SaveGraphicsAndQuit(const Char_t *fileName);
    virtual void SaveDataAscii();

  private:
    Int_t  fTimeout;         ///< timeout after which the Done signal should be emited. If timeout <= 0, no timeout is taking place
    Bool_t fBatchMode;       ///< musrview in ROOT batch mode
    Bool_t fValid;           ///< if true, everything looks OK
    Bool_t fAveragedView;    ///< tag showing that the averaged view or normal view should be presented.
    Int_t  fCurrentPlotView; ///< tag showing what the current plot view is: real, imag, power, phase, ...
    Double_t fInitialXRange[2]; ///< keeps the initial x-range
    Double_t fInitialYRange[2]; ///< keeps the initial y-range

    TString fTitle;
    TString fXaxisTitle;
    vector<PFourier*> fFourier; ///< keeps all the Fourier data, ownership is with the caller
    PFourierCanvasDataList fFourierHistos; ///< keeps all the Fourier histos
    PFourierCanvasDataSet fFourierAverage; ///< keeps the average of the Fourier histos
    Double_t fCurrentFourierPhase; ///< keeps the current Fourier phase (real/imag)
    TLatex *fCurrentFourierPhaseText; ///< used in Re/Im Fourier to show the current phase in the pad

    TStyle *fStyle; ///< A collection of all graphics attributes

    TTimer *fTimeoutTimer;  ///< timeout timer in order to terminate if no action is taking place for too long

    PIntVector fMarkerList; ///< list of markers
    PIntVector fColorList;  ///< list of colors

    // canvas menu related variables
    TRootCanvas *fImp;           ///< ROOT native GUI version of main window with menubar and drawing area
    TGMenuBar   *fBar;           ///< menu bar
    TGPopupMenu *fPopupMain;     ///< popup menu MusrFT in the main menu bar
    TGPopupMenu *fPopupSave;     ///< popup menu of the MusrFT/Save Data sub menu
    TGPopupMenu *fPopupFourier;  ///< popup menu of the MusrFT/Fourier sub menu

    // canvas related variables
    TCanvas   *fMainCanvas;        ///< main canvas
    TPaveText *fTitlePad;          ///< title pad used to display a title
    TPad      *fFourierPad;        ///< fourier pad used to display the fourier
    TLegend   *fInfoPad;           ///< info pad used to display a legend of the data plotted

    virtual void CreateXaxisTitle();
    virtual void CreateStyle();
    virtual void InitFourierDataSets();
    virtual void InitFourierCanvas(const Char_t* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);
    virtual void CleanupAverage();
    virtual void HandleAverage();

    virtual void PlotFourier();
    virtual void PlotFourierPhaseValue();
    virtual void PlotAverage();
    virtual void IncrementFourierPhase();
    virtual void DecrementFourierPhase();

    virtual Double_t GetMaximum(TH1F* histo, Double_t xmin=-1.0, Double_t xmax=-1.0);
    virtual Double_t GetMinimum(TH1F* histo, Double_t xmin=-1.0, Double_t xmax=-1.0);
    virtual Double_t GetInterpolatedValue(TH1F* histo, Double_t xVal);

    ClassDef(PFourierCanvas, 1)
};

#endif // _PFOURIERCANVAS_H_
