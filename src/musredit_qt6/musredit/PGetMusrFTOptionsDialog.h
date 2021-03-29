/****************************************************************************

  PGetMusrFTOptionsDialog.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2021 by Andreas Suter                              *
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

#ifndef _PGETMUSRFTOPTIONSDIALOG_H_
#define _PGETMUSRFTOPTIONSDIALOG_H_

#include <QDialog>
#include <QStringList>

#include "ui_PGetMusrFTOptionsDialog.h"

class PGetMusrFTOptionsDialog : public QDialog, private Ui::PGetMusrFTOptionsDialog
{
  Q_OBJECT

  public:
    PGetMusrFTOptionsDialog(QString currentMsrFile, QStringList &prevCmd, const QString helpUrl);
    QStringList getMusrFTOptions();

  public slots:
    void helpContent();

  private slots:
    void currentMsrFileTagChanged(int state);
    void allMsrFileTagChanged(int state);
    void selectMsrFileNames();
    void clearMsrFileNames();
    void selectDataFileNames();
    void clearDataFileNames();
    void createMsrFileChanged(int state);
    void resetAll();
    void averagedAll(int state);
    void averagedPerDataSet(int state);

  private:
    QStringList fMsrFilePaths;   ///< list keeping all the paths from the msr-file path-name list
    QStringList fMsrFileNames;   ///< list keeping all the names from the msr-file path-name list
    QStringList fDataFilePaths;  ///< list keeping all the paths from the data-file path-name list
    QStringList fDataFileNames;  ///< list keeping all the names from the data-file path-name list
    QString fCreateMsrFileName;  ///< keeps the msr-path-file name for msr-file creation
    QString fCurrentMsrFileName; ///< keeps the msr-path-file name of the currently active msr-file in musredit.
    QString fHelpUrl; ///< help url for the asymmetry run block
};

#endif // _PGETMUSRFTOPTIONSDIALOG_H_
