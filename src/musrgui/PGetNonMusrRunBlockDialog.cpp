/****************************************************************************

  PGetNonMusrRunBlockDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

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

#include <qlineedit.h>
#include <qvalidator.h>
#include <qcombobox.h>
#include <qmessagebox.h>

#include "PGetNonMusrRunBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PGetNonMusrRunBlockDialog::PGetNonMusrRunBlockDialog(const QString help, QWidget *parent, const char *name,
                                                     bool modal, WFlags f) :
                                                     PGetNonMusrRunBlockDialogBase(parent, name, modal, f),
                                                     fHelp(help)
{
  fFitRangeStart_lineEdit->setValidator( new QDoubleValidator(fFitRangeStart_lineEdit) );
  fFitRangeEnd_lineEdit->setValidator( new QDoubleValidator(fFitRangeEnd_lineEdit) );

  int idx = -1;
  for (int i=0; i<fFileFormat_comboBox->count(); i++) {
    if (fFileFormat_comboBox->text(i) == "DB") {
      idx = i;
      break;
    }
  }
  if (idx >= 0) {
    fFileFormat_comboBox->setCurrentItem(idx);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QString PGetNonMusrRunBlockDialog::getRunHeaderInfo()
{
  QString str;

  str  = "RUN " + fRunFileName_lineEdit->text() + " ";
  str += fBeamline_lineEdit->text().upper() + " ";
  str += fInstitute_comboBox->currentText() + " ";
  str += fFileFormat_comboBox->currentText() + "   (name beamline institute data-file-format)\n";

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QString PGetNonMusrRunBlockDialog::getMap(bool &valid)
{
  QString str = fMap_lineEdit->text().stripWhiteSpace().remove(" ");

  // check if potentially proper map line
  for (unsigned int i=0; i<str.length(); i++) {
    if (!str[i].isDigit()) {
      valid = false;
      break;
    }
  }

  str = "map             " + fMap_lineEdit->text() + "\n";

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QString PGetNonMusrRunBlockDialog::getXYData(bool &valid)
{
  QString str = "";

  if (fXData_lineEdit->text().isEmpty() || fYData_lineEdit->text().isEmpty()) {
    valid = false;
  } else {
    str  = "xy-data         ";
    str += fXData_lineEdit->text() + "   ";
    str += fYData_lineEdit->text() + "\n";
    valid = true;
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QString PGetNonMusrRunBlockDialog::getFitRange(bool &valid)
{
  QString str = "";

  if (fFitRangeStart_lineEdit->text().isEmpty() || fFitRangeEnd_lineEdit->text().isEmpty()) {
    str += "fit             0.0   10.0\n";
    valid = false;
  } else {
    str += "fit             ";
    str += fFitRangeStart_lineEdit->text() + "   ";
    str += fFitRangeEnd_lineEdit->text() + "\n";
    valid = true;
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PGetNonMusrRunBlockDialog::helpContents()
{
  QMessageBox::information(this, "helpContents",
                            fHelp, QMessageBox::Ok);
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
