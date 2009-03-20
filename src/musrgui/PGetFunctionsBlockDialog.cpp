/****************************************************************************

  PGetFunctionsBlockDialog.cpp

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

#include <qtextedit.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qregexp.h>

#include "PGetFunctionsBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PGetFunctionsBlockDialog::PGetFunctionsBlockDialog(const QString help, QWidget *parent, const char *name,
                                                   bool modal, WFlags f) :
                                                   PGetFunctionsBlockDialogBase(parent, name, modal, f),
                                                   fHelp(help)
{
  fFunctionInput_lineEdit->setFocus();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PGetFunctionsBlockDialog::addFunction()
{
  QString str = fFunctionInput_lineEdit->text();

  // validation

  // check that the function string starts with 'fun'
  if (!str.stripWhiteSpace().startsWith("fun")) {
    QMessageBox::critical(this, "addFunction",
                          "a function has to start with 'funX' (X a number).\nNeeds to be fixed.",
                          QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  // check if function string contains 'funX ='
  if (str.find( QRegExp("fun\\d+\\s*=") ) == -1) {
    QMessageBox::critical(this, "addFunction",
                          "a function has to start with 'funX =' (X a positive number).\nNeeds to be fixed.",
                          QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  // check if function string contains more than one 'funX'
  if (str.stripWhiteSpace().findRev("fun", -1, false) > 0) {
    QMessageBox::critical(this, "addFunction",
                          "a function cannot contain more than one function,\ni.e. fun2 = fun1 + par1 is not OK.",
                          QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  // add to Functions block
  fFunctionBlock_textEdit->append(str);

  // clear functions input text
  fFunctionInput_lineEdit->clear();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PGetFunctionsBlockDialog::helpContents()
{
  QMessageBox::information(this, "helpContents",
                            fHelp, QMessageBox::Ok);
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
