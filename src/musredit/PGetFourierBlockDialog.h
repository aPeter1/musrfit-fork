/****************************************************************************

  PGetFourierBlockDialog.h

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

#ifndef _PGETFOURIERBLOCKDIALOG_H_
#define _PGETFOURIERBLOCKDIALOG_H_

#include <QString>

#include "ui_PGetFourierBlockDialog.h"

//-------------------------------------------------------------------------------
/**
 * <p>Handles the Fourier dialog.
 */
class PGetFourierBlockDialog : public QDialog, private Ui::PGetFourierBlockDialog
{
  Q_OBJECT

  public:
    PGetFourierBlockDialog(const QString helpUrl);

    QString getFourierBlock() { return fFourierBlock; }

  private slots:
    void checkPhaseParameter();
    void fillFourierBlock();
    void helpContent();

  private:
    QString fFourierBlock;  ///< keeps the msr Fourier block
    QString fHelpUrl;       ///< help url for the Fourier block
};

#endif // _PGETFOURIERBLOCKDIALOG_H_
