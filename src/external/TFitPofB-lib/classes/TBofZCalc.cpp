/***************************************************************************

  TBofZCalc.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/06/30

***************************************************************************/

#include "TBofZCalc.h"
#include <omp.h>
#include <cmath>
#include <iostream>
#include <algorithm>

vector<double> DataZ() const{
  if (fZ.empty)
    Calculate();
  return fZ;
}

vector<double> DataBZ() const{
  if (fBZ.empty)
    Calculate();
  return fBZ;
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

TLondon1D_HS::TLondon1D_HS(unsigned int steps, const vector<double> &param) {

  fDZ = 200.0/double(steps);
  double ZZ, BBz;

  for (unsigned int j(0); j<steps; j++) {
    ZZ = param[1] + (double)j*fDZ;
    fZ.push_back(ZZ);
    BBz = param[0]*exp(-(ZZ-param[1])/param[2]);
    fBZ.push_back(BBz);
  }

}

//------------------
// Constructor of the TLondon1D_1L class
// 1D-London screening in a thin superconducting film
// Parameters: Bext[G], deadlayer[nm], thickness[nm], lambda[nm] 
//------------------

TLondon1D_1L::TLondon1D_1L(unsigned int steps, const vector<double> &param) {

  double N(cosh(param[2]/2.0/param[3]));

  fDZ = param[2]/double(steps);
  double ZZ, BBz;

  for (unsigned int j(0); j<steps; j++) {
    ZZ = param[1] + (double)j*fDZ;
    fZ.push_back(ZZ);
    BBz = param[0]*cosh((param[2]/2.0-(ZZ-param[1]))/param[3])/N;
    fBZ.push_back(BBz);
  }

}

//------------------
// Constructor of the TLondon1D_2L class
// 1D-London screening in a thin superconducting film, two layers, two lambda
// Parameters: Bext[G], deadlayer[nm], thickness1[nm], thickness2[nm], lambda1[nm], lambda2[nm] 
//------------------

TLondon1D_2L::TLondon1D_2L(unsigned int steps, const vector<double> &param) {

  double N1(param[5]*cosh(param[3]/param[5])*sinh(param[2]/param[4]) + param[4]*cosh(param[2]/param[4])*sinh(param[3]/param[5]));
  double N2(4.0*N1);

  fDZ = (param[2]+param[3])/double(steps);
  double ZZ, BBz;

  for (unsigned int j(0); j<steps; j++) {
    ZZ = param[1] + (double)j*fDZ;
    fZ.push_back(ZZ);
    if (ZZ < param[1]+param[2]) {
      BBz = param[0]*(param[4]*cosh((param[2]+param[1]-ZZ)/param[4])*sinh(param[3]/param[5]) - param[5]*sinh((param[1]-ZZ)/param[4]) +  param[5]*cosh(param[3]/param[5])*sinh((param[2]+param[1]-ZZ)/param[4]))/N1;
    } else {
      BBz = param[0]*(exp(-param[2]/param[4]-(param[2]+param[3]+param[1]+ZZ)/param[5]) * (exp((param[3]+2.0*param[1])/param[5])*(exp(2.0*param[2]*(param[4]+param[5])/param[4]/param[5]) * (param[5]-param[4]) - exp(2.0*param[2]/param[5]) * (param[4]-2.0*param[4]*exp(param[2]/param[4]+param[3]/param[5])+param[5])) +  exp(2.0*ZZ/param[5])*((param[4]-param[5]+(param[4]+param[5]) * exp(2.0*param[2]/param[4]))*exp(param[3]/param[5])-2.0*param[4]*exp(param[2]/param[4]))))/N2;
    }
    fBZ.push_back(BBz);
  }

}

//------------------
// Constructor of the TLondon1D_3L class
// 1D-London screening in a thin superconducting film, three layers, three lambdas
// Parameters: Bext[G], deadlayer[nm], thickness1[nm], thickness2[nm], thickness3[nm], lambda1[nm], lambda2[nm], lambda3[nm]
//------------------

TLondon1D_3L::TLondon1D_3L(unsigned int steps, const vector<double> &param)
 : fSteps(steps), fDZ((param[2]+param[3]+param[4])/double(steps)), fParam(param), fMinTag(-1), fMinZ(-1.0), fMinB(-1.0)
{
// thicknesses have to be greater or equal to zero
  for(unsigned int i(1); i<5; i++)
    assert(param[i]>=0.);

// lambdas have to be greater than zero
  for(unsigned int i(5); i<param.size(); i++)
    assert(param[i]>0.);

  fInterfaces={param[1], param[1]+param[2], param[1]+param[2]+param[3], param[1]+param[2]+param[3]+param[4]};

// Calculate the coefficients of the exponentials
  double N0(4.0*((1.0+exp(2.0*param[2]/param[5]))*param[5]*(param[7]*cosh(param[4]/param[7])*sinh(param[3]/param[6]) + \
   param[6]*cosh(param[3]/param[6])*sinh(param[4]/param[7]))+(-1.0+exp(2.0*param[2]/param[5])) * \
    param[6]*(param[7]*cosh(param[3]/param[6])*cosh(param[4]/param[7])+param[6]*sinh(param[3]/param[6])*sinh(param[4]/param[7]))));

  double N2(2.0*param[7]*cosh(param[4]/param[7])*((-1.0+exp(2.0*param[2]/param[5]))*param[6]*cosh(param[3]/param[6]) + \
   (1.0+exp(2.0*param[2]/param[5]))*param[5]*sinh(param[3]/param[6])) + \
    4.0*exp(param[2]/param[5])*param[6]*(param[5]*cosh(param[2]/param[5])*cosh(param[3]/param[6]) + \
     param[6]*sinh(param[2]/param[5])*sinh(param[3]/param[6]))*sinh(param[4]/param[7]));

  double N3(2.0*((1.0+exp(2.0*param[2]/param[5]))*param[5]*(param[7]*cosh(param[4]/param[7])*sinh(param[3]/param[6]) + \
   param[6]*cosh(param[3]/param[6])*sinh(param[4]/param[7])) + \
    (-1.0+exp(2.0*param[2]/param[5]))*param[6]*(param[7]*cosh(param[3]/param[6]) * \
     cosh(param[4]/param[7])+param[6]*sinh(param[3]/param[6])*sinh(param[4]/param[7]))));

  double N5((1.0+exp(2.0*param[2]/param[5]))*param[5]*(param[7]*cosh(param[4]/param[7])*sinh(param[3]/param[6]) + \
   param[6]*cosh(param[3]/param[6])*sinh(param[4]/param[7])) + \
    (-1.0+exp(2.0*param[2]/param[5]))*param[6]*(param[7]*cosh(param[3]/param[6]) * \
     cosh(param[4]/param[7])+param[6]*sinh(param[3]/param[6])*sinh(param[4]/param[7])));

  fCoeff[0]=(B0*exp((param[2]+param[1])/param[5]-param[3]/param[6]-param[4]/param[7]) * \
   (exp(param[2]/param[5])*(param[5]-param[6])*(-param[6]+exp(2.0*param[4]/param[7])*(param[6]-param[7])-param[7]) - \
    4.0*exp(param[3]/param[6]+param[4]/param[7]) * param[6]*param[7] + \
     exp(param[2]/param[5]+2.0*param[3]/param[6])*(param[5]+param[6]) * \
      (-param[6]+param[7]+exp(2.0*param[4]/param[7])*(param[6]+param[7]))))/N0;

  fCoeff[1]=(B0*exp(-param[1]/param[5]-param[3]/param[6]-param[4]/param[7])*(-param[6]*((-1.0+exp(2.0*param[3]/param[6])) * \
   (-1.0+exp(2.0*param[4]/param[7]))*param[6]+(1.0+exp(2.0*param[3]/param[6]) - \
    4.0*exp(param[2]/param[5]+param[3]/param[6]+param[4]/param[7]) + exp(2.0*param[4]/param[7])*(1.0+exp(2.0*param[3]/param[6])))*param[7]) + \
     4.0*exp(param[3]/param[6]+param[4]/param[7])*param[5]*(param[7]*cosh(param[4]/param[7]) * \
      sinh(param[3]/param[6])+param[6]*cosh(param[3]/param[6])*sinh(param[4]/param[7]))))/N0;

  fCoeff[2]=(B0*exp((param[2]+param[1])/param[6])*(-exp(2.0*param[2]/param[5])*(param[5]-param[6])*param[7]-(param[5]+param[6])*param[7] + \
   2.0*exp(param[2]/param[5]+param[3]/param[6])*param[5]*(param[7]*cosh(param[4]/param[7])+param[6]*sinh(param[4]/param[7]))))/N2;

  fCoeff[3]=(B0*exp(-(param[2]+param[3]+param[1])/param[6]-param[4]/param[7])*(exp(param[3]/param[6]+param[4]/param[7]) * \
   (param[5]-param[6]+exp(2.0*param[2]/param[5])*(param[5]+param[6]))*param[7] - \
    exp(param[2]/param[5])*param[5]*(param[6]+param[7]+exp(2.0*param[4]/param[7])*(param[7]-param[6]))))/N3;

  fCoeff[4]=(0.25*B0*exp(-param[3]/param[6]+(param[2]+param[3]+param[1])/param[7]) * \
   (4.0*exp(param[2]/param[5]+param[3]/param[6]+param[4]/param[7])*param[5]*param[6] - \
    (-1.0+exp(2.0*param[2]/param[5]))*param[6]*(-param[6]+exp(2.0*param[3]/param[6])*(param[6]-param[7])-param[7]) - \
     (1.0+exp(2.0*param[2]/param[5])*param[5]*(param[6]+exp(2.0*param[3]/param[6])*(param[6]-param[7])+param[7])))/N5;

  fCoeff[5]=(B0*exp(-(param[2]+param[3]+param[4]+param[1])/param[7]+param[2]/param[5]) * \
   (-param[5]*param[6]+exp(param[4]/param[7])*(param[6]*sinh(param[2]/param[5])*(param[7]*cosh(param[3]/param[6]) + \
    param[6]*sinh(param[3]/param[6])) + param[5]*cosh(param[2]/param[5]) * \
     (param[6]*cosh(param[3]/param[6])+param[7]*sinh(param[3]/param[6])))))/N5;

// none of the coefficients should be zero
  for(unsigned int i(0); i<6; i++)
    assert(fCoeff[i]);

  SetBmin();
}

void TLondon1D_3L::Calculate()
{
  if (!fBZ.empty())
    return;

  double ZZ;
  int j;
  fZ.resize(fSteps);
  fBZ.resize(fSteps);

#pragma omp parallel for default(shared) private(j,ZZ) schedule(dynamic)
  for (j=0; j<fSteps; j++) {
    ZZ = fParam[1] + (double)j*fDZ;
    fZ[j] = ZZ;
    fBZ[j] = GetBofZ(ZZ);
  }
  return;
}

double TLondon1D_3L::GetBofZ(double ZZ) const
{
  if(ZZ < 0. || ZZ < fInterfaces[0] || ZZ > fInterfaces[3])
    return fParam[0];

  double N1(fParam[7]*cosh(fParam[4]/fParam[7])*((exp(2.0*fParam[2]/fParam[5])-1.0)*fParam[6]*cosh(fParam[3]/fParam[6]) + \
   (1.0+exp(2.0*fParam[2]/fParam[5]))*fParam[5]*sinh(fParam[3]/fParam[6])) + \
    2.0*exp(fParam[2]/fParam[5])*fParam[6]*(fParam[5]*cosh(fParam[2]/fParam[5])*cosh(fParam[3]/fParam[6]) + \
     fParam[6]*sinh(fParam[2]/fParam[5])*sinh(fParam[3]/fParam[6]))*sinh(fParam[4]/fParam[7]));

  double N21(2.0*(fParam[7]*cosh(fParam[4]/fParam[7])*((-1.0+exp(2.0*fParam[2]/fParam[5]))*fParam[6]*cosh(fParam[3]/fParam[6]) + \
   (1.0+exp(2.0*fParam[2]/fParam[5]))*fParam[5]*sinh(fParam[3]/fParam[6])) + \
    2.0*exp(fParam[2]/fParam[5])*fParam[6]*(fParam[5]*cosh(fParam[2]/fParam[5])*cosh(fParam[3]/fParam[6]) + \
     fParam[6]*sinh(fParam[2]/fParam[5])*sinh(fParam[3]/fParam[6]))*sinh(fParam[4]/fParam[7])));

  double N22(((fParam[5]+exp(2.0*fParam[2]/fParam[5])*(fParam[5]-fParam[6])+fParam[6])*(-fParam[7]*cosh(fParam[4]/fParam[7]) + \
   fParam[6]*sinh(fParam[4]/fParam[7]))+exp(2.0*fParam[3]/fParam[6])*(fParam[5]-fParam[6] + \
    exp(2.0*fParam[2]/fParam[5])*(fParam[5]+fParam[6]))*(fParam[7]*cosh(fParam[4]/fParam[7])+fParam[6]*sinh(fParam[4]/fParam[7]))));

  double N3(4.0*((1.0+exp(2.0*fParam[2]/fParam[5]))*fParam[5]*(fParam[7]*cosh(fParam[4]/fParam[7])*sinh(fParam[3]/fParam[6]) + \
   fParam[6]*cosh(fParam[3]/fParam[6])*sinh(fParam[4]/fParam[7])) + \
    (-1.0+exp(2.0*fParam[2]/fParam[5]))*fParam[6]*(fParam[7]*cosh(fParam[3]/fParam[6])*cosh(fParam[4]/fParam[7]) + \
     fParam[6]*sinh(fParam[3]/fParam[6])*sinh(fParam[4]/fParam[7]))));

  if (ZZ < fInterfaces[1]) {
    return (2.0*fParam[0]*exp(fParam[2]/fParam[5])*(-fParam[6]*fParam[7]*sinh((fParam[1]-ZZ)/fParam[5]) + \
     fParam[7]*cosh(fParam[4]/fParam[7])*(fParam[6]*cosh(fParam[3]/fParam[6])*sinh((fParam[2]+fParam[1]-ZZ)/fParam[5]) + \
      fParam[5]*cosh((fParam[2]+fParam[1]-ZZ)/fParam[5])*sinh(fParam[3]/fParam[6])) + \
       fParam[6]*(fParam[5]*cosh((fParam[2]+fParam[1]-ZZ)/fParam[5])*cosh(fParam[3]/fParam[6]) + \
        fParam[6]*sinh((fParam[2]+fParam[1]-ZZ)/fParam[5])*sinh(fParam[3]/fParam[6]))*sinh(fParam[4]/fParam[7])))/N1;
  } else if (ZZ < fInterfaces[2]) {
    return (fParam[0]*exp((fParam[2]+fParam[1]-ZZ)/fParam[6])*(-fParam[5]-fParam[6]+exp(2.0*fParam[2]/fParam[5])*(fParam[6]-fParam[5]))*fParam[7])/N21 + \
     (fParam[0]*exp(-(fParam[2]+fParam[1]+ZZ)/fParam[6])*(exp((fParam[3]+2.0*ZZ)/fParam[6])*(fParam[5]-fParam[6] + \
      exp(2.0*fParam[2]/fParam[5])*(fParam[5]+fParam[6]))*fParam[7] + \
       2.0*exp(fParam[2]/fParam[5])*fParam[5]*(exp(2.0*ZZ/fParam[6])*(-fParam[7]*cosh(fParam[4]/fParam[7]) + \
        fParam[6]*sinh(fParam[4]/fParam[7]))+(cosh(2.0*(fParam[2]+fParam[3]+fParam[1])/fParam[6]) + \
         sinh(2.0*(fParam[2]+fParam[3]+fParam[1])/fParam[6]))*(fParam[7]*cosh(fParam[4]/fParam[7])+fParam[6]*sinh(fParam[4]/fParam[7])))))/N22;
  } else {
    return (fParam[0]*exp(-(fParam[2]+fParam[3]+fParam[1]+ZZ)/fParam[7])*(-exp(-fParam[3]/fParam[6] + \
     2.0*(fParam[2]+fParam[3]+fParam[1])/fParam[7])*(-4.0*exp(fParam[2]/fParam[5]+fParam[3]/fParam[6]+fParam[4]/fParam[7])*fParam[5]*fParam[6] + \
      (-1.0+exp(2.0*fParam[2]/fParam[5]))*fParam[6]*(-fParam[6]+exp(2.0*fParam[3]/fParam[6])*(fParam[6]-fParam[7])-fParam[7]) + \
       (1.0+exp(2.0*fParam[2]/fParam[5]))*fParam[5]*(fParam[6]+exp(2.0*fParam[3]/fParam[6])*(fParam[6]-fParam[7])+fParam[7])) + \
        4.0*exp(fParam[2]/fParam[5]-(fParam[4]-2.0*ZZ)/fParam[7])*(-fParam[5]*fParam[6]+exp(fParam[4]/fParam[7]) * \
         (fParam[6]*sinh(fParam[2]/fParam[5])*(fParam[7]*cosh(fParam[3]/fParam[6])+fParam[6]*sinh(fParam[3]/fParam[6])) + \
          fParam[5]*cosh(fParam[2]/fParam[5])*(fParam[6]*cosh(fParam[3]/fParam[6])+fParam[7]*sinh(fParam[3]/fParam[6]))))))/N3;
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

  assert(fminZ > 0. && fminB > 0.);
  return;
}

vector< pair<double, double> > TLondon1D_3L::GetInverseAndDerivative(double BB) const
{
  vector< pair<double, double> > inv;

  if(BB <= fminB || BB > fParam[0])
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

      if(inverse[0] > fInterfaces[0] && inverse[0] < fminZ) {
        invAndDerivative.first = inverse[0];
        invAndDerivative.second = -fParam[5]/sqrt(BB*BB-4.0*fCoeff[0]*fCoeff[1]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[1] > fminZ && inverse[1] <= fInterfaces[1]) {
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
      if(inverse[1] > fInterfaces[1] && inverse[1] < fminZ) {
        invAndDerivative.first = inverse[1];
        invAndDerivative.second = -fParam[6]/sqrt(BB*BB-4.0*fCoeff[2]*fCoeff[3]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[2] > fminZ && inverse[2] <= fInterfaces[2]) {
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
      if(inverse[2] > fInterfaces[2] && inverse[2] < fminZ) {
        invAndDerivative.first = inverse[2];
        invAndDerivative.second = -fParam[7]/sqrt(BB*BB-4.0*fCoeff[4]*fCoeff[5]);
        inv.push_back(invAndDerivative);
      }
      if(inverse[3] > fminZ && inverse[3] <= fInterfaces[3]) {
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

TLondon1D_3LS::TLondon1D_3LS(unsigned int steps, const vector<double> &param) {

  double N1(8.0*(param[5]*param[6]*cosh(param[3]/param[6])*sinh((param[2]+param[4])/param[5]) + ((param[5]*param[5]*cosh(param[2]/param[5])*cosh(param[4]/param[5])) +  (param[6]*param[6]*sinh(param[2]/param[5])*sinh(param[4]/param[5])))*sinh(param[3]/param[6])));

  double N2(2.0*param[5]*param[6]*cosh(param[3]/param[6])*sinh((param[2]+param[4])/param[5]) + 2.0*(param[5]*param[5]*cosh(param[2]/param[5])*cosh(param[4]/param[5]) + param[6]*param[6]*sinh(param[2]/param[5])*sinh(param[4]/param[5]))*sinh(param[3]/param[6]));

  double N3(8.0*(param[5]*param[6]*cosh(param[3]/param[6])*sinh((param[2]+param[4])/param[5]) + (param[5]*param[5]*cosh(param[2]/param[5])*cosh(param[4]/param[5]) + param[6]*param[6]*sinh(param[2]/param[5])*sinh(param[4]/param[5]))*sinh(param[3]/param[6])));

  fDZ = (param[2]+param[3]+param[4])/double(steps);
  double ZZ, BBz;

  for (unsigned int j(0); j<steps; j++) {
    ZZ = param[1] + (double)j*fDZ;
    fZ.push_back(ZZ);
    if (ZZ < param[1]+param[2]) {
      BBz = (param[0]*exp(-1.0*((param[2]+param[4]+param[1]+ZZ)/param[5]+(param[3]/param[6]))) *
        (-4.0*exp((param[2]+param[4])/param[5]+(param[3]/param[6]))*(exp((2.0*param[1])/param[5]) - exp((2.0*ZZ)/param[5]))*param[5]*param[6]+exp((2.0*param[3])/param[6])*(param[5]-param[6] + (exp((2.0*param[4])/param[5])*(param[5]+param[6])))*((exp((2.0*ZZ)/param[5])*(param[5]-param[6])) + (exp((2.0*(param[2]+param[1]))/param[5])*(param[5]+param[6]))) - 4.0*((param[5]*cosh(param[4]/param[5]))-(param[6]*sinh(param[4]/param[5])))*((param[5]*cosh((param[2]+param[1]-ZZ)/param[5])) - (param[6]*sinh((param[2]+param[1]-ZZ)/param[5])))*(cosh((param[2]+param[4]+param[1]+ZZ)/param[5]) + sinh((param[2]+param[4]+param[1]+ZZ)/param[5]))))/N1;
    } else if (ZZ < param[1]+param[2]+param[3]) {
      BBz = (param[0]*param[5]*(2.0*param[6]*cosh((param[2]+param[3]+param[1]-ZZ)/param[6])*sinh(param[4]/param[5]) + (param[5]+param[6])*sinh(param[2]/param[5]-(param[2]+param[1]-ZZ)/param[6]) - (param[5]-param[6])*sinh(param[2]/param[5]+(param[2]+param[1]-ZZ)/param[6]) + 2.0*param[5]*cosh(param[4]/param[5])*sinh((param[2]+param[3]+param[1]-ZZ)/param[6])))/N2;
    } else {
      BBz = (param[0]*exp(-((2.0*param[2]+param[3]+param[4]+param[1]+ZZ)/param[5])-param[3]/param[6]) * (4.0*exp(param[2]/param[5]+param[3]/param[6])*param[5]*param[6]*((-1.0)*exp(2.0*ZZ/param[5]) + cosh(2.0*(param[2]+param[3]+param[4]+param[1])/param[5])+sinh(2.0*(param[2]+param[3]+param[4]+param[1])/param[5])) + 4.0*exp(((2.0*param[2]+param[3]+param[4]+param[1]+ZZ)/param[5]) + ((2.0*param[3])/param[6]))*(param[5]*cosh(param[2]/param[5]) + param[6]*sinh(param[2]/param[5]))*(param[5]*cosh((param[2]+param[3]+param[1]-ZZ)/param[5]) - param[6]*sinh((param[2]+param[3]+param[1]-ZZ)/param[5])) - 4.0*(param[5]*cosh(param[2]/param[5])-param[6]*sinh(param[2]/param[5])) * (param[5]*cosh((param[2]+param[3]+param[1]-ZZ)/param[5]) + param[6]*sinh((param[2]+param[3]+param[1]-ZZ)/param[5]))*(cosh((2.0*param[2]+param[3]+param[4]+param[1]+ZZ)/param[5]) + sinh((2.0*param[2]+param[3]+param[4]+param[1]+ZZ)/param[5]))))/N3;
    }
    fBZ.push_back(BBz);
  }
}

//------------------
// Constructor of the TLondon1D_4L class
// 1D-London screening in a thin superconducting film, four layers, four lambdas
// Parameters: Bext[G], deadlayer[nm], thickness1[nm], thickness2[nm], thickness3[nm], thickness4[nm],
// lambda1[nm], lambda2[nm], lambda3[nm], lambda4[nm]
//------------------

TLondon1D_4L::TLondon1D_4L(unsigned int steps, const vector<double> &param) {

  double N1((param[6]+exp(2.0*param[2]/param[6])*(param[6]-param[7])+param[7])*(-1.0*param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])-param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])-param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]))+exp(2.0*param[3]/param[7])*(param[6]-param[7]+exp(2.0*param[2]/param[6])*(param[6]+param[7]))*(param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])+param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9])));

  double N11(param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])*(param[7]*cosh(param[3]/param[7])-param[6]*sinh(param[3]/param[7]))+param[7]*(-1.0*param[6]*cosh(param[3]/param[7])+param[7]*sinh(param[3]/param[7]))*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])*(-1.0*param[6]*cosh(param[3]/param[7])+param[7]*sinh(param[3]/param[7]))+param[8]*(param[7]*cosh(param[3]/param[7])-param[6]*sinh(param[3]/param[7]))*sinh(param[4]/param[8]))*sinh(param[5]/param[9]));

  double N12(exp(2.0*(param[2]+param[1])/param[6])*(param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])*(param[7]*cosh(param[3]/param[7])+param[6]*sinh(param[3]/param[7]))+param[7]*(param[6]*cosh(param[3]/param[7])+param[7]*sinh(param[3]/param[7]))*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])*(param[6]*cosh(param[3]/param[7])+param[7]*sinh(param[3]/param[7]))+param[8]*(param[7]*cosh(param[3]/param[7])+param[6]*sinh(param[3]/param[7]))*sinh(param[4]/param[8]))*sinh(param[5]/param[9])));

  double N2((param[6]+param[7]+(param[6]-param[7])*exp(2.0*param[2]/param[6]))*(-1.0*param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])-param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])-param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]))+exp(2.0*param[3]/param[7])*(param[6]-param[7]+exp(2.0*param[2]/param[6])*(param[6]+param[7]))*(param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])+param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9])));

  double N21(exp(param[3]/param[7])*param[8]*param[9]*(param[6]*cosh(param[2]/param[6])+param[7]*sinh(param[2]/param[6]))+param[6]*param[9]*cosh(param[5]/param[9])*(-1.0*param[8]*cosh(param[4]/param[8])+param[7]*sinh(param[4]/param[8]))+param[6]*param[8]*(param[7]*cosh(param[4]/param[8])-param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]));

  double N22(exp((2.0*param[2]+param[3]+2.0*param[1])/param[7])*(-1.0*param[6]*param[8]*param[9]*cosh(param[2]/param[6])+param[7]*param[8]*param[9]*sinh(param[2]/param[6])+exp(param[3]/param[7])*param[6]*param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])+param[7]*sinh(param[4]/param[8]))+exp(param[3]/param[7])*param[6]*param[8]*(param[7]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9])));

  double N3(2.0*((param[6]+exp(2.0*param[2]/param[6])*(param[6]-param[7])+param[7])*(-1.0*param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])-param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])-param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]))+exp(2.0*param[3]/param[7])*(param[6]-param[7]+exp(2.0*param[2]/param[6])*(param[6]+param[7]))*(param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])+param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]))));

  double N4(4.0*((param[6]+exp(2.0*param[2]/param[6])*(param[6]-param[7])+param[7])*(-1.0*param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])-param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])-param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]))+exp(2.0*param[3]/param[7])*(param[6]-param[7]+exp(2.0*param[2]/param[6])*(param[6]+param[7]))*(param[9]*cosh(param[5]/param[9])*(param[8]*cosh(param[4]/param[8])+param[7]*sinh(param[4]/param[8]))+param[8]*(param[7]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))*sinh(param[5]/param[9]))));

  double N42(-1.0*param[6]*param[7]*param[8]+exp(param[5]/param[9])*(param[6]*cosh(param[2]/param[6])*(param[8]*sinh(param[3]/param[7])*(param[9]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))+param[7]*cosh(param[3]/param[7])*(param[8]*cosh(param[4]/param[8])+param[9]*sinh(param[4]/param[8])))+param[7]*sinh(param[2]/param[6])*(param[8]*cosh(param[3]/param[7])*(param[9]*cosh(param[4]/param[8])+param[8]*sinh(param[4]/param[8]))+param[7]*sinh(param[3]/param[7])*(param[8]*cosh(param[4]/param[8])+param[9]*sinh(param[4]/param[8])))));

  fDZ = (param[2]+param[3]+param[4]+param[5])/double(steps);
  double ZZ, BBz;

  for (unsigned int j(0); j<steps; j++) {
    ZZ = param[1] + (double)j*fDZ;
    fZ.push_back(ZZ);
    if (ZZ < param[1]+param[2]) {
      BBz = (2.0*param[0]*exp(-(param[1]+ZZ)/param[6]+param[3]/param[7])*(-1.0*exp(param[2]/param[6])*(exp(2.0*param[1]/param[6])-exp(2.0*ZZ/param[6]))*param[7]*param[8]*param[9]-exp(2.0*ZZ/param[6])*N11+N12))/N1;
    } else if (ZZ < param[1]+param[2]+param[3]) {
      BBz = (2.0*param[0]*exp(param[2]/param[6]-(param[2]+param[1]+ZZ)/param[7])*(exp(2.0*ZZ/param[7])*N21+N22))/N2;
    } else if (ZZ < param[1]+param[2]+param[3]+param[4]) {
      BBz = (param[0]*exp(-1.0*(param[2]+param[3]+param[1]+ZZ)/param[8]-param[5]/param[9])*(2.0*exp(param[2]/param[6]+param[3]/param[7]+(2.0*param[2]+2.0*param[3]+param[4]+2.0*param[1])/param[8])*param[6]*param[7]*(param[9]-param[8]+exp(2.0*param[5]/param[9])*(param[8]+param[9]))+4.0*exp(param[2]/param[6]+param[3]/param[7]-param[4]/param[8]+param[5]/param[9])*(-1.0*exp((2.0*param[2]+2.0*param[3]+param[4]+2.0*param[1])/param[8])*param[9]*(param[7]*sinh(param[2]/param[6])*(-1.0*param[8]*cosh(param[3]/param[7])+param[7]*sinh(param[3]/param[7]))+param[6]*cosh(param[2]/param[6])*(param[7]*cosh(param[3]/param[7])-param[8]*sinh(param[3]/param[7])))+exp(2.0*ZZ/param[8])*(exp(param[4]/param[8])*param[9]*(param[7]*sinh(param[2]/param[6])*(param[8]*cosh(param[3]/param[7])+param[7]*sinh(param[3]/param[7]))+param[6]*cosh(param[2]/param[6])*(param[7]*cosh(param[3]/param[7])+param[8]*sinh(param[3]/param[7])))+param[6]*param[7]*(-1.0*param[9]*cosh(param[5]/param[9])+param[8]*sinh(param[5]/param[9]))))))/N3;
    } else {
      BBz = (param[0]*exp(-1.0*(param[2]+param[3]+param[4]+param[1]+ZZ)/param[9])*(-1.0*exp(-1.0*param[4]/param[8]+2.0*(param[2]+param[3]+param[4]+param[1])/param[9])*((-1.0+exp(2.0*param[2]/param[6]))*param[7]*(exp(2.0*param[4]/param[8])*(param[8]-param[7]+exp(2.0*param[3]/param[7])*(param[7]+param[8]))*(param[8]-param[9])+exp(2.0*param[3]/param[7])*(param[7]-param[8])*(param[8]+param[9])-(param[7]+param[8])*(param[8]+param[9]))+param[6]*(-8.0*exp(param[2]/param[6]+param[3]/param[7]+param[4]/param[8]+param[5]/param[9])*param[7]*param[8]+(1.0+exp(2.0*param[2]/param[6]))*(-1.0+exp(2.0*param[3]/param[7]))*param[8]*(-1.0*param[8]+exp(2.0*param[4]/param[8])*(param[8]-param[9])-param[9])+(1.0+exp(2.0*param[2]/param[6]))*(1.0+exp(2.0*param[3]/param[7]))*param[7]*(param[8]+exp(2.0*param[4]/param[8])*(param[8]-param[9])+param[9])))+8.0*exp(param[2]/param[6]+param[3]/param[7]-(param[5]-2.0*ZZ)/param[9])*N42))/N4;
    }
    fBZ.push_back(BBz);
  }
}
