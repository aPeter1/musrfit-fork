/***************************************************************************

  PRunAsymmetry.h

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

#ifndef _PRUNASYMMETRY_H_
#define _PRUNASYMMETRY_H_

#include "PRunBase.h"

class PRunAsymmetry : public PRunBase
{
  public:
    PRunAsymmetry();
    PRunAsymmetry(PMsrHandler *msrInfo, PRunDataHandler *rawData, unsigned int runNo);
    virtual ~PRunAsymmetry();

    virtual double CalcChiSquare(const std::vector<double>& par);
    virtual double CalcMaxLikelihood(const std::vector<double>& par);
    virtual void CalcTheory();

    virtual unsigned int GetNoOfFitBins() { return fNoOfFitBins; }

  protected:
    virtual bool PrepareData();

  private:
    unsigned int fAlphaBetaTag; ///< 1-> alpha = beta = 1; 2-> alpha != 1, beta = 1; 3-> alpha = 1, beta != 1; 4-> alpha != 1, beta != 1

    double fFitStartTime;
    double fFitStopTime;
    unsigned int fNoOfFitBins;

    vector<double> fForward;     ///< forward histo data
    vector<double> fForwardErr;  ///< forward histo errors
    vector<double> fBackward;    ///< backward histo data
    vector<double> fBackwardErr; ///< backward histo errors

    bool SubtractFixBkg();
    bool SubtractEstimatedBkg();
};

#endif // _PRUNASYMMETRY_H_
