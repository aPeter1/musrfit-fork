/***************************************************************************

  photo_meissner.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013 by Andreas Suter                                   *
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

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cassert>
using namespace std;

#include <gsl_sf_bessel.h>

//--------------------------------------------------------------------------
void photo_meissner_syntax()
{
  cout << endl << "usage: photo_meissner lamL lamP z0 deadLayer [filmThickness]";
  cout << endl << "                      {--at <zPos> | --range <zStart> <zEnd>]} [--dump <fln>] | ";
  cout << endl << "                      --version | [--help]" << endl;
  cout << endl << "       lamL          : London penetration depth in (nm)";
  cout << endl << "       lamP          : Photo induced penetration depth (nm)";
  cout << endl << "       z0            : length scale of the photo induced effect (nm)";
  cout << endl << "       deadLayer     : dead layer (nm)";
  cout << endl << "       filmThickness : film thickness in (nm), if absent a";
  cout << endl << "                       semi-infinite sample is assumed. filmThickness = 2 t";
  cout << endl << "       --at <zPos>   : calculates the field at the position <zPos> (nm).";
  cout << endl << "       --range <zStart> <zEnd> : calculate the field in the range from";
  cout << endl << "                       <zStart> to <zEnd> in (nm).";
  cout << endl << "           either --at or --range needs to be present.";
  cout << endl << "       --dump <fln>  : dump the result into the file <fln>.";
  cout << endl << endl;
}

//--------------------------------------------------------------------------
double InuMinus(double nu, double x)
{
  return gsl_sf_bessel_Inu(nu,x) + M_2_PI * sin(nu*M_PI) * gsl_sf_bessel_Knu(nu,x);
}

//--------------------------------------------------------------------------
/**
 *
 */
int main(int argc, char *argv[])
{
  double lamL = -1.0;
  double lamP = -1.0;
  double z0 = -1.0;
  double deadLayer = -1.0;
  double filmThickness = -1.0;
  double zStart = -1.0;
  double zEnd = -1.0;
  char fln[1024];

  strcpy(fln, "");

  if (argc <= 1) {
    photo_meissner_syntax();
    return 0;
  }

  if (!strcmp(argv[1], "--help")) {
    photo_meissner_syntax();
    return 0;
  }

  if (!strcmp(argv[1], "--version")) {
    cout << endl << "photo_meissner : $Id$" << endl << endl;
    return 0;
  }

  int status=0;
  for (int i=1; i<argc; i++) {
    if (i==1) {
      status = sscanf(argv[i], "%lf", &lamL);
      if (status != 1) {
        cout << endl << "**ERROR** lamL: argv[" << i << "]=" << argv[i] << " is not a double." << endl;
        photo_meissner_syntax();
        return 1;
      }
    } else if (i==2) {
      status = sscanf(argv[i], "%lf", &lamP);
      if (status != 1) {
        cout << endl << "**ERROR** lamP: argv[" << i << "]=" << argv[i] << " is not a double." << endl;
        photo_meissner_syntax();
        return 1;
      }
    } else if (i==3) {
      status = sscanf(argv[i], "%lf", &z0);
      if (status != 1) {
        cout << endl << "**ERROR** z0: argv[" << i << "]=" << argv[i] << " is not a double." << endl;
        photo_meissner_syntax();
        return 1;
      }
    } else if (i==4) {
      status = sscanf(argv[i], "%lf", &deadLayer);
      if (status != 1) {
        cout << endl << "**ERROR** deadLayer: argv[" << i << "]=" << argv[i] << " is not a double." << endl;
        photo_meissner_syntax();
        return 1;
      }
    } else if (i==5) { // either filmThickness or --at or --range
      status = sscanf(argv[i], "%lf", &filmThickness);
      if (status != 1) {
        filmThickness = -1.0;
      }
    }

    if (!strcmp(argv[i], "--at")) {
      if (argc > i+1) {
        status = sscanf(argv[i+1], "%lf", &zStart);
        if (status != 1) {
          cout << endl << "**ERROR** --at <zPos>=" << argv[i+1] << " is not a double." << endl;
          photo_meissner_syntax();
          return 1;
        }
      } else {
        cout << endl << "**ERROR** --at without <zPos> found." << endl;
        photo_meissner_syntax();
        return 1;
      }
      i += 1;
    }

    if (!strcmp(argv[i], "--range")) {
      if (argc > i+2) {
        status = sscanf(argv[i+1], "%lf", &zStart);
        if (status != 1) {
          cout << endl << "**ERROR** --range <zStart> <zEnd>: <zStart>=" << argv[i+1] << " is not a double." << endl;
          photo_meissner_syntax();
          return 1;
        }
        status = sscanf(argv[i+2], "%lf", &zEnd);
        if (status != 1) {
          cout << endl << "**ERROR** --range <zStart> <zEnd>: <zEnd>=" << argv[i+2] << " is not a double." << endl;
          photo_meissner_syntax();
          return 1;
        }
      } else {
        cout << endl << "**ERROR** --range without <zStart> and/or <zEnd> found." << endl;
        photo_meissner_syntax();
        return 1;
      }
      i += 2;
    }

    if (!strcmp(argv[i], "--dump")) {
      if (argc > i+1) {
        strcpy(fln, argv[i+1]);
      } else {
        cout << endl << "**ERROR** --dumpe without <fln> found." << endl;
        photo_meissner_syntax();
        return 1;
      }
      i += 1;
    }
  }


  if ((lamL == -1.0) || (lamP == -1.0) || (z0 == -1.0) || (deadLayer == -1.0) || (zStart == -1.0)) {
    cout << endl << "**ERROR** not all needed parameters found. All the parameters need to be positive." << endl;
    photo_meissner_syntax();
    return 1;
  }

  if (lamL == 0.0) {
    cout << endl << "**ERROR** lamL > 0.0 needed!" << endl << endl;
    return 1;
  }
  if (lamP == 0.0) {
    cout << endl << "**ERROR** lamP > 0.0 needed!" << endl << endl;
    return 1;
  }
  if (z0 == 0.0) {
    cout << endl << "**ERROR** z0 > 0.0 needed!" << endl << endl;
    return 1;
  }
  if (filmThickness == 0.0) {
    cout << endl << "**ERROR** filmThickness > 0.0 needed!" << endl << endl;
    return 1;
  }

  // estimate dz
  double dz = 0.0;
  if (lamL < lamP)
    dz = lamL / 500.0;
  else
    dz = lamP / 500.0;

  vector<double> zz;
  vector<double> BB;
  vector<double> BBL;
  double b, b1, bL; // b = B/Bext, bL = exp(-z/lamL)
  if (filmThickness == -1.0) { // semi-infinite
    double nuPlus  = 2.0*z0/lamL;
    double nuPhoto = 2.0*z0/lamP;
    if (zEnd == -1.0) { // only a single point
      if (zStart < deadLayer) {
        b = 1.0;
        bL = 1.0;
      } else {
        b1 = gsl_sf_bessel_Inu(nuPlus, nuPhoto);
        assert(b1>0.0);
        b = gsl_sf_bessel_Inu(nuPlus, nuPhoto * sqrt(exp(-(zStart-deadLayer)/z0)))/b1;
        bL = exp(-(zStart-deadLayer)/lamL);
      }

      if (!strcmp(fln, "")) // only to stdout if no dump present
        cout << endl << "result: zPos=" << zStart << ", B/Bext=" << b << ", exp(-[zPos-deadLayer]/lamL)=" << bL << endl << endl;
      // keep z, b
      zz.push_back(zStart);
      BB.push_back(b);
      BBL.push_back(bL);
    } else { // a range
      double z = zStart;
      b1 = gsl_sf_bessel_Inu(nuPlus, nuPhoto);
      assert(b1>0.0);
      do {
        if (z < deadLayer) {
          b = 1.0;
          bL = 1.0;
        } else {
          b = gsl_sf_bessel_Inu(nuPlus, nuPhoto * sqrt(exp(-(z-deadLayer)/z0)))/b1;
          bL = exp(-(z-deadLayer)/lamL);
        }
        if (!strcmp(fln, "")) // only to stdout if no dump present
          cout << z << ", " << b << ", " << bL << ", " << b-bL << endl;
        zz.push_back(z);
        BB.push_back(b);
        BBL.push_back(bL);
        z += dz;
      } while (z <= zEnd);
    }
  } else { // film
    double nuPlus = 2.0*z0/lamL;
    double nuPhoto = 2.0*z0/lamP;
    double beta = exp(-(filmThickness/2.0-deadLayer)/z0);
    double NN = InuMinus(nuPlus, nuPhoto*beta)*gsl_sf_bessel_Inu(nuPlus, nuPhoto) -
                InuMinus(nuPlus, nuPhoto)*gsl_sf_bessel_Inu(nuPlus, nuPhoto*beta);
    assert(NN>0);
    if (zEnd == -1.0) { // only a single point
      if ((zStart < deadLayer) || (zStart > filmThickness-deadLayer)) {
        b = 1.0;
        bL = 1.0;
      } else {
        b = (InuMinus(nuPlus, nuPhoto*exp(-(zStart-deadLayer)/(2.0*z0)))*(gsl_sf_bessel_Inu(nuPlus, nuPhoto)-gsl_sf_bessel_Inu(nuPlus, nuPhoto*beta))-
             gsl_sf_bessel_Inu(nuPlus, nuPhoto*exp(-(zStart-deadLayer)/(2.0*z0)))*(InuMinus(nuPlus, nuPhoto)-InuMinus(nuPlus, nuPhoto*beta)))/NN;
        bL = cosh((filmThickness/2.0-zStart)/lamL)/cosh((filmThickness/2.0-deadLayer)/lamL);
      }

      if (!strcmp(fln, "")) // only to stdout if no dump present
        cout << endl << "result: zPos=" << zStart << ", B/Bext=" << b << ", cosh[((t-deadLayer)-z)/lamL]/cosh[(t-deadLayer)/lamL]=:f(z)=" << bL << ", B/Bext-f(z)=" << b-bL << endl << endl;
      // keep z, b
      zz.push_back(zStart);
      BB.push_back(b);
      BBL.push_back(bL);
    } else { // a range
      double z = zStart;
      do {
        if ((z < deadLayer) || (z > filmThickness-deadLayer)) {
          b = 1.0;
          bL = 1.0;
        } else {
          b = (InuMinus(nuPlus, nuPhoto*exp(-(z-deadLayer)/(2.0*z0)))*(gsl_sf_bessel_Inu(nuPlus, nuPhoto)-gsl_sf_bessel_Inu(nuPlus, nuPhoto*beta))-
               gsl_sf_bessel_Inu(nuPlus, nuPhoto*exp(-(z-deadLayer)/(2.0*z0)))*(InuMinus(nuPlus, nuPhoto)-InuMinus(nuPlus, nuPhoto*beta)))/NN;
          bL = cosh((filmThickness/2.0-z)/lamL)/cosh((filmThickness/2.0-deadLayer)/lamL);
        }
        if (!strcmp(fln, "")) // only to stdout if no dump present
          cout << z << ", " << b << ", " << bL << ", " << b-bL << endl;
        zz.push_back(z);
        BB.push_back(b);
        BBL.push_back(bL);
        z += dz;
      } while (z <= zEnd);
    }
  }

  // check if a file needs to be written
  if (strcmp(fln, "")) {
    ofstream fout(fln);
    // write header
    if (filmThickness == -1.0)
      fout << "% semi-infinite" << endl;
    else
      fout << "% film with thickness: " << filmThickness << " (nm)" << endl;
    fout << "% Parameters:" << endl;
    fout << "% lamL      = " << lamL << " (nm)" << endl;
    fout << "% lamP      = " << lamP << " (nm)" << endl;
    fout << "% z0        = " << z0 << " (nm)" << endl;
    fout << "% deadLayer = " << deadLayer << " (nm)" << endl;
    if (zEnd == -1.0) {
      fout << "% --at <zPos>=" << zStart << " (nm) used." << endl;
    } else {
      fout << "% --range <zStart>=" << zStart << ", <zEnd>=" << zEnd << " in (nm) used." << endl;
    }
    if (filmThickness == -1.0)
      fout << "% z B/Bext exp(-(z-deadLayer)/lamL) B/Bext-exp(-(z-deadLayer)/lamL)" << endl;
    else
      fout << "% z B/Bext cosh[((t-2*deadLayer)-z)/lamL]/cosh[(t-2*deadLayer)/lamL]=:f(z) B/Bext-f(z)" << endl;
    for (int i=0; i<zz.size(); i++) {
      fout << zz[i] << " " << BB[i] << " " << BBL[i] << " " << BB[i]-BBL[i] << endl;
    }

    fout.close();
  }

  zz.clear();
  BB.clear();
  BBL.clear();

  return 0;
}
