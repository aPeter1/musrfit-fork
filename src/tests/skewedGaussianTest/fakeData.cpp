/***************************************************************************

  fakeData.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2008 by Andreas Suter                                   *
 *   andreas.suter@psi.c                                                   *
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
#include <fstream>
#include <vector>
using namespace std;

#include <TMath.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TFolder.h>

#include "TLemRunHeader.h"

typedef vector<double> PDoubleVector;
typedef vector<int>    PIntVector;

void fakeDataSyntax()
{
  cout << endl << "usage: fakeData <pB-File> <paramInputFile> <outputFile> [--dump <dumpFln>]";
  cout << endl << "  <pB-File>:        file holding p(B) in columns B, pB, separated by ','";
  cout << endl << "                    comment lines start with a '#'";
  cout << endl << "  <paramInputFile>: holding parameters needed to generate the histograms";
  cout << endl << "                    (see below)";
  cout << endl << "  <rootOutputFile>: output file name of the fake data histo file";
  cout << endl << "  --dump <dumpFln>: option to dump p(B) and A_i(t) in a separate root file";
  cout << endl;
  cout << endl << "<paramInputFile> structure:";
  cout << endl << "  comment lines start with a '#'";
  cout << endl << "  time resolution : \"resolution\", followed by time resolution in (us)";
  cout << endl << "  No of channles  : \"channels\", followed by number of channels in a histogram";
  cout << endl << "  t0 data line    : \"t0\", followed by the t0's for each histogram, separated by ','";
  cout << endl << "  asym data line  : \"asym\",  followed by the asymmetries for each histogram";
  cout << endl << "  phases data line: \"phase\", followed by the phases for each histogram";
  cout << endl << "  N0 data line    : \"N0\", followed by the N0's for each histogram";
  cout << endl << "  Bkg data line   : \"bkg\", followed by the background for each histogram";
  cout << endl << "  Example: ";
  cout << endl << "  # example";
  cout << endl << "  resolution, 0.0001953125";
  cout << endl << "  channels, 66601";
  cout << endl << "  t0, 3300, 3300, 3300, 3300";
  cout << endl << "  asym, 0.26, 0.26, 0.26, 0.26";
  cout << endl << "  phase, 0.0, 90.0, 180.0, 270.0";
  cout << endl << "  N0, 1000, 1100, 980, 1020";
  cout << endl << "  bkg, 8, 11, 6, 15";
  cout << endl;
}

int main(int argc, char *argv[])
{
  const Double_t gamma_mu = TMath::TwoPi() * 0.1355; // in (rad/ns/T)
  const Double_t tau_mu   = 2197.147; // muon life time in ns

  if ((argc != 4) && (argc !=6)) {
    fakeDataSyntax();
    return -1;
  }

  TString pBFileName(argv[1]);
  TString paramInputFileName(argv[2]);
  TString rootOutputFileName(argv[3]);
  TString dumpFileName("");
  if (argc == 6) {
    dumpFileName = TString(argv[5]);
  }

  vector<Double_t> B, pB;

  cout << endl << ">> read p(B) file ...";

  // read pB input file and fill B and pB
  ifstream f;

  f.open(pBFileName.Data(), iostream::in);
  if (!f.is_open()) {
    cout << endl << "**ERROR**: Sorry, couldn't open input file (p(B) file): " << pBFileName.Data();
    cout << endl << "  Will quit";
    cout << endl;
    return -1;
  }

  char str[256];
  Double_t bb, ppb;
  int status, lineNo=0;
  while (!f.eof()) {
    // read a line
    f.getline(str, sizeof(str));

    // ignore comments or empty lines
    if ((str[0] == '#') || (strlen(str)==0))
      continue;

    // get data
    status = sscanf(str, "%lf, %lf", &bb, &ppb);
    if (status != 2) {
      cout << endl << "**ERROR**: Problems while reading the input file (line no " << lineNo << "), will quit.";
      cout << endl << "  status = " << status;
      cout << endl;
      f.close();
      return -1;
    }

    // fill B and pB vector
    B.push_back(bb*1.0e-4); // G -> T
    pB.push_back(ppb);

    lineNo++;
  }

  f.close();

  // normalize p(B)
  Double_t sum = 0.0;
  for (unsigned int i=0; i<pB.size(); i++) {
    sum += pB[i];
  }
  for (unsigned int i=0; i<pB.size(); i++) {
    pB[i] /= sum;
  }

  // parameter file variables
  Double_t timeResolution;
  Int_t noOfChannels;
  vector<Int_t>    t0;
  vector<Double_t> asym;
  vector<Double_t> phase;
  vector<Int_t>    N0;
  vector<Int_t>    bkg;

  cout << endl << ">> read parameter input file ...";

  // open parameter input file and extract the parameters
  f.open(paramInputFileName.Data(), iostream::in);
  if (!f.is_open()) {
    cout << endl << "**ERROR**: Sorry, couldn't open parameter input file: " << paramInputFileName.Data();
    cout << endl << "  Will quit";
    cout << endl;
    return -1;
  }

  TObjArray  *tokens;
  TObjString *ostr;
  TString     tstr;
  Double_t    dval;
  Int_t       ival;

  lineNo = 0;
  while (!f.eof()) {
    // read a line
    f.getline(str, sizeof(str));
    lineNo++;

    // ignore comments or empty lines
    if ((str[0] == '#') || (strlen(str)==0))
      continue;

    // tokenize string
    tokens = TString(str).Tokenize(", \t");

    // find to proper tag and fill the parameters
    if (tokens->GetEntries() < 2) {
      cout << endl << "**ERROR**: found '" << str << "'";
      cout << endl << "  This is not compatible with the parameter input file, will quit";
      cout << endl;
      return -1;
    }

    ostr = dynamic_cast<TObjString*>(tokens->At(0));
    tstr = ostr->GetString();
    tstr.ToLower();
    if (tstr.Contains("resolution")) { // handle time resolution
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      timeResolution = ostr->GetString().Atof()*1000.0; // us->ns
      if (timeResolution <= 0.0) {
        cout << endl << "**ERROR** time resolution must be > 0.0!";
        cout << endl;
        return -1;
      }
    } else if (tstr.Contains("channels")) { // handle number of channels
      ostr = dynamic_cast<TObjString*>(tokens->At(1));
      noOfChannels = ostr->GetString().Atoi();
      if (noOfChannels <= 0.0) {
        cout << endl << "**ERROR** number of channels must be > 0.0!";
        cout << endl;
        return -1;
      }
    } else if (tstr.Contains("t0")) { // handle t0's
      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        ival = ostr->GetString().Atoi();
        t0.push_back(ival);
      }
    } else if (tstr.Contains("asym")) { // handle asymmetries
      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        dval = ostr->GetString().Atof();
        asym.push_back(dval);
      }
    } else if (tstr.Contains("phase")) { // handle phases
      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        dval = ostr->GetString().Atof();
        phase.push_back(dval/180.0*TMath::Pi());
      }
    } else if (tstr.Contains("n0")) { // handle N0's
      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        ival = ostr->GetString().Atoi();
        N0.push_back(ival);
      }
    } else if (tstr.Contains("bkg")) { // handle bkg's
      for (Int_t i=1; i<tokens->GetEntries(); i++) {
        ostr = dynamic_cast<TObjString*>(tokens->At(i));
        ival = ostr->GetString().Atoi();
        bkg.push_back(ival);
      }
    } else {
      cout << endl << "**ERROR** while reading the input file in line " << lineNo;
      cout << endl;
      return -1;
    }

    // clean up
    if (tokens) {
      delete tokens;
      tokens = 0;
    }
  }

  f.close();

  cout << endl << ">> number of pB's     = " << pB.size();
  cout << endl << ">> number of channels = " << noOfChannels;

  // check that all the parameter vectors have the same length
  if ((t0.size() != asym.size()) || (t0.size() != phase.size()) ||
      (t0.size() != N0.size()) || (t0.size() != bkg.size())) {
    cout << endl << "**ERROR** the number of elements of t0's etc. in the input parameter file must be the same!";
    cout << endl;
    return -1;
  }

  cout << endl << ">> calculate all the A_i(t) ..." << endl;

  PDoubleVector ddata;
  vector<PDoubleVector> asymmetry;
  // calculate the asymmetry
  for (UInt_t i=0; i<asym.size(); i++) {   // loop over all histos
    for (Int_t j=0; j<noOfChannels; j++) { // loop over time
      for (UInt_t k=0; k<pB.size(); k++) { // loop over p(B)
        if (j < t0[i])
          dval += 0.0;
        else
          dval += pB[k]*TMath::Cos(gamma_mu*B[k]*timeResolution*(j-t0[i])+phase[i]);
      }
      ddata.push_back(asym[i]*dval);
      dval = 0.0;
    }
    asymmetry.push_back(ddata);
    ddata.clear();
    cout << endl << ">> asymmetry " << i+1 << "/" << asym.size() << " done ...";
  }

  cout << endl << ">> calculate all the histo_i(t) ..." << endl;

  // calculate the histograms
  PIntVector idata;
  vector<PIntVector> histo;
  for (UInt_t i=0; i<asym.size(); i++) {    // loop over all histos
    for (Int_t j=0; j<noOfChannels; j++) { // loop over time
      if (j < t0[i])
        dval = bkg[i];
      else
        dval = N0[i]*TMath::Exp(-timeResolution*(j-t0[i])/tau_mu)*(1.0+asymmetry[i][j])+bkg[i];
      idata.push_back((int)dval);
    }
    histo.push_back(idata);
    idata.clear();
    cout << endl << ">> histo " << i+1 << "/" << asym.size() << " done ...";
  }


  if (dumpFileName.Length() > 0) {
    // dump some test stuff
    TFile tf(dumpFileName.Data(), "recreate");

    TCanvas *c1 = new TCanvas("c1", "p(B)", 10, 10, 800, 600);
    c1->Show();

    // fill in the TH1F's TGraph's etc.
    TGraph *graph_pB = new TGraph(B.size());
    for (UInt_t i=0; i<B.size(); i++) {
      graph_pB->SetPoint(i, B[i], pB[i]);
    }
    graph_pB->SetMarkerStyle(21);
    graph_pB->DrawClone("apl");

    tf.WriteTObject(c1);

    if (graph_pB)
      delete graph_pB;
    if (c1)
      delete c1;

    TCanvas *chist;
    TH1F    *hh;
    TString name, title;
    for (UInt_t i=0; i<asym.size(); i++) {
      name   = "chist";
      name  += i;
      title  = "histo";
      title += i;
      chist = new TCanvas(name.Data(), title.Data(), 10, 10, 800, 600);
      chist->Show();

      name   = "hh";
      name  += i;
      title  = "histo";
      title += i;
      hh = new TH1F(name.Data(), title.Data(), noOfChannels, 
                    -timeResolution/2.0, (noOfChannels+0.5)*timeResolution);
      for (Int_t j=0; j<noOfChannels; j++) {
        hh->SetBinContent(j, asymmetry[i][j]);
      }
      hh->Draw("*H HIST");

      tf.WriteTObject(chist);

      if (hh)
        delete hh;
      if (chist)
        delete chist;
    }

    tf.Close();
  }

  // add Poisson noise to the histograms

  cout << endl << ">> add Poisson noise ..." << endl;

  TH1F* theoHisto;
  TH1F* fakeHisto;
  vector<TH1F*> histoData;

  TString name;
  for (UInt_t i=0; i<asym.size(); i++) { // loop over all histos
    // create histos
    name   = "theoHisto";
    name  += i;
    theoHisto = new TH1F(name.Data(), name.Data(), noOfChannels,
                         -timeResolution/2.0, (noOfChannels+0.5)*timeResolution);
    if (i < 10)
      name   = "hDecay0";
    else
      name   = "hDecay";
    name  += i;
    fakeHisto = new TH1F(name.Data(), name.Data(), noOfChannels,
                         -timeResolution/2.0, (noOfChannels+0.5)*timeResolution);
    // fill theoHisto
    for (Int_t j=0; j<noOfChannels; j++)
      theoHisto->SetBinContent(j, histo[i][j]);
    // fill fakeHisto
    fakeHisto->FillRandom(theoHisto, (Int_t)theoHisto->Integral());

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
  sprintf(str, "Fake Data generated from %s", pBFileName.Data());
  runHeader->SetRunTitle(str);
  Float_t fval = timeResolution;
  runHeader->SetTimeResolution(fval);
  runHeader->SetNChannels(noOfChannels);
  runHeader->SetNHist(histoData.size());
  Int_t *t0array = new Int_t[histoData.size()];
  for (UInt_t i=0; i<histoData.size(); i++)
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
  for (UInt_t i=0; i<histoData.size(); i++)
    decayAnaModule->Add(histoData[i]);
  // post pileup corrected (PPC)
  vector<TH1F*> histoDataPPC;
  for (UInt_t i=0; i<histoData.size(); i++) {
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
  TFile fdf(rootOutputFileName.Data(), "recreate");
  runInfoFolder->Write("RunInfo", TObject::kSingleKey);
  histoFolder->Write();
  fdf.Close();

  // clean up
  B.clear();
  pB.clear();
  t0.clear();
  asym.clear();
  phase.clear();
  N0.clear();
  bkg.clear();

  for (UInt_t i=0; i<histo.size(); i++) {
    asymmetry[i].clear();
    histo[i].clear();
  }
  asymmetry.clear();
  histo.clear();

  for (UInt_t i=0; i<histoData.size(); i++) {
    delete histoData[i];
    delete histoDataPPC[i];
  }
  histoData.clear();
  histoDataPPC.clear();

  cout << endl << ">> DONE." << endl;

  return 0;
}
