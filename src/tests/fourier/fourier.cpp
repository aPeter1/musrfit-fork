/***************************************************************************

  fourier.cpp

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

#include <cstdio>

#include <iostream>
#include <vector>
using namespace std;

#include "TFile.h"
#include "TFolder.h"
#include "TH1F.h"

#include "TLemRunHeader.h"
#include "PMusrFourier.h"

void fourier_syntax()
{
  cout << endl << "usage: fourier <root-fln>";
  cout << endl << "       <root-fln> root input file";
  cout << endl << endl;
}

int main(int argc, char *argv[])
{
  PDoubleVector data;
  double timeResolution = -1.0;
  double startTime = 0.0;
  double endTime = 0.0;

  if (argc != 2) {
    fourier_syntax();
    return -1;
  }

  int histoNo = -1;
  cout << endl << ">> Which histo you would like to transform (0=hDecay00, ...): ";
  cin >> histoNo;

  // read file
  TFile f(argv[1], "READ");
  if (f.IsZombie()) {
    cout << endl << "**ERROR** Couldn't open file " << argv[1] << endl << endl;
    return -1;
  }

  // get header info
  TFolder *folder;
  f.GetObject("RunInfo", folder);
  if (!folder) {
    cout << endl << "Couldn't obtain RunInfo from " << argv[1] << endl;
    f.Close();
    return -1;
  }

  // read header and check if some missing run info need to be fed
  TLemRunHeader *runHeader = dynamic_cast<TLemRunHeader*>(folder->FindObjectAny("TLemRunHeader"));

  // check if run header is valid
  if (!runHeader) {
    cout << endl << "Couldn't obtain run header info from ROOT file " << argv[1] << endl;
    f.Close();
    return -1;
  }

  // get time resolution
  timeResolution = runHeader->GetTimeResolution();

  // get number of histogramms
  int noOfHistos = runHeader->GetNHist();
  if (noOfHistos < histoNo+1) {
    cout << endl << "**ERROR** Only " << noOfHistos << " No of histos present (you tried to get histo No " << histoNo << ")" << endl << endl;
    f.Close();
    return -1;
  }

  // get t0's
  Double_t *t0 = runHeader->GetTimeZero();
  char answer[128];
  cout << endl << "t0 of histo No " << histoNo << " = " << t0[histoNo] << ", is this ok (y/n)? ";
  cin >> answer;
  if (strstr(answer, "n")) {
    cout << endl << "please enter the new t0: ";
    cin >> t0[histoNo];
  }


  // read data ---------------------------------------------------------
  // check if histos folder is found
  f.GetObject("histos", folder);
  if (!folder) {
    cout << endl << "Couldn't obtain histos from " << argv[1] << endl;
    f.Close();
    return -1;
  }
  // get data histo
  char histoName[32];
  // read first the data which are NOT post pileup corrected
  sprintf(histoName, "hDecay%02d", histoNo);
  TH1F *histo = dynamic_cast<TH1F*>(folder->FindObjectAny(histoName));
  if (!histo) {
    cout << endl << "PRunDataHandler::ReadRootFile: Couldn't get histo " << histoName;
    f.Close();
    return -1;
  }
  // rebinning?
  unsigned int rebin;
  cout << endl << ">> Rebinning: ";
  cin >> rebin;
  // fill data
  int start = (int)t0[histoNo];

cout << endl << "t0=" << start;
cout << endl << "#bins=" << histo->GetNbinsX();
  for (int i=start; i<histo->GetNbinsX(); i++) {
    data.push_back(histo->GetBinContent(i));
  }

  f.Close();

  cout << endl << ">> Start Time in (us): ";
  cin >> startTime;
  cout << endl << ">> End Time in (us): ";
  cin >> endTime;

  PMusrFourier fourier(F_SINGLE_HISTO, data, timeResolution, startTime, endTime, 
                       rebin, F_ESTIMATE_N0_AND_BKG);

  if (fourier.IsValid()) {
    fourier.Transform(F_APODIZATION_NONE, F_FILTER_NONE);
  } else {
    cout << endl << "**ERROR** something went wrong when invoking the PMusrFourier object ...";
    cout << endl << endl;
  }

  return 0;
}
