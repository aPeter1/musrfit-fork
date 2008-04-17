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
#include <TCanvas.h>
#include <TPaveText.h>
#include <TLegend.h>
#include <TPad.h>
#include <TH1F.h>

#include "PMusr.h"
#ifndef __MAKECINT__
#include "PMsrHandler.h"
#include "PRunListCollection.h"
#endif // __MAKECINT__

#define YINFO  0.1
#define YTITLE 0.95
#define XTHEO  0.75

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

  private:
    Bool_t fValid;
    Int_t  fPlotNumber;

    TStyle    *fStyle;

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

    PIntVector fMarkerList;
    PIntVector fColorList;

    virtual void CreateStyle();
    virtual void InitMusrCanvas(const char* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);
    virtual void InitDataSet(PMusrCanvasDataSet &dataSet);
    virtual void CleanupDataSet(PMusrCanvasDataSet &dataSet);
    virtual void HandleDataSet(unsigned int runNo, PRunData *data);

  ClassDef(PMusrCanvas, 1)
};

// root dictionary stuff --------------------------------------------------
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class PMusrCanvas+;

#endif
// root dictionary stuff --------------------------------------------------

#endif // _PMUSRCANVAS_H_
