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

#include "PTextEdit.h"
#include "PAdmin.h"
#include "PFitOutputHandler.h"
#include "PGetDefaultDialog.h"
#include "forms/PMusrGuiAbout.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PTextEdit::PTextEdit( QWidget *parent, const char *name )
    : QMainWindow( parent, name )
{
  fAdmin = new PAdmin();

  fShowMlog = fAdmin->getShowMlog();

  setupFileActions();
  setupEditActions();
  setupTextActions();
  setupMusrActions();
  setupHelpActions();

  fTabWidget = new QTabWidget( this );
  setCentralWidget( fTabWidget );

  textFamily("Courier");
  textSize("11"); // 11pt

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
  a = new QAction( QPixmap::fromMimeSource( "musrasym.xpm" ), tr( "&Asymetry Default" ), CTRL + SHIFT + Key_A, this, "musrGetAsymetryDefault" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrGetAsymetryDefault() ) );
  a->addTo( tb );
  a->addTo( menu );

  a = new QAction( QPixmap::fromMimeSource( "musrsinglehisto.xpm" ), tr( "Single &Histo Default" ), CTRL + SHIFT + Key_H, this, "musrGetSinglHistoDefault" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrGetSingleHistoDefault() ) );
  a->addTo( tb );
  a->addTo( menu );

  a = new QAction( QPixmap::fromMimeSource( "musrcalcchisq.xpm" ), tr( "Calc Chisq" ), CTRL + SHIFT + Key_C, this, "cacluates for the given parameters chiSq/maxLH" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrCalcChisq() ) );
  a->addTo( tb );
  a->addTo( menu );

  a = new QAction( QPixmap::fromMimeSource( "musrfit.xpm" ), tr( "&Fit" ), CTRL + SHIFT + Key_F, this, "musrFit" );
  connect( a, SIGNAL( activated() ), this, SLOT( musrFit() ) );
  a->addTo( tb );
  a->addTo( menu );

  a = new QAction( QPixmap::fromMimeSource( "musrview.xpm" ), tr( "&View" ), CTRL + SHIFT + Key_V, this, "musrView" );
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
  QTextEdit *edit = new QTextEdit( fTabWidget );
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
QTextEdit *PTextEdit::currentEditor() const
{
  if ( fTabWidget->currentPage() && fTabWidget->currentPage()->inherits( "QTextEdit" ) )
    return (QTextEdit*)fTabWidget->currentPage();
  return 0;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::doConnections( QTextEdit *e )
{
  connect( e, SIGNAL( currentFontChanged( const QFont & ) ),
           this, SLOT( fontChanged( const QFont & ) ) );

  connect( e, SIGNAL( textChanged() ), this, SLOT( textChanged() ));
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::fileNew()
{
  QTextEdit *edit = new QTextEdit( fTabWidget );
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
                   QMessageBox::Yes, QMessageBox::Cancel);
    if (result == QMessageBox::Cancel)
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
                     QMessageBox::Yes, QMessageBox::Cancel);
      if (result == QMessageBox::Cancel)
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
void PTextEdit::musrGetAsymetryDefault()
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
  PFitOutputHandler fitOutputHandler(cmd);
  fitOutputHandler.setModal(true);
  fitOutputHandler.exec();
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

  str = *fFilenames.find( currentEditor());
  if (fShowMlog) {
    str.replace(str.find(".msr"), 4, ".mlog");
  }
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
  QMessageBox::information( this, "musrPrefs",
  "Will call musr related perferences dialog.\n"
  "NOT IMPLEMENTED YET :-(" );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PTextEdit::helpContents()
{
  QMessageBox::information( this, "helpContents",
  "Will deliver eventually a help content.\n"
  "NOT IMPLEMENTED YET :-(" );
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
