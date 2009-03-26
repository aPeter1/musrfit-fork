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

// Canvas menu id's
#define P_MENU_ID_T0                 100
#define P_MENU_ID_FIRST_BKG_CHANNEL  101
#define P_MENU_ID_LAST_BKG_CHANNEL   102
#define P_MENU_ID_FIRST_DATA_CHANNEL 103
#define P_MENU_ID_LAST_DATA_CHANNEL  104
#define P_MENU_ID_UNZOOM             105

//--------------------------------------------------------------------------
/**
 * <p>The preprocessor tag __MAKECINT__ is used to hide away from rootcint
 * the overly complex spirit header files.
 */
class PMusrT0 : public TObject, public TQObject
{
  public:
    PMusrT0();
    PMusrT0(PRawRunData *rawRunData, int runNo, int histoNo, int detectorTag);

    virtual ~PMusrT0();

    virtual void Done(Int_t status=0); // *SIGNAL*
    virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT
    virtual void HandleMenuPopup(Int_t id); // SLOT

#ifndef __MAKECINT__
    virtual void SetMsrHandler(PMsrHandler *msrHandler);
#endif // __MAKECINT__

  private:
#ifndef __MAKECINT__
    PMsrHandler *fMsrHandler;
#endif // __MAKECINT__

    int fRunNo;
    int fDetectorTag;

    // canvas menu related variables
    TRootCanvas *fImp;
    TGMenuBar   *fBar;
    TGPopupMenu *fPopupMain;

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

    void InitDataAndBkg();

  ClassDef(PMusrT0, 1)
};

#endif // _PMUSRT0_H_
