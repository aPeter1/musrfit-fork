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

#include "PMusr.h"
#include "TMusrRunHeader.h"

void closeFile(TFile *f)
{
  if (f == 0)
    return;

  f->Close();
  delete f;
}

void read_musrRoot_runHeader_syntax()
{
  cout << endl << "usage: read_musrRoot_runHeader [<fileName>] | --version";
  cout << endl << "       <fileName> is the file name including the extention root, e.g. test.root";
  cout << endl << endl;
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    read_musrRoot_runHeader_syntax();
    return 1;
  }

  if (!strcmp(argv[1], "--version")) {
    cout << endl << "read_musrRoot_runHeader version: " << PMUSR_VERSION << " / $Id$" << endl << endl;
    return 0;
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
    closeFile(f);
    return -1;
  }

  TMusrRunHeader *header = new TMusrRunHeader(argv[1]);

  if (!header->ExtractAll(runHeader)) {
    cerr << endl << ">> **ERROR** couldn't extract all RunHeader information :-(" << endl << endl;
    closeFile(f);
    return -1;
  }

  f->Close();
  delete f;

  header->DumpHeader();

  delete header;

  cout << endl << endl;

  return 0;
}
