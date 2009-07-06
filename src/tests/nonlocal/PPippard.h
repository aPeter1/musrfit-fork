// -----------------------------------------------------------------------
// Author: Andreas Suter
// $Id$
// -----------------------------------------------------------------------

#ifndef _PPIPPARD_H_
#define _PPIPPARD_H_

#include <fftw3.h>

#include <Rtypes.h>
#include <TString.h>

#include <Eigen/Core>
#include <Eigen/LU>
using namespace Eigen;

const Int_t PippardFourierPoints = 200000;
const Int_t PippardDiffusePoints = 500;

typedef struct {
  Bool_t valid;
  Double_t t;             // reduced temperature
  Double_t lambdaL;       // lambda London
  Double_t xi0;           // Pippard coherence length
  Double_t meanFreePath;  // mean free path
  Double_t filmThickness; // film thickness
  Bool_t specular;        // true -> specular reflection boundary conditions, false -> diffuse scattering boundary conditions
  Double_t specularIntegral; // int_x=0^infty B(x) dx / Bext
  TString rgeFileName;
  TString outputFileName;
  Double_t meanB;         // int_x=0^infty B(x) n(x) dx / int_x=0^infty n(x) dx / Bext
} PippardParams;

class PPippard
{
  public:
    PPippard(const PippardParams &params);
    virtual ~PPippard();

    virtual void SetTemp(Double_t temp) { fParams.t = temp; }
    virtual void SetLambdaL(Double_t lambdaL) { fParams.lambdaL = lambdaL; }
    virtual void SetXi0(Double_t xi0) { fParams.xi0 = xi0; }
    virtual void SetMeanFreePath(Double_t meanFreePath) { fParams.meanFreePath = meanFreePath; }
    virtual void SetFilmThickness(Double_t thickness) { fParams.filmThickness = thickness; }
    virtual void SetSpecular(Bool_t specular) { fParams.specular = specular; }
    virtual void SetMeanB(Double_t meanB) { fParams.meanB = meanB; }

    virtual void CalculateField();

    virtual Double_t GetStepDistance() { return f_dz; }
    virtual Double_t GetMagneticField(const Double_t x) const;

    virtual void SaveField();

  private:
    PippardParams fParams;

    Double_t f_dx;           // dx = xiPT dq
    Double_t f_dz;           // spatial step size

    bool fPlanPresent;
    fftw_plan     fPlan;
    fftw_complex *fFieldq;   // (xiPT x)/(x^2 + xiPT^2 K(x,T)), x = q xiPT
    fftw_complex *fFieldB;   // field calculated for specular boundary conditions

    Int_t fShift; // shift needed to pick up fFieldB at the maximum for B->0

    MatrixXd *fSecondDerivativeMatrix; // 2nd derivative matrix
    MatrixXd *fKernelMatrix;           // kernel matrix
    VectorXd *fBoundaryCondition;      // boundary condition vector
    VectorXd *fFieldDiffuse;           // resulting B(z)/B(0) field

    virtual Double_t DeltaBCS(const Double_t t) const;
    virtual Double_t LambdaL_T(const Double_t t) const;
    virtual Double_t XiP_T(Double_t t) const;

    virtual void CalculateFieldSpecular();
    virtual void CalculateFieldDiffuse();

    virtual Double_t Calc_v(const Double_t s) const; // see 'A.Suter, Memorandum June 17, 2004' Eq.(13)
    virtual Double_t Calc_w(const Double_t s) const; // see 'A.Suter, Memorandum June 17, 2004' Eq.(14)

    virtual Double_t Calc_a(const Int_t i) const; // see 'A.Suter, Memorandum June 17, 2004' Eq.(17)
    virtual Double_t Calc_b(const Int_t i) const; // see 'A.Suter, Memorandum June 17, 2004' Eq.(17)
    virtual Double_t Calc_c(const Int_t i, const Int_t j) const; // see 'A.Suter, Memorandum June 17, 2004' Eq.(17)
};

#endif // _PPIPPARD_H_
