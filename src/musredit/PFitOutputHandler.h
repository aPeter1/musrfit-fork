/****************************************************************************

  PFitOutputHandler.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2023 by Andreas Suter                              *
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

#include <QObject>
#include <QProcess>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QVector>

#include <cstdlib>

//---------------------------------------------------------------------------------------
/**
 * <p>This class is the capturing the output of musrfit and displays it in a dialog so
 * the user has the chance to follow the fitting process, warnings, error messages of
 * musrfit.
 */
class PFitOutputHandler : public QDialog
{
  Q_OBJECT

  public:
    PFitOutputHandler(QString workingDirectory, QVector<QString> &cmd);
    virtual ~PFitOutputHandler();

  private slots:
    virtual void readFromStdOut();
    virtual void readFromStdErr();
    virtual void quitButtonPressed();
    virtual void processDone(int exitCode, QProcess::ExitStatus exitStatus);

private:
    Q_PID fProcPID;  ///< keeps the process PID
    QProcess *fProc; ///< pointer to the musrfit process

    QVBoxLayout *fVbox; ///< pointer to the dialog layout manager
    QTextEdit *fOutput; ///< the captured musrfit output is written (read only) into this text edit object.
    QPushButton *fQuitButton; ///< quit button, either to interrupt the fit or to close the dialog at the end of the fit.
};

#endif // _PFITOUTPUTHANDLER_H_
