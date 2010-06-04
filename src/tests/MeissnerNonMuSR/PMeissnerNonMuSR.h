/***************************************************************************

  PMeissnerNonMuSR.h

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

#ifndef _PMEISSNERNONMUSR_H_
#define _PMEISSNERNONMUSR_H_

#include <vector>

#include "PUserFcnBase.h"

/**
 * <p>Meissner screen profile for non-muSR \f$B_{\rm G}\f$ versus \f$\langle z \rangle\f$,
 * where \f$B_{\rm G}\f$ is the screened field obtained from a Gaussian fit, and \f$\langle z \rangle\f$
 * is the mean muon stopping range. The functional dependency is
 * \f[ B_{\rm G}(\langle z \rangle) = B_0 \cosh\left[(\tilde{t}-z)/\lambda_{ab}\right]/\cosh\left[\tilde{t}/\lambda_{ab}\right] \f]
 * where \f$\tilde{t} = t-d\f$ with \f$2 t\f$ the thickness of the sample, and \f$d\f$ the dead layer.
 */
class PMeissnerNonMuSR : public PUserFcnBase
{
  public:
    PMeissnerNonMuSR();
    ~PMeissnerNonMuSR();

    Double_t operator()(Double_t z, const std::vector<Double_t> &param) const;

  ClassDef(PMeissnerNonMuSR, 1)
};

#endif // _PMEISSNERNONMUSR_H_
