/***************************************************************************

  TPofTCalc.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/11/16

***************************************************************************/

/***************************************************************************

  TPofTCalc::FakeData Method based on Andreas Suter's fakeData

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2008 by Andreas Suter                                   *
 *   andreas.suter@psi.ch                                                   *
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

#include "TPofTCalc.h"
#include "fftw3.h"
#include <cmath>
#include <iostream>
#include <cstdio>

#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TFile.h>
#include <TH1F.h>
#include <TFolder.h>

#include "TLemRunHeader.h"

/* USED FOR DEBUGGING -----------------------
#include <ctime>
#include <fstream>
--------------------------------------------*/

//------------------
// Constructor of the TPofTCalc class - it creates the FFT plan
// Parameters: phase, dt, dB
//------------------

TPofTCalc::TPofTCalc (const string &wisdom, const vector<double> &par) : fWisdom(wisdom) {

  fNFFT = ( int(1.0/gBar/par[1]/par[2]+1.0) % 2 ) ? int(1.0/gBar/par[1]/par[2]+2.0) : int(1.0/gBar/par[1]/par[2]+1.0);
  fTBin = 1.0/gBar/double(fNFFT-1)/par[2];

  fFFTin = (double *)malloc(sizeof(double) * fNFFT);
  fFFTout = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (fNFFT/2+1));

  cout << "TPofTCalc::TPofTCalc: Check for the FFT plan..." << endl;

  // Load wisdom from file

  int wisdomLoaded(0);

  FILE *wordsOfWisdomR;
  wordsOfWisdomR = fopen(fWisdom.c_str(), "r");
  if (wordsOfWisdomR == NULL) {
    cout << "TPofTCalc::TPofTCalc: Couldn't open wisdom file ..." << endl;
  } else {
    wisdomLoaded = fftw_import_wisdom_from_file(wordsOfWisdomR);
    fclose(wordsOfWisdomR);
  }

  if (!wisdomLoaded) {
    cout << "TPofTCalc::TPofTCalc: No wisdom is imported..." << endl;
  }

  fFFTplan = fftw_plan_dft_r2c_1d(fNFFT, fFFTin, fFFTout, FFTW_EXHAUSTIVE);

//  cout << &fFFTplan << endl;
}

//--------------
// Method that does the FFT of a given p(B)
//--------------

void TPofTCalc::DoFFT(const TPofBCalc &PofB) {

  vector<double> pB(PofB.DataPB());

/* USED FOR DEBUGGING -----------------------

  time_t seconds;
  seconds = time(NULL);

  vector<double> B(PofB.DataB());
  double Bmin(PofB.GetBmin());

  char debugfile[50];
  int n = sprintf (debugfile, "test_PB_%ld_%f.dat", seconds, Bmin);

  if (n > 0) {
    ofstream of(debugfile);

    for (unsigned int i(0); i<B.size(); i++) {
      of << B[i] << " " << pB[i] << endl;
    }
    of.close();
  }
/--------------------------------------------*/

  for (unsigned int i(0); i<fNFFT; i++) {
    fFFTin[i] = pB[i];
  }
  for (unsigned int i(0); i<fNFFT/2+1; i++) {
    fFFTout[i][0] = 0.0;
    fFFTout[i][1] = 0.0;
  }

//  cout << "perform the Fourier transform..." << endl;

  fftw_execute(fFFTplan);

}

//---------------------
// Method for calculating the muon spin polarization P(t) from the Fourier transformed p(B)
// Parameters: phase, dt, dB
//---------------------

void TPofTCalc::CalcPol(const vector<double> &par) {

  double sinph(sin(par[0]*PI/180.0)), cosph(cos(par[0]*PI/180.0));

  double polTemp(0.0);
  fT.clear();
  fPT.clear();

  for (unsigned int i(0); i<fNFFT/2+1; i++){
    fT.push_back(double(i)*fTBin);
    polTemp = cosph*fFFTout[i][0]*par[2] + sinph*fFFTout[i][1]*par[2];
    fPT.push_back(polTemp);
  }
}

//---------------------
// Method for generating fake LEM decay histograms from p(B)
// Parameters: output filename, par(dt, dB, timeres, channels, asyms, phases, t0s, N0s, bgs)
//---------------------

void TPofTCalc::FakeData(const string &rootOutputFileName, const vector<double> &par) {

  //determine the number of histograms to be built
  unsigned int numHist(0);
  if(!((par.size()-4)%5))
    numHist=(par.size()-4)/5;

  if(!numHist){
    cout << "TPofTCalc::FakeData: The number of parameters for the histogram creation is not correct. Do nothing." << endl;
    return;
  }

  cout << "TPofTCalc::FakeData: " << numHist << " histograms to be built" << endl;

  vector<unsigned int> t0;
  vector<double> asy0;
  vector<double> phase0;
  vector<double> N0;
  vector<double> bg;

  for(unsigned int i(0); i<numHist; i++) {
    t0.push_back(int(par[i+4+numHist*2]));
    asy0.push_back(par[i+4]);
    phase0.push_back(par[i+4+numHist]);
    N0.push_back(par[i+4+numHist*3]);
    bg.push_back(par[i+4+numHist*4]);
  }

  vector<double> param; // Parameters for TPofTCalc::CalcPol
  param.push_back(0.0); // phase
  param.push_back(par[0]); // dt
  param.push_back(par[1]); // dB

  vector< vector<double> > asy;
  vector<double> asydata;
  double ttime(0.0);
  double pol(0.0);

  for(unsigned int i(0); i<numHist; i++) {
    param[0]=phase0[i];
    // calculate asymmetry
    CalcPol(param);
    for(unsigned int j(0); j<par[3]; j++){
      ttime=j*par[2];
      for(unsigned int k(0); k<fT.size()-1; k++){
        if (ttime < fT[k+1]) {
          pol=fPT[k]+(fPT[k+1]-fPT[k])/(fT[k+1]-fT[k])*(ttime-fT[k]);
          asydata.push_back(asy0[i]*pol);
          break;
        }
      }
    }
    asy.push_back(asydata);
    asydata.clear();
    cout << "TPofTCalc::FakeData: " << i+1 << "/" << numHist << " calculated!" << endl;
  }

  // calculate the histograms
  vector< vector<double> > histo;
  vector<double> data;
  double value;

  for (unsigned int i(0); i<numHist; i++) {    // loop over all histos
    for (unsigned int j(0); j<par[3]; j++) { // loop over time
      if (j < t0[i]) // j<t0
        value = bg[i]; // background
      else
        value = N0[i]*exp(-par[2]*double(j-t0[i])/tauMu)*(1.0+asy[i][j-t0[i]])+bg[i];
      data.push_back(value);
    }
    histo.push_back(data);
    data.clear();
    cout << "TPofTCalc::FakeData: " << i+1 << "/" << numHist << " done ...";
  }

  // add Poisson noise to the histograms

  cout << endl << "TPofTCalc::FakeData: Adding Poisson noise ..." << endl;

  TH1F* theoHisto;
  TH1F* fakeHisto;
  vector<TH1F*> histoData;

  TString name;
  for (unsigned int i(0); i<numHist; i++) { // loop over all histos
    // create histos
    name   = "theoHisto";
    name  += i;
    theoHisto = new TH1F(name.Data(), name.Data(), int(par[3]), -par[2]/2.0, (par[3]+0.5)*par[2]);
    if (i < 10)
      name   = "hDecay0";
    else
      name   = "hDecay";
    name  += i;
    fakeHisto = new TH1F(name.Data(), name.Data(), int(par[3]), -par[2]/2.0, (par[3]+0.5)*par[2]);
    // fill theoHisto
    for (unsigned int j(0); j<par[3]; j++)
      theoHisto->SetBinContent(j, histo[i][j]);
    // fill fakeHisto
    fakeHisto->FillRandom(theoHisto, (int)theoHisto->Integral());

    // keep fake data
    histoData.push_back(fakeHisto);

    // cleanup
    if (theoHisto) {
      delete theoHisto;
      theoHisto = 0;
    }
  }

  // save the histograms as root files
  // create run info folder and content
  TFolder *runInfoFolder = new TFolder("RunInfo", "Run Info");
  TLemRunHeader *runHeader = new TLemRunHeader();
  //sprintf(str, "Fake Data generated from %s", pBFileName.Data());
  runHeader->SetRunTitle("Fake Data");
  float fval = par[2]*1000.; //us->ns
  runHeader->SetTimeResolution(fval);
  runHeader->SetNChannels(int(par[3]));
  runHeader->SetNHist(histoData.size());
  double *t0array = new double[histoData.size()];
  for (unsigned int i(0); i<histoData.size(); i++)
    t0array[i] = t0[i];
  runHeader->SetTimeZero(t0array);
  if (t0array)
    delete t0array;
  runInfoFolder->Add(runHeader);

  // create decay histo folder and content
  TFolder *histoFolder = new TFolder("histos", "histos");
  TFolder *decayAnaModule = new TFolder("DecayAnaModule", "DecayAnaModule");
  histoFolder->Add(decayAnaModule);
  // no post pileup corrected (NPP)
  for (unsigned int i(0); i<histoData.size(); i++)
    decayAnaModule->Add(histoData[i]);
  // post pileup corrected (PPC)
  vector<TH1F*> histoDataPPC;
  for (unsigned int i(0); i<histoData.size(); i++) {
    histoDataPPC.push_back(dynamic_cast<TH1F*>(histoData[i]->Clone()));
    if (i < 10)
      name   = "hDecay2";
    else
      name   = "hDecay";
    name  += i;
    histoDataPPC[i]->SetNameTitle(name.Data(), name.Data());
    decayAnaModule->Add(histoDataPPC[i]);
  }

  // write file
  TFile fdf(rootOutputFileName.c_str(), "recreate");
  runInfoFolder->Write("RunInfo", TObject::kSingleKey);
  histoFolder->Write();
  fdf.Close();

  // clean up
  for (unsigned int i(0); i<histo.size(); i++) {
    asy[i].clear();
    histo[i].clear();
  }
  asy.clear();
  histo.clear();

  for (unsigned int i(0); i<histoData.size(); i++) {
    delete histoData[i];
    delete histoDataPPC[i];
  }
  histoData.clear();
  histoDataPPC.clear();

  t0.clear();
  asy0.clear();
  phase0.clear();
  N0.clear();
  bg.clear();

  cout << endl << "TPofTCalc::FakeData: DONE." << endl;

  return;
}

//---------------------
// Method for evaluating P(t) at a given t
//---------------------

double TPofTCalc::Eval(double t) const {
  for (unsigned int i(0); i<fT.size()-1; i++) {
    if (t < fT[i+1])
      return fPT[i]+(fPT[i+1]-fPT[i])/(fT[i+1]-fT[i])*(t-fT[i]);
  }

  cout << "TPofTCalc::Eval: No data for the time " << t << " us available! Returning -999.0 ..." << endl;
  return -999.0;
}

//---------------------
// Destructor of the TPofTCalc class - it saves the FFT plan and cleans up
//---------------------

TPofTCalc::~TPofTCalc() {
  // export wisdom so it has not to be checked for the FFT-plan next time

  FILE *wordsOfWisdomW;
  wordsOfWisdomW = fopen(fWisdom.c_str(), "w");
  if (wordsOfWisdomW == NULL) {
    cout << "TPofTCalc::~TPofTCalc(): Could not open file ... No wisdom is exported..." << endl;
  }

  fftw_export_wisdom_to_file(wordsOfWisdomW);

  fclose(wordsOfWisdomW);

  fftw_destroy_plan(fFFTplan);
  free(fFFTin);
  fftw_free(fFFTout);
//  fftw_cleanup();
  fT.clear();
  fPT.clear();

}

