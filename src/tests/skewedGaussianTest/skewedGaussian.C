/***************************************************************************

  skewedGaussian.C

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

//-------------------------------------------------------------------------
Double_t PofB(Double_t B, Double_t B0, Double_t sm, Double_t sp)
{
  Double_t val = 0.0;

  if (B < B0) {
    val = TMath::Exp(-0.5*TMath::Power((B-B0)/sm, 2.0));
  } else {
    val = TMath::Exp(-0.5*TMath::Power((B-B0)/sp, 2.0));
  }

  val *= TMath::Sqrt(2.0/TMath::Pi())/(sm+sp);

  return val;
}

//-------------------------------------------------------------------------
Double_t PofBext(Double_t B, Double_t B0, Double_t s)
{
  Double_t val = 0.0;

  val = 1.0/TMath::Sqrt(2.0*TMath::Pi())/s*TMath::Exp(-0.5*TMath::Power((B-B0)/s, 2.0));

  return val;
}

//-------------------------------------------------------------------------
void skewedGaussian()
{
  FILE *fp;
  char fln[256];

  const Double_t w  = 1.0;   // weight of the skewed Gaussian
  const Double_t B0 = 2500.0; // skewed Gaussian B0 (G)
  const Double_t sm = 4.5;   // skewed Gaussian sigma- (G)
  const Double_t sp = 4.5;   // skewed Gaussian sigma+ (G)

  const Double_t B0ext = 110.0; // external field Gaussian B0 (G)
  const Double_t sext  = 1.2;   // external field Gaussian sigma (G)

  sprintf(fln, "skewedGauss-B%0.2lf-sm%0.2lf-sp%0.2lf-w%0.1lf-Bext%0.2lf-sext%0.2lf.dat",
          B0, sm, sp, w, B0ext, sext);

  const Int_t noOfPoints = 1000;

  const Double_t res = 10.0*(sp+sm)/(Double_t)noOfPoints;

  fp = fopen(fln, "w");

  fprintf(fp, "# title: skewed Gaussian + background\n");
  fprintf(fp, "# parameter: B0 = %lf, sm = %lf, sp = %lf, w = %lf\n", B0, sm, sp, w);
  fprintf(fp, "#            B0ext = %lf, sext = %lf, \n", B0ext, sext);
  fprintf(fp, "# -----------------------------------------------------\n");
  fprintf(fp, "# B, pB\n");
  Double_t b, pb;
  for (Int_t i=0; i<noOfPoints; i++) {
    b  = B0-10.0*sm + res * i;
    pb = w*PofB(b, B0, sm, sp) + (1.0-w)*PofBext(b, B0ext, sext);
    fprintf(fp, "%lf, %lf\n", b, pb);
  }
  fprintf(fp, "# end of file\n");
  fclose(fp);
}
