/****************************************************************************

  PPrefsDialog.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010 by Andreas Suter                                   *
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

#include <QCheckBox>

#include "ui_PPrefsDialog.h"

/**
 * <p>Class handling the content of the MusrFit/Preferences.
 */
class PPrefsDialog : public QDialog, private Ui::PPrefsDialog
{
  Q_OBJECT

  public:
    PPrefsDialog(const bool keep_mn2_output, const int dump_tag, const bool title_from_data_file,
                 const bool enable_musrt0);

    bool getKeepMinuit2OutputFlag() { return fKeepMn2Output_checkBox->isChecked(); }
    bool getTitleFromDataFileFlag() { return fTitleFromData_checkBox->isChecked(); }
    bool getEnableMusrT0Flag()      { return fEnableMusrT0_checkBox->isChecked(); }
    int  getDump();

  public slots:
    void dumpAscii();
    void dumpRoot();
};

#endif // _PPREFSDIALOG_H_
