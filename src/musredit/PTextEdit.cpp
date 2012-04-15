/****************************************************************************

  PTextEdit.cpp

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

#include <QTextEdit>
#include <QStatusBar>
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QTabWidget>
#include <QApplication>
#include <QFontDatabase>
#include <QComboBox>
#include <QLineEdit>
#include <QFileInfo>
#include <QFile>
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QColorDialog>
#include <QPainter>
#include <QMessageBox>
#include <QDialog>
#include <QPixmap>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTextList>

#include <QtDebug>

#include "PTextEdit.h"
#include "PHelp.h"
#include "PSubTextEdit.h"
#include "PAdmin.h"
#include "PFindDialog.h"
#include "PReplaceDialog.h"
#include "PReplaceConfirmationDialog.h"
#include "PFitOutputHandler.h"
#include "PPrefsDialog.h"
#include "PGetDefaultDialog.h"
#include "PMusrEditAbout.h"
#include "PMsr2DataDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>Constructor
 *
 * \param parent pointer to the parent object
 * \param f qt windows flags
 */
PTextEdit::PTextEdit( QWidget *parent, Qt::WindowFlags f )
    : QMainWindow( parent, f )
{
  // reads and manages the conents of the xml-startup (musredit_startup.xml) file
  fAdmin = new PAdmin();

  // enable file system watcher. Needed to get notification if the msr-file is changed outside of musrfit at runtime
  fFileSystemWatcherActive = true;
  fFileSystemWatcher = new QFileSystemWatcher();
  if (fFileSystemWatcher == 0) {
    QMessageBox::information(this, "**ERROR**", "Couldn't invoke QFileSystemWatcher!");
  } else {
    connect( fFileSystemWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
  }

  // initialize stuff
  fMusrT0Action = 0;

  fMsr2DataParam = 0;
  fFindReplaceData = 0,

  fKeepMinuit2Output = false;
  fTitleFromDataFile = fAdmin->getTitleFromDataFileFlag();
  fEnableMusrT0      = fAdmin->getEnableMusrT0Flag();
  fDump = 0; // 0 = no dump, 1 = ascii dump, 2 = root dump

  // setup menus
  setupFileActions();
  setupEditActions();
  setupTextActions();
  setupMusrActions();
  setupHelpActions();

  fTabWidget = new QTabWidget( this );
  fTabWidget->setMovable(true); // allows to shuffle around tabs
  setCentralWidget( fTabWidget );

  textFamily(fAdmin->getFontName());
  textSize(QString("%1").arg(fAdmin->getFontSize()));
  fFontChanging = false;

  QPixmap image0(":/images/musrfit.xpm");
  setWindowIcon( image0 );

  // if arguments are give, try to load those files, otherwise create an empty new file
  if ( qApp->argc() != 1 ) {
    for ( int i = 1; i < qApp->argc(); ++i )
      load( qApp->argv()[ i ] );
  } else {
    fileNew();
  }

  connect( fTabWidget, SIGNAL( currentChanged(QWidget*) ), this, SLOT( applyFontSettings(QWidget*) ));

  fLastDirInUse = fAdmin->getDefaultSavePath();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Destructor
 */
PTextEdit::~PTextEdit()
{
  if (fAdmin) {
    delete fAdmin;
    fAdmin = 0;
  }
  if (fMusrT0Action) {
    delete fMusrT0Action;
    fMusrT0Action = 0;
  }
  if (fMsr2DataParam) {
    delete fMsr2DataParam;
    fMsr2DataParam = 0;
  }
  if (fFindReplaceData) {
    delete fFindReplaceData;
    fFindReplaceData = 0;
  }
/*
  if (fFileWatcher) {
    delete fFileWatcher;
    fFileWatcher = 0;
  }
*/
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Setup the file menu and the necessary actions.
 */
void PTextEdit::setupFileActions()
{
  QToolBar *tb = new QToolBar( this );
  tb->setWindowTitle( "File Actions" );
  addToolBar( tb );

  QMenu *menu = new QMenu( tr( "F&ile" ), this );
  menuBar()->addMenu( menu );

  QAction *a;

  a = new QAction( QIcon( QPixmap(":/images/filenew.xpm") ), tr( "&New..." ), this );
  a->setShortcut( tr("Ctrl+N") );
  a->setStatusTip( tr("Create a new msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileNew() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( QIcon( QPixmap(":/images/fileopen.xpm" ) ), tr( "&Open..." ), this );
  a->setShortcut( tr("Ctrl+O") );
  a->setStatusTip( tr("Opens a msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileOpen() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( QIcon( QPixmap(":/images/filereload.xpm") ), tr( "Reload..." ), this );
  a->setShortcut( tr("F5") );
  a->setStatusTip( tr("Reload msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileReload() ) );
  tb->addAction(a);
  menu->addAction(a);

  menu->addSeparator();

  a = new QAction( QIcon( QPixmap(":/images/filesave.xpm") ), tr( "&Save..." ), this );
  a->setShortcut( tr("Ctrl+S") );
  a->setStatusTip( tr("Save msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileSave() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( tr( "Save &As..." ), this );
  a->setStatusTip( tr("Save msr-file As") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileSaveAs() ) );
  menu->addAction(a);

  menu->addSeparator();

  a = new QAction( QIcon( QPixmap(":/images/fileprint.xpm") ), tr( "&Print..." ), this );
  a->setShortcut( tr("Ctrl+P") );
  a->setStatusTip( tr("Print msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( filePrint() ) );
  tb->addAction(a);
  menu->addAction(a);

  menu->addSeparator();

  a = new QAction( tr( "&Close" ), this );
  a->setShortcut( tr("Ctrl+W") );
  a->setStatusTip( tr("Close msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileClose() ) );
  menu->addAction(a);

  a = new QAction( tr( "Close &All" ), this );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileCloseAll() ) );
  menu->addAction(a);

  a = new QAction( tr( "Clo&se All Others" ), this );
  a->setShortcut( tr("Ctrl+Shift+W") );
  a->setStatusTip( tr("Close All Other Tabs") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileCloseAllOthers() ) );
  menu->addAction(a);

  menu->addSeparator();

  a = new QAction( tr( "E&xit" ), this );
  a->setShortcut( tr("Ctrl+Q") );
  a->setStatusTip( tr("Exit Program") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileExit() ) );
  menu->addAction(a);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Setup the edit menu and the necessary actions.
 */
void PTextEdit::setupEditActions()
{
  QToolBar *tb = new QToolBar( this );
  tb->setWindowTitle( "Edit Actions" );
  addToolBar( tb );

  QMenu *menu = new QMenu( tr( "&Edit" ), this );
  menuBar()->addMenu( menu );

  QAction *a;

  a = new QAction( QIcon( QPixmap( ":/images/editundo.xpm" ) ), tr( "&Undo" ), this );
  a->setShortcut( tr("Ctrl+Z") );
  a->setStatusTip( tr("Edit Undo") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editUndo() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( QIcon( QPixmap( ":/images/editredo.xpm" ) ), tr( "&Redo" ), this );
  a->setShortcut( tr("Ctrl+Y") );
  a->setStatusTip( tr("Edit Redo") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editRedo() ) );
  tb->addAction(a);
  menu->addAction(a);
  menu->addSeparator();

  a = new QAction( tr( "Select &All" ), this );
  a->setShortcut( tr("Ctrl+A") );
  a->setStatusTip( tr("Edit Select All") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editSelectAll() ) );
  menu->addAction(a);

  menu->addSeparator();
  tb->addSeparator();

  a = new QAction( QIcon( QPixmap( ":/images/editcopy.xpm" ) ), tr( "&Copy" ), this );
  a->setShortcut( tr("Ctrl+C") );
  a->setStatusTip( tr("Edit Copy") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editCopy() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( QIcon( QPixmap( ":/images/editcut.xpm" ) ), tr( "Cu&t" ), this );
  a->setShortcut( tr("Ctrl+X") );
  a->setStatusTip( tr("Edit Cut") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editCut() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( QIcon( QPixmap( ":/images/editpaste.xpm" ) ), tr( "&Paste" ), this );
  a->setShortcut( tr("Ctrl+V") );
  a->setStatusTip( tr("Edit Paste") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editPaste() ) );
  tb->addAction(a);
  menu->addAction(a);

  menu->addSeparator();
  tb->addSeparator();

  a = new QAction( QIcon( QPixmap( ":/images/editfind.xpm" ) ), tr( "&Find" ), this );
  a->setShortcut( tr("Ctrl+F") );
  a->setStatusTip( tr("Edit Find") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editFind() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( QIcon( QPixmap( ":/images/editnext.xpm" ) ), tr( "Find &Next" ), this );
  a->setShortcut( tr("F3") );
  a->setStatusTip( tr("Edit Find Next") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editFindNext() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( QIcon( QPixmap( ":/images/editprevious.xpm" ) ) , tr( "Find Pre&vious" ), this );
  a->setShortcut( tr("Shift+F4") );
  a->setStatusTip( tr("Edit Find Previous") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editFindPrevious() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( tr( "Replace..." ), this );
  a->setShortcut( tr("Ctrl+R") );
  a->setStatusTip( tr("Edit Replace") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editFindAndReplace() ) );
  menu->addAction(a);
  menu->addSeparator();

  QMenu *addSubMenu = new QMenu( tr ("Add Block"), this);

  a = new QAction( tr("Title Block"), this );
  a->setStatusTip( tr("Invokes MSR Title Block Dialog") );
  connect( a, SIGNAL( triggered() ), this, SLOT( insertTitle() ));
  addSubMenu->addAction(a);

  a = new QAction( tr("Parameter Block"), this );
  a->setStatusTip( tr("Invokes MSR Parameter Block Dialog") );
  connect( a, SIGNAL( triggered() ), this, SLOT( insertParameterBlock() ));
  addSubMenu->addAction(a);

  a = new QAction( tr("Theory Block"), this );
  a->setStatusTip( tr("Invokes MSR Theory Block Dialog") );
  connect( a, SIGNAL( triggered() ), this, SLOT( insertTheoryBlock() ));
  addSubMenu->addAction(a);

  a = new QAction( tr("Function Block"), this );
  a->setStatusTip( tr("Invokes MSR Function Block Dialog") );
  connect( a, SIGNAL( triggered() ), this, SLOT( insertFunctionBlock() ));
  addSubMenu->addAction(a);

  // feed the theoryFunctions popup menu
  QMenu *theoryFunctions = new QMenu( tr("Add Theory Function"), this );  
  for (unsigned int i=0; i<fAdmin->getTheoryCounts(); i++) {
    PTheory *theoryItem = fAdmin->getTheoryItem(i);
    a = new QAction( theoryItem->label, this);
    theoryFunctions->addAction(a);
  }
  connect( theoryFunctions, SIGNAL( triggered(QAction*)), this, SLOT( insertTheoryFunction(QAction*) ) );

  a = new QAction( tr("Asymmetry Run Block"), this );
  a->setStatusTip( tr("Invokes MSR Asymmetry Run Block Dialog") );
  connect( a, SIGNAL( triggered() ), this, SLOT( insertAsymRunBlock() ));
  addSubMenu->addAction(a);

  a = new QAction( tr("Single Histo Run Block"), this );
  a->setStatusTip( tr("Invokes MSR Single Histo Run Block Dialog") );
  connect( a, SIGNAL( triggered() ), this, SLOT( insertSingleHistRunBlock() ));
  addSubMenu->addAction(a);

  a = new QAction( tr("NonMuSR Run Block"), this );
  a->setStatusTip( tr("Invokes MSR NonMuSR Run Block Dialog") );
  connect( a, SIGNAL( triggered() ), this, SLOT( insertNonMusrRunBlock() ));
  addSubMenu->addAction(a);

  a = new QAction( tr("Command Block"), this );
  a->setStatusTip( tr("Invokes MSR Command Block Dialog") );
  connect( a, SIGNAL( triggered() ), this, SLOT( insertCommandBlock() ));
  addSubMenu->addAction(a);

  a = new QAction( tr("Fourier Block"), this );
  a->setStatusTip( tr("Invokes MSR Fourier Block Dialog") );
  connect( a, SIGNAL( triggered() ), this, SLOT( insertFourierBlock() ));
  addSubMenu->addAction(a);

  a = new QAction( tr("Plot Block"), this );
  a->setStatusTip( tr("Invokes MSR Plot Block Dialog") );
  connect( a, SIGNAL( triggered() ), this, SLOT( insertPlotBlock() ));
  addSubMenu->addAction(a);

  a = new QAction( tr("Statistic Block"), this );
  a->setStatusTip( tr("Invokes MSR Statistic Block Dialog") );
  connect( a, SIGNAL( triggered() ), this, SLOT( insertStatisticBlock() ));
  addSubMenu->addAction(a);

  menu->addMenu(addSubMenu);
  menu->addMenu(theoryFunctions);
  menu->addSeparator();

  a = new QAction( tr( "Co&mment" ), this );
  a->setShortcut( tr("Ctrl+M") );
  a->setStatusTip( tr("Edit Comment Selected Lines") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editComment() ) );
  menu->addAction(a);

  a = new QAction( tr( "Unco&mment" ), this );
  a->setShortcut( tr("Ctrl+Shift+M") );
  a->setStatusTip( tr("Edit Uncomment Selected Lines") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editUncomment() ) );
  menu->addAction(a);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Setup the font/font size menu.
 */
void PTextEdit::setupTextActions()
{
  QToolBar *tb = new QToolBar( this );
  tb->setWindowTitle( "Format Actions" );
  addToolBar( tb );

  fComboFont = new QComboBox();
  fComboFont->setEditable(true);
  QFontDatabase db;
  fComboFont->addItems( db.families() );
  connect( fComboFont, SIGNAL( activated( const QString & ) ),
           this, SLOT( textFamily( const QString & ) ) );
  QLineEdit *edit = fComboFont->lineEdit();
  if (edit == 0) {
//    qDebug() << endl << "**ERROR** PTextEdit::setupTextActions(): cannot edit fComboFont" << endl;
    return;
  }
  edit->setText( fAdmin->getFontName() );
  tb->addWidget(fComboFont);

  fComboSize = new QComboBox( tb );
  fComboSize->setEditable(true);
  QList<int> sizes = db.standardSizes();
  QList<int>::Iterator it = sizes.begin();
  for ( ; it != sizes.end(); ++it )
    fComboSize->addItem( QString::number( *it ) );
  connect( fComboSize, SIGNAL( activated( const QString & ) ),
           this, SLOT( textSize( const QString & ) ) );
  edit = fComboSize->lineEdit();
  if (edit == 0) {
//    qDebug() << endl << "**ERROR** PTextEdit::setupTextActions(): cannot edit fComboSize" << endl;
    return;
  }
  edit->setText( QString("%1").arg(fAdmin->getFontSize()) );
  tb->addWidget(fComboSize);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Setup the musrfit menu and the necessary actions.
 */
void PTextEdit::setupMusrActions()
{
  addToolBarBreak();

  QToolBar *tb = new QToolBar( this );
  tb->setWindowTitle( "Musr Actions" );
  addToolBar( tb );

  QMenu *menu = new QMenu( tr( "&MusrFit" ), this );
  menuBar()->addMenu( menu );

  QAction *a;
  a = new QAction( QIcon( QPixmap( ":/images/musrasym.xpm" ) ), tr( "&Asymmetry Default" ), this );
  a->setShortcut( tr("Alt+A") );
  a->setStatusTip( tr("Get Default Asymmetry msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrGetAsymmetryDefault() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( QIcon( QPixmap( ":/images/musrsinglehisto.xpm" ) ), tr( "Single &Histogram Default" ), this );
  a->setShortcut( tr("Alt+H") );
  a->setStatusTip( tr("Get Default Single Histogram msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrGetSingleHistoDefault() ) );
  tb->addAction(a);
  menu->addAction(a);

  menu->addSeparator();
  tb->addSeparator();

  a = new QAction( QIcon( QPixmap( ":/images/musrcalcchisq.xpm" ) ), tr( "Calculate Chisq" ), this );
  a->setShortcut( tr("Alt+C") );
  a->setStatusTip( tr("Calculate Chi Square (Log Max Likelihood)") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrCalcChisq() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( QIcon( QPixmap( ":/images/musrfit.xpm" ) ), tr( "&Fit" ), this );
  a->setShortcut( tr("Alt+F") );
  a->setStatusTip( tr("Fit") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrFit() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( QIcon( QPixmap( ":/images/musrswap.xpm" ) ), tr( "&Swap Msr <-> Mlog" ), this );
  a->setShortcut( tr("Alt+S") );
  a->setStatusTip( tr("Swap msr-file <-> mlog-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrSwapMsrMlog() ) );
  tb->addAction(a);
  menu->addAction(a);

  a = new QAction( QIcon( QPixmap( ":/images/musrmsr2data.xpm" ) ), tr( "&Msr2Data" ), this );
  a->setShortcut( tr("Alt+M") );
  a->setStatusTip( tr("Start msr2data interface") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrMsr2Data() ) );
  tb->addAction(a);
  menu->addAction(a);

  menu->addSeparator();
  tb->addSeparator();

  a = new QAction( QIcon( QPixmap( ":/images/musrview.xpm" ) ), tr( "&View" ), this );
  a->setShortcut( tr("Alt+V") );
  a->setStatusTip( tr("Start musrview") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrView() ) );
  tb->addAction(a);
  menu->addAction(a);

  fMusrT0Action = new QAction( QIcon( QPixmap( ":/images/musrt0.xpm" ) ), tr( "&T0" ), this );
  fMusrT0Action->setStatusTip( tr("Start musrt0") );
  connect( fMusrT0Action, SIGNAL( triggered() ), this, SLOT( musrT0() ) );
  tb->addAction(fMusrT0Action);
  menu->addAction(fMusrT0Action);
  fMusrT0Action->setEnabled(fEnableMusrT0);

  a = new QAction( QIcon( QPixmap( ":/images/musrprefs.xpm" ) ), tr( "&Preferences" ), this );
  a->setStatusTip( tr("Show Preferences") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrPrefs() ) );
  tb->addAction(a);
  menu->addAction(a);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Setup the help menu and the necessary actions.
 */
void PTextEdit::setupHelpActions()
{
  QMenu *menu = new QMenu( tr( "&Help" ), this );
  menuBar()->addMenu( menu);

  QAction *a;
  a = new QAction(tr( "Contents ..." ), this );
  a->setStatusTip( tr("Help Contents") );
  connect( a, SIGNAL( triggered() ), this, SLOT( helpContents() ));
  menu->addAction(a);

  a = new QAction(tr( "About ..." ), this );
  a->setStatusTip( tr("Help About") );
  connect( a, SIGNAL( triggered() ), this, SLOT( helpAbout() ));
  menu->addAction(a);

  a = new QAction(tr( "About Qt..." ), this );
  a->setStatusTip( tr("Help About Qt") );
  connect( a, SIGNAL( triggered() ), this, SLOT( helpAboutQt() ));
  menu->addAction(a);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>load an msr-file.
 *
 * \param f filename
 * \param index if == -1, add the file as a new tab, otherwise, replace the contents of the tab at index.
 */
void PTextEdit::load( const QString &f, const int index )
{
  // check if the file exists
  if ( !QFile::exists( f ) )
    return;

  // create a new text edit object
  PSubTextEdit *edit = new PSubTextEdit( fAdmin );
  edit->setFont(QFont(fAdmin->getFontName(), fAdmin->getFontSize()));

  // place the text edit object at the appropriate tab position
  if (index == -1)
    fTabWidget->addTab( edit, QFileInfo( f ).fileName() );
  else
    fTabWidget->insertTab( index, edit, QFileInfo( f ).fileName() );
  QFile file( f );
  if ( !file.open( QIODevice::ReadOnly ) )
    return;

  // add the msr-file to the file system watchersssss
  fFileSystemWatcher->addPath(f);

  // read the file
  QTextStream ts( &file );
  QString txt = ts.readAll();
  edit->setPlainText( txt );
  doConnections( edit ); // add all necessary signal/slot connections

  // set the tab widget to the current tab
  fTabWidget->setCurrentIndex(fTabWidget->indexOf(edit));
  edit->viewport()->setFocus();

  // update the filename mapper
  fFilenames.remove( edit );
  fFilenames.insert( edit, f );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns the currently tab selected textedit object.
 */
PSubTextEdit *PTextEdit::currentEditor() const
{
  if ( fTabWidget->currentWidget() ) {
    if (fTabWidget->currentWidget()->inherits( "PSubTextEdit" )) {
      return (PSubTextEdit*)fTabWidget->currentWidget();
    }
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Setup the necessray signal/slot connections for the textedit object.
 *
 * \param e textedit object
 */
void PTextEdit::doConnections( PSubTextEdit *e )
{
//  connect( e, SIGNAL( currentFontChanged( const QFont & ) ),
//           this, SLOT( fontChanged( const QFont & ) ) );

  connect( e, SIGNAL( textChanged() ), this, SLOT( textChanged() ));

  connect( e, SIGNAL( cursorPositionChanged() ), this, SLOT( currentCursorPosition() ));
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Validates a given runlist.
 *
 * \param runList run list string which should be a space separated list of run numbers.
 */
bool PTextEdit::validRunList(const QString runList)
{
  bool success = true;

  int i = 0;
  QString subStr;
  bool done = false;
  int val = 0;
  bool ok;
  while (!done) {
    subStr = runList.section(' ', i, i, QString::SectionSkipEmpty);
    if (subStr.isEmpty()) {
      done = true;
      continue;
    }
    i++;
    val = subStr.toInt(&ok);
    if (!ok) {
      done = true;
      success = false;
    }
  }

  if (i == 0) { // no token found
    success = false;
  }

  return success;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file title. See also https://intranet.psi.ch/MUSR/MusrFit#4_1_The_Title
 */
void PTextEdit::insertTitle()
{
  currentEditor()->insertTitle();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file fit-parameter block. See also https://intranet.psi.ch/MUSR/MusrFit#4_2_The_FITPARAMETER_Block
 */
void PTextEdit::insertParameterBlock()
{
  currentEditor()->insertParameterBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file theory block. See also https://intranet.psi.ch/MUSR/MusrFit#4_3_The_THEORY_Block
 */
void PTextEdit::insertTheoryBlock()
{
  currentEditor()->insertTheoryBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Insert a selected theory function. See also https://intranet.psi.ch/MUSR/MusrFit#4_3_The_THEORY_Block
 *
 * \param a action of the selected theory function
 */
void PTextEdit::insertTheoryFunction(QAction *a)
{
  currentEditor()->insertTheoryFunction(a->text());
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file function block. See also https://intranet.psi.ch/MUSR/MusrFit#4_4_The_FUNCTIONS_Block
 */
void PTextEdit::insertFunctionBlock()
{
  currentEditor()->insertFunctionBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file asymmetry run block. See also https://intranet.psi.ch/MUSR/MusrFit#4_5_The_RUN_Block
 */
void PTextEdit::insertAsymRunBlock()
{
  currentEditor()->insertAsymRunBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file single histogram run block. See also https://intranet.psi.ch/MUSR/MusrFit#4_5_The_RUN_Block
 */
void PTextEdit::insertSingleHistRunBlock()
{
  currentEditor()->insertSingleHistRunBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file nonMusr run block. See also https://intranet.psi.ch/MUSR/MusrFit#4_5_The_RUN_Block
 */
void PTextEdit::insertNonMusrRunBlock()
{
  currentEditor()->insertNonMusrRunBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Inserts a default command block. See also https://intranet.psi.ch/MUSR/MusrFit#4_6_The_COMMANDS_Block
 */
void PTextEdit::insertCommandBlock()
{
  currentEditor()->insertCommandBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file Fourier block. See also https://intranet.psi.ch/MUSR/MusrFit#4_7_The_FOURIER_Block
 */
void PTextEdit::insertFourierBlock()
{
  currentEditor()->insertFourierBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file plot block. See also https://intranet.psi.ch/MUSR/MusrFit#4_7_The_PLOT_Block
 */
void PTextEdit::insertPlotBlock()
{
  currentEditor()->insertPlotBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Inserts a default statistics block. See also https://intranet.psi.ch/MUSR/MusrFit#4_9_The_STATISTIC_Block
 */
void PTextEdit::insertStatisticBlock()
{
  currentEditor()->insertStatisticBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: called when File/New is selected. Will generate an empty tab-textedit field in musredit.
 */
void PTextEdit::fileNew()
{
  PSubTextEdit *edit = new PSubTextEdit( fAdmin );
  edit->setFont(QFont(fAdmin->getFontName(), fAdmin->getFontSize()));
  doConnections( edit );
  fTabWidget->addTab( edit, tr( "noname" ) );
  fTabWidget->setCurrentIndex(fTabWidget->indexOf(edit));
  fFilenames.insert(edit, tr("noname"));
  edit->viewport()->setFocus();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: called when File/Open is selected. Will open an msr-/mlog-file. It checks if the file is
 * already open, and if so will just select the current tab. If you want to reload the file use the
 * fileReload() slot.
 */
void PTextEdit::fileOpen()
{
  QStringList flns = QFileDialog::getOpenFileNames( this, tr("Open msr-/mlog-File"),
                        fLastDirInUse,
                        tr( "msr-Files (*.msr);;msr-Files (*.msr *.mlog);;All Files (*)" ));

  QStringList::Iterator it = flns.begin();
  QFileInfo finfo1, finfo2;
  QString tabFln;
  bool alreadyOpen = false;

  // if flns are present, keep the corresponding directory
  if (flns.size() > 0) {
    finfo1.setFile(flns.at(0));
    fLastDirInUse = finfo1.absoluteFilePath();
  }

  while( it != flns.end() ) {
    // check if the file is not already open
    finfo1.setFile(*it);
    for (int i=0; i<fTabWidget->count(); i++) {
      tabFln = *fFilenames.find( dynamic_cast<PSubTextEdit*>(fTabWidget->widget(i)));
 //     qDebug() << endl << "tabFln=" << tabFln;
      finfo2.setFile(tabFln);
      if (finfo1.absoluteFilePath() == finfo2.absoluteFilePath()) {
        alreadyOpen = true;
        fTabWidget->setCurrentIndex(i);
        break;
      }
    }

    if (!alreadyOpen)
      load(*it);
    else
      fileReload();

    ++it;
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Will reload the currently selected msr-file.
 */
void PTextEdit::fileReload()
{
  if ( fFilenames.find( currentEditor() ) == fFilenames.end() ) {
    QMessageBox::critical(this, "**ERROR**", "Cannot reload a file not previously saved ;-)");
  } else {
    int index = fTabWidget->currentIndex();
    QString fln = *fFilenames.find( currentEditor() );
    fileClose(false);
    load(fln, index);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Will save the currently selected file.
 */
void PTextEdit::fileSave()
{
  if ( !currentEditor() )
    return;

  fFileSystemWatcherActive = false;

  if ( *fFilenames.find( currentEditor() ) == QString("noname") ) {
    fileSaveAs();
  } else {
    QFile file( *fFilenames.find( currentEditor() ) );
    if ( !file.open( QIODevice::WriteOnly ) )
      return;
    QTextStream ts( &file );
    ts << currentEditor()->toPlainText();

    // remove trailing '*' modification indicators
    QString fln = *fFilenames.find( currentEditor() );
    fTabWidget->setTabText(fTabWidget->indexOf( currentEditor() ), QFileInfo(fln).fileName());
  }

  fileSystemWatcherActivation(); // delayed activation of fFileSystemWatcherActive
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Will open a file selector dialog to obtain a file name, and then save the file under the newly
 * selected file name.
 */
void PTextEdit::fileSaveAs()
{
  if ( !currentEditor() )
    return;

  fFileSystemWatcherActive = false;

  QString fn = QFileDialog::getSaveFileName( this,
                    tr( "Save msr-/mlog-file As" ), *fFilenames.find( currentEditor() ),
                    tr( "msr-Files (*.msr *.mlog);;All Files (*)" ) );
  if ( !fn.isEmpty() ) {
    fFilenames.remove( currentEditor() );
    fFilenames.insert( currentEditor(), fn );
    fileSave();
    fTabWidget->setTabText(fTabWidget->indexOf( currentEditor() ), QFileInfo(fn).fileName());
  }

  fileSystemWatcherActivation(); // delayed activation of fFileSystemWatcherActive
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Will call a print dialog and print the msr-file.
 */
void PTextEdit::filePrint()
{
  if ( !currentEditor() )
    return;
#ifndef QT_NO_PRINTER
  QPrinter printer( QPrinter::HighResolution );
  printer.setFullPage(true);
  QPrintDialog dialog(&printer, this);
  if (dialog.exec()) { // printer dialog
    statusBar()->showMessage( "Printing..." );

    QPainter p( &printer );
    // Check that there is a valid device to print to.
    if ( !p.device() )
      return;

    QFont font( currentEditor()->QWidget::font() );
    font.setPointSize( 10 ); // we define 10pt to be a nice base size for printing
    p.setFont( font );

    int yPos        = 0;                  // y-position for each line
    QFontMetrics fm = p.fontMetrics();
    int dpiy = printer.logicalDpiY();
    int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins

    // print msr-file
    QStringList strList = currentEditor()->toPlainText().split("\n");
    for (QStringList::Iterator it = strList.begin(); it != strList.end(); ++it) {
      // new page needed?
      if ( margin + yPos > printer.height() - margin ) {
        printer.newPage();             // no more room on this page
        yPos = 0;                      // back to top of page
      }

      // print data
      p.drawText(margin, margin+yPos, printer.width(), fm.lineSpacing(),
                 Qt::TextExpandTabs | Qt::TextDontClip, *it);
      yPos += fm.lineSpacing();
    }

    p.end();
  }
#endif
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Will remove a textedit tab from musredit. Depending on check it will done with or without checking
 * if the file has been saved before.
 *
 * \param check if set to true, a warning dialog will popup if the file is not saved yet.
 */
void PTextEdit::fileClose(const bool check)
{
  // check if the has modification
  int idx = fTabWidget->currentIndex();
  if ((fTabWidget->tabText(idx).indexOf("*")>0) && check) {
    int result = QMessageBox::warning(this, "**WARNING**", 
                   "Do you really want to close this file.\nChanges will be lost",
                   "Close", "Cancel");
    if (result == 1) // Cancel
      return;
  }

  QString str = *fFilenames.find(currentEditor());
  fFileSystemWatcher->removePath(str);

  delete currentEditor();

  if ( currentEditor() )
    currentEditor()->viewport()->setFocus();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Removes all textedit tabs from musredit. It checks if the files haven't been saved, and if so it will
 * popup a warning dialog.
 */
void PTextEdit::fileCloseAll()
{
  // check if any editor tab is present, if not: get out of here
  if ( !currentEditor() )
    return;

  // check if there are any unsaved tabs
  for (int i=0; i<fTabWidget->count(); i++) {
    if (fTabWidget->tabText(i).indexOf("*") > 0) {
      int result = QMessageBox::warning(this, "**WARNING**",
                     "Do you really want to close all files.\nChanges of unsaved files will be lost",
                     "Close", "Cancel");
      if (result == 1) // Cancel
        return;
      break;
    }
  }

  // close all editor tabs
  QString str;
  do {
    // remove file from file system watcher
    str = *fFilenames.find(currentEditor());
    fFileSystemWatcher->removePath(str);

    delete currentEditor();

    if ( currentEditor() )
      currentEditor()->viewport()->setFocus();
  } while ( currentEditor() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Will close all textedit tabs but the selected one. It checks if the files haven't been saved, and if so it will
 * popup a warning dialog.
 */
void PTextEdit::fileCloseAllOthers()
{
  // check if any editor tab is present, if not: get out of here
  if ( !currentEditor() )
    return;

  // check if there are any unsaved tabs
  for (int i=0; i<fTabWidget->count(); i++) {
    if (fTabWidget->tabText(i).indexOf("*") > 0) {
      int result = QMessageBox::warning(this, "**WARNING**",
                     "Do you really want to close all files.\nChanges of unsaved files will be lost",
                     "Close", "Cancel");
      if (result == 1) // Cancel
        return;
      break;
    }
  }

  // keep label of the current editor
  QString label = fTabWidget->tabText(fTabWidget->currentIndex());

  // check if only the current editor is present. If yes: nothing to be done
  if (fTabWidget->count() == 1)
    return;

  // close all editor tabs
  int i=0;
  QString str;
  do {
    if (fTabWidget->tabText(i) != label) {
      // remove file from file system watcher
      str = *fFilenames.find(dynamic_cast<PSubTextEdit*>(fTabWidget->widget(i)));
      fFileSystemWatcher->removePath(str);

      delete fTabWidget->widget(i);
    } else {
      i++;
    }
  } while ( fTabWidget->count() > 1 );

  currentEditor()->viewport()->setFocus();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Will quit musredit.
 */
void PTextEdit::fileExit()
{
  // check if there are still some modified files open
  for (int i=0; i < fTabWidget->count(); i++) {
    if (fTabWidget->tabText(i).indexOf("*") > 0) {
      int result = QMessageBox::warning(this, "**WARNING**", 
                     "Do you really want to exit from the applcation.\nChanges will be lost",
                     "Exit", "Cancel");
      if (result == 1) // Cancel
        return;
      break;
    }
  }

  qApp->quit();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Text undo of the current textedit tab.
 */
void PTextEdit::editUndo()
{
  if ( !currentEditor() )
    return;
  currentEditor()->undo();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Text redo of the current textedit tab
 */
void PTextEdit::editRedo()
{
  if ( !currentEditor() )
    return;
  currentEditor()->redo();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Select all text of the current textedit tab.
 */
void PTextEdit::editSelectAll()
{
  if ( !currentEditor() )
    return;
  currentEditor()->selectAll();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Cut the selection of the current textedit tab.
 */
void PTextEdit::editCut()
{
  if ( !currentEditor() )
    return;
  currentEditor()->cut();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Copy the selection of the current textedit tab.
 */
void PTextEdit::editCopy()
{
  if ( !currentEditor() )
    return;
  currentEditor()->copy();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Paste at the cursor position of the current textedit tab.
 */
void PTextEdit::editPaste()
{
  if ( !currentEditor() )
    return;
  currentEditor()->paste();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Starts a find dialog, and searches for a find string.
 */
void PTextEdit::editFind()
{
  if ( !currentEditor() )
    return;

  // check if first time called, and if yes create find and replace data structure
  if (fFindReplaceData == 0) {
    fFindReplaceData = new PFindReplaceData();
    fFindReplaceData->findText = QString("");
    fFindReplaceData->replaceText = QString("");
    fFindReplaceData->caseSensitive = true;
    fFindReplaceData->wholeWordsOnly = false;
    fFindReplaceData->fromCursor = true;
    fFindReplaceData->findBackwards = false;
    fFindReplaceData->selectedText = false;
    fFindReplaceData->promptOnReplace = true;
  }

  if (fFindReplaceData == 0) {
    QMessageBox::critical(this, "**ERROR**", "Couldn't invoke find data structure, sorry :-(", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  PFindDialog *dlg = new PFindDialog(fFindReplaceData, currentEditor()->textCursor().hasSelection());
  if (dlg == 0) {
    QMessageBox::critical(this, "**ERROR**", "Couldn't invoke find dialog, sorry :-(", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  dlg->exec();

  if (dlg->result() != QDialog::Accepted) {
    delete dlg;
    return;
  }

  fFindReplaceData = dlg->getData();

  delete dlg;
  dlg = 0;

  // try to find the search text
  if (!fFindReplaceData->fromCursor)
    currentEditor()->textCursor().setPosition(0);

  QTextDocument::FindFlags flags = 0;
  if (fFindReplaceData->caseSensitive)
    flags |= QTextDocument::FindCaseSensitively;
  else if (fFindReplaceData->findBackwards)
    flags |= QTextDocument::FindBackward;
  else if (fFindReplaceData->wholeWordsOnly)
    flags |= QTextDocument::FindWholeWords;

  currentEditor()->find(fFindReplaceData->findText, flags);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Searches for a find string in the forward direction.
 */
void PTextEdit::editFindNext()
{
  QTextDocument::FindFlags flags = 0;
  if (fFindReplaceData->caseSensitive)
    flags |= QTextDocument::FindCaseSensitively;
  else if (fFindReplaceData->wholeWordsOnly)
    flags |= QTextDocument::FindWholeWords;

  currentEditor()->find(fFindReplaceData->findText, flags);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Searches for a find string in the backward direction.
 */
void PTextEdit::editFindPrevious()
{
  QTextDocument::FindFlags flags = 0;
  if (fFindReplaceData->caseSensitive)
    flags |= QTextDocument::FindCaseSensitively;
  else if (fFindReplaceData->wholeWordsOnly)
    flags |= QTextDocument::FindWholeWords;

  flags |= QTextDocument::FindBackward;

  currentEditor()->find(fFindReplaceData->findText, flags);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Starts a find/replace dialog, and searches for a find string and replaces it depending of the replace options.
 */
void PTextEdit::editFindAndReplace()
{
  if ( !currentEditor() )
    return;

  // check if first time called, and if yes create find and replace data structure
  if (fFindReplaceData == 0) {
    fFindReplaceData = new PFindReplaceData();
    fFindReplaceData->findText = QString("");
    fFindReplaceData->replaceText = QString("");
    fFindReplaceData->caseSensitive = true;
    fFindReplaceData->wholeWordsOnly = false;
    fFindReplaceData->fromCursor = true;
    fFindReplaceData->findBackwards = false;
    fFindReplaceData->selectedText = false;
    fFindReplaceData->promptOnReplace = true;
  }

  if (fFindReplaceData == 0) {
    QMessageBox::critical(this, "**ERROR**", "Couldn't invoke find&replace data structure, sorry :-(", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  PReplaceDialog *dlg = new PReplaceDialog(fFindReplaceData, currentEditor()->textCursor().hasSelection());
  if (dlg == 0) {
    QMessageBox::critical(this, "**ERROR**", "Couldn't invoke find&replace dialog, sorry :-(", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  dlg->exec();

  if (dlg->result() != QDialog::Accepted) {
    delete dlg;
    return;
  }

  fFindReplaceData = dlg->getData();

  delete dlg;
  dlg = 0;

  editFindNext();

  PReplaceConfirmationDialog confirmDlg(this);

  // connect all the necessary signals/slots
  QObject::connect(confirmDlg.fReplace_pushButton, SIGNAL(clicked()), this, SLOT(replace()));
  QObject::connect(confirmDlg.fReplaceAndClose_pushButton, SIGNAL(clicked()), this, SLOT(replaceAndClose()));
  QObject::connect(confirmDlg.fReplaceAll_pushButton, SIGNAL(clicked()), this, SLOT(replaceAll()));
  QObject::connect(confirmDlg.fFindNext_pushButton, SIGNAL(clicked()), this, SLOT(editFindNext()));
  QObject::connect(this, SIGNAL(close()), &confirmDlg, SLOT(accept()));

  confirmDlg.exec();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Comment a selection, i.e. add a '#' character in front of each line of the selection.
 * (Multiple selections not yet supported).
 */
void PTextEdit::editComment()
{
  if ( !currentEditor() )
    return;

  
  QTextCursor curs = currentEditor()->textCursor();
  
  if (curs.hasComplexSelection()) { // multiple selections
    // multiple selections (STILL MISSING)
  } else if (curs.hasSelection()) { // simple selection
    int pos = curs.position();
    int secStart = curs.selectionStart(); // keep start position of the selection
    int secEnd = curs.selectionEnd();     // keep end position of the selection
    curs.clearSelection();
    curs.setPosition(secStart);           // set the anchor to the start of the selection
    curs.movePosition(QTextCursor::StartOfBlock);
    do {
      curs.insertText("#");
    } while (curs.movePosition(QTextCursor::NextBlock) && (curs.position() <= secEnd));
    curs.setPosition(pos+1);
  } else { // no selection
    int pos = curs.position();
    curs.clearSelection();
    curs.movePosition(QTextCursor::StartOfLine);
    curs.insertText("#");
    curs.setPosition(pos+1);
  }  
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Uncomment a selection, i.e. remove a '#' character in front of each line of the selection.
 * (Multiple selections not yet supported).
 */
void PTextEdit::editUncomment()
{
  if ( !currentEditor() )
    return;


  QTextCursor curs = currentEditor()->textCursor();

  if (curs.hasComplexSelection()) {
    // multiple selections (STILL MISSING)
  } else if (curs.hasSelection()) {
    int pos = curs.position();
    int secStart = curs.selectionStart(); // keep start position of the selection
    int secEnd = curs.selectionEnd();     // keep end position of the selection
    curs.clearSelection();
    curs.setPosition(secStart);           // set the anchor to the start of the selection
    curs.movePosition(QTextCursor::StartOfBlock);
    while (curs.position() < secEnd) {
      QString line = curs.block().text();
      if (line.startsWith("#")) {
        line.remove(0, 1);
        curs.select(QTextCursor::BlockUnderCursor);
        curs.removeSelectedText();        
        curs.insertText("\n"+line);
        pos -= 1;
      }
      curs.movePosition(QTextCursor::NextBlock);
    }
    curs.setPosition(pos);
    currentEditor()->setTextCursor(curs); // needed to update document cursor
  } else { // no selection
    int pos = curs.position();
    curs.clearSelection();
    curs.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor, 1);
    QString line = curs.block().text();
    if (line.startsWith("#")) {
      line.remove(0, 1);
      curs.select(QTextCursor::BlockUnderCursor);
      curs.removeSelectedText();
      if (currentEditor()->textCursor().columnNumber() == 0)
        curs.insertText(line);
      else
        curs.insertText("\n"+line);
      pos -= 1;
    }
    curs.setPosition(pos);
    currentEditor()->setTextCursor(curs); // needed to update document cursor
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Chances the font.
 *
 * \param f font name
 */
void PTextEdit::textFamily( const QString &f )
{
  fAdmin->setFontName(f);

  if ( !currentEditor() )
    return;

  currentEditor()->setFont(QFont(f,fAdmin->getFontSize()));
  currentEditor()->viewport()->setFocus();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Chances the font size.
 *
 * \param p font size in points.
 */
void PTextEdit::textSize( const QString &p )
{
  fAdmin->setFontSize(p.toInt());

  if ( !currentEditor() )
    return;

  currentEditor()->setFont(QFont(fAdmin->getFontName(), p.toInt()));
  currentEditor()->viewport()->setFocus();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Loads a default asymmetry msr-file.
 */
void PTextEdit::musrGetAsymmetryDefault()
{
  QString runFileName, beamline, institute, fileFormat;

  PGetDefaultDialog *dlg = new PGetDefaultDialog(fAdmin->getHelpUrl("main"));

  if (dlg == 0) {
    QMessageBox::critical(this, "**ERROR**", "Couldn't invoke get default dialog, sorry :-(", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  // set defaults
  dlg->setBeamline(fAdmin->getBeamline());
  dlg->setInstitute(fAdmin->getInstitute());
  dlg->setFileFormat(fAdmin->getFileFormat());

  dlg->exec();

  if (dlg->result() != QDialog::Accepted) {
    delete dlg;
    dlg = 0;
    return;
  }

  runFileName = dlg->getRunFileName();
  beamline    = dlg->getBeamline();
  institute   = dlg->getInstitute();
  fileFormat  = dlg->getFileFormat();
  delete dlg;
  dlg = 0;

  QFile file(fAdmin->getMsrDefaultFilePath()+"/asymDefault.msr");
  if (file.open(QIODevice::ReadOnly)) {
    // make a new file tab
    fileNew();
    QTextStream ts( &file );
    QString line;
    while ( !ts.atEnd() ) {
      line = ts.readLine(); // line of text excluding '\n'
      if (line.startsWith("RUN")) {
        QString runHeader = "RUN " + runFileName + " " + beamline.toUpper() + " " + institute + " " + fileFormat.toUpper() + " (name beamline institute data-file-format)\n";
        currentEditor()->insertPlainText(runHeader);
      } else { // just copy the text
        currentEditor()->insertPlainText(line+"\n");
      }
    }
    currentEditor()->textCursor().setPosition(0);

    file.close();
  } else {
    QMessageBox::critical(this, "**ERROR**",
       "Couldn't find default asymmetry file template :-(",
       QMessageBox::Ok, QMessageBox::NoButton);
  }

}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Loads a default single histogram msr-file.
 */
void PTextEdit::musrGetSingleHistoDefault()
{
  QString runFileName, beamline, institute, fileFormat;

  PGetDefaultDialog *dlg = new PGetDefaultDialog();

  if (dlg == 0) {
    QMessageBox::critical(this, "**ERROR**", "Couldn't invoke get default dialog, sorry :-(", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  // set defaults
  dlg->setBeamline(fAdmin->getBeamline());
  dlg->setInstitute(fAdmin->getInstitute().toLower());
  dlg->setFileFormat(fAdmin->getFileFormat().toLower());

  dlg->exec();

  if (dlg->result() != QDialog::Accepted) {
    delete dlg;
    dlg = 0;
    return;
  }

  runFileName = dlg->getRunFileName();
  beamline    = dlg->getBeamline();
  institute   = dlg->getInstitute();
  fileFormat  = dlg->getFileFormat();
  delete dlg;
  dlg = 0;

  QFile file(fAdmin->getMsrDefaultFilePath()+"/singleHistoDefault.msr");
  if (file.open(QIODevice::ReadOnly)) {
    // make a new file tab
    fileNew();
    QTextStream ts( &file );
    QString line;
    while ( !ts.atEnd() ) {
      line = ts.readLine(); // line of text excluding '\n'
      if (line.startsWith("RUN")) {
        QString runHeader = "RUN " + runFileName + " " + beamline.toUpper() + " " + institute + " " + fileFormat.toUpper() + " (name beamline institute data-file-format)\n";
        currentEditor()->insertPlainText(runHeader);
      } else { // just copy the text
        currentEditor()->insertPlainText(line+"\n");
      }
    }
    currentEditor()->textCursor().setPosition(0);

    file.close();
  } else {
    QMessageBox::critical(this, "**ERROR**",
       "Couldn't find default single histogram file template :-(",
       QMessageBox::Ok, QMessageBox::NoButton);
  }

}

//----------------------------------------------------------------------------------------------------
/**
 * <p>calls musrfit --chisq-only <msr-file>.
 */
void PTextEdit::musrCalcChisq()
{
  if ( !currentEditor() )
    return;

  QString tabLabel = fTabWidget->tabText(fTabWidget->currentIndex());
  if (tabLabel == "noname") {
    QMessageBox::critical(this, "**ERROR**", "For a fit a real msr-file is needed.");
    return;
  } else if (tabLabel == "noname*") {
    fileSaveAs();
  } else if (tabLabel.indexOf("*") > 0) {
    fileSave();
  }

  QVector<QString> cmd;
  QString str;
  str = fAdmin->getExecPath() + "/musrfit";

  cmd.append(str);
  cmd.append(QFileInfo(*fFilenames.find( currentEditor())).fileName() );
  cmd.append("--chisq-only");
  PFitOutputHandler fitOutputHandler(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath(), cmd);
  fitOutputHandler.setModal(true);
  fitOutputHandler.exec();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>calls musrfit <options> <msr-file>. <options> can be set in the MusrFit/Preferences.
 */
void PTextEdit::musrFit()
{
  if ( !currentEditor() )
    return;

  QString tabLabel = fTabWidget->tabText(fTabWidget->currentIndex());
  if (tabLabel == "noname") {
    QMessageBox::critical(this, "**ERROR**", "For a fit a real msr-file is needed.");
    return;
  } else if (tabLabel == "noname*") {
    fileSaveAs();
  } else if (tabLabel.indexOf("*") > 0) {
    fileSave();
  }

  QVector<QString> cmd;
  QString str;
  str = fAdmin->getExecPath() + "/musrfit";

  cmd.append(str);
  cmd.append(QFileInfo(*fFilenames.find( currentEditor())).fileName() );

  // check if keep minuit2 output is wished
  if (fKeepMinuit2Output)
    cmd.append("--keep-mn2-output");

  // check if title of the data file should be used to replace the msr-file title
  if (fTitleFromDataFile)
    cmd.append("--title-from-data-file");

  // check if dump files are wished
  switch (fDump) {
    case 1: // ascii dump
      cmd.append("--dump");
      cmd.append("ascii");
      break;
    case 2: // root dump
      cmd.append("--dump");
      cmd.append("root");
      break;
    default:
      break;
  }

  // add timeout
  cmd.append("--timeout");
  QString numStr;
  numStr.setNum(fAdmin->getTimeout());
  cmd.append(numStr);

  PFitOutputHandler fitOutputHandler(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath(), cmd);
  fitOutputHandler.setModal(true);
  fFileSystemWatcherActive = false;
  fitOutputHandler.exec();

  // handle the reloading of the files

  // get current file name
  QString currentFileName = *fFilenames.find( currentEditor() );
  QString complementFileName;
  // check if it is a msr-, mlog-, or another file
  int idx;
  if ((idx = currentFileName.indexOf(".msr")) > 0) { // msr-file
    complementFileName = currentFileName;
    complementFileName.replace(idx, 5, ".mlog");
  } else if ((idx = currentFileName.indexOf(".mlog")) > 0) { // mlog-file
    complementFileName = currentFileName;
    complementFileName.replace(idx, 5, ".msr ");
    complementFileName = complementFileName.trimmed();
  } else { // neither a msr- nor a mlog-file
    QMessageBox::information( this, "musrFit",
    "This is neither a msr- nor a mlog-file, hence no idea what to be done.\n",
    QMessageBox::Ok );
    return;
  }

  int currentIdx = fTabWidget->currentIndex();

  // reload current file
  fileClose();
  load(currentFileName, currentIdx);

  // check if swap file is open as well, and if yes, reload it
  idx = -1;
  for (int i=0; i<fTabWidget->count(); i++) {
    if (fTabWidget->tabText(i).indexOf(complementFileName) >= 0) {
      idx = i;
      break;
    }
  }
  if (idx >= 0) { // complement file is open
    fTabWidget->setCurrentIndex(idx);
    fileClose();
    load(complementFileName, idx);
    fTabWidget->setCurrentIndex(currentIdx);
  }

  fileSystemWatcherActivation();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Starts a msr2data input dialog which collects all the necessary data before calling msr2data.
 */
void PTextEdit::musrMsr2Data()
{
  if (fMsr2DataParam == 0) {
    fMsr2DataParam = new PMsr2DataParam();
    *fMsr2DataParam = fAdmin->getMsr2DataParam();
  }

  // init fMsr2DataParam
  fMsr2DataParam->keepMinuit2Output = fKeepMinuit2Output;
  fMsr2DataParam->titleFromDataFile = fTitleFromDataFile;

  PMsr2DataDialog *dlg = new PMsr2DataDialog(fMsr2DataParam, fAdmin->getHelpUrl("msr2data"));

  if (dlg == 0) {
    QMessageBox::critical(this, "**ERROR**", "Couldn't invoke msr2data dialog, sorry :-(", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  if (dlg->exec() == QDialog::Accepted) {
    QString first, last;
    QString runList;
    QString runListFileName;
    QFileInfo fi;
    QString str;
    int i, end;

    fMsr2DataParam = dlg->getMsr2DataParam();
    fKeepMinuit2Output = fMsr2DataParam->keepMinuit2Output;
    fTitleFromDataFile = fMsr2DataParam->titleFromDataFile;

    // analyze parameters
    switch (dlg->getRunTag()) {
      case -1: // not valid
        QMessageBox::critical(this, "**ERROR**",
           "No valid run list input found :-(\nCannot do anything.",
           QMessageBox::Ok, QMessageBox::NoButton);
        return;
        break;
      case 0:  // first last
        first = QString("%1").arg(fMsr2DataParam->firstRun);
        last  = QString("%1").arg(fMsr2DataParam->lastRun);
        if (first.isEmpty() || last.isEmpty()) {
          QMessageBox::critical(this, "**ERROR**",
            "If you choose the first/last option,\nfirst AND last needs to be provided.",
            QMessageBox::Ok, QMessageBox::NoButton);
          return;
        }
        break;
      case 1:  // run list
        runList = fMsr2DataParam->runList;
        if (!validRunList(runList)) {
          QMessageBox::critical(this, "**ERROR**",
            "Invalid Run List!\nThe run list needs to be a space separated list of run numbers.",
            QMessageBox::Ok, QMessageBox::NoButton);
          return;
        }
        break;
      case 2:  // run list file name
        runListFileName = fMsr2DataParam->runListFileName;
        fi.setFile(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath() + "/" + runListFileName);
        if (!fi.exists()) {
          str = QString("Run List File '%1' doesn't exist.").arg(runListFileName);
          QMessageBox::critical(this, "**ERROR**",
                str, QMessageBox::Ok, QMessageBox::NoButton);
          return;
        }
        break;
      default: // never should reach this point
        QMessageBox::critical(this, "**ERROR**",
           "No idea how you could reach this point.\nPlease contact the developers.",
           QMessageBox::Ok, QMessageBox::NoButton);
        return;
        break;
    }

    // form command
    QVector<QString> cmd;

    str = fAdmin->getExecPath() + "/msr2data";
    cmd.append(str);

    // run list argument
    switch (dlg->getRunTag()) {
      case 0:
        cmd.append(first);
        cmd.append(last);
        break;
      case 1:
        end = 0;
        while (!runList.section(' ', end, end, QString::SectionSkipEmpty).isEmpty()) {
          end++;
        }
        // first element
        if (end == 1) {
          str = "[" + runList + "]";
          cmd.append(str);
        } else {
          str = "[" + runList.section(' ', 0, 0, QString::SectionSkipEmpty);
          cmd.append(str);
          // middle elements
          for (i=1; i<end-1; i++) {
            cmd.append(runList.section(' ', i, i, QString::SectionSkipEmpty));
          }
          // last element
          str = runList.section(' ', end-1, end-1, QString::SectionSkipEmpty) + "]";
          cmd.append(str);
        }
        break;
      case 2:
        cmd.append(runListFileName);
        break;
      default:
        break;
    }

    // file extension
    str = fMsr2DataParam->msrFileExtension;
    if (str.isEmpty())
      cmd.append("");
    else
      cmd.append(str);

    // options

    // no header flag?
    if (!fMsr2DataParam->writeDbHeader)
      cmd.append("noheader");

    // ignore data header info flag present?
    if (fMsr2DataParam->ignoreDataHeaderInfo)
      cmd.append("nosummary");

    // template run no fitting but: (i) no fit only flag, (ii) no create msr-file only flag
    if ((fMsr2DataParam->templateRunNo != -1) && !fMsr2DataParam->fitOnly && !fMsr2DataParam->createMsrFileOnly) {
      str = QString("%1").arg(fMsr2DataParam->templateRunNo);
      str = "fit-" + str;
      if (!fMsr2DataParam->chainFit) {
        str += "!";
      }
      cmd.append(str);
    }

    // template run no AND create msr-file only flag
    if ((fMsr2DataParam->templateRunNo != -1) && fMsr2DataParam->createMsrFileOnly) {
      str = QString("%1").arg(fMsr2DataParam->templateRunNo);
      str = "msr-" + str;
      cmd.append(str);
    }

    // fit only
    if (fMsr2DataParam->fitOnly) {
      str = "fit";
      cmd.append(str);
    }

    // keep minuit2 output
    if (fMsr2DataParam->keepMinuit2Output) {
      cmd.append("-k");
    }

    // replace msr-file title by data file title. Add flag only if a fit is done
    if (fMsr2DataParam->titleFromDataFile && (fMsr2DataParam->fitOnly || fMsr2DataParam->templateRunNo != -1)) {
      cmd.append("-t");
    }

    // DB output wished
    if (!fMsr2DataParam->dbOutputFileName.isEmpty()) {
      str = "-o" + fMsr2DataParam->dbOutputFileName;
      cmd.append(str);
    }

    // write column data
    if (fMsr2DataParam->writeColumnData) {
      cmd.append("data");
    }

    // global flag check
    if (fMsr2DataParam->global) {
      cmd.append("global");
    }

    // global+ flag check
    if (fMsr2DataParam->globalPlus) {
      if (fMsr2DataParam->chainFit) {
        cmd.append("global+");
      } else {
        cmd.append("global+!");
      }
    }

    // recreate db file
    if (fMsr2DataParam->recreateDbFile) {
      cmd.append("new");
    }

//   qDebug() << endl << ">> " << cmd << endl;

    PFitOutputHandler fitOutputHandler(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath(), cmd);
    fitOutputHandler.setModal(true);
    fFileSystemWatcherActive = false;
    fitOutputHandler.exec();

    // check if it is necessary to load msr-files
    if (fMsr2DataParam->openFilesAfterFitting) {
      QString fln;
      QFile *file;
      QTextStream *stream;

      if (!fMsr2DataParam->global) { // standard fits

        switch(dlg->getRunTag()) {
          case 0: // first run / last run list
            if (fMsr2DataParam->firstRun != -1) {
              for (int i=fMsr2DataParam->firstRun; i<=fMsr2DataParam->lastRun; i++) {
                if (fMsr2DataParam->msrFileExtension.isEmpty())
                  fln = QString("%1").arg(i) + ".msr";
                else
                  fln = QString("%1").arg(i) + fMsr2DataParam->msrFileExtension + ".msr";

                load(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath() + "/" + fln);
              }
            }
            break;
          case 1: // run list
            end = 0;
            while (!runList.section(' ', end, end, QString::SectionSkipEmpty).isEmpty()) {
              end++;
            }
            for (int i=0; i<end; i++) {
              fln = runList.section(' ', i, i, QString::SectionSkipEmpty);
              if (fMsr2DataParam->msrFileExtension.isEmpty())
                fln += ".msr";
              else
                fln += fMsr2DataParam->msrFileExtension + ".msr";

              load(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath() + "/" + fln);
            }
            break;
          case 2: // run list file
            file = new QFile(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath() + "/" + fMsr2DataParam->runListFileName);
            if (!file->open(QIODevice::ReadOnly)) {
              str = QString("Couldn't open run list file %1, sorry.").arg(fMsr2DataParam->runListFileName);
              QMessageBox::critical(this, "**ERROR**", str.toLatin1(), QMessageBox::Ok, QMessageBox::NoButton);
              return;
            }

            stream = new QTextStream(file);
            while ( !stream->atEnd() ) {
              str = stream->readLine(); // line of text excluding '\n'
              str.trimmed();
              if (!str.isEmpty() && !str.startsWith("#") && !str.startsWith("run", Qt::CaseInsensitive)) {
                fln = str.section(' ', 0, 0, QString::SectionSkipEmpty);
                if (fMsr2DataParam->msrFileExtension.isEmpty())
                  fln += ".msr";
                else
                  fln += fMsr2DataParam->msrFileExtension + ".msr";

                load(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath() + "/" + fln);
              }
            }

            file->close();

            // clean up
            delete stream;
            delete file;
            break;
          default:
            break;
        }
      } else { // global tag set
        // get the first run number needed to build the global fit file name
        fln = QString("");
        switch(dlg->getRunTag()) {
          case 0: // first/last run
            fln = QString("%1").arg(fMsr2DataParam->firstRun) + QString("+global") + fMsr2DataParam->msrFileExtension + QString(".msr");
            break;
          case 1: // run list
            fln = runList.section(" ", 0, 0, QString::SectionSkipEmpty) + QString("+global") + fMsr2DataParam->msrFileExtension + QString(".msr");
            break;
          case 2: // run list file name
            file = new QFile(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath() + "/" + fMsr2DataParam->runListFileName);
            if (!file->open(QIODevice::ReadOnly)) {
              str = QString("Couldn't open run list file %1, sorry.").arg(fMsr2DataParam->runListFileName);
              QMessageBox::critical(this, "**ERROR**", str.toLatin1(), QMessageBox::Ok, QMessageBox::NoButton);
              return;
            }

            stream = new QTextStream(file);
            while ( !stream->atEnd() ) {
              str = stream->readLine(); // line of text excluding '\n'
              str.trimmed();
              if (!str.isEmpty() && !str.startsWith("#") && !str.startsWith("run", Qt::CaseInsensitive)) {
                fln = str.section(' ', 0, 0, QString::SectionSkipEmpty);
                break;
              }
            }

            file->close();

            fln += QString("+global") + fMsr2DataParam->msrFileExtension + QString(".msr");

            // clean up
            delete stream;
            delete file;
            break;
          default:
            break;
        }

        load(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath() + "/" + fln);
      }
    }
  }

  delete dlg;
  dlg = 0;

  fileSystemWatcherActivation();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Calls musrview <msr-file>.
 */
void PTextEdit::musrView()
{
  if ( !currentEditor() )
    return;

  QString tabLabel = fTabWidget->tabText(fTabWidget->currentIndex());
  if (tabLabel == "noname") {
    QMessageBox::critical(this, "**ERROR**", "For a view a real mlog/msr-file is needed.");
    return;
  } else if (tabLabel == "noname*") {
    fileSaveAs();
  } else if (tabLabel.indexOf("*") > 0) {
    fileSave();
  }

  QString cmd;
  QString str;

  str = "cd \"" + QFileInfo(*fFilenames.find( currentEditor() )).absolutePath() + "\"; ";

  str += fAdmin->getExecPath() + "/musrview";
  cmd = str + " \"";

  str = *fFilenames.find( currentEditor() );
  QString numStr;
  numStr.setNum(fAdmin->getTimeout());
  cmd += str + "\" --timeout " + numStr + " &";

  int status=system(cmd.toLatin1());
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Callse musrt0 <msr-file>.
 */
void PTextEdit::musrT0()
{
  if ( !currentEditor() )
    return;

  QString tabLabel = fTabWidget->tabText(fTabWidget->currentIndex());
  if (tabLabel == "noname") {
    QMessageBox::critical(this, "**ERROR**", "For a view a real mlog/msr-file is needed.");
    return;
  } else if (tabLabel == "noname*") {
    fileSaveAs();
  } else if (tabLabel.indexOf("*") > 0) {
    fileSave();
  }

  QString cmd;
  QString str;

  str = fAdmin->getExecPath() + "/musrt0";
  cmd = str + " \"";

  str = *fFilenames.find( currentEditor() );
  QString numStr;
  numStr.setNum(fAdmin->getTimeout());
  cmd += str + "\" --timeout " + numStr + " &";

  int status=system(cmd.toLatin1());

  QString fln = *fFilenames.find( currentEditor() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Calls the preferences dialog which is used to set some global options.
 */
void PTextEdit::musrPrefs()
{
  PPrefsDialog *dlg = new PPrefsDialog(fKeepMinuit2Output, fDump, fTitleFromDataFile, fEnableMusrT0);
  if (dlg == 0) {
    QMessageBox::critical(this, "**ERROR** musrPrefs", "Couldn't invoke Preferences Dialog.");
    return;
  }

  if (dlg->exec() == QDialog::Accepted) {
    fKeepMinuit2Output = dlg->getKeepMinuit2OutputFlag();
    fTitleFromDataFile = dlg->getTitleFromDataFileFlag();
    fEnableMusrT0 = dlg->getEnableMusrT0Flag();
    fMusrT0Action->setEnabled(fEnableMusrT0);
    fDump = dlg->getDump();
  }

  delete dlg;
  dlg = 0;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Swaps the currently open msr-file with its mlog-file (if present) and updates musredit accordingly.
 */
void PTextEdit::musrSwapMsrMlog()
{
  if ( !currentEditor() )
    return;

  // get current file name
  QString currentFileName = *fFilenames.find( currentEditor() );
  QString swapFileName;
  QString tempFileName = QString("__swap__.msr");

  // check if it is a msr-, mlog-, or another file
  int idx;
  if ((idx = currentFileName.indexOf(".msr")) > 0) { // msr-file
    swapFileName = currentFileName;
    swapFileName.replace(idx, 5, ".mlog");
  } else if ((idx = currentFileName.indexOf(".mlog")) > 0) { // mlog-file
    swapFileName = currentFileName;
    swapFileName.replace(idx, 5, ".msr ");
    swapFileName = swapFileName.trimmed();
  } else { // neither a msr- nor a mlog-file
    QMessageBox::information( this, "musrSwapMsrMlog",
    "This is neither a msr- nor a mlog-file, hence nothing to be swapped.\n",
    QMessageBox::Ok );
    return;
  }

  // check if the swapFile exists
  if (!QFile::exists(swapFileName)) {
    QString msg = "swap file '" + swapFileName + "' doesn't exist.\nCannot swap anything.";
    QMessageBox::warning( this, "musrSwapMsrMlog",
                          msg, QMessageBox::Ok, QMessageBox::NoButton );
    return;
  }

  // check if the file needs to be saved
  if (fTabWidget->tabText(fTabWidget->currentIndex()).indexOf("*") > 0) { // needs to be saved first
    fileSave();
  }

  // swap files
  QString cmd;
  cmd = QString("cp \"") + currentFileName + QString("\" \"") + tempFileName + QString("\"");
  int status=system(cmd.toLatin1());
  cmd = QString("cp \"") + swapFileName + QString("\" \"") + currentFileName + QString("\"");
  status=system(cmd.toLatin1());
  cmd = QString("cp \"") + tempFileName + QString("\" \"") + swapFileName + QString("\"");
  status=system(cmd.toLatin1());
  cmd = QString("rm \"") + tempFileName + QString("\"");
  status=system(cmd.toLatin1());

  int currentIdx = fTabWidget->currentIndex();

  // reload current file
  fileClose();
  load(currentFileName);

  // check if swap file is open as well, and if yes, reload it
  idx = -1;
  for (int i=0; i<fTabWidget->count(); i++) {
    if (fTabWidget->tabText(i).indexOf(swapFileName) >= 0) {
      idx = i;
      break;
    }
  }
  if (idx >= 0) { // swap file is open
    fTabWidget->setCurrentIndex(idx);
    fileClose();
    load(swapFileName);
    fTabWidget->setCurrentIndex(currentIdx);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Starts the help content browser.
 */
void PTextEdit::helpContents()
{
  #ifdef _WIN32GCC
  QMessageBox::information(this, "**INFO**", "If a newer Qt version was available, a help window would be shown!");
  #else
  PHelp *help = new PHelp(fAdmin->getHelpUrl("main"));
  help->show();
  #endif // _WIN32GCC
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Starts the help about info box.
 */
void PTextEdit::helpAbout()
{
  PMusrEditAbout *about = new PMusrEditAbout();
  about->show();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Starts the help about Qt info box.
 */
void PTextEdit::helpAboutQt()
{
  QMessageBox::aboutQt(this);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: called when the font shall be changed.
 *
 * \param f font object
 */
void PTextEdit::fontChanged( const QFont &f )
{
  fFontChanging = true;
  fComboFont->lineEdit()->setText( f.family() );
  fComboSize->lineEdit()->setText( QString::number( f.pointSize() ) );

  if (!currentEditor())
    return;

  currentEditor()->setFont(f);
  currentEditor()->setWindowModified(false);
  currentEditor()->viewport()->setFocus();
  fFontChanging = false;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: called when the text of the document has changed. It adds a star to the tab label if the
 * document has been modified.
 *
 * \param forced
 */
void PTextEdit::textChanged(const bool forced)
{

  if (!currentEditor())
    return;

  if (fFontChanging)
    return;

  QString tabLabel = fTabWidget->tabText(fTabWidget->currentIndex());

  if ((fTabWidget->tabText(fTabWidget->currentIndex()).indexOf("*") > 0) &&
      !currentEditor()->document()->isModified()) {
    tabLabel.truncate(tabLabel.length()-1);
    fTabWidget->setTabText(fTabWidget->currentIndex(), tabLabel);
  }

  if ((fTabWidget->tabText(fTabWidget->currentIndex()).indexOf("*") < 0) &&
      currentEditor()->document()->isModified()) {
    fTabWidget->setTabText(fTabWidget->currentIndex(), tabLabel+"*");
  }

  if ((fTabWidget->tabText(fTabWidget->currentIndex()).indexOf("*") < 0) &&
      forced)
    fTabWidget->setTabText(fTabWidget->currentIndex(), tabLabel+"*");
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: called when the cursor position has changed. Updates the status bar with the current
 * cursor position.
 */
void PTextEdit::currentCursorPosition()
{
  QString str;
  int line=0, pos=0;
  
  pos = currentEditor()->textCursor().columnNumber();
  line = currentEditor()->textCursor().blockNumber();

  str = QString("cursor pos: %1, %2").arg(line+1).arg(pos+1);
  statusBar()->showMessage(str);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: handles the replaced command.
 */
void PTextEdit::replace()
{
  currentEditor()->insertPlainText(fFindReplaceData->replaceText);

  editFindNext();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: handles the repalce and close command.
 */
void PTextEdit::replaceAndClose()
{
  currentEditor()->insertPlainText(fFindReplaceData->replaceText);

  emit close();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: handles the replace all command.
 */
void PTextEdit::replaceAll()
{
  // set the cursor to the start of the document
  currentEditor()->moveCursor(QTextCursor::Start);

  // construct search flags
  QTextDocument::FindFlags flags = 0;
  if (fFindReplaceData->caseSensitive)
    flags |= QTextDocument::FindCaseSensitively;
  else if (fFindReplaceData->findBackwards)
    flags |= QTextDocument::FindBackward;
  else if (fFindReplaceData->wholeWordsOnly)
    flags |= QTextDocument::FindWholeWords;

  while (currentEditor()->find(fFindReplaceData->findText, flags)) {
    currentEditor()->insertPlainText(fFindReplaceData->replaceText);
  }

  emit close();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: updates the fonts if the textedit tab has changed.
 */
void PTextEdit::applyFontSettings(QWidget*)
{
  QFont font(fAdmin->getFontName(), fAdmin->getFontSize());
  fontChanged(font);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Called when an open file is changing outside the editor (SLOT).
 *
 * \param fileName file name of the changed file.
 */
void PTextEdit::fileChanged(const QString &fileName)
{
  if (!fFileSystemWatcherActive)
    return;

  fFileSystemWatcherActive = false;

  QString str = "File '" + fileName + "' changed on the system.\nDo you want to reload it?";
  int result = QMessageBox::question(this, "**INFO**", str, QMessageBox::Yes, QMessageBox::No);
  if (result == QMessageBox::Yes) {
    // find correct file
    int idx = -1;
    for (int i=0; i<fTabWidget->count(); i++) {
      if (fTabWidget->tabText(i) == QFileInfo(fileName).fileName()) {
        idx = i;
        break;
      }
    }

    if (idx == -1) {
      fileSystemWatcherActivation();
      return;
    }

    // remove file from file system watcher
    fFileSystemWatcher->removePath(fileName);

    // close tab
    fTabWidget->removeTab(idx);
    // load it
    load(fileName, idx);
  }

  fileSystemWatcherActivation();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>.Delayed reactivation of file system watcher, needed when saving files. At the moment the delay
 * is set to 2000 ms.
 */
void PTextEdit::fileSystemWatcherActivation()
{
  if (fFileSystemWatcherTimeout.isActive())
    return;

  fFileSystemWatcherTimeout.singleShot(2000, this, SLOT(setFileSystemWatcherActive()));
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>.slot needed to reactivate the file system watcher. It is called by PTextEdit::fileSystemWatcherActivation()
 * after some given timeout.
 */
void PTextEdit::setFileSystemWatcherActive()
{
  fFileSystemWatcherActive = true;
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
