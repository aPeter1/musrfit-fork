/****************************************************************************

  PTextEdit.cpp

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

#include <iostream>
using namespace std;

#include <qtextedit.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qtabwidget.h>
#include <qapplication.h>
#include <qfontdatabase.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>
#include <qcolordialog.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qdialog.h>
#include <qvaluevector.h>
#include <qimage.h>
#include <qpixmap.h>

#include "PTextEdit.h"
#include "PSubTextEdit.h"
#include "PAdmin.h"
#include "PFitOutputHandler.h"
#include "PPrefsDialog.h"
#include "PGetDefaultDialog.h"
#include "forms/PMusrGuiAbout.h"
#include "PMlog2DbDialog.h"

/* XPM */
static const char * const musrfit_xpm[] = {
"22 22 3 1",
" 	c None",
".	c #FF0000",
"+	c #000000",
"     ...              ",
"    +... .            ",
"  .+ ...+.            ",
" ...  . ...           ",
" ...    ...           ",
" ...    ...           ",
"+ .      .+           ",
"         .+           ",
"           .          ",
"          ...         ",
"          ...     .   ",
"          ...+    .   ",
"           .  +  ...  ",
"            ...++...  ",
"++++ + +++++...  ...  ",
"+    +   +  ...   .+  ",
"+    +   +          . ",
"++++ +   +         ...",
"+    +   +         ...",
"+    +   +         ...",
"+    +   +          . ",
"                      "};

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PTextEdit::PTextEdit( QWidget *parent, const char *name )
    : QMainWindow( parent, name )
{
  fAdmin = new PAdmin();

  fKeepMinuit2Output = false;
  fDump = 0; // 0 = no dump, 1 = ascii dump, 2 = root dump

  setupFileActions();
  setupEditActions();
  setupTextActions();
  setupMusrActions();
  setupHelpActions();

  fTabWidget = new QTabWidget( this );
  setCentralWidget( fTabWidget );

  textFamily("Courier");
  textSize("11"); // 11pt

  QImage img(musrfit_xpm);
  QPixmap image0 = img;
  setIcon( image0 );

  if ( qApp->argc() != 1 ) {
    for ( int i = 1; i < qApp->argc(); ++i )
      load( qApp->argv()[ i ] );
  } else {
    fileNew();
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::setupFileActions()
{
  QToolBar *tb = new QToolBar( this );
  tb->setLabel( "File Actions" );
  QPopupMenu *menu = new QPopupMenu( this );
  menuBar()->insertItem( tr( "&File" ), menu );

  QAction *a;
  a = new QAction( QPixmap::fromMimeSource( "filenew.xpm" ), tr( "&New..." ), CTRL + Key_N, this, "fileNew" );
  connect( a, SIGNAL( activated() ), this, SLOT( fileNew() ) );
  a->addTo( tb );
  a->addTo( menu );
  a = new QAction( QPixmap::fromMimeSource( "fileopen.xpm" ), tr( "&Open..." ), CTRL + Key_O, this, "fileOpen" );
  connect( a, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
  a->addTo( tb );
  a->addTo( menu );
  menu->insertSeparator();
  a = new QAction( QPixmap::fromMimeSource( "filesave.xpm" ), tr( "&Save..." ), CTRL + Key_S, this, "fileSave" );
  connect( a, SIGNAL( activated() ), this, SLOT( fileSave() ) );
  a->addTo( tb );
  a->addTo( menu );
  a = new QAction( tr( "Save &As..." ), 0, this, "fileSaveAs" );
  connect( a, SIGNAL( activated() ), this, SLOT( fileSaveAs() ) );
  a->addTo( menu );
  menu->insertSeparator();
  a = new QAction( QPixmap::fromMimeSource( "fileprint.xpm" ), tr( "&Print..." ), CTRL + Key_P, this, "filePrint" );
  connect( a, SIGNAL( activated() ), this, SLOT( filePrint() ) );
  a->addTo( tb );
  a->addTo( menu );
  a = new QAction( tr( "&Close" ), CTRL + Key_W, this, "fileClose" );
  connect( a, SIGNAL( activated() ), this, SLOT( fileClose() ) );
  a->addTo( menu );
  a = new QAction( tr( "E&xit" ), CTRL + Key_Q, this, "fileExit" );
  connect( a, SIGNAL( activated() ), this, SLOT( fileExit() ) );
  a->addTo( menu );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::setupEditActions()
{
  QToolBar *tb = new QToolBar( this );
  tb->setLabel( "Edit Actions" );
  QPopupMenu *menu = new QPopupMenu( this );
  menuBar()->insertItem( tr( "&Edit" ), menu );

  QAction *a;
  a = new QAction( QPixmap::fromMimeSource( "editundo.xpm" ), tr( "&Undo" ), CTRL + Key_Z, this, "editUndo" );
  connect( a, SIGNAL( activated() ), this, SLOT( editUndo() ) );
  a->addTo( tb );
  a->addTo( menu );
  a = new QAction( QPixmap::fromMimeSource( "editredo.xpm" ), tr( "&Redo" ), CTRL + Key_Y, this, "editRedo" );
  connect( a, SIGNAL( activated() ), this, SLOT( editRedo() ) );
  a->addTo( tb );
  a->addTo( menu );
  menu->insertSeparator();
  a = new QAction( tr( "Select &All" ), CTRL + Key_A, this, "editSelectAll" );
  connect( a, SIGNAL( activated() ), this, SLOT( editSelectAll() ) );
  a->addTo( menu );
  menu->insertSeparator();
  a = new QAction( QPixmap::fromMimeSource( "editcopy.xpm" ), tr( "&Copy" ), CTRL + Key_C, this, "editCopy" );
  connect( a, SIGNAL( activated() ), this, SLOT( editCopy() ) );
  a->addTo( tb );
  a->addTo( menu );
  a = new QAction( QPixmap::fromMimeSource( "editcut.xpm" ), tr( "Cu&t" ), CTRL + Key_X, this, "editCut" );
  connect( a, SIGNAL( activated() ), this, SLOT( editCut() ) );
  a->addTo( tb );
  a->addTo( menu );
  a = new QAction( QPixmap::fromMimeSource( "editpaste.xpm" ), tr( "&Paste" ), CTRL + Key_V, this, "editPaste" );
  connect( a, SIGNAL( activated() ), this, SLOT( editPaste() ) );
  a->addTo( tb );
  a->addTo( menu );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::setupTextActions()
{
  QToolBar *tb = new QToolBar( this );
  tb->setLabel( "Format Actions" );

  fComboFont = new QComboBox( TRUE, tb );
  QFontDatabase db;
  fComboFont->insertStringList( db.families() );
  connect( fComboFont, SIGNAL( activated( const QString & ) ),
           this, SLOT( textFamily( const QString & ) ) );
  fComboFont->lineEdit()->setText( "Courier" );

  fComboSize = new QComboBox( TRUE, tb );
  QValueList<int> sizes = db.standardSizes();
  QValueList<int>::Iterator it = sizes.begin();
  for ( ; it != sizes.end(); ++it )
    fComboSize->insertItem( QString::number( *it ) );
  connect( fComboSize, SIGNAL( activated( const QString & ) ),
           this, SLOT( textSize( const QString & ) ) );
  fComboSize->lineEdit()->setText( "11" ); // 11pt font size
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::setupMusrActions()
{
  QToolBar *tb = new QToolBar( this );
  tb->setLabel( "Musr Actions" );
  QPopupMenu *menu = new QPopupMenu( this );
  menuBar()->insertItem( tr( "&MusrFit" ), menu );

  QAction *a;
  a = new QAction( QPixmap::fromMimeSource( "musrasym.xpm" ), tr( "&Asymmetry Default" ), ALT + Key_A, this, "musrGetAsymmetryDefault" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrGetAsymmetryDefault() ) );
  a->addTo( tb );
  a->addTo( menu );

  a = new QAction( QPixmap::fromMimeSource( "musrsinglehisto.xpm" ), tr( "Single &Histo Default" ), ALT + Key_H, this, "musrGetSinglHistoDefault" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrGetSingleHistoDefault() ) );
  a->addTo( tb );
  a->addTo( menu );

  menu->insertSeparator();

  a = new QAction( QPixmap::fromMimeSource( "musrcalcchisq.xpm" ), tr( "Calc Chisq" ), ALT + Key_C, this, "cacluates for the given parameters chiSq/maxLH" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrCalcChisq() ) );
  a->addTo( tb );
  a->addTo( menu );

  a = new QAction( QPixmap::fromMimeSource( "musrfit.xpm" ), tr( "&Fit" ), ALT + Key_F, this, "musrFit" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrFit() ) );
  a->addTo( tb );
  a->addTo( menu );

  a = new QAction( QPixmap::fromMimeSource( "musrswap.xpm" ), tr( "&Swap Msr <-> Mlog" ), ALT + Key_S, this, "musrSwapMsrMlog" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrSwapMsrMlog() ) );
  a->addTo( tb );
  a->addTo( menu );

  a = new QAction( QPixmap::fromMimeSource( "musrmlog2db.xpm" ), tr( "&Mlog2dB" ), ALT + Key_M, this, "musrMlog2Db" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrMlog2Db() ) );
  a->addTo( tb );
  a->addTo( menu );

  menu->insertSeparator();

  a = new QAction( QPixmap::fromMimeSource( "musrview.xpm" ), tr( "&View" ), ALT + Key_V, this, "musrView" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrView() ) );
  a->addTo( tb );
  a->addTo( menu );

  a = new QAction( QPixmap::fromMimeSource( "musrt0.xpm" ), tr( "&T0" ), 0, this, "musrT0" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrT0() ) );
  a->addTo( tb );
  a->addTo( menu );

  a = new QAction( QPixmap::fromMimeSource( "musrprefs.xpm" ), tr( "&Preferences" ), 0, this, "musrPrefs" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrPrefs() ) );
  a->addTo( tb );
  a->addTo( menu );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::setupHelpActions()
{
  QPopupMenu *menu = new QPopupMenu( this );
  menuBar()->insertItem( tr( "&Help" ), menu);

  QAction *a;
  a = new QAction(tr( "Contents ..." ), 0, this, "help contents");
  connect( a, SIGNAL( activated() ), this, SLOT( helpContents() ));
  a->addTo( menu );

  a = new QAction(tr( "About ..." ), 0, this, "about");
  connect( a, SIGNAL( activated() ), this, SLOT( helpAbout() ));
  a->addTo( menu );

  a = new QAction(tr( "About Qt..." ), 0, this, "about Qt");
  connect( a, SIGNAL( activated() ), this, SLOT( helpAboutQt() ));
  a->addTo( menu );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::load( const QString &f )
{
  if ( !QFile::exists( f ) )
    return;
  PSubTextEdit *edit = new PSubTextEdit( fAdmin );
  edit->setTextFormat( PlainText );
  edit->setFamily("Courier");
  edit->setPointSize(11); // 11pt
  edit->setFont(QFont("Courier", 11));
  fTabWidget->addTab( edit, QFileInfo( f ).fileName() );
  QFile file( f );
  if ( !file.open( IO_ReadOnly ) )
    return;
  QTextStream ts( &file );
  QString txt = ts.read();
  edit->setText( txt );
  doConnections( edit );
  fTabWidget->showPage( edit );
  edit->viewport()->setFocus();
  fFilenames.replace( edit, f );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PSubTextEdit *PTextEdit::currentEditor() const
{
  if ( fTabWidget->currentPage() && fTabWidget->currentPage()->inherits( "PSubTextEdit" ) )
    return (PSubTextEdit*)fTabWidget->currentPage();
  return 0;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::doConnections( PSubTextEdit *e )
{
  connect( e, SIGNAL( currentFontChanged( const QFont & ) ),
           this, SLOT( fontChanged( const QFont & ) ) );

  connect( e, SIGNAL( textChanged() ), this, SLOT( textChanged() ));
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
void PTextEdit::fileNew()
{
  PSubTextEdit *edit = new PSubTextEdit( fAdmin );
  edit->setTextFormat( PlainText );
  edit->setFamily("Courier");
  edit->setPointSize(11); // 11pt
  doConnections( edit );
  fTabWidget->addTab( edit, tr( "noname" ) );
  fTabWidget->showPage( edit );
  edit->viewport()->setFocus();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::fileOpen()
{
  QStringList flns = QFileDialog::getOpenFileNames( 
                        tr( "msr-Files (*.msr *.mlog);;All Files (*)" ),
                        tr( fAdmin->getDefaultSavePath() ), this);

  QStringList::Iterator it = flns.begin();
  while( it != flns.end() ) {
    load(*it);
    ++it;
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

  if ( fFilenames.find( currentEditor() ) == fFilenames.end() ) {
    fileSaveAs();
  } else {
    QFile file( *fFilenames.find( currentEditor() ) );
    if ( !file.open( IO_WriteOnly ) )
      return;
    QTextStream ts( &file );
    ts << currentEditor()->text();

    // remove trailing '*' modification indicators
    QString fln = *fFilenames.find( currentEditor() );
    fTabWidget->setTabLabel(fTabWidget->currentPage(), QFileInfo(fln).fileName());
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

  QString fn = QFileDialog::getSaveFileName( QString::null, tr( "msr-Files (*.msr *.mlog);;All Files (*)" ), this );
  if ( !fn.isEmpty() ) {
    fFilenames.replace( currentEditor(), fn );
    fileSave();
    fTabWidget->setTabLabel( currentEditor(), QFileInfo( fn ).fileName() );
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
  printer.setFullPage(TRUE);
  if ( printer.setup( this ) ) {
    QPainter p( &printer );
    // Check that there is a valid device to print to.
    if ( !p.device() ) return;
    QPaintDeviceMetrics metrics( p.device() );
    int dpiy = metrics.logicalDpiY();
    int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
    QRect view( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
    QFont font( currentEditor()->QWidget::font() );
    font.setPointSize( 10 ); // we define 10pt to be a nice base size for printing

    QSimpleRichText richText( currentEditor()->text(), font,
                              currentEditor()->context(),
                              currentEditor()->styleSheet(),
                              currentEditor()->mimeSourceFactory(),
                              view.height() );
    richText.setWidth( &p, view.width() );
    int page = 1;
    do {
      richText.draw( &p, margin, margin, view, colorGroup() );
      view.moveBy( 0, view.height() );
      p.translate( 0 , -view.height() );
      p.setFont( font );
      p.drawText( view.right() - p.fontMetrics().width( QString::number( page ) ),
                  view.bottom() + p.fontMetrics().ascent() + 5, QString::number( page ) );
      if ( view.top() - margin >= richText.height() )
        break;
      printer.newPage();
      page++;
    } while (TRUE);
  }
#endif
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::fileClose()
{
  // check if the has modification
  int idx = fTabWidget->currentPageIndex();
  if (fTabWidget->label(idx).find("*")>0) {
    int result = QMessageBox::warning(this, "**WARNING**", 
                   "Do you really want to close this file.\nChanges will be lost",
                   "Exit", "Cancel");
    if (result == 1) // Cancel
      return;
  }

  delete currentEditor();
  if ( currentEditor() )
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
    if (fTabWidget->label(i).find("*") > 0) {
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
void PTextEdit::textFamily( const QString &f )
{
  if ( !currentEditor() )
    return;
  currentEditor()->setFamily( f );
  currentEditor()->viewport()->setFocus();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::textSize( const QString &p )
{
  if ( !currentEditor() )
    return;
  currentEditor()->setPointSize( p.toInt() );
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
  // set defaults
  dlg->fBeamline_lineEdit->setText(fAdmin->getBeamline());

  for (int i=0; i<dlg->fInstitute_comboBox->count(); i++) {
    if (dlg->fInstitute_comboBox->text(i).lower() == fAdmin->getInstitute().lower()) {
      dlg->fInstitute_comboBox->setCurrentItem(i);
      break;
    }
  }

  for (int i=0; i<dlg->fFileFormat_comboBox->count(); i++) {
    if (dlg->fFileFormat_comboBox->text(i).lower() == fAdmin->getFileFormat().lower()) {
      dlg->fFileFormat_comboBox->setCurrentItem(i);
      break;
    }
  }
  dlg->exec();

  if (dlg->result() != QDialog::Accepted) {
    delete dlg;
    return;
  }

  runFileName = dlg->getRunFileName();
  beamline    = dlg->getBeamline();
  institute   = dlg->getInstitute();
  fileFormat  = dlg->getFileFormat();
  delete dlg;

  QFile file(fAdmin->getMsrDefaultFilePath()+"/asymDefault.msr");
  if (file.open(IO_ReadOnly)) {
    // make a new file tab
    fileNew();
    QTextStream ts( &file );
    QString line;
    while ( !ts.atEnd() ) {
      line = ts.readLine(); // line of text excluding '\n'
      if (line.startsWith("RUN")) {
        QString runHeader = "RUN " + runFileName + " " + beamline.upper() + " " + institute + " " + fileFormat.upper() + " (name beamline institute data-file-format)\n";
        currentEditor()->insert(runHeader);
      } else { // just copy the text
        currentEditor()->insert(line+"\n");
      }
    }
    currentEditor()->setContentsPos(0, 0);

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
  // set defaults
  dlg->fBeamline_lineEdit->setText(fAdmin->getBeamline());

  for (int i=0; i<dlg->fInstitute_comboBox->count(); i++) {
    if (dlg->fInstitute_comboBox->text(i).lower() == fAdmin->getInstitute().lower()) {
      dlg->fInstitute_comboBox->setCurrentItem(i);
      break;
    }
  }

  for (int i=0; i<dlg->fFileFormat_comboBox->count(); i++) {
    if (dlg->fFileFormat_comboBox->text(i).lower() == fAdmin->getFileFormat().lower()) {
      dlg->fFileFormat_comboBox->setCurrentItem(i);
      break;
    }
  }
  dlg->exec();

  if (dlg->result() != QDialog::Accepted) {
    delete dlg;
    return;
  }

  runFileName = dlg->getRunFileName();
  beamline    = dlg->getBeamline();
  institute   = dlg->getInstitute();
  fileFormat  = dlg->getFileFormat();
  delete dlg;

  QFile file(fAdmin->getMsrDefaultFilePath()+"/singleHistoDefault.msr");
  if (file.open(IO_ReadOnly)) {
    // make a new file tab
    fileNew();
    QTextStream ts( &file );
    QString line;
    while ( !ts.atEnd() ) {
      line = ts.readLine(); // line of text excluding '\n'
      if (line.startsWith("RUN")) {
        QString runHeader = "RUN " + runFileName + " " + beamline.upper() + " " + institute + " " + fileFormat.upper() + " (name beamline institute data-file-format)\n";
        currentEditor()->insert(runHeader);
      } else { // just copy the text
        currentEditor()->insert(line+"\n");
      }
    }
    currentEditor()->setContentsPos(0, 0);

    file.close();
  } else {
    QMessageBox::critical(this, "**ERROR**",
       "Couldn't find default single histo file template :-(",
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

  QString tabLabel = fTabWidget->label(fTabWidget->currentPageIndex());
  if (tabLabel == "noname") {
    QMessageBox::critical(this, "**ERROR**", "For a fit a real msr-file is needed.");
    return;
  } else if (tabLabel == "noname*") {
    fileSaveAs();
  } else if (tabLabel.find("*") > 0) {
    fileSave();
  }

  QValueVector<QString> cmd;
  QString str;
  str = fAdmin->getExecPath() + "/musrfit";

  cmd.append(str);
  cmd.append(*fFilenames.find( currentEditor()));
  cmd.append("--chisq-only");
  PFitOutputHandler fitOutputHandler(cmd);
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

  QString tabLabel = fTabWidget->label(fTabWidget->currentPageIndex());
  if (tabLabel == "noname") {
    QMessageBox::critical(this, "**ERROR**", "For a fit a real msr-file is needed.");
    return;
  } else if (tabLabel == "noname*") {
    fileSaveAs();
  } else if (tabLabel.find("*") > 0) {
    fileSave();
  }

  QValueVector<QString> cmd;
  QString str;
  str = fAdmin->getExecPath() + "/musrfit";

  cmd.append(str);
  cmd.append(*fFilenames.find( currentEditor()));

  // check if keep minuit2 output is wished
  if (fKeepMinuit2Output)
    cmd.append("--keep-mn2-output");

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

  PFitOutputHandler fitOutputHandler(cmd);
  fitOutputHandler.setModal(true);
  fitOutputHandler.exec();

  musrSwapMsrMlog();
/*
  if (fOpenMlogAfterFit) {
    // get current file name and replace the msr-extension through mlog
    str = fTabWidget->label(fTabWidget->currentPageIndex());
    int idx = str.find(".msr");
    if (idx > 0) {
      str.replace(idx, 4, ".mlog");
    }

    // check if mlog-file is already open, and if yes close it
    for (int i=0; i<fTabWidget->count(); i++) {
      if (fTabWidget->label(i).find(str) >= 0) {
        delete fTabWidget->page(i);
      }
    }

    // open mlog files
    load(str);
  }
*/
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::musrMlog2Db()
{
  if ( !currentEditor() )
    return;

  PMlog2DbDialog *dlg = new PMlog2DbDialog(fKeepMinuit2Output);

  if (dlg->exec() == QDialog::Accepted) {
    QString first, last;
    QString runList;
    QString runListFileName;
    QFileInfo fi;
    QString str;
    int i, end;

    // analyze parameters
    switch (dlg->getRunTag()) {
      case -1: // not valid
        QMessageBox::critical(this, "**ERROR**",
           "No valid run list input found :-(\nCannot do anything.",
           QMessageBox::Ok, QMessageBox::NoButton);
        return;
        break;
      case 0:  // first last
        first = dlg->getFirstRunNo();
        last  = dlg->getLastRunNo();
        if (first.isEmpty() || last.isEmpty()) {
          QMessageBox::critical(this, "**ERROR**",
            "If you choose the first/last option,\nfirst AND last needs to be provided.",
            QMessageBox::Ok, QMessageBox::NoButton);
          return;
        }
        break;
      case 1:  // run list
        runList = dlg->getRunList();
        if (!validRunList(runList)) {
          QMessageBox::critical(this, "**ERROR**",
            "Invalid Run List!\nThe run list needs to be a space separated list of run numbers.",
            QMessageBox::Ok, QMessageBox::NoButton);
          return;
        }
        break;
      case 2:  // run list file name
        runListFileName = dlg->getRunListFileName();
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
    QValueVector<QString> cmd;
    str = fAdmin->getExecPath() + "/mlog2db";
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
          str = "[" + runList.section(' ', 0, 0) + "]";
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
    str = dlg->getExtension();
    if (str.isEmpty())
      cmd.append("");
    else
      cmd.append(str);

    // options

    // no header flag?
    if (!dlg->getWriteDbHeaderFlag())
      cmd.append("noheader");

    // no summary flag?
    if (!dlg->getSummaryFilePresentFlag())
      cmd.append("nosummary");

    // template run no
    if (!dlg->getTemplateRunNo().isEmpty()) {
      str = "fit-" + dlg->getTemplateRunNo();
      cmd.append(str);
    }

    // keep minuit2 output
    if (dlg->getMinuit2OutputFlag()) {
      fKeepMinuit2Output = dlg->getMinuit2OutputFlag();
      cmd.append("-k");
    }

    // DB output wished
    if (!dlg->getDbOutputFileName().isEmpty()) {
      str = "-o" + dlg->getDbOutputFileName();
      cmd.append(str);
    }

    PFitOutputHandler fitOutputHandler(cmd);
    fitOutputHandler.setModal(true);
    fitOutputHandler.exec();
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::musrView()
{
  if ( !currentEditor() )
    return;

  QString tabLabel = fTabWidget->label(fTabWidget->currentPageIndex());
  if (tabLabel == "noname") {
    QMessageBox::critical(this, "**ERROR**", "For a view a real mlog/msr-file is needed.");
    return;
  } else if (tabLabel == "noname*") {
    fileSaveAs();
  } else if (tabLabel.find("*") > 0) {
    fileSave();
  }

  QString cmd;
  QString str;

  str = fAdmin->getExecPath() + "/musrview";
  cmd = str + " ";

  str = *fFilenames.find( currentEditor() );
  cmd += str + " &";

  system(cmd.latin1());
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::musrT0()
{
  if ( !currentEditor() )
    return;

  QMessageBox::information( this, "musrT0",
  "Will call musrt0.\n"
  "NOT IMPLEMENTED YET :-(" );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::musrPrefs()
{
  PPrefsDialog *dlg = new PPrefsDialog(fKeepMinuit2Output, fDump);

  if (dlg->exec() == QDialog::Accepted) {
    fKeepMinuit2Output = dlg->keepMinuit2Output();
    fDump = dlg->getDump();
  }
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
  if ((idx = currentFileName.find(".msr")) > 0) { // msr-file
    swapFileName = currentFileName;
    swapFileName.replace(idx, 5, ".mlog");
  } else if ((idx = currentFileName.find(".mlog")) > 0) { // mlog-file
    swapFileName = currentFileName;
    swapFileName.replace(idx, 5, ".msr ").stripWhiteSpace();
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
  if (fTabWidget->label(fTabWidget->currentPageIndex()).find("*") > 0) { // needs to be saved first
    fileSave();
  }

  // swap files
  QString cmd;
  cmd = QString("cp ") + currentFileName + QString(" ") + tempFileName;
  system(cmd.latin1());
  cmd = QString("cp ") + swapFileName + QString(" ") + currentFileName;
  system(cmd.latin1());
  cmd = QString("cp ") + tempFileName + QString(" ") + swapFileName;
  system(cmd.latin1());
  cmd = QString("rm ") + tempFileName;
  system(cmd.latin1());

  // reload current file
  fileClose();
  load(currentFileName);

  // check if swap file is open as well, and if yes, reload it
  idx = -1;
  for (int i=0; i<fTabWidget->count(); i++) {
    if (fTabWidget->label(i).find(swapFileName) >= 0) {
      idx = i;
      break;
    }
  }
  if (idx >= 0) { // swap file is open
    int currentIdx = fTabWidget->currentPageIndex();
    fTabWidget->setCurrentPage(idx);
    fileClose();
    load(swapFileName);
    fTabWidget->setCurrentPage(currentIdx);
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
  PMusrGuiAbout *about = new PMusrGuiAbout();
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
  currentEditor()->selectAll();
  fComboFont->lineEdit()->setText( f.family() );
  fComboSize->lineEdit()->setText( QString::number( f.pointSize() ) );
  currentEditor()->setFamily( f.family() );
  currentEditor()->setPointSize( f.pointSize() );
  currentEditor()->viewport()->setFocus();
  currentEditor()->selectAll(false);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::textChanged()
{
  if (!currentEditor())
    return;

  QString tabLabel = fTabWidget->label(fTabWidget->currentPageIndex());

  if ((fTabWidget->label(fTabWidget->currentPageIndex()).find("*") > 0) &&
      !currentEditor()->isModified()) {
    tabLabel.truncate(tabLabel.length()-1);
    fTabWidget->setTabLabel(fTabWidget->currentPage(), tabLabel);
  }

  if ((fTabWidget->label(fTabWidget->currentPageIndex()).find("*") < 0) &&
      currentEditor()->isModified())
    fTabWidget->setTabLabel(fTabWidget->currentPage(), tabLabel+"*");
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
