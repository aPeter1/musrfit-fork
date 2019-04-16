/***************************************************************************

  TCalcMeanFieldsLEM.cpp

  Author: Bastian M. Wojek

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

#include <cassert>
#include <algorithm>
#include <functional>
#include <string>
#include <iostream>
#include <fstream>

#include <TSAXParser.h>
#include "BMWStartupHandler.h"

#include "TCalcMeanFieldsLEM.h"

ClassImp(TMeanFieldsForScHalfSpace)
ClassImp(TMeanFieldsForScSingleLayer)
ClassImp(TMeanFieldsForScBilayer)
ClassImp(TMeanFieldsForScTrilayer)

// Constructor: Read the energies from the xml-file and load the according TRIM.SP-data files

TMeanFieldsForScHalfSpace::TMeanFieldsForScHalfSpace() {

  // read startup file
  std::string startup_path_name("BMW_startup.xml");

  TSAXParser *saxParser = new TSAXParser();
  BMWStartupHandler *startupHandler = new BMWStartupHandler();
  saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
  int status (saxParser->ParseFile(startup_path_name.c_str()));
  // check for parse errors
  if (status) { // error
    cerr << endl << "**ERROR** reading/parsing " << startup_path_name << " failed." \
         << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
         << endl;
    assert(false);
  }

  std::string rge_path(startupHandler->GetDataPath());
  map<double, std::string> energy_vec(startupHandler->GetEnergies());

  fImpProfile = new TTrimSPData(rge_path, energy_vec, startupHandler->GetDebug());

  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }

}

// Operator-method that returns the mean field for a given implantation energy
// Parameters: field, deadlayer, lambda

double TMeanFieldsForScHalfSpace::operator()(double E, const std::vector<double> &par_vec) const{

  // Calculate field profile
  std::vector<double> parForBofZ(par_vec);

  TLondon1D_HS BofZ(parForBofZ);

  std::vector<double> energies(fImpProfile->Energy());
  std::vector<double>::const_iterator energyIter;
  energyIter = find(energies.begin(), energies.end(), E);

  if (energyIter != energies.end()) { // implantation profile found - no interpolation needed
    return CalcMeanB(E, BofZ);
  } else {
    if (E < energies.front())
      return CalcMeanB(energies.front(), BofZ);
    if (E > energies.back())
      return CalcMeanB(energies.back(), BofZ);

    energyIter = find_if(energies.begin(), energies.end(), bind2nd( greater<double>(), E));
//    cout << *(energyIter - 1) << " " << *(energyIter) << endl;

    double E1(*(energyIter - 1));
    double E2(*(energyIter));

    double B1(CalcMeanB(E1, BofZ));
    double B2(CalcMeanB(E2, BofZ));

    return B1 + (B2-B1)/(E2-E1)*(E-E1);
  }

}

double TMeanFieldsForScHalfSpace::CalcMeanB (double E, const TLondon1D_HS& BofZ) const {
    //calcData->UseHighResolution(E);

    fImpProfile->Normalize(E);

    std::vector<double> z(fImpProfile->DataZ(E));
    std::vector<double> nz(fImpProfile->DataNZ(E));
    double dz(fImpProfile->DataDZ(E));

    // calculate mean field

    double meanB(0.);

    for (unsigned int i(0); i<z.size(); i++) {
      meanB += dz*nz[i]*BofZ.GetBofZ(z[i]/10.);
    }
    return meanB;
}

// Constructor: Read the energies from the xml-file and load the according TRIM.SP-data files

TMeanFieldsForScSingleLayer::TMeanFieldsForScSingleLayer() {

  // read startup file
  std::string startup_path_name("BMW_startup.xml");

  TSAXParser *saxParser = new TSAXParser();
  BMWStartupHandler *startupHandler = new BMWStartupHandler();
  saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
  int status (saxParser->ParseFile(startup_path_name.c_str()));
  // check for parse errors
  if (status) { // error
    cerr << endl << "**ERROR** reading/parsing " << startup_path_name << " failed." \
         << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
         << endl;
    assert(false);
  }

  std::string rge_path(startupHandler->GetDataPath());
  map<double, std::string> energy_vec(startupHandler->GetEnergies());

  fImpProfile = new TTrimSPData(rge_path, energy_vec, startupHandler->GetDebug());

  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }

}

// Operator-method that returns the mean field for a given implantation energy
// Parameters: field, deadlayer, thicknessSC, lambda, weight (deadlayer), weight (SC), weight (substrate)

double TMeanFieldsForScSingleLayer::operator()(double E, const std::vector<double> &par_vec) const{

  std::vector<double> interfaces;
  interfaces.push_back(par_vec[1]);
  interfaces.push_back(par_vec[1]+par_vec[2]);

  std::vector<double> weights;
  weights.push_back(par_vec[4]);
  weights.push_back(par_vec[5]);
  weights.push_back(par_vec[6]);

  // Calculate field profile
  std::vector<double> parForBofZ(par_vec);

  TLondon1D_1L BofZ(parForBofZ);

  std::vector<double> energies(fImpProfile->Energy());
  std::vector<double>::const_iterator energyIter;
  energyIter = find(energies.begin(), energies.end(), E);

  if (energyIter != energies.end()) { // implantation profile found - no interpolation needed
    return CalcMeanB(E, interfaces, weights, BofZ);
  } else {
    if (E < energies.front())
      return CalcMeanB(energies.front(), interfaces, weights, BofZ);
    if (E > energies.back())
      return CalcMeanB(energies.back(), interfaces, weights, BofZ);

    energyIter = find_if(energies.begin(), energies.end(), bind2nd( greater<double>(), E));
//    cout << *(energyIter - 1) << " " << *(energyIter) << endl;

    double E1(*(energyIter - 1));
    double E2(*(energyIter));

    double B1(CalcMeanB(E1, interfaces, weights, BofZ));
    double B2(CalcMeanB(E2, interfaces, weights, BofZ));

    return B1 + (B2-B1)/(E2-E1)*(E-E1);
  }
}

double TMeanFieldsForScSingleLayer::CalcMeanB (double E, const std::vector<double>& interfaces, const std::vector<double>& weights, const TLondon1D_1L& BofZ) const {
    //calcData->UseHighResolution(E);
    fImpProfile->WeightLayers(E, interfaces, weights);
    fImpProfile->Normalize(E);

    std::vector<double> z(fImpProfile->DataZ(E));
    std::vector<double> nz(fImpProfile->DataNZ(E));
    double dz(fImpProfile->DataDZ(E));

    // calculate mean field

    double meanB(0.);

    for (unsigned int i(0); i<z.size(); i++) {
      meanB += dz*nz[i]*BofZ.GetBofZ(0.1*z[i]);
    }
    return meanB;
}

// Constructor: Read the energies from the xml-file and load the according TRIM.SP-data files

TMeanFieldsForScBilayer::TMeanFieldsForScBilayer() {

  // read startup file
  std::string startup_path_name("BMW_startup.xml");

  TSAXParser *saxParser = new TSAXParser();
  BMWStartupHandler *startupHandler = new BMWStartupHandler();
  saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
  int status (saxParser->ParseFile(startup_path_name.c_str()));
  // check for parse errors
  if (status) { // error
    cerr << endl << "**ERROR** reading/parsing " << startup_path_name << " failed." \
         << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
         << endl;
    assert(false);
  }

  std::string rge_path(startupHandler->GetDataPath());
  map<double, std::string> energy_vec(startupHandler->GetEnergies());

  fImpProfile = new TTrimSPData(rge_path, energy_vec, startupHandler->GetDebug(), 1);

  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }

}

// Operator-method that returns the mean field for a given implantation energy
// Parameters: field, deadlayer, layer1, layer2, lambda1, lambda2, weight1 (deadlayer), weight2, weight3, weight4 (substrate),
//             [Gss width for profile convolution]

double TMeanFieldsForScBilayer::operator()(double E, const std::vector<double> &par_vec) const{

  double width(0.0);
  if (par_vec.size() == 11) {
    width = par_vec[10];
  }

  std::vector<double> interfaces;
  interfaces.push_back(par_vec[1]);
  interfaces.push_back(par_vec[1]+par_vec[2]);
  interfaces.push_back(par_vec[1]+par_vec[2]+par_vec[3]);
  
  std::vector<double> weights;
  weights.push_back(par_vec[6]);
  weights.push_back(par_vec[7]);
  weights.push_back(par_vec[8]);
  weights.push_back(par_vec[9]);
  
  // Calculate field profile
  std::vector<double> parForBofZ;
  for (unsigned int i(0); i<6; i++)
    parForBofZ.push_back(par_vec[i]);

  TLondon1D_2L BofZ(parForBofZ);

  std::vector<double> energies(fImpProfile->Energy());
  std::vector<double>::const_iterator energyIter;
  energyIter = find(energies.begin(), energies.end(), E);

  if (energyIter != energies.end()) { // implantation profile found - no interpolation needed
    return CalcMeanB(E, interfaces, weights, BofZ, width);
  } else {
    if (E < energies.front())
      return CalcMeanB(energies.front(), interfaces, weights, BofZ, width);
    if (E > energies.back())
      return CalcMeanB(energies.back(), interfaces, weights, BofZ, width);

    energyIter = find_if(energies.begin(), energies.end(), bind2nd( greater<double>(), E));
//    cout << *(energyIter - 1) << " " << *(energyIter) << endl;

    double E1(*(energyIter - 1));
    double E2(*(energyIter));

    double B1(CalcMeanB(E1, interfaces, weights, BofZ, width));
    double B2(CalcMeanB(E2, interfaces, weights, BofZ, width));

    return B1 + (B2-B1)/(E2-E1)*(E-E1);
  }
}

double TMeanFieldsForScBilayer::CalcMeanB (double E, const std::vector<double>& interfaces, const std::vector<double>& weights, const TLondon1D_2L& BofZ, double width=0.0) const {
    //fImpProfile->UseHighResolution(E);
    //fImpProfile->ConvolveGss(width, E);
    //fImpProfile->SetOriginal();
    fImpProfile->WeightLayers(E, interfaces, weights);
    fImpProfile->ConvolveGss(width, E);
    fImpProfile->Normalize(E);

    std::vector<double> z(fImpProfile->DataZ(E));
    std::vector<double> nz(fImpProfile->DataNZ(E));
    double dz(fImpProfile->DataDZ(E));

  if (E==20.0){
  ofstream of("Implantation-profile-normal.dat");
  for (unsigned int i(0); i<z.size(); i++) {
    of << z[i] << " " << nz[i] << endl;
  }
  of.close();
 }
    // calculate mean field

    double meanB(0.);

    for (unsigned int i(0); i<z.size(); i++) {
      meanB += dz*nz[i]*BofZ.GetBofZ(z[i]/10.);
    }
    return meanB;
}

// Constructor: Read the energies from the xml-file and load the according TRIM.SP-data files

TMeanFieldsForScTrilayer::TMeanFieldsForScTrilayer() {

  // read startup file
  std::string startup_path_name("BMW_startup.xml");

  TSAXParser *saxParser = new TSAXParser();
  BMWStartupHandler *startupHandler = new BMWStartupHandler();
  saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
  int status (saxParser->ParseFile(startup_path_name.c_str()));
  // check for parse errors
  if (status) { // error
    cerr << endl << "**ERROR** reading/parsing " << startup_path_name << " failed." \
         << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
         << endl;
    assert(false);
  }

  std::string rge_path(startupHandler->GetDataPath());
  map<double, std::string> energy_vec(startupHandler->GetEnergies());

  fImpProfile = new TTrimSPData(rge_path, energy_vec, startupHandler->GetDebug());

  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }

}

// Operator-method that returns the mean field for a given implantation energy
// Parameters: field, deadlayer, layer1, layer2, layer3, lambda1, lambda2, lambda3, weight1, weight2, weight3, weight4, weight5

double TMeanFieldsForScTrilayer::operator()(double E, const std::vector<double> &par_vec) const{
  
  std::vector<double> interfaces;
  interfaces.push_back(par_vec[1]);
  interfaces.push_back(par_vec[1]+par_vec[2]);
  interfaces.push_back(par_vec[1]+par_vec[2]+par_vec[3]);
  interfaces.push_back(par_vec[1]+par_vec[2]+par_vec[3]+par_vec[4]);
  
  std::vector<double> weights;
  weights.push_back(par_vec[8]);
  weights.push_back(par_vec[9]);
  weights.push_back(par_vec[10]);
  weights.push_back(par_vec[11]);
  weights.push_back(par_vec[12]);
  
  // Calculate field profile
  std::vector<double> parForBofZ;
  for (unsigned int i(0); i<8; i++)
    parForBofZ.push_back(par_vec[i]);

  TLondon1D_3L BofZ(parForBofZ);

  std::vector<double> energies(fImpProfile->Energy());
  std::vector<double>::const_iterator energyIter;
  energyIter = find(energies.begin(), energies.end(), E);

  if (energyIter != energies.end()) { // implantation profile found - no interpolation needed
    return CalcMeanB(E, interfaces, weights, BofZ);
  } else {
    if (E < energies.front())
      return CalcMeanB(energies.front(), interfaces, weights, BofZ);
    if (E > energies.back())
      return CalcMeanB(energies.back(), interfaces, weights, BofZ);

    energyIter = find_if(energies.begin(), energies.end(), bind2nd( greater<double>(), E));
//    cout << *(energyIter - 1) << " " << *(energyIter) << endl;

    double E1(*(energyIter - 1));
    double E2(*(energyIter));

    double B1(CalcMeanB(E1, interfaces, weights, BofZ));
    double B2(CalcMeanB(E2, interfaces, weights, BofZ));

    return B1 + (B2-B1)/(E2-E1)*(E-E1);
  }
}

double TMeanFieldsForScTrilayer::CalcMeanB (double E, const std::vector<double>& interfaces, const std::vector<double>& weights, const TLondon1D_3L& BofZ) const {
  //calcData->UseHighResolution(E);
    fImpProfile->WeightLayers(E, interfaces, weights);
    fImpProfile->Normalize(E);

    std::vector<double> z(fImpProfile->DataZ(E));
    std::vector<double> nz(fImpProfile->DataNZ(E));
    double dz(fImpProfile->DataDZ(E));

    // calculate mean field

    double meanB(0.);

    for (unsigned int i(0); i<z.size(); i++) {
      meanB += dz*nz[i]*BofZ.GetBofZ(z[i]/10.);
    }
    return meanB;
}

// Constructor: Read the energies from the xml-file and load the according TRIM.SP-data files

TMeanFieldsForScTrilayerWithInsulator::TMeanFieldsForScTrilayerWithInsulator() {

  // read startup file
  std::string startup_path_name("BMW_startup.xml");

  TSAXParser *saxParser = new TSAXParser();
  BMWStartupHandler *startupHandler = new BMWStartupHandler();
  saxParser->ConnectToHandler("BMWStartupHandler", startupHandler);
  int status (saxParser->ParseFile(startup_path_name.c_str()));
  // check for parse errors
  if (status) { // error
    cerr << endl << "**ERROR** reading/parsing " << startup_path_name << " failed." \
         << endl << "**ERROR** Please make sure that the file exists in the local directory and it is set up correctly!" \
         << endl;
    assert(false);
  }

  std::string rge_path(startupHandler->GetDataPath());
  map<double, std::string> energy_vec(startupHandler->GetEnergies());

  fImpProfile = new TTrimSPData(rge_path, energy_vec, startupHandler->GetDebug());

  // clean up
  if (saxParser) {
    delete saxParser;
    saxParser = 0;
  }
  if (startupHandler) {
    delete startupHandler;
    startupHandler = 0;
  }

}

// Operator-method that returns the mean field for a given implantation energy
// Parameters: field, deadlayer, layer1, layer2, layer3, lambda1, lambda2, weight1, weight2, weight3, weight4, weight5

double TMeanFieldsForScTrilayerWithInsulator::operator()(double E, const std::vector<double> &par_vec) const{
  
  std::vector<double> interfaces;
  interfaces.push_back(par_vec[1]);
  interfaces.push_back(par_vec[1]+par_vec[2]);
  interfaces.push_back(par_vec[1]+par_vec[2]+par_vec[3]);
  interfaces.push_back(par_vec[1]+par_vec[2]+par_vec[3]+par_vec[4]);
  
  std::vector<double> weights;
  weights.push_back(par_vec[7]);
  weights.push_back(par_vec[8]);
  weights.push_back(par_vec[9]);
  weights.push_back(par_vec[10]);
  weights.push_back(par_vec[11]);
  
  // Calculate field profile
  std::vector<double> parForBofZ;
  for (unsigned int i(0); i<7; i++)
    parForBofZ.push_back(par_vec[i]);

  TLondon1D_3LwInsulator BofZ(parForBofZ);

  std::vector<double> energies(fImpProfile->Energy());
  std::vector<double>::const_iterator energyIter;
  energyIter = find(energies.begin(), energies.end(), E);

  if (energyIter != energies.end()) { // implantation profile found - no interpolation needed
    return CalcMeanB(E, interfaces, weights, BofZ);
  } else {
    if (E < energies.front())
      return CalcMeanB(energies.front(), interfaces, weights, BofZ);
    if (E > energies.back())
      return CalcMeanB(energies.back(), interfaces, weights, BofZ);

    energyIter = find_if(energies.begin(), energies.end(), bind2nd( greater<double>(), E));
//    cout << *(energyIter - 1) << " " << *(energyIter) << endl;

    double E1(*(energyIter - 1));
    double E2(*(energyIter));

    double B1(CalcMeanB(E1, interfaces, weights, BofZ));
    double B2(CalcMeanB(E2, interfaces, weights, BofZ));

    return B1 + (B2-B1)/(E2-E1)*(E-E1);
  }
}

double TMeanFieldsForScTrilayerWithInsulator::CalcMeanB
 (double E, const std::vector<double>& interfaces, const std::vector<double>& weights, const TLondon1D_3LwInsulator& BofZ) const {
  //calcData->UseHighResolution(E);
    fImpProfile->WeightLayers(E, interfaces, weights);
    fImpProfile->Normalize(E);

    std::vector<double> z(fImpProfile->DataZ(E));
    std::vector<double> nz(fImpProfile->DataNZ(E));
    double dz(fImpProfile->DataDZ(E));

    // calculate mean field

    double meanB(0.);

    for (unsigned int i(0); i<z.size(); i++) {
      meanB += dz*nz[i]*BofZ.GetBofZ(0.1*z[i]);
    }
    return meanB;
}
