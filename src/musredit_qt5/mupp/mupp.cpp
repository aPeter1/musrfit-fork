/***************************************************************************

  mupp.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2017 by Andreas Suter                              *
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

#include <QObject>
#include <QScopedPointer>
#include <QApplication>
#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QStringList>

#include "mupp_version.h"
#include "PmuppScript.h"
#include "PmuppGui.h"

//------------------------------------------------------------------------
/**
 * @brief mupp_syntax
 */
void mupp_syntax()
{
  cout << endl;
  cout << "usage: mupp [OPTIONS] [[--path <fit-param-path>] <fit-param-file-names>]" << endl;
  cout << endl;
  cout << "OPTIONS:" << endl;
  cout << "  -h, --help: this help" << endl;
  cout << "  -v, --version: current mupp version" << endl;
  cout << "  -s <fln>, --script <fln>: <fln> being a mupp script." << endl;
  cout << "  --path <fit-param-path>: path where to look for the <fit-param-file-names>" << endl;
  cout << "  <fit-param-file-names>: list of file name(s) to be loaded." << endl;
  cout << "     allowed formats are: db, dat, msr" << endl << endl;
  cout << "SCRIPT COMMANDS:" << endl;
  cout << "  Lines starting with '#', '%', or '//' are comments and will be ignored." << endl;
  cout << "  The same is true for empty lines. Comments are also allowed at the end" << endl;
  cout << "  for a command, i.e. loadPath ./ # the best place ever." << endl;
  cout << endl;
  cout << "  loadPath <dir> : set the load path to <dir>. Bash variables like" << endl;
  cout << "                   $HOME are accepted." << endl;
  cout << "  load <coll>    : will load a collection <coll>. Currently *.db and *.dat" << endl;
  cout << "                   are handled." << endl;
  cout << "  selectAll      : will select all loaded collections. Thie means every plot" << endl;
  cout << "                   of variable x/y will be carried out to ALL collections." << endl;
  cout << "  select <nn>    : selects collection <nn>, where <nn> is either the number" << endl;
  cout << "                   of the collections, or its name, e.g. " << endl;
  cout << "                   select YBCO-40nm-T5K-FC150mT-Escan.db" << endl;
  cout << "  x <label>      : add <label> as a x-variable. Only one is allowed." << endl;
  cout << "  y <label(s)>   : add <label(s)> as y-variable. Multiple labls are possible." << endl;
  cout << "  norm           : normalize data to maximum." << endl;
  cout << "  savePath <dir> : set the save path to <dir>. The place where the macros," << endl;
  cout << "                   and/or the plot output will be saved." << endl;
  cout << "  plot <fln>     : where <fln> is the file name with extension under which" << endl;
  cout << "                   the plot should be saved." << endl;
  cout << "  macro <fln>    : where <fln> is the file name under which the root macro" << endl;
  cout << "                   should be saved." << endl;
  cout << endl;
}

//------------------------------------------------------------------------
/**
 * @brief mupp_scrript_read
 * @param fln
 * @param list
 * @return
 */
int mupp_script_read(const char *fln, QStringList &list)
{
  QFile file(fln);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    cerr << endl;
    cerr << "*********" << endl;
    cerr << "**ERROR** couldn't open '" << fln << "'." << endl;
    cerr << "*********" << endl;
    return -1;
  }

  QTextStream fin(&file);
  QString line;
  int pos;
  while (!fin.atEnd()) {
    line = fin.readLine();
    line = line.simplified();
    if (line.isEmpty())
      continue;
    if (line.startsWith("#") || line.startsWith("%") || line.startsWith("//"))
      continue;
    // remove all potentially present trailing comments
    pos = line.indexOf("#");
    if (pos != -1)
      line.remove(pos, line.length()-pos);
    pos = line.indexOf("%");
    if (pos != -1)
      line.remove(pos, line.length()-pos);
    pos = line.indexOf("//");
    if (pos != -1)
      line.remove(pos, line.length()-pos);
    line = line.simplified();
    // feed script list
    list << line;
  }

  return 0;
}

//------------------------------------------------------------------------
/**
 * @brief mupp_bash_variable_exists
 * @param str
 * @return
 */
bool mupp_bash_variable_exists(const QString str)
{
  QString var = str;
  var.remove("$");

  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  QString varExpanded = procEnv.value(var, "");
  if (!varExpanded.isEmpty())
    return true;

  return false;
}

//------------------------------------------------------------------------
/**
 * @brief mupp_script_syntax_check
 * @param list
 * @return
 */
int mupp_script_syntax_check(QStringList &list)
{
  QString str;
  QStringList tok;
  for (int i=0; i<list.size(); i++) {
    tok.clear();
    str = list.at(i);
    if (str.startsWith("loadPath")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() < 2) {
        cerr << endl;
        cerr << "****************" << endl;
        cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. <path> is missing." << endl;
        cerr << "****************" << endl;
        return -1;
      }
      tok.clear();

      // check if bash variables are present, and if yes try to resolve them making sure they or NOT empty.
      str = str.remove("loadPath ");
      tok = str.split('/');
      for (int i=0; i<tok.size(); i++) {
        if (tok.at(i).startsWith("$")) {
          if (!mupp_bash_variable_exists(tok.at(i))) {
            cerr << endl;
            cerr << "****************" << endl;
            cerr << "**SYNTAX ERROR** found in command '" << list.at(i).toLatin1().constData() << "' an empty bash variable '" << tok.at(i).toLatin1().constData() << "'." << endl;
            cerr << "****************" << endl;
            return -2;
          }
        }
      }
    } else if (str.startsWith("load")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() < 2) {
        cerr << endl;
        cerr << "****************" << endl;
        cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. At least one collection file name is expected." << endl;
        cerr << "****************" << endl;
        return -1;
      }
    } else if (str.startsWith("x")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() != 2) {
        cerr << endl;
        cerr << "****************" << endl;
        cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Only one variable name is accepted." << endl;
        cerr << "****************" << endl;
        return -1;
      }
    } else if (str.startsWith("y")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() < 2) {
        cerr << endl;
        cerr << "****************" << endl;
        cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. At least one variable name is needed." << endl;
        cerr << "****************" << endl;
        return -1;
      }
    } else if (str.startsWith("select ")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() != 2) {
        cerr << endl;
        cerr << "****************" << endl;
        cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Only one row-number of collection name is accepted." << endl;
        cerr << "****************" << endl;
        return -1;
      }
    } else if (str.startsWith("selectAll")) {
      if (str.compare("selectAll")) {
        cerr << endl;
        cerr << "****************" << endl;
        cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. selectAll doesn't accept any options." << endl;
        cerr << "****************" << endl;
        return -1;
      }
    } else if (str.startsWith("savePath")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() != 2) {
        cerr << endl;
        cerr << "****************" << endl;
        cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Only one save path is accepted." << endl;
        cerr << "****************" << endl;
        return -1;
      }
    } else if (str.startsWith("plot")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() != 2) {
        cerr << endl;
        cerr << "****************" << endl;
        cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Only one file name is accepted." << endl;
        cerr << "****************" << endl;
        return -1;
      }
      // check extension
      tok.clear();
      tok = str.split('.', QString::SkipEmptyParts);
      QString ext = tok.at(tok.size()-1);
      ext = ext.toLower();
      if ((ext != "pdf") && (ext != "jpg") && (ext != "png") && (ext != "svg") && (ext != "gif")) {
        cerr << endl;
        cerr << "****************" << endl;
        cerr << "**SYNTAX ERROR** found extension " << ext.toLatin1().constData() << " in '" << str.toLatin1().constData() << "' which is not supported." << endl;
        cerr << "   Currently only: pdf, jpg, png, svg, and gif are supported." << endl;
        cerr << "****************" << endl;
        return -2;
      }
    } else if (str.startsWith("macro")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() != 2) {
        cerr << endl;
        cerr << "****************" << endl;
        cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Only one file name is accepted." << endl;
        cerr << "****************" << endl;
        return -1;
      }
      // check extension
      tok.clear();
      tok = str.split('.', QString::SkipEmptyParts);
      QString ext = tok.at(tok.size()-1);
      ext = ext.toLower();
      if (ext != "c") {
        cerr << endl;
        cerr << "****************" << endl;
        cerr << "**SYNTAX ERROR** found extension " << ext.toLatin1().constData() << " in '" << str.toLatin1().constData() << "' which is not supported." << endl;
        cerr << "   Currently only: C are supported." << endl;
        cerr << "****************" << endl;
        return -2;
      }
    } else if (str.startsWith("norm")) {
      // nothing-to-be-done
    } else {
      cerr << endl;
      cerr << "*********" << endl;
      cerr << "**ERROR** found unrecognized script command: '" << str.toLatin1().constData() << "'." << endl;
      cerr << "*********" << endl;
      cerr << " Currently supported:" << endl;
      cerr << "   load <coll(s)>  : where <coll(s)> is a file-name-list of collections (*.db, *.dat)" << endl;
      cerr << "   loadPath <path> : where <path> is the load path accepting bash variables like $HOME, etc." << endl;
      cerr << "   x <var-name>    : where <var-name> is a data tag of the db/dat-file." << endl;
      cerr << "   y <var-name>    : where <var-name> is a data tag of the db/dat-file." << endl;
      cerr << "   select <nn>     : where <nn> is the row-number or collection name of the collection to be selected." << endl;
      cerr << "   selectAll       : i.e. already set addX, addY will apply to ALL collections present." << endl;
      cerr << "   savePath <path> : where <path> is the save path accepting bash variables like $HOME, etc." << endl;
      cerr << "   plot <fln>      : where <fln> is the file name with extension under which the plot should be saved." << endl;
      cerr << "   macro <fln>     : where <fln> is the file name under which the root macro should be saved." << endl;
      cerr << endl;
      return -3;
    }
  }

  return 0;
}

//------------------------------------------------------------------------
/**
 * @brief createApplication
 * @param argc
 * @param argv
 * @param gui
 * @return
 */
QCoreApplication* createApplication(int &argc, char *argv[], bool gui)
{
  if (gui)
    return new QApplication(argc, argv);
  else
    return new QCoreApplication(argc, argv);
}

//------------------------------------------------------------------------
/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(mupp);

  QStringList fln;
  QStringList script;

  if (argc > 1) {
    if (!qstrcmp(argv[1], "-h") || !qstrcmp(argv[1], "--help")) {
      mupp_syntax();
      return 0;
    } else if (!qstrcmp(argv[1], "-v") || !qstrcmp(argv[1], "--version")) {
      cout << endl << "mupp git-branch " << GIT_BRANCH << ", git-revision: " << GIT_COMMIT_HASH << ", version: " << MUPP_VERSION << endl << endl;
      return 0;
    } else if (!qstrcmp(argv[1], "-s") || !qstrcmp(argv[1], "--script")) {
      if (argc != 3) {
        cerr << endl;
        cerr << "*********" << endl;
        cerr << "**ERROR** the script option cannot be combined with something else." << endl;
        cerr << "*********" << endl;
        mupp_syntax();
        return -1;
      }
      // make sure that the script file exists
      if (!QFile::exists(argv[2])) {
        cerr << endl;
        cerr << "*********" << endl;
        cerr << "**ERROR** the script file '" << argv[2] << "' doesn't exist." << endl;
        cerr << "*********" << endl << endl;
        return -1;
      }

      // read script file
      if (mupp_script_read(argv[2], script) != 0) {
        cerr << endl;
        cerr << "*********" << endl;
        cerr << "**ERROR** couldn't read script '" << argv[2] << "'" << endl;
        cerr << "*********" << endl;
        return -1;
      }

      // check syntax of the script
      if (mupp_script_syntax_check(script) != 0) {
        return -1;
      }
    } else if (argc >= 2) { // assume it should be a fit-param-file-name list
      // check if a path is given
      int start=1;
      QString path="";
      if (!qstrcmp(argv[1], "--path")) {
        if (argc < 4) {
          cerr << endl;
          cerr << "*********" << endl;
          cerr << "**ERROR** tag '--path' needs to be followed by a <fit-param-path> and at least one <fit-param-file-name>" << endl;
          cerr << "*********" << endl;
          mupp_syntax();
          return -1;
        }
        path = argv[2];
        start = 3;
      }
      for (int i=start; i<argc; i++) {
        fln.push_back(argv[i]);
      }
      if (!path.isEmpty()) {
        for (int i=0; i<fln.size(); i++) {
          fln[i].prepend(path+"/");
        }
      }
    }
  }

  bool guiFlag=true;
  if (script.size() > 0) { // script
    guiFlag = false;
  }
  QCoreApplication *app = createApplication(argc, argv, guiFlag);

  if (qobject_cast<QApplication *>(app)) { // GUI
    PmuppGui *gui = new PmuppGui(fln);
    gui->setWindowTitle( "muSR Parameter Plotter" );
    gui->resize( 800, 500 );
    gui->show();

    app->connect( app, SIGNAL( lastWindowClosed() ), app, SLOT( quit() ) );
    app->connect( app, SIGNAL( aboutToQuit() ), gui, SLOT( aboutToQuit() ) );
  } else { // scripting
    PmuppScript *mupp_script = new PmuppScript(script);
    if (mupp_script == 0) {
      cerr << endl;
      cerr << "*********" << endl;
      cerr << "**ERROR** couldn't invoke script class..." << endl;
      cerr << "*********" << endl;
      return -1;
    }

    // This will cause the application to exit when the task signals finished.
    QObject::connect( mupp_script, SIGNAL( finished() ), app, SLOT( quit() ) );

    // This will run the task from the application event loop.
    QTimer::singleShot(0, mupp_script, SLOT( executeScript() ) );
  }

  return app->exec();
}
