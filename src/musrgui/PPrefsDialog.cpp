/****************************************************************************

  PPrefsDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

*****************************************************************************/

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

#include "PPrefsDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PPrefsDialog::PPrefsDialog(const bool keep_mn2_output, const int dump_tag)
{
  if (keep_mn2_output)
    fKeepMn2Output_checkBox->setChecked(true);
  else
    fKeepMn2Output_checkBox->setChecked(false);

  if (dump_tag == 1) {
    fDumpAscii_checkBox->setChecked(true);
    fDumpRoot_checkBox->setChecked(false);
  } else if (dump_tag == 2) {
    fDumpAscii_checkBox->setChecked(false);
    fDumpRoot_checkBox->setChecked(true);
  } else {
    fDumpAscii_checkBox->setChecked(false);
    fDumpRoot_checkBox->setChecked(false);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
int PPrefsDialog::getDump()
{
  int result = 0;

  if (fDumpAscii_checkBox->isChecked())
    result = 1;
  else if (fDumpRoot_checkBox->isChecked())
    result = 2;

  return result;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PPrefsDialog::dumpAscii()
{
  if (fDumpAscii_checkBox->isChecked())
    fDumpRoot_checkBox->setChecked(false);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PPrefsDialog::dumpRoot()
{
  if (fDumpRoot_checkBox->isChecked())
    fDumpAscii_checkBox->setChecked(false);
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
