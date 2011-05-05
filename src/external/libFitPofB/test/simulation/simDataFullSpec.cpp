/***************************************************************************

  simDataFullSpec.cpp

  Author: Bastian M. Wojek

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2011 by Bastian M. Wojek                                *
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

#include "TFile.h"
#include "TH1.h"
#include "TMath.h"
#include "TFolder.h"

#include "TBofZCalc.h"
#include "TPofTCalc.h"
#include "TLemRunHeader.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

#include <boost/lexical_cast.hpp>

int main(int argc, char *argv[]) {
      // check the number of arguments: run number, energy (keV), field (G), lambda (nm), dead layer (nm), broadening (G), asymmetry[, sigmaLambda (lambda)]
      if (argc < 8) {
        cerr << "Please give all necessary parameters: run number, energy (keV), field (G), lambda (nm), dead layer (nm), broadening (G), asymmetry[, sigmaLambda (lambda)]" << endl;
        return 0;
      }

      // fill the specified paramters into the corresponding variables
      unsigned int runNumber;
      double impEnergy;
      double field;
      double lambda;
      double deadlayer;
      double broadening;
      double asymmetry;
      double sigmaLambda(0.0);
      try {
        runNumber = boost::lexical_cast<unsigned int>(argv[1]);
        impEnergy = boost::lexical_cast<double>(argv[2]);
        field = boost::lexical_cast<double>(argv[3]);
        lambda = boost::lexical_cast<double>(argv[4]);
        deadlayer = boost::lexical_cast<double>(argv[5]);
        broadening = boost::lexical_cast<double>(argv[6]);
        asymmetry = boost::lexical_cast<double>(argv[7]);
        if (argc == 9) {
          sigmaLambda = boost::lexical_cast<double>(argv[8])*lambda;
        }
      }
      catch(boost::bad_lexical_cast &) {
        cerr << endl;
        cerr << "Please specify parameters with a meaning:" << endl;
        cerr << "run number, energy (keV), field (G), lambda (nm), dead layer (nm), broadening (G), asymmetry" << endl;
        return -1;
      }

      // Read in the root file containing the transport energy spectrum

      string fileName("lem08_1351_ImpSpectrum_15kV.root");

      TFile *rootFile = new TFile(fileName.c_str());

      if (rootFile != NULL) {
        cout << "Opened root file: " << fileName << endl;
      } else {
        cout << "Could not open root file: " << fileName << endl;
        return 1;
      }

      TH1F* impSpec = static_cast<TH1F*>(rootFile->Get("fEnergyHisto"));

      map<double, pair<double, int> > spectrum; // E, (weight, t0shift)

      const double dE(0.1);
      const double peakEnergy(11.25);
      const double peakEnergyTrans(14.1);
      const double peakEnergyFinal(impEnergy);
      double eDiff(peakEnergyTrans-peakEnergy);
      double eDiffFinal(peakEnergyFinal-peakEnergyTrans);

      const double muMass(105658.370); // keV/c^2
      const double speedOfLight(299.792458); // mm/ns
      const double driftLength(1160.); // mm
      const double res(0.1953125); // ns
      double coeff0(driftLength/(speedOfLight*res));

      pair<double, int> tmpPair;
      unsigned int peakBin; // see lem-svn:analysis/root/macros/lemStart.C for the conversion between Etrans and TOF
      peakBin = static_cast<unsigned int>(floor(coeff0/sqrt(1.0-1.0/pow(peakEnergy/muMass + 1.0, 2.0))));

      for (double E(5.95); E <= 15.0; E += dE) {
//         // Debug: use only the peak energy
//         if (fabs(E - peakEnergy) > 0.01)
//           continue;
        if (peakEnergyTrans - (E+eDiff) >= peakEnergyFinal)
          continue; // forget about the reflected muons with too low energies in the tail
        tmpPair.first = impSpec->GetBinContent(impSpec->FindBin(E));
        tmpPair.second = static_cast<int>(floor(coeff0/sqrt(1.0-1.0/pow(E/muMass + 1.0, 2.0)))) - peakBin; // t-shift relativ to peak
        spectrum[E+eDiff] = tmpPair;
      }

      // normalize spectrum
      double tmpNorm(0.0);
      for (map<double, pair<double, int> >::const_iterator it(spectrum.begin()); it != spectrum.end(); ++it) {
        tmpNorm += it->second.first;
      }
      for (map<double, pair<double, int> >::iterator it(spectrum.begin()); it != spectrum.end(); ++it) {
        it->second.first /= tmpNorm;
      }

      // calculate phaseMag on the fly when the histogram is filled
      // in order not to rewrite all the FakeData-routine, use it as implemented in TPofTCalc to create separate files for each energy
      // in the end, add the histograms of the files together to create the resulting simulated histograms

      // read in the TrimSP files
      string rge_path("/home/l_wojek/TrimSP/YBCOxtal/Espread-40eV/YBCOxtal-50000-");
      vector<string> energy_labels;
      ostringstream tmpLabel;
      for (unsigned int i(0); i < 30; ++i) {
        tmpLabel.clear();
        tmpLabel.str("");
        tmpLabel.fill('0');
        tmpLabel.setf(ios::internal, ios::adjustfield);
        tmpLabel.width(2);
        tmpLabel << i;
        tmpLabel << "_";
        for (unsigned int j(0); j < 10; ++j) {
          if (!((i == 0) && (j == 0))) {
            energy_labels.push_back(tmpLabel.str());
            energy_labels.back().append(boost::lexical_cast<string>(j));
          }
        }
      }
      energy_labels.push_back("30_0");

      map<double,string> energy_vec;

      for (unsigned int i(0); i < energy_labels.size(); ++i) {
        energy_vec[static_cast<double>(i+1)*0.1] = energy_labels[i];
      }

      TTrimSPData calcData(rge_path, energy_vec, true);

      // parameters for the London model calculations

      vector<double> par_vec;
      par_vec.resize(5, 0.0); // phase (deg), energy (keV), applied field (G), deadlayer (nm), lambda (nm)
      par_vec[2] = field;
      par_vec[3] = deadlayer;
      par_vec[4] = lambda;

      vector<double> interfaces;
        interfaces.push_back(par_vec[3]); // deadlayer

      vector<double> parForBofZ;
      for (unsigned int i(2); i<par_vec.size(); ++i)
        parForBofZ.push_back(par_vec[i]);

      vector<double> parForPofB;
      parForPofB.resize(6, 0.0); // dt (us), dB (G), energy (keV), bkg-field (G), bkg-width (G), bkg-fraction (here from deadlayer)
      parForPofB[0] = 0.005; // dt
      parForPofB[1] = 0.05; // dB
      parForPofB[3] = field; // bkg-field
      parForPofB[4] = 0.1; // bkg-width

      vector<double> parForPofT;
      parForPofT.resize(3, 0.0); // phase, dt, dB
      parForPofT[1] = 0.005; //dt
      parForPofT[2] = 0.05; //dB

      vector<double> parForFakeData; // par(dt, dB, timeres, channels, asyms, phases, t0s, N0s, bgs)
      parForFakeData.resize(14, 0.0);
      parForFakeData[0] = 0.005;//dt for FFT
      parForFakeData[1] = 0.05;//dB
      parForFakeData[2] = res*1.e-3;//timeres
      parForFakeData[3] = 66601.;//channels
      parForFakeData[4] = asymmetry;//asys -- fixed for the moment but in principle also energy-dependent
      parForFakeData[5] = asymmetry;

      const double N0L(115.), N0R(100.), bgL(9.), bgR(8.), t0Peak(3000.);

      TLondon1D_HS *BofZ = new TLondon1D_HS(parForBofZ);
      TPofBCalc *PofB = new TPofBCalc(parForPofB);
      TPofTCalc *PofT = new TPofTCalc(PofB, "/home/l_wojek/analysis/WordsOfWisdom.dat", parForPofT);

      double tmpE, tmpTransportE, tmpPhase;
      ostringstream tmpFile;
      ostringstream tmpDouble;

      const double phaseApp(16.0); // phaseSep ~ 9 deg, phaseCryo ~ 7 deg
      const double gamma_mu(TMath::TwoPi()*0.0135538817); // MHz/G
      double coeff1(1.30406); // m/s -> for v = sqrt(2E/m) = 1.30406e6 m/s sqrt(E[keV]) no "e6" because gamma_mu is in MHz/G
      double coeff2(0.13123); // m -> for phaseMag = gamma_mu/v Int {B(x) dx} = coeff2 * B * gamma_mu/v
      double coeff3(gamma_mu*field*coeff2/coeff1);

      map<int, double> GssWeight;
      if (sigmaLambda) { // If Gaussian spread of lambdas around the specified value
        for (int i(1); i < 16; ++i) {
          GssWeight[i] = GssWeight[-i] = exp(-0.02*static_cast<double>(i)*static_cast<double>(i));
        }
        GssWeight[0] = 1.0;
        // Normalize
        double tmpSum(0.0);
        for (map<int, double>::const_iterator it(GssWeight.begin()); it != GssWeight.end(); ++it) {
          tmpSum += it->second;
        }
        for (map<int, double>::iterator it(GssWeight.begin()); it != GssWeight.end(); ++it) {
          it->second /= tmpSum;
        }
      }

      unsigned int index(0);
      for (map<double, pair<double, int> >::const_iterator it(spectrum.begin()); it != spectrum.end(); ++it) {

        tmpTransportE = it->first;
        tmpE = eDiffFinal + tmpTransportE;

        cout << "tmpTransportE = " << tmpTransportE << ", tmpE = " << tmpE << endl;

        tmpPhase = coeff3/sqrt(tmpTransportE)*180.0/TMath::Pi() + phaseApp;

        parForPofB[2] = tmpE;
        parForPofB[5] = calcData.LayerFraction(tmpE, 1, interfaces); // Fraction of muons in the deadlayer 
                                                                     // replace this expression by 1.0 if you are interested only in the background (reference above Tc)
  
        parForPofT[0] = tmpPhase;
  
        parForFakeData[6] = tmpPhase;//phases
        parForFakeData[7] = parForFakeData[6] + 165.;
        parForFakeData[8] = t0Peak + spectrum[tmpTransportE].second;//t0s
        parForFakeData[9] = parForFakeData[8];
        parForFakeData[10] = N0L*spectrum[tmpTransportE].first;//N0s
        parForFakeData[11] = N0R*spectrum[tmpTransportE].first;
        parForFakeData[12] = bgL*spectrum[tmpTransportE].first;//bgs
        parForFakeData[13] = bgR*spectrum[tmpTransportE].first;

        if (GssWeight.empty()) {
          PofB->UnsetPBExists();
          PofB->Calculate(BofZ, &calcData, parForPofB);
        } else {
          // calculate P(B) for the Gaussian distribution of lambdas and finally add them together
          vector<double> *pb = 0;
          double *tmpPB;

          for (map<int, double>::iterator it(GssWeight.begin()); it != GssWeight.end(); ++it) {
            delete BofZ;
            parForBofZ.back() = lambda + 0.2*it->first*sigmaLambda;
            BofZ = new TLondon1D_HS(parForBofZ);
            PofB->UnsetPBExists();
            PofB->Calculate(BofZ, &calcData, parForPofB);

            if (it == GssWeight.begin()) {
              pb = new vector<double>(PofB->DataPB(), PofB->DataPB() + PofB->GetPBSize());
              for (unsigned int i(0); i < pb->size(); ++i) {
                (*pb)[i] *= it->second;
              }
            } else if (pb) {
              tmpPB = PofB->DataPB();
              for (unsigned int i(0); i < pb->size(); ++i) {
                (*pb)[i] += tmpPB[i]*it->second;
              }
            }
          }
          PofB->SetPB(*pb);
          delete pb;
          pb = 0;
        }

        PofB->ConvolveGss(broadening);

//         // debug: check the calculated field distribution
//         const double *b(PofB->DataB());
//         double *pB(PofB->DataPB());
//         unsigned int s(PofB->GetPBSize());
// 
//         ofstream ofx("test-pB.dat");
//         for (unsigned int i(0); i < s; ++i) {
//           ofx << b[i] << " " << pB[i] << endl;
//         }
//         ofx.close();
//         return 100;

        tmpFile.clear();
        tmpFile.str("");
        tmpFile.fill('0');
        tmpFile.setf(ios::internal, ios::adjustfield);
        tmpFile.width(2);
        tmpFile << index;
        tmpFile << "tempFile.root";
  
        PofT->DoFFT();
        PofT->FakeData(tmpFile.str(), parForFakeData, 0);

        ++index;
      }

      delete BofZ;
      BofZ = 0;
      delete PofB;
      PofB = 0;
      delete PofT;
      PofT = 0;

      // read back the written files and add the decay histograms

      // get the first histogram, copy it and add the rest of them
      vector<TFile*> rootTmpFile;
      rootTmpFile.push_back(new TFile("00tempFile.root"));

      TFolder *tmpFolder;
      rootTmpFile.back()->GetObject("histos", tmpFolder);
      if (!tmpFolder) {
        cerr << endl << "No histogram folder found in 00tempFile.root" << endl;
        rootTmpFile.back()->Close();
        return -1;
      }

      TH1F *tmpHist = dynamic_cast<TH1F*>(tmpFolder->FindObjectAny("hDecay00"));

      vector<TH1F*> finalHistos;
      finalHistos.push_back(dynamic_cast<TH1F*>(tmpHist->Clone()));



      tmpHist = dynamic_cast<TH1F*>(tmpFolder->FindObjectAny("hDecay01"));
      finalHistos.push_back(dynamic_cast<TH1F*>(tmpHist->Clone()));


//      rootTmpFile->Close();

//      delete rootTmpFile;

      

//      ostringstream tmpStrStream;
//      tmpStrStream << "hDecay0";
//      string tmpString;

      for (unsigned int i(1); i < index; ++i) {
        tmpFile.clear();
        tmpFile.str("");
        tmpFile.fill('0');
        tmpFile.setf(ios::internal, ios::adjustfield);
        tmpFile.width(2);
        tmpFile << i;
        tmpFile << "tempFile.root";

        rootTmpFile.push_back(new TFile(tmpFile.str().c_str()));

        rootTmpFile.back()->GetObject("histos", tmpFolder);
        if (!tmpFolder) {
          cerr << endl << "No histogram folder found in " << tmpFile.str() << endl;
          rootTmpFile.back()->Close();
          return -1;
        }

        for(unsigned int j(0); j < finalHistos.size(); ++j) {
//          tmpString = tmpStrStream.str().append(boost::lexical_cast<string>(j));
          tmpHist = dynamic_cast<TH1F*>(tmpFolder->FindObjectAny(finalHistos[j]->GetName()));
          finalHistos[j]->Add(tmpHist);
        }

//        rootTmpFile->Close();
//        delete rootTmpFile;
      }

      // save everything to the final ROOT file
      // create run info folder and content
      TFolder *runInfoFolder = new TFolder("RunInfo", "Run Info");
      TLemRunHeader *runHeader = new TLemRunHeader();

      runHeader->SetRunTitle("Simulated Data");
      runHeader->SetImpEnergy(peakEnergyFinal);
      runHeader->SetSampleBField(field, 0.0f);

      runHeader->SetTimeResolution(res);
      runHeader->SetNChannels(66601);
      runHeader->SetNHist(finalHistos.size());
      double *t0array = new double[finalHistos.size()];
      for (unsigned int i(0); i<finalHistos.size(); ++i)
        t0array[i] = t0Peak;
      runHeader->SetTimeZero(t0array);
      if (t0array) {
        delete[] t0array;
        t0array = 0;
      }
      runInfoFolder->Add(runHeader);

      // create decay histo folder and content
      TFolder *histoFolder = new TFolder("histos", "histos");
      TFolder *decayAnaModule = new TFolder("DecayAnaModule", "DecayAnaModule");
      histoFolder->Add(decayAnaModule);
      // non post pileup corrected (NPP)
      for (unsigned int i(0); i<finalHistos.size(); ++i)
        decayAnaModule->Add(finalHistos[i]);
      // post pileup corrected (PPC)
      vector<TH1F*> finalHistosPPC;
      TString name;
      for (unsigned int i(0); i<finalHistos.size(); ++i) {
        finalHistosPPC.push_back(dynamic_cast<TH1F*>(finalHistos[i]->Clone()));
        if (i < 10)
          name   = "hDecay2";
        else
          name   = "hDecay";
        name  += i;
        finalHistosPPC[i]->SetNameTitle(name.Data(), name.Data());
        decayAnaModule->Add(finalHistosPPC[i]);
      }

      // write file
      tmpFile.clear();
      tmpFile.str("");
      tmpFile << "LEMsimulation-YBCO-London-lambda" << fixed << setprecision(0) << par_vec[4] << "nm-dl" << fixed << setprecision(0) << par_vec[3] << "nm_";
      tmpFile.fill('0');
      tmpFile.setf(ios::internal, ios::adjustfield);
      tmpFile.width(4);
      tmpFile << runNumber;
      tmpFile << ".root";

      TFile fdf(tmpFile.str().c_str(), "recreate");
      runInfoFolder->Write("RunInfo", TObject::kSingleKey);
      histoFolder->Write();
      fdf.Close();

      // clean up

      for (unsigned int i(0); i<finalHistos.size(); ++i) {
        delete finalHistos[i];
        delete finalHistosPPC[i];
      }
      finalHistos.clear();
      finalHistosPPC.clear();

      for (unsigned int i(0); i<rootTmpFile.size(); ++i) {
        rootTmpFile[i]->Close();
        delete rootTmpFile[i];
      }
      rootTmpFile.clear();

      histoFolder->Clear();
      delete histoFolder; histoFolder = 0;
      decayAnaModule->Clear();
      delete decayAnaModule; decayAnaModule = 0;

      runInfoFolder->Clear();
      delete runInfoFolder; runInfoFolder = 0;
      delete runHeader; runHeader = 0;


//       // debug: check read in file and normalization
//       tmpNorm = 0.0;
//       for (map<double, pair<double, int> >::const_iterator it(spectrum.begin()); it != spectrum.end(); ++it) {
//         tmpNorm += it->second.first;
//       }
//       cout << "E normalizer: " << tmpNorm << endl;
//       ofstream of0("test-spec.dat");
//       for (map<double, pair<double, int> >::const_iterator it(spectrum.begin()); it != spectrum.end(); ++it) {
//         of0 << it->first << " " << it->second.first << " " << it->second.second << " " << coeff3/sqrt(it->first)*180.0/TMath::Pi() << endl;
//       }
//       of0.close();

      rootFile->Close();
      delete rootFile;
      rootFile = 0;
      impSpec = 0;
      spectrum.clear();

      return 0;
}
