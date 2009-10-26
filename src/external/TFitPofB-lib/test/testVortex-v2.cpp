#include "TPofTCalc.h"
#include <iostream>
#include <fstream>

using namespace std;

#include <cstdio>
#include <sys/time.h>

int main(){

  unsigned int NFFT(512);

  vector<double> parForVortex;
  parForVortex.resize(3);
  
//  parForVortex[0] = 100.0; //app.field
//  parForVortex[1] = 200.0; //lambda
//  parForVortex[2] = 4.0; //xi

  vector<double> parForPofB;
  parForPofB.push_back(0.01); //dt
  parForPofB.push_back(2.0); //dB

  vector<double> parForPofT;
  parForPofT.push_back(0.0); //phase
  parForPofT.push_back(0.01); //dt
  parForPofT.push_back(2.0); //dB

  TBulkTriVortexLondonFieldCalc *vortexLattice = new TBulkTriVortexLondonFieldCalc("/home/l_wojek/analysis/WordsOfWisdom.dat", NFFT);

  parForVortex[0] = 1000.0; //app.field
  parForVortex[1] = 1000.0; //lambda
  parForVortex[2] = 4.0; //xi

  vortexLattice->SetParameters(parForVortex);
  vortexLattice->CalculateGrid();

  ofstream ofy("testVortex-B.dat");
  for (unsigned int j(0); j < NFFT * NFFT; j++) {
    ofy << vortexLattice->DataB()[j] << " ";
    if (!((j+1)%(NFFT)))
      ofy << endl;
  }
  ofy.close();

  TPofBCalc *PofB = new TPofBCalc(parForPofB);
  PofB->Calculate(vortexLattice, parForPofB);

  const double *b(PofB->DataB());
  double *pb(PofB->DataPB());
  unsigned int s(PofB->GetPBSize());

    double test(0.0);

  ofstream ofx("testVortex.dat");
  for (unsigned int i(0); i < s; i++) {
    ofx << b[i] << " " << pb[i] << endl;
    test+=pb[i];
  }
  ofx.close();
  
 cout << test << endl;

  TPofTCalc poft(PofB, "/home/l_wojek/analysis/WordsOfWisdom.dat", parForPofT);
  
  poft.DoFFT();
  poft.CalcPol(parForPofT);
  
  

     ofstream of8("testVortex-Pt.dat");
     for (double i(0.); i<12.0; i+=0.003) {
       test = poft.Eval(i);
       of8 << i << " " << poft.Eval(i) << endl;
     }
     of8.close();

//   parForVortex[0] = 500.0; //app.field
//   parForVortex[1] = 100.0; //lambda
//   parForVortex[2] = 3.0; //xi
// 
//   vortexLattice->SetParameters(parForVortex);
//   vortexLattice->CalculateGrid();
//   PofB->UnsetPBExists();
//   PofB->Calculate(vortexLattice, parForPofB);
// 
//   poft.DoFFT();
//   poft.CalcPol(parForPofT);
// 
//   ofstream of9("testVortex-Pt1.dat");
//   for (double i(0.); i<12.0; i+=0.003) {
// //      test = poft.Eval(i);
//     of9 << i << " " << poft.Eval(i) << endl;
//   }
//   of9.close();

  delete vortexLattice;
  vortexLattice = 0;
  delete PofB;
  PofB = 0;

  parForPofB.clear();
  parForPofT.clear();
  parForVortex.clear();
  
/*
  double par_arr[] = {24.4974, E, 94.6, 10.0, 130.0};
  vector<double> par_vec(par_arr, par_arr+(sizeof(par_arr)/sizeof(par_arr[0])));
  

  vector<double> parForBofZ;
  for (unsigned int i(2); i<par_vec.size(); i++)
    parForBofZ.push_back(par_vec[i]);

  vector<double> parForPofB;
  parForPofB.push_back(0.01); //dt
  parForPofB.push_back(0.1); //dB
  parForPofB.push_back(par_vec[1]);
  parForPofB.push_back(par_vec[2]); // Bkg-Field
  parForPofB.push_back(0.005); // Bkg-width (in principle zero)
  vector<double> interfaces;
  interfaces.push_back(par_vec[3]); // dead layer
  parForPofB.push_back(calcData.LayerFraction(par_vec[1], 1, interfaces)); // Fraction of muons in the deadlayer


  vector<double> zonk;
  vector<double> donk;
  vector<double> hurgaB;
  vector<double> hurgaPB;

  for (unsigned int u(0); u<10 ; u++) {
    
    parForBofZ[par_vec.size()-3] = par_vec[4] + double(u)*5.;

    TLondon1D_HS *BofZ = new TLondon1D_HS(parForBofZ);
    TPofBCalc *PofB = new TPofBCalc(*BofZ, calcData, parForPofB);

    if(u == 0){
      hurgaB = PofB->DataB();
      hurgaPB = PofB->DataPB();
      PofB->ConvolveGss(7.0);
      donk = PofB->DataPB();
    }

    delete BofZ;
    delete PofB;
  }

//  TPofBCalc sumPB(hurgaB, zonk);

//   sumPB.AddBackground(par_vec[2], 0.15, 0.1);
//   sumPB.ConvolveGss(7.0);
//   donk = sumPB.DataPB();

  char debugfile[50];
  int n = sprintf (debugfile, "testInverseExp-BpB_B-%.4f_l-%.3f_E-%.1f_normal.dat", par_vec[2], par_vec[4], par_vec[1]);

  if (n > 0) {
    ofstream of7(debugfile);
    for (unsigned int i(0); i<hurgaB.size(); i++) {
      of7 << hurgaB[i] << " " << donk[i] << " " << hurgaPB[i] << endl;
    }
    of7.close();
  }
*/
//   vector<double> parForPofT;
//   parForPofT.push_back(par_vec[0]); //phase
//   parForPofT.push_back(0.01); //dt
//   parForPofT.push_back(0.02); //dB

//   TLondon1D_HS BofZ(parForBofZ);

//   vector<double> zz;
//   vector<double> Bz;
//   for(double i(0.5); i<2001; i+=1.0) {
//     zz.push_back(i/10.0);
//     if(!(i/10.0 < parForBofZ[1])) {
//       Bz.push_back(BofZ.GetBofZ(i/10.0));
//     }
//     else {
//       Bz.push_back(parForBofZ[0]);
//     }
//   }
/*  
  char debugfile1[50];
  int nn = sprintf (debugfile1, "testInverseExp-Bz_z-%.4f_l-%.3f_E-%.1f_normal.dat", par_vec[2], par_vec[9], par_vec[1]);
  if (nn > 0) {
    ofstream of01(debugfile1);
    for (unsigned int i(0); i<2000; i++) {
      of01 << zz[i] << " " << Bz[i] << endl;
    }
    of01.close();
  }
  */
//  TPofBCalc PofB(BofZ, calcData, parForPofB);
  

//  double t1(0.0);
// get start time
//  struct timeval tv_start, tv_stop;
//  gettimeofday(&tv_start, 0);
//
//  TPofBCalc PofB(BofZ, calcData, parForPofB);
//
//  gettimeofday(&tv_stop, 0);
//  t1 = (tv_stop.tv_sec - tv_start.tv_sec)*1000.0 + (tv_stop.tv_usec - tv_start.tv_usec)/1000.0;

//  cout << "p(B) calculation time with derivatives of the inverse function: " << t1 << " ms" << endl;

//   gettimeofday(&tv_start, 0);
// 
//   BofZ.Calculate();
//   TPofBCalc PofB1(BofZ, calcData, parForPofB, 1);
// 
//   gettimeofday(&tv_stop, 0);
//   t1 = (tv_stop.tv_sec - tv_start.tv_sec)*1000.0 + (tv_stop.tv_usec - tv_start.tv_usec)/1000.0;
// 
//   cout << "p(B) calculation time without derivatives of the inverse function: " << t1 << " ms" << endl;

//  PofB.ConvolveGss(1.17);

//  PofB.AddBackground(par_vec[2], 0.2, calcData.LayerFraction(E, 4, interfaces));
//  PofB.ConvolveGss(1.17);

//   vector<double> hurgaB(PofB.DataB());
//   vector<double> hurgaPB(PofB.DataPB());
//   vector<double> hurgaPB1(PofB1.DataPB());
//   
//   char debugfile[50];
//   int n = sprintf (debugfile, "testInverseExp-BpB_B-%.4f_l-%.3f_E-%.1f_normal.dat", par_vec[2], par_vec[4], par_vec[1]);
// 
//   if (n > 0) {
//     ofstream of7(debugfile);
//     for (unsigned int i(0); i<hurgaB.size(); i++) {
//       of7 << hurgaB[i] << " " << hurgaPB[i] << " " << hurgaPB1[i] << endl;
//     }
//     of7.close();
//   }
  
//   TPofTCalc poft("/home/l_wojek/analysis/WordsOfWisdom.dat", parForPofT);
//   
//   poft.DoFFT(PofB);
//   poft.CalcPol(parForPofT);
//   
//   char debugfilex[50];
//   int nx = sprintf (debugfilex, "4Ltest-P_t-%.4f_l-%.3f_E-%.1f_normal.dat", par_vec[2], par_vec[9], par_vec[1]);
//   
//   if (nx > 0) { 
//     ofstream of8(debugfilex);
//     for (double i(0.); i<12.0; i+=0.003) {
//       of8 << i << " " << poft.Eval(i) << endl;
//     }
//     of8.close();
//   }
/*  
  PofB.ConvolveGss(8.8);
  
  vector<double> hurgaB1(PofB.DataB());
  vector<double> hurgaPB1(PofB.DataPB());
  
  n = sprintf (debugfile, "BpB_B-%.4f_l-%.3f_E-%.1f_broadend8.8G.dat", par_vec[2], par_vec[4], par_vec[1]);
  
  if (n > 0) {
    ofstream of1(debugfile);
    for (unsigned int i(0); i<hurgaB1.size(); i++) {
      of1 << hurgaB1[i] << " " << hurgaPB1[i] << endl;
    }
    of1.close();
  }

  calcData.ConvolveGss(10.0, par_vec[1]);
  calcData.Normalize(par_vec[1]);
  
  TPofBCalc PofB2(BofZ, calcData, parForPofB);
  
  vector<double> z2(calcData.DataZ(par_vec[1]));
  vector<double> nz2(calcData.DataNZ(par_vec[1]));
  
  ofstream of2("Implantation-profile-broad.dat");
  for (unsigned int i(0); i<z2.size(); i++) {
    of2 << z2[i] << " " << nz2[i] << endl;
  }
  of2.close();
  
  vector<double> hurgaB2(PofB2.DataB());
  vector<double> hurgaPB2(PofB2.DataPB());
  
  n = sprintf (debugfile, "BpB_B-%.4f_l-%.3f_E-%.1f_broadend10nm.dat", par_vec[2], par_vec[4], par_vec[1]);
  
  if (n>0) {
    ofstream of8(debugfile);
    for (unsigned int i(0); i<hurgaB2.size(); i++) {
      of8 << hurgaB2[i] << " " << hurgaPB2[i] << endl;
    }
    of8.close();
  }
  
  PofB2.ConvolveGss(7.5);
  
  vector<double> hurgaB3(PofB2.DataB());
  vector<double> hurgaPB3(PofB2.DataPB());
  
  n = sprintf (debugfile, "BpB_B-%.4f_l-%.3f_E-%.1f_broadend10nm+7.5G.dat", par_vec[2], par_vec[4], par_vec[1]);
  
  if (n > 0) {
    ofstream of9(debugfile);
    for (unsigned int i(0); i<hurgaB3.size(); i++) {
      of9 << hurgaB3[i] << " " << hurgaPB3[i] << endl;
    }
    of9.close();
  }
  
  z.clear();
  nz.clear();
  z2.clear();
  nz2.clear();
*/
/*

  double param[8] = {100.0, 5.0, 50.0, 100.0, 40.0, 0.01, 0.1, 15.0};
  vector<double> parameter(param,param+8);
  vector<double> param_for_BofZ;
  vector<double> param_for_PofB;
  vector<double> param_for_PofT;
  
  for (unsigned int i(0); i<4; i++)
    param_for_BofZ.push_back(parameter[i]);
    
  for (unsigned int i(5); i<8; i++)
    param_for_PofB.push_back(parameter[i]);
    
  for (unsigned int i(4); i<7; i++)
    param_for_PofT.push_back(parameter[i]);
    
  TLondon1D_1L bofz(param_for_BofZ);
  
  cout << "Bmin = " << bofz.GetBmin() << endl;
  cout << "Bmax = " << bofz.GetBmax() << endl;
  cout << "dZ = " << bofz.GetdZ() << endl;
  
  ofstream of5("test_Bz.dat");
  for (double i(0); i<50.; i+=0.1) {
    of5 << i << " " << bofz.GetBofZ(i) << endl;
   }
   of5.close();
   
   ofstream of6("test_zBz.dat");
  for (unsigned int i(0); i<bofz.DataZ().size(); i++) {
    of6 << bofz.DataZ()[i] << " " << bofz.DataBZ()[i] << endl;
   }
   of6.close();
   
   TPofBCalc pofb(bofz, calcData, param_for_PofB);
  
  cout << "Output to file now..." << endl;
  
  vector<double> hurgaB(pofb.DataB());
  vector<double> hurgaPB(pofb.DataPB());
  
  ofstream of7("test_BpB.dat");
  for (unsigned int i(0); i<hurgaB.size(); i++) {
    of7 << hurgaB[i] << " " << hurgaPB[i] << endl;
   }
   of7.close();
   
  TPofTCalc poft(param_for_PofT);
  
  poft.DoFFT(pofb, param_for_PofT);
  
  ofstream of8("test_tpt4.dat");
  for (double i(0.); i<12.0; i+=0.003) {
    of8 << i << " " << poft(i) << endl;
  }
  of8.close();

*/
/**************** Test TLondon1DHS *********************************

//  unsigned int parNo_arr[] = {1, 2, 5, 7, 9, 10, 11, 12};
  double par_arr[] = {24.4974, 22.0, 95.8253, 7.62096, 143.215};
  
//  vector<unsigned int> parNo_vec(parNo_arr, parNo_arr+(sizeof(parNo_arr)/sizeof(parNo_arr[0])));
  vector<double> par_vec(par_arr, par_arr+(sizeof(par_arr)/sizeof(par_arr[0])));
  
//  vector<double> par_vec_sub;
  
//  for(unsigned int i(0); i<parNo_vec.size(); i++) {
//    par_vec_sub.push_back(par_vec[parNo_vec[i]-1]);
//  }
  
  TLondon1DHS fitter;

************************************************************************/


/**************** Test TLondon1D1L *********************************

  unsigned int parNo_arr[] = {1, 3, 5, 7, 9, 10, 11, 12};
  double par_arr[] = {0.0, 999.0, 0.01, 999.0, 0.01, 999.0, 4.6, 999.0, 100.0, 5.0, 190.0, 180.0};
  
  vector<unsigned int> parNo_vec(parNo_arr, parNo_arr+(sizeof(parNo_arr)/sizeof(parNo_arr[0])));
  vector<double> par_vec(par_arr, par_arr+(sizeof(par_arr)/sizeof(par_arr[0])));
  
  vector<double> par_vec_sub;
  
  for(unsigned int i(0); i<parNo_vec.size(); i++) {
    par_vec_sub.push_back(par_vec[parNo_vec[i]-1]);
  }
  
  TLondon1D1L fitter(parNo_vec, par_vec);

************************************************************************/

/**************** Test TLondon1D2L **********************************

  unsigned int parNo_arr[] = {1, 3, 5, 7, 9, 10, 11, 12, 13, 14, 15, 16};
  double par_arr[] = {0.0, 999.0, 0.01, 999.0, 0.01, 999.0, 21.6, 999.0, 100.0, 5.0, 70.0, 70.0, 180.0, 500.0, 1.0, 0.3};
  
  vector<unsigned int> parNo_vec(parNo_arr, parNo_arr+(sizeof(parNo_arr)/sizeof(parNo_arr[0])));
  vector<double> par_vec(par_arr, par_arr+(sizeof(par_arr)/sizeof(par_arr[0])));
  
  vector<double> par_vec_sub;
  
  for(unsigned int i(0); i<parNo_vec.size(); i++) {
    par_vec_sub.push_back(par_vec[parNo_vec[i]-1]);
  }
  
  TLondon1D2L fitter(parNo_vec, par_vec);

************************************************************************/

/**************** Test TLondon1D3L ********************************

  unsigned int parNo_arr[] = {1, 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
  double par_arr[] = {0.0, 999.0, 0.01, 999.0, 0.01, 999.0, 4.6, 999.0, 100.0, 5.0, 70.0, 50.0, 70.0, 180.0, 180.0, 180.0, 1.0, 1.0, 1.0};
  
  vector<unsigned int> parNo_vec(parNo_arr, parNo_arr+(sizeof(parNo_arr)/sizeof(parNo_arr[0])));
  vector<double> par_vec(par_arr, par_arr+(sizeof(par_arr)/sizeof(par_arr[0])));
  
  vector<double> par_vec_sub;
  
  for(unsigned int i(0); i<parNo_vec.size(); i++) {
    par_vec_sub.push_back(par_vec[parNo_vec[i]-1]);
  }
  
  TLondon1D3L fitter(parNo_vec, par_vec);

************************************************************************/


/**************** Test TLondon1D3LS *********************************

  unsigned int parNo_arr[] = {1, 3, 5, 7, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
  double par_arr[] = {0.0, 999.0, 0.01, 999.0, 0.01, 999.0, 21.6, 999.0, 100.0, 5.0, 70.0, 50.0, 70.0, 180.0, 500.0, 1.0, 0.3, 1.0};
  
  vector<unsigned int> parNo_vec(parNo_arr, parNo_arr+(sizeof(parNo_arr)/sizeof(parNo_arr[0])));
  vector<double> par_vec(par_arr, par_arr+(sizeof(par_arr)/sizeof(par_arr[0])));
  
  vector<double> par_vec_sub;
  
  for(unsigned int i(0); i<parNo_vec.size(); i++) {
    par_vec_sub.push_back(par_vec[parNo_vec[i]-1]);
  }
  
  TLondon1D3LS fitter(parNo_vec, par_vec);

************************************************************************/

//  ofstream of01("test_fitter01.dat");
//   ofstream of02("test_fitter02.dat");
//   ofstream of03("test_fitter03.dat");
//   ofstream of04("test_fitter04.dat");
//   ofstream of05("test_fitter05.dat");
//   ofstream of06("test_fitter06.dat");
//   ofstream of07("test_fitter07.dat");
//   ofstream of08("test_fitter08.dat");
//   ofstream of09("test_fitter09.dat");
//   ofstream of10("test_fitter10.dat");
  

//   for (double i(0.); i<12.0; i+=0.003) {
//     of01 << i << " " << fitter(i, par_vec) << endl;
//   }
//   of01.close();
  
//   par_vec[1] = 7.7;
//   
//   for (double i(0.); i<12.0; i+=0.003) {
//     of02 << i << " " << fitter(i, par_vec) << endl;
//   }
//   of02.close();
// 
//   par_vec[0] = 0.0;
//   
//   for (double i(0.); i<12.0; i+=0.003) {
//     of03 << i << " " << fitter(i, par_vec) << endl;
//   }
//   of03.close();
//   
//     par_vec[2] = 200.0;
//   
//   for (double i(0.); i<12.0; i+=0.003) {
//     of04 << i << " " << fitter(i, par_vec) << endl;
//   }
//   of04.close();
// 
//     par_vec[4] = 100.0;
//   
//   for (double i(0.); i<12.0; i+=0.003) {
//     of05 << i << " " << fitter(i, par_vec) << endl;
//   }
//   of05.close();
// 
//   par_vec[0] = 20.0;
//   par_vec[1] = 24.6;
//   par_vec[2] = 96.5;
//   par_vec[3] = 15.0;
//   par_vec[4] = 130.0;
//   
//   for (double i(0.); i<12.0; i+=0.003) {
//     of06 << i << " " << fitter(i, par_vec) << endl;
//   }
//   of06.close();
//   
//   par_vec[0] = 20.0;
//   par_vec[1] = 24.6;
//   par_vec[2] = 96.5;
//   par_vec[3] = 15.0;
//   par_vec[4] = 140.0;
//   
//   for (double i(0.); i<12.0; i+=0.003) {
//     of07 << i << " " << fitter(i, par_vec) << endl;
//   }
//   of07.close();
//   
//   par_vec[0] = 20.0;
//   par_vec[1] = 24.6;
//   par_vec[2] = 96.5;
//   par_vec[3] = 20.0;
//   par_vec[4] = 130.0;
//   
//   for (double i(0.); i<12.0; i+=0.003) {
//     of08 << i << " " << fitter(i, par_vec) << endl;
//   }
//   of08.close();
/*
    par_vec_sub[0] = 0.0;
    par_vec_sub[7] = 1000.0;
  
  for (double i(0.); i<12.0; i+=0.003) {
    of09 << i << " " << fitter.Eval(i, par_vec_sub) << endl;
  }
  of09.close();
  
    par_vec_sub[0] = 0.0;
    par_vec_sub[7] = 10.0;
  
  for (double i(0.); i<12.0; i+=0.003) {
    of10 << i << " " << fitter.Eval(i, par_vec_sub) << endl;
  }
  of10.close();
*/
/*
    vector<double> data01, data02, data03, data04, data05, data06, data07, data08, data09, data10;
    
    for (double i(0.); i<12.0; i+=0.003)
    data01.push_back(fitter.Eval(i, par_vec_sub));

  
  par_vec_sub[0] += 10.0;
  par_vec_sub[10] -= 10.0;
  
  for (double i(0.); i<12.0; i+=0.003)
    data02.push_back(fitter.Eval(i, par_vec_sub));

  
    par_vec_sub[0] += 10.0;
    par_vec_sub[10] -= 10.0;
  
  for (double i(0.); i<12.0; i+=0.003)
  data03.push_back(fitter.Eval(i, par_vec_sub));
  
    par_vec_sub[0] += 10.0;
  
  for (double i(0.); i<12.0; i+=0.003)
   data04.push_back(fitter.Eval(i, par_vec_sub));
   

  
    par_vec_sub[0] += 10.0;
    par_vec_sub[10] -= 10.0;
  
  for (double i(0.); i<12.0; i+=0.003)
    data05.push_back(fitter.Eval(i, par_vec_sub));
  
    par_vec_sub[0] += 10.0;
    par_vec_sub[10] -= 10.0;
  
  for (double i(0.); i<12.0; i+=0.003) 
    data06.push_back(fitter.Eval(i, par_vec_sub));
  
    par_vec_sub[0] += 10.0;
  
  for (double i(0.); i<12.0; i+=0.003) 
    data07.push_back(fitter.Eval(i, par_vec_sub));
  
    par_vec_sub[0] += 10.0;
    par_vec_sub[10] = 190.0;
  
  for (double i(0.); i<12.0; i+=0.003) 
    data08.push_back(fitter.Eval(i, par_vec_sub));
  
    par_vec_sub[0] += 10.0;
    par_vec_sub[10] -= 10.0;
  
  for (double i(0.); i<12.0; i+=0.003) 
    data09.push_back(fitter.Eval(i, par_vec_sub));
  
    par_vec_sub[0] += 10.0;
  
  for (double i(0.); i<12.0; i+=0.003) 
    data10.push_back(fitter.Eval(i, par_vec_sub));
  
  */
  

//  par_vec.clear();
 
  
  
  
  
  return 0;
}
