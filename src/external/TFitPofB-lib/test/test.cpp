#include "TFitPofB.h"
#include <iostream>
#include <fstream>

using namespace std;

int main(){
/*  string rge_path("/home/l_wojek/nt/wojek/g/Bastian/ImplantationDepth/YBCO_PBCO-");
  string energy_arr[] = {"02_1", "02_5", "03_5", "05_0", "07_5", "10_0", "12_5", "15_0", "17_5", "19_0", "20_0", "22_5", "25_0"};
  
  vector<string> energy_vec(energy_arr, energy_arr+(sizeof(energy_arr)/sizeof(energy_arr[0])));

  TTrimSPData calcData(rge_path, energy_vec);
  
  vector<double> energies(calcData.Energy());
  for (unsigned int i(0); i<energies.size(); i++)
    cout << energies[i] << endl;
  
  vector<double> z(calcData.DataZ(2.5));
  vector<double> nz(calcData.DataNZ(2.5));
  
  vector<double> z2(calcData.DataZ(25.0));
  vector<double> nz2(calcData.DataNZ(25.0));
  
  ofstream of("test_out1.dat");
  for (unsigned int i(0); i<z.size(); i++) {
    of << z[i] << " " << nz[i] << endl;
  }
  of.close();
  
  ofstream of2("test_out2.dat");
  for (unsigned int i(0); i<z2.size(); i++) {
    of2 << z2[i] << " " << nz2[i] << endl;
  }
  of2.close();
  
  ofstream of3("test_out4.dat");
  for (unsigned int i(0); i<z.size(); i++) {
    of3 << i << " " << calcData.GetNofZ(double(i), 25.0) << endl;
  }
  of3.close();
  
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
  unsigned int parNo_arr[] = {1, 3, 5, 7, 9, 11, 12, 13, 14, 15, 16, 17};
  double par_arr[] = {3.0, 999.0, 0.0, 999.0, 0.01, 999.0, 0.05, 999.0, 20.0, 999.0, 100.0, 10.0, 80.0, 50.0, 80.0, 180.0, 500.0};
  
  vector<unsigned int> parNo_vec(parNo_arr, parNo_arr+(sizeof(parNo_arr)/sizeof(parNo_arr[0])));
  vector<double> par_vec(par_arr, par_arr+(sizeof(par_arr)/sizeof(par_arr[0])));
  
  vector<double> par_vec_sub;
  
  for(unsigned int i(0); i<parNo_vec.size(); i++) {
    par_vec_sub.push_back(par_vec[parNo_vec[i]-1]);
  }
  
  TFitPofB fitter(parNo_vec, par_vec);

  ofstream of01("test_fitter01.dat");
  ofstream of02("test_fitter02.dat");
  ofstream of03("test_fitter03.dat");
  ofstream of04("test_fitter04.dat");
  ofstream of05("test_fitter05.dat");
  ofstream of06("test_fitter06.dat");
  ofstream of07("test_fitter07.dat");
  ofstream of08("test_fitter08.dat");
  ofstream of09("test_fitter09.dat");
  ofstream of10("test_fitter10.dat");
  

  for (double i(0.); i<12.0; i+=0.003) {
    of01 << i << " " << fitter.Eval(i, par_vec_sub) << endl;
  }
  of01.close();
  
  par_vec_sub[1] += 10.0;
  par_vec_sub[11] -= 20.0;
  
  for (double i(0.); i<12.0; i+=0.003) {
    of02 << i << " " << fitter.Eval(i, par_vec_sub) << endl;
  }
  of02.close();
  
    par_vec_sub[1] += 10.0;
    par_vec_sub[11] -= 20.0;
  
  for (double i(0.); i<12.0; i+=0.003) {
    of03 << i << " " << fitter.Eval(i, par_vec_sub) << endl;
  }
  of03.close();
  
    par_vec_sub[1] += 10.0;
  
  for (double i(0.); i<12.0; i+=0.003) {
    of04 << i << " " << fitter.Eval(i, par_vec_sub) << endl;
  }
  of04.close();
  
    par_vec_sub[1] += 10.0;
    par_vec_sub[11] -= 20.0;
  
  for (double i(0.); i<12.0; i+=0.003) {
    of05 << i << " " << fitter.Eval(i, par_vec_sub) << endl;
  }
  of05.close();
  
    par_vec_sub[1] += 10.0;
    par_vec_sub[11] -= 20.0;
  
  for (double i(0.); i<12.0; i+=0.003) {
    of06 << i << " " << fitter.Eval(i, par_vec_sub) << endl;
  }
  of06.close();
  
    par_vec_sub[1] += 10.0;
  
  for (double i(0.); i<12.0; i+=0.003) {
    of07 << i << " " << fitter.Eval(i, par_vec_sub) << endl;
  }
  of07.close();
  
    par_vec_sub[1] += 10.0;
    par_vec_sub[7] = 75.0;
  
  for (double i(0.); i<12.0; i+=0.003) {
    of08 << i << " " << fitter.Eval(i, par_vec_sub) << endl;
  }
  of08.close();
  
    par_vec_sub[1] += 10.0;
    par_vec_sub[11] -= 20.0;
  
  for (double i(0.); i<12.0; i+=0.003) {
    of09 << i << " " << fitter.Eval(i, par_vec_sub) << endl;
  }
  of09.close();
  
    par_vec_sub[1] += 10.0;
  
  for (double i(0.); i<12.0; i+=0.003) {
    of10 << i << " " << fitter.Eval(i, par_vec_sub) << endl;
  }
  of10.close();

  
/*    vector<double> data01, data02, data03, data04, data05, data06, data07, data08, data09, data10;
    
    for (double i(0.); i<12.0; i+=0.003)
    data01.push_back(fitter.Eval(i, par_vec_sub));

  
  par_vec_sub[1] += 10.0;
  par_vec_sub[8] -= 10.0;
  
  for (double i(0.); i<12.0; i+=0.003)
    data02.push_back(fitter.Eval(i, par_vec_sub));

  
    par_vec_sub[1] += 10.0;
    par_vec_sub[8] -= 10.0;
  
  for (double i(0.); i<12.0; i+=0.003)
  data03.push_back(fitter.Eval(i, par_vec_sub));
  
    par_vec_sub[1] += 10.0;
  
  for (double i(0.); i<12.0; i+=0.003)
   data04.push_back(fitter.Eval(i, par_vec_sub));
   

  
    par_vec_sub[1] += 10.0;
    par_vec_sub[8] -= 10.0;
  
  for (double i(0.); i<12.0; i+=0.003)
    data05.push_back(fitter.Eval(i, par_vec_sub));
  
    par_vec_sub[1] += 10.0;
    par_vec_sub[8] -= 10.0;
  
  for (double i(0.); i<12.0; i+=0.003) 
    data06.push_back(fitter.Eval(i, par_vec_sub));
  
    par_vec_sub[1] += 10.0;
  
  for (double i(0.); i<12.0; i+=0.003) 
    data07.push_back(fitter.Eval(i, par_vec_sub));
  
    par_vec_sub[1] += 10.0;
    par_vec_sub[7] = 190.0;
  
  for (double i(0.); i<12.0; i+=0.003) 
    data08.push_back(fitter.Eval(i, par_vec_sub));
  
    par_vec_sub[1] += 10.0;
    par_vec_sub[8] -= 10.0;
  
  for (double i(0.); i<12.0; i+=0.003) 
    data09.push_back(fitter.Eval(i, par_vec_sub));
  
    par_vec_sub[1] += 10.0;
  
  for (double i(0.); i<12.0; i+=0.003) 
    data10.push_back(fitter.Eval(i, par_vec_sub));
  
  */
  
  parNo_vec.clear();
  par_vec.clear();
  par_vec_sub.clear();
  
  
  
  
  
  return EXIT_SUCCESS;
}
