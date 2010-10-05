/***************************************************************************

  TPofBCalc.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/09/04

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Bastian M. Wojek                                *
 *                                                                         *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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

TPofBCalc::TPofBCalc(const vector<double> &para) : fBmin(0.0), fBmax(0.0), fDT(para[0]), fDB(para[1]), fPBExists(false) {
  fPBSize = static_cast<int>(1.0/(gBar*fDT*fDB));
  if (fPBSize % 2) {
    fPBSize += 1;
  } else {
    fPBSize += 2;
  }

  fB = new double[fPBSize];
  fPB = new double[fPBSize];

  int i;

  for (i = 0; i < static_cast<int>(fPBSize); i++) {
    fB[i] = static_cast<double>(i)*fDB;
    fPB[i] = 0.0;
  }
}


// Do not actually calculate P(B) but take it from a B and a P(B) vector of the same size

TPofBCalc::TPofBCalc(const vector<double>& b, const vector<double>& pb, double dt) {
  assert(b.size() == pb.size() && b.size() >= 2);
  fPBSize = pb.size();

  fB = new double[fPBSize];
  fPB = new double[fPBSize];

  for (unsigned int i(0); i < fPBSize; i++) {
    fB[i] = b[i];
    fPB[i] = pb[i];
  }

  vector<double>::const_iterator iter, iterB;
  iterB = b.begin();

  for(iter = pb.begin(); iter != pb.end(); iter++){
    if(*iter != 0.0) {
      fBmin = *iterB;
//      cout << fBmin << endl;
      break;
    }
    iterB++;
  }

  for( ; iter != b.end(); iter++){
    if(*iter == 0.0) {
      fBmax = *(iterB-1);
//      cout << fBmax << endl;
      break;
    }
    iterB++;
  }

  fDT = dt; // needed if a convolution should be done
  fDB = b[1]-b[0];

//  cout << fDB << endl;

  fPBExists = true;
}

void TPofBCalc::UnsetPBExists() {
  int i;
#ifdef HAVE_GOMP
#pragma omp parallel for default(shared) private(i) schedule(dynamic)
#endif
  for (i = 0; i < static_cast<int>(fPBSize); i++) {
    fPB[i] = 0.0;
  }
  fPBExists = false;
}

void TPofBCalc::Calculate(const string &type, const vector<double> &para) {

  if (type == "skg"){ // skewed Gaussian

    fBmin = 0.0;
    fBmax = para[2]/gBar+10.0*fabs(para[4])/(2.0*pi*gBar);

    int a3(static_cast<int>(floor(fBmax/fDB)));
    int a4(static_cast<int>(ceil(fBmax/fDB)));

    unsigned int BmaxIndex((a3 < a4) ? a4 : (a4 + 1));
    unsigned int B0Index(static_cast<int>(ceil(para[2]/(gBar*fDB))));

    double expominus(para[3]*para[3]/(2.0*pi*pi*gBar*gBar));
    double expoplus(para[4]*para[4]/(2.0*pi*pi*gBar*gBar));
    double B0(para[2]/(gBar));

    for (unsigned int i(0); i < B0Index; i++) {
      fPB[i] = exp(-(fB[i]-B0)*(fB[i]-B0)/expominus);
    }

    for (unsigned int i(B0Index); i <= BmaxIndex; i++) {
      fPB[i] = exp(-(fB[i]-B0)*(fB[i]-B0)/expoplus);
    }

    // normalize p(B)

    double pBsum = 0.0;
    for (unsigned int i(0); i <= BmaxIndex; i++)
      pBsum += fPB[i];
    pBsum *= fDB;
    for (unsigned int i(0); i <= BmaxIndex; i++)
      fPB[i] /= pBsum;

  }

  fPBExists = true;
  return;
}

//-----------
// Calculate-method that does the P(B) calculation for given analytical inverse of B(z) and its derivative and n(z)
// Parameters: dt[us], dB[G], Energy[keV], Bbg[G], width[us^{-1}], weight[1]
//-----------

void TPofBCalc::Calculate(const TBofZCalcInverse *BofZ, const TTrimSPData *dataTrimSP, const vector<double> &para) {

  if(fPBExists)
    return;

  fBmin = BofZ->GetBmin();
  fBmax = BofZ->GetBmax();

  int a1(static_cast<int>(floor(fBmin/fDB)));
  int a2(static_cast<int>(ceil(fBmin/fDB)));
  int a3(static_cast<int>(floor(fBmax/fDB)));
  int a4(static_cast<int>(ceil(fBmax/fDB)));

  unsigned int firstZerosEnd ((a1 < a2) ? a1 : ((a1 > 0) ? (a1 - 1) : 0));
  unsigned int lastZerosStart ((a3 < a4) ? a4 : (a4 + 1));

  if (lastZerosStart >= fPBSize) {
    lastZerosStart = fPBSize - 1;
  }

  unsigned int i;

  // calculate p(B) from the inverse of B(z)

  for (i = firstZerosEnd; i <= lastZerosStart; i++) {

    vector< pair<double, double> > inv;
    inv = BofZ->GetInverseAndDerivative(fB[i]);

    for (unsigned int j(0); j < inv.size(); j++) {
      fPB[i] += dataTrimSP->GetNofZ(inv[j].first, para[2])*fabs(inv[j].second);
    }
//    if (fPB[i])
//      cout << fB[i] << " " << fPB[i] << endl;
  }

  // normalize p(B)

  double pBsum = 0.0;
  for (i = firstZerosEnd; i<=lastZerosStart; i++)
    pBsum += fPB[i];
  pBsum *= fDB;
  for (i = firstZerosEnd; i<=lastZerosStart; i++)
    fPB[i] /= pBsum;

  if(para.size() == 6 && para[5] != 0.0)
    AddBackground(para[3], para[4], para[5]);

}

//-----------
// Calculate-method that does the P(B) calculation for given B(z) and n(z)
// Parameters: dt[us], dB[G], Energy[keV]
//-----------

void TPofBCalc::Calculate(const TBofZCalc *BofZ, const TTrimSPData *dataTrimSP, const vector<double> &para, unsigned int zonk) {

  if(fPBExists)
    return;

  fBmin = BofZ->GetBmin();
  fBmax = BofZ->GetBmax();

  int a1(static_cast<int>(floor(fBmin/fDB)));
  int a2(static_cast<int>(ceil(fBmin/fDB)));
  int a3(static_cast<int>(floor(fBmax/fDB)));
  int a4(static_cast<int>(ceil(fBmax/fDB)));

  unsigned int firstZerosEnd ((a1 < a2) ? a1 : ((a1 > 0) ? (a1 - 1) : 0));
  unsigned int lastZerosStart ((a3 < a4) ? a4 : (a4 + 1));

  double BB, BBnext;
  double zm, zp, zNext, dz;

  // calculate p(B) from B(z)

  vector<double> *bofzZ = BofZ->DataZ();
  vector<double> *bofzBZ = BofZ->DataBZ();
  double ddZ(BofZ->GetDZ());

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

  unsigned int i;

  for (i = 0; i <= lastZerosStart; i++) {
    BB = fB[i];
    BBnext = fB[i+1];

    for ( unsigned int j(0); j < bofzZ->size() - 1; j++ ) {
      if ( (*bofzBZ)[j] >= BB && (*bofzBZ)[j+1] <= BB ) {
        zm = (BB-(*bofzBZ)[j])*ddZ/((*bofzBZ)[j+1]-(*bofzBZ)[j]) + (*bofzZ)[j];

        for (unsigned int k(0); k < j; k++) {
          if ( ( (*bofzBZ)[j-k] <= BBnext && (*bofzBZ)[j-k-1] >= BBnext ) ) {
//           cout << "1 " << j << " " << k << endl;
            zNext = (BBnext-(*bofzBZ)[j-k-1])*ddZ/((*bofzBZ)[j-k]-(*bofzBZ)[j-k-1]) + (*bofzZ)[j-k-1];
            zNextFound = true;
            break;
          }
        }

        if(zNextFound) {
          zNextFound = false;

          dz = zNext-zm;
          nn = dataTrimSP->GetNofZ(zm, para[2]);
          if (nn != -1.0) {
//          cout << "zNext = " << zNextm << ", zm = " << zm << ", dz = " << dz << endl;
            fPB[i] += nn*fabs(dz/fDB);
          }
        }

      } else if ((*bofzBZ)[j] <= BB && (*bofzBZ)[j+1] >= BB ) {
        zp = (BB-(*bofzBZ)[j])*ddZ/((*bofzBZ)[j+1]-(*bofzBZ)[j]) + (*bofzZ)[j];

        for (unsigned int k(0); k < bofzZ->size() - j - 1; k++) {
          if ( ( (*bofzBZ)[j+k] <= BBnext && (*bofzBZ)[j+k+1] >= BBnext ) ) {
//            cout << "2 " << j << " " << k << endl;
            zNext = (BBnext-(*bofzBZ)[j+k])*ddZ/((*bofzBZ)[j+k+1]-(*bofzBZ)[j+k]) + (*bofzZ)[j+k];
            zNextFound = true;
            break;
          }
        }

        if(zNextFound) {
          zNextFound = false;

          dz = zNext-zp;
          nn = dataTrimSP->GetNofZ(zp, para[2]);
          if (nn != -1.0) {
//            cout << "zNext = " << zNextp << ", zp = " << zp << ", dz = " << dz << endl;
            fPB[i] += nn*fabs(dz/fDB);
          }
        }
     }
    }

  }

  bofzZ = 0;
  bofzBZ = 0;

  // normalize p(B)

  double pBsum = 0.0;
  for (unsigned int i(firstZerosEnd); i<=lastZerosStart; i++)
    pBsum += fPB[i];
  pBsum *= fDB;
  for (unsigned int i(firstZerosEnd); i<=lastZerosStart; i++)
    fPB[i] /= pBsum;

  fPBExists = true;
  return;
}

//-----------
// Calculate method that does the P(B) calculation for a bulk vortex lattice
// Parameters: dt[us], dB[G] [, Bbg[G], width[us^{-1}], weight[1] ]
//-----------

void TPofBCalc::Calculate(const TBulkVortexFieldCalc *vortexLattice, const vector<double> &para) {

  if(fPBExists)
    return;

  fBmin = vortexLattice->GetBmin();
  fBmax = vortexLattice->GetBmax();

  int a1(static_cast<int>(floor(fBmin/fDB)));
  int a2(static_cast<int>(ceil(fBmin/fDB)));
  int a3(static_cast<int>(floor(fBmax/fDB)));
  int a4(static_cast<int>(ceil(fBmax/fDB)));

  //unsigned int firstZerosEnd ((a1 < a2) ? a1 : ((a1 > 0) ? (a1 - 1) : 0));
  unsigned int lastZerosStart ((a3 < a4) ? a4 : (a4 + 1));
  unsigned int numberOfSteps(vortexLattice->GetNumberOfSteps());
  unsigned int numberOfStepsSq(numberOfSteps*numberOfSteps);
  unsigned int numberOfSteps_2(numberOfSteps/2);
  //unsigned int numberOfStepsSq_2(numberOfStepsSq/2);

  if (lastZerosStart >= fPBSize)
    lastZerosStart = fPBSize - 1;

//  cout << endl << fBmin << " " << fBmax << " " << firstZerosEnd << " " << lastZerosStart << " " << numberOfSteps << endl;

  if (!vortexLattice->GridExists()) {
    vortexLattice->CalculateGrid();
  }

  double *vortexFields = vortexLattice->DataB();
  unsigned int fill_index;

  if (para.size() == 7 && para[6] == 1.0 && para[5] != 0.0 && vortexLattice->IsTriangular()) {
    // weight distribution with Gaussian around vortex-cores
    double Rsq1, Rsq2, Rsq3, Rsq4, Rsq5, Rsq6, sigmaSq(-0.5*para[5]*para[5]);
    for (unsigned int j(0); j < numberOfSteps_2; ++j) {
      for (unsigned int i(0); i < numberOfSteps_2; ++i) {
        fill_index = static_cast<unsigned int>(ceil(fabs((vortexFields[i + numberOfSteps*j]/fDB))));
        if (fill_index < fPBSize) {
          Rsq1 = static_cast<double>(3*i*i + j*j)/static_cast<double>(numberOfStepsSq);
          Rsq2 = static_cast<double>(3*(numberOfSteps_2 - i)*(numberOfSteps_2 - i) \
               + (numberOfSteps_2 - j)*(numberOfSteps_2 - j))/static_cast<double>(numberOfStepsSq);
          Rsq3 = static_cast<double>(3*(numberOfSteps - i)*(numberOfSteps - i) \
               + j*j)/static_cast<double>(numberOfStepsSq);
          Rsq4 = static_cast<double>(3*(numberOfSteps_2 - i)*(numberOfSteps_2 - i) \
               + (numberOfSteps_2 + j)*(numberOfSteps_2 + j))/static_cast<double>(numberOfStepsSq);
          Rsq5 = static_cast<double>(3*i*i \
               + (numberOfSteps - j)*(numberOfSteps - j))/static_cast<double>(numberOfStepsSq);
          Rsq6 = static_cast<double>(3*(numberOfSteps_2 + i)*(numberOfSteps_2 + i) \
               + (numberOfSteps_2 - j)*(numberOfSteps_2 - j))/static_cast<double>(numberOfStepsSq);
          fPB[fill_index] += exp(sigmaSq*Rsq1) + exp(sigmaSq*Rsq2) + exp(sigmaSq*Rsq3) \
                           + exp(sigmaSq*Rsq4) + exp(sigmaSq*Rsq5) + exp(sigmaSq*Rsq6);
        }
      }
    }
  } else if (para.size() == 7 && para[6] == 2.0 && para[5] != 0.0 && vortexLattice->IsTriangular()) {
    // weight distribution with Lorentzian around vortex-cores
    double Rsq1, Rsq2, Rsq3, Rsq4, Rsq5, Rsq6, sigmaSq(para[5]*para[5]);
//    ofstream of("LorentzWeight.dat");
    for (unsigned int j(0); j < numberOfSteps_2; ++j) {
      for (unsigned int i(0); i < numberOfSteps_2; ++i) {
        fill_index = static_cast<unsigned int>(ceil(fabs((vortexFields[i + numberOfSteps*j]/fDB))));
        if (fill_index < fPBSize) {
          Rsq1 = static_cast<double>(3*i*i + j*j)/static_cast<double>(numberOfStepsSq);
          Rsq2 = static_cast<double>(3*(numberOfSteps_2 - i)*(numberOfSteps_2 - i) \
               + (numberOfSteps_2 - j)*(numberOfSteps_2 - j))/static_cast<double>(numberOfStepsSq);
          Rsq3 = static_cast<double>(3*(numberOfSteps - i)*(numberOfSteps - i) \
               + j*j)/static_cast<double>(numberOfStepsSq);
          Rsq4 = static_cast<double>(3*(numberOfSteps_2 - i)*(numberOfSteps_2 - i) \
               + (numberOfSteps_2 + j)*(numberOfSteps_2 + j))/static_cast<double>(numberOfStepsSq);
          Rsq5 = static_cast<double>(3*i*i \
               + (numberOfSteps - j)*(numberOfSteps - j))/static_cast<double>(numberOfStepsSq);
          Rsq6 = static_cast<double>(3*(numberOfSteps_2 + i)*(numberOfSteps_2 + i) \
               + (numberOfSteps_2 - j)*(numberOfSteps_2 - j))/static_cast<double>(numberOfStepsSq);
          fPB[fill_index] += 1.0/(1.0+sigmaSq*Rsq1) + 1.0/(1.0+sigmaSq*Rsq2) + 1.0/(1.0+sigmaSq*Rsq3) \
                           + 1.0/(1.0+sigmaSq*Rsq4) + 1.0/(1.0+sigmaSq*Rsq5) + 1.0/(1.0+sigmaSq*Rsq6);

//          of << 1.0/(1.0+sigmaSq*Rsq1) + 1.0/(1.0+sigmaSq*Rsq2) + 1.0/(1.0+sigmaSq*Rsq3) \
//                           + 1.0/(1.0+sigmaSq*Rsq4) + 1.0/(1.0+sigmaSq*Rsq5) + 1.0/(1.0+sigmaSq*Rsq6) << " ";
        }
      }
//      of << endl;
    }
//    of.close();
  } else {
    for (unsigned int j(0); j < numberOfSteps_2; ++j) {
      for (unsigned int i(0); i < numberOfSteps_2; ++i) {
        fill_index = static_cast<unsigned int>(ceil(fabs((vortexFields[i + numberOfSteps*j]/fDB))));
        if (fill_index < fPBSize) {
          fPB[fill_index] += 1.0;
        }
      }
    }
  }

  vortexFields = 0;

  // normalize P(B)
  double sum(0.0);
  for (unsigned int i(0); i < fPBSize; ++i)
    sum += fPB[i];
  sum *= fDB;
  int i;
#ifdef HAVE_GOMP
#pragma omp parallel for default(shared) private(i) schedule(dynamic)
#endif
  for (i = 0; i < static_cast<int>(fPBSize); ++i)
    fPB[i] /= sum;
// end pragma omp parallel

  if(para.size() == 5)
    AddBackground(para[2], para[3], para[4]);

  fPBExists = true;
  return;
}

// TPofBCalc::AddBackground, Parameters: field B[G], width s[us], weight w

void TPofBCalc::AddBackground(double B, double s, double w) {

  if(!s || w<0.0 || w>1.0 || B<0.0)
    return;

  double BsSq(s*s/(gBar*gBar*4.0*pi*pi));

  // calculate Gaussian background
  double bg[fPBSize];
  for(unsigned int i(0); i < fPBSize; i++) {
    bg[i] = exp(-(fB[i]-B)*(fB[i]-B)/(2.0*BsSq));
  }

  // normalize background
  double bgsum(0.0);
  for (unsigned int i(0); i < fPBSize; i++)
    bgsum += bg[i];
  bgsum *= fDB;
  for (unsigned int i(0); i < fPBSize; i++)
    bg[i] /= bgsum;

  // add background to P(B)
  for (unsigned int i(0); i < fPBSize; i++)
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

  unsigned int NFFT(fPBSize);
  double TBin;
  fftw_plan FFTplanToTimeDomain;
  fftw_plan FFTplanToFieldDomain;
  fftw_complex *FFTout;

  TBin = 1.0/(gBar*double(NFFT-1)*fDB);

  FFTout = new fftw_complex[NFFT/2 + 1]; //(fftw_complex *)fftw_malloc(sizeof(fftw_complex) * (NFFT/2+1));

  // do the FFT to time domain

  FFTplanToTimeDomain = fftw_plan_dft_r2c_1d(NFFT, fPB, FFTout, FFTW_ESTIMATE);

  fftw_execute(FFTplanToTimeDomain);

  // multiply everything by a gaussian

  double GssInTimeDomain;
  double expo(-2.0*PI*PI*gBar*gBar*w*w*TBin*TBin);

  for (unsigned int i(0); i < NFFT/2+1; i++) {
    GssInTimeDomain = exp(expo*static_cast<double>(i*i));
    FFTout[i][0] *= GssInTimeDomain;
    FFTout[i][1] *= GssInTimeDomain;
  }

  // FFT back to the field domain

  FFTplanToFieldDomain = fftw_plan_dft_c2r_1d(NFFT, FFTout, fPB, FFTW_ESTIMATE);

  fftw_execute(FFTplanToFieldDomain);

  // cleanup

  fftw_destroy_plan(FFTplanToTimeDomain);
  fftw_destroy_plan(FFTplanToFieldDomain);
  delete[] FFTout; //  fftw_free(FFTout);
  FFTout = 0;
//  fftw_cleanup();

  // normalize p(B)

  double pBsum = 0.0;
  for (unsigned int i(0); i < NFFT; i++)
    pBsum += fPB[i];
  pBsum *= fDB;
  for (unsigned int i(0); i < NFFT; i++)
    fPB[i] /= pBsum;

  return;
}

double TPofBCalc::GetFirstMoment() const {

  double pBsum(0.0);
  for (unsigned int i(0); i < fPBSize; i++)
    pBsum += fB[i]*fPB[i];
  pBsum *= fDB;

  return pBsum;

}
