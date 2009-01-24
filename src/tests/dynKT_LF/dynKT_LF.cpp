#include <sys/time.h>
#include <unistd.h>

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <fstream>
using namespace std;

#include <gsl/gsl_sf_bessel.h>

typedef vector<double> PDoubleVector;

#define PI 3.14159265359

// NR p.797
void voltra(const double h, PDoubleVector &t, PDoubleVector &param, PDoubleVector &f,
            double g(const int, const PDoubleVector &, const PDoubleVector &, const PDoubleVector &), PDoubleVector &gi)
{
  int i,j;
  double sum;

  int n = f.size();
  double a;
  t[0] = 0.0;
  f[0]=g(0,t,param,gi);

  for (i=1; i<n; i++) {
    sum = g(i,t,param,gi);
    sum += 0.5*h*param[2]*g(i,t,param,gi)*f[0];
    for (j=1; j<i; j++) {
      sum += h*param[2]*g(i-j,t,param,gi)*f[j];
    }
    a = 1.0-0.5*h*param[2]*g(0,t,param,gi);

    f[i]=sum/a;
  }
}

double g_gauss(const int i, const PDoubleVector &tvec, const PDoubleVector &param, const PDoubleVector &gi)
{
  // param: 0=w0, 1=sigma, 2=nu
  double result;
  double t = tvec[i];
  double Dt2 = pow(param[1]*t, 2.0)/2.0;

  if (param[0] == 0.0) {
    result = 0.333333333333333333333333 + 0.66666666666666666666 * (1.0-2.0*Dt2) * exp(-Dt2);
  } else {
    result = 1.0 - 2.0*param[1]*param[1]/(param[0]*param[0])*(1.0-exp(-Dt2)*cos(param[0]*t)) +
             2.0*pow(param[1],4.0)/pow(param[0],3.0)*gi[i];
  }

  result *= exp(-param[2]*t);

  return result;
}

double g_lorentz(const int i, const PDoubleVector &tvec, const PDoubleVector &param, const PDoubleVector &gi)
{
  // param: 0=w0, 1=lambda, 2=nu
  double result;
  double t = tvec[i];
  double at = param[1]*t;

  if (param[0] == 0.0) {
    result = 0.333333333333333333333333 + 0.66666666666666666666 * (1.0-at) * exp(-at);
  } else {
    double awL = param[1]/param[0];
    double wLt = param[0]*t;
    double expat = exp(-param[1]*t);
    result = 1.0 - awL*gsl_sf_bessel_j1(wLt)*expat - awL*awL*(gsl_sf_bessel_j0(wLt)*expat-1.0) -
             (1.0+awL*awL)*param[1]*gi[i];
  }

  result *= exp(-param[2]*t);

  return result;
}

void calc_gi_gauss(const double h, PDoubleVector &t, PDoubleVector &param, PDoubleVector &f)
{
  // if w0=0 nothing to be done
  if (param[0] == 0.0)
    return;

  double dtHalf = h/2.0;
  PDoubleVector hh(t.size());

  hh[0] = 0.0;
  f[0] = 0.0;
  for (unsigned int i=1; i<t.size(); i++) {
    hh[i] = exp(-0.5*pow(param[1]*t[i],2.0))*sin(param[0]*t[i]);
    f[i] = f[i-1] + dtHalf*(hh[i]+hh[i-1]);
  }
}

void calc_gi_lorentz(const double h, PDoubleVector &t, PDoubleVector &param, PDoubleVector &f)
{
  // if w0=0 nothing to be done
  if (param[0] == 0.0)
    return;

  double dtHalf = h/2.0;
  PDoubleVector hh(t.size());

  hh[0] = 0.0;
  f[0] = 0.0;
  for (unsigned int i=1; i<t.size(); i++) {
    hh[i] = gsl_sf_bessel_j0(param[0]*t[i]) * exp(-param[1]*t[i]);
    f[i] = f[i-1] + dtHalf*(hh[i]+hh[i-1]);
  }
}

int main(int argc, char *argv[])
{

  bool useKeren = false;

  if (argc < 4) {
    cout << endl << "usage: dynGaussKT_LF w0 width nu [G|L N]";
    cout << endl << "          w0:    external field in Mc/s";
    cout << endl << "          width: static field width in Mc/s";
    cout << endl << "          nu:    hopping rate in Mc/s";
    cout << endl << "          G/L:   G=Gaussian field distribution; L=Lorentzain field distribution";
    cout << endl << "          if G/L not given, G is set as default";
    cout << endl << "          N:     number of sampling points";
    cout << endl << "          if N is not given, calc N internally";
    cout << endl << endl;
    return 0;
  }

  PDoubleVector param(3);
  const double Tmax = 15.0;
  unsigned int N;
  bool gaussian = true;

  // feed parameter vector
  param[0] = atof(argv[1]); // w0
  param[1] = atof(argv[2]); // width
  param[2] = atof(argv[3]); // nu
  if (argc == 6) {
    N = atoi(argv[5]); // N
  } else {
    // w0 criteria, i.e. w0 T = 2 pi, ts = T/16, N = Tmax/ts, if N < 300, N == 300
    double val = 8.0/PI*Tmax*param[0];
    if (val < 250)
      N = 250;
    else
      N = static_cast<unsigned int>(val);

    // nu/Delta criteria
    if (param[1] != 0.0) { // Delta != 0
      val = param[2]/param[1]; // nu/Delta
      if (val > 5.0) {
        useKeren = true;
        N = 3000;
      }
    }
  }

  if (argc > 4) {
    if (*argv[4] == 'L') {
      gaussian = false;
    }
  }

  char fln[128];
  if (gaussian)
    sprintf(fln, "dynKT_LF_w0_%1.1f_width%1.1f_nu%1.1f_N%d_G.dat", param[0], param[1], param[2], N);
  else 
    sprintf(fln, "dynKT_LF_w0_%1.1f_width%1.1f_nu%1.1f_N%d_L.dat", param[0], param[1], param[2], N);

  const double H = Tmax/N;

  PDoubleVector t(N);
  PDoubleVector f(N);
  PDoubleVector gi(N);
  PDoubleVector keren(N);

  // get start time
  struct timeval tv_start, tv_stop;
  double t1, t2;
  gettimeofday(&tv_start, 0);

  // feed time vector
  for (unsigned int i=0; i<N; i++) {
    t[i] = H*i;
  }

  if (gaussian) {
    calc_gi_gauss(H, t, param, gi);
  } else {
    calc_gi_lorentz(H, t, param, gi);
  }
  gettimeofday(&tv_stop, 0);
  t1 = (tv_stop.tv_sec - tv_start.tv_sec)*1000.0 + (tv_stop.tv_usec - tv_start.tv_usec)/1000.0;

  gettimeofday(&tv_start, 0);
  if (gaussian) {
    voltra(H, t, param, f, g_gauss, gi);
  } else {
    voltra(H, t, param, f, g_lorentz, gi);
  }

  // get stop time
  gettimeofday(&tv_stop, 0);
  t2 = (tv_stop.tv_sec - tv_start.tv_sec)*1000.0 + (tv_stop.tv_usec - tv_start.tv_usec)/1000.0;

  // calculate keren LF
  double w02, nu2, width2, Gamma_t;
  for (unsigned int i=0; i<t.size(); i++) {
    w02 = pow(param[0], 2.0);
    width2 = pow(param[1], 2.0);
    nu2 = pow(param[2], 2.0);
    Gamma_t = 2.0*width2/pow(w02+nu2,2.0)*((w02+nu2)*param[2]*t[i]+(w02-nu2)*(1.0-exp(-param[2]*t[i])*cos(param[0]*t[i]))-2.0*param[2]*param[0]*exp(-param[2]*t[i])*sin(param[0]*t[i]));
    if (gaussian)
      keren[i] = exp(-Gamma_t);
    else
      keren[i] = exp(-sqrt(2.0*Gamma_t));
  }

  ofstream fout;

  // open mlog-file
  fout.open(fln, iostream::out);
  if (!fout.is_open()) {
    return -1;
  }

  if (gaussian) {
    if (useKeren)
      fout << "# use Keren = true" << endl;
    else
      fout << "# use Keren = false" << endl;
  }

  fout << "# N = " << N << endl;
  if (gaussian) {
    fout << "# Gaussian field distribution" << endl;
    fout << "# w0 = " << param[0] << ", sigma = " << param[1] << ", nu = " << param[2] << endl;
  } else {
    fout << "# Lorentzian field distribution" << endl;
    fout << "# w0 = " << param[0] << ", lambda = " << param[1] << ", nu = " << param[2] << endl;
  }
  fout << "# calculation time: t1 = " << t1 << " (ms), t2 = " << t2 << " (ms)" << endl;

  fout <<  "#       time" <<  setw(13) << "Pz_dyn_LF" << setw(13) << "g" << setw(13) << "gi" << setw(13) << "keren" << endl;
  fout << fixed << setprecision(6);
  for (unsigned int nn=0; nn<N; nn++) {
    fout << setw(12) << t[nn] << setw(13) << f[nn] << setw(13) << g_gauss(nn,t,param,gi) << setw(13) << gi[nn] << setw(13) << keren[nn];
    fout << endl;
  }

  fout.close();

  return 0;
}
