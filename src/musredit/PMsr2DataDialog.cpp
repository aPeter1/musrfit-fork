/****************************************************************************

  PMsr2DataDialog.cpp

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

#include <QLineEdit>
#include <QValidator>
#include <QMessageBox>
#include <QTextEdit>
#include <QCheckBox>

#include "PHelp.h"

#include "PMsr2DataDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PMsr2DataDialog::PMsr2DataDialog(PMsr2DataParam *msr2DataParam, const QString helpUrl) : fMsr2DataParam(msr2DataParam), fHelpUrl(helpUrl)
{
  setupUi(this);

  setModal(true);

  QString str;

  fRunTag = -1;

  fFirst_lineEdit->setValidator( new QIntValidator(fFirst_lineEdit) );
  if (fMsr2DataParam->firstRun != -1) {
    str = QString("%1").arg(fMsr2DataParam->firstRun);
    fFirst_lineEdit->setText(str);
  }

  fLast_lineEdit->setValidator( new QIntValidator(fLast_lineEdit) );
  if (fMsr2DataParam->lastRun != -1) {
    str = QString("%1").arg(fMsr2DataParam->lastRun);
    fLast_lineEdit->setText(str);
  }

  if (!fMsr2DataParam->runListFileName.isEmpty()) {
    fRunListFileName_lineEdit->setText(fMsr2DataParam->runListFileName);
  }

  if (!fMsr2DataParam->runList.isEmpty()) {
    fRunList_lineEdit->setText(fMsr2DataParam->runList);
  }

  if (!fMsr2DataParam->msrFileExtension.isEmpty()) {
    fMsrFileExtension_lineEdit->setText(fMsr2DataParam->msrFileExtension);
  }

  fTemplateRunNumber_lineEdit->setValidator( new QIntValidator(fTemplateRunNumber_lineEdit) );
  if (fMsr2DataParam->templateRunNo != -1) {
    str = QString("%1").arg(fMsr2DataParam->templateRunNo);
    fTemplateRunNumber_lineEdit->setText(str);
  }

  if (!fMsr2DataParam->dbOutputFileName.isEmpty()) {
    fDataOutputFileName_lineEdit->setText(fMsr2DataParam->dbOutputFileName);
  }

  fWriteDataHeader_checkBox->setChecked(fMsr2DataParam->writeDbHeader);
  fSummaryPresent_checkBox->setChecked(fMsr2DataParam->summaryFilePresent);
  fKeepMinuit2Output_checkBox->setChecked(fMsr2DataParam->keepMinuit2Output);
  fWriteColumnData_checkBox->setChecked(fMsr2DataParam->writeColumnData);
  fRecreateDataFile_checkBox->setChecked(fMsr2DataParam->recreateDbFile);
  fChainFit_checkBox->setChecked(fMsr2DataParam->chainFit);
  fOpenFilesAfterFitting_checkBox->setChecked(fMsr2DataParam->openFilesAfterFitting);
  fTitleFromData_checkBox->setChecked(fMsr2DataParam->titleFromDataFile);
  fCreateMsrFileOnly_checkBox->setChecked(fMsr2DataParam->createMsrFileOnly);
  fFitOnly_checkBox->setChecked(fMsr2DataParam->fitOnly);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PMsr2DataParam* PMsr2DataDialog::getMsr2DataParam()
{
  if (fFirst_lineEdit->text().isEmpty()) {
    fMsr2DataParam->firstRun = -1;
  } else {
    fMsr2DataParam->firstRun = fFirst_lineEdit->text().toInt();
  }
  if (fLast_lineEdit->text().isEmpty()) {
    fMsr2DataParam->lastRun  = -1;
  } else {
    fMsr2DataParam->lastRun  = fLast_lineEdit->text().toInt();
  }
  fMsr2DataParam->runList  = fRunList_lineEdit->text();
  fMsr2DataParam->runListFileName = fRunListFileName_lineEdit->text();
  fMsr2DataParam->msrFileExtension = fMsrFileExtension_lineEdit->text();
  if (fTemplateRunNumber_lineEdit->text().isEmpty()) {
    fMsr2DataParam->templateRunNo = -1;
  } else {
    fMsr2DataParam->templateRunNo = fTemplateRunNumber_lineEdit->text().toInt();
  }
  fMsr2DataParam->dbOutputFileName = fDataOutputFileName_lineEdit->text();
  fMsr2DataParam->writeDbHeader = fWriteDataHeader_checkBox->isChecked();
  fMsr2DataParam->summaryFilePresent = fSummaryPresent_checkBox->isChecked();
  fMsr2DataParam->keepMinuit2Output = fKeepMinuit2Output_checkBox->isChecked();
  fMsr2DataParam->writeColumnData = fWriteColumnData_checkBox->isChecked();
  fMsr2DataParam->recreateDbFile = fRecreateDataFile_checkBox->isChecked();
  fMsr2DataParam->chainFit = fChainFit_checkBox->isChecked();
  fMsr2DataParam->openFilesAfterFitting = fOpenFilesAfterFitting_checkBox->isChecked();
  fMsr2DataParam->titleFromDataFile = fTitleFromData_checkBox->isChecked();
  fMsr2DataParam->createMsrFileOnly = fCreateMsrFileOnly_checkBox->isChecked();
  fMsr2DataParam->fitOnly = fFitOnly_checkBox->isChecked();

  return fMsr2DataParam;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMsr2DataDialog::runFirstEntered(const QString &str)
{

  if (str.length() == 0)
    return;

  fRunTag = 0;

  if (!fRunList_lineEdit->text().isEmpty())
    fRunList_lineEdit->clear();
  if (!fRunListFileName_lineEdit->text().isEmpty())
    fRunListFileName_lineEdit->clear();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMsr2DataDialog::runLastEntered(const QString &str)
{
  if (str.length() == 0)
    return;

  fRunTag = 0;

  if (!fRunList_lineEdit->text().isEmpty())
    fRunList_lineEdit->clear();
  if (!fRunListFileName_lineEdit->text().isEmpty())
    fRunListFileName_lineEdit->clear();

  if (fLast_lineEdit->text().length() == 1)
    fLast_lineEdit->update();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMsr2DataDialog::runListEntered(const QString &str)
{
  if (str.length() == 0)
    return;

  fRunTag = 1;

  if (!fFirst_lineEdit->text().isEmpty())
    fFirst_lineEdit->clear();
  if (!fLast_lineEdit->text().isEmpty())
    fLast_lineEdit->clear();
  if (!fRunListFileName_lineEdit->text().isEmpty())
    fRunListFileName_lineEdit->clear();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMsr2DataDialog::runListFileNameEntered(const QString &str)
{
  if (str.length() == 0)
    return;

  fRunTag = 2;

  if (!fFirst_lineEdit->text().isEmpty())
    fFirst_lineEdit->clear();
  if (!fLast_lineEdit->text().isEmpty())
    fLast_lineEdit->clear();
  if (!fRunList_lineEdit->text().isEmpty())
    fRunList_lineEdit->clear();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMsr2DataDialog::templateRunEntered(const QString &str)
{
  if (!str.isEmpty())
    fFitOnly_checkBox->setChecked(false);
  fTemplateRunNumber_lineEdit->setText(str);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMsr2DataDialog::createMsrFileOnlyChanged(int buttonState)
{
  if (buttonState == Qt::Checked) {
    fFitOnly_checkBox->setChecked(false);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMsr2DataDialog::fitOnlyChanged(int buttonState)
{
  if (buttonState == Qt::Checked) {
    fCreateMsrFileOnly_checkBox->setChecked(false);
    fTemplateRunNumber_lineEdit->clear();
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMsr2DataDialog::helpContent()
{
    if (fHelpUrl.isEmpty()) {
      QMessageBox::information(this, "**INFO**", "Will eventually show a help window");
    } else {
      PHelp *help = new PHelp(fHelpUrl);
      help->show();
    }
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
