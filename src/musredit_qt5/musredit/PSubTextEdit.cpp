/****************************************************************************

  PSubTextEdit.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2019 by Andreas Suter                              *
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
#include <QIcon>
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
 * <p>Constructor.
 *
 * \param admin pointer to the musredit internal administration object.
 * \param parent pointer to the parent object.
 */
PSubTextEdit::PSubTextEdit(PAdmin *admin, QWidget *parent) :
                           QPlainTextEdit(parent),
                           fAdmin(admin)
{
}

//----------------------------------------------------------------------------------------------------
/**
 * @brief PSubTextEdit::getFitType
 * @return
 */
int PSubTextEdit::getFitType()
{
  QString str = toPlainText();
  int idx = str.indexOf("fittype");
  if (idx == -1)
    return -1;

  bool ok;
  for (int i=idx+7; i<str.length(); i++) {
    if (str[i] != ' ') {
      idx = QString(str[i]).toInt(&ok);
      if (!ok)
        idx = -1;
      break;
    }
  }

  return idx;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Starts the msr-title input dialog window.
 */
void PSubTextEdit::insertTitle()
{
  // for the time being the url's are hard coded but should be transfered to the XML startup
  PGetTitleBlockDialog *dlg = new PGetTitleBlockDialog(fAdmin->getHelpUrl("title"));

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
 * <p>Starts the msr-fit-parameter input dialog window.
 */
void PSubTextEdit::insertParameterBlock()
{
  // for the time being the url's are hard coded but should be transfered to the XML startup
  PGetParameterBlockDialog *dlg = new PGetParameterBlockDialog(fAdmin->getHelpUrl("parameters"));

  if (dlg == 0)
    return;

  if (dlg->exec() == QDialog::Accepted) {
    insertPlainText(dlg->getParams());
  }

  delete dlg;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Inserts the selected theory item.
 *
 * \param name of the theory item to be added.
 */
void PSubTextEdit::insertTheoryFunction(QString name)
{
  QString str = "????";

  int idx = -1;
  for (unsigned int i=0; i<fAdmin->getTheoryCounts(); i++) {
    if (name == fAdmin->getTheoryItem(i)->label) {
      idx = i;
      break;
    }
  }

  if (idx == -1)
    return;

  PTheory *theoItem = fAdmin->getTheoryItem(idx);
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
 * <p>Starts the msr-theory input dialog window.
 */
void PSubTextEdit::insertTheoryBlock()
{
  // for the time being the url's are hard coded but should be transfered to the XML startup
  PGetTheoryBlockDialog *dlg = new PGetTheoryBlockDialog(fAdmin, fAdmin->getHelpUrl("theory"));

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
 * <p>Starts the msr-functions input dialog window.
 */
void PSubTextEdit::insertFunctionBlock()
{
  // for the time being the url's are hard coded but should be transfered to the XML startup
  PGetFunctionsBlockDialog *dlg = new PGetFunctionsBlockDialog(fAdmin->getHelpUrl("functions"));

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
 * <p>Starts the msr-asymmetry-run input dialog window.
 */
void PSubTextEdit::insertAsymRunBlock()
{
  // for the time being the url's are hard coded but should be transfered to the XML startup
  PGetAsymmetryRunBlockDialog *dlg = new PGetAsymmetryRunBlockDialog(fAdmin->getHelpUrl("run"));

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
 * <p>Starts the msr-single-historgram-run input dialog window.
 */
void PSubTextEdit::insertSingleHistRunBlock()
{
  // for the time being the url's are hard coded but should be transfered to the XML startup
  PGetSingleHistoRunBlockDialog *dlg = new PGetSingleHistoRunBlockDialog(fAdmin->getHelpUrl("run"));

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
 * <p>Starts the msr-nonMusr-run input dialog window.
 */
void PSubTextEdit::insertNonMusrRunBlock()
{
  PGetNonMusrRunBlockDialog *dlg = new PGetNonMusrRunBlockDialog(fAdmin->getHelpUrl("run"));

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
 * <p>Insert the command block.
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
 * <p>Starts the msr-Fourier input dialog window.
 */
void PSubTextEdit::insertFourierBlock()
{
  // for the time being the url's are hard coded but should be transfered to the XML startup
  PGetFourierBlockDialog *dlg = new PGetFourierBlockDialog(fAdmin->getHelpUrl("fourier"));

  if (dlg == 0)
    return;

  if (dlg->exec() == QDialog::Accepted) {
    insertPlainText(dlg->getFourierBlock()+"\n");
  }

  delete dlg;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Starts the msr-plot input dialog window.
 */
void PSubTextEdit::insertPlotBlock()
{
  // for the time being the url's are hard coded but should be transfered to the XML startup
  PGetPlotBlockDialog *dlg = new PGetPlotBlockDialog(fAdmin->getHelpUrl("plot"));

  if (dlg == 0)
    return;

  if (dlg->exec() == QDialog::Accepted) {
    insertPlainText(dlg->getPlotBlock()+"\n");
  }

  delete dlg;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Insert a default msr-statistics block.
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
