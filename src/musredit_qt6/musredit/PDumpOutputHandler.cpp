/****************************************************************************

  PDumpOutputHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2012-2021 by Andreas Suter                              *
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

#include "PDumpOutputHandler.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>Sets up the dump output handler GUI and starts the actual dump_header command
 *
 * \param cmd command string vector. From this the actual dump_header command will be generated and executed.
 */
PDumpOutputHandler::PDumpOutputHandler(QVector<QString> &cmd)
{
  if (cmd.empty())
    return;

  // Layout
  fVbox = new QVBoxLayout( this );
  fOutput = new QTextEdit();
  fVbox->addWidget(fOutput);
  fOutput->setMinimumSize(600, 755);
  fOutput->setReadOnly(true);
  connect( fOutput, SIGNAL( destroyed() ), this, SLOT( quitButtonPressed() ) );
  fQuitButton = new QPushButton( tr("Quit") );
  fVbox->addWidget(fQuitButton);
  connect( fQuitButton, SIGNAL( clicked() ), this, SLOT( quitButtonPressed() ) );
  resize( 600, 800 );
  fQuitButton->setFocus();

  // QProcess related code
  fProc = new QProcess( this );

  // make sure that the system environment variables are properly set
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#if defined(Q_OS_DARWIN)
  env.insert("DYLD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("DYLD_LIBRARY_PATH"));
#else
  env.insert("LD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("LD_LIBRARY_PATH"));
#endif
  fProc->setProcessEnvironment(env);

  // Set up the command and arguments.
  QString program = cmd[0];
  QStringList arguments;
  for (int i=1; i<cmd.size(); i++)
    arguments << cmd[i];

  connect( fProc, SIGNAL( readyReadStandardOutput() ), this, SLOT( readFromStdOut() ) );
  connect( fProc, SIGNAL( readyReadStandardError() ), this, SLOT( readFromStdErr() ) );


  fProc->start(program, arguments);
  if ( !fProc->waitForStarted() ) {
    // error handling
    QString msg(tr("Could not execute the output command: ")+cmd[0]);
    QMessageBox::critical( nullptr, tr("FATAL ERROR"), msg, QMessageBox::Close );
    done(0);
  }

  fProcPID = fProc->processId();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Destructor. Checks if the a dump_header is still running and if yes try to kill it.
 */
PDumpOutputHandler::~PDumpOutputHandler()
{
  if (fProc->state() == QProcess::Running) {
    fProc->terminate();
    if (!fProc->waitForFinished()) {
      qDebug() << "fProc still running, will call kill." << Qt::endl;
      fProc->kill();
    }
    fProc->waitForFinished();
  }
  if (fProc->state() == QProcess::Running) {
    QString cmd = "kill -9 "+ QString("%1").arg(fProcPID);
    QString msg = "fProc still running even after Qt kill, will try system kill cmd: "+cmd;
    qDebug() << msg << Qt::endl;
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
void PDumpOutputHandler::readFromStdOut()
{
  // Read and process the data.
  // Bear in mind that the data might be output in chunks.
  fOutput->append( fProc->readAllStandardOutput() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Captures the standard error and adds it to the output text edit.
 */
void PDumpOutputHandler::readFromStdErr()
{
  // Read and process the data.
  // Bear in mind that the data might be output in chunks.
  fOutput->append( fProc->readAllStandardError() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>If the quit button is pressed while the dump_header is still running, try to terminate dump_header, if this
 * does not work, try to kill dump_header.
 */
void PDumpOutputHandler::quitButtonPressed()
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
