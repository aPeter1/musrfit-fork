/***************************************************************************

  TBofZCalc.cpp

  Author: Bastian M. Wojek
  e-mail: bastian.wojek@psi.ch

  2008/06/30

***************************************************************************/

#include "TBofZCalc.h"
#include <cmath>
#include <iostream>
#include <algorithm>

//------------------
// Method returning the field minimum of the model
//------------------

double TBofZCalc::GetBmin() const {
  vector<double>::const_iterator iter;
  iter = min_element(fBZ.begin(),fBZ.end());

  return *iter;
}

//------------------
// Method returning the field maximum of the model
//------------------

double TBofZCalc::GetBmax() const {
  vector<double>::const_iterator iter;
  iter = max_element(fBZ.begin(),fBZ.end());

  return *iter;
}

//------------------
// Method returning the field B(z) at a given z according to the model
//------------------

double TBofZCalc::GetBofZ(double zz) const {

  bool found = false;
  unsigned int i;
  for (i=0; i<fZ.size(); i++) {
    if (fZ[i] > zz) {
     found = true;
     break;
    }
  }

  if (!found || i == 0) {
    cout << "B(z) cannot be calculated for z = " << zz << " !" << endl;
    cout << "Check your theory function!" << endl;
    return -1.0;
  }

  return fBZ[i-1]+(fBZ[i]-fBZ[i-1])*(zz-fZ[i-1])/(fZ[i]-fZ[i-1]);

}

//------------------
// Constructor of the TLondon1D_1L class
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

TLondon1D_3L::TLondon1D_3L(unsigned int steps, const vector<double> &param) {

  double N1(param[7]*cosh(param[4]/param[7])*((exp(2.0*param[2]/param[5])-1.0)*param[6]*cosh(param[3]/param[6]) + (1.0+exp(2.0*param[2]/param[5]))*param[5]*sinh(param[3]/param[6])) + 2.0*exp(param[2]/param[5])*param[6]*(param[5]*cosh(param[2]/param[5])*cosh(param[3]/param[6]) + param[6]*sinh(param[2]/param[5])*sinh(param[3]/param[6]))*sinh(param[4]/param[7]));

  double N21(2.0*(param[7]*cosh(param[4]/param[7])*((-1.0+exp(2.0*param[2]/param[5]))*param[6]*cosh(param[3]/param[6]) + (1.0+exp(2.0*param[2]/param[5]))*param[5]*sinh(param[3]/param[6])) + 2.0*exp(param[2]/param[5])*param[6]*(param[5]*cosh(param[2]/param[5])*cosh(param[3]/param[6]) + param[6]*sinh(param[2]/param[5])*sinh(param[3]/param[6]))*sinh(param[4]/param[7])));

  double N22(((param[5]+exp(2.0*param[2]/param[5])*(param[5]-param[6])+param[6])*(-param[7]*cosh(param[4]/param[7]) + param[6]*sinh(param[4]/param[7]))+exp(2.0*param[3]/param[6])*(param[5]-param[6] + exp(2.0*param[2]/param[5])*(param[5]+param[6]))*(param[7]*cosh(param[4]/param[7])+param[6]*sinh(param[4]/param[7]))));

  double N3(4.0*((1.0+exp(2.0*param[2]/param[5]))*param[5]*(param[7]*cosh(param[4]/param[7])*sinh(param[3]/param[6]) + param[6]*cosh(param[3]/param[6])*sinh(param[4]/param[7])) + (-1.0+exp(2.0*param[2]/param[5]))*param[6]*(param[7]*cosh(param[3]/param[6])*cosh(param[4]/param[7]) + param[6]*sinh(param[3]/param[6])*sinh(param[4]/param[7]))));

  fDZ = (param[2]+param[3]+param[4])/double(steps);
  double ZZ, BBz;

  for (unsigned int j(0); j<steps; j++) {
    ZZ = param[1] + (double)j*fDZ;
    fZ.push_back(ZZ);
    if (ZZ < param[1]+param[2]) {
      BBz = (2.0*param[0]*exp(param[2]/param[5])*(-param[6]*param[7]*sinh((param[1]-ZZ)/param[5]) + param[7]*cosh(param[4]/param[7])*(param[6]*cosh(param[3]/param[6])*sinh((param[2]+param[1]-ZZ)/param[5]) + param[5]*cosh((param[2]+param[1]-ZZ)/param[5])*sinh(param[3]/param[6])) + param[6]*(param[5]*cosh((param[2]+param[1]-ZZ)/param[5])*cosh(param[3]/param[6]) + param[6]*sinh((param[2]+param[1]-ZZ)/param[5])*sinh(param[3]/param[6]))*sinh(param[4]/param[7])))/N1;
    } else if (ZZ < param[1]+param[2]+param[3]) {
      BBz = (param[0]*exp((param[2]+param[1]-ZZ)/param[6])*(-param[5]-param[6]+exp(2.0*param[2]/param[5])*(param[6]-param[5]))*param[7])/N21 + (param[0]*exp(-(param[2]+param[1]+ZZ)/param[6])*(exp((param[3]+2.0*ZZ)/param[6])*(param[5]-param[6] + exp(2.0*param[2]/param[5])*(param[5]+param[6]))*param[7] + 2.0*exp(param[2]/param[5])*param[5]*(exp(2.0*ZZ/param[6])*(-param[7]*cosh(param[4]/param[7]) + param[6]*sinh(param[4]/param[7]))+(cosh(2.0*(param[2]+param[3]+param[1])/param[6]) + sinh(2.0*(param[2]+param[3]+param[1])/param[6]))*(param[7]*cosh(param[4]/param[7])+param[6]*sinh(param[4]/param[7])))))/N22;
    } else {
      BBz = (param[0]*exp(-(param[2]+param[3]+param[1]+ZZ)/param[7])*(-exp(-param[3]/param[6] + 2.0*(param[2]+param[3]+param[1])/param[7])*(-4.0*exp(param[2]/param[5]+param[3]/param[6]+param[4]/param[7])*param[5]*param[6] + (-1.0+exp(2.0*param[2]/param[5]))*param[6]*(-param[6]+exp(2.0*param[3]/param[6])*(param[6]-param[7])-param[7]) + (1.0+exp(2.0*param[2]/param[5]))*param[5]*(param[6]+exp(2.0*param[3]/param[6])*(param[6]-param[7])+param[7])) + 4.0*exp(param[2]/param[5]-(param[4]-2.0*ZZ)/param[7])*(-param[5]*param[6]+exp(param[4]/param[7]) * (param[6]*sinh(param[2]/param[5])*(param[7]*cosh(param[3]/param[6])+param[6]*sinh(param[3]/param[6])) + param[5]*cosh(param[2]/param[5])*(param[6]*cosh(param[3]/param[6])+param[7]*sinh(param[3]/param[6]))))))/N3;
    }
    fBZ.push_back(BBz);
  }
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
