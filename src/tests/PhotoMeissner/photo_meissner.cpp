/***************************************************************************

  photo_meissner.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2012 by Andreas Suter                                   *
 *   andreas.suter@psi.ch                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cstring>

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_bessel.h>

#include <iostream>
using namespace std;

void photo_meissner_syntax()
{
  cout << endl << "usage: (1) photo_meissner -b <n> <x>";
  cout << endl << "           -b : switch used to tell that the modified Bessel function of 1st kind is wanted";
  cout << endl << "           <n>: subscript value of I_n(x)";
  cout << endl << "           <x>: argument value of I_n(x)";
  cout << endl << "           returns: I_n(x)";
  cout << endl << "       (2) photo_meissner -b <n> <start_x> <end_x> <step_x>";
  cout << endl << "           returns: I_n(x) vector, starting x=start_x, ending at x=end_x with a step with of step_x";
  cout << endl << "       (3) photo_meissner -m <lam> <lamP> <x0> [<D>]";
  cout << endl << "           -m  : switch used to tell that the Meissner profile is wanted.";
  cout << endl << "           <lam>  : lambda in the absence of light";
  cout << endl << "           <lamP> : lambda induced";
  cout << endl << "           <x0>   : extend of lambda induced";
  cout << endl << "           <D>    : thickness of the film. If <D> is given, it needs to be > 0.0.";
  cout << endl << endl;
}

double In(double n, double x)
{
  double dval(0.0);

  if (n>=0.0) {
    dval = gsl_sf_bessel_Inu(n, x);
  } else {
    n = -n;
    dval = gsl_sf_bessel_Inu(n, x) + M_2_PI*sin(n*M_PI)*gsl_sf_bessel_Knu(n, x);
  }

  return dval;
}

int main(int argc, char *argv[])
{
  double dval(0.0);
  double n(0.0);
  double x(0.0);
  double start_x(0.0);
  double end_x(0.0);
  double step_x(1.0);
  double lam0(0.0);
  double lam1(0.0);
  double x0(0.0);
  double D(0.0);
  double np(0.0);
  double beta(0.0);
  double NN(0.0);

  switch (argc) {
    case 4:
      if (strcmp(argv[1], "-b")) {
        photo_meissner_syntax();
        return 1;
      }
      n = atof(argv[2]);
      x = atof(argv[3]);
      cout << endl << "> n=" << n << ", x=" << x << ", I_n(x)=" << In(n, x) << endl << endl;
      break;
    case 5:
      if (!strcmp(argv[1],"-m")) {
        lam0 = atof(argv[2]);
        lam1 = atof(argv[3]);
        x0 = atof(argv[4]);
        if (lam0 <= 0.0) {
          cerr << endl << "**ERROR** lam0 needs to be > 0.0" << endl << endl;
          return 1;
        }
        if (x0 <= 0.0) {
          cerr << endl << "**ERROR** x0 needs to be > 0.0" << endl << endl;
          return 1;
        }
        if (lam1 <= 0.0) {
          cerr << endl << "**ERROR** lam1 needs to be > 0.0" << endl << endl;
          return 1;
        }
        n = 2.0*x0/lam0;
        np = 2.0*x0/lam1;
        step_x = x0/100.0;
        start_x = 0.0;
        end_x = 5.0*lam0;
        x = start_x;
        cout << "# x exp(-x/lam0) B(x) semi-infinite" << endl;
        do {
          dval = In(n, np*sqrt(exp(-x/x0)))/In(n, np);
          cout << x << " " << exp(-x/lam0) << " " << dval << endl;
          x += step_x;
        } while (x <= end_x+step_x);
      } else {
        photo_meissner_syntax();
        return 1;
      }
      break;
    case 6:
      if (!strcmp(argv[1],"-b")) {
        n = atof(argv[2]);
        start_x = atof(argv[3]);
        end_x = atof(argv[4]);
        step_x = atof(argv[5]);
        x = start_x;
        cout << "# x In(x)" << endl;
        do {
          cout << x  << " " << In(n, x) << endl;
          x += step_x;
        } while (x <= end_x+step_x);
      } else if (!strcmp(argv[1], "-m")) {
        lam0 = atof(argv[2]);
        lam1 = atof(argv[3]);
        x0 = atof(argv[4]);
        D = atof(argv[5]);
        if (lam0 <= 0.0) {
          cerr << endl << "**ERROR** lam0 needs to be > 0.0" << endl << endl;
          return 1;
        }
        if (x0 <= 0.0) {
          cerr << endl << "**ERROR** x0 needs to be > 0.0" << endl << endl;
          return 1;
        }
        if (lam1 <= 0.0) {
          cerr << endl << "**ERROR** lam1 needs to be > 0.0" << endl << endl;
          return 1;
        }
        if (D <= 0.0) {
          cerr << endl << "**ERROR** D needs to be > 0.0" << endl << endl;
          return 1;
        }
        n = 2.0*x0/lam0;
        np = 2.0*x0/lam1;
        beta = exp(-D/(2*x0));
        NN = In(-n, np*beta)*In(n, np) - In(-n, np)*In(n,np*beta);
        step_x = x0/100.0;
        start_x = 0.0+1.0e-10;
        end_x = D;
        x = start_x;
        cout << "# lam0=" << lam0 << ", lam1=" << lam1 << ", x0=" << x0 << ", D=" << D << endl;
        cout << "# n=" << n << ", np=" << np << ", beta=" << beta << endl;
        cout << "# x cosh([D/2-x]/lam0)/cosh(D/[2*lam0]) B(x) film of thickness D=" << D << endl;
        do {
          dval = In(-n, np*exp(-x/(2.0*x0)))*(In(n, np) - In(n, np*beta)) - In(n, np*exp(-x/(2.0*x0)))*(In(-n, np) - In(-n, np*beta));
          dval /= NN;
          cout << x << " " << cosh((D/2-x)/lam0)/cosh(D/(2.0*lam0)) << " " << dval << endl;
          x += step_x;
        } while (x <= end_x+step_x);
      } else {
        photo_meissner_syntax();
        return 1;
      }
      break;
    default:
      photo_meissner_syntax();
      return 1;
  }

  return 0;
}
