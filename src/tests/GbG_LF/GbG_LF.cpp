#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

#include <gsl/gsl_integration.h>

#define GAMMA_BAR_MUON  1.35538817e-2
#define TWO_PI 6.28318530717958647692528676656

typedef struct {
  double wExt;
  double s0;
  double s1;
} fun_params;

void syntax()
{
  cout << endl;
  cout << "usage: GbG_LF Bext sigma0 sigmaGbG tmax fln" << endl;
  cout << "       Bext in (G)" << endl;
  cout << "       sigma0 in (1/us)" << endl;
  cout << "       sigmaGbG in (1/us)" << endl;
  cout << "       tmax in (us)" << endl;
  cout << "       fln : output file name" << endl;
  cout << endl << endl;
}

double pz_GbG_2(double x, void *param)
{
  fun_params *par = (fun_params*) param;
  double s02 = par->s0 * par->s0;
  double s12 = par->s1 * par->s1;
  double x2 = x*x;
  double aa = 1.0+x2*s12;

  return 2.0*(s02*s02+3.0*s12*s12*aa*aa+6.0*s02*s12*aa)/(pow(par->wExt, 3.0)*pow(aa, 4.5))*exp(-0.5*s02*x2/aa);
}

int main(int argc, char *argv[])
{
  if (argc != 6) {
    syntax();
    return -1;
  }

  double Bext = atof(argv[1]);
  double sigma0 = atof(argv[2]);
  double sigma1 = atof(argv[3]);
  double tmax = atof(argv[4]);

  // calculate needed time step size
  double dt = 1.0 / (GAMMA_BAR_MUON*Bext) / 10.0;
  if (10.0/dt < 500.0)
    dt = 0.02;
  cout << "dt = " << dt << " (us)" << endl;

  double wExt = TWO_PI * GAMMA_BAR_MUON * Bext;
  double s0   = sigma0;
  double s1   = sigma1;

  double wExt2 = wExt*wExt;
  double s02   = s0*s0;
  double s12   = s1*s1;

  gsl_integration_workspace *w = gsl_integration_workspace_alloc(65536);
  gsl_function fun;
  fun.function = &pz_GbG_2;
  fun_params param = { wExt, s0, s1 };
  fun.params = &param;

  gsl_integration_qawo_table *tab = gsl_integration_qawo_table_alloc(wExt, tmax, GSL_INTEG_SINE, 16);

  double result = 0.0, err = 0.0;
  double t = 0.0, t2 = 0.0, dval=0.0, aa=0.0;
  vector<double> time;
  vector<double> pol, pol1, pol2;
  do {
    time.push_back(t);
    t2 = t*t;

    dval = 0.0;

    // P_z^LF (GbG, 1st part)
    aa = 1.0+t2*s12;
    dval = 1.0 - 2.0*(s02+s12)/wExt2 + 2.0*(s02+s12*aa)/(wExt2*pow(aa,2.5))*cos(wExt*t)*exp(-0.5*s02*t2/aa);
    pol1.push_back(dval);

    // P_z^LF (GbG, 2nd part)
    gsl_integration_qawo_table_set_length(tab, t);
    gsl_integration_qawo(&fun, 0.0, 1.0e-9, 1.0e-9, 1024, w, tab, &result, &err);
    pol2.push_back(result);
    dval += result;

    pol.push_back(dval);

    t += dt;
  } while (t <= tmax);

  gsl_integration_workspace_free(w);
  gsl_integration_qawo_table_free(tab);

  ofstream fout(argv[5], ofstream::out);
  fout << "% Bext (G)        : " << Bext << endl;
  fout << "% sigma0 (1/us)   : " << sigma0 << endl;
  fout << "% sigma1 (1/us)   : " << sigma1 << endl;
  fout << "%----" << endl;
  fout << "% time (us), pol1, pol2, pol" << endl;
  for (unsigned int i=0; i<time.size(); i++) {
    fout << time[i] << ", " << pol1[i] << ", " << pol2[i] << ", " << pol[i] << endl;
  }

  fout.close();

  return 0;
}
