/***************************************************************************

  TPofBCalc.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/06/03

***************************************************************************/

#include "TPofBCalc.h"
#include <cmath>
#include <iostream>
#include <fstream>

/* USED FOR DEBUGGING-----------------------------------
#include <cstdio>
#include <ctime>
-------------------------------------------------------*/

//-----------
// Constructor that does the P(B) calculation for given B(z) and n(z)
// Parameters: dt[us], dB[G], Energy[keV]
//-----------

TPofBCalc::TPofBCalc( const TBofZCalc &BofZ, const TTrimSPData &dataTrimSP, const vector<double> &para ) {

  fBmin = BofZ.GetBmin();
  fBmax = BofZ.GetBmax();

  double BB, BBnext;
  double zm, zp, zNext, dz;

  // fill not used Bs before Bmin with 0.0

  for ( BB = 0.0 ; BB < fBmin ; BB += para[1] ) {
    fB.push_back(BB);
    fPB.push_back(0.0);
  }

  unsigned int firstZerosEnd(fB.size());

  // calculate p(B) from B(z)

  vector<double> bofzZ(BofZ.DataZ());
  vector<double> bofzBZ(BofZ.DataBZ());
  double ddZ(BofZ.GetdZ());

/* USED FOR DEBUGGING-----------------------------------
  cout << "Bmin = " << fBmin << ", Bmax = " << fBmax << endl;

  time_t seconds;
  seconds = time (NULL);

  char debugfile[50];
  int n = sprintf (debugfile, "test_Bz_%ld_%f.dat", seconds, fBmin);

  if (n > 0) {
    ofstream of(debugfile);
//    assure(of, debugfile);

    for (unsigned int i(0); i<bofzZ.size(); i++) {
      of << bofzZ[i] << " " << bofzBZ[i] << endl;
    }
    of.close();
  }

  char debugfile1[50];
  int n1 = sprintf (debugfile1, "test_NZ_%ld_%f.dat", seconds, para[2]);

  if (n1 > 0) {
    ofstream of1(debugfile1);
//    assure(of1, debugfile1);

    for (unsigned int i(0); i<dataTrimSP.DataZ(para[2]).size(); i++) {
      of1 << dataTrimSP.DataZ(para[2])[i] << " " << dataTrimSP.DataNZ(para[2])[i] << " " << dataTrimSP.OrigDataNZ(para[2])[i] << endl;
    }
    of1.close();
  }

---------------------------------------------------------*/

  double nn;
  bool zNextFound(false);

  for ( ; BB <= fBmax ; BB += para[1]) {
    BBnext = BB + para[1];
    fB.push_back(BB);
    fPB.push_back(0.0);

    for ( unsigned int j(0); j < bofzZ.size() - 1; j++ ) {
      if ( bofzBZ[j] >= BB && bofzBZ[j+1] <= BB ) {
        zm = (BB-bofzBZ[j])*ddZ/(bofzBZ[j+1]-bofzBZ[j]) + bofzZ[j];

        for (unsigned int k(0); k < j; k++) {
          if ( ( bofzBZ[j-k] <= BBnext && bofzBZ[j-k-1] >= BBnext ) ) {
//           cout << "1 " << j << " " << k << endl;
            zNext = (BBnext-bofzBZ[j-k-1])*ddZ/(bofzBZ[j-k]-bofzBZ[j-k-1]) + bofzZ[j-k-1];
            zNextFound = true;
            break;
          }
        }

        if(zNextFound) {
          zNextFound = false;

          dz = zNext-zm;
          nn = dataTrimSP.GetNofZ(zm, para[2]);
          if (nn != -1.0) {
//          cout << "zNext = " << zNextm << ", zm = " << zm << ", dz = " << dz << endl;
            *(fPB.end()-1) += nn*fabs(dz/para[1]);
          }
        }

      } else if (bofzBZ[j] <= BB && bofzBZ[j+1] >= BB ) {
        zp = (BB-bofzBZ[j])*ddZ/(bofzBZ[j+1]-bofzBZ[j]) + bofzZ[j];

        for (unsigned int k(0); k < bofzZ.size() - j - 1; k++) {
          if ( ( bofzBZ[j+k] <= BBnext && bofzBZ[j+k+1] >= BBnext ) ) {
//            cout << "2 " << j << " " << k << endl;
            zNext = (BBnext-bofzBZ[j+k])*ddZ/(bofzBZ[j+k+1]-bofzBZ[j+k]) + bofzZ[j+k];
            zNextFound = true;
            break;
          }
        }

        if(zNextFound) {
          zNextFound = false;

          dz = zNext-zp;
          nn = dataTrimSP.GetNofZ(zp, para[2]);
          if (nn != -1.0) {
//            cout << "zNext = " << zNextp << ", zp = " << zp << ", dz = " << dz << endl;
            *(fPB.end()-1) += nn*fabs(dz/para[1]);
          }
        }
     }
    }

  }

  unsigned int lastZerosStart(fB.size());

  // fill not used Bs after Bext with 0.0

  double BmaxFFT(1.0/gBar/para[0]);

//  cout << "N = " << int(BmaxFFT/para[1]+1.0) << endl;

  for ( ; BB <= BmaxFFT ; BB += para[1] ) {
    fB.push_back(BB);
    fPB.push_back(0.0);
  }

  // make sure that we have an even number of elements in p(B) for FFT, so we do not have to care later

  if (fB.size() % 2) {
    fB.push_back(BB);
    fPB.push_back(0.0);
  }

//  cout << "size of B = " << fB.size() << ", size of p(B) = " << fPB.size() << endl;

  // normalize p(B)

  double pBsum = 0.0;
  for (unsigned int i(firstZerosEnd); i<lastZerosStart; i++)
    pBsum += fPB[i];
  pBsum *= para[1];
  for (unsigned int i(firstZerosEnd); i<lastZerosStart; i++)
    fPB[i] /= pBsum;

}
