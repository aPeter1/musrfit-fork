/***************************************************************************

  TBNMR.cpp

  Author: Zaher Salman
  e-mail: zaher.salman@psi.ch

  2010/09/02

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010 by Zaher Salman                                *
 *   zaher.salman@psi.ch                                                  *
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

#include "TBNMR.h"
#include "TF1.h"
#include "Math/WrappedTF1.h"
#include "Math/GaussIntegrator.h"

#define tau_Li 1210
#define gamma_Li 6.3018 // In units kHz/mT
#define PI 3.14159265358979323846
#define TWOPI 6.28318530717958647692

ClassImp(ExpRlx)  // for the ROOT-dictionary
ClassImp(SExpRlx)
ClassImp(MLRes)


double ExpRlx::operator()(double x, const vector<double> &par) const {
  assert(par.size()==2); // make sure the number of parameters handed to the function is correct

  // par[0] time of beam off
  // par[1] is the relaxation rate
  double tau_p;
  double y;

  tau_p = (tau_Li/(1.+par[1]*tau_Li));
	       
  if ( x <= par[0] && x >= 0) {
    y=(tau_p/tau_Li)*(1-exp(-x/tau_p))/(1-exp(-x/tau_Li));
  } else if ( x > par[0] ){
    y=(tau_p/tau_Li)*(1-exp(-par[0]/tau_p))/(1-exp(-par[0]/tau_Li))*exp(-par[1]*(x-par[0]));
  }  else {
    y = 0;
  }

  return y;
}

double SExpRlx::operator()(double x, const vector<double> &par) const {
  assert(par.size()==3); // make sure the number of parameters handed to the function is correct
  
  // par[0] time of beam off
  // par[1] is the relaxation rate
  // par[2] is the exponent

  double y;


  if ( x >= 0 && x <= par[0] ) { 
    TF1 sexp("sexp", "exp(-([0]-x)/[3])*exp(-pow(([1]*([0]-x)),[2]))", 0.0, 20000.0);
    sexp.SetParameters(x, par[1], par[2],tau_Li);
    sexp.SetNpx(1000);
    y=sexp.Integral(0.0,x)/(1-exp(-x/tau_Li))/tau_Li;
  } else if ( x > par[0] ) {
    TF1 sexp("sexp", "exp(-([3]-x)/[4])*exp(-pow(([1]*([0]-x)),[2]))", 0.0, 20000.0);
    sexp.SetParameters(x, par[1], par[2], par[0],tau_Li);
    sexp.SetNpx(1000);
    y=sexp.Integral(0.0,par[0])/(1-exp(-par[0]/tau_Li))/tau_Li;
  }  else {
    y = 0;
  }
  return y;
}

double MLRes::operator()(double x, const vector<double> &par) const {
  assert(par.size()==3); // make sure the number of parameters handed to the function is correct
  
  // par[0] time of beam off
  // par[1] is the relaxation rate
  // par[2] is the exponent
  double tau_p;
  double y;

  tau_p = (tau_Li/(1.+par[1]*tau_Li));


  if ( x >= 0 && x <= par[0] ) { 
    TF1 sexp("sexp", "exp(-([0]-x)/[3])*exp(-pow(([1]*([0]-x)),[2]))", 0.0, 10000.0);
    sexp.SetParameters(x, par[1], par[2],tau_Li);
    sexp.SetNpx(1000);
    y=sexp.Integral(0.0,x)/(1-exp(-x/tau_Li))/tau_Li;
  } else if ( x > par[0] ) {
    TF1 sexp("sexp", "exp(-([3]-x)/[4])*exp(-pow(([1]*([0]-x)),[2]))", 0.0, 10000.0);
    sexp.SetParameters(x, par[1], par[2], par[0],tau_Li);
    sexp.SetNpx(1000);
    y=sexp.Integral(0.0,par[0])/(1-exp(-par[0]/tau_Li))/tau_Li;
  }  else {
    y = 0;
  }
  return y;
}

