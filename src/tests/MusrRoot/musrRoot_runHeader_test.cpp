/***************************************************************************

  musrRoot_runHeader_test.cpp

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

void musrRoot_runHeader_test_syntax()
{
  cout << endl << "usage: musrRoot_runHeader_test <fileName>";
  cout << endl << "       <fileName> is the file name including the extention root, e.g. test.root";
  cout << endl << endl;
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    musrRoot_runHeader_test_syntax();
    return 1;
  }

  // read the file and extract the header info
  TFile *f = new TFile(argv[1], "READ", "musrRoot_runHeader_test");
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

  // MusrRoot Run Header object
  TMusrRunHeader *header = new TMusrRunHeader(argv[1]);
  header->ExtractAll(runHeader);

  f->Close();
  delete f;

  header->DumpHeader();

  // get some information from the read file
  cout << endl << "++++++++++++++++++++++++++++";
  cout << endl << ">> get header infos " << argv[1];
  cout << endl << "++++++++++++++++++++++++++++" << endl;

  TString str("");
  TStringVector strVec;
  Int_t ival;
  TDoubleVector dvec;
  TMusrRunPhysicalQuantity prop;
  Bool_t ok;

  header->GetValue("RunInfo/Run Title", str, ok);
  if (ok)
    cout << endl << "Run Title: " << str.Data();
  else
    cout << endl << "**ERROR** Couldn't obtain the 'Run Title'.";

  header->GetValue("RunInfo/Run Number", ival, ok);
  if (ok)
    cout << endl << "Run Number: " << ival;
  else
    cout << endl << "**ERROR** Couldn't obtain the 'Run Number'.";

  header->GetValue("RunInfo/Histo Names", strVec, ok);
  if (ok) {
    cout << endl << "Histo Names: ";
    for (UInt_t i=0; i<strVec.size()-1; i++) {
      cout << strVec[i].Data() << ", ";
    }
    cout << strVec[strVec.size()-1].Data();
  } else {
    cout << endl << "**ERROR** Couldn't obtain the 'Histo Names'.";
  }

  header->GetValue("RunInfo/Time Zero Bin", dvec, ok);
  if (ok) {
    cout << endl << "Time Zero Bin: ";
    for (UInt_t i=0; i<dvec.size()-1; i++) {
      cout << dvec[i] << ", ";
    }
    cout << dvec[dvec.size()-1];
  } else {
    cout << endl << "**ERROR** Couldn't obtain the 'Time Zero Bin'.";
  }

  header->GetValue("RunInfo/Sample Temperature", prop, ok);
  if (ok) {
    cout << endl << "Sample Temperature: " << prop.GetValue() << " +- " << prop.GetError() << " " << prop.GetUnit().Data() << "; SP: " << prop.GetDemand() << "; " << prop.GetDescription().Data();
  } else {
    cout << endl << "**ERROR** Couldn't obtain the 'Sample Temperature'.";
  }

  header->GetValue("Detectors/Detector000/Name", str, ok);
  if (ok) {
    cout << endl << "Detectors/Detector000: Name=" << str;
  } else {
    cout << endl << "**ERROR** Couldn't obtain 'Detector/Detector000/Name'.";
  }

  cout << endl << endl;

  delete header;

  return 0;
}
