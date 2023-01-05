/****************************************************************************

  PTextEdit.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2023 by Andreas Suter                              *
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
#include <fstream>

#include <QString>
#include <QStringList>
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
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include <QColorDialog>
#include <QPainter>
#include <QMessageBox>
#include <QDialog>
#include <QPixmap>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTextList>
#include <QProcess>
#include <QFileSystemWatcher>
#include <QDesktopServices>
#include <QUrl>
#include <QRegExp>

#include <QtDebug>

#include "PTextEdit.h"
#include "PSubTextEdit.h"
#include "PAdmin.h"
#include "PFindDialog.h"
#include "PReplaceDialog.h"
#include "PReplaceConfirmationDialog.h"
#include "PFitOutputHandler.h"
#include "PDumpOutputHandler.h"
#include "PPrefsDialog.h"
#include "PGetMusrFTOptionsDialog.h"
#include "PMusrEditAbout.h"
#include "PMsr2DataDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>Constructor
 *
 * \param parent pointer to the parent object
 * \param f qt windows flags
 */
PTextEdit::PTextEdit( QWidget *parent )
    : QMainWindow( parent )
{
  bool gotTheme = getTheme();

  // reads and manages the conents of the xml-startup (musredit_startup.xml) file
  fAdmin = new PAdmin();

  // set default setting of the fDarkMenuIconIcons only if a theme has been recognized, otherwise take the
  // one from the xml startup file.
  if (gotTheme) {
    fAdmin->setDarkThemeIconsMenuFlag(fDarkMenuIcon);
    fAdmin->setDarkThemeIconsToolbarFlag(fDarkToolBarIcon);
  } else {
    fDarkMenuIcon = fAdmin->getDarkThemeIconsMenuFlag();
    fDarkToolBarIcon = fAdmin->getDarkThemeIconsToolbarFlag();
  }

  // keep the default editor width/height
  fEditW = fAdmin->getEditWidth();
  fEditH = fAdmin->getEditHeight();

  // enable file system watcher. Needed to get notification if the msr-file is changed outside of musrfit at runtime
  fFileSystemWatcherActive = true;
  fFileSystemWatcher = new QFileSystemWatcher();
  if (fFileSystemWatcher == nullptr) {
    QMessageBox::information(this, "**ERROR**", "Couldn't invoke QFileSystemWatcher!");
  } else {
    connect( fFileSystemWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(fileChanged(const QString&)));
  }

  // initialize stuff
  fMusrT0Action = nullptr;

  fMsr2DataParam = nullptr;
  fFindReplaceData = nullptr;

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

  QString iconName("");
  if (fDarkMenuIcon)
    iconName = QString(":/icons/musrfit-dark.svg");
  else
    iconName = QString(":/icons/musrfit-plain.svg");
  setWindowIcon( QIcon( QPixmap(iconName) ) );

  // if arguments are give, try to load those files, otherwise create an empty new file
  if ( qApp->arguments().size() != 1 ) {
    for ( int i = 1; i < qApp->arguments().size(); ++i )
      load( qApp->arguments()[ i ] );
  } else {
    fileNew();
  }

  connect( fTabWidget, SIGNAL( currentChanged(int) ), this, SLOT( applyFontSettings(int) ));

  fLastDirInUse = fAdmin->getDefaultSavePath();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>This slot is called if the main application is on the way to quit. This ensures that allocated
 * memory indeed can be free'd.
 */
void PTextEdit::aboutToQuit()
{
  if (fAdmin) {
    delete fAdmin;
    fAdmin = nullptr;
  }
  if (fMusrT0Action) {
    delete fMusrT0Action;
    fMusrT0Action = nullptr;
  }
  if (fMsr2DataParam) {
    delete fMsr2DataParam;
    fMsr2DataParam = nullptr;
  }
  if (fFindReplaceData) {
    delete fFindReplaceData;
    fFindReplaceData = nullptr;
  }
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
  QString iconName("");

  // New
  if (fDarkMenuIcon)
    iconName = QString(":/icons/document-new-dark.svg");
  else
    iconName = QString(":/icons/document-new-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&New..." ), this );
  a->setShortcut( tr("Ctrl+N") );
  a->setStatusTip( tr("Create a new msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileNew() ) );
  menu->addAction(a);
  fActions["New"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/document-new-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&New..." ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( fileNew() ) );
  }
  tb->addAction(a);
  fActions["New-tb"] = a;

  // Open
  if (fDarkMenuIcon)
    iconName = QString(":/icons/document-open-dark.svg");
  else
    iconName = QString(":/icons/document-open-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Open..." ), this );
  a->setShortcut( tr("Ctrl+O") );
  a->setStatusTip( tr("Opens a msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileOpen() ) );
  menu->addAction(a);
  fActions["Open"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/document-open-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Open..." ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( fileOpen() ) );
  }
  tb->addAction(a);
  fActions["Open-tb"] = a;

  // Recent Files
  fRecentFilesMenu = menu->addMenu( tr("Recent Files") );
  for (int i=0; i<MAX_RECENT_FILES; i++) {
    fRecentFilesAction[i] = new QAction(fRecentFilesMenu);
    fRecentFilesAction[i]->setVisible(false);
    connect( fRecentFilesAction[i], SIGNAL(triggered()), this, SLOT(fileOpenRecent()));
    fRecentFilesMenu->addAction(fRecentFilesAction[i]);
  }
  fillRecentFiles();

  // Reload
  if (fDarkMenuIcon)
    iconName = QString(":/icons/view-refresh-dark.svg");
  else
    iconName = QString(":/icons/view-refresh-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "Reload..." ), this );
  a->setShortcut( tr("F5") );
  a->setStatusTip( tr("Reload msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileReload() ) );
  menu->addAction(a);
  fActions["Reload"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/view-refresh-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "Reload..." ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( fileReload() ) );
  }
  tb->addAction(a);
  fActions["Reload-tb"] = a;

  a = new QAction( tr( "Open Prefs..." ), this);
  connect( a, SIGNAL( triggered() ), this, SLOT( fileOpenPrefs() ) );
  menu->addAction(a);

  menu->addSeparator();

  // Save
  if (fDarkMenuIcon)
    iconName = QString(":/icons/document-save-dark.svg");
  else
    iconName = QString(":/icons/document-save-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Save..." ), this );
  a->setShortcut( tr("Ctrl+S") );
  a->setStatusTip( tr("Save msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileSave() ) );
  menu->addAction(a);
  fActions["Save"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/document-save-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Save..." ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( fileSave() ) );
  }
  tb->addAction(a);
  fActions["Save-tb"] = a;

  // Save As
  a = new QAction( tr( "Save &As..." ), this );
  a->setStatusTip( tr("Save msr-file As") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileSaveAs() ) );
  menu->addAction(a);

  // Save Prefs
  a = new QAction( tr( "Save Prefs..." ), this );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileSavePrefs() ) );
  menu->addAction(a);

  menu->addSeparator();

  // Print
  if (fDarkMenuIcon)
    iconName = QString(":/icons/document-print-dark.svg");
  else
    iconName = QString(":/icons/document-print-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Print..." ), this );
  a->setShortcut( tr("Ctrl+P") );
  a->setStatusTip( tr("Print msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( filePrint() ) );
  menu->addAction(a);
  fActions["Print"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/document-print-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Print..." ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( filePrint() ) );
  }
  tb->addAction(a);
  fActions["Print-tb"] = a;

  menu->addSeparator();

  // Close
  a = new QAction( tr( "&Close" ), this );
  a->setShortcut( tr("Ctrl+W") );
  a->setStatusTip( tr("Close msr-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileClose() ) );
  menu->addAction(a);

  // Close All
  a = new QAction( tr( "Close &All" ), this );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileCloseAll() ) );
  menu->addAction(a);

  // Close All Others
  a = new QAction( tr( "Clo&se All Others" ), this );
  a->setShortcut( tr("Ctrl+Shift+W") );
  a->setStatusTip( tr("Close All Other Tabs") );
  connect( a, SIGNAL( triggered() ), this, SLOT( fileCloseAllOthers() ) );
  menu->addAction(a);

  menu->addSeparator();

  // Exit
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
  QString iconName("");

  // Undo
  if (fDarkMenuIcon)
    iconName = QString(":/icons/edit-undo-dark.svg");
  else
    iconName = QString(":/icons/edit-undo-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Undo" ), this );
  a->setShortcut( tr("Ctrl+Z") );
  a->setStatusTip( tr("Edit Undo") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editUndo() ) );
  menu->addAction(a);
  fActions["Undo"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/edit-undo-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Undo" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( editUndo() ) );
  }
  tb->addAction(a);
  fActions["Undo-tb"] = a;

  // Redo
  if (fDarkMenuIcon)
    iconName = QString(":/icons/edit-redo-dark.svg");
  else
    iconName = QString(":/icons/edit-redo-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Redo" ), this );
  a->setShortcut( tr("Ctrl+Y") );
  a->setStatusTip( tr("Edit Redo") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editRedo() ) );
  menu->addAction(a);
  fActions["Redo"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/edit-redo-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Redo" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( editRedo() ) );
  }
  tb->addAction(a);
  fActions["Redo-tb"] = a;

  menu->addSeparator();

  // Select All
  a = new QAction( tr( "Select &All" ), this );
  a->setShortcut( tr("Ctrl+A") );
  a->setStatusTip( tr("Edit Select All") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editSelectAll() ) );
  menu->addAction(a);

  menu->addSeparator();
  tb->addSeparator();

  // Copy
  if (fDarkMenuIcon)
    iconName = QString(":/icons/edit-copy-dark.svg");
  else
    iconName = QString(":/icons/edit-copy-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Copy" ), this );
  a->setShortcut( tr("Ctrl+C") );
  a->setStatusTip( tr("Edit Copy") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editCopy() ) );
  menu->addAction(a);
  fActions["Copy"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/edit-copy-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Copy" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( editCopy() ) );
  }
  tb->addAction(a);
  fActions["Copy-tb"] = a;

  // Cut
  if (fDarkMenuIcon)
    iconName = QString(":/icons/edit-cut-dark.svg");
  else
    iconName = QString(":/icons/edit-cut-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "Cu&t" ), this );
  a->setShortcut( tr("Ctrl+X") );
  a->setStatusTip( tr("Edit Cut") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editCut() ) );
  menu->addAction(a);
  fActions["Cut"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/edit-cut-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "Cu&t" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( editCut() ) );
  }
  tb->addAction(a);
  fActions["Cut-tb"] = a;

  // Paste
  if (fDarkMenuIcon)
    iconName = QString(":/icons/edit-paste-dark.svg");
  else
    iconName = QString(":/icons/edit-paste-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Paste" ), this );
  a->setShortcut( tr("Ctrl+V") );
  a->setStatusTip( tr("Edit Paste") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editPaste() ) );
  menu->addAction(a);
  fActions["Paste"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/edit-paste-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Paste" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( editPaste() ) );
  }
  tb->addAction(a);
  fActions["Paste-tb"] = a;

  menu->addSeparator();
  tb->addSeparator();

  // Find
  if (fDarkMenuIcon)
    iconName = QString(":/icons/edit-find-dark.svg");
  else
    iconName = QString(":/icons/edit-find-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Find" ), this );
  a->setShortcut( tr("Ctrl+F") );
  a->setStatusTip( tr("Edit Find") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editFind() ) );
  menu->addAction(a);
  fActions["Find"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/edit-find-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Find" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( editFind() ) );
  }
  tb->addAction(a);
  fActions["Find-tb"] = a;

  // Find Next
  if (fDarkMenuIcon)
    iconName = QString(":/icons/go-next-use-dark.svg");
  else
    iconName = QString(":/icons/go-next-use-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "Find &Next" ), this );
  a->setShortcut( tr("F3") );
  a->setStatusTip( tr("Edit Find Next") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editFindNext() ) );
  menu->addAction(a);
  fActions["Find Next"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/go-next-use-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "Find &Next" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( editFindNext() ) );
  }
  tb->addAction(a);
  fActions["Find Next-tb"] = a;

  // Find Previous
  if (fDarkMenuIcon)
    iconName = QString(":/icons/go-previous-use-dark.svg");
  else
    iconName = QString(":/icons/go-previous-use-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ) , tr( "Find Pre&vious" ), this );
  a->setShortcut( tr("Shift+F4") );
  a->setStatusTip( tr("Edit Find Previous") );
  connect( a, SIGNAL( triggered() ), this, SLOT( editFindPrevious() ) );
  menu->addAction(a);
  fActions["Find Previous"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/go-previous-use-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "Find Pre&vious" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( editFindPrevious() ) );
  }
  tb->addAction(a);
  fActions["Find Previous-tb"] = a;

  // Replace
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
  if (edit == nullptr) {
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
  if (edit == nullptr) {
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
  QString iconName("");

  // musrWiz
  if (fDarkMenuIcon)
    iconName = QString(":/icons/musrWiz-32x32-dark.svg");
  else
    iconName = QString(":/icons/musrWiz-32x32.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "musr&Wiz" ), this );
  a->setShortcut( tr("Alt+W") );
  a->setStatusTip( tr("Call musrWiz which helps to create msr-files") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrWiz() ) );
  menu->addAction(a);
  fActions["musrWiz"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/musrWiz-32x32.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "musr&Wiz" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( musrWiz() ) );
  }
  tb->addAction(a);
  fActions["musrWiz-tb"] = a;

  menu->addSeparator();
  tb->addSeparator();

  // Calculate Chisq
  if (fDarkMenuIcon)
    iconName = QString(":/icons/musrchisq-dark.svg");
  else
    iconName = QString(":/icons/musrchisq-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "Calculate &Chisq" ), this );
  a->setShortcut( tr("Alt+C") );
  a->setStatusTip( tr("Calculate Chi Square (Log Max Likelihood)") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrCalcChisq() ) );
  menu->addAction(a);
  fActions["calcChisq"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/musrchisq-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "Calculate &Chisq" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( musrCalcChisq() ) );
  }
  tb->addAction(a);
  fActions["calcChisq-tb"] = a;

  // musrfit
  if (fDarkMenuIcon)
    iconName = QString(":/icons/musrfit-dark.svg");
  else
    iconName = QString(":/icons/musrfit-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Fit" ), this );
  a->setShortcut( tr("Alt+F") );
  a->setStatusTip( tr("Fit") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrFit() ) );
  menu->addAction(a);
  fActions["musrfit"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/musrfit-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Fit" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( musrFit() ) );
  }
  tb->addAction(a);
  fActions["musrfit-tb"] = a;

  // Swap Msr/Mlog
  if (fDarkMenuIcon)
    iconName = QString(":/icons/musrswap-dark.svg");
  else
    iconName = QString(":/icons/musrswap-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Swap Msr <-> Mlog" ), this );
  a->setShortcut( tr("Alt+S") );
  a->setStatusTip( tr("Swap msr-file <-> mlog-file") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrSwapMsrMlog() ) );
  menu->addAction(a);
  fActions["Swap Msr/Mlog"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/musrswap-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Swap Msr <-> Mlog" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( musrSwapMsrMlog() ) );
  }
  tb->addAction(a);
  fActions["Swap Msr/Mlog-tb"] = a;

  // musrStep
  if (fDarkMenuIcon)
    iconName = QString(":/icons/musrStep-32x32-dark.svg");
  else
    iconName = QString(":/icons/musrStep-32x32.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "Set Ste&ps" ), this );
  a->setShortcut( tr("Alt+P") );
  a->setStatusTip( tr("Set Steps") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrSetSteps() ) );
  menu->addAction(a);
  fActions["musrStep"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/musrStep-32x32.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "Set Ste&ps" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( musrSetSteps() ) );
  }
  tb->addAction(a);
  fActions["musrStep-tb"] = a;

  // msr2data
  if (fDarkMenuIcon)
    iconName = QString(":/icons/msr2data-dark.svg");
  else
    iconName = QString(":/icons/msr2data-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "Msr&2Data" ), this );
  a->setShortcut( tr("Alt+2") );
  a->setStatusTip( tr("Start msr2data interface") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrMsr2Data() ) );
  menu->addAction(a);
  fActions["msr2data"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/msr2data-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Msr2Data" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( musrMsr2Data() ) );
  }
  tb->addAction(a);
  fActions["msr2data-tb"] = a;

  // mupp
  if (fDarkMenuIcon)
    iconName = QString(":/icons/mupp-dark.svg");
  else
    iconName = QString(":/icons/mupp-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "m&upp" ), this );
  a->setShortcut( tr("Alt+U") );
  a->setStatusTip( tr("Start mupp, the muSR parameter plotter") );
  connect( a, SIGNAL( triggered() ), this, SLOT( mupp() ) );
  menu->addAction(a);
  fActions["mupp"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/mupp-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "m&upp" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( mupp() ) );
  }
  tb->addAction(a);
  fActions["mupp-tb"] = a;

  menu->addSeparator();
  tb->addSeparator();

  // musrview
  if (fDarkMenuIcon)
    iconName = QString(":/icons/musrview-dark.svg");
  else
    iconName = QString(":/icons/musrview-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&View" ), this );
  a->setShortcut( tr("Alt+V") );
  a->setStatusTip( tr("Start musrview") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrView() ) );
  menu->addAction(a);
  fActions["musrview"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/musrview-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&View" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( musrView() ) );
  }
  tb->addAction(a);
  fActions["musrview-tb"] = a;

  // musrt0
  if (fDarkMenuIcon)
    iconName = QString(":/icons/musrt0-dark.svg");
  else
    iconName = QString(":/icons/musrt0-plain.svg");
  fMusrT0Action = new QAction( QIcon( QPixmap(iconName) ), tr( "&T0" ), this );
  fMusrT0Action->setStatusTip( tr("Start musrt0") );
  connect( fMusrT0Action, SIGNAL( triggered() ), this, SLOT( musrT0() ) );
  menu->addAction(fMusrT0Action);
  fActions["musrt0"] = fMusrT0Action;
  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/musrt0-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&T0" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( musrT0() ) );
  }
  tb->addAction(fMusrT0Action);
  fMusrT0Action->setEnabled(fAdmin->getEnableMusrT0Flag());
  fActions["musrt0-tb"] = fMusrT0Action;

  // musrFT
  if (fDarkMenuIcon)
    iconName = QString(":/icons/musrFT-dark.svg");
  else
    iconName = QString(":/icons/musrFT-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "Raw Fourier" ), this );
  a->setStatusTip( tr("Start musrFT") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrFT() ) );
  menu->addAction(a);
  fActions["musrFT"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/musrFT-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "Raw Fourier" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( musrFT() ) );
  }
  tb->addAction(a);
  fActions["musrFT-tb"] = a;

  // musrprefs
  if (fDarkMenuIcon)
    iconName = QString(":/icons/musrprefs-dark.svg");
  else
    iconName = QString(":/icons/musrprefs-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Preferences" ), this );
  a->setStatusTip( tr("Show Preferences") );
  connect( a, SIGNAL( triggered() ), this, SLOT( musrPrefs() ) );
  menu->addAction(a);
  fActions["musrprefs"] = a;

  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/musrprefs-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Preferences" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( musrPrefs() ) );
  }
  tb->addAction(a);
  fActions["musrprefs-tb"] = a;

  menu->addSeparator();
  tb->addSeparator();

  // musrdump
  if (fDarkMenuIcon)
    iconName = QString(":/icons/musrdump-dark.svg");
  else
    iconName = QString(":/icons/musrdump-plain.svg");
  a = new QAction( QIcon( QPixmap(iconName)), tr( "&Dump Header"), this);
  a->setStatusTip( tr("Dumps muSR File Header Information") );
  connect( a, SIGNAL(triggered()), this, SLOT(musrDump()));
  menu->addAction(a);
  fActions["musrdump"] = a;
  if (!fDarkToolBarIcon) { // tool bar icon is not dark, even though the theme is (ubuntu)
    iconName = QString(":/icons/musrdump-plain.svg");
    a = new QAction( QIcon( QPixmap(iconName) ), tr( "&Dump Header" ), this );
    connect( a, SIGNAL( triggered() ), this, SLOT( musrDump() ) );
  }
  tb->addAction(a);
  fActions["musrdump-tb"] = a;
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

  a = new QAction(tr( "Author(s) ..." ), this );
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
 * <p>load a msr-file.
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

  // add file name to recent file names
  fAdmin->addRecentFile(f); // keep it in admin
  fillRecentFiles();        // update menu

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
      return dynamic_cast<PSubTextEdit*>(fTabWidget->currentWidget());
    }
  }

  return nullptr;
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
 * <p>Start the dialog to enter a msr-file title. See also http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#the-title
 */
void PTextEdit::insertTitle()
{
  currentEditor()->insertTitle();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file fit-parameter block. See also http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#the-fitparameter-block
 */
void PTextEdit::insertParameterBlock()
{
  currentEditor()->insertParameterBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file theory block. See also http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#the-theory-block
 */
void PTextEdit::insertTheoryBlock()
{
  currentEditor()->insertTheoryBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Insert a selected theory function. See also http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#the-theory-block
 *
 * \param a action of the selected theory function
 */
void PTextEdit::insertTheoryFunction(QAction *a)
{
  currentEditor()->insertTheoryFunction(a->text());
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file function block. See also http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#the-functions-block
 */
void PTextEdit::insertFunctionBlock()
{
  currentEditor()->insertFunctionBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file asymmetry run block. See also http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#the-run-block
 */
void PTextEdit::insertAsymRunBlock()
{
  currentEditor()->insertAsymRunBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file single histogram run block. See also http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#the-run-block
 */
void PTextEdit::insertSingleHistRunBlock()
{
  currentEditor()->insertSingleHistRunBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file nonMusr run block. See also http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#the-run-block
 */
void PTextEdit::insertNonMusrRunBlock()
{
  currentEditor()->insertNonMusrRunBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Inserts a default command block. See also http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#the-commands-block
 */
void PTextEdit::insertCommandBlock()
{
  currentEditor()->insertCommandBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file Fourier block. See also http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#the-fourier-block
 */
void PTextEdit::insertFourierBlock()
{
  currentEditor()->insertFourierBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Start the dialog to enter a msr-file plot block. See also http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#the-plot-block
 */
void PTextEdit::insertPlotBlock()
{
  currentEditor()->insertPlotBlock();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Inserts a default statistics block. See also http://lmu.web.psi.ch/musrfit/user/html/user-manual.html#the-statistic-block
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
  QFileInfo finfo1;
  bool alreadyOpen = false;
  int idx;

  // if flns are present, keep the corresponding directory
  if (flns.size() > 0) {
    finfo1.setFile(flns.at(0));
    fLastDirInUse = finfo1.absoluteFilePath();
  }

  while( it != flns.end() ) {    
    // check if the file is not already open
    finfo1.setFile(*it);
    alreadyOpen = fileAlreadyOpen(finfo1, idx);

    if (!alreadyOpen) {
      load(*it);
    } else {
      fTabWidget->setCurrentIndex(idx);
      fileReload();
    }

    ++it;
  }

  // in case there is a 1st empty tab "noname", remove it
  QString tabStr = fTabWidget->tabText(0);
  tabStr.remove('&'); // this is needed since the QTabWidget adds short-cut info as '&' to the tab name
  if (tabStr == "noname") { // has to be the first, otherwise do nothing
    fFileSystemWatcher->removePath("noname");

    delete fTabWidget->widget(0);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>This slot will open the file from the recent file list. If already open, it will reload it.
 */
void PTextEdit::fileOpenRecent()
{
  QAction *action = qobject_cast<QAction *>(sender());

  if (action) {
    // check if this file is already open and if so, switch the tab
    QFileInfo finfo1, finfo2;
    QString tabFln;
    bool alreadyOpen = false;
    QString fln = action->text();
    fln.remove('&');
    finfo1.setFile(fln);

    for (int i=0; i<fTabWidget->count(); i++) {
      tabFln = *fFilenames.find( dynamic_cast<PSubTextEdit*>(fTabWidget->widget(i)));
      finfo2.setFile(tabFln);
      if (finfo1.absoluteFilePath() == finfo2.absoluteFilePath()) {
        alreadyOpen = true;
        fTabWidget->setCurrentIndex(i);
        break;
      }
    }

    if (!alreadyOpen) {
      // make sure the file exists
      if (!finfo1.exists()) {
        QMessageBox::critical(this, "ERROR", QString("File '%1' does not exist.\nWill not do anything.").arg(fln));
        return;
      }
      load(fln);
    } else {
      fileReload();
    }

    // in case there is a 1st empty tab "noname", remove it
    fln = fTabWidget->tabText(0);
    fln.remove("&");
    if (fln == "noname") { // has to be the first, otherwise do nothing
      fFileSystemWatcher->removePath("noname");

      delete fTabWidget->widget(0);
    }
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
void PTextEdit::fileOpenPrefs()
{
  QString fln("");
  QString msg("");
  QMessageBox msgBox;
  msgBox.setText("Which Preferences do you want to open?");
  msgBox.addButton("Default", QMessageBox::AcceptRole);
  msgBox.addButton("Custom", QMessageBox::AcceptRole);
  msgBox.setStandardButtons(QMessageBox::Cancel);
  int result = msgBox.exec();
  if (result == QMessageBox::Cancel) {
    return;
  } else if (result == 0) { // default dir
    fln = fAdmin->getDefaultPrefPathName();
    msg = QString("Current Default Preferences Path-Name:\n") + fln;
    if (QMessageBox::information(this, "Info", msg, QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel)
      return;
  } else if (result == 1) { // custom dir
    fln = QFileDialog::getOpenFileName( this, tr("Open Prefs"),
                        fLastDirInUse,
                        tr( "xml-Files (*.xml);; All Files (*)" ));
  }

  if (fAdmin->loadPrefs(fln)) {
    msg = QString("Prefs from '") + fln + QString("' loaded.");
    QMessageBox::information(nullptr, "Info", msg);
  }

  // make sure that dark/plain icon scheme is properly loaded
  if (getTheme()) {
    switchMenuIcons();
    switchToolbarIcons();
  }

  if (fAdmin->getDarkThemeIconsMenuFlag() != fDarkMenuIcon) {
    fDarkMenuIcon = !fDarkMenuIcon;
    switchMenuIcons();
  }

  if (fAdmin->getDarkThemeIconsToolbarFlag() != fDarkToolBarIcon) {
    fDarkToolBarIcon = !fDarkToolBarIcon;
    switchToolbarIcons();
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
    if ( !file.open( QIODevice::WriteOnly ) ) {
      QMessageBox::critical(this, "**ERROR**", "Couldn't save the file!\nDisk full?\nNo write access?");
      return;
    }
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
  }

  fileSystemWatcherActivation(); // delayed activation of fFileSystemWatcherActive
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Will save the current preferences.
 */
void PTextEdit::fileSavePrefs()
{
  QString fln("");
  QString msg("");
  QMessageBox msgBox;
  msgBox.setText("Which Preferences do you want to open?");
  msgBox.addButton("Default", QMessageBox::AcceptRole);
  msgBox.addButton("Custom", QMessageBox::AcceptRole);
  msgBox.setStandardButtons(QMessageBox::Cancel);
  int result = msgBox.exec();
  if (result == QMessageBox::Cancel) {
    return;
  } else if (result == 0) { // default dir
    fln = fAdmin->getDefaultPrefPathName();
    msg = QString("Current Default Preferences Path-Name:\n") + fln;
    if (QMessageBox::information(this, "Info", msg, QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel)
      return;
  } else if (result == 1) { // custom dir
    fln = QFileDialog::getSaveFileName( this,
                    tr( "Save Prefs As" ), "musredit_startup.xml",
                    tr( "xml-Files (*.xml);;All Files (*)" ) );
  }

  if ( !fln.isEmpty() ) {
    fAdmin->savePrefs(fln);
    msg = QString("Prefs to '") + fln + QString("' saved.");
    QMessageBox::information(nullptr, "Info", msg);
  }
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
    int margin = static_cast<int>( (2/2.54)*dpiy ); // 2 cm margins

    // print msr-file
    QString fln = *fFilenames.find(currentEditor());
    QString header1(fln);
    QFileInfo flnInfo(fln);
    QString header2 = QString("last modified: ") + flnInfo.lastModified().toString("dd.MM.yyyy - hh:mm:ss");
    QString line("-------------------------------------");
    QStringList strList = currentEditor()->toPlainText().split("\n");
    for (QStringList::Iterator it = strList.begin(); it != strList.end(); ++it) {
      // new page needed?
      if ( margin + yPos > printer.height() - margin ) {
        printer.newPage();             // no more room on this page
        yPos = 0;                      // back to top of page
      }

      if (yPos == 0) { // print header
        font.setPointSize( 8 );
        p.setFont( font );

        p.drawText(margin, margin+yPos, printer.width(), fm.lineSpacing(),
                   Qt::TextExpandTabs | Qt::TextDontClip, header1);
        yPos += fm.lineSpacing();
        p.drawText(margin, margin+yPos, printer.width(), fm.lineSpacing(),
                   Qt::TextExpandTabs | Qt::TextDontClip, header2);
        yPos += fm.lineSpacing();
        p.drawText(margin, margin+yPos, printer.width(), fm.lineSpacing(),
                   Qt::TextExpandTabs | Qt::TextDontClip, line);
        yPos += 1.5*fm.lineSpacing();

        font.setPointSize( 10 );
        p.setFont( font );
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
  // first check if there is any tab present
  if (fTabWidget->count()==0) // no tabs present
    return;

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
  if (fFindReplaceData == nullptr) {
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

  if (fFindReplaceData == nullptr) {
    QMessageBox::critical(this, "**ERROR**", "Couldn't invoke find data structure, sorry :-(", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  PFindDialog *dlg = new PFindDialog(fFindReplaceData, currentEditor()->textCursor().hasSelection());
  if (dlg == nullptr) {
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
  dlg = nullptr;

  // try to find the search text
  if (!fFindReplaceData->fromCursor)
    currentEditor()->textCursor().setPosition(0);

  QTextDocument::FindFlags flags;
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
  QTextDocument::FindFlags flags;
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
  QTextDocument::FindFlags flags;
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
  if (fFindReplaceData == nullptr) {
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

  if (fFindReplaceData == nullptr) {
    QMessageBox::critical(this, "**ERROR**", "Couldn't invoke find&replace data structure, sorry :-(", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  PReplaceDialog *dlg = new PReplaceDialog(fFindReplaceData, currentEditor()->textCursor().hasSelection());
  if (dlg == nullptr) {
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
  dlg = nullptr;

  if (fFindReplaceData->promptOnReplace)  {
    editFindNext();

    PReplaceConfirmationDialog confirmDlg(this);

    // connect all the necessary signals/slots
    QObject::connect(confirmDlg.fReplace_pushButton, SIGNAL(clicked()), this, SLOT(replace()));
    QObject::connect(confirmDlg.fReplaceAndClose_pushButton, SIGNAL(clicked()), this, SLOT(replaceAndClose()));
    QObject::connect(confirmDlg.fReplaceAll_pushButton, SIGNAL(clicked()), this, SLOT(replaceAll()));
    QObject::connect(confirmDlg.fFindNext_pushButton, SIGNAL(clicked()), this, SLOT(editFindNext()));
    QObject::connect(this, SIGNAL(close()), &confirmDlg, SLOT(accept()));

    confirmDlg.exec();
  } else {
    replaceAll();
  }
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
 * @brief PTextEdit::musrWiz
 */
void PTextEdit::musrWiz()
{
  QString cmd = fAdmin->getExecPath() + "/musrWiz";
#if defined(Q_OS_DARWIN)
  cmd = QString("/Applications/musrWiz.app/Contents/MacOS/musrWiz");
#endif
  QString workDir = "./"; // think about it!!
  QStringList arg;
  arg << "--log";

  QProcess *proc = new QProcess(this);
  if (proc == nullptr) {
    QMessageBox::critical(nullptr, "**ERROR**", "Couldn't invoke QProcess!");
    return;
  }

  // handle return status of musrWiz
  connect(proc, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
          [=](int exitCode, QProcess::ExitStatus exitStatus){ exitStatusMusrWiz(exitCode, exitStatus); });

  // make sure that the system environment variables are properly set
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#if defined(Q_OS_DARWIN)
  env.insert("DYLD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("DYLD_LIBRARY_PATH"));
#else
  env.insert("LD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("LD_LIBRARY_PATH"));
#endif
  proc->setProcessEnvironment(env);
  proc->setWorkingDirectory(workDir);
  proc->start(cmd, arg);
  if (!proc->waitForStarted()) {
    // error handling
    QString msg(tr("Could not execute the output command: ")+cmd[0]);
    QMessageBox::critical( nullptr,
                           tr("Fatal error"),
                           msg,
                           tr("Quit") );
    return;
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

  int result = 0;
  int fittype = currentEditor()->getFitType();
  if (fAdmin->getEstimateN0Flag() && ((fittype==0) || (fittype==4)))
    result = QMessageBox::question(this, "Estimate N0 active",
                           "Do you wish a chisq/mlh evaluation with an automatic N0 estimate?");

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
  if (fAdmin->getEstimateN0Flag() && (result == QMessageBox::Yes) && ((fittype==0) || (fittype==4)))
    cmd.append("--estimateN0");
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
  if (fAdmin->getKeepMinuit2OutputFlag())
    cmd.append("--keep-mn2-output");

  // check if title of the data file should be used to replace the msr-file title
  if (fAdmin->getTitleFromDataFileFlag())
    cmd.append("--title-from-data-file");

  // check if dump files are wished
  if (fAdmin->getDumpAsciiFlag()) {
    cmd.append("--dump");
    cmd.append("ascii");
  }
  if (fAdmin->getDumpRootFlag()) {
    cmd.append("--dump");
    cmd.append("root");
  }

  // check estimate N0 flag
  if (fAdmin->getEstimateN0Flag()) {
    cmd.append("--estimateN0");
  }

  // check per-run-block-chisq flag
  if (fAdmin->getChisqPerRunBlockFlag()) {
    cmd.append("--per-run-block-chisq");
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
  if (fMsr2DataParam == nullptr) {
    fMsr2DataParam = new PMsr2DataParam();
    *fMsr2DataParam = fAdmin->getMsr2DataParam();
  }

  // init fMsr2DataParam
  fMsr2DataParam->keepMinuit2Output = fAdmin->getKeepMinuit2OutputFlag();
  fMsr2DataParam->titleFromDataFile = fAdmin->getTitleFromDataFileFlag();
  fMsr2DataParam->estimateN0 = fAdmin->getEstimateN0Flag();
  fMsr2DataParam->perRunBlockChisq = fAdmin->getChisqPerRunBlockFlag();

  PMsr2DataDialog *dlg = new PMsr2DataDialog(fMsr2DataParam, fAdmin->getHelpUrl("msr2data"));

  if (dlg == nullptr) {
    QMessageBox::critical(this, "**ERROR**", "Couldn't invoke msr2data dialog, sorry :-(", QMessageBox::Ok, QMessageBox::NoButton);
    return;
  }

  if (dlg->exec() == QDialog::Accepted) {
    QString runList;
    QString runListFileName;
    QFileInfo fi;
    QString str;
    int i, end;   
    QStringList list;
    bool ok;

    fMsr2DataParam = dlg->getMsr2DataParam();
    fAdmin->setKeepMinuit2OutputFlag(fMsr2DataParam->keepMinuit2Output);
    fAdmin->setTitleFromDataFileFlag(fMsr2DataParam->titleFromDataFile);
    fAdmin->setEstimateN0Flag(fMsr2DataParam->estimateN0);
    fAdmin->setChisqPerRunBlockFlag(fMsr2DataParam->perRunBlockChisq);

    // analyze parameters
    switch (dlg->getRunTag()) {
      case -1: // not valid
        QMessageBox::critical(this, "**ERROR**",
           "No valid run list input found :-(\nCannot do anything.",
           QMessageBox::Ok, QMessageBox::NoButton);
        return;
        break;
      case 0:  // run list
        runList = fMsr2DataParam->runList;
        break;
      case 1:  // run list file name
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
      case 1:
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

    // parameter export list
    if (!fMsr2DataParam->paramList.isEmpty()) {
      cmd.append("paramList");
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
      QStringList list = fMsr2DataParam->paramList.split(QRegExp("[(\\s|,|;)]"), QString::SkipEmptyParts);
#else
      QStringList list = fMsr2DataParam->paramList.split(QRegExp("[(\\s|,|;)]"), Qt::SkipEmptyParts);
#endif
      for (int i=0; i<list.size(); i++)
        cmd.append(list[i]);
    }

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

    // estimate N0 (makes sence for single histo and muMinus fits only). Add flag only if a fit is done
    if (fMsr2DataParam->estimateN0 && (fMsr2DataParam->fitOnly || fMsr2DataParam->templateRunNo != -1)) {
      cmd.append("-e");
    }

    // write per-run-block chisq. Add flag only if a fit is done
    if (fMsr2DataParam->perRunBlockChisq && (fMsr2DataParam->fitOnly || fMsr2DataParam->templateRunNo != -1)) {
      cmd.append("-p");
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

    // if NO tab is present use the local directory
    QString workDir = QString("./");
    if (fTabWidget->count() != 0) {
      workDir = QFileInfo(*fFilenames.find( currentEditor() )).absolutePath();
    }
    PFitOutputHandler fitOutputHandler(workDir, cmd);
    fitOutputHandler.setModal(true);
    fFileSystemWatcherActive = false;
    fitOutputHandler.exec();

    // check if it is necessary to load msr-files
    if (fMsr2DataParam->openFilesAfterFitting) {
      QString fln;
      QFile *file;
      QTextStream *stream;
      QFileInfo finfo;
      bool alreadOpen=false;
      int idx=0;

      if (!fMsr2DataParam->global) { // standard fits
        switch(dlg->getRunTag()) {
          case 0: // run list
            list = getRunList(runList, ok);
            if (!ok)
              return;
            for (int i=0; i<list.size(); i++) {
              fln = list[i];
              if (fMsr2DataParam->msrFileExtension.isEmpty())
                fln += ".msr";
              else
                fln += fMsr2DataParam->msrFileExtension + ".msr";

              workDir = QString("./");
              if (fTabWidget->count() != 0) {
                workDir = QFileInfo(*fFilenames.find( currentEditor() )).absolutePath();
              }
              finfo.setFile(workDir + "/" + fln);
              alreadOpen = fileAlreadyOpen(finfo, idx);
              if (!alreadOpen) {
                load(workDir + "/" + fln);
              } else {
                fTabWidget->setCurrentIndex(idx);
                fileReload();
              }
            }
            break;
          case 1: // run list file
            file = new QFile(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath() + "/" + fMsr2DataParam->runListFileName);
            if (!file->open(QIODevice::ReadOnly)) {
              str = QString("Couldn't open run list file %1, sorry.").arg(fMsr2DataParam->runListFileName);
              QMessageBox::critical(this, "**ERROR**", str.toLatin1(), QMessageBox::Ok, QMessageBox::NoButton);
              return;
            }

            stream = new QTextStream(file);
            while ( !stream->atEnd() ) {
              str = stream->readLine(); // line of text excluding '\n'
              str = str.trimmed();
              if (!str.isEmpty() && !str.startsWith("#") && !str.startsWith("run", Qt::CaseInsensitive)) {
                fln = str.section(' ', 0, 0, QString::SectionSkipEmpty);
                if (fMsr2DataParam->msrFileExtension.isEmpty())
                  fln += ".msr";
                else
                  fln += fMsr2DataParam->msrFileExtension + ".msr";

                workDir = QString("./");
                if (fTabWidget->count() != 0) {
                  workDir = QFileInfo(*fFilenames.find( currentEditor() )).absolutePath();
                }
                finfo.setFile(workDir + "/" + fln);
                alreadOpen = fileAlreadyOpen(finfo, idx);
                if (!alreadOpen) {
                  load(workDir + "/" + fln);
                } else {
                  fTabWidget->setCurrentIndex(idx);
                  fileReload();
                }
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
          case 0: // run list
            fln = runList.section(" ", 0, 0, QString::SectionSkipEmpty) + QString("+global") + fMsr2DataParam->msrFileExtension + QString(".msr");
            break;
          case 1: // run list file name
            file = new QFile(QFileInfo(*fFilenames.find( currentEditor() )).absolutePath() + "/" + fMsr2DataParam->runListFileName);
            if (!file->open(QIODevice::ReadOnly)) {
              str = QString("Couldn't open run list file %1, sorry.").arg(fMsr2DataParam->runListFileName);
              QMessageBox::critical(this, "**ERROR**", str.toLatin1(), QMessageBox::Ok, QMessageBox::NoButton);
              return;
            }

            stream = new QTextStream(file);
            while ( !stream->atEnd() ) {
              str = stream->readLine(); // line of text excluding '\n'
              str = str.trimmed();
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
  dlg = nullptr;

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

  QString cmd = fAdmin->getExecPath() + "/musrview";
  QString workDir = QFileInfo(*fFilenames.find( currentEditor() )).absolutePath();
  QStringList arg;
  QString str;

  // file name
  str = *fFilenames.find( currentEditor() );
  int pos = str.lastIndexOf("/");
  if (pos != -1)
    str.remove(0, pos+1);
  arg << str;

  // timeout
  str.setNum(fAdmin->getTimeout());
  arg << "--timeout" << str;

  // start with Fourier?
  if (fAdmin->getMusrviewShowFourierFlag())
    arg << "-f";

  // start with averaged data/Fourier?
  if (fAdmin->getMusrviewShowAvgFlag())
    arg << "-a";

  // check if theory shall only be calculated at the data points
  if (fAdmin->getMusrviewShowOneToOneFlag())
    arg << "-1";

  QProcess *proc = new QProcess(this);
  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(musrViewFinished(int, QProcess::ExitStatus)));

  // make sure that the system environment variables are properly set
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#if defined(Q_OS_DARWIN)
  env.insert("DYLD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("DYLD_LIBRARY_PATH"));
#else
  env.insert("LD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("LD_LIBRARY_PATH"));
#endif
  proc->setProcessEnvironment(env);
  proc->setWorkingDirectory(workDir);
  proc->start(cmd, arg);
  if (!proc->waitForStarted()) {
    // error handling
    QString msg(tr("Could not execute the output command: ")+cmd[0]);
    QMessageBox::critical( nullptr,
                           tr("Fatal error"),
                           msg,
                           tr("Quit") );
    return;
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Checks if musrview terminated with error, and if yes pop up a message box.
 * @param exitCode
 */
void PTextEdit::musrViewFinished(int exitCode, QProcess::ExitStatus)
{
  if (exitCode != 0) {
    QString msg = QString("musrview failed. Possible reasons:\n");
    msg += QString("* corrupted msr-file.\n");
    msg += QString("* cannot read data-file.\n");
    msg += QString("* many more things can go wrong.\n");
    msg += QString("Please check!");
    QMessageBox::critical(nullptr, tr("Fatal Error"), msg, tr("Quit"));
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Calls musrt0 <msr-file>.
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

  QString cmd = fAdmin->getExecPath() + "/musrt0";
  QString workDir = QFileInfo(*fFilenames.find( currentEditor() )).absolutePath();
  QStringList arg;
  QString str;

  // file name
  str = *fFilenames.find( currentEditor() );
  int pos = str.lastIndexOf("/");
  if (pos != -1)
    str.remove(0, pos+1);
  arg << str;

  // timeout
  str.setNum(fAdmin->getTimeout());
  arg << "--timeout" << str;

  QProcess *proc = new QProcess(this);

  // make sure that the system environment variables are properly set
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#if defined(Q_OS_DARWIN)
  env.insert("DYLD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("DYLD_LIBRARY_PATH"));
#else
  env.insert("LD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("LD_LIBRARY_PATH"));
#endif
  proc->setProcessEnvironment(env);
  proc->setWorkingDirectory(workDir);
  proc->start(cmd, arg);
  if (!proc->waitForStarted()) {
    // error handling
    QString msg(tr("Could not execute the output command: ")+cmd[0]);
    QMessageBox::critical( nullptr,
                           tr("Fatal error"),
                           msg,
                           tr("Quit") );
    return;
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Calls musrFT via selection/option dialog. It will ask the user if he/she wants to overwrite some
 * of the default settings.
 */
void PTextEdit::musrFT()
{

  PGetMusrFTOptionsDialog *dlg = new PGetMusrFTOptionsDialog(*fFilenames.find( currentEditor() ), fMusrFTPrevCmd, fAdmin->getHelpUrl("musrFT"));

  if (dlg == nullptr) {
    QMessageBox::critical(this, "**ERROR** musrFT", "Couldn't invoke musrFT Options Dialog.");
    return;
  }

  if (dlg->exec() == QDialog::Accepted) {
    fMusrFTPrevCmd = dlg->getMusrFTOptions();
    QProcess *proc = new QProcess(this);
    proc->setStandardOutputFile("musrFT.log");
    proc->setStandardErrorFile("musrFT.log");
    QString cmd = fAdmin->getExecPath() + "/musrFT";
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#if defined(Q_OS_DARWIN)
  env.insert("DYLD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("DYLD_LIBRARY_PATH"));
#else
  env.insert("LD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("LD_LIBRARY_PATH"));
#endif
    proc->setProcessEnvironment(env);
    proc->start(cmd, fMusrFTPrevCmd);
    if (!proc->waitForStarted()) {
      // error handling
      QString msg(tr("Could not execute the output command: ")+cmd[0]);
      QMessageBox::critical( nullptr,
                             tr("Fatal error"),
                             msg,
                             tr("Quit") );
      return;
    }
  }

  delete dlg;
  dlg = nullptr;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Calls the preferences dialog which is used to set some global options.
 */
void PTextEdit::musrPrefs()
{
  PPrefsDialog *dlg = new PPrefsDialog(fAdmin);

  if (dlg == nullptr) {
    QMessageBox::critical(this, "**ERROR** musrPrefs", "Couldn't invoke Preferences Dialog.");
    return;
  }

  if (dlg->exec() == QDialog::Accepted) {
    fAdmin->setDarkThemeIconsMenuFlag(dlg->getDarkThemeIconsMenuFlag());
    fAdmin->setDarkThemeIconsToolbarFlag(dlg->getDarkThemeIconsToolbarFlag());
    fAdmin->setMusrviewShowFourierFlag(dlg->getMusrviewShowFourierFlag());
    fAdmin->setMusrviewShowAvgFlag(dlg->getMusrviewShowAvgFlag());
    fAdmin->setMusrviewShowOneToOneFlag(dlg->getMusrviewShowOneToOneFlag());
    fAdmin->setKeepMinuit2OutputFlag(dlg->getKeepMinuit2OutputFlag());
    fAdmin->setTitleFromDataFileFlag(dlg->getTitleFromDataFileFlag());
    fAdmin->setEnableMusrT0Flag(dlg->getEnableMusrT0Flag());
    fMusrT0Action->setEnabled(fAdmin->getEnableMusrT0Flag());
    if (dlg->getDump() == 1) {
      fAdmin->setDumpAsciiFlag(true);
      fAdmin->setDumpRootFlag(false);
    } else if (dlg->getDump() == 2) {
      fAdmin->setDumpAsciiFlag(false);
      fAdmin->setDumpRootFlag(true);
    } else {
      fAdmin->setDumpAsciiFlag(false);
      fAdmin->setDumpRootFlag(false);
    }
    fAdmin->setTimeout(dlg->getTimeout());
    fAdmin->setChisqPerRunBlockFlag(dlg->getKeepRunPerBlockChisqFlag());
    fAdmin->setEstimateN0Flag(dlg->getEstimateN0Flag());
  }

  delete dlg;
  dlg = nullptr;

  // check if the dark theme menu icons flag has changed
  if (fAdmin->getDarkThemeIconsMenuFlag() != fDarkMenuIcon) {
    fDarkMenuIcon = !fDarkMenuIcon;
    switchMenuIcons();
  }
  // check if the dark theme toolbar icons flag has changed
  if (fAdmin->getDarkThemeIconsToolbarFlag() != fDarkToolBarIcon) {
    fDarkToolBarIcon = !fDarkToolBarIcon;
    switchToolbarIcons();
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PTextEdit::musrSetSteps
 */
void PTextEdit::musrSetSteps()
{
  if ( !currentEditor() )
    return;

  // make sure I have a saved msr-file to work on
  QString tabLabel = fTabWidget->tabText(fTabWidget->currentIndex());
  if (tabLabel == "noname") {
    QMessageBox::critical(this, "**ERROR**", "For musrStep a real mlog/msr-file is needed.");
    return;
  } else if (tabLabel == "noname*") {
    fileSaveAs();
  } else if (tabLabel.indexOf("*") > 0) {
    fileSave();
  }

  // fill the command queue
  QString cmd = fAdmin->getExecPath() + "/musrStep";
#if defined(Q_OS_DARWIN)
  cmd = QString("/Applications/musrStep.app/Contents/MacOS/musrStep");
#endif
  QString workDir = QFileInfo(*fFilenames.find( currentEditor() )).absolutePath();
  QStringList arg;
  QString str;

  // get file name and feed it to the command queue
  str = *fFilenames.find( currentEditor() );
  int pos = str.lastIndexOf("/");
  if (pos != -1)
    str.remove(0, pos+1);
  arg << str;

  QProcess *proc = new QProcess(this);
  if (proc == nullptr) {
    QMessageBox::critical(nullptr, "**ERROR**", "Couldn't invoke QProcess!");
    return;
  }

  // handle return status of musrStep
  connect(proc, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
          [=](int exitCode, QProcess::ExitStatus exitStatus){ exitStatusMusrSetSteps(exitCode, exitStatus); });

  // make sure that the system environment variables are properly set
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#if defined(Q_OS_DARWIN)
  env.insert("DYLD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("DYLD_LIBRARY_PATH"));
#else
  env.insert("LD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("LD_LIBRARY_PATH"));
#endif
  proc->setProcessEnvironment(env);
  proc->setWorkingDirectory(workDir);
  proc->start(cmd, arg);
  if (!proc->waitForStarted()) {
    // error handling
    QString msg(tr("Could not execute the output command: ")+cmd[0]);
    QMessageBox::critical( nullptr,
                           tr("Fatal error"),
                           msg,
                           tr("Quit") );
    return;
  }
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
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  QString tempFileName = QString("%1/.musrfit/__swap__.msr").arg(env.value("HOME"));

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

  if (QMessageBox::information(this, "INFO",
                               QString("Will now swap files: %1 <-> %2").arg(currentFileName).arg(swapFileName),
                               QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Cancel)
    return;

  // check if the file needs to be saved
  if (fTabWidget->tabText(fTabWidget->currentIndex()).indexOf("*") > 0) { // needs to be saved first
    fileSave();
  }

  // swap files

  // copy currentFile -> tempFile
  if (QFile::exists(tempFileName)) {
    if (!QFile::remove(tempFileName)) {
      QMessageBox::critical(nullptr, "ERROR", QString("failed to remove %1").arg(tempFileName));
      return;
    }
  }
  if (!QFile::copy(currentFileName, tempFileName)) {
    QMessageBox::critical(nullptr, "ERROR", QString("failed to copy %1 -> %2").arg(currentFileName).arg(tempFileName));
    return;
  }
  // copy swapFile -> currentFile
  if (!QFile::remove(currentFileName)) {
    QMessageBox::critical(nullptr, "ERROR", QString("failed to remove %1").arg(currentFileName));
    return;
  }
  if (!QFile::copy(swapFileName, currentFileName)) {
    QMessageBox::critical(nullptr, "ERROR", QString("failed to copy %1 -> %2").arg(swapFileName).arg(currentFileName));
    return;
  }
  // copy tempFile -> swapFile
  if (!QFile::remove(swapFileName)) {
    QMessageBox::critical(nullptr, "ERROR", QString("failed to remove %1").arg(swapFileName));
    return;
  }
  if (!QFile::copy(tempFileName, swapFileName)) {
    QMessageBox::critical(nullptr, "ERROR", QString("failed to copy %1 -> %2").arg(tempFileName).arg(swapFileName));
    return;
  }
  // clean up
  if (!QFile::remove(tempFileName)) {
    QMessageBox::critical(nullptr, "ERROR", QString("failed to remove %1").arg(tempFileName));
    return;
  }

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
 * <p>Dumps the run header information of a selected muSR data file.
 */
void PTextEdit::musrDump()
{
  // select a muSR data filename
  QString fileName = QFileDialog::getOpenFileName(this, tr("Select muSR Data File"), "./", tr("muSR Data Files (*.root *.bin *.msr *.nxs)"));
  if (fileName.isEmpty())
    return;

  QVector<QString> cmd;
  QString str = fAdmin->getExecPath() + "/dump_header";
  cmd.append(str);
  cmd.append("-fn");
  cmd.append(fileName);

  PDumpOutputHandler dumpOutputHandler(cmd);
  dumpOutputHandler.setModal(false);
  dumpOutputHandler.exec();
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PTextEdit::mupp
 * <p>Calls mupp, the muSR parameter plotter.
 */
void PTextEdit::mupp()
{
  QString cmd("");
  cmd = fAdmin->getExecPath() + "/mupp";
#if defined(Q_OS_DARWIN)
  cmd = QString("/Applications/mupp.app/Contents/MacOS/mupp");
#endif

  QStringList arg;

  QProcess *proc = new QProcess(this);

  QString workDir = QFileInfo(*fFilenames.find( currentEditor() )).absolutePath();

  // make sure that the system environment variables are properly set
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#if defined(Q_OS_DARWIN)
  env.insert("DYLD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("DYLD_LIBRARY_PATH"));
#else
  env.insert("LD_LIBRARY_PATH", env.value("ROOTSYS") + "/lib:" + env.value("LD_LIBRARY_PATH"));
#endif
  proc->setProcessEnvironment(env);
  proc->setWorkingDirectory(workDir);
  proc->start(cmd, arg);
  if (!proc->waitForStarted()) {
    // error handling
    QString msg(tr("Could not execute the output command: ")+cmd);
    QMessageBox::critical( nullptr,
                           tr("Fatal error"),
                           msg,
                           tr("Quit") );
    return;
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Starts the help content browser.
 */
void PTextEdit::helpContents()
{
  bool ok = QDesktopServices::openUrl(QUrl(fAdmin->getHelpUrl("main"), QUrl::TolerantMode));
  if (!ok) {
    QString msg = QString("<p>Sorry: Couldn't open default web-browser for the help.<br>Please try: <a href=\"%1\">musrfit docu</a> in your web-browser.").arg(fAdmin->getHelpUrl("main"));
    QMessageBox::critical( nullptr,
                           tr("Fatal Error"),
                           msg,
                           tr("Quit") );
  }
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
 * @brief PTextEdit::exitStatusMusrWiz
 * @param exitCode
 * @param exitStatus
 */
void PTextEdit::exitStatusMusrWiz(int exitCode, QProcess::ExitStatus exitStatus)
{
  if (exitStatus == QProcess::CrashExit) {
    QMessageBox::critical(nullptr, "**FATAL**", "musrWiz returned CrashExit.");
    return;
  }

  // dialog finished with reject
  if (exitCode == 0)
    return;

  // read .musrWiz.log
  QProcessEnvironment procEnv = QProcessEnvironment::systemEnvironment();
  QString pathName = procEnv.value("HOME", "");
  pathName += "/.musrfit/musredit/musrWiz.log";
  QString errMsg;
  std::ifstream fin(pathName.toLatin1().data(), std::ifstream::in);
  if (!fin.is_open()) {
    errMsg = QString("PTextEdit::exitStatusMusrWiz: couldn't read %1 file.").arg(pathName);
    QMessageBox::critical(nullptr, "**ERROR**", errMsg);
    return;
  }
  char line[128];
  bool musrT0tag = false;
  QString str, pathFileName("");
  while (fin.good()) {
    fin.getline(line, 128);
    str = line;
    if (str.startsWith("path-file-name:")) {
      pathFileName = str.mid(16);
    } else if (str.startsWith("musrt0-tag: yes")) {
      musrT0tag = true;
    }
  }
  fin.close();

  load(pathFileName);

  // in case there is a 1st empty tab "noname", remove it
  QString tabStr = fTabWidget->tabText(0);
  tabStr.remove('&'); // this is needed since the QTabWidget adds short-cut info as '&' to the tab name
  if (tabStr == "noname") { // has to be the first, otherwise do nothing
    fFileSystemWatcher->removePath("noname");

    delete fTabWidget->widget(0);
  }

  if (musrT0tag)
    musrT0();
  
  // remove musrWiz.log file
  QFile logFile(pathName);
  if (!logFile.remove()) {
    errMsg = QString("PTextEdit::exitStatusMusrWiz: couldn't delete %1 file.").arg(pathName);
    QMessageBox::critical(nullptr, "**ERROR**", errMsg);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PTextEdit::existStatusMusrSetSteps
 * @param exitCode
 * @param exitStatus
 */
void PTextEdit::exitStatusMusrSetSteps(int exitCode, QProcess::ExitStatus exitStatus)
{
  if (exitStatus == QProcess::CrashExit) {
    QMessageBox::critical(nullptr, "**FATAL**", "musrStep returned CrashExit.");
    return;
  }

  // dialog finished with reject
  if (exitCode == 0)
    return;

  // dialog finished with save and quite, i.e. accept, hence reload
  QString fln = *fFilenames.find( currentEditor() );
  int idx = fTabWidget->currentIndex();

  fileClose();
  load(fln, idx);
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
  QTextDocument::FindFlags flags;
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
void PTextEdit::applyFontSettings(int)
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
 * <p>Delayed reactivation of file system watcher, needed when saving files. At the moment the delay
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
 * <p>slot needed to reactivate the file system watcher. It is called by PTextEdit::fileSystemWatcherActivation()
 * after some given timeout.
 */
void PTextEdit::setFileSystemWatcherActive()
{
  fFileSystemWatcherActive = true;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PTextEdit::getTheme
 */
bool PTextEdit::getTheme()
{
  fDarkMenuIcon = false; // true if theme is dark
  fDarkToolBarIcon = false; // needed for ubuntu dark since there the menu icons are dark, however the toolbar icons are plain!

  QString str = QIcon::themeName();

  if (str.isEmpty()) { // this is ugly and eventually needs to be fixed in a more coherent way
    str = QProcessEnvironment::systemEnvironment().value("HOME", QString("??"));
    str += "/.kde4/share/config/kdeglobals";
    bool done = false;
    if (QFile::exists(str)) {
      QFile fln(str);
      fln.open(QIODevice::ReadOnly | QIODevice::Text);
      QTextStream fin(&fln);
      QString line("");
      while (!fin.atEnd() && !done) {
        line = fin.readLine();
        if (line.contains("ColorScheme")) {
          if (line.contains("dark", Qt::CaseInsensitive)) {
            fDarkMenuIcon = true;
            fDarkToolBarIcon = true;
          }
          done = true;
        }
      }
      fln.close();
    }
    return done;
  }

  if (str.contains("dark", Qt::CaseInsensitive)) {
    fDarkMenuIcon = true;
    if (str.contains("ubuntu", Qt::CaseInsensitive)) {
      fDarkMenuIcon = false;
      fDarkToolBarIcon = false;
    } else if (str.contains("xfce", Qt::CaseInsensitive)) {
      fDarkMenuIcon = false;
      fDarkToolBarIcon = false;
    } else {
      fDarkToolBarIcon = true;
    }
  }
  
  return true;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>fill the recent file list in the menu.
 */
void PTextEdit::fillRecentFiles()
{
  for (int i=0; i<fAdmin->getNumRecentFiles(); i++) {
    fRecentFilesAction[i]->setText(fAdmin->getRecentFile(i));
    fRecentFilesAction[i]->setVisible(true);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p> run list is split (space separated) and expanded (start-end -> start, start+1, ..., end) to a list
 *
 * \param runListStr list to be split and expanded
 * \param ok true if everything is fine; false if an error has been encountered
 *
 * \return fully expanded run list
 */
QStringList PTextEdit::getRunList(QString runListStr, bool &ok)
{
  QStringList result;
  bool isInt;
  QString str;

  ok = true;

  // first split space separated parts
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
  QStringList tok = runListStr.split(' ', QString::SkipEmptyParts);
#else
  QStringList tok = runListStr.split(' ', Qt::SkipEmptyParts);
#endif
  for (int i=0; i<tok.size(); i++) {
    if (tok[i].contains('-')) { // list given, hence need to expand
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
      QStringList runListTok = tok[i].split('-', QString::SkipEmptyParts);
#else
      QStringList runListTok = tok[i].split('-', Qt::SkipEmptyParts);
#endif
      if (runListTok.size() != 2) { // error
        ok = false;
        result.clear();
        return result;
      }
      int start=0, end=0;
      start = runListTok[0].toInt(&isInt);
      if (!isInt) {
        ok = false;
        result.clear();
        return result;
      }
      end = runListTok[1].toInt(&isInt);
      if (!isInt) {
        ok = false;
        result.clear();
        return result;
      }
      for (int i=start; i<=end; i++) {
        str = QString("%1").arg(i);
        result << str;
      }
    } else { // keep it
      result << tok[i];
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PTextEdit::fileAlreadyOpen
 * @param finfo
 * @param idx
 * @return
 */
bool PTextEdit::fileAlreadyOpen(QFileInfo &finfo, int &idx)
{
  bool result = false;
  QFileInfo finfo2;
  QString tabFln;

  for (int i=0; i<fTabWidget->count(); i++) {
    tabFln = *fFilenames.find( dynamic_cast<PSubTextEdit*>(fTabWidget->widget(i)));
    finfo2.setFile(tabFln);
    if (finfo.absoluteFilePath() == finfo2.absoluteFilePath()) {
      result = true;
      idx = i;
      break;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Switch menu icons according to the fAdmin settings
 */
void PTextEdit::switchMenuIcons()
{
  if (fAdmin->getDarkThemeIconsMenuFlag()) { // dark theme icons
    fActions["New"]->setIcon(QIcon(QPixmap(":/icons/document-new-dark.svg")));
    fActions["Open"]->setIcon(QIcon(QPixmap(":/icons/document-open-dark.svg")));
    fActions["Reload"]->setIcon(QIcon(QPixmap(":/icons/view-refresh-dark.svg")));
    fActions["Save"]->setIcon(QIcon(QPixmap(":/icons/document-save-dark.svg")));
    fActions["Print"]->setIcon(QIcon(QPixmap(":/icons/document-print-dark.svg")));
    fActions["Undo"]->setIcon(QIcon(QPixmap(":/icons/edit-undo-dark.svg")));
    fActions["Redo"]->setIcon(QIcon(QPixmap(":/icons/edit-redo-dark.svg")));
    fActions["Copy"]->setIcon(QIcon(QPixmap(":/icons/edit-copy-dark.svg")));
    fActions["Cut"]->setIcon(QIcon(QPixmap(":/icons/edit-cut-dark.svg")));
    fActions["Paste"]->setIcon(QIcon(QPixmap(":/icons/edit-paste-dark.svg")));
    fActions["Find"]->setIcon(QIcon(QPixmap(":/icons/edit-find-dark.svg")));
    fActions["Find Next"]->setIcon(QIcon(QPixmap(":/icons/go-next-use-dark.svg")));
    fActions["Find Previous"]->setIcon(QIcon(QPixmap(":/icons/go-previous-use-dark.svg")));
    fActions["musrWiz"]->setIcon(QIcon(QPixmap(":/icons/musrWiz-32x32-dark.svg")));
    fActions["calcChisq"]->setIcon(QIcon(QPixmap(":/icons/musrchisq-dark.svg")));
    fActions["musrfit"]->setIcon(QIcon(QPixmap(":/icons/musrfit-dark.svg")));
    fActions["Swap Msr/Mlog"]->setIcon(QIcon(QPixmap(":/icons/musrswap-dark.svg")));
    fActions["musrStep"]->setIcon(QIcon(QPixmap(":/icons/musrStep-32x32-dark.svg")));
    fActions["msr2data"]->setIcon(QIcon(QPixmap(":/icons/msr2data-dark.svg")));
    fActions["mupp"]->setIcon(QIcon(QPixmap(":/icons/mupp-dark.svg")));
    fActions["musrview"]->setIcon(QIcon(QPixmap(":/icons/musrview-dark.svg")));
    fActions["musrt0"]->setIcon(QIcon(QPixmap(":/icons/musrt0-dark.svg")));
    fActions["musrFT"]->setIcon(QIcon(QPixmap(":/icons/musrFT-dark.svg")));
    fActions["musrprefs"]->setIcon(QIcon(QPixmap(":/icons/musrprefs-dark.svg")));
    fActions["musrdump"]->setIcon(QIcon(QPixmap(":/icons/musrdump-dark.svg")));
  } else { // plain theme icons
    fActions["New"]->setIcon(QIcon(QPixmap(":/icons/document-new-plain.svg")));
    fActions["Open"]->setIcon(QIcon(QPixmap(":/icons/document-open-plain.svg")));
    fActions["Reload"]->setIcon(QIcon(QPixmap(":/icons/view-refresh-plain.svg")));
    fActions["Save"]->setIcon(QIcon(QPixmap(":/icons/document-save-plain.svg")));
    fActions["Print"]->setIcon(QIcon(QPixmap(":/icons/document-print-plain.svg")));
    fActions["Undo"]->setIcon(QIcon(QPixmap(":/icons/edit-undo-plain.svg")));
    fActions["Redo"]->setIcon(QIcon(QPixmap(":/icons/edit-redo-plain.svg")));
    fActions["Copy"]->setIcon(QIcon(QPixmap(":/icons/edit-copy-plain.svg")));
    fActions["Cut"]->setIcon(QIcon(QPixmap(":/icons/edit-cut-plain.svg")));
    fActions["Paste"]->setIcon(QIcon(QPixmap(":/icons/edit-paste-plain.svg")));
    fActions["Find"]->setIcon(QIcon(QPixmap(":/icons/edit-find-plain.svg")));
    fActions["Find Next"]->setIcon(QIcon(QPixmap(":/icons/go-next-use-plain.svg")));
    fActions["Find Previous"]->setIcon(QIcon(QPixmap(":/icons/go-previous-use-plain.svg")));
    fActions["musrWiz"]->setIcon(QIcon(QPixmap(":/icons/musrWiz-32x32.svg")));
    fActions["calcChisq"]->setIcon(QIcon(QPixmap(":/icons/musrchisq-plain.svg")));
    fActions["musrfit"]->setIcon(QIcon(QPixmap(":/icons/musrfit-plain.svg")));
    fActions["Swap Msr/Mlog"]->setIcon(QIcon(QPixmap(":/icons/musrswap-plain.svg")));
    fActions["musrStep"]->setIcon(QIcon(QPixmap(":/icons/musrStep-32x32.svg")));
    fActions["msr2data"]->setIcon(QIcon(QPixmap(":/icons/msr2data-plain.svg")));
    fActions["mupp"]->setIcon(QIcon(QPixmap(":/icons/mupp-plain.svg")));
    fActions["musrview"]->setIcon(QIcon(QPixmap(":/icons/musrview-plain.svg")));
    fActions["musrt0"]->setIcon(QIcon(QPixmap(":/icons/musrt0-plain.svg")));
    fActions["musrFT"]->setIcon(QIcon(QPixmap(":/icons/musrFT-plain.svg")));
    fActions["musrprefs"]->setIcon(QIcon(QPixmap(":/icons/musrprefs-plain.svg")));
    fActions["musrdump"]->setIcon(QIcon(QPixmap(":/icons/musrdump-plain.svg")));
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Switch toolbar icons according to the fAdmin settings
 */
void PTextEdit::switchToolbarIcons()
{
  if (fAdmin->getDarkThemeIconsToolbarFlag()) { // dark theme icons
    fActions["New-tb"]->setIcon(QIcon(QPixmap(":/icons/document-new-dark.svg")));
    fActions["Open-tb"]->setIcon(QIcon(QPixmap(":/icons/document-open-dark.svg")));
    fActions["Reload-tb"]->setIcon(QIcon(QPixmap(":/icons/view-refresh-dark.svg")));
    fActions["Save-tb"]->setIcon(QIcon(QPixmap(":/icons/document-save-dark.svg")));
    fActions["Print-tb"]->setIcon(QIcon(QPixmap(":/icons/document-print-dark.svg")));
    fActions["Undo-tb"]->setIcon(QIcon(QPixmap(":/icons/edit-undo-dark.svg")));
    fActions["Redo-tb"]->setIcon(QIcon(QPixmap(":/icons/edit-redo-dark.svg")));
    fActions["Copy-tb"]->setIcon(QIcon(QPixmap(":/icons/edit-copy-dark.svg")));
    fActions["Cut-tb"]->setIcon(QIcon(QPixmap(":/icons/edit-cut-dark.svg")));
    fActions["Paste-tb"]->setIcon(QIcon(QPixmap(":/icons/edit-paste-dark.svg")));
    fActions["Find-tb"]->setIcon(QIcon(QPixmap(":/icons/edit-find-dark.svg")));
    fActions["Find Next-tb"]->setIcon(QIcon(QPixmap(":/icons/go-next-use-dark.svg")));
    fActions["Find Previous-tb"]->setIcon(QIcon(QPixmap(":/icons/go-previous-use-dark.svg")));
    fActions["musrWiz-tb"]->setIcon(QIcon(QPixmap(":/icons/musrWiz-32x32-dark.svg")));
    fActions["calcChisq-tb"]->setIcon(QIcon(QPixmap(":/icons/musrchisq-dark.svg")));
    fActions["musrfit-tb"]->setIcon(QIcon(QPixmap(":/icons/musrfit-dark.svg")));
    fActions["Swap Msr/Mlog-tb"]->setIcon(QIcon(QPixmap(":/icons/musrswap-dark.svg")));
    fActions["musrStep-tb"]->setIcon(QIcon(QPixmap(":/icons/musrStep-32x32-dark.svg")));
    fActions["msr2data-tb"]->setIcon(QIcon(QPixmap(":/icons/msr2data-dark.svg")));
    fActions["mupp-tb"]->setIcon(QIcon(QPixmap(":/icons/mupp-dark.svg")));
    fActions["musrview-tb"]->setIcon(QIcon(QPixmap(":/icons/musrview-dark.svg")));
    fActions["musrt0-tb"]->setIcon(QIcon(QPixmap(":/icons/musrt0-dark.svg")));
    fActions["musrFT-tb"]->setIcon(QIcon(QPixmap(":/icons/musrFT-dark.svg")));
    fActions["musrprefs-tb"]->setIcon(QIcon(QPixmap(":/icons/musrprefs-dark.svg")));
    fActions["musrdump-tb"]->setIcon(QIcon(QPixmap(":/icons/musrdump-dark.svg")));
  } else { // plain theme icons
    fActions["New-tb"]->setIcon(QIcon(QPixmap(":/icons/document-new-plain.svg")));
    fActions["Open-tb"]->setIcon(QIcon(QPixmap(":/icons/document-open-plain.svg")));
    fActions["Reload-tb"]->setIcon(QIcon(QPixmap(":/icons/view-refresh-plain.svg")));
    fActions["Save-tb"]->setIcon(QIcon(QPixmap(":/icons/document-save-plain.svg")));
    fActions["Print-tb"]->setIcon(QIcon(QPixmap(":/icons/document-print-plain.svg")));
    fActions["Undo-tb"]->setIcon(QIcon(QPixmap(":/icons/edit-undo-plain.svg")));
    fActions["Redo-tb"]->setIcon(QIcon(QPixmap(":/icons/edit-redo-plain.svg")));
    fActions["Copy-tb"]->setIcon(QIcon(QPixmap(":/icons/edit-copy-plain.svg")));
    fActions["Cut-tb"]->setIcon(QIcon(QPixmap(":/icons/edit-cut-plain.svg")));
    fActions["Paste-tb"]->setIcon(QIcon(QPixmap(":/icons/edit-paste-plain.svg")));
    fActions["Find-tb"]->setIcon(QIcon(QPixmap(":/icons/edit-find-plain.svg")));
    fActions["Find Next-tb"]->setIcon(QIcon(QPixmap(":/icons/go-next-use-plain.svg")));
    fActions["Find Previous-tb"]->setIcon(QIcon(QPixmap(":/icons/go-previous-use-plain.svg")));
    fActions["musrWiz-tb"]->setIcon(QIcon(QPixmap(":/icons/musrWiz-32x32.svg")));
    fActions["calcChisq-tb"]->setIcon(QIcon(QPixmap(":/icons/musrchisq-plain.svg")));
    fActions["musrfit-tb"]->setIcon(QIcon(QPixmap(":/icons/musrfit-plain.svg")));
    fActions["Swap Msr/Mlog-tb"]->setIcon(QIcon(QPixmap(":/icons/musrswap-plain.svg")));
    fActions["musrStep-tb"]->setIcon(QIcon(QPixmap(":/icons/musrStep-32x32.svg")));
    fActions["msr2data-tb"]->setIcon(QIcon(QPixmap(":/icons/msr2data-plain.svg")));
    fActions["mupp-tb"]->setIcon(QIcon(QPixmap(":/icons/mupp-plain.svg")));
    fActions["musrview-tb"]->setIcon(QIcon(QPixmap(":/icons/musrview-plain.svg")));
    fActions["musrt0-tb"]->setIcon(QIcon(QPixmap(":/icons/musrt0-plain.svg")));
    fActions["musrFT-tb"]->setIcon(QIcon(QPixmap(":/icons/musrFT-plain.svg")));
    fActions["musrprefs-tb"]->setIcon(QIcon(QPixmap(":/icons/musrprefs-plain.svg")));
    fActions["musrdump-tb"]->setIcon(QIcon(QPixmap(":/icons/musrdump-plain.svg")));
  }
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
