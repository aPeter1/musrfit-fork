/***************************************************************************

  userFcnTest.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
 *   andreas.suter@psi.c                                                   *
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

#include <sys/time.h>

#include <iostream>
#include <vector>
#include <cassert>
using namespace std;

#include <TSystem.h>
#include <TClass.h>
#include <TString.h>
#include <TTime.h>

#include "TUserFcnBase.h"

Double_t func(Double_t t, const std::vector<Double_t> &param)
{
  assert(param.size() >= 3);

  return param[0] + param[1]*t + param[2]*t*t;
}

int main(int argc, char *argv[])
{
  // check if class is found (argv[1] == class name)
  if (!TClass::GetDict("TUserFcn")) {
    cout << endl << "could find TUserFcn, will try to load the shared lib ...";
    // try to load the shared library
    TString libName = TString("libTUserFcn.so");
    if (gSystem->Load(libName.Data())<0) {
      cout << endl << "**ERROR**: user function class 'TUserFcn' not found.";
      cout << endl << endl;
      return 0;
    }
  }

  // generate class object
  TUserFcnBase *obj = (TUserFcnBase*)TClass::GetClass("TUserFcn")->New();
  if (obj == 0) {
    cout << endl << "**ERROR**: couldn't invoke user function class 'TUserFcn'.";
    cout << endl << endl;
    return 0;
  }

  // prepare input for the TUserFcn::Eval
  Double_t t = 0.1;
  vector<Double_t> param;
  param.push_back(0.23);
  param.push_back(0.76);
  param.push_back(2.55);
  param.push_back(8.87);
  Double_t retValue = 0.0;

  struct timeval tv1, tv2;
  gettimeofday(&tv1, 0);
  for (unsigned int i=0; i<1e4; i++) {
    if (i==0)
      cout << endl << ">> obj->Eval(t, param) = " << obj->Eval(t, param);
    obj->Eval(t, param);
  }
  gettimeofday(&tv2, 0);
  double dt1 = tv2.tv_sec * 1000.0 + tv2.tv_usec / 1000.0 - (tv1.tv_sec * 1000.0 + tv1.tv_usec / 1000.0);

  gettimeofday(&tv1, 0);
  for (unsigned int i=0; i<1e4; i++) {
    if (i==0)
      cout << endl << ">> func(t, param) = " << func(t, param);
    retValue = func(t, param);
  }
  gettimeofday(&tv2, 0);
  double dt2 = tv2.tv_sec * 1000.0 + tv2.tv_usec / 1000.0 - (tv1.tv_sec * 1000.0 + tv1.tv_usec / 1000.0);

  cout << endl << ">> dt1 = " << dt1 << " (ms), dt2 = " << dt2 << " (ms), retValue = " << retValue;
  cout << endl << endl;

  // clean up
  param.clear();

  if (obj) {
    delete obj;
    obj = 0;
  }

  return 0;
}
