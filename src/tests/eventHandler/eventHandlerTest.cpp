/***************************************************************************

  eventHandlerTest.cpp

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

#include <iostream>
using namespace std;

#include "TApplication.h"

#include "PEventHandlerTest.h"

int main(int argc, char *argv[])
{
  TApplication app("App", &argc, argv);

  PEventHandlerTest *eht = new PEventHandlerTest();
  if (eht == 0) {
    cerr << endl << "**ERROR** couldn't invoke eht ..." << endl;
    return 0;
  }

  TQObject::Connect("TCanvas", "Closed()", "PEventHandlerTest", eht, "LastCanvasClosed()");
  eht->Connect("Done(Int_t)", "TApplication", &app, "Terminate(Int_t)");

  app.Run(true);

  // clean up
  if (eht) {
    delete eht;
    eht = 0;
  }

  return 1;
}
