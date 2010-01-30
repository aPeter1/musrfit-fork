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

#include <QtDebug>

#include "PTextEdit.h"
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
 * <p>
 */
PTextEdit::PTextEdit( QWidget *parent, Qt::WindowFlags f )
    : QMainWindow( parent, f )
{
  fAdmin = new PAdmin();

  fFileSystemWatcher = new QFileSystemWatcher();
  if (fFileSystemWatcher == 0) {
    QMessageBox::information(this, "**ERROR**", "Couldn't invoke QFileSystemWatcher!");
  } else {
    connect( fFileSystemWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
  }

  fMusrT0Action = 0;

  fMsr2DataParam = 0;
  fFindReplaceData = 0,

//  fFileWatcher = 0;

  fKeepMinuit2Output = false;
  fTitleFromDataFile = fAdmin->getTitleFromDataFileFlag();
  fEnableMusrT0      = fAdmin->getEnableMusrT0Flag();
  fDump = 0; // 0 = no dump, 1 = ascii dump, 2 = root dump

  setupFileActions();
  setupEditActions();
  setupTextActions();
  setupMusrActions();
  setupHelpActions();

  fTabWidget = new QTabWidget( this );
  setCentralWidget( fTabWidget );

  textFamily(fAdmin->getFontName());
  textSize(QString("%1").arg(fAdmin->getFontSize()));

  QPixmap image0(":/images/musrfit.xpm");
  setWindowIcon( image0 );

  if ( qApp->argc() != 1 ) {
    for ( int i = 1; i < qApp->argc(); ++i )
      load( qApp->argv()[ i ] );
  } else {
    fileNew();
  }

  connect( fTabWidget, SIGNAL( currentChanged(QWidget*) ), this, SLOT( applyFontSettings(QWidget*) ));
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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
 * <p>
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
 * <p>
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
  menu->addSeparator();

  a = new QAction( tr( "(Un)Co&mment" ), this );
  a->setShortcut( tr("Ctrl+M") );
  a->setStatusTip( tr("Edit (Un)Comment Selected Lines") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editComment() ) );
  menu->addAction(a);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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
 * <p>
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
  a->setStatusTip( tr("Start musrt0") );
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
 * <p>
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
 * <p>
 */
void PTextEdit::load( const QString &f, const int index )
{
  if ( !QFile::exists( f ) )
    return;

  QFileInfo info(f);

  PSubTextEdit *edit = new PSubTextEdit( fAdmin );
  edit->setFont(QFont(fAdmin->getFontName(), fAdmin->getFontSize()));

  if (index == -1)
    fTabWidget->addTab( edit, QFileInfo( f ).fileName() );
  else
    fTabWidget->insertTab( index, edit, QFileInfo( f ).fileName() );
  QFile file( f );
  if ( !file.open( QIODevice::ReadOnly ) )
    return;

  fFileSystemWatcher->addPath(f);

  QTextStream ts( &file );
  QString txt = ts.readAll();
  edit->setPlainText( txt );
  doConnections( edit );

  fTabWidget->setCurrentIndex(fTabWidget->indexOf(edit));
  edit->viewport()->setFocus();
  fFilenames.remove( edit );
  fFilenames.insert( edit, f );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PSubTextEdit *PTextEdit::currentEditor() const
{
  if ( fTabWidget->currentWidget() && fTabWidget->currentWidget()->inherits( "PSubTextEdit" ) ) {
    return (PSubTextEdit*)fTabWidget->currentWidget();
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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
 * <p>
 */
bool PTextEdit::validRunList(const QString runList)
{
  bool success = true;

  int i = 0;
  QString subStr;
  bool done = false;
  int val;
  bool ok;
  while (!done) {
    subStr = runList.section(' ', i, i);
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
 * <p>
 */
void PTextEdit::insertTitle()
{
  currentEditor()->insertTitle();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::insertParameterBlock()
{
  currentEditor()->insertParameterBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::insertTheoryBlock()
{
  currentEditor()->insertTheoryBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::insertFunctionBlock()
{
  currentEditor()->insertFunctionBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::insertAsymRunBlock()
{
  currentEditor()->insertAsymRunBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::insertSingleHistRunBlock()
{
  currentEditor()->insertSingleHistRunBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::insertNonMusrRunBlock()
{
  currentEditor()->insertNonMusrRunBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::insertCommandBlock()
{
  currentEditor()->insertCommandBlock();
}

//----------------------------------------------------------------------------------------------------
* <p>
/**
 */
void PTextEdit::insertFourierBlock()
{
  currentEditor()->insertFourierBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::insertPlotBlock()
{
  currentEditor()->insertPlotBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::insertStatisticBlock()
{
  currentEditor()->insertStatisticBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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
 * <p>
 */
void PTextEdit::fileOpen()
{
  QStringList flns = QFileDialog::getOpenFileNames( this, tr("Open msr-/mlog-File"),
                        fAdmin->getDefaultSavePath(),
                        tr( "msr-Files (*.msr);;msr-Files (*.msr *.mlog);;All Files (*)" ));

  QStringList::Iterator it = flns.begin();
  QFileInfo finfo1, finfo2;
  QString tabFln;
  bool alreadyOpen = false;

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
 * <p>
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
 * <p>
 */
void PTextEdit::fileSave()
{
  if ( !currentEditor() )
    return;

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
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::fileSaveAs()
{
  if ( !currentEditor() )
    return;

  QString fn = QFileDialog::getSaveFileName( this,
                    tr( "Save msr-/mlog-file As" ), QString::null,
                    tr( "msr-Files (*.msr *.mlog);;All Files (*)" ) );
  if ( !fn.isEmpty() ) {
    fFilenames.remove( currentEditor() );
    fFilenames.insert( currentEditor(), fn );
    fileSave();
    fTabWidget->setTabText(fTabWidget->indexOf( currentEditor() ), QFileInfo(fn).fileName());
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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
 * <p>
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
 * <p>
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
 * <p>
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
 * <p>
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
 * <p>
 */
void PTextEdit::editUndo()
{
  if ( !currentEditor() )
    return;
  currentEditor()->undo();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::editRedo()
{
  if ( !currentEditor() )
    return;
  currentEditor()->redo();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::editSelectAll()
{
  if ( !currentEditor() )
    return;
  currentEditor()->selectAll();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::editCut()
{
  if ( !currentEditor() )
    return;
  currentEditor()->cut();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::editCopy()
{
  if ( !currentEditor() )
    return;
  currentEditor()->copy();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::editPaste()
{
  if ( !currentEditor() )
    return;
  currentEditor()->paste();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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
 * <p>
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
 * <p>
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
 * <p>
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
 * <p>
 */
void PTextEdit::editComment()
{
  if ( !currentEditor() )
    return;

// NEEDS TO BE REWORKED. Qt4.6 far too different compared to Qt3.x
/*
  QString str;
  if (currentEditor()->textCursor().hasSelection()) { // selected text present
    int paraFrom, paraTo;
    int indexFrom, indexTo;
    // get selection
    currentEditor()->getSelection(&paraFrom, &indexFrom, &paraTo, &indexTo);
    // check that indexFrom == 0, if not change the selection accordingly
    if (indexFrom != 0) {
      indexFrom = 0;
      currentEditor()->setSelection(paraFrom, indexFrom, paraTo, indexTo);
    }
    // check that cursor is not in next line without selecting anything
    if (indexTo == 0) {
      paraTo--;
      indexTo++;
    }
    // check that indexTo == end of line of paraTo
    if ((indexTo != (int)currentEditor()->text(paraTo).length()) && (indexTo != 0)) {
      indexTo = currentEditor()->text(paraTo).length();
      currentEditor()->setSelection(paraFrom, indexFrom, paraTo, indexTo);
    }
    // get selection text
    str = currentEditor()->selectedText();
    // check line by line if (un)comment is needed
    QStringList strList = QStringList::split("\n", str, TRUE);
    for (QStringList::Iterator it = strList.begin(); it != strList.end(); ++it) {
      str = *it;
      if (str.trimmed().startsWith("#")) { // comment -> uncomment it
        int idx = -1;
        for (unsigned int i=0; i<(*it).length(); i++) {
          if ((*it)[i] == '#') {
            idx = i;
            break;
          }
        }
        (*it).remove(idx,1);
        if ((*it)[idx] == ' ') {
          (*it).remove(idx,1);
        }
      } else { // no comment -> comment it
        (*it).prepend("# ");
      }
    }
    str = strList.join("\n");
    currentEditor()->insert(str);
    // set the cursor position
    currentEditor()->setCursorPosition(++paraTo, 0);
  } else { // no text selected
    int para, index;
    currentEditor()->getCursorPosition(&para, &index);
    // get current text line
    str = currentEditor()->text(para);
    // check if it is a comment line or not
    if (str.trimmed().startsWith("#")) { // comment -> uncomment it
      str = currentEditor()->text(para);
      int idx = -1;
      for (unsigned int i=0; i<str.length(); i++) {
        if (str[i] == '#') {
          idx = i;
          break;
        }
      }
      str.remove(idx,1);
      if (str[idx] == ' ') {
        str.remove(idx,1);
      }
    } else { // no comment -> comment it
      if (!str.isEmpty())
        str.prepend("# ");
    }
    // select current line
    currentEditor()->setSelection(para, 0, para, currentEditor()->text(para).length());
    // insert altered text
    currentEditor()->insert(str);
    // set the cursor position
    currentEditor()->setCursorPosition(para, 0);
  }
*/
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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
 * <p>
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
 * <p>
 */
void PTextEdit::musrGetAsymmetryDefault()
{
  QString runFileName, beamline, institute, fileFormat;

  PGetDefaultDialog *dlg = new PGetDefaultDialog();

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
 * <p>
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
 * <p>
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
 * <p>
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

  PFitOutputHandler fitOutputHandler(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath(), cmd);
  fitOutputHandler.setModal(true);
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
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::musrMsr2Data()
{
/*
  if ( !currentEditor() )
    return;
*/

  if (fMsr2DataParam == 0) {
    fMsr2DataParam = new PMsr2DataParam();
    // init fMsr2DataParam
    fMsr2DataParam->firstRun = -1;
    fMsr2DataParam->lastRun  = -1;
    fMsr2DataParam->runList = QString("");
    fMsr2DataParam->runListFileName = QString("");
    fMsr2DataParam->msrFileExtension = QString("");
    fMsr2DataParam->templateRunNo = -1;
    fMsr2DataParam->dbOutputFileName = QString("");
    fMsr2DataParam->writeDbHeader = false;
    fMsr2DataParam->summaryFilePresent = false;
    fMsr2DataParam->keepMinuit2Output = fKeepMinuit2Output;
    fMsr2DataParam->writeColumnData = false;
    fMsr2DataParam->recreateDbFile = false;
    fMsr2DataParam->chainFit = true;
    fMsr2DataParam->openFilesAfterFitting = false;
    fMsr2DataParam->titleFromDataFile = fTitleFromDataFile;
    fMsr2DataParam->createMsrFileOnly = false;
    fMsr2DataParam->fitOnly = false;
  }

  PMsr2DataDialog *dlg = new PMsr2DataDialog(fMsr2DataParam);

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
        fi = runListFileName;
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
        while (!runList.section(' ', end, end).isEmpty()) {
          end++;
        }
        // first element
        if (end == 1) {
          str = "[" + runList + "]";
          cmd.append(str);
        } else {
          str = "[" + runList.section(' ', 0, 0);
          cmd.append(str);
          // middle elements
          for (i=1; i<end-1; i++) {
            cmd.append(runList.section(' ', i, i));
          }
          // last element
          str = runList.section(' ', end-1, end-1) + "]";
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

    // no summary flag?
    if (!fMsr2DataParam->summaryFilePresent)
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

    // replace msr-file title by data file title
    if (fMsr2DataParam->titleFromDataFile) {
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

    // recreate db file
    if (fMsr2DataParam->recreateDbFile) {
      if (!QFile::remove(fMsr2DataParam->dbOutputFileName)) {
        str = QString("Couldn't delete db-file '%1'. Will **NOT** proceed.").arg(fMsr2DataParam->dbOutputFileName);
        QMessageBox::critical(this, "**ERROR**", str,
           QMessageBox::Ok, QMessageBox::NoButton);
        return;
      }
    }

/*
for (unsigned int i=0; i<cmd.size(); i++) {
  cout << endl << ">> " << cmd[i].toLatin1();
}
cout << endl;
*/

    PFitOutputHandler fitOutputHandler(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath(), cmd);
    fitOutputHandler.setModal(true);
    fitOutputHandler.exec();

    // check if it is necessary to load msr-files
    if (fMsr2DataParam->openFilesAfterFitting) {
      QString fln;
      QFile *file;
      QTextStream *stream;

      switch(dlg->getRunTag()) {
        case 0: // first run / last run list
          if (fMsr2DataParam->firstRun != -1) {
            for (int i=fMsr2DataParam->firstRun; i<=fMsr2DataParam->lastRun; i++) {
              if (fMsr2DataParam->msrFileExtension.isEmpty())
                fln = QString("%1").arg(i) + ".msr";
              else
                fln = QString("%1").arg(i) + fMsr2DataParam->msrFileExtension + ".msr";

              load(fln);
            }
          }
          break;
        case 1: // run list
          end = 0;
          while (!runList.section(' ', end, end).isEmpty()) {
            end++;
          }
          for (int i=0; i<end; i++) {
            fln = runList.section(' ', i, i);
            if (fMsr2DataParam->msrFileExtension.isEmpty())
              fln += ".msr";
            else
              fln += fMsr2DataParam->msrFileExtension + ".msr";

            load(fln);
          }
          break;
        case 2: // run list file
          file = new QFile(fMsr2DataParam->runListFileName);
          if (!file->open(QIODevice::ReadOnly)) {
            str = QString("Couldn't open run list file %1, sorry.").arg(fMsr2DataParam->runListFileName);
            QMessageBox::critical(this, "**ERROR**", str.toLatin1(), QMessageBox::Ok, QMessageBox::NoButton);
            return;
          }

          stream = new QTextStream(file);
          while ( !stream->atEnd() ) {
            str = stream->readLine(); // line of text excluding '\n'
            str.trimmed();
            if (!str.isEmpty() && !str.startsWith("#")) {
              fln = str.section(' ', 0, 0);
              if (fMsr2DataParam->msrFileExtension.isEmpty())
                fln += ".msr";
              else
                fln += fMsr2DataParam->msrFileExtension + ".msr";

              load(fln);
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
    }
  }

  delete dlg;
  dlg = 0;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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

  str = fAdmin->getExecPath() + "/musrview";
  cmd = str + " \"";

  str = *fFilenames.find( currentEditor() );
  cmd += str + "\" &";

  system(cmd.toLatin1());
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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
  cmd += str + "\" &";

  system(cmd.toLatin1());

  QString fln = *fFilenames.find( currentEditor() );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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
 * <p>
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
  system(cmd.toLatin1());
  cmd = QString("cp \"") + swapFileName + QString("\" \"") + currentFileName + QString("\"");
  system(cmd.toLatin1());
  cmd = QString("cp \"") + tempFileName + QString("\" \"") + swapFileName + QString("\"");
  system(cmd.toLatin1());
  cmd = QString("rm \"") + tempFileName + QString("\"");
  system(cmd.toLatin1());

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
 * <p>
 */
void PTextEdit::helpContents()
{
  QMessageBox::information( this, "helpContents",
                            fAdmin->getHelpMain(),
                            QMessageBox::Ok );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::helpAbout()
{
  PMusrEditAbout *about = new PMusrEditAbout();
  about->show();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::helpAboutQt()
{
  QMessageBox::aboutQt(this);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::fontChanged( const QFont &f )
{
  fFontChanging = true;
  fComboFont->lineEdit()->setText( f.family() );
  fComboSize->lineEdit()->setText( QString::number( f.pointSize() ) );
  currentEditor()->setFont(f);
  currentEditor()->setWindowModified(false);
  currentEditor()->viewport()->setFocus();
  fFontChanging = false;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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
    fTabWidget->setTabText(fTabWidget->indexOf(fTabWidget), tabLabel);
  }

  if ((fTabWidget->tabText(fTabWidget->currentIndex()).indexOf("*") < 0) &&
      currentEditor()->document()->isModified())
    fTabWidget->setTabText(fTabWidget->indexOf(fTabWidget), tabLabel+"*");

  if ((fTabWidget->tabText(fTabWidget->currentIndex()).indexOf("*") < 0) &&
      forced)
    fTabWidget->setTabText(fTabWidget->indexOf(fTabWidget), tabLabel+"*");
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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
 * <p>
 */
void PTextEdit::replace()
{
  currentEditor()->insertPlainText(fFindReplaceData->replaceText);

  editFindNext();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::replaceAndClose()
{
  currentEditor()->insertPlainText(fFindReplaceData->replaceText);

  emit close();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::replaceAll()
{
// NEEDS TO BE REWRITTEN SINCE Qt4.6 IS FAR TOO DIFFERENT TO Qt3.x
/*
  int currentPara, currentIndex;
  currentEditor()->getCursorPosition(&currentPara, &currentIndex);

  currentEditor()->setCursorPosition(0,0);

  int para = 1, index = 1;
  while (currentEditor()->find(fFindReplaceData->findText,
                               fFindReplaceData->caseSensitive,
                               fFindReplaceData->wholeWordsOnly,
                               true,
                               &para, &index)) {
    // set cursor to the correct position
    currentEditor()->setCursorPosition(para, index);

    // replace the text
    currentEditor()->insert(fFindReplaceData->replaceText);

    index++;
  }

  emit close();

  currentEditor()->setCursorPosition(currentPara, currentIndex);
*/
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
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

    if (idx == -1)
      return;

    // remove file from file system watcher
    fFileSystemWatcher->removePath(fileName);

    // close tab
    fTabWidget->removeTab(idx);
    // load it
    load(fileName, idx);
  }
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
