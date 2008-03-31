/***************************************************************************

  musrview.cpp

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

#include <TApplication.h>

#include "PMusr.h"
#include "PMusrCanvas.h"

//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void musrview_syntax()
{
  cout << endl << "usage: musrview <msr-file> | --version | --help";
  cout << endl << "       <msr-file>: msr input file";
  cout << endl << "       'musrview <msr-file>' will execute msrfit";
  cout << endl << "       'musrview' or 'msrfit --help' will show this help";
  cout << endl << "       'musrview --version' will print the msrfit version";
  cout << endl << endl;
}

//--------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  bool show_syntax = false;

  switch (argc) {
    case 1:
      show_syntax = true;
      break;
    case 2:
      if (strstr(argv[1], "--version")) {
        cout << endl << "musrview version: $Id$";
        cout << endl << endl;
        return PMUSR_SUCCESS;
      } else if (strstr(argv[1], "--help")) {
        show_syntax = true;
      } else {
        // check if filename has extension msr
        if (!strstr(argv[1], ".msr")) {
          cout << endl << "ERROR: " << argv[1] << " is not a msr-file!" << endl;
          show_syntax = true;
        }
      }
      break;
    default:
      show_syntax = true;
  }

  if (show_syntax) {
    musrview_syntax();
    return PMUSR_WRONG_STARTUP_SYNTAX;
  }

  TApplication app("App", &argc, argv);

  PMusrCanvas *musrCanvas = new PMusrCanvas("musr canvas dummy", 10, 10, 800, 600);
  if (!musrCanvas->IsValid()) {
    cout << endl << "**SEVERE ERROR** Couldn't invoke all necessary objects, will quit.";
    cout << endl;
    return -1;
  }

  musrCanvas->Connect("Done(Int_t)", "TApplication", &app, "Terminate(Int_t)");

  app.Run();

  // clean up
  if (musrCanvas) {
    delete musrCanvas;
    musrCanvas = 0;
  }

  return 0;
}
