/****************************************************************************

  PSubTextEdit.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id: PSubTextEdit.cpp 3936 2009-05-22 11:38:21Z nemu $

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

#include <QAction>
#include <QMenu>
#include <QDateTime>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QIconSet>
#include <QPixmap>
#include <QImage>

#include "PAdmin.h"
#include "PSubTextEdit.h"
#include "PGetTitleBlockDialog.h"
#include "PGetParameterBlockDialog.h"
#include "PGetTheoryBlockDialog.h"
#include "PGetFunctionsBlockDialog.h"
#include "PGetAsymmetryRunBlockDialog.h"
#include "PGetSingleHistoRunBlockDialog.h"
#include "PGetNonMusrRunBlockDialog.h"
#include "PGetFourierBlockDialog.h"
#include "PGetPlotBlockDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PSubTextEdit::PSubTextEdit(PAdmin *admin, QWidget *parent) :
                           QPlainTextEdit(parent),
                           fAdmin(admin)
{
//  fLastModified = QDateTime::fromString("1900-01-01 00:00:00");
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QMenu* PSubTextEdit::createPopupMenu(const QPoint &pos)
{
  QMenu *menu = new QMenu( this );
  QMenu *theoryFunctions = new QMenu( menu );

  QAction *a;
  a = new QAction( tr("insert Title"), this);
  a->setStatusTip( tr("insert a title") );
  connect(a, SIGNAL( activated() ), this, SLOT( insertTitle() ));
  menu->addAction(a);

  a = new QAction(tr("insert Parameter Block"), this);
  a->setStatusTip( tr("insert a parameter block") );
  connect(a, SIGNAL( activated() ), this, SLOT( insertParameterBlock() ));
  menu->addAction(a);

  // feed the theoryFunctions popup menu
  for (unsigned int i=0; i<fAdmin->getTheoryCounts(); i++) {
    PTheory *theoryItem = fAdmin->getTheoryItem(i);
    a = new QAction( theoryItem->label, this);
    theoryFunctions->addAction(a);
  }
  theoryFunctions->setTitle( tr("insert theory function") );
  menu->addMenu(theoryFunctions);
  connect(theoryFunctions, SIGNAL( activated(int) ), this, SLOT( insertTheoryFunction(int) ));

  a = new QAction(tr("insert Theory Block"), this);
  a->setStatusTip( tr("insert a theory block") );
  connect(a, SIGNAL( activated() ), this, SLOT( insertTheoryBlock() ));
  menu->addAction(a);

  a = new QAction(tr("insert Function Block"), this);
  a->setStatusTip( tr("insert a function block") );
  connect(a, SIGNAL( activated() ), this, SLOT( insertFunctionBlock() ));
  menu->addAction(a);

  menu->addSeparator();

  a = new QAction(tr("insert Asymmetry Run Block"), this);
  a->setStatusTip( tr("insert an asymmetry run block") );
  connect(a, SIGNAL( activated() ), this, SLOT( insertAsymRunBlock() ));
  menu->addAction(a);

  a = new QAction(tr("insert Single Histo Run Block"), this);
  a->setStatusTip( tr("insert a single histo run block") );
  connect(a, SIGNAL( activated() ), this, SLOT( insertSingleHistRunBlock() ));
  menu->addAction(a);

  a = new QAction(tr("insert NonMusr Block"), this);
  a->setStatusTip( tr("insert a NonMusr run block") );
  connect(a, SIGNAL( activated() ), this, SLOT( insertNonMusrRunBlock() ));
  menu->addAction(a);

  menu->addSeparator();

  a = new QAction(tr("insert Command Block"), this);
  a->setStatusTip( tr("insert a command block") );
  connect(a, SIGNAL( activated() ), this, SLOT( insertCommandBlock() ));
  menu->addAction(a);

  a = new QAction(tr("insert Fourier Block"), this);
  a->setStatusTip( tr("insert a Fourier block") );
  connect(a, SIGNAL( activated() ), this, SLOT( insertFourierBlock() ));
  menu->addAction(a);

  a = new QAction(tr("insert Plot Block"), this);
  a->setStatusTip( tr("insert a plot block") );
  connect(a, SIGNAL( activated() ), this, SLOT( insertPlotBlock() ));
  menu->addAction(a);

  a = new QAction(tr("insert Statistic Block"), this);
  a->setStatusTip( tr("insert a statistic block") );
  connect(a, SIGNAL( activated() ), this, SLOT( insertStatisticBlock() ));
  menu->addAction(a);

  return menu;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertTitle()
{
  PGetTitleBlockDialog *dlg = new PGetTitleBlockDialog();

  if (dlg == 0)
    return;

  if (dlg->exec() == QDialog::Accepted) {
    QString title = dlg->getTitle();
    insertPlainText(title+"\n");
  }

  delete dlg;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertParameterBlock()
{
  PGetParameterBlockDialog *dlg = new PGetParameterBlockDialog();

  if (dlg == 0)
    return;

  if (dlg->exec() == QDialog::Accepted) {
    insertPlainText(dlg->getParams());
  }

  delete dlg;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertTheoryFunction(int idx)
{
  if (idx < 300)
    return;

  int index = idx - 300;

  if (index >= (int)fAdmin->getTheoryCounts())
    return;

  QString str = "????";
  PTheory *theoItem = fAdmin->getTheoryItem(index);
  if (theoItem == 0)
    return;

  // add theory function name
  str = theoItem->name + "   ";
  if (theoItem->name == "userFcn") {
    str += "libMyLibrary.so  TMyFunction  ";
  }

  // add pseudo parameters
  for (int i=0; i<theoItem->params; i++) {
    str += QString("%1").arg(i+1) + "   ";
  }

  // add comment
  str += theoItem->comment;

  // add newline
  str += "\n";

  insertPlainText(str);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertTheoryBlock()
{
  PGetTheoryBlockDialog *dlg = new PGetTheoryBlockDialog(fAdmin);

  if (dlg == 0)
    return;

  if (dlg->exec() == QDialog::Accepted) {
    insertPlainText(dlg->getTheoryBlock());
    insertPlainText("\n");
  }

  delete dlg;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertFunctionBlock()
{
  PGetFunctionsBlockDialog *dlg = new PGetFunctionsBlockDialog(fAdmin->getHelpMain());

  if (dlg == 0)
    return;

  if (dlg->exec() == QDialog::Accepted) {
    insertPlainText(dlg->getFunctionsBlock());
    insertPlainText("\n\n");
  }

  delete dlg;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertAsymRunBlock()
{
  PGetAsymmetryRunBlockDialog *dlg = new PGetAsymmetryRunBlockDialog(fAdmin->getHelpMain());

  if (dlg == 0)
    return;

  if (dlg->exec() == QDialog::Accepted) {
    QString str, workStr;
    bool valid = true, present = true;
    // check if there is already a run block present, necessary because of the '####' line
    // STILL MISSING

    // add run line
    str += dlg->getRunHeaderInfo();

    // add fittype
    str += "fittype         2         (asymmetry fit)\n";

    // add alpha if present
    workStr = dlg->getAlphaParameter(present);
    if (present) {
      str += workStr;
    }

    // add beta if present
    workStr = dlg->getBetaParameter(present);
    if (present) {
      str += workStr;
    }

    // add map
    workStr = dlg->getMap(valid);
    if (valid) {
      str += workStr;
    } else {
      QMessageBox::critical(this, "**ERROR**",
         "Given map not valid, will add a default map line",
         QMessageBox::Ok, QMessageBox::NoButton);
      str += "map             0  0  0  0  0  0  0  0  0  0\n";
    }

    // add forward
    str += dlg->getForward();

    // add backward
    str += dlg->getBackward();

    // add background or backgr.fix
    workStr = dlg->getBackground(valid);
    str += workStr;
    if (!valid) {
      QMessageBox::critical(this, "**ERROR**",
         "Either <b>background</b> or <b>backgr.fix</b> is needed!\nWill set <b>background</b> to 0..10, please correct!",
         QMessageBox::Ok, QMessageBox::NoButton);
    }

    // add data
    workStr = dlg->getData(valid);
    if (valid) {
      str += workStr;
    } else {
      QMessageBox::critical(this, "**ERROR**",
         "Not all Data entries are present.Fix is needed!\nWill not set anything!",
         QMessageBox::Ok, QMessageBox::NoButton);
    }

    // add t0 if present
    workStr = dlg->getT0(present);
    if (present) {
      str += workStr;
    } else {
      QMessageBox::warning(this, "**ERROR**",
         "T0's not given, assume that they are present in the data file!",
         QMessageBox::Ok, QMessageBox::NoButton);
    }

    // add fit range
    workStr = dlg->getFitRange(valid);
    str += workStr;
    if (!valid) {
      QMessageBox::critical(this, "**ERROR**",
         "No valid fit range is given.Fix is needed!\nWill add a default one!",
         QMessageBox::Ok, QMessageBox::NoButton);
    }

    // add packing
    workStr = dlg->getPacking(present);
    str += workStr;
    if (!present) {
      QMessageBox::critical(this, "**ERROR**",
         "No valid packing/binning is given.Fix is needed!\nWill add a default one!",
         QMessageBox::Ok, QMessageBox::NoButton);
    }

    // insert Asymmetry Run Block at the current cursor position
    insertPlainText(str);
  }

  delete dlg;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertSingleHistRunBlock()
{
  PGetSingleHistoRunBlockDialog *dlg = new PGetSingleHistoRunBlockDialog(fAdmin->getHelpMain());

  if (dlg == 0)
    return;

  if (dlg->exec() == QDialog::Accepted) {
    QString str, workStr;
    bool valid = true, present = true;
    // check if there is already a run block present, necessary because of the '####' line
    // STILL MISSING

    // add run line
    str += dlg->getRunHeaderInfo();

    // add fittype
    str += "fittype         0         (single histogram fit)\n";

    // add map
    workStr = dlg->getMap(valid);
    if (valid) {
      str += workStr;
    } else {
      QMessageBox::critical(this, "**ERROR**",
         "Given map not valid, will add a default map line",
         QMessageBox::Ok, QMessageBox::NoButton);
      str += "map             0  0  0  0  0  0  0  0  0  0\n";
    }

    // add forward
    str += dlg->getForward();

    // add norm
    str += dlg->getNorm();

    // add lifetime parameter
    workStr = dlg->getMuonLifetimeParam(present);
    if (present) {
      str += workStr;
    }

    // add lifetime correction flag if present
    workStr = dlg->getLifetimeCorrection(present);
    if (present) {
      str += workStr;
    }

    // add background, backgr.fix or backgr.fit
    workStr = dlg->getBackground(valid);
    str += workStr;
    if (!valid) {
      QMessageBox::critical(this, "**ERROR**",
         "Either <b>background</b>, <b>backgr.fix</b>, or <b>backgr.fit</b> is needed!\nWill set <b>background</b> to 0..10, please correct!",
         QMessageBox::Ok, QMessageBox::NoButton);
    }

    // add t0 if present
    workStr = dlg->getT0(present);
    if (present) {
      str += workStr;
    } else {
      QMessageBox::warning(this, "**ERROR**",
         "T0's not given, assume that they are present in the data file!",
         QMessageBox::Ok, QMessageBox::NoButton);
    }

    // add data
    workStr = dlg->getData(valid);
    if (valid) {
      str += workStr;
    } else {
      QMessageBox::critical(this, "**ERROR**",
         "Not all Data entries are present.Fix is needed!\nWill not set anything!",
         QMessageBox::Ok, QMessageBox::NoButton);
    }

    // add fit range
    workStr = dlg->getFitRange(valid);
    str += workStr;
    if (!valid) {
      QMessageBox::critical(this, "**ERROR**",
         "No valid fit range is given.Fix is needed!\nWill add a default one!",
         QMessageBox::Ok, QMessageBox::NoButton);
    }

    // add packing
    workStr = dlg->getPacking(present);
    str += workStr;
    if (!present) {
      QMessageBox::critical(this, "**ERROR**",
         "No valid packing/binning is given.Fix is needed!\nWill add a default one!",
         QMessageBox::Ok, QMessageBox::NoButton);
    }

    // insert Single Histogram Run Block at the current cursor position
    insertPlainText(str);
  }

  delete dlg;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertNonMusrRunBlock()
{
  PGetNonMusrRunBlockDialog *dlg = new PGetNonMusrRunBlockDialog(fAdmin->getHelpMain());

  if (dlg == 0)
    return;

  if (dlg->exec() == QDialog::Accepted) {
    QString str, workStr;
    bool valid = true;
    // check if there is already a run block present, necessary because of the '####' line
    // STILL MISSING

    // add run line
    str += dlg->getRunHeaderInfo();

    // add fittype
    str += "fittype         8         (non musr fit)\n";

    // add map
    workStr = dlg->getMap(valid);
    if (valid) {
      str += workStr;
    } else {
      QMessageBox::critical(this, "**ERROR**",
         "Given map not valid, will add a default map line",
         QMessageBox::Ok, QMessageBox::NoButton);
      str += "map             0  0  0  0  0  0  0  0  0  0\n";
    }

    // add xy-data
    workStr = dlg->getXYData(valid);
    if (valid) {
      str += workStr;
    } else {
      QMessageBox::critical(this, "**ERROR**",
         "Not all xy-data entries are present.Fix is needed!\nWill not set anything!",
         QMessageBox::Ok, QMessageBox::NoButton);
    }

    // add fit range
    workStr = dlg->getFitRange(valid);
    str += workStr;
    if (!valid) {
      QMessageBox::critical(this, "**ERROR**",
         "No valid fit range is given.Fix is needed!\nWill add a default one!",
         QMessageBox::Ok, QMessageBox::NoButton);
    }

    // add packing
    str += "packing         1\n";

    // insert NonMusr Run Block at the current cursor position
    insertPlainText(str);
  }

  delete dlg;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertCommandBlock()
{
  insertPlainText("###############################################################\n");
  insertPlainText("COMMANDS\n");
  insertPlainText("SET BATCH\n");
  insertPlainText("STRATEGY 1\n");
  insertPlainText("MINIMIZE\n");
  insertPlainText("#MINOS\n");
  insertPlainText("SAVE\n");
  insertPlainText("END RETURN\n\n");
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertFourierBlock()
{
  PGetFourierBlockDialog *dlg = new PGetFourierBlockDialog();

  if (dlg == 0)
    return;

  if (dlg->exec() == QDialog::Accepted) {
    insertPlainText(dlg->getFourierBlock()+"\n");
  }

  delete dlg;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertPlotBlock()
{
  PGetPlotBlockDialog *dlg = new PGetPlotBlockDialog();

  if (dlg == 0)
    return;

  if (dlg->exec() == QDialog::Accepted) {
    insertPlainText(dlg->getPlotBlock()+"\n");
  }

  delete dlg;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertStatisticBlock()
{
  QDateTime dt = QDateTime::currentDateTime();
  insertPlainText("###############################################################\n");
  insertPlainText("STATISTIC --- " + dt.toString("yyyy-MM-dd hh:mm:ss") + "\n");
  insertPlainText("chisq = ????, NDF = ????, chisq/NDF = ????\n\n");
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
