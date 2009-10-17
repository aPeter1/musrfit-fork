/***************************************************************************

  TBulkVortexFieldCalc.cpp

  Author: Bastian M. Wojek, Alexander Maisuradze
  e-mail: bastian.wojek@psi.ch

  2009/10/17

***************************************************************************/

#include "TBulkVortexFieldCalc.h"

#include <cmath>
#include <omp.h>

#define PI 3.14159265358979323846
#define TWOPI 6.28318530717958647692

const double fluxQuantum(2.067833667e7); // 10e14 times CGS units %% in CGS units should be 10^-7
                                         // in this case this is Gauss per square nm

double getXi(const double hc2) { // get xi given Hc2 in Gauss
  if (hc2)
    return sqrt(fluxQuantum/(TWOPI*hc2));
  else
    return 0.0;
}

double getHc2(const double xi) { // get Hc2 given xi in nm
  if (xi)
    return fluxQuantum/(TWOPI*xi*xi);
  else
    return 0.0;
}

TBulkTriVortexLondonFieldCalc::TBulkTriVortexLondonFieldCalc(const vector<double>& param, const unsigned int steps, const unsigned int Ncomp)
 : fNFourierComp(Ncomp) {
  fSteps = steps;
  fParam = param;
  fB.clear();
}

double TBulkTriVortexLondonFieldCalc::GetBatPoint(double relX, double relY) const {

  double latConstTr(sqrt(fluxQuantum/fParam[2])*sqrt(sqrt(4.0/3.0)));
  double Hc2(getHc2(fParam[1]));

  double xCoord(latConstTr*relX); // in nanometers
  double yCoord(latConstTr*relY);

  if ((fParam[2] < Hc2) && (fParam[0] > fParam[1]/sqrt(2.0))) {
    double KLatTr(4.0*PI/(latConstTr*sqrt(3.0)));
    double fourierSum(0.0);
    for(int mm = -fNFourierComp; mm <= static_cast<int>(fNFourierComp); mm++) {
      for(int nn = -fNFourierComp; nn <= static_cast<int>(fNFourierComp); nn++) {
        fourierSum += cos(KLatTr*(xCoord*mm*(0.5*sqrt(3.0)) + yCoord*mm*0.5 + yCoord*nn))*exp(-(0.5*fParam[1]*fParam[1]*KLatTr*KLatTr)* \
         (0.75*mm*mm + (nn + 0.5*mm)*(nn + 0.5*mm)))/(1.0+(fParam[0]*KLatTr*fParam[0]*KLatTr)*(0.75*mm*mm + (nn + 0.5*mm)*(nn + 0.5*mm)));
      }
    }
    return fParam[2]*fourierSum;
  }
  else
    return fParam[2];

}

void TBulkTriVortexLondonFieldCalc::CalculateGrid() const {

  double bb(cos(PI/6.0)*0.5);
  double yStep(bb/static_cast<double>(fSteps));
  double xStep(0.5/static_cast<double>(fSteps));

  fB.resize(fSteps);
  for (unsigned int i(0); i < fSteps; i++)
    fB[i].resize(fSteps);

  for(unsigned int i(0); i < fSteps; i++) {
    int j;
#pragma omp parallel for default(shared) private(j) schedule(dynamic)
    for(j=0; j < static_cast<int>(fSteps); j++) {
      fB[i][j] = GetBatPoint(static_cast<double>(i)*xStep, static_cast<double>(j)*yStep);
    }
// end pragma omp parallel
  }

  return;

}

double TBulkTriVortexLondonFieldCalc::GetBmin() const {
  return GetBatPoint(0.5, 0.5*tan(PI/6.0));
}

double TBulkTriVortexLondonFieldCalc::GetBmax() const {
  if (!fB.empty())
    return fB[0][0];
  else
    return GetBatPoint(0.0, 0.0);
}

