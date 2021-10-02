/****************************************************************************

  main.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2021 by Andreas Suter                              *
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

#include <QApplication>

#ifdef HAVE_GIT_REV_H
#include "git-revision.h"
#endif

#include "PTextEdit.h"
#include "PFitOutputHandler.h"

/**
 * <p>musredit is a simple editor based interface to the musrfit programs. It is based on Qt 4.6
 * of Digia (<code>http://qt.digia.com</code> and <code>http://qt-project.org/</code>).
 *
 * <p>musredit is free software liensenced under GPL 2 or later (for detail license informations see
 * <code>http://www.gnu.org/licenses</code>).
 */
int main( int argc, char ** argv ) 
{
  Q_INIT_RESOURCE(musredit);

  if (argc == 2) {
    if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
      std::cout << std::endl << "usage: musredit [<msr-files>] | -h, --help | -v, --version";
      std::cout << std::endl << std::endl;
      return 0;
    } else if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v")) {
#ifdef HAVE_GIT_REV_H
      std::cout << std::endl << "musredit git-branch: " << GIT_BRANCH << ", git-rev: " << GIT_CURRENT_SHA1;
#else
      std::cout << std::endl << "musredit git-branch: unknown, git-rev: unknown.";
#endif
      std::cout << std::endl << std::endl;
      return 0;
    }
  }

  QApplication a( argc, argv );

  PTextEdit *mw = new PTextEdit();
  mw->setWindowTitle( "MusrFit Editor" );
  mw->resize( mw->getEditW(), mw->getEditH() );
  mw->show();

  a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
  a.connect( &a, SIGNAL( aboutToQuit() ), mw, SLOT( aboutToQuit() ) );
  return a.exec();
}
