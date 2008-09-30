/***************************************************************************

  fourierPhaseTest.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

#include <iostream>
using namespace std;

#include "fftw3.h"

#include <TFile.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TColor.h>

int main(int argc, char *argv[])
{
  if (argc != 2) {
    cout << endl << "usage: fourierPhaseTest <root-output-file-name>";
    cout << endl;
    return -1;
  }

  UInt_t   noOfBins = (UInt_t)pow(2, 11);
  Double_t freq = TMath::TwoPi() * 10.0 / (Double_t)noOfBins;
  Double_t phase = 23.7 * TMath::Pi() / 180.0;
  cout << endl << ">> noOfBins = " << noOfBins;
  cout << endl << ">> freq     = " << freq;
  cout << endl << ">> phase    = " << phase;

  fftw_plan fftwPlan;
  fftw_complex *in;
  fftw_complex *out;

  // allocate necessary memory
  in  = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*noOfBins);
  out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex)*noOfBins);

  // generate plan
  fftwPlan = fftw_plan_dft_1d(noOfBins, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  cout << endl << ">> fftwPlan = " << fftwPlan;
  if (!fftwPlan) {
    cout << endl << "**ERROR** Problems to establish a FFTW plan ...";
    cout << endl;
    return -1;
  }

  // fill data
  cout << endl << ">> fill data ...";
  for (UInt_t i=0; i<noOfBins; i++) {
    in[i][0] = cos(freq*i + phase)*exp(-(Double_t)i/(Double_t)noOfBins/0.1);
    in[i][1] = 0.0;
  }

  TCanvas *canv_in = new TCanvas("canv_in", "phase test ..");

  TGraph *gin = new TGraph(noOfBins);
  for (UInt_t i=0; i<noOfBins; i++) {
    gin->SetPoint(i, i, in[i][0]);
  }
  gin->Draw("ap");

  fftw_execute(fftwPlan);

  // find peak
  Double_t dval, peak = 0.0;
  UInt_t peak_pos = 0;
  for (UInt_t i=0; i<noOfBins/2; i++) {
    dval = out[i][0]*out[i][0]+out[i][1]*out[i][1];
    if (peak < dval) {
      peak = dval;
      peak_pos = i;
    }
  }
  cout << endl << ">> peak at peak_pos = " << peak_pos;
  if (2*peak_pos > noOfBins/2)
    peak_pos = noOfBins/4;

  TCanvas *canv_out = new TCanvas("canv_out", "phase test ..");

  TGraph *goutRe = new TGraph(noOfBins/2);
  TGraph *goutIm = new TGraph(noOfBins/2);

  // estimate phase
  Double_t sum;
  Double_t corr_phase;
  Double_t min, min_phase;
  for (Int_t k=-90; k<90; k++) {
    sum = 0.0;
    corr_phase = (Double_t)k/180.0*TMath::Pi();
    for (UInt_t i=0; i<2*peak_pos; i++) {
      sum += out[i][0]*sin(corr_phase) + out[i][1]*cos(corr_phase);
    }
    if (k == -90) {
      min = fabs(sum);
      min_phase = 0.0;
    }
    if (fabs(sum) < fabs(min)) {
      min = fabs(sum);
      min_phase = (Double_t)k;
    }
    cout << endl << ">> phase = " << k << ", sum = " << fabs(sum);
  }

  cout << endl << ">> min = " << min << ", min phase = " << min_phase;
  cout << endl;

  min_phase = -23.7*TMath::Pi()/180.0;
  for (UInt_t i=0; i<noOfBins/2; i++) {
    goutRe->SetPoint(i, i, out[i][0]*cos(min_phase) - out[i][1]*sin(min_phase));
    goutIm->SetPoint(i, i, out[i][0]*sin(min_phase) + out[i][1]*cos(min_phase));
  }
  goutRe->SetMarkerStyle(20); // bullet
  goutRe->SetMarkerColor(TColor::GetColor(0,0,0)); // black
  goutRe->SetLineColor(TColor::GetColor(0,0,0));   // black
  goutRe->SetFillColor(TColor::GetColor(255,255,255)); // white
  goutRe->SetTitle("Re");
  goutIm->SetMarkerStyle(29); // filled star
  goutIm->SetMarkerColor(TColor::GetColor(255,0,0)); // red
  goutIm->SetLineColor(TColor::GetColor(255,0,0));   // red
  goutIm->SetFillColor(TColor::GetColor(255,255,255)); // white
  goutIm->SetTitle("Im");
  goutRe->Draw("apl");
  goutIm->Draw("plsame");

  // write data to file
  TFile f(argv[1], "RECREATE");
  canv_in->Write();
  canv_out->Write();
  f.Close();

  // clean up
  delete canv_in;
  delete gin;
  delete canv_out;
  delete goutRe;
  delete goutIm;

  fftw_destroy_plan(fftwPlan);
  fftw_free(in);
  fftw_free(out);

  cout << endl;

  return 0;
}
