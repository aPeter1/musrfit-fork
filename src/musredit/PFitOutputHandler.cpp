/****************************************************************************

  PFitOutputHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2014 by Andreas Suter                              *
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

#include <QTimer>

#include <QtDebug>

#include "PFitOutputHandler.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>Sets up the fit output handler GUI and starts the actual fit
 *
 * \param workingDirectory string holding the working directory wished. In this directory the mlog-file will be saved.
 * \param cmd command string vector. From this the actuall fit command will be generated and executed.
 */
PFitOutputHandler::PFitOutputHandler(QString workingDirectory, QVector<QString> &cmd)
{
  if (cmd.empty())
    return;

  // Layout
  fVbox = new QVBoxLayout( this );
//Qt.3x  fVbox->resize(800, 500);
  fOutput = new QTextEdit();
  fVbox->addWidget(fOutput);
  fOutput->setMinimumSize(800, 455);
  fOutput->setReadOnly(true);
  connect( fOutput, SIGNAL( destroyed() ), this, SLOT( quitButtonPressed() ) );
  fQuitButton = new QPushButton( tr("Fitting...") );
  fVbox->addWidget(fQuitButton);
  connect( fQuitButton, SIGNAL( clicked() ), this, SLOT( quitButtonPressed() ) );
  resize( 800, 500 );
  fQuitButton->setFocus();

  // QProcess related code
  fProc = new QProcess( this );

  fProc->setWorkingDirectory(workingDirectory);

  // Set up the command and arguments.
  QString program = cmd[0];
  QStringList arguments;
  for (int i=1; i<cmd.size(); i++)
    arguments << cmd[i];

  connect( fProc, SIGNAL( readyReadStandardOutput() ), this, SLOT( readFromStdOut() ) );
  connect( fProc, SIGNAL( readyReadStandardError() ), this, SLOT( readFromStdErr() ) );
  connect( fProc, SIGNAL( finished(int, QProcess::ExitStatus) ),   this, SLOT( processDone(int, QProcess::ExitStatus) ) );

  fProc->start(program, arguments);
  if ( !fProc->waitForStarted() ) {
    // error handling
    QString msg(tr("Could not execute the output command: ")+cmd[0]);
    QMessageBox::critical( 0,
                tr("Fatal error"),
                msg,
                tr("Quit") );
    done(0);
  }
  fProcPID = fProc->pid();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Destructor. Checks if the a fit is still running and if yes try to kill it.
 */
PFitOutputHandler::~PFitOutputHandler()
{
  if (fProc->state() == QProcess::Running) {
    fProc->terminate();
    if (!fProc->waitForFinished()) {
      qDebug() << "fProc still running, will call kill." << endl;
      fProc->kill();
    }
    fProc->waitForFinished();
  }
  if (fProc->state() == QProcess::Running) {
    QString cmd = "kill -9 "+ fProcPID;
    QString msg = "fProc still running even after Qt kill, will try system kill cmd: "+cmd;
    qDebug() << msg << endl;
    system(cmd.toLatin1());
  }
  if (fProc) {
    delete fProc;
    fProc = 0;
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Captures the standard output and adds it to the output text edit.
 */
void PFitOutputHandler::readFromStdOut()
{
  // Read and process the data.
  // Bear in mind that the data might be output in chunks.
  fOutput->append( fProc->readAllStandardOutput() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Captures the standard error and adds it to the output text edit.
 */
void PFitOutputHandler::readFromStdErr()
{
  // Read and process the data.
  // Bear in mind that the data might be output in chunks.
  fOutput->append( fProc->readAllStandardError() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>If musrfit finishes, crashes, ..., the quit button text will be changed to done.
 *
 * \param exitCode exit code of musrfit
 * \param exitStatus exit status of musrfit
 */
void PFitOutputHandler::processDone(int exitCode, QProcess::ExitStatus exitStatus)
{
  if ((exitStatus == QProcess::CrashExit) && (exitCode != 0))
    qDebug() << "**ERROR** PFitOutputHandler::processDone: exitCode = " << exitCode << endl;
  fQuitButton->setText("Done");
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>If the quit button is pressed while the fit is still running, try to terminate musrfit, if this
 * does not work, try to kill musrfit.
 */
void PFitOutputHandler::quitButtonPressed()
{
  // if the fitting is still taking place, kill it
  if (fProc->state() == QProcess::Running) {
    fProc->terminate();
    if (!fProc->waitForFinished()) {
      fProc->kill();
    }
  }

  accept();
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
