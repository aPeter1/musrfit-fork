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

    virtual Bool_t Add(Int_t runNo, EPMusrHandleTag tag);

    virtual Double_t GetSingleHistoChisq(const std::vector<Double_t>& par) const;
    virtual Double_t GetAsymmetryChisq(const std::vector<Double_t>& par) const;
    virtual Double_t GetRRFChisq(const std::vector<Double_t>& par) const;
    virtual Double_t GetNonMusrChisq(const std::vector<Double_t>& par) const;

    virtual Double_t GetSingleHistoMaximumLikelihood(const std::vector<Double_t>& par) const;
    virtual Double_t GetAsymmetryMaximumLikelihood(const std::vector<Double_t>& par) const;
    virtual Double_t GetRRFMaximumLikelihood(const std::vector<Double_t>& par) const;
    virtual Double_t GetNonMusrMaximumLikelihood(const std::vector<Double_t>& par) const;

    virtual UInt_t GetTotalNoOfBinsFitted() const;

    virtual UInt_t GetNoOfSingleHisto() const { return fRunSingleHistoList.size(); }
    virtual UInt_t GetNoOfAsymmetry() const   { return fRunAsymmetryList.size(); }
    virtual UInt_t GetNoOfRRF() const         { return fRunRRFList.size(); }
    virtual UInt_t GetNoOfNonMusr() const     { return fRunNonMusrList.size(); }

    virtual PRunData* GetSingleHisto(UInt_t index, EDataSwitch tag=kIndex);
    virtual PRunData* GetAsymmetry(UInt_t index, EDataSwitch tag=kIndex);
    virtual PRunData* GetRRF(UInt_t index, EDataSwitch tag=kIndex);
    virtual PRunData* GetNonMusr(UInt_t index, EDataSwitch tag=kIndex);

    virtual vector< pair<Double_t, Double_t> > GetTemp(const TString &runName) const;
    virtual Double_t GetField(const TString &runName) const;
    virtual Double_t GetEnergy(const TString &runName) const;
    virtual const Char_t* GetSetup(const TString &runName) const;
    virtual const Char_t* GetXAxisTitle(const TString &runName, const UInt_t idx) const;
    virtual const Char_t* GetYAxisTitle(const TString &runName, const UInt_t idx) const;

  private:
    PMsrHandler *fMsrInfo;  ///< keeps all msr file info
    PRunDataHandler *fData; ///< handles all raw data

    vector<PRunSingleHisto*> fRunSingleHistoList;
    vector<PRunAsymmetry*>   fRunAsymmetryList;
    vector<PRunRRF*>         fRunRRFList;
    vector<PRunNonMusr*>     fRunNonMusrList;
};

#endif // _PRUNLISTCOLLECTION_H_
