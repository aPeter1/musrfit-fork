/***************************************************************************

  PmuppGui.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2020 by Andreas Suter                              *
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

#ifndef _PMUPPGUI_H_
#define _PMUPPGUI_H_

#include <QMainWindow>
#include <QVector>
#include <QString>
#include <QWidget>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSplitter>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QEvent>
#include <QProcess>

#include "PVarHandler.h"
#include "PVarDialog.h"
#include "PmuppAdmin.h"
#include "Pmupp.h"
#include "mupp.h"

//-----------------------------------------------------------------------------
class PmuppXY
{
public:
  PmuppXY() { init(); }

  void setCollectionTag(int tag) { fCollectionTag = tag; }
  void setXlabel(QString str) { fXlabel = str; }
  void addYlabel(QString str) { fYlabel.push_back(str); }
  void setYlabel(int idx, QString str);
  void removeYlabel(int idx);
  void removeYlabel(QString str);

  int getCollectionTag() { return fCollectionTag; }
  QString getXlabel() { return fXlabel; }
  int getXlabelIdx();
  int getYlabelSize() { return fYlabel.size(); }
  QString getYlabel(int idx);
  int getYlabelIdx(int idx);
  QVector<QString> getYlabels() { return fYlabel; }

private:
  int fCollectionTag;
  QString fXlabel;
  QVector<QString> fYlabel;

  void init();
};

//-----------------------------------------------------------------------------
// Layout Scheme of PmuppGui:
// |--------------------------------------------------------------------|
// | Main                                                               |
// | |----------------------------------------------------------------| |
// | | Top                                                            | |
// | | |----------------------------| |-----------------------------| | |
// | | | Grid Left (2 cols, 3 rows) | | Grid Right (2 cols, 6 rows) | | |
// | | | +++++++++                  | | ++++++++++                  | | |
// | | |                            | |                             | | |
// | | |----------------------------| |-----------------------------| | |
// | |----------------------------------------------------------------| |
// |                                                                    |
// | |----------------------------------------------------------------| |
// | | Cmd                                                            | |
// | |----------------------------------------------------------------| |
// ----------------------------------------------------------------------
//
// Grid Left  contains: fColLabel, fColParamSplitter,
//                      fRemoveCollection, fRefreshCollection
// Grid Right contains: f(X,Y)axisLabel, fView(X,Y), fAdd(X,Y), fRemove(X,Y),
//                      fAddDitto, fPlot
//-----------------------------------------------------------------------------
class PmuppGui : public QMainWindow
{
  Q_OBJECT

public:
  PmuppGui(QStringList fln, QWidget *parent = 0, Qt::WindowFlags f = 0);
  virtual ~PmuppGui();

public slots:
  void aboutToQuit();
  void fileOpen();
  void fileOpenRecent();
  void fileExit();

  void toolDumpCollections();
  void toolDumpXY();
  void addVar();
  void normalize();

  void helpCmds();
  void helpAbout();
  void helpAboutQt();

protected:
  bool eventFilter(QObject *o, QEvent *e);

private:
  enum EAxis {kXaxis, kYaxis};

  PmuppAdmin *fAdmin;
  bool fDarkTheme;
  bool fDarkToolBarIcon;
  bool fNormalize;

  uint fDatime;
  uint fMuppInstance;

  PParamDataHandler *fParamDataHandler;
  QVector<PmuppXY> fXY;
  QVector<PVarHandler> fVarHandler;

  QString fMacroPath;
  QString fMacroName;

  QWidget     *fCentralWidget;

  QMenu *fRecentFilesMenu;   ///< recent file menu
  QAction *fRecentFilesAction[MAX_RECENT_FILES]; ///< array of the recent file actions
  QAction *fNormalizeAction;

  QBoxLayout  *fBoxLayout_Main;     // top->bottom (0)
  QBoxLayout  *fBoxLayout_Top;      // left->right (1)
  QGridLayout *fGridLayout_Left;    // 2 columns, 3 rows
  QGridLayout *fGridLayout_Right;   // 2 columns, 6 rows
  QBoxLayout  *fBoxLayout_Cmd;      // left->right (1)

  QLabel      *fColLabel;
  QSplitter   *fColParamSplitter;
  QListWidget *fColList;
  QListWidget *fParamList;
  QPushButton *fRemoveCollection;
  QPushButton *fRefreshCollection;
  QLabel      *fXaxisLabel;
  QLabel      *fYaxisLabel;
  QListWidget *fViewX;
  QListWidget *fViewY;
  QPushButton *fAddX;
  QPushButton *fAddY;
  QPushButton *fAddDitto;
  QPushButton *fRemoveX;
  QPushButton *fRemoveY;
  QPushButton *fPlot;
  QSplitter   *fCmdSplitter;
  QPlainTextEdit *fCmdLineHistory;
  QLineEdit   *fCmdLine;
  QPushButton *fExitButton;

  QVector<QString> fCmdHistory;

  PVarDialog *fVarDlg;

  QProcess *fMuppPlot;

  void setupFileActions();
  void setupToolActions();
  void setupHelpActions();

  void getTheme();

  void fillRecentFiles();

  void readCmdHistory();
  void writeCmdHistory();

  bool isNewCollection(PmuppCollection &coll);
  int  getXlabelIndex(QString label);
  void getMinMax(QVector<double> &data, double &min, double &max);
  QString substituteDefaultLabels(QString label);
  void selectCollection(QString cmd);
  uint getFirstAvailableMuppInstance();

  void updateCollectionList();
  void updateXYList(int idx);
  void updateXYListGui();
  bool findValue(PmuppRun &run, EAxis tag);
  bool allXYEqual();
  bool indexAlreadyPresent(int idx);
  void replaceIndex(PmuppXY &data, const int idx);

  void startMuppPlot();

private slots:
  void addDitto();
  void addX(QString param="");
  void addY(QString param="");
  void createMacro();
  void handleCmds();
  void plot();
  void refresh();
  void remove();
  void removeX(QString param="");
  void removeY(QString param="");

  void handleNewData();
  void updateParamList(int currentRow);
  void editCollName(QListWidgetItem *item);
  void dropOnViewX(QListWidgetItem *item);
  void dropOnViewY(QListWidgetItem *item);
  void refreshY();

  void check(QString varStr, QVector<int> idx);
  void add(QString varStr, QVector<int> idx);
};

#endif // _PMUPPGUI_H_
