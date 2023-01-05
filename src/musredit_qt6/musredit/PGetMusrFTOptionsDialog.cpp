/****************************************************************************

  PGetMusrFTOptionsDialog.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

*****************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2023 by Andreas Suter                              *
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
#include <QComboBox>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>

#include "PGetMusrFTOptionsDialog.h"

#define MUSRFT_OPT_UNDEF         0
#define MUSRFT_OPT_REAL          1
#define MUSRFT_OPT_IMAG          2
#define MUSRFT_OPT_REAL_AND_IMAG 3
#define MUSRFT_OPT_POWER         4
#define MUSRFT_OPT_PHASE         5

#define MUSRFT_APOD_UNDEF        0
#define MUSRFT_APOD_WEAK         1
#define MUSRFT_APOD_MEDIUM       2
#define MUSRFT_APOD_STRONG       3

#define MUSRFT_UNIT_UNDEF        0
#define MUSRFT_UNIT_GAUSS        1
#define MUSRFT_UNIT_TESLA        2
#define MUSRFT_UNIT_FREQ         3
#define MUSRFT_UNIT_CYCLE        4

//----------------------------------------------------------------------------------------------------
/**
 * <p>Constructor.
 *
 * \param currentMsrFile path-file name of the currently active msr-file in musredit
 * \param prevCmd list of the last (potential) previously call.
 * \param helpUrl help url for the asymmetry run block
 */
PGetMusrFTOptionsDialog::PGetMusrFTOptionsDialog(QString currentMsrFile, QStringList &prevCmd, const QString helpUrl) :
  fCurrentMsrFileName(currentMsrFile), fHelpUrl(helpUrl)
{
  setupUi(this);
  setModal(true);

  fBkgRangeStartBin_lineEdit->setValidator( new QIntValidator(fBkgRangeStartBin_lineEdit) );
  fBkgRangeEndBin_lineEdit->setValidator( new QIntValidator(fBkgRangeEndBin_lineEdit) );
  fPacking_lineEdit->setValidator( new QIntValidator(fPacking_lineEdit) );
  fTimeRangeStart_lineEdit->setValidator( new QDoubleValidator(fTimeRangeStart_lineEdit) );
  fTimeRangeEnd_lineEdit->setValidator( new QDoubleValidator(fTimeRangeEnd_lineEdit) );
  fFourierPower_lineEdit->setValidator( new QIntValidator(fFourierPower_lineEdit) );
  fLifetimeCorrection_lineEdit->setValidator( new QDoubleValidator(fLifetimeCorrection_lineEdit) );
  fFourierRangeStart_lineEdit->setValidator( new QDoubleValidator(fFourierRangeStart_lineEdit) );
  fFourierRangeEnd_lineEdit->setValidator( new QDoubleValidator(fFourierRangeEnd_lineEdit) );
  fPacking_lineEdit->setText("1");

  // populate dialog with the previous cmd call
  bool msrTag = true;
  QString str, str1, line;
  int idx;
  for (int i=0; i<prevCmd.size(); i++) {
    // collect msr-file-names
    if (msrTag) {
      line = "";
      while (prevCmd[i].contains(".msr") && (i<prevCmd.size())) {
        // split msr-file path-name into path and name
        str = prevCmd[i];
        idx = str.lastIndexOf("/");
        str1 = str;
        str.remove(idx, str.length()-idx);
        fMsrFilePaths << str;
        str1.remove(0, idx+1);
        fMsrFileNames << str1;
        line += str1 + " ";
        i++;
      }
      line.remove(line.length()-1, 1);
      fMsrFileSelector_lineEdit->setText(line);
      msrTag = false;
    }
    // collect data-file-names
    if (prevCmd[i] == "-df") {
      i++;
      line = "";
      while (!prevCmd[i].startsWith("-") && (i<prevCmd.size())) {
        // split msr-file path-name into path and name
        str = prevCmd[i];
        idx = str.lastIndexOf("/");
        str1 = str;
        str.remove(idx, str.length()-idx);
        fDataFilePaths << str;
        str1.remove(0, idx+1);
        fDataFileNames << str1;
        line += str1 + " ";
        i++;
      }
      line.remove(line.length()-1, 1);
      fDataFileSelector_lineEdit->setText(line);
    }
    // background-range
    if (prevCmd[i] == "-br") {
      fBkgRangeStartBin_lineEdit->setText(prevCmd[++i]);
      fBkgRangeEndBin_lineEdit->setText(prevCmd[++i]);
    }
    // fourier-option
    if (prevCmd[i] == "-fo") {
      if (prevCmd[i+1] == "real")
        fFourierOption_comboBox->setCurrentIndex(MUSRFT_OPT_REAL);
      else if (prevCmd[i+1] == "imag")
        fFourierOption_comboBox->setCurrentIndex(MUSRFT_OPT_IMAG);
      else if (prevCmd[i+1] == "real+imag")
        fFourierOption_comboBox->setCurrentIndex(MUSRFT_OPT_REAL_AND_IMAG);
      else if (prevCmd[i+1] == "power")
        fFourierOption_comboBox->setCurrentIndex(MUSRFT_OPT_POWER);
      else if (prevCmd[i+1] == "phase")
        fFourierOption_comboBox->setCurrentIndex(MUSRFT_OPT_PHASE);
      else
        fFourierOption_comboBox->setCurrentIndex(MUSRFT_OPT_UNDEF);
      i++;
    }
    // apodization
    if (prevCmd[i] == "-ap") {
      if (prevCmd[i+1] == "weak")
        fApodization_comboBox->setCurrentIndex(MUSRFT_APOD_WEAK);
      else if (prevCmd[i+1] == "medium")
        fApodization_comboBox->setCurrentIndex(MUSRFT_APOD_MEDIUM);
      else if (prevCmd[i+1] == "strong")
        fApodization_comboBox->setCurrentIndex(MUSRFT_APOD_STRONG);
      else
        fApodization_comboBox->setCurrentIndex(MUSRFT_APOD_UNDEF);
      i++;
    }
    // fourier-power
    if (prevCmd[i] == "-fp") {
      fFourierPower_lineEdit->setText(prevCmd[++i]);
    }
    // units
    if (prevCmd[i] == "-u") {
      if (prevCmd[i+1] == "Gauss")
        fFourierUnits_comboBox->setCurrentIndex(MUSRFT_UNIT_GAUSS);
      else if (prevCmd[i+1] == "Tesla")
        fFourierUnits_comboBox->setCurrentIndex(MUSRFT_UNIT_TESLA);
      else if (prevCmd[i+1] == "MHz")
        fFourierUnits_comboBox->setCurrentIndex(MUSRFT_UNIT_FREQ);
      else if (prevCmd[i+1] == "Mc/s")
        fFourierUnits_comboBox->setCurrentIndex(MUSRFT_UNIT_CYCLE);
      else
        fFourierUnits_comboBox->setCurrentIndex(MUSRFT_UNIT_UNDEF);
      i++;
    }
    // phase
    if (prevCmd[i] == "-ph") {
      // NOT YET IMPLEMENTED
    }
    // fourier-range
    if (prevCmd[i] == "-fr") {
      fFourierRangeStart_lineEdit->setText(prevCmd[++i]);
      fFourierRangeEnd_lineEdit->setText(prevCmd[++i]);
    }
    // time-range
    if (prevCmd[i] == "-tr") {
      fTimeRangeStart_lineEdit->setText(prevCmd[++i]);
      fTimeRangeEnd_lineEdit->setText(prevCmd[++i]);
    }
    // histo list
    if (prevCmd[i] == "--histo") {
      i++;
      line = "";
      while (!prevCmd[i].startsWith("-") && (i<prevCmd.size())) {
        line += prevCmd[i++] + " ";
      }
      line.remove(line.length()-1, 1);
      fHistoList_lineEdit->setText(line);
    }
    // average ALL tag
    if (prevCmd[i] == "-a") {
      fAveragedView_checkBox->setCheckState(Qt::Checked);
    }
    // average per data set tag
    if (prevCmd[i] == "-ad") {
      fAveragePerDataSet_checkBox->setCheckState(Qt::Checked);
    }
    // t0 list
    if (prevCmd[i] == "--t0") {
      i++;
      line = "";
      while (!prevCmd[i].startsWith("-") && (i<prevCmd.size())) {
        line += prevCmd[i++] + " ";
      }
      line.remove(line.length()-1, 1);
      fT0_lineEdit->setText(line);
    }
    // packing
    if (prevCmd[i] == "-pa") {
      fPacking_lineEdit->setText(prevCmd[++i]);
    }
    // title
    if (prevCmd[i] == "--title") {
      fFourierTitle_lineEdit->setText(prevCmd[i+1]);
    }
    // create msr-file tag
    if (prevCmd[i] == "--create-msr-file") {
      fCreateMsrFileName = prevCmd[++i];
      fCreateMsrFile_checkBox->setCheckState(Qt::Checked);
    }
    // lifetime correction
    if (prevCmd[i] == "-lc") {
      fLifetimeCorrection_lineEdit->setText(prevCmd[++i]);
    }
  }

  // connect all necessary single and slots
  connect(fCurrentMsrFile_checkBox, SIGNAL( stateChanged(int) ), this, SLOT( currentMsrFileTagChanged(int) ) );
  connect(fAllMsrFiles_checkBox, SIGNAL( stateChanged(int) ), this, SLOT( allMsrFileTagChanged(int) ) );
  connect(fMsrFileSelector_pushButton, SIGNAL( clicked() ), this, SLOT( selectMsrFileNames() ) );
  connect(fDataFileSelector_pushButton, SIGNAL( clicked() ), this, SLOT( selectDataFileNames() ) );
  connect(fCreateMsrFile_checkBox, SIGNAL( stateChanged(int) ), this, SLOT( createMsrFileChanged(int) ) );
  connect(fMsrFileNameClear_pushButton, SIGNAL (clicked() ), this, SLOT( clearMsrFileNames() ) );
  connect(fDataFileNameClear_pushButton, SIGNAL (clicked() ), this, SLOT( clearDataFileNames() ) );
  connect(fResetAll_pushButton, SIGNAL( clicked() ), this, SLOT( resetAll() ) );
  connect(fAveragedView_checkBox, SIGNAL ( stateChanged(int) ), this, SLOT( averagedAll(int) ) );
  connect(fAveragePerDataSet_checkBox, SIGNAL ( stateChanged(int) ), this, SLOT( averagedPerDataSet(int) ) );
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>returns the musrFT command line options.
 */
QStringList PGetMusrFTOptionsDialog::getMusrFTOptions()
{
  QStringList cmd;
  QString str("");
  QStringList strList;

  // check if currently active msr-file shall be used
  if (fCurrentMsrFile_checkBox->checkState() == Qt::Checked) {
    cmd << fCurrentMsrFileName;
  } else {
    // msr-files
    for (int i=0; i<fMsrFilePaths.size(); i++) {
      str = fMsrFilePaths[i]+"/"+fMsrFileNames[i];
      cmd << str;
    }

    // data-files
    if (fDataFilePaths.size() > 0) {
      cmd << "-df";
      for (int i=0; i<fDataFilePaths.size(); i++) {
        str = fDataFilePaths[i]+"/"+fDataFileNames[i];
        cmd << str;
      }
    }
  }

  // background range
  if ((fBkgRangeStartBin_lineEdit->text().length() > 0) && (fBkgRangeEndBin_lineEdit->text().length() > 0)) {
    cmd << "-br";
    cmd << fBkgRangeStartBin_lineEdit->text();
    cmd << fBkgRangeEndBin_lineEdit->text();
  }

  // background values
  if (fBkgList_lineEdit->text().length() > 0) {

  }

  // fourier-option
  if (fFourierOption_comboBox->currentText() != "UnDef") {
    cmd << "-fo";
    cmd << fFourierOption_comboBox->currentText();
  }

  // apodization
  cmd << "-ap";
  cmd << fApodization_comboBox->currentText();

  // fourier-power
  if (fFourierPower_lineEdit->text().length() > 0) {
    cmd << "-fp";
    cmd << fFourierPower_lineEdit->text();
  }

  // units
  if (fFourierUnits_comboBox->currentText() != "UnDef") {
    cmd << "-u";
    cmd << fFourierUnits_comboBox->currentText();
  }

  // phase

  // fourier-range
  if ((fFourierRangeStart_lineEdit->text().length() > 0) && (fFourierRangeEnd_lineEdit->text().length() > 0)) {
    cmd << "-fr";
    cmd << fFourierRangeStart_lineEdit->text();
    cmd << fFourierRangeEnd_lineEdit->text();
  }

  // time-range
  if ((fTimeRangeStart_lineEdit->text().length() > 0) && (fTimeRangeEnd_lineEdit->text().length() > 0)) {
    cmd << "-tr";
    cmd << fTimeRangeStart_lineEdit->text();
    cmd << fTimeRangeEnd_lineEdit->text();
  }

  // histo list
  if (fHistoList_lineEdit->text().length() > 0) {
    cmd << "--histo";
    strList = fHistoList_lineEdit->text().split(" ", Qt::SkipEmptyParts);
    for (int i=0; i<strList.size(); i++)
      cmd << strList[i];
  }

  // averaged view ALL
  if (fAveragedView_checkBox->checkState() == Qt::Checked)
    cmd << "-a";

  // averaged view per data set
  if (fAveragePerDataSet_checkBox->checkState() == Qt::Checked)
    cmd << "-ad";

  // t0 list
  if (fT0_lineEdit->text().length() > 0) {
    cmd << "--t0";
    strList = fT0_lineEdit->text().split(" ", Qt::SkipEmptyParts);
    for (int i=0; i<strList.size(); i++)
      cmd << strList[i];
  }

  // packing
  if (fPacking_lineEdit->text().length() > 0) {
    cmd << "-pa";
    cmd << fPacking_lineEdit->text();
  }

  // title
  if (fFourierTitle_lineEdit->text().length() > 0) {
    cmd << "--title";
    cmd << fFourierTitle_lineEdit->text();
  }

  // create-msr-file
  if (fCreateMsrFile_checkBox->checkState() == Qt::Checked) {
    cmd << "--create-msr-file";
    cmd << fCreateMsrFileName;
  }

  // lifetimecorrection
  if (fLifetimeCorrection_lineEdit->text().length() > 0) {
    cmd << "-lc";
    cmd << fLifetimeCorrection_lineEdit->text();
  }

  return cmd;
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT called when fCurrentMsrFile_checkBox is checked/unchecked. If checked, it will clean up
 * the msr-file path-file list and the data-file path-file list.
 */
void PGetMusrFTOptionsDialog::currentMsrFileTagChanged(int state)
{
  if (state == Qt::Checked) {
    fAllMsrFiles_checkBox->setCheckState(Qt::Unchecked);

    // remove all msr-data-file-names and data-path-file-names
    fMsrFilePaths.clear();
    fMsrFileNames.clear();
    fMsrFileSelector_lineEdit->setText("");
    fDataFilePaths.clear();
    fDataFileNames.clear();
    fDataFileSelector_lineEdit->setText("");
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT called when fAllMsrFiles_checkBox is checked/unchecked. Currently it has no functionality.
 */
void PGetMusrFTOptionsDialog::allMsrFileTagChanged(int state)
{
  if (state == Qt::Checked) {
    fCurrentMsrFile_checkBox->setCheckState(Qt::Unchecked);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT called when fMsrFileSelector_pushButton is clicked. Collects the msr-file path-name list.
 */
void PGetMusrFTOptionsDialog::selectMsrFileNames()
{
  QStringList flns = QFileDialog::getOpenFileNames( this, tr("Open msr-File(s)"), tr( "./" ),
                                                    tr( "msr-Files (*.msr);;All Files (*)" ));

  QString str(""), str1("");
  int idx;
  if (flns.size() > 0) {
    // delete already present elements
    fMsrFilePaths.clear();
    fMsrFileNames.clear();

    // split path-name into path and name
    for (int i=0; i<flns.size(); i++) {
      str = flns[i];
      idx = str.lastIndexOf("/");
      str1 = str;
      str.remove(idx, str.length()-idx);
      fMsrFilePaths << str;
      str1.remove(0, idx+1);
      fMsrFileNames << str1;
    }

    // populate fMsrFileSelector_lineEdit
    str = QString("");
    for (int i=0; i<fMsrFileNames.size()-1; i++) {
      str += fMsrFileNames[i] + " ";
    }
    str += fMsrFileNames[fMsrFileNames.size()-1];

    fMsrFileSelector_lineEdit->setText(str);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT called when the fMsrFileNameClear_pushButton is clicked.
 */
void PGetMusrFTOptionsDialog::clearMsrFileNames()
{
  fMsrFileSelector_lineEdit->setText("");
  fMsrFilePaths.clear();
  fMsrFileNames.clear();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT called when fDataFileSelector_pushButton is clicked. Collects the data-file path-name list.
 */
void PGetMusrFTOptionsDialog::selectDataFileNames()
{
  QStringList flns = QFileDialog::getOpenFileNames( this, tr("Open msr-File(s)"), tr( "./" ),
                                                    tr( "data-Files (*.root *.bin *.msr *.nxs *.mdu);;All Files (*)" ));

  QString str(""), str1("");
  int idx;
  if (flns.size() > 0) {
    // delete already present elements
    fDataFilePaths.clear();
    fDataFileNames.clear();

    // split path-name into path and name
    for (int i=0; i<flns.size(); i++) {
      str = flns[i];
      idx = str.lastIndexOf("/");
      str1 = str;
      str.remove(idx, str.length()-idx);
      fDataFilePaths << str;
      str1.remove(0, idx+1);
      fDataFileNames << str1;
    }

    // populate fMsrFileSelector_lineEdit
    str = QString("");
    for (int i=0; i<fDataFileNames.size()-1; i++) {
      str += fDataFileNames[i] + " ";
    }
    str += fDataFileNames[fDataFileNames.size()-1];

    fDataFileSelector_lineEdit->setText(str);
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT called when the fDataFileNameClear_pushButton is clicked.
 */
void PGetMusrFTOptionsDialog::clearDataFileNames()
{
  fDataFileSelector_lineEdit->setText("");
  fDataFilePaths.clear();
  fDataFileNames.clear();
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT called when the create-msr-file tag has changed
 */
void PGetMusrFTOptionsDialog::createMsrFileChanged(int state)
{
  if (state == Qt::Checked) {
    fCreateMsrFileName = QFileDialog::getSaveFileName(this, tr("Create msr-file"), "./", tr("msr-Files (*.msr);;All Files (*)"));
    if (fCreateMsrFileName.length() == 0)
      fCreateMsrFile_checkBox->setCheckState(Qt::Unchecked);
  } else {
    fCreateMsrFileName = "";
  }
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT called when the Reset All button is pressed. Will remove all settings.
 */
void PGetMusrFTOptionsDialog::resetAll()
{
  fCurrentMsrFile_checkBox->setCheckState(Qt::Unchecked);
  fAllMsrFiles_checkBox->setCheckState(Qt::Unchecked);
  fMsrFilePaths.clear();
  fMsrFileNames.clear();
  fMsrFileSelector_lineEdit->setText("");
  fDataFilePaths.clear();
  fDataFileNames.clear();
  fDataFileSelector_lineEdit->setText("");
  fBkgRangeStartBin_lineEdit->setText("");
  fBkgRangeEndBin_lineEdit->setText("");
  fBkgList_lineEdit->setText("");
  fApodization_comboBox->setCurrentIndex(MUSRFT_APOD_UNDEF);
  fPacking_lineEdit->setText("");
  fTimeRangeStart_lineEdit->setText("");
  fTimeRangeEnd_lineEdit->setText("");
  fHistoList_lineEdit->setText("");
  fT0_lineEdit->setText("");
  fFourierOption_comboBox->setCurrentIndex(MUSRFT_OPT_UNDEF);
  fFourierUnits_comboBox->setCurrentIndex(MUSRFT_UNIT_UNDEF);
  fFourierPower_lineEdit->setText("");
  fLifetimeCorrection_lineEdit->setText("");
  fFourierRangeStart_lineEdit->setText("");
  fFourierRangeEnd_lineEdit->setText("");
  fAveragedView_checkBox->setCheckState(Qt::Unchecked);
  fAveragePerDataSet_checkBox->setCheckState(Qt::Unchecked);
  fCreateMsrFile_checkBox->setCheckState(Qt::Unchecked);
  fFourierTitle_lineEdit->setText("");
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT called when averaged view for ALL data is checked.
 */
void PGetMusrFTOptionsDialog::averagedAll(int state)
{
  if ((state == Qt::Checked) && fAveragePerDataSet_checkBox->isChecked())
    fAveragePerDataSet_checkBox->setCheckState(Qt::Unchecked);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>SLOT called when averaged view per data set is checked.
 */
void PGetMusrFTOptionsDialog::averagedPerDataSet(int state)
{
  if ((state == Qt::Checked) && fAveragedView_checkBox->isChecked())
    fAveragedView_checkBox->setCheckState(Qt::Unchecked);
}

//----------------------------------------------------------------------------------------------------
/**
 * <p>Generates a help content window showing the description for musrFT.
 */
void PGetMusrFTOptionsDialog::helpContent()
{
  if (fHelpUrl.isEmpty()) {
    QMessageBox::information(this, "INFO", "Will eventually show a help window");
  } else {
    bool ok = QDesktopServices::openUrl(QUrl(fHelpUrl, QUrl::TolerantMode));
    if (!ok) {
      QString msg = QString("<p>Sorry: Couldn't open default web-browser for the help.<br>Please try: <a href=\"%1\">musrfit docu</a> in your web-browser.").arg(fHelpUrl);
      QMessageBox::critical( nullptr, tr("FATAL ERROR"), msg, QMessageBox::Close );
    }
  }
}
