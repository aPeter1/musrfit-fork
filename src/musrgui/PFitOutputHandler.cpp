/****************************************************************************

  PFitOutputHandler.cpp

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

#include <qtimer.h>

#include "PFitOutputHandler.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PFitOutputHandler::PFitOutputHandler(QString workingDirectory, QValueVector<QString> &cmd)
{
  if (cmd.empty())
    return;

  // Layout
  fVbox = new QVBox( this );
  fVbox->resize(800, 500);
  fOutput = new QTextEdit( fVbox );
  fOutput->setMinimumSize(800, 455);
  fOutput->setReadOnly(true);
  fQuitButton = new QPushButton( tr("Fitting..."), fVbox );
  connect( fQuitButton, SIGNAL( clicked() ), this, SLOT( quitButtonPressed() ) );
  resize( 800, 500 );
  fQuitButton->setFocus();

  // QProcess related code
  fProc = new QProcess( this );

  fProc->setWorkingDirectory(workingDirectory);

  // Set up the command and arguments.
  for (unsigned int i=0; i<cmd.size(); i++)
    fProc->addArgument(cmd[i]);

  connect( fProc, SIGNAL( readyReadStdout() ), this, SLOT( readFromStdOut() ) );
  connect( fProc, SIGNAL( readyReadStderr() ), this, SLOT( readFromStdErr() ) );
  connect( fProc, SIGNAL( processExited() ),   this, SLOT( processDone() ) );

  if ( !fProc->start() ) {
    // error handling
    QMessageBox::critical( 0,
                tr("Fatal error"),
                tr("Could not execute the output command: "+cmd[0]),
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
  if (fProc->isRunning()) {
//    qDebug("fProc still running");
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
  fOutput->append( fProc->readStdout() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PFitOutputHandler::readFromStdErr()
{
  // Read and process the data.
  // Bear in mind that the data might be output in chunks.
  fOutput->append( fProc->readStderr() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PFitOutputHandler::processDone()
{
  fQuitButton->setText("Done");
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PFitOutputHandler::quitButtonPressed()
{
  // if the fitting is still taking place, kill it
  if (fProc->isRunning()) {
/*
    fProc->tryTerminate();
    QTimer::singleShot( 100, fProc, SLOT( kill() ) );
*/
    fProc->kill();
  }

  accept();
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
