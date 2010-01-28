/****************************************************************************

  main.cpp

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

#include <iostream>
using namespace std;

#include <QApplication>

#include "PTextEdit.h"
#include "PFitOutputHandler.h"

int main( int argc, char ** argv ) 
{
  Q_INIT_RESOURCE(musredit);

  if (argc == 2) {
    if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
      cout << endl << "usage: musrgui [<msr-files>] | -h, --help | -v, --version";
      cout << endl << endl;
      return 0;
    } else if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v")) {
      cout << endl << "musrgui version: $Id: main.cpp 3728 2009-02-27 11:52:00Z nemu $";
      cout << endl << endl;
      return 0;
    }
  }

  QApplication a( argc, argv );

  PTextEdit * mw = new PTextEdit();
  mw->setWindowTitle( "MusrFit Editor" );
  mw->resize( 800, 800 );
  mw->show();

  a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
  return a.exec();
}
