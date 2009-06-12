#include "TLondon1D.h"
#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;

int main(){
  string rge_path("/home/l_wojek/TrimSP/YBCOxtal/YBCOxtal-500000-");
  string energy_arr[] = {"03_0", "03_6", "05_0", "05_3", "07_0", "07_7", "08_0", "09_0", "10_0", "10_2", "12_0", "14_1", "16_0", "16_4", "18_0", "19_7", "20_0", "22_0", "24_0", "24_6"};
    
  double E(24.0); 
  
  vector<string> energy_vec(energy_arr, energy_arr+(sizeof(energy_arr)/sizeof(energy_arr[0])));

  TTrimSPData calcData(rge_path, energy_vec);
  
/*  vector<double> energies(calcData.Energy());
  for (unsigned int i(0); i<energies.size(); i++)
    cout << energies[i] << endl;
*/
  calcData.Normalize(E);

  vector<double> z(calcData.DataZ(E));
  vector<double> nz(calcData.DataNZ(E));

  ofstream of("Implantation-profile-normal.dat");
  for (unsigned int i(0); i<z.size(); i++) {
    of << z[i] << " " << nz[i] << endl;
  }
  of.close();
   
  double par_arr[] = {24.4974, E, 95.8253, 0.0, 45.0, 45.0, 45.0, 200.0, 400.0, 200.0};
  vector<double> par_vec(par_arr, par_arr+(sizeof(par_arr)/sizeof(par_arr[0])));
  
  vector<double> interfaces;
  interfaces.push_back(par_vec[3]+par_vec[4]);
  interfaces.push_back(par_vec[3]+par_vec[4]+par_vec[5]);
  interfaces.push_back(par_vec[3]+par_vec[4]+par_vec[5]+par_vec[6]);
  
  vector<double> parForBofZ;
  for (unsigned int i(2); i<par_vec.size(); i++)
    parForBofZ.push_back(par_vec[i]);

  vector<double> parForPofB;
  parForPofB.push_back(0.01); //dt
  parForPofB.push_back(0.05); //dB
  parForPofB.push_back(par_vec[1]);

  vector<double> parForPofT;
  parForPofT.push_back(par_vec[0]); //phase
  parForPofT.push_back(0.01); //dt
  parForPofT.push_back(0.05); //dB

  TLondon1D_3L BofZ(5000, parForBofZ);

  vector<double> zz;
  vector<double> Bz;
  for(double i(0.5); i<2001; i+=1.0) {
    zz.push_back(i/10.0);
    if(!(i/10.0 < parForBofZ[1]) && !(i/10.0 > parForBofZ[1]+parForBofZ[2]+parForBofZ[3]+parForBofZ[4])) {
      Bz.push_back(BofZ.GetBofZ(i/10.0));
    }
    else {
      Bz.push_back(parForBofZ[0]);
    }
  }
  
  char debugfile1[50];
  int nn = sprintf (debugfile1, "4Ltest-Bz_z-%.4f_l-%.3f_E-%.1f_normal.dat", par_vec[2], par_vec[9], par_vec[1]);
  if (nn > 0) {
    ofstream of01(debugfile1);
    for (unsigned int i(0); i<2000; i++) {
      of01 << zz[i] << " " << Bz[i] << endl;
    }
    of01.close();
  }

  TPofBCalc PofB(BofZ, calcData, parForPofB);
//  PofB.ConvolveGss(1.17);

  PofB.AddBackground(par_vec[2], 0.2, calcData.LayerFraction(E, 4, interfaces));
  PofB.ConvolveGss(1.17);

  vector<double> hurgaB(PofB.DataB());
  vector<double> hurgaPB(PofB.DataPB());
  
  char debugfile[50];
  int n = sprintf (debugfile, "4Ltest-BpB_B-%.4f_l-%.3f_E-%.1f_normal.dat", par_vec[2], par_vec[9], par_vec[1]);

  if (n > 0) {
    ofstream of7(debugfile);
    for (unsigned int i(0); i<hurgaB.size(); i++) {
      of7 << hurgaB[i] << " " << hurgaPB[i] << endl;
    }
    of7.close();
  }
  
  TPofTCalc poft("/home/l_wojek/analysis/WordsOfWisdom.dat", parForPofT);
  
  poft.DoFFT(PofB);
  poft.CalcPol(parForPofT);
  
  char debugfilex[50];
  int nx = sprintf (debugfilex, "4Ltest-P_t-%.4f_l-%.3f_E-%.1f_normal.dat", par_vec[2], par_vec[9], par_vec[1]);
  
  if (nx > 0) { 
    ofstream of8(debugfilex);
    for (double i(0.); i<12.0; i+=0.003) {
      of8 << i << " " << poft.Eval(i) << endl;
    }
    of8.close();
  }
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
 
  
  
  
  
  return EXIT_SUCCESS;
}
