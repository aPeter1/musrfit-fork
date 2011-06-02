/***************************************************************************

  psi_runHeader_test.cpp

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

void psi_runHeader_test_syntax()
{
  cout << endl << "usage: psi_runHeader_test <fileName>";
  cout << endl << "       <fileName> is the file name including the extention root, e.g. test.root";
  cout << endl << endl;
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    psi_runHeader_test_syntax();
    return 1;
  }

  TFile *f = new TFile(argv[1], "RECREATE", "psi_runHeader_test");
  if (f->IsZombie()) {
    delete f;
    return -1;
  }

  // root file header related things
  TFolder *runInfo = gROOT->GetRootFolder()->AddFolder("RunInfo", "PSI RunInfo");
  gROOT->GetListOfBrowsables()->Add(runInfo, "RunInfo");

  TPsiRunHeader *header = new TPsiRunHeader();

  runInfo->Add(header); // add header to RunInfo folder

  header->SetRunTitle("This is a run title");  
  header->SetRunNumber(12345);
  header->SetLab("PSI");
  header->SetInstrument("LEM");
  header->SetSetup("Konti-4, WEW");
  header->SetSample("Eu2CuO4 MOD thin film");
  header->SetOrientation("c-axis perp to spin");

  header->AddProperty("T0", 30.01, 0.05, "K");
  header->AddProperty("T1", 30.03, 0.03, "K");
  header->AddProperty("Field", 3.03, 0.03, "T");
  header->AddProperty("ThisIsAVeryLongPropertyName", 3.03, 0.03, "SI-Unit");

  header->DumpHeader();

  runInfo->Write();

  f->Close();

  delete f;

  return 0;
}
