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

#include "PFitOutputHandler.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PFitOutputHandler::PFitOutputHandler(QValueVector<QString> &cmd)
{
  if (cmd.empty())
    return;

  // Layout
  vbox = new QVBox( this );
  vbox->resize(800, 500);
  output = new QTextEdit( vbox );
  output->setMinimumSize(800, 455);
  output->setReadOnly(true);
  quitButton = new QPushButton( tr("Done"), vbox );
  connect( quitButton, SIGNAL(clicked()),
           this, SLOT(accept()) );
  resize( 800, 500 );
  quitButton->setFocus();

  // QProcess related code
  proc = new QProcess( this );

  // Set up the command and arguments.
  for (unsigned int i=0; i<cmd.size(); i++)
    proc->addArgument(cmd[i]);

  connect( proc, SIGNAL(readyReadStdout()), this, SLOT(readFromStdOut()) );
  connect( proc, SIGNAL(readyReadStderr()), this, SLOT(readFromStdErr()) );
//  connect( proc, SIGNAL(processExited()), this, SLOT(scrollToTop()) );

  if ( !proc->start() ) {
    // error handling
    QMessageBox::critical( 0,
                tr("Fatal error"),
                tr("Could not start the musrfit command: "+cmd[0]),
                tr("Quit") );
    done(0);
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
  output->append( proc->readStdout() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PFitOutputHandler::readFromStdErr()
{
  // Read and process the data.
  // Bear in mind that the data might be output in chunks.
  output->append( proc->readStderr() );
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
