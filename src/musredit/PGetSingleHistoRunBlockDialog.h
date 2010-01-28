/****************************************************************************

  PGetSingleHistoRunBlockDialog.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id: PGetSingleHistoRunBlockDialog.h 3788 2009-03-19 07:58:16Z nemu $

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

#ifndef _PGETSINGLEHISTORUNBLOCKDIALOG_H_
#define _PGETSINGLEHISTORUNBLOCKDIALOG_H_

#include <QString>
#include <QLineEdit>

#include "ui_PGetSingleHistoRunBlockDialog.h"

class PGetSingleHistoRunBlockDialog : public QDialog, private Ui::PGetSingleHistoRunBlockDialog
{
  Q_OBJECT

  public:
    PGetSingleHistoRunBlockDialog(const QString help = "", const bool lifetimeCorrection = true,
                                  QWidget * parent = 0, Qt::WindowFlags f = 0);

    QString getRunHeaderInfo();
    QString getMap(bool &valid);
    QString getForward() { return QString("forward         " + fForwardHistoNo_lineEdit->text() + "\n"); }
    QString getNorm() { return QString("norm            " + fNorm_lineEdit->text() + "\n"); }
    QString getData(bool &valid);
    QString getBackground(bool &valid);
    QString getFitRange(bool &valid);
    QString getPacking(bool &present);
    QString getT0(bool &present);
    QString getMuonLifetimeParam(bool &present);
    QString getLifetimeCorrection(bool &present);

  private slots:
    void helpContent();

  private:
    QString fHelp;
};

#endif // _PGETSINGLEHISTORUNBLOCKDIALOG_H_
