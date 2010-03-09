/****************************************************************************

  PGetAsymmetryRunBlockDialog.h

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

#ifndef _PGETASYMMETRYRUNBLOCKDIALOG_H_
#define _PGETASYMMETRYRUNBLOCKDIALOG_H_

#include "ui_PGetAsymmetryRunBlockDialog.h"

class PGetAsymmetryRunBlockDialog : public QDialog, private Ui::PGetAsymmetryRunBlockDialog
{
  Q_OBJECT

  public:
    PGetAsymmetryRunBlockDialog(const QString helpUrl);

    QString getRunHeaderInfo();
    QString getAlphaParameter(bool &present);
    QString getBetaParameter(bool &present);
    QString getMap(bool &valid);
    QString getForward() { return QString("forward         " + fForwardHistoNo_lineEdit->text() + "\n"); }
    QString getBackward() { return QString("backward        " + fBackwardHistoNo_lineEdit->text() + "\n"); }
    QString getBackground(bool &valid);
    QString getData(bool &valid);
    QString getT0(bool &present);
    QString getFitRange(bool &valid);
    QString getPacking(bool &present);

  private slots:
    void helpContent();

  private:
    QString fHelpUrl;
};

#endif // _PGETASYMMETRYRUNBLOCKDIALOG_H_
