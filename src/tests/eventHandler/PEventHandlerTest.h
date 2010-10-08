/***************************************************************************

  PEventHandlerTest.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010 by Andreas Suter                                   *
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

#ifndef _PEVENTHANDLERTEST_H_
#define _PEVENTHANDLERTEST_H_

#include <vector>
using namespace std;

#include <TROOT.h>
#include <TObject.h>
#include <TQObject.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TGraph.h>

class PEventHandlerTest : public TObject, public TQObject
{
public:
  PEventHandlerTest();
  virtual ~PEventHandlerTest();

  virtual void Done(Int_t status=0); // *SIGNAL*
  virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT
  virtual void LastCanvasClosed(); // SLOT

private:
  TCanvas         *fMainCanvas; ///< main canvas
  TMultiGraph     *fMultiGraph1;
  TMultiGraph     *fMultiGraph2;
  Bool_t          fSwitched;
  vector<TGraph*> fGraph;

  virtual void SwitchGraph();

  ClassDef(PEventHandlerTest, 1)
};

#endif  //_PEVENTHANDLERTEST_H_
