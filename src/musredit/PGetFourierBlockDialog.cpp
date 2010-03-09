/****************************************************************************

  PGetFourierBlockDialog.cpp

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

#include <QObject>
#include <QComboBox>
#include <QValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

#include "PHelp.h"

#include "PGetFourierBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PGetFourierBlockDialog::PGetFourierBlockDialog(const QString helpUrl) : fHelpUrl(helpUrl)
{
  setupUi(this);

  setModal(true);

  fFourierBlock = "";
  fFourierPower_lineEdit->setValidator( new QIntValidator(fFourierPower_lineEdit) );
  fPhaseCorrectionRangeLow_lineEdit->setValidator( new QDoubleValidator(fPhaseCorrectionRangeLow_lineEdit) );
  fPhaseCorrectionRangeUp_lineEdit->setValidator( new QDoubleValidator(fPhaseCorrectionRangeUp_lineEdit) );
  fRangeLow_lineEdit->setValidator( new QDoubleValidator(fRangeLow_lineEdit) );
  fRangeUp_lineEdit->setValidator( new QDoubleValidator(fRangeUp_lineEdit) );

  connect( fPhase_lineEdit, SIGNAL( lostFocus() ), this, SLOT( checkPhaseParameter() ) );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PGetFourierBlockDialog::checkPhaseParameter()
{
  QString str = fPhase_lineEdit->text();

  if (str.isEmpty())
    return;

  bool ok;
  int ival;

  ival = str.toInt(&ok);
  if (!ok) { // i.e. the phase entry is not a number. Check for parXX
    str.trimmed();
    if (str.startsWith("par")) { //
      str.remove("par");
      ival = str.toInt(&ok);
      if (!ok) {
        fPhase_lineEdit->clear();
        QMessageBox::critical(this, "**ERROR**",
                              "Allowed phase entries are either a parameter number,\n or an parXX entry, where XX is a parameter number",
                              QMessageBox::Ok, QMessageBox::NoButton);
        fPhase_lineEdit->setFocus();
      }
    } else { // neither a parXX nor a number
      fPhase_lineEdit->clear();
      QMessageBox::critical(this, "**ERROR**",
                            "Allowed phase entries are either a parameter number,\n or an parXX entry, where XX is a parameter number",
                            QMessageBox::Ok, QMessageBox::NoButton);
        fPhase_lineEdit->setFocus();
    }
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PGetFourierBlockDialog::fillFourierBlock()
{
  fFourierBlock  = "###############################################################\n";
  fFourierBlock += "FOURIER\n";
  fFourierBlock += "units " + fUnits_comboBox->currentText() + "\n";
  QString str = fFourierPower_lineEdit->text();
  if (!str.isEmpty())
    fFourierBlock += "fourier_power " + str + "\n";
  fFourierBlock += "apodization " + fApodization_comboBox->currentText() + "\n";
  fFourierBlock += "plot " + fPlot_comboBox->currentText() + "\n";
  str = fPhase_lineEdit->text();
  if (!str.isEmpty())
    fFourierBlock += "phase " + str + "\n";
  if (!fPhaseCorrectionRangeLow_lineEdit->text().isEmpty() && !fPhaseCorrectionRangeUp_lineEdit->text().isEmpty()) {
    fFourierBlock += "range_for_phase_correction " + fPhaseCorrectionRangeLow_lineEdit->text() + " " +
                     fPhaseCorrectionRangeUp_lineEdit->text() + "\n";
  }
  if (!fRangeLow_lineEdit->text().isEmpty() && !fRangeUp_lineEdit->text().isEmpty()) {
    fFourierBlock += "range " + fRangeLow_lineEdit->text() + " " + fRangeUp_lineEdit->text() + "\n";
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PGetFourierBlockDialog::helpContent()
{
  if (fHelpUrl.isEmpty()) {
    QMessageBox::information(this, "**INFO**", "Will eventually show a help window");
  } else {
    PHelp *help = new PHelp(fHelpUrl);
    help->show();
  }
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
