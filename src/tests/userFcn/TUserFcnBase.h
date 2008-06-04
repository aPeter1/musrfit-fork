/***************************************************************************

  TUserFcnBase.h

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

#ifndef _TUSERFCNBASE_H_
#define _TUSERFCNBASE_H_

#include <vector>

#include <TObject.h>

class TUserFcnBase : public TObject
{
  public:
    TUserFcnBase();
    virtual ~TUserFcnBase();

    virtual Double_t Eval(Double_t t, const std::vector<Double_t> &param) const = 0;
    virtual Double_t operator()(Double_t t, const std::vector<Double_t> &param) const = 0;

  ClassDef(TUserFcnBase, 1)
};

#endif // _TUSERFCNBASE_H_
