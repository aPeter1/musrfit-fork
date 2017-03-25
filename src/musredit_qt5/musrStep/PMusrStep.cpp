/***************************************************************************

  PMusrStep.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2016 by Andreas Suter                              *
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

#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QByteArray>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QTextStream>
#include <QPixmap>

#include <QDebug>

#include "PMusrStep.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 *
 */
PModSelect::PModSelect(QWidget *parent) :
  QDialog(parent)
{
  setWindowTitle("Modify Selected");

  fScaleByFactor = new QPushButton("Scale by &Factor");
  fScaleByFactor->setWhatsThis("if pressed it will use the Factor value, and the absolut value selection to change the selected parameter steps.");
  fFactorLabel = new QLabel("Factor");
  fFactorLineEdit = new QLineEdit("0.01");
  fFactorLineEdit->setValidator(new QDoubleValidator);
  fAbsVal = new QCheckBox("Absolute Value");
  fAbsVal->setWhatsThis("if checked, the factor is used as an absolut value rather than a multiplication factor for the selected steps.");

  QHBoxLayout *top = new QHBoxLayout;
  top->addWidget(fScaleByFactor);
  top->addWidget(fFactorLabel);
  top->addWidget(fFactorLineEdit);
  top->addWidget(fAbsVal);

  fScaleAutomatic = new QPushButton("Scale &Automatically");
  fScaleAutomatic->setWhatsThis("Will try to reset the step size of the selected items based on some crude rules");
  fCancel = new QPushButton("&Cancel");

  QHBoxLayout *bottom = new QHBoxLayout;
  bottom->addWidget(fScaleAutomatic);
  bottom->addWidget(fCancel);

  QVBoxLayout *main = new QVBoxLayout;
  main->addLayout(top);
  main->addLayout(bottom);

  setLayout(main);

  connect(fScaleAutomatic, SIGNAL(pressed()), this, SLOT(scaleAuto()));
  connect(fScaleByFactor, SIGNAL(pressed()), this, SLOT(getFactor()));
  connect(fCancel, SIGNAL(pressed()), this, SLOT(reject()));
}

//-------------------------------------------------------------------------
/**
 *
 */
void PModSelect::scaleAuto()
{
  emit scale(true, 0.01, false);
  done(1);
}

//-------------------------------------------------------------------------
/**
 *
 */
void PModSelect::getFactor()
{
  double factor = fFactorLineEdit->text().toDouble();
  bool state = fAbsVal->isChecked();

  emit scale(false, factor, state);
  done(1);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/**
 *
 */
PMusrStep::PMusrStep(const char *fln, QWidget *parent) :
  QDialog(parent),
  fMsrFileName(fln)
{
  setWindowTitle("musrStep");
  QString str = QIcon::themeName();
  bool isDarkTheme = false;
  if (str.contains("dark", Qt::CaseInsensitive))
    isDarkTheme = true;

  if (isDarkTheme)
    str = QString(":/icons/musrStep-22x22-dark.svg");
  else
    str = QString(":/icons/musrStep-22x22.svg");
  setWindowIcon(QIcon(QPixmap(str)));

  fValid = false;

  QString title = QString("<b><font size=""14"">%1</font></b>").arg(fMsrFileName);
  fTitleLabel = new QLabel(title);
  QLabel *icon = new QLabel();
  if (isDarkTheme)
    str = QString(":/icons/musrStep-32x32-dark.svg");
  else
    str = QString(":/icons/musrStep-32x32.svg");
  icon->setPixmap(QPixmap(str));

  QHBoxLayout *titleLayout = new QHBoxLayout;
  titleLayout->addWidget(fTitleLabel);
  titleLayout->addWidget(icon);
  titleLayout->insertStretch(1);

  int status = 0;
  if ((status=readMsrFile()) == 1) {
    fValid = true;
  } else {
    QString msg = QString("Failed to read msr-file: %1 (status=%2)").arg(fMsrFileName).arg(status);
    QMessageBox::critical(0, "ERROR", msg);
  }

  int height;
  if (fParamVec.size() < 70)
    height = 20*fParamVec.size();
  else
    height = 900;
  setMinimumSize(400, height);

  // populate dialog
  fParamTable = new QTableWidget(fParamVec.size(), 3);
  QStringList strL;
  strL << "name" << "value" << "step";
  fParamTable->setHorizontalHeaderLabels(strL);

  QTableWidgetItem *item;
  for (int i=0; i<fParamVec.size(); i++) {
    item = new QTableWidgetItem(fParamVec[i].name);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
    fParamTable->setItem(i, 0, item);
    item = new QTableWidgetItem(fParamVec[i].value);
    item->setFlags(Qt::ItemIsEnabled);
    fParamTable->setItem(i, 1, item);
    item = new QTableWidgetItem(fParamVec[i].step);
    fParamTable->setItem(i, 2, item);
  }
  fCheckSpecific = new QPushButton("Check S&pecific");
  fCheckSpecific->setWhatsThis("Allows to specify a template name which is used\nto select parameter names fitting to it.");
  fCheckAll = new QPushButton("Check A&ll");
  fCheckAll->setWhatsThis("Select all parameter names,\nexcept the ones with step == 0");
  fUnCheckAll = new QPushButton("&Uncheck All");  
  fUnCheckAll->setWhatsThis("Unselect all parameter names");

  QHBoxLayout *checkLayout = new QHBoxLayout;
  checkLayout->addWidget(fCheckSpecific);
  checkLayout->addWidget(fCheckAll);
  checkLayout->addWidget(fUnCheckAll);

  fModifyAuto = new QPushButton("Modify &Automatic");
  fModifyAuto->setWhatsThis("Will try to reset the step size,\nbased on some crude rules");
  fModifyAuto->setDefault(true);
  fModifySelected = new QPushButton("&Modify Selected");
  fModifySelected->setWhatsThis("Will call a dialog which all to specify how\nto proceed with the selected parameter steps.");

  QHBoxLayout *modifyLayout = new QHBoxLayout;
  modifyLayout->addWidget(fModifyAuto);
  modifyLayout->addWidget(fModifySelected);

  fSave = new QPushButton("&Save&&Quit");
  fCancel = new QPushButton("&Cancel");

  QHBoxLayout *buttomLayout = new QHBoxLayout;
  buttomLayout->addWidget(fSave);
  buttomLayout->addWidget(fCancel);

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addLayout(titleLayout);
  mainLayout->addWidget(fParamTable);
  mainLayout->addLayout(checkLayout);
  mainLayout->addLayout(modifyLayout);
  mainLayout->addLayout(buttomLayout);

  setLayout(mainLayout);

  connect(fParamTable, SIGNAL(cellChanged(int, int)), this, SLOT(handleCellChanged(int, int)));

  connect(fCheckSpecific, SIGNAL(pressed()), this, SLOT(checkSpecific()));
  connect(fCheckAll, SIGNAL(pressed()), this, SLOT(checkAll()));
  connect(fUnCheckAll, SIGNAL(pressed()), this, SLOT(unCheckAll()));

  connect(fModifyAuto, SIGNAL(pressed()), this, SLOT(modifyAuto()));
  connect(fModifySelected, SIGNAL(pressed()), this, SLOT(modifyChecked()));
  connect(fSave, SIGNAL(pressed()), this, SLOT(saveAndQuit()));
  connect(fCancel, SIGNAL(pressed()), this, SLOT(reject()));

  fModSelect = new PModSelect(this);
  connect(fModSelect, SIGNAL(scale(bool,double,bool)), this, SLOT(handleModSelect(bool,double,bool)));
}

//-------------------------------------------------------------------------
/**
 *
 */
void PMusrStep::handleCellChanged(int row, int column)
{
  QString str;
  bool ok;
  if (column == 0) {
    str = fParamTable->item(row, 2)->text();
    if ((fParamTable->item(row, column)->checkState() == Qt::Checked) &&
        ((str == "0") || (str == "0.0"))) {
      fParamTable->item(row, column)->setCheckState(Qt::Unchecked);
      QMessageBox::warning(0, "WARNING", "You cannot select a fixed value (step == 0).");
    }
  } else if (column == 2) {
    str = fParamTable->item(row, column)->text();
    str.toDouble(&ok); // result is of no interest
    if (ok) {
      fParamVec[row].step = str;
    } else {
      fParamTable->item(row, column)->setText(fParamVec[row].step);
    }
  }
}

//-------------------------------------------------------------------------
/**
 *
 */
void PMusrStep::checkSpecific()
{
  bool ok;
  QString str = QInputDialog::getText(this, "Enter Param Name Template", "Template:",
                                      QLineEdit::Normal, "", &ok);
  if (!ok)
    return;

  QString step("");
  for (int i=0; i<fParamTable->rowCount(); i++) {
    if (fParamTable->item(i,0)->text().contains(str)) {
      step = fParamTable->item(i,2)->text();
      if ((step != "0") && (step != "0.0"))
        fParamTable->item(i,0)->setCheckState(Qt::Checked);
    }
  }
}

//-------------------------------------------------------------------------
/**
 *
 */
void PMusrStep::checkAll()
{
  QString str("");
  for (int i=0; i<fParamTable->rowCount(); i++) {
    str = fParamTable->item(i,2)->text();
    if ((str != "0") && (str != "0.0"))
      fParamTable->item(i,0)->setCheckState(Qt::Checked);
  }
}

//-------------------------------------------------------------------------
/**
 *
 */
void PMusrStep::unCheckAll()
{
  for (int i=0; i<fParamTable->rowCount(); i++) {
    fParamTable->item(i,0)->setCheckState(Qt::Unchecked);
  }
}

//-------------------------------------------------------------------------
/**
 *
 */
void PMusrStep::modifyAuto()
{
  QString str;
  bool absVal;
  double factor;
  for (int i=0; i<fParamTable->rowCount(); i++) {
    str = fParamTable->item(i,2)->text();
    if ((str != "0") && (str != "0.0")) {
      factor = lookupTable(fParamTable->item(i,0)->text(), absVal);
      str = adoptStep(fParamTable->item(i,1)->text(), factor, absVal);
      fParamTable->item(i,2)->setText(str);
      fParamVec[i].step = str;
    }
  }
}

//-------------------------------------------------------------------------
/**
 *
 */
void PMusrStep::modifyChecked()
{
  fModSelect->show();
}

//-------------------------------------------------------------------------
/**
 *
 */
void PMusrStep::handleModSelect(bool automatic, double factor, bool absVal)
{
  QString str;
  for (int i=0; i<fParamTable->rowCount(); i++) {
    if (fParamTable->item(i,0)->checkState() == Qt::Checked) {
      if (automatic) {
        factor = lookupTable(fParamTable->item(i,0)->text(), absVal);
      }
      str = adoptStep(fParamTable->item(i,1)->text(), factor, absVal);
      fParamTable->item(i,2)->setText(str);
      fParamVec[i].step = str;
    }
  }
}

//-------------------------------------------------------------------------
/**
 *
 */
void PMusrStep::saveAndQuit()
{
  writeMsrFile();
  accept();
}

//-------------------------------------------------------------------------
/**
 *
 */
void PMusrStep::initParam(PParam &param)
{
  param.number = "";
  param.name = "";
  param.value = "";
  param.step = "";
  param.posErr = "";
  param.boundLow = "";
  param.boundHigh = "";
}

//-------------------------------------------------------------------------
/**
 *
 */
double PMusrStep::lookupTable(const QString str, bool &absVal)
{
  double factor = 0.01;
  absVal = false;

  if (str.startsWith("freq", Qt::CaseInsensitive) ||
      str.startsWith("frq", Qt::CaseInsensitive) ||
      str.startsWith("field", Qt::CaseInsensitive)) {
    factor = 1.0e-3;
  } else if (str.startsWith("lambda", Qt::CaseInsensitive) ||
           str.startsWith("sigma", Qt::CaseInsensitive) ||
           str.startsWith("rlx", Qt::CaseInsensitive) ||
           str.startsWith("rate", Qt::CaseInsensitive)) {
    factor = 0.1;
  } else if (str.startsWith("phase", Qt::CaseInsensitive) ||
             str.startsWith("phs", Qt::CaseInsensitive)) {
    factor = 5.0;
    absVal = true;
  } else if (str.startsWith("N0", Qt::CaseInsensitive) ||
             str.startsWith("Nrm", Qt::CaseInsensitive) ||
             str.startsWith("N_bkg", Qt::CaseInsensitive) ||
             str.startsWith("Bgr", Qt::CaseInsensitive)) {
    factor = 0.01;
  }

  return factor;
}

//-------------------------------------------------------------------------
/**
 *
 */
QString PMusrStep::adoptStep(const QString str, double factor, bool absVal)
{
  bool ok;
  double dval = str.toDouble(&ok);

  QString step("");
  if (absVal)
    step = QString("%1").arg(factor);
  else
    step = QString("%1").arg(factor*dval);

  return step;
}

//-------------------------------------------------------------------------
/**
 *
 */
int PMusrStep::readMsrFile()
{
  fParamVec.clear();

  QFile fin(fMsrFileName);

  if (!fin.open(QIODevice::ReadOnly|QIODevice::Text))
    return -1;

  bool done = false, parameter = false;
  QByteArray line;
  QString str;
  QStringList strL;
  PParam param;

  while(!done && !fin.atEnd()) {
    line = fin.readLine();
    str = line.data();
    str = str.trimmed();
    if (str.isEmpty() || str.startsWith("#")) {
      continue;
    } else if (str.startsWith("FITPARAMETER")) {
      parameter = true;
      continue;
    } else if (str.startsWith("THEORY")) {
      done = true;
      continue;
    }

    if (parameter) {
      strL = str.split(" ", QString::SkipEmptyParts);
      if ((strL.size() != 5) && (strL.size() != 7)) {
        fin.close();
        return -2;
      }
      initParam(param);

      param.number = strL[0];
      param.name = strL[1];
      param.value = strL[2];
      param.step = strL[3];
      param.posErr = strL[4];
      if (strL.size() == 7) {
        param.boundLow = strL[5];
        param.boundHigh= strL[6];
      }
      fParamVec.push_back(param);
    }
  }

  fin.close();

  return 1;
}

//-------------------------------------------------------------------------
/**
 *
 */
int PMusrStep::writeMsrFile()
{
  // read whole msr-file
  QFile fin(fMsrFileName);

  if (!fin.open(QIODevice::ReadOnly|QIODevice::Text))
    return -1;

  QByteArray data = fin.readAll();

  fin.close();

  QFile fileOut(fMsrFileName);

  if (!fileOut.open(QIODevice::WriteOnly|QIODevice::Text))
    return -2;

  QTextStream fout(&fileOut);

  int idx = 0;
  QString line, paramLine;
  bool done = false;
  bool paramBlock = false;
  do {
    line = getLine(data, idx);
    if ((idx == -1) || (idx == data.count())) {
      done = true;
    } else {
      if (line.startsWith("FITPARAMETER")) {
        paramBlock = true;
      } else if (line.startsWith("THEORY")) {
        paramBlock = false;
      }
      if (paramBlock) {
        paramLine = updateParamLine(line);
        if (paramLine == "") // comment line, etc.
          fout << line << "\n";
        else
          fout << paramLine << "\n";
      } else {
        fout << line << "\n";
      }
    }
  } while (!done);
  fout << "*** FIT DID NOT CONVERGE ***\n";

  fileOut.close();

  return 1;
}

//-------------------------------------------------------------------------
/**
 *
 */
QString PMusrStep::getLine(QByteArray &data, int &idx)
{
  int newIdx = data.indexOf('\n', idx);
  QString line("");

  if (newIdx != -1) {
    line = data.mid(idx, newIdx-idx).data();
    idx = newIdx+1;
  }

  return line;
}

//-------------------------------------------------------------------------
/**
 *
 */
QString PMusrStep::updateParamLine(const QString str)
{
  // find proper parameter index
  int idx = -1;
  QString paramStr;
  for (int i=0; i<fParamVec.size(); i++) {
    paramStr = fParamVec[i].name + " ";
    if (str.contains(paramStr)) {
      idx = i;
      break;
    }
  }

  if (idx == -1)
    return QString("");

  // construct new parameter line
  QString result;
  QString fstr;
  QTextStream ss(&result);
  ss.setFieldWidth(10);
  ss.setFieldAlignment(QTextStream::AlignRight);
  fstr = fParamVec[idx].number + " ";
  ss << fstr;
  ss.setFieldAlignment(QTextStream::AlignLeft);
  int width = 12;
  if (fParamVec[idx].name.length() >= 12)
    width = fParamVec[idx].name.length()+1;
  ss.setFieldWidth(width);
  ss << fParamVec[idx].name;
  ss.setFieldWidth(11);
  ss << fParamVec[idx].value;
  ss << fParamVec[idx].step;
  ss << fParamVec[idx].posErr;
  if (fParamVec[idx].boundLow != "") {
    ss << fParamVec[idx].boundLow;
    ss << fParamVec[idx].boundHigh;
  }

  return result;
}
