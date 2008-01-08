{
  TFile f("test.root", "recreate");

  // make a histo with errors
  TH1F *h  = new TH1F("h", "histo test", 20, -0.05, 1.05);
  TH1F *hc = new TH1F("hc", "curve", 20, -0.05, 1.05);
  for (unsigned int i=0; i<20; i++) {
    x = (double)i/20.0;
    value = TMath::Sqrt(1.5*x+0.1*TMath::Power(x,3.0)-0.4*TMath::Power(x,5.0));
    hc->SetBinContent(i, value+0.1*TMath::Sqrt(value)-value*value);
    h->SetBinContent(i, value);
    h->SetBinError(i, TMath::Power(value, 5.0));
  }

  TCanvas *c1 = new TCanvas("c1", "my Canvas c1", 10, 10, 800, 600);

  h->SetMarkerStyle(20);
  h->Draw("*H HIST E1");

  hc->SetLineColor(2);
  hc->SetLineWidth(3);
  hc->Draw("C SAME");

  TCanvas *c2 = new TCanvas("c2", "my Canvas c2", 10, 10, 800, 600);
  h->SetMarkerStyle(23);
  h->Draw("*H HIST E1");

  f.WriteTObject(c1);
  f.WriteTObject(c2);
}
