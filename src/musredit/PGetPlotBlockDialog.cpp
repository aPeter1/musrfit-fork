/****************************************************************************

  PGetPlotBlockDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2014 by Andreas Suter                              *
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
#include <QMessageBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QComboBox>

#include "PHelp.h"
#include "PGetPlotBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param helpUrl help url for the PLOT block.
 */
PGetPlotBlockDialog::PGetPlotBlockDialog(const QString helpUrl) : fHelpUrl(helpUrl)
{
  setupUi(this);

  setModal(true);

  // setup event filter
  installEventFilter(this);

  fXRangeLow_lineEdit->setValidator( new QDoubleValidator(fXRangeLow_lineEdit) );
  fXRangeUp_lineEdit->setValidator( new QDoubleValidator(fXRangeUp_lineEdit) );
  fYRangeLow_lineEdit->setValidator( new QDoubleValidator(fYRangeLow_lineEdit) );
  fYRangeUp_lineEdit->setValidator( new QDoubleValidator(fYRangeUp_lineEdit) );

  fPlot_plainTextEdit->setFont(QFont("Courier", 10));
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Extracts the information of the top part of the dialog, constructs a msr-file PLOT block and
 * writes it into the text edit window.
 */
void PGetPlotBlockDialog::addPlot()
{
  QString param = "";
  QString str = "";
  QString spaces;

  // add begining of plot block if fPlot_plainTextEdit is still empty
  if (fPlot_plainTextEdit->toPlainText().isEmpty()) {
    param = "###############################################################\n";
  }

  // write type
  param += "PLOT ";
  if (fType_comboBox->currentText() == "Single Histo") {
    param += "0   (single histo plot)\n";
  } else if (fType_comboBox->currentText() == "Asymmetry") {
    param += "2   (asymmetry plot)\n";
  } else if (fType_comboBox->currentText() == "MuMinus") {
    param += "4   (mu minus plot)\n";
  } else if (fType_comboBox->currentText() == "NonMusr") {
    param += "8   (non muSR plot)\n";
  }

  // write runs
  param += "runs     " + fRunList_lineEdit->text() + "\n";

  // write range
  param += "range    ";
  // lower x-/time range
  str = fXRangeLow_lineEdit->text();
  if (str.isEmpty()) {
    QMessageBox::critical(this, "**ERROR**",
       "empty lower time-/x-range name not allowed!",
       QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }
  param += str;
  if (str.length() < 8)
    param += spaces.fill(' ', 8 - str.length());
  else
    param += " ";

  // upper x-/time range
  str = fXRangeUp_lineEdit->text();
  if (str.isEmpty()) {
    QMessageBox::critical(this, "**ERROR**",
       "empty upper time-/x-range name not allowed!",
       QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }
  param += str;
  if (str.length() < 8)
    param += spaces.fill(' ', 8 - str.length());
  else
    param += " ";

  // check y-range: either none given or both
  if ((fYRangeLow_lineEdit->text().isEmpty() && !fYRangeUp_lineEdit->text().isEmpty()) ||
      (!fYRangeLow_lineEdit->text().isEmpty() && fYRangeUp_lineEdit->text().isEmpty())) {
      QMessageBox::critical(this, "**ERROR**",
         "Only fully empty y-range, or give lower AND upper y-range is acceptable!\n Will ignore the y-range",
         QMessageBox::Ok, QMessageBox::NoButton);
  } else if (!fYRangeLow_lineEdit->text().isEmpty() && !fYRangeUp_lineEdit->text().isEmpty()) {
      str = fYRangeLow_lineEdit->text();
      param += str;
      if (str.length() < 8)
        param += spaces.fill(' ', 8 - str.length());
      else
        param += " ";
      param += fYRangeUp_lineEdit->text() + "\n";
  } else {
    param += "\n";
  }
  param += "\n";

  fPlot_plainTextEdit->appendPlainText(param);

  // clean input
  fRunList_lineEdit->clear();
  fXRangeLow_lineEdit->clear();
  fXRangeUp_lineEdit->clear();
  fYRangeLow_lineEdit->clear();
  fYRangeUp_lineEdit->clear();
  fRunList_lineEdit->setFocus();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Generates a help content window showing the description of the PLOT block.
 */
void PGetPlotBlockDialog::helpContent()
{
  if (fHelpUrl.isEmpty()) {
    QMessageBox::information(this, "**INFO**", "Will eventually show a help window");
  } else {
    #ifdef _WIN32GCC
    QMessageBox::information(this, "**INFO**", "If a newer Qt version was available, a help window would be shown!");
    #else
    PHelp *help = new PHelp(fHelpUrl);
    help->show();
    #endif // _WIN32GCC
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>This event filter is filtering out the return key, and if present adds the current plot.
 *
 * \param obj object which is triggering the event
 * \param ev the triggered event
 */
bool PGetPlotBlockDialog::eventFilter( QObject *obj, QEvent *ev )
{
  if (obj == this) {
    if (ev->type() == QEvent::KeyPress) {
      QKeyEvent *k = (QKeyEvent*)ev;
      if (k->key() == Qt::Key_Return) {
        addPlot();
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else {
    return false;
  }
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
