/***************************************************************************

  PmuppGui.cpp

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

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#include <cmath>
#include <iostream>
using namespace std;

#include <QApplication>
#include <QToolBar>
#include <QMenuBar>
#include <QMessageBox>
#include <QRect>
#include <QSpacerItem>
#include <QFrame>
#include <QStringList>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QKeyEvent>
#include <QProcessEnvironment>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

#include <QtDebug>

#include "mupp_version.h"
#include "PmuppGui.h"

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppXY::init
 */
void PmuppXY::init()
{
  fCollectionTag = -1;
  fXlabel = "";
  fYlabel.clear();
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppXY::setYlabel
 * @param idx
 * @param str
 */
void PmuppXY::setYlabel(int idx, QString str)
{
  if (idx >= fYlabel.size())
    return;

  fYlabel[idx] = str;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppXY::removeYlabel
 * @param idx
 */
void PmuppXY::removeYlabel(int idx)
{
  if (idx >= fYlabel.size())
    return;

  fYlabel.remove(idx);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppXY::removeYlabel
 * @param str
 */
void PmuppXY::removeYlabel(QString str)
{
  for (int i=0; i<fYlabel.size(); i++) {
    if (fYlabel[i] == str) {
      fYlabel.remove(i);
      return;
    }
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppXY::getYlabel
 * @param idx
 * @return
 */
QString PmuppXY::getYlabel(int idx)
{
  if (idx >= fYlabel.size())
    return QString("");

  return fYlabel[idx];
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppXY::getXlabelIdx
 * @return
 */
int PmuppXY::getXlabelIdx()
{
  QString str = fXlabel;

  // remove trailing '-)'
  int idx = fXlabel.lastIndexOf("-)");
  if (idx == -1)
    return -1;
  str.remove(idx, str.length()-idx);

  // remove everything up to '(-'
  idx = fXlabel.indexOf("(-");
  if (idx == -1)
    return -1;
  idx += 2;
  str.remove(0, idx);

  bool ok;
  idx = str.toInt(&ok);
  if (!ok)
    return -1;

  return idx;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppXY::getYlabelIdx
 * @param idx
 * @return
 */
int PmuppXY::getYlabelIdx(int idx)
{
  if (idx >= fYlabel.size())
    return -1;

  QString str = fYlabel[idx];

  // remove trailing '-)'
  int iidx = fXlabel.lastIndexOf("-)");
  if (iidx == -1)
    return -1;
  str.remove(iidx, str.length()-iidx);

  // remove everything up to '(-'
  iidx = fXlabel.indexOf("(-");
  if (iidx == -1)
    return -1;
  iidx += 2;
  str.remove(0, iidx);

  bool ok;
  iidx = str.toInt(&ok);
  if (!ok)
    return -1;

  return iidx;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Constructor
 *
 * \param fln file names to be loaded
 * \param parent pointer to the parent object
 * \param f qt windows flags
 */
PmuppGui::PmuppGui( QStringList fln, QWidget *parent, Qt::WindowFlags f )
    : QMainWindow( parent, f )
{
  QDateTime dt = QDateTime::currentDateTime();
  fDatime = dt.toTime_t();

  fMuppPlot = 0;

  fMacroPath = QString("./");
  fMacroName = QString("");

  readCmdHistory();

  fAdmin = new PmuppAdmin();

  fParamDataHandler = new PParamDataHandler();
  bool dataAtStartup = false;
  if (fln.size() > 0) {
    if (fln[0].contains(".msr")) {
      // get collection Name
      QString collName = QString("collName0");
      fParamDataHandler->NewCollection(collName);
    }
    if (!fParamDataHandler->ReadParamFile(fln)) {
      // parameter file(s) is/are not valid
    } else {
      dataAtStartup = true; // delay to deal with the data sets until the GUI is ready to do so
    }
  }

  getTheme();

  QString iconName("");
  if (fDarkTheme)
    iconName = QString(":/icons/mupp-dark.svg");
  else
    iconName = QString(":/icons/mupp-plain.svg");
  setWindowIcon( QIcon( QPixmap(iconName) ) );

  // setup menus
  setupFileActions();
  setupToolActions();
  setupHelpActions();

  // create central widget
  fCentralWidget = new QWidget(this);

  // setup widgets
  fBoxLayout_Main = new QBoxLayout(QBoxLayout::TopToBottom);
  fBoxLayout_Main->setGeometry(QRect(10, 40, 600, 400));

  fBoxLayout_Top = new QBoxLayout(QBoxLayout::LeftToRight);
  fGridLayout_Left = new QGridLayout();
  fGridLayout_Right = new QGridLayout();
  fBoxLayout_Cmd = new QBoxLayout(QBoxLayout::LeftToRight);

  // label for the collection/parameter list widgets
  fColLabel = new QLabel(this);
  fColLabel->setText("Collection -> Parameter");
  fColLabel->setMaximumHeight(15);

  // central widget for the collection/parameter list widgets
  fColParamSplitter = new QSplitter(Qt::Horizontal, this);
  fColParamSplitter->setMinimumSize(550,330);

  fColList = new QListWidget(this);
  fColList->setSelectionMode(QAbstractItemView::ExtendedSelection);

  fParamList = new QListWidget(this);
  // the next two lines enable drag and drop facility (drag)
  fParamList->setSelectionMode(QAbstractItemView::SingleSelection);
  fParamList->setDragEnabled(true);

  fColParamSplitter->addWidget(fColList);
  fColParamSplitter->addWidget(fParamList);

  fRemoveCollection = new QPushButton("Remove Collection", this);
  fRefreshCollection = new QPushButton("Refresh Collection", this);

  fGridLayout_Left->addWidget(fColLabel, 1, 1, 1, 2);
  fGridLayout_Left->addWidget(fColParamSplitter, 2, 1, 1, 2);
  fGridLayout_Left->addWidget(fRemoveCollection, 3, 1);
  fGridLayout_Left->addWidget(fRefreshCollection, 3, 2);

  // X-axis
  fXaxisLabel = new QLabel("x-axis");
  fViewX = new QListWidget(this);
  // the next two lines enable drag and drop facility (copy drop)
  fViewX->viewport()->setAcceptDrops(true);
  fViewX->setDropIndicatorShown(true);

  fAddX = new QPushButton("add X", this);
  fRemoveX = new QPushButton("remove X", this);

  // Y-axis
  fYaxisLabel = new QLabel("y-axis");
  fViewY = new QListWidget(this);
  // the next two lines enable drag and drop facility (copy drop)
  fViewY->viewport()->setAcceptDrops(true);
  fViewY->setDropIndicatorShown(true);

  fAddY = new QPushButton("add Y", this);
  fRemoveY = new QPushButton("remove Y", this);

  // 2 column button's for the right grid
  fAddDitto = new QPushButton("Add Ditto", this);
  fPlot = new QPushButton("Plot", this);

  fGridLayout_Right->addWidget(fXaxisLabel, 1, 1);
  fGridLayout_Right->addWidget(fYaxisLabel, 1, 2);
  fGridLayout_Right->addWidget(fViewX, 2, 1);
  fGridLayout_Right->addWidget(fViewY, 2, 2);
  fGridLayout_Right->addWidget(fAddX, 3, 1);
  fGridLayout_Right->addWidget(fAddY, 3, 2);
  fGridLayout_Right->addWidget(fRemoveX, 4, 1);
  fGridLayout_Right->addWidget(fRemoveY, 4, 2);
  fGridLayout_Right->addWidget(fAddDitto, 5, 1, 1, 2);
  fGridLayout_Right->addWidget(fPlot, 6, 1, 1, 2);

  fBoxLayout_Top->addLayout(fGridLayout_Left);
  fBoxLayout_Top->addLayout(fGridLayout_Right);

  fCmdLine = new QLineEdit(this);
  fCmdLine->installEventFilter(this);
  fCmdLine->setText("$ ");
  fCmdLine->setFocus(); // sets the keyboard focus
  fExitButton = new QPushButton("E&xit", this);

  fBoxLayout_Cmd = new QBoxLayout(QBoxLayout::LeftToRight);
  fBoxLayout_Cmd->addWidget(fCmdLine);
  fBoxLayout_Cmd->addWidget(fExitButton);

  QFrame* hline = new QFrame();
  hline->setFrameShape(QFrame::HLine);
  hline->setFrameShadow(QFrame::Sunken);

  fBoxLayout_Main->addLayout(fBoxLayout_Top);
  fBoxLayout_Main->addWidget(hline);
  fBoxLayout_Main->addLayout(fBoxLayout_Cmd);

  // establish all the necessary signal/slots
  connect(fRefreshCollection, SIGNAL( pressed() ), this, SLOT( refresh() ));
  connect(fRemoveCollection, SIGNAL( pressed() ), this, SLOT( remove() ));
  connect(fAddX, SIGNAL( pressed() ), this, SLOT( addX() ));
  connect(fAddY, SIGNAL( pressed() ), this, SLOT( addY() ));
  connect(fRemoveX, SIGNAL( pressed() ), this, SLOT( removeX() ));
  connect(fRemoveY, SIGNAL( pressed() ), this, SLOT( removeY() ));
  connect(fAddDitto, SIGNAL( pressed() ), this, SLOT( addDitto() ));
  connect(fPlot, SIGNAL( pressed()), this, SLOT( plot()) );
  connect(fCmdLine, SIGNAL ( returnPressed() ), this, SLOT( handleCmds() ));
  connect(fExitButton, SIGNAL( pressed() ), this, SLOT( aboutToQuit() ));

  connect(fColList, SIGNAL( currentRowChanged(int) ), this, SLOT( updateParamList(int) ));
  connect(fColList, SIGNAL( itemDoubleClicked(QListWidgetItem*) ), this, SLOT( editCollName(QListWidgetItem*) ));

  connect(fViewX, SIGNAL( currentRowChanged(int) ), this, SLOT( refreshY() ));
  connect(fViewX, SIGNAL( itemChanged(QListWidgetItem*) ), this, SLOT( dropOnViewX(QListWidgetItem*) ));
  connect(fViewY, SIGNAL( itemChanged(QListWidgetItem*) ), this, SLOT( dropOnViewY(QListWidgetItem*) ));

  // deal with parameter data specifics
  if (dataAtStartup)
    handleNewData();

  connect(fParamDataHandler, SIGNAL( newData() ), this, SLOT( handleNewData() ));

  fCentralWidget->setLayout(fBoxLayout_Main);
  setCentralWidget(fCentralWidget);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::~PmuppGui
 */
PmuppGui::~PmuppGui()
{
  // all relevant clean up job are done in ::aboutToQuit()
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::aboutToQuit
 */
void PmuppGui::aboutToQuit()
{
  // kill mupp_plot if active
  if (fMuppPlot) {
    if (fMuppPlot->state() != QProcess::NotRunning) {
      fMuppPlot->terminate();
    }
  }

  // remove message queue
  key_t key;
  int flags, msqid;

  // generate the ICP message queue key
  key = ftok(QCoreApplication::applicationFilePath().toLatin1().data(), 1);

  if (key != -1) {
    // set the necessary IPC message queue flags
    flags = IPC_CREAT;

    // open the message queue
    msqid = msgget(key, flags | S_IRUSR | S_IWUSR);
    if (msqid != -1) {
      if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        cerr << "**ERROR** couldn't removed the message queue (msqid=" << msqid << ")." << endl << endl;
      }
    }
  }  

  // clean up temporary plot files
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  QString pathName = QString("%1/.musrfit/mupp/_mupp_%2.dat").arg(env.value("HOME")).arg(fDatime);
  pathName = QString("%1/.musrfit/mupp/_mupp_ftok.dat").arg(env.value("HOME"));

  // needed for clean up and to save the cmd history
  writeCmdHistory();

  if (fParamDataHandler) {
    delete fParamDataHandler;
    fParamDataHandler = 0;
  }

  if (fAdmin) {
    delete fAdmin;
    fAdmin = 0;
  }

  exit(0);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::setupFileActions
 */
void PmuppGui::setupFileActions()
{
  QToolBar *tb = new QToolBar( this );
  tb->setWindowTitle( "File Actions" );
  addToolBar( tb );

  QMenu *menu = new QMenu( tr( "F&ile" ), this );
  menuBar()->addMenu( menu );

  QAction *a;

  QString iconName("");
  if (fDarkTheme)
    iconName = QString(":/icons/document-open-dark.svg");
  else
    iconName = QString(":/icons/document-open-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Open..." ), this );
  a->setShortcut( tr("Ctrl+O") );
  a->setStatusTip( tr("Open a musrfit parameter file.") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileOpen() ) );
  menu->addAction(a);
  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/document-open-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&New..." ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( fileOpen() ) );
  }
  tb->addAction(a);

  a = new QAction( tr( "E&xit" ), this );
  a->setShortcut( tr("Ctrl+Q") );
  a->setStatusTip( tr("Exit Program") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileExit() ) );
  menu->addAction(a);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::setupToolActions
 */
void PmuppGui::setupToolActions()
{
  QMenu *menu = new QMenu( tr( "&Tools" ), this );
  menuBar()->addMenu( menu );

  QAction *a;

  a = new QAction(tr( "Plot ..." ), this );
  a->setStatusTip( tr("Plot x/y parameters") );
  connect( a, SIGNAL( triggered() ), this, SLOT( plot() ) );
  menu->addAction(a);

  a = new QAction(tr( "Create ROOT Macro ..." ), this );
  a->setStatusTip( tr("Creates a ROOT Macro with the given x/y parameters") );
  connect( a, SIGNAL( triggered() ), this, SLOT( createMacro() ) );
  menu->addAction(a);

  menu->addSeparator();

  a = new QAction(tr( "Dump Collections ..." ), this );
  a->setStatusTip( tr("Dump all collections") );
  connect( a, SIGNAL( triggered() ), this, SLOT( toolDumpCollections() ) );
  menu->addAction(a);

  a = new QAction(tr( "Dump XY ..." ), this );
  a->setStatusTip( tr("Dump XY parameter list") );
  connect( a, SIGNAL( triggered() ), this, SLOT( toolDumpXY() ) );
  menu->addAction(a);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::setupHelpActions
 */
void PmuppGui::setupHelpActions()
{
  QMenu *menu = new QMenu( tr( "&Help" ), this );
  menuBar()->addMenu( menu );

  QAction *a;
/*
  a = new QAction(tr( "Contents ..." ), this );
  a->setStatusTip( tr("Help Contents") );
  connect( a, SIGNAL( triggered() ), this, SLOT( helpContents() ));
  menu->addAction(a);
*/

  a = new QAction( tr( "Cmd's" ), this );
  a->setStatusTip( tr( "Lists the command line commands" ) );
  connect( a, SIGNAL( triggered() ), this, SLOT( helpCmds() ));
  menu->addAction(a);

  a = new QAction( tr( "Author(s) ..." ), this );
  a->setStatusTip( tr( "About the Author(s)") );
  connect( a, SIGNAL( triggered() ), this, SLOT( helpAbout() ));
  menu->addAction(a);

  a = new QAction( tr( "About Qt..." ), this );
  a->setStatusTip( tr( "Help About Qt") );
  connect( a, SIGNAL( triggered() ), this, SLOT( helpAboutQt() ));
  menu->addAction(a);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::fileOpen
 */
void PmuppGui::fileOpen()
{
  QStringList list = QFileDialog::getOpenFileNames(
                                      this,
                                      "muSR Parameter Files",
                                      "./",
                                      "db-files (*.db);; All Params (*.msr *.db *.dat);; Msr-Files (*.msr);; dat-Files (*.dat);; All (*)");

  if (list.count() == 0)
    return;

  bool msrPresent = false;
  bool dbPresent = false;
  for (int i=0; i<list.count(); i++) {
    if (list[i].endsWith(".msr"))
      msrPresent = true;
    else if (list[i].endsWith(".db") || list[i].endsWith(".dat"))
      dbPresent = true;
    else {
      QMessageBox::critical(this, "ERROR", QString("Found file with unkown extension:\n%1\nWill not do anything here.").arg(list[i]));
      return;
    }
  }

  if (msrPresent && dbPresent) {
    QMessageBox::critical(this, "ERROR", "Currently mixed reading of msr and db/dat is not supported.");
    return;
  }

  fParamDataHandler->ReadParamFile(list);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::fileExit
 */
void PmuppGui::fileExit()
{
  qApp->quit();
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::toolDump
 */
void PmuppGui::toolDumpCollections()
{
  if (fParamDataHandler->GetNoOfCollections()) {
    fParamDataHandler->Dump();
  } else {
    QMessageBox::warning(this, "dump collections", "no collections present");
  }
}

//----------------------------------------------------------------------------------------------------
void PmuppGui::toolDumpXY()
{
  if (fXY.size() > 0) {
    for (int i=0; i<fXY.size(); i++) {
      qInfo() << "<-><-><-><-><-><-><-><-><-><->";
      qInfo() << i << ": collection tag: " << fXY[i].getCollectionTag();
      qInfo() << "     x-param: " << fXY[i].getXlabel();
      for (int j=0; j<fXY[i].getYlabelSize(); j++)
        qInfo() << "     y-param: " << fXY[i].getYlabel(j);
    }
  } else {
    QMessageBox::warning(this, "dump XY", "no XY list present yet");
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::helpCmds
 */
void PmuppGui::helpCmds()
{
  QMessageBox::information(this, "cmd help", "<b>help:</b> this help.<br>"\
                           "<b>exit/quit:</b> close mupp.<br>"\
                           "<b>load:</b> calls the open file dialog.<br>"\
                           "<b>dump collections:</b> dumps all currently loaded collections.<br>"\
                           "<b>dump XY:</b> dumps X/Y tree.<br>"\
                           "<b>refresh:</b> refresh the currently selected collection.<br>"\
                           "<b>plot:</b> plot the X/Y data.<br>"\
                           "<b>macro &lt;fln&gt;:</b> saves the X/Y data as a ROOT macro (*.C).<br>"\
                           "<b>path &lt;macroPath&gt;:</b> sets the path to the place where the macros will be saved.<br>"\
                           "<b>select collection &lt;nn&gt;:</b> select collection in GUI.<br>"\
                           "&nbsp;&nbsp;&nbsp;&nbsp;&lt;nn&gt; is either the collection name or the row number.<br>"\
                           "<b>flush:</b> flush the history buffer.");
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::helpAbout
 */
void PmuppGui::helpAbout()
{
  QMessageBox::information(this, "about", QString("mupp: created by Andreas Suter.\nVersion: %1\nBranch: %2\nHash: %3").arg(MUPP_VERSION).arg(GIT_BRANCH).arg(GIT_COMMIT_HASH));
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::helpAboutQt
 */
void PmuppGui::helpAboutQt()
{
  QMessageBox::aboutQt(this);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::eventFilter
 * @param o
 * @param e
 * @return
 */
bool PmuppGui::eventFilter(QObject *o, QEvent *e)
{
  static int idx = fCmdHistory.size();

  if (e->type() == QEvent::KeyPress) {
    QKeyEvent *k = static_cast<QKeyEvent*>(e);
    int key = k->key();
    if (key == Qt::Key_Up) {
      if (idx > 0)
        idx--;
      if (idx < fCmdHistory.size())
        fCmdLine->setText(fCmdHistory[idx]);
    } else if (key == Qt::Key_Down) {
      if (idx < fCmdHistory.size())
        idx++;
      if (idx < fCmdHistory.size())
        fCmdLine->setText(fCmdHistory[idx]);
      else
        fCmdLine->setText("$ ");
    } else if (key == Qt::Key_Return) {
      QString cmd = fCmdLine->text();
      bool found = false;
      for (int i=0; i<fCmdHistory.size(); i++) {
        if (cmd == fCmdHistory[i]) {
          found = true;
        }
      }
      if (!found) {
        if (fCmdHistory.size() >= 50)
          fCmdHistory.removeFirst();
        fCmdHistory.push_back(cmd);
      }
    }
    return false;
  }

  // make sure the base class can handle all the events not handled here
  return QMainWindow::eventFilter(o, e);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::getTheme
 */
void PmuppGui::getTheme()
{
  fDarkTheme = false; // true if theme is dark
  fDarkToolBarIcon = false; // needed for ubuntu dark since there the menu icons are dark, however the toolbar icons are plain!

  QString str = QIcon::themeName();

  if (str.contains("dark", Qt::CaseInsensitive)) {
    fDarkTheme = true;
    if (str.contains("ubuntu", Qt::CaseInsensitive)) {
      fDarkToolBarIcon = false;
    } else {
      fDarkToolBarIcon = true;
    }
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::readCmdHistory
 */
void PmuppGui::readCmdHistory()
{
  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  QString home = procEnv.value("HOME", "");
  if (home.isEmpty())
    return;

  QString pathName = home + "/.musrfit/mupp/mupp_history.txt";

  QFile file(pathName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    cerr << endl;
    cerr << "***********" << endl;
    cerr << "**WARNING** readCmdHistory(): couldn't open mupp_history.txt for reading ..." << endl;;
    cerr << "***********" << endl;
    return;
  }

  QTextStream fin(&file);

  fCmdHistory.clear();
  QString line;
  while (!fin.atEnd()) {
    line = fin.readLine();
    if (line.startsWith("%") || line.isEmpty())
      continue;
    fCmdHistory.push_back(line);
  }
}

//----------------------------------------------------------------------------------------------------
void PmuppGui::writeCmdHistory()
{
  if (fCmdHistory.size() == 0)
    return;

  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  QString home = procEnv.value("HOME", "");
  if (home.isEmpty())
    return;

  QString pathName = home + "/.musrfit/mupp/";
  QDir dir(pathName);
  if (!dir.exists()) {
    // directory $HOME/.musrfit/musredit does not exist hence create it
    dir.mkpath(pathName);
  }
  pathName += "mupp_history.txt";

  QFile file(pathName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    return;

  QTextStream fout(&file);

  // write header
  QDateTime dt = QDateTime::currentDateTime();
  fout << "% mupp history file" << endl;
  fout << "% " << dt.toString("HH:mm:ss - yy/MM/dd") << endl;

  // write history
  for (int i=0; i<fCmdHistory.size(); i++)
    fout << fCmdHistory[i] << endl;

  fout << "% end mupp history file" << endl;

  file.close();
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::refresh
 */
void PmuppGui::refresh()
{
  QListWidgetItem *item = fColList->currentItem();
  if (item == 0)
    return;
  QString label = item->text();

  QString pathName("");
  int collIdx=-1;

  for (int i=0; i<fParamDataHandler->GetNoOfCollections(); i++) {
    if (fParamDataHandler->GetCollection(i)->GetName() == label) { // found collection
      pathName = fParamDataHandler->GetCollection(i)->GetPathName();
      collIdx = i;
      break;
    }
  }

  // re-load collection
  PmuppCollection coll;
  bool ok=false;
  if (pathName.endsWith(".db")) {
    coll = fParamDataHandler->ReadDbFile(pathName, ok);
    if (!ok) {
      QMessageBox::critical(this, "ERROR - REFRESH",
                            QString("Couldn't refresh %1\nFile corrupted?!").arg(fParamDataHandler->GetCollection(collIdx)->GetName()));
      return;
    }
  } else if (pathName.endsWith(".dat")) {
    coll = fParamDataHandler->ReadColumnParamFile(pathName, ok);
    if (!ok) {
      QMessageBox::critical(this, "ERROR - REFRESH",
                            QString("Couldn't refresh %1\nFile corrupted?!").arg(fParamDataHandler->GetCollection(collIdx)->GetName()));
      return;
    }
  } else {
    QMessageBox::critical(this, "ERROR - REFRESH",
                          QString("Couldn't refresh %1").arg(fParamDataHandler->GetCollection(collIdx)->GetName()));
    return;
  }

  QString collName = pathName;
  int pos = collName.lastIndexOf("/");
  collName.remove(0, pos+1);
  coll.SetName(collName);
  coll.SetPathName(pathName);
  fParamDataHandler->ReplaceCollection(coll, collIdx);

  // update the parameter list (GUI)
  updateParamList(collIdx);

  // update XY list (GUI)
  updateXYListGui();
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::remove
 */
void PmuppGui::remove()
{
  // get current collection
  QListWidgetItem *item = fColList->currentItem();
  if (item == 0)
    return;

  QString collName = item->text();

  // remove it from the list widget but keep the index for further use
  int idx = fColList->row(item);
  fColList->removeItemWidget(item);
  delete item;

  // remove it from the collection handler
  fParamDataHandler->RemoveCollection(collName);

  // next it is necessary to update the XY-parameter list
  updateXYList(fColList->currentRow());

  // if there no collections left remove all the parameters from the X/Y list
  if (fColList->count() == 0) {
    fViewX->clear();
    fViewY->clear();
    return;
  }

  // select the row before the delete one if possible
  if (idx > 0)
    idx -= 1;
  fColList->setCurrentRow(idx, QItemSelectionModel::Select);

  // update the parameter list (GUI)
  updateParamList(idx);

  // update XY list (GUI)
  updateXYListGui();
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::addX
 */
void PmuppGui::addX()
{
  // get the parameter name from the parameter list widget
  QListWidgetItem *item = fParamList->currentItem();
  if (item == 0)
    return;
  QString paramName = item->text();
  int idx = fColList->currentRow();
  QString xLabel = QString("%1 (-%2-)").arg(paramName).arg(idx);

  // check if it is not already present
  for (int i=0; i<fViewX->count(); i++) {
    if (fViewX->item(i)->text() == xLabel)
      return;
  }

  // set the parameter name on the view X list widget
  fViewX->addItem(xLabel);
  fViewX->setCurrentRow(fViewX->count()-1);

  // add the item to the XY list
  PmuppXY xyItem;
  xyItem.setCollectionTag(idx);
  xyItem.setXlabel(xLabel);

  fXY.push_back(xyItem);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::addY
 */
void PmuppGui::addY()
{
  // get the parameter name from the parameter list widget
  QListWidgetItem *item = fParamList->currentItem();
  if (item == 0)
    return;
  QString paramName = item->text();
  int idx = fColList->currentRow();
  QString yLabel = QString("%1 (-%2-)").arg(paramName).arg(idx);

  // make sure that any x-parameter selection is already present
  if (fViewX->count() == 0) {
    QMessageBox::warning(this, "**WARNING**", "It is compulsory to have at least one x-parameter set\nbefore trying to add a y-parameter.");
    return;
  }

  // check if collection of x- and y-parameter selection correspond
  idx=getXlabelIndex(fViewX->currentItem()->text());
  if (idx == -1)
    return;
  if (fXY[idx].getCollectionTag() != fColList->currentRow()) {
    QMessageBox::warning(this, "**WARNING**", "x/y parameters need to originate from the same collection.");
    return;
  }

  // check if it is not already present
  for (int i=0; i<fViewY->count(); i++) {
    if (fViewY->item(i)->text() == yLabel)
      return;
  }

  // set the parameter name on the view Y list widget
  fViewY->addItem(yLabel);

  // add the item to the XY list
  idx=getXlabelIndex(fViewX->currentItem()->text());
  if (idx == -1)
    return;

  fXY[idx].addYlabel(yLabel);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::removeX
 */
void PmuppGui::removeX()
{
  QListWidgetItem *item = fViewX->currentItem();
  if (item == 0)
    return;

  int idx=getXlabelIndex(item->text());
  if (idx == -1)
    return;
  fXY.remove(idx);

  fViewX->removeItemWidget(item);
  delete item;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::removeY
 */
void PmuppGui::removeY()
{
  QListWidgetItem *item = fViewY->currentItem();
  if (item == 0)
    return;

  // find y in XY and remove it
  QString xLabel = fViewX->currentItem()->text();
  QString yLabel = item->text();
  int idx = getXlabelIndex(xLabel);
  if (idx == -1)
    return;
  fXY[idx].removeYlabel(yLabel);

  fViewY->removeItemWidget(item);
  delete item;
}


//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::addDitto
 */
void PmuppGui::addDitto()
{
  if (fXY.size() == 0) {
    QMessageBox::critical(this, "ERROR", "No x/y items present.\nTherefore nothing to be done.");
    return;
  }

  if (!allXYEqual()) {
    QMessageBox::critical(this, "ERROR", "Multiple unequal sets of x/y items present.\nDo not know how to handle.");
    return;
  }

  // get the selected items from the collection
  QList<QListWidgetItem*> items = fColList->selectedItems();
  if (items.count() == 0) { // no selections present hence drop out
    return;
  }

  // verify that all selected items have corresponding x/y values
  bool ok=false;
  PmuppCollection muCol;
  PmuppRun muRun;
  for (int i=0; i<items.count(); i++) {
    ok = false;
    muCol = fParamDataHandler->GetCollection(items[i]->text(), ok);
    if (!ok)
      return;
    muRun = muCol.GetRun(0);
    // check x-values
    if (!findValue(muRun, kXaxis))
      return;
    // check y-values
    if (!findValue(muRun, kYaxis))
      return;
  }

  // populate all necessary x- and y-values for all selected collections
  PmuppXY muXY = fXY[0];
  int idx;
  for (int i=0; i<items.count(); i++) {
    idx = fColList->row(items[i]);
    if (indexAlreadyPresent(idx))
      continue;
    replaceIndex(muXY, idx);
    fXY.push_back(muXY);
    // add x-axis view
    fViewX->addItem(muXY.getXlabel());
    fViewX->setCurrentRow(fViewX->count()-1);
  }

  // un-select the collecion list
  fColList->clearSelection();
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::findValue
 * @param run
 * @param tag
 * @return
 */
bool PmuppGui::findValue(PmuppRun &run, EAxis tag)
{
  bool result = false;

  if (tag == kXaxis) {
    for (int i=0; i<fXY.size(); i++) {
      for (int j=0; j<run.GetNoOfParam(); j++) {
        if (fXY[i].getXlabel().startsWith(run.GetParam(j).GetName())) {
          result = true;
          break;
        }
      }
    }
  } else if (tag == kYaxis) {
    for (int i=0; i<fXY.size(); i++) {
      for (int j=0; j<fXY[i].getYlabelSize(); j++) {
        for (int k=0; k<run.GetNoOfParam(); k++) {
          if (fXY[i].getYlabel(j).startsWith(run.GetParam(k).GetName())) {
            result = true;
            break;
          }
        }
      }
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::allXYEqual
 * @return
 */
bool PmuppGui::allXYEqual()
{
  // first make sure that all x-labels are equivalent. They have the form '<label> (-#-)' and
  // all <label> must be equal
  QString str1, str2;
  int idx;
  str1 = fXY[0].getXlabel();
  idx = str1.lastIndexOf(" (-");
  str1.remove(idx, str1.length()-idx);
  for (int i=1; i<fXY.size(); i++) {
    str2 = fXY[i].getXlabel();
    idx = str2.lastIndexOf(" (-");
    str2.remove(idx, str2.length()-idx);
    if (str1 != str2)
      return false;
  }

  // make sure that the number of all y-label items is equal
  for (int i=1; i<fXY.size(); i++) {
    if (fXY[0].getYlabelSize() != fXY[i].getYlabelSize())
      return false;
  }

  // make sure that the y-labels are equivalent.
  for (int j=0; j<fXY[0].getYlabelSize(); j++) {
    str1 = fXY[0].getYlabel(j);
    idx = str1.lastIndexOf(" (-");
    str1.remove(idx, str1.length()-idx);
    for (int i=1; i<fXY.size(); i++) {
      str2 = fXY[i].getYlabel(j);
      idx = str2.lastIndexOf(" (-");
      str2.remove(idx, str2.length()-idx);
      if (str1 != str2)
        return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::indexAlreadyPresent
 * @param idx
 * @return
 */
bool PmuppGui::indexAlreadyPresent(const int idx)
{
  QString str = QString("(-%1-)").arg(idx);

  for (int i=0; i<fXY.size(); i++) {
    if (fXY[i].getXlabel().contains(str))
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::replaceIndex
 * @param data
 * @param idx
 */
void PmuppGui::replaceIndex(PmuppXY &data, const int idx)
{
  // handle x-label
  QString label = data.getXlabel();
  int pos = label.lastIndexOf(" (-");
  label.truncate(pos);
  QString str = QString(" (-%1-)").arg(idx);
  label += str;
  data.setXlabel(label);

  // handle y-label(s)
  for (int i=0; i<data.getYlabelSize(); i++) {
    label = data.getYlabel(i);
    int pos = label.lastIndexOf(" (-");
    label.truncate(pos);
    label += str;
    data.setYlabel(i, label);
  }

  data.setCollectionTag(idx);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::getXlabelIndex
 * @param label
 * @return
 */
int PmuppGui::getXlabelIndex(QString label)
{
  int idx=-1;

  for (int i=0; i<fXY.size(); i++) {
    if (fXY[i].getXlabel() == label) {
      idx = i;
      break;
    }
  }

  return idx;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::dropOnViewX
 * @param item
 */
void PmuppGui::dropOnViewX(QListWidgetItem *item)
{
  if (item == 0)
    return;

  if (item->text().contains("(-"))
    return;

  int idx = fColList->currentRow();
  QString xLabel = QString("%1 (-%2-)").arg(item->text()).arg(idx);

  // check if it is not already present
  for (int i=0; i<fViewX->count(); i++) {
    if (fViewX->item(i)->text() == xLabel) {
      fViewX->removeItemWidget(item);
      delete item;
      return;
    }
  }

  fViewX->setCurrentItem(item);
  fViewX->currentItem()->setText(xLabel);

  // add the item to the XY list
  PmuppXY xyItem;
  xyItem.setCollectionTag(idx);
  xyItem.setXlabel(xLabel);

  fXY.push_back(xyItem);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::dropOnViewY
 * @param item
 */
void PmuppGui::dropOnViewY(QListWidgetItem *item)
{
  if (item == 0)
    return;

  if (item->text().contains("(-"))
    return;

  // make sure that any x-parameter selection is already present
  if (fViewX->count() == 0) {
    fViewY->removeItemWidget(item);
    delete item;
    QMessageBox::warning(this, "**WARNING**", "It is compulsory to have at least one x-parameter set\nbefore trying to add a y-parameter.");
    return;
  }

  int idx = fColList->currentRow();
  QString yLabel = QString("%1 (-%2-)").arg(item->text()).arg(idx);

  // check if it is not already present
  for (int i=0; i<fViewY->count(); i++) {
    if (fViewY->item(i)->text() == yLabel) {
      fViewY->removeItemWidget(item);
      delete item;
      return;
    }
  }

  fViewY->setCurrentItem(item);
  fViewY->currentItem()->setText(yLabel);

  // add the item to the XY list
  idx=getXlabelIndex(fViewX->currentItem()->text());
  if (idx == -1)
    return;

  fXY[idx].addYlabel(yLabel);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::refreshY
 */
void PmuppGui::refreshY()
{
  // clear Y view
  fViewY->clear();

  // get proper X view item
  QListWidgetItem *item = fViewX->currentItem();
  if (item == 0)
    return;

  QString xLabel = item->text();
  int idx = getXlabelIndex(xLabel);
  if (idx == -1)
    return;

  for (int i=0; i<fXY[idx].getYlabelSize(); i++)
    fViewY->addItem(fXY[idx].getYlabel(i));
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::createMacro
 */
void PmuppGui::createMacro()
{
  QVector<PmuppMarker> marker = fAdmin->getMarkers();
  QVector<PmuppColor> color = fAdmin->getColors();

  if (fXY.size() == 0) {
    QMessageBox::information(this, "createMacro", "No parameter list present.");
    return;
  }

  if (fMacroName.isEmpty()) {
    fMacroName = "__mupp.C";
  }
  if (!fMacroPath.isEmpty()) {
    if (fMacroPath.endsWith("/"))
      fMacroName.prepend(fMacroPath);
    else
      fMacroName.prepend(fMacroPath + "/");
  }

  QFile file(fMacroName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "**ERROR**", "Couldn't open macro file for writting.");
    return;
  }

  QTextStream fout(&file);

  fout << "// " << fMacroName.toLatin1().data() << endl;
  fout << "// " << QDateTime::currentDateTime().toString("yy/MM/dd - HH:mm:ss") << endl;
  fout << "// " << endl;
  fout << "{" << endl;
  fout << "  gROOT->Reset();" << endl;
  fout << endl;
  fout << "  gStyle->SetOptTitle(0);" << endl;
  fout << "  gStyle->SetOptDate(0);" << endl;
  fout << "  gStyle->SetPadColor(TColor::GetColor(255,255,255));    // pad bkg to white" << endl;
  fout << "  gStyle->SetCanvasColor(TColor::GetColor(255,255,255)); // canvas bkg to white" << endl;
  fout << endl;
  fout << "  Int_t nn=0, i=0;" << endl;
  fout << "  Double_t null[512];" << endl;
  fout << "  Double_t xx[512];" << endl;
  fout << "  Double_t yy[512];" << endl;
  fout << "  Double_t yyPosErr[512];" << endl;
  fout << "  Double_t yyNegErr[512];" << endl;
  // create all the necessary TGraph's
  int collTag = -1, pos;
  QString collName("");
  QString xLabel(""), yLabel("");
  char gLabel[128];
  QVector<double> xx, yy, yyPosErr, yyNegErr;
  double xMin=1.0e10, xMax=-1.0e10, yMin=1.0e10, yMax=-1.0e10;
  for (int i=0; i<fXY.size(); i++) {
    collTag = fXY[i].getCollectionTag();
    collName = fColList->item(collTag)->text();
    // x-label
    xLabel = fXY[i].getXlabel();
    pos = xLabel.indexOf(" (-");
    xLabel.remove(pos, xLabel.length()-pos);
    // get x-vector
    xx = fParamDataHandler->GetValues(collName, xLabel);
    getMinMax(xx, xMin, xMax);
    // a couple of x-vector specifics
    if ((xLabel == "dataT") || (xLabel == "dataE"))
      xMin = 0.0;
    // get y-, yPosErr-, and yNegErr-vector
    for (int j=0; j<fXY[i].getYlabelSize(); j++) {
      // y-label
      yLabel = fXY[i].getYlabel(j);
      pos = yLabel.indexOf(" (-");
      yLabel.remove(pos, yLabel.length()-pos);
      // get y-vector
      yy = fParamDataHandler->GetValues(collName, yLabel);
      yyPosErr = fParamDataHandler->GetPosErr(collName, yLabel);
      yyNegErr = fParamDataHandler->GetNegErr(collName, yLabel);
      yLabel = substituteDefaultLabels(yLabel);
      getMinMax(yy, yMin, yMax);
      // create TGraph objects
      fout << endl;
      snprintf(gLabel, sizeof(gLabel), "g_%d_%d", i, j);
      fout << "  nn = " << xx.size() << ";" << endl;
      fout << endl;
      fout << "  // null value vector" << endl;
      for (int k=0; k<xx.size(); k++) {
        fout << "  null[" << k << "]=0.0;" << endl;
      }
      fout << "  // xx" << endl;
      for (int k=0; k<xx.size(); k++) {
        fout << "  xx[" << k << "]=" << xx[k] << ";" << endl;
      }
      fout << "  // yy" << endl;
      for (int k=0; k<xx.size(); k++) {
        fout << "  yy[" << k << "]=" << yy[k] << ";" << endl;
      }
      fout << "  // yyPosErr" << endl;
      for (int k=0; k<xx.size(); k++) {
        fout << "  yyPosErr[" << k << "]=" << yyPosErr[k] << ";" << endl;
      }
      fout << "  // yyNegErr" << endl;
      for (int k=0; k<xx.size(); k++) {
        fout << "  yyNegErr[" << k << "]=" << fabs(yyNegErr[k]) << ";" << endl;
      }
      fout << endl;
      fout << "  TGraphAsymmErrors *" << gLabel << " = new TGraphAsymmErrors(nn, xx, yy, null, null, yyNegErr, yyPosErr);" << endl;
    }
  }
  fout << endl;
  fout << "  //***************" << endl;
  fout << "  // plotting " << endl;
  fout << "  //***************" << endl;
  fout << "  TCanvas *c1 = new TCanvas(\"c1\", \"" << fMacroName.toLatin1().data() << "\", 10, 10, 600, 700);" << endl;
  fout << endl;
  int idx, r, g, b;
  PmuppMarker markerObj;
  for (int i=0; i<fXY.size(); i++) {
    xLabel = fXY[i].getXlabel();
    pos = xLabel.indexOf(" (-");
    xLabel.remove(pos, xLabel.length()-pos);
    xLabel = substituteDefaultLabels(xLabel);
    for (int j=0; j<fXY[i].getYlabelSize(); j++) {
      idx = i*fXY[i].getYlabelSize()+j; // graph number idx
      markerObj = marker[idx];
      yLabel = fXY[i].getYlabel(j);
      pos = yLabel.indexOf(" (-");
      yLabel.remove(pos, yLabel.length()-pos);
      yLabel = substituteDefaultLabels(yLabel);
      snprintf(gLabel, sizeof(gLabel), "g_%d_%d", i, j);
      if ((i==0) && (j==0)) { // first graph
        if (idx < marker.size()) {
          fout << "  " << gLabel << "->SetMarkerStyle(" << markerObj.getMarker() << ");" << endl;
          fout << "  " << gLabel << "->SetMarkerSize(" << markerObj.getMarkerSize() << ");" << endl;
        } else {
          fout << "  " << gLabel << "->SetMarkerStyle(20); // bullet" << endl;
          fout << "  " << gLabel << "->SetMarkerSize(1.5);" << endl;
        }
        if (idx < color.size()) {
          color[idx].getRGB(r, g, b);
          fout << "  " << gLabel << "->SetMarkerColor(TColor::GetColor(" << r << "," << g << "," << b << "));" << endl;
          fout << "  " << gLabel << "->SetLineColor(TColor::GetColor(" << r << "," << g << "," << b << "));" << endl;
        } else {
          fout << "  " << gLabel << "->SetMarkerColor(kBlue);" << endl;
          fout << "  " << gLabel << "->SetLineColor(kBlue);" << endl;
        }
        fout << "  " << gLabel << "->SetFillColor(kWhite);" << endl;
        fout << "  " << gLabel << "->GetXaxis()->SetTitle(\"" << xLabel.toLatin1().data() << "\");" << endl;
        fout << "  " << gLabel << "->GetXaxis()->SetTitleSize(0.05);" << endl;
        fout << "  " << gLabel << "->GetXaxis()->SetRangeUser(" << 0.95*xMin << ", " << 1.05*xMax << ");" << endl;
        fout << "  " << gLabel << "->GetYaxis()->SetTitle(\"" << yLabel.toLatin1().data() << "\");" << endl;
        fout << "  " << gLabel << "->GetYaxis()->SetTitleSize(0.05);" << endl;
        fout << "  " << gLabel << "->GetYaxis()->SetTitleOffset(1.30);" << endl;
        fout << "  " << gLabel << "->GetYaxis()->SetRangeUser(" << 0.95*yMin << ", " << 1.05*yMax << ");" << endl;
        fout << "  " << gLabel << "->GetXaxis()->SetDecimals(kTRUE);" << endl;
        fout << "  " << gLabel << "->Draw(\"AP\");" << endl;
      } else { // consecutive graphs
        if (idx < marker.size()) {
          fout << "  " << gLabel << "->SetMarkerStyle(" << markerObj.getMarker() << ");" << endl;
          fout << "  " << gLabel << "->SetMarkerSize(" << markerObj.getMarkerSize() << ");" << endl;
        } else {
          fout << "  " << gLabel << "->SetMarkerStyle(" << 21+j << ");" << endl;
          fout << "  " << gLabel << "->SetMarkerSize(1.5);" << endl;
        }
        if (idx < color.size()) {
          color[idx].getRGB(r, g, b);
          fout << "  " << gLabel << "->SetMarkerColor(TColor::GetColor(" << r << "," << g << "," << b << "));" << endl;
          fout << "  " << gLabel << "->SetLineColor(TColor::GetColor(" << r << "," << g << "," << b << "));" << endl;
        } else {
          fout << "  " << gLabel << "->SetMarkerColor(kBlue);" << endl;
          fout << "  " << gLabel << "->SetLineColor(kBlue);" << endl;
        }
        fout << "  " << gLabel << "->SetFillColor(kWhite);" << endl;
        fout << "  " << gLabel << "->Draw(\"Psame\");" << endl;
      }
    }
  }
  fout << "}" << endl;

  // clear macro name
  fMacroName = QString("");
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::plot
 */
void PmuppGui::plot()
{
  // check if there is something to be plotted
  if (fXY.size() == 0) {
    QMessageBox::information(this, "plot", "No parameter list present.");
    return;
  }

  // write data file with path name: $HOME/.musrfit/mupp/_mupp_<startUpTime>.dat
  // where <startUpTime is the time when mupp has been started.
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  QString pathName = QString("%1/.musrfit/mupp/_mupp_%2.dat").arg(env.value("HOME")).arg(fDatime);

  int collTag = -1, pos;
  QString collName("");
  QString xLabel(""), yLabel("");
  QVector<double> xx, yy, yyPosErr, yyNegErr;
  QVector< QVector<double> > yyy, yyyPosErr, yyyNegErr;

  QFile file(pathName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "ERROR", "Couldn't write necessary temporary file!");
    return;
  }
  QTextStream fout(&file);

  fout << "% path file name: " << pathName << endl;
  fout << "% creation time : " << QDateTime::currentDateTime().toString("yyyy.MM.dd - hh:mm:ss") << endl;
  fout << "%" << endl;
  for (int i=0; i<fXY.size(); i++) {
    // header info
    collTag = fXY[i].getCollectionTag();
    collName = fColList->item(collTag)->text();

    fout << "% ---------" << endl;
    fout << "% collName = " << collName << endl;
    fout << "% start ---" << endl;

    // x-label
    xLabel = fXY[i].getXlabel();
    pos = xLabel.indexOf(" (-");
    xLabel.remove(pos, xLabel.length()-pos);
    // get x-vector
    xx = fParamDataHandler->GetValues(collName, xLabel);

    xLabel = substituteDefaultLabels(xLabel);
    fout << "xLabel: " << xLabel << ", ";
    for (int j=0; j<fXY[i].getYlabelSize()-1; j++) {
      // get y-label
      yLabel = fXY[i].getYlabel(j);
      pos = yLabel.indexOf(" (-");
      yLabel.remove(pos, yLabel.length()-pos);
      // get y-vector and errors
      yy = fParamDataHandler->GetValues(collName, yLabel);
      yyy.push_back(yy);
      yyPosErr = fParamDataHandler->GetPosErr(collName, yLabel);
      yyyPosErr.push_back(yyPosErr);
      yyNegErr = fParamDataHandler->GetNegErr(collName, yLabel);
      yyyNegErr.push_back(yyNegErr);
      yLabel = substituteDefaultLabels(yLabel);
      fout << "yLabel: " << yLabel << ", ";
    }
    // get last y-label
    int idx=fXY[i].getYlabelSize()-1;
    yLabel = fXY[i].getYlabel(idx);
    pos = yLabel.indexOf(" (-");
    yLabel.remove(pos, yLabel.length()-pos);
    // get y-vector and errors
    yy = fParamDataHandler->GetValues(collName, yLabel);
    yyy.push_back(yy);
    yyPosErr = fParamDataHandler->GetPosErr(collName, yLabel);
    yyyPosErr.push_back(yyPosErr);
    yyNegErr = fParamDataHandler->GetNegErr(collName, yLabel);
    yyyNegErr.push_back(yyNegErr);

    yLabel = substituteDefaultLabels(yLabel);
    fout << "yLabel: " << yLabel << endl;

    // data
    for (int j=0; j<xx.size(); j++) {
      fout << xx[j] << ", ";
      for (int k=0; k<yyy.size()-1; k++) {
        fout << yyy[k][j] << ", " << yyyPosErr[k][j] << ", " << yyyNegErr[k][j] << ", ";
      }
      idx = yyy.size()-1;
      fout << yyy[idx][j] << ", " << yyyPosErr[idx][j] << ", " << yyyNegErr[idx][j];
      fout << endl;
    }

    fout << "% end -----" << endl;

    // clear collection related vectors
    yyy.clear();
    yyyPosErr.clear();
    yyyNegErr.clear();
  }

  file.close();

  // issue a system message to inform to ROOT parameter plotter (rpp) that new data are available
  key_t key;
  struct mbuf msg;
  int flags, msqid;

  // generate the ICP message queue key
  QString tmpPathName = QString("%1/.musrfit/mupp/_mupp_ftok.dat").arg(env.value("HOME"));
  file.setFileName(tmpPathName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::critical(this, "ERROR", "Couldn't write necessary temporary file!");
    return;
  }
  fout.setDevice(&file);
  fout << QCoreApplication::applicationFilePath().toLatin1().data() << endl;
  file.close();

  key = ftok(QCoreApplication::applicationFilePath().toLatin1().data(), 1);
  if (key == -1) {
    QMessageBox::critical(this, "ERROR", "Couldn't obtain necessary key to install the IPC message queue.");
    return;
  }

  // set the necessary IPC message queue flags
  flags = IPC_CREAT;

  // open the message queue
  msqid = msgget(key, flags | S_IRUSR | S_IWUSR);
  if (msqid == -1) {
    QMessageBox::critical(this, "ERROR", "Couldn't open the IPC message queue.");
    return;
  }

  // send message
  flags = IPC_NOWAIT;
  msg.mtype = 1;
  strncpy(msg.mtext, pathName.toLatin1().data(), PMUPP_MAX_MTEXT);
  if (msgsnd(msqid, &msg, strlen(msg.mtext)+1, flags) == -1) {
    QMessageBox::critical(this, "ERROR", "Couldn't send the IPC message.");
    return;
  }

  // start native ROOT parameter plot application if not already running
  if (fMuppPlot == 0) { // not running yet
    startMuppPlot();
  } else {
    // check if mupp_plot is still running
    if (fMuppPlot->state() == QProcess::NotRunning) { // not running hence start it
      startMuppPlot();
    }
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::startMuppPlot
 */
void PmuppGui::startMuppPlot()
{
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  QString cmd = QString("%1/bin/mupp_plot").arg(MUPP_PREFIX);
#if defined(Q_OS_DARWIN) || defined(Q_OS_MAC)
  cmd = QString("/Applications/mupp.app/Contents/MacOS/mupp_plot"); // as35 not yet ready
#endif
  QString workDir = QString("./");
  QStringList arg;

  fMuppPlot = new QProcess(this);
  if (fMuppPlot == nullptr) {
    QMessageBox::critical(0, "**ERROR**", "Couldn't invoke QProcess for mupp_plot!");
    return;
  }

  // make sure that the system environment variables are properly set
  env.insert("LD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("LD_LIBRARY_PATH"));
  fMuppPlot->setProcessEnvironment(env);
  fMuppPlot->setWorkingDirectory(workDir);
  fMuppPlot->start(cmd, arg);
  if (!fMuppPlot->waitForStarted()) {
    // error handling
    QString msg(tr("Could not execute the output command: ")+cmd);
    QMessageBox::critical( 0,
                          tr("Fatal error"),
                          msg,
                          tr("Quit") );
    return;
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::handleCmds
 */
void PmuppGui::handleCmds()
{
  QString cmd = fCmdLine->text();

  fCmdLine->setText("$ ");

  cmd = cmd.trimmed();
  cmd = cmd.remove("$ ");
  cmd = cmd.trimmed();

  if (!cmd.compare("exit", Qt::CaseInsensitive) || !cmd.compare("quit", Qt::CaseInsensitive)) {
    aboutToQuit();
  } else if (!cmd.compare("load", Qt::CaseInsensitive) || !cmd.compare("open", Qt::CaseInsensitive)) {
    fileOpen();
  } else if (!cmd.compare("dump collections", Qt::CaseInsensitive)) {
    toolDumpCollections();
  } else if (!cmd.compare("dump XY", Qt::CaseInsensitive)) {
    toolDumpXY();
  } else if (!cmd.compare("refresh", Qt::CaseInsensitive)) {
    refresh();
  } else if (!cmd.compare("plot", Qt::CaseInsensitive)) {
    plot();
  } else if (cmd.startsWith("macro")) { // cmd: macro <fln>
    QStringList tok = cmd.split(" ", QString::SkipEmptyParts);
    if (tok.size() != 2) {
      QMessageBox::critical(this, "ERROR", QString("wrong macro cmd: %1.\nPlease check the help.").arg(cmd));
      return;
    }
    fMacroName = tok[1];
    createMacro();
  } else if (cmd.startsWith("path")) { // cmd: path <macro_path>
    QMessageBox::information(0, "INFO", "set's eventually the path for the macros to be saved.");
    // will set the path to where to save the macro
    QStringList tok = cmd.split(" ", QString::SkipEmptyParts);
    if (tok.size() != 2) {
      QMessageBox::critical(this, "ERROR", QString("wrong path cmd: %1.\nPlease check the help.").arg(cmd));
      return;
    }
    // replace "~" -> "$HOME"
    fMacroPath = tok[1].replace("~", "$HOME");
    // if environment variable is found -> expand it
    QString envVar=fMacroPath;
    if (envVar.startsWith("$")) {
      envVar.remove("$");
      int pos = envVar.indexOf("/");
      if (pos > 0)
        envVar.remove(pos, fMacroPath.length()-pos);
      QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
      QString fullPath = env.value(envVar); // get environment variable value if present, otherwise "" is returned
      if (fullPath.isEmpty()) {
        fMacroPath = "";
        QMessageBox::critical(0, "ERROR", QString("Environment variable '%1' not present. Typo?!").arg(envVar.prepend("$")));
        return;
      }
      fMacroPath.replace(envVar.prepend("$"), fullPath);
    }
  } else if (cmd.startsWith("select collection") || cmd.startsWith("sc")) {
    selectCollection(cmd);
  } else if (cmd.startsWith("flush")) {
    fCmdHistory.clear();
    fCmdLine->setText("$ ");
  } else if (cmd.startsWith("help")) {
    helpCmds();
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::handleNewData
 */
void PmuppGui::handleNewData()
{
  // update collection list view
  updateCollectionList();
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::updateCollectionList
 */
void PmuppGui::updateCollectionList()
{
  // check if collection is already present
  bool valid=false;
  PmuppCollection coll;
  for (int i=0; i<fParamDataHandler->GetNoOfCollections(); i++) {
    coll = fParamDataHandler->GetCollection(i, valid);
    if (isNewCollection(coll)) {
      fColList->addItem(coll.GetName());
    }
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::updateParamList
 * @param currentRow
 */
void PmuppGui::updateParamList(int currentRow)
{
  QListWidgetItem *item = fColList->item(currentRow);
  if (item == 0) {
    fParamList->clear();
    return;
  }

  // update parameter list widget
  bool valid = false;
  PmuppCollection coll = fParamDataHandler->GetCollection(item->text(), valid);
  if (!valid) {
    return;
  }

  PmuppRun run = coll.GetRun(0);

  fParamList->clear();
  for (int i=0; i<run.GetNoOfParam(); i++) {
    fParamList->insertItem(i, run.GetParam(i).GetName());
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::updateXYList
 * @param idx collection index which has been removed
 */
void PmuppGui::updateXYList(int idx)
{
  // 1st: remove XY with collection index idx from the list
  for (int i=0; i<fXY.size(); i++) {
    // since each collection is only once present, the following is good enough
    if (fXY[i].getCollectionTag() == idx) {
      fXY.remove(i);
      break;
    }
  }

  // 2nd: re-number the XY list according to the new collections
  QString str("");
  int pos=-1;
  int oldIdx=-1;
  for (int i=0; i<fXY.size(); i++) {
    oldIdx = fXY[i].getCollectionTag();
    if (oldIdx > idx) {
      // correct collection tag
      fXY[i].setCollectionTag(oldIdx-1);
      // correct X label
      str = fXY[i].getXlabel();
      pos = str.indexOf("(");
      str.remove(pos, str.length()-pos);
      str += QString("(-%1-)").arg(oldIdx-1);
      fXY[i].setXlabel(str);
      // correct Y label
      for (int j=0; j<fXY[i].getYlabelSize(); j++) {
        str = fXY[i].getYlabel(j);
        pos = str.indexOf("(");
        str.remove(pos, str.length()-pos);
        str += QString("(-%1-)").arg(oldIdx-1);
        fXY[i].setYlabel(j, str);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::updateXYListGui
 */
void PmuppGui::updateXYListGui()
{
  fViewX->clear();

  for (int i=0; i<fXY.size(); i++) {
    fViewX->addItem(fXY[i].getXlabel());
  }

  fViewX->setCurrentRow(0);
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::isNewCollection
 * @param coll
 * @return
 */
bool PmuppGui::isNewCollection(PmuppCollection &coll)
{
  for (int i=0; i<fColList->count(); i++)
    if (fColList->item(i)->text() == coll.GetName())
      return false;
  return true;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::editCollName
 * @param item
 */
void PmuppGui::editCollName(QListWidgetItem *item)
{
  bool ok;
  QString oldName = item->text();
  QString newName = QInputDialog::getText(this, "Change Collection Name", "Collection Name",
                                          QLineEdit::Normal, oldName, &ok);
  if (ok && !newName.isEmpty()) {
    item->setText(newName);
    fParamDataHandler->GetCollection(oldName)->SetName(newName);
    fParamDataHandler->Dump();
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::getMinMax
 * @param data
 * @param min
 * @param max
 */
void PmuppGui::getMinMax(QVector<double> &data, double &min, double &max)
{
  for (int i=0; i<data.size(); i++) {
    if (data[i] < min)
      min = data[i];
    if (data[i] > max)
      max = data[i];
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::substituteDefaultLabels
 * @param label
 * @return
 */
QString PmuppGui::substituteDefaultLabels(QString label)
{
  if (label == "dataT")
    return QString("T (K)");
  else if (label == "dataB")
    return QString("B (G)");
  else if (label == "dataE")
    return QString("E (keV)");
  else if (!label.compare("sigma", Qt::CaseInsensitive))
    return QString("#sigma (1/#mus)");
  else if (!label.compare("lambda", Qt::CaseInsensitive))
    return QString("#lambda (1/#mus)");
  else if (!label.compare("alpha_LR", Qt::CaseInsensitive))
    return QString("#alpha_{LR}");
  else if (!label.compare("alpha_TB", Qt::CaseInsensitive))
    return QString("#alpha_{TB}");
  else
    return label;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PmuppGui::selectCollection
 * @param cmd
 */
void PmuppGui::selectCollection(QString cmd)
{
  bool ok;
  int ival, pos=-1;
  QString str = cmd;
  if (cmd.contains("select collection")) {
    str.remove("select collection");
    str = str.trimmed();
  } else if (cmd.contains("sc")) {
    pos = cmd.indexOf("sc");
    str.remove(0, pos+2);
    str = str.trimmed();
  } else {
    return;
  }

  ival = str.toInt(&ok);
  if (ok) { // it is a number
    if (ival < fColList->count())
      fColList->setCurrentRow(ival);
  } else { // assume it is a collection name
    for (int i=0; i<fColList->count(); i++) {
      if (fColList->item(i)->text() == str) {
        fColList->setCurrentRow(i);
        break;
      }
    }
  }
}