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

ClassImp(ExpRlx)  // for the ROOT-dictionary
ClassImp(SExpRlx)

double ExpRlx::operator()(double x, const std::vector<double> &par) const {
  assert(par.size()==2); // make sure the number of parameters handed to the function is correct

  // par[0] time of beam on (pulse length) in seconds
  // par[1] is the relaxation rate in 1/s
  double tau_p;
  tau_p = (tau_Li/(1.+par[1]*tau_Li));

  // x should be in seconds, otherwise it should be rescaled here
  if ( x <= par[0] && x >= 0) {
    return (tau_p/tau_Li)*(1-exp(-x/tau_p))/(1-exp(-x/tau_Li));
  } else if ( x > par[0] ){
    return (tau_p/tau_Li)*(1-exp(-par[0]/tau_p))/(1-exp(-par[0]/tau_Li))*exp(-par[1]*(x-par[0]));
  }  
  return 0;
}

//initialize Integrators
TF1 SExpRlx::sexp1=TF1("sexp", "exp(-([0]-x)/[3])*exp(-pow(([1]*([0]-x)),[2]))", 0.0, 20000.0);
TF1 SExpRlx::sexp2=TF1("sexp", "exp(-([3]-x)/[4])*exp(-pow(([1]*([0]-x)),[2]))", 0.0, 20000.0);

double SExpRlx::operator()(double x, const std::vector<double> &par) const {

  assert(par.size()==3); // make sure the number of parameters handed to the function is correct
  
  // par[0] beam of beam on (pulse length) in seconds 
  // par[1] is the relaxation rate in 1/s
  // par[2] is the exponent

  // x should be in seconds, otherwise it should be rescaled here
  if ( x >= 0 && x <= par[0] ) {
    TF1 sexp1("sexp1", "exp(-([0]-x)/[3])*exp(-pow(([1]*([0]-x)),[2]))", 0.0, 20.0);
    sexp1.SetParameters(x, par[1], par[2],tau_Li);
    sexp1.SetNpx(1000);
    return sexp1.Integral(0.0,x)/(1-exp(-x/tau_Li))/tau_Li;
  } else if ( x > par[0] ) {
    TF1 sexp2("sexp2", "exp(-([3]-x)/[4])*exp(-pow(([1]*([0]-x)),[2]))", 0.0, 20.0);
    sexp2.SetParameters(x, par[1], par[2], par[0],tau_Li);
    sexp2.SetNpx(1000);
    return sexp2.Integral(0.0,par[0])/(1-exp(-par[0]/tau_Li))/tau_Li;
  }  
  return 0;
}

