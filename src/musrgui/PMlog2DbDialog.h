/****************************************************************************

  PMlog2DbDialog.h

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

#ifndef _PMLOG2DBDIALOG_H_
#define _PMLOG2DBDIALOG_H_

#include "musrgui.h"
#include "forms/PMlog2DbDialogBase.h"

class PMlog2DbDialog : public PMlog2DbDialogBase
{
  public:
    PMlog2DbDialog(PMlog2DbDataSet *mlog2DbDataSet);

    virtual int getRunTag() { return fRunTag; }
    virtual PMlog2DbDataSet* getMlog2DbDataSet();

  public slots:
    void runFirstLastEntered();
    void runListEntered();
    void runListFileNameEntered();

  private:
    int fRunTag; // -1 = not valid, 0 = first last, 1 = run list, 2 = run list file name
    PMlog2DbDataSet *fMlog2DbDataSet;
};

#endif // _PMLOG2DBDIALOG_H_
