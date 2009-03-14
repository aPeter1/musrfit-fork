/****************************************************************************

  PGetSingleHistoRunBlockDialog.h

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

#ifndef _PGETSINGLEHISTORUNBLOCKDIALOG_H_
#define _PGETSINGLEHISTORUNBLOCKDIALOG_H_

#include <qstring.h>
#include <qlineedit.h>

#include "forms/PGetSingleHistoRunBlockDialogBase.h"

class PGetSingleHistoRunBlockDialog : public PGetSingleHistoRunBlockDialogBase
{
  public:
    PGetSingleHistoRunBlockDialog();

    QString GetRunHeaderInfo();
    QString GetMap(bool &valid);
    QString GetForward() { return QString("forward         " + fForward_lineEdit->text() + "\n"); }
    QString GetNorm() { return QString("norm            " + fNorm_lineEdit->text() + "\n"); }
    QString GetData(bool &valid);
    QString GetBackground(bool &valid);
    QString GetFitRange(bool &valid);
    QString GetPacking(bool &present);
    QString GetT0(bool &present);
    QString GetMuonLifetimeParam(bool &present);
    QString GetLifetimeCorrection(bool &present);
};

#endif // _PGETSINGLEHISTORUNBLOCKDIALOG_H_
