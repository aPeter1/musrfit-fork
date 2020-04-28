/***************************************************************************

  mupp_plot.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2017 by Andreas Suter                              *
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

#include <TApplication.h>
#include <TSAXParser.h>
#include <TROOT.h>

#include "PMuppStartupHandler.h"
#include "PMuppCanvas.h"

//--------------------------------------------------------------------------
/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
  if (argc != 2) {
    return -1;
  }
  int mupp_instance = (int)strtol(argv[1], NULL, 10);
  if ((mupp_instance < 0) || (mupp_instance > 255)) {
    return -2;
  }

  // read startup file
  char startup_path_name[128];
  TSAXParser *saxParser = new TSAXParser();
  PMuppStartupHandler *startupHandler = new PMuppStartupHandler();
  if (!startupHandler->StartupFileFound()) {
    std::cerr << std::endl << ">> mupp_plot **WARNING** couldn't find " << startupHandler->GetStartupFilePath().Data();
    std::cerr << std::endl;
    // clean up
    if (saxParser) {
      delete saxParser;
      saxParser = 0;
    }
    if (startupHandler) {
      delete startupHandler;
      startupHandler = 0;
    }
  } else {
    strcpy(startup_path_name, startupHandler->GetStartupFilePath().Data());
    saxParser->ConnectToHandler("PMuppStartupHandler", startupHandler);
    // parsing the file as above seems to lead to problems in certain environments;
    // use the parseXmlFile function instead (see PMuppStartupHandler.cpp for the definition)
    Int_t status = parseXmlFile(saxParser, startup_path_name);
    // check for parse errors
    if (status) { // error
      std::cerr << std::endl << ">> mupp_plot **WARNING** Reading/parsing mupp_startup.xml failed.";
      std::cerr << std::endl << ">>                       Graph will appear with random symbols and colors!";
      std::cerr << std::endl;
      // clean up
      if (saxParser) {
        delete saxParser;
        saxParser = 0;
      }
      if (startupHandler) {
        delete startupHandler;
        startupHandler = 0;
      }
    } else {
      startupHandler->CheckLists();
    }
  }

  TApplication app("App", &argc, argv);

  Bool_t ok=kTRUE;
  PMuppCanvas *muppCanvas = new PMuppCanvas("mupp", 10, 10, 600, 800,
                                            startupHandler->GetMarkerStyleList(),
                                            startupHandler->GetMarkerSizeList(),
                                            startupHandler->GetColorList(),
                                            mupp_instance);

  if (muppCanvas != 0) {
    if (muppCanvas->IsValid()) {
      // connect signal/slot
      TQObject::Connect("TCanvas", "Closed()", "PMuppCanvas", muppCanvas, "LastCanvasClosed()");
      muppCanvas->Connect("Done(Int_t)", "TApplication", &app, "Terminate(Int_t)");
    } else {
      std::cerr << std::endl << ">> mupp_plot **SEVERE ERROR** invoke object is invalid, will quit.";
      std::cerr << std::endl;
      ok = false;
    }
  } else {
    std::cerr << std::endl << ">> mupp_plot **SEVERE ERROR** Couldn't invoke all necessary objects, will quit.";
    std::cerr << std::endl;
    ok = false;
  }


  // check that everything is ok
  if (ok)
    app.Run(true); // true needed that Run will return after quit so that cleanup works

  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }
  if (muppCanvas) {
    delete muppCanvas;
    muppCanvas = 0;
  }

  return 0;
}
