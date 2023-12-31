/***************************************************************************

  TPofTCalc.cpp

  Author: Bastian M. Wojek

***************************************************************************/

/***************************************************************************

  TPofTCalc::FakeData Method based on Andreas Suter's fakeData

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2008-2023 by Bastian M. Wojek, Andreas Suter            *
 *                                                                         *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "TPofTCalc.h"
#include "fftw3.h"
#include <cmath>
#include <iostream>
#include <cstdio>
#include <cstdlib>

#ifdef HAVE_GOMP
#include <omp.h>
#endif

#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TFile.h>
#include <TH1F.h>
#include <TFolder.h>

#include "TLemRunHeader.h"

/* USED FOR DEBUGGING -----------------------*/
#include <ctime>
#include <fstream>
/*--------------------------------------------*/

//------------------
// Constructor of the TPofTCalc class - it creates the FFT plan
// Parameters: phase, dt, dB
//------------------

TPofTCalc::TPofTCalc (const TPofBCalc *PofB, const std::string &wisdom, const std::vector<double> &par) : fWisdom(wisdom) {

#if !defined(_WIN32GCC) && defined(HAVE_LIBFFTW3_THREADS) && defined(HAVE_GOMP)
  int init_threads(fftw_init_threads());
  if (!init_threads)
    std::cout << "TPofTCalc::TPofTCalc: Couldn't initialize multiple FFTW-threads ..." << std::endl;
  else
    fftw_plan_with_nthreads(omp_get_num_procs());
#endif

  fNFFT = static_cast<int>(1.0/(gBar*par[1]*par[2]));
  if (fNFFT % 2) {
    fNFFT += 1;
  } else {
    fNFFT += 2;
  }

  fTBin = 1.0/(gBar*double(fNFFT-1)*par[2]);

  const int NFFT_2p1(fNFFT/2 + 1);

  // allocating memory for the time- and polarisation vectors

  fT = new double[NFFT_2p1]; //static_cast<double *>(malloc(sizeof(double) * NFFT_2p1));
  fPT = new double[NFFT_2p1]; //static_cast<double *>(malloc(sizeof(double) * NFFT_2p1));

  int i;

  #ifdef HAVE_GOMP
  int chunk = NFFT_2p1/omp_get_num_procs();
  if (chunk < 10)
    chunk = 10;
  #pragma omp parallel for default(shared) private(i) schedule(dynamic,chunk)
  #endif
  for (i = 0; i < NFFT_2p1; ++i) {
    fT[i] = static_cast<double>(i)*fTBin;
  }

  fFFTin = PofB->DataPB();
  fFFTout = new fftw_complex[NFFT_2p1]; //static_cast<fftw_complex *>(fftw_malloc(sizeof(fftw_complex) * NFFT_2p1));

  // Load wisdom from file if it exists and should be used

  fUseWisdom = true;
  int wisdomLoaded(0);

  FILE *wordsOfWisdomR;
  wordsOfWisdomR = fopen(wisdom.c_str(), "r");
  if (wordsOfWisdomR == NULL) {
    fUseWisdom = false;
  } else {
    wisdomLoaded = fftw_import_wisdom_from_file(wordsOfWisdomR);
    fclose(wordsOfWisdomR);
  }

  if (!wisdomLoaded) {
    fUseWisdom = false;
  }

// create the FFT plan

  if (fUseWisdom)
    fFFTplan = fftw_plan_dft_r2c_1d(fNFFT, fFFTin, fFFTout, FFTW_EXHAUSTIVE);
  else
    fFFTplan = fftw_plan_dft_r2c_1d(fNFFT, fFFTin, fFFTout, FFTW_ESTIMATE);

}

//---------------------
// Destructor of the TPofTCalc class - it saves the FFT plan and cleans up
//---------------------

TPofTCalc::~TPofTCalc() {
  // if a wisdom file is used export the wisdom so it has not to be checked for the FFT-plan next time
  if (fUseWisdom) {
    FILE *wordsOfWisdomW;
    wordsOfWisdomW = fopen(fWisdom.c_str(), "w");
    if (wordsOfWisdomW == NULL) {
      std::cout << "TPofTCalc::~TPofTCalc(): Could not open file ... No wisdom is exported..." << std::endl;
    } else {
      fftw_export_wisdom_to_file(wordsOfWisdomW);
      fclose(wordsOfWisdomW);
    }
  }

  // clean up

  fftw_destroy_plan(fFFTplan);
  delete[] fFFTout; //fftw_free(fFFTout);
  fFFTout = 0;
//  fftw_cleanup();
//  fftw_cleanup_threads();

  delete[] fT;
  fT = 0;
  delete[] fPT;
  fPT = 0;
}

//--------------
// Method that does the FFT of a given p(B)
//--------------

void TPofTCalc::DoFFT() {

  fftw_execute(fFFTplan);

}

//---------------------
// Method for calculating the muon spin polarization P(t) from the Fourier transformed p(B)
// Parameters: phase, dt, dB
//---------------------

void TPofTCalc::CalcPol(const std::vector<double> &par) {

  double sinph(sin(par[0]*PI/180.0)), cosph(cos(par[0]*PI/180.0));
  int i;

  #ifdef HAVE_GOMP
  int chunk = (fNFFT/2+1)/omp_get_num_procs();
  if (chunk < 10)
    chunk = 10;
  #pragma omp parallel for default(shared) private(i) schedule(dynamic,chunk)
  #endif
  for (i=0; i<fNFFT/2+1; ++i) {
    fPT[i] = (cosph*fFFTout[i][0] + sinph*fFFTout[i][1])*par[2];
  }
}

//---------------------
// Method for evaluating P(t) at a given t
//---------------------

double TPofTCalc::Eval(double t) const {

  int i(static_cast<int>(t/fTBin));
  if (i < fNFFT/2){
    return fPT[i]+(fPT[i+1]-fPT[i])/(fT[i+1]-fT[i])*(t-fT[i]);
  }
//as35  std::cout << "TPofTCalc::Eval: No data for the time " << t << " us available! Returning -999.0 ..." << std::endl;
  return -999.0;
}



//---------------------
// Method for generating fake LEM decay histograms from p(B)
// Parameters: output filename, par(dt, dB, timeres, channels, asyms, phases, t0s, N0s, bgs) optPar(field, energy)
//---------------------

void TPofTCalc::FakeData(const string &rootOutputFileName, const std::vector<double> &par, const std::vector<double> *optPar = 0) {

  //determine the number of histograms to be built
  unsigned int numHist(0);
  if(!((par.size()-4)%5))
    numHist=(par.size()-4)/5;

  if(!numHist){
    std::cout << "TPofTCalc::FakeData: The number of parameters for the histogram creation is not correct. Do nothing." << std::endl;
    return;
  }

  std::cout << "TPofTCalc::FakeData: " << numHist << " histograms to be built" << std::endl;

  int nChannels = int(par[3]);
  std::vector<int> t0;
  std::vector<double> asy0;
  std::vector<double> phase0;
  std::vector<double> N0;
  std::vector<double> bg;

  for(unsigned int i(0); i<numHist; ++i) {
    t0.push_back(int(par[i+4+numHist*2]));
    asy0.push_back(par[i+4]);
    phase0.push_back(par[i+4+numHist]);
    N0.push_back(par[i+4+numHist*3]);
    bg.push_back(par[i+4+numHist*4]);
  }

  std::vector<double> param; // Parameters for TPofTCalc::CalcPol
  param.push_back(0.0); // phase
  param.push_back(par[0]); // dt
  param.push_back(par[1]); // dB

  std::vector< std::vector<double> > asy;
  std::vector<double> asydata(nChannels);
  double ttime;
  int j,k;

  #ifdef HAVE_GOMP
  int chunk = nChannels/omp_get_num_procs();
  if (chunk < 10)
    chunk = 10;
  #endif

  for(unsigned int i(0); i<numHist; ++i) {
    param[0]=phase0[i];
    // calculate asymmetry
    CalcPol(param);

    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(j,ttime,k) schedule(dynamic,chunk)
    #endif
    for(j=0; j<nChannels; ++j) {
      ttime=j*par[2];
      k = static_cast<int>(floor(ttime/fTBin));
      asydata[j]=asy0[i]*(fPT[k]+(fPT[k+1]-fPT[k])/fTBin*(ttime-fT[k]));
    }
// end omp

//       for(unsigned int k(0); k<fT.size()-1; k++){
//         if (ttime < fT[k+1]) {
//           pol=fPT[k]+(fPT[k+1]-fPT[k])/(fT[k+1]-fT[k])*(ttime-fT[k]);
//           asydata.push_back(asy0[i]*pol);
//           break;
//         }
//       }
    asy.push_back(asydata);
//    asydata.clear();
    std::cout << "TPofTCalc::FakeData: " << i+1 << "/" << numHist << " calculated!" << std::endl;
  }

  // calculate the histograms
  std::vector< std::vector<double> > histo;
  std::vector<double> data(nChannels);

  for (unsigned int i(0); i<numHist; ++i) {    // loop over all histos

    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(j) schedule(dynamic,chunk)
    #endif
    for (j = 0; j<nChannels; ++j) { // loop over time
      if (j < t0[i]) // j<t0
        data[j] = bg[i]; // background
      else
        data[j] = N0[i]*exp(-par[2]*static_cast<double>(j-t0[i])/tauMu)*(1.0+asy[i][j-t0[i]])+bg[i];
    }
// end omp

    histo.push_back(data);
    std::cout << "TPofTCalc::FakeData: " << i+1 << "/" << numHist << " done ..." << std::endl;
  }

  // add Poisson noise to the histograms

  std::cout << "TPofTCalc::FakeData: Adding Poisson noise ..." << std::endl;

  TH1F* theoHisto;
  TH1F* fakeHisto;
  std::vector<TH1F*> histoData;

  TString name;
  for (unsigned int i(0); i<numHist; ++i) { // loop over all histos
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
    #ifdef HAVE_GOMP
    #pragma omp parallel for default(shared) private(j) schedule(dynamic,chunk)
    #endif
    for (j = 0; j<nChannels; ++j)
      theoHisto->SetBinContent(j, histo[i][j]);
// end omp

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

  std::cout << "TPofTCalc::FakeData: Write histograms and header information to the file ..." << std::endl;

  // save the histograms as root files
  // create run info folder and content
  TFolder *runInfoFolder = new TFolder("RunInfo", "Run Info");
  TLemRunHeader *runHeader = new TLemRunHeader();
  //snprintf(str, sizeof(str), "Fake Data generated from %s", pBFileName.Data());
  runHeader->SetRunTitle("Fake Data");
  if (optPar && (optPar->size() > 1)) { // set energy and field if they were specified
    runHeader->SetImpEnergy((*optPar)[1]);
    runHeader->SetSampleBField((*optPar)[0], 0.0f);
  }
  float fval = par[2]*1000.; //us->ns
  runHeader->SetTimeResolution(fval);
  runHeader->SetNChannels(nChannels);
  runHeader->SetNHist(histoData.size());
  double *t0array = new double[histoData.size()];
  for (unsigned int i(0); i<histoData.size(); i++)
    t0array[i] = t0[i];
  runHeader->SetTimeZero(t0array);
  if (t0array) {
    delete[] t0array;
    t0array = 0;
  }
  runInfoFolder->Add(runHeader);

  // create decay histo folder and content
  TFolder *histoFolder = new TFolder("histos", "histos");
  TFolder *decayAnaModule = new TFolder("DecayAnaModule", "DecayAnaModule");
  histoFolder->Add(decayAnaModule);
  // no post pileup corrected (NPP)
  for (unsigned int i(0); i<histoData.size(); i++)
    decayAnaModule->Add(histoData[i]);
  // post pileup corrected (PPC)
  std::vector<TH1F*> histoDataPPC;
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
  fakeHisto = 0;

  histoFolder->Clear();
  delete histoFolder; histoFolder = 0;
  decayAnaModule->Clear();
  delete decayAnaModule; decayAnaModule = 0;

  runInfoFolder->Clear();
  delete runInfoFolder; runInfoFolder = 0;
  delete runHeader; runHeader = 0;

  t0.clear();
  asy0.clear();
  phase0.clear();
  N0.clear();
  bg.clear();

  std::cout << "TPofTCalc::FakeData: DONE." << std::endl << std::endl;

  return;
}



