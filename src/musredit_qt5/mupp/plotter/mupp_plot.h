/***************************************************************************
 
 mupp_plot.h
 
 Author: Andreas Suter
 e-mail: andreas.suter@psi.ch
 
 ***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2020 by Andreas Suter                              *
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

#ifndef _MUPP_PLOT_H_
#define _MUPP_PLOT_H_

#include <vector>
#include <utility>

#include <TROOT.h>
#include <TString.h>

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of a bool vector.
 */
typedef std::vector<Bool_t> PBoolVector;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of an unsigned int vector
 */
typedef std::vector<UInt_t> PUIntVector;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of an int vector
 */
typedef std::vector<Int_t> PIntVector;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of an int pair
 */
typedef std::pair<Int_t, Int_t> PIntPair;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of an int pair vector
 */
typedef std::vector<PIntPair> PIntPairVector;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of a double vector
 */
typedef std::vector<Double_t> PDoubleVector;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of a double pair
 */
typedef std::pair<Double_t, Double_t> PDoublePair;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of a double pair vector
 */
typedef std::vector<PDoublePair> PDoublePairVector;

//-------------------------------------------------------------
/**
 * <p>typedef to make to code more readable. Definition of a string vector
 */
typedef std::vector<TString> PStringVector;

#endif // _MUPP_PLOT_H_
