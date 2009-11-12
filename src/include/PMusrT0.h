/***************************************************************************

  PMusrT0.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Andreas Suter                                   *
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

#ifndef _PMUSRT0_H_
#define _PMUSRT0_H_

#include <TObject.h>
#include <TQObject.h>
#include <TStyle.h>
#include <TRootCanvas.h>
#include <TGMenu.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TLine.h>

#include "PMusr.h"
#ifndef __MAKECINT__
#include "PMsrHandler.h"
#endif // __MAKECINT__

//--------------------------------------------------------------------------
/**
 * <p>The preprocessor tag __MAKECINT__ is used to hide away from rootcint
 * the overly complex spirit header files.
 */
class PMusrT0 : public TObject, public TQObject
{
  public:
    PMusrT0();
    PMusrT0(PRawRunData *rawRunData, Int_t runNo, Int_t histoNo, Int_t detectorTag, Int_t addRunNo);

    virtual ~PMusrT0();

    virtual void Done(Int_t status=0); // *SIGNAL*
    virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT

#ifndef __MAKECINT__
    virtual void SetMsrHandler(PMsrHandler *msrHandler);
#endif // __MAKECINT__

    virtual Int_t GetStatus() { return fStatus; }

  private:
#ifndef __MAKECINT__
    PMsrHandler *fMsrHandler;
#endif // __MAKECINT__

    Int_t fStatus;

    Int_t fRunNo;
    Int_t fDetectorTag;
    Int_t fAddRunNo;
    Int_t fAddRunOffset;
    Int_t fT0Estimated;

    // canvas related variables
    TCanvas   *fMainCanvas;

    TH1F *fHisto;
    TH1F *fData;
    TH1F *fBkg;

    TLine *fT0Line;
    TLine *fFirstBkgLine;
    TLine *fLastBkgLine;
    TLine *fFirstDataLine;
    TLine *fLastDataLine;

    Int_t fPx;
    Int_t fPy;

    Int_t fDataRange[2];
    Int_t fBkgRange[2];

    void InitDataAndBkg();
    void SetT0Channel();
    void SetEstimatedT0Channel();
    void SetDataFirstChannel();
    void SetDataLastChannel();
    void SetBkgFirstChannel();
    void SetBkgLastChannel();
    void UnZoom();

  ClassDef(PMusrT0, 1)
};

#endif // _PMUSRT0_H_
