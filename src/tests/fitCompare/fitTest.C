/*******************************************************************
  fitTest.C

  author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$
********************************************************************/

#include <iostream>

#include <TF1.h>
#include <TH1.h>
#include <TVirtualFitter.h>
#include <TCanvas.h>
#include <TMath.h>

const double gGammaMu = TMath::TwoPi()*0.01355;
const double gTauMu   = 2.19705; // in (us)

TF1 *gFitFcn;

//------------------------------------------------------------------
/**
 * <p>Background (flat for the moment)
 *
 * \param x 
 * \param par par[0] is the background
 */
Double_t Background(Double_t *x, Double_t *par)
{
  return par[0];
}

//------------------------------------------------------------------
/**
 * <p>Asymmetry
 *
 * \param x
 * \param par par[0] = asymmetry
 *            par[1] = lambda
 *            par[2] = B
 *            par[3] = phase
 */
Double_t Asymmetry(Double_t *x, Double_t *par)
{
  return par[0]*TMath::Exp(-par[1]*x[0])*TMath::Cos(gGammaMu*par[2]*x[0]+par[3]/180.0*TMath::Pi());
}

//------------------------------------------------------------------
/**
 * <p>Spectrum
 *
 * \param x 
 * \param par 
 */
Double_t Spectrum(Double_t *x, Double_t *par)
{
  return par[0]*TMath::Exp(-x[0]/gTauMu)*(1.0 + Asymmetry(x,&par[1])) + Background(x,&par[5]);
}

//------------------------------------------------------------------
/**
 * <p>fitTest
 *
 */
void fitTest(const char *fln, const char *fitter)
{
  TH1::AddDirectory(kFALSE);
  TCanvas *c1 = new TCanvas("c1","Fitting Test",10,10,500,500);

  // create a TF1 with the range from 0.0 to 12.0 and 6 parameters
  gFitFcn = new TF1("gFitFcn", Spectrum, 0.0, 12.0, 6);
  gFitFcn->SetNpx(63082);

  // set parameters
  gFitFcn->SetParNames("N0", "asym", "lambda", "B", "phase", "Bkg");
  gFitFcn->SetParameter(0, 30.0);   // N0
//  gFitFcn->SetParLimits(0, 0.0, 1.0e6);
  gFitFcn->SetParameter(1, 0.26);    // asym
//  gFitFcn->SetParLimits(1, 0.0, 0.33);
  gFitFcn->SetParameter(2, 0.3);     // lambda
//  gFitFcn->SetParLimits(2, 0.0, 100.0);
  gFitFcn->SetParameter(3, 100.0);   // B
//  gFitFcn->SetParLimits(3, 0.0, 1000.0);
  gFitFcn->SetParameter(4, 0.0);     // phase
//  gFitFcn->SetParLimits(4, -90.0, 90.0);
  gFitFcn->SetParameter(5, 5.0);    // Bkg
//  gFitFcn->SetParLimits(5, 0.0, 1000.0);

  cout << endl << "gFitFcn->Integral(0.0, 12.0) = " << gFitFcn->Integral(0.0, 12.0);
  cout << endl;

  // read data
  ifstream ifs(fln, ifstream::in);
  char line[256];
  TObjArray  *tok;
  TObjString *ostr;
  TString     str;
  Int_t lineNo = 0;
  Double_t dval;
  Double_t data[63082];
  while (ifs.good()) {
    ifs.getline(line, 256);
    if (line[0]=='%') continue;
    str = line;
    if (str.Length() == 0) break;
    tok = str.Tokenize("\t");
    ostr = (TObjString*)tok->At(1);
    str = ostr->GetString();
    dval = str.Atof();
    data[lineNo++] = dval;
    if (tok) delete tok;
  }
  ifs.close();

  // fill histo
  TH1F *histo = new TH1F("histo","Minuit Test",63083,-0.00009765625,12.32080078125);
  for (Int_t i=1; i<63083; i++)
    histo->SetBinContent(i,data[i]);

  histo->Rebin(50);
  histo->Draw();

  gFitFcn->SetParameter(0, 10000.0); // N0
  gFitFcn->SetParameter(1, 0.2);    // asym
  gFitFcn->SetParameter(2, 0.11);    // lambda
  gFitFcn->SetParameter(3, 200.0);   // B
  gFitFcn->SetParameter(4, 6.0);    // phase
  gFitFcn->SetParameter(5, 300.0);  // Bkg
  
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(2);
//  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(1);
  if (!strcmp(fitter, "Minuit2"))
    TVirtualFitter::SetDefaultFitter("Minuit2");
  else
    TVirtualFitter::SetDefaultFitter("Minuit");
  histo->Fit("gFitFcn", ""); // L->likleyhood, E->minos
  histo->Fit("gFitFcn", "ME"); // L->likleyhood, E->minos
}
