/***************************************************************************

  PNL_PippardFitter.cpp

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

#include <cassert>

#include <iostream>
using namespace std;

#include <TSAXParser.h>

#include "PNL_PippardFitter.h"

ClassImp(PNL_PippardFitter)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PNL_PippardFitter::PNL_PippardFitter()
{
  // read XML startup file
  char startup_path_name[128];
  TSAXParser *saxParser = new TSAXParser();
  PNL_StartupHandler *fStartupHandler = new PNL_StartupHandler();
  strcpy(startup_path_name, fStartupHandler->GetStartupFilePath().Data());
  saxParser->ConnectToHandler("PNL_StartupHandler", fStartupHandler);
  Int_t status = saxParser->ParseFile(startup_path_name);
  // check for parse errors
  if (status) { // error
    cout << endl << "**WARNING** reading/parsing nonlocal_startup.xml.";
    cout << endl;
    // clean up
    if (saxParser) {
      delete saxParser;
      saxParser = 0;
    }
    if (fStartupHandler) {
      delete fStartupHandler;
      fStartupHandler = 0;
    }
    assert(false);
  }

  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }

  // load all the TRIM.SP rge-files
  fRgeHandler = new PNL_RgeHandler(fStartupHandler->GetTrimSpDataPathList());
  if (!fRgeHandler->IsValid())
    assert(false);
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PNL_PippardFitter::~PNL_PippardFitter()
{
  if (fStartupHandler) {
    delete fStartupHandler;
    fStartupHandler = 0;
  }
}

//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------
/**
 *
 */
Double_t PNL_PippardFitter::operator()(Double_t t, const std::vector<Double_t> &param) const
{
  // expected parameters: energy, temp, thickness, meanFreePath, xi0, lambdaL
  assert(param.size() != 6);

  return 0.0;
}
