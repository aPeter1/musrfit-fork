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

#include "PSubTextEdit.h"
#include "forms/PGetTitleDialog.h"
#include "PGetParameterDialog.h"
#include "PGetAsymmetryRunBlockDialog.h"
#include "PGetFourierDialog.h"
#include "PGetPlotDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PSubTextEdit::PSubTextEdit(QWidget *parent, const char *name) : QTextEdit(parent, name)
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
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertFunctionBlock()
{
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertAsymRunBlock()
{
  PGetAsymmetryRunBlockDialog *dlg = new PGetAsymmetryRunBlockDialog();
  if (dlg->exec() == QDialog::Accepted) {
    QString str, workStr;
    // check if there is already a run block present, necessary because of the '####' line
    // STILL MISSING

    // add run line
    str += "RUN " + dlg->fRunFileName_lineEdit->text() + " ";
    str += dlg->fBeamline_lineEdit->text().upper() + " ";
    str += dlg->fInstitute_comboBox->currentText() + " ";
    str += dlg->fFileFormat_comboBox->currentText() + "   (name beamline institute data-file-format)\n";

    // add fittype
    str += "fittype         2         (asymmetry fit)\n";

    // add alpha if present
    workStr = dlg->fAlpha_lineEdit->text();
    if (!workStr.isEmpty()) {
      str += "alpha           " + workStr + "\n";
    }

    // add beta if present
    workStr = dlg->fBeta_lineEdit->text();
    if (!workStr.isEmpty()) {
      str += "beta            " + workStr + "\n";
    }

    // add map
    str += "map             " + dlg->fMap_lineEdit->text() + "\n";

    // add forward
    str += "forward         " + dlg->fForward_lineEdit->text() + "\n";

    // add backward
    str += "backward        " + dlg->fBackward_lineEdit->text() + "\n";

    // check that either background or background.fix is given
    if (dlg->fBackgroundForwardStart_lineEdit->text().isEmpty() && dlg->fBackgroundForwardEnd_lineEdit->text().isEmpty() &&
        dlg->fBackgroundBackwardStart_lineEdit->text().isEmpty() && dlg->fBackgroundBackwardEnd_lineEdit->text().isEmpty() &&
        dlg->fBackgroundForwardFix_lineEdit->text().isEmpty() && dlg->fBackgroundBackwardFix_lineEdit->text().isEmpty()) {
      QMessageBox::critical(this, "**ERROR**",
         "Either Background or Background.Fix is needed!\nWill set Background to 0..10, please correct!",
         QMessageBox::Ok, QMessageBox::NoButton);
      str += "background      0  10  0  10\n";
    } else {
      if (!dlg->fBackgroundForwardStart_lineEdit->text().isEmpty()) { // assume the rest is given, not fool prove but ...
        str += "background      ";
        str += dlg->fBackgroundForwardStart_lineEdit->text() + "   ";
        str += dlg->fBackgroundForwardEnd_lineEdit->text() + "   ";
        str += dlg->fBackgroundBackwardStart_lineEdit->text() + "   ";
        str += dlg->fBackgroundBackwardEnd_lineEdit->text() + "\n";
      }
      if (!dlg->fBackgroundForwardFix_lineEdit->text().isEmpty()) { // assume the rest is given, not fool prove but ...
        str += "backgr.fix      ";
        str += dlg->fBackgroundForwardFix_lineEdit->text() + "   ";
        str += dlg->fBackgroundBackwardFix_lineEdit->text() + "\n";
      }
    }

    // add data
    if (dlg->fDataForwardStart_lineEdit->text().isEmpty() || dlg->fDataForwardEnd_lineEdit->text().isEmpty() ||
        dlg->fDataBackwardStart_lineEdit->text().isEmpty() || dlg->fDataBackwardEnd_lineEdit->text().isEmpty()) {
      QMessageBox::critical(this, "**ERROR**",
         "Not all Data entries are present.Fix is needed!\nWill not set anything!",
         QMessageBox::Ok, QMessageBox::NoButton);
    } else {
      str += "data            ";
      str += dlg->fDataForwardStart_lineEdit->text() + "   ";
      str += dlg->fDataForwardEnd_lineEdit->text() + "   ";
      str += dlg->fDataBackwardStart_lineEdit->text() + "   ";
      str += dlg->fDataBackwardEnd_lineEdit->text() + "\n";
    }

    // add t0 if present
    if (!dlg->fT0Forward_lineEdit->text().isEmpty() && !dlg->fT0Forward_lineEdit->text().isEmpty()) {
      str += "t0              ";
      str += dlg->fT0Forward_lineEdit->text() + "   ";
      str += dlg->fT0Backward_lineEdit->text() + "\n";
    }

    // add fit range
    if (dlg->fFitRangeStart_lineEdit->text().isEmpty() || dlg->fFitRangeEnd_lineEdit->text().isEmpty()) {
      QMessageBox::critical(this, "**ERROR**",
         "No valid fit range is given.Fix is needed!\nWill add a default one!",
         QMessageBox::Ok, QMessageBox::NoButton);
      str += "fit             0.0   10.0\n";
    } else {
      str += "fit             ";
      str += dlg->fFitRangeStart_lineEdit->text() + "   ";
      str += dlg->fFitRangeEnd_lineEdit->text() + "\n";
    }

    // add packing
    if (dlg->fPacking_lineEdit->text().isEmpty()) {
      QMessageBox::critical(this, "**ERROR**",
         "No valid packing/binning is given.Fix is needed!\nWill add a default one!",
         QMessageBox::Ok, QMessageBox::NoButton);
      str += "packing         1\n";
    } else {
      str += "packing         " + dlg->fPacking_lineEdit->text() + "\n\n";
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
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PSubTextEdit::insertNonMusrRunBlock()
{
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
