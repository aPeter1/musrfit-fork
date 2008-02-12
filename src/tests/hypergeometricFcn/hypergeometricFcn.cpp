#include <iostream>
#include <fstream>
using namespace std;

#include <cmath>

#include <gsl_sf_hyperg.h>

#define PI 3.14159265359

extern "C" {
  double gsl_sf_hyperg_1F1(double a, double b, double x);
}

int main(int argc, char *argv[])
{
  // load Mathematica test file
  ifstream fin;

  fin.open("erfi.dat", iostream::in);
  if (!fin.is_open()) {
    return -1;
  }

  int max=0;
  char str[256];
  double t, fval;
  double data[3][1000];
  while (!fin.eof()) {
    fin.getline(str, sizeof(str));
    sscanf(str, "%lf %lf", &t, &fval);
    data[0][max] = t;
    data[1][max] = fval;
    max++;
  }

  fin.close();

  // evaluate gsl function
  for (int i=0; i<max; i++) {
    t = i*0.01;
    data[2][i] = 2*t/sqrt(PI)*gsl_sf_hyperg_1F1(1.0/2.0,3.0/2.0,t*t);
  }

  // save everything together in an output file
  ofstream fout;

  fout.open("erfi_gsl.dat", iostream::out);
  if (!fout.is_open()) {
    return -1;
  }

  for (int i=0; i<max; i++) {
    fout << data[0][i] << ", " << data[1][i] << ", " << data[2][i] << endl;
  }

  fout.close();

  return 0;
}
