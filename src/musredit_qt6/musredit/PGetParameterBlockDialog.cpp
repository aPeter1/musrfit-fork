/****************************************************************************

  PGetParameterBlockDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2021 by Andreas Suter                              *
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
#include <QPushButton>
#include <QEvent>
#include <QDesktopServices>
#include <QUrl>

#include "PGetParameterBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param helpUrl help url of the PARAMETER block.
 */
PGetParameterBlockDialog::PGetParameterBlockDialog(const QString helpUrl) : fHelpUrl(helpUrl)
{
  setupUi(this);

  setModal(true);

  // setup event filter
  installEventFilter(this);

  fValue_lineEdit->setValidator( new QDoubleValidator(fValue_lineEdit) );
  fStep_lineEdit->setValidator( new QDoubleValidator(fStep_lineEdit) );

  fParam_plainTextEdit->setFont(QFont("Courier", 10));
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>This event filter is filtering out the return key, and if present adds the current parameters
 * to the parameter list.
 *
 * \param obj object which was generating the event.
 * \param ev event of the object.
 */
bool PGetParameterBlockDialog::eventFilter( QObject *obj, QEvent *ev )
{
  if (obj == this) {
    if (ev->type() == QEvent::KeyPress) {
      QKeyEvent *k = (QKeyEvent*)ev;
      if (k->key() == Qt::Key_Return) {
        paramAdd();
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
/**
 * <p>Adds a parameter to the text edit window.
 */
void PGetParameterBlockDialog::paramAdd()
{
  QString param, str, spaces;

  // get No
  str = fParamNo_spinBox->text();
  if (str.toInt() < 10)
    param = "         " + str + " ";
  else
    param = "        " + str + " ";

  // get name
  str = fName_lineEdit->text();
  if (str.isEmpty()) {
    QMessageBox::critical(this, "ERROR",
       "empty parameter name not allowed!",
       QMessageBox::Ok, QMessageBox::NoButton);
    return;
  } else {
    param += str;
    if (str.length() < 13)
      param += spaces.fill(' ', 13-str.length());
    else
      param += " ";
  }

  // get value
  str = fValue_lineEdit->text();
  if (str.isEmpty()) {
    QMessageBox::critical(this, "ERROR",
       "empty parameter value not allowed!",
       QMessageBox::Ok, QMessageBox::NoButton);
    return;
  } else {
    param += str;
    if (str.length() < 10)
      param += spaces.fill(' ', 10-str.length());
    else
      param += " ";
  }

  // get step
  str = fStep_lineEdit->text();
  if (str.isEmpty()) {
    QMessageBox::critical(this, "ERROR",
       "empty parameter step not allowed!",
       QMessageBox::Ok, QMessageBox::NoButton);
    return;
  } else {
    param += str;
    if (str.length() < 10)
      param += spaces.fill(' ', 10-str.length());
    else
      param += " ";
  }

  // add positive error none
  param += "none       ";

  if ((fLower_lineEdit->text() != "none") || (fUpper_lineEdit->text() != "none")) {
    // get lower boundary
    str = fLower_lineEdit->text();
    bool ok;
    double val = str.toDouble(&ok);
    if (!ok && (str != "none")) {
      QMessageBox::critical(this, "ERROR",
         "invalid lower boundary! Must be a double are the key word 'none'",
         QMessageBox::Ok, QMessageBox::NoButton);
      return;
    } else {
      param += str;
      if (str.length() < 10)
        param += spaces.fill(' ', 10-str.length());
      else
        param += " ";
    }

    // get upper boundary
    str = fUpper_lineEdit->text();
    val = str.toDouble(&ok);
    if (!ok && (str != "none")) {
      QMessageBox::critical(this, "ERROR",
         "invalid upper boundary! Must be a double are the key word 'none'",
         QMessageBox::Ok, QMessageBox::NoButton);
      return;
    } else {
      param += str;
      if (str.length() < 10)
        param += spaces.fill(' ', 10-str.length());
      else
        param += " ";
    }
  }

//  param += "\n";

  // write parameter string into fParam_textEdit
  fParam_plainTextEdit->appendPlainText(param);

  // increment No counter in spinBox
  fParamNo_spinBox->stepUp();

  // reset name lineEdit
  fName_lineEdit->setText("");

  // reset value lineEdit
  fValue_lineEdit->setText("");

  // reset step lineEdit
  fStep_lineEdit->setText("");

  // reset lower boundary lineEdit
  fLower_lineEdit->setText("none");

  // reset upper boundary lineEdit
  fUpper_lineEdit->setText("none");

  fName_lineEdit->setFocus();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Generates a help content window showing the description of the FITPARAMETER block.
 */
void PGetParameterBlockDialog::helpContent()
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
