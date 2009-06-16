/****************************************************************************

  PFitOutputHandler.h

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

#ifndef _PFITOUTPUTHANDLER_H_
#define _PFITOUTPUTHANDLER_H_

#include <qobject.h>
#include <qprocess.h>
#include <qdialog.h>
#include <qvbox.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qvaluevector.h>

#include <stdlib.h>

class PFitOutputHandler : public QDialog
{
  Q_OBJECT

public:
  PFitOutputHandler(QString workingDirectory, QValueVector<QString> &cmd);
  ~PFitOutputHandler() {}

private slots:
    void readFromStdOut();
    void readFromStdErr();
    void quitButtonPressed();
    void processDone();

private:
    QProcess *fProc;
    QVBox *fVbox;
    QTextEdit *fOutput;
    QPushButton *fQuitButton;
};

#endif // _PFITOUTPUTHANDLER_H_
