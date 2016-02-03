/***************************************************************************

  PRunListCollection.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2016 by Andreas Suter                              *
 *   andreas.suter@psi.ch                                                  *
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
#include "PRunSingleHistoRRF.h"
#include "PRunAsymmetry.h"
#include "PRunAsymmetryRRF.h"
#include "PRunMuMinus.h"
#include "PRunNonMusr.h"

/**
 * <p>Handler class handling all processed data of an msr-file. All calls of minuit2 are going through this class.
 */
class PRunListCollection
{
  public:
    PRunListCollection(PMsrHandler *msrInfo, PRunDataHandler *data);
    virtual ~PRunListCollection();

    enum EDataSwitch { kIndex, kRunNo };

    virtual Bool_t Add(Int_t runNo, EPMusrHandleTag tag);

    virtual void SetFitRange(const PDoublePairVector fitRange);
    virtual void SetFitRange(const TString fitRange);

    virtual Double_t GetSingleHistoChisq(const std::vector<Double_t>& par) const;
    virtual Double_t GetSingleHistoRRFChisq(const std::vector<Double_t>& par) const;
    virtual Double_t GetAsymmetryChisq(const std::vector<Double_t>& par) const;
    virtual Double_t GetAsymmetryRRFChisq(const std::vector<Double_t>& par) const;
    virtual Double_t GetMuMinusChisq(const std::vector<Double_t>& par) const;
    virtual Double_t GetNonMusrChisq(const std::vector<Double_t>& par) const;

    virtual Double_t GetSingleHistoChisqExpected(const std::vector<Double_t>& par, const UInt_t idx) const;
    virtual Double_t GetSingleRunChisq(const std::vector<Double_t>& par, const UInt_t idx) const;

    virtual Double_t GetSingleHistoMaximumLikelihood(const std::vector<Double_t>& par) const;
    virtual Double_t GetSingleHistoRRFMaximumLikelihood(const std::vector<Double_t>& par) const;
    virtual Double_t GetAsymmetryMaximumLikelihood(const std::vector<Double_t>& par) const;
    virtual Double_t GetAsymmetryRRFMaximumLikelihood(const std::vector<Double_t>& par) const;
    virtual Double_t GetMuMinusMaximumLikelihood(const std::vector<Double_t>& par) const;
    virtual Double_t GetNonMusrMaximumLikelihood(const std::vector<Double_t>& par) const;

    virtual UInt_t GetNoOfBinsFitted(const UInt_t idx) const;
    virtual UInt_t GetTotalNoOfBinsFitted() const;

    virtual UInt_t GetNoOfSingleHisto() const { return fRunSingleHistoList.size(); } ///< returns the number of single histogram data sets present in the msr-file
    virtual UInt_t GetNoOfSingleHistoRRF() const { return fRunSingleHistoRRFList.size(); } ///< returns the number of single histogram RRF data sets present in the msr-file
    virtual UInt_t GetNoOfAsymmetry() const { return fRunAsymmetryList.size(); }     ///< returns the number of asymmetry data sets present in the msr-file
    virtual UInt_t GetNoOfAsymmetryRRF() const { return fRunAsymmetryRRFList.size(); }  ///< returns the number of asymmetry RRF data sets present in the msr-file
    virtual UInt_t GetNoOfMuMinus() const { return fRunMuMinusList.size(); }         ///< returns the number of mu minus data sets present in the msr-file
    virtual UInt_t GetNoOfNonMusr() const { return fRunNonMusrList.size(); }         ///< returns the number of non-muSR data sets present in the msr-file

    virtual PRunData* GetSingleHisto(UInt_t index, EDataSwitch tag=kIndex);
    virtual PRunData* GetSingleHistoRRF(UInt_t index, EDataSwitch tag=kIndex);
    virtual PRunData* GetAsymmetry(UInt_t index, EDataSwitch tag=kIndex);
    virtual PRunData* GetAsymmetryRRF(UInt_t index, EDataSwitch tag=kIndex);
    virtual PRunData* GetMuMinus(UInt_t index, EDataSwitch tag=kIndex);
    virtual PRunData* GetNonMusr(UInt_t index, EDataSwitch tag=kIndex);

    virtual const PDoublePairVector *GetTemp(const TString &runName) const;
    virtual Double_t GetField(const TString &runName) const;
    virtual Double_t GetEnergy(const TString &runName) const;
    virtual const Char_t* GetSetup(const TString &runName) const;
    virtual const Char_t* GetXAxisTitle(const TString &runName, const UInt_t idx) const;
    virtual const Char_t* GetYAxisTitle(const TString &runName, const UInt_t idx) const;

  private:
    PMsrHandler *fMsrInfo;  ///< pointer to the msr-file handler
    PRunDataHandler *fData; ///< pointer to the run-data handler

    vector<PRunSingleHisto*>    fRunSingleHistoList;    ///< stores all processed single histogram data
    vector<PRunSingleHistoRRF*> fRunSingleHistoRRFList; ///< stores all processed single histogram RRF data
    vector<PRunAsymmetry*>      fRunAsymmetryList;      ///< stores all processed asymmetry data
    vector<PRunAsymmetryRRF*>   fRunAsymmetryRRFList;   ///< stores all processed asymmetry RRF data
    vector<PRunMuMinus*>        fRunMuMinusList;        ///< stores all processed mu-minus data
    vector<PRunNonMusr*>        fRunNonMusrList;        ///< stores all processed non-muSR data
};

#endif // _PRUNLISTCOLLECTION_H_
