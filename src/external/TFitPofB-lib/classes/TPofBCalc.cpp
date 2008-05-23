/***************************************************************************

  TPofBCalc.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/05/23

***************************************************************************/

#include "TPofBCalc.h"
#include <cmath>
#include <iostream>

TPofBCalc::TPofBCalc( const TBofZCalc &BofZ, const TTrimSPData &dataTrimSP, const vector<double> &para ) {

  // Parameters: dt[us], dB[G], Energy[keV]
  
  double BB, BBnext;
  double zm, zp, zNextm, zNextp, dz;

  // fill not used Bs before Bmin with 0.0
  
  for ( BB = 0.0 ; BB < BofZ.GetBmin() ; BB += para[1] ) {
    fB.push_back(BB);
    fPB.push_back(0.0);
  }
  
  unsigned int firstZerosEnd(fB.size());
  
  // calculate p(B) from B(z)
  
  vector<double> bofzZ(BofZ.DataZ());
  vector<double> bofzBZ(BofZ.DataBZ());
  double ddZ(BofZ.GetdZ());
  double Bmax(BofZ.GetBmax());
  
  double nn;
  for ( ; BB <= Bmax ; BB += para[1]) {
    BBnext = BB + para[1];
    fB.push_back(BB);
    fPB.push_back(0.0);
    
    for ( unsigned int j(0); j < bofzZ.size() - 1; j++ ) {
      if ( bofzBZ[j] >= BB && bofzBZ[j+1] <= BB ) {
        zm = (BB-bofzBZ[j])*ddZ/(bofzBZ[j+1]-bofzBZ[j]) + bofzZ[j];
        
        for (unsigned int k(0); k < j; k++) {
          if ( ( bofzBZ[j-k] <= BBnext && bofzBZ[j-k-1] >= BBnext ) ) {
//           cout << "1 " << j << " " << k << endl;
            zNextm = (BBnext-bofzBZ[j-k-1])*ddZ/(bofzBZ[j-k]-bofzBZ[j-k-1]) + bofzZ[j-k-1];
            break;
          }
        }
        
        dz = zNextm-zm;
        nn = dataTrimSP.GetNofZ(zm, para[2]);
        if (nn != -1.0) {
//          cout << "zNext = " << zNextm << ", zm = " << zm << ", dz = " << dz << endl;
          *(fPB.end()-1) += nn*fabs(dz/para[1]);
        }
          
      } else if (bofzBZ[j] <= BB && bofzBZ[j+1] >= BB ) {
        zp = (BB-bofzBZ[j])*ddZ/(bofzBZ[j+1]-bofzBZ[j]) + bofzZ[j];
        
        for (unsigned int k(0); k < bofzZ.size() - j - 1; k++) {
          if ( ( bofzBZ[j+k] <= BBnext && bofzBZ[j+k+1] >= BBnext ) ) {
//            cout << "2 " << j << " " << k << endl;
            zNextp = (BBnext-bofzBZ[j+k])*ddZ/(bofzBZ[j+k+1]-bofzBZ[j+k]) + bofzZ[j+k];
            break;
          }
        }
        
        dz = zNextp-zp;
        nn = dataTrimSP.GetNofZ(zp, para[2]);
        if (nn != -1.0) {
//          cout << "zNext = " << zNextp << ", zp = " << zp << ", dz = " << dz << endl;
          *(fPB.end()-1) += nn*fabs(dz/para[1]);
        }
     }
    }
  
  }
  
  unsigned int lastZerosStart(fB.size());
  
  // fill not used Bs after Bext with 0.0
  
  double BmaxFFT(1.0/gBar/para[0]);
  
////  cout << "N = " << int(BmaxFFT/para[1]+1.0) << endl;
  
  for ( ; BB <= BmaxFFT ; BB += para[1] ) {
    fB.push_back(BB);
    fPB.push_back(0.0);
  }
  
  // make sure that we have an even number of elements in p(B) for FFT
  
  if (fB.size() % 2) {
    fB.push_back(BB);
    fPB.push_back(0.0);
  }
  
  cout << "size of B = " << fB.size() << ", size of p(B) = " << fPB.size() << endl;

  // normalize pB
  
  double pBsum = 0.0;
  for (unsigned int i(firstZerosEnd); i<lastZerosStart; i++)
    pBsum += fPB[i];
  pBsum *= para[1];
  for (unsigned int i(firstZerosEnd); i<lastZerosStart; i++)
    fPB[i] /= pBsum;

}
