/***************************************************************************

  PMusrT0.cpp

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

#include <iostream>
#include <fstream>
using namespace std;

#include <TColor.h>
#include <TRandom.h>
#include <TROOT.h>
#include <TObjString.h>

#include "PMusrT0.h"

ClassImpQ(PMusrT0)

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 */
PMusrT0::PMusrT0()
{
}

//--------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------
/**
 *
 * \param rawRunData
 * \param histoNo
 */
PMusrT0::PMusrT0(PRawRunData *rawRunData, unsigned int histoNo)
{
}

//--------------------------------------------------------------------------
// Destructor
//--------------------------------------------------------------------------
/**
 *
 */
PMusrT0::~PMusrT0()
{
}

//--------------------------------------------------------------------------
// Done (SIGNAL)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrT0::Done(Int_t status)
{
  Emit("Done(Int_t)", status);
}

//--------------------------------------------------------------------------
// HandleCmdKey (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrT0::HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected)
{
  if (event != kKeyPress)
     return;

//   cout << ">this          " << this << endl;
//   cout << ">fMainCanvas   " << fMainCanvas << endl;
//   cout << ">selected      " << selected << endl;
//
//cout << "x : "  << (char)x << endl;
//cout << "px: "  << (char)fMainCanvas->GetEventX() << endl;

  // handle keys and popup menu entries
  if (x == 'q') { // quit
    Done(0);
  } else if (x == 'u') { // unzoom to the original range
    cout << endl << "will unzoom ..." << endl;
  } else if (x == 't') { // set t0 channel
    cout << endl << "will set t0 channel ..." << endl;
  } else if (x == 'b') { // set first background channel
    cout << endl << "will set first background channel ..." << endl;
  } else if (x == 'B') { // set last background channel
    cout << endl << "will set last background channel ..." << endl;
  } else if (x == 'd') { // set first data channel
    cout << endl << "will set first data channel ..." << endl;
  } else if (x == 'D') { // set last data channel
    cout << endl << "will set last data channel ..." << endl;
  }
}

//--------------------------------------------------------------------------
// HandleMenuPopup (SLOT)
//--------------------------------------------------------------------------
/**
 * <p>
 *
 */
void PMusrT0::HandleMenuPopup(Int_t id)
{
}

//--------------------------------------------------------------------------
// END
//--------------------------------------------------------------------------
