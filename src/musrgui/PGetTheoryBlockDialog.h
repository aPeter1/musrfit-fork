/****************************************************************************

  PGetTheoryBlockDialog.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

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

#ifndef _PGETTHEORYBLOCKDIALOG_H_
#define _PGETTHEORYBLOCKDIALOG_H_

#include "PAdmin.h"
#include "forms/PGetTheoryBlockDialogBase.h"

class PGetTheoryBlockDialog : public PGetTheoryBlockDialogBase
{
  public:
    PGetTheoryBlockDialog(PAdmin *admin = 0,
                          QWidget * parent = 0, const char * name = 0, bool modal = FALSE,
                          WFlags f = 0);

    QString getTheoryBlock() { return fTheoryBlock_textEdit->text(); }

  private slots:
    QString getTheoFuncString();
    void addPlus();
    void addMultiply();
    void helpContents();

  private:
    PAdmin *fAdmin;
};

#endif // _PGETTHEORYBLOCKDIALOG_H_
