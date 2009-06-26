// -----------------------------------------------------------------------
// Author: Andreas Suter
// $Id$
// -----------------------------------------------------------------------

#ifndef _PPIPPARD_H_
#define _PPIPPARD_H_

#include <fftw3.h>

#include <Rtypes.h>

#include <Eigen/Core>
#include <Eigen/LU>
using namespace Eigen;

const Int_t PippardFourierPoints = 200000;
const Int_t PippardDiffusePoints = 500;

class PPippard
{
  public:
    PPippard(Double_t temp, Double_t lambdaL, Double_t xi0, Double_t meanFreePath, Double_t filmThickness, Bool_t specular = true);
    virtual ~PPippard();

    virtual void SetTemp(Double_t temp) { fTemp = temp; }
    virtual void SetLambdaL(Double_t lambdaL) { fLambdaL = lambdaL; }
    virtual void SetXi0(Double_t xi0) { fXi0 = xi0; }
    virtual void SetMeanFreePath(Double_t meanFreePath) { fMeanFreePath = meanFreePath; }
    virtual void SetFilmThickness(Double_t thickness) { fFilmThickness = thickness; }
    virtual void SetSpecular(Bool_t specular) { fSpecular = specular; }

    virtual void CalculateField();

    virtual Double_t GetMagneticField(const Double_t x) const;

    virtual void SaveField(const char *fileName);

  private:
    Double_t fTemp;          // reduced temperature, i.e. t = T/T_c
    Double_t fLambdaL;       // lambdaL in (nm)
    Double_t fXi0;           // xi0 in (nm)
    Double_t fMeanFreePath;  // mean free path in (nm)
    Double_t fFilmThickness; // film thickness in (nm)
    Bool_t   fSpecular;      // = 1 -> specular, 0 -> diffuse

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
