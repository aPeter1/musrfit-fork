/***************************************************************************

  PRunSingleHisto.h

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

#ifndef _PRUNSINGLEHISTO_H_
#define _PRUNSINGLEHISTO_H_

#include "PRunBase.h"

class PRunSingleHisto : public PRunBase
{
  public:
    PRunSingleHisto();
    PRunSingleHisto(PMsrHandler *msrInfo, PRunDataHandler *rawData, unsigned int runNo);
    virtual ~PRunSingleHisto();

    virtual double CalcChiSquare(const std::vector<double>& par);
    virtual double CalcMaxLikelihood(const std::vector<double>& par);
    virtual void CalcTheory();

    virtual unsigned int GetNoOfFitBins() { return fNoOfFitBins; }

  protected:
    virtual bool PrepareData();

  private:
    double fFitStartTime;
    double fFitStopTime;
    unsigned int fNoOfFitBins;
};

#endif // _PRUNSINGLEHISTO_H_
