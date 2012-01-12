/***************************************************************************

  read_musrRoot_runHeader.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2012 by Andreas Suter                              *
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

#include "TMusrRunHeader.h"

void read_musrRoot_runHeader_syntax()
{
  cout << endl << "usage: read_musrRoot_runHeader <fileName>";
  cout << endl << "       <fileName> is the file name including the extention root, e.g. test.root";
  cout << endl << endl;
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    read_musrRoot_runHeader_syntax();
    return 1;
  }

  // read the file back and extract the header info
  TFile *f = new TFile(argv[1], "READ", "read_musrRoot_runHeader");
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
  TMusrRunHeader *header = new TMusrRunHeader(argv[1]);

  // first read map!!
  TMap *map = (TMap*) runHeader->FindObjectAny("__map");
  if (map == 0) {
    cerr << endl << ">> **ERROR** couldn't find required __map :-(" << endl;
    f->Close();
    return -1;
  }
  header->SetMap(map);

  // get RunHeader
  oarray = (TObjArray*) runHeader->FindObjectAny("RunInfo");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get RunInfo" << endl;
  }
  if (!header->ExtractHeaderInformation(oarray, "RunInfo")) return -1;


  // get SampleEnv
  oarray = (TObjArray*) runHeader->FindObjectAny("SampleEnv");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get SampleEnv" << endl;
  }
  if (!header->ExtractHeaderInformation(oarray, "SampleEnv")) return -1;

  // get MagFieldEnv
  oarray = (TObjArray*) runHeader->FindObjectAny("MagFieldEnv");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get MagFieldEnv" << endl;
  }
  if (!header->ExtractHeaderInformation(oarray, "MagFieldEnv")) return -1;

  // get Beamline
  oarray = (TObjArray*) runHeader->FindObjectAny("Beamline");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get Beamline" << endl;
  }
  if (!header->ExtractHeaderInformation(oarray, "Beamline")) return -1;

  // get Scaler
  oarray = (TObjArray*) runHeader->FindObjectAny("Scaler");
  if (oarray == 0) {
    cerr << endl << ">> **ERROR** Couldn't get Scaler" << endl;
  }
  if (!header->ExtractHeaderInformation(oarray, "Scaler")) return -1;

  f->Close();
  delete f;

  header->DumpHeader();

  cout << endl << endl;

  return 0;
}
