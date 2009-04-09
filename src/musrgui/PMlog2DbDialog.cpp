/****************************************************************************

  PMlog2DbDialog.cpp

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

#include "PMlog2DbDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PMlog2DbDialog::PMlog2DbDialog(PMlog2DbDataSet *mlog2DbDataSet) : fMlog2DbDataSet(mlog2DbDataSet)
{
  QString str;

  fRunTag = -1;

  fFirst_lineEdit->setValidator( new QIntValidator(fFirst_lineEdit) );
  if (fMlog2DbDataSet->firstRun != -1) {
    str = QString("%1").arg(fMlog2DbDataSet->firstRun);
    fFirst_lineEdit->setText(str);
  }

  fLast_lineEdit->setValidator( new QIntValidator(fLast_lineEdit) );
  if (fMlog2DbDataSet->lastRun != -1) {
    str = QString("%1").arg(fMlog2DbDataSet->lastRun);
    fLast_lineEdit->setText(str);
  }

  if (!fMlog2DbDataSet->runListFileName.isEmpty()) {
    fRunListFileName_lineEdit->setText(fMlog2DbDataSet->runListFileName);
  }

  if (!fMlog2DbDataSet->runList.isEmpty()) {
    fRunList_lineEdit->setText(fMlog2DbDataSet->runList);
  }

  if (!fMlog2DbDataSet->msrFileExtension.isEmpty()) {
    fExtension_lineEdit->setText(fMlog2DbDataSet->msrFileExtension);
  }

  fTemplateRunNumber_lineEdit->setValidator( new QIntValidator(fTemplateRunNumber_lineEdit) );
  if (fMlog2DbDataSet->templateRunNo != -1) {
    str = QString("%1").arg(fMlog2DbDataSet->templateRunNo);
    fTemplateRunNumber_lineEdit->setText(str);
  }

  if (!fMlog2DbDataSet->dbOutputFileName.isEmpty()) {
    fDbOutputFileName_lineEdit->setText(fMlog2DbDataSet->dbOutputFileName);
  }

  fWriteDbHeader_checkBox->setChecked(fMlog2DbDataSet->writeDbHeader);
  fSummaryPresent_checkBox->setChecked(fMlog2DbDataSet->summaryFilePresent);
  fKeepMinuit2Output_checkBox->setChecked(fMlog2DbDataSet->keepMinuit2Output);
  fWriteColumnData_checkBox->setChecked(fMlog2DbDataSet->writeColumnData);
  fRecreateDbFile_checkBox->setChecked(fMlog2DbDataSet->recreateDbFile);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
PMlog2DbDataSet* PMlog2DbDialog::getMlog2DbDataSet()
{
  if (fFirst_lineEdit->text().isEmpty()) {
    fMlog2DbDataSet->firstRun = -1;
  } else {
    fMlog2DbDataSet->firstRun = fFirst_lineEdit->text().toInt();
  }
  if (fLast_lineEdit->text().isEmpty()) {
    fMlog2DbDataSet->lastRun  = -1;
  } else {
    fMlog2DbDataSet->lastRun  = fLast_lineEdit->text().toInt();
  }
  fMlog2DbDataSet->runList  = fRunList_lineEdit->text();
  fMlog2DbDataSet->runListFileName = fRunListFileName_lineEdit->text();
  fMlog2DbDataSet->msrFileExtension = fExtension_lineEdit->text();
  if (fTemplateRunNumber_lineEdit->text().isEmpty()) {
    fMlog2DbDataSet->templateRunNo = -1;
  } else {
    fMlog2DbDataSet->templateRunNo = fTemplateRunNumber_lineEdit->text().toInt();
  }
  fMlog2DbDataSet->dbOutputFileName = fDbOutputFileName_lineEdit->text();
  fMlog2DbDataSet->writeDbHeader = fWriteDbHeader_checkBox->isChecked();
  fMlog2DbDataSet->summaryFilePresent = fSummaryPresent_checkBox->isChecked();
  fMlog2DbDataSet->keepMinuit2Output = fKeepMinuit2Output_checkBox->isChecked();
  fMlog2DbDataSet->writeColumnData = fWriteColumnData_checkBox->isChecked();
  fMlog2DbDataSet->recreateDbFile = fRecreateDbFile_checkBox->isChecked();

  return fMlog2DbDataSet;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>
 */
void PMlog2DbDialog::runFirstLastEntered()
{
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
void PMlog2DbDialog::runListEntered()
{
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
void PMlog2DbDialog::runListFileNameEntered()
{
  fRunTag = 2;

  if (!fFirst_lineEdit->text().isEmpty())
    fFirst_lineEdit->clear();
  if (!fLast_lineEdit->text().isEmpty())
    fLast_lineEdit->clear();
  if (!fRunList_lineEdit->text().isEmpty())
    fRunList_lineEdit->clear();
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
