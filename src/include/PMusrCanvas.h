/***************************************************************************

  PMusrCanvas.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2016 by Andreas Suter                              *
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
#include <TMultiGraph.h>
#include <TLatex.h>

#include "PMusr.h"
#ifndef __MAKECINT__
#include "PMsrHandler.h"
#include "PRunListCollection.h"
#endif // __MAKECINT__

#define YINFO  0.1
#define YTITLE 0.95
#define XTHEO  0.75

// Current Plot Views
#define PV_DATA                  1
#define PV_FOURIER_REAL          2
#define PV_FOURIER_IMAG          3
#define PV_FOURIER_REAL_AND_IMAG 4
#define PV_FOURIER_PWR           5
#define PV_FOURIER_PHASE         6

// Canvas menu id's
#define P_MENU_ID_DATA          10001
#define P_MENU_ID_FOURIER       10002
#define P_MENU_ID_DIFFERENCE    10003
#define P_MENU_ID_AVERAGE       10004
#define P_MENU_ID_EXPORT_DATA   10005

#define P_MENU_PLOT_OFFSET      1000

#define P_MENU_ID_FOURIER_REAL          100
#define P_MENU_ID_FOURIER_IMAG          101
#define P_MENU_ID_FOURIER_REAL_AND_IMAG 102
#define P_MENU_ID_FOURIER_PWR           103
#define P_MENU_ID_FOURIER_PHASE         104
#define P_MENU_ID_FOURIER_PHASE_PLUS    105
#define P_MENU_ID_FOURIER_PHASE_MINUS   106

//------------------------------------------------------------------------
/**
 * <p>
 */
class PMusrCanvasPlotRange : public TObject
{
  public:
    PMusrCanvasPlotRange();
    virtual ~PMusrCanvasPlotRange() {}

    virtual void SetXRange(Double_t xmin, Double_t xmax);
    virtual void SetYRange(Double_t ymin, Double_t ymax);

    virtual Bool_t IsXRangePresent() { return fXRangePresent; }
    virtual Bool_t IsYRangePresent() { return fYRangePresent; }

    virtual Double_t GetXmin() { return fXmin; }
    virtual Double_t GetXmax() { return fXmax; }
    virtual Double_t GetYmin() { return fYmin; }
    virtual Double_t GetYmax() { return fYmax; }

  private:
    Bool_t fXRangePresent;
    Bool_t fYRangePresent;
    Double_t fXmin;
    Double_t fXmax;
    Double_t fYmin;
    Double_t fYmax;

  ClassDef(PMusrCanvasPlotRange, 1)
};

//------------------------------------------------------------------------
/**
 * <p>Structure holding all necessary histograms for a single plot block entry for
 * fit types: asymmetry fit and single histogram fit.
 */
typedef struct {
  TH1F *data;                ///< data histogram
  TH1F *dataFourierRe;       ///< real part of the Fourier transform of the data histogram
  TH1F *dataFourierIm;       ///< imaginary part of the Fourier transform of the data histogram
  TH1F *dataFourierPwr;      ///< power spectrum of the Fourier transform of the data histogram
  TH1F *dataFourierPhase;    ///< phase spectrum of the Fourier transform of the data histogram
  TH1F *theory;              ///< theory histogram belonging to the data histogram
  TH1F *theoryFourierRe;     ///< real part of the Fourier transform of the theory histogram
  TH1F *theoryFourierIm;     ///< imaginary part of the Fourier transform of the theory histogram
  TH1F *theoryFourierPwr;    ///< power spectrum of the Fourier transform of the theory histogram
  TH1F *theoryFourierPhase;  ///< phase spectrum of the Fourier transform of the theory histogram
  TH1F *diff;                ///< difference histogram, i.e. data-theory
  TH1F *diffFourierRe;       ///< real part of the Fourier transform of the diff histogram
  TH1F *diffFourierIm;       ///< imaginary part of the Fourier transform of the diff histogram
  TH1F *diffFourierPwr;      ///< power spectrum of the Fourier transform of the diff histogram
  TH1F *diffFourierPhase;    ///< phase spectrum of the Fourier transform of the diff histogram
  PMusrCanvasPlotRange *dataRange;    ///< keep the msr-file plot data range
  UInt_t diffFourierTag; ///< 0=not relevant, 1=d-f (Fourier of difference time spectra), 2=f-d (difference of Fourier spectra)
} PMusrCanvasDataSet;

//------------------------------------------------------------------------
/**
 * <p>typedef to make to code more readable: list of histogram data sets.
 */
typedef vector<PMusrCanvasDataSet> PMusrCanvasDataList;

//------------------------------------------------------------------------
/**
 * <p>Structure holding all necessary error graphs for a single plot block entry for
 * fit types: non-muSR fit.
 */
typedef struct {
  TGraphErrors *data;               ///< data error graph
  TGraphErrors *dataFourierRe;      ///< real part of the Fourier transform of the data error graph
  TGraphErrors *dataFourierIm;      ///< imaginary part of the Fourier transform of the data error graph
  TGraphErrors *dataFourierPwr;     ///< power spectrum of the Fourier transform of the data error graph
  TGraphErrors *dataFourierPhase;   ///< phase spectrum of the Fourier transform of the data error graph
  TGraphErrors *theory;             ///< theory histogram belonging to the data error graph
  TGraphErrors *theoryFourierRe;    ///< real part of the Fourier transform of the theory error graph
  TGraphErrors *theoryFourierIm;    ///< imaginary part of the Fourier transform of the theory error graph
  TGraphErrors *theoryFourierPwr;   ///< power spectrum of the Fourier transform of the theory error graph
  TGraphErrors *theoryFourierPhase; ///< phase spectrum of the Fourier transform of the theory error graph
  TGraphErrors *diff;               ///< difference error graph, i.e. data-theory
  TGraphErrors *diffFourierRe;      ///< real part of the Fourier transform of the diff error graph
  TGraphErrors *diffFourierIm;      ///< imaginary part of the Fourier transform of the diff error graph
  TGraphErrors *diffFourierPwr;     ///< power spectrum of the Fourier transform of the diff error graph
  TGraphErrors *diffFourierPhase;   ///< phase spectrum of the Fourier transform of the diff error graph
  PMusrCanvasPlotRange *dataRange;    ///< keep the msr-file plot data range
  UInt_t diffFourierTag; ///< 0=not relevant, 1=d-f (Fourier of difference time spectra), 2=f-d (difference of Fourier spectra)
} PMusrCanvasNonMusrDataSet;

//------------------------------------------------------------------------
/**
 * <p>typedef to make to code more readable: list of error graph data sets.
 */
typedef vector<PMusrCanvasNonMusrDataSet> PMusrCanvasNonMusrDataList;

//------------------------------------------------------------------------
/**
 * <p> data structure needed for ascii dump within musrview.
 */
typedef struct {
  PDoubleVector dataX;   ///< x-axis data set
  PDoubleVector data;    ///< y-axis data set
  PDoubleVector dataErr; ///< error of the y-axis data set
  PDoubleVector theoryX; ///< x-axis theory set
  PDoubleVector theory;  ///< y-axis theory set
} PMusrCanvasAsciiDump;

//------------------------------------------------------------------------
/**
 * <p> typedef to make to code more readable: vector of the above data structure.
 * Used if there are multiple histogramms to be dumped.
 */
typedef vector<PMusrCanvasAsciiDump> PMusrCanvasAsciiDumpVector;

//--------------------------------------------------------------------------
/**
 * <p>The preprocessor tag __MAKECINT__ is used to hide away from rootcint
 * the overly complex spirit header files. 
 */
class PMusrCanvas : public TObject, public TQObject
{
  public:
    PMusrCanvas();
    PMusrCanvas(const Int_t number, const Char_t* title,
                Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh, const Bool_t batch,
                const Bool_t fourier=false);
    PMusrCanvas(const Int_t number, const Char_t* title,
                Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
                PMsrFourierStructure fourierDefault,
                const PIntVector markerList, const PIntVector colorList, const Bool_t batch,
                const Bool_t fourier=false);
    virtual ~PMusrCanvas();

    virtual Bool_t IsValid() { return fValid; }

#ifndef __MAKECINT__
    virtual void SetMsrHandler(PMsrHandler *msrHandler);
    virtual void SetRunListCollection(PRunListCollection *runList) { fRunList = runList; }
#endif // __MAKECINT__

    virtual void SetTimeout(Int_t ival);
    virtual void UpdateParamTheoryPad();
    virtual void UpdateDataTheoryPad();
    virtual void UpdateInfoPad();

    virtual void Done(Int_t status=0); // *SIGNAL*
    virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT
    virtual void HandleMenuPopup(Int_t id); // SLOT
    virtual void LastCanvasClosed(); // SLOT

    virtual void SaveGraphicsAndQuit(Char_t *fileName, Char_t *graphicsFormat);
    virtual void ExportData(const Char_t *fileName);

  private:
    Bool_t fStartWithFourier; ///< flag if true, the Fourier transform will be presented bypassing the time domain representation
    Int_t  fTimeout;          ///< timeout after which the Done signal should be emited. If timeout <= 0, no timeout is taking place
    Bool_t fScaleN0AndBkg;    ///< true=N0 and background is scaled to (1/ns), otherwise (1/bin) for the single histogram case
    Bool_t fBatchMode;        ///< musrview in ROOT batch mode
    Bool_t fValid;            ///< if true, everything looks OK
    Bool_t fAveragedView;     ///< tag showing that the averaged view or normal view should be presented.
    Bool_t fDifferenceView;   ///< tag showing that the shown data, fourier, are the difference between data and theory
    Int_t  fCurrentPlotView;  ///< tag showing what the current plot view is: data, fourier, ...
    Int_t  fPreviousPlotView; ///< tag showing the previous plot view
    Int_t  fPlotType;         ///< plot type tag: -1 == undefined, MSR_PLOT_SINGLE_HISTO == single histogram, MSR_PLOT_ASYM == asymmetry, MSR_PLOT_MU_MINUS == mu minus (not yet implemented), MSR_PLOT_NON_MUSR == non-muSR
    Int_t  fPlotNumber;       ///< plot number

    Bool_t fXRangePresent, fYRangePresent; ///< flag indicating if x-/y-range is present
    Double_t fXmin, fXmax, fYmin, fYmax;   ///< data/theory frame range

    Double_t fCurrentFourierPhase;    ///< holds the current Fourier phase
    TLatex *fCurrentFourierPhaseText; ///< used in Re/Im Fourier to show the current phase in the pad
    TString *fRRFText;      ///< RRF information
    TLatex  *fRRFLatexText; ///< used to display RRF info

    TStyle    *fStyle; ///< A collection of all graphics attributes

    TTimer *fTimeoutTimer; ///< timeout timer in order to terminate if no action is taking place for too long

    // canvas menu related variables
    TRootCanvas *fImp;           ///< ROOT native GUI version of main window with menubar and drawing area
    TGMenuBar   *fBar;           ///< menu bar
    TGPopupMenu *fPopupMain;     ///< popup menu Musrfit in the main menu bar
    TGPopupMenu *fPopupFourier;  ///< popup menu of the Musrfit/Fourier sub menu

    // canvas related variables
    TCanvas   *fMainCanvas;        ///< main canvas
    TPaveText *fTitlePad;          ///< title pad used to display a title
    TPad      *fDataTheoryPad;     ///< data/theory pad used to display the data/theory
    TPaveText *fParameterPad;      ///< parameter pad used to display the fitting parameters
    TPaveText *fTheoryPad;         ///< theory pad used to display the theory and functions
    TLegend   *fInfoPad;           ///< info pad used to display a legend of the data plotted
    TLegend   *fMultiGraphLegend;  ///< used for non-muSR plots to display a legend

    TH1F *fHistoFrame; ///< fHistoFrame is a 'global' frame needed in order to plot histograms with (potentially) different x-frames

#ifndef __MAKECINT__
    PMsrHandler        *fMsrHandler; ///< msr-file handler
    PRunListCollection *fRunList;    ///< data handler
#endif // __MAKECINT__

    PMusrCanvasDataSet fDataAvg; ///< set of all averaged data to be plotted (asymmetry/single histogram)

    PMusrCanvasDataList fData; ///< list of all histogram data to be plotted (asymmetry/single histogram)
    PMusrCanvasNonMusrDataList fNonMusrData; ///< list of all error graphs to be plotted (non-muSR)
    TMultiGraph *fMultiGraphData; ///< fMultiGraphData is a 'global' graph needed in order to plot error graphs (data) with (potentially) different x-frames
    TMultiGraph *fMultiGraphDiff; ///< fMultiGraphDiff is a 'global' graph needed in order to plot error graphs (data-theory) with (potentially) different x-frames

    PMsrFourierStructure fFourier; ///< structure holding all the information necessary to perform the Fourier transform
    PIntVector fMarkerList; ///< list of markers
    PIntVector fColorList;  ///< list of colors

    virtual void CreateStyle();
    virtual void InitFourier();
    virtual void InitAverage();
    virtual void InitMusrCanvas(const Char_t* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);
    virtual void InitDataSet(PMusrCanvasDataSet &dataSet);
    virtual void InitDataSet(PMusrCanvasNonMusrDataSet &dataSet);
    virtual void CleanupDataSet(PMusrCanvasDataSet &dataSet);
    virtual void CleanupDataSet(PMusrCanvasNonMusrDataSet &dataSet);
    virtual void HandleDataSet(UInt_t plotNo, UInt_t runNo, PRunData *data);
    virtual void HandleNonMusrDataSet(UInt_t plotNo, UInt_t runNo, PRunData *data);
    virtual void HandleDifference();
    virtual void HandleFourier();
    virtual void HandleDifferenceFourier();
    virtual void HandleFourierDifference();
    virtual void HandleAverage();
    virtual Double_t FindOptimalFourierPhase();
    virtual void CleanupDifference();
    virtual void CleanupFourier();
    virtual void CleanupFourierDifference();
    virtual void CleanupAverage();

    virtual Double_t CalculateDiff(const Double_t x, const Double_t y, TH1F *theo);
    virtual Double_t CalculateDiff(const Double_t x, const Double_t y, TGraphErrors *theo);
    virtual Int_t  FindBin(const Double_t x, TGraphErrors *graph);

    virtual Double_t GetMaximum(TH1F* histo, Double_t xmin=-1.0, Double_t xmax=-1.0);
    virtual Double_t GetMinimum(TH1F* histo, Double_t xmin=-1.0, Double_t xmax=-1.0);
    virtual Double_t GetMaximum(TGraphErrors* graph, Double_t xmin=-1.0, Double_t xmax=-1.0);
    virtual Double_t GetMinimum(TGraphErrors* graph, Double_t xmin=-1.0, Double_t xmax=-1.0);

    virtual void PlotData(Bool_t unzoom=false);
    virtual void PlotDifference(Bool_t unzoom=false);
    virtual void PlotFourier(Bool_t unzoom=false);
    virtual void PlotFourierDifference(Bool_t unzoom=false);
    virtual void PlotFourierPhaseValue(Bool_t unzoom=false);
    virtual void PlotAverage(Bool_t unzoom=false);
    virtual void IncrementFourierPhase();
    virtual void DecrementFourierPhase();

    virtual Bool_t IsScaleN0AndBkg();
    virtual UInt_t GetNeededAccuracy(PMsrParamStructure param);

    virtual Double_t GetInterpolatedValue(TH1F* histo, Double_t xVal);

  ClassDef(PMusrCanvas, 1)
};

#endif // _PMUSRCANVAS_H_
