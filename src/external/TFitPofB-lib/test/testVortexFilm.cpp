#include "TPofTCalc.h"
#include "TFilmTriVortexFieldCalc.h"
#include <iostream>
#include <fstream>

using namespace std;

#include <cstdio>
#include <sys/time.h>
#include <cmath>
 
int main(){

  unsigned int NFFT(64), NFFTz(64), k(0);

  vector<float> parForVortex; 
  parForVortex.resize(4);
  
//  parForVortex[0] = 100.0; //app.field
//  parForVortex[1] = 200.0; //lambda
//  parForVortex[2] = 4.0; //xi

  vector<double> parForPofB;
  parForPofB.push_back(0.001); //dt
  parForPofB.push_back(1.0); //dB

  vector<double> parForPofT;
  parForPofT.push_back(0.0); //phase
  parForPofT.push_back(0.001); //dt
  parForPofT.push_back(1.0); //dB

  TFilmTriVortexNGLFieldCalc *vortexLattice = new TFilmTriVortexNGLFieldCalc("/home/l_wojek/analysis/WordsOfWisdomFloat.dat", NFFT, NFFTz);

  parForVortex[0] = 258.0f; //app.field
  parForVortex[1] = 10.0f; //lambda
  parForVortex[2] = 7.143f; //xi
  parForVortex[3] = 80.f; // film-thickness

  vortexLattice->SetParameters(parForVortex); 
  vortexLattice->CalculateGrid();

  char debugfile[50];
  int n;
  ofstream *of;
  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-AkReal%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetAkMatrix()[l + NFFTz*(j + NFFT*i)][0] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }
  
  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-AkImag%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetAkMatrix()[l + NFFTz*(j + NFFT*i)][1] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }

  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-BkReal%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetBkMatrix()[l + NFFTz*(j + NFFT*i)][0] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }
  
  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-BkImag%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetBkMatrix()[l + NFFTz*(j + NFFT*i)][1] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }

  n = sprintf (debugfile, "testFilmVortex-BkS%02u.dat", 0);

  if (n > 0) {
    of = new ofstream(debugfile);
    for (unsigned int i(0); i < NFFT; i++) {
      for (unsigned int j(0); j < NFFT; j++) {
        *of << vortexLattice->GetBkSMatrix()[(j + NFFT*i)] << " ";
      }
      *of << endl; 
    }
  }
  of->close();
  delete of; of = 0;


  k = 1;

  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-omega%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetOmegaMatrix()[l + NFFTz*(j + NFFT*i)] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }
  
  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-RealSpaceReal%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetRealSpaceMatrix()[l + NFFTz*(j + NFFT*i)][0] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }

  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-RealSpaceImag%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetRealSpaceMatrix()[l + NFFTz*(j + NFFT*i)][1] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }

  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-omegaDX%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetOmegaDiffMatrix()[0][l + NFFTz*(j + NFFT*i)] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }
  
  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-omegaDY%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetOmegaDiffMatrix()[1][l + NFFTz*(j + NFFT*i)] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }



  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-omegaDZ%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetOmegaDiffMatrix()[2][l + NFFTz*(j + NFFT*i)] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }

  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-Qx%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetQMatrix()[l + NFFTz*(j + NFFT*i)][0] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }
  
  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-Qy%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetQMatrix()[l + NFFTz*(j + NFFT*i)][1] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }
  
  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-Px%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetPMatrix()[l + NFFTz*(j + NFFT*i)][0] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }
  
  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-Py%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->GetPMatrix()[l + NFFTz*(j + NFFT*i)][1] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }

  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-Bx%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->DataB()[0][l + NFFTz*(j + NFFT*i)] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }

  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-By%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->DataB()[1][l + NFFTz*(j + NFFT*i)] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }
  
  for (unsigned int l = 0; l < NFFTz; ++l) {
    n = sprintf (debugfile, "testFilmVortex-Bz%02u.dat", l);

    if (n > 0) {
      of = new ofstream(debugfile);
      for (unsigned int i(0); i < NFFT; i++) {
        for (unsigned int j(0); j < NFFT; j++) {
          *of << vortexLattice->DataB()[2][l + NFFTz*(j + NFFT*i)] << " ";
        }
        *of << endl; 
      }
    }
    of->close();
    delete of; of = 0;
  }

/*
  ofstream ofx1("testVortex-omegaDY.dat");
  for (unsigned int j(0); j < NFFT * NFFT; j++) {
    ofx1 << vortexLattice->GetOmegaDiffMatrix()[j][1] << " ";
    if (!((j+1)%(NFFT)))
      ofx1 << endl;
  }
  ofx1.close();
  
  ofstream ofx2("testVortex-B.dat");
  for (unsigned int j(0); j < NFFT * NFFT; j++) {
    ofx2 << vortexLattice->GetBMatrix()[j] << " ";
    if (!((j+1)%(NFFT)))
      ofx2 << endl;
  }
  ofx2.close();
  
  ofstream ofx3("testVortex-Qx.dat");
  for (unsigned int j(0); j < NFFT * NFFT; j++) {
    ofx3 << vortexLattice->GetQMatrix()[j][0] << " ";
    if (!((j+1)%(NFFT)))
      ofx3 << endl;
  }
  ofx3.close();

  ofstream ofx4("testVortex-Qy.dat");
  for (unsigned int j(0); j < NFFT * NFFT; j++) {
    ofx4 << vortexLattice->GetQMatrix()[j][1] << " ";
    if (!((j+1)%(NFFT)))
      ofx4 << endl;
  }
  ofx4.close(); 
*/
//   ofstream ofx5("testVortex-TempReal.dat");
//   for (unsigned int j(0); j < NFFT * NFFT; j++) {
//     ofx5 << vortexLattice->GetTempMatrix()[j][0] << " ";
//     if (!((j+1)%(NFFT)))
//       ofx5 << endl;
//   }
//   ofx5.close();
// 
//   ofstream ofx6("testVortex-TempImag.dat");
//   for (unsigned int j(0); j < NFFT * NFFT; j++) {
//     ofx6 << vortexLattice->GetTempMatrix()[j][1] << " ";
//     if (!((j+1)%(NFFT)))
//       ofx6 << endl;
//   }
//   ofx6.close(); 
/*
  ofstream ofz("testVortex-AReal.dat");
  for (unsigned int j(0); j < (NFFT/2 + 1) * NFFT; j++) {
    ofz << vortexLattice->GetAkMatrix()[j][0] << " ";
    if (!((j+1)%(NFFT/2 + 1)))
      ofz << endl;
  }
  ofz.close();
 
  ofstream ofz1("testVortex-AImag.dat");
  for (unsigned int j(0); j < (NFFT/2 + 1) * NFFT; j++) {
    ofz1 << vortexLattice->GetAkMatrix()[j][1] << " ";
    if (!((j+1)%(NFFT/2 + 1)))
      ofz1 << endl;
  }
  ofz1.close();
  
   ofstream ofz2("testVortex-BReal.dat");
  for (unsigned int j(0); j < NFFT * NFFT; j++) {
    ofz2 << vortexLattice->GetBkMatrix()[j][0] << " ";
    if (!((j+1)%(NFFT)))
      ofz2 << endl;
  }
  ofz2.close();
  
  ofstream ofz3("testVortex-BImag.dat");
  for (unsigned int j(0); j < NFFT * NFFT; j++) {
    ofz3 << vortexLattice->GetBkMatrix()[j][1] << " ";
    if (!((j+1)%(NFFT)))
      ofz3 << endl;
  }
  ofz3.close();

  TPofBCalc *PofB = new TPofBCalc(parForPofB);
  PofB->Calculate(vortexLattice, parForPofB);

  const double *b(PofB->DataB());
  double *pb(PofB->DataPB());
  unsigned int s(PofB->GetPBSize());

  double test(0.0);

  ofstream ofxx("testVortex.dat");
  for (unsigned int i(0); i < s; i++) {
    ofxx << b[i] << " " << pb[i] << endl;
    test+=pb[i];
  }
  ofxx.close();
  
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
*/
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
//   delete PofB;
//   PofB = 0;

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
