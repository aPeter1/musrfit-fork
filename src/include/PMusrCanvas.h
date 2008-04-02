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
#include <TCanvas.h>
#include <TPaveText.h>
#include <TPad.h>

#include "PMusr.h"

#define YINFO  0.1
#define YTITLE 0.95
#define XTHEO  0.75

//--------------------------------------------------------------------------
/**
 * <p>
 */
class PMusrCanvas : public TObject, public TQObject
{
  public:
    PMusrCanvas();
    PMusrCanvas(const char* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);
    virtual ~PMusrCanvas();

    virtual Bool_t IsValid() { return fValid; }

    virtual void SetParameterList(PMsrParamList &paramList);
    virtual void SetTheoryList(PMsrLines &theoryList);
    virtual void SetFunctionList(PMsrLines &functionList);

    virtual void UpdateParamTheoryPad();
    virtual void UpdateInfoPad();

    virtual void Done(Int_t status=0); // *SIGNAL*
    virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT

  private:
    Bool_t fValid;

    TCanvas   *fMainCanvas;
    TPaveText *fTitlePad;
    TPad      *fDataTheoryPad;
    TPaveText *fParameterTheoryPad;
    TPaveText *fInfoPad;

    TPaveText *fKeyboardHandlerText;

    PMsrParamList fParamList;
    PMsrLines     fTheoryList;
    PMsrLines     fFunctionList;

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
