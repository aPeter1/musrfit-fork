/***************************************************************************

  PRunListCollection.h

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

#ifndef _PRUNLISTCOLLECTION_H_
#define _PRUNLISTCOLLECTION_H_

#include <vector>
using namespace std;

#include "PMusr.h"
#include "PMsrHandler.h"
#include "PRunDataHandler.h"
#include "PRunSingleHisto.h"
#include "PRunAsymmetry.h"
#include "PRunRRF.h"
#include "PRunNonMusr.h"

class PRunListCollection
{
  public:
    PRunListCollection(PMsrHandler *msrInfo, PRunDataHandler *data);
    virtual ~PRunListCollection();

    enum EDataSwitch { kIndex, kRunNo };

    virtual bool Add(int runNo, EPMusrHandleTag tag);

    virtual double GetSingleHistoChisq(const std::vector<double>& par);
    virtual double GetAsymmetryChisq(const std::vector<double>& par);
    virtual double GetRRFChisq(const std::vector<double>& par);
    virtual double GetNonMusrChisq(const std::vector<double>& par);

    virtual double GetSingleHistoMaximumLikelihood(const std::vector<double>& par);
    virtual double GetAsymmetryMaximumLikelihood(const std::vector<double>& par);
    virtual double GetRRFMaximumLikelihood(const std::vector<double>& par);
    virtual double GetNonMusrMaximumLikelihood(const std::vector<double>& par);

    virtual unsigned int GetTotalNoOfBinsFitted();

    virtual unsigned int GetNoOfSingleHisto() { return fRunSingleHistoList.size(); }
    virtual unsigned int GetNoOfAsymmetry()   { return fRunAsymmetryList.size(); }
    virtual unsigned int GetNoOfRRF()         { return fRunRRFList.size(); }
    virtual unsigned int GetNoOfNonMusr()     { return fRunNonMusrList.size(); }

    virtual PRunData* GetSingleHisto(unsigned int index, EDataSwitch tag=kIndex);
    virtual PRunData* GetAsymmetry(unsigned int index, EDataSwitch tag=kIndex);
    virtual PRunData* GetRRF(unsigned int index, EDataSwitch tag=kIndex);
    virtual PRunData* GetNonMusr(unsigned int index, EDataSwitch tag=kIndex);

    virtual double GetTemp(TString &runName);
    virtual double GetField(TString &runName);
    virtual double GetEnergy(TString &runName);
    virtual const char* GetSetup(TString &runName);
    virtual const char* GetXAxisTitle(TString &runName, const unsigned int idx);
    virtual const char* GetYAxisTitle(TString &runName, const unsigned int idx);

  private:
    PMsrHandler *fMsrInfo;  ///< keeps all msr file info
    PRunDataHandler *fData; ///< handles all raw data

    vector<PRunSingleHisto*> fRunSingleHistoList;
    vector<PRunAsymmetry*>   fRunAsymmetryList;
    vector<PRunRRF*>         fRunRRFList;
    vector<PRunNonMusr*>     fRunNonMusrList;
};

#endif // _PRUNLISTCOLLECTION_H_
