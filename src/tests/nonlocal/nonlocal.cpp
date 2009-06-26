// -----------------------------------------------------------------------
// Author: Andreas Suter
// $Id$
// -----------------------------------------------------------------------

#include <cstdlib>

#include <iostream>
using namespace std;

#include "PPippard.h"


void syntax()
{
  cout << endl << "usage: nonlocal temp lambdaL xi0 meanFreePath filmThickness specular [<fileName>]";
  cout << endl << "  temp    : reduced temperature, i.e. t = T/T_c";
  cout << endl << "  specular: 1 = specular, 0 = diffuse";
  cout << endl << "  all lengths given in (nm)";
  cout << endl << "  if <fileName> is given, the field profile will be saved";
  cout << endl << endl;
}

int main(int argc, char *argv[])
{
  if ((argc < 7) || (argc>8)){
    syntax();
    return 0;
  }

  Double_t temp          = strtod(argv[1], (char **)NULL);
  Double_t lambdaL       = strtod(argv[2], (char **)NULL);
  Double_t xi0           = strtod(argv[3], (char **)NULL);
  Double_t meanFreePath  = strtod(argv[4], (char **)NULL);
  Double_t filmThickness = strtod(argv[5], (char **)NULL);

  char fln[1024];
  if (argc == 8) {
    strncpy(fln, argv[7], sizeof(fln));
  } else {
    strncpy(fln, "", sizeof(fln));
  }


  Bool_t specular;
  if (!strcmp(argv[6], "1"))
    specular = true;
  else if (!strcmp(argv[6], "0"))
    specular = false;
  else {
    syntax();
    return 0;
  }

  cout << endl << ">> temp          = " << temp;
  cout << endl << ">> lambdaL       = " << lambdaL;
  cout << endl << ">> xi0           = " << xi0;
  cout << endl << ">> meanFreePath  = " << meanFreePath;
  cout << endl << ">> filmThickness = " << filmThickness;
  if (specular)
    cout << endl << ">> specular     = true";
  else
    cout << endl << ">> specular     = false";
  cout << endl;

  PPippard *pippard = new PPippard(temp, lambdaL, xi0, meanFreePath, filmThickness, specular);

  pippard->CalculateField();

  if (strlen(fln) > 0)
    pippard->SaveField(fln);

  cout << endl << ">> magnetic field = " << pippard->GetMagneticField(0.0);
  cout << endl;

  if (pippard) {
    delete pippard;
    pippard = 0;
  }

  return 1;
}
