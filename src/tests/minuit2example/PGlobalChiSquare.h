/***************************************************************************

  PGlobalChiSquare.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by Andreas Suter                                   *
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

#ifndef _PGLOBALCHISQUARE_H_
#define _PGLOBALCHISQUARE_H_

#include <vector>

#include "Minuit2/FCNBase.h"

#include "mn2test.h"

namespace ROOT {
  namespace Minuit2 {

class PGlobalChiSquare : public FCNBase
{

  public:
    PGlobalChiSquare(PRunList &runList);
    ~PGlobalChiSquare();

    double Up() const { return 1.0; }
    double operator()(const std::vector<double>&) const;

  private:
    PRunList fRunList;

};

  } // namespace Minuit2
} // namespace ROOT

#endif // _PGLOBALCHISQUARE_H_
