/****************************************************************************

  PMsr2DataDialog.h

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

#ifndef _PMSR2DATADIALOG_H_
#define _PMSR2DATADIALOG_H_

#include <QDialog>
#include <QString>

#include "musredit.h"
#include "ui_PMsr2DataDialog.h"

/**
 * <p>Class handling the content of the MusrFit/Msr2Data GUI. It collects the input
 * for B.M. Wojek's msr2data program.
 */
class PMsr2DataDialog : public QDialog, private Ui::PMsr2DataDialog
{
  Q_OBJECT

  public:
    PMsr2DataDialog(PMsr2DataParam *msr2DataParam, const QString helpUrl);

    virtual int getRunTag() { return fRunTag; }
    virtual PMsr2DataParam* getMsr2DataParam();

  public slots:
    void runFirstEntered(const QString&);
    void runLastEntered(const QString&);
    void runListEntered(const QString&);
    void runListFileNameEntered(const QString&);
    void templateRunEntered(const QString&);
    void createMsrFileOnlyChanged(int);
    void fitOnlyChanged(int);
    void globalChanged(int);
    void helpContent();

  private:
    int fRunTag; ///< -1 = not valid, 0 = first last, 1 = run list, 2 = run list file name
    PMsr2DataParam *fMsr2DataParam; ///< data structure used to handle the necessary input for msr2data.
    QString fHelpUrl; ///< help url for the Fourier block
};

#endif // _PMSR2DATADIALOG_H_
