/***************************************************************************

  read_psi_runHeader.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2011 by Andreas Suter                              *
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

#include <TROOT.h>
#include <TFile.h>
#include <TFolder.h>

#include "TPsiRunHeader.h"

void read_psi_runHeader_syntax()
{
  cout << endl << "usage: read_psi_runHeader <fileName> <headerDefinition> [<strict>]";
  cout << endl << "       <fileName> is the file name including the extention root, e.g. test.root";
  cout << endl << "       <headerDefinition> is the header definition XML-file.";
  cout << endl << "       <strict> 'strict'=strict validation; otherwise=less strict validation.";
  cout << endl << endl;
}

int main(int argc, char *argv[])
{
  if ((argc != 3) && (argc != 4)) {
    read_psi_runHeader_syntax();
    return 1;
  }

  Bool_t strict = false;
  if (argc == 4) {
    if (!strcmp(argv[3], "strict"))
      strict = true;
  }

  // read the file back and extract the header info
  TFile *f = new TFile(argv[1], "READ", "read_psi_runHeader");
  if (f->IsZombie()) {
    delete f;
    return -1;
  }

  TFolder *runHeader = 0;
  f->GetObject("RunHeader", runHeader);
  if (runHeader == 0) {
    cerr << endl << ">> **ERROR** Couldn't get top folder RunHeader";
    f->Close();
    return -1;
  }

  TObjArray *oarray = 0;
  TPsiRunHeader *header = new TPsiRunHeader(argv[2]);

  // get RunHeader
  oarray = (TObjArray*) runHeader->FindObjectAny("RunInfo");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get RunInfo" << endl;
  }
  header->ExtractHeaderInformation(oarray, "RunInfo");

  // get SampleEnv
  oarray = (TObjArray*) runHeader->FindObjectAny("SampleEnv");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get SampleEnv" << endl;
  }
  header->ExtractHeaderInformation(oarray, "SampleEnv");

  // get MagFieldEnv
  oarray = (TObjArray*) runHeader->FindObjectAny("MagFieldEnv");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get MagFieldEnv" << endl;
  }
  header->ExtractHeaderInformation(oarray, "MagFieldEnv");

  // get Beamline
  oarray = (TObjArray*) runHeader->FindObjectAny("Beamline");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get Beamline" << endl;
  }
  header->ExtractHeaderInformation(oarray, "Beamline");

  // get Scaler
  oarray = (TObjArray*) runHeader->FindObjectAny("Scaler");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get Scaler" << endl;
  }
  header->ExtractHeaderInformation(oarray, "Scaler");

  f->Close();
  delete f;

  if (!header->IsValid(strict)) {
    cerr << endl << ">> **ERROR** run header validation failed." << endl;
    if (strict) { // clean up and quit
      delete header;
      return -1;
    }
  }

  header->DumpHeader();

  cout << endl << endl;

  return 0;
}
