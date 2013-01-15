/***************************************************************************

  PUserFcnBase.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2013 by Andreas Suter                              *
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

#ifndef _PUSERFCNBASE_H_
#define _PUSERFCNBASE_H_

#include <vector>
using namespace std;

#include "TObject.h"
#include "TSAXParser.h"

//--------------------------------------------------------------------------------------------
/**
 * <p>Interface class for the user function.
 */
class PUserFcnBase : public TObject
{
  public:
    PUserFcnBase();
    virtual ~PUserFcnBase();

    virtual Bool_t NeedGlobalPart() const = 0; ///< if a user function needs a global part this function should return true, otherwise false
    virtual void SetGlobalPart(vector<void *> &globalPart, UInt_t idx) = 0; ///< if a user function is using a global part, this function is used to invoke and retrieve the proper global object
    virtual Bool_t GlobalPartIsValid() const = 0; ///< if a user function is using a global part, this function returns if the global object part is valid

    virtual Double_t operator()(Double_t t, const std::vector<Double_t> &param) const = 0;

  ClassDef(PUserFcnBase, 1)
};

//--------------------------------------------------------------------------
// This function is a replacement for the ParseFile method of TSAXParser.
//--------------------------------------------------------------------------
Int_t parseXmlFile(TSAXParser*, const Char_t*);

#endif // _PUSERFCNBASE_H_
