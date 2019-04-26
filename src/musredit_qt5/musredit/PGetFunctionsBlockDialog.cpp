/****************************************************************************

  PGetFunctionsBlockDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2019 by Andreas Suter                              *
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

#include <QTextEdit>
#include <QLineEdit>
#include <QMessageBox>
#include <QRegExp>
#include <QEvent>

#include <QtDebug>

#include "PHelp.h"

#include "PGetFunctionsBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param helpUrl help url for the FUNCTIONS block.
 */
PGetFunctionsBlockDialog::PGetFunctionsBlockDialog(const QString helpUrl) : fHelpUrl(helpUrl)
{
  setupUi(this);

  setModal(true);

  fFunctionInput_lineEdit->setFocus();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Adds a function to the FUNCTIONS block text edit after carrying out some primitve tests about
 * the consistency of the function (far from being a syntax/semantic checker!!).
 */
void PGetFunctionsBlockDialog::addFunction()
{
  QString str = fFunctionInput_lineEdit->text();

  if (str.isEmpty())
    return;

  // validation

  // check that the function string starts with 'fun'
  if (!str.trimmed().startsWith("fun")) {
    QMessageBox::critical(this, "addFunction",
                          "a function has to start with 'funX' (X a number).\nNeeds to be fixed.",
                          QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  // check if function string contains 'funX ='
  if (str.indexOf( QRegExp("fun\\d+\\s*=") ) == -1) {
    QMessageBox::critical(this, "addFunction",
                          "a function has to start with 'funX =' (X a positive number).\nNeeds to be fixed.",
                          QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  // check if function string contains more than one 'funX'
  if (str.trimmed().lastIndexOf("fun", -1, Qt::CaseInsensitive) > 0) {
    QMessageBox::critical(this, "addFunction",
                          "a function cannot contain more than one function,\ni.e. fun2 = fun1 + par1 is not OK.",
                          QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  // add to Functions block
  fFunctionBlock_plainTextEdit->appendPlainText(str);

  // clear functions input text
  fFunctionInput_lineEdit->clear();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Generates a help content window showing the description of the FUNCTIONS block.
 */
void PGetFunctionsBlockDialog::helpContent()
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
// END
//----------------------------------------------------------------------------------------------------
