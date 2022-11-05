/***************************************************************************

  photo_meissner.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013 by Andreas Suter                                   *
 *   andreas.suter@psi.ch                                                  *
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

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cassert>
using namespace std;

#include <gsl_sf_bessel.h>

typedef struct {
  double lamL;
  double lamP;
  double z0;
  double deadLayer;
  double filmThickness;
} PhotoParam;

double InuMinus(double nu, double x);

//--------------------------------------------------------------------------
void photo_meissner_syntax()
{
  cout << endl << "usage: (0) photo_meissner --version | [--help]" << endl;
  cout << endl << "       (1) photo_meissner lamL lamP z0 deadLayer [filmThickness]";
  cout << endl << "                      {--at <zPos> | --range <zStart> <zEnd>]} [--dump <fln>]" << endl;
  cout << endl << "           this option is used to calculate fields without any muon stopping";
  cout << endl << "           distribution averaging" << endl;
  cout << endl << "       (2) photo_meissner lamL lamP z0 deadLayer [filmThickness] --file <rge-file>";
  cout << endl << "              [--dump <fln>]" << endl;
  cout << endl << "           this option is used to calculate <z> and <B>, for a given muon";
  cout << endl << "           implantation energy, given by the rge-file." << endl;
  cout << endl << "       (3) photo_meissner lamL lamP z0 deadLayer [filmThickness]";
  cout << endl << "              --prefix <rge-prefix> --energies <list> [--dump <fln>]" << endl;
  cout << endl << "           this option is used to calculate <z> and <B>, for a whole list of muon energies";
  cout << endl << "           as given in the rge-file list which will be generated by <rge-prefix> and <list>." << endl;
  cout << endl << "       lamL          : London penetration depth in (nm)";
  cout << endl << "       lamP          : Photo induced penetration depth (nm)";
  cout << endl << "       z0            : length scale of the photo induced effect (nm)";
  cout << endl << "       deadLayer     : dead layer (nm)";
  cout << endl << "       filmThickness : film thickness in (nm), if absent a";
  cout << endl << "                       semi-infinite sample is assumed. filmThickness = 2 t";
  cout << endl << "       --at <zPos>   : calculates the field at the position <zPos> (nm).";
  cout << endl << "       --range <zStart> <zEnd> : calculate the field in the range from";
  cout << endl << "                       <zStart> to <zEnd> in (nm).";
  cout << endl << "                       either --at or --range needs to be present.";
  cout << endl << "       --dump <fln>  : dump the result into the file <fln>.";
  cout << endl << "                       if this option is not given, the result is dumped to stdout";
  cout << endl << "       --file <rge-file> : muon stopping distribution to calculate <B>.";
  cout << endl << "       --prefix <rge-prefix> : prefix to generate the rge-path-file-name, e.g. --prefix YBCO_E";
  cout << endl << "       --energies <list> : energy list, where list is a comma seprated energy list in eV,";
  cout << endl << "                      e.g. --energies 1000,1500,2000,2500,...,250000. Optionally, it can also";
  cout << endl << "                      be --energies 1000:25000:1000, in which case it means start at 1000eV";
  cout << endl << "                      increment by 1000eV, and fill the energy list up to 25000eV.";
  cout << endl << "                      NO spaces between the comma separated list allowed!";
  cout << endl << endl;
}

//--------------------------------------------------------------------------
/**
 * <p>Routine to tokenize a string. Empty tokens will be ignored.
 *
 * \param cstr string to be tokenized
 * \param delim string of characters to be parsed
 * \param tok vector of tokens
 */
void tokenizer(const char *cstr, const char *delim, vector<string> &tok)
{
  tok.clear();
  char sstr[128];

  int ss=0;
  for (int i=0; i<strlen(cstr); i++) {
    for (int j=0; j<strlen(delim); j++) { // loop over all deliminator characters
      if (cstr[i] == delim[j]) { // deliminator found, create token
        memset(sstr,0,128);
        for (int k=0; k<i-ss; k++)
          sstr[k] = cstr[ss+k];
        if (strlen(sstr)>0) // ignore empty tokens
          tok.push_back(sstr);
        ss = i+1;
      }
    }
  }

  // handle last token if the last character is not a delim
  for (int i=0; i<strlen(delim); i++) {
    if (cstr[strlen(cstr)-1] != delim[i]) {
      memset(sstr,0,128);
      for (int j=0; j<strlen(cstr)-ss; j++)
        sstr[j] = cstr[ss+j];
      if (strlen(sstr)>0)
        tok.push_back(sstr);
    }
  }
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param prefix used to generate the path file name
 * \param elist is the list of energies, which is either a comma sparated list, or has the format <start>:<end>:<step>
 * \param rgeFln rge-file list
 */
int generateRgeFln(const string prefix, const string elist, vector<string> &rgeFln)
{
  vector<string> tok;
  string str("");
  char istr[128];
  int start, end, step, ival;

  if (elist.find(",") != string::npos) { // comma separated energy list
    tokenizer(elist.c_str(), ",", tok);
    for (unsigned int i=0; i<tok.size(); i++) {
      str = prefix + tok[i] + ".rge";
      rgeFln.push_back(str);
    }
  } else if (elist.find(":") != string::npos) { // format for the energy list: start:end:step
    tokenizer(elist.c_str(), ":", tok);
    // check for the proper number of tokens
    if (tok.size() != 3) {
      cerr << endl << "**ERROR** in generateRgeFln: option --energies <start>:<end>:<step>. Found " << tok.size() << " tokens instead of 3." << endl;
      return 0;
    }
    // check that all tokens are indeed numbers > 0
    start = atoi(tok[0].c_str());
    end   = atoi(tok[1].c_str());
    step  = atoi(tok[2].c_str());
    if ((start <= 0) || (end <= 0) || (step <= 0) || (end <= start)) {
      cerr << endl << "**ERROR** in generateRgeFln: option --energies <start>:<end>:<step>. One of these value is not a number > 0." << endl;
      return 0;
    }
    ival = start;
    do {
      snprintf(istr, sizeof(istr), "%d", ival);
      str = prefix + istr + ".rge";
      rgeFln.push_back(str);
      ival += step;
    } while (ival <= end);
  } else { // something is wrong
    cerr << endl << "**ERROR** in generateRgeFln: found wrong format for option --energies: " << elist << endl;
    return 0;
  }
  return 1;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param line of a rge-file.
 * \param z value: position in nm
 * \param n value: number of particles at position z
 */
int getRgeValue(const char *line, double &z, double &n)
{
  vector<string> tok;
  tokenizer(line, " ", tok);

  z = -1.0; n = -1.0;
  if (tok.size() == 2) {
    if (isdigit(tok[0][0])) {
      z = strtod(tok[0].c_str(), 0);
    } else {
      tok.clear();
      return 1;
    }
    if (isdigit(tok[1][0])) {
      n = strtod(tok[1].c_str(), 0);
    } else {
      tok.clear();
      return 1;
    }
  } else {
    tok.clear();
    return 1;
  }
  tok.clear();

  if ((z == -1.0) || (n == -1.0)) {
    cerr << endl << "**ERROR in getRgeValue(): line '" << line << "' found. Seems to be invalid." << endl;
    return 0;
  }

  return 1;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param fln rge-file name
 * \param z position vector in nm
 * \param n number of particle vector corresponding to z
 */
int readRgeFile(const char *fln, vector<double> &z, vector<double> &n) {

  ifstream fin(fln, ifstream::in);
  if (!fin.is_open()) {
    cerr << endl << "**ERROR** couldn't open '" << fln << "', will quit." << endl;
    return 0;
  }

  char line[512];
  double zz, nn;
  z.clear();
  n.clear();
  while (fin.good()) {
    fin.getline(line, 512);
    if (!getRgeValue(line, zz, nn)) {
      fin.close();
      return 0;
    }
    if (zz != -1.0) { // no comment or empty line
      z.push_back(zz/10.0); // A -> nm
      n.push_back(nn);
    }
  }

  fin.close();

  return 1;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param fln rge-path-file-name vector
 * \param zz all position vectors in nm
 * \param nn all number of particle vectors corresponding to zz
 */
int readAllRgeFiles(const vector<string> &fln, vector< vector<double> > &zz, vector< vector<double> > &nn)
{
  for (unsigned int i=0; i<fln.size(); i++) {
    zz.resize(i+1);
    nn.resize(i+1);
    if (!readRgeFile(fln[i].c_str(), zz[i], nn[i])) {
      zz.clear();
      nn.clear();
      return 0;
    }
  }

  return 1;
}

//--------------------------------------------------------------------------
/**
 * <p> get interpolated number of particles at position z for a given rge-set.
 *
 * \param z requested position (in nm).
 * \param zz position vector (in nm).
 * \param nn number of particle vector corresponding to zz
 */
double get_n(const double z, vector<double> &zz, vector<double> &nn)
{
  int idx=-1;
  double n=0.0;

  // find proper position index
  for (unsigned int i=0; i<zz.size(); i++) {
    if (zz[i] > z) {
      idx = (int)i-1;
      break;
    }
  }

  if (idx == -1)
    return 0.0;

  // linear interpolation
  n = nn[idx] + (nn[idx+1]-nn[idx])*(zz[idx+1]-z)/(zz[idx+1]-zz[idx]);

  return n;
}

//--------------------------------------------------------------------------
/**
 * <p> interpolated number of particle distribution.
 *
 * \param rgeZ position values of an rge-vector (in nm)
 * \param rgeN number of particle vector corresponding to rgeZ
 * \param zz interpolated position vector
 * \param nn interpolated number of particle vector corresponding to zz
 */
void matched_nz(vector<double> &rgeZ, vector<double> &rgeN, vector<double> &zz, vector<double> &nn)
{
  for (unsigned int i=0; i<zz.size(); i++) {
    nn.push_back(get_n(zz[i], rgeZ, rgeN));
  }
}

//--------------------------------------------------------------------------
/**
 * <p>Calculates photo Meissner screening field at position z for a given set of parameters param.
 *
 * \param z position at which the screening profile shall be calculated (in nm).
 * \param param structure containing all relvant parameters
 */
double calcSingleField(const double z, const PhotoParam param)
{
  // estimate dz
  double dz = 0.0;
  if (param.lamL < param.lamP)
    dz = param.lamL / 500.0;
  else
    dz = param.lamP / 500.0;

  double nuPlus=0.0;
  double nuPhoto=0.0;
  double b=-1.0, b1=0.0;
  if (param.filmThickness == -1.0) { // semi-infinite
    nuPlus  = 2.0*param.z0/param.lamL;
    nuPhoto = 2.0*param.z0/param.lamP;
    if (z < param.deadLayer) {
      b = 1.0;
    } else {
      b1 = gsl_sf_bessel_Inu(nuPlus, nuPhoto);
      assert(b1>0.0);
      b = gsl_sf_bessel_Inu(nuPlus, nuPhoto * sqrt(exp(-(z-param.deadLayer)/param.z0)))/b1;
    }
  } else { // film
    nuPlus = 2.0*param.z0/param.lamL;
    nuPhoto = 2.0*param.z0/param.lamP;
    double beta = exp(-(param.filmThickness/2.0-param.deadLayer)/param.z0);
    double NN = InuMinus(nuPlus, nuPhoto*beta)*gsl_sf_bessel_Inu(nuPlus, nuPhoto) -
                InuMinus(nuPlus, nuPhoto)*gsl_sf_bessel_Inu(nuPlus, nuPhoto*beta);
    assert(NN>0);
    if ((z < param.deadLayer) || (z > param.filmThickness-param.deadLayer)) {
      b = 1.0;
    } else {
      b = (InuMinus(nuPlus, nuPhoto*exp(-(z-param.deadLayer)/(2.0*param.z0)))*(gsl_sf_bessel_Inu(nuPlus, nuPhoto)-gsl_sf_bessel_Inu(nuPlus, nuPhoto*beta))-
           gsl_sf_bessel_Inu(nuPlus, nuPhoto*exp(-(z-param.deadLayer)/(2.0*param.z0)))*(InuMinus(nuPlus, nuPhoto)-InuMinus(nuPlus, nuPhoto*beta)))/NN;
    }
  }
  return b;
}

//--------------------------------------------------------------------------
/**
 * <p>Calculates photo Meissner screening field between zStart and zEnd for a given set of parameters param.
 *
 * \param zStart start position from where to calculate B(z)
 * \param zEnd end position up to where to calculate B(z)
 * \param zz position vector in nm (return value)
 * \param bb photo Meissner screening vector corresponding to zz
 * \param bbL London Meissner screening vector corresponding to zz
 */
void calcFields(const double zStart, const double zEnd, const PhotoParam param, vector<double> &zz, vector<double> &bb, vector<double> &bbL)
{
  // estimate dz
  double dz = 0.0;
  if (param.lamL < param.lamP)
    dz = param.lamL / 500.0;
  else
    dz = param.lamP / 500.0;

  double nuPlus  = 2.0*param.z0/param.lamL;
  double nuPhoto = 2.0*param.z0/param.lamP;

  double z = zStart;
  double b=-1.0, bL=-1.0;
  if (param.filmThickness == -1.0) { // semi-infinite
    double b1 = gsl_sf_bessel_Inu(nuPlus, nuPhoto);
    assert(b1>0.0);
    do {
      if (z < param.deadLayer) {
        b = 1.0;
        bL = 1.0;
      } else {
        b = gsl_sf_bessel_Inu(nuPlus, nuPhoto * sqrt(exp(-(z-param.deadLayer)/param.z0)))/b1;
        bL = exp(-(z-param.deadLayer)/param.lamL);
      }
      zz.push_back(z);
      bb.push_back(b);
      bbL.push_back(bL);
      z += dz;
    } while (z <= zEnd+dz);
  } else { // film
    double beta = exp(-(param.filmThickness/2.0-param.deadLayer)/param.z0);
    double NN = InuMinus(nuPlus, nuPhoto*beta)*gsl_sf_bessel_Inu(nuPlus, nuPhoto) -
                InuMinus(nuPlus, nuPhoto)*gsl_sf_bessel_Inu(nuPlus, nuPhoto*beta);
    assert(NN>0);
    do {
      if ((z < param.deadLayer) || (z > param.filmThickness-param.deadLayer)) {
        b = 1.0;
        bL = 1.0;
      } else {
        b = (InuMinus(nuPlus, nuPhoto*exp(-(z-param.deadLayer)/(2.0*param.z0)))*(gsl_sf_bessel_Inu(nuPlus, nuPhoto)-gsl_sf_bessel_Inu(nuPlus, nuPhoto*beta))-
             gsl_sf_bessel_Inu(nuPlus, nuPhoto*exp(-(z-param.deadLayer)/(2.0*param.z0)))*(InuMinus(nuPlus, nuPhoto)-InuMinus(nuPlus, nuPhoto*beta)))/NN;
        bL = cosh((param.filmThickness/2.0-z)/param.lamL)/cosh((param.filmThickness/2.0-param.deadLayer)/param.lamL);
      }
      zz.push_back(z);
      bb.push_back(b);
      bbL.push_back(bL);
      z += dz;
    } while (z <= zEnd+dz);
  }
}

//--------------------------------------------------------------------------
/**
 * <p> calculates the average of a vector aa based on the distribution nn.
 *
 * \param aa vector to be averaged
 * \param nn distribution over which aa has to be averaged
 */
double averaged(vector<double> &aa, vector<double> &nn)
{
  if (aa.size() != nn.size()) {
    cerr << endl << "**ERROR** in averaged: vector sizes do not match!" << endl << endl;
    return 0.0;
  }

  double sum_n = 0.0;
  double result = 0.0;
  for (unsigned int i=0; i<nn.size(); i++) {
    sum_n += nn[i];
    result += aa[i]*nn[i];
  }

  if (sum_n == 0.0) {
    cerr << endl << "**ERROR** in averaged: sum_i n == 0!" << endl << endl;
    return 0.0;
  }

  return result/sum_n;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param fln file name
 * \param param photo Meissner parameter set used.
 * \param zz position vector (in nm)
 * \param bb photo Meissner screening profile
 * \param bbL London Meissner screening profile
 */
int dumpFile(const char *fln, const PhotoParam param, vector<double> &zz, vector<double> &bb, vector<double> &bbL)
{
  ofstream fout(fln);
  if (!fout.is_open()) {
    cerr << endl << "**ERROR** couldn't open '" << fln << "' for writting." << endl << endl;
    return 1;
  }

  // write header
  if (param.filmThickness == -1.0)
    fout << "% semi-infinite" << endl;
  else
    fout << "% film with thickness: " << param.filmThickness << " (nm)" << endl;
  fout << "% Parameters:" << endl;
  fout << "% lamL      = " << param.lamL << " (nm)" << endl;
  fout << "% lamP      = " << param.lamP << " (nm)" << endl;
  fout << "% z0        = " << param.z0 << " (nm)" << endl;
  fout << "% deadLayer = " << param.deadLayer << " (nm)" << endl;
  if (zz.size() == 1) {
    fout << "% --at <zPos>=" << zz[0] << " (nm) used." << endl;
  } else {
    fout << "% --range <zStart>=" << zz[0] << ", <zEnd>=" << zz[zz.size()-1] << " in (nm) used." << endl;
  }
  if (param.filmThickness == -1.0) {
    fout << "% z B/Bext exp(-(z-deadLayer)/lamL) B/Bext-exp(-(z-deadLayer)/lamL)" << endl;
  } else {
    fout << "% z B/Bext cosh[((t-2*deadLayer)-z)/lamL]/cosh[(t-2*deadLayer)/lamL]=:f(z) B/Bext-f(z)" << endl;
  }
  for (int i=0; i<zz.size(); i++) {
    fout << zz[i] << " " << bb[i] << " " << bbL[i] << " " << bb[i]-bbL[i] << endl;
  }

  fout.close();

  return 0;
}

//--------------------------------------------------------------------------
/**
 * <p>
 *
 * \param fln file name
 * \param param photo Meissner parameter set used.
 * \param rgeFln rge-path-file-name vector
 * \param zz averaged position vector
 * \param bb averaged photo Meissner screening vector
 * \param bbL London Meissner screening vector
 */
int dumpAvgFile(const char *fln, const PhotoParam param, const vector<string> rgeFln, vector<double> &zz, vector<double> &bb, vector<double> &bbL)
{
  ofstream fout(fln);
  if (!fout.is_open()) {
    cerr << endl << "**ERROR** couldn't open '" << fln << "' for writting." << endl << endl;
    return 1;
  }

  // write header
  if (param.filmThickness == -1.0)
    fout << "% semi-infinite" << endl;
  else
    fout << "% film with thickness: " << param.filmThickness << " (nm)" << endl;
  fout << "% Parameters:" << endl;
  fout << "% lamL      = " << param.lamL << " (nm)" << endl;
  fout << "% lamP      = " << param.lamP << " (nm)" << endl;
  fout << "% z0        = " << param.z0 << " (nm)" << endl;
  fout << "% deadLayer = " << param.deadLayer << " (nm)" << endl;
  fout << "% rge-file list:" << endl;
  for (unsigned int i=0; i<rgeFln.size(); i++) {
    fout << "% " << rgeFln[i] << endl;
  }
  fout << "% <z> <B>/Bext <BL>/Bext --- B: photo Meissner, BL: London Meissner" << endl;
  for (unsigned int i=0; i<zz.size(); i++) {
    fout << zz[i] << " " << bb[i] << " " << bbL[i] << endl;
  }

  fout.close();

  return 0;
}

//--------------------------------------------------------------------------
/**
 * <p> irregular negative bessel function
 *
 * \param nu
 * \param x
 */
double InuMinus(double nu, double x)
{
  return gsl_sf_bessel_Inu(nu,x) + M_2_PI * sin(nu*M_PI) * gsl_sf_bessel_Knu(nu,x);
}

//--------------------------------------------------------------------------
/**
 *
 */
int main(int argc, char *argv[])
{
  PhotoParam param;
  // init photo param
  param.lamL = -1.0;
  param.lamP = -1.0;
  param.z0   = -1.0;
  param.deadLayer = -1.0;
  param.filmThickness = -1.0;

  double zStart = -1.0;
  double zEnd = -1.0;
  char fln[1024];
  char rgeFln[1024];
  string prefix("");
  vector<string> rgeFlnList;
  vector< vector<double> > rgeZ;
  vector< vector<double> > rgeN;
  int jobTag=0; // 1=calc field without muon stopping distribution; 2=<z>, <B> for a given implantation energy (single rge-file); 3=<z>, <B> for list of rge-files

  strcpy(fln, "");
  strcpy(rgeFln, "");

  // handle input parameters
  if (argc <= 1) {
    photo_meissner_syntax();
    return 0;
  }

  if (!strcmp(argv[1], "--help")) {
    photo_meissner_syntax();
    return 0;
  }

  if (!strcmp(argv[1], "--version")) {
    cout << endl << "photo_meissner : $Id$" << endl << endl;
    return 0;
  }

  int status=0;
  for (int i=1; i<argc; i++) {
    if (i==1) {
      status = sscanf(argv[i], "%lf", &param.lamL);
      if (status != 1) {
        cerr << endl << "**ERROR** lamL: argv[" << i << "]=" << argv[i] << " is not a double." << endl;
        photo_meissner_syntax();
        return 1;
      }
    } else if (i==2) {
      status = sscanf(argv[i], "%lf", &param.lamP);
      if (status != 1) {
        cerr << endl << "**ERROR** lamP: argv[" << i << "]=" << argv[i] << " is not a double." << endl;
        photo_meissner_syntax();
        return 1;
      }
    } else if (i==3) {
      status = sscanf(argv[i], "%lf", &param.z0);
      if (status != 1) {
        cerr << endl << "**ERROR** z0: argv[" << i << "]=" << argv[i] << " is not a double." << endl;
        photo_meissner_syntax();
        return 1;
      }
    } else if (i==4) {
      status = sscanf(argv[i], "%lf", &param.deadLayer);
      if (status != 1) {
        cerr << endl << "**ERROR** deadLayer: argv[" << i << "]=" << argv[i] << " is not a double." << endl;
        photo_meissner_syntax();
        return 1;
      }
    } else if (i==5) { // either filmThickness or --at or --range
      status = sscanf(argv[i], "%lf", &param.filmThickness);
      if (status != 1) {
        param.filmThickness = -1.0;
      }
    }

    if (!strcmp(argv[i], "--at")) {
      jobTag = 1;
      if (argc > i+1) {
        status = sscanf(argv[i+1], "%lf", &zStart);
        if (status != 1) {
          cerr << endl << "**ERROR** --at <zPos>=" << argv[i+1] << " is not a double." << endl;
          photo_meissner_syntax();
          return 1;
        }
      } else {
        cerr << endl << "**ERROR** --at without <zPos> found." << endl;
        photo_meissner_syntax();
        return 1;
      }
      i += 1;
    }

    if (!strcmp(argv[i], "--range")) {
      jobTag = 2;
      if (argc > i+2) {
        status = sscanf(argv[i+1], "%lf", &zStart);
        if (status != 1) {
          cerr << endl << "**ERROR** --range <zStart> <zEnd>: <zStart>=" << argv[i+1] << " is not a double." << endl;
          photo_meissner_syntax();
          return 1;
        }
        status = sscanf(argv[i+2], "%lf", &zEnd);
        if (status != 1) {
          cerr << endl << "**ERROR** --range <zStart> <zEnd>: <zEnd>=" << argv[i+2] << " is not a double." << endl;
          photo_meissner_syntax();
          return 1;
        }
      } else {
        cerr << endl << "**ERROR** --range without <zStart> and/or <zEnd> found." << endl;
        photo_meissner_syntax();
        return 1;
      }
      i += 2;
    }

    if (!strcmp(argv[i], "--dump")) {
      if (argc > i+1) {
        strcpy(fln, argv[i+1]);
      } else {
        cerr << endl << "**ERROR** --dump without <fln> found." << endl;
        photo_meissner_syntax();
        return 1;
      }
      i += 1;
    }

    if (!strcmp(argv[i], "--file")) {
      jobTag = 3;
      if (argc > i+1) {
        strcpy(rgeFln, argv[i+1]);
      } else {
        cerr << endl << "**ERROR** --file without <rge-file> found." << endl;
        photo_meissner_syntax();
        return 1;
      }
      i += 1;
    }

    if (!strcmp(argv[i], "--prefix")) {
      jobTag = 4;
      if (argc > i+1) {
        prefix = argv[i+1];
      } else {
        cerr << endl << "**ERROR** --prefix without <rge-prefix> found." << endl;
        photo_meissner_syntax();
        return 1;
      }
      i += 1;
    }

    if (!strcmp(argv[i], "--energies")) {
      if (argc > i+1) {
        // check if format is either a comma separated list, or of the form start:end:step
        string str(argv[i+1]);
        if (!generateRgeFln(prefix, str, rgeFlnList)) {
          photo_meissner_syntax();
          return 1;
        }
        if (!readAllRgeFiles(rgeFlnList, rgeZ, rgeN))
          return 1;
      } else {
        cerr << endl << "**ERROR** --energies without <list> found." << endl;
        photo_meissner_syntax();
        return 1;
      }
    }
  }

  // read single rge-file
  if (strlen(rgeFln) > 0) {
    vector<double> zz, nn;
    if (!readRgeFile(rgeFln, zz, nn)) {
      return 1;
    }
    rgeZ.clear();
    rgeN.clear();
    rgeZ.resize(1);
    rgeN.resize(1);
    rgeZ[0] = zz;
    rgeN[0] = nn;
  }

  // check if all necessary parameters are present
  if ((param.lamL == -1.0) || (param.lamP == -1.0) || (param.z0 == -1.0) || (param.deadLayer == -1.0) || ((zStart == -1.0) && (rgeZ.size() == 0))) {
    cerr << endl << "**ERROR** not all needed parameters found. All the parameters need to be positive." << endl;
    photo_meissner_syntax();
    return 1;
  }

  if (param.lamL == 0.0) {
    cerr << endl << "**ERROR** lamL > 0.0 needed!" << endl << endl;
    return 1;
  }
  if (param.lamP == 0.0) {
    cerr << endl << "**ERROR** lamP > 0.0 needed!" << endl << endl;
    return 1;
  }
  if (param.z0 == 0.0) {
    cerr << endl << "**ERROR** z0 > 0.0 needed!" << endl << endl;
    return 1;
  }
  if (param.filmThickness == 0.0) {
    cerr << endl << "**ERROR** filmThickness > 0.0 needed!" << endl << endl;
    return 1;
  }

  // do the calculation depending on the job request
  double b=-1.0, bL = -1.0;
  vector<double> zz, bb, bbL, nn;
  vector<double> zMean, BMean, BLMean;
  switch (jobTag) {
    case 1: // single field value
      b = calcSingleField(zStart, param);
      if (param.filmThickness == -1.0) { // semi-infinite
        if (zStart < param.deadLayer)
          bL = 1.0;
        else
          bL = exp(-(zStart-param.deadLayer)/param.lamL);
      } else {
        if ((zStart < param.deadLayer) || (zStart > param.filmThickness - param.deadLayer))
          bL = 1.0;
        else
          bL = cosh((param.filmThickness/2.0-zStart)/param.lamL)/cosh((param.filmThickness/2.0-param.deadLayer)/param.lamL);
      }
      if (!strcmp(fln, "")) { // only to stdout if no dump present
        if (param.filmThickness == -1.0) // semi-infinite
          cout << endl << "result: zPos=" << zStart << ", B/Bext=" << b << ", exp(-[zPos-deadLayer]/lamL)=:f(z)=" << bL << ", B/Bext-f(z)=" << b-bL << endl << endl;
        else // film
          cout << endl << "result: zPos=" << zStart << ", B/Bext=" << b << ", cosh[((t-deadLayer)-z)/lamL]/cosh[(t-deadLayer)/lamL]=:f(z)=" << bL << ", B/Bext-f(z)=" << b-bL << endl << endl;
      } else {
        zz.push_back(zStart);
        bb.push_back(b);
        bbL.push_back(bL);
        dumpFile(fln, param, zz, bb, bbL);
      }
      break;
    case 2: // range of field values
      calcFields(zStart, zEnd, param, zz, bb, bbL);
      if (!strcmp(fln, "")) { // only to stdout if no dump present
        // write header
        if (param.filmThickness == -1.0)
          cout << "% semi-infinite" << endl;
        else
          cout << "% film with thickness: " << param.filmThickness << " (nm)" << endl;
        cout << "% Parameters:" << endl;
        cout << "% lamL      = " << param.lamL << " (nm)" << endl;
        cout << "% lamP      = " << param.lamP << " (nm)" << endl;
        cout << "% z0        = " << param.z0 << " (nm)" << endl;
        cout << "% deadLayer = " << param.deadLayer << " (nm)" << endl;
        cout << "% --range <zStart>=" << zz[0] << ", <zEnd>=" << zz[zz.size()-1] << " in (nm) used." << endl;
        if (param.filmThickness == -1.0)
          cout << "% z B/Bext exp(-(z-deadLayer)/lamL) B/Bext-exp(-(z-deadLayer)/lamL)" << endl;
        else
          cout << "% z B/Bext cosh[((t-2*deadLayer)-z)/lamL]/cosh[(t-2*deadLayer)/lamL]=:f(z) B/Bext-f(z)" << endl;
        for (int i=0; i<zz.size(); i++) {
          cout << zz[i] << ", " << bb[i] << ", " << bbL[i] << ", " << bb[i]-bbL[i] << endl;
        }
      } else {
        dumpFile(fln, param, zz, bb, bbL);
      }
      break;
    case 3: // <z>, <B> for a single rge-file
      // define a proper z-range
      zStart = 0.0;
      if (param.filmThickness == -1.0)
        zEnd = 10.0*param.lamL;
      else
        zEnd = param.filmThickness;
      // calculate the field for the given parameters
      calcFields(zStart, zEnd, param, zz, bb, bbL);
      // generate a matched n(z) vector
      nn.clear();
      matched_nz(rgeZ[0], rgeN[0], zz, nn);
      zMean.push_back(averaged(zz, nn));
      BMean.push_back(averaged(bb, nn));
      BLMean.push_back(averaged(bbL, nn));
      rgeFlnList.push_back(rgeFln);
      if (!strcmp(fln, ""))
        cout << "<z>=" << zMean[0] << " (nm), <B>/Bext=" << BMean[0] << ", <BL>/Bext=" << BLMean[0] << " for rge-file: '" << rgeFln << "'." << endl;
      else
        dumpAvgFile(fln, param, rgeFlnList, zMean, BMean, BLMean);
      break;
    case 4: // <z>, <B> for a list of rge-files
      // define a proper z-range
      zStart = 0.0;
      if (param.filmThickness == -1.0)
        zEnd = 10.0*param.lamL;
      else
        zEnd = param.filmThickness;
      // calculate the field for the given parameters
      calcFields(zStart, zEnd, param, zz, bb, bbL);
      for (unsigned int i=0; i<rgeZ.size(); i++) {
        // generate a matched n(z) vector
        nn.clear();
        matched_nz(rgeZ[i], rgeN[i], zz, nn);
        zMean.push_back(averaged(zz, nn));
        BMean.push_back(averaged(bb, nn));
        BLMean.push_back(averaged(bbL, nn));
      }
      if (!strcmp(fln, "")) { // only to stdout if no dump present
        cout << "# <z> (nm), <B>/Bext, <BL>/Bext" << endl;
        for (unsigned int i=0; i<rgeZ.size(); i++) {
          cout << zMean[i] << " , " << BMean[i] << " , " << BLMean[i] << endl;
        }
      } else {
        dumpAvgFile(fln, param, rgeFlnList, zMean, BMean, BLMean);
      }
      break;
    default:
      break;
  }

  zz.clear();
  bb.clear();
  bbL.clear();
  nn.clear();
  rgeZ.clear();
  rgeN.clear();

  return 0;
}
