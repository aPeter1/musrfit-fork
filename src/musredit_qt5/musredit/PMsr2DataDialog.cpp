/****************************************************************************

  PMsr2DataDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2021 by Andreas Suter                              *
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
#include <QDesktopServices>
#include <QUrl>

#include "PMsr2DataDialog.h"

//----------------------------------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param msr2DataParam data structure keeping the necessary information to feed msr2data
 * \param helpUrl help url for msr2data
 */
PMsr2DataDialog::PMsr2DataDialog(PMsr2DataParam *msr2DataParam, const QString helpUrl) : fMsr2DataParam(msr2DataParam), fHelpUrl(helpUrl)
{
  setupUi(this);

  setModal(true);

  QString str;

  fRunTag = -1;

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

  if (!fMsr2DataParam->paramList.isEmpty()) {
    fParamList_lineEdit->setText(fMsr2DataParam->paramList);
  }

  fWriteDataHeader_checkBox->setChecked(fMsr2DataParam->writeDbHeader);
  fIgnoreDataHeaderInfo_checkBox->setChecked(fMsr2DataParam->ignoreDataHeaderInfo);
  fKeepMinuit2Output_checkBox->setChecked(fMsr2DataParam->keepMinuit2Output);
  fEstimateN0_checkBox->setChecked(fMsr2DataParam->estimateN0);
  fWriteColumnData_checkBox->setChecked(fMsr2DataParam->writeColumnData);
  fRecreateDataFile_checkBox->setChecked(fMsr2DataParam->recreateDbFile);
  fChainFit_checkBox->setChecked(fMsr2DataParam->chainFit);
  fOpenFilesAfterFitting_checkBox->setChecked(fMsr2DataParam->openFilesAfterFitting);
  fWritePerRunBlockChisq_checkBox->setChecked(fMsr2DataParam->perRunBlockChisq);
  fTitleFromData_checkBox->setChecked(fMsr2DataParam->titleFromDataFile);
  fCreateMsrFileOnly_checkBox->setChecked(fMsr2DataParam->createMsrFileOnly);
  fFitOnly_checkBox->setChecked(fMsr2DataParam->fitOnly);
  fGlobal_checkBox->setChecked(fMsr2DataParam->global);
  fGlobalPlus_checkBox->setChecked(fMsr2DataParam->globalPlus);

  fOk_pushButton->setDefault(true);

  connect(fGlobal_checkBox, SIGNAL(clicked(bool)), this, SLOT(globalOptionSet(bool)));
  connect(fGlobalPlus_checkBox, SIGNAL(clicked(bool)), this, SLOT(globalPlusOptionSet(bool)));
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns the msr2data relevant parameters from the GUI.
 */
PMsr2DataParam* PMsr2DataDialog::getMsr2DataParam()
{
  fMsr2DataParam->runList  = fRunList_lineEdit->text();
  fMsr2DataParam->runListFileName = fRunListFileName_lineEdit->text();
  fMsr2DataParam->msrFileExtension = fMsrFileExtension_lineEdit->text();
  if (fTemplateRunNumber_lineEdit->text().isEmpty()) {
    fMsr2DataParam->templateRunNo = -1;
  } else {
    fMsr2DataParam->templateRunNo = fTemplateRunNumber_lineEdit->text().toInt();
  }
  fMsr2DataParam->paramList = fParamList_lineEdit->text();
  fMsr2DataParam->dbOutputFileName = fDataOutputFileName_lineEdit->text();
  fMsr2DataParam->writeDbHeader = fWriteDataHeader_checkBox->isChecked();
  fMsr2DataParam->ignoreDataHeaderInfo = fIgnoreDataHeaderInfo_checkBox->isChecked();
  fMsr2DataParam->keepMinuit2Output = fKeepMinuit2Output_checkBox->isChecked();
  fMsr2DataParam->estimateN0 = fEstimateN0_checkBox->isChecked();
  fMsr2DataParam->writeColumnData = fWriteColumnData_checkBox->isChecked();
  fMsr2DataParam->recreateDbFile = fRecreateDataFile_checkBox->isChecked();
  fMsr2DataParam->chainFit = fChainFit_checkBox->isChecked();
  fMsr2DataParam->openFilesAfterFitting = fOpenFilesAfterFitting_checkBox->isChecked();
  fMsr2DataParam->perRunBlockChisq = fWritePerRunBlockChisq_checkBox->isChecked();
  fMsr2DataParam->titleFromDataFile = fTitleFromData_checkBox->isChecked();
  fMsr2DataParam->createMsrFileOnly = fCreateMsrFileOnly_checkBox->isChecked();
  fMsr2DataParam->fitOnly = fFitOnly_checkBox->isChecked();
  fMsr2DataParam->global = fGlobal_checkBox->isChecked();
  fMsr2DataParam->globalPlus = fGlobalPlus_checkBox->isChecked();

  return fMsr2DataParam;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: called when in the 'Run List Input', the 'Run List' text field is activated. It clears any
 * entries in 'First', 'Last', and 'Run List File Name'. It furthermore sets the run tag.
 *
 * \param str string content of the QTextEdit field.
 */
void PMsr2DataDialog::runListEntered(const QString &str)
{
  if (str.length() == 0)
    return;

  fRunTag = 0;

  if (!fRunListFileName_lineEdit->text().isEmpty())
    fRunListFileName_lineEdit->clear();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: called when in the 'Run List Input', the 'Run List File Name' text field is activated. It clears any
 * entries in 'First', 'Last', and 'Run List'. It furthermore sets the run tag.
 *
 * \param str string content of the QTextEdit field.
 */
void PMsr2DataDialog::runListFileNameEntered(const QString &str)
{
  if (str.length() == 0)
    return;

  fRunTag = 1;

  if (!fRunList_lineEdit->text().isEmpty())
    fRunList_lineEdit->clear();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: called when in 'Template Run Input', the 'Template Run Number' text field is activated.
 * It set at the same time the fit-only flag to false.
 *
 * \param str string content of the QTextEdit field.
 */
void PMsr2DataDialog::templateRunEntered(const QString &str)
{
  if (!str.isEmpty())
    fFitOnly_checkBox->setChecked(false);
  fTemplateRunNumber_lineEdit->setText(str);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: called when the 'Create msr-File only' QCheckBox under 'Options' is activated.
 * Sets at the same time the fit-only flag to false.
 *
 * \param buttonState state of the button.
 */
void PMsr2DataDialog::createMsrFileOnlyChanged(int buttonState)
{
  if (buttonState == Qt::Checked) {
    fFitOnly_checkBox->setChecked(false);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT: called when the 'Fit Only' QCheckBox under 'Options' is activated.
 * Sets at the same time the create-msr-file-only and global flags to false. Clears the 'Template Run Number'
 * field.
 *
 * \param buttonState state of the button.
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
 * <p>Generates a help content window showing the description for msr2data.
 */
void PMsr2DataDialog::helpContent()
{
    if (fHelpUrl.isEmpty()) {
      QMessageBox::information(this, "**INFO**", "Will eventually show a help window");
    } else {
      bool ok = QDesktopServices::openUrl(QUrl(fHelpUrl, QUrl::TolerantMode));
      if (!ok) {
        QString msg = QString("<p>Sorry: Couldn't open default web-browser for the help.<br>Please try: <a href=\"%1\">musrfit docu</a> in your web-browser.").arg(fHelpUrl);
        QMessageBox::critical( nullptr,
                               tr("Fatal Error"),
                               msg,
                               tr("Quit") );
      }
    }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Unchecks global+ if global is checked since global/global+ are excluding options
 *
 * \param checked true, if the check-box is checked
 */
void PMsr2DataDialog::globalOptionSet(bool checked)
{
  if (checked) {
    if (fGlobalPlus_checkBox->isChecked())
      fGlobalPlus_checkBox->setChecked(false);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Unchecks global if global+ is checked  since global/global+ are excluding options
 *
 * \param checked true, if the check-box is checked
 */
void PMsr2DataDialog::globalPlusOptionSet(bool checked)
{
  if (checked) {
    if (fGlobal_checkBox->isChecked())
      fGlobal_checkBox->setChecked(false);
  }
}

//----------------------------------------------------------------------------------------------------
// END
//----------------------------------------------------------------------------------------------------
