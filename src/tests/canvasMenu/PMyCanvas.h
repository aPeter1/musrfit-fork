/***************************************************************************

  PMyCanvas.h

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

#ifndef _PMYCANVAS_H_
#define _PMYCANVAS_H_

#include "TObject.h"
#include "TQObject.h"
#include "TCanvas.h"
#include "TRootCanvas.h"
#include "TGMenu.h"

class PMyCanvas : public TObject, public TQObject
{
  public:
    PMyCanvas();
    virtual ~PMyCanvas();

    bool IsValid() { return fValid; }

    void HandlePopup(Int_t id); // SLOT

  private:
    bool fValid;

    TCanvas *fMainCanvas;

    TRootCanvas *fImp;
    TGMenuBar   *fBar;
    TGPopupMenu *fPopup;

  ClassDef(PMyCanvas, 1)
};

#endif // _PMYCANVAS_H_
