// -----------------------------------------------------------------------
// Author: Andreas Suter
// $Id$
// -----------------------------------------------------------------------

#include <cstdlib>

#include <iostream>
using namespace std;

#include "PPippard.h"

typedef struct {
  Double_t energy;
  vector<Double_t> x;
  vector<Double_t> n;
} Pnofx;

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
  cout << endl << "%   reducedTemp      : reduced temperature";
  cout << endl << "%   lambdaL          : lambda London (nm)";
  cout << endl << "%   xi0              : coherence length (nm)";
  cout << endl << "%   meanFreePath     : mean free path (nm)";
  cout << endl << "%   filmThickness    : film thickness (nm)";
  cout << endl << "%   specular         : 1=specular, 0=diffuse";
  cout << endl << "%   [Bext]           : external magnetic field (G), i.e. this is optional";
  cout << endl << "%   [deadLayer]      : dead layer (nm), i.e. this is optional";
  cout << endl << "%   [rgeFileName]    : rge input file name, i.e. this is optional. Multiple rge-files possible";
  cout << endl << "%   [outputFileName] : output file name, i.e. this is optional";
  cout << endl << "%   [outputFileNameBmean] : output file name for <B> data";
  cout << endl;
  cout << endl << "reducedTemp = 0.8287";
  cout << endl << "lambdaL = 30.0";
  cout << endl << "xi0 = 380.0";
  cout << endl << "meanFreePath = 12000.0";
  cout << endl << "filmThickness = 5000.0";
  cout << endl << "specular = 1";
  cout << endl << "Bext = 47.14";
  cout << endl << "deadLayer = 0.4";
  cout << endl << "rgeFileName = /afs/psi.ch/project/nemu/analysis/2009/Nonlocal/trimsp/InSn_E6000.rge";
  cout << endl << "rgeFileName = /afs/psi.ch/project/nemu/analysis/2009/Nonlocal/trimsp/InSn_E14100.rge";
  cout << endl << "outputFileName = In_37_T2P83.dat";
  cout << endl << "outputFileNameBmean = In_37_T2P83_Bmean.dat";
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

    if (tstr.BeginsWith("Bext = ")) {
      result = sscanf(line, "Bext = %lf", &dval);
      if (result != 1) {
        cout << endl << "**ERROR** while trying to extracted the external field value.";
        cout << endl << "line = " << line;
        cout << endl;
        param.valid = false;
        break;
      }
      param.b_ext = dval;
    }

    if (tstr.BeginsWith("deadLayer = ")) {
      result = sscanf(line, "deadLayer = %lf", &dval);
      if (result != 1) {
        cout << endl << "**ERROR** while trying to extracted the dead layer value.";
        cout << endl << "line = " << line;
        cout << endl;
        param.valid = false;
        break;
      }
      param.deadLayer = dval;
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
      param.rgeFileName.push_back(TString(str));
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

    if (tstr.BeginsWith("outputFileNameBmean =")) {
      result = sscanf(line, "outputFileNameBmean = %s", str);
      if (result != 1) {
        cout << endl << "**ERROR** while trying to extracted the Bmean output file name";
        cout << endl << "line = " << line;
        cout << endl;
        param.valid = false;
        break;
      }
      param.outputFileNameBmean = TString(str);
    }
  }

  fclose(fp);

  return 1;
}

//-----------------------------------------------------------------------------------------
/**
 *
 */
int readRgeFile(const char *fln, vector<Double_t> &x, vector<Double_t> &n, Double_t &energy)
{
  FILE *fp;

  x.clear();
  n.clear();

  TString str = TString(fln);
  str.Remove(0, str.Index("_E")+2);
  str.Remove(str.Index(".rge"));
  if (str.Length() > 0) {
    if (str.IsFloat()) {
      energy = str.Atof();
    } else {
      cerr << endl << "**ERROR** couldn't extract energy from the file name '" << fln << "', will quit." << endl << endl;
      return 0;
    }
  } else {
    cerr << endl << "**ERROR** couldn't extract energy from the file name '" << fln << "', will quit." << endl << endl;
    return 0;
  }

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
  int headerCount = 0;
  while (!feof(fp)) {
    fgets(line, sizeof(line), fp);
    result = sscanf(line, "%lf %lf", &valX, &valN);
    if (result != 2) {
      if (headerCount >= 2) {
        cout << endl << "**ERROR** in line no " << lineNo;
        cout << endl << "Couldn't extract x and n(x), will quit.";
        cout << endl << endl;
        fclose(fp);
        return 0;
      } else {
        headerCount++;
        continue;
      }
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
  params.b_ext = -1.0;
  params.deadLayer = -1.0;
  params.rgeFileName.clear();
  params.inputFileName = TString(argv[1]);
  params.outputFileName = "";
  params.outputFileNameBmean = "";
  params.meanX.clear();
  params.meanB.clear();

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
  Pnofx rgeSet;
  vector<Pnofx> nOfx;
  double energy;
  vector<Double_t> x, n;
  for (UInt_t i=0; i<params.rgeFileName.size(); i++) {
    x.clear();
    n.clear();
    energy = 0.0;
    rgeSet.energy = 0.0;
    rgeSet.x.clear();
    rgeSet.n.clear();

    result = readRgeFile(params.rgeFileName[i].Data(), x, n, energy);
    if (result == 0) {
      cerr << endl << "**ERROR** Couldn't read rge-file " << params.rgeFileName[i].Data() << ", will quit." << endl << endl;
      return 0;
    } else {
      cout << endl << ">> read rge-file '" << params.rgeFileName[i].Data() << "'";
    }

    cout << endl << ">> energy        = " << energy << " eV";
    rgeSet.energy = energy;
    rgeSet.x = x;
    rgeSet.n = n;
    nOfx.push_back(rgeSet);
  }
  cout << endl << ">> found " << nOfx.size() << " rge-files.";


  cout << endl << ">> temp          = " << params.t;
  cout << endl << ">> lambdaL       = " << params.lambdaL;
  cout << endl << ">> xi0           = " << params.xi0;
  cout << endl << ">> meanFreePath  = " << params.meanFreePath;
  cout << endl << ">> filmThickness = " << params.filmThickness;
  if (params.specular)
    cout << endl << ">> specular      = true";
  else
    cout << endl << ">> specular      = false";
  cout << endl;

  PPippard *pippard = new PPippard(params);

//  pippard->DumpParams();

  pippard->CalculateField();

  // check if it is necessary to calculate the <B(x)> and <[B(x)-<B(x)>]^2>
  if (nOfx.size() > 0) {
    if ((params.b_ext == -1.0) || (params.deadLayer == -1.0)) {
      cout << endl << "**ERROR** Bext or deadLayer missing :-(" << endl;
      return 0;
    }

    Double_t meanX = 0.0;
    Double_t meanB = 0.0;
    Double_t varB = 0.0;
    Double_t BB = 0.0;
    for (UInt_t i=0; i<nOfx.size(); i++) {
      meanX = 0.0;
      for (unsigned int j=0; j<nOfx[i].x.size(); j++) {
        meanX += nOfx[i].x[j]*nOfx[i].n[j];
      }
      meanX *= (nOfx[i].x[1]-nOfx[i].x[0]);

      meanB = 0.0;
      varB = 0.0;
      BB = 0.0;
      for (unsigned int j=0; j<nOfx[i].x.size()-1; j++) {
        if (nOfx[i].x[j] <= params.deadLayer) {
          meanB += 1.0 * nOfx[i].n[j];
        } else if (nOfx[i].x[j] > params.deadLayer+params.filmThickness) {
          meanB += 1.0 * nOfx[i].n[j];
        } else {
          BB = pippard->GetMagneticField(nOfx[i].x[j]-params.deadLayer);
          meanB += BB * nOfx[i].n[j];
        }
      }
      meanB *= (nOfx[i].x[1]-nOfx[i].x[0]);
      for (unsigned int j=0; j<nOfx[i].x.size()-1; j++) {
        if (nOfx[i].x[j] > params.deadLayer) {
          BB = pippard->GetMagneticField(nOfx[i].x[j]-params.deadLayer);
          varB += (BB-meanB) * (BB-meanB) * nOfx[i].n[j];
        }
      }
      varB *= (nOfx[i].x[1]-nOfx[i].x[0]);

      cout << endl << "----------------------------------------";
      cout << endl << ">> energy = " << nOfx[i].energy << "eV";
      cout << endl << ">> mean x = " << meanX << " (nm), mean field = " << params.b_ext * meanB << " (G),";
      cout << " <(B-<B>)^2> = " << varB * params.b_ext * params.b_ext << " (G^2)";

      pippard->SetEnergy(nOfx[i].energy);
      pippard->SetMeanX(meanX);
      pippard->SetVarB(varB);
      pippard->SetMeanB(meanB);
    }
  }

  if (params.outputFileName.Length() > 0)
    pippard->SaveField();

  if (params.outputFileNameBmean.Length() > 0)
    pippard->SaveBmean();

  cout << endl << endl;

  if (pippard) {
    delete pippard;
    pippard = 0;
  }

  return 1;
}
