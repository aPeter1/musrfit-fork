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
 * \param keep_mn2_output if true, keep the minuit2 output for each fitted msr-file, i.e.
 *        MINUIT2.OUTPUT -> <msr-file-name>-mn2.output, and MINUIT2.root -> <msr-file-name>-mn2.root.
 *        See the '-k' option of musrfit.
 * \param dump_tag tag telling if dumps ('ascii' == 1, 'root' == 2) are wanted. See '--dump' option of musrfit.
 * \param title_from_data_file flag telling if musrfit shall, by default, take the title from the data file.
 *        See the '-t' option of musrfit.
 * \param enable_musrt0 if true, musrt0 is enabled from within musredit.
 */
PPrefsDialog::PPrefsDialog(const bool keep_mn2_output, const int dump_tag, const bool title_from_data_file,
                           const bool enable_musrt0)
{
  setupUi(this);

  setModal(true);

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

  fTitleFromData_checkBox->setChecked(title_from_data_file);
  fEnableMusrT0_checkBox->setChecked(enable_musrt0);
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
