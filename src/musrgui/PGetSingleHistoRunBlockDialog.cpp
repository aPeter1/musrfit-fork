/****************************************************************************

  PGetSingleHistoRunBlockDialog.cpp

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
#include <qcheckbox.h>
#include <qmessagebox.h>

#include "PGetSingleHistoRunBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PGetSingleHistoRunBlockDialog::PGetSingleHistoRunBlockDialog(const QString help, const bool lifetimeCorrection,
                                                             QWidget * parent, const char *name,
                                                             bool modal, WFlags f) :
              PGetSingleHistoRunBlockDialogBase(parent, name, modal, f),
              fHelp(help)
{
  fForward_lineEdit->setValidator( new QIntValidator(fForward_lineEdit) );
  fNorm_lineEdit->setValidator( new QIntValidator(fNorm_lineEdit) );
  fDataStart_lineEdit->setValidator( new QIntValidator(fDataStart_lineEdit) );
  fDataEnd_lineEdit->setValidator( new QIntValidator(fDataEnd_lineEdit) );
  fBackgroundFix_lineEdit->setValidator( new QDoubleValidator(fBackgroundFix_lineEdit) );
  fBackgroundFit_lineEdit->setValidator( new QIntValidator(fBackgroundFit_lineEdit) );
  fBackgroundStart_lineEdit->setValidator( new QIntValidator(fBackgroundStart_lineEdit) );
  fBackgroundEnd_lineEdit->setValidator( new QIntValidator(fBackgroundEnd_lineEdit) );
  fFitRangeStart_lineEdit->setValidator( new QDoubleValidator(fFitRangeStart_lineEdit) );
  fFitRangeEnd_lineEdit->setValidator( new QDoubleValidator(fFitRangeEnd_lineEdit) );
  fPacking_lineEdit->setValidator( new QIntValidator(fPacking_lineEdit) );
  fT0_lineEdit->setValidator( new QIntValidator(fT0_lineEdit) );
  fLifetime_lineEdit->setValidator( new QIntValidator(fLifetime_lineEdit) );
  fLifetimeCorrection_checkBox->setChecked(lifetimeCorrection);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QString PGetSingleHistoRunBlockDialog::getRunHeaderInfo()
{
  QString str="";

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
QString PGetSingleHistoRunBlockDialog::getMap(bool &valid)
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
QString PGetSingleHistoRunBlockDialog::getData(bool &valid)
{
  QString str="";

  if (fDataStart_lineEdit->text().isEmpty() || fDataEnd_lineEdit->text().isEmpty()) {
    valid = false;
  } else {
    str  = "data            ";
    str += fDataStart_lineEdit->text() + "   ";
    str += fDataEnd_lineEdit->text() + "\n";
    valid = true;
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QString PGetSingleHistoRunBlockDialog::getBackground(bool &valid)
{
  QString str="";

  valid = true;

  // check that either backgr.fix or background is given, but not both
  if (fBackgroundStart_lineEdit->text().isEmpty() && fBackgroundEnd_lineEdit->text().isEmpty() &&
      fBackgroundFix_lineEdit->text().isEmpty() &&
      fBackgroundFit_lineEdit->text().isEmpty()) {
    valid = false;
    str = "background      0  10\n";
  } else {
    if (!fBackgroundStart_lineEdit->text().isEmpty()) { // assume the rest is given, not fool prove but ...
      str  = "background      ";
      str += fBackgroundStart_lineEdit->text() + "   ";
      str += fBackgroundEnd_lineEdit->text() + "\n";
    }
    if (!fBackgroundFix_lineEdit->text().isEmpty()) {
      str  = "backgr.fix      ";
      str += fBackgroundFix_lineEdit->text() + "\n";
    }
    if (!fBackgroundFit_lineEdit->text().isEmpty()) {
      str  = "backgr.fit      ";
      str += fBackgroundFit_lineEdit->text() + "\n";
    }
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QString PGetSingleHistoRunBlockDialog::getFitRange(bool &valid)
{
  QString str="";

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
QString PGetSingleHistoRunBlockDialog::getPacking(bool &present)
{
  QString str="";

  if (fPacking_lineEdit->text().isEmpty()) {
    present = false;
    str += "packing         1\n";
  } else {
    present = true;
    str += "packing         " + fPacking_lineEdit->text() + "\n\n";
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QString PGetSingleHistoRunBlockDialog::getT0(bool &present)
{
  QString str="";

  if (!fT0_lineEdit->text().isEmpty()) {
    str  = "t0              ";
    str += fT0_lineEdit->text() + "\n";
    present = true;
  } else {
    present = false;
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QString PGetSingleHistoRunBlockDialog::getMuonLifetimeParam(bool &present)
{
  QString str="";

  if (!fLifetime_lineEdit->text().isEmpty()) {
    str  = "lifetime        ";
    str += fLifetime_lineEdit->text() + "\n";
    present = true;
  } else {
    present = false;
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QString PGetSingleHistoRunBlockDialog::getLifetimeCorrection(bool &present)
{
  QString str="";

  if (fLifetimeCorrection_checkBox->isChecked()) {
    str = "lifetimecorrection\n";
    present = true;
  } else {
    present = false;
  }

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PGetSingleHistoRunBlockDialog::helpContents()
{
  QMessageBox::information(this, "helpContents",
                            fHelp, QMessageBox::Ok);
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
