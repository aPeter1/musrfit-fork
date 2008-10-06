/***************************************************************************

  PMyCanvas.cpp

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

#include "PMyCanvas.h"

#define P_ID_FOURIER    1
#define P_ID_DIFFERENCE 2
#define P_ID_SAVE_DATA  3

ClassImpQ(PMyCanvas)

PMyCanvas::PMyCanvas()
{
  fValid = true;

  fMainCanvas = 0;
  fImp = 0;
  fBar = 0;
  fPopup = 0;

  TCanvas *fMainCanvas = new TCanvas("fMainCanvas", "my canvas ..");
cout << endl << ">> fMainCanvas=" << fMainCanvas << endl;

  TRootCanvas *fImp = (TRootCanvas*)fMainCanvas->GetCanvasImp(); 
cout << endl << ">> fImp=" << fImp << endl;
  TGMenuBar *fBar = fImp->GetMenuBar(); 
cout << endl << ">> fBar=" << fBar << endl;
  TGPopupMenu *fPopup = fBar->AddPopup("m&y menu");
cout << endl << ">> fPopup=" << fPopup << endl;
  fPopup->AddEntry("F&ourier", 1);
  fPopup->AddEntry("&Difference", 2);
  fPopup->AddSeparator();
  fPopup->AddEntry("Save Data", 3);
  fBar->MapSubwindows(); 
  fBar->Layout();

  fPopup->Connect("TGPopupMenu", "Activated(Int_t)", "PMyCanvas", this, "HandlePopup(Int_t)");
}

PMyCanvas::~PMyCanvas()
{
  if (fMainCanvas) {
    delete fMainCanvas;
    fMainCanvas = 0;
  }
}

//---------------------------------------------------------------------------------------
// HandlePopup (SLOT)
//---------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMyCanvas::HandlePopup(Int_t id)
{
  switch (id) {
    case P_ID_FOURIER:
      cout << endl << ">> id=" << id << ", you actived the \"Fourier\" menu item." << endl;
      break;
    case P_ID_DIFFERENCE:
      cout << endl << ">> id=" << id << ", you actived the \"Difference\" menu item." << endl;
      break;
    case P_ID_SAVE_DATA:
      cout << endl << ">> id=" << id << ", you actived the \"Save Data\" menu item." << endl;
      break;
    default:
      cout << endl << ">> id=" << id << ", you never should have reached this point :-( " << endl << endl;
      break;
  }
}

