Double_t sKTLF(Double_t *x, Double_t *par)
{
  Double_t result;
  Double_t t = x[0];
  Double_t delta = par[0];
  Double_t w0 = par[1];
  TF1 gs("gs", "exp(-0.5*pow([0]*x,2.0))*sin([1]*x)", 0.0, 13.0);
  gs.SetParameters(delta, w0);
  gs.SetNpx(1000);

  if (w0 == 0.0) {
    result = 1.0/3.0 + 2.0/3.0 * (1.0 - TMath::Power(delta*t,2.0)) * 
             TMath::Exp(-0.5*TMath::Power(delta*t,2.0));
  } else {
    result = 1.0 - 2.0*TMath::Power(delta/w0,2.0)*(1.0- 
                     TMath::Exp(-0.5*TMath::Power(delta*t,2.0))*TMath::Cos(w0*t))+
                   2.0*TMath::Power(delta,4.0)/TMath::Power(w0,3.0)*gs.Integral(0.0,t);
  }

  return result;
}

void staticKT_LF()
{
  TF1 *fKTLF = new TF1("sKTLF", sKTLF, 0.0, 13.0, 2);

  fKTLF->SetParameters(0.51, 0.0); // Delta = 6G, B0 = 0G
  fKTLF->SetNpx(1000);
  fKTLF->DrawCopy();
  fKTLF->SetParameters(0.51, 0.85); // Delta = 6G, B0 = 10G
  fKTLF->DrawCopy("sames");
  fKTLF->SetParameters(0.51, 1.70); // Delta = 6G, B0 = 20G
  fKTLF->DrawCopy("sames");
  fKTLF->SetParameters(0.51, 4.26); // Delta = 6G, B0 = 50G
  fKTLF->DrawCopy("sames");
  fKTLF->SetParameters(0.51, 8.51); // Delta = 6G, B0 = 100G
  fKTLF->DrawCopy("sames");
}