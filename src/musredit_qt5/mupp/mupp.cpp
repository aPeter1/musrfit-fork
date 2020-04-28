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

#include <QObject>
#include <QScopedPointer>
#include <QApplication>
#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QVector>

#include "mupp_version.h"
#include "PmuppScript.h"
#include "PmuppGui.h"

//------------------------------------------------------------------------
/**
 * @brief mupp_syntax
 */
void mupp_syntax()
{
  std::cout << std::endl;
  std::cout << "usage: mupp [OPTIONS] [[--path <fit-param-path>] <fit-param-file-names>]" << std::endl;
  std::cout << std::endl;
  std::cout << "OPTIONS:" << std::endl;
  std::cout << "  -h, --help: this help" << std::endl;
  std::cout << "  -v, --version: current mupp version" << std::endl;
  std::cout << "  -s <fln>, --script <fln>: <fln> being a mupp script." << std::endl;
  std::cout << "  --path <fit-param-path>: path where to look for the <fit-param-file-names>" << std::endl;
  std::cout << "  <fit-param-file-names>: list of file name(s) to be loaded." << std::endl;
  std::cout << "     allowed formats are: db, dat, msr" << std::endl << std::endl;
  std::cout << "SCRIPT COMMANDS:" << std::endl;
  std::cout << "  Lines starting with '#', '%', or '//' are comments and will be ignored." << std::endl;
  std::cout << "  The same is true for empty lines. Comments are also allowed at the end" << std::endl;
  std::cout << "  for a command, i.e. loadPath ./ # the best place ever." << std::endl;
  std::cout << std::endl;
  std::cout << "  loadPath <dir> : set the load path to <dir>. Bash variables like" << std::endl;
  std::cout << "                   $HOME are accepted." << std::endl;
  std::cout << "  load <coll>    : will load a collection <coll>. Currently *.db and *.dat" << std::endl;
  std::cout << "                   are handled." << std::endl;
  std::cout << "  selectAll      : will select all loaded collections. Thie means every plot" << std::endl;
  std::cout << "                   of variable x/y will be carried out to ALL collections." << std::endl;
  std::cout << "  select <nn>    : selects collection <nn>, where <nn> is either the number" << std::endl;
  std::cout << "                   of the collections, or its name, e.g. " << std::endl;
  std::cout << "                   select YBCO-40nm-T5K-FC150mT-Escan.db" << std::endl;
  std::cout << "  x <label>      : add <label> as a x-variable. Only one is allowed." << std::endl;
  std::cout << "  y <label(s)>   : add <label(s)> as y-variable. Multiple labls are possible." << std::endl;
  std::cout << "  norm           : normalize data to maximum." << std::endl;
  std::cout << "  savePath <dir> : set the save path to <dir>. The place where the macros," << std::endl;
  std::cout << "                   and/or the plot output will be saved." << std::endl;
  std::cout << "  plot <fln>     : where <fln> is the file name with extension under which" << std::endl;
  std::cout << "                   the plot should be saved." << std::endl;
  std::cout << "  macro <fln>    : where <fln> is the file name under which the root macro" << std::endl;
  std::cout << "                   should be saved." << std::endl;
  std::cout << std::endl;
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
    std::cerr << std::endl;
    std::cerr << "*********" << std::endl;
    std::cerr << "**ERROR** couldn't open '" << fln << "'." << std::endl;
    std::cerr << "*********" << std::endl;
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
void mupp_script_syntax()
{
  std::cerr << std::endl;
  std::cerr << " Currently supported script commands:" << std::endl;
  std::cerr << "   load <coll(s)>  : where <coll(s)> is a file-name-list of collections (*.db, *.dat)" << std::endl;
  std::cerr << "   loadPath <path> : where <path> is the load path accepting bash variables like $HOME, etc." << std::endl;
  std::cerr << "   x <var-name>    : where <var-name> is a data tag of the db/dat-file." << std::endl;
  std::cerr << "   y <var-name>    : where <var-name> is a data tag of the db/dat-file." << std::endl;
  std::cerr << "   select <nn>     : where <nn> is the row-number or collection name of the collection to be selected." << std::endl;
  std::cerr << "   selectAll       : i.e. already set addX, addY will apply to ALL collections present." << std::endl;
  std::cerr << "   savePath <path> : where <path> is the save path accepting bash variables like $HOME, etc." << std::endl;
  std::cerr << "   plot <fln>      : where <fln> is the file name with extension under which the plot should be saved." << std::endl;
  std::cerr << "   macro <fln>     : where <fln> is the file name under which the root macro should be saved." << std::endl;
  std::cerr << "   var <var_name> = <expr> : defines a variable." << std::endl;
  std::cerr << "   col <nn> : <var_name>   : links <var_name> to the collection <nn>, where <nn> is the number of the" << std::endl;
  std::cerr << "                             collection as defined by the order of load, starting with 0." << std::endl;
  std::cerr << std::endl;
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
  QVector<QString> var_names;
  QVector<bool> var_linked;
  int noOfCollections = 0;
  for (int i=0; i<list.size(); i++) {
    tok.clear();
    str = list.at(i);
    if (str.startsWith("loadPath")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() < 2) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. <path> is missing." << std::endl;
        std::cerr << "****************" << std::endl;
        return -1;
      }
      tok.clear();

      // check if bash variables are present, and if yes try to resolve them making sure they or NOT empty.
      str = str.remove("loadPath ");
      tok = str.split('/');
      for (int i=0; i<tok.size(); i++) {
        if (tok.at(i).startsWith("$")) {
          if (!mupp_bash_variable_exists(tok.at(i))) {
            std::cerr << std::endl;
            std::cerr << "****************" << std::endl;
            std::cerr << "**SYNTAX ERROR** found in command '" << list.at(i).toLatin1().constData() << "' an empty bash variable '" << tok.at(i).toLatin1().constData() << "'." << std::endl;
            std::cerr << "****************" << std::endl;
            return -2;
          }
        }
      }
    } else if (str.startsWith("load")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() < 2) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. At least one collection file name is expected." << std::endl;
        std::cerr << "****************" << std::endl;
        return -1;
      }
      noOfCollections++;
    } else if (str.startsWith("x")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() != 2) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Only one variable name is accepted." << std::endl;
        std::cerr << "****************" << std::endl;
        return -1;
      }
    } else if (str.startsWith("y")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() < 2) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. At least one variable name is needed." << std::endl;
        std::cerr << "****************" << std::endl;
        return -1;
      }
    } else if (str.startsWith("select ")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() != 2) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Only one row-number of collection name is accepted." << std::endl;
        std::cerr << "****************" << std::endl;
        return -1;
      }
    } else if (str.startsWith("selectAll")) {
      if (str.compare("selectAll")) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. selectAll doesn't accept any options." << std::endl;
        std::cerr << "****************" << std::endl;
        return -1;
      }
    } else if (str.startsWith("savePath")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() != 2) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Only one save path is accepted." << std::endl;
        std::cerr << "****************" << std::endl;
        return -1;
      }
    } else if (str.startsWith("plot")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() != 2) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Only one file name is accepted." << std::endl;
        std::cerr << "****************" << std::endl;
        return -1;
      }
      // check extension
      tok.clear();
      tok = str.split('.', QString::SkipEmptyParts);
      QString ext = tok.at(tok.size()-1);
      ext = ext.toLower();
      if ((ext != "pdf") && (ext != "jpg") && (ext != "png") && (ext != "svg") && (ext != "gif")) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found extension " << ext.toLatin1().constData() << " in '" << str.toLatin1().constData() << "' which is not supported." << std::endl;
        std::cerr << "   Currently only: pdf, jpg, png, svg, and gif are supported." << std::endl;
        std::cerr << "****************" << std::endl;
        return -2;
      }
    } else if (str.startsWith("macro")) {
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() != 2) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Only one file name is accepted." << std::endl;
        std::cerr << "****************" << std::endl;
        return -1;
      }
      // check extension
      tok.clear();
      tok = str.split('.', QString::SkipEmptyParts);
      QString ext = tok.at(tok.size()-1);
      ext = ext.toLower();
      if (ext != "c") {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found extension " << ext.toLatin1().constData() << " in '" << str.toLatin1().constData() << "' which is not supported." << std::endl;
        std::cerr << "   Currently only: C are supported." << std::endl;
        std::cerr << "****************" << std::endl;
        return -2;
      }
    } else if (str.startsWith("norm")) {
      // nothing-to-be-done
    } else if (str.startsWith("var")) {
      tok.clear();
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() < 2) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'." << std::endl;
        std::cerr << "   Illegal variable definition." << std::endl;
        std::cerr << "****************" << std::endl;
        return -1;
      }
      // keep var name for later use
      var_names.push_back(tok[1]);
      var_linked.push_back(false);
      // the parsing etc is dealt within the scripting class
    } else if (str.startsWith("col")) {
      tok.clear();
      tok = str.split(' ', QString::SkipEmptyParts);
      if (tok.size() != 4) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Syntax for variable collection linking is:" << std::endl;
        std::cerr << "   col <nn> : <var_name>. Where <nn> is the collection index, and <var_name> the variable name to be linked." << std::endl;
        std::cerr << "****************" << std::endl;
        return -1;
      }
      if (tok[2] != ":") {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Syntax for variable collection linking is:" << std::endl;
        std::cerr << "   col <nn> : <var_name>. Where <nn> is the collection index, and <var_name> the variable name to be linked." << std::endl;
        std::cerr << "****************" << std::endl;
        return -2;
      }
      // will check that the to be linked variable and the target collection exists
      // first check that the collection exists
      bool ok;
      int idx = tok[1].toInt(&ok);
      if (!ok) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** found '" << str.toLatin1().constData() << "'. Syntax for variable collection linking is:" << std::endl;
        std::cerr << "   col <nn> : <var_name>. Where <nn> is the collection index, and <var_name> the variable name to be linked." << std::endl;
        std::cerr << "   collection index is not a number here!" << std::endl;
        std::cerr << "****************" << std::endl;
        return -2;
      }
      if (idx >= noOfCollections) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** in '" << str.toLatin1().constData() << "'." << std::endl;
        std::cerr << "   Requested collection '" << idx << "' is out-of-range." << std::endl;
        std::cerr << "   Only " << noOfCollections << " or present." << std::endl;
        std::cerr << "   Collection indexing starts at 0, not 1." << std::endl;
        std::cerr << "****************" << std::endl;
        return -3;
      }
      // make sure that the variable to be linked has been defined and is NOT an error variable.
      if (tok[3].endsWith("Err")) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** in '" << str.toLatin1().constData() << "'." << std::endl;
        std::cerr << "  Only variables can be linked, NOT error variables." << std::endl;
        std::cerr << "  Error variables are linked implicitly." << std::endl;
        std::cerr << "****************" << std::endl;
        return -4;
      }
      idx = -1;
      for (unsigned int i=0; i<var_names.size(); i++) {
        if (tok[3] == var_names[i]) {
          idx = i;
          break;
        }
      }
      if (idx == -1) {
        std::cerr << std::endl;
        std::cerr << "****************" << std::endl;
        std::cerr << "**SYNTAX ERROR** in '" << str.toLatin1().constData() << "'." << std::endl;
        std::cerr << "  Variable to be linked not defined yet." << std::endl;
        std::cerr << "****************" << std::endl;
        return -5;
      }
      var_linked[idx] = true;
    } else {
      std::cerr << "*********" << std::endl;
      std::cerr << "**ERROR** found unrecognized script command: '" << str.toLatin1().constData() << "'." << std::endl;
      std::cerr << "*********" << std::endl;
      mupp_script_syntax();
      return -3;
    }
  }
  if (noOfCollections == -1) {
    std::cerr << "****************" << std::endl;
    std::cerr << "**SYNTAX ERROR** no data loaded." << std::endl;
    std::cerr << "****************" << std::endl;
    mupp_script_syntax();
    return -4;
  }
  if (var_names.size() != var_linked.size()) {
    std::cerr << "****************" << std::endl;
    std::cerr << "**SYNTAX ERROR**" << std::endl;
    std::cerr << "****************" << std::endl;
    std::cerr << "** Your never should have reached this point **" << std::endl;
    std::cerr << "****************" << std::endl;
    return -4;
  }
  for (int i=0; i<var_names.size(); i++) {
    if (!var_names[i].endsWith("Err") && !var_linked[i]) {
      std::cerr << "****************" << std::endl;
      std::cerr << "**  WARNING   **" << std::endl;
      std::cerr << "****************" << std::endl;
      std::cerr << "  Found unlinked variable : " << var_names[i].toLatin1().constData() << std::endl;
      std::cerr << "****************" << std::endl;
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
      std::cout << std::endl << "mupp git-branch " << GIT_BRANCH << ", git-revision: " << GIT_COMMIT_HASH << ", version: " << MUPP_VERSION << std::endl << std::endl;
      return 0;
    } else if (!qstrcmp(argv[1], "-s") || !qstrcmp(argv[1], "--script")) {
      if (argc != 3) {
        std::cerr << std::endl;
        std::cerr << "*********" << std::endl;
        std::cerr << "**ERROR** the script option cannot be combined with something else." << std::endl;
        std::cerr << "*********" << std::endl;
        mupp_syntax();
        return -1;
      }
      // make sure that the script file exists
      if (!QFile::exists(argv[2])) {
        std::cerr << std::endl;
        std::cerr << "*********" << std::endl;
        std::cerr << "**ERROR** the script file '" << argv[2] << "' doesn't exist." << std::endl;
        std::cerr << "*********" << std::endl << std::endl;
        return -1;
      }

      // read script file
      if (mupp_script_read(argv[2], script) != 0) {
        std::cerr << std::endl;
        std::cerr << "*********" << std::endl;
        std::cerr << "**ERROR** couldn't read script '" << argv[2] << "'" << std::endl;
        std::cerr << "*********" << std::endl;
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
          std::cerr << std::endl;
          std::cerr << "*********" << std::endl;
          std::cerr << "**ERROR** tag '--path' needs to be followed by a <fit-param-path> and at least one <fit-param-file-name>" << std::endl;
          std::cerr << "*********" << std::endl;
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
      std::cerr << std::endl;
      std::cerr << "*********" << std::endl;
      std::cerr << "**ERROR** couldn't invoke script class..." << std::endl;
      std::cerr << "*********" << std::endl;
      return -1;
    }

    // This will cause the application to exit when the task signals finished.
    QObject::connect( mupp_script, SIGNAL( finished() ), app, SLOT( quit() ) );

    // This will run the task from the application event loop.
    QTimer::singleShot(0, mupp_script, SLOT( executeScript() ) );
  }

  return app->exec();
}
