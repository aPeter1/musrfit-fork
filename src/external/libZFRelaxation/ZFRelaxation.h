/***************************************************************************

  ZFRelaxation.h

  Author: Bastian M. Wojek

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Bastian M. Wojek                                *
 *                                                                         *
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

#ifndef _ZFRelaxation_H_
#define _ZFRelaxation_H_

#include "PUserFcnBase.h"
#include "BMWIntegrator.h"

#include <vector>
using namespace std;

//-----------------------------------------------------------------------------------------------------------------
/**
 * <p>User function for the muon-spin depolarization resulting from static Gaussian broadened randomly oriented internal fields
 * See also: E.I. Kornilov and V.Yu. Pomjakushin
 * \htmlonly Phys. Lett. A <b>153</b>, 364&#150;367 (1991), doi:<a href="http://dx.doi.org/10.1016/0375-9601(91)90959-C">10.1016/0375-9601(91)90959-C</a>
 * \endhtmlonly
 * \latexonly Phys. Lett. A \textbf{153}, 364--367 (1991), \texttt{http://dx.doi.org/10.1016/0375-9601(91)90959-C}
 * \endlatexonly
 */
class ZFMagGss : public PUserFcnBase {

public:
  // default constructor
  ZFMagGss(){}  ///< default constructor
  ~ZFMagGss(){} ///< default destructor

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

  ClassDef(ZFMagGss,1)
};

//-----------------------------------------------------------------------------------------------------------------
/**
 * <p>User function for the muon-spin depolarization resulting from static Lorentzian broadened randomly oriented internal fields
 * See also: M. I. Larkin, Y. Fudamoto, I. M. Gat, A. Kinkhabwala, K. M. Kojima, G. M. Luke, J. Merrin, B. Nachumi, Y. J. Uemura, M. Azuma, T. Saito, and M. Takano
 * \htmlonly Physica B <b>289&#150;290</b>, 153&#150;156 (2000), doi:<a href="http://dx.doi.org/10.1016/S0921-4526(00)00337-9">10.1016/S0921-4526(00)00337-9</a>
 * \endhtmlonly
 * \latexonly Physica B \textbf{289--290}, 153--156 (2000), \texttt{http://dx.doi.org/10.1016/S0921-4526(00)00337-9}
 * \endlatexonly
 */
class ZFMagExp : public PUserFcnBase {

public:
  // default constructor
  ZFMagExp(){}  ///< default constructor
  ~ZFMagExp(){} ///< default destructor

  Bool_t NeedGlobalPart() const { return false; }
  void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

  ClassDef(ZFMagExp,1)
};

//-----------------------------------------------------------------------------------------------------------------
/**
 * <p>User function for the muon-spin depolarization resulting from static Gaussian distributed fields with uniaxial anisotropy
 * See also: G. Solt
 * \htmlonly Hyperfine Interactions <b>96</b>, 167&#150;175 (1995), doi:<a href="http://dx.doi.org/10.1007/BF02066280">10.1007/BF02066280</a>
 * \endhtmlonly
 * \latexonly Hyperfine Interactions \textbf{96}, 167--175 (1995), \texttt{http://dx.doi.org/10.1007/BF02066280}
 * \endlatexonly
 */
class UniaxialStatGssKT : public PUserFcnBase {

public:
  UniaxialStatGssKT();
  virtual ~UniaxialStatGssKT();

  virtual Bool_t NeedGlobalPart() const { return false; }
  virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) { }
  virtual Bool_t GlobalPartIsValid() const { return true; }

  double operator()(double, const vector<double>&) const;

private:
  TFirstUniaxialGssKTIntegral *fIntFirst;
  TSecondUniaxialGssKTIntegral *fIntSecond;

  ClassDef(UniaxialStatGssKT,1)
};

#endif //_ZFRelaxation_H_
