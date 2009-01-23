#include <sys/time.h>
#include <unistd.h>

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
using namespace std;

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

double g(const int i, const PDoubleVector &tvec, const PDoubleVector &param, const PDoubleVector &gi)
{
  // param: 0=w0, 1=Delta, 2=nu
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

void calc_gi(const double h, PDoubleVector &t, PDoubleVector &param, PDoubleVector &f)
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

int main(int argc, char *argv[])
{

  bool useKeren = false;

  if (argc != 5) {
    cout << endl << "usage: dynGaussKT_LF w0 Delta nu [N]";
    cout << endl << "          w0:    external field in Mc/s";
    cout << endl << "          Delta: static field width in Mc/s";
    cout << endl << "          nu:    hopping rate in Mc/s";
    cout << endl << "          N:     number of sampling points";
    cout << endl << "          if N == -1 -> calc N internally";
    cout << endl << "          if N is not given, N=1000";
    cout << endl << endl;
    return 0;
  }

  PDoubleVector param(4);
  const double Tmax = 15.0;

  // feed parameter vector
  param[0] = atof(argv[1]); // w0
  param[1] = atof(argv[2]); // Delta
  param[2] = atof(argv[3]); // nu
  if (argc == 5) {
    param[3] = atof(argv[4]); // N
    if (param[3] == -1.0) { // estimate N by itself
      // w0 criteria, i.e. w0 T = 2 pi, ts = T/16, N = Tmax/ts, if N < 300, N == 300
      double val = 8.0/PI*Tmax*param[0];
      if (val < 250)
        param[3] = 250;
      else
        param[3] = val;

      // nu/Delta criteria
      if (param[1] != 0.0) { // Delta != 0
        val = param[2]/param[1]; // nu/Delta
        if (val > 5.0) {
          useKeren = true;
          param[3] = 1000;
        }
      }
    }
  } else {
    param[3] = 1000;
  }

  const unsigned int N=static_cast<unsigned int>(param[3]);
  const double H = Tmax/N;

  PDoubleVector t(N);
  PDoubleVector f(N);
  PDoubleVector gi(N);
  PDoubleVector abragam(N);
  PDoubleVector keren(N);

  // get start time
  struct timeval tv_start, tv_stop;
  double t1, t2;
  gettimeofday(&tv_start, 0);

  // feed time vector
  for (unsigned int i=0; i<N; i++) {
    t[i] = H*i;
  }

  calc_gi(H, t, param, gi);
  gettimeofday(&tv_stop, 0);
  t1 = (tv_stop.tv_sec - tv_start.tv_sec)*1000.0 + (tv_stop.tv_usec - tv_start.tv_usec)/1000.0;
  gettimeofday(&tv_start, 0);
  voltra(H, t, param, f, g, gi);

  // get stop time
  gettimeofday(&tv_stop, 0);
  t2 = (tv_stop.tv_sec - tv_start.tv_sec)*1000.0 + (tv_stop.tv_usec - tv_start.tv_usec)/1000.0;

  bool abragam_present = false;
  if (param[1] != 0) {
    if (param[2]/param[1] >= 1.0) {
      for (unsigned int i=0; i<t.size(); i++)
        abragam[i]  = exp(-2.0*pow(param[1]/param[2],2.0)*(exp(-param[2]*t[i])-1.0+param[2]*t[i]));
      abragam_present = true;
    }
  }

  // calculate keren LF
  double w02, nu2, Delta2, Gamma_t;
  for (unsigned int i=0; i<t.size(); i++) {
    w02 = pow(param[0], 2.0);
    Delta2 = pow(param[1], 2.0);
    nu2 = pow(param[2], 2.0);
    Gamma_t = 2.0*Delta2/pow(w02+nu2,2.0)*((w02+nu2)*param[2]*t[i]+(w02-nu2)*(1.0-exp(-param[2]*t[i])*cos(param[0]*t[i]))-2.0*param[2]*param[0]*exp(-param[2]*t[i])*sin(param[0]*t[i]));
    keren[i] = exp(-Gamma_t);
  }

  if (useKeren)
    cout << "# use Keren = true" << endl;
  else
    cout << "# use Keren = false" << endl;

  cout << "# N = " << param[3] << endl;
  cout << "# calculation time: t1 = " << t1 << " (ms), t2 = " << t2 << " (ms)" << endl;

  if (abragam_present)
    cout <<  setw(12) << "# time" <<  setw(13) << "Pz_dyn_LF" << setw(13) << "g" << setw(13) << "gi" << setw(13) << "agragam" << setw(13) << "keren" << endl;
  else
    cout <<  setw(12) << "# time" <<  setw(13) << "Pz_dyn_LF" << setw(13) << "g" << setw(13) << "gi" << setw(13) << "keren" << endl;
  cout << fixed << setprecision(6);
  for (unsigned int nn=0; nn<N; nn++) {
    if (abragam_present)
      cout << setw(12) << t[nn] << setw(13) << f[nn] << setw(13) << g(nn,t,param,gi) << setw(13) << gi[nn] << setw(13) << abragam[nn] << setw(13) << keren[nn];
    else
      cout << setw(12) << t[nn] << setw(13) << f[nn] << setw(13) << g(nn,t,param,gi) << setw(13) << gi[nn] << setw(13) << keren[nn];
    cout << endl;
   }

  return 0;
}
