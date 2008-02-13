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

  return val;
}

//-------------------------------------------------------------------------
void skewedGaussian()
{
  FILE *fp;
  char fln[256];

  strcpy(fln, "skewedGauss.dat");

  const Double_t B0 = 100.0; // (G)
  const Double_t sm = 2.0;   // (G)
  const Double_t sp = 10.0;  // (G)

  const Int_t noOfPoints = 1000;

  const Double_t res = 10.0*(sp+sm)/(Double_t)noOfPoints;

  fp = fopen(fln, "w");

  fprintf(fp, "# title: skewed Gaussian\n");
  fprintf(fp, "# parameter: B0 = %lf, sm = %lf, sp = %lf\n", B0, sm, sp);
  fprintf(fp, "# -----------------------------------------------------\n");
  fprintf(fp, "# B, pB\n");
  Double_t b, pb;
  for (Int_t i=0; i<noOfPoints; i++) {
    b  = B0-10.0*sm + res * i;
    pb = PofB(b, B0, sm, sp);
    fprintf(fp, "%lf, %lf\n", b, pb);
  }
  fprintf(fp, "# end of file\n");
  fclose(fp);
}
