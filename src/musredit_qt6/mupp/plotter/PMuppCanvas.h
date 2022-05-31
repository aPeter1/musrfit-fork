/***************************************************************************

  PMuppCanvas.h

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

#ifndef _PMUPPCANVAS_H_
#define _PMUPPCANVAS_H_

#include <vector>

#include <TObject.h>
#include <TQObject.h>
#include <TStyle.h>
#include <TRootCanvas.h>
#include <TGMenu.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TGraphAsymmErrors.h>
#include <TTimer.h>

#include "mupp_plot.h"
#include "mupp.h"

// Canvas menu id's
#define P_MENU_ID_EXPORT 10001
#define P_MENU_ID_ABOUT  10002

//--------------------------------------------------------------------------
struct PDataPoint {
  Double_t y;
  Double_t eYpos;
  Double_t eYneg;
};

//--------------------------------------------------------------------------
struct PDataCollection {
  TString name; // collection name
  TString xLabel;
  PStringVector yLabel;
  PDoubleVector xValue;
  std::vector< std::vector<PDataPoint> > yValue;
};

//--------------------------------------------------------------------------
class PMuppCanvas : public TObject, public  TQObject
{
public:
  PMuppCanvas();
  PMuppCanvas(const Char_t* title,
              Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
              const PIntVector markerSytleList, const PDoubleVector markerSizeList,
              const PIntVector colorList,
              const int mupp_instance);
  virtual ~PMuppCanvas();

  virtual Bool_t IsValid() { return fValid; }

  virtual void Done(Int_t status=0); // *SIGNAL*
  virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT
  virtual void HandleMenuPopup(Int_t id); // SLOT
  virtual void LastCanvasClosed(); // SLOT
  virtual void WindowClosed(); // SLOT

  virtual void CheckIPCMsgQueue();

private:
  Bool_t fValid;
  Int_t fMuppInstance;

  TString fFtokName;
  TTimer *fCheckMsgQueue; ///< timer needed to check if a message in the IPC message queue is pending

  std::vector<PDataCollection> fPlotData;

  TStyle    *fStyle; ///< A collection of all graphics attributes

  // canvas menu related variables
  TRootCanvas *fImp;           ///< ROOT native GUI version of main window with menubar and drawing area
  TGMenuBar   *fBar;           ///< menu bar
  TGPopupMenu *fPopupMain;     ///< popup menu mupp in the main menu bar

  // canvas related variables
  TCanvas   *fMainCanvas;            ///< main canvas
  TMultiGraph *fMultiGraph;          ///< main multi graph
  std::vector<TGraphAsymmErrors*> fGraphE; ///< all error graphs

  // perdefined markers, colors
  PIntVector    fMarkerStyleList;
  PDoubleVector fMarkerSizeList;
  PIntVector    fColorList;

  virtual void CreateStyle();
  virtual void InitMuppCanvas(const Char_t* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);

  virtual int ReadPlotData(const Char_t *fln);
  virtual void InitDataCollection(PDataCollection &coll);
  virtual void UpdateGraphs();
  virtual void ExportData();

  ClassDef(PMuppCanvas, 1)
};

#endif // _PMUPPCANVAS_H_
