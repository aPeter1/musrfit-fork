/****************************************************************************

  PDumpOutputHandler.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2012 by Andreas Suter                                   *
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

#include <qtimer.h>

#include "PDumpOutputHandler.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PDumpOutputHandler::PDumpOutputHandler(QValueVector<QString> &cmd)
{
  if (cmd.empty())
    return;

  // Layout
  fVbox = new QVBox( this );
  fVbox->resize(600, 800);
  fOutput = new QTextEdit( fVbox );
  fOutput->setMinimumSize(600, 755);
  fOutput->setReadOnly(true);
  connect( fOutput, SIGNAL( destroyed() ), this, SLOT(quitButtonPressed() ) );
  fQuitButton = new QPushButton( tr("Close"), fVbox );
  connect( fQuitButton, SIGNAL( clicked() ), this, SLOT( quitButtonPressed() ) );
  resize( 600, 800 );
  fQuitButton->setFocus();

  // QProcess related code
  fProc = new QProcess( this );

  // Set up the command and arguments.
  for (unsigned int i=0; i<cmd.size(); i++)
    fProc->addArgument(cmd[i]);

  connect( fProc, SIGNAL( readyReadStdout() ), this, SLOT( readFromStdOut() ) );
  connect( fProc, SIGNAL( readyReadStderr() ), this, SLOT( readFromStdErr() ) );

  if ( !fProc->start() ) {
    // error handling
    QMessageBox::critical( 0,
                tr("Fatal error"),
                tr("Could not execute the output command: "+cmd[0]),
                tr("Quit") );
    done(0);
  }
  fProcPID = fProc->processIdentifier();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PDumpOutputHandler::~PDumpOutputHandler()
{
  if (fProc->isRunning()) {
    fProc->tryTerminate();
    QTimer::singleShot( 3000, fProc, SLOT( kill() ) );
  }
  if (fProc->isRunning()) { // try low level kill
    char cmd[128];
    snprintf(cmd, sizeof(cnd), "kill -9 %ld", fProcPID);
    system(cmd);
  }
  if (fProc) {
    delete fProc;
    fProc = 0;
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PDumpOutputHandler::readFromStdOut()
{
  // Read and process the data.
  // Bear in mind that the data might be output in chunks.
  fOutput->append( fProc->readStdout() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PDumpOutputHandler::readFromStdErr()
{
  // Read and process the data.
  // Bear in mind that the data might be output in chunks.
  fOutput->append( fProc->readStderr() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PDumpOutputHandler::quitButtonPressed()
{
  // if the fitting is still taking place, kill it
  if (fProc->isRunning()) {
    fProc->tryTerminate();
    QTimer::singleShot( 1000, fProc, SLOT( kill() ) );
  }

  accept();
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
