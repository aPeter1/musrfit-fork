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

#include "forms/PGetAsymmetryRunBlockDialogBase.h"

class PGetAsymmetryRunBlockDialog : public PGetAsymmetryRunBlockDialogBase
{
  public:
    PGetAsymmetryRunBlockDialog();

    QString GetRunHeaderInfo();
    QString GetAlphaParameter(bool &present);
    QString GetBetaParameter(bool &present);
    QString GetMap(bool &valid);
    QString GetForward() { return QString("forward         " + fForward_lineEdit->text() + "\n"); }
    QString GetBackward() { return QString("backward        " + fBackward_lineEdit->text() + "\n"); }
    QString GetBackground(bool &valid);
    QString GetData(bool &valid);
    QString GetT0(bool &present);
    QString GetFitRange(bool &valid);
    QString GetPacking(bool &present);
};

#endif // _PGETASYMMETRYRUNBLOCKDIALOG_H_