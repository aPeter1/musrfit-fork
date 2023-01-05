/****************************************************************************

  PGetTheoryBlockDialog.h

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

#ifndef _PGETTHEORYBLOCKDIALOG_H_
#define _PGETTHEORYBLOCKDIALOG_H_

#include "PAdmin.h"
#include "ui_PGetTheoryBlockDialog.h"

//-----------------------------------------------------------------------------
/**
 * <p>Class handling the content of the menu: Edit/Add Block/Theory Block.
 */
class PGetTheoryBlockDialog : public QDialog, private Ui::PGetTheoryBlockDialog
{
  Q_OBJECT

  public:
    PGetTheoryBlockDialog(PAdmin *admin = 0, const QString helpUrl = "");

    QString getTheoryBlock() { return fTheoryBlock_plainTextEdit->toPlainText(); }

  private slots:
    QString getTheoFuncString();
    void addPlus();
    void addMultiply();
    void helpContent();

  private:
    PAdmin *fAdmin;   ///< pointer to the administration class object needed to extract the default theory set informations.
    QString fHelpUrl; ///< help url for the asymmetry run block
};

#endif // _PGETTHEORYBLOCKDIALOG_H_
