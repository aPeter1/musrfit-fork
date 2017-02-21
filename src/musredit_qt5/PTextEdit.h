/****************************************************************************

  PTextEdit.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010-2016 by Andreas Suter                              *
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

#ifndef _PTEXTEDIT_H_
#define _PTEXTEDIT_H_

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QTimer>
#include <QString>
#include <QVector>

#include <QtDebug>


#include "musredit.h"

class PSubTextEdit;
class PAdmin;
class QFileSystemWatcher;
class QAction;
class QComboBox;
class QTabWidget;
class QTextEdit;
class QMenu;

//----------------------------------------------------------------------------------------------
/**
 * <p>Main class for musredit.
 */
class PTextEdit : public QMainWindow
{
  Q_OBJECT

public:
  PTextEdit( QWidget *parent = 0, Qt::WindowFlags f = 0 );
  virtual ~PTextEdit() {}

public slots:
  void aboutToQuit();

signals:
  void close();

private:
  void setupFileActions();
  void setupEditActions();
  void setupTextActions();
  void setupMusrActions();
  void setupHelpActions();
  void load( const QString &f, const int index=-1 );
  PSubTextEdit *currentEditor() const;
  void doConnections( PSubTextEdit *e );
  void fileSystemWatcherActivation();

private slots:
  void insertTitle();
  void insertParameterBlock();
  void insertTheoryBlock();
  void insertTheoryFunction(QAction *a);
  void insertFunctionBlock();
  void insertAsymRunBlock();
  void insertSingleHistRunBlock();
  void insertNonMusrRunBlock();
  void insertCommandBlock();
  void insertFourierBlock();
  void insertPlotBlock();
  void insertStatisticBlock();

  void fileNew();
  void fileOpen();
  void fileOpenRecent();
  void fileReload();
  void fileOpenPrefs();
  void fileSave();
  void fileSaveAs();
  void fileSavePrefs();
  void filePrint();
  void fileClose( const bool check = true );
  void fileCloseAll();
  void fileCloseAllOthers();
  void fileExit();

  void editUndo();
  void editRedo();
  void editSelectAll();
  void editCut();
  void editCopy();
  void editPaste();
  void editFind();
  void editFindNext();
  void editFindPrevious();
  void editFindAndReplace();
  void editComment();
  void editUncomment();

  void textFamily( const QString &f );
  void textSize( const QString &p );

  void musrWiz();
  void musrCalcChisq();
  void musrFit();
  void musrMsr2Data();
  void musrView();
  void musrT0();
  void musrFT();
  void musrPrefs();
  void musrSwapMsrMlog();
  void musrDump();

  void helpContents();
  void helpAboutQt();
  void helpAbout();

  void fontChanged( const QFont &f );
  void textChanged(const bool forced = false);
  void currentCursorPosition();

  void replace();
  void replaceAndClose();
  void replaceAll();

  void applyFontSettings(int);
  void fileChanged(const QString &fileName);
  void setFileSystemWatcherActive();

private:
  PAdmin *fAdmin; ///< pointer to the xml-startup file informations. Needed for different purposes like default working- and executable directories etc.
  QFileSystemWatcher *fFileSystemWatcher; ///< checks if msr-files are changing on the disk while being open in musredit.
  bool fFileSystemWatcherActive; ///< flag to enable/disable the file system watcher
  QTimer fFileSystemWatcherTimeout; ///< timer used to re-enable file system watcher. Needed to delay the re-enabling
  QString fLastDirInUse; ///< string holding the path from where the last file was loaded.
  QStringList fMusrFTPrevCmd;

  QAction *fMusrT0Action;

  PMsr2DataParam *fMsr2DataParam; ///< structure holding the necessary input information for msr2data
  PFindReplaceData *fFindReplaceData; ///< structure holding the ncessary input for find/replace

  QComboBox *fComboFont; ///< combo box for the font selector
  QComboBox *fComboSize; ///< combo box for the font size
  bool fFontChanging; ///< flag needed to prevent some textChanged feature to occure when only the font changed

  QTabWidget *fTabWidget; ///< tab widget in which the text editor(s) are placed
  QMap<PSubTextEdit*, QString> fFilenames; ///< mapper between tab widget object and filename

  QMenu *fRecentFilesMenu;   ///< recent file menu
  QAction *fRecentFilesAction[MAX_RECENT_FILES]; ///< array of the recent file actions

  void fillRecentFiles();
  QStringList getRunList(QString runListStr, bool &ok);
};


#endif // _PTEXTEDIT_H_
