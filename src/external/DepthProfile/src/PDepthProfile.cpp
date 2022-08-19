/***************************************************************************

  PDepthProfile.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2022 by Andreas Suter                              *
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

#include <cassert>
#include <cmath>

#include <iostream>

#include <TSAXParser.h>
#include <TMath.h>

#include "PDepthProfile.h"

ClassImp(PDepthProfileGlobal)

//--------------------------------------------------------------------------
// Constructor (PDepthProfileGlobal)
//--------------------------------------------------------------------------
/**
 * <p>Constructor. Reads the necessary rge-files based on the depth_profile_startup.xml
 */
PDepthProfileGlobal::PDepthProfileGlobal() {
    // load all the TRIM.SP rge-files
    fRgeHandler = new PRgeHandler("./depth_profile_startup.xml");
    if (!fRgeHandler->IsValid()) {
        std::cout << std::endl << ">> PDepthProfileGlobal::PDepthProfileGlobal **PANIC ERROR**";
        std::cout << std::endl << ">>  rge data handler too unhappy. Will terminate unfriendly, sorry.";
        std::cout << std::endl;
        fValid = false;
    }
}

//--------------------------------------------------------------------------
// Destructor (PDepthProfileGlobal)
//--------------------------------------------------------------------------
/**
 * <p>Clean up the rge-handler.
 */
PDepthProfileGlobal::~PDepthProfileGlobal() {
    if (fRgeHandler) {
        delete fRgeHandler;
        fRgeHandler = nullptr;
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

ClassImp(PDepthProfile)

//--------------------------------------------------------------------------
// Destructor (PDepthProfile)
//--------------------------------------------------------------------------
/**
 * <p>Clean up the global part.
 */
PDepthProfile::~PDepthProfile() {
    if ((fDepthProfileGlobal != 0) && fInvokedGlobal) {
        delete fDepthProfileGlobal;
        fDepthProfileGlobal = nullptr;
    }
}

//--------------------------------------------------------------------------
// SetGlobalPart (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 *
 * \param globalPart
 * \param idx
 */
void PDepthProfile::SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) {
    fIdxGlobal = static_cast<Int_t>(idx);

    if ((Int_t) globalPart.size() <= fIdxGlobal) {
        fDepthProfileGlobal = new PDepthProfileGlobal();
        if (fDepthProfileGlobal == nullptr) {
            fValid = false;
            std::cerr << std::endl
                      << ">> PDepthProfile::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..."
                      << std::endl;
        } else if (!fDepthProfileGlobal->IsValid()) {
            fValid = false;
            std::cerr << std::endl
                      << ">> PDepthProfile::SetGlobalPart(): **ERROR** initialization of global user function object failed, sorry ..."
                      << std::endl;
        } else {
            fValid = true;
            fInvokedGlobal = true;
            globalPart.resize(fIdxGlobal + 1);
            globalPart[fIdxGlobal] = dynamic_cast<PDepthProfileGlobal *>(fDepthProfileGlobal);
        }
    } else {
        fValid = true;
        fDepthProfileGlobal = (PDepthProfileGlobal * )
        globalPart[fIdxGlobal];
    }
}

//--------------------------------------------------------------------------
// GlobalPartIsValid (public)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 */
Bool_t PDepthProfile::GlobalPartIsValid() const {
    return (fValid && fDepthProfileGlobal->IsValid());
}

//--------------------------------------------------------------------------
// GetStoppingProbability()
//--------------------------------------------------------------------------

double PDepthProfileGlobal::GetStoppingProbability(double a, double b, Double_t energy) const {

    // calculation of stopping probability for a given  z interval and experimental energy
    // \int n(z, E) dz ~ dz [1/2 n_0 + n_1 + n_2 + ... + n_(N-1) + 1/2 n_N]
    energy = energy * 1000;
    double zMax = fRgeHandler->GetZmax(energy);

    std::vector<double> z;
    double x = a;
    double xMax = b;
    std::cout  << " a: " << x << "xmax: " << xMax<< std::endl;

    while (x < xMax) {
        //z[j]=x;
        z.push_back(x);
        x++;
    }

    double probability = 0;
    double prob = 0;
    for (int j = a; j < b - 1; j++) {
        prob = fRgeHandler->Get_n(energy, j);
        if ((j == a) || (j == b - 1)) {
        }
        probability = probability + prob;
    }

    return probability;
}


//--------------------------------------------------------------------------
// operator()
//--------------------------------------------------------------------------

Double_t PDepthProfile::operator()(Double_t t, const std::vector <Double_t> &param) const {
    //verify number of parameters: 2n+1
    // parameters: {E,f1, f2, ..., f_n, x1, ..., x_(n-1)}
    assert(param.size() > 2);
    assert(((param.size() - 1) % 2) == 0);
    //number of steps: n+1
    int n = (param.size() - 1) / 2;
    std::vector<double> parameters;

    if (fPreviousParam.size() == 0) {
        for (int i = 0; i < param.size(); i++) {
            fPreviousParam.push_back(param[i]);
            parameters.push_back(param[i]);
        }
    }else{
        for (int i = 0;  i <(n+1); i++) {
            if (param[i]>=0 & param[i] <=1) {
                parameters.push_back(param[i]);
            } else{
                parameters.push_back(fPreviousParam[i]);
            }
        }
        if (n==1){
            if(param[n + 1]>0) {
                parameters.push_back(param[n+1]);
            } else{
                parameters.push_back(fPreviousParam[n+1]);
            }
        } else {
            for (int i = n + 1; i < param.size() ; i++) {
            //std::cout << "param[i]: " << param[i] << "param[i+1]: " << param[i + 1] << std::endl;
                if (i!=(param.size() -1)){
                    if (param[i] > param[i + 1]) {
                        parameters.push_back(fPreviousParam[i+1]);
                    } else {
                    //std::cout<<"bem" << std::endl;
                        parameters.push_back(param[i]);
                    }
                } else{
                    if (param[i] < param[i - 1])  {
                        parameters.push_back(fPreviousParam[i-1]);
                    } else {
                        //std::cout<<"bem" << std::endl;
                        parameters.push_back(param[i]);
                    }
                }
            }
        }
        for (int i = 0; i < param.size(); i++) {
            fPreviousParam.push_back(parameters[i]);
            std::cout << "param[i]: " << fPreviousParam[i] << std::endl;
        }
    }

    Double_t energy = t;
    std::vector<double> probability;

    // get stopping probability
    //int l=0;
    for (int i = 0; i < n + 1; i++) {
        double probE;
        double a;
        double b;
        if (i == 0) { //prob between 0 and x_1
            a = 0;
            b = parameters[n + 1];
        } else if (i == n) { //prob between x_(n-1) and inf
            a = parameters[2 * n];
            b = 179;
        } else {//prob between x_1-x_2, ..., x_n-x_(n-1)
            a = parameters[n + i];
            b = parameters[n + 1 + i];
        }
        probE = fDepthProfileGlobal->GetStoppingProbability(a, b, energy);
        probability.push_back(probE);
    }

    double fit=0;

    for (int j = 0; j < n + 1; j++) {
        fit = fit + parameters[j] * probability[j];
    }

   /* std::cout << "Energy  " << energy << std::endl;
    std::cout << "FRACTION  " << fit << std::endl;*/
    return fit;

}


