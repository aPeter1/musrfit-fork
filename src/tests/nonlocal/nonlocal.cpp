// -----------------------------------------------------------------------
// Author: Andreas Suter
// $Id$
// -----------------------------------------------------------------------

#include <cstdlib>

#include <iostream>
using namespace std;

#include "PPippard.h"

//-----------------------------------------------------------------------------------------
/**
 *
 */
void syntax()
{
  cout << endl << "usage: nonlocal <inputFile>";
  cout << endl << "The input file has the following structure:";
  cout << endl << "% input file for nonlocal";
  cout << endl << "% --------------------------------";
  cout << endl << "% input parameters:";
  cout << endl << "%   reduced temperature";
  cout << endl << "%   lambda London";
  cout << endl << "%   xi0";
  cout << endl << "%   mean free path";
  cout << endl << "%   film thickness";
  cout << endl << "%   boundary conditions";
  cout << endl << "%   [rge input file name], i.e. this is optional";
  cout << endl << "%   [output file name], i.e. this is optional";
  cout << endl;
  cout << endl << "reducedTemp = 0.8287";
  cout << endl << "lambdaL = 30.0";
  cout << endl << "xi0 = 380.0";
  cout << endl << "meanFreePath = 12000.0";
  cout << endl << "filmThickness = 5000.0";
  cout << endl << "specular = 1";
  cout << endl << "rgeFileName = /afs/psi.ch/project/nemu/analysis/2009/Nonlocal/trimsp/InSne060.rge";
  cout << endl << "outputFileName = In_37_T2P83.dat";
  cout << endl << endl;
  cout << endl << "lines starting with a '%' or a '#' are comment lines.";
  cout << endl << "empty lines are ignored.";
  cout << endl << "all lengths given in (nm)";
  cout << endl << endl;
}

//-----------------------------------------------------------------------------------------
/**
 *
 */
int readInputFile(char *fln, PippardParams &param)
{
  FILE *fp;

  fp = fopen(fln, "r");
  if (fp == NULL) {
    cout << endl << "Couldn't read input file " << fln;
    cout << endl;
    return 0;
  }

  char line[128], str[128];
  TString tstr;
  Double_t dval;
  Int_t result, ival;
  while (!feof(fp)) {
    fgets(line, sizeof(line), fp);
    tstr = line;
    if (tstr.BeginsWith("#") || tstr.BeginsWith("%") || tstr.IsWhitespace()) {
      continue;
    }

    if (tstr.BeginsWith("reducedTemp =")) {
      result = sscanf(line, "reducedTemp = %lf", &dval);
      if (result != 1) {
        cout << endl << "**ERROR** while trying to extracted the reduced temperature";
        cout << endl << "line = " << line;
        cout << endl;
        param.valid = false;
        break;
      }
      param.t = dval;
    }

    if (tstr.BeginsWith("lambdaL =")) {
      result = sscanf(line, "lambdaL = %lf", &dval);
      if (result != 1) {
        cout << endl << "**ERROR** while trying to extracted the London penetration length";
        cout << endl << "line = " << line;
        cout << endl;
        param.valid = false;
        break;
      }
      param.lambdaL = dval;
    }

    if (tstr.BeginsWith("xi0 =")) {
      result = sscanf(line, "xi0 = %lf", &dval);
      if (result != 1) {
        cout << endl << "**ERROR** while trying to extracted the Pippard coherence length";
        cout << endl << "line = " << line;
        cout << endl;
        param.valid = false;
        break;
      }
      param.xi0 = dval;
    }

    if (tstr.BeginsWith("meanFreePath =")) {
      result = sscanf(line, "meanFreePath = %lf", &dval);
      if (result != 1) {
        cout << endl << "**ERROR** while trying to extracted the mean free path";
        cout << endl << "line = " << line;
        cout << endl;
        param.valid = false;
        break;
      }
      param.meanFreePath = dval;
    }

    if (tstr.BeginsWith("filmThickness =")) {
      result = sscanf(line, "filmThickness = %lf", &dval);
      if (result != 1) {
        cout << endl << "**ERROR** while trying to extracted the film thickness";
        cout << endl << "line = " << line;
        cout << endl;
        param.valid = false;
        break;
      }
      param.filmThickness = dval;
    }

    if (tstr.BeginsWith("specular =")) {
      result = sscanf(line, "specular = %d", &ival);
      if (result != 1) {
        cout << endl << "**ERROR** while trying to extracted the boundary conditions";
        cout << endl << "line = " << line;
        cout << endl;
        param.valid = false;
        break;
      }
      if (ival == 1)
        param.specular = true;
      else
        param.specular = false;
    }

    if (tstr.BeginsWith("rgeFileName =")) {
      result = sscanf(line, "rgeFileName = %s", str);
      if (result != 1) {
        cout << endl << "**ERROR** while trying to extracted the rge file name";
        cout << endl << "line = " << line;
        cout << endl;
        param.valid = false;
        break;
      }
      param.rgeFileName = TString(str);
    }

    if (tstr.BeginsWith("outputFileName =")) {
      result = sscanf(line, "outputFileName = %s", str);
      if (result != 1) {
        cout << endl << "**ERROR** while trying to extracted the output file name";
        cout << endl << "line = " << line;
        cout << endl;
        param.valid = false;
        break;
      }
      param.outputFileName = TString(str);
    }
  }

  fclose(fp);

  return 1;
}

//-----------------------------------------------------------------------------------------
/**
 *
 */
int readRgeFile(const char *fln, vector<Double_t> &x, vector<Double_t> &n)
{
  FILE *fp;

  fp = fopen(fln, "r");
  if (fp == NULL) {
    cout << endl << "Couldn't read rge file " << fln;
    cout << endl;
    return 0;
  }

  char line[128];
  int lineNo = 1;
  int result;
  double valX, valN;
  // read first line and ignore it since it is the header line
  fgets(line, sizeof(line), fp);
  while (!feof(fp)) {
    fgets(line, sizeof(line), fp);
    result = sscanf(line, "%lf %lf", &valX, &valN);
    if (result != 2) {
      cout << endl << "**ERROR** in line no " << lineNo;
      cout << endl << "Couldn't extract x and n(x), will quit.";
      cout << endl << endl;
      fclose(fp);
      return 0;
    }
    x.push_back(valX/10.0); // x in nm
    n.push_back(valN);
    lineNo++;
  }

  // normalize n(z)
  Double_t norm = 0.0;
  for (unsigned int i=0; i<n.size(); i++) {
    norm += n[i];
  }
  for (unsigned int i=0; i<n.size(); i++) {
    n[i] /= norm * (x[1]-x[0]);
  }

/*
for (unsigned int i=0; i<x.size(); i++) {
  cout << endl << i << ": " << x[i] << ", " << n[i];
}
cout << endl;
*/

  fclose(fp);

  return 1;
}

//-----------------------------------------------------------------------------------------
/**
 *
 */
int main(int argc, char *argv[])
{
  if (argc != 2) {
    syntax();
    return 0;
  }

  PippardParams params;

  // init params
  params.valid = true;
  params.t = -1.0;
  params.lambdaL = -1.0;
  params.xi0 = -1.0;
  params.meanFreePath = -1.0;
  params.filmThickness = -1.0;
  params.specular = true;
  params.specularIntegral = -1.0;
  params.rgeFileName = "";
  params.outputFileName = "";
  params.meanB = 0.0;

  if (!readInputFile(argv[1], params)) {
    cout << endl << "**ERROR** Couldn't open the input file " << argv[1] << ". Will quit.";
    cout << endl << endl;
    return 0;
  }

  // check params
  if (!params.valid) {
    cout << endl << "**ERROR** parameters are not valid will quit.";
    cout << endl << endl;
    return 0;
  }
  if (params.t < 0.0) {
    cout << endl << "**ERROR** reduced temperature not defined, will quit.";
    cout << endl << endl;
    return 0;
  }
  if (params.lambdaL < 0.0) {
    cout << endl << "**ERROR** London pentration depth not defined, will quit.";
    cout << endl << endl;
    return 0;
  }
  if (params.xi0 < 0.0) {
    cout << endl << "**ERROR** Pippard coherence length not defined, will quit.";
    cout << endl << endl;
    return 0;
  }
  if (params.meanFreePath < 0.0) {
    cout << endl << "**ERROR** mean free path not defined, will set it to 1.0e6 nm.";
    cout << endl << endl;
    params.meanFreePath = 1.0e6;
  }
  if (params.filmThickness < 0.0) {
    cout << endl << "**ERROR** film thickness not defined, will set it to 1.0e6 nm.";
    cout << endl << endl;
    params.filmThickness = 1.0e6;
  }

  int result;
  vector<Double_t> x, n;
  if (params.rgeFileName.Length() > 0) {
    result = readRgeFile(params.rgeFileName.Data(), x, n);
    if (result == 0)
      params.rgeFileName = "";
  }

  cout << endl << ">> temp          = " << params.t;
  cout << endl << ">> lambdaL       = " << params.lambdaL;
  cout << endl << ">> xi0           = " << params.xi0;
  cout << endl << ">> meanFreePath  = " << params.meanFreePath;
  cout << endl << ">> filmThickness = " << params.filmThickness;
  if (params.specular)
    cout << endl << ">> specular     = true";
  else
    cout << endl << ">> specular     = false";
  cout << endl;

  PPippard *pippard = new PPippard(params);

  pippard->CalculateField();

  // check if it is necessary to calculate the <B(x)>
  if (x.size() > 0) {
    Double_t dz = pippard->GetStepDistance();
    Int_t max = (Int_t)((Double_t)(x[x.size()-1])/dz);
//cout << endl << "max = " << max;
    Double_t mean = 0.0;
    Double_t nn;
    for (Int_t i=0; i<max; i++) {
      // find propper z value index of n(z)
      Int_t idx = -1;
      for (unsigned int j=0; j<x.size(); j++) {
        if (x[j] >= i*dz) {
          idx = j;
          break;
        }
      }
      // get n(z) at the proper index
      if (idx == -1) {
        nn = 0.0;
      } else if (idx == 0) {
        nn = n[0];
      } else {
        nn = n[idx-1] + (n[idx]-n[idx-1]) * (i*dz-x[idx-1])/(x[idx]-x[idx-1]);
      }
//cout << endl << i << ", idx = " << idx << ", i*dz = " << i*dz << ", x[idx] = " << x[idx] << ", x[idx-1] = " << x[idx-1] << ", nn = " << nn;
      mean += pippard->GetMagneticField(i*dz) * nn;
    }
    mean *= dz/(x[1]-x[0]);

    cout << endl << ">> mean field = " << mean;
    pippard->SetMeanB(mean);
  }

  if (params.outputFileName.Length() > 0)
    pippard->SaveField();

  cout << endl << ">> magnetic field = " << pippard->GetMagneticField(0.0);
  cout << endl;

  if (pippard) {
    delete pippard;
    pippard = 0;
  }

  return 1;
}