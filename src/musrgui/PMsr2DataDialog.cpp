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

#include <qlineedit.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qtextedit.h>
#include <qcheckbox.h>

#include "PMsr2DataDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PMsr2DataDialog::PMsr2DataDialog(PMsr2DataParam *msr2DataParam) : fMsr2DataParam(msr2DataParam)
{
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
    fExtension_lineEdit->setText(fMsr2DataParam->msrFileExtension);
  }

  fTemplateRunNumber_lineEdit->setValidator( new QIntValidator(fTemplateRunNumber_lineEdit) );
  if (fMsr2DataParam->templateRunNo != -1) {
    str = QString("%1").arg(fMsr2DataParam->templateRunNo);
    fTemplateRunNumber_lineEdit->setText(str);
  }

  if (!fMsr2DataParam->dbOutputFileName.isEmpty()) {
    fDbOutputFileName_lineEdit->setText(fMsr2DataParam->dbOutputFileName);
  }

  fWriteDbHeader_checkBox->setChecked(fMsr2DataParam->writeDbHeader);
  fSummaryPresent_checkBox->setChecked(fMsr2DataParam->summaryFilePresent);
  fKeepMinuit2Output_checkBox->setChecked(fMsr2DataParam->keepMinuit2Output);
  fWriteColumnData_checkBox->setChecked(fMsr2DataParam->writeColumnData);
  fRecreateDbFile_checkBox->setChecked(fMsr2DataParam->recreateDbFile);
  fChainFit_checkBox->setChecked(fMsr2DataParam->chainFit);
  fOpenAfterFitting_checkBox->setChecked(fMsr2DataParam->openFilesAfterFitting);
  fTitleFromData_checkBox->setChecked(fMsr2DataParam->titleFromDataFile);
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
  fMsr2DataParam->msrFileExtension = fExtension_lineEdit->text();
  if (fTemplateRunNumber_lineEdit->text().isEmpty()) {
    fMsr2DataParam->templateRunNo = -1;
  } else {
    fMsr2DataParam->templateRunNo = fTemplateRunNumber_lineEdit->text().toInt();
  }
  fMsr2DataParam->dbOutputFileName = fDbOutputFileName_lineEdit->text();
  fMsr2DataParam->writeDbHeader = fWriteDbHeader_checkBox->isChecked();
  fMsr2DataParam->summaryFilePresent = fSummaryPresent_checkBox->isChecked();
  fMsr2DataParam->keepMinuit2Output = fKeepMinuit2Output_checkBox->isChecked();
  fMsr2DataParam->writeColumnData = fWriteColumnData_checkBox->isChecked();
  fMsr2DataParam->recreateDbFile = fRecreateDbFile_checkBox->isChecked();
  fMsr2DataParam->chainFit = fChainFit_checkBox->isChecked();
  fMsr2DataParam->openFilesAfterFitting = fOpenAfterFitting_checkBox->isChecked();
  fMsr2DataParam->titleFromDataFile = fTitleFromData_checkBox->isChecked();

  return fMsr2DataParam;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMsr2DataDialog::runFirstEntered(const QString &str)
{
  fRunTag = 0;

  if (!fRunList_lineEdit->text().isEmpty())
    fRunList_lineEdit->clear();
  if (!fRunListFileName_lineEdit->text().isEmpty())
    fRunListFileName_lineEdit->clear();

  fFirst_lineEdit->setText(str);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMsr2DataDialog::runLastEntered(const QString &str)
{
  fRunTag = 0;

  if (!fRunList_lineEdit->text().isEmpty())
    fRunList_lineEdit->clear();
  if (!fRunListFileName_lineEdit->text().isEmpty())
    fRunListFileName_lineEdit->clear();

  fLast_lineEdit->setText(str);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMsr2DataDialog::runListEntered(const QString &str)
{
  fRunTag = 1;

  if (!fFirst_lineEdit->text().isEmpty())
    fFirst_lineEdit->clear();
  if (!fLast_lineEdit->text().isEmpty())
    fLast_lineEdit->clear();
  if (!fRunListFileName_lineEdit->text().isEmpty())
    fRunListFileName_lineEdit->clear();

  fRunList_lineEdit->setText(str);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMsr2DataDialog::runListFileNameEntered(const QString &str)
{
  fRunTag = 2;

  if (!fFirst_lineEdit->text().isEmpty())
    fFirst_lineEdit->clear();
  if (!fLast_lineEdit->text().isEmpty())
    fLast_lineEdit->clear();
  if (!fRunList_lineEdit->text().isEmpty())
    fRunList_lineEdit->clear();

  fRunListFileName_lineEdit->setText(str);
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
