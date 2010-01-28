/****************************************************************************

  PFitOutputHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id: PFitOutputHandler.cpp 4148 2009-09-11 12:37:01Z nemu $

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

#include <QTimer>

#include <QtDebug>

#include "PFitOutputHandler.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PFitOutputHandler::~PFitOutputHandler()
{
  if (fProc->state() == QProcess::Running) {
    qDebug() << "fProc still running" << endl;
    fProc->kill();
  }
/*
  if (fProc->isRunning()) {
    QString msg = "fProc still running ...";
    qDebug(msg);
  }
*/
  if (fProc) {
    delete fProc;
    fProc = 0;
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PFitOutputHandler::readFromStdOut()
{
  // Read and process the data.
  // Bear in mind that the data might be output in chunks.
  fOutput->append( fProc->readAllStandardOutput() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PFitOutputHandler::readFromStdErr()
{
  // Read and process the data.
  // Bear in mind that the data might be output in chunks.
  fOutput->append( fProc->readAllStandardError() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PFitOutputHandler::processDone(int exitCode, QProcess::ExitStatus exitStatus)
{
  if (exitStatus == QProcess::CrashExit)
    qDebug() << "**ERROR** PFitOutputHandler::processDone: exitCode = " << exitCode << endl;
  fQuitButton->setText("Done");
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PFitOutputHandler::quitButtonPressed()
{
  // if the fitting is still taking place, kill it
  if (fProc->state() == QProcess::Running) {
    fProc->terminate();
    if (!fProc->waitForFinished())
      fProc->kill();
  }

  accept();
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
