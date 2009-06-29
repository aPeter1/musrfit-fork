/***************************************************************************

  PNL_PippardFitter.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009 by Andreas Suter                                   *
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

#ifndef _PNL_PIPPARDFITTER_H_
#define _PNL_PIPPARDFITTER_H_

#include "PUserFcnBase.h"

class PNL_PippardFitter : public PUserFcnBase
{
  public:
    PNL_PippardFitter();
    virtual ~PNL_PippardFitter();

    virtual Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  ClassDef(PNL_PippardFitter, 1)
};

#endif // _PNL_PIPPARDFITTER_H_
