// -----------------------------------------------------------------------
// Author: Andreas Suter
// $Id$
// -----------------------------------------------------------------------

#include <cstdio>
#include <cmath>

#include <iostream>
using namespace std;

#include <gsl_sf_gamma.h>

#include <TMath.h>

#include "PPippard.h"

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
PPippard::PPippard(const PippardParams &params) : fParams(params)
{
  fPlanPresent = false;
  fFieldq = 0;
  fFieldB = 0;

  fSecondDerivativeMatrix = 0;
  fKernelMatrix = 0;
  fBoundaryCondition = 0;
  fFieldDiffuse = 0;

  f_dx = 0.02;
  f_dz = XiP_T(fParams.t)*TMath::TwoPi()/PippardFourierPoints/f_dx; // see lab-book p.137, used for specular reflection boundary conditions (default)
  fShift  = 0;
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
PPippard::~PPippard()
{
  if (fPlanPresent) {
    fftw_destroy_plan(fPlan);
  }
  if (fFieldq) {
    fftw_free(fFieldq);
    fFieldq = 0;
  }

  if (fFieldB) {
    fftw_free(fFieldq);
    fFieldB = 0;
  }
  if (fSecondDerivativeMatrix) {
    delete fSecondDerivativeMatrix;
    fSecondDerivativeMatrix = 0;
  }
  if (fKernelMatrix) {
    delete fKernelMatrix;
    fKernelMatrix = 0;
  }
  if (fBoundaryCondition) {
    delete fBoundaryCondition;
    fBoundaryCondition = 0;
  }
  if (fFieldDiffuse) {
    delete fFieldDiffuse;
    fFieldDiffuse = 0;
  }
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
Double_t PPippard::GetMagneticField(const Double_t z) const
{
  Double_t result = -1.0;

  if (fParams.specular) {

    if (fFieldB == 0)
      return -1.0;

    if (z < 0.0)
      return 1.0;

    if (z > f_dz*PippardFourierPoints/2.0)
      return 0.0;

    Int_t bin = (Int_t)(z/f_dz);

    result = fFieldB[bin+fShift][1];

  } else { // diffuse

    if (fFieldDiffuse == 0)
      return -1.0;

    if (z < 0.0)
      return 1.0;

    if (z > PippardDiffusePoints * f_dz * XiP_T(fParams.xi0))
      return 0.0;

    Int_t bin = (Int_t)(z/(f_dz*XiP_T(fParams.xi0)));

    result = (*fFieldDiffuse)(bin);
  }

  return result;
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
Double_t PPippard::DeltaBCS(const Double_t t) const
{
  Double_t result = 0.0;

  // reduced temperature table
  Double_t tt[] = {1, 0.98, 0.96, 0.94, 0.92, 0.9, 0.88, 0.86, 0.84, 0.82,
                   0.8, 0.78, 0.76, 0.74, 0.72, 0.7, 0.68, 0.66, 0.64, 0.62,
                   0.6, 0.58, 0.56, 0.54, 0.52, 0.5, 0.48, 0.46, 0.44, 0.42,
                   0.4, 0.38, 0.36, 0.34, 0.32, 0.3, 0.28, 0.26, 0.24, 0.22,
                   0.2, 0.18, 0.16, 0.14};

  // gap table from Muehlschlegel
  Double_t ee[] = {0, 0.2436, 0.3416, 0.4148, 0.4749, 0.5263, 0.5715, 0.6117,
                   0.648, 0.681, 0.711, 0.7386, 0.764, 0.7874, 0.8089, 0.8288,
                   0.8471, 0.864, 0.8796, 0.8939, 0.907, 0.919, 0.9299, 0.9399,
                   0.9488, 0.9569, 0.9641, 0.9704, 0.976, 0.9809, 0.985, 0.9885,
                   0.9915, 0.9938, 0.9957, 0.9971, 0.9982, 0.9989, 0.9994, 0.9997,
                   0.9999, 1, 1, 1, 1};

  if (t>1.0)
    result = 0.0;
  else if (t<0.14)
    result = 1.0;
  else {
     // find correct bin for t
     UInt_t i=0;
     for (i=0; i<sizeof(tt); i++) {
       if (tt[i]<=t) break;
     }
     // interpolate linear between 2 bins
     result = ee[i]-(tt[i]-t)*(ee[i]-ee[i-1])/(tt[i]-tt[i-1]);
  }

  return result;
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
Double_t PPippard::LambdaL_T(const Double_t t) const
{
  return fParams.lambdaL/sqrt(1.0-pow(t,4.0));
}

//-----------------------------------------------------------------------------------------------------------
/**
 * <p> Approximated xi_P(T). The main approximation is that (lamdaL(T)/lambdaL(0))^2 = 1/(1-t^2). This way
 * xi_P(T) is close the the BCS xi_BCS(T).
 */
Double_t PPippard::XiP_T(Double_t t) const
{
  if (t>0.96)
    t=0.96;

  Double_t J0T = DeltaBCS(t)/(1.0-pow(t,2.0)) * tanh(0.881925 * DeltaBCS(t) / t);

  return fParams.xi0*fParams.meanFreePath/(fParams.meanFreePath*J0T+fParams.xi0);
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
void PPippard::CalculateField()
{
  // calculate the field
  if (fParams.specular)
    CalculateFieldSpecular();
  else
    CalculateFieldDiffuse();
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
void PPippard::CalculateFieldSpecular()
{
  // check if it is necessary to allocate memory
  if (fFieldq == 0) {
    fFieldq = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * PippardFourierPoints);
    if (fFieldq == 0) {
      cout << endl << "PPippard::CalculateField(): **ERROR** couldn't allocate memory for fFieldq";
      cout << endl;
      return;
    }
  }
  if (fFieldB == 0) {
    fFieldB = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * PippardFourierPoints);
    if (fFieldB == 0) {
      cout << endl << "PPippard::CalculateField(): **ERROR** couldn't allocate memory for fFieldB";
      cout << endl;
      return;
    }
  }

  // calculate the prefactor of the reduced kernel
  Double_t xiP = XiP_T(fParams.t);
  Double_t preFactor = pow(xiP/(LambdaL_T(fParams.t)),2.0)*xiP/fParams.xi0;


  // calculate the fFieldq vector, which is x/(x^2 + alpha k(x)), with alpha = xiP(T)^3/(lambdaL(T)^2 xiP(0)), and
  // k(x) = 3/2 [(1+x^2) arctan(x) - x]/x^3, see lab-book p.137
  Double_t x;
  fFieldq[0][0] = 0.0;
  fFieldq[0][1] = 0.0;
  for (Int_t i=1; i<PippardFourierPoints; i++) {
    x = i * f_dx;
    fFieldq[i][0] = x/(pow(x,2.0)+preFactor*(1.5*((1.0+pow(x,2.0))*atan(x)-x)/pow(x,3.0)));
    fFieldq[i][1] = 0.0;
  }

  // Fourier transform
  if (!fPlanPresent) {
    fPlan = fftw_plan_dft_1d(PippardFourierPoints, fFieldq, fFieldB, FFTW_FORWARD, FFTW_ESTIMATE);
    fPlanPresent = true;
  }

  fftw_execute(fPlan);

  // normalize fFieldB
  Double_t norm = 0.0;
  fShift=0;
  for (Int_t i=0; i<PippardFourierPoints/2; i++) {
    if (fabs(fFieldB[i][1]) > fabs(norm)) {
      norm = fFieldB[i][1];
      fShift = i;
    }
  }

cout << endl << "fShift = " << fShift;

  for (Int_t i=0; i<PippardFourierPoints; i++) {
    fFieldB[i][1] /= norm;
  }

  if (fParams.filmThickness < PippardFourierPoints/2.0*f_dz) {
    // B(z) = b(z)+b(D-z)/(1+b(D)) is the B(z) result
    Int_t idx = (Int_t)(fParams.filmThickness/f_dz);
    norm = 1.0 + fFieldB[idx+fShift][1];
    for (Int_t i=0; i<PippardFourierPoints; i++) {
      fFieldB[i][0] = 0.0;
    }
    for (Int_t i=fShift; i<idx+fShift; i++) {
      fFieldB[i][0] = (fFieldB[i][1] + fFieldB[idx-i+2*fShift][1])/norm;
    }
    for (Int_t i=0; i<PippardFourierPoints; i++) {
      fFieldB[i][1] = fFieldB[i][0];
    }
  }

  Double_t integral = 0.0;
  for (Int_t i=fShift; i<PippardFourierPoints/2; i++)
    integral += fFieldB[i][1];
  cout << endl << ">> specular Integral = " << integral*f_dz;
  fParams.specularIntegral = integral*f_dz;
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
void PPippard::CalculateFieldDiffuse()
{
  f_dz = 5.0/XiP_T(fParams.t);

  Double_t invL = 1/f_dz;
  Double_t ampl = 1.0/pow(f_dz,2.0)/(3.0/4.0*pow(XiP_T(fParams.t),3.0)/(fParams.xi0*pow(LambdaL_T(fParams.t),2.0)));

cout << endl << ">> 1/alpha = " << 1.0/(3.0/4.0*pow(XiP_T(fParams.t),3.0)/(fParams.xi0*pow(LambdaL_T(fParams.t),2.0)));
cout << endl << ">> 1/l^2   = " << 1.0/pow(f_dz,2.0);
cout << endl << ">> ampl    = " << ampl << endl;

  // 2nd derivative matrix
  if (fSecondDerivativeMatrix == 0) { // first time call, hence generate the 2nd derivative matrix
    fSecondDerivativeMatrix = new MatrixXd(PippardDiffusePoints+1, PippardDiffusePoints+1);
    fSecondDerivativeMatrix->setZero(PippardDiffusePoints+1, PippardDiffusePoints+1);
    for (Int_t i=1; i<PippardDiffusePoints; i++) {
      (*fSecondDerivativeMatrix)(i,i-1) = ampl;
      (*fSecondDerivativeMatrix)(i,i) = -2.0*ampl;
      (*fSecondDerivativeMatrix)(i,i+1) = ampl;
    }
  }

//cout << endl << "fSecondDerivativeMatrix = \n" << *fSecondDerivativeMatrix << endl;

  // kernel matrix
  if (fKernelMatrix == 0) { // first time call, hence generate the kernel matrix
    fKernelMatrix = new MatrixXd(PippardDiffusePoints+1, PippardDiffusePoints+1);
    fKernelMatrix->setZero(PippardDiffusePoints+1, PippardDiffusePoints+1);
    // 1st line (dealing with boundary conditions)
    (*fKernelMatrix)(0,0) = -1.5*invL;
    (*fKernelMatrix)(0,1) =  2.0*invL;
    (*fKernelMatrix)(0,2) = -0.5*invL;
    // Nth line (dealing with boundary conditions)
    (*fKernelMatrix)(PippardDiffusePoints,PippardDiffusePoints-2) =  0.5*invL;
    (*fKernelMatrix)(PippardDiffusePoints,PippardDiffusePoints-1) = -2.0*invL;
    (*fKernelMatrix)(PippardDiffusePoints,PippardDiffusePoints)   =  1.5*invL;
    // the real kernel
    for (Int_t i=1; i<PippardDiffusePoints; i++) {
      (*fKernelMatrix)(i,0)=Calc_a(i);
      (*fKernelMatrix)(i,PippardDiffusePoints)=Calc_b(i);
      for (Int_t j=1; j<PippardDiffusePoints; j++) {
        (*fKernelMatrix)(i,j) = Calc_c(i,j);
      }
    }
  }

//cout << endl << "fKernelMatrix = \n" << *fKernelMatrix << endl;

  // boundary condition vector
  if (fBoundaryCondition == 0) {
    fBoundaryCondition = new VectorXd(PippardDiffusePoints+1);
    fBoundaryCondition->setZero(PippardDiffusePoints+1);
    (*fBoundaryCondition)(0) = 1.0;
  }

//cout << endl << "fBoundaryCondition = " << *fBoundaryCondition << endl;

  if (fFieldDiffuse == 0) {
    fFieldDiffuse = new VectorXd(PippardDiffusePoints+1);
    fFieldDiffuse->setZero(PippardDiffusePoints+1);
  }

  // solve equation
  *fSecondDerivativeMatrix = (*fSecondDerivativeMatrix)-(*fKernelMatrix);
  fSecondDerivativeMatrix->lu().solve(*fBoundaryCondition, fFieldDiffuse);

  // normalize field
  Double_t norm = 0.0;
  for (Int_t i=0; i<PippardDiffusePoints+1; i++)
    if (norm < (*fFieldDiffuse)(i))
      norm = (*fFieldDiffuse)(i);

  for (Int_t i=0; i<PippardDiffusePoints+1; i++)
    (*fFieldDiffuse)(i) /= norm;

  Double_t integral = 0.0;
  for (Int_t i=0; i<PippardDiffusePoints+1; i++)
    integral += (*fFieldDiffuse)(i);
  cout << endl << "Diffuse Integral = " << integral*f_dz*XiP_T(fParams.t);
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
void PPippard::SaveField()
{
  FILE *fp;

  fp = fopen(fParams.outputFileName.Data(), "w");
  if (fp == NULL) {
    cout << endl << "Coudln't open " << fParams.outputFileName.Data() << " for writting, sorry ...";
    cout << endl << endl;
    return;
  }

  // write header
  fprintf(fp, "%% Header ------------------------------------\n");
  fprintf(fp, "%% Parameters:\n");
  fprintf(fp, "%%   Reduced Temperature = %lf\n", fParams.t);
  fprintf(fp, "%%   LambdaL(0)          = %lf, LambdaL(t) = %lf\n", fParams.lambdaL, LambdaL_T(fParams.t));
  if (fParams.specularIntegral > 0.0)
    fprintf(fp, "%%   int_x=0^infty B(x) dx / Bext = %lf\n", fParams.specularIntegral);
  fprintf(fp, "%%   xiP(0)              = %lf, xiP(t)     = %lf\n", fParams.xi0, XiP_T(fParams.t));
  fprintf(fp, "%%   Mean Free Path      = %lf\n", fParams.meanFreePath);
  fprintf(fp, "%%   Film Thickness      = %lf\n", fParams.filmThickness);
  if (fParams.specular)
    fprintf(fp, "%%   Boundary Conditions: Specular\n");
  else
    fprintf(fp, "%%   Boundary Conditions: Diffuse\n");
  if (fParams.rgeFileName.Length() > 0)
    fprintf(fp, "%%   rge file name : %s\n", fParams.rgeFileName.Data());
  if (fParams.meanB != 0.0)
    fprintf(fp, "%%   Mean Field/Bext     = %lf\n", fParams.meanB);
  fprintf(fp, "%%\n");

  // write data
  fprintf(fp, "%% Data --------------------------------------\n");
  fprintf(fp, "%% z (nm), B/B_0 \n");
  if (fParams.specular) {
    for (Int_t i=0; i<PippardFourierPoints/2; i++) {
      fprintf(fp, "%lf, %lf\n", f_dz*(Double_t)i, fFieldB[i+fShift][1]);
    }
  } else {
    for (Int_t i=0; i<PippardDiffusePoints; i++) {
      fprintf(fp, "%lf, %lf\n", f_dz * XiP_T(fParams.t) * (Double_t)i, (*fFieldDiffuse)(i));
    }
  }

  fclose(fp);
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
Double_t PPippard::Calc_v(const Double_t s) const
{
  if (s == 0.0)
    return 0.0;

  Double_t s2 = pow(s,2.0);
  Double_t ss = fabs(s);
  Double_t v;

  if (ss < 0.001) {
    v = (-0.0772157-log(ss))*ss+s2; // series expansion in s up to 2nd order
  } else {
    v = 1/6.0*(exp(-ss)*(s2-ss-4.0) + 4.0 - ss*(s2-6.0) * gsl_sf_gamma_inc(0.0, ss));
  }

  if (s < 0)
    v = -v;

  return v;
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
Double_t PPippard::Calc_w(const Double_t s) const
{
  if (s == 0.0)
    return 0.0;

  Double_t s2 = pow(s,2.0);
  Double_t ss = fabs(s);
  Double_t w;

  if (ss < 0.001) {
    w = (0.211392 - 0.5*log(ss))*s2; // series expansion in s up to 2nd order
  } else {
    w = 1/24.0*(exp(-ss)*(6.0-10.0*ss-s2+ss*s2)+16.0*ss-6.0-s2*(s2-12.0)*gsl_sf_gamma_inc(0.0, ss));
  }

  return w;
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
Double_t PPippard::Calc_a(const Int_t i) const
{ 
  return -Calc_v(-i*f_dz) + (Calc_w((1-i)*f_dz)-Calc_w(-i*f_dz))/f_dz;
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
Double_t PPippard::Calc_b(const Int_t i) const
{
  return Calc_v((PippardDiffusePoints-i)*f_dz) - (Calc_w((PippardDiffusePoints-i)*f_dz)-Calc_w((PippardDiffusePoints-1-i)*f_dz))/f_dz;
}

//-----------------------------------------------------------------------------------------------------------
/**
 *
 */
Double_t PPippard::Calc_c(const Int_t i, const Int_t j) const
{
  return (Calc_w((i+1-j)*f_dz)-2.0*Calc_w((i-j)*f_dz)+Calc_w((i-1-j)*f_dz))/f_dz;
}
