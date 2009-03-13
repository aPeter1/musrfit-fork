/****************************************************************************

  PPrefsDialog.h

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

#ifndef _PPREFSDIALOG_H_
#define _PPREFSDIALOG_H_

#include <qcheckbox.h>

#include "forms/PPrefsDialogBase.h"

class PPrefsDialog : public PPrefsDialogBase
{
  public:
    PPrefsDialog(const bool keep_mn2_output, const bool open_mlog_after_fit, const int dump_tag);

    bool keepMinuit2Output() { return fKeepMn2Output_checkBox->isChecked(); }
    bool openMlogAfterFit() { return fOpenMlogAfterFit_checkBox->isChecked(); }
    int  getDump();

  public slots:
    void dumpAscii();
    void dumpRoot();
};

#endif // _PPREFSDIALOG_H_
