/****************************************************************************

  PGetDefaultDialog.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2019 by Andreas Suter                              *
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

#ifndef _PGETDEFAULTDIALOG_H_
#define _PGETDEFAULTDIALOG_H_

#include <QString>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>

#include "ui_PGetDefaultDialog.h"

//---------------------------------------------------------------------------
/**
 * <p>Handels the default dialog which is used to generate a default asymmetry or
 * single histogram msr input file.
 */
class PGetDefaultDialog : public QDialog, private Ui::PGetDefaultDialog
{
  Q_OBJECT

  public:
    PGetDefaultDialog(const QString helpUrl = "");
    virtual ~PGetDefaultDialog() {}

    virtual const QString getRunFileName() const { return fRunFileName_lineEdit->text(); }
    virtual const QString getBeamline() const { return fBeamline_lineEdit->text(); }
    virtual const QString getInstitute() const { return fInstitute_comboBox->currentText(); }
    virtual const QString getFileFormat() const { return fFileFormat_comboBox->currentText(); }

    virtual void setBeamline(const QString &str) { fBeamline_lineEdit->setText(str); }
    virtual void setInstitute(const QString &str);
    virtual void setFileFormat(const QString &str);

  protected slots:
    virtual void helpContent();

  private:
    QString fHelpUrl; ///< help url for the default dialog
};

#endif // _PGETDEFAULTDIALOG_H_
