/****************************************************************************

  PGetParameterBlockDialog.h

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

#ifndef _PGETPARAMETERBLOCKDIALOG_H_
#define _PGETPARAMETERBLOCKDIALOG_H_

#include "ui_PGetParameterBlockDialog.h"

//-----------------------------------------------------------------------------------
/**
 * <p>Handles the content of the PARAMETER block dialog.
 */
class PGetParameterBlockDialog : public QDialog, private Ui::PGetParameterBlockDialog
{
  Q_OBJECT

  public:
    PGetParameterBlockDialog(const QString helpUrl);

    QString getParams() { return fParam_plainTextEdit->toPlainText(); }

  protected:
    bool eventFilter( QObject *obj, QEvent *ev );

  private slots:
    void paramAdd();
    void helpContent();

  private:
    QString fHelpUrl; ///< help url of the PARAMETER block description.
};

#endif // _PGETPARAMETERBLOCKDIALOG_H_
