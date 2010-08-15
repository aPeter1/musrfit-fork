/***************************************************************************

  TBofZCalc.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2009/04/25

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

#include "TBofZCalc.h"
#include <omp.h>
#include <cmath>
//#include <iostream>
//#include <algorithm>
#include <cassert>

//------------------
// Method filling the z and B(z)-vectors for the case B(z) should be used numerically
//------------------

void TBofZCalc::Calculate()
{
  if (!fBZ.empty())
    return;

  double ZZ;
  int j;
  fZ.resize(fSteps);
  fBZ.resize(fSteps);

#pragma omp parallel for default(shared) private(j,ZZ) schedule(dynamic)
  for (j=0; j<fSteps; j++) {
    ZZ = fParam[1] + static_cast<double>(j)*fDZ;
    fZ[j] = ZZ;
    fBZ[j] = GetBofZ(ZZ);
  }
  return;
}

// //------------------
// // Method returning the field minimum of the model
// //------------------
// 
// double TBofZCalc::GetBmin() const {
//   vector<double>::const_iterator iter;
//   iter = min_element(fBZ.begin(),fBZ.end());
// 
//   return *iter;
// }
// 
// //------------------
// // Method returning the field maximum of the model
// //------------------
// 
// double TBofZCalc::GetBmax() const {
//   vector<double>::const_iterator iter;
//   iter = max_element(fBZ.begin(),fBZ.end());
// 
//   return *iter;
// }
// 
// //------------------
// // Method returning the field B(z) at a given z according to the model
// //------------------
// 
// double TBofZCalc::GetBofZ(double zz) const {
// 
//   bool found = false;
//   unsigned int i;
//   for (i=0; i<fZ.size(); i++) {
//     if (fZ[i] > zz) {
//      found = true;
//      break;
//     }
//   }
// 
//   if (!found || i == 0) {
//     cout << "TBofZCalc::GetBofZ: B(z) cannot be calculated for z = " << zz << " !" << endl;
//     cout << "TBofZCalc::GetBofZ: Check your theory function!" << endl;
//     return -1.0;
//   }
// 
//   return fBZ[i-1]+(fBZ[i]-fBZ[i-1])*(zz-fZ[i-1])/(fZ[i]-fZ[i-1]);
// 
// }

//------------------
// Constructor of the TLondon1D_HS class
// 1D-London screening in a superconducting half-space
// Parameters: Bext[G], deadlayer[nm], lambda[nm] 
//------------------

TLondon1D_HS::TLondon1D_HS(const vector<double> &param, unsigned int steps)
{
  fSteps = steps;
  fDZ = 200.0/double(steps);
  fParam = param;
}

double TLondon1D_HS::GetBofZ(double ZZ) const
{
  if(ZZ < 0. || ZZ < fParam[1])
    return fParam[0];

  return fParam[0]*exp((fParam[1]-ZZ)/fParam[2]);
}

double TLondon1D_HS::GetBmax() const
{
  // return applied field
  return fParam[0];
}

double TLondon1D_HS::GetBmin() const
{
  // return field minimum
  return fParam[0]*exp((fParam[1]-200.0)/fParam[2]);
}

vector< pair<double, double> > TLondon1D_HS::GetInverseAndDerivative(double BB) const
{
  vector< pair<double, double> > inv;

  if(BB <= 0.0 || BB > fParam[0])
    return inv;

  pair<double, double> invAndDerivative;

  invAndDerivative.first = fParam[1] - fParam[2]*log(BB/fParam[0]);
  invAndDerivative.second = -fParam[2]/BB;

  inv.push_back(invAndDerivative);

  return inv;
}


//------------------
// Constructor of the TLondon1D_1L class
// 1D-London screening in a thin superconducting film
// Parameters: Bext[G], deadlayer[nm], thickness[nm], lambda[nm] 
//------------------

TLondon1D_1L::TLondon1D_1L(const vector<double> &param, unsigned int steps)
{
  fSteps = steps;
  fDZ = param[2]/double(steps);
  fParam = param;
  fMinZ = -1.0;
  fMinB = -1.0;

// thicknesses have to be greater or equal to zero
  for(unsigned int i(1); i<3; i++){
    if(param[i] < 0.){
      fParam[i] = 0.;
    }
  }

// lambdas have to be greater than zero
  if(param[3] < 0.1){
    fParam[3] = 0.1;
  }

// Calculate the coefficients of the exponentials
  double N0(param[0]/(1.0+exp(param[2]/param[3])));

  fCoeff[0]=N0*exp((param[1]+param[2])/param[3]);
  fCoeff[1]=N0*exp(-param[1]/param[3]);

// none of the coefficients should be zero
  for(unsigned int i(0); i<2; i++)
    assert(fCoeff[i]);

  SetBmin();
}

double TLondon1D_1L::GetBofZ(double ZZ) const
{
  if(ZZ < 0. || ZZ < fParam[1] || ZZ > fParam[1]+fParam[2])
    return fParam[0];

  return fCoeff[0]*exp(-ZZ/fParam[3])+fCoeff[1]*exp(ZZ/fParam[3]);
}

double TLondon1D_1L::GetBmax() const
{
  // return applied field
  return fParam[0];
}

double TLondon1D_1L::GetBmin() const
{
  // return field minimum
  return fMinB;
}

void TLondon1D_1L::SetBmin()
{
  double b_a(fCoeff[1]/fCoeff[0]);
// assert(b_a>0.);
  if(b_a<10E-7){
    b_a = 10E-7;
  }

  double minZ;
  // check if the minimum is in the first layer
  minZ=-0.5*fParam[3]*log(b_a);
  if (minZ > fParam[1] && minZ <= fParam[1]+fParam[2]) {
    fMinZ = minZ;
    fMinB = GetBofZ(minZ);
    return;
  }

//  assert(fMinZ > 0. && fMinB > 0.);
  if(fMinZ <= 0.){
    fMinZ = 0.;
  }
  if(fMinB <= 0.){
    fMinB = 0.;
  }

  return;
}

vector< pair<double, double> > TLondon1D_1L::GetInverseAndDerivative(double BB) const
{
  vector< pair<double, double> > inv;

  if(BB <= fMinB || BB > fParam[0])
    return inv;

  double inverse[2];
  pair<double, double> invAndDerivative;

  inverse[0]=fParam[3]*log((BB-sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));
  inverse[1]=fParam[3]*log((BB+sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));

  if(inverse[0] > fParam[1] && inverse[0] < fMinZ) {
    invAndDerivative.first = inverse[0];
    invAndDerivative.second = -fParam[3]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
    inv.push_back(invAndDerivative);
  }
  if(inverse[1] > fMinZ && inverse[1] <= fParam[1]+fParam[2]) {
    invAndDerivative.first = inverse[1];
    invAndDerivative.second = +fParam[3]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
    inv.push_back(invAndDerivative);
  }

  return inv;
}


//------------------
// Constructor of the TLondon1D_2L class
// 1D-London screening in a thin superconducting film, two layers, two lambda
// Parameters: Bext[G], deadlayer[nm], thickness1[nm], thickness2[nm], lambda1[nm], lambda2[nm] 
//------------------

TLondon1D_2L::TLondon1D_2L(const vector<double> &param, unsigned int steps)
{
  fSteps = steps;
  fDZ = (param[2]+param[3])/double(steps);
  fParam = param;
  fMinTag = -1;
  fMinZ = -1.0;
  fMinB = -1.0;

// thicknesses have to be greater or equal to zero
  for(unsigned int i(1); i<4; i++)
    assert(param[i]>=0.);

// lambdas have to be greater than zero
  for(unsigned int i(4); i<6; i++){
    assert(param[i]!=0.);
  }
  fInterfaces[0]=param[1];
  fInterfaces[1]=param[1]+param[2];
  fInterfaces[2]=param[1]+param[2]+param[3];

// Calculate the coefficients of the exponentials
  double B0(param[0]), dd(param[1]), d1(param[2]), d2(param[3]), l1(param[4]), l2(param[5]);

  double N0((1.0+exp(2.0*d1/l1))*(-1.0+exp(2.0*d2/l2))*l1+(-1.0+exp(2.0*d1/l1))*(1.0+exp(2.0*d2/l2))*l2);

  double N1(4.0*(l2*cosh(d2/l2)*sinh(d1/l1)+l1*cosh(d1/l1)*sinh(d2/l2)));

  fCoeff[0]=B0*exp((d1+dd)/l1)*(-2.0*exp(d2/l2)*l2+exp(d1/l1)*(l2-l1)+exp(d1/l1+2.0*d2/l2)*(l1+l2))/N0;

  fCoeff[1]=-B0*exp(-(dd+d1)/l1-d2/l2)*(l1-exp(2.0*d2/l2)*l1+(1.0-2.0*exp(d1/l1+d2/l2)+exp(2.0*d2/l2))*l2)/N1;

  fCoeff[2]=B0*exp((d1+d2+dd)/l2)*(-(1.0+exp(2.0*d1/l1)-2.0*exp(d1/l1+d2/l2))*l1+(-1.0+exp(2.0*d1/l1))*l2)/N0;

  fCoeff[3]=B0*exp(-d1/l1-(d1+d2+dd)/l2)*(-2.0*exp(d1/l1)*l1+exp(d2/l2)*(l1-l2+exp(2.0*d1/l1)*(l1+l2)))/N1;

// none of the coefficients should be zero
  for(unsigned int i(0); i<4; i++)
    assert(fCoeff[i]);

  SetBmin();
}

double TLondon1D_2L::GetBofZ(double ZZ) const
{
  if(ZZ < 0. || ZZ < fInterfaces[0] || ZZ > fInterfaces[2])
    return fParam[0];

  if (ZZ < fInterfaces[1]) {
    return fCoeff[0]*exp(-ZZ/fParam[4])+fCoeff[1]*exp(ZZ/fParam[4]);
  } else {
    return fCoeff[2]*exp(-ZZ/fParam[5])+fCoeff[3]*exp(ZZ/fParam[5]);
  }
}

double TLondon1D_2L::GetBmax() const
{
  // return applied field
  return fParam[0];
}

double TLondon1D_2L::GetBmin() const
{
  // return field minimum
  return fMinB;
}

void TLondon1D_2L::SetBmin()
{
  double b_a(fCoeff[1]/fCoeff[0]);
  assert (b_a>0.);

  double minZ;
  // check if the minimum is in the first layer
  minZ=-0.5*fParam[4]*log(b_a);
  if (minZ > fInterfaces[0] && minZ <= fInterfaces[1]) {
    fMinTag = 1;
    fMinZ = minZ;
    fMinB = GetBofZ(minZ);
    return;
  }

  double d_c(fCoeff[3]/fCoeff[2]);
  assert (d_c>0.);

  // check if the minimum is in the second layer
  minZ=-0.5*fParam[5]*log(d_c);
  if (minZ > fInterfaces[1] && minZ <= fInterfaces[2]) {
    fMinTag = 2;
    fMinZ = minZ;
    fMinB = GetBofZ(minZ);
    return;
  }

  assert(fMinZ > 0. && fMinB > 0.);
  return;
}

vector< pair<double, double> > TLondon1D_2L::GetInverseAndDerivative(double BB) const
{
  vector< pair<double, double> > inv;

  if(BB <= fMinB || BB >= fParam[0])
    return inv;

  double inverse[3];
  pair<double, double> invAndDerivative;

  switch(fMinTag)
  {
    case 1:
      inverse[0]=fParam[4]*log((BB-sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));
      inverse[1]=fParam[4]*log((BB+sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));
      inverse[2]=fParam[5]*log((BB+sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]))/(2.0*fCoeff[3]));

      if(inverse[0] > fInterfaces[0] && inverse[0] < fMinZ) {
        invAndDerivative.first = inverse[0];
        invAndDerivative.second = -fParam[4]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[1] > fMinZ && inverse[1] <= fInterfaces[1]) {
        invAndDerivative.first = inverse[1];
        invAndDerivative.second = +fParam[4]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[2] > fInterfaces[1] && inverse[2] <= fInterfaces[2]) {
        invAndDerivative.first = inverse[2];
        invAndDerivative.second = +fParam[5]/sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]);
        inv.push_back(invAndDerivative);
      }
      break;

    case 2:
      inverse[0]=fParam[4]*log((BB-sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));
      inverse[1]=fParam[5]*log((BB-sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]))/(2.0*fCoeff[3]));
      inverse[2]=fParam[5]*log((BB+sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]))/(2.0*fCoeff[3]));

      if(inverse[0] > fInterfaces[0] && inverse[0] <= fInterfaces[1]) {
        invAndDerivative.first = inverse[0];
        invAndDerivative.second = -fParam[4]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[1] > fInterfaces[1] && inverse[1] < fMinZ) {
        invAndDerivative.first = inverse[1];
        invAndDerivative.second = -fParam[5]/sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[2] > fMinZ && inverse[2] <= fInterfaces[2]) {
        invAndDerivative.first = inverse[2];
        invAndDerivative.second = +fParam[5]/sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]);
        inv.push_back(invAndDerivative);
      }
      break;

    default:
      break;
  }
  return inv;
}

//------------------
// Constructor of the TProximity1D_1LHS class
// 1D-Proximity screening in a thin superconducting film, 1layer+bulk, linear + exponential decay
// Parameters: Bext[G], B1[G], deadlayer[nm], thickness1[nm], lambda[nm]
//------------------

TProximity1D_1LHS::TProximity1D_1LHS(const vector<double> &param, unsigned int steps)
{
  fSteps = steps;
  fDZ = 200./double(steps);
  fMinTag = -1;
  fMinZ = -1.0;
  fMinB = -1.0;

  // replaced the former assertions of positive lengths by fixed parameter limits

  fParam.resize(param.size());
  fParam[0] = param[0]; // Bext
  (param[0] != param[1]) ? fParam[1] = param[1] : fParam[1] = param[1]-0.001; // B1
  (param[2] >= 0.) ? fParam[2] = param[2] : fParam[2] = 0.; // deadlayer
  (param[3] >= 0.) ? fParam[3] = param[3] : fParam[3] = 0.; // thickness1
  (param[3] >= 1.) ? fParam[4] = param[4] : fParam[4] = 1.; // lambda

  fInterfaces[0]=fParam[2];
  fInterfaces[1]=fParam[2]+fParam[3];

  SetBmin();
}

double TProximity1D_1LHS::GetBofZ(double ZZ) const
{
  if(ZZ < fInterfaces[0])
    return fParam[0];

  if (ZZ < fInterfaces[1]) {
    return fParam[0]-(fParam[0]-fParam[1])/fParam[3]*(ZZ-fParam[2]);
  } else {
    return fParam[1]*exp((fParam[2]+fParam[3]-ZZ)/fParam[4]);
  }
}

double TProximity1D_1LHS::GetBmax() const
{
  // return applied field
  return fParam[0];
}

double TProximity1D_1LHS::GetBmin() const
{
  // return field minimum
  return fMinB;
}

void TProximity1D_1LHS::SetBmin()
{
  fMinTag = 2;
  fMinZ = 200.;
  fMinB = GetBofZ(fMinZ);
  return;
}

vector< pair<double, double> > TProximity1D_1LHS::GetInverseAndDerivative(double BB) const
{
  vector< pair<double, double> > inv;

  if(BB <= fMinB || BB > fParam[0])
    return inv;

  double inverse[2];
  pair<double, double> invAndDerivative;

  inverse[0]=(fParam[0]*(fParam[2]+fParam[3])-fParam[1]*fParam[2]-BB*fParam[3])/(fParam[0]-fParam[1]);
  inverse[1]=fParam[2]+fParam[3]-fParam[4]*log(BB/fParam[1]);

  if(inverse[0] > fInterfaces[0] && inverse[0] <= fInterfaces[1]) {
    invAndDerivative.first = inverse[0];
    invAndDerivative.second = fParam[3]/(fParam[1]-fParam[0]);
    inv.push_back(invAndDerivative);
  }
  if(inverse[1] > fInterfaces[1]) {
    invAndDerivative.first = inverse[1];
    invAndDerivative.second = -fParam[4]/BB;
    inv.push_back(invAndDerivative);
  }

  return inv;
}

//------------------
// Constructor of the TLondon1D_3L class
// 1D-London screening in a thin superconducting film, three layers, three lambdas
// Parameters: Bext[G], deadlayer[nm], thickness1[nm], thickness2[nm], thickness3[nm], lambda1[nm], lambda2[nm], lambda3[nm]
//------------------

TLondon1D_3L::TLondon1D_3L(const vector<double> &param, unsigned int steps)
// : fSteps(steps), fDZ((param[2]+param[3]+param[4])/double(steps)), fParam(param), fMinTag(-1), fMinZ(-1.0), fMinB(-1.0)
{
// no members of TLondon1D_3L, therefore the initialization list cannot be used!!
  fSteps = steps;
  fDZ = (param[2]+param[3]+param[4])/double(steps);
  fParam = param;
  fMinTag = -1;
  fMinZ = -1.0;
  fMinB = -1.0;

// thicknesses have to be greater or equal to zero
  for(unsigned int i(1); i<5; i++)
    assert(param[i]>=0.);

// lambdas have to be greater than zero
  for(unsigned int i(5); i<8; i++)
    assert(param[i]!=0.);

  fInterfaces[0]=param[1];
  fInterfaces[1]=param[1]+param[2];
  fInterfaces[2]=param[1]+param[2]+param[3];
  fInterfaces[3]=param[1]+param[2]+param[3]+param[4];

// Calculate the coefficients of the exponentials
  double B0(param[0]), dd(param[1]), d1(param[2]), d2(param[3]), d3(param[4]), l1(param[5]), l2(param[6]), l3(param[7]);

  double N0(4.0*((1.0+exp(2.0*d1/l1))*l1*(l3*cosh(d3/l3)*sinh(d2/l2) + l2*cosh(d2/l2)*sinh(d3/l3))+(-1.0+exp(2.0*d1/l1)) * \
    l2*(l3*cosh(d2/l2)*cosh(d3/l3)+l2*sinh(d2/l2)*sinh(d3/l3))));

  double N2(2.0*l3*cosh(d3/l3)*((-1.0+exp(2.0*d1/l1))*l2*cosh(d2/l2) + (1.0+exp(2.0*d1/l1))*l1*sinh(d2/l2)) + \
    4.0*exp(d1/l1)*l2*(l1*cosh(d1/l1)*cosh(d2/l2) + l2*sinh(d1/l1)*sinh(d2/l2))*sinh(d3/l3));

  double N3(2.0*((1.0+exp(2.0*d1/l1))*l1*(l3*cosh(d3/l3)*sinh(d2/l2) + l2*cosh(d2/l2)*sinh(d3/l3)) + \
    (-1.0+exp(2.0*d1/l1))*l2*(l3*cosh(d2/l2) * cosh(d3/l3)+l2*sinh(d2/l2)*sinh(d3/l3))));

  double N5((1.0+exp(2.0*d1/l1))*l1*(l3*cosh(d3/l3)*sinh(d2/l2) + l2*cosh(d2/l2)*sinh(d3/l3)) + \
    (-1.0+exp(2.0*d1/l1))*l2*(l3*cosh(d2/l2) * cosh(d3/l3)+l2*sinh(d2/l2)*sinh(d3/l3)));

  fCoeff[0]=(B0*exp((d1+dd)/l1-d2/l2-d3/l3) * (exp(d1/l1)*(l1-l2)*(-l2+exp(2.0*d3/l3)*(l2-l3)-l3) - \
    4.0*exp(d2/l2+d3/l3) * l2*l3 + exp(d1/l1+2.0*d2/l2)*(l1+l2) * (-l2+l3+exp(2.0*d3/l3)*(l2+l3))))/N0;

  fCoeff[1]=(B0*exp(-dd/l1-d2/l2-d3/l3)*(-l2*((-1.0+exp(2.0*d2/l2)) * (-1.0+exp(2.0*d3/l3))*l2+(1.0+exp(2.0*d2/l2) - \
    4.0*exp(d1/l1+d2/l2+d3/l3) + exp(2.0*d3/l3)*(1.0+exp(2.0*d2/l2)))*l3) + 4.0*exp(d2/l2+d3/l3)*l1*(l3*cosh(d3/l3) * \
      sinh(d2/l2)+l2*cosh(d2/l2)*sinh(d3/l3))))/N0;

  fCoeff[2]=(B0*exp((d1+dd)/l2)*(-exp(2.0*d1/l1)*(l1-l2)*l3-(l1+l2)*l3 + 2.0*exp(d1/l1+d2/l2)*l1*(l3*cosh(d3/l3)+l2*sinh(d3/l3))))/N2;

  fCoeff[3]=(B0*exp(-(d1+d2+dd)/l2-d3/l3)*(exp(d2/l2+d3/l3) * (l1-l2+exp(2.0*d1/l1)*(l1+l2))*l3 - \
   exp(d1/l1)*l1*(l2+l3+exp(2.0*d3/l3)*(l3-l2))))/N3;

  fCoeff[4]=(0.25*B0*exp(-d2/l2+(d1+d2+dd)/l3) * (4.0*exp(d1/l1+d2/l2+d3/l3)*l1*l2 - \
    (-1.0+exp(2.0*d1/l1))*l2*(-l2+exp(2.0*d2/l2)*(l2-l3)-l3) - (1.0+exp(2.0*d1/l1))*l1*(l2+exp(2.0*d2/l2)*(l2-l3)+l3)))/N5;

  fCoeff[5]=(B0*exp(-(d1+d2+d3+dd)/l3+d1/l1) * (-l1*l2+exp(d3/l3)*(l2*sinh(d1/l1)*(l3*cosh(d2/l2) + \
    l2*sinh(d2/l2)) + l1*cosh(d1/l1) * (l2*cosh(d2/l2)+l3*sinh(d2/l2)))))/N5;

// none of the coefficients should be zero
  for(unsigned int i(0); i<6; i++)
    assert(fCoeff[i]);

  SetBmin();
}

double TLondon1D_3L::GetBofZ(double ZZ) const
{
  if(ZZ < 0. || ZZ < fInterfaces[0] || ZZ > fInterfaces[3])
    return fParam[0];

  if (ZZ < fInterfaces[1]) {
    return fCoeff[0]*exp(-ZZ/fParam[5])+fCoeff[1]*exp(ZZ/fParam[5]);
  } else if (ZZ < fInterfaces[2]) {
    return fCoeff[2]*exp(-ZZ/fParam[6])+fCoeff[3]*exp(ZZ/fParam[6]);
  } else {
    return fCoeff[4]*exp(-ZZ/fParam[7])+fCoeff[5]*exp(ZZ/fParam[7]);
  }
}

double TLondon1D_3L::GetBmax() const
{
  // return applied field
  return fParam[0];
}

double TLondon1D_3L::GetBmin() const
{
  // return field minimum
  return fMinB;
}

void TLondon1D_3L::SetBmin()
{
  double b_a(fCoeff[1]/fCoeff[0]);
  assert (b_a>0.);

  double minZ;
  // check if the minimum is in the first layer
  minZ=-0.5*fParam[5]*log(b_a);
  if (minZ > fInterfaces[0] && minZ <= fInterfaces[1]) {
    fMinTag = 1;
    fMinZ = minZ;
    fMinB = GetBofZ(minZ);
    return;
  }

  double d_c(fCoeff[3]/fCoeff[2]);
  assert (d_c>0.);

  // check if the minimum is in the second layer
  minZ=-0.5*fParam[6]*log(d_c);
  if (minZ > fInterfaces[1] && minZ <= fInterfaces[2]) {
    fMinTag = 2;
    fMinZ = minZ;
    fMinB = GetBofZ(minZ);
    return;
  }

  double f_e(fCoeff[5]/fCoeff[4]);
  assert (f_e>0.);

  // check if the minimum is in the third layer
  minZ=-0.5*fParam[7]*log(f_e);
  if (minZ > fInterfaces[2] && minZ <= fInterfaces[3]) {
    fMinTag = 3;
    fMinZ = minZ;
    fMinB = GetBofZ(minZ);
    return;
  }

  assert(fMinZ > 0. && fMinB > 0.);
  return;
}

vector< pair<double, double> > TLondon1D_3L::GetInverseAndDerivative(double BB) const
{
  vector< pair<double, double> > inv;

  if(BB <= fMinB || BB > fParam[0])
    return inv;

  double inverse[4];
  pair<double, double> invAndDerivative;

  switch(fMinTag)
  {
    case 1:
      inverse[0]=fParam[5]*log((BB-sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));
      inverse[1]=fParam[5]*log((BB+sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));
      inverse[2]=fParam[6]*log((BB+sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]))/(2.0*fCoeff[3]));
      inverse[3]=fParam[7]*log((BB+sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]))/(2.0*fCoeff[5]));

      if(inverse[0] > fInterfaces[0] && inverse[0] < fMinZ) {
        invAndDerivative.first = inverse[0];
        invAndDerivative.second = -fParam[5]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[1] > fMinZ && inverse[1] <= fInterfaces[1]) {
        invAndDerivative.first = inverse[1];
        invAndDerivative.second = +fParam[5]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[2] > fInterfaces[1] && inverse[2] <= fInterfaces[2]) {
        invAndDerivative.first = inverse[2];
        invAndDerivative.second = +fParam[6]/sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[3] > fInterfaces[2] && inverse[3] <= fInterfaces[3]) {
        invAndDerivative.first = inverse[3];
        invAndDerivative.second = +fParam[7]/sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]);
        inv.push_back(invAndDerivative);
      }
      break;

    case 2:
      inverse[0]=fParam[5]*log((BB-sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));
      inverse[1]=fParam[6]*log((BB-sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]))/(2.0*fCoeff[3]));
      inverse[2]=fParam[6]*log((BB+sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]))/(2.0*fCoeff[3]));
      inverse[3]=fParam[7]*log((BB+sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]))/(2.0*fCoeff[5]));

      if(inverse[0] > fInterfaces[0] && inverse[0] <= fInterfaces[1]) {
        invAndDerivative.first = inverse[0];
        invAndDerivative.second = -fParam[5]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[1] > fInterfaces[1] && inverse[1] < fMinZ) {
        invAndDerivative.first = inverse[1];
        invAndDerivative.second = -fParam[6]/sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[2] > fMinZ && inverse[2] <= fInterfaces[2]) {
        invAndDerivative.first = inverse[2];
        invAndDerivative.second = +fParam[6]/sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[3] > fInterfaces[2] && inverse[3] <= fInterfaces[3]) {
        invAndDerivative.first = inverse[3];
        invAndDerivative.second = +fParam[7]/sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]);
        inv.push_back(invAndDerivative);
      }

      break;

    case 3:
      inverse[0]=fParam[5]*log((BB-sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));
      inverse[1]=fParam[6]*log((BB-sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]))/(2.0*fCoeff[3]));
      inverse[2]=fParam[7]*log((BB-sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]))/(2.0*fCoeff[5]));
      inverse[3]=fParam[7]*log((BB+sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]))/(2.0*fCoeff[5]));

      if(inverse[0] > fInterfaces[0] && inverse[0] <= fInterfaces[1]) {
        invAndDerivative.first = inverse[0];
        invAndDerivative.second = -fParam[5]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[1] > fInterfaces[1] && inverse[1] <= fInterfaces[2]) {
        invAndDerivative.first = inverse[1];
        invAndDerivative.second = -fParam[6]/sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[2] > fInterfaces[2] && inverse[2] < fMinZ) {
        invAndDerivative.first = inverse[2];
        invAndDerivative.second = -fParam[7]/sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[3] > fMinZ && inverse[3] <= fInterfaces[3]) {
        invAndDerivative.first = inverse[3];
        invAndDerivative.second = +fParam[7]/sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]);
        inv.push_back(invAndDerivative);
      }

      break;

    default:
      break;
  }
  return inv;
}

//------------------
// Constructor of the TLondon1D_3LS class
// 1D-London screening in a thin superconducting film, three layers, two lambdas (top and bottom layer: lambda1)
// Parameters: Bext[G], deadlayer[nm], thickness1[nm], thickness2[nm], thickness3[nm], lambda1[nm], lambda2[nm] 
//------------------

TLondon1D_3LS::TLondon1D_3LS(const vector<double> &param, unsigned int steps)
{
  fSteps = steps;
  fDZ = (param[2]+param[3]+param[4])/double(steps);
  fParam = param;
  fMinTag = -1;
  fMinZ = -1.0;
  fMinB = -1.0;

// thicknesses have to be greater or equal to zero
  for(unsigned int i(1); i<5; i++)
    assert(param[i]>=0.);

// lambdas have to be greater than zero
  for(unsigned int i(5); i<7; i++){
    assert(param[i]!=0.);
  }
  fInterfaces[0]=param[1];
  fInterfaces[1]=param[1]+param[2];
  fInterfaces[2]=param[1]+param[2]+param[3];
  fInterfaces[3]=param[1]+param[2]+param[3]+param[4];

// Calculate the coefficients of the exponentials
  double B0(param[0]), dd(param[1]), d1(param[2]), d2(param[3]), d3(param[4]), l1(param[5]), l2(param[6]);

  double N0(8.0*(l1*l2*cosh(d2/l2)*sinh((d1+d3)/l1)+(l1*l1*cosh(d1/l1)*cosh(d3/l1)+l2*l2*sinh(d1/l1)*sinh(d3/l1))*sinh(d2/l2)));

  fCoeff[0]=B0*exp((dd-d3)/l1-d2/l2)*(-4.0*exp(d3/l1+d2/l2)*l1*l2-exp(d1/l1)*(l1-l2)*(l1+exp(2.0*d3/l1)*(l1-l2)+l2) + \
   exp(d1/l1+2.0*d2/l2)*(l1+l2)*(l1-l2+exp(2.0*d3/l1)*(l1+l2)))/N0;

  fCoeff[1]=B0*exp(-(d1+d3+dd)/l1-d2/l2)*((1.0+exp(2.0*d3/l1))*(-1.0+exp(2.0*d2/l2))*l1*l1-2.0*(1.0-2.0*exp((d1+d3)/l1+d2/l2) + \
   exp(2.0*d2/l2))*l1*l2-(-1.0+exp(2.0*d3/l1))*(-1.0+exp(2.0*d2/l2))*l2*l2)/N0;

  fCoeff[2]=2.0*B0*exp(-(d1+d3)/l1+(d1+dd)/l2)*l1*(-exp(d3/l1)*(l1+exp(2.0*d1/l1)*(l1-l2)+l2) + \
   exp(d1/l1+d2/l2)*(l1-l2+exp(2.0*d3/l1)*(l1+l2)))/N0;

  fCoeff[3]=2.0*B0*exp(-(d1+d3)/l1-(d1+d2+dd)/l2)*l1*(-exp(d1/l1)*(l1+exp(2.0*d3/l1)*(l1-l2)+l2) + \
   exp(d3/l1+d2/l2)*(l1-l2+exp(2.0*d1/l1)*(l1+l2)))/N0;

  fCoeff[4]=B0*exp((d2+dd)/l1-d2/l2)*((1.0+exp(2.0*d1/l1))*(-1.0+exp(2.0*d2/l2))*l1*l1-2.0*(1.0-2.0*exp((d1+d3)/l1+d2/l2) + \
   exp(2.0*d2/l2))*l1*l2-(-1.0+exp(2.0*d1/l1))*(-1.0+exp(2.0*d2/l2))*l2*l2)/N0;

  fCoeff[5]=B0*exp(-(2.0*d1+d2+d3+dd)/l1-d2/l2)*(-4.0*exp(d1/l1+d2/l2)*l1*l2+exp(d3/l1)*(-l1*l1-exp(2.0*d1/l1)*(l1-l2)*(l1-l2)+l2*l2) + \
   exp(d3/l1+2.0*d2/l2)*(l1*l1-l2*l2+exp(2.0*d1/l1)*(l1+l2)*(l1+l2)))/N0;

// none of the coefficients should be zero
  for(unsigned int i(0); i<6; i++)
    assert(fCoeff[i]);

  SetBmin();
}

double TLondon1D_3LS::GetBofZ(double ZZ) const
{
  if(ZZ < 0. || ZZ < fInterfaces[0] || ZZ > fInterfaces[3])
    return fParam[0];

  if (ZZ < fInterfaces[1]) {
    return fCoeff[0]*exp(-ZZ/fParam[5])+fCoeff[1]*exp(ZZ/fParam[5]);
  } else if (ZZ < fInterfaces[2]) {
    return fCoeff[2]*exp(-ZZ/fParam[6])+fCoeff[3]*exp(ZZ/fParam[6]);
  } else {
    return fCoeff[4]*exp(-ZZ/fParam[5])+fCoeff[5]*exp(ZZ/fParam[5]);
  }
}

double TLondon1D_3LS::GetBmax() const
{
  // return applied field
  return fParam[0];
}

double TLondon1D_3LS::GetBmin() const
{
  // return field minimum
  return fMinB;
}

void TLondon1D_3LS::SetBmin()
{
  double b_a(fCoeff[1]/fCoeff[0]);
  assert (b_a>0.);

  double minZ;
  // check if the minimum is in the first layer
  minZ=-0.5*fParam[5]*log(b_a);
  if (minZ > fInterfaces[0] && minZ <= fInterfaces[1]) {
    fMinTag = 1;
    fMinZ = minZ;
    fMinB = GetBofZ(minZ);
    return;
  }

  double d_c(fCoeff[3]/fCoeff[2]);
  assert (d_c>0.);

  // check if the minimum is in the second layer
  minZ=-0.5*fParam[6]*log(d_c);
  if (minZ > fInterfaces[1] && minZ <= fInterfaces[2]) {
    fMinTag = 2;
    fMinZ = minZ;
    fMinB = GetBofZ(minZ);
    return;
  }

  double f_e(fCoeff[5]/fCoeff[4]);
  assert (f_e>0.);

  // check if the minimum is in the third layer
  minZ=-0.5*fParam[5]*log(f_e);
  if (minZ > fInterfaces[2] && minZ <= fInterfaces[3]) {
    fMinTag = 3;
    fMinZ = minZ;
    fMinB = GetBofZ(minZ);
    return;
  }

  assert(fMinZ > 0. && fMinB > 0.);
  return;
}

vector< pair<double, double> > TLondon1D_3LS::GetInverseAndDerivative(double BB) const
{
  vector< pair<double, double> > inv;

  if(BB <= fMinB || BB > fParam[0])
    return inv;

  double inverse[4];
  pair<double, double> invAndDerivative;

  switch(fMinTag)
  {
    case 1:
      inverse[0]=fParam[5]*log((BB-sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));
      inverse[1]=fParam[5]*log((BB+sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));
      inverse[2]=fParam[6]*log((BB+sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]))/(2.0*fCoeff[3]));
      inverse[3]=fParam[5]*log((BB+sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]))/(2.0*fCoeff[5]));

      if(inverse[0] > fInterfaces[0] && inverse[0] < fMinZ) {
        invAndDerivative.first = inverse[0];
        invAndDerivative.second = -fParam[5]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[1] > fMinZ && inverse[1] <= fInterfaces[1]) {
        invAndDerivative.first = inverse[1];
        invAndDerivative.second = +fParam[5]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[2] > fInterfaces[1] && inverse[2] <= fInterfaces[2]) {
        invAndDerivative.first = inverse[2];
        invAndDerivative.second = +fParam[6]/sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[3] > fInterfaces[2] && inverse[3] <= fInterfaces[3]) {
        invAndDerivative.first = inverse[3];
        invAndDerivative.second = +fParam[5]/sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]);
        inv.push_back(invAndDerivative);
      }
      break;

    case 2:
      inverse[0]=fParam[5]*log((BB-sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));
      inverse[1]=fParam[6]*log((BB-sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]))/(2.0*fCoeff[3]));
      inverse[2]=fParam[6]*log((BB+sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]))/(2.0*fCoeff[3]));
      inverse[3]=fParam[5]*log((BB+sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]))/(2.0*fCoeff[5]));

      if(inverse[0] > fInterfaces[0] && inverse[0] <= fInterfaces[1]) {
        invAndDerivative.first = inverse[0];
        invAndDerivative.second = -fParam[5]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[1] > fInterfaces[1] && inverse[1] < fMinZ) {
        invAndDerivative.first = inverse[1];
        invAndDerivative.second = -fParam[6]/sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[2] > fMinZ && inverse[2] <= fInterfaces[2]) {
        invAndDerivative.first = inverse[2];
        invAndDerivative.second = +fParam[6]/sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[3] > fInterfaces[2] && inverse[3] <= fInterfaces[3]) {
        invAndDerivative.first = inverse[3];
        invAndDerivative.second = +fParam[5]/sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]);
        inv.push_back(invAndDerivative);
      }

      break;

    case 3:
      inverse[0]=fParam[5]*log((BB-sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]))/(2.0*fCoeff[1]));
      inverse[1]=fParam[6]*log((BB-sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]))/(2.0*fCoeff[3]));
      inverse[2]=fParam[5]*log((BB-sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]))/(2.0*fCoeff[5]));
      inverse[3]=fParam[5]*log((BB+sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]))/(2.0*fCoeff[5]));

      if(inverse[0] > fInterfaces[0] && inverse[0] <= fInterfaces[1]) {
        invAndDerivative.first = inverse[0];
        invAndDerivative.second = -fParam[5]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[1] > fInterfaces[1] && inverse[1] <= fInterfaces[2]) {
        invAndDerivative.first = inverse[1];
        invAndDerivative.second = -fParam[6]/sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[2] > fInterfaces[2] && inverse[2] < fMinZ) {
        invAndDerivative.first = inverse[2];
        invAndDerivative.second = -fParam[5]/sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[3] > fMinZ && inverse[3] <= fInterfaces[3]) {
        invAndDerivative.first = inverse[3];
        invAndDerivative.second = +fParam[5]/sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]);
        inv.push_back(invAndDerivative);
      }

      break;

    default:
      break;
  }
  return inv;
}

//------------------
// Constructor of the TLondon1D_3LwInsulator class
// 1D-London screening in a two thin superconducting layers fully insulated by a buffer layer
// Parameters: Bext[G], deadlayer[nm], thickness1[nm], thickness2[nm], thickness3[nm], lambda1[nm], lambda2[nm] 
//------------------

TLondon1D_3LwInsulator::TLondon1D_3LwInsulator(const vector<double> &param, unsigned int steps)
{
  fSteps = steps;
  fDZ = (param[2]+param[3]+param[4])/static_cast<double>(steps);
  fParam = param;
  fMinZ = -1.0;
  fMinB = -1.0;

// thicknesses have to be greater or equal to zero
  for(unsigned int i(1); i<5; i++) {
    if(param[i] < 0.){
      fParam[i] = 0.;
    }
  }

// lambdas have to be greater than zero
  for(unsigned int i(5); i<7; i++) {
    if(param[i] < 0.1){
      fParam[i] = 0.1;
    }
  }

// Calculate the coefficients of the exponentials
  double N0(fParam[0]/(1.0+exp(fParam[2]/fParam[5])));
  double N1(fParam[0]/(1.0+exp(fParam[4]/fParam[6])));

  fCoeff[0]=N0*exp((fParam[1]+fParam[2])/fParam[5]);
  fCoeff[1]=N0*exp(-fParam[1]/fParam[5]);
  fCoeff[2]=N1*exp((fParam[1]+fParam[2]+fParam[3]+fParam[4])/fParam[6]);
  fCoeff[3]=N1*exp(-(fParam[1]+fParam[2]+fParam[3])/fParam[6]);

// none of the coefficients should be zero
  for(unsigned int i(0); i<4; i++)
    assert(fCoeff[i]);

  SetBmin();
}

double TLondon1D_3LwInsulator::GetBofZ(double ZZ) const
{
  if(ZZ < 0. || ZZ < fParam[1] || ZZ > fParam[1]+fParam[2]+fParam[3]+fParam[4] || \
   (ZZ > fParam[1]+fParam[2] && ZZ < fParam[1]+fParam[2]+fParam[3]))
    return fParam[0];

  if(ZZ <= fParam[1]+fParam[2])
    return fCoeff[0]*exp(-ZZ/fParam[5])+fCoeff[1]*exp(ZZ/fParam[5]);
  else
    return fCoeff[2]*exp(-ZZ/fParam[6])+fCoeff[3]*exp(ZZ/fParam[6]);
}

double TLondon1D_3LwInsulator::GetBmax() const
{
  // return applied field
  return fParam[0];
}

double TLondon1D_3LwInsulator::GetBmin() const
{
  // return field minimum
  return fMinB;
}

void TLondon1D_3LwInsulator::SetBmin()
{
  double b_a(fCoeff[1]/fCoeff[0]);
  double d_c(fCoeff[3]/fCoeff[2]);

  if(b_a<1E-7) {
    b_a = 1E-7;
  }
  if(d_c<1E-7) {
    d_c = 1E-7;
  }

  double minZ1, minZ2, temp;
  // check if the minimum is in the first or third layer
  minZ1=-0.5*fParam[5]*log(b_a);
  minZ2=-0.5*fParam[6]*log(d_c);

  if (minZ1 > fParam[1] && minZ1 <= fParam[1]+fParam[2]) {
    fMinZ = minZ1;
    fMinB = GetBofZ(minZ1);
    if (minZ2 > fParam[1]+fParam[2]+fParam[3] && minZ2 <= fParam[1]+fParam[2]+fParam[3]+fParam[4]) {
      temp = GetBofZ(minZ2);
      if (temp < fMinB) {
        fMinZ = minZ2;
        fMinB = temp;
      }
    }
    return;
  } else if (minZ2 > fParam[1]+fParam[2]+fParam[3] && minZ2 <= fParam[1]+fParam[2]+fParam[3]+fParam[4]) {
    fMinZ = minZ2;
    fMinB = GetBofZ(minZ2);
    return;
  }

//  assert(fMinZ > 0. && fMinB > 0.);
  if(fMinZ <= 0.){
    fMinZ = 0.;
  }
  if(fMinB <= 0.){
    fMinB = 0.;
  }

  return;
}



// //------------------
// // Constructor of the TLondon1D_4L class
// // 1D-London screening in a thin superconducting film, four layers, four lambdas
// // Parameters: Bext[G], deadlayer[nm], thickness1[nm], thickness2[nm], thickness3[nm], thickness4[nm],
// // lambda1[nm], lambda2[nm], lambda3[nm], lambda4[nm]
// //------------------
// 
// TLondon1D_4L::TLondon1D_4L(unsigned int steps, const vector<double> &param) {
// 
//   double N1((param[6]+exp(2.0*param[2]/param[6])*(param[6]-param[7])+param[7])*(-1.0*param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])-param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])-param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]))+exp(2.0*param[3]/param[7])*(param[6]-param[7]+exp(2.0*param[2]/param[6])*(param[6]+param[7]))*(param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])+param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9])));
// 
//   double N11(param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])*(param[7]*cosh(param[3]/param[7])-param[6]*sinh(param[3]/param[7]))+param[7]*(-1.0*param[6]*cosh(param[3]/param[7])+param[7]*sinh(param[3]/param[7]))*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])*(-1.0*param[6]*cosh(param[3]/param[7])+param[7]*sinh(param[3]/param[7]))+param[8]*(param[7]*cosh(param[3]/param[7])-param[6]*sinh(param[3]/param[7]))*sinh(param[4]/param[8]))*sinh(param[5]/param[9]));
// 
//   double N12(exp(2.0*(param[2]+param[1])/param[6])*(param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])*(param[7]*cosh(param[3]/param[7])+param[6]*sinh(param[3]/param[7]))+param[7]*(param[6]*cosh(param[3]/param[7])+param[7]*sinh(param[3]/param[7]))*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])*(param[6]*cosh(param[3]/param[7])+param[7]*sinh(param[3]/param[7]))+param[8]*(param[7]*cosh(param[3]/param[7])+param[6]*sinh(param[3]/param[7]))*sinh(param[4]/param[8]))*sinh(param[5]/param[9])));
// 
//   double N2((param[6]+param[7]+(param[6]-param[7])*exp(2.0*param[2]/param[6]))*(-1.0*param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])-param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])-param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]))+exp(2.0*param[3]/param[7])*(param[6]-param[7]+exp(2.0*param[2]/param[6])*(param[6]+param[7]))*(param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])+param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9])));
// 
//   double N21(exp(param[3]/param[7])*param[8]*param[9]*(param[6]*cosh(param[2]/param[6])+param[7]*sinh(param[2]/param[6]))+param[6]*param[9]*cosh(param[5]/param[9])*(-1.0*param[8]*cosh(param[4]/param[8])+param[7]*sinh(param[4]/param[8]))+param[6]*param[8]*(param[7]*cosh(param[4]/param[8])-param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]));
// 
//   double N22(exp((2.0*param[2]+param[3]+2.0*param[1])/param[7])*(-1.0*param[6]*param[8]*param[9]*cosh(param[2]/param[6])+param[7]*param[8]*param[9]*sinh(param[2]/param[6])+exp(param[3]/param[7])*param[6]*param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])+param[7]*sinh(param[4]/param[8]))+exp(param[3]/param[7])*param[6]*param[8]*(param[7]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9])));
// 
//   double N3(2.0*((param[6]+exp(2.0*param[2]/param[6])*(param[6]-param[7])+param[7])*(-1.0*param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])-param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])-param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]))+exp(2.0*param[3]/param[7])*(param[6]-param[7]+exp(2.0*param[2]/param[6])*(param[6]+param[7]))*(param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])+param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]))));
// 
//   double N4(4.0*((param[6]+exp(2.0*param[2]/param[6])*(param[6]-param[7])+param[7])*(-1.0*param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])-param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])-param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]))+exp(2.0*param[3]/param[7])*(param[6]-param[7]+exp(2.0*param[2]/param[6])*(param[6]+param[7]))*(param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])+param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]))));
// 
//   double N42(-1.0*param[6]*param[7]*param[8]+exp(param[5]/param[9])*(param[6]*cosh(param[2]/param[6])*(param[8]*sinh(param[3]/param[7])*(param[9]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))+param[7]*cosh(param[3]/param[7])*(param[8]*cosh(param[4]/param[8])+param[9]*sinh(param[4]/param[8])))+param[7]*sinh(param[2]/param[6])*(param[8]*cosh(param[3]/param[7])*(param[9]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))+param[7]*sinh(param[3]/param[7])*(param[8]*cosh(param[4]/param[8])+param[9]*sinh(param[4]/param[8])))));
// 
//   fDZ = (param[2]+param[3]+param[4]+param[5])/double(steps);
//   double ZZ, BBz;
// 
//   for (unsigned int j(0); j<steps; j++) {
//     ZZ = param[1] + (double)j*fDZ;
//     fZ.push_back(ZZ);
//     if (ZZ < param[1]+param[2]) {
//       BBz = (2.0*param[0]*exp(-(param[1]+ZZ)/param[6]+param[3]/param[7])*(-1.0*exp(param[2]/param[6])*(exp(2.0*param[1]/param[6])-exp(2.0*ZZ/param[6]))*param[7]*param[8]*param[9]-exp(2.0*ZZ/param[6])*N11+N12))/N1;
//     } else if (ZZ < param[1]+param[2]+param[3]) {
//       BBz = (2.0*param[0]*exp(param[2]/param[6]-(param[2]+param[1]+ZZ)/param[7])*(exp(2.0*ZZ/param[7])*N21+N22))/N2;
//     } else if (ZZ < param[1]+param[2]+param[3]+param[4]) {
//       BBz = (param[0]*exp(-1.0*(param[2]+param[3]+param[1]+ZZ)/param[8]-param[5]/param[9])*(2.0*exp(param[2]/param[6]+param[3]/param[7]+(2.0*param[2]+2.0*param[3]+param[4]+2.0*param[1])/param[8])*param[6]*param[7]*(param[9]-param[8]+exp(2.0*param[5]/param[9])*(param[8]+param[9]))+4.0*exp(param[2]/param[6]+param[3]/param[7]-param[4]/param[8]+param[5]/param[9])*(-1.0*exp((2.0*param[2]+2.0*param[3]+param[4]+2.0*param[1])/param[8])*param[9]*(param[7]*sinh(param[2]/param[6])*(-1.0*param[8]*cosh(param[3]/param[7])+param[7]*sinh(param[3]/param[7]))+param[6]*cosh(param[2]/param[6])*(param[7]*cosh(param[3]/param[7])-param[8]*sinh(param[3]/param[7])))+exp(2.0*ZZ/param[8])*(exp(param[4]/param[8])*param[9]*(param[7]*sinh(param[2]/param[6])*(param[8]*cosh(param[3]/param[7])+param[7]*sinh(param[3]/param[7]))+param[6]*cosh(param[2]/param[6])*(param[7]*cosh(param[3]/param[7])+param[8]*sinh(param[3]/param[7])))+param[6]*param[7]*(-1.0*param[9]*cosh(param[5]/param[9])+param[8]*sinh(param[5]/param[9]))))))/N3;
//     } else {
//       BBz = (param[0]*exp(-1.0*(param[2]+param[3]+param[4]+param[1]+ZZ)/param[9])*(-1.0*exp(-1.0*param[4]/param[8]+2.0*(param[2]+param[3]+param[4]+param[1])/param[9])*((-1.0+exp(2.0*param[2]/param[6]))*param[7]*(exp(2.0*param[4]/param[8])*(param[8]-param[7]+exp(2.0*param[3]/param[7])*(param[7]+param[8]))*(param[8]-param[9])+exp(2.0*param[3]/param[7])*(param[7]-param[8])*(param[8]+param[9])-(param[7]+param[8])*(param[8]+param[9]))+param[6]*(-8.0*exp(param[2]/param[6]+param[3]/param[7]+param[4]/param[8]+param[5]/param[9])*param[7]*param[8]+(1.0+exp(2.0*param[2]/param[6]))*(-1.0+exp(2.0*param[3]/param[7]))*param[8]*(-1.0*param[8]+exp(2.0*param[4]/param[8])*(param[8]-param[9])-param[9])+(1.0+exp(2.0*param[2]/param[6]))*(1.0+exp(2.0*param[3]/param[7]))*param[7]*(param[8]+exp(2.0*param[4]/param[8])*(param[8]-param[9])+param[9])))+8.0*exp(param[2]/param[6]+param[3]/param[7]-(param[5]-2.0*ZZ)/param[9])*N42))/N4;
//     }
//     fBZ.push_back(BBz);
//   }
// }
