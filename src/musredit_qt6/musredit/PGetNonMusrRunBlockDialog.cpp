/****************************************************************************

  PGetNonMusrRunBlockDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2023 by Andreas Suter                              *
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

#include <QLineEdit>
#include <QValidator>
#include <QComboBox>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

#include "PGetNonMusrRunBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>Constructor
 *
 * \param helpUrl help url for the NonMusr run block.
 */
PGetNonMusrRunBlockDialog::PGetNonMusrRunBlockDialog(const QString helpUrl) : fHelpUrl(helpUrl)
{
  setupUi(this);

  setModal(true);

  fFitRangeStart_lineEdit->setValidator( new QDoubleValidator(fFitRangeStart_lineEdit) );
  fFitRangeEnd_lineEdit->setValidator( new QDoubleValidator(fFitRangeEnd_lineEdit) );

  int idx = -1;
  for (int i=0; i<fFileFormat_comboBox->count(); i++) {
    if (fFileFormat_comboBox->itemText(i) == "DB") {
      idx = i;
      break;
    }
  }
  if (idx >= 0) {
    fFileFormat_comboBox->setCurrentIndex(idx);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Extracts the run header information from the dialog, and returns it as a string.
 */
QString PGetNonMusrRunBlockDialog::getRunHeaderInfo()
{
  QString str;

  str  = "RUN " + fRunFileName_lineEdit->text() + " ";
  str += fBeamline_lineEdit->text().toUpper() + " ";
  str += fInstitute_comboBox->currentText() + " ";
  str += fFileFormat_comboBox->currentText() + "   (name beamline institute data-file-format)\n";

  return str;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Extracts the map from the dialog, and returns it as a string.
 *
 * \param valid flag indicating of the map is potentially being valid.
 */
QString PGetNonMusrRunBlockDialog::getMap(bool &valid)
{
  QString str = fMap_lineEdit->text().trimmed().remove(" ");

  // check if potentially proper map line
  for (int i=0; i<str.length(); i++) {
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
 * <p>Extracts xy-data from the dialog and returns it as a string.
 *
 * \param valid flag showing that x/y-data are present.
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
 * <p>Extracts the fit-range from the dialog and returns it as a string. If not fit-range was given,
 * a fit-range from 0 to 10 us will be returned and the valid flag will be set to false.
 *
 * \param valid flag showing if a fit-range is given.
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
 * <p>Generates a help content window showing the description of the NonMusr run block.
 */
void PGetNonMusrRunBlockDialog::helpContent()
{
  if (fHelpUrl.isEmpty()) {
    QMessageBox::information(this, "INFO", "Will eventually show a help window");
  } else {
    bool ok = QDesktopServices::openUrl(QUrl(fHelpUrl, QUrl::TolerantMode));
    if (!ok) {
      QString msg = QString("<p>Sorry: Couldn't open default web-browser for the help.<br>Please try: <a href=\"%1\">musrfit docu</a> in your web-browser.").arg(fHelpUrl);
      QMessageBox::critical( nullptr, tr("FATAL ERROR"), msg, QMessageBox::Close );
    }
  }
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
