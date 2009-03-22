/****************************************************************************

  PSubTextEdit.cpp

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

#include <qaction.h>
#include <qpopupmenu.h>
#include <qdatetime.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qmessagebox.h>

#include "PAdmin.h"
#include "PSubTextEdit.h"
#include "forms/PGetTitleDialog.h"
#include "PGetParameterDialog.h"
#include "PGetTheoryBlockDialog.h"
#include "PGetFunctionsBlockDialog.h"
#include "PGetAsymmetryRunBlockDialog.h"
#include "PGetSingleHistoRunBlockDialog.h"
#include "PGetNonMusrRunBlockDialog.h"
#include "PGetFourierDialog.h"
#include "PGetPlotDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PSubTextEdit::PSubTextEdit(PAdmin *admin,
                           QWidget *parent, const char *name) :
                           QTextEdit(parent, name),
                           fAdmin(admin)
{
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
QPopupMenu* PSubTextEdit::createPopupMenu(const QPoint &pos)
{
  QPopupMenu *menu = new QPopupMenu( this );

  QAction *a;
  a = new QAction(tr("insert Title"), 0, this, "insertTitle");
  connect(a, SIGNAL( activated() ), this, SLOT( insertTitle() ));
  a->addTo( menu );

  a = new QAction(tr("insert Parameter Block"), 0, this, "insertParameterBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertParameterBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Theory Block"), 0, this, "insertTheoryBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertTheoryBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Function Block"), 0, this, "insertFunctionBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertFunctionBlock() ));
  a->addTo( menu );

  menu->insertSeparator();

  a = new QAction(tr("insert Asymmetry Block"), 0, this, "insertAsymRunBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertAsymRunBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Single Histo Block"), 0, this, "insertSingleHistRunBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertSingleHistRunBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert NonMusr Block"), 0, this, "insertNonMusrRunBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertNonMusrRunBlock() ));
  a->addTo( menu );

  menu->insertSeparator();

  a = new QAction(tr("insert Command Block"), 0, this, "insertCommandBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertCommandBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Fourier Block"), 0, this, "insertFourierBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertFourierBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Plot Block"), 0, this, "insertPlotBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertPlotBlock() ));
  a->addTo( menu );

  a = new QAction(tr("insert Statistic Block"), 0, this, "insertStatisticBlock");
  connect(a, SIGNAL( activated() ), this, SLOT( insertStatisticBlock() ));
  a->addTo( menu );

  return menu;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertTitle()
{
  PGetTitleDialog *dlg = new PGetTitleDialog();
  if (dlg->exec() == QDialog::Accepted) {
    QString title = dlg->fTitle_lineEdit->text();
    insert(title+"\n");
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertParameterBlock()
{
  PGetParameterDialog *dlg = new PGetParameterDialog();
  if (dlg->exec() == QDialog::Accepted) {
    insert(dlg->getParams());
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertTheoryBlock()
{
  PGetTheoryBlockDialog *dlg = new PGetTheoryBlockDialog(fAdmin);
  if (dlg->exec() == QDialog::Accepted) {
    insert(dlg->getTheoryBlock());
    insert("\n");
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertFunctionBlock()
{
  PGetFunctionsBlockDialog *dlg = new PGetFunctionsBlockDialog(fAdmin->getHelpMain());
  if (dlg->exec() == QDialog::Accepted) {
    insert(dlg->getFunctionsBlock());
    insert("\n\n");
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertAsymRunBlock()
{
  PGetAsymmetryRunBlockDialog *dlg = new PGetAsymmetryRunBlockDialog(fAdmin->getHelpMain());
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
    insert(str);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertSingleHistRunBlock()
{
  PGetSingleHistoRunBlockDialog *dlg = new PGetSingleHistoRunBlockDialog(fAdmin->getHelpMain());
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
    insert(str);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertNonMusrRunBlock()
{
  PGetNonMusrRunBlockDialog *dlg = new PGetNonMusrRunBlockDialog(fAdmin->getHelpMain());
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
    insert(str);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertCommandBlock()
{
  insert("###############################################################\n");
  insert("COMMANDS\n");
  insert("SET BATCH\n");
  insert("STRATEGY 1\n");
  insert("MINIMIZE\n");
  insert("#MINOS\n");
  insert("SAVE\n");
  insert("END RETURN\n\n");
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertFourierBlock()
{
  PGetFourierDialog *dlg = new PGetFourierDialog();

  if (dlg->exec() == QDialog::Accepted) {
    insert(dlg->getFourierBlock()+"\n");
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertPlotBlock()
{
  PGetPlotDialog *dlg = new PGetPlotDialog();

  if (dlg->exec() == QDialog::Accepted) {
    insert(dlg->getPlotBlock()+"\n");
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertStatisticBlock()
{
  QDateTime dt = QDateTime::currentDateTime();
  insert("###############################################################\n");
  insert("STATISTIC --- " + dt.toString("yyyy-MM-dd hh:mm:ss") + "\n");
  insert("chisq = ????, NDF = ????, chisq/NDF = ????\n\n");
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
