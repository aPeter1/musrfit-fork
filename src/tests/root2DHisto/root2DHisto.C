{
  gStyle->SetOptStat(000000000);

  TCanvas *cov = new TCanvas("cov", "Minuit2 Output Covariance Matrix", 500, 500);

  TH2D *f = new TH2D("f", "Minuit2 Output Covariance Matrix", 16, 0.0, 16.0, 16, 0.0, 16.0);

  double x, y, z;

  for (unsigned int i=0; i<16; i++) {
    for (unsigned int j=0; j<16; j++) {
      x = (double)i;
      y = (double)j;
      z = x/16.0-y/16.0+(x-8.0)*(y-8.0)/(8.0*8.0);
      f->Fill(x,y,z);
    }
  }

  TFile ff("test.root", "recreate");
  cov->Draw();
  f->Draw("COLZ");
  cov->Write();
  ff.Close();
}