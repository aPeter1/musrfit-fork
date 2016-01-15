/***************************************************************************

  PUserFcn.h

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

#ifndef _PUSERFCN_H_
#define _PUSERFCN_H_

#include <vector>

#include "PUserFcnBase.h"

/**
 * <p>User function example class. Polynome of 3rd order.
 */
class PUserFcn : public PUserFcnBase
{
  public:
    PUserFcn();
    ~PUserFcn();

    Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  ClassDef(PUserFcn, 1)
};

#endif // _PUSERFCN_H_
