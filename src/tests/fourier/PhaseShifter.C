void PhaseShifter(Double_t phase, TH1F *re, TH1F *im, TH1F *pwr)
{
  static Double_t phaseAbs = 0.0;
  phaseAbs += phase;
  cout << ">> phase = " << phaseAbs << endl;

  Int_t noOfEntries = re->GetEntries();
  Double_t min = re->GetBinLowEdge(1);
  Int_t maxBin = re->GetNbinsX()-1;
  Double_t max = re->GetBinLowEdge(maxBin)+re->GetBinWidth(maxBin);
  TH1F reShift("reShift", "reShift", noOfEntries, min, max);
  TH1F imShift("imShift", "imShift", noOfEntries, min, max);

  Double_t phaseRad = phase/180.0*TMath::Pi();
  for (Int_t i=1; i<noOfEntries-1; i++) {
    reShift.SetBinContent(i, re->GetBinContent(i)*TMath::Cos(phaseRad) - im->GetBinContent(i)*TMath::Sin(phaseRad));
    imShift.SetBinContent(i, im->GetBinContent(i)*TMath::Cos(phaseRad) + re->GetBinContent(i)*TMath::Sin(phaseRad));
  }

  for (Int_t i=1; i<noOfEntries-1; i++) {
    re->SetBinContent(i, reShift.GetBinContent(i));
    im->SetBinContent(i, imShift.GetBinContent(i));
  }

  re->SetMarkerStyle(20);
  re->SetMarkerColor(TColor::GetColor(0,0,0));
  re->SetLineColor(TColor::GetColor(0,0,0));
  im->SetMarkerStyle(21);
  im->SetMarkerColor(TColor::GetColor(255,0,0));
  im->SetLineColor(TColor::GetColor(255,0,0));
  pwr->SetMarkerStyle(22);
  pwr->SetMarkerColor(TColor::GetColor(0,0,255));
  pwr->SetLineColor(TColor::GetColor(0,0,255));
  re->Draw("alp");
  im->Draw("plsame");
  pwr->Draw("plsame");
}
