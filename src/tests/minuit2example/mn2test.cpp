/***************************************************************************

  mn2test.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
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

#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <vector>

#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnUserParameters.h"
#include "Minuit2/MnMinimize.h"
#include "Minuit2/MnMinos.h"

#include "PGlobalChiSquare.h"

#include "mn2test.h"

using namespace ROOT::Minuit2;

//----------------------------------------------------------------------------
/**
 * <p>generates data for testing
 * model = ampl * exp(-lambda * t), where ampl is depending on the run, but
 *                                  not lambda
 */
void generateData(PRunList &data)
{
  PRun run;
  const unsigned int length = 100;
  double value;

  for (unsigned int i=0; i<2; i++) { // run loop
    for (unsigned int j=0; j<length; j++) { // data loop
      run.fTime.push_back((double)j/(double)length);
      value = (2*i+10)*exp(-1.7*(double)j/(double)length);
      value += 2.0*sqrt(value)*((double)random()/(double)RAND_MAX-0.5);
      run.fValue.push_back(value);
      run.fError.push_back(sqrt(value/4.0));
    }
    data.push_back(run);

    // clean up
    run.fTime.clear();
    run.fValue.clear();
    run.fError.clear();
  }
}

//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  if (argc || argv);

  int success = 1;

  PRunList runs;
  generateData(runs);

unsigned int max = 25;
if (runs[0].fTime.size() < max)
  max = runs[0].fTime.size();
for (unsigned int i=0; i<runs.size(); i++) { // run loop
  std::cout << std::endl << "run " << i;
  for (unsigned int j=0; j<max; j++) { // data loop
    std::cout << std::endl << runs[i].fTime[j] << ", " << runs[i].fValue[j] << ", " << runs[i].fError[j];
  }
  std::cout << std::endl;
}

  PGlobalChiSquare chi2(runs); // chi2 object derived from ROOT::Minuit2::FCNBase

  // create parameters ---------------------------------
  MnUserParameters upar;

  // model = ampl_i * exp(-lambda * t), i = run no.
  upar.Add("ampl1",  10.0, 1.0); // ampl1
  upar.Add("ampl2",  12.0, 1.0); // ampl2
  upar.Add("lambda", 0.1, 0.01); // lambda

  // create minimizer ---------------------------------
  MnMinimize minimize(chi2, upar);

  // minimize
  FunctionMinimum min = minimize();

  if (min.IsValid()) {
    std::cout << std::endl << "minimize (MIGRAD/SIMPLEX) found minimum ...";
  } else {
    std::cout << std::endl << "minimize (MIGRAD/SIMPLEX) failed to find minimum ...";
  }
  std::cout << std::endl << "Chi2 = " << min.Fval() 
                         << ", NDF  = " << 2*runs[0].fTime.size()-3 
                         << ", Chi2r = " << min.Fval()/((double)(2*runs[0].fTime.size()-3)) << std::endl;

  // make minos analysis
  MnMinos minos(chi2, min);

  // 1-sigma MINOS errors
  std::pair<double, double> e0 = minos(0);
  std::pair<double, double> e1 = minos(1);
  std::pair<double, double> e2 = minos(2);

  // print out parameters
  unsigned int i=0;
  std::cout << std::endl << "1-sigma minos errors:" << std::endl;
  i=0;
  std::cout << std::endl << upar.Name(i) << ": " 
                         << min.UserState().Value(i) << " "
                         << e0.first << " "
                         << e0.second;
  i++;
  std::cout << std::endl << upar.Name(i) << ": " 
                         << min.UserState().Value(i) << " "
                         << e1.first << " "
                         << e1.second;
  i++;
  std::cout << std::endl << upar.Name(i) << ": " 
                         << min.UserState().Value(i) << " "
                         << e2.first << " "
                         << e2.second;

  std::cout << std::endl << std::endl;

  // clean up
  runs.clear();

  return success;
}
