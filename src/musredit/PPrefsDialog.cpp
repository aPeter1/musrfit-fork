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
 * <p>Constructor.
 *
 * \param fAdmin keeps all the needed flags
 */
PPrefsDialog::PPrefsDialog(PAdmin *admin) : fAdmin(admin)
{
  if (!fAdmin)
    return;

  setupUi(this);

  setModal(true);

  fKeepMn2Output_checkBox->setChecked(fAdmin->getKeepMinuit2OutputFlag());

  if (fAdmin->getDumpAsciiFlag() && !fAdmin->getDumpRootFlag()) {
    fDumpAscii_checkBox->setChecked(true);
    fDumpRoot_checkBox->setChecked(false);
  } else if (!fAdmin->getDumpAsciiFlag() && fAdmin->getDumpRootFlag()) {
    fDumpAscii_checkBox->setChecked(false);
    fDumpRoot_checkBox->setChecked(true);
  } else {
    fDumpAscii_checkBox->setChecked(false);
    fDumpRoot_checkBox->setChecked(false);
  }

  fTitleFromData_checkBox->setChecked(fAdmin->getTitleFromDataFileFlag());
  fEnableMusrT0_checkBox->setChecked(fAdmin->getEnableMusrT0Flag());
  fPerRunBlockChisq_checkBox->setChecked(fAdmin->getChisqPerRunBlockFlag());
  fEstimateN0_checkBox->setChecked(fAdmin->getEstimateN0Flag());

  fTimeout_lineEdit->setText(QString("%1").arg(fAdmin->getTimeout()));
  fTimeout_lineEdit->setValidator(new QIntValidator(fTimeout_lineEdit));
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns the dump flag (see the '--dump' option of musrfit). 0 == no dump, 1 == ascii dump, 2 == root dump
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
 * <p>SLOT: called when the QCheckBox 'dump ascii' is selected. Will uncheck 'dump root' since these
 * two options are mutually exclusive.
 */
void PPrefsDialog::dumpAscii()
{
  if (fDumpAscii_checkBox->isChecked())
    fDumpRoot_checkBox->setChecked(false);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: called when the QCheckBox 'dump root' is selected. Will uncheck 'dump ascii' since these
 * two options are mutually exclusive.
 */
void PPrefsDialog::dumpRoot()
{
  if (fDumpRoot_checkBox->isChecked())
    fDumpAscii_checkBox->setChecked(false);
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
