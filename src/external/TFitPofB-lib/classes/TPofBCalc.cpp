/***************************************************************************

  TPofBCalc.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/09/04

***************************************************************************/

#include "TPofTCalc.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>

#include <omp.h>

/* USED FOR DEBUGGING-----------------------------------
#include <cstdio>
#include <ctime>
/-------------------------------------------------------*/

// Do not actually calculate P(B) but take it from a B and a P(B) vector of the same size

TPofBCalc::TPofBCalc( const vector<double>& b, const vector<double>& pb, double dt) {
  assert(b.size() == pb.size() && b.size() >= 2);

  fB = b;
  fPB = pb;

  vector<double>::const_iterator iter, iterB;
  iterB = b.begin();

  for(iter = pb.begin(); iter != pb.end(); iter++){
    if(*iter != 0.0) {
      fBmin = *iterB;
      cout << fBmin << endl;
      break;
    }
    iterB++;
  }

  for( ; iter != b.end(); iter++){
    if(*iter == 0.0) {
      fBmax = *(iterB-1);
      cout << fBmax << endl;
      break;
    }
    iterB++;
  }

  fDT = dt; // needed if a convolution should be done
  fDB = b[1]-b[0];
  
  cout << fDB << endl;
}


TPofBCalc::TPofBCalc( const string &type, const vector<double> &para ) : fDT(para[0]), fDB(para[1]) {

if (type == "skg"){ // skewed Gaussian

  fBmin = 0.0;
  fBmax = para[2]/gBar+10.0*fabs(para[4])/(2.0*pi*gBar);

  double BB;
  double expominus(para[3]*para[3]/(2.0*pi*pi*gBar*gBar));
  double expoplus(para[4]*para[4]/(2.0*pi*pi*gBar*gBar));
  double B0(para[2]/gBar);
  double BmaxFFT(1.0/gBar/fDT);

  for ( BB = 0.0 ; BB < B0 ; BB += fDB ) {
    fB.push_back(BB);
    fPB.push_back(exp(-(BB-B0)*(BB-B0)/expominus));
  }

  for ( ; BB < fBmax ; BB += fDB ) {
    fB.push_back(BB);
    fPB.push_back(exp(-(BB-B0)*(BB-B0)/expoplus));
  }

  unsigned int lastZerosStart(fB.size());

  for ( ; BB <= BmaxFFT ; BB += fDB ) {
    fB.push_back(BB);
    fPB.push_back(0.0);
  }

  // make sure that we have an even number of elements in p(B) for FFT, so we do not have to care later

  if (fB.size() % 2) {
    fB.push_back(BB);
    fPB.push_back(0.0);
  }

  // normalize p(B)

  double pBsum = 0.0;
  for (unsigned int i(0); i<lastZerosStart; i++)
    pBsum += fPB[i];
  pBsum *= fDB;
  for (unsigned int i(0); i<lastZerosStart; i++)
    fPB[i] /= pBsum;

} else { // fill the vectors with zeros

  fBmin = 0.0;
  fBmax = 1.0/gBar/fDT;

  double BB;

  for (BB=0.0 ; BB < fBmax ; BB += fDB ) {
    fB.push_back(BB);
    fPB.push_back(0.0);
  }

  // make sure that we have an even number of elements in p(B) for FFT, so we do not have to care later

  if (fB.size() % 2) {
    fB.push_back(BB);
    fPB.push_back(0.0);
  }

}

}

//-----------
// Constructor that does the P(B) calculation for given analytical inverse of B(z) and its derivative and n(z)
// Parameters: dt[us], dB[G], Energy[keV], Bbg[G], width[us^{-1}], weight[1]
//-----------

TPofBCalc::TPofBCalc( const TBofZCalcInverse &BofZ, const TTrimSPData &dataTrimSP, const vector<double> &para ) : fDT(para[0]), fDB(para[1])
{

  fBmin = BofZ.GetBmin();
  fBmax = BofZ.GetBmax();

  double BB;

  // fill not used Bs before Bmin with 0.0

  for (BB = 0.0; BB < fBmin; BB += fDB)
    fB.push_back(BB);
  fPB.resize(fB.size(),0.0);

  unsigned int firstZerosEnd(fB.size());

  // calculate p(B) from the inverse of B(z)

  for ( ; BB <= fBmax ; BB += fDB) {
    fB.push_back(BB);
    fPB.push_back(0.0);

    vector< pair<double, double> > inv;
    inv = BofZ.GetInverseAndDerivative(BB);

    for (unsigned int i(0); i<inv.size(); i++)
      *(fPB.end()-1) += dataTrimSP.GetNofZ(inv[i].first, para[2])*fabs(inv[i].second);

  }

  unsigned int lastZerosStart(fB.size());

  // fill not used Bs after Bext with 0.0

  double BmaxFFT(1.0/gBar/fDT);

  for ( ; BB <= BmaxFFT ; BB += fDB )
    fB.push_back(BB);
  fPB.resize(fB.size(),0.0);


  // make sure that we have an even number of elements in p(B) for FFT, so we do not have to care later
  if (fB.size() % 2) {
    fB.push_back(BB);
    fPB.push_back(0.0);
  }

  // normalize p(B)

  double pBsum = 0.0;
  for (unsigned int i(firstZerosEnd); i<lastZerosStart; i++)
    pBsum += fPB[i];
  pBsum *= fDB;
  for (unsigned int i(firstZerosEnd); i<lastZerosStart; i++)
    fPB[i] /= pBsum;

  if(para.size() == 6)
    AddBackground(para[3], para[4], para[5]);

}



//-----------
// Constructor that does the P(B) calculation for given B(z) and n(z)
// Parameters: dt[us], dB[G], Energy[keV]
//-----------

TPofBCalc::TPofBCalc( const TBofZCalc &BofZ, const TTrimSPData &dataTrimSP, const vector<double> &para, unsigned int zonk ) : fDT(para[0]), fDB(para[1]) {

  fBmin = BofZ.GetBmin();
  fBmax = BofZ.GetBmax();

  double BB, BBnext;
  double zm, zp, zNext, dz;

  // fill not used Bs before Bmin with 0.0

  for (BB = 0.0; BB < fBmin; BB += fDB) {
    fB.push_back(BB);
    fPB.push_back(0.0);
  }

  unsigned int firstZerosEnd(fB.size());

  // calculate p(B) from B(z)

  vector<double> bofzZ(BofZ.DataZ());
  vector<double> bofzBZ(BofZ.DataBZ());
  double ddZ(BofZ.GetDZ());

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

  char debugfile2[50];
  int n2 = sprintf (debugfile2, "test_NZgss_%ld_%f.dat", seconds, para[2]);

  if (n1 > 0) {
    ofstream of1(debugfile1);
//    assure(of1, debugfile1);

    dataTrimSP.Normalize(para[2]);

    for (unsigned int i(0); i<dataTrimSP.DataZ(para[2]).size(); i++) {
      of1 << dataTrimSP.DataZ(para[2])[i] << " " << dataTrimSP.DataNZ(para[2])[i] << " " << dataTrimSP.OrigDataNZ(para[2])[i] << endl;
    }
    of1.close();
  }

  if (n2 > 0) {
    ofstream of2(debugfile2);
//    assure(of1, debugfile1);

    dataTrimSP.ConvolveGss(10.0,para[2]);
    dataTrimSP.Normalize(para[2]);
    
    for (unsigned int i(0); i<dataTrimSP.DataZ(para[2]).size(); i++) {
      of2 << dataTrimSP.DataZ(para[2])[i] << " " << dataTrimSP.DataNZ(para[2])[i] << " " << dataTrimSP.OrigDataNZ(para[2])[i] << endl;
    }
    of2.close();
  }

/---------------------------------------------------------*/

//  dataTrimSP.ConvolveGss(10.0,para[2]); // convolve implantation profile by gaussian

  double nn;
  bool zNextFound(false);

  for ( ; BB <= fBmax ; BB += fDB) {
    BBnext = BB + fDB;
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
            *(fPB.end()-1) += nn*fabs(dz/fDB);
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
            *(fPB.end()-1) += nn*fabs(dz/fDB);
          }
        }
     }
    }

  }

  unsigned int lastZerosStart(fB.size());

  // fill not used Bs after Bext with 0.0

  double BmaxFFT(1.0/gBar/fDT);

//  cout << "N = " << int(BmaxFFT/para[1]+1.0) << endl;

  for ( ; BB <= BmaxFFT ; BB += fDB ) {
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
  pBsum *= fDB;
  for (unsigned int i(firstZerosEnd); i<lastZerosStart; i++)
    fPB[i] /= pBsum;

}

//-----------
// Constructor that does the P(B) calculation for a bulk vortex lattice
// Parameters: dt[us], dB[G] [, Bbg[G], width[us^{-1}], weight[1] ]
//-----------

TPofBCalc::TPofBCalc( const TBulkVortexFieldCalc &vortexLattice, const vector<double> &para ) : fDT(para[0]), fDB(para[1])
{

  fBmin = vortexLattice.GetBmin();
  fBmax = vortexLattice.GetBmax();

  unsigned int a1(static_cast<int>(floor(fBmin/fDB)));
  unsigned int a2(static_cast<int>(ceil(fBmin/fDB)));
  unsigned int a3(static_cast<int>(floor(fBmax/fDB)));
  unsigned int a4(static_cast<int>(ceil(fBmax/fDB)));

  unsigned int firstZerosEnd ((a1 != a2) ? a1 : (a1 - 1));
  unsigned int lastZerosStart ((a3 != a4) ? a4 : (a4 + 1));

  // fill the B vector
  unsigned int indexBmaxFFT(static_cast<int>(ceil(1.0/(gBar*fDT*fDB))));
  fB.resize(indexBmaxFFT, 0.0);
  fPB.resize(indexBmaxFFT, 0.0);

  int i;
#pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = 0; i < static_cast<int>(indexBmaxFFT); i++)
    fB[i] = fDB*static_cast<double>(i);
// end pragma omp parallel

  // make sure that we have an even number of elements in p(B) for FFT, so we do not have to care later
  if (fB.size() % 2) {
    fB.push_back(*(fB.end()-1)+fDB);
    fPB.push_back(0.0);
  }

  vector< vector<double> > vortexFields(vortexLattice.DataB());

  if (vortexFields.empty()) {
    vortexLattice.CalculateGrid();
    vortexFields = vortexLattice.DataB();
  }

  unsigned int numberOfPoints(vortexFields.size());

  for (unsigned int j(0); j < numberOfPoints; j++){
    for (unsigned int k(0); k < numberOfPoints; k++){
      fPB[static_cast<int>(ceil(vortexFields[j][k]/fDB))] += 1.0;
    }
  }

  double normalizer(static_cast<double>(numberOfPoints*numberOfPoints)*fDB);

#pragma omp parallel for default(shared) private(i) schedule(dynamic)
  for (i = firstZerosEnd; i <= static_cast<int>(lastZerosStart); i++) {
    fPB[i] /= normalizer;
  }
// end pragma omp parallel

//   // normalize p(B)
// 
//   double pBsum = 0.0;
//   for (unsigned int i(firstZerosEnd); i<lastZerosStart; i++)
//     pBsum += fPB[i];
//   pBsum *= fDB;
//   for (unsigned int i(firstZerosEnd); i<lastZerosStart; i++)
//     fPB[i] /= pBsum;

  if(para.size() == 5)
    AddBackground(para[2], para[3], para[4]);

}



// TPofBCalc::AddBackground, Parameters: field B[G], width s[us], weight w

void TPofBCalc::AddBackground(double B, double s, double w) {

  if(!s || w<0.0 || w>1.0 || B<0.0)
    return;

  unsigned int sizePB(fPB.size());
  double BsSq(s*s/(gBar*gBar*4.0*pi*pi));

  // calculate Gaussian background
  vector<double> bg;
  for(unsigned int i(0); i < sizePB; i++) {
    bg.push_back(exp(-(fB[i]-B)*(fB[i]-B)/(2.0*BsSq)));
  }

  // normalize background
  double bgsum(0.0);
  for (unsigned int i(0); i < sizePB; i++)
    bgsum += bg[i];
  bgsum *= fDB;
  for (unsigned int i(0); i < sizePB; i++)
    bg[i] /= bgsum;

  // add background to P(B)
  for (unsigned int i(0); i < sizePB; i++)
    fPB[i] = (1.0 - w)*fPB[i] + w*bg[i];

//   // check if normalization is still valid
//   double pBsum(0.0);
//   for (unsigned int i(0); i < sizePB; i++)
//     pBsum += fPB[i];
//
//   cout << "pBsum = " << pBsum << endl;

}

void TPofBCalc::ConvolveGss(double w) {

  if(!w)
    return;

  unsigned int NFFT;
  double TBin;
  fftw_plan FFTplanToTimeDomain;
  fftw_plan FFTplanToFieldDomain;
  fftw_complex *FFTout;
  double *FFTin;

  NFFT = ( int(1.0/gBar/fDT/fDB+1.0) % 2 ) ? int(1.0/gBar/fDT/fDB+2.0) : int(1.0/gBar/fDT/fDB+1.0);
  TBin = 1.0/gBar/double(NFFT-1)/fDB;

  FFTin = (double *)malloc(sizeof(double) * NFFT);
  FFTout = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (NFFT/2+1));

  // do the FFT to time domain

  FFTplanToTimeDomain = fftw_plan_dft_r2c_1d(NFFT, FFTin, FFTout, FFTW_ESTIMATE);

  for (unsigned int i(0); i<NFFT; i++) {
    FFTin[i] = fPB[i];
  }
  for (unsigned int i(0); i<NFFT/2+1; i++) {
    FFTout[i][0] = 0.0;
    FFTout[i][1] = 0.0;
  }

  fftw_execute(FFTplanToTimeDomain);

  // multiply everything by a gaussian

  double GssInTimeDomain;
  double expo(-2.0*PI*PI*gBar*gBar*w*w*TBin*TBin);

  for (unsigned int i(0); i<NFFT/2+1; i++) {
    GssInTimeDomain = exp(expo*double(i)*double(i));
    FFTout[i][0] *= GssInTimeDomain;
    FFTout[i][1] *= GssInTimeDomain;
  }

  // FFT back to the field domain

  FFTplanToFieldDomain = fftw_plan_dft_c2r_1d(NFFT, FFTout, FFTin, FFTW_ESTIMATE);

  fftw_execute(FFTplanToFieldDomain);

  for (unsigned int i(0); i<NFFT; i++) {
    fPB[i] = FFTin[i];
  }

  // cleanup

  fftw_destroy_plan(FFTplanToTimeDomain);
  fftw_destroy_plan(FFTplanToFieldDomain);
  free(FFTin);
  fftw_free(FFTout);
//  fftw_cleanup();

  // normalize p(B)

  double pBsum = 0.0;
  for (unsigned int i(0); i<NFFT; i++)
    pBsum += fPB[i];
  pBsum *= fDB;
  for (unsigned int i(0); i<NFFT; i++)
    fPB[i] /= pBsum;

  return;
}
