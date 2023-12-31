/****************************************************************************

  PTextEdit.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

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

#ifndef _PTEXTEDIT_H_
#define _PTEXTEDIT_H_

#include <qmainwindow.h>
#include <qmap.h>

#include "musrgui.h"
#include "PFileWatcher.h"

class PSubTextEdit;
class PAdmin;
class QAction;
class QComboBox;
class QTabWidget;
class QTextEdit;
class QPopupMenu;

class PTextEdit : public QMainWindow
{
  Q_OBJECT

public:
  PTextEdit( QWidget *parent = 0, const char *name = 0 );
  virtual ~PTextEdit();

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

private slots:
  void fileNew();
  void fileOpen();
  void fileReload();
  void fileSave();
  void fileSaveAs();
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

  void musrGetAsymmetryDefault();
  void musrGetSingleHistoDefault();
  void musrCalcChisq();
  void musrFit();
  void musrMsr2Data();
  void musrView();
  void musrT0();
  void musrPrefs();
  void musrSwapMsrMlog();
  void musrDump();

  void helpContents();
  void helpAboutQt();
  void helpAbout();

  void fontChanged( const QFont &f );
  void textChanged(const bool forced = false);
  void currentCursorPosition(int para, int pos);

  void replace();
  void replaceAndClose();
  void replaceAll();

  void applyFontSettings(QWidget*);
  void checkIfModified(QWidget*);

private:
  PAdmin *fAdmin;

  QAction *fMusrT0Action;

  bool fKeepMinuit2Output;
  bool fTitleFromDataFile;
  bool fEnableMusrT0;
  int  fDump;

  PMsr2DataParam *fMsr2DataParam;
  PFindReplaceData *fFindReplaceData;

  QComboBox *fComboFont;
  QComboBox *fComboSize;
  bool fFontChanging; ///< flag needed to prevent some textChanged feature to occure when only the font changed

  QTabWidget *fTabWidget;
  QMap<PSubTextEdit*, QString> fFilenames;

  PFileWatcher *fFileWatcher;
};


#endif // _PTEXTEDIT_H_
